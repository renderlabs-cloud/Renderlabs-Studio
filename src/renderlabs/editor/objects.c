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

#include "types.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Object* sphere(Transform transform, int num_triangles) {
    Object* sphere = (Object*)malloc(sizeof(Object));

    sphere->transform.position[0] = transform.position[0];
    sphere->transform.position[1] = transform.position[1];
    sphere->transform.position[2] = transform.position[2];

    sphere->transform.scale[0] = transform.scale[0];
    sphere->transform.scale[1] = transform.scale[1];
    sphere->transform.scale[2] = transform.scale[2];

    sphere->transform.rotation[0] = transform.rotation[0];
    sphere->transform.rotation[1] = transform.rotation[1];
    sphere->transform.rotation[2] = transform.rotation[2];

    sphere->properties.vertices_count = num_triangles * 3;
    sphere->mesh.vertices = (Vertices*)malloc(sizeof(Vertices) * sphere->properties.vertices_count);

    for (int i = 0; i < sphere->properties.vertices_count; i++) {
        sphere->mesh.vertices[i].x = sin(i) * transform.scale[0];
        sphere->mesh.vertices[i].y = cos(i) * transform.scale[1];
        sphere->mesh.vertices[i].z = 0;
    }

    return sphere;
}

Object* debug_shape() {
    Object* debug = (Object*)malloc(sizeof(Object));
    return debug;
}

Vertices transform_vertex(Vertices v, Transform t) {
    v.x *= t.scale[0];
    v.y *= t.scale[1];
    v.z *= t.scale[2];

    float cosY = cos(t.rotation[1]);
    float sinY = sin(t.rotation[1]);

    if (t.rotation[0]) {
        float x_new = v.x * cosY - v.z * sinY;
        v.x = x_new;
    }

    if (t.rotation[1]) {
        float y_new = v.y;
        v.y = y_new;
    }
    if (t.rotation[0]) {
        float z_new = v.x * sinY + v.z * cosY;
        v.z = z_new;
    }

    v.x += t.position[0];
    v.y += t.position[1];
    v.z += t.position[2];

    return v;
}

Object* cube(Transform transform) {
    Object* cube = (Object*)malloc(sizeof(Object));
    if (!cube)
        return NULL;

    Vertices local_vertices[8]
      = {{0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}};

    cube->properties.vertices_count = 8;
    cube->mesh.vertices = (Vertices*)malloc(sizeof(Vertices) * cube->properties.vertices_count);
    if (!cube->mesh.vertices) {
        free(cube);
        return NULL;
    }

    for (int i = 0; i < cube->properties.vertices_count; ++i) {
        cube->mesh.vertices[i] = transform_vertex(local_vertices[i], transform);
    }

    return cube;
}

void translate_objects(Objects* objects, Transform translation) {
    for (int i = 0; i < objects->length; i++) {
        Object* obj = objects->objects[i];

        obj->transform.position[0] += translation.position[0];
        obj->transform.position[1] += translation.position[1];
        obj->transform.position[2] += translation.position[2];

        obj->transform.scale[0] *= translation.scale[0];
        obj->transform.scale[1] *= translation.scale[1];
        obj->transform.scale[2] *= translation.scale[2];

        obj->transform.rotation[0] += translation.rotation[0];
        obj->transform.rotation[1] += translation.rotation[1];
        obj->transform.rotation[2] += translation.rotation[2];
    }
}