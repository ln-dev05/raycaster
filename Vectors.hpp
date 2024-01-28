#if !defined(Vectors_hpp)
#define Vectors_hpp

struct Vec2INT {
  int x, y;
  public:
    Vec2INT(const int rx, const int ry) {
      x = rx;
      y = ry;
    }
};

struct Vec2 {
  double x, y;
    Vec2(const double rx, const double ry) {
    x = rx;
    y = ry;
  }
};

struct Vec3 {
  double x, y, z;
    Vec3(const double rx, const double ry, const double rz) {
    x = rx;
    y = ry;
    z = rz;
  }
};

#endif // Vectors_hpp
