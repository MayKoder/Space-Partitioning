#include"MaykMath.h"

float MaykMath::GetTriangleArea(Point p1, Point p2, Point p3)
{
	return ABS((p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) / 2.0);
}

Point MaykMath::NegatedYVector(Point p1, Point p2) 
{
	return {(p2.x - p1.x), (-1 * (p2.y - p1.y))};
}

bool MaykMath::IsPointInsideOffAxisRectangle(Point B, Point A, Point C, Point D, Point m)
{
	//Rect vectors
	Point AB = NegatedYVector(A, B); 
	Point AD = NegatedYVector(A, D); 
	Point BC = NegatedYVector(B, C); 
	Point CD = NegatedYVector(C, D);  

	//Determinant to detect in which side the point is of the vector
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
