#pragma once

#include <iostream>
#include <ctime>
#include <math.h>
#include "longnums.h"
#include "discrete_log.h"

using namespace std;

void cmp_one(int num, int equal, int mod, int*& x, int& n);
void factorize(long num, long *& a, long& len);
__int64 deg(__int64 num, __int64 deg, __int64 mod);


// НОД с коэфф-тами Безу. Используется расширенный алгоритм Евклида
template<class T>
T gcdBezout(T a, T b, T& bez1, T& bez2)
{
	if (b == 0)
	{
		bez1 = T(1), bez2 = T(0);
		return a;
	}
	else if (a == 0)
	{
		bez1 = T(0), bez2 = T(1);
		return b;
	}
	bool rot = false;
	T num1(a);
	T num2(b);
	T tmp;
	T x = T(0);
	T y = T(1);
	T lastx = T(1);
	T lasty = T(0);
	T q;
	if (!(a >= b))
	{
		tmp = num1;
		num1 = num2;
		num2 = tmp;
		rot = true;
	}

	while (num2 != 0)
	{
		q = num1 / num2;
		tmp = num1;
		num1 = num2;
		num2 = tmp % num2;
		tmp = x;
		x = lastx - q * x;
		lastx = tmp;
		tmp = y;
		y = lasty - q * y;
		lasty = tmp;
	}
	if (rot)
	{
		bez1 = lasty;
		bez2 = lastx;
	}
	else {
		bez1 = lastx;
		bez2 = lasty;
	}
	return num1;
}

// Обычный НОД. Простой алгоритм Евклида
template<class T>
T gcd(T& a, T& b)
{
	if (a == 0)
		return b;
	else if (b == 0)
		return a;
	T num1(a);
	T num2(b);
	T tmp;
	if (!(a >= b))
	{
		tmp = num1;
		num1 = num2;
		num2 = tmp;
	}
	while (num2 != 0)
	{
		tmp = num1;
		num1 = num2;
		num2 = tmp % num2;
	}
	return num1;
}

template<class T>
T CRT(T* ri, T* mi, const T& n)
{
	if (ri == nullptr || mi == nullptr || n <= 0)
	{
		std::cout << "Illegal params";
		return T(0);
	}
	T m = mi[0];
	for (int i = 1; i < n; i++)
		m *= mi[i];
	T b1, b2;
	gcdBezout<T>(m / mi[0], mi[0], b1, b2);
	T res = ri[0] * (m / mi[0]) * b1;
	for (int i = 1; i < n; i++)
	{
		gcdBezout<T>(m / mi[i], mi[i], b1, b2);
		res += ri[i] * (m / mi[i]) * b1;
	}
	res %= m;
	if (res < 0)
		res += m;
	return res;
}

// Обратный элемент в кольце вычетов по простому модулю n
template<class T>
T opposite(const T& a, const T& n)
{
	T b, z;
	T tmp = gcdBezout(n, a, b, z);
	while (z < 0) z += n;
	return z;
}

template<class T>
// решение СЛАУ по простому модулю методом Гаусса-Жордана
void solve(T *matr, const T& m, const T& n, T *res, const T& mod)
{
	int i = 0;
	int j = 0;
	T opp;
	T tmp;
	for (j = 0; j < n - 1; ++j)
	{
		i = 0;
		while (matr[i * n + j] == 0 && i < m) i++;
		if (i == m)
		{
			continue;
		}
		opp = opposite(matr[i * n + j], mod);
		for (int k = j; k < n; k++)
			matr[i * n + k] = (matr[i * n + k] * opp) % mod;
		for (int z = 0; z < m; z++)
		{
			if (z == i) continue;
			tmp = matr[z * n + j];
			if (tmp != 0)
				for (int k = j; k < n; k++)
				{
					matr[z * n + k] -= ((tmp * matr[i * n + k]) % mod);
					while (matr[z * n + k] < 0)
						matr[z * n + k] += mod;
				}
		}
	}
	bool flag;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0, flag = true; j < n - 1 && flag; j++)
		{
			if (matr[i * n + j])
			{
				res[j] = matr[i * n + (n - 1)];
				flag = false;
			}
		}
	}
}

// Функция f метода Полларда. Вычисляет следующий элемент последовательности
template <class T>
T f(T& x, T& n)
{
	return (((x * x) % n + 1) % n);
}

template <class T>
T rho_pollard(const T& n)
{
	T x(random(T(0), n));
	T z(f(f(x, n), n));
	T tmp;
	T d;
	while (true)
	{
		tmp = abs(z - x);
		d = gcd(n, tmp);
		if (d != 1 && d != n)
			cout << d << " ";
		if (d != 1)
			break;
		x = f(x, n);
		z = f(f(z, n), n);
	}
	return d;
}

// Ро-метод для длинных чисел
Longnum rho_pollard(Longnum& n, uint thread_num);