#include "main.h"

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} pin_type;

#define NUM_LEN 44

uint32_t shift_interval = 250;

typedef struct {
	pin_type digit_activators[4];
	pin_type BCD_input[4];
	int64_t digits[NUM_LEN + 8];
	char number[NUM_LEN];
	uint32_t shift_flag;
	uint32_t shift_counter;
} seven_segment_type;

seven_segment_type seven_segment = { .digit_activators = { { .port = GPIOB,
		.pin = GPIO_PIN_2 }, { .port = GPIOB, .pin = GPIO_PIN_0 }, { .port =
GPIOC, .pin = GPIO_PIN_4 }, { .port = GPIOA, .pin = GPIO_PIN_4 } }, .BCD_input =
		{ { .port = GPIOC, .pin = GPIO_PIN_6 }, { .port = GPIOD, .pin =
		GPIO_PIN_13 }, { .port = GPIOD, .pin = GPIO_PIN_9 }, { .port =
		GPIOB, .pin = GPIO_PIN_13 } }, .digits = { 0, 0, 0, 0 }, .number = 0,
		.shift_flag = 0, .shift_counter = 0 };

void seven_segment_display_decimal(uint64_t n) {
	if (n < 10) {
		HAL_GPIO_WritePin(seven_segment.BCD_input[0].port,
				seven_segment.BCD_input[0].pin,
				(n & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(seven_segment.BCD_input[1].port,
				seven_segment.BCD_input[1].pin,
				(n & 2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(seven_segment.BCD_input[2].port,
				seven_segment.BCD_input[2].pin,
				(n & 4) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(seven_segment.BCD_input[3].port,
				seven_segment.BCD_input[3].pin,
				(n & 8) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
}

void seven_segment_deactivate_digits(void) {
	for (int i = 0; i < 4; ++i) {
		HAL_GPIO_WritePin(seven_segment.digit_activators[i].port,
				seven_segment.digit_activators[i].pin, GPIO_PIN_SET);
	}
}

void seven_segment_deactivate_digit(int i) {
	HAL_GPIO_WritePin(seven_segment.digit_activators[i].port,
			seven_segment.digit_activators[i].pin, GPIO_PIN_SET);
}

void seven_segment_activate_digit(uint32_t d) {
	if (d < 4) {
		HAL_GPIO_WritePin(seven_segment.digit_activators[d].port,
				seven_segment.digit_activators[d].pin, GPIO_PIN_RESET);
	}
}

void seven_segment_set_num(uint8_t *n) {
	if (NUM_LEN > 4) {
		seven_segment.shift_flag = 1;
		seven_segment.shift_counter = NUM_LEN + 4;

	}
	for (uint32_t i = 0; i < 4; ++i) {
		seven_segment.digits[i] = -1;
	}

	for (uint32_t j = 0; j < NUM_LEN; ++j) {
		seven_segment.digits[4 + NUM_LEN - 1 - j] = n[NUM_LEN - 1 - j] - '0';
	}

	for (uint32_t i = 4 + NUM_LEN; i < 8 + NUM_LEN; ++i) {
		seven_segment.digits[i] = -1;
	}
}

void seven_segment_refresh(void) {
	static uint32_t state = 0;
	static uint32_t last_time = 0;
	static uint32_t last_time_shift = 0;
	static uint32_t st_index = 0;
	if (HAL_GetTick() - last_time > 5) {
		seven_segment_deactivate_digits();
//        seven_segment_activate_digit(state);
//	   seven_segment_display_decimal(seven_segment.digits[st_index + state]);
		if (st_index + state == 13) {
			seven_segment_activate_digit(0);
		}
		if (seven_segment.digits[st_index + state] != -1) {
			seven_segment_activate_digit(state);
			seven_segment_display_decimal(
					seven_segment.digits[st_index + state]);
		}

		if (seven_segment.shift_flag == 1
				&& HAL_GetTick() - last_time_shift > shift_interval
				&& state == 3) {
			if (seven_segment.shift_counter == 0) {
				st_index = 0;
				seven_segment.shift_counter = NUM_LEN + 4;
			} else {
				st_index = st_index + 1;
				seven_segment.shift_counter--;
			}
			last_time_shift = HAL_GetTick();
		}

		state = (state + 1) % 4;
		last_time = HAL_GetTick();

	}

}

#include <stdint.h>

uint64_t convertBytesToUint64(uint8_t *byteArray) {
	uint64_t result = 0;

	for (int i = 0; i < NUM_LEN; i++) {
		result;
	}

	return result;
}

void programInit(uint8_t *number) {

	seven_segment_set_num(number);
//	seven_segment_set_num(12345678987654321);
}

void programLoop() {
	seven_segment_refresh();

}

extern UART_HandleTypeDef huart2;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	static uint32_t last_time = 0;
	if (HAL_GetTick() - last_time < 100)
		return;

	if (GPIO_Pin == GPIO_PIN_1) {
		if (seven_segment.shift_flag == 0) {
			seven_segment.shift_flag = 1;
			HAL_UART_Transmit(&huart2, "Resumed", 7, 10);

		} else {
			seven_segment.shift_flag = 0;
			HAL_UART_Transmit(&huart2, "Paused", 6, 10);
		}
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
	}
	last_time = HAL_GetTick();

}
extern TIM_HandleTypeDef htim1;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim1) {
	if (htim1->Instance == TIM1) {
		programLoop();
	}
}
