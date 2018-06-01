
class Fruit{
private:
    GLuint  VBO = 0;
    GLuint  VAO = 0;
    GLuint  shader = 0;
public:
    Fruit(GLuint shader = 0);
    ~Fruit();
    point   coords;
    bool    isEaten = true;
    GLuint  getVBO();
    GLuint  getVAO();
    GLuint  getShader();
        
};
