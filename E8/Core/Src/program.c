#include "main.h"
#include <math.h>

extern ADC_HandleTypeDef hadc3;
extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim2;

//TIM_HandleTypeDef *pwm_timer_buzzer = &htim1; // Point to PWM timer configured in CubeMX
//uint32_t pwm_channel_buzz = TIM_CHANNEL_1;  // Specify configured PWM channel

//void PWM_Change_Tone(uint32_t pwm_freq, uint16_t volume) // pwm_freq (1 - 20000), volume (0 - 1000)
//{
//
////  volume=5;
//  if (pwm_freq == 0 || pwm_freq > 20000)
//  {
//    __HAL_TIM_SET_COMPARE(pwm_timer_buzzer, pwm_channel_buzz, 0);
//  }
//  else
//  {
//    const uint32_t internal_clock_freq = HAL_RCC_GetSysClockFreq();
//	const uint16_t prescaler = 1 + internal_clock_freq / pwm_freq / 60000;
//    const uint32_t timer_clock = internal_clock_freq / prescaler;
//    const uint32_t period_cycles = timer_clock / pwm_freq;
//    const uint32_t pulse_width = volume * period_cycles / 1000 / 2;
//
//    pwm_timer_buzzer->Instance->PSC = prescaler - 1;
//    pwm_timer_buzzer->Instance->ARR = period_cycles - 1;
//    pwm_timer_buzzer->Instance->EGR = TIM_EGR_UG;
//    __HAL_TIM_SET_COMPARE(pwm_timer_buzzer, pwm_channel_buzz, pulse_width); // pwm_timer->Instance->CCR2 = pulse_width;
//  }
//}

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
int currentBR;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	if (hadc->Instance == ADC3) {
		int x = HAL_ADC_GetValue(&hadc3);
		currentBR=x;
		float fx = ((float) x * 100 / 3800);
		unsigned char data[100];

//		int n = sprintf(data, "LDR: %d  %.2f \n", x, fx);
//		HAL_UART_Transmit(&huart3, data, n, 1000);
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

//void checkBrightness(){
//	if (initFlag == 0){
//		if (currentBR >= threshold){
//			setNumber(currentBR);
//
//			if(alert == 0){
//				warnCount=(warnCount+1)%10;
//				alert = 1;
//				HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
//				sprintf(timeStr, "%02d:%02d:%02d", rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds);
//				char data[100];
//				int n = sprintf(data, "[WARN] %s Critical Situation\n", timeStr);
//				HAL_UART_Transmit(&huart3, data, n, 1000);
//			}
//			turn_off_leds();
//			playAlarm();
//			numbers[3]=warnCount;
//
//		}else{
//			alert=0;
//			seven_segment_set_num(numbers);
//			PWM_Change_Tone(0, 0);
//			turn_on_leds();
//		}
//	}
//}

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
}

char messages[50];
char input[50];

int flag = 1; //1 ==> light, 0 ==> message

void programLoop() {
	char data[100];
	char data2[100];
	if (flag == 1){
		// lamp
		int percent = 5;
		int critical = 12;
		int n = sprintf(data, " Light: %d%%",percent);
//		HAL_UART_Transmit(&huart3, data, n, 1000);
		createChar(0, lamp);
		setCursor(0, 1);
		write(0);
		print(data);
		n = sprintf(data2, "Critical Light < %d%%",critical);
		setCursor(0, 4);
		print(data2);

	}else{
		//message
		createChar(0, message);
		setCursor(0, 1);
		write(0);
		int n = sprintf(data, " Message: %s",messages);
		print(data);
	}
}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) { //doesnt work !!!
//	char data[100];
//	int n = sprintf(data, "Hi\n");
//	HAL_UART_Transmit(&huart3, data, n, 1000);
//	programLoop();
//}

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
			memcpy(messages, input, sizeof(input));
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

        clear(); //programLoop(); //doesnt work, maybe cause for Priority
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
		last_time2=HAL_GetTick();
		}
    }
}
