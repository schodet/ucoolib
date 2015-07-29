#ifndef ucoo_math_vect3d_hh
#define ucoo_math_vect3d_hh
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

namespace ucoo {

template<typename T>
struct vect3d
{
    T x, y, z;
  public:
    /// Default constructor, coordinates are default initialized.
    constexpr vect3d ();
    /// Constructor from coordinates.
    constexpr vect3d (T x_, T y_, T z_);
    /// Constructor from two points (B - A).
    vect3d (const vect3d<T> &a, const vect3d<T> &b);
    /// Addition (translate).
    vect3d<T> operator+ (const vect3d<T> &o) const;
    vect3d<T> operator+= (const vect3d<T> &o);
    /// Subtraction.
    vect3d<T> operator- (const vect3d<T> &o) const;
    vect3d<T> operator-= (const vect3d<T> &o);
    /// Scaling.
    template<typename U>
    vect3d<T> operator* (U s) const;
    template<typename U>
    vect3d<T> operator*= (U s);
    template<typename U>
    vect3d<T> operator/ (U s) const;
    template<typename U>
    vect3d<T> operator/= (U s);
    /// Dot product.
    T dot (const vect3d<T> &o) const;
    /// Cross product.
    vect3d<T> cross (const vect3d<T> &o) const;
    /// Euclidean norm.
    T norm () const;
    /// Return a vector of norm 1.
    vect3d<T> unit () const;
};

} // namespace ucoo

#include "vect3d.tcc"

#endif // ucoo_math_vect3d_hh
