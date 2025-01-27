Ich habe die Varianten Fixed, Variable(SIMD), und HashMap(Global) implementiert. 
Es gibt jeweils eine ausführbare Datei für jede Variante unter ./build/{fixed,hashmap,variable}.
In ./build/main werden alle 3 Varianten hintereinander ausgeführt, was aber für die Result-Ausgabe
vermutlich wenig Sinn macht.

Das Script build.sh baut das Projekt.

Der Source-Code zu den Tries befindet sich in ./src/trie.h. 
./src/util.h enthält Utility-Funktion für I/O.
Für Memory-Counting wird malloc_count verwendet.