#include "objmodel.h"

#include <fstream>

using namespace std;

bool ObjModel::load(const char *filename)
{
    ifstream ifs(filename);

    ObjParser parser;

    parser.setListener(this);

    return parser.parse(ifs);
}

void ObjModel::foundVertexGeometry(float x, float y, float z, float w)
{
    //_geometryBuffer.push_back(VertexGeometry<)
}

/*
virtual void foundVertexNormal(float x, float y, float z);
virtual void foundTextureCoordinate(float u, float v, float w);
virtual void foundFace(const std::vector<FaceVertexIndices> &v);
*/
