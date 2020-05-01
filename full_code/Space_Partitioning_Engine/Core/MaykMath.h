#ifndef _MAYKMATH_H_
#define _MAYKMATH_H_

#define ABS(N) ((N<0)?(-N):(N))

enum TreeType
{
	ORTHOGRAPHIC,
	ISOMETRIC
};

struct Point
{
	int x = 0, y = 0;

	float operator*(Point const &b) 
	{
		return (x * b.x) + (y * b.y);
	}

};

struct Rect
{
	int x, y, w, h;
};

Point vect2d(Point p1, Point p2);

float GetTriangleArea(Point, Point, Point);
bool IsPointInsideOffAxisRectangle(Point, Point, Point, Point, Point);
bool IsPointInsideAxisAlignedRectangle(Rect, Point);



#endif