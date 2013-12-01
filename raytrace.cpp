//////////////////////////////////////////////////////////////////////////////
//                        PREPROCESSOR DIRECTIVES                           //
//////////////////////////////////////////////////////////////////////////////

#include "Scene.h"
#include "Camera.h"
#include "RenderSettings.h"
#include "Primitives.h"
#include "Material.h"
#include "Shapes.h"
#include "Lights.h"
#include "ObjParser.h"
#include "algebra3.h"
#include "objParser.h"
#include <iostream>


//For file input
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
using namespace std;

//////////////////////////////////////////////////////////////////////////////
//                            ENUMS FOR LIGHTS                              //
//////////////////////////////////////////////////////////////////////////////

enum LightType {
    pointLight,
    directionalLight,
	areaLight
};

//////////////////////////////////////////////////////////////////////////////
//                               PARSER                                     //
//////////////////////////////////////////////////////////////////////////////

bool parseScene(ifstream &inputFile, Camera *&cam, RenderSettings &settings) {
    // Parsing the main scene. Currently, you must give all the proper
    // camera coordinates as well as the pixel length and with.
    //      camera eyeX eyeY eyeZ llx lly llz lrx lry lrz urx ury urz ulx uly ulz;
    //      pixel 100 100
    //      depth 1
    //      bias .005
    //      name testImage
    bool camera = false, pixel = false, sample = false, bias = false, depth = false, name = false;
    char line[1024];
    
    for (int i = 0; i < 6; i++) {
        string op;
        stringstream ss (stringstream::in | stringstream::out);
        inputFile.getline(line, 1023);
        ss.str(line);
        ss >> op;
        if (op.compare("pinhole") == 0) {
            double eyeX, eyeY, eyeZ, llx, lly, llz, lrx, lry, lrz, 
                  urx, ury, urz, ulx, uly, ulz; 
            if (! (ss >> eyeX >> eyeY >> eyeZ >> 
                         ulx >> uly >> ulz >>
                         llx >> lly >> llz >> 
                         urx >> ury >> urz >> 
                         lrx >> lry >> lrz)) {
                return false;
            }
            vec3 eye (eyeX,eyeY,eyeZ);
            vec3 UL (ulx,uly,ulz);
            vec3 LL (llx,lly,llz);
            vec3 UR (urx,ury,urz);
            vec3 LR (lrx,lry,lrz);
            cam = new PinholeCamera(eye, UL, LL, UR, LR);
            camera = true;
            continue;
        }
        
        if (op.compare("lens") == 0 && sample) {
            double pointx, pointy, pointz;
            double edge1x, edge1y, edge1z;
            double edge2x, edge2y, edge2z;
            double llx, lly, llz;
            double lrx, lry, lrz;
            double urx, ury, urz;
            double ulx, uly, ulz;
            if (! (ss >> pointx >> pointy >> pointz >>
                         edge1x >> edge1y >> edge1z >>
                         edge2x >> edge2y >> edge2z >>
                         ulx >> uly >> ulz >>
                         llx >> lly >> llz >> 
                         urx >> ury >> urz >> 
                         lrx >> lry >> lrz )) {
                return false;
            }
            vec3 point(pointx, pointy, pointz);
            vec3 edge1(edge1x, edge1y, edge1z);
            vec3 edge2(edge2x, edge2y, edge2z);
            vec3 UL (ulx,uly,ulz);
            vec3 LL (llx,lly,llz);
            vec3 UR (urx,ury,urz);
            vec3 LR (lrx,lry,lrz);
            cam = new LensCamera(point, edge1, edge2, UL, LL, UR, LR, settings.sqrtSamplesPerPixel);
            camera = true;
            continue;
        }
        
        if (op.compare("pixel") == 0) {
            unsigned int x, y;
            if (! (ss >> x >> y))
                return false;
            settings.pixelWidth = x;                                                                
            settings.pixelHeight = y;   
            pixel = true;
            continue;
        }
        
        if (op.compare("sample") == 0) {
            unsigned int sampleRate;
            if (! (ss >> sampleRate))
                return false;
            settings.sqrtSamplesPerPixel = sampleRate;
            sample = true;
            continue;
        }
        
        if (op.compare("depth") == 0) {
            int i;
            if (! (ss >> i))
                return false;
            settings.recursionDepth = i;
            depth = true;
            continue;
        }

        if (op.compare("bias") == 0) {
            double i;
            if (! (ss >> i))
                return false;
            settings.rayBias = i;
            bias = true;
            continue;
        }
        
        if (op.compare("name") == 0) {
            string filename;
            if (! (ss >> filename))
                return false;
            settings.filename = filename;
            name = true;
            continue;
        }
    }
    
    if (camera && pixel && sample && bias && depth && name) 
        return true;
    
    return false;
}

// Must give all the reflective matieral at the moment.
// Each material must be called in a seperate line.
bool parseMaterial (ifstream &inputFile, map<string,Texture*>& textures, Material*& mat) {
    bool ka = false, kd = false, ks = false, kr = false, kt = false, index = false, 
         sp = false, tex = false, isTex = false, rough = false;
	Reflectance refl;
	Texture* texture;
    for (int i = 0; i < 8; i++) {
        stringstream ss (stringstream::in | stringstream::out);
        string op;
        char line[1024];
        inputFile.getline(line, 1023);
        ss.str(line);
        ss >> op;
        double r, g, b, exp;
        
        if (op.compare("ka")==0) {
            if (! (ss >> r >> g >> b))
                return false;;
            refl.kA = rgb(r,g,b);
            ka = true;
            continue;
        }
        
        if (op.compare("kd")==0) {
            if (! (ss >> r >> g >> b))
                return false;
            refl.kD = rgb(r,g,b);
            kd = true;
            continue;
        }
        
        if (op.compare("ks")==0) {
            if (! (ss >> r >> g >> b))
                return false;
            refl.kS = rgb(r,g,b);
            ks = true;
            continue;
        }
        
        if (op.compare("kr")==0) {
            if (! (ss >> r >> g >> b))
                return false;
            refl.kR = rgb(r,g,b);
            kr = true;
            continue;
        }
        
        if (op.compare("sp")==0) {
            if (! (ss >> exp))
                return false;
            refl.pExp = exp;
            sp = true;
            continue;
        }
        
        if (op.compare("kt")==0) {
            if (!(ss >> r >> g >> b))
                return false;
            refl.kT = rgb(r,g,b);
            kt = true;
            continue;
        }
        
        if (op.compare("index")==0) {
            if (!(ss >> exp))
                return false;
            refl.indexOfRefraction = exp;
            index = true;
            continue;
        }

		if (op.compare("Texture:")==0) {
			tex = true;
			string texstring;
			ss >> texstring;
			if (texstring.compare("NONE") != 0) {
				isTex = true;
				texture = textures[texstring];
				ss >> texstring;
				if (texstring.compare("rough") == 0)
					rough = true;
				else if (texstring.compare("smooth") == 0)
					rough = false;
				else return false;
			}
			continue;
		}
    }
    
	if (ka && kd && ks && kr && sp && tex && index && kt) {
		if (isTex)
			mat = new TexturedMaterial(refl, texture, rough);
		else mat = new Material(refl);
        return true;
	}
    return false;
}

bool parseTransform(ifstream& inputFile, mat4* transform) {

	vec3 trans, scale, rot;
	bool translation = false, scaling = false, rotation = false;

	for (int i = 0; i < 3; i++) {
		stringstream ss (stringstream::in | stringstream::out);
		string op;
		char line[1024];
		inputFile.getline(line, 1023);
		ss.str(line);
		ss >> op;

		if (op.compare("scaleXYZ") == 0) {
			if (!(ss >> scale))
				return false;
			scaling = true;
			continue;
		}

		if (op.compare("translateXYZ") == 0) {
			if (!(ss >> trans))
				return false;
			translation = true;
			continue;
		}

		if (op.compare("rotateXYZ") == 0) {
			if (!(ss >> rot))
				return false;
			rotation = true;
			continue;
		}
	}

	if (!(translation && scaling && rotation))
		return false;

	//*transform = translation3D(trans) * rotation3D(vec3(0,0,1),rot[VZ]) * rotation3D(vec3(0,1,0),rot[VY])
	//	* rotation3D(vec3(1,0,0),rot[VX]) * scaling3D(scale);
	*transform = translation3D(trans) * rotation3D(vec3(1,0,0),rot[VX]) * rotation3D(vec3(0,1,0),rot[VY])
		* rotation3D(vec3(0,0,1),rot[VZ]) * scaling3D(scale);
	return true;

}

//  Must give the argments to create the shape and then set the method.
//  The coordinates for the triangle must be given all in one line.
//      a 1 2 3 b 1 2 3 c 1 2 3
bool parseTriangle(ifstream &inputFile, Shape *&shape) {
    stringstream ss (stringstream::in | stringstream::out);
    string op;
    vec3 a, b, c;
    char line[1024];
    inputFile.getline(line, 1023);
    ss.str(line);
    ss >> op;
    if (op.compare("a") == 0) {
        double x, y, z;
        if (! (ss >> x >> y >> z))
            return false;
        a = vec3(x, y, z);
    } else 
        return false;
    
    ss >> op;
    if (op.compare("b") == 0) {
        double x, y, z;
        if (! (ss >> x >> y >> z))
            return false;
        b = vec3(x, y, z);
    } else 
        return false;

    ss >> op;    
    if (op.compare("c") == 0) {
        double x, y, z;
        if (! (ss >> x >> y >> z))
            return false;
        c = vec3(x, y, z);
    } else 
        return false;

	// Transforms
	stringstream ss2 (stringstream::in | stringstream::out);
	inputFile.getline(line, 1023);
    ss2.str(line);
    ss2 >> op;
	if (op.compare("Transform:") != 0)
		return false;
	ss2 >> op;
	if (op.compare("NONE") == 0) {
		shape = new Triangle(a,b,c);
		return true;
	}
	mat4 transform;
	if (!parseTransform(inputFile, &transform))
		return false;
	shape = new TransformedShape(new Triangle(a,b,c), transform);
	return true;
}

bool parseSphere(ifstream &inputFile, Shape *&shape) {
    stringstream ss (stringstream::in | stringstream::out);
    string op;
    vec3 center;
    double radius;
    char line[1024];
    inputFile.getline(line, 1023);
    ss.str(line);
    ss >> op;
    if (op.compare("center") == 0) {
        double x, y, z;
        if ((ss >> x >> y >> z))
            center = vec3(x,y,z);
        else 
            return false;
    } else 
        return false;
        
    ss >> op;
    if (op.compare("radius") == 0) {
        if (!(ss >> radius))
            return false;
    } else
        return false;

	// Transforms
	stringstream ss2 (stringstream::in | stringstream::out);
	inputFile.getline(line, 1023);
    ss2.str(line);
    ss2 >> op;
	if (op.compare("Transform:") != 0)
		return false;
	ss2 >> op;
	if (op.compare("NONE") == 0) {
		shape = new Sphere (radius, center);
		return true;
	}
	mat4 transform;
	if (!parseTransform(inputFile, &transform))
		return false;
	shape = new TransformedShape(new Sphere(radius,center), transform);
	return true;
}

bool parseEllipsoid(ifstream &inputFile, Shape *&shape) {

	mat4 transform;
	if (parseTransform(inputFile, &transform)) {
		shape = new Ellipsoid(transform);
		return true;
	} else return false;
}

bool parseLighting(stringstream &ss, Light *&light, LightType type, const RenderSettings& settings) {
    double x, y, z, r, g, b;
    if ((ss >> x >> y >> z >> r >> g >> b)) {
        if (type == directionalLight) {
            light = new DirectionalLight (x,y,z, rgb(r,g,b));
			return true;
        } else if (type == pointLight) {
            light =  new PointLight (x,y,z, rgb(r,g,b));
			return true;
		} else {
			string op;
			ss >> op;
			if (op.compare("edges") != 0)
				return false;
			vec3 e1, e2;
			if (!(ss >> e1 >> e2))
				return false;
			light = new AreaLight(vec3(x,y,z), e1, e2, rgb(r,g,b), settings.sqrtSamplesPerPixel);
			return true;
		}
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////
//                            MAIN FUNCTION                                 //
//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {

	if (argc != 2) {
		cerr << "Usage: raytrace filename" << endl;
		exit(1);
	}

	// WELCOME MESSAGE
	cout << "Raytracer started!" << endl;

    vector<Primitive*> objects;
	map<string,Texture*> textures;
	map<string,Primitive*> meshes;
    
    RenderSettings settings;
    Scene *mainScene;
    //Reflectance refl;
	Material* mat;
    Light *light;
    Shape *shape;
    Camera *cam;
    bool sceneCreated = false;
    string filename = argv[1];

	// [START] LOAD FILE
	cout << "Loading file \"" << filename << "\"...";

    char line[1024]; //Buffer
    ifstream inputFile (filename.c_str(), ifstream::in); //Open up the stream
    if (!inputFile) {
		cout << endl;
		cerr << "Error: Count not open given file" << filename << endl;
        exit(1);
    }
    
	// [END] LOAD FILE
	cout << "DONE" << endl;
	// [START] BUILD SCENE
	cout << "Building Scene...";


	while (!inputFile.eof()) {
		string op;
		inputFile.getline (line, 1023);
		stringstream ss (stringstream::in |stringstream::out);
		ss.str(line);
		ss >> op;
		if (op.empty())
			continue;
		if (op.compare("Scene:") == 0) {
			if (!parseScene(inputFile, cam, settings)) {
				cout << endl;
				cerr << "Error: Incomplete scene description in " << filename << endl;
				exit(1);
			} else {
				mainScene = new Scene(cam);
				sceneCreated = true;
			}
		}

		if (op.compare("TEXTURE:") == 0) {
			string name, texfilename;
			ss >> name >> texfilename;
			textures[name] = new Texture(texfilename);
		}

		if (op.compare("Mesh:") == 0 && sceneCreated) {

			string meshname;
			ss >> meshname;

			inputFile.getline(line, 1023);
			stringstream ss2 (stringstream::in |stringstream::out);
			ss2.str(line);
			string transform;
			ss2 >> transform;
			if (transform.compare("Transform:") != 0) {
				cout << endl;
				cerr << "Error: OBJ transform unspecified in " << filename << endl;
				exit(1);
			}
			ss2 >> transform;
			mat4 transMat;
			if (transform.compare("NONE") == 0)
				transMat = identity3D();
			else if (!parseTransform(inputFile, &transMat)) {
				cout << endl;
				cerr << "Error: Bad OBJ transform in " << filename << endl;
				exit(1);
			}

			if (parseMaterial(inputFile, textures, mat)) {
				inputFile.getline (line, 1023);
				stringstream ss3 (stringstream::in |stringstream::out);
				ss3.str(line);

				string objfile;
				string shadeType;
				string wireframe;
				bool phongShade;
				bool wireframeOnly = false;

				if (!(ss3 >> objfile >> shadeType)) {
					cout << endl;
					cerr << "Error: OBJ shading type unspecified in " << filename << endl;
					exit(1);
				}
				if (shadeType.compare("phongShading") == 0)
					phongShade = true;
				else if (shadeType.compare("flatShading") == 0)
					phongShade = false;
				else {
					cout << endl;
					cerr << "Error: Unknown shading type in " << filename << endl;
					exit(1);
				}
				ss3 >> wireframe;
				if (wireframe.compare("wireframeOnly") == 0)
					wireframeOnly = true;

				ObjParser parser(objfile, mat, transMat, phongShade, wireframeOnly);
				vector<Primitive*> temp = parser.getObjects();
				// If we have a true mesh (MeshPrimitive), then store it in the map.
				if (temp.size() == 1 && meshname.compare("") != 0)
					meshes[meshname] = temp[0];
				for (unsigned int i = 0; i < temp.size(); i++)
					objects.push_back(temp[i]);

			} else {
				cout << endl;
				cerr << "Error: Material was bad in " << filename << endl;
				exit(1);
			}
		}

		if (op.compare("InstanceOf:") == 0) {

			string meshname;
			ss >> meshname;
			if (meshname.compare("") == 0) {
				cout << endl;
				cerr << "Error: Bad Instance name in " << filename << endl;
				exit(1);
			}

			mat4 transform;
			if (!parseTransform(inputFile, &transform)) {
				cout << endl;
				cerr << "Error: Bad Instance transform in " << filename << endl;
				exit(1);
			}
			if (!parseMaterial(inputFile, textures, mat)) {
				cout << endl;
				cerr << "Error: Bad Material in " << filename << endl;
				exit(1);
			}

			objects.push_back(meshes[meshname]->instance(transform, mat));
		}

		if (op.compare("DirectionalLight") == 0 && sceneCreated) {
			if (!parseLighting(ss, light, directionalLight, settings)) {
				cout << endl;
				cerr << "Error: Incomplete directional light description in " << filename << endl;
				exit(1);
			} else {
				mainScene->addLight(light);
			}
		}

		if (op.compare("AreaLight") == 0 && sceneCreated) {
			if (!parseLighting(ss, light, areaLight, settings)) {
				cout << endl;
				cerr << "Error: Incomplete area light description in " << filename << endl;
				exit(1);
			} else {
				mainScene->addLight(light);
			}
		}

		if (op.compare("PointLight") == 0 && sceneCreated) {
			if (!parseLighting(ss, light, pointLight, settings)) {
				cout << endl;
				cerr << "Error: Incomplete point light description in " << filename << endl;
				exit(1);
			} else {
				mainScene->addLight(light);
			}
		}

		if (op.compare("Triangle:") == 0 && sceneCreated) {
			if (parseTriangle(inputFile, shape)) {
				if (parseMaterial(inputFile, textures, mat)) {
					GeoPrimitive *object = new GeoPrimitive (shape, mat);
					objects.push_back(object);
				} else {
					cout << endl;
					cerr << "Error: Material was bad in " << filename << endl;
					exit(1);
				}
			} else {
				cout << endl;
				cerr << "Error: Incomplete triangle description in " << filename << endl;
				exit(1);
			}
		}

		if (op.compare("Sphere:") == 0 && sceneCreated) {
			if (parseSphere(inputFile, shape)) {
				if (parseMaterial(inputFile, textures, mat)) {
					GeoPrimitive *object = new GeoPrimitive (shape, mat);
					objects.push_back(object);
				} else {
					cout << endl;
					cerr << "Error: Material was bad in " << filename << endl;
					exit(1);
				}
			} else {
				cout << endl;
				cerr << "Error: Incomplete sphere description in " << filename << endl;
				exit(1);
			}
		}

		if (op.compare("Ellipsoid:") == 0 && sceneCreated) {
			if (parseEllipsoid(inputFile, shape)) {
				if (parseMaterial(inputFile, textures, mat)) {
					GeoPrimitive *object = new GeoPrimitive (shape, mat);
					objects.push_back(object);
				} else {
					cout << endl;
					cerr << "Error: Material was bad in " << filename << endl;
					exit(1);
				}
			} else {
				cout << endl;
				cerr << "Error: Incomplete ellipsoid description in " << filename << endl;
				exit(1);
			}
		} 
	}

    inputFile.close();
    BoundingBoxTree tree = BoundingBoxTree(objects, VZ);
    mainScene->setHierarchy(&tree);

	// [END] BUILD SCENE
	cout << "DONE" << endl;

    mainScene->render(settings);
}