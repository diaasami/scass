#include "objparser.h"

using namespace std;

bool ObjParser::parse(std::istream& is)
{
    char firstChar;

    while (is >> firstChar)
    {
        if (firstChar == '#')
        {
            string s;
            getline(is, s);

            if (_listener)
            {
                _listener->foundComment(s);
            }
        }
        else if (firstChar == 'v')
        {
            char secondChar = static_cast<char>(is.peek());

            if (secondChar == ' ')
            {
                float x, y, z, w = 1.0;
                is >> x >> y >> z;

                if (is.peek() == ' ')
                {
                    is >> w;
                }

                if (_listener)
                {
                    _listener->foundVertexGeometry(x, y, z, w);
                }
            }
            else if (secondChar == 't')
            {
                // Read it to get it out of the way
                is >> secondChar;

                float u, v, w = 0.0;
                is >> u >> v;

                if (is.peek() == ' ')
                {
                    is >> w;
                }

                if (_listener)
                {
                    _listener->foundTextureCoordinate(u, v, w);
                }

            }

            else if (secondChar == 'n')
            {
                // Read it to get it out of the way
                is >> secondChar;

                float x, y, z;
                is >> x >> y >> z;

                if (_listener)
                {
                    _listener->foundVertexNormal(x, y, z);
                }
            }
        }
        else if (firstChar == 'f')
        {
            vector<ObjParserEvents::FaceVertexIndices> v;

            // No face has less than 3 vertices, pre-allocate them.
            v.reserve(3);

            while (is.good())
            {
                vector<decltype(ObjParserEvents::FaceVertexIndices::geometryIndex)> indices;

                do
                {
                    decltype(ObjParserEvents::FaceVertexIndices::geometryIndex) i = 0;

                    // If we find another slash, we consider than index a zero.
                    if (is.peek() == '/')
                    {
                        indices.push_back(i);
                    }
                    else if (is >> i)
                    {
                        indices.push_back(i);
                    }
                }
                while (is.peek() == '/' && is.get());

                if (indices.size() == 1)
                {
                    v.push_back(ObjParserEvents::FaceVertexIndices{indices[0], 0, 0});
                }
                else if (indices.size() == 2)
                {
                    v.push_back(ObjParserEvents::FaceVertexIndices{indices[0], indices[1], 0});
                }
                else if (indices.size() == 3)
                {
                    v.push_back(ObjParserEvents::FaceVertexIndices{indices[0], indices[1], indices[2]});
                }
            }

            is.clear();

            if (_listener)
            {
                _listener->foundFace(v);
            }
        }
        else
        {
            string line;

            getline(is, line);

            if (_listener)
            {
                _listener->foundUnsupportedLine(firstChar + line);
            }
        }

    }

    return true;
}

void ObjParser::setListener(ObjParserEvents *listener)
{
    _listener = listener;
}
