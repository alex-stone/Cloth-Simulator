#ifndef SPRING_H
#define SPRING_H

//****************************************************
// Spring Header Definition
//****************************************************

class Spring {
  private:
    Vertex point1;
    Vertex point2;

    float springConstant;
    float distance;
    // Stretch, Shear, Bend


  public:
    // Constructors:
    Spring();
    Spring(Vertex v1, Vertex v2);
    
    // Functions: 
    float getForce();
    float getDistance();

};
#endif
