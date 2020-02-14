#include "ofApp.h"


void ofApp::setup(){
    
    ofBackground(80);
    char json_prompt;
    cout << "would you like to save this json Y/N?" << endl;
    cin >> json_prompt;
    cout << json_prompt;
    //load json data from kinect image
    std::string timestamp = "2020-01-25-10-16-07-866";
    ofLoadImage(rgbPixels, timestamp + ".png");
    ofJson depthJson = ofLoadJson(timestamp + ".json");
    
    std::vector<float> data = depthJson["data"];
    std::size_t width = depthJson["width"];
    std::size_t height = depthJson["height"];
    std::size_t numChannels = 1;
    
    auto minMax = std::minmax_element(data.begin(), data.end());
    
    depthPixels.setFromPixels(data.data(), width, height, numChannels);
    
    //crop depth and rgb kinect images
    ofFloatPixels croppedDepth;
    ofPixels cropRGB;
    depthPixels.cropTo(croppedDepth, 50, 20, 400, 400);
    rgbPixels.cropTo(cropRGB, 50, 20, 400, 400);
    
    //load into textures
    rgbTexture.loadData(rgbPixels);
    depthTexture.loadData(croppedDepth);
    
    //create cols and rows based on cropped data to index through
    meshRows = 400;
    meshCols = 400;
    meshWidth = 400;
    meshHeight = 400;

    // Create the mesh.
    mesh.setMode(OF_PRIMITIVE_LINES);

    // generate vertices for mesh
    for (std::size_t row = 0; row < meshRows; row++){
        for (std::size_t col = 0; col < meshCols; col++){

            std::size_t depthX = col;
            std::size_t depthY = row;

            // 3D Positions in space.
            ofColor color = cropRGB.getColor(col, row);
            mesh.addColor(color);
            
            float x = ofMap(col, 0, meshCols, 0, meshWidth);
            float y = ofMap(row, 0, meshRows, 0, meshHeight);
            float z = ofMap(croppedDepth[row * meshCols + col], *minMax.first, *minMax.second, -600, 600);
            
            glm::vec3 position = {x, y, z};
            mesh.addVertex(position);
            
            glm::vec3 noise = {ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)};
            offsets.push_back(noise);
            
        }
    }
    
    std::size_t numCoordinates = 0;
    glm::vec3 avgPosition = { 0, 0, 0 };
    
    //connect the vertices to generate triangles for mesh
    for (std::size_t row = 0; row < meshRows; row++){
        for (std::size_t col = 0; col < meshCols; col++){
            if (col < (meshCols - 1) && row < (meshRows - 1)){
                
                ofIndexType currentIndex   = (row     ) * meshCols + (col     );
                ofIndexType eastIndex      = (row + 15) * meshCols + (col     );
                ofIndexType southEastIndex = (row + 15) * meshCols + (col + 15);
                ofIndexType southIndex     = (row     ) * meshCols + (col + 15);

                glm::vec3 currentPosition = mesh.getVertices()[currentIndex];
                glm::vec3 eastPosition = mesh.getVertices()[eastIndex];
                glm::vec3 southPosition = mesh.getVertices()[southIndex];
                glm::vec3 southEastPosition = mesh.getVertices()[southEastIndex];

                float distance1 = glm::distance(currentPosition.z, eastPosition.z);
                float distance2 = glm::distance(currentPosition.z, southPosition.z);
                float distance3 = glm::distance(currentPosition.z, eastPosition.z);
                float distance4 = glm::distance(currentPosition.z, southEastPosition.z);
                float avgDist = (distance1 + distance2 + distance3 + distance4)/4;

                if (avgDist < 10 && currentPosition.z < -300 && currentPosition.z > -440){
                
                    avgPosition += currentPosition;
                    numCoordinates++;
                        
                        // Triangle 0
                    mesh.addIndex(currentIndex);
                    mesh.addIndex(eastIndex);
                    mesh.addIndex(southEastIndex);

                        // Triangle 1
                    mesh.addIndex(currentIndex);
                    mesh.addIndex(southEastIndex);
                    mesh.addIndex(southIndex);

                        //push back values into x y and z vectors to create json
                    xValues.push_back(currentPosition.x);
                    yValues.push_back(currentPosition.y);
                    zValues.push_back(currentPosition.z);
                
                    ofColor meshColors = mesh.getColors()[currentIndex];
                        
                    int meshRed = meshColors.r;
                    int meshGreen = meshColors.g;
                    int meshBlue = meshColors.b;
                    int grayScale = (meshRed + meshGreen + meshBlue)/3;
                        
                    rChannel.push_back(meshRed);
                    gChannel.push_back(meshGreen);
                    bChannel.push_back(meshBlue);
                    grayPixels.push_back(grayScale);
                    
                }
            }
        }
    }
    
    //save mesh x y and z vertices as well as rgb color arrays to new json
    ofJson json;
    
    json["x"] = xValues;
    json["y"] = yValues;
    json["z"] = zValues;
    
    json["red"] = rChannel;
    json["green"] = gChannel;
    json["blue"] = bChannel;
    json["gray"] = grayPixels;
    
    //save json
    std::string jsonDepthImage = ofGetTimestampString();
    if (json_prompt == 'y'){
        ofSaveJson(jsonDepthImage + ".json", json);
        cout << "saved json" << endl;
    }
    //fix mesh display
    avgPosition /= double(numCoordinates);

    for (auto& vertex: mesh.getVertices()){
        vertex -= avgPosition;
    }
    
    meshCopy = mesh;
    
}


void ofApp::update(){
    
    //add perlin noise to mesh copy
    int numVerts = meshCopy.getNumVertices();
    for (int i = 0; i < numVerts; i+=3){

        glm::vec3 vert = meshCopy.getVertex(i);
        float time = ofGetElapsedTimef();
        float timeScale = 1.0;
        float displacementScale = 1.0;

        glm::vec3 timeOffsets = offsets[i];
        vert.x += (ofSignedNoise(time*timeScale+timeOffsets.x)) * displacementScale;
        vert.y += (ofSignedNoise(time*timeScale+timeOffsets.y)) * displacementScale;
        vert.z += (ofSignedNoise(time*timeScale+timeOffsets.z)) * displacementScale;

        meshCopy.setVertex(i, vert);

    }
    
}


void ofApp::draw(){
    
    camera.begin();
    ofEnableDepthTest();
    ofPushMatrix();
    ofScale(-1, -1);
    meshCopy.draw();
    ofPopMatrix();
    ofDisableDepthTest();
    camera.end();
    
    rgbTexture.draw(0, 0, rgbTexture.getWidth(), rgbTexture.getHeight());
    cout << "frame rate: " << ofGetFrameRate() << endl;
}


void ofApp::keyPressed(int key){

}

