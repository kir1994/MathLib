 #ifndef _CMATRIX_H
  #define _CMATRIX_H
  //#include <iostream>
//#pragma warning (disable:1624)
  template<class T>
  class CMatrix {
  public:
    long m_;
    long n_;
    T *matr_;
  
	CMatrix(): m_(0), n_(0), matr_(nullptr) {}
	CMatrix(const CMatrix& m): m_(m.m_), n_(m.n_)
    {
      long size = m_ * n_;
      this->matr_ = new T[size];
      memcpy(this->matr_, m.matr_, sizeof(T) * size);
    }
	CMatrix(T* elems, long m, long n): m_(m), n_(n)
    {
      long size = m_ * n_;
      this->matr_ = new T[size];
      memcpy(this->matr_, elems, sizeof(T) * size);
    }
	CMatrix(CMatrix&& m): m_(m.m_), n_(m.n_), matr(m.matr)
    {
      m.clear();
    }
	CMatrix& operator=(CMatrix&& m)
    {
      this->m_ = m.m_;
      this->n_ = m.n_;
      this->matr_ = m.matr_;
      m.clear();
      return *this;
    }
	CMatrix& operator=(const CMatrix& m)
    {
      this->m_ = m.m_;
      this->n_ = m.n_;
      long size = m_ * n_;
      this->matr_ = new T[size];
      memcpy(this->matr_, m.matr_, sizeof(T) * size);

      return *this;
    }
    ~CMatrix()
    {
      if(matr_ != nullptr)
        delete[] matr_;
      this->clear();
    }
    void clear()
    {
      m_ = 0;
      n_ = 0;
      matr_ = nullptr;
    }
    int* decomp_LUP()
    {
      int *pivot = new int[n_];
      T p;
      long k2;
      T tmp2;
      int tmp1;
      for(long i = 0; i < n_; i++)
        pivot[i] = i;
      for(long k = 0; k < n_; k++){
        p = 0;
        for(long i = k; i < n_; i++){
          if(abs(matr_[i * n_ + k]) > p){
            p = abs(matr_[i * n_ + k]);
            k2 = i;
          }
        }
        if(p == 0)
          return nullptr;
        tmp1 = pivot[k];
        pivot[k] = pivot[k2];
        pivot[k2] = tmp1;
        for(long i = 0; i < n_; i++)
        {
          tmp2 = matr_[k * n_ + i];
          matr_[k * n_ + i] = matr_[k2 * n_ + i];
          matr_[k2 * n_ + i] = tmp2;
        }
        for(long i = k + 1; i < n_; i++)
        {
          matr_[i * n_ + k] = matr_[i * n_ + k] / matr_[k * n_ + k];
          for(long j = k + 1; j < n_; j++)
            matr_[i * n_ + j] = matr_[i * n_ + j] - matr_[i * n_ + k] * matr_[k * n_ + j];
        }
      }
      return pivot;
    }

    friend T * solve_LUP(const CMatrix<T>& m, int *pivot, T *b);
  };

  template<class T>
  T * solve_LUP(const CMatrix<T>& m, int *pivot, T *b)
  {
    T* y = new T[m.n_];
    T* x = new T[m.n_];
    for(long i = 0; i < m.n_; i++)
    {
      y[i] = b[pivot[i]];
      for(long j = 0; j < i; j++)
        y[i] = y[i] - m.matr_[i * m.n_ + j] * y[j];
    }
    for(long i = m.n_ - 1; i >= 0; i--)
    {
      x[i] = y[i];
      for(long j = i + 1; j < m.n_; j++)
        x[i] = x[i] - m.matr_[i * m.n_ + j] * x[j];
      x[i] = x[i] / m.matr_[i * m.n_ + i];
    }
    delete[] y;
    return x;
  }

#endif