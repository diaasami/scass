#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <istream>

class ObjParserEvents
{
    virtual ~ObjParserEvents() = 0;
    virtual void FoundVertexGeometry(float x, float y, float z, float w) = 0;
};

class ObjParser
{
    ObjParser(const std::istream *input);
    ~ObjParser();

};

#endif // OBJPARSER_H
