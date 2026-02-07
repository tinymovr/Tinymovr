#!/bin/bash
# PyOCD wrapper script for flexible discovery
# Tries pyocd from PATH first, then falls back to project venv

# Get the workspace root (parent of firmware/scripts)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE="$(cd "$SCRIPT_DIR/../.." && pwd)"

if command -v pyocd &> /dev/null; then
    exec pyocd "$@"
elif [ -f "${WORKSPACE}/venv/bin/pyocd" ]; then
    exec "${WORKSPACE}/venv/bin/pyocd" "$@"
else
    echo "Error: pyocd not found."
    echo "Install with: pip install pyocd"
    echo "Or create a virtual environment at ${WORKSPACE}/venv with pyocd installed."
    exit 1
fi
