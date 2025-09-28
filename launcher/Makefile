CC = clang
CFLAGS = -Wall `pkg-config --cflags gtk4`
LDFLAGS = `pkg-config --libs gtk4` -ljson-c
SRC = src/utils/ini_utils.c src/main.c src/config.c src/ui/main_window.c src/ui/settings_page.c src/core/repair.c src/core/forge_installer.c src/core/updater.c src/utils/download.c src/utils/github_tree.c src/utils/fs_utils.c
OUT = bin/launcher

all: $(OUT)

$(OUT): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -rf bin
