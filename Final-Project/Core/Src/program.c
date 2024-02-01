#include "main.h"
//#include <math.h>
//#include <strings.h>
//#include <time.h>





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
}player;

player player1 = {
		.player_name = 'A',
		.position_x = 19,
		.position_y = 2,
		.direction = 1,
		.points = 0,
		.health = 3,
		.arrow = 5
};

player player2 = {
		.player_name = 'B',
		.position_x = 0,
		.position_y = 1,
		.direction = 3,
		.points = 0,
		.health = 3,
		.arrow = 5
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



void programInit() {
	LiquidCrystal(GPIOC, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_9, GPIO_PIN_8, GPIO_PIN_7);
	begin(20, 4);


//temp
//	char data[100];
//	int n = sprintf(data, "Salammmmm");
//	setCursor(5, 1);



//	createChar(num_health, health);
//	createChar(num_arrow_vert, arrow_vert);
//	createChar(num_arrow_hori, arrow_hori);

	createChar(num_tank_right, tank_right);
	createChar(num_tank_up, tank_up);
	createChar(num_tank_down, tank_down);
	createChar(num_tank_left, tank_left);
	createChar(num_extra_bullet, extra_bullet);
	createChar(num_chance, chance);
	createChar(num_health, health);
	createChar(num_arrow, arrow);


	init_board();



//	lcd[0][0] = num_tank_right; //heart
//	lcd[1][0] = num_tank_up; //chance
//	lcd[2][0] = num_tank_down; //obstacle
//	lcd[3][0] = num_tank_left; //tank_left
//	lcd[4][0] = num_wall; //wall
//	lcd[5][0] = num_extra_bullet; //extrab
//	lcd[6][0] = num_chance; //chance
//	lcd[7][0] = num_obstacle; //obstacle
//	lcd[8][0] = num_health; //health
//	lcd[9][0] = num_arrow; //chance



	update_board();
//	print(data);

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

void update_board(){

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 4; j++) {
        	setCursor(i, j);
        	if(lcd[i][j] != 9 && lcd[i][j] != 10 && lcd[i][j] != 0){
        		write(lcd[i][j]);
        	}
        	else if (lcd[i][j] == 9 ){
        		print("#");
        	}
        	else if (lcd[i][j] == 10 ){
        		print("I");
        	}
        	else if (lcd[i][j] == 0){
        		print(" ");

        	}
        	else{
        		print("U");

        	}
        }
    }
}

void programLoop() {
    seven_segment_refresh();
    setNumber(1234);
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
	//TODO

}

void move(int player){
	int dir;

	if (player == 1){
		dir = player1.direction;
		if (dir == 1){
			int curr_col = player1.position_x;
			int curr_row = player1.position_y;
			if(curr_col - 1 >= 0 && lcd[curr_col - 1][curr_row] != num_obstacle && lcd[curr_col - 1][curr_row] != num_wall){
				lcd[curr_col][curr_row] = 0; //TODO add prizes
				lcd[curr_col - 1][curr_row] = player1.direction;
				player1.position_x = player1.position_x - 1;
			}
		}else if (dir == 2){
			int curr_col = player1.position_x;
			int curr_row = player1.position_y;
			if(curr_row - 1 >= 0 && lcd[curr_col][curr_row - 1] != num_obstacle && lcd[curr_col][curr_row - 1] != num_wall){
				lcd[curr_col][curr_row] = 0; //TODO add prizes
				lcd[curr_col][curr_row - 1] = player1.direction;
				player1.position_y = player1.position_y - 1;
			}
		}else if (dir == 3){
			int curr_col = player1.position_x;
			int curr_row = player1.position_y;
			if(curr_col + 1 <= 19 && lcd[curr_col + 1][curr_row] != num_obstacle && lcd[curr_col + 1][curr_row] != num_wall){
				lcd[curr_col][curr_row] = 0; //TODO add prizes
				lcd[curr_col + 1][curr_row] = player1.direction;
				player1.position_x = player1.position_x + 1;
			}
		}else if (dir == 4){
			int curr_col = player1.position_x;
			int curr_row = player1.position_y;
			if(curr_row + 1 <= 3 && lcd[curr_col][curr_row + 1] != num_obstacle && lcd[curr_col][curr_row + 1] != num_wall){
				lcd[curr_col][curr_row] = 0; //TODO add prizes
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
				lcd[curr_col][curr_row] = 0; //TODO add prizes
				lcd[curr_col - 1][curr_row] = player2.direction;
				player2.position_x = player2.position_x - 1;
			}
		}else if (dir == 2){
			int curr_col = player2.position_x;
			int curr_row = player2.position_y;
			if(curr_row - 1 >= 0 && lcd[curr_col][curr_row - 1] != num_obstacle && lcd[curr_col][curr_row - 1] != num_wall){
				lcd[curr_col][curr_row] = 0; //TODO add prizes
				lcd[curr_col][curr_row - 1] = player2.direction;
				player2.position_y = player2.position_y - 1;
			}
		}else if (dir == 3){
			int curr_col = player2.position_x;
			int curr_row = player2.position_y;
			if(curr_col + 1 <= 19 && lcd[curr_col + 1][curr_row] != num_obstacle && lcd[curr_col + 1][curr_row] != num_wall){
				lcd[curr_col][curr_row] = 0; //TODO add prizes
				lcd[curr_col + 1][curr_row] = player2.direction;
				player2.position_x = player2.position_x + 1;
			}
		}else if (dir == 4){
			int curr_col = player2.position_x;
			int curr_row = player2.position_y;
			if(curr_row + 1 <= 3 && lcd[curr_col][curr_row + 1] != num_obstacle && lcd[curr_col][curr_row + 1] != num_wall){
				lcd[curr_col][curr_row] = 0; //TODO add prizes
				lcd[curr_col][curr_row + 1] = player2.direction;
				player2.position_y = player2.position_y + 1;
			}
		}
	}


}
