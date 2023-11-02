#include "main.h"
#include "math.h"

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

int state = 2; //0,1,2
int numbers[3] = {1,1,1}; //show value of states

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
        HAL_GPIO_WritePin(seven_segment.BCD_input[0].port, seven_segment.BCD_input[0].pin,
                          (n & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(seven_segment.BCD_input[1].port, seven_segment.BCD_input[1].pin,
                          (n & 2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(seven_segment.BCD_input[2].port, seven_segment.BCD_input[2].pin,
                          (n & 4) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(seven_segment.BCD_input[3].port, seven_segment.BCD_input[3].pin,
                          (n & 8) ? GPIO_PIN_SET : GPIO_PIN_RESET);

//        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
    }
}

void seven_segment_deactivate_digits(void) {
    for (int i = 0; i < 4; ++i) {
        HAL_GPIO_WritePin(seven_segment.digit_activators[i].port, seven_segment.digit_activators[i].pin,
                          GPIO_PIN_SET);
    }
}

void seven_segment_activate_digit(uint32_t d) {
    if (d < 4) {
        HAL_GPIO_WritePin(seven_segment.digit_activators[d].port, seven_segment.digit_activators[d].pin,
                          GPIO_PIN_RESET);
    }
}

void seven_segment_set_num(int numbers[3]) {
        for (uint32_t i = 0; i < 3; ++i) {
            seven_segment.digits[2 - i] = numbers[i];
    }
}

void seven_segment_refresh(void) {
    static uint32_t state_tmp = 2;
    static uint32_t last_time_tmp = 0;
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);
    if (HAL_GetTick() - last_time_tmp > 5) {
        seven_segment_deactivate_digits();
        seven_segment_activate_digit(state_tmp);
        seven_segment_display_decimal(seven_segment.digits[state_tmp]);
        if ((state == 2 && state_tmp == 0) || (state==1 && state_tmp==1) || (state == 0 && state_tmp == 2)){
        	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 1);
        }else{
        	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, 0);
        }
        state_tmp = (state_tmp + 1) % 3;
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
// ِD3 : E9
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

	for (int i = 1; i < 9; i++)
		HAL_GPIO_WritePin(leds_ltr.digit[i-1].port, leds_ltr.digit[i-1].pin,0);

	if (c==0){
		for (int i = a; i <= a+b-1; i++){
			HAL_GPIO_WritePin(leds_ltr.digit[(i-1)%8].port, leds_ltr.digit[(i-1)%8].pin,1);
		}
	}else{
		for (int i = a; i <= a+b-1; i++){
					HAL_GPIO_WritePin(leds_rtl.digit[(i-1)%8].port, leds_rtl.digit[(i-1)%8].pin,1);
			}
	}

}


int last_time2 = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
		if (GPIO_Pin == GPIO_PIN_0) { //Left button ==> PF4
			if (HAL_GetTick() - last_time2 > 300){
			state = state - 1;
			if(state < 0)
				state += 3;
			state = state % 3;
			last_time2=HAL_GetTick();
			}
		}

		else if (GPIO_Pin == GPIO_PIN_1){	 //middle button ==> PA1
			if (HAL_GetTick() - last_time2 > 180){
				if (state != 0){
					numbers[state]=(numbers[state] + 1) % 9;
					if (numbers[state] == 0)
						numbers[state]++;
				}else{
					if (numbers[state] == 1)
						numbers[state]=0;
					else
						numbers[state]=1;
				}
				last_time2=HAL_GetTick();
				event();
			}

		}

		else if (GPIO_Pin == GPIO_PIN_4) { //Right button ==> PC0

			if (HAL_GetTick() - last_time2 > 300){
				state = (state + 1) % 3;
				last_time2=HAL_GetTick();
				}

			}

		seven_segment_set_num(numbers);
}
