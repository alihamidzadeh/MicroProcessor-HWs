#include "main.h"
#include <math.h>

extern ADC_HandleTypeDef hadc3;
extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

TIM_HandleTypeDef *pwm_timer_buzzer = &htim3; // Point to PWM timer configured in CubeMX
uint32_t pwm_channel_buzz = TIM_CHANNEL_1;  // Specify configured PWM channel

void PWM_Change_Tone(uint32_t pwm_freq, uint16_t volume) // pwm_freq (1 - 20000), volume (0 - 1000)
{

//  volume=5;
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

//void square_signal(int counter){
//	if (counter >= 96){
//		counter = counter % 96;
//
//	}
//
//	if (counter < 48) {
//		uint32_t x = 20000;
//		PWM_Change_Tone(x, 1000);
//
////		printf("%f   %d\n", x, counter);
//	}
//	else if (counter >= 48 && counter < 96){
//		uint32_t x = 5000;
//		PWM_Change_Tone(x, 1000);
//
////		printf("%f   %d\n", x, counter);
//	}
//
//}
int currentBR = 5;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	if (hadc->Instance == ADC3) {
		int x = HAL_ADC_GetValue(&hadc3);
		currentBR = ((int) x * 100 / 3800);
		unsigned char data[100];

		int n = sprintf(data, "LDR: %d  %d \n", x, currentBR);
		HAL_UART_Transmit(&huart3, data, n, 1000);
//		checkBrightness();
	}
}



//Play Warn
uint64_t counter = 0;
int buzz_type = 2;
int warnCount=0;

//void playAlarm(){
//	if (initFlag == 0 && alert == 1){
//		if(buzz_type == 3)
//			triangle_signal(counter);
//		else if(buzz_type == 2)
//			square_signal(counter);
//		else if(buzz_type == 1)
//			sin_signal(counter);
//	}
//}

int threshhold = 15;


void checkBrightness(){
		if (currentBR <= threshhold){
				  PWM_Change_Tone(2000,1000);
		}else{
			  PWM_Change_Tone(0,0);
	}
}

unsigned char lamp[] = {
  0x00,
  0x06,
  0x0F,
  0x0F,
  0x0F,
  0x06,
  0x06,
  0x00
};

unsigned char message[] = {
  0x00,
  0x00,
  0x1F,
  0x1F,
  0x15,
  0x11,
  0x1F,
  0x00
};

void programInit() {
	LiquidCrystal(GPIOD, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14);

	char data[50];
	int n = sprintf(data, "Program Started...\n");
	HAL_UART_Transmit(&huart3, data, n, 1000);
	begin(20, 4);

	write("vv");
}

char messages[50];
char input[50];

int flag = 0; //0 ==> light, 1 ==> message

int flag_current = 1;
int message_arrived = 0;

void programLoop() {
	char data[100];
	char data2[100];
	if (flag_current == 1 && flag == 0){
		clear();
		// lamp
		int percent = currentBR;
		int n = sprintf(data, " Light: %d%%",percent);
//		HAL_UART_Transmit(&huart3, data, n, 1000);
		createChar(0, lamp);
		setCursor(0, 1);
		write(0);
		print(data);
		n = sprintf(data2, "Critical Light < %d%%",threshhold);
		setCursor(0, 4);
		print(data2);
		flag_current = 0;

	}
	else if((flag_current == 0 && flag == 1) || message_arrived){
		clear();
		//message
		createChar(0, message);
		setCursor(0, 1);
		write(0);
		int n = sprintf(data, " Message: %s",input);
		print(data);
		flag_current = 1;
		message_arrived = 0;
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if (htim->Instance == TIM2){
    	checkBrightness();
//    	char data[50];
//    	int n = sprintf(data, "hhhh...\n");
//    	HAL_UART_Transmit(&huart3, data, n, 1000);
//     	programLoop();
//    	write("------");
    }
}

char character;
int index_arr = 0;

void uart_rx_enable_it(void) {
	HAL_UART_Receive_IT(&huart3, &character, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if (huart->Instance == USART3){
		if(character != 10){
			input[index_arr++] = character;
		} else{
			input[index_arr++] = '\0';
			index_arr = 0;
//			strncpy(messages, input, sizeof(input));
//			memcpy(messages, input, sizeof(input));
			message_arrived = 1;
		}
		uart_rx_enable_it();
	}
}

int last_time2 = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_1) { //button: PC1 for change LCD flag
		if (HAL_GetTick() - last_time2 > 400){
			if (flag == 1)
				flag = 0;
			else
				flag = 1;

//			clear(); //programLoop(); //doesnt work, maybe cause for Priority
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
			last_time2=HAL_GetTick();
		}
    }
}
