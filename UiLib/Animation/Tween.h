#pragma once

#include<math.h>
#include<list>
#include<vector>

using namespace std;

#pragma warning (disable : 4244)
#pragma warning (disable : 4018)

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

namespace UiLib
{
	class UILIB_API Easing 
	{
	public:
		Easing() {}
		//pure virtual
		virtual float easeIn(float t,float b , float c, float d)=0;
		virtual float easeOut(float t,float b , float c, float d)=0;
		virtual float easeInOut(float t,float b , float c, float d)=0;

	};

	class UILIB_API Back : public Easing
	{
	public:
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	class UILIB_API Bounce : public Easing
	{
	public:
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	class UILIB_API Circ : public Easing
	{
	public:
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	class UILIB_API Cubic : public Easing
	{
	public:
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	class UILIB_API Elastic : public Easing
	{
	public:
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	class UILIB_API Expo : public Easing
	{
	public:
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	class UILIB_API Quad : public Easing 
	{	
	public:
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};


	class UILIB_API Quart : public Easing 
	{
	public:
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	class UILIB_API Quint : public Easing 
	{
	public :
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	class UILIB_API Sine : public Easing 
	{
	public :
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	class UILIB_API Linear : public Easing 
	{
	public :
		float easeNone(float t,float b , float c, float d);
		float easeIn(float t,float b , float c, float d);
		float easeOut(float t,float b , float c, float d);
		float easeInOut(float t,float b , float c, float d);
	};

	enum 
	{
		LINEAR,
		SINE,
		QUINT,
		QUART,
		QUAD,
		EXPO,
		ELASTIC,
		CUBIC,
		CIRC,
		BOUNCE,
		BACK
	};

	enum 
	{
		EASE_IN,
		EASE_OUT,
		EASE_IN_OUT,
		EASE_CUSTOM,
		EASE_SPRING,
		EASE_DECAY
	};

	static Linear fLinear;
	static Sine fSine;
	static Quint fQuint;
	static Quart fQuart;
	static Quad  fQuad;
	static Expo fExpo;
	static Elastic fElastic;
	static Cubic fCubic;
	static Circ fCirc;
	static Bounce fBounce;
	static Back fBack;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////
	//配合贝塞尔曲线获取自己想要的缓动方式
	//http://isux.tencent.com/css3/tools.html

	struct UnitBezier
	{
		UnitBezier(double p1x, double p1y, double p2x, double p2y)
		{
			// Calculate the polynomial coefficients, implicit first and last control points are (0,0) and (1,1).
			cx = 3.0 * p1x;
			bx = 3.0 * (p2x - p1x) - cx;
			ax = 1.0 - cx -bx;

			cy = 3.0 * p1y;
			by = 3.0 * (p2y - p1y) - cy;
			ay = 1.0 - cy - by;
		}

		double sampleCurveX(double t)
		{
			// `ax t^3 + bx t^2 + cx t' expanded using Horner's rule.
			return ((ax * t + bx) * t + cx) * t;
		}

		double sampleCurveY(double t)
		{
			return ((ay * t + by) * t + cy) * t;
		}

		double sampleCurveDerivativeX(double t)
		{
			return (3.0 * ax * t + 2.0 * bx) * t + cx;
		}

		// Given an x value, find a parametric value it came from.
		double solveCurveX(double x, double epsilon)
		{
			double t0;
			double t1;
			double t2;
			double x2;
			double d2;
			int i;

			// First try a few iterations of Newton's method -- normally very fast.
			for (t2 = x, i = 0; i < 8; i++) {
				x2 = sampleCurveX(t2) - x;
				if (fabs (x2) < epsilon)
					return t2;
				d2 = sampleCurveDerivativeX(t2);
				if (fabs(d2) < 1e-6)
					break;
				t2 = t2 - x2 / d2;
			}

			// Fall back to the bisection method for reliability.
			t0 = 0.0;
			t1 = 1.0;
			t2 = x;

			if (t2 < t0)
				return t0;
			if (t2 > t1)
				return t1;

			while (t0 < t1) {
				x2 = sampleCurveX(t2);
				if (fabs(x2 - x) < epsilon)
					return t2;
				if (x > x2)
					t0 = t2;
				else
					t1 = t2;
				t2 = (t1 - t0) * .5 + t0;
			}

			// Failure.
			return t2;
		}

		float solve(float x, float epsilon)
		{
			return sampleCurveY(solveCurveX(x, epsilon));
		}

	private:
		double ax;
		double bx;
		double cx;

		double ay;
		double by;
		double cy;
	};


	// The epsilon value we pass to UnitBezier::solve given that the animation is going to run over |dur| seconds. The longer the
	// animation, the more precision we need in the timing function result to avoid ugly discontinuities.
	static inline float solveEpsilon(float duration)
	{
		return 1.0 / (200.0 * duration);
	}

	static inline float solveCubicBezierFunction(float p1x, float p1y, float p2x, float p2y, float t, float duration)
	{
		// Convert from input time to parametric value in curve, then from
		// that to output time.
		UnitBezier bezier(p1x, p1y, p2x, p2y);
		return bezier.solve(t, solveEpsilon(duration));
	}


#if 0
	double dFrom = 0.0;
	double dTo = 100.0;
	double duration = 1000;

	int pfs = 30;

	int nTime = 0;
	int nTimeStart = GetTickCount();
	double timepercent = 0;

	double dprev = 0;
	while (1)
	{
		nTime = GetTickCount() - nTimeStart;
		if (nTime > duration)
		{
			cout << dTo << endl;
			break;
		}

		timepercent = nTime / duration;
		double dPercent = solveCubicBezierFunction(0.68, -0.55, 0.265, 1.55, timepercent, duration);
		double dCur = dFrom + (dTo-dFrom)*dPercent;

		cout << dCur << "   (" << (dCur-dprev) << ")" << endl;
		dprev = dCur;
		Sleep(pfs);
	}
#endif


	
}