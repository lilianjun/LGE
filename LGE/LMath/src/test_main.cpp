#include "lvector2.h"
#include "lvector3.h"
#include "lvector4.h"

#include <iostream>
#include <vector>

#include <d3d11.h>
#include "SimpleMath.h"

using namespace std;
using namespace DirectX::SimpleMath;

using flt2 = lvector2<float>;
using flt3 = lvector3<float>;
using flt4 = lvector4<float>;

ostream& operator<<(ostream& os, const flt2& f2)
{
	return os << "(" << f2.x << "," << f2.y << ")";
}

ostream& operator<<(ostream& os, const flt3& f3)
{
	return os << "(" << f3.x << "," << f3.y << ","<< f3.z << ")";
}

ostream& operator<<(ostream& os, const flt4& f4)
{
	return os << "(" << f4.x << "," << f4.y << "," << f4.z << "," << f4.w << ")";
}

bool operator==(const flt2& lf2, const Vector2& df2)
{
	return lequal(lf2.x, df2.x) && lequal(lf2.y, df2.y);
}

bool operator==(const flt3& lf2, const Vector3& df2)
{
	return lequal(lf2.x, df2.x) && lequal(lf2.y, df2.y) && lequal(lf2.z, df2.z);
}

float randf(float f_min = 0.f, float f_max = 1.f)
{
	//float r = f_max - f_min;
	// 取得这个范围的数
	return fmodf((float)rand()/1.11111f, f_max - f_min) + f_min;
	// 扩展到f_min 到f_max
	//return f1;
}

#define LTEST(e) if(!(e)) cout<<#e<<endl;

void test_flt2()
{

	flt2 v1(randf(), randf());
	flt2 v2(randf(), randf());
	Vector2 dv1(v1.x, v1.y);
	Vector2 dv2(v2.x, v2.y);

	float flerp = randf();
	LTEST(v1 + v2 == dv1 + dv2);
	LTEST(v1 - v2 == dv1 - dv2);
	LTEST(v1 * v2 == dv1 * dv2);
	LTEST(v1 / v2 == dv1 / dv2);
	LTEST(v1.len() == dv1.Length());
	LTEST(v1.distance(v2) == Vector2::Distance(dv1, dv2));
	Vector2 dvor = dv1;
	dvor.Normalize();
	LTEST(v1.getnor() == dvor);
	LTEST(v1.lerp(v2, flerp) == Vector2::Lerp(dv1, dv2, flerp));
	LTEST(v1.dot(v2) == dv1.Dot(dv2));
}

void test_flt3()
{

	flt3 v1(randf(), randf(), randf());
	flt3 v2(randf(), randf(), randf());
	Vector3 dv1(v1.x, v1.y, v1.z);
	Vector3 dv2(v2.x, v2.y, v2.z);

	float flerp = randf();
	LTEST(v1 + v2 == dv1 + dv2);
	LTEST(v1 - v2 == dv1 - dv2);
	LTEST(v1 * v2 == dv1 * dv2);
	LTEST(v1 / v2 == dv1 / dv2);
	LTEST(v1.len() == dv1.Length());
	LTEST(v1.distance(v2) == Vector3::Distance(dv1, dv2));
	Vector3 dvor = dv1;
	dvor.Normalize();
	LTEST(v1.getnor() == dvor);
	LTEST(v1.lerp(v2, flerp) == Vector3::Lerp(dv1, dv2, flerp));
	//LTEST(v1.dot(v2) == dv1.Dot(dv2));
	//LTEST(v1.cross(v2) == dv1.Cross(dv2));
}

int main()
{
	for (int i = 0; i < 10; i++)
	{
		test_flt2();
		test_flt3();
	}
	system("pause");
}