
struct point{
    GLfloat x;
    GLfloat y;
    bool operator==(const point& rhs){
        if(this->x == rhs.x && this->y == rhs.y)
            return true;
        else
            return false;
    }
};

