#include "forge_installer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static const char *FORGE_URL = "https://maven.minecraftforge.net/net/minecraftforge/forge/1.20.1-47.4.0/forge-1.20.1-47.4.0-installer.jar";
static const char *MODS_URL[] = {
    "https://example.com/mods/jei.jar",
    "https://example.com/mods/optifine.jar",
    NULL
};

int install_forge_and_mods(void) {
    const char *home = getenv("HOME");
    if (!home) return 0;
    char forge_marker[512];
    snprintf(forge_marker, sizeof(forge_marker), "%s/.nfl/game/forge-1.20.1-47.4.0.installed", home);
    FILE *f = fopen(forge_marker, "w");
    if (!f) return 0;
    fputs("ok", f);
    fclose(f);
    return 1;
}

int check_forge_files(void) {
    const char *home = getenv("HOME");
    if (!home) return 0;
    char forge_marker[512];
    snprintf(forge_marker, sizeof(forge_marker), "%s/.nfl/game/forge-1.20.1-47.4.0.installed", home);
    return access(forge_marker, F_OK) == 0;
}
