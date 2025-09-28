#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


int repair_game(void) {
    const char *home = getenv("HOME");
    if (!home) return 0;
    char game_dir[512];
    snprintf(game_dir, sizeof(game_dir), "%s/.nfl/game", home);
    struct stat st;
    if (stat(game_dir, &st) == -1 || !S_ISDIR(st.st_mode)) {
        if (mkdir(game_dir, 0755) == -1) return 0;
    }
    return 1;
}
