#!/bin/bash

# Script to automatically update src/main/CMakeLists.txt with current source files
# and include directories from the src directory

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$PROJECT_ROOT/src"
CMAKE_FILE="$SRC_DIR/main/CMakeLists.txt"

# Check if src directory exists
if [ ! -d "$SRC_DIR" ]; then
    echo "Error: src directory not found at $SRC_DIR"
    exit 1
fi

# Check if CMakeLists.txt exists
if [ ! -f "$CMAKE_FILE" ]; then
    echo "Error: CMakeLists.txt not found at $CMAKE_FILE"
    exit 1
fi

echo "Scanning src directory and updating CMakeLists.txt..."

# Create temporary file
TEMP_FILE=$(mktemp)

# Extract everything before SRCS
sed -n '1,/^[[:space:]]*SRCS[[:space:]]*$/p' "$CMAKE_FILE" > "$TEMP_FILE"

# Add main.cpp first
echo '        "main.cpp"' >> "$TEMP_FILE"
echo "" >> "$TEMP_FILE"

# Find all subdirectories under src (excluding main)
SUBDIRS=$(find "$SRC_DIR" -mindepth 1 -maxdepth 1 -type d ! -name "main" | sort)

# Process each subdirectory
for subdir in $SUBDIRS; do
    subdir_name=$(basename "$subdir")
    
    # Find all .cpp and .h files in this subdirectory
    files=$(find "$subdir" -maxdepth 1 -type f \( -name "*.cpp" -o -name "*.h" \) | sort)
    
    if [ -n "$files" ]; then
        # Add files for this subdirectory
        while IFS= read -r file; do
            relative_path="../$subdir_name/$(basename "$file")"
            echo "        \"$relative_path\"" >> "$TEMP_FILE"
        done <<< "$files"
        
        # Add empty line after subdirectory
        echo "" >> "$TEMP_FILE"
    fi
done

# Add the REQUIRES section and everything after SRCS but before INCLUDE_DIRS
sed -n '/^[[:space:]]*REQUIRES/,/^[[:space:]]*INCLUDE_DIRS[[:space:]]*$/p' "$CMAKE_FILE" | sed '$d' >> "$TEMP_FILE"

# Add INCLUDE_DIRS line
echo "        INCLUDE_DIRS" >> "$TEMP_FILE"

# Add include directories for each subdirectory (excluding main)
for subdir in $SUBDIRS; do
    subdir_name=$(basename "$subdir")
    echo "        \"../$subdir_name\"" >> "$TEMP_FILE"
done

# Add closing parenthesis and any remaining content
sed -n '/^[[:space:]]*INCLUDE_DIRS/,$p' "$CMAKE_FILE" | tail -n +2 | grep -A999 "^)" >> "$TEMP_FILE"

# Replace the original file
mv "$TEMP_FILE" "$CMAKE_FILE"

echo "Successfully updated $CMAKE_FILE"
echo "Updated source files and include directories based on current src/ structure"