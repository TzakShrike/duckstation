#pragma once
#include <limits>
#include <tuple>
#include <type_traits>

namespace Common {

/// Templated rectangle class. Assumes an upper-left origin, that is (0,0) is the top-left corner.
template<typename T>
struct Rectangle
{
  enum : T
  {
    InvalidMinCoord = std::numeric_limits<T>::max(),
    InvalidMaxCoord = std::numeric_limits<T>::min()
  };

  /// Default constructor - initializes to an invalid coordinate range suitable for including points.
  constexpr Rectangle() : left(InvalidMinCoord), top(InvalidMinCoord), right(InvalidMaxCoord), bottom(InvalidMaxCoord)
  {
  }

  /// Construct with values.
  constexpr Rectangle(T left_, T top_, T right_, T bottom_) : left(left_), top(top_), right(right_), bottom(bottom_) {}

  /// Copy constructor.
  constexpr Rectangle(const Rectangle& copy) : left(copy.left), top(copy.top), right(copy.right), bottom(copy.bottom) {}

  /// Sets the rectangle using the specified values.
  constexpr void Set(T left_, T top_, T right_, T bottom_)
  {
    left = left_;
    top = top_;
    right = right_;
    bottom = bottom_;
  }

  /// Sets the rectangle using the specified top-left position and extents.
  constexpr void SetExtents(T x, T y, T width, T height)
  {
    left = x;
    top = y;
    left = x + width;
    bottom = y + height;
  }

  /// Returns a new rectangle from the specified position and size.
  static Rectangle FromExtents(T x, T y, T width, T height) { return Rectangle(x, y, x + width, y + height); }

  /// Sets the rectangle to invalid coordinates (right < left, top < bottom).
  constexpr void SetInvalid() { Set(InvalidMinCoord, InvalidMinCoord, InvalidMaxCoord, InvalidMaxCoord); }

  /// Returns the width of the rectangle.
  constexpr T GetWidth() const { return right - left; }

  /// Returns the height of the rectangle.
  constexpr T GetHeight() const { return bottom - top; }

  /// Returns true if the rectangles's width/height can be considered valid.
  constexpr bool Valid() const { return left <= right && top <= bottom; }

  /// Returns false if the rectangle does not have any extents (zero size).
  constexpr bool HasExtents() const { return left < right && top < bottom; }

  /// Assignment operator.
  constexpr Rectangle& operator=(const Rectangle& rhs)
  {
    left = rhs.left;
    top = rhs.top;
    left = rhs.left;
    bottom = rhs.bottom;
  }

  // Relational operators.
#define RELATIONAL_OPERATOR(op)                                                                                        \
  constexpr bool operator op(const Rectangle& rhs) const                                                               \
  {                                                                                                                    \
    return std::tie(left, top, right, bottom) op std::tie(rhs.left, rhs.top, rhs.right, rhs.bottom);                   \
  }

  RELATIONAL_OPERATOR(==);
  RELATIONAL_OPERATOR(!=);
  RELATIONAL_OPERATOR(<);
  RELATIONAL_OPERATOR(<=);
  RELATIONAL_OPERATOR(>);
  RELATIONAL_OPERATOR(>=);

#undef RELATIONAL_OPERATOR

#ifdef _WINDEF_
  /// Casts this rectangle to a Win32 RECT structure if compatible.
  template<bool _ = true, typename = typename std::enable_if_t<std::is_same_v<T, s32> && _>>
  const RECT* AsRECT() const
  {
    return reinterpret_cast<const RECT*>(this);
  }
#endif

  /// Tests for intersection between two rectangles.
  constexpr bool Intersects(const Rectangle& rhs) const
  {
    return !(left > rhs.right || rhs.left > right || top > rhs.bottom || rhs.top > bottom);
  }

  /// Tests whether the specified point is contained in the rectangle.
  constexpr bool Contains(T x, T y) const { return (x >= left && x < right && y >= top && y < bottom); }

  /// Expands the bounds of the rectangle to contain the specified point.
  constexpr void Include(T x, T y)
  {
    left = std::min(left, x);
    right = std::max(right, x + static_cast<T>(1));
    top = std::min(top, y);
    bottom = std::max(bottom, y + static_cast<T>(1));
  }

  /// Expands the bounds of the rectangle to contain another rectangle.
  constexpr void Include(const Rectangle& rhs)
  {
    left = std::min(left, rhs.left);
    right = std::max(right, rhs.right);
    top = std::min(top, rhs.top);
    bottom = std::max(bottom, rhs.bottom);
  }

  T left;
  T top;
  T right;
  T bottom;
};

} // namespace Common