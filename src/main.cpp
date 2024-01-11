#include <Arduino.h>
#include <Keypad.h>
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RCSwitch.h>

#define Red_LED_PIN 2
#define Blue_LED_PIN 3
#define RC_PIN 4

RTC_DS3231 rtc;
RCSwitch mySwitch;

const int HALF_SEASON = 2000;
const int FULL_SEASON = 2001;
const int TROPICAL = 2002;
const int VEGGIE = 2003;
const int PRODUCTION = 2004;
const int SHORT_VEGGIE = 2005;

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns

// Map the buttons to an array for the Keymap instance
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte colPins[ROWS] = {5, 6, 7, 8};    // Pins used for the rows of the keypad
byte rowPins[COLS] = {9, 10, 11, 12}; // Pins used for the columns of the keypad

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

// const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = 10, d7 = 11;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address, number of columns and rows

enum zustaende
{
  Knopf_A,
  Knopf_B,
  Knopf_C,
  Knopf_D,
};

int LICHT_CHECK_intern; // Sagt aus ob Licht an sein muss
int LICHT_CHECK_extern; // Sagt aus ob Licht gerade an ist via. Photosensor
unsigned days_passed;   // Vergangene Tage beginnend bei 1
char getKey_puffer;     // zuletzt eingegebene Taste wird hier gespeichert

void set_usertime();                                            // Erm├╢glicht den Benutzer seine Einstellung vorzunehmen.
void print_time();                                              // Printed die aktuelle Uhrzeit der Uhr
void print_time_passed();                                       // Printed die verstrichene Zeit seit anfang der Einstellung.
unsigned int get_days_passed();                                 // Berechnet die verstrichenen Tage seit anfang der Einstellung.
uint32_t get_seconds_passed_daily();                            // Berechnet die verstrichenen Sekunden seit 0 Uhr.
int check_status(uint32_t aufgang_sek, uint32_t untergang_sek); // gibt 1 f├╝r Licht muss an sein und 0 f├╝r Licht muss aus sein.
uint32_t get_offset();                                          // Berechnet den Offset in bezug zu 1970 zu dem jeweiligen start datum der einstellung. Bsp sekunden von 1970 bis 2002
void increaseDateTime(DateTime &dt, int days);                  // Erh├╢ht das Datum einer DateTime Variablen um eine Anzahl von Tagen
void check_star();                                              // ├£berpr├╝ft ob die Sternchentaste gedr├╝ckt geahlten wird, nach 3 Sekunden f├╝hrt sie dann set_usertime() aus.
uint32_t check_aufgang_sek(unsigned int days);                  // Bekommt die vergangen Tage ├╝bergeben und gibt die Aufgangsuhrzeit zur├╝ck
uint32_t check_untergang_sek(unsigned int days);                // Bekommt die vergangen Tage ├╝bergeben und gibt die Untergangsuhrzeit zur├╝ck
void display_idle(char Knopf);                                  // Bekommt A,B,C,D ├╝bergebn und wechselt dann in den entsprechenden Modi
void check_Knopf();                                             // ├£berpr├╝ft ob Knopf gedr├╝ckt wurde und ruft entsprechend die Funktion auf
void secondsToTime(uint32_t seconds, char *timeString);         // Bekommt Sekunden als eingabe Wert und gibt sie ahls "HH:MM" format aus
void secondsToFullTime(uint32_t seconds, char *timeString);     // Bekommt Sekunden als eingabe Wert und gibt sie ahls "HH:MM:SS" format aus
void secondsToHour(uint32_t seconds, char *timeString);         // Bekommt Sekunden als eingabe Wert und gibt sie ahls "HH,H" format aus
float read_LichtSensor();                                       // Gibt die Aktuelle Lichtintensität in % zurück
uint32_t get_lightseconds();                                    // Gibt die Sekunden die an dem Tag schon das Lich an ist wieder

void init_RCSwitch(int PIN, int PulseLength);
void steckdose_on(int state); // Kann Steckdose ein und ausschalten 1:EIN / 0:AUS

void lcd_energiesparen(int timer_reset); // Steuert die Hitnergrundbeleuchtung
int licht_check_photosensor();

// void transmit_data(int state);

void setup()
{
  Serial.begin(9600); // Initialise the serial monitor

  lcd.init();
  lcd_energiesparen(1);

  lcd.print("Loading");
  delay(300);
  lcd.print(".");
  delay(300);
  lcd.print(".");
  delay(300);
  lcd.print(".");
  delay(300);

  Wire.begin();
  rtc.begin();

  init_RCSwitch(RC_PIN, 324);

  if (!rtc.begin())
  {
    lcd.clear();
    lcd.println("RTC not found!");
    while (1)
      ;
  }

  pinMode(Red_LED_PIN, OUTPUT);  // Rote LED
  pinMode(Blue_LED_PIN, OUTPUT); // Blaue LED

  pinMode(A0, INPUT); // Licht_Sensor

  /*digitalWrite(Red_LED_PIN, HIGH);
  for (int i = 1; i < 121; i++)
  {
    Serial.println((String)i + ":" + check_aufgang_sek(i) + ":" + check_untergang_sek(i));
  }*/
  // DateTime dt1(2006, 1, 1, 0, 0, 0); //Findet den Offset raus
  // Serial.println(dt1.unixtime());
}

void loop()
{
  static unsigned long lastTime_1000 = 0;
  static unsigned long lastTime_30000 = 0;

  check_Knopf();

  if (millis() - lastTime_1000 >= 1000)
  {

    lastTime_1000 = millis(); // setzt Schleife zur├╝ck

    days_passed = get_days_passed();

    LICHT_CHECK_intern = check_status(check_aufgang_sek(days_passed), check_untergang_sek(days_passed)); // 6.5ms
    analogWrite(Blue_LED_PIN, LICHT_CHECK_intern * 30);
    LICHT_CHECK_extern = licht_check_photosensor();

    static boolean status = !LICHT_CHECK_intern; // Gibt an ob die Initialsendung nach Switch schon erfolgt ist
    if (status != LICHT_CHECK_intern)
    {
      status = LICHT_CHECK_intern;
      steckdose_on(LICHT_CHECK_intern);
    }

    if (LICHT_CHECK_extern != LICHT_CHECK_intern)
    {
      digitalWrite(Red_LED_PIN, HIGH);
      steckdose_on(LICHT_CHECK_intern);
    }
    else
    {
      digitalWrite(Red_LED_PIN, LOW);
    }

    lcd_energiesparen(0); // Aktualisiert die energiespar Funktion

    display_idle(' '); // 9ms
  }

  if (millis() - lastTime_30000 >= 30000) // Setzt alle 30 Sekunden Signal raus
  {
    lastTime_30000 = millis(); // setzt Schleife zur├╝ck

    if (LICHT_CHECK_intern)
    {
      steckdose_on(1);
    }
    else
    {
      steckdose_on(0);
    }
  }
}

void check_Knopf()
{
  getKey_puffer = customKeypad.getKey();

  if (getKey_puffer)
  {
    lcd_energiesparen(1);
  }
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
  else if (getKey_puffer == '0')
  {
    steckdose_on(0);
  }
  else if (getKey_puffer == '1')
  {
    steckdose_on(1);
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
    else if (now.year() == VEGGIE)
    {
      lcd.print("Veggie       18h ");
    }
    else if (now.year() == PRODUCTION)
    {
      lcd.print("Production   12h ");
    }
    else if (now.year() == SHORT_VEGGIE)
    {
      lcd.print("Short Veg.   16h ");
    }
    else
    {
      lcd.print("NO MODE          ");
    }

    break;

  case Knopf_D:

    static float brightness_precent;

    brightness_precent = read_LichtSensor();

    lcd.setCursor(0, 0);
    lcd.print((String) "Glow: " + brightness_precent + "%           ");
    lcd.setCursor(0, 1);

    lcd.print((String) "                         ");

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
    else if (atoi(user_time_code) == VEGGIE)
    {
      set_process = 1;
      user_time.jahr = atoi(user_time_code);
      lcd.setCursor(0, 1);
      lcd.print("VEGGIE");
      delay(2000);
    }
    else if (atoi(user_time_code) == PRODUCTION)
    {
      set_process = 1;
      user_time.jahr = atoi(user_time_code);
      lcd.setCursor(0, 1);
      lcd.print("PRODUCTION");
      delay(2000);
    }
    else if (atoi(user_time_code) == SHORT_VEGGIE)
    {
      set_process = 1;
      user_time.jahr = atoi(user_time_code);
      lcd.setCursor(0, 1);
      lcd.print("SHORT_VEGGIE");
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

  lcd_energiesparen(1);
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
{ //TODO Check if Offsets are correct
  static DateTime now;

  now = rtc.now();

  if (now.year() == HALF_SEASON)
  {
    return 946684800;
  }
  if (now.year() == FULL_SEASON)
  {
    return 978307200;
  }
  if (now.year() == TROPICAL)
  {
    return 1009843200;
  }
  if (now.year() == VEGGIE)
  {
    return 1041379200;
  }
  if (now.year() == PRODUCTION)
  {
    return 1072915200;
  }
  if (now.year() == SHORT_VEGGIE)
  {
    return 1104537600;
  }

  return 0; // Falls es das Jahr nicht mehr geben sollte, k├╢nnte passieren wenn die Uhr lange nicht benutzt wird.
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

  if (now.year() == HALF_SEASON)
  {
    if (days > 80)
    {
      return (uint32_t)(27000);
    }
    return (uint32_t)(21600 + (5400 / 79.0) * (days - 1)); // Von 6:00 -> 7:30
  }

  if (now.year() == FULL_SEASON)
  {
    if (days > 120)
    {
      return (uint32_t)(27000);
    }
    return (uint32_t)(18000 + (9000 / 119.0) * (days - 1)); // Von 5:00 -> 7:30
  }

  if (now.year() == TROPICAL) 
  {
    if (days <= 10)
    {
      return (uint32_t)(21600);
    }
    else if (days <= 20)
    {
      return (uint32_t)(21600 + (1800 / 10) * (days - 10));
    }
    else if (days <= 40)
    {
      return (uint32_t)(23400 + (1800 / 20) * (days - 20));
    }
    else if (days <= 80)
    {
      return (uint32_t)(25200);
    }
    else if (days <= 100)
    {
      return (uint32_t)(25200 + (1800 / 20) * (days - 80));
    }
    else if (days > 100)
    {
      return (uint32_t)(27000);
    }
  }

  if (now.year() == VEGGIE)
  {
    return (uint32_t)(18000); // 5:00 Uhr
  }

  if (now.year() == PRODUCTION)
  {
    return (uint32_t)(28800); // 8:00 Uhr
  }

  if (now.year() == SHORT_VEGGIE)
  {
    return (uint32_t)(21600); // 6:00 Uhr
  }

  return 0;

}

uint32_t check_untergang_sek(unsigned int days)
{
  static DateTime now;

  now = rtc.now();

  if (now.year() == STATIC)
  {
    return (uint32_t)(64800); // 18 Uhr
  }

  if (now.year() == HALF_SEASON)
  {
    if (days > 80)
    {
      return (uint32_t)(66600);
    }
    return (uint32_t)(72000 - (5400 / 79.0) * (days - 1)); // Von 20:00 -> 18:30
  }

  if (now.year() == FULL_SEASON)
  {
    if (days > 120)
    {
      return (uint32_t)(66600);
    }
    return (uint32_t)(75600 - (9000 / 119.0) * (days - 1)); // Von 21:00 -> 18:30
  }

  if (now.year() == TROPICAL)
  {
    if (days <= 10)
    {
      return (uint32_t)(68400);
    }
    else if (days <= 20)
    {
      return (uint32_t)(68400 - (1800 / 10) * (days - 10));
    }
    else if (days <= 40)
    {
      return (uint32_t)(66600 - (1800 / 20) * (days - 20));
    }
    else if (days <= 80)
    {
      return (uint32_t)(64800);
    }
    else if (days <= 100)
    {
      return (uint32_t)(64800 - (1800 / 20) * (days - 80));
    }
    else if (days > 100)
    {
      return (uint32_t)(63000);
    }
  }

  if (now.year() == VEGGIE)
  {
    return (uint32_t)(82800); // 23:00 Uhr
  }

  if (now.year() == PRODUCTION)
  {
    return (uint32_t)(72000); // 20:00 Uhr
  }

  if (now.year() == SHORT_VEGGIE)
  {
    return (uint32_t)(79200); // 22:00 Uhr
  }

  return 0;
}

void secondsToTime(uint32_t seconds, char *timeString)
{
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  sprintf(timeString, "%02d:%02d", hours, minutes);
}

void secondsToFullTime(uint32_t seconds, char *timeString)
{
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int sec = seconds % 60;
  sprintf(timeString, "%02d:%02d:%02d", hours, minutes, sec);
}

void secondsToHour(uint32_t seconds, char *timeString)
{
  float hours = (float)seconds / 3600.0;
  int wholeHours = (int)hours;
  int decimalHours = (int)(hours * 10) % 10;
  sprintf(timeString, "%02d.%01d", wholeHours, decimalHours);
}

uint32_t get_lightseconds()
{
  static uint32_t days;

  days = get_days_passed();

  if (get_seconds_passed_daily() > check_aufgang_sek(days))
  {
    if (get_seconds_passed_daily() < check_untergang_sek(days))
    {
      return (get_seconds_passed_daily() - check_aufgang_sek(days)); // Wenn noch TAG
    }
    else
    {
      return (check_untergang_sek(days) - check_aufgang_sek(days)); // Bei NACHT
    }
  }
  else
  {
    return 0; // AM Morgen
  }
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

void init_RCSwitch(int PIN, int PulseLength)
{
  mySwitch = RCSwitch();
  mySwitch.enableTransmit(PIN);
  mySwitch.setPulseLength(PulseLength);
  // mySwitch.setProtocol(1);
}

void steckdose_on(int state)
{
  if (state == 1)
  { // Steckdose Einschalten
    mySwitch.sendTriState("00000FFF0F0F");
    // mySwitch.send(5393, 24);
  }
  else
  { // Steckdose Ausschalten
    mySwitch.sendTriState("00000FFF0FF0");
    // mySwitch.send(5396,24);
  }
}

void lcd_energiesparen(int timer_reset)
{
  static uint16_t last_millis = 0;
  static uint16_t delta = 0;

  if (timer_reset == 1)
  {
    last_millis = millis();
    lcd.backlight();
  }
  delta = millis() - last_millis;

  if (delta > 30000) // Macht nach 30 Sekunden wieder die Hintergrundbeleuchtung AUS
  {
    lcd.noBacklight();
  }
}
float read_LichtSensor()
{
  return map(analogRead(A0), 0, 1023, 100.0, 0.0);
}

int licht_check_photosensor()
{
  static float puffer;
  puffer = read_LichtSensor();
  if (puffer > 80)
  {
    return 1;
  }
  else if (puffer < 30)
  {
    return 0;
  }
}