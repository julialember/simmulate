#include "header.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

VisualBody createCube(float side_length, Color color, bool fill) {
    static float vertices[24];
    static uint indices[24];
    
    float len = side_length/2;
    
    float temp_verts[] = {
        -len, -len, -len, len, -len, -len, len, len, -len, -len, len, -len,
        -len, -len, len, len, -len, len, len, len, len, -len, len, len
    };
    uint temp_indices[] = {
        0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7
    };
    
    memcpy(vertices, temp_verts, sizeof(temp_verts));
    memcpy(indices, temp_indices, sizeof(temp_indices));
    
    return createVisualBody(vertices, 24, indices, 24, color, fill);
}

VisualBody createCoorPols() {
#ifndef GRID_SIZE
#define GRID_SIZE 10
#endif
#ifndef GRID_SPACING  
#define GRID_SPACING 0.2f
#endif
    static float grid_vertices[GRID_SIZE * 3 * 4];
    static uint grid_edges[GRID_SIZE * 4 * 2];
    
    int vertex_index = 0;
    int edge_index = 0;
    
    for (int z = 0; z < GRID_SIZE; z++) {
        float z_pos = (z - (float)GRID_SIZE/2) * GRID_SPACING;
        
        grid_vertices[vertex_index++] = -1.0f;
        grid_vertices[vertex_index++] = -0.4f;
        grid_vertices[vertex_index++] = z_pos;
        
        grid_vertices[vertex_index++] = 1.0f;
        grid_vertices[vertex_index++] = -0.4f;
        grid_vertices[vertex_index++] = z_pos;
        
        grid_edges[edge_index++] = z * 2;
        grid_edges[edge_index++] = z * 2 + 1;
    }
    
    for (int x = 0; x < GRID_SIZE; x++) {
        float x_pos = (x - (float)GRID_SIZE/2) * GRID_SPACING;
        
        grid_vertices[vertex_index++] = x_pos;
        grid_vertices[vertex_index++] = -0.4f;
        grid_vertices[vertex_index++] = -1.0f;
        
        grid_vertices[vertex_index++] = x_pos;
        grid_vertices[vertex_index++] = -0.4f;
        grid_vertices[vertex_index++] = 1.0f;
        
        grid_edges[edge_index++] = GRID_SIZE * 2 + x * 2;
        grid_edges[edge_index++] = GRID_SIZE * 2 + x * 2 + 1;
    }
    
    return createVisualBody(grid_vertices, vertex_index, grid_edges, edge_index, WHITE, false);
}

VisualBody createAxis() {
    static float axes_vertices[] = {
        -1.0f, 0, 0.0f,
         1.0f, 0, 0.0f,
        
        0.0f, 1, 0.0f,
        0.0f, -1, 0.0f,
        
        0.0f, 0, -1.0f,
        0.0f, 0,  1.0f,
    };
    
    static uint axes_edges[] = {
        0, 1,  
        2, 3,  
        4, 5  
    };

    return createVisualBody(axes_vertices, 18, axes_edges, 6, RED, false);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Window* win = glfwGetWindowUserPointer(window);
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        win->is_rotating = (action == GLFW_PRESS);
        if (win->is_rotating) 
            glfwGetCursorPos(window, &win->last_mouse_x, &win->last_mouse_y);
        
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Window* win = glfwGetWindowUserPointer(window);
    if (win->is_rotating) {
        double dx = xpos - win->last_mouse_x;
        double dy = ypos - win->last_mouse_y;
        
        win->rotation_angles.y += dx * 0.01f; 
        win->rotation_angles.x += dy * 0.01f;
        
        win->last_mouse_x = xpos;
        win->last_mouse_y = ypos;
    }
}

void rotationMatrix(float angle_x, float angle_y, float angle_z, float* matrix) {
    float cx = cosf(angle_x), sx = sinf(angle_x);
    float cy = cosf(angle_y), sy = sinf(angle_y); 
    float cz = cosf(angle_z), sz = sinf(angle_z);
    
    float temp[16] = {
        cy*cz, cz*sx*sy - cx*sz,    cx*cz*sy + sx*sz,    0,
        cy*sz, cx*cz + sx*sy*sz,    -cz*sx + cx*sy*sz,   0,
        -sy,   cy*sx,               cx*cy,               0,
        0,     0,                   0,                   1
    };
    
    memcpy(matrix, temp, sizeof(temp));
}

void setupRotation(Window* win) {
    glfwSetWindowUserPointer(win->handler, win);
    
    glfwSetMouseButtonCallback(win->handler, mouse_button_callback);
    glfwSetCursorPosCallback(win->handler, cursor_position_callback);
    
    win->rotation_angles = (vec3){0, 0, 0};
    win->is_rotating = false;
}

void updateRotation(Window* win) {
    float rotation_matrix[16];
    
    rotationMatrix(win->rotation_angles.x, win->rotation_angles.y, win->rotation_angles.z, rotation_matrix);
    
    glUseProgram(win->shader_program);
    
    int rotation_loc = glGetUniformLocation(win->shader_program, "rotation");
    glUniformMatrix4fv(rotation_loc, 1, GL_FALSE, rotation_matrix);
}

Color createColor(float r, float g, float b, float alpha) {
    return (Color){
    .r = r,
    .g = g, 
    .b = b,
    .a = alpha, };
}

const uint createShader(char *code, ShaderType type, bool fromfile) {
    uint answer;    
    answer = glCreateShader(type == VERTEX ?
            GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
    
    const char *code_const = code;
    glShaderSource(answer, 1, &code_const, NULL);
    glCompileShader(answer);
    if (fromfile) free(code);
    return answer;
}

void compileShader(Window* win, char *vertex, char *fragment) {
    uint vertex_shader = createShader(vertex, VERTEX, false);
    uint fragment_shader = createShader(fragment, FRAGMENT, false);

    win->shader_program = createProgramm(2, vertex_shader, fragment_shader);
}

inline uint createProgramm(uint count, ...) {
    va_list args;
    uint programm = glCreateProgram();
    va_start(args, count);
    while (count-- > 0) {
        uint next_shader = va_arg(args, uint);
        if (next_shader != 0) {
            glAttachShader(programm, next_shader);
            glDeleteShader(next_shader);
        }
    }
    va_end(args);
    glLinkProgram(programm);
    return programm;
}

int addVisualBodys(Window* win, uint shape_count, ...) {
    if (!win) return 1;
    
    uint old_count = win->shape_count;
    uint new_count = old_count + shape_count;
    
    void *temp = realloc(win->shapes, sizeof(VisualBody) * new_count);
    if (!temp) {
        fprintf(stderr, "error with mem_allocate\n");   
        return 1;
    }
    win->shapes = (VisualBody*)temp;
    
    va_list args;
    va_start(args, shape_count);
    for(uint i = 0; i < shape_count; i++) {
        win->shapes[old_count + i] = va_arg(args, VisualBody);
    }
    va_end(args);
    
    win->shape_count = new_count;
    
    return 0;
}

inline void EndProcess(Window* pr) {
    if (!pr) return;

    for (uint i = 0; i < pr->shape_count; i++) {
        VisualBody *shape = &pr->shapes[i];
        
        glDeleteVertexArrays(1, &shape->VAO);
        glDeleteBuffers(1, &shape->VBO);
        if (shape->EBO != 0) {
            glDeleteBuffers(1, &shape->EBO);
        }
    }

    free(pr->shapes);

    if (pr->shader_program != 0) {
        glDeleteProgram(pr->shader_program);
    }

    if (pr->handler) {
        glfwDestroyWindow(pr->handler);
    }

    free(pr);
    glfwTerminate();
}

VisualBody createVisualBody(float *vertices, uint vc, uint *edges, uint ec,  Color color, bool fillcolor) {
    return (VisualBody) {
    .edges = edges,
    .vertices = vertices,
    .fillcolor = fillcolor,
    .vertices_count = vc,
    .edges_count = ec,
    .color = color,
    };
}

inline int SystemInit(void) {
    if (!glfwInit()) {
        fprintf(stderr, "error with system init\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);
    return 0;
}

inline int GraphicInit(uint wei, uint hei, Color color) {
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "error with init\n");
        return 1;
    }
    glViewport(0, 0, wei, hei);
    glClearColor(color.r, color.g, color.b, color.a);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return 0;
}

Window* VisualInit(uint w, uint h, const char* title, Color c) {
    if (SystemInit()) return NULL;
    
    Window* win = malloc(sizeof(Window));
    if (!win) {
        fprintf(stderr, "error with mem allocate");
        return NULL;
    }

    win->handler = glfwCreateWindow(w, h, title, NULL, NULL);
    if (!win->handler) {
        fprintf(stderr, "error with window create\n");
        free(win);
        return NULL;
    }
    win->shapes = NULL;
    win->shader_program = 0;
    win->shape_count = 0;
    
    win->rotation_angles = (vec3){0, 0, 0};
    win->is_rotating = false;
    
    glfwMakeContextCurrent(win->handler);
    if(GraphicInit(w, h, c)) {
        EndProcess(win);
        return NULL;
    };
    return win;
}

void DrawVisualBody(VisualBody *shape, uint shader_program) {
    
    int color_loc = glGetUniformLocation(shader_program, "shapeColor");
    glUniform4f(color_loc, shape->color.r, shape->color.g, shape->color.b, shape->color.a);
    
    glBindVertexArray(shape->VAO);
    if (shape->edges && shape->edges_count > 0) 
        glDrawElements(shape->fillcolor ? GL_TRIANGLES : GL_LINES, shape->edges_count, GL_UNSIGNED_INT, 0);
    else 
        glDrawArrays(GL_POINTS, 0, shape->vertices_count / 3);

    glBindVertexArray(0);
}

inline void RunGraphic(Window* win, bool shouldclose) {
    char* vertex_shader_source = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 rotation;\n"  
        "uniform vec4 shapeColor;\n"
        "void main() {\n"
        "   gl_Position = rotation * vec4(aPos, 1.0);\n"
        "}\0";
        
    char* fragment_shader_source = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 shapeColor;\n"
        "void main() {\n"
        "   FragColor = shapeColor;\n"
        "}\0";
    
    compileShader(win, vertex_shader_source, fragment_shader_source);
    setupVisualBodys(win);
    
    while (!shouldclose && !glfwWindowShouldClose(win->handler)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        updateRotation(win);
        
        for (uint i = 0; i < win->shape_count; i++) {
            DrawVisualBody(&win->shapes[i], win->shader_program);
        }
        
        glfwSwapBuffers(win->handler);
        glfwPollEvents();
        
        if (glfwGetKey(win->handler, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            shouldclose = true;
        }
    }
}

void setupVisualBodys(Window *win) {
    if (!win->shapes || win->shape_count <= 0) return;
    
    for (uint i = 0; i < win->shape_count; i++) {
        VisualBody *shape = &win->shapes[i];
        
        glGenVertexArrays(1, &shape->VAO);
        glGenBuffers(1, &shape->VBO);
        if (shape->edges_count > 0) {
            glGenBuffers(1, &shape->EBO);
        }
        
        glBindVertexArray(shape->VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, shape->VBO);
        glBufferData(GL_ARRAY_BUFFER, 
                     shape->vertices_count * sizeof(float),
                     shape->vertices, 
                     GL_STATIC_DRAW);
        
        if (shape->edges_count > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         shape->edges_count * sizeof(uint),
                         shape->edges,
                         GL_STATIC_DRAW);
        }
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                             3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);
    }
}
