#pragma once

#include "ofMain.h"

class ofApp: public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();;
    void keyPressed(int key);
    
    ofFloatPixels depthPixels;
    ofPixels rgbPixels;
    ofTexture rgbTexture, depthTexture;
    
    ofMesh mesh, meshCopy;
    ofEasyCam camera;

    std::size_t meshRows, meshCols;
    float meshWidth, meshHeight;
 
    std::vector<glm::vec3> allCoordinates;
    std::vector<float> xValues, yValues, zValues;
    std::vector<int> grayPixels, rChannel, gChannel, bChannel;
    
    //mesh manipulation stuff
    vector<glm::vec3> offsets;
    vector<float> distances;
    
//    float startOrbitTime;
//    vector<float> angles;
//    glm::vec3 meshCentroid;
    
};
