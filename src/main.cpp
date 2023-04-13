#include <Arduino.h>
#include <Keypad.h>
#include <RTClib.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <RCSwitch.h>

#include "sun_shine_time.h"

const int HALF_SEASON = 2002;
const int FULL_SEASON = 2004;
const int TROPICAL = 2006;



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

int LICHT_CHECK;      // Sagt aus ob Licht an sein muss
unsigned days_passed; // Vergangene Tage beginnend bei 1
char getKey_puffer;   // zuletzt eingegebene Taste wird hier gespeichert

void set_usertime();                                            // Erm├╢glicht den Benutzer seine Einstellung vorzunehmen.
void print_time();                                              // Printed die aktuelle Uhrzeit der Uhr
void print_time_passed();                                       // Printed die verstrichene Zeit seit anfang der Einstellung.
unsigned int get_days_passed();                                 // Berechnet die verstrichenen Tage seit anfang der Einstellung.
int check_status(uint32_t aufgang_sek, uint32_t untergang_sek); // gibt 1 f├╝r Licht muss an sein und 0 f├╝r Licht muss aus sein.
void check_star();                                              // ├£berpr├╝ft ob die Sternchentaste gedr├╝ckt geahlten wird, nach 3 Sekunden f├╝hrt sie dann set_usertime() aus.
void display_idle(char Knopf);                                  // Bekommt A,B,C,D ├╝bergebn und wechselt dann in den entsprechenden Modi
void check_Knopf();                                             // ├£berpr├╝ft ob Knopf gedr├╝ckt wurde und ruft entsprechend die Funktion auf
//void transmit_data(int state);                  

void setup()
{
  Serial.begin(57600); // Initialise the serial monitor

  lcd.begin(16, 2);
  lcd.print("Loading");
  delay(300);
  lcd.print(".");
  delay(300);
  lcd.print(".");
  delay(300);
  lcd.print(".");
  delay(300);

  mySwitch.enableTransmit(3);
  mySwitch.setPulseLength(321); // 321ms
  mySwitch.setProtocol(1);

  Wire.begin();
  rtc.begin();

  if (!rtc.begin())
  {
    lcd.clear();
    lcd.println("RTC not found!");
    while (1)
      ;
  }

  pinMode(12, OUTPUT);

  // DateTime dt1(2006, 1, 1, 0, 0, 0); //Findet den Offset raus
  // Serial.println(dt1.unixtime());
}

void loop()
{
  static unsigned long lastTime_1000 = 0;

  check_Knopf();
  
  if (millis() - lastTime_1000 >= 1000)
  {
    lastTime_1000 = millis(); // setzt Schleife zur├╝ck

    days_passed = get_days_passed();
    LICHT_CHECK = check_status(check_aufgang_sek(days_passed), check_untergang_sek(days_passed)); // 6.5ms
    digitalWrite(12, LICHT_CHECK);

    transmit_data(LICHT_CHECK);

    display_idle(' '); // 9ms
  }
}

void check_Knopf()
{
  getKey_puffer = customKeypad.getKey();

  if (getKey_puffer == 'A')
  {
    display_idle('A');
  }
  else if (getKey_puffer == 'B')
  {
    display_idle('B');
  }
  else if (getKey_puffer == 'C')
  {
    display_idle('C');
  }
  else if (getKey_puffer == 'D')
  {
    display_idle('D');
  }
  else if (getKey_puffer == '*')
  {
    check_star();
  }
}

void display_idle(char Knopf)
{
  static DateTime now;
  static zustaende idle_zustand = Knopf_A;
  static char puffer = Knopf_A;
  static char time_puffer[10];

  if (Knopf != ' ')
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

    secondsToFullTime(get_seconds_passed_daily(), time_puffer);
    lcd.setCursor(0, 0);
    lcd.print((String) "Uhrzeit:" + time_puffer + "    ");
    lcd.setCursor(0, 1);
    lcd.print((String) "Tag:" + get_days_passed() + "   ");
    lcd.setCursor(8, 1);
    secondsToHour(get_lightseconds(), time_puffer);
    lcd.print((String) "LH:" + time_puffer + "h        ");
    break;

  case Knopf_B:

    secondsToFullTime(check_aufgang_sek(get_days_passed()), time_puffer);
    lcd.setCursor(0, 0);
    lcd.print((String) "Rising: " + time_puffer + "   ");
    secondsToFullTime(check_untergang_sek(get_days_passed()), time_puffer);
    lcd.setCursor(0, 1);
    lcd.print((String) "Dawn  : " + time_puffer + "   ");
    break;

  case Knopf_C:

    now = rtc.now();

    lcd.setCursor(0, 0);
    lcd.print((String) "Cycle: " + now.year() + "      ");
    lcd.setCursor(0, 1);
    if (now.year() == HALF_SEASON)
    {
      lcd.print("Half Season  80d  ");
    }
    else if (now.year() == FULL_SEASON)
    {
      lcd.print("Full Season 120d ");
    }
    else if (now.year() == TROPICAL)
    {
      lcd.print("Tropical    100d ");
    }
    else
    {
      lcd.print("NO MODE          ");
    }

    break;

  case Knopf_D:
    now = rtc.now();

    lcd.setCursor(0, 0);
    lcd.print((String)now.day() + "/" + now.month() + "/" + now.year() + " " + now.hour() + ":" + now.minute() + "        ");
    lcd.setCursor(0, 1);
    lcd.print((String) "HEUTE:                   ");

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

  if (getKey_puffer == '*')
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

    if (customKeypad.waitForKey() == '#') // hier m∩┐╜sste eigentlich eine bessere L∩┐╜sung her
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

  // erste Schleife f├╝r Jahr / Code
  do
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Modi eingeben");
    lcd.setCursor(0, 1);
    lcd.print("Bsp.'2008' ");

    char user_time_code[5] = "2008";

    for (int i = 0; i < 4; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
      lcd.print(user_time_code[i]);
    }
    delay(400);
    lcd.clear();
    lcd.print("Eingabe: ");
    lcd.print(user_time_code[0]);
    lcd.print(user_time_code[1]);
    lcd.print(user_time_code[2]);
    lcd.print(user_time_code[3]);
    // Serial.println(String("Eingabe: ") + user_time_code[0] + user_time_code[1] + user_time_code[2] + user_time_code[3]);

    if (atoi(user_time_code) == HALF_SEASON)
    {
      set_process = 1;
      user_time.jahr = atoi(user_time_code);
      lcd.setCursor(0, 1);
      lcd.print("HALF SEASON");
      delay(2000);
    }
    else if (atoi(user_time_code) == FULL_SEASON)
    {
      set_process = 1;
      user_time.jahr = atoi(user_time_code);
      lcd.setCursor(0, 1);
      lcd.print("FULL SEASON");
      delay(2000);
    }
    else if (atoi(user_time_code) == TROPICAL)
    {
      set_process = 1;
      user_time.jahr = atoi(user_time_code);
      lcd.setCursor(0, 1);
      lcd.print("TROPICAL");
      delay(2000);
    }
    else
    {
      lcd.clear();
      lcd.print("NOT FOUND!");
      delay(600);
    }

  } while (set_process == 0);

  // zweite Schleife f├╝r Stunde
  do
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Stunde eingeben");
    lcd.setCursor(0, 1);
    lcd.print("Bsp.'13' ");

    char user_time_code[3] = "13";

    for (int i = 0; i < 2; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
      lcd.print(user_time_code[i]);
    }
    delay(400);
    lcd.clear();
    lcd.print("Eingabe: ");
    lcd.print(user_time_code[0]);
    lcd.print(user_time_code[1]);
    lcd.print(" Uhr!");
    delay(1000);

    if (atoi(user_time_code) < 24)
    {
      set_process = 2;
      user_time.stunde = atoi(user_time_code);
    }
    else
    {
      lcd.clear();
      lcd.print("UHRZEIT ZU GROSS");
      delay(600);
    }

  } while (set_process == 1);

  // dritte Schleife f├╝r Minuten
  do
  {
    lcd.clear();
    lcd.print("Minuten Eingeben");
    lcd.setCursor(0, 1);
    lcd.print("Bsp.'41' ");

    char user_time_code[3] = "41";

    for (int i = 0; i < 2; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
      lcd.print(user_time_code[i]);
    }

    delay(400);

    lcd.clear();
    lcd.print("Eingabe: ");
    lcd.print(user_time_code[0]);
    lcd.print(user_time_code[1]);
    lcd.print(" min!");

    delay(1000);

    if (atoi(user_time_code) < 60)
    {
      set_process = 3;
      user_time.minute = atoi(user_time_code);
    }
    else
    {
      lcd.clear();
      lcd.print("UHRZEIT ZU GROSS");
      delay(600);
    }

  } while (set_process == 2);

  // vierte Schleife f├╝r Tage
  do
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tage skippen");
    lcd.setCursor(0, 1);
    lcd.print("Bsp.'023' ");

    char user_time_code[4] = "023";

    for (int i = 0; i < 3; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
      lcd.print(user_time_code[i]);
    }

    delay(400);
    lcd.clear();
    lcd.print("Eingabe: ");
    lcd.print(user_time_code[0]);
    lcd.print(user_time_code[1]);
    lcd.print(user_time_code[2]);
    delay(1000);

    if (atoi(user_time_code) < 300)
    {
      set_process = 4;

      rtc.adjust(DateTime(user_time.jahr, 1, 1, user_time.stunde, user_time.minute, 30) + TimeSpan(atoi(user_time_code), 0, 0, 0));
    }
    else
    {
      lcd.clear();
      lcd.println("TAGE ZU GROSS");
      delay(600);
    }

  } while (set_process == 3);

  lcd.clear();
  lcd.print("SUCCESS");
  lcd.setCursor(0, 1);
  lcd.print("Loading...");

  delay(2000);
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

/*void transmit_data(int state)
{
  if (state == 1)
  {
    mySwitch.send("000000000001010100010001"); // D AN
    mySwitch.send("000000000001010100010001");
    mySwitch.send("000000000001010100010001");
  }
  else if (state == 0)
  {
    mySwitch.send("000000000001010100010100"); // D AUS
    mySwitch.send("000000000001010100010100");
    mySwitch.send("000000000001010100010100");
  }
  return 0;

}*/