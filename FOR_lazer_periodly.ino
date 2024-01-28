// C++ code в бинарный
#define LASER_PIN 5
static unsigned long previousMillis = 0; // Переменная для хранения времени последнего срабатывания
const unsigned long interval = 40; // Интервал в миллисекундах
unsigned long currentMillis = millis(); // Получение текущего времени

void setup() {
  Serial.begin(115200); // Инициализация Serial порта
  pinMode(LASER_PIN, OUTPUT);

}

static String input1 = "Battery: 83%";
static String input2 = "X:781 Y:75 Z:1619";
static String input3 = "Fatal ERROR";

int what_to_send = 0;

unsigned long previousFire = millis();

char bin_data[12]; // Создание массива для хранения двоичных данных (начинающий 0 символ + 8 бит + бит четности+завершающий 1 символ + завершающий нулевой символ)

void loop() {
  static String input; // Переменная для хранения ввода
  if ((millis() - previousFire) > 5000) {
    if (what_to_send == 0) {
      input = input1;
    }
    if (what_to_send == 1) {
      input = input2;
    }
    if (what_to_send == 2) {
      input = input3;
    }
    Serial.println(input);
    for (int j = 0; j < input.length(); j++) { // Обработка каждого символа в строке
      for (int i = 7; i >= 0; i--) { // Преобразование символа в двоичный сигнал и вывод в Serial порт
        int bit = (input[j] >> i) & 1; // Получение каждого бита
        //Serial.print(bit); // Вывод каждого бита в Serial порт
        // Добавление каждого бита в массив bin_data
        if (bit == 1) {
          bin_data[8 - i] = '1';
        } else {
          bin_data[8 - i] = '0';
        }
      }
      bin_data[0] = '0';
      if (checkParityBit(bin_data)) {
        bin_data[9] = '0';
      }
      else {
        bin_data[9] = '1';
      }

      bin_data[10] = '1';

      bin_data[11] = '\0'; // Установка завершающего нулевого символа
      //Serial.print(" "); // Вывод пробела между каждым байтом
      Serial.print(bin_data); // Вывод массива bin_data в Serial порт
      for (int q = 0; q < 11; q++) {
        currentMillis = millis(); // Получение текущего времени
        while (currentMillis - previousMillis < interval) { // Проверка прошедшего времени
          delayMicroseconds(20);
          currentMillis = millis(); // Получение текущего времени
        }
        previousMillis = currentMillis; // Обновление времени последнего срабатывания
        if (bin_data[q] == '1') {
          digitalWrite(LASER_PIN, HIGH);
        }
        else {
          digitalWrite(LASER_PIN, LOW);
        }
      }
    }
    Serial.println(); // Переход на новую строку
    input = ""; // Очистка строки ввода для следующей порции данных
    what_to_send++;
    if (what_to_send == 3) {
      what_to_send = 0;
    }
    previousFire = millis();
  }
  else {
    digitalWrite(LASER_PIN, HIGH);
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
