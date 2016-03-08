#include <map>
#include "primes_list.h"
#include "math.h"
/* Методы вычисления дискретного логарифма */

// Полный перебор
long full_search(long x, long g, long mod)
{
  if(mod == 0)
  {
    cerr << "Wrong module!\n";
    return -1;
  }
  long res = 1, temp;
  mod = abs(mod);
  while(x < 0)
    x += mod;
  while(g < 0)
    g += mod;
  x %= mod;
  g %= mod;
  temp = g;
  if(x == 0)
  {
    if(g == 0)
      cerr << "Every degree suits\n";
    else
      cerr << "No solutions!\n";
    return -1;
  }
  if(g == 0)
  {
    cerr << "No solutions\n";
    return -1;
  }
  if(x == 1)
    return 0;
  while(temp != x)
  {
    temp = (temp * g) % mod;
    if(temp == g)
    {
      cerr << "No solutions!" << endl;
      return -1;
    }
    res++;
  }
  return res;
}

// Служебные функции для Р-метода Полларда
long phi(long z, long n, long mod)
{
  if(z % 3 == 1)
    return n;
  else if(z % 3 == 2)
    return (2 * n) % (mod - 1);
  else
    return (n + 1) % (mod - 1);
}
long psi(long z, long n, long mod)
{
  if(z % 3 == 1)
    return (n + 1) % (mod - 1);
  else if(z % 3 == 2)
    return (2 * n) % (mod - 1);
  else
    return n;
}
long f(long z, long x, long g, long mod)
{
  if(z % 3 == 1)
  {
    return (z * x) % mod;
  }
  else if(z % 3 == 2)
  {
    return ((z * z) % mod);
  }
  else
  {
    return ((z * g) % mod);
  }
}

// Р-метод Полларда
long rho_log(long x, long g, long mod)
{
  if(mod == 0)
  {
    cerr << "Wrong module!\n";
    return -1;
  }
  mod = abs(mod);
  while(x < 0)
    x += mod;
  while(g < 0)
    g += mod;
  x %= mod;
  g %= mod;
  if(x == 0)
  {
    if(g == 0)
      cout << "Every degree suits\n";
    else
      cout << "No solutions!\n";
    return -1;
  }
  if(g == 0)
  {
    cerr << "No solutions\n";
    return -1;
  }
  
  long u1 = 0, v1 = 0, u2 = 0, v2 = 0;
  long z1 = 1, z2 = 1;
  do{
    u1 = phi(z1, u1, mod);
    v1 = psi(z1, v1, mod);
    z1 = f(z1, x, g, mod);
    u2 = phi(f(z2, x, g, mod), phi(z2, u2, mod), mod);
    v2 = psi(f(z2, x, g, mod), psi(z2, v2, mod), mod);
    z2 = f(f(z2, x, g, mod), x, g, mod);
  }while(z1 != z2);
  long r = v1 - v2;
  if(r == 0)
  {
    cerr << "FAIL!";
    return -2;
  }
  else
  {
    int len;
    int *res;
    bool flag = true;
    int i = 0;
    cmp_one(r, u2 - u1, mod - 1, res, len);
    if(res == nullptr || len == 0)
    {
      cout << "No solutions found\n";
      return -1;
    }
    for(; i < len && flag; i++)
    {
      if (deg(g, res[i], mod) == x)
      {
        flag = false;
      }
    }    
    cout << endl;
    z1 = res[i - 1];
    delete[] res;
    if(deg(g, z1, mod) != x)
    {
      cout << "No solutions!\n";
      return -1;
    }
    return z1;
  }
}

// Детский шаг-гигантский шаг. Алгоритм Шэнкса
long child_giant(long x, long g, long mod)
{
  if(mod == 0)
  {
    cerr << "Wrong module!\n";
    return -1;
  }
  mod = abs(mod);
  while(x < 0)
    x += mod;
  while(g < 0)
    g += mod;
  x %= mod;
  g %= mod;

  if(x == 0)
  {
    if(g == 0)
      cout << "Every degree suits\n";
    else
      cout << "No solutions!\n";
    return -1;
  }
  if(g == 0)
  {
    cerr << "No solutions\n";
    return -1;
  }

  long b = g;
  long m = long(sqrt(float(mod)));
  for(long i = 1; i < m; i++)
    b = (g * b) % mod;
  std::map<long, long> mymap;
  long temp = 1;
  long res;
  for(long i = 1; i < m; i++)
  {
    res = (temp * b) % mod;
    try{
      mymap.at(res);
      temp = res;
    }catch(std::out_of_range ex){    
      mymap[res] = i;
      temp = res;
    }
  }
  temp = (x * g) % mod;
  for(int j = 1; j <= m; j++)
  {
    try{
      long i = mymap.at(temp);
      b = (m * i - j) % mod;
      break;
    }catch(std::out_of_range ex) {}
    temp = (temp * g) % mod;
  }
  if(deg(g, b, mod) != x)
  {
    cout << "No solutions!\n";
    return -1;
  }
  return b;
}

// Алгоритм Полига-Хеллмана
long pohlig_hellman(long x, long g, long m)
{
  if(m == 0)
  {
    cerr << "Wrong module!\n";
    return -1;
  }
  m = abs(m);
  while(x < 0)
    x += m;
  while(g < 0)
    g += m;
  x %= m;
  g %= m;

  if(x == 0)
  {
    if(g == 0)
      cout << "Every degree suits\n";
    else
      cout << "No solutions!\n";
    return -1;
  }
  if(g == 0)
  {
    cerr << "No solutions\n";
    return -1;
  }

  long* fact;
  long len;
  factorize(m - 1, fact, len);
  long q, y, k, p, degree, mod, h, mul;
  long *a = new long[len];
  memset(a, 0, len * sizeof(long));
  for(unsigned long i = 0; i < len; i++)
  {
    k = fact[i + len];
    q = (m - 1) / fact[i];
    p = 1;
    mul = x;
    degree = 0;
    h = long(deg(g, q, m));
    for(int j = 0; j < k; j++)
    {
      mod = p * fact[i];
      mul = mul * long(deg(g, -(degree * p), m));
      y = long(deg(mul, q, m));
      degree = (mod > 10) ? child_giant(y, h, m) : full_search(y, h, m);
      a[i] += degree * p;
      p = mod;
      q /= fact[i];
    }
    fact[i] = p;
  }
  p = CRT(a, fact, len);
  delete[] fact;
  if(deg(g, p, m) != x)
  {
    cerr << "No solutions\n";
    return -1;
  }
  return p;
}

// Разложение по факторной базе. Возвращает вектор степеней элементов факторной базы
bool factDeg(long num, long * a, uint len, long *factBase, uint lenBase)
{
  len = 0;
  for(uint i = 0; i < lenBase; i++)
  {
    if(num % factBase[i] == 0)
    {
      num /= factBase[i];
      a[i] = 1;
      while((num % factBase[i]) == 0)
      {
        a[i]++;
        num /= factBase[i];
      }
      len++;
    }
    else a[i] = 0;
  }
  return (num == 1);
}

// Алгоритм исчисления индексов
long index(long x, long g, long n)
{
  if(n == 0)
  {
    cerr << "Wrong module!\n";
    return -1;
  }
  n = abs(n);
  while(x < 0)
    x += n;
  while(g < 0)
    g += n;
  x %= n;
  g %= n;
  if(x == 0)
  {
    if(g == 0)
      cerr << "Every degree suits\n";
    else
      cerr << "No solutions!\n";
    return -1;
  }
  if(g == 0)
  {
    cerr << "No solutions\n";
    return -1;
  }
  if(x == 1)
    return 0;
  int c = 10;
  long k = 1;
  long res = 0;
  long tmp;
  long t = long(ceil(exp((1. / sqrt(2.)) * sqrt(log((double(n)) * log(log(double(n))))))));
  long *matr = new long[(t + 1)*(t + c)];
  long *resVec = new long[t + c];
  long m = t + c;
  for(int i = 0; i < t + c; i++)
  {
    k++;
    matr[i * (t + 1) + t] = k;
    tmp = long(deg(g, k, n));
    if(tmp % n == 1)
    {
      m = i;
      break;
    }
    if(!factDeg(tmp, matr + i * (t + 1), t, primes, t)) i--;
  }
  long *factMod;
  long fModLen;
  factorize(n - 1, factMod, fModLen);
  bool flag = true;
  for(uint i = 0; i < fModLen && flag; i++)
    if(factMod[fModLen + 1] > 1) flag = false;
  if(flag)
  {
    long *logVec = new long[(m) * fModLen];
    memset(logVec, 0, (m) * fModLen * sizeof(long));
    long *tmpMatr = new long[(t + 1)*(m)];
    for(uint i = 0; i < fModLen; i++)
    {
      for(int j = 0; j < m; j++)
        for(int k = 0; k < t + 1; k++)
          tmpMatr[j * (t + 1) + k] = matr[j * (t + 1) + k] % factMod[i];
      solve(tmpMatr, m, t + 1, logVec + i * (m), factMod[i]);
    }
    memset(matr, 0, sizeof(long) * t);
    long d = (m > t) ? t : m;
    for(int i = 0; i < d; i++)
    {
      for(uint j = 0; j < fModLen; j++)
        matr[i] += (logVec[j * (m) + i]) ? (logVec[j * (m) + i] * ((n - 1) / factMod[j]) * opposite(((n - 1) / factMod[j]), factMod[j])) : 0;
      matr[i] %= (n - 1);
    }
    k = 0;
    do{
      k++;
      c = (x * long(deg(g, k , n))) % n;
    }while(c == 1 || !factDeg(c, resVec, m, primes, m));
    for(int i = 0; i < d; i++)
    {
      res += resVec[i] * matr[i];
    }
    res %= n;
    res -= (k % n);
    while(res < 0) res += n;
    delete[] logVec;
    delete[] tmpMatr;
    if(deg(g, res, n) != x)
    {
      cout << "No solutions!\n";
      res = -1;
    }
  }
  else
    res = child_giant(x, g, n);
  delete[] factMod;
  delete[] resVec;
  delete[] matr;
  return res;
}