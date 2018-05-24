#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

unsigned long getFileLength(std::ifstream& file)
{
    if(!file.good()) return 0;
    unsigned long pos=file.tellg();
    file.seekg(0,std::ios::end);
    unsigned long len = file.tellg();
    file.seekg(std::ios::beg);
    return len;
}

int loadshader(char filename[], GLchar** ShaderSource)
{
    unsigned long len;
    std::ifstream file;
    file.open(filename,std::ios::in); // opens as ASCII!
    if(!file) return -1;

    len = getFileLength(file);
    
    if (len==0) return -2;   // Error: Empty File 
    
    *ShaderSource = (GLchar*) new char[(len)+1];
    if (*ShaderSource == 0) return -3;   // can't reserve memory
    // len isn't always strlen cause some characters are stripped in ascii read...
    // it is important to 0-terminate the real length later, len is just max possible value... 
    (*ShaderSource)[len] = '\0';
    unsigned int i = 0;
    while (file.good())
    {
        file.get((*ShaderSource)[i]);       // get character from file.
        if (!file.eof())
        i++;
    }
    (*ShaderSource)[i] = 0;  // 0-terminate it at the correct position
    
    file.close();
      
    return 0; // No Error
}

int unloadshader(GLchar** ShaderSource)
{
    if (*ShaderSource != 0)
        delete[] *ShaderSource;
    *ShaderSource = 0;
    return 0;
}

GLuint makeShaderProgram(GLchar *pfragshadersrc, GLchar *pvertshadersrc){
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &pvertshadersrc, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar log[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        std::cout << "Vertex shader compilation failed:\n"
                  << log << std::endl;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &pfragshadersrc, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
        std::cout << "Fragment shader compilation failed:\n"
                  << log << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, log);
        std::cout << "Shader program linking failed:\n"
                  << log << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

