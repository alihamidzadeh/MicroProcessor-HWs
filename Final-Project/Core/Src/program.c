#include "main.h"
//#include <math.h>
//#include <strings.h>
//#include <time.h>


int pageflag = 0;
int menu_curser_r = 1;

extern TIM_HandleTypeDef htim4;
int game_started = 0;


// Input pull down rising edge trigger interrupt pins:
// Row1 PD3, Row2 PD5, Row3 PD7, Row4 PB4
GPIO_TypeDef *const Row_ports[] = {GPIOD, GPIOD, GPIOD, GPIOD};
const uint16_t Row_pins[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};
// Output pins: Column1 PD4, Column2 PD6, Column3 PB3, Column4 PB5
GPIO_TypeDef *const Column_ports[] = {GPIOD, GPIOD, GPIOD, GPIOD};
const uint16_t Column_pins[] = {GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13};
volatile uint32_t last_gpio_exti;

int change_page = 0;

extern RTC_HandleTypeDef hrtc;

RTC_TimeTypeDef mytime ;
RTC_DateTypeDef mydate ;

void set_start_time(int hour, int minute, int second){

	RTC_TimeTypeDef start_t ;

	start_t.Hours = 20;
	start_t.Minutes = 20;
	start_t.Seconds = 20;

    HAL_RTC_SetTime(&hrtc, &start_t, RTC_FORMAT_BIN);
}

void set_start_day(int year, int month, int day){

	RTC_DateTypeDef start_t ;

	start_t.Year = 20;
	start_t.Month = 20;
	start_t.Date = 20;

    HAL_RTC_SetDate(&hrtc, &start_t, RTC_FORMAT_BIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (last_gpio_exti + 70 > HAL_GetTick()) // Simple button debouncing
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
  switch (button_number){
  case 1: //move	//1
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, 1);
	move(1);

    break;
  case 2: //boom 	//2
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, 0);
	boom(1);
    break;
  case 3:
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 1);
    break;
  case 4:
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 0);
    break;
  case 5: //dir	 	//4
	 HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 1);
	 change_dir(1);
    break;
  case 6:
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);
    break;
  case 7:
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, 1);
    break;
  case 8:
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, 0);
    break;
  case 9: // menu curser up
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 1);
		if(pageflag==1){
			if (menu_curser_r > 1){
				menu_curser_r -= 1;
			}
		}
    break;
  case 10: // menu cursor down
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 0);
		if(pageflag==1){
			if (menu_curser_r  < 3){
				menu_curser_r +=1;
			}
		}
    break;
  case 11:
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, 1);
    break;
  case 12: //Dir	//C
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, 0);
		change_dir(2);
    break;
  case 13: //goto menu
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
		if(pageflag == 0 || pageflag == 3 || pageflag == 4){
			change_page = 1;
			pageflag = 1;
		}
    break;
  case 14: // select in menu
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		if(pageflag == 1){
			if(menu_curser_r == 1){
				pageflag = 2;
				change_page = 1;
				game_started = 1;
			}
			else if(menu_curser_r == 2){
				pageflag = 3;
				change_page = 1;

			}

			else if(menu_curser_r == 3){
				pageflag = 4;
				change_page = 1;

			}
		}

    break;
  case 15: //Boom	//#
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		boom(2);
    break;
  case 16: //move	//D
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
		move(2);
    break;

  default:
    break;
  }
}


unsigned char obstacle[] = {
  0x1F,
  0x1F,
  0x0E,
  0x0E,
  0x0E,
  0x0E,
  0x1F,
  0x1F
};

unsigned char extra_bullet[] = {
  0x08,
  0x1C,
  0x08,
  0x00,
  0x1E,
  0x1F,
  0x1E,
  0x00
};

unsigned char chance[] = {
  0x1F,
  0x11,
  0x15,
  0x1D,
  0x1B,
  0x1B,
  0x1F,
  0x1B
};



unsigned char wall[] = {
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};


unsigned char arrow_vert[] = {
  0x00,
  0x0C,
  0x0C,
  0x0C,
  0x0C,
  0x0C,
  0x00,
  0x00
};

unsigned char arrow_hori[] = {
  0x00,
  0x00,
  0x00,
  0x1F,
  0x1F,
  0x00,
  0x00,
  0x00
};

unsigned char tank_up[] = {
  0x00,
  0x00,
  0x00,
  0x04,
  0x04,
  0x1F,
  0x1F,
  0x1F
};

unsigned char tank_left[] = {
  0x07,
  0x07,
  0x07,
  0x1F,
  0x1F,
  0x07,
  0x07,
  0x07
};

unsigned char tank_right[] = {
  0x1C,
  0x1C,
  0x1C,
  0x1F,
  0x1F,
  0x1C,
  0x1C,
  0x1C
};



unsigned char health[] = {
  0x00,
  0x00,
  0x0A,
  0x1F,
  0x1F,
  0x1F,
  0x0E,
  0x04
};

unsigned char  tank_down[] = {
  0x1F,
  0x1F,
  0x1F,
  0x04,
  0x04,
  0x00,
  0x00,
  0x00
};

unsigned char arrow[] = {
  0x00,
  0x00,
  0x0E,
  0x0E,
  0x0E,
  0x00,
  0x00,
  0x00
};

typedef struct {
	char player_name;
	int position_x;
	int position_y;
	int direction;
	int points;
	int health;
	int arrow;
	int player_id;

}player;



player player1 = {
		.player_name = 'A',
		.position_x = 19,
		.position_y = 2,
		.direction = 1,
		.points = 0,
		.health = 3,
		.arrow = 5,
		.player_id = 1
};

player player2 = {
		.player_name = 'B',
		.position_x = 0,
		.position_y = 1,
		.direction = 3,
		.points = 0,
		.health = 3,
		.arrow = 5,
		.player_id = 2

};

int num_tank_left = 1;
int num_tank_up = 2;
int num_tank_right = 3;
int num_tank_down = 4;
int num_extra_bullet = 5;
int num_chance = 6;
int num_health = 7;
int num_arrow = 8;

int num_wall = 9;
int num_obstacle = 10;

uint8_t lcd [20][4];

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
		   {.port=GPIOB, .pin=GPIO_PIN_2}, //D4
		   {.port=GPIOB, .pin=GPIO_PIN_0}, //D3
		   {.port=GPIOC, .pin=GPIO_PIN_4}, //D2
		   {.port=GPIOA, .pin=GPIO_PIN_4}}, //D1

	.BCD_input={
			{.port=GPIOC, .pin=GPIO_PIN_6}, //A
			{.port=GPIOD, .pin=GPIO_PIN_15}, //B
			{.port=GPIOD, .pin=GPIO_PIN_9}, //C
			{.port=GPIOB, .pin=GPIO_PIN_13}}, //D
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


struct bullet{
	int player_id;
	int position_x;
	int position_y;
	int direction;
	int active;
};

struct bullet bul[10];

void programInit() {
	LiquidCrystal(GPIOC, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_9, GPIO_PIN_8, GPIO_PIN_7);
	begin(20, 4);
//    setNumber(1234);

	//temp
//	char data[100];
//	int n = sprintf(data, "Salammmmm");
//	setCursor(5, 1);
	//	print(data);


	set_start_time(20, 20, 20);
	set_start_day(20,20,20);
	createChar(num_tank_right, tank_right);
	createChar(num_tank_up, tank_up);
	createChar(num_tank_down, tank_down);
	createChar(num_tank_left, tank_left);
	createChar(num_extra_bullet, extra_bullet);
	createChar(num_chance, chance);
	createChar(num_health, health);
	createChar(num_arrow, arrow);
//	starter();
	pageflag=0;
	for(int i = 0; i < 10; i++){
		bul[i].active=0;
		bul[i].direction=1;
		bul[i].player_id=1;
		bul[i].position_x=-1;
		bul[i].position_y=-1;

	}
//	init_board();


}

void starter(){
	setCursor(0, 2);
	write(num_tank_right, tank_right);

	setCursor(1, 1);
	print("#");

	setCursor(1, 2);
	print("#");

	char data[100];
	int n = sprintf(data, "TANK BATTLE");
	setCursor(4, 1);
	print(data);
	setCursor(4, 2);
	n = sprintf(data, "===========");
	print(data);

	setCursor(18, 1);
	print("#");
	setCursor(18, 2);
	print("#");

	setCursor(19, 1);
	write(num_tank_left, tank_left);

}

void init_board(){
	//i == soton, j ==> radif

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 4; j++) {
        	setCursor(i, j);
        	lcd[i][j] = 0;
//    		print(" ");

        }
    }

	lcd[1][1] = num_wall;
	lcd[1][2] = num_wall;
	lcd[18][1] = num_wall;
	lcd[18][2] = num_wall;

	lcd[0][1] = num_tank_right;
	lcd[19][2] = num_tank_left;

	int element_counter = 2;
	while(element_counter > 0){ //extra_bullet
		int col = rand() % 24;
		int row = rand() % 4;
		if(lcd[col][row] == 0){
			lcd[col][row] = num_extra_bullet;
			element_counter--;
		}
	}

	element_counter = 2;
	while(element_counter > 0){ //health
		int col = rand() % 24;
		int row = rand() % 4;
		if(lcd[col][row] == 0){
			lcd[col][row] = num_health;
			element_counter--;
		}
	}

	element_counter = 2;
	while(element_counter > 0){ //obstacle
		int col = rand() % 24;
		int row = rand() % 4;
		if(lcd[col][row] == 0){
			lcd[col][row] = num_obstacle;
			element_counter--;
		}
	}

}
// LCD pins
// D8 -> C0
// D9 -> C1
// D10 -> C2
// D11 -> C3
// D12 -> C9
// D13 -> C8
// D14 -> C7

void update_lcd(){
	if (game_started==1){
		clear();
		init_board();
		change_page = 0;
		game_started = 0;
	}

	if(pageflag==0){
		starter();
	}


	if(pageflag==2){
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 4; j++) {
				setCursor(i, j);
				if(lcd[i][j] != 9 && lcd[i][j] != 10 && lcd[i][j] != 0)
					write(lcd[i][j]);
				else if (lcd[i][j] == 9)
					print("#");
				else if (lcd[i][j] == 10)
					print("I");
				else if (lcd[i][j] == 0)
					print(" ");
				else
					print("U");
			}
		}
	}
	else if (change_page==1){
		clear();
		change_page = 0;
	}

	else if (change_page==1){
		clear();
		change_page = 0;
	}
	else if (pageflag==1){
		menu();

	}
	else if (pageflag==3){
		setting_page();
	}
	else if (pageflag==4){
		about_page();
	}

}
void about_page(){
	setCursor(6, 1);
	print("POURIA");
	setCursor(8, 2);
	print("ALI");
	char timeStr[100];
	char dateStr[100];

	setCursor(5, 3);
	print("          ");
	HAL_RTC_GetTime(&hrtc, &mytime, RTC_FORMAT_BIN);
//	HAL_RTC_GetDate(&hrtc, &mytime, RTC_FORMAT_BIN);

	sprintf(timeStr, "%02d:%02d:%02d", mytime.Hours, mytime.Minutes, mytime.Seconds);

	setCursor(5, 3);
	print(timeStr);

}

void setting_page(){
	setCursor(6, 1);
	print("KHALI");

}
//void remove_starter(){
//	if (change_page == 1){
//			setCursor(0, 2);
//			print(" ");
//
//			setCursor(1, 1);
//			print(" ");
//
//			setCursor(1, 2);
//			print(" ");
//
//			char data[100];
//			int n = sprintf(data, "           ");
//			setCursor(4, 1);
//			print(data);
//			setCursor(4, 2);
//			n = sprintf(data, "           ");
//			print(data);
//
//			setCursor(18, 1);
//			print(" ");
//			setCursor(18, 2);
//			print(" ");
//
//			setCursor(19, 1);
//			print(" ");
//			pageflag = 1;
//
//		}
//}

void menu(){

	setCursor(1, 0);
	print("MENU:");
	setCursor(1, 1);
	print("PLAY");
	setCursor(1, 2);
	print("SETTING");
	setCursor(1, 3);
	print("ABOUT");
	setCursor(0, 0);
	print(" ");
	setCursor(0, 1);
	print(" ");
	setCursor(0, 2);
	print(" ");
	setCursor(0, 3);
	print(" ");
	setCursor(0, menu_curser_r);
	print(">");

}

void programLoop() {
//    seven_segment_refresh();

    update_lcd();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM4) {
//		update_lcd();
		seven_segment_refresh();
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_11);
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_10);

	}

}


void change_dir(int player){
	if (player == 1){
		int new_dir = player1.direction + 1;
		if(new_dir == 5){
			new_dir = 1;
		}
		player1.direction = new_dir;
		lcd[player1.position_x][player1.position_y] = player1.direction;

	}else if (player == 2){
		int new_dir = player2.direction + 1;
		if(new_dir == 5){
			new_dir = 1;
		}
		player2.direction = new_dir;
		lcd[player2.position_x][player2.position_y] = player2.direction;

	}
}

void boom(int player){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0); //Temp
	int i;
	for(i = 0; i<10;i++){
		if(bul[i].active==0)
			break;
	}
	bul[i].active = 1;
	if (player == 1){
		bul[i].direction=player1.direction;
		bul[i].position_x=player1.position_x;
		bul[i].position_y=player1.position_y;
		bul[i].player_id=1;
		player1.arrow--;

	}else{
		bul[i].direction=player2.direction;
		bul[i].position_x=player2.position_x;
		bul[i].position_y=player2.position_y;
		bul[i].player_id=2;
		player2.arrow--;

	}
//entesab be array

}

int sev_result = 0;
void collect(uint8_t pos, int player){
	if (pos == num_extra_bullet){
		if (player == 1)
			player1.arrow+=1;
		else
			player2.arrow+=1;
	}else if (pos == num_health){
		if (player == 1)
			player1.health+=1;
		else
			player2.health+=1;
	}

//	uint8_t a = player2.arrow;
//	uint8_t b = player2.health;
//	uint8_t c = player1.arrow;
//	uint8_t d = player1.health;
//	int results[4] = {d,c,b,a};
//	seven_segment_set_num(results);

	sev_result = player2.arrow * 1000 + player2.health * 100 + player1.arrow * 10 + player1.health;
	setNumber(sev_result);
}

void test shelik{
	for(i = 0; i<10;i++){
		if(bul[i].active==1){
			//check direction
			// check next
			// update position and show
			//active =0
		}
	}
}
void move(int player){
	int dir;

	if (player == 1){
		dir = player1.direction;
		if (dir == 1){
			int curr_col = player1.position_x;
			int curr_row = player1.position_y;
			if(curr_col - 1 >= 0 && lcd[curr_col - 1][curr_row] != num_obstacle && lcd[curr_col - 1][curr_row] != num_wall){
				collect(lcd[curr_col - 1][curr_row], 1); //TODO add prizes
				lcd[curr_col][curr_row] = 0;
				lcd[curr_col - 1][curr_row] = player1.direction;
				player1.position_x = player1.position_x - 1;
			}
		}else if (dir == 2){
			int curr_col = player1.position_x;
			int curr_row = player1.position_y;
			if(curr_row - 1 >= 0 && lcd[curr_col][curr_row - 1] != num_obstacle && lcd[curr_col][curr_row - 1] != num_wall){
				collect(lcd[curr_col][curr_row - 1], 1); //TODO add prizes
				lcd[curr_col][curr_row] = 0;
				lcd[curr_col][curr_row - 1] = player1.direction;
				player1.position_y = player1.position_y - 1;
			}
		}else if (dir == 3){
			int curr_col = player1.position_x;
			int curr_row = player1.position_y;
			if(curr_col + 1 <= 19 && lcd[curr_col + 1][curr_row] != num_obstacle && lcd[curr_col + 1][curr_row] != num_wall){
				collect(lcd[curr_col + 1][curr_row], 1); //TODO add prizes
				lcd[curr_col][curr_row] = 0;
				lcd[curr_col + 1][curr_row] = player1.direction;
				player1.position_x = player1.position_x + 1;
			}
		}else if (dir == 4){
			int curr_col = player1.position_x;
			int curr_row = player1.position_y;
			if(curr_row + 1 <= 3 && lcd[curr_col][curr_row + 1] != num_obstacle && lcd[curr_col][curr_row + 1] != num_wall){
				collect(lcd[curr_col][curr_row + 1], 1); //TODO add prizes
				lcd[curr_col][curr_row] = 0;
				lcd[curr_col][curr_row + 1] = player1.direction;
				player1.position_y = player1.position_y + 1;
			}
		}
	}else if (player == 2){
		dir = player2.direction;
		if (dir == 1){
			int curr_col = player2.position_x;
			int curr_row = player2.position_y;
			if(curr_col - 1 >= 0 && lcd[curr_col - 1][curr_row] != num_obstacle && lcd[curr_col - 1][curr_row] != num_wall){
				collect(lcd[curr_col - 1][curr_row], 2); //TODO add prizes
				lcd[curr_col][curr_row] = 0;
				lcd[curr_col - 1][curr_row] = player2.direction;
				player2.position_x = player2.position_x - 1;
			}
		}else if (dir == 2){
			int curr_col = player2.position_x;
			int curr_row = player2.position_y;
			if(curr_row - 1 >= 0 && lcd[curr_col][curr_row - 1] != num_obstacle && lcd[curr_col][curr_row - 1] != num_wall){
				collect(lcd[curr_col][curr_row - 1], 2); //TODO add prizes
				lcd[curr_col][curr_row] = 0;
				lcd[curr_col][curr_row - 1] = player2.direction;
				player2.position_y = player2.position_y - 1;
			}
		}else if (dir == 3){
			int curr_col = player2.position_x;
			int curr_row = player2.position_y;
			if(curr_col + 1 <= 19 && lcd[curr_col + 1][curr_row] != num_obstacle && lcd[curr_col + 1][curr_row] != num_wall){
				collect(lcd[curr_col + 1][curr_row], 2); //TODO add prizes
				lcd[curr_col][curr_row] = 0;
				lcd[curr_col + 1][curr_row] = player2.direction;
				player2.position_x = player2.position_x + 1;
			}
		}else if (dir == 4){
			int curr_col = player2.position_x;
			int curr_row = player2.position_y;
			if(curr_row + 1 <= 3 && lcd[curr_col][curr_row + 1] != num_obstacle && lcd[curr_col][curr_row + 1] != num_wall){
				collect(lcd[curr_col][curr_row + 1], 2); //TODO add prizes
				lcd[curr_col][curr_row] = 0;
				lcd[curr_col][curr_row + 1] = player2.direction;
				player2.position_y = player2.position_y + 1;
			}
		}
	}
}

char character;
char input[50];
int  index_arr = 0;
extern UART_HandleTypeDef huart1;

void uart_rx_enable_it(void) {
	HAL_UART_Receive_IT(&huart1, &character, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	char data[100];

    if (huart->Instance == USART1){
    	if(character != 10){
    		input[index_arr++] = character;
    	}else{
			input[index_arr++] = '\0';
			index_arr = 0;
			int value;
			HAL_RTC_GetTime(&hrtc, &mytime, RTC_FORMAT_BIN);
//			sprintf(timeStr, "%02d:%02d:%02d", mytime.Hours, mytime.Minutes, mytime.Seconds);
		}
		uart_rx_enable_it();
    }
}


