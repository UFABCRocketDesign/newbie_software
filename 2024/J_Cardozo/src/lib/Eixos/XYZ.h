#ifndef XYZ_H
#define XYZ_H

class XYZ {
protected:
    float x = 0;
    float y = 0;
    float z = 0;
public:
    virtual void lerTudo() = 0;
    float getX();
	float getY();
	float getZ();
};


#endif