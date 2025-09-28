#include "ini_utils.h"
#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>

// возвращает N или -1 при ошибке
int read_build_from_ini(const char *ini_path) {
    g_print("[DEBUG] read_build_from_ini: ini_path=%s\n", ini_path);
    fflush(stdout);
    FILE *f = fopen(ini_path, "r");
    if (!f) {
        g_print("[DEBUG] read_build_from_ini: fopen failed\n");
        fflush(stdout);
        return -1;
    }
    char line[256];
    int build = -1;
    while (fgets(line, sizeof(line), f)) {
        g_print("[DEBUG] read_build_from_ini: line='%s'\n", line);
        fflush(stdout);
        if (strncmp(line, "builds", 5) == 0) { 
            char *eq = strchr(line, '=');
            if (eq) {
                build = atoi(eq + 1);
                g_print("[DEBUG] read_build_from_ini: build parsed = %d\n", build);
                fflush(stdout);
                break;
            }
        }
    }
    fclose(f);
    g_print("[DEBUG] read_build_from_ini: returning %d\n", build);
    fflush(stdout);
    return build;
}
