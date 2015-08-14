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
#include "ucoo/utils/function.hh"
#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"

int
sum (int a, int b)
{
    return a + b;
}

struct Accum
{
    int cur;
    Accum (int base) : cur (base) { }
    void acc (int val) { cur += val; }
};

struct AbstractReduce
{
    virtual void acc (int val) = 0;
};

struct ConcreteAccum : public AbstractReduce
{
    int cur;
    ConcreteAccum (int base) : cur (base) { }
    void acc (int val) override { cur += val; }
};

bool __attribute__ ((noinline))
test_function (const ucoo::Function<int (int, int)> &f)
{
    return f (1, 2) == 3;
}

bool __attribute__ ((noinline))
test_function (const ucoo::Function<void (int)> &f, int &cur)
{
    f (2);
    return cur == 3;
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::TestSuite tsuite ("function");
    {
        ucoo::Test test (tsuite, "function pointer");
        ucoo::Function<int (int, int)> f (sum);
        if (!test_function (f))
            test.fail ();
    }
    {
        ucoo::Test test (tsuite, "bound member");
        Accum a (1);
        ucoo::Function<void (int)> f (&a, &Accum::acc);
        if (!test_function (f, a.cur))
            test.fail ();
    }
    {
        ucoo::Test test (tsuite, "bound virtual member");
        ConcreteAccum a (1);
        AbstractReduce *aa = &a;
        ucoo::Function<void (int)> f (aa, &AbstractReduce::acc);
        if (!test_function (f, a.cur))
            test.fail ();
    }
    {
        ucoo::Test test (tsuite, "copy");
        ucoo::Function<int (int, int)> f (sum);
        ucoo::Function<int (int, int)> f2 (f);
        if (!test_function (f2))
            test.fail ();
    }
    {
        ucoo::Test test (tsuite, "assignment");
        ucoo::Function<int (int, int)> f (sum);
        ucoo::Function<int (int, int)> f2;
        f2 = f;
        if (!test_function (f2))
            test.fail ();
    }
    {
        ucoo::Test test (tsuite, "function assignment");
        ucoo::Function<int (int, int)> f;
        f = sum;
        if (!f || !test_function (f))
            test.fail ();
    }
    {
        ucoo::Test test (tsuite, "nullptr assignment");
        ucoo::Function<int (int, int)> f (sum);
        f.reset ();
        if (f)
            test.fail ();
    }
    return tsuite.report () ? 0 : 1;
}

