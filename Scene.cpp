#include "Scene.h"
#include "Sampler.h"
#include "Film.h"
#include "RayTracer.h"
#include "rgb.h"
#include <iostream>

using namespace std;


/* Constructors */

Scene::Scene(Camera* cam) {

	sceneCam = cam;

}

Scene::~Scene() {

	delete sceneCam;

}


/* Instance methods */

void Scene::render(const RenderSettings& settings) {

	// [START] RENDER
	cout << "Rendering...";

	Sampler samples(settings.pixelWidth, settings.pixelHeight, settings.sqrtSamplesPerPixel);
	Film output(settings.pixelWidth, settings.pixelHeight);
	RayTracer tracer(this, settings.recursionDepth, settings.rayBias);

	while (samples.hasMoreSamples()) {
		Sample s = samples.nextSample();
		Ray viewRay = sceneCam->createViewingRay(samples.normalizeSample(s));
		rgb pixelColor = tracer.traceViewingRay(viewRay);
		output.commit(s, pixelColor);
	}

	// [END] RENDER
	cout << "DONE" << endl;

	output.writeImage(settings.filename);
}

void Scene::addLight(Light* light) {

	sceneLights.push_back(light);

}

void Scene::setHierarchy(BoundingBoxTree* tree) {

	hierarchy = tree;
}

vector<Light*> Scene::getLights() {

	return sceneLights;

}

BoundingBoxTree* Scene::getHierarchy() {

	return hierarchy;

}