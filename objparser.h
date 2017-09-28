#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <istream>
#include <vector>

class ObjParserEvents
{
public:
    struct FaceVertexIndices
    {
        // Indices are the same as in the object file format
        // Using 1-based indexing and allowing negative indices to count from the end.
        // Processing is left to the client

        int geometryIndex;
        int texCoordIndex;
        int normalIndex;

        bool operator==(const FaceVertexIndices &rhs) const
        {
            return rhs.geometryIndex == geometryIndex &&
                    rhs.texCoordIndex == texCoordIndex &&
                    rhs.normalIndex == normalIndex;
        }
    };

    virtual ~ObjParserEvents() {}

    virtual void foundVertexGeometry(float x, float y, float z, float w) = 0;
    virtual void foundVertexNormal(float x, float y, float z) = 0;
    virtual void foundTextureCoordinate(float u, float v, float w) = 0;
    virtual void foundFace(std::vector<FaceVertexIndices> &&v) = 0;
    virtual void foundComment(const std::string &) {}
    virtual void foundUnsupportedLine(const std::string &) {}
};

class ObjParser
{
public:
    void setListener(ObjParserEvents *listener);

    bool parse(std::istream& is);

private:
    bool parseNextLine();

    ObjParserEvents *_listener = nullptr;
};

#endif // OBJPARSER_H
