#ifndef _sun_shine_time_
#define _sun_shine_time_

#include <stdint.h>
#include <RTClib.h>

uint32_t check_aufgang_sek(unsigned int days);                  // Bekommt die vergangen Tage übergeben und gibt die Aufgangsuhrzeit zurück

uint32_t check_untergang_sek(unsigned int days);                // Bekommt die vergangen Tage übergeben und gibt die Untergangsuhrzeit zurück

uint32_t get_lightseconds();                                    // Gibt die Sekunden die an dem Tag schon das Lich an ist wieder

uint32_t get_seconds_passed_daily();                            // Berechnet die verstrichenen Sekunden seit 0 Uhr.

uint32_t get_offset();                                          // Berechnet den Offset in bezug zu 1970 zu dem jeweiligen start datum der einstellung. Bsp sekunden von 1970 bis 2002

int check_status(uint32_t aufgang_sek, uint32_t untergang_sek); // gibt 1 f├╝r Licht muss an sein und 0 f├╝r Licht muss aus sein.

void secondsToTime(uint32_t seconds, char *timeString);         // Bekommt Sekunden als eingabe Wert und gibt sie ahls "HH:MM" format aus

void secondsToFullTime(uint32_t seconds, char *timeString);     // Bekommt Sekunden als eingabe Wert und gibt sie ahls "HH:MM:SS" format aus

void secondsToHour(uint32_t seconds, char *timeString);         // Bekommt Sekunden als eingabe Wert und gibt sie ahls "HH,H" format aus

void increaseDateTime(DateTime &dt, int days);                  // Erhöht das Datum einer DateTime Variablen um eine Anzahl von Tagen

#endif