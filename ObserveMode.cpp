#include "ObserveMode.hpp"
#include "DrawLines.hpp"
#include "LitColorTextureProgram.hpp"
#include "Mesh.hpp"
#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "data_path.hpp"
#include "Sound.hpp"
#include "load_save_png.hpp"
#include "CopyToScreenProgram.hpp"
#include <iostream>

GLuint meshes_for_lit_color_texture_program = 0;
static Load< MeshBuffer > meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer *ret = new MeshBuffer(data_path("map.pnct"));
	meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > scene(LoadTagLate, []() -> Scene * {
	Scene *ret = new Scene();

	ret->load(data_path("map.scene"), [](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		auto &mesh = meshes->lookup(mesh_name);
		scene.drawables.emplace_back(transform);
		Scene::Drawable::Pipeline &pipeline = scene.drawables.back().pipeline;

		pipeline = lit_color_texture_program_pipeline;
		pipeline.vao = meshes_for_lit_color_texture_program;
		//pipeline.program = copy_to_screen_program->program;
		pipeline.type = mesh.type;
		pipeline.start = mesh.start;
		pipeline.count = mesh.count;
	});

	return ret;
});

ObserveMode::ObserveMode() {

	assert(scene->cameras.size() && "Observe requires cameras.");
	SDL_SetRelativeMouseMode(SDL_TRUE);
	camera = &scene->cameras.front();
}

ObserveMode::~ObserveMode() {

}

bool ObserveMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {

		if (evt.key.keysym.sym == SDLK_w) forward = true;
		if (evt.key.keysym.sym == SDLK_a) left = true;
		if (evt.key.keysym.sym == SDLK_s) backward = true;
		if (evt.key.keysym.sym == SDLK_d) right = true;
		if (evt.key.keysym.sym == SDLK_m) mouse_captured = true;
	}

	if (evt.type == SDL_KEYUP) {

		if (evt.key.keysym.sym == SDLK_w) forward = false;
		if (evt.key.keysym.sym == SDLK_a) left = false;
		if (evt.key.keysym.sym == SDLK_s) backward = false;
		if (evt.key.keysym.sym == SDLK_d) right = false;
		if (evt.key.keysym.sym == SDLK_m) mouse_captured = false;
	}

	if (evt.type == SDL_MOUSEMOTION) {
		if (mouse_captured) {

			float yaw = evt.motion.xrel / float(window_size.x) * camera->fovy;
			float pitch = evt.motion.yrel / float(window_size.y) * camera->fovy;

			glm::quat roty = glm::angleAxis(pitch, glm::vec3(-1.0f, 0.0f, 0.0f));
			glm::quat rotx = glm::angleAxis(yaw, glm::vec3(0.0f, -1.0f, 0.0f));
			camera->transform->rotation *= rotx * roty;

		}
	}
	return false;
}


void ObserveMode::update(float elapsed) {

	
	{
		float step_up = 0.0f;
		float step_right = 0.0f;
		if (forward) { step_up += elapsed * -4.0f;}
		if (backward) step_up -= elapsed * -4.0f;
		if (right) step_right += elapsed * -2.0f;
		if (left) step_right -= elapsed * -2.0f;
		camera->transform->position.y += step_up;
		camera->transform->position.x += step_right;
	}

}

void ObserveMode::draw(glm::uvec2 const &drawable_size) {
	//--- actual drawing ---
	glClearColor(0.85f, 0.85f, 0.90f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	const_cast< Scene::Camera * >(camera)->aspect = drawable_size.x / float(drawable_size.y);

	scene->draw(*camera);
}
