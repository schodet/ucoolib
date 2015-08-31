#ifndef ucoo_utils_fifo_tcc
#define ucoo_utils_fifo_tcc
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include <algorithm>

namespace ucoo {

template<typename T, int size>
inline
Fifo<T, size>::Fifo ()
    : head_ (0), tail_ (0)
{
}

template<typename T, int size>
T &
Fifo<T, size>::top ()
{
    assert (head_ != tail_);
    return buffer_[head_];
}

template<typename T, int size>
inline T
Fifo<T, size>::pop ()
{
    // Reader, can only update head.
    int head = head_;
    assert (head != tail_);
    T v = buffer_[head];
    head = next (head);
    barrier ();
    head_ = head;
    return v;
}

template<typename T, int size>
inline void
Fifo<T, size>::push (const T &e)
{
    // Writer, can only update tail.
    int tail = tail_;
    buffer_[tail] = e;
    tail = next (tail);
    assert (head_ != tail);
    barrier ();
    tail_ = tail;
}

template<typename T, int size>
inline int
Fifo<T, size>::read (T *buf, int count)
{
    // Reader, can only update head.
    int r = 0;
    int head = head_;
    int tail = access_once (tail_);
    while (r < count && head != tail)
    {
        buf[r] = buffer_[head];
        head = next (head);
        r++;
    }
    // Ensure data is copied, then update head.
    barrier ();
    head_ = head;
    return r;
}

template<typename T, int size>
inline int
Fifo<T, size>::write (const T *buf, int count)
{
    // Writer, can only update tail.
    int r = 0;
    int head = access_once (head_);
    int tail = tail_;
    int tailp1 = next (tail);
    while (r < count && tailp1 != head)
    {
        buffer_[tail] = buf[r];
        tail = tailp1;
        r++;
        tailp1 = next (tail);
    }
    // Ensure data is copied, then update tail.
    barrier ();
    tail_ = tail;
    return r;
}

} // namespace ucoo

#endif // ucoo_utils_fifo_tcc
