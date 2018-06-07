#pragma once

#include <cmath>

float sqr(float x)
{
	return x * x;
}

float gauss(float x)
{
	return expf(-sqr(x));
}

float gauss(float x, float y)
{
	return expf(-sqr(x) - sqr(y));
}