#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <deque>
#include <vector>
#include <map>

#include "objparser.h"

class Renderer
{
public:
    virtual ~Renderer();
    enum class PrimitiveType {Triangle, Quad};

    virtual void setVertexBuffer(const void *vBuffer) = 0;
    virtual void setIndexBuffer(const void *idxBuffer) = 0;
    virtual void issueCommand(PrimitiveType type, size_t index, size_t length) = 0;
};

class ObjModel : public ObjParserEvents
{
public:
    bool load(const char *filename);
    void setRendered(Renderer *r);
    void prepareBuffers();
    void makeDrawCalls();

    virtual void foundVertexGeometry(float x, float y, float z, float w);
    virtual void foundVertexNormal(float x, float y, float z);
    virtual void foundTextureCoordinate(float u, float v, float w);
    virtual void foundFace(const std::vector<FaceVertexIndices> &v);

private:
    struct FullVertex
    {
        float x, y, z, w;
        float u, v, p;
        float nx, ny, nz;
    };

    struct VertexGeometry
    {
        float x, y, z, w;
    };

    struct VertexNormal
    {
        float x, y, z;
    };

    struct VertexTexCoord
    {
        float u, v, w;
    };

    struct FullVertexIndices
    {
        unsigned int geometryIndex;
        unsigned int texCoordIndex;
        unsigned int normalIndex;
    };

    std::deque<VertexGeometry> _geometryBuffer;
    std::deque<VertexTexCoord> _texCoordBuffer;
    std::deque<VertexNormal> _normalBuffer;

    std::map<FullVertexIndices, size_t> mapping;
    std::vector<FullVertex> _vertexBuffer;
    std::vector<size_t> _indexBuffer;
};

#endif // OBJMODEL_H
