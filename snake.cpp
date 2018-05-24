#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common.h"
#include "snake.h"

Snake::Snake(unsigned int size,GLuint shader){
    this->dir = None;
    this->size = size;
    this->snake = new snakeUnit[size];
    this->shader = shader;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);
}
Snake::~Snake(){
    delete[] this->snake;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

Direction Snake::getDirection(){
    return this->dir;
}
void Snake::setDirection(Direction a){
    this->dir = a;
}
unsigned int Snake::getLen()
{
    return this->len;
}
void Snake::setLen(unsigned int len)
{
    this->len = len;
}

GLuint Snake::getVBO()
{
    return this->VBO;
}
GLuint Snake::getVAO()
{
    return this->VAO;
}

GLuint Snake::getShader()
{
    return this->shader;
}

