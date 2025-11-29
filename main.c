#include "include/Header.h"

int main(void) {
    TRUST_INIT(900, 900, "3D Shapes with Wireframes", BLACK);

    float pyramid_vertices[] = {
    -0.15f, -0.15f, -0.15f, 
     0.15f, -0.15f, -0.15f, 
     0.15f, -0.15f,  0.15f, 
    -0.15f, -0.15f,  0.15f,
    
     0.0f,   0.15f,  0.0f, 
    };

    uint pyramid_edges[] = {
        0, 1,
        1, 2,  
        2, 3, 
        3, 0,
        
        4, 0,  
        4, 1,  
        4, 2,  
        4, 3, 
    };

    
    CREATE_GRID_AXIS(true, false);
    SHAPE_t(pyramid_vertices, pyramid_edges, BLUE);
    ADD_SHAPES(CUBE(1.0f, BLUE, false));
    TRUST_RUN();
    
    return 0;
}
