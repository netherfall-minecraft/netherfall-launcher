#include <string.h>
#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0755)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "../utils/github_tree.h"
#include "../utils/fs_utils.h"
#include "updater.h"
#include "../utils/download.h"

static void mkdirs(const char *path) {
    char tmp[512];
    strncpy(tmp, path, sizeof(tmp));
    tmp[sizeof(tmp)-1] = 0;
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char old = *p;
            *p = 0;
            MKDIR(tmp);
            *p = old;
        }
    }
    MKDIR(tmp);
}

#define STATE_URL "https://raw.githubusercontent.com/netherfall-minecraft/state/main/appstate_lastet.ini"
#define FILES_BASE "https://raw.githubusercontent.com/netherfall-minecraft/files/main/"



int update_game_files(struct LauncherConfig *cfg) {
    if (!cfg || !cfg->minecraft_path[0]) return 0;
    mkdirs(cfg->minecraft_path);
    DIR *dir = opendir(cfg->minecraft_path);

    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            if (strcmp(entry->d_name, "appstate_lastet.ini") == 0) continue;
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", cfg->minecraft_path, entry->d_name);
            remove_dir_recursive(path);
        }
        closedir(dir);
    }
    char state_path[512];
    snprintf(state_path, sizeof(state_path), "%s/appstate_lastet.ini", cfg->minecraft_path);
    char state_dir[512];
    strncpy(state_dir, state_path, sizeof(state_dir));
    state_dir[sizeof(state_dir)-1] = 0;
    char *slash = strrchr(state_dir, '/');
    if (slash) {
        *slash = 0;
        mkdirs(state_dir);
    }
    if (!download_file(STATE_URL, state_path)) return 0;
    const char *tree_api = "https://api.github.com/repos/netherfall-minecraft/files/git/trees/main?recursive=1";
    if (!download_github_tree(tree_api, FILES_BASE, cfg->minecraft_path)) return 0;
    return 1;
}
