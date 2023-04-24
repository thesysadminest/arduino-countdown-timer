#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x38, 16, 4);  // set the LCD address to 0x38 for a 16 chars and 4 line display
SoftwareSerial esp01(7, 6);

unsigned long time_local = 1672542000;
unsigned long time_local_last_sync;
unsigned long time_end = 1684832400;

unsigned long last_button_push_time;
bool update_whole_screen = true;
unsigned long last_screen_update_time;

uint8_t rus_v[] = {0x0, 0x0, 0x1c, 0x12, 0x1c, 0x12, 0x1c, 0x0};
uint8_t rus_G[] = {0x1e, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0};
uint8_t rus_D[] = {0x6, 0xa, 0xa, 0xa, 0xa, 0xa, 0x1f, 0x11};
uint8_t rus_d[] = {0x0, 0x0, 0x6, 0xa, 0xa, 0xa, 0x1f, 0x11};
uint8_t rus_YO[] = {0xa, 0x0, 0x1e, 0x10, 0x1e, 0x10, 0x1e, 0x0};
uint8_t rus_yo[] = {0xa, 0x0, 0xe, 0x11, 0x1f, 0x10, 0xe, 0x0};
uint8_t rus_z[] = {0x0, 0x0, 0x1e, 0x2, 0xc, 0x2, 0x1c, 0x0};
uint8_t rus_I[] = {0x11, 0x11, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0};
uint8_t rus_i[] = {0x0, 0x0, 0x12, 0x12, 0x16, 0x1a, 0x12, 0x0};
uint8_t rus_j[] = {0xc, 0x0, 0x12, 0x12, 0x16, 0x1a, 0x12, 0x0};
uint8_t rus_k[] = {0x0, 0x0, 0x12, 0x14, 0x18, 0x14, 0x12, 0x0};
uint8_t rus_L[] = {0x6, 0xa, 0xa, 0xa, 0x12, 0x12, 0x12, 0x0};
uint8_t rus_M[] = {0x11, 0x1b, 0x15, 0x11, 0x11, 0x11, 0x11, 0x0};
uint8_t rus_m[] = {0x0, 0x0, 0x11, 0x1b, 0x15, 0x11, 0x11, 0x0};
uint8_t rus_n[] = {0x0, 0x0, 0x12, 0x12, 0x1e, 0x12, 0x12, 0x0};
uint8_t rus_P[] = {0x1f, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0};
uint8_t rus_p[] = {0x0, 0x0, 0x1e, 0x12, 0x12, 0x12, 0x12, 0x0};
uint8_t rus_T[] = {0x1f, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x0};
uint8_t rus_t[] = {0x0, 0x0, 0x1c, 0x8, 0x8, 0x8, 0x8, 0x0};
uint8_t rus_u[] = {0x0, 0x0, 0x12, 0x12, 0xe, 0x2, 0x12, 0xc};
uint8_t rus_CH[] = {0x12, 0x12, 0x12, 0x1e, 0x2, 0x2, 0x2, 0x0};
uint8_t rus_ch[] = {0x0, 0x0, 0x12, 0x12, 0x1e, 0x2, 0x2, 0x0};
uint8_t rus_sh[] = {0x0, 0x0, 0x15, 0x15, 0x15, 0x15, 0x1f, 0x0};
uint8_t rus_YI[] = {0x11, 0x11, 0x11, 0x1d, 0x15, 0x15, 0x1d, 0x0};
uint8_t rus_YA[] = {0xe, 0x12, 0xe, 0x12, 0x12, 0x12, 0x12, 0x0};

uint8_t snake_up[] = {0x1f, 0x1f, 0x1f, 0x1f, 0x0, 0x0, 0x0, 0x0};
uint8_t snake_down[] = {0x0, 0x0, 0x0, 0x0, 0x1f, 0x1f, 0x1f, 0x1f};
uint8_t snake_full[] = {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};

uint8_t food_up[] = {0x0, 0x4, 0xa, 0x4, 0x0, 0x0, 0x0, 0x0};
uint8_t food_down[] = {0x0, 0x0, 0x0, 0x0, 0x4, 0xa, 0x4, 0x0};

uint8_t snake_up_food[] = {0x1f, 0x1f, 0x1f, 0x1f, 0x4, 0xa, 0x4, 0x0};
uint8_t snake_down_food[] = {0x0, 0x4, 0xa, 0x4, 0x1f, 0x1f, 0x1f, 0x1f};

uint8_t sieve_up[] = {0x15, 0xa, 0x15, 0xa, 0x0, 0x0, 0x0, 0x0};
uint8_t sieve_down[] = {0x0, 0x0, 0x0, 0x0, 0x15, 0xa, 0x15, 0xa};
uint8_t sieve_full[] = {0x15, 0xa, 0x15, 0xa, 0x15, 0xa, 0x15, 0xa};

uint8_t snake_up_sieve[] = {0x1f, 0x1f, 0x1f, 0x1f, 0x15, 0xa, 0x15, 0xa};
uint8_t snake_down_sieve[] = {0x15, 0xa, 0x15, 0xa, 0x1f, 0x1f, 0x1f, 0x1f};

char screen_time_mode = 4;
unsigned long disp_time_prev_int = 0;
char disp_time_prev_frac = 0;

bool buttons_state[] = {false, false, false, false};

void arr_copy (char* a[4][4], char* b[4][4]) {
    for (char i = 0; i < 4; ++i) {
        for (char j = 0; j < 4; ++j) {
            a[i][j] = b[i][j];
        }
    }
    return;
}

struct StupidBitBool {
    char data[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  
    bool getState (char column, char row) {
        char place = row * 2;
        if (column >= 8) { ++place; column -= 8; }
        return data[place] & (1 << column);
    }
  
    void setState (char column, char row, bool state) {
        char place = row * 2;
        if (column >= 8) { ++place; column -= 8; }
        if (!state) data[place] &= ~(1 << column);
        else {
            char cur = data[place];
            cur = ~cur;
            data[place] = ~(cur & ~(1 << column));
        }
        return;
    }
};

void lcd_setCursor_safe (char col, char row) {
    if (row < 2) { lcd.setCursor(col, row); }
    else { lcd.setCursor(col - 4, row); }
    return;
}

void lcd_setCursor_safe_div2 (char col, char row) {
    lcd_setCursor_safe (col, row / 2);
    return;
}


StupidBitBool left, right, up, down;
char snake_tail_col, snake_tail_row, snake_head_col, snake_head_row, old_snake_tail_col, old_snake_tail_row, food_col, food_row;
int game_score;

StupidBitBool tetris_field;
bool tetris_figure[4][4];
bool tetris_figure_prev[4][4];
char tetris_figure_row, tetris_figure_col, tetris_figure_spes, tetris_figure_rotation;
char tetris_figure_row_prev, tetris_figure_col_prev;

char next_game_switcher = 0;


bool tetris_figure_check_laid () {
    for (char i = 0; i < 4; ++i) {
        for (char j = 0; j < 4; ++j) {
            if (tetris_figure[i][j]) {
                if (tetris_figure_col + j >= 15) { return true; }
                if (tetris_figure_col + j + 1 < 16 && tetris_figure_row + i < 8 &&
                    tetris_field.getState(tetris_figure_col + j + 1, tetris_figure_row + i)) { return true; }
            }
        }
    }
    return false;
}

bool tetris_figure_check_collision () {
    for (char i = 0; i < 4; ++i) {
        for (char j = 0; j < 4; ++j) {
            if (tetris_figure[i][j]) {
                if (tetris_figure_col + j < 0 || tetris_figure_col + j >= 16) { return true; }
                if (tetris_figure_row + i < 0 || tetris_figure_row + i >= 8) { return true; }
                if (tetris_field.getState(tetris_figure_col + j, tetris_figure_row + i)) { return true; }
            }
        }
    }
    return false;
}

void tetris_figure_push_left (bool (&a)[4][4]) {
    for (char j = 0; j < 3; ++j) {
        for (char i = 0; i < 4; ++i) {
            a[i][j] = a[i][j+1];
        }
    }
    for (char i = 0; i < 4; ++i) {
        a[i][3] = false;
    }
    return;
}

void tetris_figure_push_right (bool (&a)[4][4]) {
    for (char j = 2; j > -1; --j) {
        for (char i = 0; i < 4; ++i) {
            a[i][j] = a[i][j-1];
        }
    }
    for (char i = 0; i < 4; ++i) {
        a[i][0] = false;
    }
    return;
}

void tetris_figure_push_up (bool (&a)[4][4]) {
    for (char i = 0; i < 3; ++i) {
        for (char j = 0; j < 4; ++j) {
            a[i][j] = a[i+1][j];
        }
    }
    for (char j = 0; j < 4; ++j) {
        a[3][j] = false;
    }
    return;
}

void tetris_figure_push_down (bool (&a)[4][4]) {
    for (char i = 2; i > -1; --i) {
        for (char j = 0; j < 4; ++j) { 
            a[i][j] = a[i-1][j];
        }
    }
    for (char j = 0; j < 4; ++j) { 
        a[0][j] = false;
    }
    return;
}


void tetris_figure_rotate_clockwise () {
    bool b[4][4];
    for (char i = 0; i < 4; ++i) {
        for (char j = 0; j < 4; ++j) {
            b[j][3-i] = tetris_figure[i][j];
        }
    }
    for (char i = 0; i < 4; ++i) {
        for (char j = 0; j < 4; ++j) {
            tetris_figure[i][j] = b[i][j];
        }
    }
            
    ++tetris_figure_rotation;
    if (tetris_figure_spes == 0) {tetris_figure_rotation = 0;}
    if (tetris_figure_spes >= 1 && tetris_figure_spes <= 3 && tetris_figure_rotation >= 2) {tetris_figure_rotation -= 2;}
    if (tetris_figure_spes >= 4 && tetris_figure_rotation >= 4) {tetris_figure_rotation -= 4;}

    if (tetris_figure_spes == 1 && tetris_figure_rotation == 1) {tetris_figure_push_left(tetris_figure);}
    if (tetris_figure_spes == 2 || tetris_figure_spes == 3) {
        tetris_figure_push_left(tetris_figure);
        if (tetris_figure_rotation == 0) 
            {tetris_figure_push_left(tetris_figure);}
    }
    if (tetris_figure_spes >= 4) {tetris_figure_push_left(tetris_figure);}
    return;
}

void tetris_figure_rotate_counterclockwise () {
    bool b[4][4];
    for (char i = 0; i < 4; ++i) {
        for (char j = 0; j < 4; ++j) {
            b[3-j][i] = tetris_figure[i][j];
        }
    }
    for (char i = 0; i < 4; ++i) {
        for (char j = 0; j < 4; ++j) {
            tetris_figure[i][j] = b[i][j];
        }
    }
    
    --tetris_figure_rotation;
    if (tetris_figure_spes == 0) {tetris_figure_rotation = 0;}
    if (tetris_figure_spes >= 1 && tetris_figure_spes <= 3 && tetris_figure_rotation < 0) {tetris_figure_rotation += 2;}
    if (tetris_figure_spes >= 4 && tetris_figure_rotation < 0) {tetris_figure_rotation += 4;}

    if (tetris_figure_spes == 1 && tetris_figure_rotation == 0) {tetris_figure_push_up(tetris_figure);}
    if (tetris_figure_spes == 2 || tetris_figure_spes == 3) {
        tetris_figure_push_up(tetris_figure);
        if (tetris_figure_rotation == 1) 
            {tetris_figure_push_up(tetris_figure);}
    }
    if (tetris_figure_spes >= 4) {tetris_figure_push_up(tetris_figure);}
    return;
}

void tetris_figure_initialize (bool (&a)[4][4], char q1, char q2, char q3, char q4) {
    a[q1 / 4][q1 % 4] = true;
    a[q2 / 4][q2 % 4] = true;
    a[q3 / 4][q3 % 4] = true;
    a[q4 / 4][q4 % 4] = true;
    return;
}

char num_get_digits_count (unsigned long n) {
    char ans = 0;
    if (n == 0) ans = 1;
    while (n != 0) {
        n /= 10;
        ++ans;
    }
    return ans;
}



void setup() {  
    pinMode(2, INPUT); // button_left
    pinMode(3, INPUT); // button_right
    pinMode(4, INPUT); // button_up
    pinMode(5, INPUT); // button_down
    pinMode(8, OUTPUT); // esp power
    digitalWrite(8, LOW);
  
    lcd.init();
    lcd.backlight();
    lcd.clear();

    //DEBUG: Serial.begin(9600);

    while (!timeInit()) {
        digitalWrite(8, LOW);
    }
    digitalWrite(8, LOW);
  
    return;
}

void loop() {
    next_game_switcher = 0;
    update_whole_screen = true;
    
    transition();
    while (timeLoop()) {};

    if (next_game_switcher == 1) {
        transition();
        tetrisInit();
        while (tetrisLoop()) {};
        transition();
        snakeEnd();
    }
    else if (next_game_switcher == 2) {
        transition();
        snakeInit();
        while (snakeLoop()) {};
        transition();
        snakeEnd();
    }
    return;
}



void lcd_clearLine(char line) {
    if (line <= 1) {lcd.setCursor(0, line);}
    else {lcd.setCursor(-4, line);}
    for (char i = 0; i < 16; ++i) {lcd.write(' ');}
  
    if (line <= 1) {lcd.setCursor(0, line);}
    else {lcd.setCursor(-4, line);}
    return;
}

void esp01_printWithLcd(String command) {
    lcd_clearLine(1);
    lcd.print(command);
    esp01.println(command);
    return;
}

bool esp01_read(short timeout, String good_str, String bad_str) {
    while (timeout >= 0) {
        while (esp01.available()) {
            String cur = esp01.readString();
            // Serial.println("###" + cur + "^^^");
            lcd_clearLine(2);
            for (int i = 0; i < cur.length(); ++i) {
                if (cur[i] == '\r') { continue;}
                if (cur[i] == '\n' && i < cur.length() - 3) {
                    lcd_clearLine(2);
                    continue;
                }
                lcd.print(cur[i]);
            }
            if (good_str && cur.indexOf(good_str) >= 0) {return true;}
            if (bad_str && cur.indexOf(bad_str) >= 0) {return false;}
        }
        delay(100);
        timeout -= 100;
    }
  
    //DEBUG: Serial.println("TIMEOUT REACHED!");
    return false;
}

void transition() {
    for (char i = 0; i < 16; ++i) {
        lcd.setCursor(i, 0);
        lcd.write(255);
        lcd.setCursor(i, 1);
        lcd.write(255);
        lcd.setCursor(-4 + i, 2);
        lcd.write(255);
        lcd.setCursor(-4 + i, 3);
        lcd.write(255);
        delay(15 - i * i / 15);
    }
    delay(150);
    for (char i = 0; i < 16; ++i) {
        lcd.setCursor(i, 0);
        lcd.write(' ');
        lcd.setCursor(i, 1);
        lcd.write(' ');
        lcd.setCursor(-4 + i, 2);
        lcd.write(' ');
        lcd.setCursor(-4 + i, 3);
        lcd.write(' ');
        delay(i * i / 15);
    }
    return;
}

bool any_StupidBitBools (char col, char row) {
    return left.getState(col, row) || right.getState(col, row) || up.getState(col, row) || down.getState(col, row);
}

bool timeInit() {
    /*time_local_last_sync = millis();
    time_local = 1684832380;
      return true;*/
  
    lcd.clear();

    esp01.begin(9600);

    digitalWrite(8, HIGH);
    if (!esp01_read(1000, "ready", "ERROR")) {return false;}

    lcd.setCursor(0, 0);
    lcd.print("!! TIME SYNC !!");
    lcd.setCursor(-4, 3);
    lcd.print("!! TIME SYNC !!");
  
    esp01_printWithLcd("AT");
    if (!esp01_read(700, "OK", "ERROR")) {return false;}
  
    esp01_printWithLcd("AT+CWMODE=3");
    if (!esp01_read(3000, "OK", "ERROR")) {return false;}

    esp01_printWithLcd("ATE0");
    if (!esp01_read(2000, "OK", "ERROR")) {return false;}
  
    lcd_clearLine(1);
    lcd.print("AT+CWJAP_CUR=\"s10038_net\",\"\"");
    esp01.println("AT+CWJAP_CUR=\"s10038_net\",\"3270482659\"");
    if (!esp01_read(32000, "OK", "ERROR")) {return false;}

    esp01_printWithLcd("ATE1");
    if (!esp01_read(2000, "OK", "ERROR")) {return false;}
  
    esp01_printWithLcd("AT+CIPMUX=1");
    if (!esp01_read(1000, "OK", "ERROR")) {return false;}

    esp01_printWithLcd("AT+CIPSTART=1,\"TCP\",\"google.com\",80");
    if (!esp01_read(20000, "OK", "ERROR")) {return false;}
  
    esp01_printWithLcd("AT+CIPSEND=1,36");
    if (!esp01_read(1000, ">", "ERROR")) {return false;}
  
    // Serial.println("SENDING...");
    esp01_printWithLcd("GET / HTTP/1.1\r\nHost: google.com\r\n\r\n");
 
    short flag = 0;
    String comp_string = "Date: ";
    String date_string;
    short n = 0;
    
    while (true) {
        if (esp01.available()) {
            flag = 0;
            String cur = "";
            char c = esp01.read();
            //DEBUG: Serial.write(c);
            if (n >= 0) {
                if (c == comp_string[n]) {
                    ++n;
                    if (n >= comp_string.length()) n = -1;
                }
                else { n = 0; }
            }
            else if (n < 0 && n > -40) {
                date_string += c;
                --n;
            }
            

            if (n == -40) {
                time_local = 1672531200;
                String date = (String) date_string[5] + date_string[6];
                String month = (String) date_string[8] + date_string[9] + date_string[10];
                String year = (String) date_string[12] + date_string[13] + date_string[14] + date_string[15];
                String hours = (String) date_string[17] + date_string[18];
                String mins = (String) date_string[20] + date_string[21];
                String secs = (String) date_string[23] + date_string[24];

                char month_mul = 0;
                if (month == "Jan") {month_mul = 0;}
                if (month == "Feb") {month_mul = 1;}
                if (month == "Mar") {month_mul = 2;}
                if (month == "Apr") {month_mul = 3;}
                if (month == "May") {month_mul = 4;}
                if (month == "Jun") {month_mul = 5;}
                if (month == "Jul") {month_mul = 6;}
                if (month == "Aug") {month_mul = 7;}
                if (month == "Sep") {month_mul = 8;}
                if (month == "Oct") {month_mul = 9;}
                if (month == "Nov") {month_mul = 10;}
                if (month == "Dec") {month_mul = 11;}

                char month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                for (char i = 0; i < month_mul; ++i) {time_local += (long) month_days[i] * 86400;}
        
                time_local += ((long) 10 * (date[0] - '0') + (date[1] - '0') - 1) * 86400;
                time_local += ((long) 10 * (hours[0] - '0') + (hours[1] - '0')) * 3600;
                time_local += ((int) 10 * (mins[0] - '0') + (mins[1] - '0')) * 60;
                time_local += ((int) 10 * (secs[0] - '0') + (secs[1] - '0'));
                time_local += 2;
                time_local_last_sync = millis();
                break;
            }
        }
        else { ++flag; delay(10); }
        if (flag > 1000) {return false;}
    }

    esp01_printWithLcd("AT+CIPCLOSE=1");
    if (!esp01_read(4000, "OK", "ERROR")) { return false; }
    last_button_push_time = millis();

    return true;
}

bool timeLoop() {
    unsigned long disp_time_new = 0;
    unsigned long time_delta = 0;
    
    if (update_whole_screen)
        { lcd.clear(); update_whole_screen = false; disp_time_prev_int = -1; disp_time_prev_frac = -1; }

    time_delta = (time_end > (millis() - time_local_last_sync) / 1000 + time_local) ? (time_end - ((millis() - time_local_last_sync) / 1000 + time_local)) : 0;
    
    if (screen_time_mode != 4) {
        lcd.createChar(0, rus_d);
        lcd.createChar(1, rus_p);
        lcd.createChar(2, rus_i);
        lcd.createChar(3, rus_k);
        lcd.createChar(4, rus_z);
        lcd.setCursor(2, 1);
        lcd.write(0);
        lcd.write('o');
        lcd.write(' ');
        lcd.write(1);
        lcd.write('p');
        lcd.write(2);
        lcd.write(3);
        lcd.write('a');
        lcd.write(4);
        lcd.write('a');
        lcd.write(':');
    }

    // days
    if (screen_time_mode == 0) { 
        lcd.createChar(5, rus_D);
        lcd.createChar(6, rus_n);
        lcd.createChar(7, rus_j);
        lcd.setCursor(5, 0);
        lcd.write(5);
        lcd.write(6);
        lcd.write('e');
        lcd.write(7);
      
        disp_time_new = time_delta * 100 / 86400;
    }
    
    // hours
    if (screen_time_mode == 1) {
        lcd.createChar(5, rus_CH);
        lcd.createChar(6, rus_v);
        lcd.setCursor(5, 0);
        lcd.write(5);
        lcd.write('a');
        lcd.write('c');
        lcd.write('o');
        lcd.write(6);
      
        disp_time_new = time_delta * 100  / 3600;
    }
    
    // minutes
    if (screen_time_mode == 2) {
        lcd.createChar(5, rus_M);
        lcd.createChar(6, rus_n);
        lcd.createChar(7, rus_t);
        lcd.setCursor(5, 0);
        lcd.write(5);
        lcd.write(2);
        lcd.write(6);
        lcd.write('y');
        lcd.write(7);

        disp_time_new = time_delta * 100  / 60;
    }
    
    // seconds
    if (screen_time_mode == 3) {
        lcd.createChar(5, rus_k);
        lcd.createChar(6, rus_u);
        lcd.createChar(7, rus_n);
        lcd.setCursor(5, 0);
        lcd.write('C');
        lcd.write('e');
        lcd.write(5);
        lcd.write(6);
        lcd.write(7);
        lcd.write(0);

        disp_time_new = time_delta * 100;
    }

    
    if (screen_time_mode != 4 && (disp_time_prev_frac < 0 || disp_time_new != disp_time_prev_int * 100 + disp_time_prev_frac)) {
        disp_time_prev_int = disp_time_new / 100;
        disp_time_prev_frac = disp_time_new % 100;
        lcd.setCursor(-4, 3);
        for (char i = 0; i < 16; ++i) {lcd.print(' ');}
        
        lcd_setCursor_safe((16 - num_get_digits_count(disp_time_prev_int) - (screen_time_mode == 3 ? 0 : 3)) / 2, 3);
        lcd.print(String(disp_time_prev_int));
        if (screen_time_mode != 3) {
            lcd.print('.');
            if (disp_time_prev_frac < 10)
                {lcd.print('0');}
            lcd.print(String((int)disp_time_prev_frac));
        }
    }
    
    
    // summary
    if (screen_time_mode == 4) {
        lcd.createChar(0, rus_D);
        lcd.createChar(1, rus_p);
        lcd.createChar(2, rus_i);
        lcd.createChar(3, rus_k);
        lcd.createChar(4, rus_z);
        lcd.setCursor(3, 0);
        lcd.write(0);
        lcd.write('o');
        lcd.write(' ');
        lcd.write(1);
        lcd.write('p');
        lcd.write(2);
        lcd.write(3);
        lcd.write('a');
        lcd.write(4);
        lcd.write('a');
        lcd.write(':');

        lcd_setCursor_safe(0, 2);
        for (char i = 0; i < 16; ++i) {lcd.print(' ');}
        lcd_setCursor_safe(2, 2);
        unsigned long temp_time = time_delta, ttemp_time;
        
        ttemp_time = temp_time / 86400;
        if (ttemp_time < 10) {lcd.print("0");}
        lcd.print(ttemp_time);
        lcd.print(":");
        temp_time %= 86400;
        ttemp_time = temp_time / 3600;
        if (ttemp_time < 10) {lcd.print("0");}
        lcd.print(ttemp_time);
        lcd.print(":");
        temp_time %= 3600;
        ttemp_time = temp_time / 60;
        if (ttemp_time < 10) {lcd.print("0");}
        lcd.print(ttemp_time);
        lcd.print(":");
        temp_time %= 60;
        if (temp_time < 10) {lcd.print("0");}
        lcd.print(temp_time);
      
        lcd.createChar(5, rus_d);
        lcd.createChar(6, rus_ch);
        lcd.createChar(7, rus_m);
        lcd.setCursor(-1, 3);
        lcd.write(5);
        lcd.print(". ");
        lcd.write(6);
        lcd.print(". ");
        lcd.write(7);
        lcd.print(". ");
        lcd.print("c.");
    }

    char update_state = 0;
    unsigned long time_delay = (1000 - millis() + last_screen_update_time) / 10;
    if (time_delay < 0 || time_delay > 1000) { time_delay = 0; }
    for (char i = 0; i < 10; ++i) {
        if (digitalRead(2) == LOW && !buttons_state[0]) { last_button_push_time = millis(); next_game_switcher = 1; return false; }
        if (digitalRead(3) == LOW && !buttons_state[1]) { last_button_push_time = millis(); next_game_switcher = 2; return false; }
        if (digitalRead(4) == LOW && !buttons_state[2]) { last_button_push_time = millis(); buttons_state[2] = true; update_state = -1; break; }
        if (digitalRead(5) == LOW && !buttons_state[3]) { last_button_push_time = millis(); buttons_state[3] = true; update_state = 1; break; }
        if (digitalRead(2) == HIGH ) {buttons_state[0] = false;}
        if (digitalRead(3) == HIGH ) {buttons_state[1] = false;}
        if (digitalRead(4) == HIGH ) {buttons_state[2] = false;}
        if (digitalRead(5) == HIGH ) {buttons_state[3] = false;}
        delay(time_delay);
    }

    last_screen_update_time = millis();
    
    if (millis() - last_button_push_time > 60000) { update_state = 1; last_button_push_time = millis(); }
    if (update_state) {update_whole_screen = true;}
    
    screen_time_mode += update_state;
    if (screen_time_mode >= 5) {screen_time_mode = 0;}
    if (screen_time_mode < 0) {screen_time_mode = 4;}

    return true;
}

void snakeInit() {
    left = StupidBitBool();
    right = StupidBitBool();
    up = StupidBitBool();
    down = StupidBitBool();
  
    right.setState(1, 0, true);
    right.setState(2, 0, true);
    snake_head_col = 2;
    snake_head_row = 0;
    snake_tail_col = 1;
    snake_tail_row = 0;
    old_snake_tail_row = 0;
    old_snake_tail_col = 0;
  
    game_score = 0;

    randomSeed(time_local + millis());
    do {
        food_col = random(16);
        food_row = random(8);
    } while (any_StupidBitBools(food_col, food_row));

    lcd.createChar(0, snake_up);
    lcd.createChar(1, snake_down);
    lcd.createChar(2, snake_full);
    lcd.createChar(3, food_up);
    lcd.createChar(4, food_down);
    lcd.createChar(5, snake_up_food);
    lcd.createChar(6, snake_down_food);
  
    lcd.setCursor(1, 0);
    lcd.write(0);
    lcd.write(0);

    last_button_push_time = millis();
  
    return;
}

bool snakeLoop() {
    if (old_snake_tail_row >= 0 && old_snake_tail_col >= 0) {
        if (old_snake_tail_row < 4) {lcd.setCursor(old_snake_tail_col, old_snake_tail_row / 2);}
        else {lcd.setCursor(old_snake_tail_col - 4, old_snake_tail_row / 2);}
        lcd.write(' ');
        
        if (old_snake_tail_row % 2 == 0 && any_StupidBitBools(old_snake_tail_col, old_snake_tail_row + 1)) {
            if (old_snake_tail_row < 4) {lcd.setCursor(old_snake_tail_col, old_snake_tail_row / 2);}
            else {lcd.setCursor(old_snake_tail_col - 4, old_snake_tail_row / 2);}
            lcd.write(1);
        }
        if (old_snake_tail_row % 2 == 1 && any_StupidBitBools(old_snake_tail_col, old_snake_tail_row - 1)) {
            if (old_snake_tail_row < 4) {lcd.setCursor(old_snake_tail_col, old_snake_tail_row / 2);}
            else {lcd.setCursor(old_snake_tail_col - 4, old_snake_tail_row / 2);}
            lcd.write(0);
        }
    }
    

    if (snake_head_row < 4) {lcd.setCursor(snake_head_col, snake_head_row / 2);}
    else {lcd.setCursor(snake_head_col - 4, snake_head_row / 2);}
    if (snake_head_row % 2 == 0) {lcd.write(0);}
    else {lcd.write(1);}
    
    if (snake_head_row % 2 == 0 && any_StupidBitBools(snake_head_col, snake_head_row + 1)) {
        if (snake_head_row < 4) {lcd.setCursor(snake_head_col, snake_head_row / 2);}
        else {lcd.setCursor(snake_head_col - 4, snake_head_row / 2);}
        lcd.write(2);
    }
    if (snake_head_row % 2 == 1 && any_StupidBitBools(snake_head_col, snake_head_row - 1)) {
        if (snake_head_row < 4) {lcd.setCursor(snake_head_col, snake_head_row / 2);}
        else {lcd.setCursor(snake_head_col - 4, snake_head_row / 2);}
        lcd.write(2);
    }

    if (food_row < 4) {lcd.setCursor(food_col, food_row / 2);}
    else {lcd.setCursor(food_col - 4, food_row / 2);}
    if (food_row % 2 == 0) {lcd.write(3);}
    else {lcd.write(4);}
        
    if (food_row % 2 == 0 && any_StupidBitBools(food_col, food_row + 1)) {
        if (food_row < 4) {lcd.setCursor(food_col, food_row / 2);}
        else {lcd.setCursor(food_col - 4, food_row / 2);}
        lcd.write(6);
    }
    if (food_row % 2 == 1 && any_StupidBitBools(food_col, food_row - 1)) {
        if (food_row < 4) {lcd.setCursor(food_col, food_row / 2);}
        else {lcd.setCursor(food_col - 4, food_row / 2);}
        lcd.write(5);
    }
    

    char update = 0; // LRUD <---> 1234
    short delay_time = (game_score < 35) ? (float) 1000 / ((float) 0.5 * game_score + 2.5) : 50;
    while (delay_time >= 15) {
        if (digitalRead(2) == LOW && digitalRead(3) == LOW && digitalRead(4) == LOW && digitalRead(5) == LOW) { delay(1000); return false; }
        if (digitalRead(2) == LOW && !buttons_state[0] && !right.getState(snake_head_col, snake_head_row)) { update = 1; buttons_state[0] = true; }
        if (digitalRead(3) == LOW && !buttons_state[1] && !left.getState(snake_head_col, snake_head_row)) { update = 2; buttons_state[1] = true; }
        if (digitalRead(4) == LOW && !buttons_state[2] && !down.getState(snake_head_col, snake_head_row)) { update = 3; buttons_state[2] = true; }
        if (digitalRead(5) == LOW && !buttons_state[3] && !up.getState(snake_head_col, snake_head_row)) { update = 4; buttons_state[3] = true; }

        if (digitalRead(2) == HIGH) {buttons_state[0] = false;}
        if (digitalRead(3) == HIGH) {buttons_state[1] = false;}
        if (digitalRead(4) == HIGH) {buttons_state[2] = false;}
        if (digitalRead(5) == HIGH) {buttons_state[3] = false;}
        delay_time -= 15;
        delay(15);
    }
    if (delay_time > 0) {delay(delay_time);}
    if (update) last_button_push_time = millis();
    if (millis() - last_button_push_time > 60000) return false;
    
    if (update != 0) {
        left.setState(snake_head_col, snake_head_row, false);
        right.setState(snake_head_col, snake_head_row, false);
        up.setState(snake_head_col, snake_head_row, false);
        down.setState(snake_head_col, snake_head_row, false);
    }
    if (update == 1) {left.setState(snake_head_col, snake_head_row, true);}
    if (update == 2) {right.setState(snake_head_col, snake_head_row, true);}
    if (update == 3) {up.setState(snake_head_col, snake_head_row, true);}
    if (update == 4) {down.setState(snake_head_col, snake_head_row, true);}

    char head_state = 0;

    if (left.getState(snake_head_col, snake_head_row)) {
        --snake_head_col;
        if (snake_head_col < 0) {snake_head_col = 15;}
        head_state = 1;
    }
    if (right.getState(snake_head_col, snake_head_row)) {
        ++snake_head_col;
        if (snake_head_col >= 16) {snake_head_col = 0;}
        head_state = 2;
    }
    if (up.getState(snake_head_col, snake_head_row)) {
        --snake_head_row;
        if (snake_head_row < 0) {snake_head_row = 7;}
        head_state = 3;
    }
    if (down.getState(snake_head_col, snake_head_row)) {
        ++snake_head_row;
        if (snake_head_row >= 8) {snake_head_row = 0;}
        head_state = 4;
    }

    old_snake_tail_row = snake_tail_row;
    old_snake_tail_col = snake_tail_col;

    if (snake_head_col != food_col || snake_head_row != food_row) {
        if (left.getState(snake_tail_col, snake_tail_row)) {
            left.setState(snake_tail_col, snake_tail_row, false);
            --snake_tail_col;
            if (snake_tail_col < 0) {snake_tail_col = 15;}
        }
        else if (right.getState(snake_tail_col, snake_tail_row)) {
            right.setState(snake_tail_col, snake_tail_row, false);
            ++snake_tail_col;
            if (snake_tail_col >= 16) {snake_tail_col = 0;}
        }
        else if (up.getState(snake_tail_col, snake_tail_row)) {
            up.setState(snake_tail_col, snake_tail_row, false);
            --snake_tail_row;
            if (snake_tail_row < 0) {snake_tail_row = 7;}
        }
        else if (down.getState(snake_tail_col, snake_tail_row)) {
            down.setState(snake_tail_col, snake_tail_row, false);
            ++snake_tail_row;
            if (snake_tail_row >= 8) {snake_tail_row = 0;}
        }
    }
    else {
        old_snake_tail_row = -1;
        old_snake_tail_col = -1;
        ++game_score;
        do {
            food_col = random(16);
            food_row = random(8);
        } while (any_StupidBitBools(food_col, food_row));
    }

    if (any_StupidBitBools(snake_head_col, snake_head_row)) 
        { delay(1000); return false; }
      
    if (head_state == 0) {}
    if (head_state == 1) {left.setState(snake_head_col, snake_head_row, true);}
    if (head_state == 2) {right.setState(snake_head_col, snake_head_row, true);}
    if (head_state == 3) {up.setState(snake_head_col, snake_head_row, true);}
    if (head_state == 4) {down.setState(snake_head_col, snake_head_row, true);}
    return true;
}

void snakeEnd() {
    lcd.clear();

    int i = random(8);
    if (i == 0) {
        lcd.setCursor(3, 0);
        lcd.print("GAME OVER!");
    }
    else if (i == 1) {
        lcd.createChar(0, rus_P);
        lcd.createChar(1, rus_CH);

        lcd.setCursor(3, 0);
        lcd.write(0); // rus_p
        lcd.print("OTPA");
        lcd.write(1); // rus_ch
        lcd.print("EHO!");
    }
    else if (i == 2) {
        lcd.createChar(0, rus_D);
        lcd.createChar(1, rus_I);
        lcd.createChar(2, rus_G);
        lcd.createChar(3, rus_L);
        lcd.createChar(6, rus_YA);
        
        lcd.setCursor(3, 0);
        lcd.write(0); // rus_D
        lcd.write('O');
        lcd.write(1); //rus_I
        lcd.write(2);
        lcd.write('P');
        lcd.write('A');
        lcd.write(3);
        lcd.write('C');
        lcd.write(6);
        lcd.write('!');
    }
    else if (i == 3) {
        lcd.createChar(0, rus_P);

        lcd.setCursor(5, 0);
        lcd.write(0);
        lcd.print("OMEP!");
    }
    else if (i == 4) {
        lcd.createChar(0, rus_G);

        lcd.setCursor(4, 0);
        lcd.write(0);
        lcd.print("AMOBEP!");
    }
    else if (i == 5) {
        lcd.setCursor(6, 0);
        lcd.print("XAHA!");
    }
    else if (i == 6) {
        lcd.createChar(0, rus_YI);
        
        lcd.setCursor(5, 0);
        lcd.print("KPAHT");
        lcd.write(0);
        lcd.write('!');
    }
    else if (i == 7) {
        lcd.createChar(0, rus_YO);

        lcd.setCursor(6, 0);
        lcd.print("BC");
        lcd.write(0);
        lcd.write('!');
    }


    lcd.createChar(4, rus_sh);
    lcd.createChar(5, rus_ch);
    if (i != 2) { lcd.createChar(6, rus_yo); }
    lcd.createChar(7, rus_t);
    
    lcd.setCursor(0, 2);
    lcd.print("Ba");
    lcd.write(4); // rus_sh
    lcd.print(" c");
    lcd.write(5); // rus_ch
    if (i != 2) { lcd.write(6); } // rus_yo
    else { lcd.write('e'); }
    lcd.write(7); // rus_t
    lcd.write(':');

    lcd_setCursor_safe((16 - num_get_digits_count(game_score)) / 2, 3);

    lcd.print(game_score);

    last_button_push_time = millis();

    while (true) {
        if (digitalRead(2) == LOW || digitalRead(3) == LOW || digitalRead(4) == LOW || digitalRead(5) == LOW) 
            { for (char i = 0; i < 4; ++i) buttons_state[i] = true; return; }
        if (millis() - last_button_push_time > 60000) return;
    }
      
    delay(100);
      
    return;
}

void tetrisInit () {
    tetris_field = StupidBitBool();
    tetris_figure_spes = -1;
    game_score = 0;

    lcd.clear();
    lcd.createChar(0, snake_up);
    lcd.createChar(1, snake_down);
    lcd.createChar(2, snake_full);
    lcd.createChar(3, sieve_up);
    lcd.createChar(4, sieve_down);
    lcd.createChar(5, sieve_full);
    lcd.createChar(6, snake_up_sieve);
    lcd.createChar(7, snake_down_sieve);
    randomSeed(time_local + millis());
    
    return;
}

bool tetrisLoop () {
    tetris_figure_spes = random(7);
    tetris_figure_rotation = 0;

    tetris_figure_col = 0;
    tetris_figure_row = 2;
    tetris_figure_col_prev = -10;
    tetris_figure_row_prev = -10;

    for (char i = 0; i < 4; ++i) {
        for (char j = 0; j < 4; ++j) {
            tetris_figure[i][j] = false;
            tetris_figure_prev[i][j] = false;
        }
    }
        
    if (tetris_figure_spes == 0) {tetris_figure_initialize (tetris_figure, 5, 6, 9, 10);}
    else if (tetris_figure_spes == 1) {tetris_figure_initialize (tetris_figure, 4, 5, 6, 7);}
    else if (tetris_figure_spes == 2) {tetris_figure_initialize (tetris_figure, 0, 4, 5, 9);}
    else if (tetris_figure_spes == 3) {tetris_figure_initialize (tetris_figure, 1, 4, 5, 8);}
    else if (tetris_figure_spes == 4) {tetris_figure_initialize (tetris_figure, 1, 2, 6, 10);}
    else if (tetris_figure_spes == 5) {tetris_figure_initialize (tetris_figure, 0, 1, 4, 8);}
    else if (tetris_figure_spes == 6) {tetris_figure_initialize (tetris_figure, 1, 4, 5, 9);}

    for (char i = 0; i < random(4); ++i) 
      tetris_figure_rotate_counterclockwise();


    bool tetris_figure_falling = true;
    if (tetris_figure_check_collision()) 
        { delay(1000); return false; }
    char figure_update = 0; // CW CCW U D
    short delay_time = 600;
    
    while (tetris_figure_falling) {
    draw_tetris_figure:
      
        if (tetris_figure_row_prev >= -10 && tetris_figure_col_prev >= -10) {
            for (char i = 0; i < 4; ++i) {
                for (char j = 0; j < 4; ++j) {
                    if (tetris_figure_prev[i][j]) {
                        lcd_setCursor_safe_div2(tetris_figure_col_prev + j, tetris_figure_row_prev + i);
                        if ((tetris_figure_row_prev + i) % 2 == 0) {
                            if (tetris_figure_row_prev + i + 1 < 16 && tetris_field.getState(tetris_figure_col_prev + j, tetris_figure_row_prev + i + 1)) 
                                { lcd.write(4); }
                            else 
                                { lcd.write(' '); }
                        }
                        else {
                            if (tetris_figure_row_prev + i - 1 >= 0 && tetris_field.getState(tetris_figure_col_prev + j, tetris_figure_row_prev + i - 1)) 
                                { lcd.write(3); }
                            else 
                                { lcd.write(' '); }
                        }
                    }
                }
            }
        }


        for (char i = 0; i < 4; ++i) {
            for (char j = 0; j < 4; ++j) {
                if (tetris_figure[i][j]) {
                    lcd_setCursor_safe_div2(tetris_figure_col + j, tetris_figure_row + i);
                    if ((tetris_figure_row + i) % 2 == 0) {
                        if (i < 3 && tetris_figure[i+1][j]) { /* just do nothing! */ }
                            
                        else if (tetris_figure_row + i + 1 < 16 && tetris_field.getState(tetris_figure_col + j, tetris_figure_row + i + 1)) 
                            {lcd.write(6);}
                        else 
                            {lcd.write(0);}
                    }
                    else {
                        if (i > 0 && tetris_figure[i-1][j]) 
                            {lcd.write(2);}
                        else if (tetris_figure_row + i - 1 >= 0 && tetris_field.getState(tetris_figure_col + j, tetris_figure_row + i - 1)) 
                            {lcd.write(7);}
                        else 
                            {lcd.write(1);}
                    }
                }
            }
        }
      

        tetris_figure_row_prev = tetris_figure_row;
        tetris_figure_col_prev = tetris_figure_col;
        for (char i = 0; i < 4; ++i) {
            for (char j = 0; j < 4; ++j) {
                tetris_figure_prev[i][j] = tetris_figure[i][j];
            }
        }

        while (delay_time >= 50) {
            delay_time -= 50;
            delay(50);
            figure_update = 0;
            if (digitalRead(2) == LOW && digitalRead(3) == LOW && digitalRead(4) == LOW && digitalRead(5) == LOW) {return false;}
            if (digitalRead(2) == LOW && !buttons_state[0]) { buttons_state[0] = true; figure_update |= (1 << 3); }
            if (digitalRead(3) == LOW /*&& !buttons_state[1]*/) { buttons_state[1] = true; figure_update |= (1 << 2); }
            if (digitalRead(4) == LOW /*&& !buttons_state[2]*/) { buttons_state[2] = true; figure_update |= (1 << 1); }
            if (digitalRead(5) == LOW /*&& !buttons_state[3]*/) { buttons_state[3] = true; figure_update |= (1 << 0); }

            if (digitalRead(2) == HIGH) {buttons_state[0] = false;}
            if (digitalRead(3) == HIGH) {buttons_state[1] = false;}
            if (digitalRead(4) == HIGH) {buttons_state[2] = false;}
            if (digitalRead(5) == HIGH) {buttons_state[3] = false;}
          
            if (figure_update == 0) {continue;}

            if (figure_update & (1 << 3))  // rotate CCW
                {tetris_figure_rotate_counterclockwise ();}

            if (figure_update & (1 << 2))  // move right
                { ++tetris_figure_col; }
          
            if (figure_update & (1 << 1))  // move up
                {--tetris_figure_row;}
          
            if (figure_update & (1 << 0))  // move down
                {++tetris_figure_row;}

            if (tetris_figure_check_collision()) {
                if (figure_update & (1 << 3))
                    {tetris_figure_rotate_clockwise();}

                if (figure_update & (1 << 2))
                    { --tetris_figure_col; }
          
                if (figure_update & (1 << 1))
                    {++tetris_figure_row;}
              
                if (figure_update & (1 << 0))
                    {--tetris_figure_row;}
            }
            else 
                {goto draw_tetris_figure;}
          
          
        }
        if (delay_time > 0) {delay(delay_time);}

        if (tetris_figure_check_laid()) {break;}

        ++tetris_figure_col;
        if (tetris_figure_check_collision())
            { --tetris_figure_col; break; }
      
        //++game_score;
        delay_time = (game_score < 30) ? (float) 1000 / ((float) 0.29 * game_score + 1.25) : 100;
    }


    for (char i = 0; i < 4; ++i) {
        for (char j = 0; j < 4; ++j) {
            if (tetris_figure[i][j]) {
                tetris_field.setState(tetris_figure_col + j, tetris_figure_row + i, true);
                lcd_setCursor_safe_div2(tetris_figure_col + j, tetris_figure_row + i);
                if ((tetris_figure_row + i) % 2 == 0) {
                    if (tetris_figure_row + i + 1 < 16 && tetris_field.getState(tetris_figure_col + j, tetris_figure_row + i + 1)) 
                        {lcd.write(5);}
                    else 
                        {lcd.write(3);}
                }
                else {
                    if (tetris_figure_row + i - 1 >= 0 && tetris_field.getState(tetris_figure_col + j, tetris_figure_row + i - 1)) 
                        {lcd.write(5);}
                    else 
                        {lcd.write(4);}
                }
            }
        }
    }
    

    bool full_cols[16];
    for (char i = 0; i < 16; ++i) {full_cols[i] = false;}
    
    for (char i = 0; i < 16; ++i) {
        bool flag = true;
        for (char j = 0; j < 8; ++j) {
            if (!tetris_field.getState(i, j)) {flag = false;}
        }
        if (flag) {full_cols[i] = true;}
    }

    char mul = 0;
    bool flag = false;
    for (char i = 0; i < 16; ++i) {
        if (full_cols[i]) { 
            flag = true; 
            ++mul;
        }
    }

    if (!flag) {return true;}

    if (mul == 1) {game_score += 1;}
    else if (mul == 2) {game_score += 3;}
    else if (mul == 3) {game_score += 7;}
    else if (mul >= 4) {game_score += 15;}
            
    for (char i = 3; i > -1; --i) {
        for (char j = 0; j < 16; ++j) {
            if (full_cols[j]) {
                tetris_field.setState(j, i, false);
                tetris_field.setState(j, 7-i, false);
                lcd_setCursor_safe_div2(j, i);
                if (i % 2 == 0) {lcd.write(' ');}
                else {lcd.write(3);}
                lcd_setCursor_safe_div2(j, 7-i);
                if ((7-i) % 2 == 0) {lcd.write(4);}
                else {lcd.write(' ');}
            }
        }
        delay(80);
    }
    for (char q = 0; q < 16; ++q) {
        if (full_cols[q]) {
            for (char i = q; i > 0; --i) {
                for (char j = 0; j < 8; ++j) 
                    {tetris_field.setState(i, j, tetris_field.getState(i-1, j));}
                for (char j = 0; j < 8; ++j) 
                    {tetris_field.setState(0, j, false);}
                for (char j = 1; j < 8; j += 2) {
                    lcd_setCursor_safe_div2(i, j);
                    if (tetris_field.getState(i, j)) {
                        if (tetris_field.getState(i, j-1)) 
                            {lcd.write(5);}
                        else 
                            {lcd.write(4);}
                    }
                    else {
                        if (tetris_field.getState(i, j-1)) 
                            {lcd.write(3);}
                        else 
                            {lcd.write(' ');}
                    }
                }
            }
            delay(50);
        }
    }
    
    return true;
}
