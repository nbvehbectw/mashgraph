#pragma once

class index3
{
public:
	int v1, v2, v3;

	index3() {}
	index3(int a, int b, int c) {
		v1 = a;
		v2 = b;
		v3 = c;
	}
};

class index2
{
public:
	int v1, v2;

	index2() {}
	index2(int a, int b) {
		v1 = a;
		v2 = b;
	}
};