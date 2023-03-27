#include <LiquidCrystal_I2C.h>
#include <GyverNTC.h>
#include <SoftwareSerial.h>

#define BUTTON_PIN 3
#define RS485_RX_PIN 4
#define RS485_TX_PIN 5


LiquidCrystal_I2C lcd(0x3F, 16, 2);
GyverNTC ntc(A0, 10000, 3920, 25, 10000);
SoftwareSerial rs485(RS485_RX_PIN, RS485_TX_PIN);

boolean btn_flag = 0;
boolean btn;

void setup()
{
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.noBacklight();

  rs485.begin(9600);
}

/* Print temperature to LCD */
void printTemperatureToLCD()
{
  float temp = ntc.getTempAverage();

  Serial.print("[NTC]: ");
  Serial.print(temp);
  Serial.print(" Celcius; ");

  lcd.setCursor(0, 0);
  lcd.print("Temp = ");
  lcd.print(temp);
  lcd.print(" C");

  temp = temp * 1.8 + 32.0;

  Serial.print(temp);
  Serial.println(" Fahrenheit");

  lcd.setCursor(0, 1);
  lcd.print("Temp = ");
  lcd.print(temp);
  lcd.print(" F");
}

/* Return true if button pressed */
bool isButtonPressed() {
  return !digitalRead(BUTTON_PIN);
}

/* If button pressed backlight is on */
void checkBacklightState()
{
  if (isButtonPressed() && !btn_flag)
  {
    btn_flag = 1;
    lcd.backlight();
  }else if (!isButtonPressed() && btn_flag)
  {
    btn_flag = 0;
    lcd.noBacklight();
  }
}

/* Send tempreture throw rs485 */
void sendTemperatureRS485()
{
  float temperature = ntc.getTempAverage();

  Serial.print("[RS485]: ");
  Serial.println(temperature);

  rs485.println(temperature);
}

/* Send float array in bytes throw rs485 */
void sendArrayRS485(byte *data, int size)
{
  rs485.write(data, size);
  
  Serial.print("[RS485]: ");

  for (int i = 0; i < size; i++)
  {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  Serial.println("");
}

/* Main loop */
void loop()
{
  checkBacklightState();

  // Displaying temperature on LCD every second
  static uint32_t timerLCD = millis();
  if (millis() - timerLCD >= 1000)
  {
    timerLCD = millis();
    printTemperatureToLCD();
  }

  // Send tempreture throw rs485 every 4 seconds  
  static uint32_t timerRS485 = millis();
  if (millis() - timerRS485 >= 4000)
  {
    timerRS485 = millis();
    //sendTemperatureRS485();
    float temp = ntc.getTempAverage();
    float values[] = {temp, (temp * 1.8 + 32.0)};

    sendArrayRS485((byte*)values, sizeof(values));
  }
}
