@echo off
echo Initializing git submodules...
git submodule update --init --recursive

echo Updating git submodules...
git submodule update --recursive --remote

echo Submodules updated.
pause

