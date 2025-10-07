#!/bin/bash

# Script to automatically update CMakeLists.txt in src/main with latest source files
# Run from project root: ./scripts/update-cmake-sources.sh
# Use --verbose or -v for detailed output

set -e

CMAKE_FILE="src/main/CMakeLists.txt"
TEMP_FILE=$(mktemp)
VERBOSE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        *)
            echo "Usage: $0 [-v|--verbose]"
            exit 1
            ;;
    esac
done

if [[ ! -f "$CMAKE_FILE" ]]; then
    echo "Error: $CMAKE_FILE not found"
    exit 1
fi

log_verbose() {
    if [[ "$VERBOSE" == "true" ]]; then
        echo "$1"
    fi
}

log_verbose "Updating $CMAKE_FILE with latest source files..."

# Function to find and sort files for a module
generate_module_files() {
    local module="$1"
    local module_path="src/$module"

    if [[ ! -d "$module_path" ]]; then
        return
    fi

    # Find all .cpp and .h files, sort them
    find "$module_path" -type f \( -name "*.cpp" -o -name "*.h" \) | \
        sed "s|src/|../|g" | \
        sort
}

# Function to find include directories (public and private)
generate_include_dirs() {
    local include_type="$1"  # "public" or "private"

    for module in $(ls src/ | grep -v main | sort); do
        if [[ -d "src/$module" ]]; then
            if [[ "$include_type" == "public" ]]; then
                # Add the main module directory
                echo "        \"../$module\""

                # Add any subdirectories that don't contain "priv"
                find "src/$module" -type d -mindepth 1 | \
                    grep -v "/priv" | \
                    sed "s|src/|        \"../|g; s|$|\"|" | \
                    sort
            else  # private
                # Add directories containing "priv"
                find "src/$module" -type d -name "*priv*" | \
                    sed "s|src/|        \"../|g; s|$|\"|" | \
                    sort
            fi
        fi
    done
}

# Extract the existing REQUIRES section
log_verbose "Extracting existing REQUIRES section..."
REQUIRES_SECTION=$(awk '
    /^[[:space:]]*idf_component_register/ { in_register = 1 }
    in_register && /REQUIRES/ { in_requires = 1; next }
    in_register && in_requires && /SRCS/ { in_requires = 0; next }
    in_register && in_requires { print }
    in_register && /^\)/ { in_register = 0 }
' "$CMAKE_FILE")

# Extract existing SRCS for comparison
OLD_SRCS=$(awk '
    /^[[:space:]]*idf_component_register/ { in_register = 1 }
    in_register && /SRCS/ { in_srcs = 1; next }
    in_register && in_srcs && /(INCLUDE_DIRS|PRIV_INCLUDE_DIRS|\))/ { in_srcs = 0; next }
    in_register && in_srcs && /^[[:space:]]*"/ {
        gsub(/^[[:space:]]*"|"[[:space:]]*$/, "")
        print
    }
    in_register && /^\)/ { in_register = 0 }
' "$CMAKE_FILE")

# Generate new SRCS section
log_verbose "Generating SRCS section..."
SRCS_SECTION=""
NEW_SRCS_LIST=""

# Add main files first
for main_file in $(find src/main -maxdepth 1 -name "*.cpp" | sort); do
    filename=$(basename "$main_file")
    SRCS_SECTION="${SRCS_SECTION}        \"$filename\"\n"
    NEW_SRCS_LIST="${NEW_SRCS_LIST}$filename\n"
done

# Add empty line after main files
if [[ -n "$SRCS_SECTION" ]]; then
    SRCS_SECTION="${SRCS_SECTION}\n"
fi

# Process each module
for module in $(ls src/ | grep -v main | sort); do
    if [[ -d "src/$module" ]]; then
        module_files=$(generate_module_files "$module")
        if [[ -n "$module_files" ]]; then
            # Add module files
            while IFS= read -r file; do
                SRCS_SECTION="${SRCS_SECTION}        \"$file\"\n"
                NEW_SRCS_LIST="${NEW_SRCS_LIST}$file\n"
            done <<< "$module_files"

            # Add empty line between modules
            SRCS_SECTION="${SRCS_SECTION}\n"
        fi
    fi
done

# Remove trailing newline
SRCS_SECTION=$(echo -e "$SRCS_SECTION" | sed '$ { /^[[:space:]]*$/ d; }')
NEW_SRCS_LIST=$(echo -e "$NEW_SRCS_LIST" | sed '/^[[:space:]]*$/d')

# Compare old and new SRCS
TEMP_OLD=$(mktemp)
TEMP_NEW=$(mktemp)
echo "$OLD_SRCS" > "$TEMP_OLD"
echo "$NEW_SRCS_LIST" > "$TEMP_NEW"

ADDED_FILES=$(comm -13 <(sort "$TEMP_OLD") <(sort "$TEMP_NEW"))
REMOVED_FILES=$(comm -23 <(sort "$TEMP_OLD") <(sort "$TEMP_NEW"))

ADDED_COUNT=$(echo "$ADDED_FILES" | grep -c . || echo 0)
REMOVED_COUNT=$(echo "$REMOVED_FILES" | grep -c . || echo 0)

# Generate INCLUDE_DIRS section
log_verbose "Generating INCLUDE_DIRS section..."
INCLUDE_DIRS=$(generate_include_dirs "public")

# Generate PRIV_INCLUDE_DIRS section
log_verbose "Generating PRIV_INCLUDE_DIRS section..."
PRIV_INCLUDE_DIRS=$(generate_include_dirs "private")

# Extract everything before idf_component_register
log_verbose "Extracting content before idf_component_register..."
BEFORE_REGISTER=$(awk '
    /^[[:space:]]*idf_component_register/ { exit }
    { print }
' "$CMAKE_FILE")

# Extract everything after the closing parenthesis of idf_component_register
log_verbose "Extracting content after idf_component_register..."
AFTER_REGISTER=$(awk '
    BEGIN { in_register = 0; after_register = 0 }
    /^[[:space:]]*idf_component_register/ { in_register = 1; next }
    in_register && /^\)/ { in_register = 0; after_register = 1; next }
    after_register { print }
' "$CMAKE_FILE")

# Generate the new CMakeLists.txt content
log_verbose "Generating new CMakeLists.txt content..."
{
    # Content before idf_component_register (if any)
    if [[ -n "$BEFORE_REGISTER" ]]; then
        echo "$BEFORE_REGISTER"
    fi

    # Start idf_component_register
    echo "idf_component_register("

    # REQUIRES section
    if [[ -n "$REQUIRES_SECTION" ]]; then
        echo "        REQUIRES"
        echo "$REQUIRES_SECTION"
        echo ""
    fi

    # SRCS section
    echo "        SRCS"
    echo -e "$SRCS_SECTION"
    echo ""

    # INCLUDE_DIRS section
    if [[ -n "$INCLUDE_DIRS" ]]; then
        echo "        INCLUDE_DIRS"
        echo "$INCLUDE_DIRS"
    fi

    # PRIV_INCLUDE_DIRS section (only if there are private dirs)
    if [[ -n "$PRIV_INCLUDE_DIRS" ]]; then
        echo ""
        echo "        PRIV_INCLUDE_DIRS"
        echo "$PRIV_INCLUDE_DIRS"
    fi

    # Close idf_component_register
    echo ")"

    # Content after idf_component_register (if any)
    if [[ -n "$AFTER_REGISTER" ]]; then
        echo ""
        echo "$AFTER_REGISTER"
    fi

} > "$TEMP_FILE"

# Replace the original file
mv "$TEMP_FILE" "$CMAKE_FILE"

# Clean up temp files
rm -f "$TEMP_OLD" "$TEMP_NEW"

# Output results
if [[ "$VERBOSE" == "true" ]]; then
    echo "✅ Successfully updated $CMAKE_FILE"
    echo "📂 Modules processed: $(ls src/ | grep -v main | tr '\n' ' ')"

    if [[ $ADDED_COUNT -gt 0 ]]; then
        echo "➕ Added files ($ADDED_COUNT):"
        echo "$ADDED_FILES" | sed 's/^/  /'
    fi

    if [[ $REMOVED_COUNT -gt 0 ]]; then
        echo "➖ Removed files ($REMOVED_COUNT):"
        echo "$REMOVED_FILES" | sed 's/^/  /'
    fi

    if [[ $ADDED_COUNT -eq 0 && $REMOVED_COUNT -eq 0 ]]; then
        echo "📋 No changes detected"
    fi
else
    # Non-verbose output
    echo "Updated CMakeLists.txt: +$ADDED_COUNT new files, -$REMOVED_COUNT deleted files"
fi