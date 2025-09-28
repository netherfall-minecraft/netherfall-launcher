#define LAUNCHER_BUILD 1
#pragma once


struct LauncherConfig {
    char minecraft_path[256];
    char java_path[256];
    char nickname[64];
    int ram_mb;
};
int load_config(struct LauncherConfig *config, const char *filename);
int save_config(const struct LauncherConfig *config, const char *filename);
