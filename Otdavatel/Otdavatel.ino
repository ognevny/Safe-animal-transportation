#include <SPI.h>       // Подключаем библиотеку SPI
#include <nRF24L01.h>  // Подключаем библиотеку nRF24L01
#include <RF24.h>      // Подключаем библиотеку RF24
#include <Wire.h>
#include <DHT.h>
#define DHTPIN 2
#define MQPIN A0

RF24 radio(9, 10);                // Указываем номера выводов nRF24L01: CE и CSN
const uint32_t pipe = 111156789;  // адрес "трубы" - адрес передатчика
DHT dht(DHTPIN, DHT11);

int temperature, val; // значение температуры и угол потенциометра
int save_temperature = 25, save_co2 = 100, save_humidity; // настроенные значения
bool bool_temperature, bool_co2; // идёт ли нагрев или охлаждение
int humidity; // значение влажности
int co2; // значение CO2
int regim = 0;  // начальное значение переменной состояния
int time_regim = 0; // по истечении данного времени regim переходит в 0
long long time_send = 0; // время передачи
int buttonPin = 4;  // номер пина, к которому подключена кнопка
int RelePin1 = 5; // пин на температуру
int RelePin2 = 6; //------------------------------------

void setup() {
  Serial.begin(9600);                // инициализация serial порта
  radio.begin();                     // Инициируем работу nRF24L01
  radio.setChannel(0x5f);            // Устанавливаем канал
  radio.openWritingPipe(pipe);       // Открываем "трубу" отправки данных
  radio.stopListening();             // Прекращаем прослушивание
  pinMode(buttonPin, INPUT_PULLUP);  // устанавливаем пин кнопки в режим входа с подтяжкой к питанию
  pinMode(A1, INPUT);                // устанавливаем пин A1 как вход
  pinMode(RelePin1, OUTPUT);         // установка пина на реле в вывод сигнала
  pinMode(RelePin2, OUTPUT);         // установка пина на реле в вывод сигнала
  dht.begin();
}
void loop() {
  switch (regim) { // отправляет нужные данные в зависимости режима
    case 0: // стандартный режим. считывание всех данных
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();
      co2 = analogRead(MQPIN);
      break;
    case 1: // настройка температуры
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();
      co2 = analogRead(MQPIN);
      val = analogRead(A1);                    // считываем значение с потенциометра
      temperature = map(val, 0, 1023, 0, 40);  // преобразуем значение в выходное значение от 0 до 100
      save_temperature = temperature;
      humidity = 0;
      co2 = 0;

      break;
    case 2: // настройка влажности
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();
      co2 = analogRead(MQPIN);
      temperature = 0;
      humidity = map(analogRead(A1), 0, 1023, 0, 100);
      save_humidity = humidity;
      co2 = 0;
      break;
    case 3: // настройка CO2
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();
      co2 = analogRead(MQPIN);
      temperature = 0;
      humidity = 0;
      co2 = map(analogRead(A1), 0, 1023, 0, 100);
      save_co2 = co2;
      break;
  }

  if (time_regim / 40 == 1) { // через 60 секунд режим обнуляется
    regim = 0;
    time_regim = 0;
  }

  if (save_temperature - temperature < 0) bool_temperature = 0; // перегрев
  if (save_temperature - temperature > 5 or bool_temperature) { // переохлаждение
    digitalWrite(RelePin1, 1);
    bool_temperature = 1;
  } else {
    digitalWrite(RelePin1, 0);
  }

  if (save_co2 - co2 < 0) bool_co2 = 0; // превышение CO2
  if (save_co2 - co2 > 5 or bool_co2) { // мало CO2
    digitalWrite(RelePin2, 1);
    bool_co2 = 1;
  } else {
    digitalWrite(RelePin2, 0);
  }



  if (digitalRead(buttonPin) == LOW) {    // если кнопка нажата
    delay(100);                           // задержка для фиксации нажатия
    if (digitalRead(buttonPin) == LOW) {  // если кнопка все еще нажата после задержки
      regim++;                            // увеличиваем значение переменной состояния на 1
      time_regim = 0;
      if (regim > 3) {  // если значение переменной состояния больше 3
        regim = 0;      // устанавливаем значение переменной состояния в 0
      }
    }
  }
  if (millis() - time_send > 1500) { // каждые 1.5 секунды отправляются данные на второе устройства
    time_send = millis();
    // преобразование полученных данных в сообщение
    String message = String(temperature) + " " + String(humidity) + " " + String(co2) + " " + String(regim);
    // Serial.println(message);
    uint8_t buffer[32]; // тип данных, которые отправляет антенна

    message.getBytes(buffer, message.length() + 1); // преобразование в массив
    radio.write(buffer, sizeof(buffer));  // Отсылаем текст
    // Serial.println("send");
    time_regim++;
  }
}