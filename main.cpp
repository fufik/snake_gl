#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "common.h"
#include "shader.h"
#include "snake.h"
#include "fruit.h"

GLchar *vertexShaderSource,
       *vertexTextShaderSource,
       *fragmentShaderSource, 
       *fragmentLineShaderSource,
       *fragmentSnakeShaderSource,
       *fragmentFruitShaderSource,
       *fragmentTextShaderSource;
       
FT_Library ft;

const int windowWidth = 768;
const int verticesAmount = 20;
const int fieldWidth = verticesAmount - 1;
const float coordSync  = verticesAmount % 2 == 0?(fieldWidth/2):(fieldWidth/2 - 0.5);
const int snakeMaxLength = (verticesAmount-1)*(verticesAmount-1);

GLuint fieldVBO, fieldVAO;

void drawField(GLuint pShader,GLuint lShader);
unsigned long getFileLength(std::ifstream& file);
void drawSnake(Snake *pSnake);
void drawGameOver(GLuint shader,FT_Face f);
void moveSnake(Snake* pSnake,Fruit* pFruit);
void setFruit(Snake* pSnake,Fruit* pFruit);
void drawFruit(Fruit* pFruit);

Snake* pPlayer;

bool isOver = false;

Direction getDirByButton(int button){
    if(button == GLFW_KEY_W)
            return Up;
        else if(button == GLFW_KEY_S)
            return Down;
        else if(button == GLFW_KEY_A)
            return Left;
        else if(button == GLFW_KEY_D)
            return Right;
        else return None;
}

static void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    Direction curDir = pPlayer->getDirection();
    if(curDir + getDirByButton(key) != 0){
        pPlayer->setDirection(getDirByButton(key));
    }
}
int main(){
    loadShader("f.vert",&vertexShaderSource);
    loadShader("text.vert",&vertexTextShaderSource);
    loadShader("f.frag",&fragmentShaderSource);
    loadShader("fl.frag",&fragmentLineShaderSource);
    loadShader("snake.frag",&fragmentSnakeShaderSource);
    loadShader("fruit.frag",&fragmentFruitShaderSource);
    loadShader("text.frag",&fragmentTextShaderSource);
    if (glfwInit() == GLFW_FALSE){
        std::cout << "Failed to initialise the GLFW" << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(windowWidth, windowWidth, "Snake", nullptr, nullptr);
    if (window == nullptr){
        std::cout << "Failed to create a GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK){
        std::cout << "Failed to initialise the GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderProgram = makeShaderProgram(fragmentShaderSource,vertexShaderSource);
    GLuint shaderLProgram = makeShaderProgram(fragmentLineShaderSource,vertexShaderSource);
    GLuint shaderSnakeProgram = makeShaderProgram(fragmentSnakeShaderSource,vertexShaderSource);
    GLuint shaderFruitProgram = makeShaderProgram(fragmentFruitShaderSource,vertexShaderSource);
    GLuint shaderTextProgram = makeShaderProgram(fragmentTextShaderSource,vertexTextShaderSource);
    unloadShader(&vertexShaderSource);
    unloadShader(&vertexTextShaderSource);
    unloadShader(&fragmentShaderSource);
    unloadShader(&fragmentLineShaderSource);
    unloadShader(&fragmentSnakeShaderSource);
    unloadShader(&fragmentFruitShaderSource);
    unloadShader(&fragmentTextShaderSource);
    if(FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetype library\n");
        return EXIT_FAILURE;
    }
    FT_Face face;
    if(FT_New_Face(ft, "LiberationSerif-Bold.ttf", 0, &face)) {
        fprintf(stderr, "Could not open font\n");
        return EXIT_FAILURE;
    }
    FT_Set_Pixel_Sizes(face, 0, 70);
    
    point vertices[verticesAmount + 1][verticesAmount];
    for(int i = 0; i < verticesAmount; i++) {
        for(int j = 0; j < verticesAmount; j++) {
            if(verticesAmount%2 == 0){
                vertices[i][j].x = (j - (verticesAmount-1)/2.0)/10.0;
                vertices[i][j].y = (i - (verticesAmount-1)/2.0)/10.0;
            } else {
                vertices[i][j].x = (j - verticesAmount/2)/10.0;
                vertices[i][j].y = (i - verticesAmount/2)/10.0;
            }
        }
    }
    vertices[verticesAmount][0] = vertices[0][0];
    vertices[verticesAmount][1] = vertices[0][verticesAmount-1];
    vertices[verticesAmount][2] = vertices[verticesAmount-1][verticesAmount-1];
    vertices[verticesAmount][3] = vertices[verticesAmount-1][0];
    vertices[verticesAmount][4] = vertices[0][0];
    
    Snake* pSnake = new Snake(snakeMaxLength,shaderSnakeProgram);
    pPlayer = pSnake;
    pSnake->setLen(3);
    pSnake->snake[0].coords = {coordSync,coordSync};
    pSnake->snake[0].is = true;
    pSnake->snake[1].coords = {coordSync,coordSync - 1};
    pSnake->snake[1].is = true;
    pSnake->snake[2].coords = {coordSync,coordSync - 2};
    pSnake->snake[2].is = true;
    
    Fruit* pFruit = new Fruit(shaderFruitProgram);
    pFruit->coords = {0,0};
    glGenBuffers(1, &fieldVBO);
    glGenVertexArrays(1, &fieldVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fieldVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(fieldVAO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,0,0);
	

    glfwSetKeyCallback(window,keycallback);
    glPointSize(5.0);
	while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if(!isOver)
            setFruit(pSnake,pFruit);
            moveSnake(pSnake,pFruit); // changes isOver, therefore I divided these calls
        if(!isOver){
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            drawField(shaderProgram,shaderLProgram);
            drawFruit(pFruit);
            drawSnake(pSnake);
            
        }
        if(isOver)
           drawGameOver(shaderTextProgram,face);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &fieldVAO);
    glDeleteBuffers(1, &fieldVBO);
    delete pSnake;
    glfwTerminate();
    return EXIT_SUCCESS;
}

void drawField(GLuint pointShader,GLuint borderShader){
    //Field        
        glBindBuffer(GL_ARRAY_BUFFER, fieldVBO);
        glBindVertexArray(fieldVAO);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,0,0);
        glEnableVertexAttribArray(0);
        glPointSize( 5 );
        glUseProgram(pointShader);
        for(int i = 0; i < verticesAmount; i++)
            glDrawArrays(GL_POINTS,verticesAmount*i,verticesAmount);
        glUseProgram(borderShader);
        glDrawArrays(GL_LINE_STRIP,verticesAmount*verticesAmount,5);
        //glDrawArrays(GL_POINTS,verticesAmount*verticesAmount+5,2);
        
}

void setFruit(Snake* pSnake,Fruit* pFruit){
    if(pFruit->isEaten){
        srand(time(NULL));
        pFruit->coords.x = rand()%fieldWidth;
        pFruit->coords.y = rand()%fieldWidth;
        //pFruit->coords = {0,0};
        std::cout << "fruit: "<< pFruit->coords.x << " " << pFruit->coords.y << std::endl;
      /*  bool collides = true;
        while(collides)
        {
            srand(time(NULL));
            fruit.x = rand()%fieldWidth;
            fruit.y = rand()%fieldWidth;
            int l = pSnake->getLen();
            for(int i = 0; i < l; i++)
            {
                if (fruit != pSnake->snake[i].coords)
                {
                    collides = false;
                }else{
                    collides = true;
                    break;
                }
            }
        }*/
        pFruit->isEaten = false;
    }
}

void drawFruit(Fruit* pFruit){
    point toRender = {(pFruit->coords.x - coordSync)/10,(pFruit->coords.y - coordSync)/10};
    glBindBuffer(GL_ARRAY_BUFFER, pFruit->getVBO());
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), &toRender, GL_STATIC_DRAW);
    glBindVertexArray(pFruit->getVAO());
    glPointSize( 10 );
    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,0,0);
    //glEnableVertexAttribArray(0);
    glUseProgram(pFruit->getShader());
    glDrawArrays(GL_POINTS,0,1);
}

void drawSnake(Snake* pSnake){
    int snakeLen = pSnake->getLen();
    point toRender[snakeLen*5];
    for (int i = 0;i < snakeLen; i++){ //turning every dot into a square
        if (pSnake->snake[i].is == false)
            break;
        //toRender[i*snakeLen + 0] = {snake[i].coords.x,snake[i].coords.y};
        toRender[i*5 + 0] = {(pSnake->snake[i].coords.x - (GLfloat)0.5 - coordSync)/10,
                             (pSnake->snake[i].coords.y - (GLfloat)0.5 - coordSync)/10};  //LD
        toRender[i*5 + 1] = {(pSnake->snake[i].coords.x - (GLfloat)0.5 - coordSync)/10,
                             (pSnake->snake[i].coords.y + (GLfloat)0.5 - coordSync)/10};  //LU
        toRender[i*5 + 2] = {(pSnake->snake[i].coords.x + (GLfloat)0.5 - coordSync)/10,
                             (pSnake->snake[i].coords.y + (GLfloat)0.5 - coordSync)/10};  //RU
        toRender[i*5 + 3] = {(pSnake->snake[i].coords.x + (GLfloat)0.5 - coordSync)/10,
                             (pSnake->snake[i].coords.y - (GLfloat)0.5 - coordSync)/10};  //RD
        toRender[i*5 + 4] = {(pSnake->snake[i].coords.x - (GLfloat)0.5 - coordSync)/10,
                             (pSnake->snake[i].coords.y - (GLfloat)0.5 - coordSync)/10};  //LD
    }
    glBindBuffer(GL_ARRAY_BUFFER, pSnake->getVBO());
    glBufferData(GL_ARRAY_BUFFER, sizeof(point)*snakeLen*5, toRender, GL_STATIC_DRAW);
    glBindVertexArray(pSnake->getVAO());
    glUseProgram(pSnake->getShader());
    
    for (int i = 0; i < snakeLen; i++){
        glDrawArrays(GL_TRIANGLES,i*5,3);
        glDrawArrays(GL_TRIANGLES,i*5 + 2,3);
    }
}

void moveSnake(Snake* pSnake,Fruit* pFruit){
    if(pSnake->getDirection() == None)
        return;
    int len = pSnake->getLen();
    for(int i = len - 1; i > 0; i--)
        pSnake->snake[i] = pSnake->snake[i-1];
         
    if(pSnake->getDirection() == Up)
        pSnake->snake[0].coords.y++;
    else if(pSnake->getDirection() == Down)
        pSnake->snake[0].coords.y--;
    else if(pSnake->getDirection() == Left)
        pSnake->snake[0].coords.x--;
    else if(pSnake->getDirection() == Right)
        pSnake->snake[0].coords.x++;
    
            
    if(pSnake->snake[0].coords == pFruit->coords){
        pFruit->isEaten = true;
        pSnake->setLen(pSnake->getLen() + 1);
        pSnake->snake[pSnake->getLen() - 1].is = true;
        pSnake->snake[pSnake->getLen() - 1].coords = pSnake->snake[pSnake->getLen() - 2].coords;
    }
    if(pSnake->snake[0].coords.x>fieldWidth - 1||
       pSnake->snake[0].coords.x<0||
       pSnake->snake[0].coords.y>fieldWidth - 1||
       pSnake->snake[0].coords.y<0)
        isOver = true;

    int w = 1;
    while(pSnake->snake[w].is){
        if (pSnake->snake[w].coords == pSnake->snake[0].coords){
            isOver = true;
            break;
        } else {
            w++;
        }
    }
}

void drawGameOver(GLuint shader, FT_Face f){
    std::string str = "GAME  OVER";
    float sx = 2.0/windowWidth;
    float sy = 2.0/windowWidth;
    //float sx = 0;
    //float sy = 0;
    float x = - 0.5;
    float y = 0;
    GLuint tex;
    GLuint vbo;
    GLuint uniform_color   = glGetUniformLocation(shader,"color"),
           uniform_tex     = glGetUniformLocation(shader,"tex"),
           attribute_coord = glGetAttribLocation(shader,"coord");
    GLfloat white[4] = {1,1,1,1};
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC1_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D,tex);
    glUniform1i(uniform_tex,0);
    glUniform4fv(uniform_color,1,white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    
    glGenBuffers(1,&vbo);
    
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glVertexAttribPointer(attribute_coord,4,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(0);
    
    
    for(char ch: str){
        if (FT_Load_Char(f,ch,FT_LOAD_RENDER))
            continue;
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            f->glyph->bitmap.width,
            f->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            f->glyph->bitmap.buffer
        );
        float x2 = x + f->glyph->bitmap_left * sx;
        float y2 = -y - f->glyph->bitmap_top * sy;
        float w = f->glyph->bitmap.rows * sx;
        float h = f->glyph->bitmap.rows * sy;
        
        GLfloat box[4][4] = {
            {x2,    -y2,     0, 0},
            {x2 + w,-y2,     1, 0},
            {x2,    -y2 - h, 0, 1},
            {x2 + w,-y2 - h, 1, 1},
            
        };
        
        glBufferData(GL_ARRAY_BUFFER,sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP,0,4);
        x += (f->glyph->advance.x/64)*sx;
        y += (f->glyph->advance.y/64)*sy;
    }
    
    glDisable(GL_BLEND);
}


