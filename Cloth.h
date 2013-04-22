#ifndef CLOTH_H
#define CLOTH_H

//****************************************************
// Cloth Header Definition
//****************************************************

class Cloth {
  private:
    int width;   // Number of Vertices
    int height;  // Number of Vertices 

    // Vertices:
    // 2D array of Vertices 
    
    Vertex** vertices;
           
    // Color: 

    // Texture

    // Bool Tearable
    // Bool Wireframe

    // Private Functions:
//    initializeDefaultVertices();
    initializeTestCloth();


  public:
    // Constructors:
    Cloth();
    // TODO: Constructor Takes a list of Vertices
    // Constructor Options:
    // Spring Constants
    // Stretch, Shear, Bend Constants 

    Cloth(int w, int h);


    // Getters:
    int getWidth() { return width; };;
    int getHeight() { return height; };    
    Vertex* getVertex(int i, int j) { return vertices[width][height] };


}

#endif
