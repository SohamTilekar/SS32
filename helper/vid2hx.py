import tkinter as tk
from tkinter import filedialog
import cv2

def video_to_compressed_pixel_values(video_path, output_file):
    # Open the video file
    cap = cv2.VideoCapture(video_path)
    
    if not cap.isOpened():
        print("Error opening video file.")
        return

    # Open the output file for writing
    with open(output_file, 'w') as file:
        frame_count = 0
        fps = 3  # Set the desired FPS
        frame_skip = int(cap.get(cv2.CAP_PROP_FPS) / fps)
        
        while cap.isOpened():
            ret, frame = cap.read()
            if not ret:
                break
            # Skip frames based on the desired FPS
            if frame_count % frame_skip != 0:
                frame_count += 1
                continue
            # Resize the frame to 256x256
            frame = cv2.resize(frame, (256, 256))
            # Convert the frame to RGB (OpenCV uses BGR by default)
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            height, width, _ = frame.shape
            
            # Iterate over each row of pixels in the frame
            for y in range(height):
                x = 0
                while x < width:
                    r, g, b = frame[y, x]
                    hex_value = f"00{r:02X}{g:02X}{b:02X}\n"
                    # Write the hex value to the file
                    file.write(hex_value)
                    x += 1
            frame_count += 1
            print(f"Processed frame {frame_count}")
        
    cap.release()
    print(f"Compressed pixel values have been written to {output_file}")

def main():
    # Create a tkinter root window
    root = tk.Tk()
    root.withdraw()  # Hide the root window

    # Ask the user to select the input video file
    video_path = filedialog.askopenfilename(
        title="Select a Video File",
        filetypes=[("Video files", "*.mp4;*.avi;*.mov;*.mkv;*.wmv")]
    )
    if not video_path:
        print("No video file selected.")
        return

    # Ask the user to select the output text file
    output_file = filedialog.asksaveasfilename(
        title="Save Output File",
        defaultextension=".txt",
        filetypes=[("Text files", "*.txt"), ("Hex Files", "*.hex")]
    )
    if not output_file:
        print("No output file selected.")
        return

    # Process the video and write the compressed pixel values to the output file
    video_to_compressed_pixel_values(video_path, output_file)
    print(f"Compressed pixel values have been written to {output_file}")

if __name__ == "__main__":
    main()
