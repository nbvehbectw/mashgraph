#pragma once

float clamp(float t, float min, float max)  //����������~
{
	if (t < min) {
		return min;
	}
	if (t > max) {
		return max;
	}
	return t;
}