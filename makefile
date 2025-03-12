.PHONY: build
build:
	g++ -o main  src/*.cpp src/*.c src/**/*.cpp  -Iinclude -lfreetype  -I/usr/include/freetype2   -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl 
