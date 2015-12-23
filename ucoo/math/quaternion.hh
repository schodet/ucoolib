#ifndef ucoo_math_quaternion_hh
#define ucoo_math_quaternion_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2015 Nicolas Schodet
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// }}}
#include "ucoo/math/yaw_pitch_roll.hh"
#include "ucoo/math/vect3d.hh"

namespace ucoo {

/// Quaternion: w + ix + jy + kz.
template<typename T>
struct Quaternion
{
    T w, x, y, z;
  public:
    /// Default constructor, values are default initialized.
    constexpr Quaternion ();
    /// Construct from quaternion values.
    constexpr Quaternion (T w_, T x_, T y_, T z_);
    /// Construct a rotation quaternion from Yaw, Pitch and Roll.
    Quaternion (const YawPitchRoll<T> &ypr);
    /// Rotate a vector by a unit quaternion.
    vect3d<T> rotate (const vect3d<T> &v) const;
    /// Compute Yaw.
    T yaw () const;
    /// Compute Pitch.
    T pitch () const;
    /// Compute Roll.
    T roll () const;
    /// Multiply by another quaternion.
    Quaternion<T> operator* (const Quaternion<T> &r) const;
};

} // namespace ucoo

#include "quaternion.tcc"

#endif // ucoo_math_quaternion_hh
