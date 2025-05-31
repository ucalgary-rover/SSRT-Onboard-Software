#!/bin/bash
set -e

echo "Updating package list..."
sudo apt update

echo "Installing Python3..."
sudo apt install -y python3

echo "Installing python3-opencv..."
sudo apt install -y python3-opencv

echo "Installing python3-flask..."
sudo apt install -y python3-flask

echo "Installing python3-psutil..."
sudo apt install -y python3-psutil

echo "All dependencies installed."
echo "You can now run the server with: python3 CameraFeedServer.py"
