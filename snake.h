
struct snakeUnit{
    point coords;
    bool  is = false; // you know, "it is" means it exists L0L, | I think therefore I am.
};

enum Direction{
  None =    9,
  Up =      1,
  Right =   2,
  Down =    -1,
  Left =    -2
};

class Snake {
private:
    Direction       dir;
    unsigned int    len = 2;
    unsigned int    size = 0;
    GLuint          VBO = 0;
    GLuint          VAO = 0;
    GLuint          shader = 0;
public:
    Snake(unsigned int size, GLuint shader = 0);
    ~Snake();

    snakeUnit*      snake;
    
    void            setDirection(Direction a);
    void            setLen(unsigned int len);
    //void setVBO(GLuint vbo);
    //void setVAO(GLuint vao);
    //void setShader(GLuint shader);
    
    Direction       getDirection();
    unsigned int    getLen();
    GLuint          getVBO();
    GLuint          getVAO();
    GLuint          getShader();
    
    
};
