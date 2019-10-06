#ifndef ucoolib_utils_trace_hh
#define ucoolib_utils_trace_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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

/// Trace buffer to be read with debugger, no initialisation, should be
/// instantiated in BSS.
class TraceBuffer
{
    /// Maximum number of arguments.
    static const int args_nb = 4;
    /// Maximum number of trace entries.
    static const int entries_nb = 32;
  public:
    /// Trace without argument.
    inline void operator() (const char *str);
    /// Trace with N arguments...
    inline void operator() (const char *str, int a0);
    inline void operator() (const char *str, int a0, int a1);
    inline void operator() (const char *str, int a0, int a1, int a2);
    inline void operator() (const char *str, int a0, int a1, int a2, int a3);
  private:
    /// Trace entry, contains all given parameters.
    struct Entry
    {
        const char *str;
        int args[args_nb];
    };
    /// Trace entries array.
    Entry entries[entries_nb];
    /// Index of next entry to be written in entries array.
    unsigned int index;
};

/// Dummy trace, trace nothing.
class TraceDummy
{
  public:
    inline void operator() (const char *str) { }
    inline void operator() (const char *str, int a0) { }
    inline void operator() (const char *str, int a0, int a1) { }
    inline void operator() (const char *str, int a0, int a1, int a2) { }
    inline void operator() (const char *str, int a0, int a1, int a2, int a3) { }
};

/// Conditional trace, whether it trace or not depends on the template
/// argument.
template<bool ENABLED>
class Trace
{
};

template<>
class Trace<true> : public TraceBuffer
{
};

template<>
class Trace<false> : public TraceDummy
{
};

inline void
TraceBuffer::operator() (const char *str)
{
    entries[index].str = str;
    index = (index + 1) % entries_nb;
}

inline void
TraceBuffer::operator() (const char *str, int a0)
{
    entries[index].str = str;
    entries[index].args[0] = a0;
    index = (index + 1) % entries_nb;
}

inline void
TraceBuffer::operator() (const char *str, int a0, int a1)
{
    entries[index].str = str;
    entries[index].args[0] = a0;
    entries[index].args[1] = a1;
    index = (index + 1) % entries_nb;
}

inline void
TraceBuffer::operator() (const char *str, int a0, int a1, int a2)
{
    entries[index].str = str;
    entries[index].args[0] = a0;
    entries[index].args[1] = a1;
    entries[index].args[2] = a2;
    index = (index + 1) % entries_nb;
}

inline void
TraceBuffer::operator() (const char *str, int a0, int a1, int a2, int a3)
{
    entries[index].str = str;
    entries[index].args[0] = a0;
    entries[index].args[1] = a1;
    entries[index].args[2] = a2;
    entries[index].args[3] = a3;
    index = (index + 1) % entries_nb;
}

} // namespace ucoo

#endif // ucoolib_utils_trace_hh
