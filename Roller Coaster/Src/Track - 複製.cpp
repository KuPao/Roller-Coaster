/************************************************************************
     File:        Track.cpp

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

#include <cmath>
#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include "Track.H"
#include "ObjLoader.h"
#include "Utilities/3dUtils.h"

//****************************************************************************
//
// * Constructor
//============================================================================
CTrack::
CTrack() : trainU(0)
//============================================================================
{
	resetPoints();
}

//****************************************************************************
//
// * provide a default set of points
//============================================================================
void CTrack::
resetPoints()
//============================================================================
{

	points.clear();
	points.push_back(ControlPoint(Pnt3f(50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,50)));
	points.push_back(ControlPoint(Pnt3f(-50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,-50)));

	// we had better put the train back at the start of the track...
	trainU = 0.0;
}

//****************************************************************************
//
// * Handy utility to break a string into a list of words
//============================================================================
void breakString(char* str, std::vector<const char*>& words) 
//============================================================================
{
	// start with no words
	words.clear();

	// scan through the string, starting at the beginning
	char* p = str;

	// stop when we hit the end of the string
	while(*p) {
		// skip over leading whitespace - stop at the first character or end of string
		while (*p && *p<=' ') p++;

		// now we're pointing at the first thing after the spaces
		// make sure its not a comment, and that we're not at the end of the string
		// (that's actually the same thing)
		if (! (*p) || *p == '#')
		break;

		// so we're pointing at a word! add it to the word list
		words.push_back(p);

		// now find the end of the word
		while(*p > ' ') p++;	// stop at space or end of string

		// if its ethe end of the string, we're done
		if (! *p) break;

		// otherwise, turn this space into and end of string (to end the word)
		// and keep going
		*p = 0;
		p++;
	}
}

//****************************************************************************
//
// * The file format is simple
//   first line: an integer with the number of control points
//	  other lines: one line per control point
//   either 3 (X,Y,Z) numbers on the line, or 6 numbers (X,Y,Z, orientation)
//============================================================================
void CTrack::
readPoints(const char* filename)
//============================================================================
{
	FILE* fp = fopen(filename,"r");
	if (!fp) {
		printf("Can't Open File!\n");
	} 
	else {
		char buf[512];

		// first line = number of points
		fgets(buf,512,fp);
		size_t npts = (size_t) atoi(buf);

		if( (npts<4) || (npts>65535)) {
			printf("Illegal Number of Points Specified in File");
		} else {
			points.clear();
			// get lines until EOF or we have enough points
			while( (points.size() < npts) && fgets(buf,512,fp) ) {
				Pnt3f pos,orient;
				vector<const char*> words;
				breakString(buf,words);
				if (words.size() >= 3) {
					pos.x = (float) strtod(words[0],0);
					pos.y = (float) strtod(words[1],0);
					pos.z = (float) strtod(words[2],0);
				} else {
					pos.x=0;
					pos.y=0;
					pos.z=0;
				}
				if (words.size() >= 6) {
					orient.x = (float) strtod(words[3],0);
					orient.y = (float) strtod(words[4],0);
					orient.z = (float) strtod(words[5],0);
				} else {
					orient.x = 0;
					orient.y = 1;
					orient.z = 0;
				}
				orient.normalize();
				points.push_back(ControlPoint(pos,orient));
			}
		}
		fclose(fp);
	}
	trainU = 0;
}

//****************************************************************************
//
// * write the control points to our simple format
//============================================================================
void CTrack::
writePoints(const char* filename)
//============================================================================
{
	FILE* fp = fopen(filename,"w");
	if (!fp) {
		printf("Can't open file for writing");
	} else {
		fprintf(fp,"%d\n",points.size());
		for(size_t i=0; i<points.size(); ++i)
			fprintf(fp,"%g %g %g %g %g %g\n",
				points[i].pos.x, points[i].pos.y, points[i].pos.z, 
				points[i].orient.x, points[i].orient.y, points[i].orient.z);
		fclose(fp);
	}
}

void CTrack::CalLinear()
{
	splinePts.clear();
	splineVecs.clear();
	for (size_t i = 0; i < this->points.size(); ++i)
	{
		float xD = (this->points[(i + 1) % this->points.size()].pos.x - this->points[i].pos.x);
		float yD = (this->points[(i + 1) % this->points.size()].pos.y - this->points[i].pos.y);
		float zD = (this->points[(i + 1) % this->points.size()].pos.z - this->points[i].pos.z);
		float gap =  1 / sqrt(pow(xD, 2) + pow(yD, 2) + pow(zD, 2));
		for (float t = 0; t < 1; t += gap)
		{
			float x = this->points[i].pos.x + xD * t;
			float y = this->points[i].pos.y + yD * t;
			float z = this->points[i].pos.z + zD * t;
			splinePts.push_back(Pnt3f(x, y, z));
			float xVec = (this->points[i].orient.x + this->points[(i + 1) % this->points.size()].orient.x) / 2;
			float yVec = (this->points[i].orient.y + this->points[(i + 1) % this->points.size()].orient.y) / 2;
			float zVec = (this->points[i].orient.z + this->points[(i + 1) % this->points.size()].orient.z) / 2;
			splineVecs.push_back(Pnt3f(xVec, yVec, zVec).normalize());
		}
	}
	CalTrackPts();
}

void CTrack::CalCubicBSplines()
{
	splinePts.clear();
	splineVecs.clear();
	for (size_t i = 0; i < this->points.size(); ++i)
	{
		float xD = (this->points[(i + 1) % this->points.size()].pos.x - this->points[(i + 2) % this->points.size()].pos.x);
		float yD = (this->points[(i + 1) % this->points.size()].pos.y - this->points[(i + 2) % this->points.size()].pos.y);
		float zD = (this->points[(i + 1) % this->points.size()].pos.z - this->points[(i + 2) % this->points.size()].pos.z);
		float gap = 1 / sqrt(pow(xD, 2) + pow(yD, 2) + pow(zD, 2));
		for (float t = 0; t < 1; t += gap)
		{
			float x = this->points[i].pos.x * pow(1 - t, 3) +
					  this->points[(i + 1) % this->points.size()].pos.x * (3 * pow(t, 3) - 6 * pow(t, 2) + 4) +
					  this->points[(i + 2) % this->points.size()].pos.x * (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) +
					  this->points[(i + 3) % this->points.size()].pos.x * pow(t, 3);
			float y = this->points[i].pos.y * pow(1 - t, 3) +
					  this->points[(i + 1) % this->points.size()].pos.y * (3 * pow(t, 3) - 6 * pow(t, 2) + 4) +
					  this->points[(i + 2) % this->points.size()].pos.y * (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) +
					  this->points[(i + 3) % this->points.size()].pos.y * pow(t, 3);
			float z = this->points[i].pos.z * pow(1 - t, 3) +
					  this->points[(i + 1) % this->points.size()].pos.z * (3 * pow(t, 3) - 6 * pow(t, 2) + 4) +
					  this->points[(i + 2) % this->points.size()].pos.z * (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) +
					  this->points[(i + 3) % this->points.size()].pos.z * pow(t, 3);
			splinePts.push_back(Pnt3f(0.166 * x, 0.166 * y, 0.166 * z));
			float xVec = this->points[i].orient.x * pow(1 - t, 3) +
				this->points[(i + 1) % this->points.size()].orient.x * (3 * pow(t, 3) - 6 * pow(t, 2) + 4) +
				this->points[(i + 2) % this->points.size()].orient.x * (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) +
				this->points[(i + 3) % this->points.size()].orient.x * pow(t, 3);
			float yVec = this->points[i].orient.y * pow(1 - t, 3) +
				this->points[(i + 1) % this->points.size()].orient.y * (3 * pow(t, 3) - 6 * pow(t, 2) + 4) +
				this->points[(i + 2) % this->points.size()].orient.y * (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) +
				this->points[(i + 3) % this->points.size()].orient.y * pow(t, 3);
			float zVec = this->points[i].orient.z * pow(1 - t, 3) +
				this->points[(i + 1) % this->points.size()].orient.z * (3 * pow(t, 3) - 6 * pow(t, 2) + 4) +
				this->points[(i + 2) % this->points.size()].orient.z * (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) +
				this->points[(i + 3) % this->points.size()].orient.z * pow(t, 3);
			splineVecs.push_back(Pnt3f(xVec, yVec, zVec).normalize());
		}
	}
	CalTrackPts();
}

void CTrack::CalCatmullRom()
{
	splinePts.clear();
	splineVecs.clear();
	for (size_t i = 0; i < this->points.size(); ++i)
	{
		float xD = (this->points[(i + 1) % this->points.size()].pos.x - this->points[(i + 2) % this->points.size()].pos.x);
		float yD = (this->points[(i + 1) % this->points.size()].pos.y - this->points[(i + 2) % this->points.size()].pos.y);
		float zD = (this->points[(i + 1) % this->points.size()].pos.z - this->points[(i + 2) % this->points.size()].pos.z);
		float gap = 1 / sqrt(pow(xD, 2) + pow(yD, 2) + pow(zD, 2));
		for (float t = 0; t < 1; t += gap)
		{
			float x = this->points[i].pos.x * (-0.5 * pow(t, 3) + pow(t, 2) - 0.5 * t) +
					  this->points[(i + 1) % this->points.size()].pos.x * (1.5 * pow(t, 3) - 2.5 * pow(t, 2) + 1) +
					  this->points[(i + 2) % this->points.size()].pos.x * (-1.5 * pow(t, 3) + 2 * pow(t, 2) + 0.5 * t) +
					  this->points[(i + 3) % this->points.size()].pos.x * (0.5 * pow(t, 3) - 0.5 * pow(t, 2));
			float y = this->points[i].pos.y * (-0.5 * pow(t, 3) + pow(t, 2) - 0.5 * t) +
					  this->points[(i + 1) % this->points.size()].pos.y * (1.5 * pow(t, 3) - 2.5 * pow(t, 2) + 1) +
					  this->points[(i + 2) % this->points.size()].pos.y * (-1.5 * pow(t, 3) + 2 * pow(t, 2) + 0.5 * t) +
					  this->points[(i + 3) % this->points.size()].pos.y * (0.5 * pow(t, 3) - 0.5 * pow(t, 2));
			float z = this->points[i].pos.z * (-0.5 * pow(t, 3) + pow(t, 2) - 0.5 * t) +
					  this->points[(i + 1) % this->points.size()].pos.z * (1.5 * pow(t, 3) - 2.5 * pow(t, 2) + 1) +
					  this->points[(i + 2) % this->points.size()].pos.z * (-1.5 * pow(t, 3) + 2 * pow(t, 2) + 0.5 * t) +
					  this->points[(i + 3) % this->points.size()].pos.z * (0.5 * pow(t, 3) - 0.5 * pow(t, 2));
			splinePts.push_back(Pnt3f(x, y, z));			
			float xVec = this->points[i].orient.x * (-0.5 * pow(t, 3) + pow(t, 2) - 0.5 * t) +
				this->points[(i + 1) % this->points.size()].orient.x * (1.5 * pow(t, 3) - 2.5 * pow(t, 2) + 1) +
				this->points[(i + 2) % this->points.size()].orient.x * (-1.5 * pow(t, 3) + 2 * pow(t, 2) + 0.5 * t) +
				this->points[(i + 3) % this->points.size()].orient.x * (0.5 * pow(t, 3) - 0.5 * pow(t, 2));
			float yVec = this->points[i].orient.y * (-0.5 * pow(t, 3) + pow(t, 2) - 0.5 * t) +
				this->points[(i + 1) % this->points.size()].orient.y * (1.5 * pow(t, 3) - 2.5 * pow(t, 2) + 1) +
				this->points[(i + 2) % this->points.size()].orient.y * (-1.5 * pow(t, 3) + 2 * pow(t, 2) + 0.5 * t) +
				this->points[(i + 3) % this->points.size()].orient.y * (0.5 * pow(t, 3) - 0.5 * pow(t, 2));
			float zVec = this->points[i].orient.z * (-0.5 * pow(t, 3) + pow(t, 2) - 0.5 * t) +
				this->points[(i + 1) % this->points.size()].orient.z * (1.5 * pow(t, 3) - 2.5 * pow(t, 2) + 1) +
				this->points[(i + 2) % this->points.size()].orient.z * (-1.5 * pow(t, 3) + 2 * pow(t, 2) + 0.5 * t) +
				this->points[(i + 3) % this->points.size()].orient.z * (0.5 * pow(t, 3) - 0.5 * pow(t, 2));
			splineVecs.push_back(Pnt3f(xVec, yVec, zVec).normalize());
		}
	}
	CalTrackPts();
}

void CTrack::CalCubicHermiteSpline()
{
	splinePts.clear();
	for (size_t i = 0; i < points.size(); ++i)
	{
		for (float t = 0; t < 1; t += 0.01)
		{
			float x = this->points[i].pos.x * (2 * pow(t, 3) - 3 * pow(t, 2) + 1) +
				this->points[(i + 1) % this->points.size()].pos.x * (-2 * pow(t, 3) + 3 * pow(t, 2)) +
				this->points[(i + 2) % this->points.size()].pos.x * (pow(t, 3) - 2 * pow(t, 2) + t) +
				this->points[(i + 3) % this->points.size()].pos.x * (pow(t, 3) - pow(t, 2));
			float y = this->points[i].pos.y * (2 * pow(t, 3) - 3 * pow(t, 2) + 1) +
				this->points[(i + 1) % this->points.size()].pos.y * (-2 * pow(t, 3) + 3 * pow(t, 2)) +
				this->points[(i + 2) % this->points.size()].pos.y * (pow(t, 3) - 2 * pow(t, 2) + t) +
				this->points[(i + 3) % this->points.size()].pos.y * (pow(t, 3) - pow(t, 2));
			float z = this->points[i].pos.z * (2 * pow(t, 3) - 3 * pow(t, 2) + 1) +
				this->points[(i + 1) % this->points.size()].pos.z * (-2 * pow(t, 3) + 3 * pow(t, 2)) +
				this->points[(i + 2) % this->points.size()].pos.z * (pow(t, 3) - 2 * pow(t, 2) + t) +
				this->points[(i + 3) % this->points.size()].pos.z * (pow(t, 3) - pow(t, 2));
			splinePts.push_back(Pnt3f(x, y, z));
		}
	}
}

void CTrack::CalTrackPts()
{
	trackPts.clear();
	int start = 0, end = 0;
	bool round = false;
	for (int i = 0; i < splinePts.size();) {
		for (int j = 1;; j++) {
			float xD = (splinePts[(i + j) % splinePts.size()].x - splinePts[i].x);
			float yD = (splinePts[(i + j) % splinePts.size()].y - splinePts[i].y);
			float zD = (splinePts[(i + j) % splinePts.size()].z - splinePts[i].z);
			float dis = sqrt(pow(xD, 2) + pow(yD, 2) + pow(zD, 2));
			if (i + j >= splinePts.size()) round = true;
			if (dis >= 30) {
				end = (i + j) % splinePts.size();
				break;
			}
		}
		float x = (splinePts[i].x + splinePts[end].x) / 2;
		float y = (splinePts[i].y + splinePts[end].y) / 2;
		float z = (splinePts[i].z + splinePts[end].z) / 2;
		trackPts.push_back(Pnt3f(x, y, z));;
		if (round) break;
		i = end;
	}
}

void CTrack::drawLine(bool doingShadows)
{
	float width = 2;
	glPushMatrix();
	if(!doingShadows)
		glColor3ub(240, 60, 60);
	glBegin(GL_TRIANGLE_STRIP);
	int i;
	for (i = 0; i < splinePts.size(); i++) {
		Pnt3f vec = Pnt3f(splinePts[(i + 1) % splinePts.size()].x - splinePts[i].x, splinePts[(i + 1) % splinePts.size()].y - splinePts[i].y, splinePts[(i + 1) % splinePts.size()].z - splinePts[i].z);
		Pnt3f cross;
		cross.x = vec.y * splineVecs[i].z - vec.z * splineVecs[i].y;
		cross.y = vec.z * splineVecs[i].x - vec.x * splineVecs[i].z;
		cross.z = vec.x * splineVecs[i].y - vec.y * splineVecs[i].x;
		cross.normalize();
		cross = width / 2 * cross;
		glVertex3f(splinePts[i].x + cross.x, splinePts[i].y + cross.y, splinePts[i].z + cross.z);
		glVertex3f(splinePts[(i + 1) % splinePts.size()].x - cross.x, splinePts[(i + 1) % splinePts.size()].y - cross.y, splinePts[(i + 1) % splinePts.size()].z - cross.z);
	}
	Pnt3f vec = Pnt3f(splinePts[1].x - splinePts[0].x, splinePts[1].y - splinePts[0].y, splinePts[1].z - splinePts[0].z);
	Pnt3f cross;
	cross.x = vec.y * splineVecs[0].z - vec.z * splineVecs[0].y;
	cross.y = vec.z * splineVecs[0].x - vec.x * splineVecs[0].z;
	cross.z = vec.x * splineVecs[0].y - vec.y * splineVecs[0].x;
	cross.normalize();
	cross = width / 2 * cross;
	glVertex3f(splinePts[0].x + cross.x, splinePts[0].y + cross.y, splinePts[0].z + cross.z);
	glVertex3f(splinePts[1].x - cross.x, splinePts[1].y - cross.y, splinePts[1].z - cross.z);
	glEnd();
	glPopMatrix();
}

void CTrack::drawTrack(bool doingShadows)
{
	/*float scale = 0.1; //模型300單位 30單位
	for (int i = 0; i < trackPts.size(); i++)
		drawATrack(trackPts[i].x, trackPts[i].y, trackPts[i].z, scale);*/
	float trackWidth = 5;
	float railWidth = 2;
	vector<Pnt3f> crossVecs;
	glPushMatrix();
	if (!doingShadows)
		glColor3ub(70, 70, 70);
	glBegin(GL_TRIANGLE_STRIP);
	int i;
	for (i = 0; i < splinePts.size(); i++) {
		Pnt3f vec = Pnt3f(splinePts[(i + 1) % splinePts.size()].x - splinePts[i].x, splinePts[(i + 1) % splinePts.size()].y - splinePts[i].y, splinePts[(i + 1) % splinePts.size()].z - splinePts[i].z);
		Pnt3f cross;
		cross.x = vec.y * splineVecs[i].z - vec.z * splineVecs[i].y;
		cross.y = vec.z * splineVecs[i].x - vec.x * splineVecs[i].z;
		cross.z = vec.x * splineVecs[i].y - vec.y * splineVecs[i].x;
		cross.normalize();
		crossVecs.push_back(cross);

		glVertex3f(splinePts[i].x + cross.x * (trackWidth + railWidth) / 2, splinePts[i].y + cross.y * (trackWidth + railWidth) / 2, splinePts[i].z + cross.z * (trackWidth + railWidth) / 2);
		glVertex3f(splinePts[(i + 1) % splinePts.size()].x + cross.x * (trackWidth - railWidth) / 2, splinePts[(i + 1) % splinePts.size()].y + cross.y * (trackWidth - railWidth) / 2, splinePts[(i + 1) % splinePts.size()].z + cross.z * (trackWidth - railWidth) / 2);
	}
	Pnt3f vec = Pnt3f(splinePts[1].x - splinePts[0].x, splinePts[1].y - splinePts[0].y, splinePts[1].z - splinePts[0].z);
	Pnt3f cross;
	cross.x = vec.y * splineVecs[0].z - vec.z * splineVecs[0].y;
	cross.y = vec.z * splineVecs[0].x - vec.x * splineVecs[0].z;
	cross.z = vec.x * splineVecs[0].y - vec.y * splineVecs[0].x;
	cross.normalize();
	crossVecs.push_back(cross);

	glVertex3f(splinePts[0].x + cross.x * (trackWidth + railWidth) / 2, splinePts[0].y + cross.y * (trackWidth + railWidth) / 2, splinePts[0].z + cross.z * (trackWidth + railWidth) / 2);
	glVertex3f(splinePts[1].x + cross.x * (trackWidth - railWidth) / 2, splinePts[1].y + cross.y * (trackWidth - railWidth) / 2, splinePts[1].z + cross.z * (trackWidth - railWidth) / 2);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	if (!doingShadows)
		glColor3ub(70, 70, 70);
	glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i < splinePts.size() + 1; i++) {
		cross = crossVecs[i];
		glVertex3f(splinePts[i % splinePts.size()].x + cross.x * (-trackWidth + railWidth) / 2, splinePts[i % splinePts.size()].y + cross.y * (-trackWidth + railWidth) / 2, splinePts[i % splinePts.size()].z + cross.z * (-trackWidth + railWidth) / 2);
		glVertex3f(splinePts[(i + 1) % splinePts.size()].x + cross.x * (-trackWidth - railWidth) / 2, splinePts[(i + 1) % splinePts.size()].y + cross.y * (-trackWidth - railWidth) / 2, splinePts[(i + 1) % splinePts.size()].z + cross.z * (-trackWidth - railWidth) / 2);
	}
	glEnd();
	glPopMatrix();

	int lastWood = 0;
	float woodW = 2;
	float woodH = .75;

	glPushMatrix();
	if (!doingShadows)
		glColor3ub(77, 61, 40);
	glTranslatef(splinePts[0].x, splinePts[0].y, splinePts[0].z);
	float theta1 = -radiansToDegrees(atan2(crossVecs[0].z, crossVecs[0].x));
	glRotatef(theta1, 0, 1, 0);
	Pnt3f vecY = Pnt3f(splinePts[1].x - splinePts[0].x, splinePts[1].y - splinePts[0].y, splinePts[1].z - splinePts[0].z).normalize();
	float theta3 = radiansToDegrees(asin(vecY.y));
	if (splineVecs[0].y < 0)
		theta3 = -theta3;
	float theta2 = radiansToDegrees(asin(crossVecs[0].y));
	glRotatef(theta2, 0, 0, 1);
	
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(trackWidth, woodH, woodW);
	glVertex3f(-trackWidth, woodH, woodW);
	glVertex3f(-trackWidth, -woodH, woodW);
	glVertex3f(trackWidth, -woodH, woodW);

	glNormal3f(0, 0, -1);
	glVertex3f(trackWidth, woodH, -woodW);
	glVertex3f(trackWidth, -woodH, -woodW);
	glVertex3f(-trackWidth, -woodH, -woodW);
	glVertex3f(-trackWidth, woodH, -woodW);

	glNormal3f(0, 1, 0);
	glVertex3f(trackWidth, woodH, woodW);
	glVertex3f(trackWidth, woodH, -woodW);
	glVertex3f(-trackWidth, woodH, -woodW);
	glVertex3f(-trackWidth, woodH, woodW);

	glNormal3f(0, -1, 0);
	glVertex3f(trackWidth, -woodH, woodW);
	glVertex3f(-trackWidth, -woodH, woodW);
	glVertex3f(-trackWidth, -woodH, -woodW);
	glVertex3f(trackWidth, -woodH, -woodW);

	glNormal3f(1, 0, 0);
	glVertex3f(trackWidth, woodH, woodW);
	glVertex3f(trackWidth, -woodH, woodW);
	glVertex3f(trackWidth, -woodH, -woodW);
	glVertex3f(trackWidth, woodH, -woodW);

	glNormal3f(-1, 0, 0);
	glVertex3f(-trackWidth, woodH, woodW);
	glVertex3f(-trackWidth, woodH, -woodW);
	glVertex3f(-trackWidth, -woodH, -woodW);
	glVertex3f(-trackWidth, -woodH, woodW);

	glEnd();
	glPopMatrix();
	
	for (i = 0; i < splinePts.size(); i++) {
		float xD = (splinePts[i].x - splinePts[lastWood].x);
		float yD = (splinePts[i].y - splinePts[lastWood].y);
		float zD = (splinePts[i].z - splinePts[lastWood].z);
		float dis = sqrt(pow(xD, 2) + pow(yD, 2) + pow(zD, 2));
		if (dis >= 10) {
			glPushMatrix();
			if (!doingShadows)
				glColor3ub(77, 61, 40);

			glTranslatef(splinePts[i].x, splinePts[i].y, splinePts[i].z);
			
			float theta1 = -radiansToDegrees(atan2(crossVecs[i].z, crossVecs[i].x));
			glRotatef(theta1, 0, 1, 0);
			Pnt3f vecY = Pnt3f(splinePts[(i + 1) % splinePts.size()].x - splinePts[i].x, splinePts[(i + 1) % splinePts.size()].y - splinePts[i].y, splinePts[(i + 1) % splinePts.size()].z - splinePts[i].z).normalize();
			float theta3 = radiansToDegrees(asin(vecY.y));
			if (splineVecs[i%splineVecs.size()].y < 0)
				theta3 = -theta3;
			glRotatef(theta3, 1, 0, 0);
			float theta2 = radiansToDegrees(asin(crossVecs[i].y));
			glRotatef(theta2, 0, 0, 1);
			
			

			glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);
			glVertex3f(trackWidth, woodH, woodW);
			glVertex3f(-trackWidth, woodH, woodW);
			glVertex3f(-trackWidth, -woodH, woodW);
			glVertex3f(trackWidth, -woodH, woodW);

			glNormal3f(0, 0, -1);
			glVertex3f(trackWidth, woodH, -woodW);
			glVertex3f(trackWidth, -woodH, -woodW);
			glVertex3f(-trackWidth, -woodH, -woodW);
			glVertex3f(-trackWidth, woodH, -woodW);

			glNormal3f(0, 1, 0);
			glVertex3f(trackWidth, woodH, woodW);
			glVertex3f(trackWidth, woodH, -woodW);
			glVertex3f(-trackWidth, woodH, -woodW);
			glVertex3f(-trackWidth, woodH, woodW);

			glNormal3f(0, -1, 0);
			glVertex3f(trackWidth, -woodH, woodW);
			glVertex3f(-trackWidth, -woodH, woodW);
			glVertex3f(-trackWidth, -woodH, -woodW);
			glVertex3f(trackWidth, -woodH, -woodW);

			glNormal3f(1, 0, 0);
			glVertex3f(trackWidth, woodH, woodW);
			glVertex3f(trackWidth, -woodH, woodW);
			glVertex3f(trackWidth, -woodH, -woodW);
			glVertex3f(trackWidth, woodH, -woodW);

			glNormal3f(-1, 0, 0);
			glVertex3f(-trackWidth, woodH, woodW);
			glVertex3f(-trackWidth, woodH, -woodW);
			glVertex3f(-trackWidth, -woodH, -woodW);
			glVertex3f(-trackWidth, -woodH, woodW);

			glEnd();
			glPopMatrix();

			lastWood = i;
		}
	}
}

void CTrack::drawATrack(float x, float y, float z, float scale)
{
	/*int maxIndex, minIndex;
	float max, min;
	max = 0;
	min = 10000;*/
	if (vertices.size() != 0) {
		glPushMatrix();
		glTranslatef(x, y, z);
		glColor3ub(137, 88, 46);
		for (int i = 0; i < vertices.size(); i += 3) {
			glBegin(GL_TRIANGLES);
			glNormal3f(normals[i].x, normals[i].y, normals[i].z);
			glVertex3f(vertices[i].x * scale, vertices[i].y * scale, vertices[i].z * scale);
			/*if (vertices[i+2] > max) {
			max = vertices[i+2];
			maxIndex = i+2;
			}
			if (vertices[i+2] < min) {
			min = vertices[i+2];
			minIndex = i+2;
			}*/
			glNormal3f(normals[i + 1].x, normals[i + 1].y, normals[i + 1].z);
			glVertex3f(vertices[i + 1].x * scale, vertices[i + 1].y * scale, vertices[i + 1].z * scale);
			/*if (vertices[i + 5] > max) {
			max = vertices[i + 5];
			maxIndex = i + 5;
			}
			if (vertices[i + 5] < min) {
			min = vertices[i + 5];
			minIndex = i + 5;
			}*/
			glNormal3f(normals[i + 2].x, normals[i + 2].y, normals[i + 2].z);
			glVertex3f(vertices[i + 2].x * scale, vertices[i + 2].y * scale, vertices[i + 2].z * scale);
			/*if (vertices[i + 8] > max) {
			max = vertices[i + 8];
			maxIndex = i + 8;
			}
			if (vertices[i + 8] < min) {
			min = vertices[i + 8];
			minIndex = i + 8;
			}*/
			glEnd();
		}
		glPopMatrix();
	}
	/*std::cout << vertices[maxIndex] << " " << vertices[maxIndex + 1] << " " << vertices[maxIndex + 2] << std::endl;
	std::cout << vertices[minIndex] << " " << vertices[minIndex + 1] << " " << vertices[minIndex + 2] << std::endl;*/
}

void CTrack::drawRoad(bool doingShadows)
{
	float width = 10;
	glPushMatrix();
	if (!doingShadows)
		glColor3ub(60, 60, 60);
	glBegin(GL_TRIANGLE_STRIP);
	int i;
	for (i = 0; i < splinePts.size(); i++) {
		Pnt3f vec = Pnt3f(splinePts[(i + 1) % splinePts.size()].x - splinePts[i].x, splinePts[(i + 1) % splinePts.size()].y - splinePts[i].y, splinePts[(i + 1) % splinePts.size()].z - splinePts[i].z);
		Pnt3f cross;
		cross.x = vec.y * splineVecs[i].z - vec.z * splineVecs[i].y;
		cross.y = vec.z * splineVecs[i].x - vec.x * splineVecs[i].z;
		cross.z = vec.x * splineVecs[i].y - vec.y * splineVecs[i].x;
		cross.normalize();
		cross = width / 2 * cross;
		glVertex3f(splinePts[i].x + cross.x, splinePts[i].y + cross.y, splinePts[i].z + cross.z);
		glVertex3f(splinePts[(i + 1) % splinePts.size()].x - cross.x, splinePts[(i + 1) % splinePts.size()].y - cross.y, splinePts[(i + 1) % splinePts.size()].z - cross.z);
	}
	Pnt3f vec = Pnt3f(splinePts[1].x - splinePts[0].x, splinePts[1].y - splinePts[0].y, splinePts[1].z - splinePts[0].z);
	Pnt3f cross;
	cross.x = vec.y * splineVecs[0].z - vec.z * splineVecs[0].y;
	cross.y = vec.z * splineVecs[0].x - vec.x * splineVecs[0].z;
	cross.z = vec.x * splineVecs[0].y - vec.y * splineVecs[0].x;
	cross.normalize();
	cross = width / 2 * cross;
	glVertex3f(splinePts[0].x + cross.x, splinePts[0].y + cross.y, splinePts[0].z + cross.z);
	glVertex3f(splinePts[1].x - cross.x, splinePts[1].y - cross.y, splinePts[1].z - cross.z);
	glEnd();
	glPopMatrix();
}

void CTrack::drawTrain(bool doingShadows)
{
	float size = 3;
	float trainH = 4.0;
	int index = (splinePts.size() - 1) * trainU;

	Pnt3f vec = Pnt3f(splinePts[(index + 1) % splinePts.size()].x - splinePts[index].x, splinePts[(index + 1) % splinePts.size()].y - splinePts[index].y, splinePts[(index + 1) % splinePts.size()].z - splinePts[index].z);
	Pnt3f cross;
	cross.x = vec.y * splineVecs[index].z - vec.z * splineVecs[index].y;
	cross.y = vec.z * splineVecs[index].x - vec.x * splineVecs[index].z;
	cross.z = vec.x * splineVecs[index].y - vec.y * splineVecs[index].x;
	cross.normalize();

	glPushMatrix();
	glTranslatef(splinePts[index].x, splinePts[index].y, splinePts[index].z);
	float theta1 = -radiansToDegrees(atan2(cross.z, cross.x));
	glRotatef(theta1, 0, 1, 0);

	Pnt3f vecY = Pnt3f(splinePts[(index + 1) % splinePts.size()].x - splinePts[index].x, splinePts[(index + 1) % splinePts.size()].y - splinePts[index].y, splinePts[(index + 1) % splinePts.size()].z - splinePts[index].z).normalize();
	float theta3 = radiansToDegrees(asin(vecY.y));
	if (splineVecs[index%splineVecs.size()].y < 0) {
		theta3 = -theta3;
		trainH = -trainH;
	}
	glRotatef(theta3, 1, 0, 0);
	float theta2 = radiansToDegrees(asin(cross.y));
	glRotatef(theta2, 0, 0, 1);
	if (!doingShadows)
		glColor3ub(40, 40, 200);

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(size, size + trainH, size*2);
	glVertex3f(-size, size + trainH, size*2);
	glVertex3f(-size, -size + trainH, size*2);
	glVertex3f(size, -size + trainH, size*2);

	glNormal3f(0, 0, -1);
	glVertex3f(size, size + trainH, -size*2);
	glVertex3f(size, -size + trainH, -size*2);
	glVertex3f(-size, -size + trainH, -size*2);
	glVertex3f(-size, size + trainH, -size*2);

	glNormal3f(0, 1, 0);
	glVertex3f(size, size + trainH, size*2);
	glVertex3f(size, size + trainH, -size*2);
	glVertex3f(-size, size + trainH, -size*2);
	glVertex3f(-size, size + trainH, size*2);

	glNormal3f(0, -1, 0);
	glVertex3f(size, -size + trainH, size*2);
	glVertex3f(-size, -size + trainH, size*2);
	glVertex3f(-size, -size + trainH, -size * 2);
	glVertex3f(size, -size + trainH, -size * 2);

	glNormal3f(1, 0, 0);
	glVertex3f(size, size + trainH, size * 2);
	glVertex3f(size, -size + trainH, size * 2);
	glVertex3f(size, -size + trainH, -size * 2);
	glVertex3f(size, size + trainH, -size * 2);

	glNormal3f(-1, 0, 0);
	glVertex3f(-size, size + trainH, size * 2);
	glVertex3f(-size, size + trainH, -size * 2);
	glVertex3f(-size, -size + trainH, -size * 2);
	glVertex3f(-size, -size + trainH, size * 2);
	glEnd();
	glPopMatrix();

	if(trainH < 0)
		trainH = -trainH;

	for (int n = 1, last = index; n < trainNums; n++) {
		for (int i = 0;; i++) {
			float xD = (splinePts[last].x - splinePts[(last + i) % splinePts.size()].x);
			float yD = (splinePts[last].y - splinePts[(last + i) % splinePts.size()].y);
			float zD = (splinePts[last].z - splinePts[(last + i) % splinePts.size()].z);
			float dis = sqrt(pow(xD, 2) + pow(yD, 2) + pow(zD, 2));
			if (dis > size * 4.5) {
				last = (last + i) % splinePts.size();

				vec = Pnt3f(splinePts[(last + 1) % splinePts.size()].x - splinePts[last].x, splinePts[(last + 1) % splinePts.size()].y - splinePts[last].y, splinePts[(last + 1) % splinePts.size()].z - splinePts[last].z);
				cross.x = vec.y * splineVecs[last].z - vec.z * splineVecs[last].y;
				cross.y = vec.z * splineVecs[last].x - vec.x * splineVecs[last].z;
				cross.z = vec.x * splineVecs[last].y - vec.y * splineVecs[last].x;
				cross.normalize();

				glPushMatrix();
				glTranslatef(splinePts[last].x, splinePts[last].y, splinePts[last].z);
				float theta1 = -radiansToDegrees(atan2(cross.z, cross.x));
				glRotatef(theta1, 0, 1, 0);
				Pnt3f vecY = Pnt3f(splinePts[(last + 1) % splinePts.size()].x - splinePts[last].x, splinePts[(last + 1) % splinePts.size()].y - splinePts[last].y, splinePts[(last + 1) % splinePts.size()].z - splinePts[last].z).normalize();
				float theta3 = radiansToDegrees(asin(vecY.y));

				if (splineVecs[last%splineVecs.size()].y < 0) {
					theta3 = -theta3;
					trainH = -trainH;
				}
				glRotatef(theta3, 1, 0, 0);
				float theta2 = radiansToDegrees(asin(cross.y));
				glRotatef(theta2, 0, 0, 1);
				if (!doingShadows)
					glColor3ub(40, 40, 200);

				glBegin(GL_QUADS);
				glNormal3f(0, 0, 1);
				glVertex3f(size, size + trainH, size * 2);
				glVertex3f(-size, size + trainH, size * 2);
				glVertex3f(-size, -size + trainH, size * 2);
				glVertex3f(size, -size + trainH, size * 2);

				glNormal3f(0, 0, -1);
				glVertex3f(size, size + trainH, -size * 2);
				glVertex3f(size, -size + trainH, -size * 2);
				glVertex3f(-size, -size + trainH, -size * 2);
				glVertex3f(-size, size + trainH, -size * 2);

				glNormal3f(0, 1, 0);
				glVertex3f(size, size + trainH, size * 2);
				glVertex3f(size, size + trainH, -size * 2);
				glVertex3f(-size, size + trainH, -size * 2);
				glVertex3f(-size, size + trainH, size * 2);

				glNormal3f(0, -1, 0);
				glVertex3f(size, -size + trainH, size * 2);
				glVertex3f(-size, -size + trainH, size * 2);
				glVertex3f(-size, -size + trainH, -size * 2);
				glVertex3f(size, -size + trainH, -size * 2);

				glNormal3f(1, 0, 0);
				glVertex3f(size, size + trainH, size * 2);
				glVertex3f(size, -size + trainH, size * 2);
				glVertex3f(size, -size + trainH, -size * 2);
				glVertex3f(size, size + trainH, -size * 2);

				glNormal3f(-1, 0, 0);
				glVertex3f(-size, size + trainH, size * 2);
				glVertex3f(-size, size + trainH, -size * 2);
				glVertex3f(-size, -size + trainH, -size * 2);
				glVertex3f(-size, -size + trainH, size * 2);
				glEnd();
				glPopMatrix();
				if (trainH < 0)
					trainH = -trainH;
				break;
			}
			if (n == trainNums - 1) {
				trainCam = splinePts[last];
				trainCam.z += size * 2;
				trainCam.y += trainH;
			}
		}
	}
	trainU += trainSpeed;
}

void CTrack::calTrainCam()
{
	float size = 3;
	float trainH = 4.0;
	int index = (splinePts.size() - 1) * trainU;

	Pnt3f vec = Pnt3f(splinePts[(index + 1) % splinePts.size()].x - splinePts[index].x, splinePts[(index + 1) % splinePts.size()].y - splinePts[index].y, splinePts[(index + 1) % splinePts.size()].z - splinePts[index].z);
	Pnt3f cross;
	cross.x = vec.y * splineVecs[index].z - vec.z * splineVecs[index].y;
	cross.y = vec.z * splineVecs[index].x - vec.x * splineVecs[index].z;
	cross.z = vec.x * splineVecs[index].y - vec.y * splineVecs[index].x;
	cross.normalize();

	glPushMatrix();
	glTranslatef(splinePts[index].x, splinePts[index].y, splinePts[index].z);
	float theta1 = -radiansToDegrees(atan2(cross.z, cross.x));
	glRotatef(theta1, 0, 1, 0);

	Pnt3f vecY = Pnt3f(splinePts[(index + 1) % splinePts.size()].x - splinePts[index].x, splinePts[(index + 1) % splinePts.size()].y - splinePts[index].y, splinePts[(index + 1) % splinePts.size()].z - splinePts[index].z).normalize();
	float theta3 = radiansToDegrees(asin(vecY.y));
	if (splineVecs[index%splineVecs.size()].y < 0) {
		theta3 = -theta3;
		trainH = -trainH;
	}
	glRotatef(theta3, 1, 0, 0);
	float theta2 = radiansToDegrees(asin(cross.y));
	glRotatef(theta2, 0, 0, 1);
	glPopMatrix();

	if (trainH < 0)
		trainH = -trainH;

	for (int n = 1, last = index; n < trainNums; n++) {
		for (int i = 0;; i++) {
			float xD = (splinePts[last].x - splinePts[(last + i) % splinePts.size()].x);
			float yD = (splinePts[last].y - splinePts[(last + i) % splinePts.size()].y);
			float zD = (splinePts[last].z - splinePts[(last + i) % splinePts.size()].z);
			float dis = sqrt(pow(xD, 2) + pow(yD, 2) + pow(zD, 2));
			if (dis > size * 4.5) {
				last = (last + i) % splinePts.size();

				vec = Pnt3f(splinePts[(last + 1) % splinePts.size()].x - splinePts[last].x, splinePts[(last + 1) % splinePts.size()].y - splinePts[last].y, splinePts[(last + 1) % splinePts.size()].z - splinePts[last].z);
				cross.x = vec.y * splineVecs[last].z - vec.z * splineVecs[last].y;
				cross.y = vec.z * splineVecs[last].x - vec.x * splineVecs[last].z;
				cross.z = vec.x * splineVecs[last].y - vec.y * splineVecs[last].x;
				cross.normalize();

				glPushMatrix();
				glTranslatef(splinePts[last].x, splinePts[last].y, splinePts[last].z);
				float theta1 = -radiansToDegrees(atan2(cross.z, cross.x));
				glRotatef(theta1, 0, 1, 0);
				Pnt3f vecY = Pnt3f(splinePts[(last + 1) % splinePts.size()].x - splinePts[last].x, splinePts[(last + 1) % splinePts.size()].y - splinePts[last].y, splinePts[(last + 1) % splinePts.size()].z - splinePts[last].z).normalize();
				float theta3 = radiansToDegrees(asin(vecY.y));

				if (splineVecs[last%splineVecs.size()].y < 0) {
					trainH = -trainH;
				}
				glRotatef(theta3, 1, 0, 0);
				float theta2 = radiansToDegrees(asin(cross.y));
				glRotatef(theta2, 0, 0, 1);
				if (n == trainNums - 1) {
					trainCam = splinePts[last];
					trainCam.y += trainH;

					trainCamRotate = Pnt3f(theta3, theta1, theta2);
					if (splineVecs[last%splineVecs.size()].y < 0) {
						if (abs(trainCamRotate.x) >= abs(trainCamRotate.z)) {
							//trainCamRotate.y *= -1;
							//trainCamRotate.x = +180;
							trainCamRotate.z = +180;
							trainCamRotate.x *= -1;
							//trainCamRotate.z *= -1;
							trainCamRotate.y = +180;
						}
						else {
							trainCamRotate.x = +180;
							trainCamRotate.z *= -1;
							trainCamRotate.y = +180;
						}
					}
				}
				glPopMatrix();
				if (trainH < 0)
					trainH = -trainH;
				break;
			}
		}
	}
	trainU += trainSpeed;
}

void CTrack::calTrainCam2()
{
	float size = 3;
	float trainH = 4.0;
	int index = (splinePts.size() - 1) * trainU;

	Pnt3f vec = Pnt3f(splinePts[(index + 1) % splinePts.size()].x - splinePts[index].x, splinePts[(index + 1) % splinePts.size()].y - splinePts[index].y, splinePts[(index + 1) % splinePts.size()].z - splinePts[index].z);
	Pnt3f cross;
	cross.x = vec.y * splineVecs[index].z - vec.z * splineVecs[index].y;
	cross.y = vec.z * splineVecs[index].x - vec.x * splineVecs[index].z;
	cross.z = vec.x * splineVecs[index].y - vec.y * splineVecs[index].x;
	cross.normalize();

	glPushMatrix();
	glTranslatef(splinePts[index].x, splinePts[index].y, splinePts[index].z);
	float theta1 = -radiansToDegrees(atan2(cross.z, cross.x));
	glRotatef(theta1, 0, 1, 0);

	Pnt3f vecY = Pnt3f(splinePts[(index + 1) % splinePts.size()].x - splinePts[index].x, splinePts[(index + 1) % splinePts.size()].y - splinePts[index].y, splinePts[(index + 1) % splinePts.size()].z - splinePts[index].z).normalize();
	float theta3 = radiansToDegrees(asin(vecY.y));
	if (splineVecs[index%splineVecs.size()].y < 0) {
		theta3 = -theta3;
		trainH = -trainH;
	}
	glRotatef(theta3, 1, 0, 0);
	float theta2 = radiansToDegrees(asin(cross.y));
	glRotatef(theta2, 0, 0, 1);
	glPopMatrix();

	if (trainH < 0)
		trainH = -trainH;

	for (int n = 1, last = index; n < trainNums; n++) {
		for (int i = 0;; i++) {
			float xD = (splinePts[last].x - splinePts[(last + i) % splinePts.size()].x);
			float yD = (splinePts[last].y - splinePts[(last + i) % splinePts.size()].y);
			float zD = (splinePts[last].z - splinePts[(last + i) % splinePts.size()].z);
			float dis = sqrt(pow(xD, 2) + pow(yD, 2) + pow(zD, 2));
			if (dis > size * 4.5) {
				last = (last + i) % splinePts.size();

				vec = Pnt3f(splinePts[(last + 1) % splinePts.size()].x - splinePts[last].x, splinePts[(last + 1) % splinePts.size()].y - splinePts[last].y, splinePts[(last + 1) % splinePts.size()].z - splinePts[last].z);
				cross.x = vec.y * splineVecs[last].z - vec.z * splineVecs[last].y;
				cross.y = vec.z * splineVecs[last].x - vec.x * splineVecs[last].z;
				cross.z = vec.x * splineVecs[last].y - vec.y * splineVecs[last].x;
				cross.normalize();

				glPushMatrix();
				glTranslatef(splinePts[last].x, splinePts[last].y, splinePts[last].z);
				float theta1 = radiansToDegrees(atan2(cross.z, cross.x));//heading
				glRotatef(theta1, 0, 1, 0);
				Pnt3f vecY = Pnt3f(splinePts[(last + 1) % splinePts.size()].x - splinePts[last].x, splinePts[(last + 1) % splinePts.size()].y - splinePts[last].y, splinePts[(last + 1) % splinePts.size()].z - splinePts[last].z).normalize();
				float theta3 = radiansToDegrees(asin(vecY.y));//pitch

				if (splineVecs[last%splineVecs.size()].y < 0) {
					trainH = -trainH;
				}
				glRotatef(theta3, 1, 0, 0);

				Pnt3f W0 = Pnt3f(-vecY.z, 0, vecY.x);
				Pnt3f U0;
				U0.x = W0.y * vecY.z - W0.z * vecY.y;
				U0.y = W0.z * vecY.x - W0.x * vecY.z;
				U0.z = W0.x * vecY.y - W0.y * vecY.x;

				float theta2 = radiansToDegrees(atan2(Dot(W0, splineVecs[last%splineVecs.size()]) / W0.length(), Dot(U0, splineVecs[last%splineVecs.size()]) / U0.length()));
				glRotatef(theta2, 0, 0, 1);
				if (n == trainNums - 1) {
					trainCam = splinePts[last];
					trainCam.y += trainH;

					trainCamRotate = Pnt3f(theta3, theta1, theta2);
					if (splineVecs[last%splineVecs.size()].y < 0) {
						//trainCamRotate.x *= -1;
						trainCamRotate.y = +180;
						/*if (vecY.y < 0) {
							trainCamRotate.z *= -1;
						}*/
					}
				}
				glPopMatrix();
				if (trainH < 0)
					trainH = -trainH;
				break;
			}
		}
	}
	trainU += trainSpeed;
}

float CTrack::Dot(Pnt3f a, Pnt3f b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
