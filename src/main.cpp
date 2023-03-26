#include <Arduino.h>
#include <Keypad.h>
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal.h>

RTC_DS3231 rtc;

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns

// Map the buttons to an array for the Keymap instance
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte colPins[ROWS] = {5, 4, 3, 2}; // Pins used for the rows of the keypad
byte rowPins[COLS] = {9, 8, 7, 6}; // Pins used for the columns of the keypad

// Initialise the Keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

typedef struct
{
  uint16_t jahr = 2008;
  uint8_t stunde = 13;
  uint8_t minute = 17;
  uint8_t sekunde = 8;
  uint16_t tag = 1;

} time_struct;

const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

enum zustaende
{
  Knopf_A,
  Knopf_B,
  Knopf_C,
  Knopf_D,
};

void set_usertime();                                            // Ermöglicht den Benutzer seine Einstellung vorzunehmen.
void print_time();                                              // Printed die aktuelle Uhrzeit der Uhr
void print_time_passed();                                       // Printed die verstrichene Zeit seit anfang der Einstellung.
unsigned int get_days_passed();                                 // Berechnet die verstrichenen Tage seit anfang der Einstellung.
uint32_t get_seconds_passed_daily();                            // Berechnet die verstrichenen Sekunden seit 0 Uhr.
int check_status(uint32_t aufgang_sek, uint32_t untergang_sek); // gibt 1 für Licht muss an sein und 0 für Licht muss aus sein.
uint32_t get_offset();                                          // Berechnet den Offset in bezug zu 1970 zu dem jeweiligen start datum der einstellung. Bsp sekunden von 1970 bis 2002
void increaseDateTime(DateTime &dt, int days);                  // Erhöht das Datum einer DateTime Variablen um eine Anzahl von Tagen
void check_star();                                              // Überprüft ob die Sternchentaste gedrückt geahlten wird, nach 3 Sekunden führt sie dann set_usertime() aus.
uint32_t check_aufgang_sek(unsigned int days);
uint32_t check_untergang_sek(unsigned int days);
void display_idle(char Knopf);
void check_Knopf();

void setup()
{
  Serial.begin(57600); // Initialise the serial monitor

  lcd.begin(16, 2);
  lcd.print("Loading...");

  Wire.begin();
  rtc.begin();

  if (!rtc.begin())
  {
    lcd.println("RTC not found!");
    while (1)
      ;
  }

  DateTime dt1(2002, 1, 1, 0, 0, 0);
  uint32_t start_2002 = dt1.unixtime();
  Serial.println(start_2002);
}

void loop()
{
  static unsigned long lastTime = 0;

  check_star();
  check_Knopf();

  if (millis() - lastTime >= 1000)

  {
    lastTime = millis();
    /*
    Serial.print("Tag: ");
    Serial.println(get_days_passed());
    Serial.print("Aufgang: ");
    Serial.println(check_aufgang_sek(get_days_passed()));
    Serial.print("Untergang: ");
    Serial.println(check_untergang_sek(get_days_passed()));
    Serial.print("Licht:  ");
    Serial.println(check_status(check_aufgang_sek(get_days_passed()), check_untergang_sek(get_days_passed())));
    Serial.print("Akteulle Sek:");
    Serial.println(get_seconds_passed_daily());
    */
    // print_time();
    // print_time_passed();
    // Serial.println(get_seconds_passed_daily());
    display_idle('E');
  }
}

void check_Knopf()
{
  static char puffer;
  puffer = customKeypad.getKey();
  if (puffer == 'A')
  {
    display_idle('A');
  }
  if (puffer == 'B')
  {
    display_idle('B');
  }
  if (puffer == 'C')
  {
    display_idle('C');
  }
  if (puffer == 'D')
  {
    display_idle('D');
  }
  
}

void display_idle(char Knopf)
{

  static zustaende idle_zustand = Knopf_A;
  static char puffer = Knopf_A;
  
  if (Knopf != 'E')
  {
    puffer = Knopf;
  }
  
  if (puffer == 'A')
  {
    idle_zustand = Knopf_A;
  }
  else if (puffer == 'B')
  {
    idle_zustand = Knopf_B;
  }
  else if (puffer == 'C')
  {
    idle_zustand = Knopf_C;
  }
  else if (puffer == 'D')
  {
    idle_zustand = Knopf_D;
  }

  switch (idle_zustand)
  {
  case Knopf_A:
    
    lcd.setCursor(0, 0);
    lcd.print((String)"Uhrzeit:" + get_seconds_passed_daily() + "sek        ");
    lcd.setCursor(0,1);
    lcd.print((String)"Tag: " + get_days_passed() + "            ");
    break;

  case Knopf_B:
   
    lcd.setCursor(0, 0);
    lcd.print("Idle B");
    break;

  case Knopf_C:
    
    lcd.setCursor(0, 0);
    lcd.print("Idle C");
    break;

  case Knopf_D:
    
    lcd.setCursor(0, 0);
    lcd.print("Idle D");
    break;
  }
}

void check_star()
{
  static uint8_t counter = 0;
  static unsigned long lasttime = 0;

  // Serial.println(millis() - lasttime);

  if (millis() - lasttime >= 1000)
  {
    counter = 0;
    lasttime = millis();
  }

  if (customKeypad.getKey() == '*')
  {
    counter++;
    lasttime = millis();
  }

  if (counter > 2)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESET MENU!");
    lcd.setCursor(0, 1);
    lcd.print("USE '#' 4 RESET");

    if (customKeypad.waitForKey() == '#')
    {
      set_usertime();
      counter = 0;
    }
    else
    {
      counter = 0;
    }
  }
}

void set_usertime()
{
  int set_process = 0;   // Zaehler Variable
  time_struct user_time; // Defeniert neuen Zeit Speicher

  // erste Schleife für Jahr / Code
  do
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Modi eingeben");
    lcd.setCursor(0,1);
    lcd.print("bsp.'2008' ");

    char user_time_code[5] = "2008";

    for (int i = 0; i < 4; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
      lcd.print(user_time_code[i]);
    }
    lcd.clear();
    lcd.print("Eingabe: "); lcd.print(user_time_code[0]); lcd.print(user_time_code[1]); lcd.print(user_time_code[2]); lcd.print(user_time_code[3]);
    //Serial.println(String("Eingabe: ") + user_time_code[0] + user_time_code[1] + user_time_code[2] + user_time_code[3]);

    if (atoi(user_time_code) == 2002) // hier muss Funktion kommen die Checkt, ob der ModiCode auch wirklich  einen Modi hat.
    {
      set_process = 1;
      user_time.jahr = atoi(user_time_code);
      lcd.setCursor(0,1);
      lcd.print("Half Season");
      delay(2000);
    }
    else
    {
      lcd.clear();
      lcd.print("NOT FOUND!");
      delay(400);
    }

  } while (set_process == 0);

  // zweite Schleife für Stunde
  do
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Stunde eingeben");
    lcd.setCursor(0,1);
    lcd.print("bsp.'13' ");

    char user_time_code[3] = "13";

    for (int i = 0; i < 2; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
      Serial.print(user_time_code[i]);
    }

    Serial.println();
    Serial.println(String("Eingabe: ") + user_time_code[0] + user_time_code[1]);

    if (atoi(user_time_code) < 24)
    {
      set_process = 2;
      user_time.stunde = atoi(user_time_code);
    }
    else
    {
      Serial.println("UHRZEIT ZU GROSS");
      delay(400);
    }

  } while (set_process == 1);

  // dritte Schleife für Minuten
  do
  {
    Serial.println("Minuten Eingeben bsp'13'");

    char user_time_code[3] = "13";

    for (int i = 0; i < 2; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
      Serial.print(user_time_code[i]);
    }

    Serial.println();
    Serial.println(String("Eingabe: ") + user_time_code[0] + user_time_code[1]);

    if (atoi(user_time_code) < 60)
    {
      set_process = 3;
      user_time.minute = atoi(user_time_code);
    }
    else
    {
      Serial.println("UHRZEIT ZU GROSS");
      delay(400);
    }

  } while (set_process == 2);

  // vierte Schleife für Tage
  do
  {
    Serial.println("Tage skippen Bsp. '023'");

    char user_time_code[4] = "000";

    for (int i = 0; i < 3; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
      Serial.print(user_time_code[i]);
    }

    Serial.println();
    Serial.println(String("Eingabe: ") + user_time_code[0] + user_time_code[1] + user_time_code[2]);

    if (atoi(user_time_code) < 300)
    {
      set_process = 4;

      rtc.adjust(DateTime(user_time.jahr, 1, 1, user_time.stunde, user_time.minute, 45) + TimeSpan(atoi(user_time_code), 0, 0, 0));
    }
    else
    {
      Serial.println("TAGE ZU GROSS");
      delay(400);
    }

  } while (set_process == 3);

  Serial.println("SUCCESS");

  delay(1000);
}

void print_time()
{

  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

void print_time_passed()
{
  static DateTime now;
  static uint32_t sekunden_offset;

  now = rtc.now();

  sekunden_offset = get_offset();

  Serial.println(now.unixtime() - sekunden_offset);
}

uint32_t get_seconds_passed_daily()
{
  static DateTime now;
  static uint32_t sekunden_passed_day;

  now = rtc.now();

  sekunden_passed_day = (now.unixtime() - get_offset()) % (86400); // 86400 = 60*60*24

  return sekunden_passed_day;
}

int check_status(uint32_t aufgang_sek, uint32_t untergang_sek)
{
  if ((get_seconds_passed_daily() > aufgang_sek))
  {
    if ((get_seconds_passed_daily() < untergang_sek))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

uint32_t get_offset()
{
  static DateTime now;

  now = rtc.now();
  if (now.year() == 2002)
  {
    return 1009843200;
  }

  return 0; // Falls es das Jahr nicht mehr geben sollte, könnte passieren wenn die Uhr lange nicht benutzt wird.
}

unsigned int get_days_passed()
{

  static DateTime now;
  static uint32_t puffer;
  static unsigned int counter;

  counter = 1;
  now = rtc.now();

  puffer = now.unixtime() - get_offset();

  while (puffer >= 86400)
  {
    puffer = puffer - 86400;
    counter++;
  }

  return counter;
}

void increaseDateTime(DateTime &dt, int days)
{
  dt = dt + TimeSpan(days, 0, 0, 0);
}

uint32_t check_aufgang_sek(unsigned int days)
{
  static DateTime now;

  now = rtc.now();
  if (now.year() == 2002)
  {
    return (uint32_t)(21600 - (3600 / 90) * (days - 1));
  }
  return 0;
}

uint32_t check_untergang_sek(unsigned int days)
{
  static DateTime now;

  now = rtc.now();
  if (now.year() == 2002)
  {
    return (uint32_t)(64800 + (3600 / 90) * (days - 1));
  }
  return 0;
}