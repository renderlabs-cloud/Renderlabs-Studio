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

#include "./welcome.h"
#include "../editor/renderer.h"
#include "../release.h"
#include "../tools.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <windows.h>

Router router001;

void RENDERLABS_PAGES_WELCOME_on_start_button_clicked(GtkApplication* app, GtkWidget* window) {
    reroute(&router001, 1);
    testery(0, NULL);
}

void RENDERLABS_PAGES_WELCOME_load(
  GtkApplication* app, GtkWidget* window, GtkBuilder* builder, Router routerf) {
    router001 = routerf;
    GObject* start_button;
    start_button = gtk_builder_get_object(builder, "start_button");
    g_signal_connect(
      start_button, "clicked", G_CALLBACK(RENDERLABS_PAGES_WELCOME_on_start_button_clicked), NULL);
}