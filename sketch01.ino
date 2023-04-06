#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x38, 16, 4);  // set the LCD address to 0x38 for a 16 chars and 4 line display
SoftwareSerial esp01(7, 6);
unsigned long time = 1672542000;
unsigned long time_local_last_sync;
unsigned long time_delta;
unsigned long time_end = 1684832400;

bool update_whole_screen = true;
unsigned long prev_screen_update;

uint8_t rus_v[] = {0x0, 0x0, 0x1c, 0x12, 0x1c, 0x12, 0x1c, 0x0};
uint8_t rus_D[] = {0x6, 0xa, 0xa, 0xa, 0xa, 0xa, 0x1f, 0x11};
uint8_t rus_d[] = {0x0, 0x0, 0x6, 0xa, 0xa, 0xa, 0x1f, 0x11};
uint8_t rus_yo[] = {0xa, 0x0, 0xe, 0x11, 0x1f, 0x10, 0xe};
uint8_t rus_z[] = {0x0, 0x0, 0x1e, 0x2, 0xc, 0x2, 0x1c, 0x0};
uint8_t rus_i[] = {0x0, 0x0, 0x12, 0x12, 0x16, 0x1a, 0x12, 0x0};
uint8_t rus_j[] = {0xc, 0x0, 0x12, 0x12, 0x16, 0x1a, 0x12, 0x0};
uint8_t rus_k[] = {0x0, 0x0, 0x12, 0x14, 0x18, 0x14, 0x12, 0x0};
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

uint8_t snake_up[] = {0x1f, 0x1f, 0x1f, 0x1f, 0x0, 0x0, 0x0, 0x0};
uint8_t snake_down[] = {0x0, 0x0, 0x0, 0x0, 0x1f, 0x1f, 0x1f, 0x1f};
uint8_t snake_full[] = {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
uint8_t food_up[] = {0x0, 0x4, 0xa, 0x4, 0x0, 0x0, 0x0, 0x0};
uint8_t food_down[] = {0x0, 0x0, 0x0, 0x0, 0x4, 0xa, 0x4, 0x0};
uint8_t snake_up_food[] = {0x1f, 0x1f, 0x1f, 0x1f, 0x4, 0xa, 0x4, 0x0};
uint8_t snake_down_food[] = {0x0, 0x4, 0xa, 0x4, 0x1f, 0x1f, 0x1f, 0x1f};

char screen_state = 4;

bool buttons_state[] = {false, false, false, false};

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
  }
};

StupidBitBool left, right, up, down;
char snake_tail_col, snake_tail_row, snake_head_col, snake_head_row, food_col, food_row, snake_score;


void setup()
{  
  pinMode(2, INPUT); // button_left
  pinMode(3, INPUT); // button_right
  pinMode(4, INPUT); // button_up
  pinMode(5, INPUT); // button_down
  pinMode(8, OUTPUT); // esp power
  digitalWrite(8, LOW);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();

  while (!timeInit()) 
    digitalWrite(8, LOW);
  digitalWrite(8, LOW);
  
  Serial.begin(9600);
}

void loop() {
  transition();
  update_whole_screen = true;
  timeLoop();
  transition();
  snakeInit();
  snakeLoop();
  transition();
  snakeEnd();
}

void lcd_clearLine(char line) {
  if (line <= 1) lcd.setCursor(0, line);
  else lcd.setCursor(-4, line);
  for (char i = 0; i < 16; ++i) lcd.write(' ');
  if (line <= 1) lcd.setCursor(0, line);
  else lcd.setCursor(-4, line);
}

void esp01_printWithLcd(String command) {
  //lcd_clearLine(2);
  lcd_clearLine(1);
  lcd.print(command);
  esp01.println(command);
}

bool esp01_read(short timeout, String good_str, String bad_str) {
  while (timeout >= 0) {
    while (esp01.available()) {
      String cur = esp01.readString();
      // Serial.println("###" + cur + "^^^");
      lcd_clearLine(2);
      for (int i = 0; i < cur.length(); ++i) {
        if (cur[i] == '\r') continue;
        if (cur[i] == '\n' && i < cur.length() - 3) {
          lcd_clearLine(2);
          continue;
        }
        lcd.print(cur[i]);
      }
      if (good_str && cur.indexOf(good_str) >= 0) return true;
      if (bad_str && cur.indexOf(bad_str) >= 0) return false;
    }
    delay(100);
    timeout -= 100;
  }
  
  Serial.println("TIMEOUT REACHED!");
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
}

bool timeInit() {
  //time_local_last_sync = millis();
  //return;
  
  lcd.clear();
  
  esp01.begin(9600);

  digitalWrite(8, HIGH);
  if (!esp01_read(1000, "ready", "ERROR")) return false;

  lcd.setCursor(0, 0);
  lcd.print("!! TIME SYNC !!");
  lcd.setCursor(-4, 3);
  lcd.print("!! TIME SYNC !!");
  
  esp01_printWithLcd("AT");
  if (!esp01_read(700, "OK", "ERROR")) return false;
  
  esp01_printWithLcd("AT+CWMODE=3");
  if (!esp01_read(3000, "OK", "ERROR")) return false;

  esp01_printWithLcd("ATE0");
  if (!esp01_read(2000, "OK", "ERROR")) return false;
  
  lcd_clearLine(1);
  lcd.print("AT+CWJAP_CUR=\"s10038_net\",\"\"");
  esp01.println("AT+CWJAP_CUR=\"s10038_net\",\"3270482659\"");
  if (!esp01_read(20000, "OK", "ERROR")) return false;

  esp01_printWithLcd("ATE1");
  if (!esp01_read(2000, "OK", "ERROR")) return false;
  
  esp01_printWithLcd("AT+CIPMUX=1");
  if (!esp01_read(1000, "OK", "ERROR")) return false;

  esp01_printWithLcd("AT+CIPSTART=1,\"TCP\",\"vml35.ru\",80");
  if (!esp01_read(10000, "OK", "ERROR")) return false;
  
  esp01_printWithLcd("AT+CIPSEND=1,34");
  if (!esp01_read(1000, ">", "ERROR")) return false;
  
  // Serial.println("SENDING...");
  esp01_printWithLcd("GET / HTTP/1.1\r\nHost: vml35.ru\r\n\r\n");
 
  short flag = 0;
  int n = -1;
  while (true) {
    if (esp01.available()) {
      flag = 0;
      String cur = esp01.readString();
      int n = cur.indexOf("Date:");
      
      Serial.println("###" + cur + "^^^");

      if (n >= 0) {
        time = 1672531200;
        String date = (String) cur[n+11] + cur[n+12];
        String month = (String) cur[n+14] + cur[n+15] + cur[n+16];
        String year = (String) cur[n+18] + cur[n+19] + cur[n+20] + cur[n+21];
        String hours = (String) cur[n+23] + cur[n+24];
        String mins = (String) cur[n+26] + cur[n+27];
        String secs = (String) cur[n+29] + cur[n+30];

        //time += (year - 2023) * 
        char month_mul = 0;
        if (month == "Jan") month_mul = 0;
        if (month == "Feb") month_mul = 1;
        if (month == "Mar") month_mul = 2;
        if (month == "Apr") month_mul = 3;
        if (month == "May") month_mul = 4;
        if (month == "Jun") month_mul = 5;
        if (month == "Jul") month_mul = 6;
        if (month == "Aug") month_mul = 7;
        if (month == "Sep") month_mul = 8;
        if (month == "Oct") month_mul = 9;
        if (month == "Nov") month_mul = 10;
        if (month == "Dec") month_mul = 11;

        char month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        for (char i = 0; i < month_mul; ++i) time += (long) month_days[i] * 86400;
        
        time += ((long) 10 * (date[0] - '0') + (date[1] - '0') - 1) * 86400;
        time += ((long) 10 * (hours[0] - '0') + (hours[1] - '0')) * 3600;
        time += ((int) 10 * (mins[0] - '0') + (mins[1] - '0')) * 60;
        time += ((int) 10 * (secs[0] - '0') + (secs[1] - '0'));
        time += 3;
        time_local_last_sync = millis();
        break;
      }
    }
    else { ++flag; delay(10); }
    if (flag > 200) return false;
  }

  esp01_printWithLcd("AT+CIPCLOSE=1");
  if (!esp01_read(4000, "OK", "ERROR")) return false;

  return true;
}

void timeLoop() {
  while (true) {
    if (update_whole_screen)
      { lcd.clear(); update_whole_screen = false; }
    else {
      if (screen_state == 4)
        lcd.setCursor(-4, 2);
      else
        lcd.setCursor(-4, 3);
      for (char i = 0; i < 16; ++i) lcd.print(' ');
    }

    time_delta = time_end - ((millis() - time_local_last_sync) / 1000 + time);
  
    if (screen_state != 4) {
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
    if (screen_state == 0) { 
      lcd.createChar(5, rus_D);
      lcd.createChar(6, rus_n);
      lcd.createChar(7, rus_j);
      lcd.setCursor(5, 0);
      lcd.write(5);
      lcd.write(6);
      lcd.write('e');
      lcd.write(7);
      lcd.setCursor(0, 3);
      lcd.print(time_delta / 86400);
    }
    
    // hours
    if (screen_state == 1) {
      lcd.createChar(5, rus_CH);
      lcd.createChar(6, rus_v);
      lcd.setCursor(5, 0);
      lcd.write(5);
      lcd.write('a');
      lcd.write('c');
      lcd.write('o');
      lcd.write(6);
      lcd.setCursor(0, 3);
      lcd.print(time_delta / 3600);
    }
    
    // minutes
    if (screen_state == 2) {
      lcd.createChar(5, rus_M);
      lcd.createChar(6, rus_n);
      lcd.createChar(7, rus_t);
      lcd.setCursor(5, 0);
      lcd.write(5);
      lcd.write(2);
      lcd.write(6);
      lcd.write('y');
      lcd.write(7);
      lcd.setCursor(0, 3);
      lcd.print(time_delta / 60);
    }
    
    // seconds
    if (screen_state == 3) {
      lcd.createChar(5, rus_k);
      lcd.createChar(6, rus_u);
      lcd.createChar(7, rus_n);
      lcd.setCursor(4, 0);
      lcd.write('C');
      lcd.write('e');
      lcd.write(5);
      lcd.write(6);
      lcd.write(7);
      lcd.write(0);
      lcd.setCursor(0, 3);
      lcd.print(time_delta);
    }
    
    // summary
    if (screen_state == 4) {
      lcd.createChar(0, rus_D);
      lcd.createChar(1, rus_p);
      lcd.createChar(2, rus_i);
      lcd.createChar(3, rus_k);
      lcd.createChar(4, rus_z);
      lcd.setCursor(2, 0);
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
  
      lcd.setCursor(-2, 2);
      unsigned long temp_time = time_delta, ttemp_time;
      ttemp_time = temp_time / 86400;
      if (temp_time < 10) lcd.print("0");
      lcd.print(ttemp_time);
      lcd.print(":");
      temp_time %= 86400;
      ttemp_time = temp_time / 3600;
      if (ttemp_time < 10) lcd.print("0");
      lcd.print(ttemp_time);
      lcd.print(":");
      temp_time %= 3600;
      ttemp_time = temp_time / 60;
      if (ttemp_time < 10) lcd.print("0");
      lcd.print(ttemp_time);
      lcd.print(":");
      temp_time %= 60;
      if (temp_time < 10) lcd.print("0");
      lcd.print(temp_time);
      
      lcd.createChar(5, rus_d);
      lcd.createChar(6, rus_ch);
      lcd.createChar(7, rus_m);
      lcd.setCursor(-2, 3);
      lcd.write(5);
      lcd.print(". ");
      lcd.write(6);
      lcd.print(". ");
      lcd.write(7);
      lcd.print(". ");
      lcd.print("c.");
    }

    char update_state = 0;
    unsigned long time_delay = (1000 - millis() + prev_screen_update) / 10;
    if (time_delay < 0 || time_delay > 1000) time_delay = 0;
    for (char i = 0; i < 10; ++i) {
      if (digitalRead(3) == LOW && !buttons_state[1]) return;
      if (digitalRead(4) == LOW && !buttons_state[2]) { buttons_state[2] = true; update_state = -1; break; }
      if (digitalRead(5) == LOW && !buttons_state[3]) { buttons_state[3] = true; update_state = 1; break; }
      if (digitalRead(2) == HIGH ) buttons_state[0] = false;
      if (digitalRead(3) == HIGH ) buttons_state[1] = false;
      if (digitalRead(4) == HIGH ) buttons_state[2] = false;
      if (digitalRead(5) == HIGH ) buttons_state[3] = false;
      delay(time_delay);
    }

    prev_screen_update = millis();
   
    if (update_state) update_whole_screen = true;
    
    screen_state += update_state;
    if (screen_state >= 5) screen_state = 0;
    if (screen_state < 0) screen_state = 4;
  }
}

void snakeInit() {
  left = StupidBitBool();
  right = StupidBitBool();
  up = StupidBitBool();
  down = StupidBitBool();
  
  left.setState(0, 0, true);
  left.setState(1, 0, true);
  left.setState(2, 0, true);
  snake_tail_col = 2;
  snake_tail_row = 0;
  snake_head_row = 0;
  snake_head_col = 0;
  snake_score = 0;

  randomSeed(time + millis());
  do {
      food_col = random(16);
      food_row = random(8);
  } while (left.getState(food_col, food_row) || right.getState(food_col, food_row) || up.getState(food_col, food_row) || down.getState(food_col, food_row));

  lcd.createChar(0, snake_up);
  lcd.createChar(1, snake_down);
  lcd.createChar(2, snake_full);
  lcd.createChar(3, food_up);
  lcd.createChar(4, food_down);
  lcd.createChar(5, snake_up_food);
  lcd.createChar(6, snake_down_food);
  return;
}

void snakeLoop() {
  while (true) {
    lcd.clear();

    if (food_row < 4) lcd.setCursor(food_col, food_row / 2);
    else lcd.setCursor(food_col-4, food_row / 2);
    if (food_row % 2 == 0) lcd.write(3);
    else lcd.write(4);

    bool prev = false;
    for (char col = 0; col < 16; ++col) 
    for (char row = 0; row < 8; ++row) {
      if (left.getState(col, row) || right.getState(col, row) || up.getState(col, row) || down.getState(col, row)) {
        if (row < 4) lcd.setCursor(col, row / 2);
        else lcd.setCursor(col-4, row / 2);
        if (prev) {
          lcd.write(2);
        }
        else {
          if (row % 2 == 0) lcd.write(0);
          else lcd.write(1);
        }
        if (row % 2 == 0) prev = true;
        else prev = false;
      }
      else {
        prev = false;
      }
    }
    if (food_row % 2 == 0 && 
    (left.getState(food_col, food_row+1) || right.getState(food_col, food_row+1) || up.getState(food_col, food_row+1) || down.getState(food_col, food_row+1))) {
      if (food_row < 4) lcd.setCursor(food_col, food_row / 2);
      else lcd.setCursor(food_col-4, food_row / 2);
      lcd.write(6);
    }
    else if (food_row % 2 != 0 && 
    (left.getState(food_col, food_row-1) || right.getState(food_col, food_row-1) || up.getState(food_col, food_row-1) || down.getState(food_col, food_row-1))) {
      if (food_row < 4) lcd.setCursor(food_col, food_row / 2);
      else lcd.setCursor(food_col-4, food_row / 2);
      lcd.write(5);
    }


    char update = 0; // LRUD <---> 1234
    for (char i = 0; i < 4; ++i) {
        if (digitalRead(2) == LOW && digitalRead(3) == LOW && digitalRead(4) == LOW && digitalRead(5) == LOW) { delay(1000); return; }
        if (digitalRead(2) == LOW && !buttons_state[0] && !right.getState(snake_head_col, snake_head_row)) { update = 1; buttons_state[0] = true; }
        if (digitalRead(3) == LOW && !buttons_state[1] && !left.getState(snake_head_col, snake_head_row)) { update = 2; buttons_state[1] = true; }
        if (digitalRead(4) == LOW && !buttons_state[2] && !down.getState(snake_head_col, snake_head_row)) { update = 3; buttons_state[2] = true; }
        if (digitalRead(5) == LOW && !buttons_state[3] && !up.getState(snake_head_col, snake_head_row)) { update = 4; buttons_state[3] = true; }

        if (digitalRead(2) == HIGH) buttons_state[0] = false;
        if (digitalRead(3) == HIGH) buttons_state[1] = false;
        if (digitalRead(4) == HIGH) buttons_state[2] = false;
        if (digitalRead(5) == HIGH) buttons_state[3] = false;
        delay(max(100 - 5 * snake_score, 3));

    }
    if (update != 0) {
      left.setState(snake_head_col, snake_head_row, false);
      right.setState(snake_head_col, snake_head_row, false);
      up.setState(snake_head_col, snake_head_row, false);
      down.setState(snake_head_col, snake_head_row, false);
    }
    if (update == 1) left.setState(snake_head_col, snake_head_row, true);
    if (update == 2) right.setState(snake_head_col, snake_head_row, true);
    if (update == 3) up.setState(snake_head_col, snake_head_row, true);
    if (update == 4) down.setState(snake_head_col, snake_head_row, true);

    char head_state = 0;

    if (left.getState(snake_head_col, snake_head_row)) {
        --snake_head_col;
        if (snake_head_col < 0) snake_head_col = 15;
        head_state = 1;
    }
    if (right.getState(snake_head_col, snake_head_row)) {
        ++snake_head_col;
        if (snake_head_col >= 16) snake_head_col = 0;
        head_state = 2;
    }
    if (up.getState(snake_head_col, snake_head_row)) {
        --snake_head_row;
        if (snake_head_row < 0) snake_head_row = 7;
        head_state = 3;
    }
    if (down.getState(snake_head_col, snake_head_row)) {
        ++snake_head_row;
        if (snake_head_row >= 8) snake_head_row = 0;
        head_state = 4;
    }

    if (snake_head_col != food_col || snake_head_row != food_row) {
      if (left.getState(snake_tail_col, snake_tail_row)) {
          left.setState(snake_tail_col, snake_tail_row, false);
          --snake_tail_col;
          if (snake_tail_col < 0) snake_tail_col = 15;
      }
      else if (right.getState(snake_tail_col, snake_tail_row)) {
          right.setState(snake_tail_col, snake_tail_row, false);
          ++snake_tail_col;
          if (snake_tail_col >= 16) snake_tail_col = 0;
      }
      else if (up.getState(snake_tail_col, snake_tail_row)) {
          up.setState(snake_tail_col, snake_tail_row, false);
          --snake_tail_row;
          if (snake_tail_row < 0) snake_tail_row = 7;
      }
      else if (down.getState(snake_tail_col, snake_tail_row)) {
          down.setState(snake_tail_col, snake_tail_row, false);
          ++snake_tail_row;
          if (snake_tail_row >= 8) snake_tail_row = 0;
      }
    }
    else {
      ++snake_score;
      do {
        food_col = random(16);
        food_row = random(8);
      } while (left.getState(food_col, food_row) || right.getState(food_col, food_row) || up.getState(food_col, food_row) || down.getState(food_col, food_row));
    }

    if (left.getState(snake_head_col, snake_head_row) || right.getState(snake_head_col, snake_head_row) || 
    up.getState(snake_head_col, snake_head_row) || down.getState(snake_head_col, snake_head_row)) 
      { delay(1000); return; }
      
    if (head_state == 0) {false;}
    if (head_state == 1) left.setState(snake_head_col, snake_head_row, true);
    if (head_state == 2) right.setState(snake_head_col, snake_head_row, true);
    if (head_state == 3) up.setState(snake_head_col, snake_head_row, true);
    if (head_state == 4) down.setState(snake_head_col, snake_head_row, true);
  }
}

void snakeEnd() {
      lcd.clear();

      lcd.createChar(0, rus_P);
      lcd.createChar(1, rus_CH);
      lcd.createChar(2, rus_T);
      lcd.createChar(3, rus_sh);
      lcd.createChar(4, rus_ch);
      lcd.createChar(5, rus_yo);
      lcd.createChar(6, rus_t);

      lcd.setCursor(3, 0);
      /*lcd.write(0); // rus_p
      lcd.write('O');
      lcd.write(2); // rus_t
      lcd.print("PA");
      lcd.write(1); // rus_ch
      lcd.print("EHO!");*/
      lcd.print("GAME OVER!");

      lcd.setCursor(0, 2);
      lcd.print("Ba");
      lcd.write(3); // rus_sh
      lcd.print(" c");
      lcd.write(4); // rus_ch
      lcd.write(5); // rus_yo
      lcd.write(6); // rus_t
      lcd.write(':');

      lcd.setCursor(3, 3);

      lcd.print(String((int) snake_score));

      while (true) {
          if (digitalRead(2) == LOW || digitalRead(3) == LOW || digitalRead(4) == LOW || digitalRead(5) == LOW) 
              { for (char i = 0; i < 4; ++i) buttons_state[i] = true; return; }
          }
          delay(100);
      }


 
