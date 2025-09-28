#include "settings_page.h"
#include "../config.h"
#include <gtk/gtk.h>
#include <string.h>
extern struct LauncherConfig g_config;
extern GtkWidget *g_path_entry;
extern GtkWidget *g_java_entry;
extern GtkWidget *g_nick_entry;

static void on_path_changed(GtkEditable *editable, gpointer user_data) {
    struct LauncherConfig *cfg = (struct LauncherConfig *)user_data;
    strncpy(cfg->minecraft_path, gtk_editable_get_text(editable), sizeof(cfg->minecraft_path)-1);
    save_config(cfg, NULL);
}
static void on_java_changed(GtkEditable *editable, gpointer user_data) {
    struct LauncherConfig *cfg = (struct LauncherConfig *)user_data;
    strncpy(cfg->java_path, gtk_editable_get_text(editable), sizeof(cfg->java_path)-1);
    save_config(cfg, NULL);
}
static void on_nick_changed(GtkEditable *editable, gpointer user_data) {
    struct LauncherConfig *cfg = (struct LauncherConfig *)user_data;
    strncpy(cfg->nickname, gtk_editable_get_text(editable), sizeof(cfg->nickname)-1);
    save_config(cfg, NULL);
}
static void on_ram_changed(GtkSpinButton *spin, gpointer user_data) {
    struct LauncherConfig *cfg = (struct LauncherConfig *)user_data;
    if (g_path_entry)
        strncpy(cfg->minecraft_path, gtk_editable_get_text(GTK_EDITABLE(g_path_entry)), sizeof(cfg->minecraft_path)-1);
    if (g_java_entry)
        strncpy(cfg->java_path, gtk_editable_get_text(GTK_EDITABLE(g_java_entry)), sizeof(cfg->java_path)-1);
    if (g_nick_entry)
        strncpy(cfg->nickname, gtk_editable_get_text(GTK_EDITABLE(g_nick_entry)), sizeof(cfg->nickname)-1);
    cfg->ram_mb = gtk_spin_button_get_value_as_int(spin);
    save_config(cfg, NULL);
}
GtkWidget* g_path_entry = NULL;
GtkWidget* g_java_entry = NULL;
GtkWidget* g_nick_entry = NULL;

GtkWidget* create_settings_page(void) {
    GtkWidget *settings_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_name(settings_box, "settings_box");
    gtk_widget_set_halign(settings_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(settings_box, GTK_ALIGN_CENTER);
    GtkWidget *path_label = gtk_label_new("Путь к Minecraft:");
    g_path_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(g_path_entry), g_config.minecraft_path);
    gtk_widget_set_name(g_path_entry, "entry");
    gtk_box_append(GTK_BOX(settings_box), path_label);
    gtk_box_append(GTK_BOX(settings_box), g_path_entry);
    GtkWidget *java_label = gtk_label_new("Путь к Java:");
    g_java_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(g_java_entry), g_config.java_path);
    gtk_widget_set_name(g_java_entry, "entry");
    gtk_box_append(GTK_BOX(settings_box), java_label);
    gtk_box_append(GTK_BOX(settings_box), g_java_entry);
    GtkWidget *nickname_label = gtk_label_new("Никнейм:");
    g_nick_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(g_nick_entry), g_config.nickname);
    gtk_widget_set_name(g_nick_entry, "entry");
    gtk_box_append(GTK_BOX(settings_box), nickname_label);
    gtk_box_append(GTK_BOX(settings_box), g_nick_entry);
    GtkWidget *ram_label = gtk_label_new("ОЗУ (МБ):");
    GtkWidget *ram_entry = gtk_spin_button_new_with_range(512, 32768, 256);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ram_entry), g_config.ram_mb);
    gtk_widget_set_name(ram_entry, "entry");
    gtk_box_append(GTK_BOX(settings_box), ram_label);
    gtk_box_append(GTK_BOX(settings_box), ram_entry);
    g_signal_connect(g_path_entry, "changed", G_CALLBACK(on_path_changed), &g_config);
    g_signal_connect(g_java_entry, "changed", G_CALLBACK(on_java_changed), &g_config);
    g_signal_connect(g_nick_entry, "changed", G_CALLBACK(on_nick_changed), &g_config);
    g_signal_connect(ram_entry, "value-changed", G_CALLBACK(on_ram_changed), &g_config);
    return settings_box;
}
