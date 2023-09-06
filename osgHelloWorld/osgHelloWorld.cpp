#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Texture2D>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

osg::Geode* createPyramid() {
	osg::Geode* pyramidGeode = new osg::Geode();
	osg::Geometry* pyramidGeometry = new osg::Geometry();
	pyramidGeode->addDrawable(pyramidGeometry);

	osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
	pyramidVertices->push_back(osg::Vec3(0, 0, 0)); // Front Left
	pyramidVertices->push_back(osg::Vec3(10, 0, 0)); // Front Right
	pyramidVertices->push_back(osg::Vec3(10, 10, 0)); // Back Left
	pyramidVertices->push_back(osg::Vec3(0, 10, 0)); // Back Right
	pyramidVertices->push_back(osg::Vec3(5, 5, 10)); // Peak

	pyramidGeometry->setVertexArray(pyramidVertices);

	// Create a QUAD primitive for the base by specifying the vertices from our vertex list that make this QUAD
	osg::DrawElementsUInt* pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	pyramidBase->push_back(3);
	pyramidBase->push_back(2);
	pyramidBase->push_back(1);
	pyramidBase->push_back(0);
	pyramidGeometry->addPrimitiveSet(pyramidBase);

	osg::DrawElementsUInt* pyramidFaceOne = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceOne->push_back(0);
	pyramidFaceOne->push_back(1);
	pyramidFaceOne->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceOne);

	osg::DrawElementsUInt* pyramidFaceTwo = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceTwo->push_back(1);
	pyramidFaceTwo->push_back(2);
	pyramidFaceTwo->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceTwo);

	osg::DrawElementsUInt* pyramidFaceThree = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceThree->push_back(2);
	pyramidFaceThree->push_back(3);
	pyramidFaceThree->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceThree);

	osg::DrawElementsUInt* pyramidFaceFour = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceFour->push_back(3);
	pyramidFaceFour->push_back(0);
	pyramidFaceFour->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceFour);

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Index 0 RED
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); // Index 0 GREEN
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)); // Index 0 BLUE
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Index 0 WHITE
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Index 0 RED

	pyramidGeometry->setColorArray(colors);
	pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	osg::Vec2Array* texcoords = new osg::Vec2Array(5);
	(*texcoords)[0].set(0.00f, 0.0f); // Tex coord for vertex 0
	(*texcoords)[1].set(0.25f, 0.0f); // Tex coord for vertex 1
	(*texcoords)[2].set(0.50f, 0.0f); // Tex coord for vertex 2
	(*texcoords)[3].set(0.75f, 0.0f); // Tex coord for vertex 3
	(*texcoords)[4].set(0.50f, 1.0f); // Tex coord for vertex 4
	pyramidGeometry->setTexCoordArray(0, texcoords);

	return pyramidGeode;
}

int main( int argc, char** argv){
	osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("../data/Models/HelloRobot.osg");


	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(50, 50, 800, 600);
	viewer.setSceneData(root.get());
	return viewer.run();
}

