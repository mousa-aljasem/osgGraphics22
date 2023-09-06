// osgRobotController.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgUtil/UpdateVisitor>
#include <osgViewer/ViewerEventHandlers>
#include <osg/ImageStream>
#include <osg/ShapeDrawable>
#include "findNodeVisitor.h"
using namespace std;

string fileLength = "";
osg::Node* g_pModel = 0;
osg::Group* rootNode = 0;


// sample of a node visitor class. This will not enact traversal
class nodePrinter : public osg::NodeVisitor
{
public:
	nodePrinter() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {};

	virtual void apply(osg::Node& node)
	{
		fileLength += "-";
		osg::Vec3 objectCenter = node.getBound().center() * node.getWorldMatrices()[0];
		std::cout << fileLength << node.getName() << " | X: " << objectCenter.x() << " | Y: " << objectCenter.y() << " | Z: " << objectCenter.z() << std::endl;

		// If the Node is part of a group, call recursively
		if (node.asGroup()) {
			nodePrinter printer;
			printer.traverse(node);
		}
		fileLength.pop_back();
	}

protected:
};

// Class to encapsulate the data so it is available to
// both the event handler and update callback
class robotInputDeviceStateType {
public: 
	robotInputDeviceStateType::
		robotInputDeviceStateType() : 
		moveRequest(false), direction(0.0) {}
	  bool moveRequest;
	  float direction;
	  std::string nodeName;
	  osg::Vec3d axis;
	  string PartSelected;

};

//Keyboard Event Handler
class myKeyboardEventHandler : public osgGA::GUIEventHandler {
public:
	myKeyboardEventHandler(robotInputDeviceStateType* tids) {
		robotInputDeviceState = tids;
	}
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);
protected:
	robotInputDeviceStateType* robotInputDeviceState;
};

void getNodeVisitor() {
	//Use the node visitor to find the robot hand
	findNodeVisitor findNodeHand("HandLocator");
	g_pModel->accept(findNodeHand);

	osg::Group* pGroupHand = dynamic_cast<osg::Group*>(findNodeHand.getFirst());
	//Convert the local matrix transform to world coordinates for the robot hand
	osg::Vec3 handCenter = pGroupHand->getChild(0)->getBound().center() * pGroupHand->getChild(0)->getWorldMatrices()[0];

	string toFind = "";
	float distance = 0.0f;
	for (int i = 1; i <= 3; i++) {
		//Use the node visitor to find the object we have named 'obj1'
		findNodeVisitor findNodeObject("Object" + to_string(i) + "Locator");
		g_pModel->accept(findNodeObject);


		//print to the console x,y,z
		//std::cout << handCenter[0] << ", " << handCenter[1] << ", " << handCenter[2] << std::endl;

		osg::Group* pGroupObject = dynamic_cast<osg::Group*>(findNodeObject.getFirst());
		

		//Convert the local matrix transform to world coordinates for 'obj1'
		osg::Vec3 objectCenter = pGroupObject->getChild(0)->getBound().center() * pGroupObject->getChild(0)->getWorldMatrices()[0];

		//print to the console x,y,z
		//std::cout << objectCenter[0] << ", " << objectCenter[1] << ", " << objectCenter[2] << std::endl;

		//Use trigonometry to calculate the distance between the 'obj1' and the robot hand 
		float x = objectCenter[0] - handCenter[0];
		float y = objectCenter[1] - handCenter[1];
		float z = objectCenter[2] - handCenter[2];
		float d = sqrt((x * x) + (y * y) + (z * z));
		if (distance == 0.0f) {
			distance = d;
			toFind = "Object" + to_string(i) + "Locator";
		}
		else {
			if (d < distance) {
				distance = d;
				toFind = "Object" + to_string(i) + "Locator";
			}
		}
		cout << "Object" + to_string(i) + "Locator: " << d <<std::endl;
	}

	findNodeVisitor findNodeObject(toFind);
	g_pModel->accept(findNodeObject);

	//remove 'obj1' from its parent and add it to the robot hand
	findNodeHand.getFirst()->asGroup()->addChild(findNodeObject.getFirst());
	findNodeObject.getFirst()->getParents()[0]->removeChild(findNodeObject.getFirst());

}

bool myKeyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
	switch (ea.getEventType()) {
	case(osgGA::GUIEventAdapter::KEYDOWN): {
		switch (ea.getKey()) {
		case 'a':
			robotInputDeviceState->nodeName = robotInputDeviceState->PartSelected;
			robotInputDeviceState->direction = 0.05;
			robotInputDeviceState->moveRequest = true;

			if (robotInputDeviceState->PartSelected == "BodyRotator") {
				robotInputDeviceState->axis = osg::Vec3d(0, 0, 1);
			}
			else if (robotInputDeviceState->PartSelected == "LowerArmRotator" || robotInputDeviceState->PartSelected == "UpperArmRotator") {
				robotInputDeviceState->axis = osg::Vec3d(0, 1, 0);
			}
			else if (robotInputDeviceState->PartSelected == "HandRotator") {
				robotInputDeviceState->axis = osg::Vec3d(0, 1, 0);
			}

			std::cout << " A key pressed" << std::endl;
			return false;
			break;
		case 'd':
			robotInputDeviceState->nodeName = robotInputDeviceState->PartSelected;
			robotInputDeviceState->direction = -0.05;
			robotInputDeviceState->moveRequest = true;

			if (robotInputDeviceState->PartSelected == "BodyRotator") {
				robotInputDeviceState->axis = osg::Vec3d(0, 0, 1);
			}
			else if (robotInputDeviceState->PartSelected == "LowerArmRotator" || robotInputDeviceState->PartSelected == "UpperArmRotator") {
				robotInputDeviceState->axis = osg::Vec3d(0, 1, 0);
			}
			else if (robotInputDeviceState->PartSelected == "HandRotator") {
				robotInputDeviceState->axis = osg::Vec3d(0, 1, 0);
			}
			std::cout << " D key pressed" << std::endl;
			return false;
			break;
		case 'c':
			std::cout << "C key pressed" << std::endl;
			getNodeVisitor();
			break;
		default:
			return false;
		}

	}
	case(osgGA::GUIEventAdapter::KEYUP): {
			robotInputDeviceState->moveRequest = false;
	}
	default:
		return false;
	}


}

//Callback for Robot Body
class updateRobotPosCallback : public osg::NodeCallback {
public:
	updateRobotPosCallback::updateRobotPosCallback(robotInputDeviceStateType*
		robotIDevState)
		: rotation(0.0) {
		robotInputDeviceState = robotIDevState;
	}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		if (node->getName() == robotInputDeviceState->nodeName && robotInputDeviceState->moveRequest) {
			osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*> (node);
			rotation += robotInputDeviceState->direction;
			mt->setMatrix(osg::Matrix::rotate(rotation, robotInputDeviceState->axis));
		}
		traverse(node, nv);
	}
protected:
	float rotation;
	osg::Vec3d robotPos;
	robotInputDeviceStateType* robotInputDeviceState;
};

// PickHandler -- A GUIEventHandler that implements picking.
class PickHandler : public osgGA::GUIEventHandler
{
public:
	PickHandler(robotInputDeviceStateType* iDev) : _mX(0.), _mY(0.), robotInputDeviceState(iDev) {}
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		if (!viewer)
			return(false);

		switch (ea.getEventType())
		{
			case osgGA::GUIEventAdapter::PUSH:
			case osgGA::GUIEventAdapter::MOVE:
			{
				// Record mouse location for the button press
				//   and move events.
				_mX = ea.getX();
				_mY = ea.getY();

				return(false);
			}
			case osgGA::GUIEventAdapter::RELEASE:
			{
				// If the mouse hasn't moved since the last
				//   button press or move event, perform a
				//   pick. (Otherwise, the trackball
				//   manipulator will handle it.)
				if (_mX == ea.getX() && _mY == ea.getY())
				{
					std::cout << " Clicked!: " << _mX << std::endl;
					if (pick(ea.getXnormalized(),
						ea.getYnormalized(), viewer))
						return(true);
				}
				return(false);
			}

			default:
				return(false);
		}
	}

protected:
	// Store mouse xy location for button press & move events.
	float _mX, _mY;
	robotInputDeviceStateType* robotInputDeviceState;
	osg::Node* child;

	// Perform a pick operation.
	bool pick(const double x, const double y, osgViewer::Viewer* viewer)
	{
		if (!viewer->getSceneData())
			// Nothing to pick.
			return(false);

		double w(.05), h(.05);
		osgUtil::PolytopeIntersector* picker =
			new osgUtil::PolytopeIntersector(
				osgUtil::Intersector::PROJECTION,
				x - w, y - h, x + w, y + h);

		osgUtil::IntersectionVisitor iv(picker);
		viewer->getCamera()->accept(iv);

		if (picker->containsIntersections())
		{
			const osg::NodePath& nodePath =
				picker->getFirstIntersection().nodePath;
			unsigned int idx = nodePath.size();

			while (idx--)
			{
				// Find the LAST MatrixTransform in the node
				//   path; this will be the MatrixTransform
				//   to attach our callback to.


				osg::MatrixTransform* mt =
					dynamic_cast<osg::MatrixTransform*>(
						nodePath[idx]);
				if (mt == NULL)
					continue;

				//restrict robot arm movement dependant on which part is selected 
				if (mt->getName() == "BodyRotator" || mt->getName() == "UpperArmRotator" || mt->getName() == "LowerArmRotator" || mt->getName() == "HandRotator") {

					robotInputDeviceState->PartSelected = mt->getName();

					// outline selected part of robot blue
					std::cout << "Highlighted Node = " << mt->getName() << std::endl;

				}
				break;
			}
		}
	}

};

// function to run before rendering starts -> use this to identify nodes
void init()
{
	if (g_pModel)
	{
		std::cout << "Loaded Model" << std::endl;
		nodePrinter printer;
		printer.traverse(*g_pModel);
		
		// Create a NodeVisitor Object and apply it to our tree
		findNodeVisitor findNode("BodyRotator");
		g_pModel->accept(findNode);

		// Sanity Check - find the type of the node we have found
		//std::cout << findNode.getFirst()->className() << std::endl;

		// Create a Matrix Transform connected to our node
		osg::MatrixTransform* rotate = dynamic_cast<osg::MatrixTransform*> (findNode.getFirst());

		// If succeeded, rotate the body by 0.8 radians
		if (rotate) {
			const double angle = 0.8;
			const osg::Vec3d axis(0, 0, 1);
			rotate->setMatrix(osg::Matrix::rotate(angle, axis));
		}

	}
	else
	{
		std::cout << " Failed to load a Model" << std::endl;
	}
}

int main(int argc, char** argv)
{
	rootNode = new osg::Group;

	osg::ArgumentParser arguments(&argc, argv);

	// load model
	g_pModel = osgDB::readNodeFile("../Data/robot.osg");
	g_pModel->ref();

	if (!g_pModel) return 0;

	init();

	// setup viewer
	osgViewer::Viewer viewer;

	// Declare instance of class to record state of keyboard
	robotInputDeviceStateType* rIDevState = new robotInputDeviceStateType;

	// Set up the robot update callback
	/*
	BodyRotator
	UpperArmRotator
	LowerArmRotator
	HandRotator
	*/
	findNodeVisitor findBodyNode("BodyRotator");
	findNodeVisitor findUpperArmNode("UpperArmRotator");
	findNodeVisitor findLowerArmNode("LowerArmRotator");
	findNodeVisitor findHandNode("HandRotator");

	g_pModel->accept(findBodyNode);
	g_pModel->accept(findUpperArmNode);
	g_pModel->accept(findLowerArmNode);
	g_pModel->accept(findHandNode);

	findBodyNode.getFirst()->setUpdateCallback(new updateRobotPosCallback(rIDevState));
	findUpperArmNode.getFirst()->setUpdateCallback(new updateRobotPosCallback(rIDevState));
	findLowerArmNode.getFirst()->setUpdateCallback(new updateRobotPosCallback(rIDevState));
	findHandNode.getFirst()->setUpdateCallback(new updateRobotPosCallback(rIDevState));

	// The constructor for our event handler also gets a 
	// pointer to our robot input device state instance
	myKeyboardEventHandler* robotEventHandler = new myKeyboardEventHandler(rIDevState);
	PickHandler* pickHandler = new PickHandler(rIDevState);

	// Add our event handler to the list
	viewer.addEventHandler(robotEventHandler);
	viewer.addEventHandler(pickHandler);


	osg::GraphicsContext::Traits* pTraits = new osg::GraphicsContext::Traits();
	pTraits->x = 20;
	pTraits->y = 20;
	pTraits->width = 600;
	pTraits->height = 480;
	pTraits->windowDecoration = true;
	pTraits->doubleBuffer = true;
	pTraits->sharedContext = 0;

	osg::GraphicsContext* pGC = osg::GraphicsContext::createGraphicsContext(pTraits);
	osgGA::KeySwitchMatrixManipulator* pKeyswitchManipulator = new osgGA::KeySwitchMatrixManipulator();
	pKeyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
	pKeyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator());
	pKeyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator());
	viewer.setCameraManipulator(pKeyswitchManipulator);
	osg::Camera* pCamera = viewer.getCamera();
	pCamera->setGraphicsContext(pGC);
	pCamera->setViewport(new osg::Viewport(0, 0, pTraits->width, pTraits->height));


	// add the state manipulator
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

	// add the thread model handler
	viewer.addEventHandler(new osgViewer::ThreadingHandler);

	// add the window size toggle handler
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

	// add the stats handler
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// add the record camera path handler
	viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);

	// add the LOD Scale handler
	viewer.addEventHandler(new osgViewer::LODScaleHandler);

	// add the screen capture handler
	viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);

	osg::Geode* g_pWall = 0;
	osg::Geometry* pGeoWall = new osg::Geometry;

	osg::Vec3Array* pVertsWall = new osg::Vec3Array;
	pVertsWall->push_back(osg::Vec3(0, 38, 10));
	pVertsWall->push_back(osg::Vec3(0, 38, 0));
	pVertsWall->push_back(osg::Vec3(0, 0, 0));
	pVertsWall->push_back(osg::Vec3(0, 0, 10));
	pGeoWall->setVertexArray(pVertsWall);

	osg::DrawElementsUInt* pPrimitiveSetWall = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	pPrimitiveSetWall->push_back(3);
	pPrimitiveSetWall->push_back(2);
	pPrimitiveSetWall->push_back(1);
	pPrimitiveSetWall->push_back(0);
	pGeoWall->addPrimitiveSet(pPrimitiveSetWall);

	osg::Vec2Array* pTexCoordsWall = new  osg::Vec2Array(4);
	(*pTexCoordsWall)[0].set(0.0f, 0.0f);
	(*pTexCoordsWall)[1].set(1.0f, 0.0f);
	(*pTexCoordsWall)[2].set(1.0f, 1.0f);
	(*pTexCoordsWall)[3].set(0.0f, 1.0f);
	pGeoWall->setTexCoordArray(0, pTexCoordsWall);

	osg::Geode* pGeodeWall = new osg::Geode; osg::StateSet* pStateSetWall = pGeodeWall->getOrCreateStateSet();
	pStateSetWall->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	pGeodeWall->addDrawable(pGeoWall);

	osg::Texture2D* pTexWall = new osg::Texture2D;
	osg::ref_ptr<osg::Image> imgWall = osgDB::readImageFile("../data/Textures/wall_1024_tex1_05.tga");
	osg::ImageStream* isWall = dynamic_cast<osg::ImageStream*>(imgWall.get());
	pTexWall->setImage(imgWall.get());
	pStateSetWall->setTextureAttributeAndModes(0, pTexWall, osg::StateAttribute::ON);

	osg::MatrixTransform* mtWall = new osg::MatrixTransform;
	osg::Matrixf locationWall;
	locationWall.setTrans(osg::Vec3d(-10, -10, 0));
	mtWall->setMatrix(locationWall);
	mtWall->addChild(pGeodeWall);
	rootNode->addChild(mtWall);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(new osg::ShapeDrawable(new  osg::Sphere(osg::Vec3(2.0f, 12.0f, 12.0f), 3.0f)));

	osg::ref_ptr<osg::Texture2D> pTexBall = new osg::Texture2D;
	osg::StateSet* pStateSetBall = geode->getOrCreateStateSet();	//Get stateset of geode 
	osg::ref_ptr<osg::Image>  pImageBall(osgDB::readImageFile("../data/ball.tga")); //Read in image
	if (!pImageBall) {
		std::cout << "Error: Couldn't find texture!" << std::endl;	//Check if image exists
	}
	pTexBall->setImage(pImageBall.get());	//Set image
	pStateSetBall->setTextureAttributeAndModes(0, pTexBall.get(), osg::StateAttribute::ON);	//Set texture on

	osg::ref_ptr<osg::MatrixTransform> mt = new  osg::MatrixTransform;
	osg::ref_ptr<osg::AnimationPathCallback> cb = new osg::AnimationPathCallback(osg::Vec3(2, 12, 12), osg::Z_AXIS, osg::inDegrees(300.0f));
	mt->setUpdateCallback(cb.get());
	mt->addChild(geode.get());
	rootNode->addChild(mt.get());		//Turn off disco ball untill toggled by user

	osg::ref_ptr<osg::Group> lightGroup(new osg::Group);
	osg::ref_ptr<osg::StateSet> lightSS(rootNode->getOrCreateStateSet());	//Get stateset of scene
	osg::ref_ptr<osg::LightSource>  lightSource1 = new osg::LightSource;

	osg::Vec4f lightPosition(osg::Vec4f(0, 10, 10, 1.0f));
	osg::ref_ptr<osg::Light> myLight = new osg::Light;
	myLight->setLightNum(4);								//Set up light id (4)
	myLight->setPosition(lightPosition);
	myLight->setAmbient(osg::Vec4(1.0f, 1.0f, 0.8f, 1.0f));
	myLight->setDiffuse(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	myLight->setSpotExponent(0.5);

	lightSource1->setLight(myLight.get());								//Set light
	lightSource1->setLocalStateSetModes(osg::StateAttribute::ON);		//Turn on local stateset
	lightSource1->setStateSetModes(*lightSS, osg::StateAttribute::ON);	//Set stateset of scene
	lightGroup->addChild(lightSource1.get());							//Add light to group

	rootNode->addChild(lightGroup.get());		//Remove light object untill toggled by user

	rootNode->addChild(g_pModel);

	// set the scene to render
	//viewer.setSceneData(g_pModel);
	viewer.setSceneData(rootNode);

	viewer.realize();

	return viewer.run();

	return 0;
}

