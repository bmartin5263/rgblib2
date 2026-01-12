#!/bin/bash

# Sets up your terminal session for development

set -e

ORIGINAL_DIRECTORY=pwd


cd ~esp/esp-idf
./export.sh
cd $ORIGINAL_DIRECTORY