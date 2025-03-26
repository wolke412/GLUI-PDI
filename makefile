.PHONY: build
build:
	g++                            \
		-march=native -mavx2 -O3 \
		-o main  src/*.cpp src/*.c src/**/*.cpp  \
		-Iinclude -lfreetype  -I/usr/include/freetype2   -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl 

