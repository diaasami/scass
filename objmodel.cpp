#include "objmodel.h"

#include <fstream>

using namespace std;

void ObjModel::setRenderer(Renderer *r)
{
    _renderer = r;
}

bool ObjModel::load(const char *filename)
{
    ifstream ifs(filename);

    ObjParser parser;

    parser.setListener(this);

    return parser.parse(ifs);
}

void ObjModel::foundVertexGeometry(float x, float y, float z, float w)
{
    _geometryBuffer.push_back(VertexGeometry{x, y, z, w});
}

void ObjModel::foundVertexNormal(float x, float y, float z)
{
    _normalBuffer.push_back(VertexNormal{x, y, z});
}

void ObjModel::foundTextureCoordinate(float u, float v, float w)
{
    _texCoordBuffer.push_back(VertexTexCoord{u, v, w});
}

void ObjModel::addFaceToBuffers(const std::vector<FaceVertexIndices> &vertices)
{
    for (const auto &v: vertices)
    {
        FullVertexIndices indices{v.geometryIndex - 1, v.texCoordIndex - 1, v.normalIndex - 1};

        decltype(_mapping)::iterator itr;
        if ((itr = _mapping.find(indices)) == _mapping.end())
        {
            VertexBufferData fullVertex = {_geometryBuffer[v.geometryIndex].x,
                                    _geometryBuffer[v.geometryIndex].y,
                                    _geometryBuffer[v.geometryIndex].z,
                                    _geometryBuffer[v.geometryIndex].w};

            if (indices.normalIndex >= 0)
            {
                fullVertex.nx = _normalBuffer[v.normalIndex].x;
                fullVertex.ny = _normalBuffer[v.normalIndex].y;
                fullVertex.nz = _normalBuffer[v.normalIndex].z;
            }

            if (indices.texCoordIndex >= 0)
            {
                fullVertex.u = _texCoordBuffer[v.texCoordIndex].u;
                fullVertex.v = _texCoordBuffer[v.texCoordIndex].v;
                fullVertex.w = _texCoordBuffer[v.texCoordIndex].w;
            }

            _vertexBuffer.push_back(fullVertex);
            _indexBuffer.push_back(_vertexBuffer.size() - 1);
            _mapping[indices] = _vertexBuffer.size() - 1;
        }
        else
        {
            _indexBuffer.push_back(itr->second);
        }
    }
}

void ObjModel::foundFace(const std::vector<FaceVertexIndices> &vertices)
{
    if (3 == vertices.size())
    {
        addFaceToBuffers(vertices);
    }
    else if (4 == vertices.size())
    {
        _quads.push_back(vertices);
    }
}

void ObjModel::prepareBuffers()
{
    _quadsOffset = _indexBuffer.size();

    for (const auto &q: _quads)
    {
        addFaceToBuffers(q);
    }

    // temporary buffers are not needed anymore
    _geometryBuffer.clear();
    _geometryBuffer.shrink_to_fit();
    _texCoordBuffer.clear();
    _texCoordBuffer.shrink_to_fit();
    _normalBuffer.clear();
    _normalBuffer.shrink_to_fit();

    _quads.clear();
    _quads.shrink_to_fit();
}

void ObjModel::makeDrawCalls()
{
    if (_renderer)
    {
        _renderer->setVertexBuffer(&_vertexBuffer[0]);
        _renderer->setIndexBuffer(&_indexBuffer[0]);

        _renderer->renderCommand(Renderer::PrimitiveType::Triangle, 0, _quadsOffset);
        _renderer->renderCommand(Renderer::PrimitiveType::Quad, _quadsOffset, _indexBuffer.size() - _quadsOffset);
    }
}
