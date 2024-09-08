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

#include "resource.h"
#include <gdk/gdkpixbuf.h>
#include <gtk/gtk.h>
#include <png.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

char* load_text_resource(HINSTANCE hInstance, LPCSTR resourceName, LPCSTR resourceType) {
    HRSRC hResource = FindResource(hInstance, resourceName, resourceType);
    if (!hResource) {
        fprintf(stderr, "Resource not found!\n");
        return NULL;
    }

    HGLOBAL hLoadedResource = LoadResource(hInstance, hResource);
    if (!hLoadedResource) {
        fprintf(stderr, "Failed to load resource!\n");
        return NULL;
    }

    LPVOID pResourceData = LockResource(hLoadedResource);
    if (!pResourceData) {
        fprintf(stderr, "Failed to lock resource!\n");
        return NULL;
    }

    DWORD resourceSize = SizeofResource(hInstance, hResource);
    if (resourceSize == 0) {
        fprintf(stderr, "Resource size is zero!\n");
        return NULL;
    }

    return (char*)pResourceData;
}

char* load_sized_resource(
  HINSTANCE hInstance, LPCSTR resourceName, LPCSTR resourceType, size_t* size) {
    HRSRC hResource = FindResource(hInstance, resourceName, resourceType);
    if (!hResource) {
        fprintf(stderr, "Resource not found!\n");
        return NULL;
    }

    HGLOBAL hLoadedResource = LoadResource(hInstance, hResource);
    if (!hLoadedResource) {
        fprintf(stderr, "Failed to load resource!\n");
        return NULL;
    }

    LPVOID pResourceData = LockResource(hLoadedResource);
    if (!pResourceData) {
        fprintf(stderr, "Failed to lock resource!\n");
        return NULL;
    }

    DWORD resourceSize = SizeofResource(hInstance, hResource);
    if (resourceSize == 0) {
        fprintf(stderr, "Resource size is zero!\n");
        return NULL;
    }

    if (size) {
        *size = (size_t)resourceSize;
    }
    return (char*)pResourceData;
}

void load_image(GtkImage* image, LPCSTR resourceName) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    size_t resource_size;
    char* resource_data = load_sized_resource(hInstance, resourceName, "PNG", &resource_size);
    if (!resource_data) {
        g_printerr("Failed to load resource\n");
        return;
    }
    size_t data_size;
    unsigned char* data = g_base64_decode(resource_data, &data_size);
    FILE* fptr = fopen("debug.png", "wb");
    if (fptr == NULL) {
        g_printerr("Failed to open file for writing\n");
        return;
    }

    fwrite(data, sizeof(char), data_size, fptr);

    png_image image_info = {0};
    image_info.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_memory(&image_info, data, data_size)) {
        g_printerr("Failed to read PNG image info\n");
        return;
    }

    int rowstride = PNG_IMAGE_ROW_STRIDE(image_info);

    png_bytep buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image_info));
    if (!buffer) {
        g_printerr("Failed to allocate memory for image\n");
        png_image_free(&image_info);
        return;
    }
    if (!png_image_finish_read(&image_info, NULL, buffer, rowstride, NULL)) {
        g_printerr("Failed to finish reading PNG image\n");
        png_image_free(&image_info);
        free(buffer);
        return;
    }
    GdkPixbufLoader* loader = gdk_pixbuf_loader_new();
    if (!gdk_pixbuf_loader_write(loader, (const guchar*)data, data_size, NULL)) {
        g_printerr("Failed to load PNG data into GdkPixbufLoader\n");
        return;
    }
    GdkPixbuf* pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);

    if (pixbuf) {
        gtk_image_set_from_pixbuf(image, pixbuf);
        g_object_unref(pixbuf);
    } else {
        g_printerr("Failed to create pixbuf from PNG data\n");
    }

    gdk_pixbuf_loader_close(loader, NULL);

    png_image_free(&image_info);
    free(buffer);
}