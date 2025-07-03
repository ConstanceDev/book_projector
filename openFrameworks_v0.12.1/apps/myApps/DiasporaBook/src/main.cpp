#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	// Setup window size for your project
	ofSetupOpenGL(1024, 768, OF_WINDOW);
	
	// Create and run the application
	ofRunApp(new ofApp());
	
	return 0;
}
