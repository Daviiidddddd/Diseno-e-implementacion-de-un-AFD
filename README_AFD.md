# AFD Recognizer — README

Archivos incluidos:
- afd_recognizer.py         Python implementation
- afd_recognizer.c          C implementation (C99)
- afd_recognizer_fixed.c    C implementation (C89-compatible)
- conf_example.txt          ejemplo de configuración de AFD
- cadenas_example.txt       ejemplo de cadenas de prueba

Formato de conf.txt (ejemplo):

states: q0,q1,q2
alphabet: a,b
start: q0
accept: q2
transitions:
q0,a,q1
q0,b,q0
q1,a,q2
q1,b,q0
q2,a,q2
q2,b,q2

Format for cadenas.txt: one test string per line. Empty line represents epsilon (empty string).

Uso (Python):
python3 afd_recognizer.py conf_example.txt cadenas_example.txt

Uso (C):
gcc -std=c99 afd_recognizer.c -o afd_recognizer
./afd_recognizer conf_example.txt cadenas_example.txt
