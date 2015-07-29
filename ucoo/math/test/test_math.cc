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
#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"
#include "ucoo/common.hh"

#include <cmath>
#include <cfloat>

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
    float fb = b;
    float pfb = std::fabs (fb);
    ucoo::assert (pfb >= FLT_MIN);
    float diff = std::fabs (a - fb);
    return diff <= pfb * 4 * FLT_EPSILON;
}

template<>
bool
almost_eq (double a, double b)
{
    double pb = std::fabs (b);
    ucoo::assert (pb >= DBL_MIN);
    double diff = std::fabs (a - b);
    return diff <= pb * 4 * DBL_EPSILON;
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

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::TestSuite tsuite ("math");
    test_group_vect3d<int> (tsuite, "vect3d int");
    test_group_vect3d<float> (tsuite, "vect3d float");
    test_group_vect3d<double> (tsuite, "vect3d double");
    return tsuite.report () ? 0 : 1;
}
