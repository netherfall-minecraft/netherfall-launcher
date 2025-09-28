
#include "config.h"
#include "launcher_settings.h"
#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static void get_config_path(char *out, size_t out_size) {
    const char *home = getenv("HOME");
    snprintf(out, out_size, "%s/%s%s", home, LAUNCHER_CONFIG_DIR, LAUNCHER_CONFIG_FILENAME);
}

int load_config(struct LauncherConfig *config, const char *unused) {
    char path[512];
    get_config_path(path, sizeof(path));
    FILE *fp = fopen(path, "r");
    int need_save = 0;
    char def_mc_path[512];
    if (getenv("HOME")) {
        snprintf(def_mc_path, sizeof(def_mc_path), "%s/.nfl/game", getenv("HOME"));
    } else {
        strcpy(def_mc_path, "./.nfl/game");
    }
    if (!fp) {
        strcpy(config->minecraft_path, def_mc_path);
        strcpy(config->java_path, "/usr/bin/java");
        strcpy(config->nickname, "Player");
        config->ram_mb = 4096;
        save_config(config, NULL);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *data = malloc(len + 1);
    fread(data, 1, len, fp);
    data[len] = '\0';
    fclose(fp);

    struct json_object *parsed = json_tokener_parse(data);
    if (!parsed) {
        strcpy(config->minecraft_path, def_mc_path);
        strcpy(config->java_path, "/usr/bin/java");
        strcpy(config->nickname, "Player");
        config->ram_mb = 4096;
        save_config(config, NULL);
        free(data);
        return 1;
    }

    const char *mp = json_object_get_string(json_object_object_get(parsed, "minecraft_path"));
    const char *jp = json_object_get_string(json_object_object_get(parsed, "java_path"));
    const char *nn = json_object_get_string(json_object_object_get(parsed, "nickname"));
    int ram = json_object_object_get(parsed, "ram_mb") ? json_object_get_int(json_object_object_get(parsed, "ram_mb")) : 4096;
    if (!mp) { mp = def_mc_path; need_save = 1; }
    if (!jp) { jp = "/usr/bin/java"; need_save = 1; }
    if (!nn) { nn = "Player"; need_save = 1; }
    if (!ram) { ram = 4096; need_save = 1; }
    strncpy(config->minecraft_path, mp, sizeof(config->minecraft_path)-1);
    strncpy(config->java_path, jp, sizeof(config->java_path)-1);
    strncpy(config->nickname, nn, sizeof(config->nickname)-1);
    config->ram_mb = ram;
    if (need_save) save_config(config, NULL);
    json_object_put(parsed);
    free(data);
    return 1;
}

int save_config(const struct LauncherConfig *config, const char *unused) {
    char path[512];
    get_config_path(path, sizeof(path));
    char dir[512];
    snprintf(dir, sizeof(dir), "%s/%s", getenv("HOME"), LAUNCHER_CONFIG_DIR);
    mkdir(dir, 0700);

    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "minecraft_path", json_object_new_string(config->minecraft_path));
    json_object_object_add(obj, "java_path", json_object_new_string(config->java_path));
    json_object_object_add(obj, "nickname", json_object_new_string(config->nickname));
    json_object_object_add(obj, "ram_mb", json_object_new_int(config->ram_mb));

    const char *json_str = json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PRETTY);
    FILE *fp = fopen(path, "w");
    if (!fp) { json_object_put(obj); return 0; }
    fputs(json_str, fp);
    fclose(fp);
    json_object_put(obj);
    return 1;
}
