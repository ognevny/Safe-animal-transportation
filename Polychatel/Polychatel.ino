#include <SPI.h>                  // Подключаем библиотеку SPI
#include <nRF24L01.h>             // Подключаем библиотеку nRF24L01
#include <RF24.h>
#include <LiquidCrystal_I2C.h>                 // Подключаем библиотеку RF24
#include <Wire.h>
int regim = 0;
LiquidCrystal_I2C lcd(0x27,16,2);

RF24 radio(9, 10);                // Указываем номера выводов nRF24L01: CE и CSN
const uint32_t pipe = 111156789;  // адрес "трубы" - адрес приёмника
int count = 0;
void setup() {
  while (!Serial);
  Serial.begin(9600);              // устанавливаем скор. обмена данными 9600 бод
  radio.begin();                   // Инициируем работу nRF24L01
  radio.setChannel(0x5f);          // Устанавливаем канал
  radio.openReadingPipe(0, pipe);  // Открываем "трубу" чтения
  radio.startListening();
  
  lcd.init();                      // Инициализация дисплея  
  lcd.backlight();                 // Подключение подсветки
  lcd.setCursor(0,0);              // Установка курсора в начало первой строки
  lcd.print("Hello");       // Набор текста на первой строке
  lcd.setCursor(0,1);              // Установка курсора в начало второй строки
  lcd.print("Start");       // Набор текста на второй строке
  delay(1000);

}
void loop() {
  if (radio.available()) {            // если модуль получил информацию
    lcd.setCursor(0,0);
    lcd.print("                 ");
    lcd.setCursor(0,1);
    lcd.print("                 ");

    uint8_t buffer[32];          // переменная, хранящая текст сообщения
    radio.read(&buffer, sizeof(buffer));  // читаем текст сообщения
    String message = String((char*)buffer);
    //Serial.println(message);

    char* token = strtok(message.c_str(), " ");
    int temperature = atoi(token);
    token = strtok(NULL, " ");
    int humidity = atoi(token);
    token = strtok(NULL, " ");
    int co2 = atoi(token);
    token = strtok(NULL, " ");
    int regim = atoi(token);

    Serial.print(temperature);
    Serial.print(":"); 
    Serial.print(humidity);
    Serial.print(":");
    Serial.print(co2);
    Serial.print(":");
    Serial.print(regim);
    Serial.print("   ");           // выводим текст в монитор порта
    Serial.println(count);
    count++;

    switch (regim) {
    case 0:
        lcd.setCursor(0,0);
        lcd.print("Temp:");
        lcd.setCursor(8,0);
        lcd.print("CO2:");
        lcd.setCursor(0,1);
        lcd.print("Humi:");
        lcd.setCursor(8,1);
        lcd.print("Con:");
        lcd.setCursor(5,0);
        lcd.print(temperature);
        lcd.setCursor(5,1);
        lcd.print(humidity);
        lcd.setCursor(12,0);
        lcd.print(co2);
        lcd.setCursor(12,1);
        lcd.print(count);
        break;
    case 1:
        lcd.setCursor(0,0);
        lcd.print("Setings:");
        lcd.setCursor(0,1);
        lcd.print("Temperature:");
        lcd.setCursor(13,1);
        lcd.print(temperature);
        break;
    case 2:
        lcd.setCursor(0,0);
        lcd.print("Setings:");
        lcd.setCursor(0,1);
        lcd.print("Humidity:");
        lcd.setCursor(9,1);
        lcd.print(humidity);
        break;
    case 3:
        lcd.setCursor(0,0);
        lcd.print("Setings:");
        lcd.setCursor(0,1);
        lcd.print("Co2:");
        lcd.setCursor(5,1);
        lcd.print(co2);
        break;
} 
       // Набор текста на второй строке  
  }
}