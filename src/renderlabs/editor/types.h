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

#ifndef TYPES_H
#define TYPES_H

enum AXIS { X_AXIS, Y_AXIS, Z_AXIS };

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float m[4][4];
} Mat4;

typedef struct {
    float x;
    float y;
    float z;
} Vertices;

typedef struct {
    Vertices* vertices;
} Mesh;

typedef struct {
    float position[3];
    float rotation[3];
    float scale[3];
} Transform;

typedef struct {
    long r;
    long g;
    long b;
} Color;

typedef struct {
    int corner_radius;
    int brightness;
    Color color;
} Modifiers;

typedef struct {
    int vertices_count;
} Properties;

typedef struct {
    Transform transform;
    Mesh mesh;
    Modifiers modifiers;
    Properties properties;
} Object;

typedef struct {
    Object** objects;
    int length;
} Objects;

typedef struct {
    float x;
    float y;
    float z;
} Point;

typedef struct {
    Point** points;
    int length;
} Points;

typedef struct {
    Transform transform;
} Camera;

typedef struct {
    Objects objects;
    Camera camera;
} Scene;

#endif