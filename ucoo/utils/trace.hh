#ifndef ucoo_utils_trace_hh
#define ucoo_utils_trace_hh
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

/// Used when there is no timestamp.
struct NoTimestamp
{
    /// Information added to entry.
    struct Entry { };
    /// Fill an entry with current timestamp.
    void operator () (Entry &) { }
};

/// Trace buffer to be read with debugger.
template<typename Timestamp = NoTimestamp>
class TraceBuffer
{
    /// Maximum number of arguments.
    static const int args_nb = 4;
    /// Maximum number of trace entries.
    static const int entries_nb = 512;
  public:
    /// Constructor.
    TraceBuffer (const Timestamp &timestamp = Timestamp ());
    /// Trace without argument.
    inline void operator() (const char *str);
    /// Trace with N arguments...
    inline void operator() (const char *str, int a0);
    inline void operator() (const char *str, int a0, int a1);
    inline void operator() (const char *str, int a0, int a1, int a2);
    inline void operator() (const char *str, int a0, int a1, int a2, int a3);
  private:
    /// Trace entry, contains all given parameters.
    struct Entry : public Timestamp::Entry
    {
        const char *str;
        int args[args_nb];
    };
    /// Trace entries array.
    Entry entries[entries_nb];
    /// Index of next entry to be written in entries array.
    unsigned int index;
    /// Time stamping object.
    Timestamp timestamp_;
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
template<bool ENABLED, typename Timestamp = NoTimestamp>
class Trace
{
};

template<typename Timestamp>
class Trace<true, Timestamp> : public TraceBuffer<Timestamp>
{
};

template<typename Timestamp>
class Trace<false, Timestamp> : public TraceDummy
{
};

} // namespace ucoo

#include "ucoo/utils/trace.tcc"

#endif // ucoo_utils_trace_hh
