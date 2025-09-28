#pragma once
// base_url — https://api.github.com/repos/netherfall-minecraft/files/git/trees/main?recursive=1
// raw_base —  https://raw.githubusercontent.com/netherfall-minecraft/files/main/
// dest_dir — локальная папка
int download_github_tree(const char *base_url, const char *raw_base, const char *dest_dir);
