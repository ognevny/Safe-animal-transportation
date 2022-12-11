#include "DHT.h" // библиотека для работы с датчиком DHT11
#define DHTPIN 2 // номер пина DHT 11
#include <Wire.h> // библиотека для работы 
#include <LiquidCrystal_I2C.h> // библиотека для работы экрана 

LiquidCrystal_I2C lcd(0x27, 16, 2); // инициализация экрана 

int gasValue; 

DHT dht(DHTPIN, DHT11);
void setup() {

  dht.begin();



  lcd.init();
  lcd.backlight(); // Включаем подсветку дисплея
  lcd.setCursor(0, 0);
  lcd.print("CO2");

  lcd.setCursor(0, 1);
  lcd.print("Hum");
  lcd.setCursor(8, 1);
  lcd.print("Tem");
}

void loop() {
  delay(2000); // 2 секунды задержки
  int h = dht.readHumidity(); // Измеряем влажность
  int t = dht.readTemperature(); // Измеряем температуру

  gasValue = analogRead(A0);

  lcd.setCursor(4, 1);
  lcd.print(h);
  lcd.setCursor(12, 1);
  lcd.print(t);
  lcd.setCursor(4, 0);
  lcd.print(gasValue);
}