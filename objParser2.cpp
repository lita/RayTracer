/*
 *  objParser.cpp
 *  RayTracer
 *
 *  Created by Lita Cho on 10/5/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "objParser.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

ObjParser::ObjParser(string filename) {
    char line[1024]
    ifstream inputFile (filename.c_str(), ifstream::in);
    if (!inputFile) {
        cout << "Could not open OBJ file\n";
        exit(1);
    }
    while (!inputFile.eof()) {
        inputFile.getline(line, 1023);
        if (!parseLine(line)) {
            cout << "Error with OBJ file\n";
            exit(1);
        }
    }
}

bool ObjParser::parseLine(string line) {
    string op;
    if (line.empty())
        return true;
    stringstream ss(stringstream:in | stringstream::out);
}
