#include "main.h"

// Input pull down rising edge trigger interrupt pins:
// Row1 PD3, Row2 PD5, Row3 PD7, Row4 PB4
GPIO_TypeDef *const Row_ports[] = {GPIOD, GPIOD, GPIOD, GPIOD};
const uint16_t Row_pins[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};
// Output pins: Column1 PD4, Column2 PD6, Column3 PB3, Column4 PB5
GPIO_TypeDef *const Column_ports[] = {GPIOD, GPIOD, GPIOD, GPIOD};
const uint16_t Column_pins[] = {GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13};
volatile uint32_t last_gpio_exti;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (last_gpio_exti + 200 > HAL_GetTick()) // Simple button debouncing
  {
    return;
  }
  last_gpio_exti = HAL_GetTick();

  int8_t row_number = -1;
  int8_t column_number = -1;

  if (GPIO_Pin == GPIO_PIN_0)
  {
    // blue_button_pressed = 1;
    // return;
  }

  for (uint8_t row = 0; row < 4; row++) // Loop through Rows
  {
    if (GPIO_Pin == Row_pins[row])
    {
      row_number = row;
    }
  }

  HAL_GPIO_WritePin(Column_ports[0], Column_pins[0], 0);
  HAL_GPIO_WritePin(Column_ports[1], Column_pins[1], 0);
  HAL_GPIO_WritePin(Column_ports[2], Column_pins[2], 0);
  HAL_GPIO_WritePin(Column_ports[3], Column_pins[3], 0);

  for (uint8_t col = 0; col < 4; col++) // Loop through Columns
  {
    HAL_GPIO_WritePin(Column_ports[col], Column_pins[col], 1);
    if (HAL_GPIO_ReadPin(Row_ports[row_number], Row_pins[row_number]))
    {
      column_number = col;
    }
    HAL_GPIO_WritePin(Column_ports[col], Column_pins[col], 0);
  }

  HAL_GPIO_WritePin(Column_ports[0], Column_pins[0], 1);
  HAL_GPIO_WritePin(Column_ports[1], Column_pins[1], 1);
  HAL_GPIO_WritePin(Column_ports[2], Column_pins[2], 1);
  HAL_GPIO_WritePin(Column_ports[3], Column_pins[3], 1);

  if (row_number == -1 || column_number == -1)
  {
    return; // Reject invalid scan
  }
  //   C0   C1   C2   C3
  // +----+----+----+----+
  // | 1  | 2  | 3  | 4  |  R0
  // +----+----+----+----+
  // | 5  | 6  | 7  | 8  |  R1
  // +----+----+----+----+
  // | 9  | 10 | 11 | 12 |  R2
  // +----+----+----+----+
  // | 13 | 14 | 15 | 16 |  R3
  // +----+----+----+----+
  const uint8_t button_number = row_number * 4 + column_number + 1;
  switch (button_number)
  {
  case 1:
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, 1);
    break;
  case 2:
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, 0);
    break;
  case 3:
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 1);
    break;
  case 4:
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 0);
    break;
  case 5:
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
    break;
  case 6:
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
    break;
  case 7:
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
    break;
  case 8:
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
    break;
  case 9:
    /* code */
    break;
  case 10:
    /* code */
    break;
  case 11:
    /* code */
    break;
  case 12:
    /* code */
    break;
  case 13:
    /* code */
    break;
  case 14:
    /* code */
    break;
  case 15:
    /* code */
    break;
  case 16:
    /* code */
    break;

  default:
    break;
  }
}
