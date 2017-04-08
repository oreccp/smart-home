#!/bin/bash

pip install virtualenv

rm -rf venv
virtualenv venv
source venv/bin/activate
pip install rpi.gpio flask flask-ask
