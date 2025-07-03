#include "ofApp.h"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Diaspora Book - Interactive Projection");
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	// Initialize variables
	projectionActive = false;
	videoLoaded = false;
	processingFrame = false;
	lastDetectionTime = 0;
	detectionCooldown = 2.0; // seconds
	showDebugInfo = true;
	showProcessedImage = false;
	ocrThreadRunning = false;
	
	// Set video path
	videoPath = "diaspora_video.mp4";
	
	// Setup target keywords
	targetKeywords = {"immigrants", "immigrant", "immigration", "migrant", "migrants", "diaspora"};
	
	// Setup components
	setupCamera();
	setupVideo();
	setupOCR();
	setupFallbackContent();
	setupGUI();
	
	ofLogNotice() << "=== Diaspora Book Interactive System ===";
	ofLogNotice() << "Target keywords: immigrants, immigrant, immigration, migrant, migrants, diaspora";
	ofLogNotice() << "Video file: " << videoPath;
	ofLogNotice() << "Press 'h' for help, 'd' for debug, 't' for manual trigger";
}

//--------------------------------------------------------------
void ofApp::setupCamera() {
	cameraWidth = 1280;
	cameraHeight = 720;
	
	camera.setVerbose(true);
	camera.setDeviceID(0);
	camera.setDesiredFrameRate(30);
	camera.initGrabber(cameraWidth, cameraHeight);
	
	currentFrame.allocate(cameraWidth, cameraHeight, OF_IMAGE_COLOR);
	processedFrame.allocate(cameraWidth, cameraHeight, OF_IMAGE_GRAYSCALE);
	debugImage.allocate(cameraWidth, cameraHeight, OF_IMAGE_GRAYSCALE);
	
	ofLogNotice() << "Camera initialized: " << cameraWidth << "x" << cameraHeight;
}

//--------------------------------------------------------------
void ofApp::setupVideo() {
	projectionWidth = 800;
	projectionHeight = 600;
	
	// Try to load video file
	videoLoaded = diasporaVideo.load(videoPath);
	
	if (videoLoaded) {
		diasporaVideo.setLoopState(OF_LOOP_NORMAL);
		diasporaVideo.setVolume(1.0);
		ofLogNotice() << "Video loaded successfully: " << videoPath;
		ofLogNotice() << "Video duration: " << diasporaVideo.getDuration() << " seconds";
	} else {
		ofLogWarning() << "Could not load video: " << videoPath;
		ofLogNotice() << "Will use fallback animated content";
	}
}

//--------------------------------------------------------------
void ofApp::setupOCR() {
	ocrEngine = new TesseractOCR();
	if (!ocrEngine->initialize()) {
		ofLogError() << "Failed to initialize OCR engine";
	} else {
		ofLogNotice() << "OCR engine initialized successfully";
	}
}

//--------------------------------------------------------------
void ofApp::setupFallbackContent() {
	fallbackTexts = {
		"The hidden weight of waiting...",
		"Years pass, applications pending,",
		"Dreams deferred, hopes suspended.",
		"Each rejection letter carries",
		"the weight of a thousand tomorrows",
		"that may never come.",
		"",
		"We smile in interviews,",
		"speak of integration,",
		"while our hearts ache",
		"for certainty, for home,",
		"for the right to simply... be."
	};
	
	lineDelay = 1.5; // seconds between lines
	charDelay = 0.05; // seconds between characters
	
	// Load font for projection
	projectionFont.load("fonts/arial.ttf", 24);
	if (!projectionFont.isLoaded()) {
		ofLogWarning() << "Could not load projection font, using default";
	}
}

//--------------------------------------------------------------
void ofApp::setupGUI() {
	gui.setup("OCR Controls");
	gui.add(scaleFactor.setup("Scale Factor", 2.0, 1.0, 4.0));
	gui.add(adaptiveThreshBlockSize.setup("Thresh Block Size", 21, 3, 51));
	gui.add(adaptiveThreshC.setup("Thresh C", 10, 2, 20));
	gui.add(claheClipLimit.setup("CLAHE Clip Limit", 2.0, 1.0, 8.0));
	gui.add(enableOCR.setup("Enable OCR", true));
}

//--------------------------------------------------------------
void ofApp::update() {
	camera.update();
	
	if (camera.isFrameNew()) {
		currentFrame.setFromPixels(camera.getPixels());
		
		// Perform OCR processing every 30 frames (similar to Python version)
		if (ofGetFrameNum() % 30 == 0 && enableOCR && !processingFrame) {
			performOCR();
		}
		
		// Always detect text regions for visual feedback
		detectTextRegions(currentFrame);
	}
	
	// Check for OCR results
	string ocrResult;
	if (resultChannel.tryReceive(ocrResult)) {
		processingFrame = false;
		
		if (checkForKeywords(ocrResult)) {
			ofLogNotice() << "The keyword is captured";
			triggerProjection();
		}
	}
	
	// Update video if playing
	if (projectionActive && videoLoaded) {
		diasporaVideo.update();
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(30);
	
	if (projectionActive) {
		drawProjection();
	} else {
		drawCameraFeed();
	}
	
	if (showDebugInfo) {
		drawDebugInfo();
	}
	
	// Draw GUI
	if (showDebugInfo) {
		gui.draw();
	}
}

//--------------------------------------------------------------
void ofApp::drawCameraFeed() {
	// Draw camera feed
	ofSetColor(255);
	currentFrame.draw(0, 0, ofGetWidth() * 0.6, ofGetHeight() * 0.6);
	
	// Draw processed image if enabled
	if (showProcessedImage && processedFrame.getWidth() > 0) {
		processedFrame.draw(ofGetWidth() * 0.65, 0, ofGetWidth() * 0.35, ofGetHeight() * 0.35);
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("Processed Frame", ofGetWidth() * 0.65, ofGetHeight() * 0.35 + 20);
	}
	
	// Draw text regions
	drawTextRegions();
	
	// Status overlay
	ofSetColor(0, 255, 0);
	ofDrawBitmapString("Searching for keywords: immigrants, immigrant, immigration, migrant, migrants, diaspora", 10, 30);
	
	if (processingFrame) {
		ofSetColor(255, 255, 0);
		ofDrawBitmapString("PROCESSING OCR...", 10, 50);
	}
}

//--------------------------------------------------------------
void ofApp::drawProjection() {
	if (videoLoaded && diasporaVideo.isLoaded()) {
		// Draw video projection
		ofSetColor(255);
		diasporaVideo.draw(0, 0, ofGetWidth(), ofGetHeight());
		
		// Video controls overlay
		ofSetColor(255, 255, 255, 200);
		string videoInfo = "Video: " + ofToString(diasporaVideo.getPosition() * diasporaVideo.getDuration(), 1) +
						  "s / " + ofToString(diasporaVideo.getDuration(), 1) + "s";
		ofDrawBitmapString(videoInfo, 10, ofGetHeight() - 20);
		
		// Check if video ended
		if (diasporaVideo.getIsMovieDone()) {
			projectionActive = false;
			ofLogNotice() << "Video projection ended";
		}
	} else {
		// Draw fallback animated content
		drawFallbackProjection();
	}
}

//--------------------------------------------------------------
void ofApp::drawFallbackProjection() {
	ofBackground(245, 245, 220); // Cream background
	
	float currentTime = ofGetElapsedTimef() - fallbackStartTime;
	
	ofSetColor(0); // Black text
	
	float yOffset = 100;
	for (int i = 0; i < fallbackTexts.size(); i++) {
		float lineStartTime = i * lineDelay;
		
		if (currentTime >= lineStartTime) {
			// Calculate characters to show (typewriter effect)
			int charsToShow = (int)((currentTime - lineStartTime) / charDelay);
			string displayText = fallbackTexts[i].substr(0, charsToShow);
			
			if (!displayText.empty()) {
				if (projectionFont.isLoaded()) {
					projectionFont.drawString(displayText, 50, yOffset);
				} else {
					ofDrawBitmapString(displayText, 50, yOffset);
				}
			}
		}
		
		yOffset += 40;
	}
	
	// Auto-close after content is displayed
	if (currentTime > fallbackTexts.size() * lineDelay + 5) {
		projectionActive = false;
		ofLogNotice() << "Fallback projection ended";
	}
}

//--------------------------------------------------------------
void ofApp::drawTextRegions() {
	// Draw detected text regions
	ofSetColor(255, 0, 0, 100);
	ofNoFill();
	ofSetLineWidth(2);
	
	float scaleX = (ofGetWidth() * 0.6) / cameraWidth;
	float scaleY = (ofGetHeight() * 0.6) / cameraHeight;
	
	for (const auto& rect : textRegions) {
		ofDrawRectangle(rect.x * scaleX, rect.y * scaleY,
					   rect.width * scaleX, rect.height * scaleY);
	}
	
	ofFill();
}

//--------------------------------------------------------------
void ofApp::drawDebugInfo() {
	// Debug information overlay
	ofSetColor(255, 255, 0);
	int yPos = ofGetHeight() - 120;
	
	ofDrawBitmapString("=== Debug Info ===", 10, yPos);
	yPos += 15;
	ofDrawBitmapString("Frame: " + ofToString(ofGetFrameNum()), 10, yPos);
	yPos += 15;
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate(), 1), 10, yPos);
	yPos += 15;
	ofDrawBitmapString("Text Regions: " + ofToString(textRegions.size()), 10, yPos);
	yPos += 15;
	ofDrawBitmapString("OCR Processing: " + string(processingFrame ? "YES" : "NO"), 10, yPos);
	yPos += 15;
	ofDrawBitmapString("Video Loaded: " + string(videoLoaded ? "YES" : "NO"), 10, yPos);
	yPos += 15;
	
	ofDrawBitmapString("Controls: 'h'=help, 'd'=debug, 't'=trigger, 'p'=processed image, 'q'=quit", 10, yPos);
}

//--------------------------------------------------------------
void ofApp::detectTextRegions(ofImage& frame) {
	// Convert to OpenCV format
	cv::Mat mat = ofxCv::toCv(frame);
	cv::Mat gray;
	cv::cvtColor(mat, gray, cv::COLOR_RGB2GRAY);
	
	// Use MSER for text region detection
	cv::Ptr<cv::MSER> mser = cv::MSER::create();
	vector<vector<cv::Point>> regions;
	vector<cv::Rect> bboxes;
	
	mser->detectRegions(gray, regions, bboxes);
	
	// Filter regions by size (likely to contain text)
	textRegions.clear();
	for (const auto& bbox : bboxes) {
		if (bbox.width > 20 && bbox.width < 300 &&
			bbox.height > 10 && bbox.height < 100) {
			textRegions.push_back(bbox);
		}
	}
}

//--------------------------------------------------------------
void ofApp::processFrameForOCR(ofImage& frame) {
	// Convert to OpenCV format
	cv::Mat mat = ofxCv::toCv(frame);
	cv::cvtColor(mat, grayMat, cv::COLOR_RGB2GRAY);
	
	// Resize for better OCR
	cv::resize(grayMat, processedMat, cv::Size(), scaleFactor, scaleFactor, cv::INTER_CUBIC);
	
	// Apply bilateral filter
	cv::Mat filtered;
	cv::bilateralFilter(processedMat, filtered, 9, 75, 75);
	
	// Apply CLAHE
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(claheClipLimit, cv::Size(8, 8));
	cv::Mat enhanced;
	clahe->apply(filtered, enhanced);
	
	// Apply adaptive thresholding
	cv::adaptiveThreshold(enhanced, thresholdMat, 255,
						 cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY,
						 adaptiveThreshBlockSize, adaptiveThreshC);
	
	// Morphological operations
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::morphologyEx(thresholdMat, thresholdMat, cv::MORPH_CLOSE, kernel);
	cv::morphologyEx(thresholdMat, thresholdMat, cv::MORPH_OPEN, kernel);
	
	// Convert back to ofImage for display
	processedFrame = matToOfImage(thresholdMat);
}

//--------------------------------------------------------------
void ofApp::performOCR() {
	if (processingFrame || !ocrEngine) return;
	
	processingFrame = true;
	
	// Process frame for OCR
	processFrameForOCR(currentFrame);
	
	// Send frame to OCR thread
	frameChannel.send(processedFrame);
	
	if (!ocrThreadRunning) {
		startOCRThread();
	}
}

//--------------------------------------------------------------
void ofApp::startOCRThread() {
	ocrThreadRunning = true;
	thread ocrThread(&ofApp::ocrThreadFunction, this);
	ocrThread.detach();
}

//--------------------------------------------------------------
void ofApp::ocrThreadFunction() {
	ofImage frameToProcess;
	
	while (ocrThreadRunning && frameChannel.tryReceive(frameToProcess)) {
		if (frameToProcess.getWidth() > 0) {
			// Convert to OpenCV Mat
			cv::Mat mat = ofxCv::toCv(frameToProcess);
			
			// Perform OCR
			string result = ocrEngine->recognizeText(mat);
			
			// Send result back
			resultChannel.send(result);
		}
		
		ofSleepMillis(100); // Small delay
	}
	
	ocrThreadRunning = false;
}

//--------------------------------------------------------------
bool ofApp::checkForKeywords(const string& text) {
	string cleanText = preprocessText(text);
	
	for (const string& keyword : targetKeywords) {
		if (cleanText.find(keyword) != string::npos) {
			float currentTime = ofGetElapsedTimef();
			if (currentTime - lastDetectionTime > detectionCooldown) {
				lastDetectionTime = currentTime;
				return true;
			}
		}
	}
	
	return false;
}

//--------------------------------------------------------------
string ofApp::preprocessText(const string& text) {
	string result = ofToLower(text);
	
	// Remove extra whitespace
	result = ofStringReplace(result, "  ", " ");
	result = ofStringReplace(result, "\n", " ");
	result = ofStringReplace(result, "\t", " ");
	
	return result;
}

//--------------------------------------------------------------
void ofApp::triggerProjection() {
	if (!projectionActive) {
		projectionActive = true;
		fallbackStartTime = ofGetElapsedTimef();
		
		if (videoLoaded) {
			diasporaVideo.setPosition(0);
			diasporaVideo.play();
			ofLogNotice() << "🎬 Playing video projection...";
		} else {
			ofLogNotice() << "🎭 Playing fallback animated content...";
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
		case 'h':
			ofLogNotice() << "=== HELP ===";
			ofLogNotice() << "'h' - Show help";
			ofLogNotice() << "'d' - Toggle debug info";
			ofLogNotice() << "'t' - Manual trigger projection";
			ofLogNotice() << "'p' - Toggle processed image view";
			ofLogNotice() << "'s' - Save current frame";
			ofLogNotice() << "'q' - Quit application";
			break;
			
		case 'd':
			showDebugInfo = !showDebugInfo;
			ofLogNotice() << "Debug info: " << (showDebugInfo ? "ON" : "OFF");
			break;
			
		case 't':
			ofLogNotice() << "Manual trigger activated";
			triggerProjection();
			break;
			
		case 'p':
			showProcessedImage = !showProcessedImage;
			ofLogNotice() << "Processed image view: " << (showProcessedImage ? "ON" : "OFF");
			break;
			
		case 's':
			{
				string filename = "debug_frame_" + ofToString(ofGetUnixTime()) + ".png";
				currentFrame.save(filename);
				ofLogNotice() << "Saved frame: " << filename;
			}
			break;
			
		case 'q':
			ofExit();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	stopOCRThread();
	
	if (ocrEngine) {
		ocrEngine->cleanup();
		delete ocrEngine;
	}
	
	camera.close();
	
	ofLogNotice() << "Application shutdown complete";
}

//--------------------------------------------------------------
void ofApp::stopOCRThread() {
	ocrThreadRunning = false;
}

//--------------------------------------------------------------
// Utility functions
//--------------------------------------------------------------
ofImage ofApp::matToOfImage(const cv::Mat& mat) {
	ofImage img;
	if (mat.channels() == 1) {
		img.allocate(mat.cols, mat.rows, OF_IMAGE_GRAYSCALE);
	} else {
		img.allocate(mat.cols, mat.rows, OF_IMAGE_COLOR);
	}
	
	img.setFromPixels(mat.data, mat.cols, mat.rows,
					 mat.channels() == 1 ? OF_IMAGE_GRAYSCALE : OF_IMAGE_COLOR);
	return img;
}

//--------------------------------------------------------------
cv::Mat ofApp::ofImageToMat(const ofImage& img) {
	return ofxCv::toCv(img);
}

//--------------------------------------------------------------
// TesseractOCR Implementation
//--------------------------------------------------------------
TesseractOCR::TesseractOCR() : tesseractAPI(nullptr), initialized(false) {
	configString = "-c tessedit_char_whitelist=ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz --psm 6";
}

//--------------------------------------------------------------
TesseractOCR::~TesseractOCR() {
	cleanup();
}

//--------------------------------------------------------------
bool TesseractOCR::initialize() {
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	
	if (api->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY)) {
		ofLogError() << "Could not initialize Tesseract";
		delete api;
		return false;
	}
	
	api->SetPageSegMode(tesseract::PSM_AUTO);
	tesseractAPI = static_cast<void*>(api);
	initialized = true;
	
	return true;
}

//--------------------------------------------------------------
string TesseractOCR::recognizeText(const cv::Mat& image) {
	if (!initialized || !tesseractAPI) {
		return "";
	}
	
	tesseract::TessBaseAPI* api = static_cast<tesseract::TessBaseAPI*>(tesseractAPI);
	
	api->SetImage(image.data, image.cols, image.rows, image.channels(), image.step);
	
	char* result = api->GetUTF8Text();
	string textResult = "";
	
	if (result) {
		textResult = string(result);
		delete[] result;
	}
	
	return textResult;
}

//--------------------------------------------------------------
void TesseractOCR::cleanup() {
	if (tesseractAPI) {
		tesseract::TessBaseAPI* api = static_cast<tesseract::TessBaseAPI*>(tesseractAPI);
		api->End();
		delete api;
		tesseractAPI = nullptr;
	}
	initialized = false;
}

//--------------------------------------------------------------
// Additional required methods for ofBaseApp
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}
void ofApp::mouseMoved(int x, int y) {}
void ofApp::mouseDragged(int x, int y, int button) {}
void ofApp::mousePressed(int x, int y, int button) {}
void ofApp::mouseReleased(int x, int y, int button) {}
void ofApp::windowResized(int w, int h) {}
void ofApp::dragEvent(ofDragInfo dragInfo) {}
void ofApp::gotMessage(ofMessage msg) {}
