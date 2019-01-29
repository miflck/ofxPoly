//
//  ofxLines.cpp
//  example
//
//  Created by Lukasz Karluk on 14/09/2015.
//
//

#include "ofxPoly.h"

//--------------------------------------------------------------
void ofxPolyGrow(ofPolyline & poly, const ofPolyline & polySource, float amount) {
    
    poly.clear();
    poly.setClosed(polySource.isClosed());
    
    if(polySource.size() < 2) {
        poly = polySource;
        return;
    }
    
    const vector<glm::vec3> & points = polySource.getVertices();
    int numOfPoints = points.size();

    bool bClosed = true;
    bClosed = bClosed && (polySource.isClosed() == true);
    bClosed = bClosed && (numOfPoints >= 3);
    
    for(int i=0; i<numOfPoints; i++) {
        
        bool bEndings = false;
        bEndings = bEndings || (i == 0);
        bEndings = bEndings || (i == numOfPoints-1);
        bEndings = bEndings && (bClosed == false);
        
        if(bEndings == true) {

            const  glm::vec3 & p0 = points[i];
             glm::vec3 n0 = polySource.getNormalAtIndex(i);
             glm::vec3 point = p0 + (n0 * amount);
            poly.addVertex(point);

            continue;
        }
        
        int i0 = i-1;
        if(i0 < 0) {
            i0 += numOfPoints;
        }
        
        const  glm::vec3 & p0 = points[i0];
        const  glm::vec3 & p1 = points[i];
        glm::vec3 p = p0-p1;
        //glm::vec3 n0 =  glm::vec3(p0 - p1).getPerpendicular();
        glm::vec3 n0 = glm::perp( p0, -p1 );
         glm::vec3 n1 = polySource.getNormalAtIndex(i);
        
      //  float angle =  glm::vec3(n0).angle( glm::vec3(n1));
        float angle =  glm::angle(n0,n1);

        float length = amount / cos(angle * DEG_TO_RAD);
        
         glm::vec3 point = p1 + (n1 * length);
        poly.addVertex(point);
    }
}

//--------------------------------------------------------------
void ofxPolyGrowAlongNormals(ofPolyline & poly, const ofPolyline & polySource, float normalLength) {
    vector<float> thicknesses;
    thicknesses.insert(thicknesses.begin(), polySource.size(), normalLength);
    ofxPolyGrowAlongNormals(poly, polySource, thicknesses);
}

void ofxPolyGrowAlongNormals(ofPolyline & poly, const ofPolyline & polySource, const vector<float> & normalLengths) {

    poly = polySource;

    if(poly.size() < 2) {
        return;
    }
    
    vector< glm::vec3> & points = poly.getVertices();
    int numOfPoints = points.size();
    
    for(int i=0; i<numOfPoints; i++) {
        
        float normalLength = 0.0;
        if(i < normalLengths.size()) {
            normalLength = normalLengths[i];
        }
        
         glm::vec3 & point = points[i];
         glm::vec3 normal = poly.getNormalAtIndex(i);
        point += (normal * normalLength);
    }
}

//--------------------------------------------------------------
void ofxPolyToMesh(ofMesh & mesh, const ofPolyline & polySource, float normalLength) {
    
    float normalLength0 = -normalLength;
    float normalLength1 = normalLength;
    
    ofPolyline poly0, poly1;
    ofxPolyGrowAlongNormals(poly0, polySource, normalLength0);
    ofxPolyGrowAlongNormals(poly1, polySource, normalLength1);
    
    ofxPolyToMesh(mesh, poly0, poly1);
}

void ofxPolyToMesh(ofMesh & mesh, const ofPolyline & polySource, const vector<float> & normalLengths) {

    vector<float> normalLengths0;
    vector<float> normalLengths1;
    
    for(int i=0; i<normalLengths.size(); i++) {
        float normalLength = normalLengths[i];
        float normalLength0 = -normalLength;
        float normalLength1 = normalLength;
        
        normalLengths0.push_back(normalLength0);
        normalLengths1.push_back(normalLength1);
    }
    
    ofPolyline poly0, poly1;
    ofxPolyGrowAlongNormals(poly0, polySource, normalLengths0);
    ofxPolyGrowAlongNormals(poly1, polySource, normalLengths1);
    
    ofxPolyToMesh(mesh, poly0, poly1);
}

void ofxPolyToMesh(ofMesh & mesh, const ofPolyline & poly0, const ofPolyline & poly1) {
    
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    int numOfPoints = MIN(poly0.size(), poly1.size());
    int numOfCycles = numOfPoints;
    if(poly0.isClosed() == true) {
        numOfCycles += 1;
    }
    
    for(int i=0; i<numOfCycles; i++) {
        int j = i % numOfPoints;
        const  glm::vec3 & p0 = poly0.getVertices()[j];
        const  glm::vec3 & p1 = poly1.getVertices()[j];
        
        mesh.addVertex(p0);
        mesh.addVertex(p1);
    }
}

//--------------------------------------------------------------
void ofxPolyDrawNormals(const ofPolyline & poly, float normalLength) {

    const vector< glm::vec3> & points = poly.getVertices();
    
    for(int i=0; i<points.size(); i++) {
        const  glm::vec3 & point = points[i];
         glm::vec3 normal = poly.getNormalAtIndex(i);

        ofDrawLine(point, point + (normal * normalLength));
    }
}

//--------------------------------------------------------------
void ofxPolySave(const ofPolyline & poly, string xmlPath) {
/*    ofXml xml;
    xml.addChild("poly");
    xml.setAttribute("closed", ofToString(poly.isClosed()));
    for(int i=0; i<poly.size(); i++) {
        const ofPoint & point = poly.getVertices()[i];
        
        xml.addChild("point");
        xml.setToChild(i);
        xml.setAttribute("x", ofToString(point.x));
        xml.setAttribute("y", ofToString(point.y));
        xml.setToParent();
    }
    
    xml.save(xmlPath);*/
}

//--------------------------------------------------------------
void ofxPolyLoad(ofPolyline & poly, string xmlPath) {
   /* ofXml xml;
    bool bLoaded = xml.load(xmlPath);
    if(bLoaded == false) {
        return;
    }
    
    xml.setTo("poly");
    bool bClosed = ofToInt(xml.getAttribute("closed"));
    
    poly.clear();
    
    int numOfPoints = xml.getNumChildren();
    for(int i=0; i<numOfPoints; i++) {
        xml.setToChild(i);
        float x = ofToFloat(xml.getAttribute("x"));
        float y = ofToFloat(xml.getAttribute("y"));
        
        poly.addVertex(x, y);
    }
    
    poly.setClosed(bClosed);*/
}

