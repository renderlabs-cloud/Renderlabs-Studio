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
#define RESOURCE_H
#ifdef RESOURCE_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <windows.h>

#define IDI_STYLES_MAIN 101

char* load_text_resource(HINSTANCE hInstance, LPCSTR resourceName, LPCSTR resourceType);

char* load_sized_resource(
  HINSTANCE hInstance, LPCSTR resourceName, LPCSTR resourceType, size_t* size);

void load_image(GtkImage* image, LPCSTR resourceName);

#endif