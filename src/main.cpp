
#include <Keypad.h>
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>

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

void set_usertime();                                            // Ermöglicht den Benutzer seine Einstellung vorzunehmen.
void print_time();                                              // Printed die aktuelle Uhrzeit der Uhr
void print_time_passed();                                       // Printed die verstrichene Zeit seit anfang der Einstellung.
unsigned int get_days_passed();                                 // Berechnet die verstrichenen Tage seit anfang der Einstellung.
uint32_t get_seconds_passed_daily();                            // Berechnet die verstrichenen Sekunden seit 0 Uhr.
int check_status(uint32_t aufgang_sek, uint32_t untergang_sek); // gibt 1 für Licht muss an sein und 0 für Licht muss aus sein.
uint32_t get_offset();                                          // Berechnet den Offset in bezug zu 1970 zu dem jeweiligen start datum der einstellung. Bsp sekunden von 1970 bis 2002
void increaseDateTime(DateTime &dt, int days);                  // Erhöht das Datum einer DateTime Variablen um eine Anzahl von Tagen
void check_star();                                              // Überprüft ob die Sternchentaste gedrückt geahlten wird, nach 3 Sekunden führt sie dann set_usertime() aus.

void setup()
{
  Serial.begin(57600); // Initialise the serial monitor
  Wire.begin();
  rtc.begin();

  while (!Serial)
    ;

  if (!rtc.begin())
  {
    Serial.println("RTC konnte nicht gefunden werden!");
    while (1)
      ;
  }

  customKeypad.setHoldTime(3000);

  // set_usertime(); // Neues Programmstarten holt dafür vom nutzer die Infos
  DateTime dt1(2002, 1, 1, 0, 0, 0);
  uint32_t start_2002 = dt1.unixtime();
  Serial.println(start_2002);
}

void loop()
{

  check_star();

  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000)
  {
    lastTime = currentTime;
    Serial.println(get_days_passed());
    print_time();

    // print_time_passed();
    // Serial.println(get_seconds_passed_daily());
  }

  //  Read the pushed button
  /*char button = customKeypad.getKey();

  if (button)
  {
    Serial.println(button);
  }*/
}

void check_star()
{
  static uint8_t counter = 0;
  static unsigned long lasttime = 0;

  //Serial.println(millis() - lasttime);

  if(millis() - lasttime  >= 1000){
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
    Serial.println("Einstellung aendern");
    Serial.println("Drücke # um löschen zu bestätigen");

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
    Serial.println("Modi code eingeben bsp. '2008'");

    char user_time_code[5] = "2008";

    for (int i = 0; i < 4; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
    }

    Serial.println(String("Eingabe: ") + user_time_code[0] + user_time_code[1] + user_time_code[2] + user_time_code[3]);

    if (atoi(user_time_code) == 2002) // hier muss Funktion kommen die Checkt, ob der ModiCode auch wirklich  einen Modi hat.
    {
      set_process = 1;
      user_time.jahr = atoi(user_time_code);
    }
    else
    {
      Serial.println("CODE NICHT ERKANNT");
      delay(400);
    }

  } while (set_process == 0);

  // zweite Schleife für Stunde
  do
  {
    Serial.println("Stunde Eingeben bsp'13'");

    char user_time_code[3] = "13";

    for (int i = 0; i < 2; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
    }

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
    }

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
    Serial.println("Tage eingeben Bsp. '000'");

    char user_time_code[4] = "000";

    for (int i = 0; i < 3; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
    }

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
  static DateTime now;
  static uint32_t sekunden_passed_day;

  now = rtc.now();

  sekunden_passed_day = (now.unixtime() - get_offset()) % (86400); // 86400 = 60*60*24

  if (sekunden_passed_day > aufgang_sek && sekunden_passed_day < untergang_sek)
  {
    return 1;
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
