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

#include "dashboard.h"
#include "../tools.h"
#include "../release.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <windows.h>

Router router002;

void RENDERLABS_PAGES_DASHBOARD_load(
  GtkApplication* app, GtkWidget* window, GtkBuilder* builder, Router routerf) {
    router002 = routerf;
    // WAVE_RUN(WAVE, "print('true')");
}