#pragma warning(disable: 4996)
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include "math.h"
#include <ctime>
//#include "discrete_log.h"

int list[95] = {503, 509, 521, 523, 541
  , 547, 557, 563, 569, 571, 577, 587, 593, 599, 601
  , 607, 613, 617, 619, 631, 641, 643, 647, 653, 659
  , 661, 673, 677, 683, 691, 701, 709, 719, 727, 733
  , 739, 743, 751, 757, 761, 769, 773, 787, 797, 809
  , 811, 821, 823, 827, 829, 839, 853, 857, 859, 863
  , 877, 881, 883, 887, 907, 911, 919, 929, 937, 941
  , 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013
  , 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069
  , 1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151};

int main()
{  
  srand(time(0));
  clock_t tm;
  ofstream out("out.txt", ios::trunc);

  //int i = 4;
  //t = t * 0x3;
  for(uint j = 0; j < 10; ++j)
  {
    word *var2 = new word[512];
    for(uint i = 0; i < 512; ++i)
      var2[i] = ((rand() << 8) + rand()) % 0x3fffffff;
    //var2[0] <<= 1;
    Longnum t(var2, 256, S_POS);
    for(uint i = 1; i <= 20; ++i)
    {
      std::cout << "Threads: " << i << std::endl;
      tm = clock();
      rho_pollard(t, i);
      out << ((clock() - tm) / double(CLOCKS_PER_SEC)) << "\t";
    } 
    out << "\n";
  }
  out.close();
  // Variables used: 
/*    int x, y, z, t;          // Integers
    int *temp = nullptr;
    int *temp2 = nullptr;
//    time_t timer1, timer2;

    __int64 num, degree;  // Long long integers
    
    Gauss g1;             // Gaussian integers
    Gauss g2;
    Gauss g3; 
    Gauss g4;

    Polynom_GF2 p1;        // Polynoms from GF(2)
    Polynom_GF2 p2;
    Polynom_GF2 p3;
    Polynom_GF2 p4;
    Polynom_GF2 p5;

    Longnum ln1;
    Longnum ln2;

  bool flag = 1;
  while(flag)
  {
    system("cls");
    cout << "Made by Kirill Ivanov\n\n"
         << "Menu: \n"
         << "0 - Exit\n"
         << "1 - GCD & Bezout\n"
         << "2 - Comparison of first degree\n"
         << "3 - Solve \"big number to big degree\" \n"
         << "4 - Work with primitive polynoms\n"
         << "5 - Chinese remander theorem\n"
         << "6 - Factorization\n"
         << "Your choice: \n";
    switch(getch())
    {
    case '0':
      flag = 0;
      break;
    case '1':
      system("cls");
      cout << "GCD: \n"
           << "0 - Exit\n"
           << "1 - Integers\n"
           << "2 - Polynoms\n"
           << "3 - Gaussian integers\n"
           << "Your choice: \n";
      switch(getch())
      {
      case '0':
        break;
      case '1':
        cout << "Input integers: ";
        cin >> x >> y;
        cout << gcdBezout<int>(x, y, z, t) << endl;
        cout << z << "\n" << t;
        cout << "Press any key to continue...";
        if(getch())
          break;
      case '2':
        p1 = Polynom_GF2::createPolynom_GF2();
        p2 = Polynom_GF2::createPolynom_GF2();
        cout << gcdBezout<Polynom_GF2>(p1, p2, p3, p4) << endl;
        cout << p3 << "\n" << p4;
        cout << "Press any key to continue...";
        if(getch())
          break;
      case '3':
        cout << "Enter first Gaussian integer (a, b): \n";
        cin >> x >> y;
        g1 = Gauss(x, y);
        cout << "Enter second Gaussian integer (a, b): \n";
        cin >> x >> y;
        g2 = Gauss(x, y);
        cout << gcdBezout<Gauss>(g1, g2, g3, g4) << endl;
        cout << g3 << "\n" << g4;
        cout << "Press any key to continue...";
        if(getch())
          break;
      default:
        cout << "\n Wrong key. Try again\n"
           << "Press any key to continue...";
        if(getch())
          break;
      }
      break;
    case '2':
      cout << "Comparison ax = b mod m\n"
           << "Enter a, b, m: ";
      cin >> x >> y >> z;
      cout << endl;
      cmp_one(x, y, z, temp, t);
      for(int i = 0; i < t; i++)
        cout << temp[i] << endl;
      delete temp;
      temp = nullptr;
      cout << "Press any key to continue...";
      if(getch())
        break;
    case '3':
      cout << "Solution of a^b mod c\n"
           << "Enter a, b, c: ";     
      cin >> num >> degree >> x;
      cout << deg(num, degree, x);
      cout << "Press any key to continue...";
      if(getch())
        break;
    case '4':
      cout << "Solving (x^a + x^b)/(x^c + x^d) \n"
           << "Enter a, b, c, d: ";
      cin >> x >> y >> z >> t;
      cout << "\nEnter primitive polynom: ";
      p3 = Polynom_GF2::createPolynom_GF2();
      p1 = Polynom_GF2(1, 2).modDeg(x, p3) + Polynom_GF2(1, 2).modDeg(y, p3);
      p2 = Polynom_GF2(1, 2).modDeg(z, p3) + Polynom_GF2(1, 2).modDeg(t, p3);
      p4 = Polynom_GF2(0, 1);
      p5 = Polynom_GF2(1, 2);
      while(p2.modMult(p4, p3) != p1)
        p4 = p4.modMult(p5, p3);
      cout << "\n" << p4;
      cout << "Press any key to continue...";
      if(getch())
        break;
    case '5':
      cout << "Input number of comparisons";
      cin >> t;
      temp = new int[t];
      temp2 = new int[t];
      for(int i = 0; i < t; i++)
      {
        cout << "\nEnter x, module";
        cin >> temp[i] >> temp2[i];
      }
      cout << endl;
      cout << chinRemTheorem(temp, temp2, t);
      cout << "Press any key to continue...";
      delete[] temp;
      delete[] temp2;
      if(getch())
        break;
    case '6':
      cout << "Input number: ";
      cin >> ln1;
      pollard_rho(ln1);
      cout << "Press any key to continue...";
      if(getch())
        break;
    default:
      cout << "\n Wrong key. Try again\n"
           << "Press any key to continue...";
      if(getch())
        break;
    }
  }*/
  //cout << rho_log(8, 2, 19);

  return 0;
}