// C++ code в char

#define SOLAR_PIN A0
bool Is_got_data = false;
bool Is_start_of_transieve = false;
bool Is_end_of_transieve = false;
bool Is_start_idle = false;

const unsigned long interval = 40*1000; // Интервал в миллисекундах

unsigned long time_till_start_idle = 0;

unsigned long time_till_last_iteraton = micros(); 

static unsigned long time_till_last_char = 0;
const unsigned long interval_of_ans = 1000; // Интервал в миллисекундах

int solar_panel_porogovoe_znach = 800;

void setup() {
  Serial.begin(9600); // Инициализация Serial порта
  pinMode(SOLAR_PIN, INPUT);
  delay(10);

}
int solar_panel = 0;


void loope() { //main
  static String input; // Переменная для хранения ввода
  static String answer = "";
  char char_data = '1';
  solar_panel = analogRead(SOLAR_PIN);
  //Serial.println(solar_panel);
  if (solar_panel > solar_panel_porogovoe_znach) {
    char_data = '1';
  }
  else {
    char_data = '0';
    time_till_last_char = millis();
  }
  if (char_data == '0' && (!Is_got_data)) {
    Is_got_data = true;
    answer = "";
    
  }
  else {
    if ((Is_got_data) && ((millis() - time_till_last_char) > interval_of_ans)) {
      Serial.println();
      input = "";
      answer = "";
      Is_got_data = false;
      Is_end_of_transieve = true;

    }
  }
  if (Is_got_data) {
    input += char_data; // Добавление символа к строке ввода
    //Serial.print(char_data);
    if (input.length() % 11 == 0) { // Проверка, что строка содержит целое количество байт
      if (char_data == '1') { // Проверка на завершение байта
        char binary_data[10]; // Создание массива для хранения двоичных данных (8 бит + завершающий нулевой символ)
        for (int i = 0; i < 9; i++) { // Инициализация массива binary_data
          binary_data[i] = input[i + 1];
        }
        int counter_of_1 = 0;
        for (int i = 0; i < 9; i++) {
          if (binary_data[i] == '1') {
            counter_of_1++;
          }
        }
        binary_data[9] = '\0'; // Установка завершающего нулевого символа

        char char_data = 0; // Инициализация переменной для хранения ASCII символов
        if (binary_data[0] == '0') {
          if (checkParityBit(binary_data)) { // Проверка на чётность
            for (int i = 0; i < 8; i++) { // Преобразование двоичных данных в ASCII символы
              char_data |= (binary_data[i] - '0') << (7 - i); // Сдвиг и объединение битов
            }

            Serial.print(char_data); // Вывод ASCII символа в Serial порт
            answer += char_data;
          }
          else {
            Serial.println("*");
          }
        }
        input = ""; // Очистка строки ввода для следующей порции данных
      }
      else {
        Serial.print("*");
      }
    }
  }

    while((micros()-time_till_last_iteraton) < interval){
    delayMicroseconds(1);
  }
  time_till_last_iteraton = micros();
}



void loop() {  // код для нахождения начального IDLE и запуска программы (убрать е в конце и поменять main)
  solar_panel = analogRead(SOLAR_PIN);
  if (Is_start_of_transieve) {
    loope();
  }
  else {
    if (solar_panel > solar_panel_porogovoe_znach) {
      if (!Is_start_idle) {
        time_till_start_idle = millis();
        Is_start_idle = true;
      }
      Is_start_of_transieve = true;
    }
    else {
      Is_start_idle = false;
    }
  }
  if (Is_end_of_transieve) {
    Is_start_of_transieve = false;
    Is_end_of_transieve = false;
    time_till_start_idle = millis();
  }
}




bool checkParityBit(const char* binary_data) {
  int count = 0;
  for (int i = 1; i < 9; i++) {
    if (binary_data[i] == '1') {
      count++;
    }
  }
  return count % 2 == 0;
}
