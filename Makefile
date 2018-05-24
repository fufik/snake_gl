all :
	g++ main.cpp shader.cpp snake.cpp -std=c++14 -fpermissive -Wwrite-strings -lglfw -lGL -lGLEW -o test
