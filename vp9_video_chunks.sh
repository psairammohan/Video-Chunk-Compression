#!/bin/bash

# Iterate over each .avi file in the current directory
for file in chunk_*.mkv; do
    # Generate output filename with .webm extension
    output="output_${file%.mkv}.webm"
    
    # Re-encode the file with VP9 codec using FFmpeg
    ffmpeg -i "$file" -c:v libvpx-vp9 -b:v 4000k -minrate 4000k -maxrate 4000k -quality realtime -speed 5 "$output"
done
