#ifndef GLBATCHBASE_H
#define GLBATCHBASE_H

// This class is a pure virtual class with one single virtual function, Draw().
// The GLBatch class is derived from this.

class GLBatchBase
{
public:

    virtual void draw() = 0;

};

#endif // GLBATCHBASE_H
