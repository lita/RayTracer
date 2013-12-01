/*
 *  objParser.cpp
 *  RayTracer
 *
 *  Created by Lita Cho on 10/5/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "objParser.h"
#include "Shapes.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

ObjParser::ObjParser(string filename, Material* mat, mat4 transform, bool phongShade, bool wireframeOnly) {
    char line[1024];
    ifstream inputFile (filename.c_str(), ifstream::in);
    if (!inputFile) {
		cout << endl;
		cerr << "Error: Could not open OBJ file\n";
        exit(1);
    }

	mesh = new Mesh;
	this->mat = mat;
	phongShading = phongShade;
	this->wireframeOnly = wireframeOnly;
	vertsParsed = 0;
	this->transform = transform;
	if (transform == identity3D())
		transformIsIdentity = true;
	else transformIsIdentity = false;

    while (!inputFile.eof()) {
        inputFile.getline(line, 1023);
        if (!parseLine(line)) {
			cout << endl;
			cerr << "Error: Bad OBJ file\n";
            exit(1);
        }
    }
    inputFile.close();
}

bool ObjParser::parseLine(string line) {
    string op;
    if (line.empty())
        return true;
    stringstream ss(stringstream::in | stringstream::out);
    ss.str(line);
    ss >> op;

    if (op.compare("v") == 0) {
		vertsParsed++;
        vec3 vertex;
        if (ss >> vertex)
            mesh->vertices.push_back(vertex);
        else 
            return false;
    }

	if (op.compare("vn") == 0) {
		vec3 normal;
		if (ss >> normal)
			mesh->normals.push_back(normal);
		else 
			return false;
	}

	if (op.compare("vt") == 0) {
		vec2 texture;
		if (ss >> texture) {
			// Correct bad UVs...
			while (texture[0] > 1)
				texture[0]--;
			while (texture[1] > 1)
				texture[1]--;
			while (texture[0] < 0)
				texture[0]++;
			while (texture[1] < 0)
				texture[1]++;
			mesh->textures.push_back(texture);
		}
		else 
			return false;
	}
    
    if (op.compare("f") == 0) {

		// CASE: We only have vertices
		if (mesh->normals.size() == 0 || mesh->textures.size() == 0) {
			int a, b, c;
			if (ss >> a >> b >> c) {
				// Ignore degenerate triangles
				if (a == b || a == c || b == c)
					return true;
				Triangle* tri = new Triangle(mesh->vertices[a-1],mesh->vertices[b-1],mesh->vertices[c-1]);
				if (transformIsIdentity)
					triangles.push_back(tri);
				else triangles.push_back(new TransformedShape(tri, transform));

			} else return false;
		}
		// CASE: We have a full-blown .obj file
		else {

			// Grow the phongNormals list to keep up with the vertices
			if (vertsParsed > 0) {
				for (int i = 0; i < vertsParsed; i++)
					phongNormals.push_back(vec3(0,0,0));
				vertsParsed = 0;
			}

			int vertI[3];
			int normI[3];
			int texI[3];

			for (int i = 0; i < 3; i++) {
				string group;
				ss >> group;

				// Replace slashes with whitespace
				size_t slashIndex;
				slashIndex = group.find_first_of('/');
				while (slashIndex != string::npos)
				{
					group[slashIndex] = ' ';
					slashIndex = group.find_first_of('/',slashIndex+1);
				}

				// Parse data
				stringstream gs(stringstream::in | stringstream::out);
				gs.str(group);
				int v, t, n;
				if (!(gs >> v >> t >> n))
					return false;

				vertI[i] = v - 1;
				texI[i] = t - 1;
				// Flat shading
				if (!phongShading)
					normI[i] = n - 1;
				// Phong shading
				else {
					phongNormals[v-1] += mesh->normals[n-1];
					normI[i] = v - 1;
				}
			}

			// Ignore degenerate triangles
			vec3 a = mesh->vertices[vertI[0]];
			vec3 b = mesh->vertices[vertI[1]];
			vec3 c = mesh->vertices[vertI[2]];
			if (a == b || a == c || b == c)
				return true;

			MeshTriangle* tri;
			if (wireframeOnly)
				tri = new WireframeTriangle(mesh, vertI, normI, texI);
			else tri = new MeshTriangle(mesh, vertI, normI, texI);
			if (transformIsIdentity)
				triangles.push_back(tri);
			else triangles.push_back(new TransformedShape(tri, transform));
		}
    }
    return true;
}

vector<Primitive*> ObjParser::getObjects() {

	vector<Primitive*> objects;

	if (phongShading && phongNormals.size() > 0) {
		for (unsigned int i = 0; i < phongNormals.size(); i++)
			phongNormals[i].normalize();
		mesh->normals = phongNormals;
	}

	if (mesh->normals.size() == 0 || mesh->textures.size() == 0)
		for (unsigned int i = 0; i < triangles.size(); i++)
			objects.push_back(new GeoPrimitive(triangles[i], mat));
	else objects.push_back(new MeshPrimitive(mesh, triangles, mat));

	return objects;
}