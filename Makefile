all :
	g++ main.cpp shader.cpp snake.cpp fruit.cpp -std=c++14 -fpermissive -Wwrite-strings -I/usr/include/freetype2 -lglfw -lGL -lopenal -lalut -lGLEW -lfreetype -o test
