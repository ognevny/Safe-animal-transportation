#include <SPI.h>       // Подключаем библиотеку SPI
#include <nRF24L01.h>  // Подключаем библиотеку nRF24L01
#include <RF24.h>      // Подключаем библиотеку RF24
#include <Wire.h>
#include <DHT.h>
//зжссф
#define DHTPIN 2
#define MQPIN A0

RF24 radio(9, 10);                // Указываем номера выводов nRF24L01: CE и CSN
const uint32_t pipe = 111156789;  // адрес "трубы" - адрес передатчика
DHT dht(DHTPIN, DHT11);

int temperature, val;
int save_temperature = 25, save_co2 = 100, save_humidity;
bool bool_temperature, bool_co2;
int humidity;
int co2;
int regim = 0;// начальное значение переменной состояния
int time_regim = 0;
long long time_send = 0;
int buttonPin = 4; // номер пина, к которому подключена кнопка
int RelePin1 = 5;
int RelePin2 = 6; //------------------------------------

void setup() {
  Serial.begin(9600);
  radio.begin();                // Инициируем работу nRF24L01
  radio.setChannel(0x5f);       // Устанавливаем канал
  radio.openWritingPipe(pipe);  // Открываем "трубу" отправки данных
  radio.stopListening();        // Прекращаем прослушивание
  pinMode(buttonPin, INPUT_PULLUP); // устанавливаем пин кнопки в режим входа с подтяжкой к питанию
  pinMode(A1, INPUT); // устанавливаем пин A1 как вход
  pinMode(RelePin1, OUTPUT);
  pinMode(RelePin2, OUTPUT);
  dht.begin();
}
void loop() {
  
  
  switch (regim){
    case 0:
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        co2 = analogRead(MQPIN);
        break;
    case 1:
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        co2 = analogRead(MQPIN);
        val = analogRead(A1); // считываем значение с потенциометра
        temperature = map(val, 0, 1023, 0, 40); // преобразуем значение в выходное значение от 0 до 100
        save_temperature = temperature;
        humidity = 0; 
        co2 = 0;
        
        break;
    case 2:
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        co2 = analogRead(MQPIN);
        temperature = 0;
        humidity = map(analogRead(A1), 0, 1023, 0, 100);
        save_humidity = humidity;
        co2 = 0;
        break;
    case 3:
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        co2 = analogRead(MQPIN);
        temperature = 0;
        humidity = 0;
        co2 = map(analogRead(A1), 0, 1023, 0, 100);
        save_co2 = co2;
        break;
  }

  if (time_regim/40 == 1){
    regim = 0;
    time_regim = 0;
  }
  
  if (save_temperature - temperature < 0) bool_temperature = 0;
  if (save_temperature - temperature > 5 or bool_temperature){
    digitalWrite(RelePin1, 1);
    bool_temperature = 1;
  } else{
   digitalWrite(RelePin1, 0);
   }

  if (save_co2 - co2 < 0) bool_co2 = 0;
  if (save_co2 - co2 > 5 or bool_co2){
    digitalWrite(RelePin2, 1);
    bool_co2 = 1;
  } else{
   digitalWrite(RelePin2, 0);
   }


  
  if (digitalRead(buttonPin) == LOW) { // если кнопка нажата
    delay(100); // задержка для фиксации нажатия
    if (digitalRead(buttonPin) == LOW) { // если кнопка все еще нажата после задержки
      regim++; // увеличиваем значение переменной состояния на 1
      time_regim = 0;
      if (regim > 3) { // если значение переменной состояния больше 3
        regim = 0; // устанавливаем значение переменной состояния в 0
      }
    }
  } 
  if (millis() - time_send > 1500){
  time_send = millis();
  
  String message = String(temperature) + " " + String(humidity) + " " + String(co2) + " " + String(regim);
  Serial.println(message);
  uint8_t buffer[32];
  
  message.getBytes(buffer, message.length() + 1);
  radio.write(buffer, sizeof(buffer));  // Отсылаем текст
  Serial.println("send");
  time_regim++;
  
  }
  
}