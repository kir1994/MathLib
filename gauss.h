#pragma once

using namespace std;

template<class T>
class Gauss
{
public:
  T a;
  T b;
  explicit Gauss()
  {
    a = 0, b = 0;
  }
  explicit Gauss(const T& a, const T& b)
  {
    this->a = a, this->b = b;
  }
  explicit Gauss(const T& a)
  {
    this->a = a, this->b = 0;
  }
  explicit Gauss(T&& a, T&& b) : this->a(a), this->b(b) {}

  bool operator==(const Gauss& p)
  {
    return ((this->a == p.a) && (this->b == p.b));
  }
  bool operator!=(const Gauss& p)
  {
    return !(*this == p);
  }
  Gauss& operator=(const Gauss& p)
  {
    this->a = p.a;
    this->b = p.b;

    return *this;
  }
  Gauss& operator=(const T& i)
  {
    this->a = i;
    this->b = 0;

    return *this;
  }
  Gauss& operator+=(const Gauss& p)
  {
    this->a += p.a;
    this->b += p.b;
    return *this;
  }
  Gauss& operator-=(const Gauss& p)
  {
    this->a -= p.a;
    this->b -= p.b;
    return *this;
  }
  Gauss& operator *=(const Gauss &p)
  {
	  Gauss tmp = this->a * p.b + this->b * p.a;
	  this->a = this->a * p.a - this->b * p.b;
	  this->b = tmp;
	  return *this;
  }
  Gauss& operator/=(const Gauss& p)
  {
	  T div = !p;
	  *this *= Gauss(p.a, -p.b);
	  this->a /= div, this->b /= div;
	  return *this;
  }
  Gauss operator+(const Gauss& p)
  {
    return Gauss(this->a + p.a, this->b + p.b);
  }
  Gauss operator-(const Gauss& p)
  {
    return Gauss(this->a - p.a, this->b - p.b);
  }
  Gauss operator*(const Gauss& p)
  {
    return Gauss(this->a * p.a - this->b * p.b, this->a * p.b + this->b * p.a);
  }
  bool operator >=(const Gauss&p)
  {
    return (!(*this) >= !p);
  }
  Gauss operator%(const Gauss& p)
  {
    return *this - (*this / p) * p;
  }

  Gauss operator/(const Gauss& p)
  {
	  T div = !p;
	  Gauss g(p.a, -p.b);
	  g *= (*this);
	  g.a /= div, g.b /= div;
	  return g;
  }
  bool operator<(const T& i)
  {
    return (this->a < i && this->b < i);
  }
  friend ostream& operator<<(ostream& o, const Gauss& p);
  friend Gauss& operator-(Gauss& p);
  friend Gauss operator*(const Gauss& p, const T&i);
  friend Gauss operator*(const T&i, const Gauss& p);
  friend bool operator!=(const T&i, const Gauss& p);
  friend bool operator!=(const Gauss& p, const T&i);
  friend bool operator==(const T&i, const Gauss& p);
  friend bool operator==(const Gauss& p, const T&i);
  friend T operator!(const Gauss& p);
};

template<class T>
T operator!(const Gauss& p)
{
  return (p.a * p.a + p.b * p.b);
}

template<class T>
ostream& operator<<(ostream& o, const Gauss<T>& p)
{
  o << p.a << " + " << p.b << "i" << endl;

  return o;
}

template<class T>
Gauss<T>& operator-(Gauss<T>& p)
{
  p.a = -p.a;
  p.b = -p.b;
  return p;
}
template<class T>
bool operator==(const T& i, const Gauss<T>& p)
{
  return ((i == p.a) && (p.b == 0));
}
template<class T>
bool operator==(const Gauss<T>& p, const T& i)
{
  return (i == p);
}
template<class T>
bool operator!=(const T& i, const Gauss<T>& p)
{
  return !(i == p);
}
template<class T>
bool operator!=(const Gauss<T>& p, const T& i)
{
  return !(p == i);
}
template<class T>
Gauss<T> operator*(const T& i, const Gauss<T>& p)
{
  return Gauss<T>(i * p.a, i * p.b);
}
template<class T>
Gauss<T> operator*(const Gauss<T>& p, const T& i)
{
  return (i * p);
}