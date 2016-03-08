#pragma once

#include "math.h"
#include <algorithm>

using namespace std;

enum states{DEGSYM, DEG, PLUSSYM, XSYM};
class GFPoly // deg < 64
{  
public:

  int degree;
  long val;

  explicit GFPoly(int deg, long v)
  {
    if(deg < 0)
    {
      cout << "Incorrect degree!" << endl;
      return;
    }
    degree = deg;
    val = v;
  } 
  explicit GFPoly(int val) 
  {
    degree = 0;
    this->val = abs(val % 2);
  }
  explicit GFPoly()
  {
    degree = 0;
    val = 0;
  }
  static GFPoly CreateFromStream(istream& ifs = cin)
  {
    int degree, nonZeroCoeffs;
    long val = 0;
    int res;
	ifs >> degree >> nonZeroCoeffs;
	if (degree < 0 || degree < nonZeroCoeffs)
		throw exception("Incorrect degree");
    int i = 0;
    while(i <= nonZeroCoeffs)
    {
	  ifs >> res; 
	  if (res > degree)
		  throw exception("Incorrect coefficient");
      val |= (1 << res);
      i++;
    }
    return GFPoly(degree, val);
  }
  static GFPoly CreateFromString(char *str, int n)
  {
    GFPoly pol;
    int i = 0;
    while(i < n && str[i] != ' ')
    {
      int new_num = int(str[i] - '0');
      if(new_num > 9 || new_num < 0)
        throw exception("Wrong param string");
      pol.degree = pol.degree * 10 + new_num;
      i++;
    }
    if(pol.degree < 0)
      throw exception("Illegal degree value");
    i++;
    int temp = 0;
    states st;
    while(i < n)
    {      
      if(str[i] == 'x')
      {
        st = XSYM;
      }else if(str[i] == '^')
      {
        if(st == XSYM)
        {
          st = DEGSYM;
        }else throw exception("Wrong param string");
      }else if(str[i] == '+')
      {
        if(st == DEG)
        {
          st = PLUSSYM;
          pol.val |= (1 << temp);
          temp = 0;
        }else throw exception("Wrong param string");
      }else if(str[i] >= '0' && str[i] <= '9')
      {
        if(st == DEGSYM || st == DEG)
        {
          st = DEG;
          temp = temp * 10 + int(str[i] - '0');
        }else throw exception("Wrong param string");
      }else throw exception("Wrong param string");
      i++;
    }
    pol.val |= (1 << temp);
    if(pol.val - pol.degree > pol.degree)
      throw exception("Incorrect degree");
    return pol;
  }
 
  bool operator>=(const GFPoly& p)
  {
    return (this->degree >= p.degree);
  }
  bool operator==(const GFPoly& p)
  {
    return ((this->degree == p.degree) && (this->val == p.val));
  }
  bool operator!=(const GFPoly& p)
  {
    return !(*this == p);
  }

  GFPoly& operator=(const GFPoly& p)
  {
    this->val = p.val;
    this->degree = p.degree;

    return *this;
  }
  GFPoly& operator=(int i)
  {
    this->val = i % 2;
    this->degree = 0;

    return *this;
  }
  GFPoly& operator+=(const GFPoly& p)
  {
    this->val ^= p.val;
    if(this->degree < p.degree)
      this->degree = p.degree;
    while(this->degree > 0 && (((1 << this->degree) & this->val) == 0))
      this->degree--;

    return *this;
  }
  bool operator<(int i)
  {
    return false;
  }
  GFPoly& operator-=(const GFPoly& p)
  {
    return (*this += p);
  }

  GFPoly operator+(const GFPoly& p)
  {
    int deg = std::max(this->degree, p.degree);
    long val = this->val ^ p.val;
    while(deg > 0 && (((1 << deg) & val) == 0))
      deg--;

    return GFPoly(deg, val);
  }
  GFPoly operator-(const GFPoly& p)
  {
    return (*this + p);
  }
  GFPoly operator*(const GFPoly& p)
  {
    int deg = this->degree + p.degree;
    long val = 0;
    for(int i = 0; i <= deg; i++)
      val ^= this->val * (p.val & (1 << i));

    return GFPoly(this->degree + p.degree, val);
  }
  GFPoly operator%(const GFPoly& p)
  {
    GFPoly dd = *this;
    GFPoly div = p;
    GFPoly res;
    int deg = this->degree - p.degree;
    long temp = 0;
    if(deg < 0)
      return *this;
    while(dd != 0 && dd.degree >= div.degree )
    {
      res.val = 1 << (dd.degree - div.degree);
      res.degree = (dd.degree - div.degree);
      temp |= res.val;
      dd -= div * res;
    }

    return dd;
  }
  GFPoly operator/(const GFPoly& p)
  {
    GFPoly dd = *this;
    GFPoly div = p;
    GFPoly res(0, 0);
    int deg = this->degree - p.degree;
    long temp = 0;
    if(deg < 0)
      return *this;
    while(dd.degree >= div.degree)
    {
      res.val = 1 << (dd.degree - div.degree);
      res.degree = (dd.degree - div.degree);
      temp |= res.val;
      dd -= div * res;
    }
    res.val = temp;
    res.degree = deg;

    return res;
  }

  GFPoly& modDeg(int degree, GFPoly &mod)
  {
    GFPoly mult = *this;
    if(degree > 0)
      for(int i = 1; i < degree; i++)
        *this = ((*this) * mult) % mod;
    else if(degree == 0)
      *this = 1;
    else if(degree == -1)
      *this = 0;
    else
      cout << "Wrong degree!";

    return *this;
  }
  GFPoly modMult(GFPoly &p, GFPoly &mod)
  {
    GFPoly res = (*this * p) % mod;
    return res;
  }
  friend ostream& operator<<(ostream& o, const GFPoly& p);
  friend GFPoly& operator-(GFPoly& p);
  friend GFPoly& operator*(const GFPoly& p, int i);
  friend GFPoly& operator*(int i, const GFPoly& p);
  friend bool operator!=(int i, const GFPoly& p);
  friend bool operator!=(const GFPoly& p, int i);
  friend bool operator==(int i, const GFPoly& p);
  friend bool operator==(const GFPoly& p, int i);
  friend int operator!(const GFPoly& p);
};
int operator!(const GFPoly& p)
{
  return 0;
}
bool operator!=(const GFPoly& p, int i)
{
  return (p.val != i);
}
bool operator!=(int i, const GFPoly& p)
{
  return (p != i);
}
bool operator==(const GFPoly& p, int i)
{
  return (p.val == i);
}
bool operator==(int i, const GFPoly& p)
{
  return (p == i);
}
GFPoly& operator*(GFPoly& p, int i)
{
  if(i % 2 == 0)
    p.degree = 0, p.val = 0;

  return p;
}
GFPoly& operator*(int i, GFPoly& p)
{
  return (p * i);
}


GFPoly& operator-(GFPoly& p)
{
  return p;
}

ostream& operator<<(ostream& o, const GFPoly& p)
{
  for(int i = p.degree; i >= 0; i--)
    if((p.val & (1 << i)) != 0)
      o << "x^" << i << " + ";
  o << "\n";

  return o;
}

int poly_order(const GFPoly& p)
{
  GFPoly res(0, 1);
  GFPoly mult(1, 2);
  res = (res * mult) % p;
  int i = 1;
  while(res != 1)
  {
    res = (res * mult) % p;
    i++;
  }

  return i;
}