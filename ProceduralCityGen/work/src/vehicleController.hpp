/*
 * vehicleController.hpp
 *
 *  Created on: 6/10/2015
 *      Author: bryerscame
 */
#pragma once
#ifndef SRC_VEHICLECONTROLLER_HPP_
#define SRC_VEHICLECONTROLLER_HPP_


/**
 * Used to show if there are cars at the surrounding roads at an intersection.
 */
struct intersection {
	bool foward, port, starboard;
};

class RoadNetwork;

// Controller for vehicles
class VehicleController {

private:

	std::vector<comp308::vec3> m_keyframes;
	std::vector<comp308::vec3> m_points;

	// Pointer to road network
	RoadNetwork *m_network;

	// Points that vehicles cannot occupy
	std::vector<comp308::vec3> m_outOfBounds;

	// Size of the map
	comp308::vec3 m_mapSize;

	// textures and cars
	std::vector<std::string> m_filenames_car;
	std::vector<std::string> m_filenames_tex;

	std::vector<Vehicle> m_vehicles;

	// Textures for vehicles
	GLuint *m_textures;

	void readConfig(std::string);
	void readTextures(std::string);
	void parseRoadNetwork(RoadNetwork*);
	void initVehicles();
	void initTexture(std::string, int);

	float disToNextVehicle(Vehicle*);
	Direction turnToTake(Vehicle*);
	Direction calculateTurn(Direction, Direction, Direction, Direction, float);
	intersection checkIntersections(comp308::vec3*);
	comp308::vec3 findTarget(Vehicle*);
	void interpolate_straight(Vehicle*, comp308::vec3*, comp308::vec3*);
	void interpolate_curve(Vehicle*, comp308::vec3*, comp308::vec3*);

public:

	VehicleController(std::string, std::string, std::vector<comp308::vec3>, comp308::vec3);
	virtual ~VehicleController();

	// Render all of the vehicles
	void tick();

	// Render a specific vehicle given a transformation and a texture
	void renderVehicle(Vehicle*, comp308::vec3, comp308::vec3, comp308::vec3, int);

	// Delete all pointers
	void cleanUp();

};




#endif /* SRC_VEHICLECONTROLLER_HPP_ */
