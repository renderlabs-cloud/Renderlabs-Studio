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

#include "app.h"
#include "config.h"
#include "release.h"
#include "resource.h"
#include "tools.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <windows.h>

Router MainRouter = {NULL, 0, 0, NULL, NULL};

static void RENDERLABS_WINDOW_DESTROY() {
    gtk_main_quit();
    exit(0);
}

void RENDERLABS_CSS(GtkWidget* window) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    GError* perror = NULL;
    char* header = load_text_resource(hInstance, "IDI_STYLES_HEADER", "TEXTFILE");
    GtkWidget* headerbar = gtk_header_bar_new();
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar), "Renderlabs Studio");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);
    GtkCssProvider* provider1 = gtk_css_provider_new();
    GtkCssProvider* provider2 = gtk_css_provider_new();
    if (!gtk_css_provider_load_from_data(provider1, header, -1, &perror)) {
        printw("Style load failed!\n");
        printw("%s\n", perror->message);
    }
    GtkStyleContext* style_context1 = gtk_widget_get_style_context(headerbar);
    gtk_style_context_add_provider(
      style_context1, GTK_STYLE_PROVIDER(provider1), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
    char* renderlabs = load_text_resource(hInstance, "IDI_STYLES_RENDERLABS", "TEXTFILE");
    if (!gtk_css_provider_load_from_data(provider2, renderlabs, -1, &perror)) {
        printw("Style load failed!\n");
        printw("%s\n", perror->message);
    }
    GtkStyleContext* style_context2 = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(
      style_context2, GTK_STYLE_PROVIDER(provider2), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void RENDERLABS_MAIN(GtkApplication* app, GtkWidget* window) {
    g_signal_connect(window, "destroy", G_CALLBACK(RENDERLABS_WINDOW_DESTROY), NULL);
    LINK_WITH_tools(window);
    SETUP_DEBUGGER();
    MainRouter = router(app, window);
    RENDERLABS_CSS(window);
    gtk_widget_show_all(window);
    gtk_main();
}
