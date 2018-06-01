#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common.h"
#include "fruit.h"

Fruit::Fruit(GLuint shader):shader(shader){

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);
}
Fruit::~Fruit(){
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}



GLuint Fruit::getVBO()
{
    return this->VBO;
}
GLuint Fruit::getVAO()
{
    return this->VAO;
}

GLuint Fruit::getShader()
{
    return this->shader;
}



