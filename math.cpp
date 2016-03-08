#include "math.h"
#include "ThreadPool.h"

// Решение сравнений первой степени вида x[i] * num = equal mod m
void cmp_one(int num, int equal, int m, int*& x, int& n)          
{
  if(m == 0)
  {
    cerr << "Incorrect parameter: mod\n";
    x = nullptr;
    n = 0;
    return;
  }
  while(equal < 0)
    equal += m;
  while(num < 0)
    num += m;
  num %= m;
  equal %= m;  
  if(num == 0 || equal == 0)
  {
    if(num == 0 && equal != 0)
      cerr << "No solutions: a equals to 0 and b not\n";      
    else
      cerr << "Infinite amount of solutions: a, b equals to 0\n";
    x = nullptr;
    n = 0;
    return;
  }
  int gcd_am = gcd(num, m);
  if(gcd_am > 1 && equal % gcd_am != 0)
  {
    cerr << "There is no solutions!\n";
    x = nullptr, n = 0;
    return;
  }
  num /= gcd_am, equal /= gcd_am, m /= gcd_am;
  int gcd_ab = gcd(num, equal);
  num /= gcd_ab, equal /= gcd_ab;

  int i;
  int tmp;
  gcdBezout(num, m, i, tmp);
  i = (equal * i) % m;

  x = new int[gcd_am];
  n = gcd_am;
  while(i < 0)
    i += m;
  for(int j = 0; j < gcd_am; j++)
  {
    x[j] = i + j * m;
    if(x[j] < 0)
      x[j] += m * gcd_am;
  }
}

// Возведение в степень. Используется последовательное возведение в квадрат
__int64 deg(__int64 num, __int64 deg, __int64 mod)
{
  if(mod == 0)
  {
    cerr << "Incorrect module\n";
    return -1;
  }else if(mod == 1)
    return 0;
  num %= mod;
  deg %= mod - 1;
  if(num == 0)
    return 0;
  else if(num == 1)
    return 1;
  if(deg == 0) 
	  return 1;
  __int64 tmp;
  while(deg < 0)
  {
    deg += (mod - 1);
  }
  __int64 res = 1;
  tmp = deg;
  __int64 x = num;
  while(tmp != 0)
  {
    if(tmp & 1)
    {
      res = (res * x) % mod;
    }
    x = (x * x) % mod;
    tmp >>= 1;
  }

  return res;
}

// Факторизация полным перебором. Максимум 500 множителей типа int
void factorize(long num, long *& a, long& len)          // num = sum(a[i]^a[i + len])
{
  long temp[500][2];
  len = 0;
  a = nullptr;
  long bord = num >> 1;//long(sqrt(float(num)));
  for(int i = 2; num > 1 && i <= bord; i++)
  {
    if(num % i == 0)
    {
      num /= i;
      temp[len][0] = i;
      temp[len][1] = 1;
      while((num % i) == 0)
      {
        temp[len][1]++;
        num /= i;
      }
      len++;
    }
  }
  if(len != 0)
  {
    a = new long[2 * len];
    for(unsigned int i = 0; i < len; i++)
    {
      a[i] = temp[i][0];
      a[i + len] = temp[i][1];
    }
  }
}

// Следующий элемент последовательности Ро-метода Полларда для длинных чисел. Дополнительный параметр нужен для метода Барретта
Longnum f(Longnum& x, Longnum& n, Longnum& u)
{
  Longnum res;
  Longnum tmp = (x * x + 1);
  tmp.div(n, res, u);
  return res;
}

// Генерация случайного числа, меньшего n. Дополнительный параметр нужен для деления по методу Барретта
Longnum random(Longnum& n, Longnum& u)
{
  word *res = new word[n.len + 1];
  if(res == nullptr)
    throw std::exception("Bad alloc");
  memset(res, 0, (n.len + 1) * sizeof(word));
  for(uint i = 0; i < n.len; i++)
    res[i] = (((word)(rand()) << (sizeof(int) * 4)) + rand()) % (RADIX - 1);
  for(; n.len > 1 && res[n.len - 1] == 0; n.len--);
  Longnum r;
  Longnum(res, n.len, n.len + 1, S_POS).div(n, r, u);
  return r;
}

bool a = true;
Longnum x0;

// Ро-метод Полларда для длинных чисел
Longnum rho_pollard(Longnum& n, uint thread_num)
{
  Longnum u = (Longnum(1).leftShift(n.len << 1)).divKnuth(n);
  Longnum x;
  if(a == true)
  {
    x0 = random(n, u);
    a = false;
  }
  x = x0;
  Longnum z(f(f(x, n, u), n, u));
  Longnum tmp;
  ThreadPool tp(thread_num);
  tp.num = n;
  while(!tp.flag)
  { 
    tmp = abs(z - x);
    if(!tp.acquire(tmp))
      break;
    x = f(x, n, u);
    z = f(f(z, n, u), n, u);
  }
  return tp.res;
}
