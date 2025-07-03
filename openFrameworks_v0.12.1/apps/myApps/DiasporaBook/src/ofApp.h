#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

// Forward declarations for OCR
class TesseractOCR;

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

private:
	// Camera and video capture
	ofVideoGrabber camera;
	ofImage currentFrame;
	ofImage processedFrame;
	
	// Video projection
	ofVideoPlayer diasporaVideo;
	bool projectionActive;
	bool videoLoaded;
	string videoPath;
	
	// OCR and text detection
	TesseractOCR* ocrEngine;
	bool processingFrame;
	float lastDetectionTime;
	float detectionCooldown;
	vector<string> targetKeywords;
	
	// Image processing
	cv::Mat grayMat;
	cv::Mat processedMat;
	cv::Mat thresholdMat;
	ofImage debugImage;
	
	// Text regions detection
	vector<cv::Rect> textRegions;
	
	// Display settings
	int cameraWidth, cameraHeight;
	int projectionWidth, projectionHeight;
	bool showDebugInfo;
	bool showProcessedImage;
	
	// Threading for OCR
	ofThreadChannel<ofImage> frameChannel;
	ofThreadChannel<string> resultChannel;
	bool ocrThreadRunning;
	
	// GUI
	ofxPanel gui;
	ofxFloatSlider scaleFactor;
	ofxIntSlider adaptiveThreshBlockSize;
	ofxFloatSlider adaptiveThreshC;
	ofxFloatSlider claheClipLimit;
	ofxToggle enableOCR;
	
	// Fallback projection content
	vector<string> fallbackTexts;
	float fallbackStartTime;
	float lineDelay;
	float charDelay;
	ofTrueTypeFont projectionFont;
	
	// Methods
	void setupCamera();
	void setupVideo();
	void setupOCR();
	void setupFallbackContent();
	void setupGUI();
	
	void processFrameForOCR(ofImage& frame);
	void detectTextRegions(ofImage& frame);
	void performOCR();
	bool checkForKeywords(const string& text);
	void triggerProjection();
	
	void drawCameraFeed();
	void drawProjection();
	void drawFallbackProjection();
	void drawDebugInfo();
	void drawTextRegions();
	
	void startOCRThread();
	void stopOCRThread();
	void ocrThreadFunction();
	
	// Utility functions
	string preprocessText(const string& text);
	vector<string> tokenizeText(const string& text);
	ofImage matToOfImage(const cv::Mat& mat);
	cv::Mat ofImageToMat(const ofImage& img);
};

// Separate OCR class for better organization
class TesseractOCR {
public:
	TesseractOCR();
	~TesseractOCR();
	
	bool initialize();
	string recognizeText(const cv::Mat& image);
	void cleanup();
	
private:
	void* tesseractAPI; // Will hold TessBaseAPI*
	bool initialized;
	string configString;
};
