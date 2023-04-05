# Ferdinant

## Verbesserungs Ideen

- Idle D bearbeiten
- energie_spar_modus (für LCD_Display)
- 3D Modell für Box
- vielleicht mehr in Stunden und Minuten anzeigen statt Sekunden

- Fritzing steckplan
- Modi 2012 implementiern
- im reset menu waitForKey() durch andere Loesung ersetzen um Sicherheit zu erhoehen
- Modi int durch Konstanten auswechseln, für mehr übersichtlichkeit im Code


## **Resetablauf / Einstellen eines anderen Modus**

Da zwischen 2 Modis unterschieden werden muss, ist wichtig zu verstehen wie man den Modi richtig einstellt.

Nach dreimaligen druecken der Sternchen Taste   ` "*" `  auf dem Keypad, fragt das System, ob es einen Reset durch fuehren soll. Welches mit ` "#" ` bestaetigt werden kann, mit dem Druck einer anderen Taste wird das Menue automatischen wieder verlassen.



<pre><code>RESET MENU!
USE '#' 4 RESET
</code></pre>

Beachte, dass es moeglich ist zu jedem Zeitpunkt des Resetsablaufes, dass System kurz vom Strom zu entfernen um den Vorgang zu unterbrechen. Dadurch wird einfach der Zustand vor dem Resetablauf beibehalten.

### *Modi Auswahl*

Wird der Reset bestaetigt oeffnet sich, das Menue das fragt welcher Modi ausgewaehlt werden soll. Dazu muss ein 4-stelliger Code eingegebn werden.

<pre><code>Modi eingeben
Bsp. '2008'
</code></pre>
Modi Liste:
<pre><code>Half Season (3 Monate) = 2002
Full Season (6 Montat) = 2012
</code></pre>
Es tritt ein Fehler auf, wenn ein Code eingegben wird den das System nicht kennt. Danach wird man erneut angewiesen einen Code einzugeben.

### *Setzen der Uhrzeit*

Ist der Code eingegebn wird nun nach der Uhrzeit (Stunde und Minute) gefragt. Hier empfielt es sich die aktuelle Uhrzeit zu benutzen um die Steuerung der Tageszeit anzupassen. Natuerlich ist eine andere Uhrzeit auch moeglich. <br />
(Beacht: bei der eingabe einer einstelligen Zahl wird folgende Formatierung erwartet Bsp.: fuer 6 Uhr muesste **'06'** eingegeben werden, das selbe gilt fuer die Minuten).

<pre><code>Stunde eingeben
Bsp. '13'
</code></pre>




