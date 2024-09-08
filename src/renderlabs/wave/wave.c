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

#include "wave.h"
#include "../where.h"
#include "duktape.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_CTX_FILE_SIZE 100000

static double current_time = 0;

typedef struct Scheduler {
    double time;
    duk_context* ctx;
    void* callback_ref;
} Scheduler;

Scheduler* schedulers[2048];
int scheduler_count = 0;

void keep(duk_context* ctx, Scheduler* scheduler) { schedulers[scheduler_count++] = scheduler; }

void onRestore(duk_context* ctx, Scheduler* scheduler) {
    duk_get_global_string(ctx, "Time");
    duk_get_prop_string(ctx, -1, "getTime");
    duk_call(ctx, 0);
    double current_time = duk_get_number(ctx, -1);
    duk_pop_2(ctx);

    if (!duk_push_heapptr(ctx, scheduler->callback_ref)) {
        return;
    }

    if (current_time >= scheduler->time) {
        free(scheduler);
        if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
            printf("Error calling function: %s\n", duk_safe_to_string(ctx, -1));
        }
        duk_pop(ctx);
    }
}

duk_ret_t next(duk_context* ctx) {
    current_time += 1;
    for (int i = 0; i < scheduler_count; i++) {
        Scheduler* scheduler = schedulers[i];
        onRestore(ctx, scheduler);
    }
    return 0;
}

duk_ret_t Time_backwards(duk_context* ctx) {
    double time = duk_require_number(ctx, 0);
    current_time -= time;
    return 0;
}

duk_ret_t Time_forward(duk_context* ctx) {
    double time = duk_require_number(ctx, 0);
    current_time += time;
    return 0;
}

duk_ret_t Time_getTime(duk_context* ctx) {
    duk_push_number(ctx, current_time);
    return 1;
}

duk_ret_t Time_setTime(duk_context* ctx) {
    double time = duk_require_number(ctx, 0);
    current_time = time;
    return 0;
}

void register_Time_class(duk_context* ctx) {
    duk_push_object(ctx);
    duk_push_c_function(ctx, Time_backwards, 1);
    duk_put_prop_string(ctx, -2, "backwards");
    duk_push_c_function(ctx, Time_forward, 1);
    duk_put_prop_string(ctx, -2, "forward");
    duk_push_c_function(ctx, Time_getTime, 0);
    duk_put_prop_string(ctx, -2, "getTime");
    duk_push_c_function(ctx, Time_setTime, 1);
    duk_put_prop_string(ctx, -2, "setTime");
    duk_put_global_string(ctx, "Time");
}

duk_ret_t Scheduler_constructor(duk_context* ctx) {
    double time = duk_require_number(ctx, 0);
    Scheduler* scheduler = malloc(sizeof(Scheduler));
    scheduler->time = time;
    scheduler->ctx = ctx;
    duk_dup(ctx, 1);
    scheduler->callback_ref = duk_get_heapptr(ctx, -1);

    schedulers[scheduler_count++] = scheduler;
    keep(ctx, scheduler);
    return 0;
}

void register_Scheduler_class(duk_context* ctx) {
    duk_push_c_function(ctx, Scheduler_constructor, 2);
    duk_put_global_string(ctx, "Scheduler");
}

void process_schedulers(duk_context* ctx) {
    for (int i = 0; i < scheduler_count; i++) {
        onRestore(ctx, schedulers[i]);
    }
}

void wave_bind(duk_context* ctx, WAVE_INTERFACE wave_context, WAVE_TYPE type) {
    duk_push_object(ctx);

    duk_push_string(ctx, wave_context.VERSION.value);
    duk_put_prop_string(ctx, -2, wave_context.VERSION.key);

    if (type == WAVE_ANIMATION) {
        duk_push_c_function(ctx, wave_context.getObjectById, 1);
        duk_put_prop_string(ctx, -2, "getObjectById");

        duk_push_c_function(ctx, wave_context.getObjectsByClass, 1);
        duk_put_prop_string(ctx, -2, "getObjectsByClass");

        duk_push_c_function(ctx, wave_context.getObjectsByShape, 1);
        duk_put_prop_string(ctx, -2, "getObjectsByShape");

        duk_push_c_function(ctx, wave_context.getAllObjects, 0);
        duk_put_prop_string(ctx, -2, "getAllObjects");
    }

    if (type == WAVE_ANIMATION || type == WAVE_AUDIO) {
        duk_push_c_function(ctx, wave_context.getTime, 0);
        duk_put_prop_string(ctx, -2, "getTime");

        duk_push_c_function(ctx, wave_context.getFrame, 0);
        duk_put_prop_string(ctx, -2, "getFrame");

        duk_push_c_function(ctx, wave_context.getFrameRate, 0);
        duk_put_prop_string(ctx, -2, "getFrameRate");
    }

    duk_push_c_function(ctx, wave_context.quit, 0);
    duk_put_prop_string(ctx, -2, "quit");

    duk_push_c_function(ctx, next, 0);
    duk_put_global_string(ctx, "next");

    if (wave_context.extra_variables)
        for (int i = 0; i < wave_context.extra_variables->length; i++) {
            duk_push_string(ctx, wave_context.extra_variables->variables[i].value);
            duk_put_prop_string(ctx, -2, wave_context.extra_variables->variables[i].value);
        }
    if (wave_context.extra_functions)
        for (int i = 0; i < wave_context.extra_functions->length; i++) {
            duk_push_c_function(ctx, wave_context.extra_functions->functions[i].function,
              wave_context.extra_functions->functions[i].parameters);
            duk_put_prop_string(ctx, -2, wave_context.extra_functions->functions[i].function_name);
        }

    duk_put_global_string(ctx, "Wave");
    duk_get_global_string(ctx, "Wave");
    duk_set_global_object(ctx);
    register_Time_class(ctx);
    register_Scheduler_class(ctx);
}

WAVE_ERROR WAVE_RUN(WAVE_INSTANCE wave_instance, char* code) {
    duk_context* ctx = duk_create_heap_default();
    FILE* ctxFile;
    char* ctxFilePath = WD();
    strcat(ctxFilePath, "\\context.js");
    ctxFile = fopen(ctxFilePath, "r");
    char ctxCode[MAX_CTX_FILE_SIZE];
    fgets(ctxCode, sizeof(ctxCode), ctxFile);
    wave_bind(ctx, wave_instance.wave_context, wave_instance.type);

    if (duk_peval_string(ctx, code) != 0) {
        return duk_safe_to_string(ctx, -1);
    }

    duk_destroy_heap(ctx);
    return "";
}