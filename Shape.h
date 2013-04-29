#ifndef SHAPE_H
#define SHAPE_H

//****************************************************
// Shape Header Definition
//****************************************************

class Shape {
  public:
    // If Collides return true and updates Vertex, else return false
    virtual bool collide(Vertex* v);
    

}


