# Ferdinant

## Verbesserungs Ideen

- im reset menu waitForKey() durch andere L”sung ersetzen um Sicherheit zu erh”hen
- Idle D bearbeiten
- Modi 2012 implementiern
  


## **Resetablauf / Einstellen eines anderen Modus**

Da zwischen 2 Modis unterschieden werden muss, ist wichtig zu verstehen wie man den Modi richtig einstellt.

Nach dreimaligen drcken der Sternchen Taste   ` "*" `  auf dem Keypad, fragt das System, ob es einen Reset durch fhren soll. Welches mit ` "#"  ` best„tigt werden kann, mit dem Druck einer anderen Taste wird das Men automatischen wieder verlassen.



<pre><code>RESET MENU!
USE '#' 4 RESET
</code></pre>

Beachte, dass es m”glich ist zu jedem Zeitpunkt des Resetsablaufes, dass System kurz vom Strom zu entfernen um den Vorgang zu unterbrechen. Dadurch wird einfach der Zustand vor dem Resetablauf beibehalten.

### *Modi Auswahl*

Wird der Reset best„tigt ”ffnet sich, das Men das fragt welcher Modi ausgew„hlt werden soll. Dazu muss ein 4-stelliger Code eingegebn werden.

<pre><code>Modi eingeben
Bsp. '2008'
</code></pre>
Modi Liste:
<pre><code>Half Season (3 Monate) = 2002
Full Season (6 Montat) = 2012
</code></pre>
Es tritt ein Fehler auf, wenn ein Code eingegben wird den das System nicht kennt. Danach wird man erneut angewiesen einen Code einzugeben.

### *Setzen der Uhrzeit*

Ist der Code eingegebn wird nun nach der Uhrzeit (Stunde und Minute) gefragt. Hier empfielt es sich die aktuelle Uhrzeit zu benutzen um die Steuerung der Tageszeit anzupassen. Natrlich ist eine andere Uhrzeit auch m”glich. <br />
(Beacht: bei der eingabe einer einstelligen Zahl wird folgende Formatierung erwartet Bsp.: fr 6 Uhr msste **'06'** eingegeben werden, das selbe gilt fr die Minuten).

<pre><code>Stunde eingeben
Bsp. '13'
</code></pre>




