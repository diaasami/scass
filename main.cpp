#include <iostream>

#include "objparser.h"
#include "objmodel.h"

using namespace std;

int main()
{
    ObjModel model;
    model.load("cube.obj");

    return 0;
}
