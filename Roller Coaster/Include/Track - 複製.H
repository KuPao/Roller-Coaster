/************************************************************************
     File:        Track.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Container for the "World"

						This provides a container for all of the "stuff" 
						in the world.

						It could have been all global variables, or it could 
						have just been
						contained in the window. The advantage of doing it 
						this way is that
						we might have multiple windows looking at the same 
						world. But, I don't	think we'll actually do that.

						See the readme for commentary on code style

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/
#pragma once

#include <vector>
#include <utility>
#include <glm/glm.hpp>

using std::vector; // avoid having to say std::vector all of the time

// make use of other data structures from this project
#include "ControlPoint.H"
#include "Utilities/Pnt3f.H"

#define PARTSNUM 6

class CTrack {
	public:		
		// Constructor
		CTrack();

	public:
		// when we want to clear the control points, we really "reset" them 
		// to have 4 default positions (since we should never have fewer
		// than 4 points)
		void resetPoints();


		// read and write to files
		void readPoints(const char* filename);
		void writePoints(const char* filename);

		//draw track
		void CalLinear();
		void CalCubicBSplines();
		void CalCatmullRom();
		void CalCubicHermiteSpline();
		void CalTrackPts();
		void drawLine(bool doingShadows);
		void drawTrack(bool doingShadows);
		void drawATrack(float x, float y, float z, float scale);
		void drawRoad(bool doingShadows);
		void drawTrain(bool doingShadows);
		void calTrainCam();
		void calTrainCam2();
		float Dot(Pnt3f a, Pnt3f b);

	public:
		// rather than have generic objects, we make a special case for these few
		// objects that we know that all implementations are going to need and that
		// we're going to have to handle specially
		vector<ControlPoint> points;
		vector<Pnt3f> splinePts;
		vector<Pnt3f> trackPts;
		vector<Pnt3f> splineVecs;
		vector<std::pair<int, Pnt3f>> CPVector;

		//###################################################################
		// TODO: you might want to do this differently
		//###################################################################
		// the state of the train - basically, all I need to remember is where
		// it is in parameter space
		float trainU;
		float trainSpeed = 0;
		int trainNums = 3;
		bool upsidedown = false;
		Pnt3f trainCam;
		Pnt3f trainCamRotate;
		Pnt3f trainCamRotate2;

		std::vector<glm::vec3> vertices, normals;
		std::vector<glm::vec2> uvs;
		std::vector<unsigned int> faces[PARTSNUM];//face count;
		std::vector<std::string> mtls[PARTSNUM];//use material
};