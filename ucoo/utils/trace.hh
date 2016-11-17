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
#include <functional>

namespace ucoo {

/// Used when there is no timestamp.
struct NoTimestamp
{
    /// Information added to entry.
    struct Entry { };
    /// Fill an entry with current timestamp.
    void operator () (Entry &) { }
    /// Dump an entry timestamp, there is room for 32 characters, return the
    /// dumped text length.
    int dump (const Entry &e, const Entry &eprev, char *buf) const { return 0; }
};

/// Trace buffer common base.
class TraceBufferBase
{
  public:
    /// Dump as text, call dump_callback several time with text dump.
    virtual bool dump (std::function<bool (
            const char *str, int str_size)> dump_callback) const = 0;
  protected:
    constexpr TraceBufferBase (const char *name) : name_ (name) { }
    friend class TraceRegistry;
    const char *name_;
    TraceBufferBase *next_ = nullptr;
};

/// Trace buffer to be read with debugger.
template<typename Timestamp = NoTimestamp>
class TraceBuffer : public TraceBufferBase
{
    /// Maximum number of arguments.
    static const int args_nb = 4;
    /// Maximum number of trace entries.
    static const int entries_nb = 512;
  public:
    /// Constructor.
    TraceBuffer (const char *name, const Timestamp &timestamp = Timestamp ());
    /// Trace without argument.
    inline void operator() (const char *str);
    /// Trace with N arguments...
    inline void operator() (const char *str, int a0);
    inline void operator() (const char *str, int a0, int a1);
    inline void operator() (const char *str, int a0, int a1, int a2);
    inline void operator() (const char *str, int a0, int a1, int a2, int a3);
    /// See TraceBufferBase::dump.
    bool dump (std::function<bool (
            const char *str, int str_size)> dump_callback) const override;
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
    TraceDummy (const char *name) { }
    inline void operator() (const char *str) { }
    inline void operator() (const char *str, int a0) { }
    inline void operator() (const char *str, int a0, int a1) { }
    inline void operator() (const char *str, int a0, int a1, int a2) { }
    inline void operator() (const char *str, int a0, int a1, int a2, int a3) { }
    bool dump (std::function<bool (
            const char *str, int str_size)> dump_callback) const
        { return true; }
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
  public:
    constexpr Trace (const char *name) : TraceBuffer<Timestamp> (name) { }
};

template<typename Timestamp>
class Trace<false, Timestamp> : public TraceDummy
{
  public:
    constexpr Trace (const char *name) : TraceDummy (name) { }
};

/// Registry of all active traces.
class TraceRegistry
{
  public:
    /// Register a trace buffer (called automatically on trace buffer
    /// construction).
    static void register_trace_buffer (TraceBufferBase &b);
    /// Dump all active traces as text.
    static bool dump_all (std::function<bool (
            const char *str, int str_size)> dump_callback);
    /// Dump specified trace as text.
    static bool dump (const char *name, std::function<bool (
            const char *str, int str_size)> dump_callback);
  private:
    static TraceRegistry &get_instance ();
    TraceBufferBase *first = nullptr;
};

} // namespace ucoo

#include "ucoo/utils/trace.tcc"

#endif // ucoo_utils_trace_hh
