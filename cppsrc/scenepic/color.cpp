#include <cassert>
#include <sstream>
#include <iomanip>
#include <limits>

#include "color.h"
#include "matrix.h"

namespace scenepic
{
const Color Color::None()
{
    return Color(-std::numeric_limits<float>::infinity(),
                 -std::numeric_limits<float>::infinity(),
                 -std::numeric_limits<float>::infinity());
}

Color::Color(const Base::Scalar &red, const Base::Scalar &green, const Base::Scalar &blue) : Base(red, green, blue)
{    
    assert((red >= 0.0 && red <= 1.0) || red == -std::numeric_limits<float>::infinity());
    assert((green >= 0.0 && green <= 1.0) || green == -std::numeric_limits<float>::infinity());
    assert((blue >= 0.0 && blue <= 1.0) || blue == -std::numeric_limits<float>::infinity());
}

Color Color::from_bytes(std::uint8_t red, std::uint8_t green, std::uint8_t blue)
{
    return Color(red / 255.0f, green / 255.0f, blue / 255.0f);
}

std::float_t Color::r() const
{
    return this->x();
}

std::float_t &Color::r()
{
    return this->x();
}

std::float_t Color::g() const
{
    return this->y();
}

std::float_t &Color::g()
{
    return this->y();
}

std::float_t Color::b() const
{
    return this->z();
}

std::float_t &Color::b()
{
    return this->z();
}

std::uint8_t Color::R() const
{
    if(this->is_none())
    {
        return 0;
    }

    return static_cast<std::uint8_t>(std::min(this->r() * 256.0f, 255.0f));
}

std::uint8_t Color::G() const
{
    if(this->is_none())
    {
        return 0;
    }

    return static_cast<std::uint8_t>(std::min(this->g() * 256.0f, 255.0f));
}

std::uint8_t Color::B() const
{
    if(this->is_none())
    {
        return 0;
    }

    return static_cast<std::uint8_t>(std::min(this->b() * 256.0f, 255.0f));
}

std::string Color::to_html_hex() const
{
    std::stringstream buffer;
    int red = static_cast<int>(this->R());
    int green = static_cast<int>(this->G());
    int blue = static_cast<int>(this->B());
    buffer << '#';
    buffer << std::hex << std::setw(2) << std::setfill('0') << red;
    buffer << std::hex << std::setw(2) << std::setfill('0') << green;
    buffer << std::hex << std::setw(2) << std::setfill('0') << blue;
    return buffer.str();
}

std::string Color::to_string() const
{
    std::stringstream stream;
    stream << "Color(r=" << std::to_string(this->r()) << ", g=" << std::to_string(this->g()) << ", b=" << std::to_string(this->b()) << ')';
    return stream.str();
}

bool Color::is_none() const
{
    return *this == Color::None();
}
} // namespace scenepic