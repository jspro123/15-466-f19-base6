#pragma once

#include "Mode.hpp"
#include "Scene.hpp"
#include "Sound.hpp"


struct ObserveMode : Mode {
	ObserveMode();
	virtual ~ObserveMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	Scene::Camera const *camera = nullptr;
	float camera_radius = 10.0f;
	float camera_azimuth = glm::radians(60.0f);
	float camera_elevation = glm::radians(45.0f);

	/* ----- Controls ----- */
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool mouse_captured = true;

};

