//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Tana Tanoi
//
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "comp308.hpp"
#include "building.hpp"
#include "imageLoader.hpp"
#include "shaderLoader.hpp"
using namespace std;
using namespace comp308;

Building::Building(){
//	initShader();
}


GLuint g_texture = 0;
void Building::initShader() {
	//Gets stuck here, i.e. cout won't print
	g_shader = makeShaderProgram("../work/res/shaders/shaderDemo.vert", "../work/res/shaders/shaderDemo.frag");
	cout<<"Initd shader"<<endl;
}
void Building::initTexture() {//TODO this method
}

/*Generates a block from the given floor plan *floor from the given
 * height; *elevation.
 * *floor must be a list of points where a wall is connected between
 * point i and point i +1 (where last and first point are connected)
 *
 */
float Building::generateBlock(std::vector<comp308::vec2> floor, float elevation) {
	glUseProgram(g_shader);
	int n = floor.size();
	vector<vec3> bot;
	vector<vec3> top;
	/*Height is a value between 1 and 1.5 + the elevation (so height-elevation is the change in Y)*/
	float height = 0.5f;//static_cast <float> (rand()) / static_cast <float> (RAND_MAX/0.5f)+1.0f;
	height+=elevation;
	for (vec2 v2 : floor) {
		bot.push_back(vec3(v2.x, elevation, v2.y));
		top.push_back(vec3(v2.x, height, v2.y));
	}

	glBegin(GL_QUADS);
	/*n amount of walls*/
	for (int i = 0; i < n; i++) {
		if(i %2==0){
			glColor3f(0.7f,0.1f,0.4f);
		}else{
			glColor3f(0.1f,0.2f,0.9f);
		}
		vec3 topl = top[i];
		vec3 topr = top[(i + 1) % n];
		vec3 botr = bot[(i + 1) % n];
		vec3 botl = bot[i];
		vec3 normal = cross((botl - topr), (topl - topr));
		normal = normalize(normal);
		//glTexCoord2d(0, 0);
		glNormal3f(normal.x, normal.y, normal.z);//baisc normal, probably the same as the other stuff
		glVertex3f(topl.x, topl.y, topl.z);
		glVertex3f(botl.x, botl.y, botl.z);
		glVertex3f(botr.x, botr.y, botr.z);
		glVertex3f(topr.x, topr.y, topr.z);
	}
	glEnd();
	glBegin(GL_POLYGON);
	/*roof*/
	glColor3f(0,0,0);
	glNormal3f(0, 1, 0);
	for (vec3 roofpart : top) {
		glVertex3f(roofpart.x, roofpart.y, roofpart.z);
	}
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3f(0, -1, 0);
	for (vec3 floorPoint: bot) {
		glVertex3f(floorPoint.x, floorPoint.y, floorPoint.z);
	}

	glEnd();

	return height;
}

/*Generates a building based on the instructions from the input string,
 * and the floor points.
 */
void Building::generateFromString(std::vector<comp308::vec2> floor,string input) {
	/*Generate first floor REPLACE ME ONCE TEXTURES ARE IN AND WE CAN HAVE AN ACTUAL FLOOR*/

	float height = generateBlock(floor, 0.0f);
	for (int i = 0; i < input.length(); i++) {
		switch (input[i]) {
		case 'S':
			floor = shrinkPoints(floor);
			break;
		case 'E':
			height = generateBlock(floor, height);
			break;
		case 'R':
			if(rand()%2==0){
				generatePointRoof(floor, height);
			}else{
				generateFlatPointRoof(floor,height);
			}
			return;
		}

	}
}



void Building::generatePointRoof(std::vector<comp308::vec2> points, float elevation){
	float height = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 1.0f)+0.3f;
	vec2 mid = centerPoint(points);
	vec3 peak = vec3(mid.x, elevation +height,mid.y);
	int n = points.size();
	glBegin(GL_TRIANGLES);
		for (int i = 0; i < n; i++) {
			glVertex3f(points[i].x, elevation, points[i].y);
			glVertex3f(peak.x, peak.y, peak.z);
			glVertex3f(points[(i + 1) % n].x, elevation, points[(i + 1) % n].y );
		}
	glEnd();

}

void Building::generateFlatPointRoof(std::vector<comp308::vec2> points, float elevation){
	int n = points.size();
		vector<vec3> bot;
		vector<vec2> topPlan = shrinkPoints(shrinkPoints(points));
		vector<vec3> top;
		/*Height is a value between 1 and 1.5 + the elevation (so height-elevation is the change in Y)*/
		float height = 0.2f;//static_cast <float> (rand()) / static_cast <float> (RAND_MAX/0.5f)+1.0f;
		height+=elevation;
		for (int i = 0;i<n;i++) {
			bot.push_back(vec3(points[i].x, elevation, points[i].y));
			top.push_back(vec3(topPlan[i].x, height, topPlan[i].y));
		}

		glBegin(GL_QUADS);
		/*n amount of walls*/
		for (int i = 0; i < n; i++) {
			if(i %2==0){
				glColor3f(0.7f,0.1f,0.4f);
			}else{
				glColor3f(0.1f,0.2f,0.9f);
			}
			vec3 topl = top[i];
			vec3 topr = top[(i + 1) % n];
			vec3 botr = bot[(i + 1) % n];
			vec3 botl = bot[i];
			vec3 normal = cross((botl - topr), (topl - topr));
			normal = normalize(normal);
			glNormal3f(normal.x, normal.y, normal.z);//baisc normal, probably the same as the other stuff
			glVertex3f(topl.x, topl.y, topl.z);
			glVertex3f(botl.x, botl.y, botl.z);
			glVertex3f(botr.x, botr.y, botr.z);
			glVertex3f(topr.x, topr.y, topr.z);
		}
		glEnd();
		glBegin(GL_POLYGON);
		/*roof*/
		glColor3f(0,0,0);
		glNormal3f(0, 1, 0);
		for (vec3 roofpart : top) {
			glVertex3f(roofpart.x, roofpart.y, roofpart.z);
		}
		glEnd();
		glBegin(GL_POLYGON);
		glNormal3f(0, -1, 0);
		for (vec3 floorPoint: bot) {
			glVertex3f(floorPoint.x, floorPoint.y, floorPoint.z);
		}

		glEnd();

}


vector<vec2> Building::shrinkPoints(std::vector<vec2> points){
	vec2 mid = centerPoint(points);
	vector<vec2> smallPoints;
	float dist = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/0.2f)+0.1f;
	for(vec2 v2:points){
		vec2 diff = (mid - v2)*dist;

		smallPoints.push_back(v2+diff);
	}
	return smallPoints;
}


/*Returns a vec2 that represents a point in the center of the
 * shape created by *points.
 */
vec2 Building::centerPoint(vector<vec2> points){
	float x = 0;
	float y = 0;

	for(int i = 0; i < points.size();i++){
		x+=points[i].x;
		y+=points[i].y;
	}
	y/=points.size();
	x/=points.size();
	return vec2(x,y);

}

