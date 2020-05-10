//Code by https://github.com/MayKoder

//Copyright(c) 2020 MayKoder

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

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
	int x = 0, y = 0, w = 0, h = 0;

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