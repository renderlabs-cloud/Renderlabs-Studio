/*
 * RenderLabs Studio - Software
 *
 * Copyright (c) 2024 Renderlabs
 * All rights reserved.
 *
 * This software is the proprietary property of Renderlabs.
 * It is intended solely for use as specified in the associated documentation.
 * Redistribution of this software, in whole or in part, in any form or medium,
 * is strictly prohibited without the prior written permission of the copyright
 * owner.
 *
 * You may reference this software in your documentation, publications, or other
 * works provided that such references are limited to describing its
 * functionality or usage. All references must clearly acknowledge Renderlabs as
 * the creator of the software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING
 * FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * For licensing inquiries, please contact
 * [renderthegreat.business+renderlabs@gmail.com].
 */

#include "tools.h"
#include "pages/dashboard.h"
#include "pages/welcome.h"
#include "resource.h"
#include <SDL2/SDL.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <windows.h>

GtkWidget* Debugger = NULL;
GtkWidget* text_view = NULL;
GtkTextBuffer* buffer = NULL;

void SHOW_DEBUGGER() {
    if (Debugger) {
        gtk_widget_show(Debugger);
    }
}

void HIDE_DEBUGGER() {
    if (Debugger) {
        gtk_widget_hide(Debugger);
    }
}

gboolean on_key_press_DEBUG(GtkWidget* widget, GdkEventKey* event, gpointer data) {
    static char key_sequence[6] = {0};
    const char* sequence = "debug";

    char key = gdk_keyval_name(event->keyval)[0];

    memmove(key_sequence, key_sequence + 1, 5);
    key_sequence[5] = key;

    if (strncmp(key_sequence, sequence, 5) == 0 && (event->state & GDK_CONTROL_MASK)) {
        SHOW_DEBUGGER();
        return TRUE;
    }

    return FALSE;
}

void SETUP_DEBUGGER() {
    Debugger = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(Debugger), "Debugger");
    gtk_window_set_default_size(GTK_WINDOW(Debugger), 400, 300);

    text_view = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);

    gtk_container_add(GTK_CONTAINER(Debugger), text_view);
    gtk_widget_show_all(Debugger);
    gtk_widget_hide(Debugger);
}

void printw(const char* format, ...) {
    if (buffer) {
        va_list args;
        va_start(args, format);

        char message[1024];
        vsnprintf(message, sizeof(message), format, args);

        va_end(args);

        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, message, -1);
        gtk_text_buffer_insert(buffer, &iter, "\n", -1);
    }
}

void ALERT_BOX(GtkWindow* parent, GtkMessageType type, const char* message) {
    GtkWidget* dialog;

    dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, type, GTK_BUTTONS_OK, "%s", message);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void DIE_on_button_clicked(GtkWidget* widget, gpointer data) {
    GtkWidget* window = (GtkWidget*)data;
    gtk_widget_destroy(window);
    SHOW_DEBUGGER();
}

void DIE(GtkWidget* parent_window) {
    GtkWidget* error_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(error_window), "Error");
    gtk_window_set_default_size(GTK_WINDOW(error_window), 300, 150);
    gtk_container_set_border_width(GTK_CONTAINER(error_window), 10);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(error_window), vbox);

    GtkWidget* label = gtk_label_new("Sorry, but an error has occurred.\n We are "
                                     "very sorry about that.\n :(");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);

    GtkWidget* close_button = gtk_button_new_with_label("Check the debugger and leave");
    gtk_box_pack_start(GTK_BOX(vbox), close_button, FALSE, TRUE, 0);
    gtk_widget_set_halign(close_button, GTK_ALIGN_CENTER);

    g_signal_connect(close_button, "clicked", G_CALLBACK(DIE_on_button_clicked), error_window);

    gtk_widget_show_all(error_window);

    if (parent_window) {
        gtk_widget_set_sensitive(parent_window, FALSE);
        g_signal_connect(
          error_window, "destroy", G_CALLBACK(gtk_widget_set_sensitive), parent_window);
    }
}

void LINK_WITH_tools(GtkWidget* window) {
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press_DEBUG), NULL);
}

static Uint8* audio_pos;
static Uint32 audio_len;

void audio_callback(void* userdata, gint8* stream, unsigned int len) {
    if (audio_len == 0) {
        return;
    }

    len = (len > audio_len ? audio_len : len);

    for (unsigned int i = 0; i < len; i++) {
        stream[i] = i / 1000; // Fade out gradually
    }

    SDL_memcpy(stream, audio_pos, len);

    audio_pos += len;
    audio_len -= len;
}

void play_sound_wav(char* audio_buffer, size_t audio_length) {
    SDL_AudioSpec wav_spec;
    wav_spec.freq = 44100 * 1.10;
    wav_spec.format = AUDIO_S16LSB;
    wav_spec.channels = 2;
    wav_spec.samples = 4096;
    wav_spec.callback = (SDL_AudioCallback)audio_callback;
    wav_spec.userdata = NULL;

    audio_pos = (unsigned char*)audio_buffer;
    audio_len = (Uint32)audio_length;

    if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_PauseAudio(0);

    while (audio_len > 0) {
        SDL_Delay(10);
    }

    SDL_CloseAudio();
    // Do not free audio_buffer as it is managed by the resource loader
}

void reroute(Router* router, int routeid) {
    router->routeid = routeid;
    GtkBuilder* builder = gtk_builder_new();
    HINSTANCE hInstance = GetModuleHandle(NULL);

    switch (router->routeid) {
    case 0: {
        router->route = "IDI_PAGES_WELCOME";
        break;
    }
    case 1: {
        router->route = "IDI_PAGES_DASHBOARD";
        break;
    }
    default: {
        printw("Page not found!\n");
        return;
    }
    }

    char* pagew = load_text_resource(hInstance, router->route, "TEXTFILE");
    GError* perror = NULL;
    if (!gtk_builder_add_from_string(builder, pagew, -1, &perror)) {
        printf("Page load failed!\n");
        if (perror != NULL) {
            printw("%s\n", perror->message);
            g_error_free(perror);
        }
        return;
    }

    switch (router->routeid) {
    case 0: {
        RENDERLABS_PAGES_WELCOME_load(router->app, router->widget, builder, *router);
        break;
    }
    case 1: {
        RENDERLABS_PAGES_DASHBOARD_load(router->app, router->widget, builder, *router);
        break;
    }
    }
    GtkWidget* vbox = GTK_WIDGET(gtk_builder_get_object(builder, "vbox"));

    if (gtk_bin_get_child(GTK_BIN(router->widget)) != NULL) {
        gtk_container_remove(
          GTK_CONTAINER(router->widget), gtk_bin_get_child(GTK_BIN(router->widget)));
    }

    gtk_container_add(GTK_CONTAINER(router->widget), vbox);
    gtk_widget_show_all(router->widget);
}

Router router(GtkApplication* app, GtkWidget* window) {
    Router router = {NULL, 0, 0, app, window};
    reroute(&router, 0);
    return router;
}

void process_gtk_events() {
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
}