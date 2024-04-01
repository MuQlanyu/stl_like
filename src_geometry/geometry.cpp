#include "geometry.h"

///Point///

bool operator==(const Point& left, const Point& right) {
  return (std::abs(left.x - right.x) < Math::epsilon) &&
      (std::abs(left.y - right.y) < Math::epsilon);
}

bool operator!=(const Point& left, const Point& right) {
  return !(left == right);
}

Point operator+(const Point& left, const Point& right) {
  return {left.x + right.x, left.y + right.y};
}

Point operator-(const Point& left, const Point& right) {
  return {left.x - right.x, left.y - right.y};
}

Point operator*(const Point& point, double lambda) {
  return {point.x * lambda, point.y * lambda};
}

Point operator/(const Point& point, double lambda) {
  return {point.x / lambda, point.y / lambda};
}

double Distance(const Point& left, const Point& right) {
  return std::sqrt(std::pow(left.x - right.x, 2) + std::pow(left.y - right.y, 2));
}

void Point::Rotate(const Point& center, double angle) {
  Point vector = *this - center;
  double angle_radian = Math::pi * angle / 180;
  x = center.x + vector.x * std::cos(angle_radian) - vector.y * std::sin(angle_radian);
  y = center.y + vector.x * std::sin(angle_radian) + vector.y * std::cos(angle_radian);
}

void Point::Scale(const Point& center, double coefficient) {
  *this = center + (*this - center) * coefficient;
}

void Point::Reflect(const Point& center) {
  Scale(center, -1);
}

void Reflect(Point& point, const Line& line) {
  point.Reflect(line.Intersection(line.Normal(point)));
}

double Point::VectorSize() const {
  return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}

double Cos(const Point& first, const Point& second, const Point& third) {
  Point first_segment = second - first;
  Point second_segment = third - second;
  return (first_segment.x * second_segment.x + first_segment.y * second_segment.y) /
      first_segment.VectorSize() / second_segment.VectorSize();
}

double Sin(const Point& first, const Point& second, const Point& third) {
  Point first_segment = second - first;
  Point second_segment = third - second;
  return (first_segment.x * second_segment.y - second_segment.x * first_segment.y) /
      first_segment.VectorSize() / second_segment.VectorSize();
}

///Line///

Line::Line(const Point& first, const Point& second) :
    Line(first, (second.y - first.y) / (second.x - first.x)) {
  if (first.x == second.x) {
    //Вертикальная прямая
    direction_[0] = 0;
    direction_[1] = 1;
  }
}

Line::Line(const Point& point, double degree) : starting_point_(point), direction_{1, degree} {}

Line::Line(double degree, double shift) : Line(Point(0, shift), degree) {}

bool operator==(const Line& left, const Line& right) {
  Line start_points_line = Line(left.starting_point_, right.starting_point_);
  return std::abs(left.direction_[0] - start_points_line.direction_[0]) < Math::epsilon &&
      std::abs(left.direction_[1] - start_points_line.direction_[1]) < Math::epsilon &&
      std::abs(right.direction_[0] - start_points_line.direction_[0]) < Math::epsilon &&
      std::abs(right.direction_[1] - start_points_line.direction_[1]) < Math::epsilon;
}

bool operator!=(const Line& left, const Line& right) {
  return !(left == right);
}

Line Line::Normal(const Point& start_point) const {
  if (direction_[1] == 0) {
    return {start_point, start_point + Point(0, 1)};
  }
  return {start_point, -1 / direction_[1]};
}

Point Line::Intersection(const Line& other) const {
  //Предполагается, что они пересекаются
  double coefficient =
      (other.direction_[0] * (starting_point_.y - other.starting_point_.y) - other.direction_[1] * (starting_point_.x - other.starting_point_.x)) /
          (direction_[0] * other.direction_[1] - other.direction_[0] * direction_[1]);
  return starting_point_ + Point(direction_[0] * coefficient, direction_[1] * coefficient);
}

///Polygon///

template<class... Args>
Polygon::Polygon(const Point& p, Args... args) : Polygon(args...) {
  vertices_.push_back(p);
}

Polygon::Polygon(const std::vector<Point>& vertices) : vertices_(vertices) {
}

bool Polygon::isConvex() const {
  Point first_segment = vertices_[0] - vertices_.back();
  Point second_segment = vertices_[1] - vertices_[0];
  bool is_positive = first_segment.x * second_segment.y - second_segment.x * first_segment.y > 0;
  for (size_t i = 0; i < vertices_.size() - 1; ++i) {
    if ((Sin(vertices_[i], vertices_[i + 1], vertices_[(i + 2) % vertices_.size()]) > 0) != is_positive) {
      return false;
    }
  }
  return true;
}

double Polygon::perimeter() const {
  double perimeter = Distance(vertices_.back(), vertices_[0]);
  for (size_t i = 1; i < vertices_.size(); ++i) {
    perimeter += Distance(vertices_[i], vertices_[i - 1]);
  }
  return perimeter;
}

double Polygon::area() const {
  double area = vertices_.back().x * vertices_[0].y - vertices_[0].x * vertices_.back().y;
  for (size_t i = 1; i < vertices_.size(); ++i) {
    area += vertices_[i - 1].x * vertices_[i].y - vertices_[i].x * vertices_[i - 1].y;
  }
  return std::abs(area) / 2;
}

bool Polygon::operator==(const Shape& other) const {
  const Polygon* another = dynamic_cast<const Polygon*>(&other);
  if (another == nullptr or vertices_.size() != another->vertices_.size()) {
    return false;
  }
  int init_point = -1;
  size_t size = verticesCount();
  for (size_t i = 0; i < size; ++i) {
    if (another->vertices_[i] == vertices_[0]) {
      init_point = i;
      break;
    }
  }
  if (init_point == -1) {
    return false;
  }
  if (vertices_[1] == another->vertices_[(init_point + 1) % size]) {
    for (size_t i = 1; i < size; ++i) {
      if (vertices_[i] != another->vertices_[(init_point + i) % size]) {
        return false;
      }
    }
  } else {
    for (size_t i = 1; i < size; ++i) {
      if (vertices_[i] != another->vertices_[(init_point - i) % size]) {
        return false;
      }
    }
  }
  return true;
}

bool Polygon::operator!=(const Shape& other) const {
  return !(*this == other);
}

bool operator==(const Polygon& left, const Polygon& right) {
  return left == static_cast<const Shape&>(right);
}

bool operator!=(const Polygon& left, const Polygon& right) {
  return left != static_cast<const Shape&>(right);
}

bool Polygon::AngleComparison(const Polygon& other, int side, size_t this_ind, size_t other_ind, int* sin_side) const {
  size_t size = vertices_.size();
  double first_cos = Cos(vertices_[this_ind % size], vertices_[(this_ind + 1) % size],
                         vertices_[(this_ind + 2) % size]);
  double second_cos = Cos(other.vertices_[(side * other_ind + size) % size],
                          other.vertices_[(side * (other_ind + 1) + 2 * size) % size],
                          other.vertices_[(side * (other_ind + 2) + 2 * size) % size]);
  double first_sin = Sin(vertices_[this_ind % size], vertices_[(this_ind + 1) % size],
                         vertices_[(this_ind + 2) % size]);
  double second_sin = Sin(other.vertices_[(side * other_ind + size) % size],
                          other.vertices_[(side * (other_ind + 1) + 2 * size) % size],
                          other.vertices_[(side * (other_ind + 2) + 2 * size) % size]);
  if (*sin_side == 0) {
    *sin_side = (first_sin / second_sin > 0 ? 1 : -1);
  }
  return std::abs(first_cos - second_cos) < Math::epsilon &&
      std::abs(first_sin / second_sin - *sin_side) < Math::epsilon;
}

bool Polygon::OneSideCheck(const Polygon& other, int side, size_t init_point) const {
  int size = vertices_.size();
  double first_segment = Distance(vertices_[(init_point + 1) % size], vertices_[init_point]);
  double second_segment = Distance(other.vertices_[(side + size) % size], other.vertices_[0]);
  if (std::abs(first_segment - second_segment) > Math::epsilon) {
    return false;
  }
  int sin_side = 0;
  AngleComparison(other, side, init_point, 0, &sin_side);
  for (int i = 0; i < size; ++i) {
    first_segment = Distance(vertices_[(init_point + 2 + i) % size], vertices_[(init_point + 1 + i) % size]);
    second_segment =
        Distance(other.vertices_[(side * (i + 2) + 2 * size) % size],
                 other.vertices_[(side * (i + 1) + 2 * size) % size]);
    if (std::abs(first_segment - second_segment) > Math::epsilon) {
      return false;
    }
    if (!AngleComparison(other, side, init_point + i, i, &sin_side)) {
      return false;
    }
  }
  return true;
}

bool Polygon::SimilarCheck(const Polygon& other, int side, size_t init_point) const {
  int size = vertices_.size();
  double first_segment = Distance(vertices_[(init_point + 1) % size], vertices_[init_point]);
  double second_segment = Distance(other.vertices_[(side + size) % size], other.vertices_[0]);
  double coef = first_segment / second_segment;
  int sin_side = 0;
  AngleComparison(other, side, init_point, 0, &sin_side);
  for (int i = 0; i < size; ++i) {
    first_segment = Distance(vertices_[(init_point + i + 2) % size],
                             vertices_[(init_point + i + 1) % size]);
    second_segment = Distance(other.vertices_[(side * (i + 2) + 2 * size) % size],
                              other.vertices_[(side * (i + 1) + 2 * size) % size]);
    if (std::abs(first_segment / second_segment - coef) > Math::epsilon) {
      return false;
    }
    if (!AngleComparison(other, side, init_point + i, i, &sin_side)) {
      return false;
    }
  }
  return true;
}

bool Polygon::isCongruentTo(const Shape& other) const {
  const Polygon* another = dynamic_cast<const Polygon*>(&other);
  if (another == nullptr or vertices_.size() != another->vertices_.size()) {
    return false;
  }
  size_t size = vertices_.size();
  for (size_t i = 0; i < size; ++i) {
    if (OneSideCheck(*another, 1, i) || OneSideCheck(*another, -1, i)) {
      return true;
    }
  }
  return false;
}

bool Polygon::isSimilarTo(const Shape& other) const {
  const Polygon* another = dynamic_cast<const Polygon*>(&other);
  if (another == nullptr or vertices_.size() != another->vertices_.size()) {
    return false;
  }
  size_t size = vertices_.size();
  for (size_t i = 0; i < size; ++i) {
    if (SimilarCheck(*another, 1, i) || SimilarCheck(*another, -1, i)) {
      return true;
    }
  }
  return false;
}

bool Polygon::containsPoint(const Point& point) const {
  size_t size = vertices_.size();
  Point first_vector, second_vector;
  double sin, cos, angel_sum = 0;
  for (size_t i = 0; i < vertices_.size(); ++i) {
    if (std::abs(Distance(vertices_[(i - 1 + size) % size], vertices_[i]) -
        Distance(vertices_[(i - 1 + size) % size], point) - Distance(vertices_[i], point)) < Math::epsilon) {
      return true;
    }
    first_vector = vertices_[(i - 1 + size) % size] - point;
    second_vector = vertices_[i] - point;
    cos = Cos(point + point - vertices_[(i - 1 + size) % size], point, vertices_[i]);
    sin = first_vector.x * second_vector.y - first_vector.y * second_vector.x;
    angel_sum += std::acos(cos) * (sin > 0 ? 1 : -1);
  }
  if (std::abs(angel_sum) < Math::epsilon) {
    return false;
  }
  return true;
}

void Polygon::rotate(const Point& center, double angle) {
  for (auto& vertice : vertices_) {
    vertice.Rotate(center, angle);
  }
}

void Polygon::reflect(const Point& center) {
  for (auto& vertice : vertices_) {
    vertice.Reflect(center);
  }
}

void Polygon::reflect(const Line& axis) {
  for (Point& vertice : vertices_) {
    Reflect(vertice, axis);
  }
}

void Polygon::scale(const Point& center, double coefficient) {
  for (auto& vertice : vertices_) {
    vertice.Scale(center, coefficient);
  }
}

///Ellipse///

double Ellipse::getMajorAxis() const {
  return major_axis_;
}

double Ellipse::getMinorAxis() const {
  return std::sqrt(std::pow(major_axis_, 2) -
      std::pow(Distance(center(), first_focus_), 2));
}

Point Ellipse::center() const {
  return {(first_focus_.x + second_focus_.x) / 2,
          (first_focus_.y + second_focus_.y) / 2};
}

double Ellipse::eccentricity() const {
  return Distance(first_focus_, center()) / major_axis_;
}

std::pair<Line, Line> Ellipse::directrices() const {
  Point core = center();
  double directrices = major_axis_ / eccentricity();
  if (first_focus_.y == second_focus_.y) {
    return {Line(Point(directrices + core.x, first_focus_.y),
                 Point(directrices + core.x, first_focus_.y + 1)),
            Line(Point(-directrices + core.x, first_focus_.y),
                 Point(-directrices + core.x, first_focus_.y + 1))};
  }
  double degree = (first_focus_.x - core.x) / (first_focus_.y - core.y);
  double cos = Distance(core, first_focus_) / (first_focus_.x - core.x);
  double sin = Distance(core, first_focus_) / (first_focus_.y - core.y);
  return {Line(Point(core.x + cos * directrices, core.y + sin * directrices), degree),
          Line(Point(core.x - cos * directrices, core.y - sin * directrices), degree)};
}

double Ellipse::perimeter() const {
  double minor_axis = getMinorAxis();
  double coef = 3 * std::pow(major_axis_ - minor_axis, 2) / std::pow(major_axis_ + minor_axis, 2);
  return Math::pi * (major_axis_ + minor_axis) * (1 + coef / (10 + std::sqrt(4 - coef)));
}

double Ellipse::area() const {
  return Math::pi * major_axis_ * getMinorAxis();
}

bool Ellipse::operator==(const Shape& other) const {
  const Ellipse* another = dynamic_cast<const Ellipse*>(&other);
  if (another == nullptr) {
    return false;
  }
  if ((first_focus_ == another->first_focus_ && second_focus_ == another->second_focus_) ||
      (first_focus_ == another->second_focus_ && second_focus_ == another->first_focus_)) {
    if (std::abs(major_axis_ - another->major_axis_) < Math::epsilon)
      return true;
  }
  return false;
}

bool Ellipse::operator!=(const Shape& other) const {
  return !(*this == other);
}

bool operator==(const Ellipse& left, const Ellipse& right) {
  return left == static_cast<const Shape&>(right);
}

bool operator!=(const Ellipse& left, const Ellipse& right) {
  return left != static_cast<const Shape&>(right);
}

bool Ellipse::isCongruentTo(const Shape& other) const {
  const Ellipse* another = dynamic_cast<const Ellipse*>(&other);
  if (another == nullptr) {
    return false;
  }
  return (major_axis_ == another->major_axis_) && (getMinorAxis() == another->getMinorAxis());
}

bool Ellipse::isSimilarTo(const Shape& other) const {
  const Ellipse* another = dynamic_cast<const Ellipse*>(&other);
  if (another == nullptr) {
    return false;
  }
  return std::abs(major_axis_ / getMinorAxis() - another->major_axis_ / another->getMinorAxis()) < Math::epsilon;
}

bool Ellipse::containsPoint(const Point& point) const {
  return Distance(point, first_focus_) + Distance(point, second_focus_) <= 2 * major_axis_;
}

void Ellipse::rotate(const Point& center, double angle) {
  first_focus_.Rotate(center, angle);
  second_focus_.Rotate(center, angle);
}

void Ellipse::reflect(const Point& center) {
  first_focus_.Reflect(center);
  second_focus_.Reflect(center);
}

void Ellipse::reflect(const Line& axis) {
  Reflect(first_focus_, axis);
  Reflect(second_focus_, axis);
}

void Ellipse::scale(const Point& center, double coefficient) {
  first_focus_.Scale(center, coefficient);
  second_focus_.Scale(center, coefficient);
  major_axis_ *= coefficient;
}

///Rectangle///

Point Rectangle::FindPoint(const Point& first, const Point& second, double ratio) const {
  double segment = Distance(first, second);
  double lesser_side = segment / std::sqrt(1 + ratio * ratio);
  lesser_side *= (ratio > 1 ? 1 : ratio);
  double segment_lesser = lesser_side * lesser_side / segment;
  Point vector = second - first;
  Point perpendicular_root = first + ((vector * segment_lesser) / segment);
  Point normal = Point(-vector.y, vector.x);
  return perpendicular_root + (normal / normal.VectorSize() * std::sqrt(segment_lesser * (segment - segment_lesser)));
}

Rectangle::Rectangle(const Point& first, const Point& second, double ratio) :
    Polygon(first, FindPoint(first, second, ratio), second, FindPoint(second, first, ratio)) {}

Point Rectangle::center() const {
  return {(vertices_[0].x + vertices_[2].x) / 2,
          (vertices_[0].y + vertices_[2].y) / 2};
}

std::pair<Line, Line> Rectangle::diagonals() const {
  return {Line(vertices_[0], vertices_[2]),
          Line(vertices_[1], vertices_[3])};
}

///Square///

Square::Square(const Point& first, const Point& second) : Rectangle(first, second, 1) {}

Circle Square::circumscribedCircle() const {
  return {center(), Distance(center(), vertices_[0])};
}

Circle Square::inscribedCircle() const {
  return {center(), Distance(vertices_[0], vertices_[1]) / 2};
}

///Triangle///

Triangle::Triangle(const Point& first, const Point& second, const Point& third)
    : Polygon(first, second, third) {};

Circle Triangle::circumscribedCircle() const {
  Line first_mid_perpendicular =
      Line(vertices_[0], vertices_[1]).Normal((vertices_[0] + vertices_[1]) / 2);
  Line second_mid_perpendicular =
      Line(vertices_[0], vertices_[2]).Normal((vertices_[0] + vertices_[2]) / 2);
  Point center = first_mid_perpendicular.Intersection(second_mid_perpendicular);
  return {center, Distance(center, vertices_[0])};
}

Circle Triangle::inscribedCircle() const {
  double first_segment = Distance(vertices_[1], vertices_[2]);
  double second_segment = Distance(vertices_[0], vertices_[2]);
  double third_segment = Distance(vertices_[0], vertices_[1]);
  double coef_first = second_segment / (second_segment + third_segment);
  double coef_second = first_segment / (first_segment + third_segment);
  Line first_bisector = Line(vertices_[0], vertices_[2] +
      (vertices_[1] - vertices_[2]) * coef_first);
  Line second_bisector = Line(vertices_[1], vertices_[2] +
      (vertices_[0] - vertices_[2]) * coef_second);
  Point center = first_bisector.Intersection(second_bisector);
  Line line = Line(vertices_[0], vertices_[1]);
  return {center, Distance(center, line.Normal(center).Intersection(line))};
}

Point Triangle::centroid() const {
  Line first_median = Line(vertices_[0], (vertices_[1] + vertices_[2]) / 2);
  Line second_median = Line(vertices_[1], (vertices_[0] + vertices_[2]) / 2);
  return first_median.Intersection(second_median);
}

Point Triangle::orthocenter() const {
  Line first_perpendicular =
      Line(vertices_[1], vertices_[2]).Normal(vertices_[0]);
  Line second_perpendicular =
      Line(vertices_[0], vertices_[2]).Normal(vertices_[1]);
  return first_perpendicular.Intersection(second_perpendicular);
}

Line Triangle::EulerLine() const {
  return {centroid(), orthocenter()};
}

Circle Triangle::ninePointsCircle() const {
  Triangle mid_triangle = Triangle((vertices_[1] + vertices_[2]) / 2,
                                   (vertices_[0] + vertices_[2]) / 2,
                                   (vertices_[0] + vertices_[1]) / 2);
  return mid_triangle.circumscribedCircle();
}