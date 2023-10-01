# Ferdinant©️ 
-------------------
## **IDLE Screens** 
Es gibt 4 Anzeigen die mit den A,B,C und D ausgewählt werden können.

### **IDLE A**

<pre><code>Uhrzeit: hh:mm:ss
Tag: XX   LH:hh,h
</code></pre>
- "Uhrzeit", zeigt die interne Uhrzeit der Steuerung.
- "Tag", zeigt den aktuellen Tag des Modi, startet bei Tag 1.
- "LH", steht für Light Hours und gibt an wie viele Stunden Licht an diesem Tag schon vergangen sind.

### **IDLE B**

<pre><code>Rising: hh:mm:ss
Dawn  : hh:mm:ss
</code></pre>
- "Rising", gibt an wann das nächste mal das Licht angeschaltet wird.
- "Dawn", gibt an wann das Licht das nächste mal ausgeschaltet wird.

### **IDLE C**

<pre><code>Cycle: "MODE"
"MODE NAME"   "Days"
</code></pre>

- "MODE", gibt an welcher Modus gerade aktiv ist. (2002, 2004, 2006)
- "Mode Name", gibt den Namen des aktuellen Modus zurück (Half_Season, Full_season, Tropical)
- "Days", gibt an wie viele Tage der Modi benötigt.

### **IDLE D**

<pre><code>Glow: "%"

</code></pre>

"Glow", gibt an wie groß die Lichtintensität des Lichtsensor ist. 
ℹ️ Das Licht gilt als an bei über 80% und als aus bei 30%, dazwischen ist eine Grauzone/Puffer und wechselt erst wenn die Schwelle über/unterschritten wurde.
  
-------------------

## **LED's**

| Farbe | Bedeutung |
| ------ | ----------- |
| 🔵 Blau   | Leuchtet wenn Steckdose "An" sein soll und ist aus wenn Steckdose "Aus" sein soll. |
| 🟡 Gelb  | Leuchtet auf wenn gefunkt wird. |
| 🔴 Rot    | Leuchtet wenn Fehler erkannt wird, unzwar wenn Lichtsensor nicht mit dem internen Wert übereinstimmt.  |

--------------------

## **Modus Auswahl**

Da zwischen 3 Modis unterschieden werden muss, ist wichtig zu verstehen wie man den Modi richtig einstellt.

Nach dreimaligen druecken der Sternchen Taste   ` "*" `  auf dem Keypad, fragt das System, ob es einen Reset durch fuehren soll. Welches mit ` "#" ` bestaetigt werden kann, mit dem Druck einer anderen Taste wird das Menue automatischen wieder verlassen.



<pre><code>RESET MENU!
USE '#' 4 RESE
</code></pre>

Beachte, dass es moeglich ist zu jedem Zeitpunkt des Resetsablaufes, dass System kurz vom auszuschalten um den Vorgang zu unterbrechen. Dadurch wird einfach der Zustand vor dem Modi Auswahl beibehalten.

:warning: Beachtet werden muss, dass während man in der Auswahl ist, dass System nicht die Steckdose steuern kann.


### **Jahr einstellen**

Wird der Reset bestaetigt oeffnet sich, das Menue das fragt welcher Modi ausgewaehlt werden soll. Dazu muss ein 4-stelliger Code eingegebn werden.Es tritt ein Fehler auf, wenn ein Code eingegben wird den das System nicht kennt. Danach wird man erneut angewiesen einen Code einzugeben.

| Modi-Code | Modi-Bedeutung |
| ---------- | ----------- |
| 2002  | Half Season (80 Tage) |
| 2004  | Full Season (120 Tage) |
| 2006  | Tropical (100 Tage)  |

### **Setzen der Uhrzeit**

Ist der Code eingegebn wird nun nach der Uhrzeit (Stunde und Minute) gefragt. Hier empfielt es sich die aktuelle Uhrzeit zu benutzen um die Steuerung der Tageszeit anzupassen. Natuerlich ist eine andere Uhrzeit auch moeglich. <br />
(Beacht: bei der eingabe einer einstelligen Zahl wird folgende Formatierung erwartet Bsp.: fuer 6 Uhr muesste **'06'** eingegeben werden, das selbe gilt fuer die Minuten).

<pre><code>Stunde eingeben
Bsp. '13'
</code></pre>

### Setzen des Tages

Nun kann eingestellt werden wie viele, Tage übersprugnen werden sollen. 
<pre><code>Tage skippen
Bsp. '023'
</code></pre>
Bsp. um am Tag 8 anzufangen muss die Zahl **'007'** eingegeben werden.

---------
# **Schaltzyklen**
ℹ️ Sobald der Zyklus über sein Ende hinaus verwendet wird, benutzt er seine letzten Schaltzeiten die ganze Zeit weiter.

![Screenshot 2023-10-01 151442](https://github.com/faBi292/Ferdinant/assets/128408093/40d7a6c4-8387-4b13-ab67-f5a3f7043f9b)

---------
# **Schaltplan**
![IMG_0281](https://github.com/faBi292/Ferdinant/assets/128408093/43a7783e-2ffe-4670-a9e3-5ab0416d7f00)
---------
# **Code spezifische Notizen**
## Tri-State Codes
<pre><code>AN = mySwitch.sendTriState("00000FFF0F0F");
</code></pre>
<pre><code>AUS = mySwitch.sendTriState("00000FFF0FF0");
</code></pre>

# Bauteile

| Komponente | Modell/Bezeichnung |
| ---------- | ----------- |
| Steckdose  | RCS 1000 N Comfort (Mx. 1000 Watt) |
| Uhr  | RTC_DS3231 |
| Sender  | 433_MHz Transmitter |
| Board  | 16Mhz NANO V 3,0 Atmega328 |
| Display  | LCD1602 |
| Keypad  | 4x4 Matrix Tastatur  |

