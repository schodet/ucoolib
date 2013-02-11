#ifndef ucoolib_utils_trace_hh
#define ucoolib_utils_trace_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
