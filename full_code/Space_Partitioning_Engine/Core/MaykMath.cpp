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

#include"MaykMath.h"

float MaykMath::GetTriangleArea(Point p1, Point p2, Point p3)
{
	return ABS((p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) / 2.0);
}

Point MaykMath::NegatedYVectorFromPoints(Point p1, Point p2) 
{
	return {(p2.x - p1.x), (-1 * (p2.y - p1.y))};
}

bool MaykMath::IsPointInsideOffAxisRectangle(Point B, Point A, Point C, Point D, Point m)
{
	//Rect vectors
	Point AB = NegatedYVectorFromPoints(A, B); 
	Point AD = NegatedYVectorFromPoints(A, D); 
	Point BC = NegatedYVectorFromPoints(B, C); 
	Point CD = NegatedYVectorFromPoints(C, D);  

	//Matrix determinant to detect where the point is in reference to the vector
	float detAB = (AB.y * m.x + AB.x * m.y) - (AB.y * A.x + AB.x * A.y);
	float detAD = (AD.y * m.x + AD.x * m.y) - (AD.y * A.x + AD.x * A.y);
	float detBC = (BC.y * m.x + BC.x * m.y) - (BC.y * B.x + BC.x * B.y);
	float detCD = (CD.y * m.x + CD.x * m.y) - (CD.y * C.x + CD.x * C.y);

	//Is the point inside the rect?
	return 0 > detAB && 0 > detCD && 0 < detAD && 0 > detBC;
}

bool MaykMath::IsPointInsideAxisAlignedRectangle(Rect r, Point p)
{
	if ((p.x >= r.x && p.x <= r.x + r.w) && (p.y >= r.y && p.y <= r.y + r.h))
		return true;
	return false;
}

bool MaykMath::CheckRectCollision(const Rect& rect, const Rect& r)
{
	bool detectedX = true;
	bool detectedY = true;

	if ((rect.x + rect.w) < r.x || (r.x + r.w) < rect.x)
		detectedX = false;

	if ((rect.y + rect.h) < r.y || (r.y + r.h) < rect.y)
		detectedY = false;

	return detectedX && detectedY;
}

Point MaykMath::GetMinPoint(Point A, Point B)
{
	Point temp = {0, 0};

	temp.x = (A.x < B.x) ? A.x : B.x;
	temp.y = (A.y < B.y) ? A.y : B.y;

	return temp;
}

Point MaykMath::GetMaxPoint(Point A, Point B)
{
	Point temp = { 0, 0 };

	temp.x = (A.x > B.x) ? A.x : B.x;
	temp.y = (A.y > B.y) ? A.y : B.y;

	return temp;
}
