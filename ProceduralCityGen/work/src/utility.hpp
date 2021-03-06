/*
 * utility.hpp
 *
 *  Created on: 26/09/2015
 *      Author: craighhann
 */
#pragma once

#include "comp308.hpp"
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
using namespace comp308;
using namespace std;

namespace util {

/*Cross platform _isnan*/
inline bool isNAN(float x) {
	return x != x;
}
/*Inverted cross platform _isfinite*/
inline bool isInfinite(float x) {
	return !isNAN(x) && isNAN(x - x);
}

/**
 A line represents a single line in a section outline
 */
struct line {
	comp308::vec2 start;
	comp308::vec2 end;
	int ID;
	float length;
};

struct section {
	std::vector<util::line> lines;
	int ID;
	float area;
	//Building:: building;
};

class noIntersectionException: public exception {
	const char* what() const throw () {
		return "The lines were parallel";
	}
};

/**
 * An oval that is used to check if one point is near another.
 */
class checkZone {
private:

	float m_x, m_y, m_width, m_height;

public:

	checkZone(float x, float y, float width, float height) :
			m_x(x), m_y(y), m_width(width), m_height(height) {}

	checkZone(float x, float y, float radius) :
			m_x(x), m_y(y), m_width(radius), m_height(radius) {}

	inline bool contains(comp308::vec3* position) {
		return contains(position->x, position->z);
	}

	inline bool contains(comp308::vec2* position) {
		return contains(position->x, position->y);
	}

	/**
	 * Does the zone contain this point?
	 */
	inline bool contains(float x, float y) {

		// Normalize the coordinates compared to the ellipse
		// having a center at 0,0 and a radius of 0.5.

		double ellw = m_width;

		if (ellw <= 0.0)
			return false;

		double normx = (x - m_x) / ellw - 0.5;
		double ellh = m_height;

		if (ellh <= 0.0)
			return false;
		double normy = (y - m_y) / ellh - 0.5;

		return (normx * normx + normy * normy) < 0.25;
	}
};

inline vec2 findPerp(vec2 dir){
	//cout << "finding perp" << endl;
	dir = normalize(dir);
	//cout << "It could be the normalizing" << endl;
	vec3 direction = vec3(dir.x, 0, dir.y);
	vec3 upvec = vec3(0,1,0);
	//cout << "It's probably the cross product" << endl;
	vec3 perp = cross(direction, upvec);
	//cout << "found perp " << endl;
	return vec2(perp.x,perp.z);
}

inline vec2 rotate(vec2 centrePoint, vec2 toRotate, float deg) {
	double angleInRadians = radians(deg);
	double cosTheta = cos(angleInRadians);
	double sinTheta = sin(angleInRadians);
	//cout << "cosTheta " << cosTheta << "  sinTheta " << sinTheta << endl;
	vec2 dir = toRotate-centrePoint;
	float x = (cosTheta * (dir.x) - sinTheta * (dir.y));
	float y = (sinTheta * (dir.x) + cosTheta * (dir.y));


	//cout << "  X  " << x << "   Y   " << y << endl;
	return vec2(x,y)+centrePoint;
}

/*Returns a vec2 where r.x = gradient and r.y = y-offset*/
inline vec2 getEquation(vec2 a, vec2 b) {
	vec2 equation;
	equation.x = (b.y - a.y) / (b.x - a.x);
	equation.y = (a.y - (equation.x * a.x));
	if (isNAN(equation.x) || isInfinite(equation.x)) { //if x is nan, then it is paralell to the y-axis, and thus can't be represented properly
	//instead we leave x as nan and change y to the x point along the axis where the line is
	////cout << "Equation from " << a.x << " " << a.y << " to " << b.x << " " << b.y << " is y-axis" << endl;
		equation.y = a.x;
	}
	return equation;
}

inline vec2 getIntersection(line l, vec2 cutDir, vec2 cutPoint) {
	////cout << "Line ID " << l.ID;
	float m_l = (l.end.y - l.start.y) / (l.end.x - l.start.x);
	float c_l = l.end.y - m_l * l.end.x;
	////cout << "Original line "<< m_l << "  " << c_l << endl;
	////cout << "Cut x " << cutDir.x <<  "Cut y " << cutDir.y << endl;

	float m = cutDir.y / cutDir.x;
	float c = cutPoint.y - m * cutPoint.x; // y = mx + c so c =  y - mx

	if (m == m_l || m == -m_l) { // two parallel lines
	////cout << "Parallel lines " << endl;
		throw noIntersectionException();
	}

	if (l.end.x == l.start.x) { // line is vertical
		////cout << "Existing line is vertical " << endl;
		float y = m * l.start.x + c; // finds appropriate y value on other line
		return vec2(l.start.x, y);
	}

	if (cutDir.x == 0) { // other line is vertical
		////cout << "New line is vertical " << endl;
		////cout << "Cut point " << cutPoint.x << "  " << cutPoint.y << endl;
		float y = m_l * cutPoint.x + c_l; // finds appropriate y value on  line
		return vec2(cutPoint.x, y);
	}

	////cout << "Bisector line "<< m << "  " << c << endl;

	// Calculates the intersection point
	float x = (c - c_l) / (m_l - m);
	float y = m_l * x + c_l;

	if (x == 0.0 && y == 0.0) {
		////cout << "Returning blank vec2" << endl;
	}

	return vec2(x, y);
}

inline vec2 getIntersection(line l, line o) {
	float m_l = (l.end.y - l.start.y) / (l.end.x - l.start.x);
	float c_l = l.end.y - m_l * l.end.x;

	float m = (o.end.y - o.start.y) / (o.end.x - o.start.x);
	float c = o.end.y - m * o.end.x;

	if (m == m_l || m == -m_l) {
		//cout << "Threw exception" << endl;
		throw noIntersectionException();
	}
	if (l.end.x == l.start.x) { // line is vertical
		////cout << "Vertical line" << endl;
		////cout << "Line " << l.start.x << ", " << l.start.y << "  "  << l.end.x << ", " << l.end.y << endl;
		////cout << "Other " << o.start.x << ", " << o.start.y << "  "  << o.end.x << ", " << o.end.y << endl;

		float y = m * l.start.x + c; // finds appropriate y value on other line

		////cout << "New " << l.start.x << " , " << y << endl;
		return vec2(l.start.x, y);
	}

	if (o.end.x == o.start.x) { // other line is vertical
		////cout << "Vertical line" << endl;
		float y = m_l * o.start.x + c_l; // finds appropriate y value on  line
		return vec2(o.start.x, y);
	}

	// Calculates the intersection point
	float x = (c - c_l) / (m_l - m);
	float y = m * x + c; // was m_l and c_l

	if (x == 337.5) {
		//cout << "Found bisector cut" << endl;
	}

	return vec2(x, y);
}

/*REQUIRES: The lines to actually intersect
 Intersection method given two lines, represented by 2 start and end vectors*/
inline vec2 getIntersection(vec2 a1, vec2 a2, vec2 b1, vec2 b2) {


	vec2 e1 = getEquation(a1, a2);
	vec2 e2 = getEquation(b1, b2);
	////cout << "Equations " << e1.x << " " << e1.y << " to " << e2.x << " " << e2.y << "|" << endl;
	//If paralell lines
	//get x value in which they meet
	float x = (e2.y - e1.y) / (e1.x - e2.x);
	//then also find the y value
	float y = e1.x * x + e1.y;
	vec2 toReturn = vec2(x, y);
	if (isInfinite(e1.x) || isNAN(e1.x)) {	//if equation one is parallel to Y
	//Set the toReturn.x value to the value of e1.y (which is the x-intercept)
		toReturn.x = e1.y;
		toReturn.y = e2.x * toReturn.x + e2.y;
	} else if (isInfinite(e2.x) || isNAN(e2.x)) {//if equation two is parallel to Y
	//Set the toReturn.x value to the value of e2.y (which is the x-intercept)
		toReturn.x = e2.y;  //then set the y value to mx+c using the new x value
		toReturn.y = e1.x * toReturn.x + e1.y;
	}
	return toReturn;
}

/*Tests if the two lines, represented as two vec2s each, intersect within the bounds of the points*/
inline bool intersects(vec2 a1, vec2 a2, vec2 b1, vec2 b2) {
	vec2 e1 = getEquation(a1, a2);
	vec2 e2 = getEquation(b1, b2);
	//If paralell lines or if both are paralell to the y-axis
	if (e1.x == e2.x) {
		//		//cout << "gradients are equal" << e1.x <<  " " << e2.x <<endl;
		return false;
	}

	if ((isInfinite(e1.x)) && (isInfinite(e2.x))) {
		//		//cout << "both are y axis" << endl;
		return false;
	}
	vec2 value = getIntersection(a1, a2, b1, b2);
	//float x = (e2.y - e1.y) / (e1.x - e2.x);
	float x = value.x;
	//if x is out of bounds X VALUE GOTTEN HERE IS WRONG , SHOULD BE 0.5
	if (x < min(a1.x, a2.x) || x > max(a2.x, a1.x) || x < min(b2.x, b1.x)
			|| x > max(b1.x, b2.x)) {
		//		//cout << "X is out of bounds "<<x << " " << a1.x  << " to " << a2.x<< endl;
		return false;
	}

	//if y is out of bounds
	if (value.y > max(a1.y, a2.y) || value.y < min(a2.y, a1.y)
			|| value.y > max(b1.y, b2.y) || value.y < min(b2.y, b1.y)) {
		//		//cout << a1.y << " to " << a2.y << endl;
		//		//cout << "Y value is out of bounds " << value.y <<  endl;
		return false;
	}

	//Return true if not parallel and if the point is within the bounds.
	return true;
}

inline bool intersects(line lon, vec2 cut, vec2 cutPoint) {
	//	//cout << "cutPoint " <<cutPoint.x << "  " << cutPoint.y << endl;

	vec2 intersection;
	try {
		intersection = getIntersection(lon, cut, cutPoint);
	} catch (const noIntersectionException &e) {
		//cout << "Caught parallel line" << endl;
		return false;
	} // This was a set of two parallel lines

	////cout << "Intersection Point " << intersection.x << " " << intersection.y << endl;

	// Now uses the intersection point of these two lines to determine if this is
	//the line it should split
	if (intersection.x == 0.0 && intersection.y == 0.0) {
		////cout << "Blank vec2 here " << endl;
	}

	if (intersection.x <= max(lon.start.x, lon.end.x)
			&& intersection.x >= min(lon.start.x, lon.end.x)
			&& intersection.y <= max(lon.start.y, lon.end.y)
			&& intersection.y >= min(lon.start.y, lon.end.y)) {
		//	//cout << "INTERSECTS" << endl;
		return true;
	}

	return false;
}

inline bool shareSlope(line edge, line secEdge) {
	// Checks if the lines are vertical
	if (edge.start.x - edge.end.x == 0
			&& secEdge.start.x - secEdge.end.x == 0) {
		return true;
	}

	// Gets line gradients
	float m = (edge.end.y - edge.start.y) / (edge.end.x - edge.start.x);
	float ms = (secEdge.end.y - secEdge.start.y)
			/ (secEdge.end.x - secEdge.start.x);
	// Casts to ints for comparison
	int m1 = int(round(ms));
	int m2 = int(round(m));

	if (m1 == m2) {
		//cout << "Shared slope" << endl;
		return true; // have same slope
	}
	return false; // have different slopes
}

inline float pointToLineDist( vec2 point, vec2 start, vec2 end){
     vec2 v = end - start;
     vec2 w = point - start;

     double c1 = dot(w,v);
     double c2 = dot(v,v);
     double b = c1 / c2;

     vec2 Pb = start + b * v;
     return distance(point, Pb);
}

inline vec2 getClosestPointOnLine( vec2 point, vec2 start, vec2 end){
     vec2 v = end - start;
     vec2 w = point - start;

     double c1 = dot(w,v);
     double c2 = dot(v,v);
     double b = c1 / c2;

     vec2 intersectPoint = start + b * v;
    return intersectPoint;
}

inline bool shareSide(line edge, line secEdge) {
	if (shareSlope(edge, secEdge)) {

		// Special vertical case
		if (edge.end.x - edge.start.x == 0) { // i.e. both are vertical as they share a slope
			float minHeight = min(edge.start.y, edge.end.y);
			float maxHeight = max(edge.start.y, edge.end.y);

			if ((secEdge.start.y > minHeight && secEdge.start.y < maxHeight)
					|| (secEdge.end.y > minHeight && secEdge.end.y < maxHeight)) {
				return (secEdge.end.x - edge.end.x == 0); // have same x intercept
			}
			return false;
		}

		float m = (edge.end.y - edge.start.y) / (edge.end.x - edge.start.x);

		float c = edge.end.y - m * edge.end.x;

		//cout << "slope " << m << " y intercept " << c << endl;

		// finish this
		// should now check if the start or end point lies on the equation line
		// i.e. calculate for edge.start.x what y is and see if they compare

		float y = secEdge.start.x * m + c;

		//cout << y << " " << int(round(secEdge.start.y)) << endl;

		if (int(round(y)) == int(round(secEdge.start.y))) {
			return true;
		}

		y = secEdge.end.x * m + c;
		//cout << y << " " << int(round(secEdge.end.y)) << endl;
		if (int(round(y)) == int(round(secEdge.end.y))) {
			return true;
		}

	}
	//cout << "Returned false" << endl;
	return false;
}

inline vector<vec2> linesToPoints(vector<line> lines) {
	vector<vec2> toReturn = vector<vec2>();
	for (line l : lines) {
		toReturn.push_back(l.start);
	}
	return toReturn;
}

inline vec2 getBisector(line l) {
	vec2 lineVec = l.end - l.start;
	vec2 lineVec2 = l.start - l.end;
	vec2 perpBi = vec2(-lineVec.y, lineVec.x); // gets perpendicular bisector to longest edge
	return perpBi;
}

inline float getLength(line l) {
	float xLength = abs(l.end.x - l.start.x);
	float yLength = abs(l.end.y - l.start.y);
	return sqrt(xLength * xLength + yLength * yLength);
}

inline vec2 getLineForLength(vec2 startPoint, vec2 direction, float length) {
	direction = normalize(direction);
	return startPoint + (direction * length);
}

inline vec2 centrePointOfLine(line l){
	vec2 dir = l.end - l.start;
	float leng = abs(length(dir));
	dir = normalize(dir);
	vec2 toReturn = l.start + dir * (leng / 2.0f);
	return toReturn;
}


inline bool floatEqual(float x, float y){
	//cout << "Equating " << x << " " << y <<  " " << ((int)round(x*10.0f) == (int)round(y*10.0f)) << endl;
	return (int)round(x*10.0f) == (int)round(y*10.0f);
}

inline bool vecEqual(vec2 a, vec2 b){
	return floatEqual(a.x, b.x) && floatEqual(a.y, b.y);
}

inline vector<line> linesIntersectingWithSection(section s, vec2 perpBi,
		vec2 centrePoint, line longLine) {
	// Now finds the first intersection point with another line within the section
	vector<line> intersectors = vector<line>();

	for (int i = 0; i < (int) s.lines.size(); i++) {
		if (s.lines[i].ID != longLine.ID) {
			if (intersects(s.lines[i], perpBi, centrePoint)) {
				intersectors.push_back(s.lines[i]);
			}
		}
	}
	return intersectors;
}

inline bool isPointOnLine(vec2 point, line edge){

	vec2 dir = edge.start - edge.end ;
	vec2 comp_dir = point-edge.end;
	if(length(comp_dir)==0){return true;}
	if(length(comp_dir)>=length(dir)){return false;}
	dir = normalize(dir);
	comp_dir = normalize(comp_dir);
	return (dir.x == comp_dir.x&&dir.y==comp_dir.y);
}

}
