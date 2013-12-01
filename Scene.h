#ifndef SCENEH
#define SCENEH

#include "Camera.h"
#include "Lights.h"
#include "RenderSettings.h"
#include "Primitives.h"
#include <string>
#include <vector>

using namespace std;

/* Represents a scene for the raytracer to render. */
class Scene {

private:

	/* Instance vars */
	Camera* sceneCam;					// Camera for this scene.
	vector<Light*> sceneLights;			// Lights for this scene.
	BoundingBoxTree* hierarchy;			// Object hierarchy for this scene.

public:

	/* Constructors */
	Scene(Camera* cam);
	~Scene();
	/* Instance methods */
	void render(const RenderSettings& settings);
	void addLight(Light* light);
	void setHierarchy(BoundingBoxTree* tree);
	vector<Light*> getLights();
	BoundingBoxTree* getHierarchy();

};


#endif
