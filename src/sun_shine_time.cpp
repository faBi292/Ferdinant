#include "sun_shine_time.h"
#include <RTClib.h>
#include <stdint.h>

const int HALF_SEASON = 2002;
const int FULL_SEASON = 2004;
const int TROPICAL = 2006;

RTC_DS3231 rtc;

uint32_t check_aufgang_sek(unsigned int days)
{
  static DateTime now;
  
  now = rtc.now();

  if (now.year() == HALF_SEASON)
  {
    return (uint32_t)(21600 + (5400 / 80) * (days - 1));
  }
  
  if (now.year() == FULL_SEASON)
  {
    return 0; // TODO
  }

  if (now.year() == TROPICAL) //Struktur muss noch getestet werden, gehe davon aus das er nach dem ersten return die Funktion verlässt
  {
    if (days < 10)
    {
      return 0;
    }
    else if (days < 20)
    {
      return 0;
    }
    else if (days < 30)
    {
      return 0;
    }
    else if (days < 40)
    {
      return 0;
    }
    else if (days < 50)
    {
      return 0;
    }
    else if (days < 60)
    {
      return 0;
    }
    else if (days < 80)
    {
      return 0;
    }
    else if (days < 90)
    {
      return 0;
    }
    else if (days < 100)
    {
      return 0;
    }
    else if (days >= 100)
    {
      return 0;
    }
  }

  return 0; //Falls keine der Funktionen greift
}

uint32_t check_untergang_sek(unsigned int days)
{
  static DateTime now;

  now = rtc.now();
  if (now.year() == HALF_SEASON)
  {
    return (uint32_t)(72000 - (5400 / 80) * (days - 1));
  }

  if (now.year() == FULL_SEASON)
  {
    return 0; // TODO
  }

  if (now.year() == TROPICAL) //Struktur muss noch getestet werden, gehe davon aus das er nach dem ersten return die Funktion verlässt
  {
    if (days < 10)
    {
      return 0;
    }
    else if (days < 20)
    {
      return 0;
    }
    else if (days < 30)
    {
      return 0;
    }
    else if (days < 40)
    {
      return 0;
    }
    else if (days < 50)
    {
      return 0;
    }
    else if (days < 60)
    {
      return 0;
    }
    else if (days < 80)
    {
      return 0;
    }
    else if (days < 90)
    {
      return 0;
    }
    else if (days < 100)
    {
      return 0;
    }
    else if (days >= 100)
    {
      return 0;
    }
  }

  return 0;
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

uint32_t get_seconds_passed_daily()
{
  static DateTime now;
  static uint32_t sekunden_passed_day;

  now = rtc.now();

  sekunden_passed_day = (now.unixtime() - get_offset()) % (86400); // 86400 = 60*60*24

  return sekunden_passed_day;
}

uint32_t get_offset()
{
  static DateTime now;

  now = rtc.now();

  if (now.year() == HALF_SEASON)
  {
    return 1009843200;
  }
  if (now.year() == FULL_SEASON)
  {
    return 1072915200;
  }
  if (now.year() == TROPICAL)
  {
    return 1136073600;
  }

  return 0; // Falls es das Jahr nicht mehr geben sollte, könnte passieren wenn die Uhr lange nicht benutzt wird.
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

void increaseDateTime(DateTime &dt, int days)
{
  dt = dt + TimeSpan(days, 0, 0, 0);
}




