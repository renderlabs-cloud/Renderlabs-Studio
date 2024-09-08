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

#ifndef WAVE_H
#define WAVE_H

#include "duktape.h"
typedef struct {
  char *key;
  char *value;
} WAVE_KV;
typedef const char* WAVE_ERROR;
typedef const char* WAVE_STRING;
typedef duk_ret_t (*WAVE_FUNCTION)(duk_context *ctx);
typedef WAVE_KV WAVE_VARIABLE;
typedef struct {
  WAVE_VARIABLE* variables;
  int length;
} WAVE_VARIABLES;
typedef struct {
  WAVE_FUNCTION function;
  WAVE_STRING function_name;
  int parameters;
} WAVE_FUNCTION_PLUS;
typedef struct {
  WAVE_FUNCTION_PLUS* functions;
  int length;
} WAVE_FUNCTIONS;

typedef enum {     //     USE FOR SCRIPTING ONLY
  WAVE_NONE,       // <---/
  WAVE_ANIMATION,  // <--- ALL ANIMATIONS USE THIS
  WAVE_AUDIO,      // <--- ALL AUDIO      USE THIS
  WAVE_SHADER,     // <--- ALL SHADERS    USE THIS
  WAVE_TEXTURE,    // <--- ALL TEXTURES   USE THIS
  WAVE_SPECIAL     // <--- ALL PLUGINS    USE THIS
} WAVE_TYPE;

typedef struct {
    WAVE_VARIABLE VERSION;               // THE CURRENT VERSION OF THE PROGRAM USING WAVE 
    WAVE_FUNCTION getObjectById;         // GET AN OBJECT BY ID (ANIMATION)
    WAVE_FUNCTION getObjectsByClass;     // GET ALL OBJECTS OF A CLASS (ANIMATION)
    WAVE_FUNCTION getObjectsByShape;     // GET ALL OBJECTS OF A SHAPE (ANIMATION)
    WAVE_FUNCTION getAllObjects;         // GET ALL OBJECTS (ANIMATION)
    WAVE_FUNCTION getTime;               // GET THE CURRENT TIME (ANIMATION & AUDIO)
    WAVE_FUNCTION getFrame;              // GET THE CURRENT FRAME (ANIMATION & AUDIO)
    WAVE_FUNCTION getFrameRate;          // GET THE CURRENT FRAME RATE (ANIMATION & AUDIO)
    WAVE_FUNCTION quit;                  // QUIT THE PROGRAM

    WAVE_VARIABLES *extra_variables;     // EXTRA VARIABLES FOR THE PROGRAM USING WAVE
    WAVE_FUNCTIONS *extra_functions;     // EXTRA FUNCTIONS FOR THE PROGRAM USING WAVE
} WAVE_INTERFACE;

typedef struct {
  WAVE_INTERFACE wave_context;
  WAVE_TYPE type;
} WAVE_INSTANCE;

WAVE_ERROR WAVE_RUN(WAVE_INSTANCE wave_instance, char* code);

#endif

/*                                                                                              *\
  . // \\  ! USAGE WARNING !                                                             // \\ .
  .// ! \\ WAVE MUST BE USED CORRECTLY OR YOU WILL BREAK THE ENTIRE ENGINE EXPERIENCE   // ! \\.
\*                                                                                              */