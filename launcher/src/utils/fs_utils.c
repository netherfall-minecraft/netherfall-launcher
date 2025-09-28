#include "fs_utils.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

int remove_dir_recursive(const char *path) {
#if defined(_WIN32)
    WIN32_FIND_DATA fd;
    char pattern[MAX_PATH];
    snprintf(pattern, sizeof(pattern), "%s\\*", path);
    HANDLE h = FindFirstFile(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return 1;
    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;
        char buf[MAX_PATH];
        snprintf(buf, sizeof(buf), "%s\\%s", path, fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            remove_dir_recursive(buf);
        } else {
            DeleteFile(buf);
        }
    } while (FindNextFile(h, &fd));
    FindClose(h);
    RemoveDirectory(path);
    return 1;
#else
    DIR *d = opendir(path);
    if (!d) return 1;
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        char buf[1024];
        snprintf(buf, sizeof(buf), "%s/%s", path, entry->d_name);
        struct stat st;
        if (!stat(buf, &st)) {
            if (S_ISDIR(st.st_mode)) {
                remove_dir_recursive(buf);
            } else {
                remove(buf);
            }
        }
    }
    closedir(d);
    rmdir(path);
    return 1;
#endif
}
