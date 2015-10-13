/*
 * spline.cpp
 *
 *  Created on: 9/10/2015
 *      Author: bryerscame
 */
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include "spline.hpp"
#include "comp308.hpp"

using namespace std;
using namespace comp308;

void Spline::renderSpline() {
	// cout << "RENDERING SPLINE..." << endl;
	// cout << numPts << endl;

	// Points on the spine
	float xcr, ycr;

	if ((int) m_points.size() > 3) {

		m_points.clear();

		for (int i = 1; i < (int) m_points.size() - 2; ++i) {
			float length = 0;

			for (int k = 0; k < 50; ++k) {
				float t = k * 0.02;

				// To the calculations
				xcr = m_keyframes[i].x
						+ 0.5 * t
								* (-m_keyframes[i - 1].x + m_keyframes[i + 1].x)
						+ t * t
								* (m_keyframes[i - 1].x - 2.5 * m_keyframes[i].x
										+ 2 * m_keyframes[i + 1].x
										- 0.5 * m_keyframes[i + 2].x)
						+ t * t * t
								* (-0.5 * m_keyframes[i - 1].x
										+ 1.5 * m_keyframes[i].x
										- 1.5 * m_keyframes[i + 1].x
										+ 0.5 * m_keyframes[i + 2].x);

				ycr = m_keyframes[i].y
						+ 0.5 * t
								* (-m_keyframes[i - 1].y + m_keyframes[i + 1].y)
						+ t * t
								* (m_keyframes[i - 1].y - 2.5 * m_keyframes[i].y
										+ 2 * m_keyframes[i + 1].y
										- 0.5 * m_keyframes[i + 2].y)
						+ t * t * t
								* (-0.5 * m_keyframes[i - 1].y
										+ 1.5 * m_keyframes[i].y
										- 1.5 * m_keyframes[i + 1].y
										+ 0.5 * m_keyframes[i + 2].y);

				m_points.push_back(vec3(xcr, ycr, 0));
			}
		}
	}
}


/* Calculates the Y value given four points
 * Uses equation from
 * http://www.mvps.org/directx/articles/catmull/ */
comp308::vec2 Spline::calculatePoint(vec2 p0,vec2 p1,vec2 p2,vec2 p3, float t){

	float resultY = ((2*p1.y)+
			(-p0.y + p2.y)*t+
			(2*p0.y - 5*p1.y + 4*p2.y - p3.y)*t*t+
			(-p0.y+3*p1.y-3*p2.y+p3.y)*t*t*t)*0.5f;
	float resultX = ((2*p1.x)+
			(-p0.x+ p2.x)*t+
			(2*p0.x - 5*p1.x + 4*p2.x- p3.x)*t*t+
			(-p0.x+3*p1.x-3*p2.x+p3.x)*t*t*t)*0.5f;

	vec2 result = {resultX,resultY};
	return result;
}
float Spline::calculateYValue(vector<vec2> points,float x_dist){

	float x = points[1].x;//set first point for comparison
	int i = 2;
	while(x<x_dist&&i<=points.size()){
		x = points[i].x;
		++i;
	}//find the mid point of the spline
	vec2 a = points[i-3];
	vec2 b = points[i-2];
	vec2 c = points[i-1];
	vec2 d = points[i];
	float t = c.x - b.x;	//difference between middle points
	t = (x_dist-b.x)/t;		//over the difference between middle point and x distnace
	vec2 result = calculatePoint(a,b,c,d,t);
	return result.y;//return value between 0 and 1
}

Spline::~Spline() {
	// TODO Auto-generated destructor stub
}

