all :
	g++ main.cpp shader.cpp snake.cpp -std=c++14 -fpermissive -Wwrite-strings -I/usr/include/freetype2 -lglfw -lGL -lGLEW -lfreetype -o test
