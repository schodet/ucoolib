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
#include "ucoo/math/vect3d.hh"
#include "ucoo/math/quaternion.hh"
#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"
#include "ucoo/common.hh"

#include <cmath>
#include <cfloat>
#include <cstdio>

template<typename T>
bool
almost_eq (T a, double b);

template<>
bool
almost_eq (int a, double b)
{
    return a == (int) b;
}

template<>
bool
almost_eq (float a, double b)
{
    float pfa = std::fabs (a);
    float fb = b;
    float pfb = std::fabs (fb);
    float diff = std::fabs (a - fb);
    if (pfa >= FLT_MIN && pfb >= FLT_MIN)
        return diff <= pfb * 4 * FLT_EPSILON;
    else
        return diff <= 4 * FLT_EPSILON;
}

template<>
bool
almost_eq (double a, double b)
{
    double pa = std::fabs (a);
    double pb = std::fabs (b);
    double diff = std::fabs (a - b);
    if (pa >= DBL_MIN && pb >= DBL_MIN)
        return diff <= pb * 4 * DBL_EPSILON;
    else
        return diff <= 4 * DBL_EPSILON;
}

template<typename T>
bool
almost_eq_vect (const ucoo::vect3d<T> v, T x, T y, T z)
{
    return almost_eq (v.x, x)
        && almost_eq (v.y, y)
        && almost_eq (v.z, z);
}

template<typename T>
void
test_group_vect3d (ucoo::TestSuite &tsuite, const char *tname)
{
    tsuite.group (tname);
    ucoo::vect3d<T> a (1, 2, 3);
    ucoo::vect3d<T> b (4, 6, 8);
    do {
        ucoo::Test test (tsuite, "constructor add sub");
        test_fail_break_unless (test, a.x == 1 && a.y == 2 && a.z == 3);
        test_fail_break_unless (test, b.x == 4 && b.y == 6 && b.z == 8);
        ucoo::vect3d<T> c (a, b);
        test_fail_break_unless (test, c.x == 3 && c.y == 4 && c.z == 5);
        ucoo::vect3d<T> d (b + a);
        test_fail_break_unless (test, d.x == 5 && d.y == 8 && d.z == 11);
        d = b;
        test_fail_break_unless (test, d.x == 4 && d.y == 6 && d.z == 8);
        d += a;
        test_fail_break_unless (test, d.x == 5 && d.y == 8 && d.z == 11);
        d = b - a;
        test_fail_break_unless (test, d.x == 3 && d.y == 4 && d.z == 5);
        d = b;
        d -= a;
        test_fail_break_unless (test, d.x == 3 && d.y == 4 && d.z == 5);
        d -= d;
        test_fail_break_unless (test, d.x == 0 && d.y == 0 && d.z == 0);
        test_fail_break_unless (test, a.x == 1 && a.y == 2 && a.z == 3);
        test_fail_break_unless (test, b.x == 4 && b.y == 6 && b.z == 8);
    } while (0);
    do
    {
        ucoo::Test test (tsuite, "scaling");
        ucoo::vect3d<T> c (a * 2);
        test_fail_break_unless (test, c.x == 2 && c.y == 4 && c.z == 6);
        c = a;
        c *= 3;
        test_fail_break_unless (test, c.x == 3 && c.y == 6 && c.z == 9);
        c = b / 2;
        test_fail_break_unless (test, c.x == 2 && c.y == 3 && c.z == 4);
        c = b;
        c /= 2;
        test_fail_break_unless (test, c.x == 2 && c.y == 3 && c.z == 4);
        test_fail_break_unless (test, a.x == 1 && a.y == 2 && a.z == 3);
        test_fail_break_unless (test, b.x == 4 && b.y == 6 && b.z == 8);
    } while (0);
    do
    {
        ucoo::Test test (tsuite, "dot product");
        T d = a.dot (a);
        test_fail_break_unless (test, d == 14);
        d = a.dot (b);
        test_fail_break_unless (test, d == 40);
        test_fail_break_unless (test, a.x == 1 && a.y == 2 && a.z == 3);
        test_fail_break_unless (test, b.x == 4 && b.y == 6 && b.z == 8);
    } while (0);
    do
    {
        ucoo::Test test (tsuite, "cross product");
        ucoo::vect3d<T> c = a.cross (b);
        test_fail_break_unless (test, c.x == -2 && c.y == 4 && c.z == -2);
        c = a.cross (a);
        test_fail_break_unless (test, c.x == 0 && c.y == 0 && c.z == 0);
        test_fail_break_unless (test, a.x == 1 && a.y == 2 && a.z == 3);
        test_fail_break_unless (test, b.x == 4 && b.y == 6 && b.z == 8);
    } while (0);
    do
    {
        ucoo::Test test (tsuite, "norm");
        T n = a.norm ();
        test_fail_break_unless (test, almost_eq (n, 3.74165738677394));
    } while (0);
    do
    {
        ucoo::Test test (tsuite, "unit int");
        ucoo::vect3d<T> c (42, 0, 0);
        c = c.unit ();
        test_fail_break_unless (test, c.x == 1 && c.y == 0 && c.z == 0);
    } while (0);
    if (static_cast<T> (0.1) != 0)
    {
        do
        {
            ucoo::Test test (tsuite, "unit");
            ucoo::vect3d<T> c = a.unit ();
            test_fail_break_unless (test, almost_eq (c.x, 0.2672612419124244)
                                    && almost_eq (c.y, 0.53452248382484879)
                                    && almost_eq (c.z, 0.80178372573727319));
        } while (0);
    }
}

template<typename T>
void
test_group_quaternion (ucoo::TestSuite &tsuite, const char *tname)
{
    tsuite.group (tname);
    ucoo::vect3d<T> x (1, 0, 0);
    ucoo::vect3d<T> y (0, 1, 0);
    ucoo::vect3d<T> z (0, 0, 1);
    do {
        ucoo::Test test (tsuite, "rotate x");
        ucoo::Quaternion<T> q (ucoo::YawPitchRoll<T> (0, 0, M_PI_2));
        ucoo::vect3d<T> r;
        r = q.rotate (x);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 1, 0, 0));
        r = q.rotate (y);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 0, 0, 1));
        r = q.rotate (z);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 0, -1, 0));
    } while (0);
    do {
        ucoo::Test test (tsuite, "rotate y");
        ucoo::Quaternion<T> q (ucoo::YawPitchRoll<T> (0, M_PI_2, 0));
        ucoo::vect3d<T> r;
        r = q.rotate (x);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 0, 0, -1));
        r = q.rotate (y);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 0, 1, 0));
        r = q.rotate (z);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 1, 0, 0));
    } while (0);
    do {
        ucoo::Test test (tsuite, "rotate z");
        ucoo::Quaternion<T> q (ucoo::YawPitchRoll<T> (M_PI_2, 0, 0));
        ucoo::vect3d<T> r;
        r = q.rotate (x);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 0, 1, 0));
        r = q.rotate (y);
        test_fail_break_unless (test, almost_eq_vect<T> (r, -1, 0, 0));
        r = q.rotate (z);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 0, 0, 1));
    } while (0);
    do {
        ucoo::Test test (tsuite, "rotate zyx");
        ucoo::Quaternion<T> q (ucoo::YawPitchRoll<T> (M_PI_2, M_PI_2, M_PI_2));
        ucoo::vect3d<T> r;
        r = q.rotate (x);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 0, 0, -1));
        r = q.rotate (y);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 0, 1, 0));
        r = q.rotate (z);
        test_fail_break_unless (test, almost_eq_vect<T> (r, 1, 0, 0));
    } while (0);
    do {
        ucoo::Test test (tsuite, "yaw-pitch-roll to quaternion and back");
        ucoo::YawPitchRoll<T> yprs[] = {
            { M_PI_4, 0, 0 },
            { 0, M_PI_4, 0 },
            { 0, 0, M_PI_4 },
            { M_PI / 3, M_PI_4, 0 },
            { M_PI / 3, 0, M_PI_4 },
            { 0, M_PI / 3, M_PI_4 },
            { M_PI / 3, M_PI_4, M_PI / 6 },
            { -M_PI_4, 0, 0 },
            { 0, -M_PI_4, 0 },
            { 0, 0, -M_PI_4 },
            { M_PI_2 + M_PI_4, 0, 0 },
            // Pitch is between -pi/2 and +pi/2.
            { 0, 0, M_PI_2 + M_PI_4 },
        };
        for (auto &ypr : yprs)
        {
            ucoo::Quaternion<T> q (ypr);
            test_fail_break_unless (test, almost_eq (ypr.yaw, q.yaw ()));
            test_fail_break_unless (test, almost_eq (ypr.pitch, q.pitch ()));
            test_fail_break_unless (test, almost_eq (ypr.roll, q.roll ()));
        }
    } while (0);
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::TestSuite tsuite ("math");
    test_group_vect3d<int> (tsuite, "vect3d int");
    test_group_vect3d<float> (tsuite, "vect3d float");
    test_group_vect3d<double> (tsuite, "vect3d double");
    test_group_quaternion<float> (tsuite, "quaternion float");
    test_group_quaternion<double> (tsuite, "quaternion double");
    return tsuite.report () ? 0 : 1;
}
