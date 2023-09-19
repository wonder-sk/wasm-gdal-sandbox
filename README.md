
# GDAL test harness for WebAssembly build

This is a clone of GDAL repository for development and testing of patches specific to WebAssembly builds, for use in qgis-js.

 - All dependencies are coming from vcpkg (like with qgis-js)
 - Using custom vcpkg triplet to support multi-threading and wasm exceptions
 - There's an extra executable to test modifications (`test_cog_read`) with a sample COG file

Custom WebAssembly patches:

 - Vsicurl implemented using emscripten's fetch API (proof of concept with many to-dos, just enough to read remote COGs)
 - Avoid warnings in console on CPLGetUsablePhysicalRAM() call ("unsupported syscall: __syscall_prlimit64")


## Using this repo

Clone the repo at this branch:

```
git clone https://github.com/wonder-sk/wasm-gdal-sandbox.git --branch vsicurl-fetch-api
```


Get emscripten env variables loaded:

```
source /home/martin/inst/emsdk/emsdk_env.sh
```


Configure the build:
```
VCPKG_DIR=/home/martin/inst/vcpkg \
EMSCRIPTEN_DIR=/home/martin/inst/emsdk/upstream/emscripten \
${VCPKG_DIR}/downloads/tools/cmake-3.27.1-linux/cmake-3.27.1-linux-x86_64/bin/cmake \
  -B build-wasm -S . -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=wasm32-emscripten-threads \
  -DVCPKG_OVERLAY_TRIPLETS=./vcpkg-triplets \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${PWD}/vcpkg-triplets/toolchain.cmake \
  -DBUILD_TESTING=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_APPS=OFF \
  -DGDAL_USE_HDF5=OFF \
  -DGDAL_ENABLE_DRIVER_HDF5=OFF \
  -DGDAL_USE_ICONV=OFF \
  -DGDAL_ENABLE_DRIVER_JPEG=OFF \
  -DGDAL_ENABLE_DRIVER_MRF=OFF \
  -DGDAL_USE_PNG=OFF \
  -DGDAL_ENABLE_DRIVER_PNG=OFF \
  -DCMAKE_BUILD_TYPE=Debug
```

Build GDAL library and the test executable:
```
cmake --build build-wasm --target test_cog_read
```

## Testing

To test remote COG fetching, we need a HTTP server that support range requests and cross-origin resource sharing.

```
cd test-cog/data
npm install http-server
```

Run the server in the "data" directory:

```
npx http-server --cors
```

Finally run the test executable (in the repo root dir):

```
emrun build-wasm/test-cog/test_cog_read.html
```

## Notes

JPG, PNG, MRF drivers are disabled in the configuration because of some clash of setjmp and exception handling in LLVM.
Iconv was also throwing some compilation error.
HDF5 was also causing compilation issues (forgotten what exactly).
