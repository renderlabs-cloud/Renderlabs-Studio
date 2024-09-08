/*
#include <epoxy/gl.h>
#include <gtk/gtk.h>
#include <math.h>

const GLchar *FRAGMENT_SOURCE = "#version 330\n"
                                "in vec4 inputColor;\n"
                                "out vec4 outputColor;\n"
                                "void main() {\n"
                                "outputColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                                "}";
const GLchar *VERTEX_SOURCE = "#version 330\n"
                              "in vec4 position;\n"
                              "void main()\n{gl_Position =  position;\n"
                              "}";

GLuint VBO, VAO;
static GLuint program;
static GLuint position_buffer;

float *verticesf(Vertices *vertices, int size) {
  float *flat = malloc(size * 3 * sizeof(float));
  if (!flat) {
    printf("Failed to allocate memory for vertices");
    return NULL;
  }

  for (int i = 0; i < size; ++i) {
    flat[i * 3] = isnan(vertices[i].x) ? 100.0f : vertices[i].x;
    flat[i * 3 + 1] = isnan(vertices[i].y) ? 100.0f : vertices[i].y;
    flat[i * 3 + 2] = isnan(vertices[i].z) ? 100.0f : vertices[i].z;
  }
  return flat;
}

void print_matrix(Mat4 *m) {
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (isnan(m->m[i][j])) {
        m->m[i][j] = 5;
      }
      printf("%f,", m->m[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void set_perspective_projection(GLuint program, GLfloat fov, GLfloat aspect,
                                GLfloat near, GLfloat far) {
  if (fov <= 0.0f || aspect <= 0.0f || near <= 0.0f || far <= 0.0f ||
      near >= far) {
    printf("Invalid parameters for perspective projection.\n");
    return;
  }

  GLfloat rad_fov = fov * M_PI / 180.0f;

  if (rad_fov <= 0.0f || rad_fov >= M_PI) {
    printf("FOV angle out of range.\n");
    return;
  }

  GLfloat f = 1.0f / tanf(rad_fov / 2.0f);

  GLfloat proj_matrix[16] = {f / aspect,
                             0.0f,
                             0.0f,
                             0.0f,
                             0.0f,
                             f,
                             0.0f,
                             0.0f,
                             0.0f,
                             0.0f,
                             (far + near) / (near - far),
                             -1.0f,
                             0.0f,
                             0.0f,
                             (2.0f * far * near) / (near - far),
                             0.0f};

  glUseProgram(program);
  GLint proj_matrix_location = glGetUniformLocation(program, "projection");
  glUniformMatrix4fv(proj_matrix_location, 1, GL_FALSE, proj_matrix);
}

static GLuint create_shader(int type) {
  GLuint shader;
  int status;
  shader = glCreateShader(type);
  if (type == GL_FRAGMENT_SHADER) {
    glShaderSource(shader, 1, &FRAGMENT_SOURCE, NULL);
  }
  if (type == GL_VERTEX_SHADER) {
    glShaderSource(shader, 1, &VERTEX_SOURCE, NULL);
  }
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    int log_len;
    char *buffer;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
    buffer = g_malloc(log_len + 1);
    glGetShaderInfoLog(shader, log_len, NULL, buffer);
    g_warning("Compile failure in %s shader:\n%s",
              type == GL_VERTEX_SHADER ? "vertex" : "fragment", buffer);
    g_free(buffer);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

static void init_shaders(GLuint *program_out) {
  GLuint vertex, fragment;
  int status;
  vertex = create_shader(GL_VERTEX_SHADER);

  if (vertex == 0) {
    *program_out = 0;
    return;
  }

  fragment = create_shader(GL_FRAGMENT_SHADER);

  if (fragment == 0) {
    glDeleteShader(vertex);
    *program_out = 0;
    return;
  }

  program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);

  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    int log_len;
    char *buffer;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

    buffer = g_malloc(log_len + 1);
    glGetProgramInfoLog(program, log_len, NULL, buffer);

    g_warning("Linking failure:\n%s", buffer);

    g_free(buffer);

    glDeleteProgram(program);
    program = 0;

    goto out;
  }

  glDetachShader(program, vertex);
  glDetachShader(program, fragment);

  GLint success;
  GLchar infoLog[512];

  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    printf("Vertex Shader Compilation Error: %s\n", infoLog);
  }

  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    printf("Fragment Shader Compilation Error: %s\n", infoLog);
  }

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    printf("Program Linking Error: %s\n", infoLog);
  }

out:
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  if (program_out != NULL)
    *program_out = program;
}

static void init_buffers(GLuint *vao_out, GLuint *buffer_out,
                         GLfloat *vertex_data) {
  GLuint vao, buffer;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if (vao_out != NULL)
    *vao_out = vao;

  if (buffer_out != NULL)
    *buffer_out = buffer;
}

static void realize(GtkWidget *widget) {
  GdkGLContext *context;
  gtk_gl_area_make_current(GTK_GL_AREA(widget));
  if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL)
    return;
  context = gtk_gl_area_get_context(GTK_GL_AREA(widget));
  init_buffers(&position_buffer, NULL, NULL);
  init_shaders(&program);
}

static void unrealize(GtkWidget *widget) {
  gtk_gl_area_make_current(GTK_GL_AREA(widget));

  if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL)
    return;

  glDeleteBuffers(1, &program);
  glDeleteProgram(program);
}

static void draw_triangle(void) {
glUseProgram(program);

glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);/
glDrawArrays(GL_TRIANGLES, 0, 3);

glDisableVertexAttribArray(0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glUseProgram(0);
}

static gboolean render(GtkGLArea *gl_area, GdkGLContext *context) {
  printf("rendering\n");
  gtk_gl_area_make_current(gl_area);

  if (gtk_gl_area_get_error(gl_area) != NULL) {
    g_warning("OpenGL context is not current");
    return FALSE;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, 800, 600);

  Object **test_s = malloc(sizeof(Object *));
  if (test_s == NULL) {
    fprintf(stderr, "Failed to allocate memory for test_s\n");
    return FALSE;
  }
  draw_triangle();

  Transform test_s_t = {
      .position = {1, 1, 1}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}};
  test_s[0] = debug_shape(test_s_t);

  Scene scene = {.camera = {.transform = {.position = {1, 1, 1},
                                          .rotation = {0, 0, 0},
                                          .scale = {1, 1, 1}}},
                 .objects = {.length = 1, .objects = test_s}};

  glUseProgram(program);
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  set_perspective_projection(program, 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

  Mat4 view = {.m = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, -5}, {0, 0, 0, 1}}};
  GLint view_location = glGetUniformLocation(program, "view");
  glUniformMatrix4fv(view_location, 1, GL_FALSE, (const GLfloat *)view.m);

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  for (int i = 0; i < scene.objects.length; ++i) {
    Object *object = scene.objects.objects[i];

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    float *vertices =
        verticesf(object->mesh.vertices, object->properties.vertices_count);
    if (vertices == NULL) {
      fprintf(stderr, "Failed to flatten vertices\n");
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      continue;
    }

    glBufferData(GL_ARRAY_BUFFER,
                 object->properties.vertices_count * 3 * sizeof(float),
                 vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    free(vertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Mat4 model = {
        .m = {{1, 0, 0, 0},
              {0, 1, 0, 0},
              {0, 0, 1, 0},
              {object->transform.position[0], object->transform.position[1],
               object->transform.position[2], 1}}};

    GLint model_location = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(model_location, 1, GL_FALSE, (const GLfloat *)model.m);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, object->properties.vertices_count / 3);
    glBindVertexArray(0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      printf("OpenGL Error: %d\n", err);
    }
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  return TRUE;
}

static gboolean render_callback(gpointer user_data) {
  GtkGLArea *gl_area = GTK_GL_AREA(user_data);
  gtk_gl_area_queue_render(gl_area);

  return TRUE;
}

int testery(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Renderlabs Test");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  GtkWidget *gl_area = gtk_gl_area_new();
  gtk_container_add(GTK_CONTAINER(window), gl_area);

  g_signal_connect(window, "destroy", G_CALLBACK(exit), NULL);
  g_signal_connect(gl_area, "realize", G_CALLBACK(realize), NULL);
  g_signal_connect(gl_area, "unrealize", G_CALLBACK(unrealize), NULL);
  g_signal_connect(gl_area, "render", G_CALLBACK(render), NULL);

  g_timeout_add(16, render_callback, gl_area);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
* /

/* OpenGL Area
 *
 * GtkGLArea is a widget that allows custom drawing using OpenGL calls.
 */
#include "../release.h"
#include "objects.h"
#include "types.h"
#include <epoxy/gl.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

const GLchar* FRAGMENT_SOURCE
  = "#version 330\n"
    "in vec4 inputColor;\n"
    "out vec4 outputColor;\n"
    "void main() {\n"
    "outputColor = vec4(1.0f, 0.0f, 0.0f, 0.0f);\n" // constant red. I know it's
                                                    // a poor shader
    "}";
const GLchar* VERTEX_SOURCE = "#version 330\n"
                              "in vec4 position;\n"
                              "void main()\n{gl_Position =  position;\n"
                              "}";

/* the GtkGLArea widget */
static GtkWidget* gl_area = NULL;

float* verticesf(Vertices* vertices, int size) {
    float* flat = malloc(size * 3 * sizeof(float));
    if (!flat) {
        printf("Failed to allocate memory for vertices");
        return NULL;
    }

    for (int i = 0; i < size; ++i) {
        flat[i * 3] = isnan(vertices[i].x) ? 100.0f : vertices[i].x;
        flat[i * 3 + 1] = isnan(vertices[i].y) ? 100.0f : vertices[i].y;
        flat[i * 3 + 2] = isnan(vertices[i].z) ? 100.0f : vertices[i].z;
    }
    return flat;
}

void print_vertices_array(const Vertices* vertices, size_t count) {
    if (vertices == NULL) {
        printf("Invalid vertices pointer.\n");
        return;
    }

    printf("Vertices Array:\n");
    for (size_t i = 0; i < count; ++i) {
        printf("Vertex %zu:\n", i);
        printf("  x: %f\n", vertices[i].x);
        printf("  y: %f\n", vertices[i].y);
        printf("  z: %f\n", vertices[i].z);
    }
}

void print_float_array(const float* vertices, size_t count) {
    if (vertices == NULL) {
        printf("Invalid vertices pointer.\n");
        return;
    }

    printf("Floats Vertices Array:\n");
    for (size_t i = 0; i < count; ++i) {
        printf("Vertex %zu:\n", i);
        printf("  %lli: %f\n", i * 3, vertices[i]);
        printf("  %lli: %f\n", i * 3 + 1, vertices[i]);
        printf("  %lli: %f\n", i * 3 + 2, vertices[i]);
    }
}

/* Initialize the GL buffers */
static void init_buffers(GLuint* vao_out, GLuint* buffer_out) {
    GLuint vao, buffer;

    /* We only use one VAO, so we always keep it bound */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* This is the buffer that holds the vertices */
    glGenBuffers(1, &buffer);

    if (vao_out != NULL)
        *vao_out = vao;

    if (buffer_out != NULL)
        *buffer_out = buffer;
}

/* Create and compile a shader */
static GLuint create_shader(int type) {
    GLuint shader;
    int status;
    shader = glCreateShader(type);
    if (type == GL_FRAGMENT_SHADER) {
        glShaderSource(shader, 1, &FRAGMENT_SOURCE, NULL);
    }
    if (type == GL_VERTEX_SHADER) {
        glShaderSource(shader, 1, &VERTEX_SOURCE, NULL);
    }
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int log_len;
        char* buffer;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        buffer = g_malloc(log_len + 1);
        glGetShaderInfoLog(shader, log_len, NULL, buffer);
        g_warning("Compile failure in %s shader:\n%s",
          type == GL_VERTEX_SHADER ? "vertex" : "fragment", buffer);
        g_free(buffer);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

/* Initialize the shaders and link them into a program */
static void init_shaders(GLuint* program_out) {
    GLuint vertex, fragment;
    GLuint program = 0;
    int status;
    vertex = create_shader(GL_VERTEX_SHADER);

    if (vertex == 0) {
        *program_out = 0;
        return;
    }

    fragment = create_shader(GL_FRAGMENT_SHADER);

    if (fragment == 0) {
        glDeleteShader(vertex);
        *program_out = 0;
        return;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        int log_len;
        char* buffer;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

        buffer = g_malloc(log_len + 1);
        glGetProgramInfoLog(program, log_len, NULL, buffer);

        g_warning("Linking failure:\n%s", buffer);

        g_free(buffer);

        glDeleteProgram(program);
        program = 0;

        goto out;
    }

    glDetachShader(program, vertex);
    glDetachShader(program, fragment);

out:
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (program_out != NULL)
        *program_out = program;
}

static GLuint position_buffer;
static GLuint program;

/* We need to set up our state when we realize the GtkGLArea widget */
static void realize(GtkWidget* widget) {
    GdkGLContext* context;
    gtk_gl_area_make_current(GTK_GL_AREA(widget));
    if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL)
        return;
    context = gtk_gl_area_get_context(GTK_GL_AREA(widget));
    init_buffers(&position_buffer, NULL);
    init_shaders(&program);
}

/* We should tear down the state when unrealizing */
static void unrealize(GtkWidget* widget) {
    gtk_gl_area_make_current(GTK_GL_AREA(widget));

    if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL)
        return;

    glDeleteBuffers(1, &position_buffer);
    glDeleteProgram(program);
}

int y = 0; // Debugging only
static void draw(void) {
    printf("rendering\n");
    glUseProgram(program);

    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    Object* cuber
      = cube((Transform) {.position = {2, 2, 0}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}});
    int vertex_count = cuber->properties.vertices_count;
    float* temp_vertex_data = verticesf(cuber->mesh.vertices, vertex_count);
    float vertex_data[] = {temp_vertex_data[0], temp_vertex_data[1], temp_vertex_data[2],
      temp_vertex_data[3], temp_vertex_data[4], temp_vertex_data[5], temp_vertex_data[6],
      temp_vertex_data[7], temp_vertex_data[8]};

    printf("Size:%i\n", vertex_count);
    print_float_array(vertex_data, 9 * 3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

static gboolean render(GtkGLArea* area, GdkGLContext* context) {
    if (gtk_gl_area_get_error(area) != NULL)
        return FALSE;

    /* Clear the viewport */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw our object */
    draw();

    /* Flush the contents of the pipeline */
    glFlush();

    return TRUE;
}

static gboolean render_callback(gpointer user_data) {
    GtkGLArea* gl_area = GTK_GL_AREA(user_data);
    gtk_gl_area_queue_render(gl_area);

    return TRUE;
}

static void on_axis_value_change(void) { gtk_widget_queue_draw(gl_area); }

int testery(int argc, char** argv) {
    GtkWidget *window, *box;
    /* initialize gtk */
    gtk_init(&argc, &argv);
    /* Create new top level window. */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 1000);
    gtk_window_set_title(GTK_WINDOW(window), "GL Area");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, FALSE);
    g_object_set(box, "margin", 12, NULL);
    gtk_box_set_spacing(GTK_BOX(box), 6);
    gtk_container_add(GTK_CONTAINER(window), box);
    gl_area = gtk_gl_area_new();
    gtk_box_pack_start(GTK_BOX(box), gl_area, 1, 1, 0);
    /* We need to initialize and free GL resources, so we use
     * the realize and unrealize signals on the widget
     */
    g_signal_connect(gl_area, "realize", G_CALLBACK(realize), NULL);
    g_signal_connect(gl_area, "unrealize", G_CALLBACK(unrealize), NULL);

    /* The main "draw" call for GtkGLArea */
    g_signal_connect(gl_area, "render", G_CALLBACK(render), NULL);
    g_timeout_add(160, render_callback, gl_area);
    /* Quit form main if got delete event */
    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_main();

    return 0;
}