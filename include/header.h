#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <stdbool.h>
#include <stdarg.h>

#ifndef VIS_C 
#define VIS_C

typedef unsigned int uint;

typedef struct {
    float r, g, b, a;
} Color;

typedef struct {
    float x, y, z;
} vec3;

typedef struct {
    float *vertices;
    uint vertices_count;
    uint *edges;
    uint edges_count;
    bool fillcolor;
    Color color;
    uint VAO, VBO, EBO;
} VisualBody;

typedef struct {
    GLFWwindow *handler; 
    VisualBody *shapes;
    uint shape_count;
    uint shader_program;
    
    //rotate pols
    vec3 rotation_angles;      
    bool is_rotating;          
    double last_mouse_x;       
    double last_mouse_y;       
} Window;

typedef struct {
    float x, y;
} pos_2D;


// helpers
Color createColor(float, float, float, float);
#define COLOR(r,g,b,a) (createColor(r,g,b,a))
VisualBody createVisualBody(float*, uint, uint*, uint, Color, bool);
void DrawVisualBody(VisualBody*, uint);
void setupVisualBodys(Window*);
VisualBody createCube(float, Color, bool);
int addVisualBodys(Window*, uint, ...);
VisualBody createCoorPols();
VisualBody createAxis();
VisualBody createSphere(float, Color, bool);


// shaders
typedef enum {
    VERTEX, FRAGMENT
} ShaderType;
const uint createShader(char*, ShaderType, bool);
uint createProgramm(uint, ...);
void compileShader(Window*, char*, char*);

// init
int SystemInit(void);
int GraphicInit(uint, uint, Color);
Window* VisualInit(uint, uint, const char*, Color);

//main func
void RunGraphic(Window*, bool);
void EndProcess(Window*);

//rotate func
void setupRotation(Window* win); 
void updateRotation(Window* win);
void rotationMatrix(float angle_x, float angle_y, float angle_z, float* matrix); 

//boolean (for cups_lock)
#define TRUE true 
#define FALSE false

//colors


extern const Color WHITE;
extern const Color BLACK;
extern const Color RED;
extern const Color BLUE;
extern const Color GREEN;

//automatisation
#define TRUST_INIT(w, h, tit, c) \
    Window* ___VIS_WINDOW___ = VisualInit(w, h, tit, c); \
    if (!___VIS_WINDOW___) return 1

#define TRUST_RUN() \
    do { \
        setupRotation(___VIS_WINDOW___); \
        RunGraphic(___VIS_WINDOW___, false); \
        EndProcess(___VIS_WINDOW___); \
    } while(0)


#define CREATE_GRID_AXIS(show_grid, show_axis)\
    do {\
        if ((show_axis) && !(show_grid)) \
            addVisualBodys(___VIS_WINDOW___, 1, createAxis()); \
        else if (!(show_axis) && (show_grid)) \
            addVisualBodys(___VIS_WINDOW___, 1, createCoorPols());\
        else if ((show_axis) && (show_grid)) \
            addVisualBodys(___VIS_WINDOW___, 2, createAxis(), createCoorPols());\
    } while(0)
        

#define ADD_SHAPES(...) \
    do {\
        VisualBody arr[] = {__VA_ARGS__}; \
        uint count = sizeof(arr) / sizeof(arr[0]); \
        if (addVisualBodys(___VIS_WINDOW___, count, __VA_ARGS__)) return 1; \
    } while (0) 

#define SHAPE_t(vert, edge, color) \
    do {\
        size_t vert_count = sizeof(vert) / sizeof(vert[0]);\
        size_t edges_count = sizeof(edge) / sizeof(edge[0]);\
        VisualBody s = createVisualBody(vert, vert_count, edge, edges_count, color, false);\
        if (addVisualBodys(___VIS_WINDOW___, 1, s)) return 1; \
    }while(0)


//right shapes 
#define CUBE(side_length, color, fill) createCube(side_length, color, fill)



#endif


