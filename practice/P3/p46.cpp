#include <iostream>
using namespace std;

class Vector2D
{
public:
    Vector2D ():_x(0),_y(0){};
    Vector2D (int x, int y):_x(x),_y(y){};
    Vector2D& operator + (const Vector2D& entry)const;
    Vector2D& operator - (const Vector2D& entry)const; 
    Vector2D& operator += (const Vector2D& entry);
    Vector2D& operator -= (const Vector2D& entry);
    Vector2D operator -- (int);
    Vector2D operator ++ (int);
    Vector2D& operator ++ ();
    Vector2D& operator -- ();
    int operator * (const Vector2D& entry)const;
    void print() const{
        cout << "("<< _x << " , " << _y << ")" << endl;
    }
private:
    int _x;
    int _y;
};

int main(){
    Vector2D a(1,2);
    Vector2D b(2,5);
    Vector2D c;
    a.print();b.print();c.print();
    a++;b+=c;--c;
    a.print();b.print();c.print();
    c = a+b;
    (++a).print();
    c.print();
}



Vector2D& Vector2D::operator + (const Vector2D& entry)const{
    Vector2D* v = new Vector2D;
    v->_x = this->_x + entry._x;
    v->_y = this->_y + entry._y;
    return *v;
} 

Vector2D& Vector2D::operator - (const Vector2D& entry)const{
    Vector2D* v = new Vector2D;
    v->_x = this->_x - entry._x;
    v->_y = this->_y - entry._y;
    return *v;
} 

Vector2D& Vector2D::operator += (const Vector2D& entry){
    this->_x += entry._x;
    this->_y += entry._y;
    return *this;
} 

Vector2D& Vector2D::operator -= (const Vector2D& entry){
    this->_x -= entry._x;
    this->_y -= entry._y;
    return *this;
}

Vector2D Vector2D::operator ++ (int){
    Vector2D tmp = *this;
    ++(*this);
    return tmp;
}

Vector2D Vector2D::operator -- (int){
    Vector2D tmp = *this;
    --(*this);
    return tmp;
}

int Vector2D::operator * (const Vector2D& entry) const{
    return (this->_x*entry._x + this->_y*entry._y);
}

Vector2D& Vector2D::operator ++ (){
    ++this->_x;
    ++this->_y;
    return *this;
}

Vector2D& Vector2D::operator -- (){
    --this->_x;
    --this->_y;
    return *this;
}



























