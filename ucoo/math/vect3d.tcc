#ifndef ucoo_math_vect3d_tcc
#define ucoo_math_vect3d_tcc
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
vect3d<T>::vect3d ()
{
}

template<typename T>
constexpr
vect3d<T>::vect3d (T x_, T y_, T z_)
    : x (x_), y (y_), z (z_)
{
}

template<typename T>
vect3d<T>::vect3d (const vect3d<T> &a, const vect3d<T> &b)
{
    *this = b - a;
}

template<typename T>
vect3d<T>
vect3d<T>::operator+ (const vect3d<T> &o) const
{
    return vect3d<T> (x + o.x, y + o.y, z + o.z);
}

template<typename T>
vect3d<T>
vect3d<T>::operator+= (const vect3d<T> &o)
{
    *this = *this + o;
    return *this;
}

template<typename T>
vect3d<T>
vect3d<T>::operator- (const vect3d<T> &o) const
{
    return vect3d<T> (x - o.x, y - o.y, z - o.z);
}

template<typename T>
vect3d<T>
vect3d<T>::operator-= (const vect3d<T> &o)
{
    *this = *this - o;
    return *this;
}

template<typename T>
template<typename U>
vect3d<T>
vect3d<T>::operator* (U s) const
{
    return vect3d<T> (x * s, y * s, z * s);
}

template<typename T>
template<typename U>
vect3d<T>
vect3d<T>::operator*= (U s)
{
    *this = *this * s;
    return *this;
}

template<typename T>
template<typename U>
vect3d<T>
vect3d<T>::operator/ (U s) const
{
    return vect3d<T> (x / s, y / s, z / s);
}

template<typename T>
template<typename U>
vect3d<T>
vect3d<T>::operator/= (U s)
{
    *this = *this / s;
    return *this;
}

template<typename T>
T
vect3d<T>::dot (const vect3d<T> &o) const
{
    return x * o.x + y * o.y + z * o.z;
}

template<typename T>
vect3d<T>
vect3d<T>::cross (const vect3d<T> &o) const
{
    return vect3d<T> (
        y * o.z - z * o.y,
        z * o.x - x * o.z,
        x * o.y - y * o.x
        );
}

template<typename T>
T
vect3d<T>::norm () const
{
    return std::sqrt (dot (*this));
}

template<typename T>
vect3d<T>
vect3d<T>::unit () const
{
    return *this / norm ();
}

} // namespace ucoo

#endif // ucoo_math_vect3d_tcc
