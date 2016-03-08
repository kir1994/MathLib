#pragma once 

#include <iostream>

#define MSIZE_INC 3
#define S_POS 1
#define S_NEG -1
#define S_ZERO 0

typedef unsigned long word;
typedef unsigned int uint;

const uint MAX_DEG_COL = 7;
const uint NUM_SIZE = sizeof(word);
const uint NUM_BIT_SIZE = 30;//(NUM_SIZE << 3);
const unsigned long long RADIX = (1ull << NUM_BIT_SIZE);
//const int BETA = -2;

class Longnum
{
  word *digits;
  uint len;
  uint max_size;
  char sign;
public:
  explicit Longnum();
  explicit Longnum(word* newDigits, uint newlen);
  explicit Longnum(const word* newDigits, uint newLen, char newSign);
  explicit Longnum(word* newDigits, uint newLen, uint newMSize, char newSign);
  explicit Longnum(word num);
  Longnum(const Longnum& l);
  Longnum(Longnum&& l);
  ~Longnum();

  void clear();
  word operator[](int i);
  Longnum& operator=(const Longnum& l);
  Longnum& operator=(Longnum&& l);
  Longnum operator+(Longnum& l);
  Longnum& operator+=(Longnum& l);
  Longnum& operator+=(word l);
  Longnum operator-(Longnum& l);
  Longnum& operator-=(Longnum& l);
  Longnum& operator-=(word l);
  Longnum operator*(Longnum& l);
  Longnum operator/(Longnum& l);
  Longnum operator%(Longnum& l);
  Longnum& operator*=(Longnum& l);
  bool operator>(Longnum& l);
  bool operator>=(Longnum& l);
  bool operator<(Longnum& l);
  bool operator<(word l);
  bool operator!=(Longnum& l);
  bool operator==(Longnum& l);
  bool operator==(word num);
  bool operator!=(word num);
  Longnum leftShift(word i);
  Longnum rightShift(word i);
  Longnum operator>>(word i);
  Longnum operator<<(word i);
  Longnum operator-();
  Longnum operator+(word l);
  Longnum operator*(word l);
  Longnum operator/(word l);
  Longnum operator%(word l);
  Longnum div(Longnum& b, Longnum& r, Longnum& u);
  Longnum& divBarret(Longnum& b, Longnum& d, Longnum& r, Longnum& u);
  Longnum divKnuth(Longnum& b);
  Longnum& modMulBarrett(Longnum& b, Longnum& m, Longnum& z, Longnum& u);
  friend std::ostream& operator<<(std::ostream& os, const Longnum& l);
  friend std::istream& operator>>(std::istream& is, Longnum& l);
  friend Longnum abs(Longnum& l);
  friend Longnum rho_pollard(Longnum& n, uint thread_num);
  friend Longnum random(Longnum& n, Longnum& u);
};

uint add(word *a, word *b, word *res, uint lenA, uint lenB);
uint mul(word *a, word *b, word *res, uint lenA, uint lenB, word *temp);
uint sub(word *a, word *b, word *res, uint lenA, uint lenB);