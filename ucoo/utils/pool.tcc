#ifndef ucoo_utils_pool_tcc
#define ucoo_utils_pool_tcc
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
#include "ucoo/common.hh"

namespace ucoo {

template<typename T, int size>
Pool<T, size>::Pool ()
{
    for (int i = 0; i < size - 1; i++)
        pool_[i].next_free = &pool_[i + 1];
    pool_[size - 1].next_free = nullptr;
    next_free_ = &pool_[0];
}

template<typename T, int size>
Pool<T, size>::~Pool ()
{
    int free = 0;
    for (PoolStorage<T> *s = next_free_; s; s = s->next_free)
        free++;
    assert (free == size);
}

template<typename T, int size>
template<typename... Args>
T *
Pool<T, size>::construct (Args&&... args)
{
    PoolStorage<T> *p = next_free_;
    if (p)
    {
        next_free_ = p->next_free;
        return new (&p->object) T (args...);
    }
    else
        return nullptr;
}

template<typename T, int size>
void
Pool<T, size>::destroy (T *object)
{
    object->~T ();
    PoolStorage<T> *s = reinterpret_cast<PoolStorage<T> *> (object);
    s->next_free = next_free_;
    next_free_ = s;
}

} // namespace ucoo

#endif // ucoo_utils_pool_tcc
