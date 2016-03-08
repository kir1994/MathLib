//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
#include "longnums.h"
#include <iomanip>
#include <emmintrin.h>
#include <intrin.h>
#include <malloc.h>
#include <bitset>
#include <sstream>

/* ��������� ���������. 
 * ���������: 
 *    - word *a, word *b - �����, ������� ���� �����������
 *    - uint lenA, uint lenB - ����� ���� �����
 *    - word *out - ���� ��������� ��������� �����������
 *    - uint outSh - � ����� ��������� ��������� ��������� � out
 *    - word *mid - ����� ���� (a0 + a1)(b0 + b1)
 *    - uint lenMid - ��� �����
 */
struct Task
{
  uint lenA;
  uint lenB;
  uint lenMid;
  word *mid;
  word *a;
  word *b;
  word *out;
  uint outSh;
};

/* �������, ������ ��� ������ ������ �������� */
void fill(Task& t, unsigned newlenA, unsigned newlenB,
  word *newa, word *newb, word *newout, unsigned newSh);
bool less(word *a, word *b, uint lenA, uint lenB);
uint add(word *a, word *b, word *res, uint lenA, uint lenB);
uint sub(word *a, word *b, word *res, uint lenA, uint lenB);
uint store(word *a, word *res, uint len);
uint mul(word *a, word *b, word *res, uint lenA, uint lenB, word *temp); // ����� tmp = (ceil(double(lenA)/4)+ceil(double(lenB)/4)) * 8
uint cutZeros(word *a, uint lenA);

/* ������������ ������ Longnum */
// �� ���������. ����� ����� 0
Longnum::Longnum()
{  
  digits = new word[2];
  if(digits == nullptr)
    throw std::exception("Bad Alloc");
  digits[0] = 0;
  digits[1] = 0;
  len = 1;
  max_size = 2;
  sign = S_ZERO;
}
// ���������� ������ ����, ��� ����� � ����. ���������� ������ ����������� �����
Longnum::Longnum(const word* newDigits, uint newLen, char newSign): len(newLen), sign(newSign), max_size(newLen + 1) // Copy digits
{
  this->digits = new word[max_size];
  if(this->digits == nullptr)
      throw std::exception("Bad Alloc");
  memcpy(this->digits, newDigits, NUM_SIZE * len);
  if(len == 1 && this->digits[0] == 0)
    this->sign = S_ZERO;
  this->digits[len] = 0;
}
// ��� �������� ���������� ������ ����, ����� �����, ����� ������� � ���� �����. ������ ������� �� ���������
Longnum::Longnum(word* newDigits, uint newLen, uint newMSize, char newSign): len(newLen), sign(newSign), 
  max_size(newMSize), digits(newDigits) {}
// ���������� ����� � �������� �������(������������ ��� �����), ���������� ����. ��������� ����� ������ ����� newlen+1
Longnum::Longnum(word* newDigits, uint newlen): len(newlen), max_size(len + 1)
{
  if(newDigits == nullptr || newlen <= 0)
  {    
    this->digits = new word[2];
    if(digits == nullptr)
      throw std::exception("Bad Alloc");
    this->digits[0] = 0;
    this->sign = S_ZERO;
    this->len = 1, this->max_size = 2;
  }else
  {
    this->sign = S_POS;
    
    this->digits = new word[(this->len + 1)];
    if(this->digits == nullptr)
      throw std::exception("Bad Alloc");
    for(uint i = 0; i < this->len; i++)
      this->digits[i] = newDigits[this->len - i - 1];
    this->digits[this->len] = 0;
    if(this->len == 1 && this->digits[0] == 0)
      this->sign = S_ZERO;
  }
}
// ���������� ����� � ���� Longnum. ��������� ����� ����� 1
Longnum::Longnum(word num)
{
  this->sign = (num == 0) ? S_ZERO : S_POS;
  this->max_size = 2;
  this->len = 1;
  
  this->digits = new word[2];
  if(this->digits == nullptr)
    throw std::exception("Bad alloc");
  memset(this->digits, 0, NUM_SIZE << 1);
  this->digits[0] = ((num << (NUM_SIZE * 8 - NUM_BIT_SIZE)) >> (NUM_SIZE * 8 - NUM_BIT_SIZE));
  this->digits[1] = 0;
}
// �����������
Longnum::Longnum(const Longnum& l): len(l.len), sign(l.sign), max_size(l.max_size)
{  
  this->digits = new word[this->max_size];
  if(this->digits == nullptr)
        throw std::exception("Bad Alloc");
  memcpy(this->digits, l.digits, NUM_SIZE * this->max_size);
}
// ����������� ��������� ����������, ������ ���� �� ����������, ���������� ������������ ���������
Longnum::Longnum(Longnum&& l): len(l.len), sign(l.sign), max_size(l.max_size)
{
  this->digits = l.digits;
  l.clear();
}
// ������� ������
Longnum::~Longnum()
{
  if(digits != nullptr)
  {
    delete[] digits;
    digits = nullptr;
  }
}
// ��������� ��������� �� ������ ����, ����� ��� rvalue-������
void Longnum::clear()
{
  digits = nullptr;
}
/* ��������� */
// ��������� i-� �� ������� ����� �����
word Longnum::operator[](int i)        
{
  if(i >= 0 && uint(i) <= len)
    return this->digits[i];
  else
    return 0;
}
// ����������� ���� ������, �������� ������ ������� ���� ��� ����������� ����
Longnum& Longnum::operator=(const Longnum& l)
{
  if(this == &l)
    return *this;
  if(this->max_size <= l.len)
  {
    if(this->digits != nullptr)
    {
      delete[] this->digits;      
      this->digits = nullptr;
    }    
    this->max_size = l.max_size;
    
    this->digits = new word[this->max_size];
    if(this->digits == nullptr)
        throw std::exception("Bad Alloc");
  } 
  this->len = l.len;
  memset(this->digits + this->len, 0, (this->max_size - this->len) * NUM_SIZE);
  this->sign = l.sign;
  memcpy(this->digits, l.digits, NUM_SIZE * this->len);

  return *this;
}
// ������ �� ���������, ���������� ������������ ���������
Longnum& Longnum::operator=(Longnum&& l)
{
  this->len = l.len;
  this->max_size = l.max_size;
  this->sign = l.sign;
  if(this->digits != nullptr)
    delete[] this->digits;
  this->digits = l.digits;
  l.clear();
  
  return *this;
}
// ������� ����������� �������� � ���������
Longnum Longnum::operator+(Longnum& l)
{ 
  if(this->sign == 0)
    return l;
  else if(l.sign == 0)
    return *this;
  else if(this->sign > l.sign)
    return (*this - (-l));
  else if(this->sign < l.sign)
    return (l - (-*this));
  uint maxLen = (this->len >= l.len) ? this->len : l.len;
  uint maxSize = maxLen + 1;
  
  word *buf = new word[maxSize];
  if(buf == nullptr)
    throw std::exception("Bad Alloc");
  memset(buf, 0, NUM_SIZE * maxSize);
  uint finLen = add(this->digits, l.digits, buf, this->len, l.len);
  if(finLen != maxSize)
  {
    return Longnum(buf, finLen, maxSize, this->sign);
  }else
  {
    uint newMSize = maxSize + MSIZE_INC;
   
    word *resBuf = new word[newMSize];
    if(resBuf == nullptr)
      throw std::exception("Bad Alloc");
    memcpy(resBuf, buf, NUM_SIZE * finLen);
    delete[] buf;
    resBuf[finLen] = 0;
    return Longnum(resBuf, finLen, newMSize, this->sign);
  }
}
// ������� ��������, ������ � ������� ���������� � *this
Longnum& Longnum::operator+=(Longnum& l)
{
  if(l.sign == 0)
    return *this;
  else if(this->sign == 0)
  {
    *this = l;
    return *this;
  }
  else if(this->sign > l.sign)
    return (*this -= (-l));
  else if(this->sign < l.sign)
    return (l -= (-*this));
  uint maxLen = (this->len >= l.len) ? this->len : l.len;
  uint maxSize = maxLen + 1;
  if(this->max_size > maxLen + 1)
  {
    this->len = add(this->digits, l.digits, this->digits, this->len, l.len);
    return *this;
  }
  word *buf = new word[maxSize];
  if(buf == nullptr)
    throw std::exception("Bad Alloc");
  memset(buf, 0, NUM_SIZE * maxSize);
  uint finLen = add(this->digits, l.digits, buf, this->len, l.len);
  if(finLen != maxSize)
  {
    delete[] this->digits;
    this->digits = buf;
    this->len = finLen;
    this->max_size = maxSize;
    
    return *this;
  }else
  {
    uint newMSize = maxSize + MSIZE_INC;
    delete[] this->digits;
    this->digits = new word[newMSize];
    if(this->digits == nullptr)
      throw std::exception("Bad Alloc");
    this->max_size = newMSize;
    this->len = finLen;      
    memcpy(this->digits, buf, NUM_SIZE * finLen);
    this->digits[finLen] = 0;
    delete[] buf;
    
    return *this;
  }
}
// ����������� ���������. �������������� ��������� �� �������� �� ������ ����� �������� 
Longnum Longnum::operator-(Longnum& l)
{
  if(this->sign == 0)
    return -l;
  else if(l.sign == 0)
    return *this;
  if(this->sign != l.sign)
    return (l + *this);
  if(*this < l)
    return -(l - *this);
  uint maxSize = this->max_size;
  
  word *buf = new word[maxSize];
  if(buf == nullptr)
    throw std::exception("Bad Alloc");
  memset(buf, 0, NUM_SIZE * maxSize);
  uint finLen = sub(this->digits, l.digits, buf, this->len, l.len);
  if(maxSize - finLen <= MSIZE_INC)
  {    
    return Longnum(buf, finLen, maxSize, this->sign);
  }else
  {    
    word *resBuf = new word[(finLen + 1)];
    if(resBuf == nullptr)
      throw std::exception("Bad Alloc");
    resBuf[finLen] = 0;
    memcpy(resBuf, buf, NUM_SIZE * finLen);
    delete[] buf;
    
    return Longnum(resBuf, finLen, finLen + 1, this->sign);
  }
}
// ��������� � ������� ���������� � *this
Longnum& Longnum::operator-=(Longnum& l)
{
  if(this->sign == 0)
    return (*this = -l);
  else if(l.sign == 0)
    return *this;
  if(this->sign != l.sign)
    return (*this += l);
  if(*this < l)
    return (*this = -(l -= *this));

  this->len = sub(this->digits, l.digits, this->digits, this->len, l.len);
  if(this->max_size - this->len <= MSIZE_INC)
  {
    return *this;
  }else
  {
    word *buf = new word[(this->len + 1)];
    this->max_size = this->len + 1;
    if(buf == nullptr)
      throw std::exception("Bad Alloc");
    memset(buf + this->len, 0, NUM_SIZE);
    memcpy(buf, this->digits, NUM_SIZE * this->len);
    delete[] this->digits;
    this->digits = buf;
    return *this;
  }
}
// ��������� ��������(�� ��������) ����� �������� � 1 ����� ��������
Longnum& Longnum::operator-=(word l)
{
  if(this->sign == 0)
  {
    this->digits[0] = l;
    this->sign = (l) ? S_NEG : S_ZERO;
  }else
  {
    *this = *this - Longnum(l);
  }  
  return *this;
}
// �������� � �������(�� �������) ������ �������� � 1 ����� ��������
Longnum Longnum::operator+(word l)
{
  return (*this + Longnum(l));
}
Longnum& Longnum::operator+=(word l)
{
  if(this->sign == 0)
  {
    this->digits[0] = l;
    this->sign = (l) ? S_POS : S_ZERO;
  }else
  {
    *this = *this + Longnum(l);
  }  
  return *this;
}
// ���������, ���������� ����� �������� � ������� ������� � �������
Longnum Longnum::operator*(Longnum& l)
{
  if(this->sign == S_ZERO || l.sign == S_ZERO)
    return Longnum();
  if (*this < l)
    return l * (*this);
  word bord = this->len + l.len;
  int maxLen = this->len;
  
  word *res = new word[(bord + 1)];
  if(res == nullptr)
      throw std::exception("Bad Alloc");
  memset(res, 0, NUM_SIZE * (bord + 1));
  word mDepth = word(ceil(log(double(maxLen) / log(2.0))));
  word tmp = (maxLen << 2) + 3 * (mDepth + 1);
  word *buf = new word[(tmp << 1)];
  word *tempBuf = new word[8 * uint(ceil(this->len/4.)+ceil(l.len/4.))];
  if(tempBuf == nullptr)
      throw std::exception("Bad Alloc");  
  if(buf == nullptr)
      throw std::exception("Bad Alloc");
  word *end = buf + (tmp << 1);
  memset(buf, 0, NUM_SIZE * (tmp << 1));
  word degA1, degA2, degB1, degB2;
  Task *st = new Task[((3 * (mDepth + 1)) << 1)];
  if(st == nullptr)
      throw std::exception("Bad Alloc");
  Task *curTask;
  fill(st[0], this->len, l.len, this->digits, l.digits, res, this->len);
  long i = 0;
  while(i >= 0)
  {
    curTask = &st[i];
    if((curTask->lenB  <= MAX_DEG_COL || curTask->lenA <= MAX_DEG_COL) && (curTask->mid == nullptr))
    {
      tmp = mul(curTask->a, curTask->b, curTask->out, curTask->lenA, curTask->lenB, tempBuf);
      i--;
    }else
    {
      if(curTask->mid)  // (a0 + a1)(b0 + b1) - a0b0 - a1b1
      {
        tmp = sub(curTask->mid, curTask->out, curTask->mid, cutZeros(curTask->mid, curTask->lenMid), cutZeros(curTask->out, (curTask->outSh << 1))/*curTask->lenA*/);        // -a0b0
        tmp = sub(curTask->mid, curTask->out + (curTask->outSh << 1), curTask->mid, tmp, cutZeros(curTask->out + (curTask->outSh << 1), curTask->lenB + curTask->lenA - (curTask->outSh << 1)));     // -a1b1
        
        store(curTask->mid, curTask->out + curTask->outSh, tmp);
        buf = curTask->mid;
        memset(buf, 0, (end - buf) * NUM_SIZE);
        i--;
      }else
      {
        degA2 = (curTask->lenA >> 1);
        degA1 = (curTask->lenA >> 1) + (curTask->lenA & 1);
        degB1 = (curTask->lenB >= degA1) ? degA1 : curTask->lenB;
        degB2 = (degB1 == degA1) ? curTask->lenB - degB1 : 0;        
        
        if(degB1 == degA1 || degA1 > 17)
        {
          i++;
          st[i - 1].outSh = degA1;
          st[i - 1].mid = st[i].out = buf;                                       // (a0 + a1)(b0 + b1)

          st[i - 1].lenMid = ((st[i - 1].outSh/*degA1*/ + 1) << 1);        
          buf += st[i - 1].lenMid;
          st[i].a = buf;
          buf += (st[i - 1].outSh/*degA1*/ + 1);
          st[i].b = buf;
          buf += (st[i - 1].outSh/*degA1*/ + 1);        
          st[i].mid = 0;
          st[i].outSh = st[i - 1].outSh;
          st[i].lenA = //degA1 + 1;
            add(curTask->a, curTask->a + degA1, st[i].a, degA1, degA2);   // a0 + a1
        //if(degB1 == degA1)
        //{
          st[i].lenB = //degB1 + 1;
            add(curTask->b, curTask->b + degB1, st[i].b, degB1, degB2);   // b0 + b1
          fill(st[++i], cutZeros(curTask->a, degA1), cutZeros(curTask->b, degB1), curTask->a, curTask->b, curTask->out, curTask->outSh);  // a0 * b0
          if(degB2 != 0)
            fill(st[++i], cutZeros(curTask->a + degA1, degA2), cutZeros(curTask->b + degB1, degB2), curTask->a + degA1, curTask->b + degB1, curTask->out + (curTask->outSh << 1), curTask->outSh); // a1 * b1
       // }
        }
        else
        {
          tmp = mul(curTask->a, curTask->b, buf, cutZeros(curTask->a, curTask->lenA), cutZeros(curTask->b, curTask->lenB), tempBuf);
          store(buf, curTask->out, tmp);
          memset(buf, 0, tmp * NUM_SIZE);
          i--;
        }        
      }
    }
  }
  //buf = w;
  delete []buf;
  delete[] tempBuf;
  delete[] st;
  for(tmp = bord; tmp > 1 && res[tmp - 1] == 0; tmp--);
  
  return Longnum(res, tmp, bord + 1, ((this->sign == l.sign) ? S_POS : S_NEG));
}
// ��������� � ������������� ���������� � *this
Longnum& Longnum::operator*=(Longnum& l)
{
  *this = *this * l;
  
  return *this;
}
// ��������� �� ����� ������ � 1 ����� ��������
Longnum Longnum::operator*(word l)
{
  if(l == 0)
    return Longnum();
  else
  {
    word *res = new word[(this->len + 2)];
    res[this->len] = 0, res[this->len + 1] = 0;
    word *buf = new word[(uint(ceil((this->len + 1)/4.)) + 4) * 8];
    uint len = mul(this->digits, &l, res, this->len, 1, buf);
    delete[] buf;
    return Longnum(res, len, this->len + 2, this->sign);
  }
}
// ����������� ������ ������ � �����
Longnum Longnum::rightShift(word i)
{
  return (this->len > i) ? Longnum(this->digits + i, this->len - i, this->sign) : Longnum();
}
Longnum Longnum::leftShift(word i)
{
  word *newDigs = new word[(this->len + i + 1)];
  memset(newDigs, 0, NUM_SIZE * i);
  memcpy(newDigs + i, this->digits, NUM_SIZE * this->len);
  newDigs[this->len + i] = 0;
  return Longnum(newDigs, this->len + i, this->len + i + 1, this->sign);
}
// ������� ��������. b - ��������, r - �������, u - ���������������� �����. 
Longnum& Longnum::divBarret(Longnum& b, Longnum& q, Longnum& r, Longnum& u)
{
  if(b.sign == S_ZERO)
    throw std::exception("Division by zero!");
  if(*this < b)
  {
    q = Longnum();
    r = *this;
    return *this;
  }
  else if(*this == b)
  {
    q = Longnum(1);
    r = Longnum();
    return *this;
  }else
  {
    q = (*this * u).rightShift(b.len << 1);
    r = *this - q * b;
    while(true)
    {
      if(r < 0)
      {
        r = r + b;
        q -= 1;
        continue;
      }else if(r >= b)
      {
        //q += r.divKnuth(b);
        r = r - b;
        q += 1;
        continue;
      }
      for(; r.len > 1 && r.digits[r.len - 1] == 0; r.len--);
      for(; q.len > 1 && q.digits[q.len - 1] == 0; q.len--);
      
      return *this;
    }
  }
}
// ���������� ������� �� �������. ������������ ���� ����� ��������, ���� ����� �����
Longnum Longnum::operator%(Longnum& l)
{
  if(l.sign == S_ZERO)
    throw std::exception("Division by zero!");
  if(*this < l)
    return *this;
  else if(*this == l)
    return Longnum();
  else
  {
    if(l.len == 1 && this->len == 1)
    {
      return Longnum(this->digits[0] % l.digits[0]);
    }else if(this->len > (l.len << 1))
    {
      Longnum res(*this);
      res.divKnuth(l);
      
      return res;
    }
    Longnum rem;
    Longnum u((Longnum(1).leftShift(l.len << 1)).divKnuth(l));
    this->divBarret(l, Longnum(), rem, u);
    
    return rem;
  }
}
// ���������� �������� �� �������. ������������ ���� ������� �����, ���� ������� ��������
Longnum Longnum::operator/(Longnum& l)
{
  if(l.sign == S_ZERO)
    throw std::exception("Division by zero!");
  if(*this < l)
    return Longnum();
  else if(*this == l)
    return Longnum(1);
  else
  {
    if(l.len == 1 && this->len == 1)
    {
      return Longnum(this[0] % l[0]);
    }else if(this->len > (l.len << 1))
      return this->divKnuth(l);
    Longnum quot;
    Longnum u((Longnum(1).leftShift(l.len << 1)).divKnuth(l));
    this->divBarret(l, quot, Longnum(), u);
    
    return quot;
  }
}
// ������� �� ����� ����� 1 ����� ��������
Longnum Longnum::operator/(word l)
{  
  if(l == S_ZERO)
    throw std::exception("Division by zero!");
  if(this->len == 1)
    return Longnum(this->digits[0] / l);  
  word *res = new word[(this->len + 1)];
  memset(res, 0, NUM_SIZE * (this->len + 1));
  char carry = 0;
  long long a, b;
  int i;
  for(i = this->len - 1; i >= 0; i--)
  {
    if(!carry)
    {
      if(this->digits[i] < l)
      {
        carry = 1;
      }else
      {
        res[i] = this->digits[i] - (this->digits[i] / l) * l;
      }
    }else
    {
      carry = 0;
      a = ((long long)(this->digits[i + 1]) << (NUM_BIT_SIZE)) + this->digits[i];
      b = (a / l) * l;
      res[i] = ((word(a - b) << (NUM_SIZE * 8 - NUM_BIT_SIZE)) >> (NUM_SIZE * 8 - NUM_BIT_SIZE));
    }
  }
  for(i = this->len; i > 1 && res[i - 1] == 0; i--);
  
  return Longnum(res, i, this->len + 1, this->sign);
}
// ������� �� ������ �������
Longnum Longnum::operator%(word l)
{
  return (*this - (*this / l) * l);
}
// ������� �� �����. ���������� ������� �� �������, ������� ��������� � *this 
Longnum Longnum::divKnuth(Longnum& l)
{
  if(l.sign == S_ZERO)
    throw std::exception("Division by zero!");
  unsigned long long q;
  unsigned long long r;
  unsigned long long z = RADIX;
  word d = word(z / ((unsigned long long)(l[l.len - 1]) + 1));
  *this = *this * d;
  bool flag = false;
  Longnum b(l * d);
  uint tmp = this->len - b.len + 1;
  
  word *qRad = new word[tmp];
  if(qRad == nullptr)
    throw std::exception("Bad alloc");
  memset(qRad, 0, tmp * NUM_SIZE);
  for(int j = this->len - b.len; j >= 0; j--)
  {
    q = ((z * (*this)[b.len + j] + (*this)[b.len + j - 1]) / b[b.len - 1]);
    r = ((z * (*this)[b.len + j] + (*this)[b.len + j - 1]) % b[b.len - 1]);
    if(q == z || q * b[b.len - 2] > (z * r + (*this)[b.len + j - 2]))
    {
      q--;
      r += b[b.len - 1];
      if(r < z)
      {
        if(q == z || q * b[b.len - 2] > (z * r + (*this)[b.len + j - 2]))
        {
          q--;
          r += b[b.len - 1];
        }
      }
    }    
    Longnum temp(b * word(q));
    if(less(this->digits + j, temp.digits, b.len + 1, temp.len))
    {
      sub(temp.digits, this->digits + j, this->digits + j, temp.len, b.len + 1);
      flag = true;
      Longnum ad(Longnum(1).leftShift(b.len));
      sub(ad.digits, this->digits + j, this->digits + j, ad.len, b.len + 1);
    }else
      sub(this->digits + j, temp.digits, this->digits + j, b.len + 1, temp.len);
    qRad[j] = word(q);
    if(flag)
    {
      qRad[j]--;
      add(this->digits + j, b.digits, this->digits + j, b.len + 1, b.len);
      flag = false;
    }
  }
  for(;(*this)[this->len - 1] == 0 && this->len > 1; this->len--);
  *this = *this / d;
  for(d = tmp; d > 1 && qRad[d - 1] == 0; d--);
  
  return Longnum(qRad, d, tmp, this->sign * l.sign);
}
// ����� �������� �������. � ����������� �� �������� ������������ ��������� ������� ����� ��� ��������
Longnum Longnum::div(Longnum& b, Longnum& r, Longnum& u)
{
  if(b.sign == S_ZERO)
    throw std::exception("Division by zero!");
  if(*this < b)
  {
    return Longnum();
    r = *this;
  }else if(*this == b)
  {
    return Longnum(1);
    r = Longnum();
  }else
  {
    if(b.len == 1 && this->len == 1)
    {
      return Longnum(this->digits[0] % b.digits[0]);
    }else if(this->len > (b.len << 1))
    {
      u = *this;
      r = u.divKnuth(b);
      
      return u;
    }else
    {
      Longnum quot;
      this->divBarret(b, quot, r, u);
      
      return quot;
    }
  }
}
// ��������� ��������� �����, ������� ����������� �����, ����� �����, � ����� ����������
bool Longnum::operator>(Longnum& l)
{
  if(this->sign > l.sign)
    return true;
  else if(this->sign < l.sign)
    return false;
  if(this->len > l.len)
    return true;
  else if(this->len < l.len)
    return false;
  for(int i = this->len - 1; i >= 0; i--)
  {
    if(this->digits[i] > l.digits[i])
      return true;
    else if(this->digits[i] < l.digits[i])
      return false;
  }
  return false;
}
bool Longnum::operator>=(Longnum& l)
{
  if(this->sign > l.sign)
    return true;
  else if(this->sign < l.sign)
    return false;
  if(this->len > l.len)
    return true;
  else if(this->len < l.len)
    return false;
  for(int i = this->len - 1; i >= 0; i--)
  {
    if(this->digits[i] > l.digits[i])
      return true;
    else if(this->digits[i] < l.digits[i])
      return false;
  }
  return true;
}
bool Longnum::operator<(Longnum& l)
{
  return !(*this >= l);
}
bool Longnum::operator<(word l)
{
  if(this->sign >= 0)
    return (this->digits[0] < l);
  else
    return true;
}
bool Longnum::operator!=(Longnum& l)
{
  return !(*this == l);
}
bool Longnum::operator==(Longnum& l)
{
  if(this->sign != this->sign)
    return false;
  if(this->len != l.len)
    return false;
  else
  {
    for(uint i = 0; i < l.len; i++)
      if(this->digits[i] != l.digits[i])
        return false;
    return true;
  }
}
bool Longnum::operator==(word num)
{
  return (this->digits[0] == num);
}
bool Longnum::operator!=(word num)
{
  return !(*this == num);
}
// ������� �����, ������ ���� �����
Longnum Longnum::operator-()
{
  return Longnum(this->digits, this->len, -this->sign);
}
/* ����� ������������ ������� ������ Longnum */

/* ������� ��� ������ �������� */
// ��������� ���������
void fill(Task& t, unsigned newlenA, unsigned newlenB,
  word *newa, word *newb, word *newout, unsigned newSh = 0)
{
  t.lenA = newlenA;
  t.lenMid = 0;
  t.lenB = newlenB;
  t.a = newa;
  t.b = newb;
  t.out = newout;
  t.outSh = newSh;
  t.mid = 0;
}
// �������� ������ ���� ������� ��� ��������� ����������
uint cutZeros(word *a, uint lenA)
{
  for(; lenA > 1 && a[lenA - 1] == 0; lenA--);
  return lenA;
}
// ������� � �������� ��������� � res
uint add(word *a, word *b, word *res, uint lenA, uint lenB)
{
  word *tmp;
  uint tempLen;
  if(lenA < lenB)
  {
    tmp = a;
    a = b;
    b = tmp;
    tempLen = lenA;
    lenA = lenB;
    lenB = tempLen;
  }
  uint carry = 0;
  __asm {
    mov ebx, dword ptr [res]; // ��������� �� ���������
    mov ecx, dword ptr [a];  // ��������� �� a
    mov edx, dword ptr [b];  // ��������� �� b
    xor eax, eax; // �������� �������
lp2:
    cmp eax, lenB;
    je lp3; // ���� ������� �������� ����� ����� ������������
    push ecx;
    mov ecx, dword ptr [ecx + eax * 4]; // ��������� ����� �� a
    
    add ecx, dword ptr [edx + eax * 4]; // ���������� � ������ �� b � ������ ��������
    add ecx, carry;
    and carry, 0;
    push edx;
    // ����������� �������
    // � ecx - �����
    mov edx, ecx;
    and edx, 0x3fffffff; // ������� ������ ����
    mov dword ptr [ebx + eax * 4], edx; // ���������� ���������
    shr ecx, 30; // �������� ��� ��������
    cmp ecx, 1;
    jne noCarryIn; // ���� �� ��������� �������
    or carry, 1;
noCarryIn:
    pop edx;
    pop ecx;
    inc eax;
    jmp lp2;
lp3:
    // ��� �������� �� b ����������, �������� ���������� ��� �� a
    // carry = 1, ���� ��������� ������� � �������� ����
    cmp carry, 1;
    jne copy1; // �������� ��� - ��������� ����������
    cmp eax, lenA;
    je end; // ���� ������� ������� ����� ����� ������������
    push ecx;
    mov ecx, dword ptr [ecx + eax * 4];
    inc ecx; // ���������� �������
    mov edx, ecx;
    and edx, 0x3fffffff;
    mov dword ptr [ebx + eax * 4], edx;
    shr ecx, 30;
    mov carry, ecx; // ��������� ��������
    pop ecx;
    inc eax;
    jmp lp3;
copy1: // �������� �� a �� ������
    push eax;
    add eax, 4;
    cmp eax, lenA;
    pop eax;
    ja copy2;
    movdqu xmm0, xmmword ptr [ecx + eax * 4];
    movdqu xmmword ptr [ebx + eax * 4], xmm0;
    add eax, 4;
    jmp copy1;
copy2: // ���������� ������� �� a
    cmp eax, lenA;
    je end;
    mov edx, dword ptr [ecx + eax * 4];
    mov dword ptr [ebx + eax * 4], edx;
    inc eax;
    jmp copy2;
end:
  }

  if(carry == 1)
  {
    res[lenA] = 1;
    tempLen = lenA + 1;
  }
  else
    tempLen = lenA;
  return tempLen;
}
// ��������
bool less(word *a, word *b, uint lenA, uint lenB)
{
  if(lenA > lenB)
    return false;
  else if(lenB > lenA)
    return true;
  else
  {
    for(int i = lenA - 1; i >= 0; i--)
    {
      if(a[i] > b[i])
        return false;
      else if(a[i] < b[i])
        return true;
    }
    return false;
  }
}
// �������
uint sub(word *a, word *b, word *res, uint lenA, uint lenB)
{
  uint carry = 0;

  __asm {
    mov ebx, dword ptr [res]; // ��������� �� ���������
    mov ecx, dword ptr [a];  // ��������� �� a
    mov edx, dword ptr [b];  // ��������� �� b
    xor eax, eax; // �������� �������

lp2: 
    cmp eax, lenB;
    je lp3; // ���� ������� ������� ����� ����� ������������
    push ecx;
    mov ecx, dword ptr [ecx + eax * 4]; // ��������� ����� �� a
    
    sub ecx, dword ptr [edx + eax * 4]; // �������� ����� �� b � ������ �����
    sub ecx, carry;
    and carry, 0;
    push edx;
    // ����������� ����
    // � ecx - ��������
    mov edx, ecx;
    and edx, 0x3fffffff; // ������� ������ ����
    mov dword ptr [ebx + eax * 4], edx; // ���������� ���������
    shr ecx, 31; // �������� �������� �����
    cmp ecx, 1;
    jne noCarryIn; // ���� �� ��������� ����
    or carry, 1;
noCarryIn:
    pop edx;
    pop ecx;
    inc eax;
    jmp lp2;
lp3:
    // ��� �������� �� b ����������, �������� ���������� ��� �� a
    // carry = 1, ���� ������� ���� � �������� ����
    cmp carry, 1;
    jne copy1; // ��� ����� - ��������� ����������
    cmp eax, lenA;
    je end; // ���� ������� �������� ����� ����� ������������
    push ecx;
    mov ecx, dword ptr [ecx + eax * 4];
    sub ecx, 1; // �������� ����
    mov edx, ecx;
    and edx, 0x3fffffff;
    mov dword ptr [ebx + eax * 4], edx;
    shr ecx, 31;
    mov carry, ecx; // ��������� ����
    pop ecx;
    inc eax;
    jmp lp3;
copy1: // �������� �� a �� ������
    push eax;
    add eax, 4;
    cmp eax, lenA;
    pop eax;
    ja copy2;
    movdqu xmm0, xmmword ptr [ecx + eax * 4];
    movdqu xmmword ptr [ebx + eax * 4], xmm0;
    add eax, 4;
    jmp copy1;
copy2: // ������ ������� �� a
    cmp eax, lenA;
    je end;
    mov edx, dword ptr [ecx + eax * 4];
    mov dword ptr [ebx + eax * 4], edx;
    inc eax;
    jmp copy2;
end:
  }
  uint i;
  for(i = lenA; i > 1 && res[i - 1] == 0; i--);
  return i;
}
// ����������� ������� "� �������". ��������� O(lenA*lenB)
uint mul(word *a, word *b, word *res, uint lenA, uint lenB, word *temp)
{
  uint tmpLen = (uint)(ceil(lenA / 4.) + ceil(lenB/4.)) * 8;

  memset(temp, 0, NUM_SIZE * tmpLen);
  memset(res, 0, NUM_SIZE * (lenA + lenB));

  if (lenA == 0 || lenB == 0)
	  return 1;
  __asm {
    mov ecx, [temp]; // ��������� �� temp
    mov ebx, [b]; // ��������� �� b

    xor eax, eax; // ������� b
    xor ebx, ebx; // ������� a

    mov eax, [a]; // ��������� �� a

outer:
    cmp ebx, lenA;
    jae end_outer; // ���� ebx >= lenA;

    mov eax, dword ptr [a];
    
    push ebx;
    add ebx, 4;
    cmp ebx, lenA;
    pop ebx;
    jbe four_elem;
    mov edx, lenA;
    sub edx, ebx; 
    pxor xmm0, xmm0;
    pinsrd xmm0, dword ptr [eax + ebx * 4], 0;
    sub edx, 1;
    cmp edx, 0;
    jz end_elem;
    pinsrd xmm0, dword ptr [eax + ebx * 4 + 4], 1;
    sub edx, 1;
    cmp edx, 0;
    jz end_elem;
    pinsrd xmm0, dword ptr [eax + ebx * 4 + 8], 2;
    jmp end_elem;
    four_elem: // ������ ������ �� 4� ���������
      movdqu xmm0, xmmword ptr [eax + ebx * 4];
    end_elem:

    mov edi, ebx;
    push ebx;

    mov ebx, dword ptr [b];
    xor eax, eax; // eax = 0
    inner:
      cmp eax, lenB;
      jae end_inner; // �����, ����� eax >= lenB

      push eax;
      add eax, 4;
      cmp eax, lenB;
      pop eax;
      jbe four_elem2;
      mov edx, lenB;
      sub edx, eax;
      pxor xmm1, xmm1;

      pinsrd xmm1, dword ptr [ebx + eax * 4], 0;
      sub edx, 1;
      cmp edx, 0;
      jz end_elem2;
      pinsrd xmm1, dword ptr [ebx + eax * 4 + 4], 1;
      sub edx, 1;
      cmp edx, 0;
      jz end_elem2;
      pinsrd xmm1, dword ptr [ebx + eax * 4 + 8], 2;
      jmp end_elem2;
      four_elem2:
        movdqu xmm1, xmmword ptr [ebx + eax * 4];
      end_elem2:

      pshufd xmm2, xmm0, 0x0; // �������� a1 a1
      pshufd xmm3, xmm1, 0x10; // �������� b2 b1
      pshufd xmm4, xmm1, 0x32; // �������� b4 b3
      movdqu xmm5, xmm2;
      pmuludq xmm5, xmm3; // a1 a1 * b2 b1
      pmuludq xmm2, xmm4; // a1 a1 * b4 b3
      movdqu xmm6, [ecx + edi * 8];
      paddq xmm6, xmm5; 
      movdqu xmm7, [ecx + edi * 8 + 16];
      paddq xmm7, xmm2;
      movdqu [ecx + edi * 8], xmm6;
      movdqu [ecx + edi * 8 + 16], xmm7;

      pshufd xmm2, xmm0, 0x11;
      movdqu xmm5, xmm2;
      pmuludq xmm5, xmm3; // a2 a2 * b2 b1
      pmuludq xmm2, xmm4; // a2 a2 * b4 b3
      movdqu xmm6, [ecx + edi * 8 + 8];
      paddq xmm6, xmm5; 
      movdqu xmm7, [ecx + edi * 8 + 24];
      paddq xmm7, xmm2;
      movdqu [ecx + edi * 8 + 8], xmm6;
      movdqu [ecx + edi * 8 + 24], xmm7;

      pshufd xmm2, xmm0, 0x22; // �������� a3 a3
      movdqu xmm5, xmm2;
      pmuludq xmm5, xmm3; // a3 a3 * b2 b1
      pmuludq xmm2, xmm4; // a3 a3 * b4 b3
      movdqu xmm6, [ecx + edi * 8 + 16];
      paddq xmm6, xmm5; 
      movdqu xmm7, [ecx + edi * 8 + 32];
      paddq xmm7, xmm2;
      movdqu [ecx + edi * 8 + 16], xmm6;
      movdqu [ecx + edi * 8 + 32], xmm7;

      pshufd xmm2, xmm0, 0x33;
      movdqu xmm5, xmm2;
      pmuludq xmm5, xmm3; // a3 a3 * b2 b1
      pmuludq xmm2, xmm4; // a3 a3 * b4 b3
      movdqu xmm6, [ecx + edi * 8 + 24];
      paddq xmm6, xmm5; 
      movdqu xmm7, [ecx + edi * 8 + 40];
      paddq xmm7, xmm2;
      movdqu [ecx + edi * 8 + 24], xmm6;
      movdqu [ecx + edi * 8 + 40], xmm7;

      add eax, 4;
      add edi, 4;
      jmp inner;
  end_inner:
      pop ebx;
      add ebx, 4;
      jmp outer;
end_outer:
      // ��������� ���������
      mov eax, dword ptr [res];
      mov ebx, dword ptr [temp];
      mov edx, lenA;
      add edx, lenB;
      mov ecx, 0x3fffffff;
      movd mm3, ecx; 
      xor ecx, ecx;
      pxor mm0, mm0; // ������� � ����������� ����
carry_loop:
      cmp ecx, edx;
      je carry_end;
      movq mm1, [ebx + ecx * 8]; // ��������� ��������� 64 ����
      movq mm2, mm1; 
      pand mm2, mm3; // � ������� - ������ 30 ���
      paddq mm0, mm2; // ���������� �������
      movq mm2, mm0;
      pand mm2, mm3; // ��������� 30 ���
      movd [eax + ecx * 4], mm2; // ������� ���������
      psrlq mm0, 30; 
      psrlq mm1, 30;
      paddq mm0, mm1;
      add ecx, 1;
      push edx;
      sub edx, 1;
      cmp ecx, edx;
      pop edx;
      jne carry_loop;
      movq2dq xmm0, mm0;
      ptest xmm0, xmm0;

      jnz carry_loop;
carry_end:
      mov tmpLen, ecx;
      emms;
  }
  return tmpLen;
  /*unsigned long long temp = 0;
  unsigned long long carry = 0;
  uint i;
  uint k;
  for(i = 0; i < lenA; i++)
  {
    for(uint j = 0; j < lenB; j++)
    {
      temp = __emulu(a[i], b[j]) + res[i + j];
      res[i + j] = word(temp);
      carry = (temp >> (NUM_BIT_SIZE));
      for(k = 1; carry; k++)
      {
        res[i + j + k] = res[i + j + k] + word(carry);
        carry = (res[i + j + k] < carry) ? 1 : 0;
      }
    }
  }
  for(i = lenA + lenB; i > 1 && res[i - 1] == 0; i--);
  return i;*/
}
// ��������� �� ��������� � �������� �������
uint store(word *a, word *res, uint len)
{
  uint res0;
  _asm{
    mov ecx, dword ptr [res]
    mov eax, dword ptr [a]
    xor edx, edx  
    xor edi, edi
simple_block_add:             // �������� ������ �� ������ 32-������� �����
    push eax
    mov eax, dword ptr [eax + edx * 4]
    mov ebx, dword ptr [ecx + edx * 4]
    add eax, ebx
    push edx
    add eax, edi
    mov edx, eax
    shr edx, 30
    test edx, edx    
    pop edx
    mov edi, 0
    jz end_simple_loop        
    mov edi, 1
    shl eax, 2
    shr eax, 2
end_simple_loop:
    mov dword ptr [ecx + edx * 4], eax
    pop eax
    inc edx
    push edx
    push eax
    inc edx
    mov eax, len
    cmp edx, eax
    pop eax
    pop edx
    jbe simple_block_add
  test edi, edi
  jz end
add_simple_loop:            // ������ � ����� �� ������ 32-������� �����
    push eax
    mov eax, dword ptr [ecx + edx * 4]
    add eax, edi
    mov ebx, eax
    shr ebx, 30
    test ebx, ebx
    mov edi, 0
    jz end_add
    mov edi, 1
    shl eax, 2
    shr eax, 2
end_add:
    mov dword ptr [ecx + edx * 4], eax
    pop eax
    inc edx
    test edi, edi 
    jnz add_simple_loop
end:
  mov res0, edx
  }
  return res0;
}

/* ���. ������� ��� ������ � �������� ������� */
// ������ �����
Longnum abs(Longnum& l)
{
  return Longnum(l.digits, l.len, abs(l.sign));
}
// ����� �����, ������� ��������� '0x', ��� ��� ����� � 16-������ ����. ����� ����������� ���������
std::ostream& operator<<(std::ostream& out, const Longnum& l)
{
    
  std::bitset<30> tmp;
  std::string s, inc;
  std::stringstream ss;

  if (l.sign == S_ZERO)
  {
    out << std::hex << "0x0";
    return out;
  }
  out << std::hex << "0x";
  for (uint i = 0; i < l.len; ++i)
  {
    tmp = std::bitset<30>((unsigned __int64)(l.digits[i]));
    inc = tmp.to_string();
    std::reverse(inc.begin(), inc.end());
    s.append(inc);
  }
  uint size = s.length() / 32;
  uint lastSize = s.length() - size * 32;
  inc = s.substr(s.length() - lastSize, lastSize);
  s.erase(s.length() - lastSize, lastSize);
  std::reverse(inc.begin(), inc.end());
  if (std::bitset<32>(inc).to_ulong() != 0)
    out << std::hex << std::bitset<32>(inc).to_ulong() << ' ';
  else
  {
    inc = s.substr(s.length() - 32, 32);
    s.erase(s.length() - 32, 32);
    std::reverse(inc.begin(), inc.end());
    out << std::hex << std::bitset<32>(inc).to_ulong() << ' ';
    --size;
  }
  for (uint i = 0; i < size; ++i)
  {
    inc = s.substr(s.length() - 32, 32);
    std::reverse(inc.begin(), inc.end());
    out << std::hex << std::setfill('0') << std::setw(4 * 2) << std::bitset<32>(inc).to_ulong() << ' ';
    s.erase(s.length() - 32, 32);
  }

  return out;
}   
// ���� ����� �� ������ �����, ���� �������� �� �������� �������
std::istream& operator>>(std::istream& is, Longnum& l)
{
  word sign;
  uint length;
  is >> length;
  if(l.digits != nullptr || l.len != length)
  {
    delete[] l.digits;
    l.digits = nullptr;
  }
  int i = length - 1;
  
  l.digits = new word[length];  
  if(l.digits == nullptr)
      throw std::exception("Bad Alloc");
  is >> sign;
  if(sign == '-')
    l.sign = S_NEG;
  else
  {
    l.sign = S_POS;
    l.digits[i--] = sign;
  }
  l.len = length;
  for(; i >= 0; i--)
  {
    is >> l.digits[i];
  }
  return is;
}