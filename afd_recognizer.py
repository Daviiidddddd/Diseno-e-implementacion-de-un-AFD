#!/usr/bin/env python3
"""AFD recognizer (Python)
Usage: python3 afd_recognizer.py conf.txt cadenas.txt
Output: for each line in cadenas.txt prints 'acepta' or 'NO acepta'
"""
import sys

def parse_conf(path):
    conf = {'states': [], 'alphabet': [], 'start': None, 'accept': [], 'transitions': []}
    with open(path, encoding='utf-8') as f:
        lines = [ln.strip() for ln in f if ln.strip() and not ln.strip().startswith('#')]
    i = 0
    while i < len(lines):
        ln = lines[i]
        if ln.startswith('states:'):
            conf['states'] = [s.strip() for s in ln.split(':',1)[1].split(',') if s.strip()]
        elif ln.startswith('alphabet:'):
            conf['alphabet'] = [s.strip() for s in ln.split(':',1)[1].split(',') if s.strip()]
        elif ln.startswith('start:'):
            conf['start'] = ln.split(':',1)[1].strip()
        elif ln.startswith('accept:'):
            conf['accept'] = [s.strip() for s in ln.split(':',1)[1].split(',') if s.strip()]
        elif ln.startswith('transitions:'):
            # transitions follow in subsequent lines until next key or EOF
            i += 1
            while i < len(lines) and (',' in lines[i]):
                parts = [p.strip() for p in lines[i].split(',')]
                if len(parts) != 3:
                    raise ValueError(f"Linea de transicion invalida: {lines[i]}")
                src, sym, dst = parts
                conf['transitions'].append((src, sym, dst))
                i += 1
            continue
        else:
            # unknown line -- ignore or error
            pass
        i += 1
    # Basic validation
    if not conf['states'] or conf['start'] is None:
        raise ValueError('Archivo de configuracion incompleto: falta states o start')
    return conf

def build_transition_table(conf):
    states = conf['states']
    idx = {s:i for i,s in enumerate(states)}
    n = len(states)
    # transitions: dict (state_idx, symbol) -> dst_idx
    trans = {}
    for (src, sym, dst) in conf['transitions']:
        if src not in idx or dst not in idx:
            raise ValueError(f'Transicion usa estado desconocido: {src} -> {dst}')
        # symbols: allow multi-char but we expect single characters in input; we'll treat sym as exact token (usually 1 char)
        trans[(idx[src], sym)] = idx[dst]
    alphabet = conf['alphabet']
    accept = set(conf['accept'])
    return idx, trans, alphabet, conf['start'], accept

def run(conf_path, tests_path):
    conf = parse_conf(conf_path)
    idx_map, trans, alphabet, start, accept_set = build_transition_table(conf)
    start_idx = idx_map[start]
    accept_idx_set = set()
    for a in accept_set:
        if a in idx_map:
            accept_idx_set.add(idx_map[a])
    with open(tests_path, encoding='utf-8') as f:
        for raw in f:
            line = raw.rstrip('\n').rstrip('\r')
            # empty line -> empty string
            s = line
            cur = start_idx
            ok = True
            if s == '':
                # empty string: accept only if start is accept
                ok = (cur in accept_idx_set)
            else:
                for ch in s:
                    # symbol matching: we expect alphabet entries to be single characters (or tokens).
                    symbol = ch
                    # check symbol in alphabet (if alphabet is defined)
                    if alphabet and symbol not in alphabet:
                        ok = False
                        break
                    key = (cur, symbol)
                    if key not in trans:
                        ok = False
                        break
                    cur = trans[key]
                if ok:
                    ok = (cur in accept_idx_set)
            print('acepta' if ok else 'NO acepta')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Uso: python3 afd_recognizer.py conf.txt cadenas.txt')
        sys.exit(1)
    run(sys.argv[1], sys.argv[2])
