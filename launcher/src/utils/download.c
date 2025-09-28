#include "download.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int download_file(const char *url, const char *dest_path) {
    char cmd[1024];
    printf("[DEBUG] download_file: url=%s, dest_path=%s\n", url, dest_path);
    snprintf(cmd, sizeof(cmd), "curl -L -o '%s' '%s'", dest_path, url);
    int res = system(cmd);
    printf("[DEBUG] download_file: system() returned %d\n", res);
    return res == 0;
}
