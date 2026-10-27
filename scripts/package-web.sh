#!/bin/sh
# Builds the browser version and packages it for upload. itch.io wants a zip
# with index.html at the top, so the page is renamed on the way in. The
# build-web/itch folder is also what goes on a regular website.
#
# Run from the project folder:  sh scripts/package-web.sh
set -e # stop at the first command that fails

emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web

rm -rf build-web/itch build-web/dusk-catchers-web.zip
mkdir build-web/itch
cp build-web/dusk_catchers.html build-web/itch/index.html
cp build-web/dusk_catchers.js build-web/dusk_catchers.wasm build-web/dusk_catchers.data build-web/itch/
(cd build-web/itch && zip -q -r ../dusk-catchers-web.zip .)

echo "Ready to upload:"
echo "  itch.io:  build-web/dusk-catchers-web.zip"
echo "  website:  the files in build-web/itch/"
