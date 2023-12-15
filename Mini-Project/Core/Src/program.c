#include "main.h"
#include <math.h>


typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} pin_type;

typedef struct {
    pin_type digit_activators[4];
    pin_type BCD_input[4];
    uint32_t digits[4];
    uint32_t number;
} seven_segment_type;

typedef struct {
    pin_type digit[8];
} led_types;

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim3;



TIM_HandleTypeDef *pwm_timer_buzzer = &htim3; // Point to PWM timer configured in CubeMX
uint32_t pwm_channel_buzz = TIM_CHANNEL_1;  // Specify configured PWM channel

void PWM_Change_Tone(uint32_t pwm_freq, uint16_t volume) // pwm_freq (1 - 20000), volume (0 - 1000)
{
  if (pwm_freq == 0 || pwm_freq > 20000)
  {
    __HAL_TIM_SET_COMPARE(pwm_timer_buzzer, pwm_channel_buzz, 0);
  }
  else
  {
    const uint32_t internal_clock_freq = HAL_RCC_GetSysClockFreq();
	const uint16_t prescaler = 1 + internal_clock_freq / pwm_freq / 60000;
    const uint32_t timer_clock = internal_clock_freq / prescaler;
    const uint32_t period_cycles = timer_clock / pwm_freq;
    const uint32_t pulse_width = volume * period_cycles / 1000 / 2;

    pwm_timer_buzzer->Instance->PSC = prescaler - 1;
    pwm_timer_buzzer->Instance->ARR = period_cycles - 1;
    pwm_timer_buzzer->Instance->EGR = TIM_EGR_UG;
    __HAL_TIM_SET_COMPARE(pwm_timer_buzzer, pwm_channel_buzz, pulse_width); // pwm_timer->Instance->CCR2 = pulse_width;
  }
}

void sin_signal(int counter){
	int max = 10000;
	uint32_t x = (uint32_t) (max * sin(counter * (M_PI  / 180)) + 10000);
	PWM_Change_Tone(x, 1000);
//	printf("%f   %d\n", x, counter);

}

void square_signal(int counter){
	if (counter >= 96){
		counter = counter % 96;

	}

	if (counter < 48) {
		uint32_t x = 20000;
		PWM_Change_Tone(x, 1000);

//		printf("%f   %d\n", x, counter);
	}
	else if (counter >= 48 && counter < 96){
		uint32_t x = 5000;
		PWM_Change_Tone(x, 1000);

//		printf("%f   %d\n", x, counter);
	}

}

void triangle_signal(int counter){
	if (counter >= 96){
		counter = counter % 96;
	}

	if (counter < 80) {
		uint32_t x = 250 * counter;
		PWM_Change_Tone(x, 1000);

//		printf("%f   %d\n", x, counter);
	}
	else if (counter >= 80 && counter < 96){
		uint32_t x = 0;
		PWM_Change_Tone(x, 1000);

//		printf("%f   %d\n", x, counter);
	}

}


int state = 1; //0,1,2
int numbers[4] = {1,2,3,4}; //show value of states

seven_segment_type seven_segment = {
		.digit_activators={
			   {.port=GPIOB, .pin=GPIO_PIN_2},
			   {.port=GPIOB, .pin=GPIO_PIN_0},
			   {.port=GPIOC, .pin=GPIO_PIN_4},
			   {.port=GPIOA, .pin=GPIO_PIN_4}},

        .BCD_input={
        		{.port=GPIOC, .pin=GPIO_PIN_6},
				{.port=GPIOD, .pin=GPIO_PIN_13},
				{.port=GPIOD, .pin=GPIO_PIN_9},
				{.port=GPIOB, .pin=GPIO_PIN_13}},
        .digits={0, 0, 0, 0},
        .number = 0};



void seven_segment_display_decimal(uint32_t n) {
    if (n < 10) {
        HAL_GPIO_WritePin(seven_segment.BCD_input[0].port, seven_segment.BCD_input[0].pin, (n & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(seven_segment.BCD_input[1].port, seven_segment.BCD_input[1].pin, (n & 2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(seven_segment.BCD_input[2].port, seven_segment.BCD_input[2].pin, (n & 4) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(seven_segment.BCD_input[3].port, seven_segment.BCD_input[3].pin, (n & 8) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
    }
}

void seven_segment_deactivate_digits(void) {
    for (int i = 0; i < 4; ++i) {
        HAL_GPIO_WritePin(seven_segment.digit_activators[i].port, seven_segment.digit_activators[i].pin, GPIO_PIN_SET);
    }
}

int last_time_on = 0;
void seven_segment_activate_digit(uint32_t d) {
    if (d < 4) {
//		if(d == (2-state)) {//&& (HAL_GetTick() - last_time_on) > 40){
////			HAL_Delay(35);
//			HAL_GPIO_TogglePin(seven_segment.digit_activators[d].port, seven_segment.digit_activators[d].pin);
////			last_time_on = HAL_GetTick();
//		}
//		else if (d != (2-state)){
//			HAL_GPIO_WritePin(seven_segment.digit_activators[d].port, seven_segment.digit_activators[d].pin, GPIO_PIN_RESET);
//		}

		if(d == state && (HAL_GetTick() - last_time_on) > 40){
//			HAL_Delay(35);
			HAL_GPIO_TogglePin(seven_segment.digit_activators[d].port, seven_segment.digit_activators[d].pin);
			last_time_on = HAL_GetTick();
		}
		else if (d != state){
			HAL_GPIO_WritePin(seven_segment.digit_activators[d].port, seven_segment.digit_activators[d].pin, GPIO_PIN_RESET);
		}
    }
}

void seven_segment_set_num(int numbers[4]) {
        for (uint32_t i = 0; i <= 3; ++i) {
            seven_segment.digits[i] = numbers[i];
		}
}

void seven_segment_refresh(void) {
    static uint32_t state_tmp = 0;
    static uint32_t last_time_tmp = 0;
    //HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);
    if (HAL_GetTick() - last_time_tmp > 5) {
        seven_segment_deactivate_digits();
        seven_segment_activate_digit(state_tmp);
        seven_segment_display_decimal(seven_segment.digits[state_tmp]);
        if ((state == 2 && state_tmp == 2) || (state==1 && state_tmp==1) || (state == 0 && state_tmp == 0)){
        	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
        }else{
        	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
        }
        state_tmp = (state_tmp + 1) % 4; //active each 4 segments
        last_time_tmp = HAL_GetTick();
    }
}

void programInit() {
    seven_segment_set_num(numbers);
    event();
}

void programLoop() {
    seven_segment_refresh();
}

//numbers[3] = {3,2,1} ==> 3: start | 2: length | 3: direction
//LEDs:
// ÙD3 : E9
// D4 : E8
// D6 : E15
// D8 : E14
// D10 : E13
// D9 : E12
// D7 : E11
// D5 : E10

led_types leds_ltr= {.digit={
	   {.port=GPIOE, .pin=GPIO_PIN_9},  //1: D3
	   {.port=GPIOE, .pin=GPIO_PIN_8},  //2: D4
	   {.port=GPIOE, .pin=GPIO_PIN_15}, //3: D6
	   {.port=GPIOE, .pin=GPIO_PIN_14}, //4: D8
	   {.port=GPIOE, .pin=GPIO_PIN_13}, //5: D10
	   {.port=GPIOE, .pin=GPIO_PIN_12}, //6: D9
	   {.port=GPIOE, .pin=GPIO_PIN_11}, //7: D7
	   {.port=GPIOE, .pin=GPIO_PIN_10}, //8: D5
}};

led_types leds_rtl= {.digit={
		{.port=GPIOE, .pin=GPIO_PIN_9},  //1: D3
		{.port=GPIOE, .pin=GPIO_PIN_10}, //2: D5
		{.port=GPIOE, .pin=GPIO_PIN_11}, //3: D7
		{.port=GPIOE, .pin=GPIO_PIN_12}, //4: D9
		{.port=GPIOE, .pin=GPIO_PIN_13}, //5: D10
		{.port=GPIOE, .pin=GPIO_PIN_14}, //6: D8
		{.port=GPIOE, .pin=GPIO_PIN_15}, //7: D6
		{.port=GPIOE, .pin=GPIO_PIN_8},  //8: D4
}};

void event(){
	int a=numbers[2];
	int b=numbers[1];
	int c=numbers[0];
	seven_segment_deactivate_digits();

	//TODO: write event there

}

void setSegment(int state, int digit, int flag){
//(flag==0) ==> for first part, show 1 number with 4 digits
//(flag==1) ==> for second part, show 4 number with 1 digits (has limit)

	if (flag == 1){
		if (state == 0){
			if (digit >= 0 && digit <= 9){
				numbers[state]=digit;
			}
		}else if (state == 1){
			if (digit >= 1 && digit <= 4){
				numbers[state]=digit;
			}
		}else if (state == 2){
			if (digit >= 1 && digit <= 3){
				numbers[state]=digit;
			}
		}else if (state == 3){
			if (digit >= 0 && digit <= 9){
				numbers[state]=digit;
			}
		}
	}else{
		numbers[state]=digit;
	}
	seven_segment_set_num(numbers);
}

void setNumber(int number){
	int a = 0, b = 0, c = 0, d = 0;
	if (number <= 9999 && number >= 0){
		a = number % 10;
		number /= 10;
		b = number % 10;
		number /= 10;
		c = number % 10;
		number /= 10;
		d = number % 10;

		setSegment(3,a,0);
		setSegment(2,b,0);
		setSegment(1,c,0);
		setSegment(0,d,0);
	}

}

int last_time2 = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
		if (GPIO_Pin == GPIO_PIN_4) { //Left button (Decrease Number) ==> PF4
			if (HAL_GetTick() - last_time2 > 400){
				if (state == 0){
					numbers[state]=(numbers[state] - 1);
					if (numbers[state] == -1)
						numbers[state]=9;

				}else if (state == 1){
					numbers[state]=(numbers[state] - 1);
						if (numbers[state] == 0)
							numbers[state]=4;

				}else if (state == 2){
					numbers[state]=(numbers[state] - 1);
						if (numbers[state] == 0)
							numbers[state]=3;
				}
				last_time2=HAL_GetTick();
				event();
			}
		}

		else if (GPIO_Pin == GPIO_PIN_1){	 //middle button (Increase Number) ==> PA1
			if (HAL_GetTick() - last_time2 > 400){
				if (state == 0){
					numbers[state]=(numbers[state] + 1) % 10;

				}else if (state == 1){
					numbers[state]=(numbers[state] + 1) % 5;
					if (numbers[state] == 0)
						numbers[state]++;

				}else if (state == 2){
					numbers[state]=(numbers[state] + 1) % 4;
					if (numbers[state] == 0)
						numbers[state]++;
				}
				last_time2=HAL_GetTick();
				event();
			}
		}

		else if (GPIO_Pin == GPIO_PIN_0) { //Right button (Next Number)==> PC0
			if (HAL_GetTick() - last_time2 > 400){
				state = (state + 1) % 3;
				last_time2=HAL_GetTick();
				}
			}
		seven_segment_set_num(numbers);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	if (hadc->Instance == ADC1) {
//		  HAL_UART_Transmit(&huart3,"TTT",3,1000);

		int x = HAL_ADC_GetValue(&hadc1);
		float fx = ((float) x * 100 / 4095);
		unsigned char data[100];
		int n = sprintf(data, "%d  %.4f\n", x, fx);
		HAL_UART_Transmit(&huart3, data, n, 1000);
//		HAL_Delay(10);
	}
	else if(hadc->Instance == ADC2){

	}

}

//Play Warn
//uint64_t counter = 0;
//int buzz_type = 1;

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//	if (htim->Instance == TIM2) {
//		if(buzz_type == 1){
//			triangle_signal(counter);
//		}
//		else if(buzz_type == 2){
//			square_signal(counter);
//		}
//		else if(buzz_type == 3){
//			sin_signal(counter);
//		}
//		counter = counter + 1;
//		if(counter > 5000){
//			counter = 0;
//		}
//		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15);
//
//		HAL_ADC_Start_IT(&hadc1);
//	}
//}
