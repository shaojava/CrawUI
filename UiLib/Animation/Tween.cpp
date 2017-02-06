#include "StdAfx.h"
#include "Tween.h"

namespace UiLib
{
	/***** LINEAR ****/
	float Linear::easeNone (float t,float b , float c, float d) 
	{
		return c*t/d + b;
	}

	float Linear::easeIn (float t,float b , float c, float d) 
	{
		return c*t/d + b;
	}

	float Linear::easeOut(float t,float b , float c, float d) 
	{
		return c*t/d + b;
	}

	float Linear::easeInOut(float t,float b , float c, float d) 
	{
		return c*t/d + b;
	}

	/***** SINE ****/

	float Sine::easeIn (float t,float b , float c, float d) 
	{
		return -c * cos(t/d * (PI/2)) + c + b;
	}
	float Sine::easeOut(float t,float b , float c, float d) 
	{
		return c * sin(t/d * (PI/2)) + b;
	}

	float Sine::easeInOut(float t,float b , float c, float d) 
	{
		return -c/2 * (cos(PI*t/d) - 1) + b;
	}

	/**** Quint ****/

	float Quint::easeIn (float t,float b , float c, float d) 
	{
		return c*(t/=d)*t*t*t*t + b;
	}
	float Quint::easeOut(float t,float b , float c, float d) 
	{
		return c*((t=t/d-1)*t*t*t*t + 1) + b;
	}

	float Quint::easeInOut(float t,float b , float c, float d) 
	{
		if ((t/=d/2) < 1) return c/2*t*t*t*t*t + b;
		return c/2*((t-=2)*t*t*t*t + 2) + b;
	}

	/**** Quart ****/
	float Quart::easeIn (float t,float b , float c, float d) 
	{
		return c*(t/=d)*t*t*t + b;
	}
	float Quart::easeOut(float t,float b , float c, float d) 
	{
		return -c * ((t=t/d-1)*t*t*t - 1) + b;
	}

	float Quart::easeInOut(float t,float b , float c, float d) 
	{
		if ((t/=d/2) < 1) return c/2*t*t*t*t + b;
		return -c/2 * ((t-=2)*t*t*t - 2) + b;
	}

	/**** Quad ****/
	float Quad::easeIn (float t,float b , float c, float d)
	{
		return c*(t/=d)*t + b;
	}
	float Quad::easeOut(float t,float b , float c, float d) 
	{
		return -c *(t/=d)*(t-2) + b;
	}

	float Quad::easeInOut(float t,float b , float c, float d) 
	{
		if ((t/=d/2) < 1) return ((c/2)*(t*t)) + b;
		return -c/2 * (((t-2)*(--t)) - 1) + b;
	}

	/**** Expo ****/
	float Expo::easeIn (float t,float b , float c, float d) 
	{
		return (t==0) ? b : c * pow(2, 10 * (t/d - 1)) + b;
	}

	float Expo::easeOut(float t,float b , float c, float d) 
	{
		return (t==d) ? b+c : c * (-pow(2, -10 * t/d) + 1) + b;
	}

	float Expo::easeInOut(float t,float b , float c, float d) 
	{
		if (t==0) return b;
		if (t==d) return b+c;
		if ((t/=d/2) < 1) return c/2 * pow(2, 10 * (t - 1)) + b;
		return c/2 * (-pow(2, -10 * --t) + 2) + b;
	}


	/****  Elastic ****/

	float Elastic::easeIn (float t,float b , float c, float d) 
	{
		if (t==0) return b;  if ((t/=d)==1) return b+c;
		float p=d*.3f;
		float a=c;
		float s=p/4;
		float postFix =a*pow(2,10*(t-=1));
		return -(postFix * sin((t*d-s)*(2*PI)/p )) + b;
	}

	float Elastic::easeOut(float t,float b , float c, float d) 
	{
		if (t==0) return b;  if ((t/=d)==1) return b+c;
		float p=d*.3f;
		float a=c;
		float s=p/4;
		return (a*pow(2,-10*t) * sin( (t*d-s)*(2*PI)/p ) + c + b);
	}


	float Elastic::easeInOut(float t,float b , float c, float d) 
	{
		if (t==0) return b;  if ((t/=d/2)==2) return b+c;
		float p=d*(.3f*1.5f);
		float a=c;
		float s=p/4;

		if (t < 1) {
			float postFix =a*pow(2,10*(t-=1)); 
			return -.5f*(postFix* sin( (t*d-s)*(2*PI)/p )) + b;
		}
		float postFix =  a*pow(2,-10*(t-=1)); 
		return postFix * sin( (t*d-s)*(2*PI)/p )*.5f + c + b;
	}

	/****  Cubic ****/
	float Cubic::easeIn (float t,float b , float c, float d) 
	{
		return c*(t/=d)*t*t + b;
	}
	float Cubic::easeOut(float t,float b , float c, float d) 
	{
		return c*((t=t/d-1)*t*t + 1) + b;
	}

	float Cubic::easeInOut(float t,float b , float c, float d) 
	{
		if ((t/=d/2) < 1) return c/2*t*t*t + b;
		return c/2*((t-=2)*t*t + 2) + b;
	}

	/*** Circ ***/

	float Circ::easeIn (float t,float b , float c, float d) 
	{
		return -c * (sqrt(1 - (t/=d)*t) - 1) + b;
	}
	float Circ::easeOut(float t,float b , float c, float d) 
	{
		return c * sqrt(1 - (t=t/d-1)*t) + b;
	}

	float Circ::easeInOut(float t,float b , float c, float d) 
	{
		if ((t/=d/2) < 1) return -c/2 * (sqrt(1 - t*t) - 1) + b;
		return c/2 * (sqrt(1 - t*(t-=2)) + 1) + b;
	}

	/****  Bounce ****/
	float Bounce::easeIn (float t,float b , float c, float d) 
	{
		return c - easeOut (d-t, 0, c, d) + b;
	}

	float Bounce::easeOut(float t,float b , float c, float d) 
	{
		if ((t/=d) < (1/2.75f)) 
		{
			return c*(7.5625f*t*t) + b;
		} 
		else if (t < (2/2.75f)) 
		{
			float postFix = t-=(1.5f/2.75f);
			return c*(7.5625f*(postFix)*t + .75f) + b;
		} 
		else if (t < (2.5/2.75)) 
		{
			float postFix = t-=(2.25f/2.75f);
			return c*(7.5625f*(postFix)*t + .9375f) + b;
		} 
		else 
		{
			float postFix = t-=(2.625f/2.75f);
			return c*(7.5625f*(postFix)*t + .984375f) + b;
		}
	}

	float Bounce::easeInOut(float t,float b , float c, float d)
	{
		if (t < d/2) return easeIn (t*2, 0, c, d) * .5f + b;
		else return easeOut (t*2-d, 0, c, d) * .5f + c*.5f + b;
	}

	/**** Back *****/

	float Back::easeIn (float t,float b , float c, float d) 
	{
		float s = 1.70158f;
		float postFix = t/=d;
		return c*(postFix)*t*((s+1)*t - s) + b;
	}
	float Back::easeOut(float t,float b , float c, float d) 
	{
		float s = 1.70158f;
		return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
	}

	float Back::easeInOut(float t,float b , float c, float d) 
	{
		float s = 1.70158f;
		if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525f))+1)*t - s)) + b;
		float postFix = t-=2;
		return c/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2) + b;
	}
}