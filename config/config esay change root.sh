#!/bin/bash

# Check if the new root directory is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <new_root_directory>"
    exit 1
fi

NEW_ROOT="$1"

CONFIG_FILE="default.conf"

cp "$CONFIG_FILE" "${CONFIG_FILE}.bak"

CURRENT_ROOT=$(grep -oP 'root \K[^;]+' "$CONFIG_FILE" | head -n 1)

if [ -z "$CURRENT_ROOT" ]; then
    echo "No 'root' directive found in the configuration file."
    exit 1
fi

sed -i.bak "s|$CURRENT_ROOT|$NEW_ROOT|g;" "$CONFIG_FILE"

echo "Updated 'root' directives and aliases in $CONFIG_FILE to '$NEW_ROOT'."