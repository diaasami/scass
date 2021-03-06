#include "gtest/gtest.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "objparser.h"
#include "objmodel.h"

#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

class ObjParserEventsMock : public ObjParserEvents
{
public:
    // HACK: since gmock does not support matching against rvalue refs.
    void foundFace(std::vector<FaceVertexIndices> &&v) {foundFace_ref(v);}

    MOCK_METHOD4(foundVertexGeometry, void(float x, float y, float z, float w));
    MOCK_METHOD3(foundVertexNormal, void(float x, float y, float z));
    MOCK_METHOD3(foundTextureCoordinate, void(float u, float v, float w));
    MOCK_METHOD1(foundFace_ref, void(const std::vector<FaceVertexIndices> &v));
    MOCK_METHOD1(foundComment, void(const std::string &));
    MOCK_METHOD1(foundUnsupportedLine, void(const std::string &));
};

class RendererMock : public Renderer
{
public:
    MOCK_METHOD1(setVertexBuffer, void(const void *vBuffer));
    MOCK_METHOD1(setIndexBuffer, void(const void *idxBuffer));
    MOCK_METHOD3(renderCommand, void(PrimitiveType type, size_t index, size_t length));
};

using namespace std;
using ::testing::_;
using ::testing::AtLeast;

TEST(ObjParserTest, ParsingEmptyFile_ShouldNotCrash)
{
    ObjParser op;

    istringstream iss("");
    op.parse(iss);
}

TEST(ObjParserTest, Triggering_foundVertexGeometry)
{
    ObjParser op;

    istringstream iss("v 1.0 2.0 3.0 4.0\n");

    ObjParserEventsMock listener;
    op.setListener(&listener);
    EXPECT_CALL(listener, foundVertexGeometry(1.0f, 2.0f, 3.0f, 4.0f)).Times(1);

    op.parse(iss);
}

TEST(ObjParserTest, Triggering_foundVertexNormal)
{
    ObjParser op;

    istringstream iss("vn -1.0 -2.0 -3.0");

    ObjParserEventsMock listener;
    op.setListener(&listener);
    EXPECT_CALL(listener, foundVertexNormal(-1.0f, -2.0f, -3.0f)).Times(1);

    op.parse(iss);
}

TEST(ObjParserTest, Triggering_foundTextureCoordinate)
{
    ObjParser op;

    istringstream iss("vt 5.0 6.0 7.0");

    ObjParserEventsMock listener;
    op.setListener(&listener);
    EXPECT_CALL(listener, foundTextureCoordinate(5.0f, 6.0f, 7.0f)).Times(1);

    op.parse(iss);
}

TEST(ObjParserTest, Triggering_foundFace)
{
    ObjParser op;

    istringstream iss("f 1/1/1 2/2/1 3/3/1");


    ObjParserEventsMock listener;
    op.setListener(&listener);

    const vector<ObjParserEvents::FaceVertexIndices> v{{1, 1, 1}, {2, 2, 1}, {3, 3, 1}};
    EXPECT_CALL(listener, foundFace_ref(v)).Times(1);

    op.parse(iss);
}

TEST(ObjParserTest, Triggering_foundComment)
{
    ObjParser op;

    istringstream iss("# comment");

    ObjParserEventsMock listener;
    op.setListener(&listener);
    EXPECT_CALL(listener, foundComment(" comment")).Times(1);

    op.parse(iss);
}

TEST(ObjParserTest, Triggering_WholeFile_Cube)
{
    ObjParser op;
    ifstream ifs("cube.obj");

    ObjParserEventsMock listener;
    op.setListener(&listener);
    EXPECT_CALL(listener, foundVertexGeometry(_, _, _, _)).Times(8);
    EXPECT_CALL(listener, foundVertexNormal(_, _, _)).Times(6);
    EXPECT_CALL(listener, foundTextureCoordinate(_, _, _)).Times(4);
    EXPECT_CALL(listener, foundFace_ref(_)).Times(12);

    EXPECT_CALL(listener, foundUnsupportedLine(_)).Times(AtLeast(1));
    EXPECT_CALL(listener, foundComment(_)).Times(AtLeast(1));

    op.parse(ifs);
}

TEST(ObjModelTest, Rendering_WholeFile_Cube)
{
    RendererMock r;

    ObjModel model;

    model.load("cube.obj");
    model.setRenderer(&r);

    EXPECT_CALL(r, setVertexBuffer(_)).Times(1);
    EXPECT_CALL(r, setIndexBuffer(_)).Times(1);

    EXPECT_CALL(r, renderCommand(Renderer::PrimitiveType::Triangle, 0, 36)).Times(1);
    EXPECT_CALL(r, renderCommand(Renderer::PrimitiveType::Quad, _, 0)).Times(1);

    model.prepareBuffers();
    model.makeDrawCalls();
}

int main(int argc, char** argv)
{
  // The following line must be executed to initialize Google Mock
  // (and Google Test) before running the tests.
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
