import cv2
import pytesseract
import pygame
import numpy as np
import time
import threading
from collections import deque

class InteractiveBookCV:
    def __init__(self):
        # Initialize camera
        self.cap = cv2.VideoCapture(0)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
        
        # Target keyword
        self.target_keyword = "immigrants"
        
        # OCR configuration
        # pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'  # Windows path
        # For Mac/Linux, Tesseract should be in PATH after brew install
        
        # Detection settings
        self.detection_confidence = 0.7
        self.keyword_detected = False
        self.last_detection_time = 0
        self.detection_cooldown = 2.0  # seconds
        
        # Frame processing
        self.frame_queue = deque(maxlen=10)
        self.processing_frame = False
        
        # Pygame for projection display
        pygame.init()
        self.projection_active = False
        
        print(f"Initialized Interactive Book CV")
        print(f"Target keyword: '{self.target_keyword}'")
        print(f"Press 'q' to quit, 's' to save current frame")
        
    def preprocess_frame_for_ocr(self, frame):
        """Preprocess frame to improve OCR accuracy"""
        # Convert to grayscale
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        
        # Resize image for better OCR (larger is often better)
        height, width = gray.shape
        scale_factor = 2.0
        resized = cv2.resize(gray, (int(width * scale_factor), int(height * scale_factor)), 
                           interpolation=cv2.INTER_CUBIC)
        
        # Apply bilateral filter to reduce noise while keeping edges sharp
        filtered = cv2.bilateralFilter(resized, 9, 75, 75)
        
        # Apply CLAHE (Contrast Limited Adaptive Histogram Equalization)
        clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8,8))
        enhanced = clahe.apply(filtered)
        
        # Apply adaptive thresholding with larger block size
        thresh = cv2.adaptiveThreshold(
            enhanced, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
            cv2.THRESH_BINARY, 21, 10
        )
        
        # Morphological operations to clean up text
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (2, 2))
        cleaned = cv2.morphologyEx(thresh, cv2.MORPH_CLOSE, kernel)
        cleaned = cv2.morphologyEx(cleaned, cv2.MORPH_OPEN, kernel)
        
        return cleaned
    
    def detect_text_regions(self, frame):
        """Detect potential text regions in the frame"""
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        
        # Use MSER (Maximally Stable Extremal Regions) for text detection
        mser = cv2.MSER_create()
        regions, _ = mser.detectRegions(gray)
        
        # Create bounding boxes for text regions
        text_regions = []
        for region in regions:
            x, y, w, h = cv2.boundingRect(region.reshape(-1, 1, 2))
            # Filter regions by size (likely to contain text)
            if 20 < w < 300 and 10 < h < 100:
                text_regions.append((x, y, w, h))
        
        return text_regions
    
    def perform_ocr(self, frame):
        """Perform OCR on the frame and look for target keyword"""
        try:
            # Preprocess frame
            processed_frame = self.preprocess_frame_for_ocr(frame)
            
            # More aggressive OCR configuration for better accuracy
            custom_config = r'--oem 3 --psm 6 -c tessedit_char_whitelist=ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz '
            
            # Try multiple OCR approaches
            ocr_results = []
            
            # Method 1: Standard OCR
            result1 = pytesseract.image_to_string(processed_frame, config=custom_config)
            ocr_results.append(result1)
            
            # Method 2: Different PSM mode (treat as single word)
            config2 = r'--oem 3 --psm 8 -c tessedit_char_whitelist=ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz '
            result2 = pytesseract.image_to_string(processed_frame, config=config2)
            ocr_results.append(result2)
            
            # Method 3: Use original frame with different preprocessing
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            result3 = pytesseract.image_to_string(gray, config=custom_config)
            ocr_results.append(result3)
            
            # Combine and clean all results
            all_text = " ".join(ocr_results).lower()
            detected_text = " ".join(all_text.split())  # Clean whitespace
            
            # More flexible keyword matching
            keywords_to_check = [
                "immigrants", "immigrant", "immigration", 
                "migrant", "migrants", "diaspora"
            ]
            
            for keyword in keywords_to_check:
                if keyword in detected_text:
                    current_time = time.time()
                    if current_time - self.last_detection_time > self.detection_cooldown:
                        self.last_detection_time = current_time
                        return True, detected_text
            
            # Only print detected text if it contains reasonable words (length > 3)
            reasonable_words = [word for word in detected_text.split() if len(word) > 3]
            if reasonable_words:
                display_text = " ".join(reasonable_words[:10])  # Show first 10 reasonable words
                return False, display_text
            
            return False, ""
            
        except Exception as e:
            print(f"OCR Error: {e}")
            return False, ""
    
    def trigger_projection(self):
        """Trigger projection content when keyword is detected"""
        if not self.projection_active:
            self.projection_active = True
            print("🎬 Triggering projection content...")
            
            # Start projection in separate thread
            threading.Thread(target=self.run_projection, daemon=True).start()
    
    def run_projection(self):
        """Run the projection display"""
        try:
            # Set up projection window
            screen_info = pygame.display.Info()
            projection_width = 800
            projection_height = 600
            
            # Create projection window
            projection_screen = pygame.display.set_mode((projection_width, projection_height))
            pygame.display.set_caption("Diaspora Projection")
            
            # Define colors
            BLACK = (0, 0, 0)
            WHITE = (255, 255, 255)
            CREAM = (245, 245, 220)
            
            # Font setup
            font_large = pygame.font.Font(None, 48)
            font_medium = pygame.font.Font(None, 36)
            font_small = pygame.font.Font(None, 24)
            
            # Projection content - simulating diary entries
            projection_texts = [
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
            ]
            
            # Animation variables
            start_time = time.time()
            line_delay = 1.5  # seconds between lines
            char_delay = 0.05  # seconds between characters
            
            running = True
            clock = pygame.time.Clock()
            
            while running and self.projection_active:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        running = False
                        
                projection_screen.fill(CREAM)
                
                current_time = time.time() - start_time
                
                # Display text line by line with typewriter effect
                y_offset = 50
                for i, line in enumerate(projection_texts):
                    line_start_time = i * line_delay
                    
                    if current_time >= line_start_time:
                        # Calculate how many characters to show
                        chars_to_show = int((current_time - line_start_time) / char_delay)
                        displayed_text = line[:chars_to_show]
                        
                        if displayed_text:
                            text_surface = font_medium.render(displayed_text, True, BLACK)
                            projection_screen.blit(text_surface, (50, y_offset))
                    
                    y_offset += 40
                
                # Add a subtle fade effect for atmosphere
                alpha = min(255, int((current_time / 2) * 255))
                fade_surface = pygame.Surface((projection_width, projection_height))
                fade_surface.set_alpha(max(0, 50 - alpha//5))
                fade_surface.fill(BLACK)
                projection_screen.blit(fade_surface, (0, 0))
                
                pygame.display.flip()
                clock.tick(60)
                
                # Auto-close after content is displayed
                if current_time > len(projection_texts) * line_delay + 5:
                    running = False
            
            pygame.quit()
            self.projection_active = False
            print("📺 Projection ended")
            
        except Exception as e:
            print(f"Projection error: {e}")
            self.projection_active = False
    
    def run(self):
        """Main loop for camera capture and OCR processing"""
        frame_count = 0
        
        while True:
            ret, frame = self.cap.read()
            if not ret:
                print("Failed to capture frame")
                break
            
            frame_count += 1
            
            # Display camera feed
            display_frame = frame.copy()
            
            # Add status overlay
            status_text = f"Searching for: '{self.target_keyword}'"
            cv2.putText(display_frame, status_text, (10, 30), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
            
            if self.projection_active:
                cv2.putText(display_frame, "PROJECTION ACTIVE", (10, 60), 
                           cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
            
            # Detect text regions and draw bounding boxes
            text_regions = self.detect_text_regions(frame)
            for (x, y, w, h) in text_regions:
                cv2.rectangle(display_frame, (x, y), (x + w, y + h), (255, 0, 0), 1)
            
            cv2.imshow('Interactive Book CV - Camera Feed', display_frame)
            
            # Perform OCR every few frames to avoid overprocessing
            if frame_count % 30 == 0 and not self.processing_frame:  # Reduced frequency
                self.processing_frame = True
                
                # Run OCR in separate thread to avoid blocking
                def ocr_thread():
                    keyword_found, detected_text = self.perform_ocr(frame)
                    if keyword_found:
                        print("The keyword is captured")
                        self.trigger_projection()
                    self.processing_frame = False
                    
                    # Only output when keyword is found - no debug text
                
                threading.Thread(target=ocr_thread, daemon=True).start()
            
            # Handle key presses
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                break
            elif key == ord('s'):
                # Save current frame for debugging
                filename = f"debug_frame_{int(time.time())}.jpg"
                cv2.imwrite(filename, frame)
                print(f"Saved frame: {filename}")
            elif key == ord('t'):
                # Manual trigger for testing
                print("Manual trigger activated")
                self.trigger_projection()
    
    def cleanup(self):
        """Clean up resources"""
        self.cap.release()
        cv2.destroyAllWindows()
        if pygame.get_init():
            pygame.quit()

if __name__ == "__main__":
    print("=== Interactive Book CV System ===")
    print("Initializing system...")
    
    try:
        book_cv = InteractiveBookCV()
        print("System ready! Point camera at text containing 'immigrants'")
        print("Controls:")
        print("  q - Quit")
        print("  s - Save current frame")
        print("  t - Manual trigger (for testing)")
        print("\nStarting camera feed...")
        
        book_cv.run()
        
    except KeyboardInterrupt:
        print("\nShutting down...")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        if 'book_cv' in locals():
            book_cv.cleanup()
        print("System shutdown complete.")