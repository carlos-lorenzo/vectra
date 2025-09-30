#!/usr/bin/env bash
echo "Updating git submodules..."
git submodule update --recursive --remote
echo "Submodules updated."