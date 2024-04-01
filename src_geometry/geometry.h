#pragme once

#include <cmath>
#include <iostream>
#include <vector>

namespace Math {
const double epsilon = 0.000001; // 1e-6
const double pi = 3.1415926535;
}

//Иногда буду использовать как вектор
struct Point {
  Point(double x, double y) : x(x), y(y) {};
  Point() = default;

  void Rotate(const Point& center, double angle);
  void Scale(const Point& center, double coefficient);
  void Reflect(const Point& center);
  double VectorSize() const;

  double x = 0;
  double y = 0;
};

class Line {
 public:
  Line(const Point& first, const Point& second);
  Line(const Point& point, double degree);
  Line(double degree, double shift);
  Line() = default;

  Line Normal(const Point& start_point) const; //нормаль
  Point Intersection(const Line& other) const;
 private:
  Point starting_point_; //начальная точка
  std::vector<double> direction_;

  friend bool operator==(const Line& left, const Line& right);
};

class Shape {
 public:
  virtual double perimeter() const = 0;
  virtual double area() const = 0;
  virtual bool operator==(const Shape& other) const = 0;
  virtual bool operator!=(const Shape& other) const = 0;
  virtual bool isCongruentTo(const Shape& other) const = 0;
  virtual bool isSimilarTo(const Shape& other) const = 0;
  virtual bool containsPoint(const Point& point) const = 0;

  virtual void rotate(const Point& center, double angle) = 0;
  virtual void reflect(const Point& center) = 0;
  virtual void reflect(const Line& axis) = 0;
  virtual void scale(const Point& center, double coefficient) = 0;

  virtual ~Shape() = default;
};

class Polygon : public Shape {
 public:
  template<class... Args>
  Polygon(const Point& p, Args... args);
  Polygon(const std::vector<Point>& vertices);
  Polygon() = default;

  size_t verticesCount() const { return vertices_.size(); }
  std::vector<Point> getVertices() const { return vertices_; }
  bool isConvex() const;

  bool AngleComparison(const Polygon& other, int side, size_t this_ind, size_t other_ind, int* sin_side) const;
  bool OneSideCheck(const Polygon& other, int side, size_t init_point) const;
  bool SimilarCheck(const Polygon& other, int side, size_t init_point) const;

  double perimeter() const final;
  double area() const final;
  bool operator==(const Shape& other) const final;
  bool operator!=(const Shape& other) const final;
  bool isCongruentTo(const Shape& other) const final;
  bool isSimilarTo(const Shape& other) const final;
  bool containsPoint(const Point& point) const final;

  void rotate(const Point& center, double angle) final;
  void reflect(const Point& center) final;
  void reflect(const Line& axis) final;
  void scale(const Point& center, double coefficient) final;
 protected:
  std::vector<Point> vertices_;
};

class Ellipse : public Shape {
 public:
  Ellipse(const Point& first, const Point& second, double dist) :
      first_focus_(first), second_focus_(second), major_axis_(dist / 2) {}

  std::pair<Point, Point> focuses() { return {first_focus_, second_focus_}; }
  std::pair<Point, Point> focuses() const { return {first_focus_, second_focus_}; }

  Point center() const;
  double eccentricity() const;
  std::pair<Line, Line> directrices() const;

  double perimeter() const final;
  double area() const final;
  bool operator==(const Shape& other) const final;
  bool operator!=(const Shape& other) const final;
  bool isCongruentTo(const Shape& other) const final;
  bool isSimilarTo(const Shape& other) const final;
  bool containsPoint(const Point& point) const final;

  void rotate(const Point& center, double angle) final;
  void reflect(const Point& center) final;
  void reflect(const Line& axis) final;
  void scale(const Point& center, double coefficient) final;

  double getMajorAxis() const;
  double getMinorAxis() const;
 private:
  Point first_focus_;
  Point second_focus_;
  double major_axis_;
};

class Circle : public Ellipse {
 public:
  Circle(const Point& center, double radius) : Ellipse(center, center, 2 * radius) {}

  double radius() const { return getMajorAxis(); }
};

class Rectangle : public Polygon {
 public:
  Rectangle(const Point& first, const Point& second, double ratio);

  Point center() const;
  std::pair<Line, Line> diagonals() const;
 private:
  Point FindPoint(const Point& first, const Point& second, double ratio) const;
};

class Square : public Rectangle {
 public:
  Square(const Point& first, const Point& second);

  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
};

class Triangle : public Polygon {
 public:
  Triangle(const Point& first, const Point& second, const Point& third);

  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
  Point centroid() const;
  Point orthocenter() const;
  Line EulerLine() const;
  Circle ninePointsCircle() const;
};
