
#include "github_tree.h"
#include "download.h"
#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int download_github_tree(const char *base_url, const char *raw_base, const char *dest_dir) {
    char tmp_json[512];
    snprintf(tmp_json, sizeof(tmp_json), "%s/github_tree.json", dest_dir);
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "curl -s -H 'Accept: application/vnd.github.v3+json' '%s' -o '%s'", base_url, tmp_json);
    if (system(cmd) != 0) return 0;

    FILE *f = fopen(tmp_json, "r");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *data = malloc(len+1);
    fread(data, 1, len, f);
    data[len] = 0;
    fclose(f);

    struct json_object *root = json_tokener_parse(data);
    free(data);
    if (!root) return 0;
    struct json_object *tree = NULL;
    if (!json_object_object_get_ex(root, "tree", &tree) || !json_object_is_type(tree, json_type_array)) {
        json_object_put(root);
        return 0;
    }
    int n = json_object_array_length(tree);
    for (int i = 0; i < n; ++i) {
        struct json_object *item = json_object_array_get_idx(tree, i);
        const char *type = json_object_get_string(json_object_object_get(item, "type"));
        const char *relpath = json_object_get_string(json_object_object_get(item, "path"));
        if (!type || !relpath) continue;
        if (strcmp(type, "blob") == 0) {
            // Создаём папку, если нужно
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dest_dir, relpath);
            char *slash = strrchr(fullpath, '/');
            if (slash) {
                *slash = 0;
                mkdir(fullpath, 0755);
                *slash = '/';
            }
            char url[1024];
            snprintf(url, sizeof(url), "%s%s", raw_base, relpath);
            download_file(url, fullpath);
        } else if (strcmp(type, "tree") == 0) {
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dest_dir, relpath);
            mkdir(fullpath, 0755);
        }
    }
    json_object_put(root);
    remove(tmp_json);
    return 1;
}
