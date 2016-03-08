#pragma once 

#include "math.h"

class Polynom
{
  int deg;
  int *coeffs;
  explicit Polynom()
  {
    deg = 0;
    coeffs = nullptr;
  }
  explicit Polynom(int degree)
  {
    if(degree < 0)
    {
      cout << "Wrong param!";
      return;
    }
    deg = degree;
    coeffs = new int[degree + 1];
    for(int i = 0; i <= degree; i++)
    {
      cout << "Input x^" << i << ": ";
      cin >> coeffs[i];
      cout << endl;
    }
  }
  Polynom(const Polynom& p)
  {
    this->deg = p.deg;
    if(p.coeffs != nullptr)
    {
      this->coeffs = new int[this->deg];
      for(int i = 0; i <= this->deg; i++)
        this->coeffs[i] = p.coeffs[i];
    }
  }
  ~Polynom()
  {
    if(coeffs != nullptr)
      delete[] coeffs;
  }
  Polynom& operator!()
  {
    if(coeffs == nullptr)
      return *this;
    int* temp = (deg > 0) ? new int[deg] : new int[1];
    for(int i = 0; i < deg; i++)
      temp[i] = coeffs[i + 1] * (i + 1);
    temp[deg] = 0;
    deg = (deg > 0) ? (deg - 1) : 0;

    delete[] coeffs;
    coeffs = temp;

    return *this;
  }
  Polynom operator+(const Polynom& p)
  {
    Polynom res;
    res.deg = maxEq<int>(this->deg, p.deg);
    int mindeg = min<int>(this->deg, p.deg);
    res.coeffs = new int[res.deg];

    for(int i = 0; i <= mindeg; i++)
      res.coeffs[i] = p.coeffs[i] + this->coeffs[i];
    if(this->deg == mindeg)
      while(++mindeg <= p.deg)
        res.coeffs[mindeg] = p.coeffs[mindeg];
    else
      while(++mindeg <= this->deg)
        res.coeffs[mindeg] = this->coeffs[mindeg];

    return res;
  }
  Polynom operator+(int i)
  {
    Polynom p(*this);
    if(this->coeffs == nullptr)
      cout << "Error!";
    else
      p.coeffs[0] += i;

    return p;
  }
  Polynom operator-(int i)
  {
    return (*this + (-i));
  }
  Polynom operator-(const Polynom& p)
  {
    return (*this + (-p));
  }
  long retVal(int i)
  {
    long res = 0;
    for(int i = 0; i <= this->deg; i++)
      res += long(pow(float(this->coeffs[i]), float(i)));

    return res;
  }
  friend Polynom operator-(const Polynom& p);


  void cmp(int i)
  {

  }
};

Polynom operator-(const Polynom& p)
{
  Polynom res(p);
  if(res.coeffs == nullptr)
    cout << "Error!";
  else
    for(int i = 0; i <= res.deg; i++)
      res.coeffs[i] = -res.coeffs[i];

  return res;

}