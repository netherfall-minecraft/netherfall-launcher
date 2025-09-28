#include <gtk/gtk.h>
#include <glib.h>
#include "core/updater.h"
#include "core/forge_installer.h"
#include "core/updater.h"
#include "config.h"
#include "ui/main_window.h"
#include "utils/download.h"


struct LauncherConfig g_config;


#include "utils/ini_utils.h"
#include <gtk/gtk.h>
#define LAUNCHER_STATE_URL "https://raw.githubusercontent.com/netherfall-minecraft/state/main/appstate_lastet.ini"



typedef struct {
    GtkApplication *app;
    int remote_build;
    int error; // 0 - ok, 1 - ошибка загрузки, 2 - ошибка парсинга
    int allow_window; // 1 - можно показывать окно, 0 - нет
} VersionCheckResult;


static gboolean show_main_window_if_ok(gpointer data) {
    g_print("[DEBUG] show_main_window_if_ok: called\n");
    VersionCheckResult *res = (VersionCheckResult *)data;
    g_print("[DEBUG] show_main_window_if_ok called\n");
    if (res->allow_window) {
        g_print("[DEBUG] Creating main window...\n");
        GtkWidget *window = create_main_window(res->app);
        g_print("[DEBUG] Main window created, presenting...\n");
        gtk_window_present(GTK_WINDOW(window));
        g_print("[DEBUG] Main window presented.\n");
    }
    g_application_release(G_APPLICATION(res->app));
    g_free(res);
    return G_SOURCE_REMOVE;
}

static gboolean show_update_dialog_and_quit(gpointer data) {
    g_print("[DEBUG] show_update_dialog_and_quit: called\n");
    VersionCheckResult *res = (VersionCheckResult *)data;
    g_print("[DEBUG] show_update_dialog_and_quit called, error=%d, remote_build=%d, allow_window=%d\n", res->error, res->remote_build, res->allow_window);
    if (res->error == 1) {
        g_print("Не удалось проверить обновления лаунчера (ошибка загрузки state)\n");
        g_timeout_add(2000, (GSourceFunc)g_application_quit, res->app);
    } else if (res->error == 2) {
        g_print("Не удалось прочитать build из appstate_lastet.ini\n");
        g_timeout_add(2000, (GSourceFunc)g_application_quit, res->app);
    } else if (res->remote_build > LAUNCHER_BUILD) {
        GtkAlertDialog *dlg = gtk_alert_dialog_new("Доступна новая версия лаунчера! Пожалуйста, обновите программу.");
        gtk_alert_dialog_show(dlg, NULL);
        g_object_unref(dlg);
        g_timeout_add(2000, (GSourceFunc)g_application_quit, res->app);
    } else {
        res->allow_window = 1;
        g_print("[DEBUG] Version is up to date, calling show_main_window_if_ok...\n");
        g_idle_add(show_main_window_if_ok, res);
        g_application_release(G_APPLICATION(res->app));
        return G_SOURCE_REMOVE;
    }
    g_application_release(G_APPLICATION(res->app));
    g_free(res);
    return G_SOURCE_REMOVE;
}

static gpointer check_launcher_update_thread(gpointer user_data) {
    g_print("[DEBUG] check_launcher_update_thread: called\n");
    GtkApplication *app = (GtkApplication *)user_data;
    VersionCheckResult *res = g_new0(VersionCheckResult, 1);
    res->app = app;
    char tmp_ini[256];
    snprintf(tmp_ini, sizeof(tmp_ini), "/tmp/appstate_lastet.ini");
    g_print("[DEBUG] check_launcher_update_thread: downloading state...\n");
    if (!download_file(LAUNCHER_STATE_URL, tmp_ini)) {
        g_print("[DEBUG] check_launcher_update_thread: download failed\n");
        fflush(stdout);
        res->error = 1;
        g_idle_add(show_update_dialog_and_quit, res);
        return NULL;
    }
    g_print("[DEBUG] check_launcher_update_thread: before read_build_from_ini('%s')\n", tmp_ini);
    fflush(stdout);
    int remote_build = read_build_from_ini(tmp_ini);
    g_print("[DEBUG] check_launcher_update_thread: after read_build_from_ini, remote_build=%d\n", remote_build);
    fflush(stdout);
    remove(tmp_ini);
    if (remote_build == -1) {
        g_print("[DEBUG] check_launcher_update_thread: read_build_from_ini failed\n");
        res->error = 2;
        g_idle_add(show_update_dialog_and_quit, res);
        return NULL;
    }
    res->remote_build = remote_build;
    res->error = 0;
    g_print("[DEBUG] check_launcher_update_thread: state ok, remote_build=%d\n", remote_build);
    g_idle_add(show_update_dialog_and_quit, res);
    return NULL;
}

static void check_launcher_update(GtkApplication *app) {
    g_print("[DEBUG] check_launcher_update: called\n");
    g_thread_new("version-check", check_launcher_update_thread, app);
}

static void activate(GtkApplication *app, gpointer user_data) {
    g_print("[DEBUG] activate: called\n");
    if (!load_config(&g_config, NULL)) {
        g_print("Ошибка загрузки пользовательского конфига\n");
        exit(1);
    }
    g_application_hold(G_APPLICATION(app));
    check_launcher_update(app);
    g_print("[DEBUG] activate: check_launcher_update called\n");
}

int main(int argc, char *argv[]) {
    g_print("[DEBUG] main: start\n");
    GtkApplication *app;
    int status;
    app = gtk_application_new("com.example.minecraftlauncher", G_APPLICATION_DEFAULT_FLAGS);
    g_print("[DEBUG] main: gtk_application_new done\n");
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_print("[DEBUG] main: g_signal_connect done\n");
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_print("[DEBUG] main: g_application_run returned\n");
    g_object_unref(app);
    g_print("[DEBUG] main: end\n");
    return status;
}

