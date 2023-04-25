namespace wee {
  template <typename T>
  class circulator
  {
    T _beg;
    T _end;
    T _cur;

  public:
      circulator(const circulator& other) 
      : _beg(other._beg)
      , _end(other._end)
      , _cur(other._cur)
      {

      }
    circulator(T begin, T end) 
    : _beg(begin)
    , _end(end)
    , _cur(begin) 
    {
    }

    bool operator == (const circulator& other) const;
    bool operator != (const circulator& other) const;

    typename T::reference operator * () const { return *_cur; }

    circulator& operator ++ () { if(++_cur == _end) _cur = _beg; return *this; }
    circulator operator ++ (int) { circulator copy(*this); ++*this; return copy; };
    circulator& operator -- () { if(_cur == _beg) _cur = _end; --_cur; return *this; };
    circulator operator -- (int) { circulator copy(*this); --*this; return copy; };
  };


}
