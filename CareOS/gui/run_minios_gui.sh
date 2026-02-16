#!/usr/bin/env bash
# Run MiniOS GUI wrapper

set -e
cd "$(dirname "$0")/.." || exit 1

if [ ! -x ./shell ]; then
    echo "ERROR: ./shell not found. Build using: make"
    exit 1
fi

python3 gui/elderly_gui.py ./shell
