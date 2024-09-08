/*
 * RenderLabs Studio - Software
 *
 * Copyright (c) 2024 Renderlabs
 * All rights reserved.
 *
 * This software is the proprietary property of Renderlabs.
 * It is intended solely for use as specified in the associated documentation.
 * Redistribution of this software, in whole or in part, in any form or medium, is strictly
 * prohibited without the prior written permission of the copyright owner.
 *
 * You may reference this software in your documentation, publications, or other works provided that
 * such references are limited to describing its functionality or usage. All references must clearly
 * acknowledge Renderlabs as the creator of the software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES, OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM,
 * OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For licensing inquiries, please contact [renderthegreat.business+renderlabs@gmail.com].
 */

#include "app.h"
#include "release.h"
#include "resource.h"
#include "tools.h"
#include <glib.h>
#include <gtk/gtk.h>
#include <mmsystem.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <windows.h>

DWORD WINAPI play_start_sound_thread(LPVOID lpParam) {
    size_t audio_size;
    char* audio_data
      = load_sized_resource(GetModuleHandle(NULL), "IDR_INTRO_SOUND", "TEXTFILE", &audio_size);
    play_sound_wav(audio_data, audio_size);
    return 0;
}

HANDLE play_start_sound() {
    HANDLE thread;
    DWORD threadId;
    thread = CreateThread(NULL, 0, play_start_sound_thread, NULL, 0, &threadId);
    return thread;
}

DWORD WINAPI load_thread(LPVOID lpParam) {
    // Nothing to load for now
    return 0;
}

HANDLE load() {
    HANDLE thread;
    DWORD threadId;
    thread = CreateThread(NULL, 0, load_thread, NULL, 0, &threadId);
    return thread;
}

void startup(GtkApplication* app) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    GtkWidget* window = gtk_application_window_new(app);
    GtkBuilder* builder = gtk_builder_new();
#ifdef ALPHA
    ALERT_BOX(GTK_WINDOW(window), GTK_BUTTONS_OK, "This is an alpha, You may experience bugs.");
#endif
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    char* splash = load_text_resource(hInstance, "IDI_WINDOWS_SPLASH", "TEXTFILE");
    GError* perror = NULL;
    if (!gtk_builder_add_from_string(builder, splash, -1, &perror)) {
        printf("Page load failed!\n");
        if (perror != NULL) {
            printw("%s\n", perror->message);
            g_error_free(perror);
        }
        return;
    }

    GtkWidget* vbox = GTK_WIDGET(gtk_builder_get_object(builder, "vbox"));
    GtkWidget* sbox = GTK_WIDGET(gtk_builder_get_object(builder, "sbox"));
    GtkWidget* image = gtk_image_new();
    gtk_container_add(GTK_CONTAINER(sbox), image);
    gtk_container_add(GTK_CONTAINER(vbox), sbox);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    load_image(GTK_IMAGE(image), "IDI_IMAGES_SPLASH");

    gtk_widget_show_all(window);
    process_gtk_events();
    HANDLE sound_thread = play_start_sound();
    HANDLE load_thread = load();
    // If sound finishes first wait for loading, else wait until sound is over
    HANDLE threads[] = {sound_thread, load_thread};
    DWORD result = WaitForMultipleObjects(2, threads, FALSE, INFINITE);

    if (result == WAIT_OBJECT_0) {
        WaitForSingleObject(load_thread, INFINITE);
    } else if (result == WAIT_OBJECT_0 + 1) {
        WaitForSingleObject(sound_thread, INFINITE);
    }
    CloseHandle(sound_thread);
    CloseHandle(load_thread);
    gtk_widget_destroy(window);
}

static void activate(GtkApplication* app) {
    startup(app);
    GtkWidget* window;
    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 2000, 1000);
    RENDERLABS_MAIN(app, window);
}

int main(int argc, char* argv[]) {
    GtkApplication* app;
    int status;
    app = gtk_application_new("cloud.renderlabs.studio", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
