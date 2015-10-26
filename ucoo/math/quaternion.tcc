#ifndef ucoo_math_quaternion_tcc
#define ucoo_math_quaternion_tcc
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
#include <cmath>

namespace ucoo {

template<typename T>
constexpr
Quaternion<T>::Quaternion ()
{
}

template<typename T>
constexpr
Quaternion<T>::Quaternion (T w_, T x_, T y_, T z_)
    : w (w_), x (x_), y (y_), z (z_)
{
}

template<typename T>
Quaternion<T>::Quaternion (const YawPitchRoll<T> &ypr)
{
    T cyaw = std::cos (ypr.yaw / 2);
    T syaw = std::sin (ypr.yaw / 2);
    T cpitch = std::cos (ypr.pitch / 2);
    T spitch = std::sin (ypr.pitch / 2);
    T croll = std::cos (ypr.roll / 2);
    T sroll = std::sin (ypr.roll / 2);
    w = cpitch * croll * cyaw + spitch * sroll * syaw;
    x = cpitch * cyaw * sroll - croll * spitch * syaw;
    y = cpitch * sroll * syaw + croll * cyaw * spitch;
    z = cpitch * croll * syaw - cyaw * spitch * sroll;
}

template<typename T>
vect3d<T>
Quaternion<T>::rotate (const vect3d<T> &v) const
{
    T nx = w*w*v.x + 2*y*w*v.z - 2*z*w*v.y + x*x*v.x
        + 2*y*x*v.y + 2*z*x*v.z - z*z*v.x - y*y*v.x;
    T ny = 2*x*y*v.x + y*y*v.y + 2*z*y*v.z + 2*w*z*v.x
        - z*z*v.y + w*w*v.y - 2*x*w*v.z - x*x*v.y;
    T nz = 2*x*z*v.x + 2*y*z*v.y + z*z*v.z - 2*w*y*v.x
        - y*y*v.z + 2*w*x*v.y - x*x*v.z + w*w*v.z;
    return vect3d<T> (nx, ny, nz);
}

} // namespace ucoo

#endif // ucoo_math_quaternion_tcc
