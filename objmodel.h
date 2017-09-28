#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <deque>
#include <vector>
#include <unordered_map>
#include <functional>

#include "objparser.h"

class Renderer
{
public:
    virtual ~Renderer() {};
    enum class PrimitiveType {Triangle, Quad};

    virtual void setVertexBuffer(const void *vBuffer) = 0;
    virtual void setIndexBuffer(const void *idxBuffer) = 0;
    virtual void renderCommand(PrimitiveType type, size_t index, size_t length) = 0;
};

class ObjModel : public ObjParserEvents
{
public:
    void setRenderer(Renderer *r);

    bool load(const char *filename);
    void prepareBuffers();
    void makeDrawCalls();

    void foundVertexGeometry(float x, float y, float z, float w);
    void foundVertexNormal(float x, float y, float z);
    void foundTextureCoordinate(float u, float v, float w);
    void foundFace(std::vector<FaceVertexIndices> &&v);

private:
    struct VertexBufferData
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
        // 0-based index, negative means none
        int geometryIndex;
        int texCoordIndex;
        int normalIndex;

        bool operator==(const FullVertexIndices &rhs) const
        {
            return rhs.geometryIndex == geometryIndex &&
                    rhs.texCoordIndex == texCoordIndex &&
                    rhs.normalIndex == normalIndex;
        }
    };

    struct FullVertexIndicesHasher
    {
        std::size_t operator()(const FullVertexIndices &obj) const noexcept
        {
            return std::hash<decltype(obj.geometryIndex)>()(obj.geometryIndex) ^
                    std::hash<decltype(obj.texCoordIndex)>()(obj.texCoordIndex) ^
                    std::hash<decltype(obj.normalIndex)>()(obj.normalIndex);
        }
    };

    void addFaceToBuffers(const std::vector<FaceVertexIndices> &vertices);

    // temporary buffers
    std::deque<VertexGeometry> _geometryBuffer;
    std::deque<VertexTexCoord> _texCoordBuffer;
    std::deque<VertexNormal> _normalBuffer;

    std::deque<std::vector<FaceVertexIndices>> _quads;

    // To find out whether a vertex/tc/normal combo has been referenced before
    // or we need to append it to the vertexBuffer
    std::unordered_map<FullVertexIndices, size_t, FullVertexIndicesHasher> _mapping;

    // Renderer requires vertices and indices to be contiguous in memory, so
    // we use std::vector.
    std::vector<VertexBufferData> _vertexBuffer;
    std::vector<size_t> _indexBuffer;

    // Offset of quads in the index buffer
    size_t _quadsOffset = 0;

    Renderer *_renderer = nullptr;
};

#endif // OBJMODEL_H
