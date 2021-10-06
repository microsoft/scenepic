// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_COLOR_H_
#define _SCENEPIC_COLOR_H_

#include "matrix.h"

#include <Eigen/Core>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

namespace scenepic
{
  /** Class for representing color values */
  class Color : public Eigen::Vector3f
  {
  public:
    /** Empty constructor. */
    Color(void) : Eigen::Vector3f() {}

    /** Needed for pybind */
    Color(Eigen::Index rows, Eigen::Index cols) : Eigen::Vector3f(rows, cols) {}

    typedef Eigen::Vector3f Base;

    /** Constructor which allows construction of Colors from Eigen expressions.
     */
    template<typename OtherDerived>
    Color(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Vector3f(other)
    {}

    /** Constructor which allows Eigen assignments to Color */
    template<typename OtherDerived>
    Color& operator=(const Eigen::MatrixBase<OtherDerived>& other)
    {
      this->Base::operator=(other);
      return *this;
    }

    /** Constructor.
     * \param red Red channel [0,1]
     * \param green Green channel [0,1]
     * \param blue Blue channel [0,1]
     */
    Color(const Scalar& red, const Scalar& green, const Scalar& blue);

    /** Construct a color object from byte values.
     * \param red Red channel
     * \param green Green channel
     * \param blue Blue channel
     * \return a valid color object
     */
    static Color
    from_bytes(std::uint8_t red, std::uint8_t green, std::uint8_t blue);

    /** Convert the color to an HTML hex color string */
    std::string to_html_hex() const;

    /** The red value [0,1] */
    Scalar r() const;
    /** The green value [0,1] */
    Scalar g() const;
    /** The blue value [0,1] */
    Scalar b() const;

    /** The red value [0,255] */
    std::uint8_t R() const;
    /** The green value [0,255] */
    std::uint8_t G() const;
    /** The blue value [0,255] */
    std::uint8_t B() const;

    /** Reference to the red value [0,1] */
    Scalar& r();
    /** Reference to the green value [0,1] */
    Scalar& g();
    /** Reference to the blue value [0,1] */
    Scalar& b();

    /** Represent the color as a string */
    std::string to_string() const;

    /** Whether this is a "no color" instance */
    bool is_none() const;

    /** Constant value indicating "no color" */
    static const Color None();
  };

  namespace Colors
  {
    const Color Black = Color::from_bytes(0, 0, 0);
    const Color White = Color::from_bytes(255, 255, 255);
    const Color Red = Color::from_bytes(255, 0, 0);
    const Color Maroon = Color::from_bytes(128, 0, 0);
    const Color Pink = Color::from_bytes(255, 200, 220);
    const Color Brown = Color::from_bytes(170, 110, 40);
    const Color Orange = Color::from_bytes(255, 150, 0);
    const Color Coral = Color::from_bytes(255, 215, 180);
    const Color Olive = Color::from_bytes(128, 128, 0);
    const Color Yellow = Color::from_bytes(255, 235, 0);
    const Color Beige = Color::from_bytes(255, 250, 200);
    const Color Lime = Color::from_bytes(190, 255, 0);
    const Color Green = Color::from_bytes(0, 190, 0);
    const Color Mint = Color::from_bytes(170, 255, 195);
    const Color Teal = Color::from_bytes(0, 128, 128);
    const Color Cyan = Color::from_bytes(100, 255, 255);
    const Color Navy = Color::from_bytes(0, 0, 128);
    const Color Blue = Color::from_bytes(67, 133, 255);
    const Color Purple = Color::from_bytes(130, 0, 150);
    const Color Lavender = Color::from_bytes(230, 190, 255);
    const Color Magenta = Color::from_bytes(255, 0, 255);
    const Color Gray = Color::from_bytes(128, 128, 128);
  } // namespace Colors
} // namespace scenepic

#endif