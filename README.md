# AFD Recognizer (Deterministic Finite Automaton) — Python & C

**Repositorio listo para subir a GitHub**: implementaciones completas en **Python** y **C** para reconocer cadenas usando un AFD (Autómata Finito Determinista).
Este README está en español y contiene toda la documentación necesaria: formato de entrada, validaciones, uso, ejemplos, tests, integración básica con CI y licencia.

---

## Contenido del repositorio

- `afd_recognizer.py` — Implementación en **Python 3**.  
  - Soporta: validación de configuración, detección de no determinismo, ejecución de pruebas y opción de traza (muestra la ruta de estados por cada cadena).
  - Uso: `python3 afd_recognizer.py conf.txt cadenas.txt [--trace] [--check]`
- `afd_recognizer.c` — Implementación en **C** (C99).  
  - Uso: compilar con `gcc -std=c99 afd_recognizer.c -o afd_recognizer`
  - Ejecución similar: `./afd_recognizer conf.txt cadenas.txt`  
  - Nota: la versión C es robusta pero básica; se incluyen también instrucciones para ampliarla.
- `afd_recognizer_fixed.c` — Variante C compatible con compiladores antiguos (C89).
- `conf_example.txt` — Ejemplo de archivo de configuración (formato `conf.txt`).
- `cadenas_example.txt` — Ejemplo de archivo de pruebas (una cadena por línea).
- `README.md` — este archivo.
- `LICENSE` — Licencia MIT.
- `.github/workflows/ci.yml` — Ejemplo de workflow de GitHub Actions (ver sección *CI*).

---

## Objetivo

Proveer una herramienta simple y didáctica para:
- Cargar la definición de un AFD desde un archivo `conf.txt`.
- Leer un conjunto de cadenas desde `cadenas.txt`.
- Por cada cadena, imprimir `acepta` si el AFD la acepta, o `NO acepta` si no.
- Opcionalmente, verificar que la configuración sea **determinista** y mostrar la **traza** de estados (ruta) usada para reconocer cada cadena.

---

## Formato del archivo de configuración (`conf.txt`)

El archivo de configuración define el autómata. Es texto plano; las líneas que empiezan con `#` son comentarios y se ignoran. Las claves principales son:

```
states: q0,q1,q2
alphabet: a,b
start: q0
accept: q2,q3
transitions:
q0,a,q1
q0,b,q0
q1,a,q2
q1,b,q0
q2,a,q2
q2,b,q2
```

- `states:` — lista separada por comas de nombres de estados (identificadores).  
  - Los nombres pueden contener letras, dígitos y `_`, por ejemplo `q0`, `S_final`.  
- `alphabet:` — lista separada por comas de símbolos terminales (normalmente un solo carácter por símbolo).  
  - Ejemplo: `a,b` o `0,1`. Si se omite, el programa asume que el alfabeto se infiere de las transiciones y/o cadenas (pero se recomienda declararlo).
- `start:` — estado inicial (uno solo).  
- `accept:` — lista separada por comas de estados de aceptación (finales).  
- `transitions:` — a partir de esta línea, cada línea de transición tiene el formato `src,symbol,dst` (sin espacios obligatorios). Ejemplo: `q0,a,q1`.

**Notas importantes**
- El autómata debe ser **determinista**: para cada par `(estado, símbolo)` debe existir a lo sumo una transición. Si hay más de una transición con el mismo par, el programa puede detectar y reportar no determinismo.
- Si una transición falta para algún par `(estado,símbolo)`, la entrada se considera rechazada al encontrarse esa ausencia durante el procesamiento de la cadena.

---

## Formato del archivo de pruebas (`cadenas.txt`)

- Cada línea del archivo representa una cadena a probar.  
- Una línea vacía representa la cadena vacía (ε).  
- No usar comentarios en este archivo (mejor mantenerlo limpio).

Ejemplo:
```
ab
aab
aaabbb
a
b

```

---

## Uso — Python (recomendado para desarrollo)

### Requisitos
- Python 3.8+.

### Ejecutar
```bash
python3 afd_recognizer.py conf_example.txt cadenas_example.txt
```

### Opciones útiles
- `--trace` — Muestra la traza de estados para cada cadena (ejemplo: `q0 -a-> q1 -b-> q0`).  
  Ejemplo:
  ```bash
  python3 afd_recognizer.py conf_example.txt cadenas_example.txt --trace
  ```
- `--check` — Valida la configuración y reporta problemas (estados desconocidos, no determinismo, símbolos fuera del alfabeto).  
  ```bash
  python3 afd_recognizer.py conf_example.txt cadenas_example.txt --check
  ```
- Puedes combinar ambas opciones.

### Salida
- Para cada cadena, el programa imprime `acepta` o `NO acepta`. Si se pasa `--trace`, además muestra la ruta de estados utilizada.

---

## Uso — C

### Compilar
(recomendado usar un compilador moderno que soporte C99)
```bash
gcc -std=c99 afd_recognizer.c -o afd_recognizer
```
O, si tu compilador no soporta C99:
```bash
gcc afd_recognizer_fixed.c -o afd_recognizer
```

### Ejecutar
```bash
./afd_recognizer conf_example.txt cadenas_example.txt
```

El binario imprimirá `acepta` o `NO acepta` por cada línea de `cadenas.txt`.

---

## Validaciones que realiza el código (Python)

El script de Python realiza:
- Verificación de que `start` y `states` estén definidos y consistentes.  
- Verificación de que todas las transiciones referencien estados existentes (si no, se informa error).  
- Detección de no determinismo: si encuentra más de una transición para `(estado, símbolo)` se reporta un error (y, si se solicita `--check`, el programa termina con código de error).  
- Verificación de símbolos desconocidos frente al `alphabet` (si `alphabet` fue declarado).

La versión en C realiza validaciones básicas (estados y transiciones), aunque su verificación de no determinismo es menos detallada en la salida — puedes mejorarla clonando la lógica mostrada en Python.

---

## Trazas y depuración

- `--trace` (Python) muestra por cada símbolo procesado la transición aplicada y el estado resultante.  
- En C se provee un cuerpo principal compacto; para traza, puedes modificar `afd_recognizer.c` añadiendo prints dentro del bucle de consumo de símbolos (se indican comentarios en el código).

---

## Ejemplos prácticos

**conf_example.txt**
```
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
```

**cadenas_example.txt**
```
ab
aab
abab
b
(línea vacía) <-- representa ε
```

Ejecutar:
```bash
python3 afd_recognizer.py conf_example.txt cadenas_example.txt --trace
```

Resultado esperado (ejemplo ilustrativo):
```
ab -> acepta
trace: q0 -a-> q1 -b-> q0
aab -> NO acepta
trace: q0 -a-> q1 -a-> q2 -b-> q2
...
```

---

## Tests automáticos (sugerencia)

Puedes añadir un pequeño script (por ejemplo `tests/run_tests.sh`) que ejecute el programa contra varios `conf`/`cadenas` y compare salidas esperadas. En CI, ejecutar estos tests garantiza que no se rompa la funcionalidad.

---

## GitHub Actions — CI (ejemplo)

Añade el siguiente workflow en `.github/workflows/ci.yml` para ejecutar tests básicos en Ubuntu con Python y GCC:

```yaml
name: CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.10'
      - name: Run Python linter
        run: python -m pyflakes afd_recognizer.py || true
      - name: Run Python example
        run: python3 afd_recognizer.py conf_example.txt cadenas_example.txt
      - name: Build C
        run: gcc -std=c99 afd_recognizer.c -o afd_c
      - name: Run C example
        run: ./afd_c conf_example.txt cadenas_example.txt
```

---

## Buenas prácticas para el repo

- Incluye `LICENSE` (ya incluido en este repo).  
- Añade `README.md` claro (este).  
- Añade ejemplos y tests en una carpeta `examples/` o `tests/`.  
- Define un CONTRIBUTING.md si quieres aceptar PRs.  
- Añade CI que ejecute pruebas y linters.

---

## Contribuciones

Si quieres que mejore:
- Añadir `--trace` también en la versión en C (lo hago si lo pides).  
- Exportar la función de validación como script separado (`check_conf.py`).  
- Incluir un modo interactivo (preguntar cadenas en stdin).  
- Añadir conversión desde JSON/CSV para la conf.

---

## Licencia

Este proyecto está bajo la **Licencia MIT** (archivo `LICENSE` incluido).
