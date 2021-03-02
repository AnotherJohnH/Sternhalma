
EXECUTABLE = mines

#-------------------------------------------------------------------------------

all: native Emscripten

clean:
	rm -rf build_*

debug:
	Platform/BUILD.py native -s -d

#-------------------------------------------------------------------------------

docs: Emscripten
	cp build_Emscripten/sternh.html docs
	cp build_Emscripten/sternh.js   docs
	cp build_Emscripten/sternh.wasm docs
	doxygen

#-------------------------------------------------------------------------------

native:
	Platform/BUILD.py native

Emscripten:
	Platform/BUILD.py Emscripten

mbedLPC1768:
	Platform/BUILD.py mbedLPC1768
