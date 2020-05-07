//Code by https://github.com/MayKoder

#ifndef _MAYKMATH_H_
#define _MAYKMATH_H_

#define ABS(N) ( (N < 0) ? -(N) : (N))

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

	float DistanceManhattan(const Point& v) const
	{
		return ABS(v.x - x) + ABS(v.y - y);
	}

	bool IsZero() 
	{
		return (x == 0 && y == 0);
	}
};

struct Rect
{
	int x, y, w, h;

	Point GetCentralPoint() 
	{
		return {x + w / 2, y + h / 2};
	}
};

struct Vector4
{
	int w, x, y, z;

	Vector4() : w(0), x(0), y(0), z(255)
	{}

	Vector4(int w, int x, int y, int z = 255) : w(w), x(x), y(y), z(z)
	{}

	void SetColor(int w, int x, int y, int z = 255)
	{
		this->w = w; 
		this->x = x;
		this->y = y;
		this->z = z;
	}
};


namespace MaykMath
{
	Point NegatedYVectorFromPoints(Point, Point);
	float GetTriangleArea(Point, Point, Point);
	bool IsPointInsideOffAxisRectangle(Point, Point, Point, Point, Point);
	bool IsPointInsideAxisAlignedRectangle(Rect, Point);
	bool CheckRectCollision(const Rect&, const Rect&);

	Point GetMinPoint(Point, Point);
	Point GetMaxPoint(Point, Point);
}


#endif