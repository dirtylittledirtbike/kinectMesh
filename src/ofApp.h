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
    ofTexture rgbTexture;
    ofTexture depthTexture;
    ofMesh mesh;
    ofMesh meshCopy;
    ofEasyCam camera;

    std::size_t meshRows;
    std::size_t meshCols;
    
    float meshWidth;
    float meshHeight;
 
    std::vector<glm::vec3> allCoordinates;
    std::vector<float> xValues;
    std::vector<float> yValues;
    std::vector<float> zValues;
    std::vector<int> grayPixels;
    
    std::vector<int> rChannel;
    std::vector<int> gChannel;
    std::vector<int> bChannel;
    
    //mesh manipulation stuff
    vector<glm::vec3> offsets;
    
    float startOrbitTime;
    vector<float> distances;
    vector<float> angles;
    glm::vec3 meshCentroid;
    
};
