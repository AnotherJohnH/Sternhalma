
targets = native Emscripten

all: $(targets)

docs: Emscripten
	cp build_Emscripten/sternh.html docs
	cp build_Emscripten/sternh.js   docs
	cp build_Emscripten/sternh.wasm docs
	doxygen

include Platform/build.make
