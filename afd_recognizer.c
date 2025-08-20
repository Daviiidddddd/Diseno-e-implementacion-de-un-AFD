// AFD recognizer (C)
// Compile: gcc -std=c99 afd_recognizer.c -o afd_recognizer
// Usage: ./afd_recognizer conf.txt cadenas.txt
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STATES 256
#define MAX_NAME 64
#define MAX_LINE 1024

char states[MAX_STATES][MAX_NAME];
int n_states = 0;

int get_state_idx(const char *name) {
    for (int i = 0; i < n_states; i++) {
        if (strcmp(states[i], name) == 0) return i;
    }
    return -1;
}

void add_state(const char *name) {
    if (get_state_idx(name) == -1) {
        if (n_states >= MAX_STATES) { fprintf(stderr, "Too many states\n"); exit(1); }
        strncpy(states[n_states], name, MAX_NAME-1);
        states[n_states][MAX_NAME-1] = '\0';
        n_states++;
    }
}

// transition table: -1 means no transition
int trans[MAX_STATES][256];

void init_trans() {
    for (int i = 0; i < MAX_STATES; i++)
        for (int j = 0; j < 256; j++)
            trans[i][j] = -1;
}

// trim helpers
char *trim(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    char *end = s + strlen(s) - 1;
    while (end >= s && isspace((unsigned char)*end)) { *end = '\0'; end--; }
    return s;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s conf.txt cadenas.txt\n", argv[0]);
        return 1;
    }
    const char *confpath = argv[1];
    const char *tests = argv[2];
    FILE *f = fopen(confpath, "r");
    if (!f) { perror("fopen conf"); return 1; }
    char line[MAX_LINE];
    char alphabet[256]; int n_alpha = 0;
    char start_state[MAX_NAME] = "";
    char accept_list[1024] = "";
    int in_trans = 0;
    init_trans();

    while (fgets(line, sizeof(line), f)) {
        char *ln = trim(line);
        if (ln[0] == '\0' || ln[0] == '#') continue;
        if (!in_trans) {
            if (strncmp(ln, "states:", 7) == 0) {
                char *rest = trim(ln + 7);
                // split by comma
                char *tok = strtok(rest, ",");
                while (tok) { add_state(trim(tok)); tok = strtok(NULL, ","); }
            } else if (strncmp(ln, "alphabet:", 9) == 0) {
                char *rest = trim(ln + 9);
                char *tok = strtok(rest, ",");
                while (tok) {
                    char sym = trim(tok)[0];
                    alphabet[(unsigned char)sym] = 1;
                    n_alpha++;
                    tok = strtok(NULL, ",");
                }
            } else if (strncmp(ln, "start:", 6) == 0) {
                strcpy(start_state, trim(ln + 6));
                add_state(start_state);
            } else if (strncmp(ln, "accept:", 7) == 0) {
                strcpy(accept_list, trim(ln + 7));
                // add accept states to list; states may already be added above
                char tmp[1024]; strcpy(tmp, accept_list);
                char *tok = strtok(tmp, ",");
                while (tok) { add_state(trim(tok)); tok = strtok(NULL, ","); }
            } else if (strncmp(ln, "transitions:", 12) == 0) {
                in_trans = 1;
            }
        } else {
            // transitions lines: src,sym,dst
            char buf[MAX_LINE]; strcpy(buf, ln);
            char *p1 = strtok(buf, ","); if (!p1) continue;
            char *p2 = strtok(NULL, ","); if (!p2) continue;
            char *p3 = strtok(NULL, ","); if (!p3) continue;
            char *src = trim(p1); char *sym = trim(p2); char *dst = trim(p3);
            add_state(src); add_state(dst);
            int sidx = get_state_idx(src);
            int didx = get_state_idx(dst);
            unsigned char ch = (unsigned char)sym[0]; // symbol as char
            trans[sidx][ch] = didx;
        }
    }
    fclose(f);

    // build accept set
    int accept_set[MAX_STATES]; for (int i=0;i<MAX_STATES;i++) accept_set[i]=0;
    if (strlen(accept_list)>0) {
        char tmp[1024]; strcpy(tmp, accept_list);
        char *tok = strtok(tmp, ",");
        while (tok) { int id=get_state_idx(trim(tok)); if (id>=0) accept_set[id]=1; tok = strtok(NULL, ","); }
    }

    // run tests
    FILE *ft = fopen(tests, "r");
    if (!ft) { perror("fopen tests"); return 1; }
    while (fgets(line, sizeof(line), ft)) {
        // strip newline
        char *ln = line;
        size_t L = strlen(ln);
        while (L>0 && (ln[L-1]=='\n' || ln[L-1]=='\r')) { ln[--L]=0; }
        char *s = ln;
        int cur = get_state_idx(start_state);
        int ok = 1;
        if (s[0] == '\0') {
            // empty input -> accept if start is accept
            ok = accept_set[cur];
        } else {
            for (size_t i=0; i<strlen(s); i++) {
                unsigned char ch = (unsigned char)s[i];
                if (trans[cur][ch] == -1) { ok = 0; break; }
                cur = trans[cur][ch];
            }
            if (ok) ok = accept_set[cur];
        }
        printf(ok ? "acepta\n" : "NO acepta\n");
    }
    fclose(ft);
    return 0;
}
