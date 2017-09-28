#include "gtest/gtest.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "objparser.h"

#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

class ObjParserEventsMock : public ObjParserEvents
{
public:
    MOCK_METHOD4(foundVertexGeometry, void(float x, float y, float z, float w));
    MOCK_METHOD3(foundVertexNormal, void(float x, float y, float z));
    MOCK_METHOD3(foundTextureCoordinate, void(float u, float v, float w));
    MOCK_METHOD1(foundFace, void(const std::vector<FaceVertexIndices> &v));
    MOCK_METHOD1(foundComment, void(const std::string &));
    MOCK_METHOD1(foundUnsupportedLine, void(const std::string &));
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
    EXPECT_CALL(listener, foundFace(v)).Times(1);

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

TEST(ObjParserTest, Triggering_WhileFile_Cube)
{
    ObjParser op;
    ifstream ifs("cube.obj");

    ObjParserEventsMock listener;
    op.setListener(&listener);
    EXPECT_CALL(listener, foundVertexGeometry(_, _, _, _)).Times(8);
    EXPECT_CALL(listener, foundVertexNormal(_, _, _)).Times(6);
    EXPECT_CALL(listener, foundTextureCoordinate(_, _, _)).Times(4);
    EXPECT_CALL(listener, foundFace(_)).Times(12);

    EXPECT_CALL(listener, foundUnsupportedLine(_)).Times(AtLeast(1));
    EXPECT_CALL(listener, foundComment(_)).Times(AtLeast(1));

    op.parse(ifs);
}

int main(int argc, char** argv) {
  // The following line must be executed to initialize Google Mock
  // (and Google Test) before running the tests.
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
