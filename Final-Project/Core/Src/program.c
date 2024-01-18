#include "main.h"
//#include <math.h>
//#include <strings.h>
//#include <time.h>


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

int state = 1;
int numbers[4] = {0,0,0,0}; //show value of states

seven_segment_type seven_segment = {
	.digit_activators={
		   {.port=GPIOB, .pin=GPIO_PIN_2},
		   {.port=GPIOB, .pin=GPIO_PIN_0},
		   {.port=GPIOC, .pin=GPIO_PIN_4},
		   {.port=GPIOA, .pin=GPIO_PIN_4}},

	.BCD_input={
			{.port=GPIOC, .pin=GPIO_PIN_6},
			{.port=GPIOD, .pin=GPIO_PIN_15},
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
    }
}

void seven_segment_deactivate_digits(void) {
    for (int i = 0; i < 4; ++i)
        HAL_GPIO_WritePin(seven_segment.digit_activators[i].port, seven_segment.digit_activators[i].pin, GPIO_PIN_SET);
}

int last_time_on = 0;
void seven_segment_activate_digit(uint32_t d) {
    if (d < 4) {
    	HAL_GPIO_WritePin(seven_segment.digit_activators[d].port, seven_segment.digit_activators[d].pin, GPIO_PIN_RESET);
//		if(d == state && (HAL_GetTick() - last_time_on) > 40){
//			HAL_GPIO_TogglePin(seven_segment.digit_activators[d].port, seven_segment.digit_activators[d].pin);
//			last_time_on = HAL_GetTick();
//		}
//		else if (d != state){
//			HAL_GPIO_WritePin(seven_segment.digit_activators[d].port, seven_segment.digit_activators[d].pin, GPIO_PIN_RESET);
//		}
    }
}

void seven_segment_set_num(int num[4]) {
	for (uint32_t i = 0; i <= 3; ++i) {
		seven_segment.digits[i] = num[i];
	}
}

void seven_segment_refresh(void) {
    static uint32_t state_tmp = 0;
    static uint32_t last_time_tmp = 0;
    if (HAL_GetTick() - last_time_tmp > 5) {
        seven_segment_deactivate_digits();
        seven_segment_activate_digit(state_tmp);
        seven_segment_display_decimal(seven_segment.digits[state_tmp]);
        state_tmp = (state_tmp + 1) % 4;
        last_time_tmp = HAL_GetTick();
    }
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
//		int numbers2[4]={d,c,b,a};
		int numbers2[4]={a,b,c,d};
		seven_segment_set_num(numbers2);
	}

}

void programInit() {
	setNumber(1111);

}

void programLoop() {
    seven_segment_refresh();
//    setNumber(1234);
}
