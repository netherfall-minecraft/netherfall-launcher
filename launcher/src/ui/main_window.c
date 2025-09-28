#include <gtk/gtk.h>
#include <stddef.h>
#include "../core/repair.h"
#include "../core/updater.h"
#include "../core/forge_installer.h"
#include "../config.h"
#include "../launcher_settings.h"
#include "settings_page.h"

extern struct LauncherConfig g_config;
static void on_repair_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *parent = NULL;
    if (button) {
        GtkWidget *widget = GTK_WIDGET(button);
        parent = GTK_WINDOW(gtk_widget_get_root(widget));
    }
    if (repair_game()) {
        GtkAlertDialog *dlg = gtk_alert_dialog_new("Игра успешно восстановлена!");
        gtk_alert_dialog_show(dlg, parent);
        g_object_unref(dlg);
    } else {
        GtkAlertDialog *dlg = gtk_alert_dialog_new("Ошибка при восстановлении игры!");
        gtk_alert_dialog_show(dlg, parent);
        g_object_unref(dlg);
    }
    }

typedef struct {
    GtkWindow *parent;
    int result; // 0 - ok 1 - ошибка обновления, 2 - ошибка установки Forge
} GameUpdateResult;

static gboolean show_game_update_result(gpointer data) {
    GameUpdateResult *res = (GameUpdateResult *)data;
    if (res->result == 1) {
        GtkAlertDialog *dlg = gtk_alert_dialog_new("Ошибка обновления файлов игры!");
        gtk_alert_dialog_show(dlg, res->parent);
        g_object_unref(dlg);
    } else if (res->result == 2) {
        GtkAlertDialog *dlg = gtk_alert_dialog_new("Ошибка установки Forge!");
        gtk_alert_dialog_show(dlg, res->parent);
        g_object_unref(dlg);
    } else {
        GtkAlertDialog *dlg = gtk_alert_dialog_new("Игра успешно обновлена и готова к запускy!");
        gtk_alert_dialog_show(dlg, res->parent);
        g_object_unref(dlg);
    }
    g_free(res);
    return G_SOURCE_REMOVE;
}

static gpointer update_game_thread(gpointer user_data) {
    GameUpdateResult *res = (GameUpdateResult *)user_data;
    // проверка и обновление файлов игры
    if (!update_game_files(&g_config)) {
        res->result = 1;
        g_idle_add(show_game_update_result, res);
        return NULL;
    }
    // проверка Forge и модов
    if (!check_forge_files()) {
        if (!install_forge_and_mods()) {
            res->result = 2;
            g_idle_add(show_game_update_result, res);
            return NULL;
        }
    }
    res->result = 0;
    g_idle_add(show_game_update_result, res);
    return NULL;
}

static void on_launch_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *parent = NULL;
    if (button) {
        GtkWidget *widget = GTK_WIDGET(button);
        parent = GTK_WINDOW(gtk_widget_get_root(widget));
    }
    GameUpdateResult *res = g_new0(GameUpdateResult, 1);
    res->parent = parent;
    g_thread_new("game-update", update_game_thread, res);
}

GtkWidget* create_main_window(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), LAUNCHER_TITLE);
    gtk_window_set_default_size(GTK_WINDOW(window), LAUNCHER_DEFAULT_WIDTH, LAUNCHER_DEFAULT_HEIGHT);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkWidget *notebook = gtk_notebook_new();

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_name(main_box, "main_box");
    gtk_widget_set_halign(main_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_box, GTK_ALIGN_CENTER);

    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *icon = gtk_image_new_from_icon_name("applications-games");
    GtkWidget *title = gtk_label_new("<span size='20000' weight='bold'>Minecraft Launcher</span>");
    gtk_label_set_use_markup(GTK_LABEL(title), TRUE);
    gtk_box_append(GTK_BOX(header_box), icon);
    gtk_box_append(GTK_BOX(header_box), title);
    gtk_widget_set_halign(header_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), header_box);

    char info[512];
    snprintf(info, sizeof(info), "Ник: <b>%s</b>  |  ОЗУ: <b>%d MB</b>  |  Java: <b>%s</b>",
        g_config.nickname, g_config.ram_mb, g_config.java_path);
    GtkWidget *info_label = gtk_label_new(info);
    gtk_label_set_use_markup(GTK_LABEL(info_label), TRUE);
    gtk_widget_set_name(info_label, "info_label");
    gtk_box_append(GTK_BOX(main_box), info_label);


    GtkWidget *launch_btn = gtk_button_new_with_label("Запустить Minecraft");
    gtk_widget_set_name(launch_btn, "launch_btn");
    g_signal_connect(launch_btn, "clicked", G_CALLBACK(on_launch_clicked), NULL);
    gtk_box_append(GTK_BOX(main_box), launch_btn);

    GtkWidget *repair_btn = gtk_button_new_with_label("Починить игру");
    gtk_widget_set_name(repair_btn, "repair_btn");
    g_signal_connect(repair_btn, "clicked", G_CALLBACK(on_repair_clicked), NULL);
    gtk_box_append(GTK_BOX(main_box), repair_btn);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), main_box, gtk_label_new("Главная"));

    GtkWidget *settings_page = create_settings_page();
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), settings_page, gtk_label_new("Настройки"));

    gtk_window_set_child(GTK_WINDOW(window), notebook);
    return window;
}
