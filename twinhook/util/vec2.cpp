#include "stdafx.h"
#include "vec2.h"

vec2::vec2() : x(0), y(0)
{
}

vec2::vec2(float x, float y) : x(x), y(y)
{
}


vec2::vec2(const vec2& other)
	: x(other.x),
	y(other.y)
{
}

vec2::vec2(vec2&& other) noexcept
	: x(other.x),
	y(other.y)
{
}

vec2& vec2::operator=(const vec2& other)
{
	if (this == &other)
		return *this;
	x = other.x;
	y = other.y;
	return *this;
}

vec2& vec2::operator=(vec2&& other) noexcept
{
	if (this == &other)
		return *this;
	x = other.x;
	y = other.y;
	return *this;
}

bool vec2::operator==(const vec2& o) const
{
	return x == o.x && y == o.y;
}

bool vec2::operator!=(const vec2& o) const
{
	return !(*this == o);
}

vec2 operator+(const vec2& a, const vec2& b)
{
	return vec2(a.x + b.x, a.y + b.y);
}

vec2 operator-(const vec2& a, const vec2& b)
{
	return vec2(a.x - b.x, a.y - b.y);
}

vec2 operator*(const vec2& a, float o)
{
	return vec2(a.x * o, a.y * o);
}

vec2 operator/(const vec2& a, float o)
{
	return vec2(a.x / o, a.y / o);
}

vec2 operator*(float o, const vec2& a)
{
	return operator*(a, o);
}

vec2 operator/(float o, const vec2& a)
{
	return operator/(a, o);
}

void vec2::operator+=(const vec2& o)
{
	x += o.x;
	y += o.y;
}

void vec2::operator-=(const vec2& o)
{
	x -= o.x;
	y -= o.y;
}

void vec2::operator*=(float o)
{
	x *= o;
	y *= o;
}

void vec2::operator/=(float o)
{
	x /= o;
	y /= o;
}

void vec2::set(const vec2& o)
{
	x = o.x;
	y = o.y;
}

vec2 vec2::normal() const
{
	return vec2(-y, x);
}

void vec2::normalize()
{
	float l = len();
	x /= l;
	y /= l;
}

vec2 vec2::unit() const
{
	vec2 c(*this);
	c.normalize();
	return c;
}

float vec2::lensq() const
{
	return x * x + y * y;
}

float vec2::len() const
{
	return sqrt(lensq());
}

bool vec2::zero() const
{
	return abs(x) < ZERO_EPSILON && abs(y) < ZERO_EPSILON;
}

bool vec2::nan() const
{
	return isnan(x) || isnan(y);
}

vec2 vec2::rotate(float rad) const
{
	return vec2(
		x * cos(rad) - y * sin(rad),
		x * sin(rad) + y * cos(rad)
	);
}

vec2 vec2::transform(float(*t)(float)) const
{
	return vec2(t(x), t(y));
}

void vec2::aabbVert(const vec2& p, const vec2& s, std::vector<vec2>& vertices)
{
	vertices.push_back(p);
	vertices.push_back(vec2(p.x + s.x, p.y));
	vertices.push_back(vec2(p.x, p.y + s.y));
	vertices.push_back(vec2(p.x + s.x, p.y + s.y));
}

float vec2::dot(const vec2& a, const vec2& b)
{
	return a.x * b.x + a.y * b.y;
}

vec2 vec2::proj(const vec2& a, const vec2& b)
{
	return b * (dot(a, b) / b.lensq());
}

vec2 vec2::perp(const vec2& a, const vec2& b)
{
	return a - proj(a, b);
}

vec2 vec2::minv(const vec2& a, const vec2& b)
{
	return vec2(std::min(a.x, b.x), std::min(a.y, b.y));
}

vec2 vec2::minv(const std::vector<vec2>& vs)
{
	vec2 minvt(FLT_MAX, FLT_MAX);
	for(vec2 v : vs)
		minvt = minv(minvt, v);
	return minvt;
}

vec2 vec2::maxv(const vec2& a, const vec2& b)
{
	return vec2(std::max(a.x, b.x), std::max(a.y, b.y));
}

vec2 vec2::maxv(const std::vector<vec2>& vs)
{
	vec2 maxvt(FLT_MIN, FLT_MIN);
	for (vec2 v : vs)
		maxvt = maxv(maxvt, v);
	return maxvt;
}

bool vec2::inAABB(const vec2 &p, const vec2 &a, const vec2 &b)
{
	vec2 topleft = minv(a, b);
	vec2 botright = maxv(a, b);
	return p.x >= topleft.x && p.x <= botright.x && p.y >= topleft.y && p.y <= botright.y;
}

bool vec2::isCollideAABB(const vec2 &p1, const vec2 &p2, const vec2 &s1, const vec2 &s2)
{
	return p1.x <= p2.x + s2.x
		&& p1.x + s1.x >= p2.x
		&& p1.y <= p2.y + s2.y
		&& s1.y + p1.y >= p2.y;
}

bool vec2::isContainAABB(const vec2& p1, const vec2& p2, const vec2& s1, const vec2& s2)
{
	return p2.x >= p1.x
		&& p2.x + s2.x <= p1.x + s1.x
		&& p2.y >= p1.y
		&& p2.y + s2.y <= p1.y + s1.y;
}

float vec2::willCollideAABB(const vec2 &p1, const vec2 &p2, const vec2 &s1, const vec2 &s2,
	const vec2 &v1, const vec2 &v2)
{
	// BUG this implementation is way too inefficient

	// check if they're already colliding
	if (isCollideAABB(p1, p2, s1, s2))
		return 0;

	// check if they're moving away from each other
	// actually, this code doesn't work
	/*if (p1.x < p2.x && v2.x > v1.x || p1.x > p2.x && v2.x < v1.x)
		return -1;
	if (p1.y < p2.y && v2.y > v1.y || p1.y > p2.y && v2.y < v1.y)
		return -1;*/

		// check time required until collision for each side
	float t = (p1.x - p2.x - s2.x) / (v2.x - v1.x);
	float minE = FLT_MAX;
	if (t >= 0 && isCollideAABB(p1 + t * v1, p2 + t * v2, s1, s2))
		minE = std::min(minE, t);
	t = (p1.x - p2.x + s1.x) / (v2.x - v1.x);
	if (t >= 0 && isCollideAABB(p1 + t * v1, p2 + t * v2, s1, s2))
		minE = std::min(minE, t);
	t = (p1.y - p2.y - s2.y) / (v2.y - v1.y);
	if (t >= 0 && isCollideAABB(p1 + t * v1, p2 + t * v2, s1, s2))
		minE = std::min(minE, t);
	t = (p1.y - p2.y + s1.y) / (v2.y - v1.y);
	if (t >= 0 && isCollideAABB(p1 + t * v1, p2 + t * v2, s1, s2))
		minE = std::min(minE, t);

	// check if finite collision time exists
	if (minE != FLT_MAX && minE < 6000 /* imposed limit of 100 seconds */)
		return minE;
	return -1;
}

float vec2::willExitAABB(const vec2& p1, const vec2& p2, const vec2& s1, const vec2& s2, const vec2& v1, const vec2& v2)
{
	// check if they're already exited
	if (!isContainAABB(p1, p2, s1, s2))
		return 0;

	// check time required for each side to exit

	float minE = FLT_MAX;
	float t = (p1.x + s1.x - p2.x - s2.x) / (v2.x - v1.x);
	if (t >= 0)
		minE = std::min(minE, t);
	t = (p1.x - p2.x) / (v2.x - v1.x);
	if (t >= 0)
		minE = std::min(minE, t);
	t = (p1.y - p2.y) / (v2.y - v1.y);
	if (t >= 0)
		minE = std::min(minE, t);
	t = (p1.y + s1.y - p2.y - s2.y) / (v2.y - v1.y);
	if (t >= 0)
		minE = std::min(minE, t);

	if (minE != FLT_MAX && minE < 6000 /* imposed limit of 100 seconds */)
		return minE;

	return -1;
}

bool vec2::isCollideCircle(const vec2& p1, const vec2& p2, float r1, float r2)
{
	return (p2 - p1).lensq() <= (r1 + r2) * (r1 + r2);
}

float vec2::willCollideCircle(const vec2& p1, const vec2& p2, float r1, float r2,
	const vec2 &v1, const vec2 &v2)
{
	if (isCollideCircle(p1, p2, r1, r2))
		return 0;

	float a = (v2 - v1).lensq();
	float b = 2 * dot(p2 - p1, v2 - v1);
	float c = (p2 - p1).lensq() - (r1 + r2) * (r1 + r2);

	float disc = b * b - 4 * a*c;
	if (disc < 0)
	{
		return -1;
	}

	// WARNING: there is no imposed limit on x1, x2
	float x1, x2;
	int rts = quadraticSolve(a, b, c, x1, x2);
	if (rts == 0)
		return -1;
	if (rts == 1)
		return x1;
	return std::min(x1, x2);
}

float vec2::willCollideCircleLine(const vec2& ct, const vec2& v, float r, 
									const vec2& p1, const vec2& p2)
{
	// TODO this is not correct
	float a = p1.y - p2.y;
	float b = p2.x - p1.x;
	float c = (p1.x - p2.x)*p1.y + (p2.y - p1.y)*p1.x;

	float bn = a * a + 2 * a*b + b * b;
	float d = sqrt(
		a*a*a*a*r*r + 2 * a*a*a*b*r*r + 2 * a*a*b*b*r*r + 2 * a*b*b*b*r*r + b * b*b*b*r*r);
	float sh = a * b*ct.x - a * b*ct.y - a * c - b * b*ct.y - b * c;

	float t1 = (-a * a*ct.x - d - sh) / bn;
	float t2 = (-a * a*ct.x + d - sh) / bn;

	return std::min(t1, t2);
}

vec2 vec2::closestPointOnCircle(const vec2& ct, float r, const vec2& o)
{
	return ct + r * (o - ct).unit();
}

int vec2::quadraticSolve(float a, float b, float c, float& x1, float& x2)
{
	x1 = NAN;
	x2 = NAN;
	if (a == 0)
	{
		if (b == 0)
			return 0;
		x1 = -c / b;
		return 1;
	}
	float d = b * b - 4 * a * c;
	if (d < 0)
	{
		return 0;
	}
	if (d == 0)
	{
		x1 = -b / (2 * a);
		return 1;
	}
	float rtd = sqrt(d);
	x1 = (-b - rtd) / (2 * a);
	x2 = (-b + rtd) / (2 * a);
	return 2;
}

bool vec2::isCollideConvexPolygon(const std::vector<vec2>& a, const std::vector<vec2>& b)
{
	std::vector<vec2> normals;
	int sizeA = a.size();
	int sizeB = b.size();

	// calculate normals
	for(int i = 0; i < sizeA; ++i)
		normals.push_back((a[(i + sizeA + 1) % sizeA] - a[i]).normal());
	for (int i = 0; i < sizeB; ++i)
		normals.push_back((b[(i + sizeB + 1) % sizeB] - b[i]).normal());

	// check for separating axis
	for(vec2 n : normals)
	{
		float minProjA = FLT_MAX, maxProjA = FLT_MIN;
		float minProjB = FLT_MAX, maxProjB = FLT_MIN;

		// determine extents of projections onto axis
		for(vec2 pa : a)
		{
			float pj = dot(pa, n);
			minProjA = std::min(minProjA, pj);
			maxProjA = std::max(maxProjA, pj);
		}
		for(vec2 pb : b)
		{
			float pj = dot(pb, n);
			minProjB = std::min(minProjB, pj);
			maxProjB = std::max(maxProjB, pj);
		}

		// determine if projection extents overlap
		if (minProjB < minProjA && maxProjB < minProjA 
		 || minProjB > maxProjA && maxProjB > maxProjA)
			return false;
	}
	return true;
}
