#ifndef ucoo_utils_trace_tcc
#define ucoo_utils_trace_tcc
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

template<typename Timestamp>
TraceBuffer<Timestamp>::TraceBuffer (const Timestamp &timestamp)
    : entries{}, index (0), timestamp_ (timestamp)
{
}

template<typename Timestamp>
inline void
TraceBuffer<Timestamp>::operator() (const char *str)
{
    timestamp_ (entries[index]);
    entries[index].str = str;
    index = (index + 1) % entries_nb;
}

template<typename Timestamp>
inline void
TraceBuffer<Timestamp>::operator() (const char *str, int a0)
{
    timestamp_ (entries[index]);
    entries[index].str = str;
    entries[index].args[0] = a0;
    index = (index + 1) % entries_nb;
}

template<typename Timestamp>
inline void
TraceBuffer<Timestamp>::operator() (const char *str, int a0, int a1)
{
    timestamp_ (entries[index]);
    entries[index].str = str;
    entries[index].args[0] = a0;
    entries[index].args[1] = a1;
    index = (index + 1) % entries_nb;
}

template<typename Timestamp>
inline void
TraceBuffer<Timestamp>::operator() (const char *str, int a0, int a1, int a2)
{
    timestamp_ (entries[index]);
    entries[index].str = str;
    entries[index].args[0] = a0;
    entries[index].args[1] = a1;
    entries[index].args[2] = a2;
    index = (index + 1) % entries_nb;
}

template<typename Timestamp>
inline void
TraceBuffer<Timestamp>::operator() (const char *str, int a0, int a1, int a2,
                                    int a3)
{
    timestamp_ (entries[index]);
    entries[index].str = str;
    entries[index].args[0] = a0;
    entries[index].args[1] = a1;
    entries[index].args[2] = a2;
    entries[index].args[3] = a3;
    index = (index + 1) % entries_nb;
}

} // namespace ucoo

#endif // ucoo_utils_trace_tcc
