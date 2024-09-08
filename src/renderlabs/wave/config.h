/*
 * RenderLabs Studio - Software
 * 
 * Copyright (c) 2024 Renderlabs
 * All rights reserved.
 * 
 * This software is the proprietary property of Renderlabs.
 * It is intended solely for use as specified in the associated documentation.
 * Redistribution of this software, in whole or in part, in any form or medium, is strictly prohibited without the prior written permission of the copyright owner.
 * 
 * You may reference this software in your documentation, publications, or other works provided that such references are limited to describing its functionality or usage. 
 * All references must clearly acknowledge Renderlabs as the creator of the software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For licensing inquiries, please contact [renderthegreat.business+renderlabs@gmail.com].
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "wave.h"
#include "duktape.h"

duk_ret_t WAVE_PRINT(duk_context *ctx) {
    if (duk_get_top(ctx) > 0) {
    const char *str = duk_safe_to_string(ctx, -1);
    printf("%s\n", str);
  } else {
    printf("\n");
  }
  return 0;
};

duk_ret_t WAVE_QUIT(duk_context *ctx) {
    exit(0);
    return 0;
};

WAVE_FUNCTIONS WAVE_EXTRA_FUNCTIONS = {
    .functions = (WAVE_FUNCTION_PLUS[]){{
        .function = WAVE_PRINT,
        .function_name = "print",
        .parameters = 1,
    }},
    .length = 1
};
const WAVE_INTERFACE WAVE_CONTEXT = {
    .VERSION = {.key = "version", .value = "0.1.0"},
    .getObjectById = NULL,
    .getObjectsByClass = NULL,
    .getObjectsByShape = NULL,
    .getAllObjects = NULL,
    .getTime = NULL,
    .getFrame = NULL,
    .getFrameRate = NULL,
    .quit = WAVE_QUIT,
    .extra_variables = NULL,
    .extra_functions = &WAVE_EXTRA_FUNCTIONS};
WAVE_INSTANCE WAVE = {
    .wave_context = WAVE_CONTEXT,
    .type = WAVE_NONE
};
#endif