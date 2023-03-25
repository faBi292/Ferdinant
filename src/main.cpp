
#include <Keypad.h>

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

void set_usertime();

void setup()
{
  Serial.begin(9600); // Initialise the serial monitor

  set_usertime(); //Neues Programmstarten holt dafür vom nutzer die Infos
}

void loop()
{
  // Read the pushed button
  char button = customKeypad.getKey();

  if (button)
  {
    Serial.println(button);
  }
}

void set_usertime()
{
  int set_process = 0; //Zaehler Variable
  time_struct user_time; //Defeniert neuen Zeit Speicher

  do
  {
    Serial.println("Modi code eingeben bsp. '2008'");

    char user_time_code[5] = "2008";

    for (int i = 0; i < 4; i++)
    {
      user_time_code[i] = customKeypad.waitForKey();
    }

    Serial.println(String("Eingabe: ") + user_time_code[0] + user_time_code[1] + user_time_code[2] + user_time_code[3]);

    if (atoi(user_time_code) == 2002) //hier muss Funktion kommen die Checkt, ob der ModiCode auch wirklich  einen Modi hat.
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

  Serial.println("SUCCES");
}
