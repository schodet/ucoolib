// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2016 Nicolas Schodet
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
#include "ucoo/utils/trace.hh"
#include <cstring>

namespace ucoo {

void
TraceRegistry::register_trace_buffer (TraceBufferBase &b)
{
    TraceRegistry &self = get_instance ();
    b.next_ = self.first;
    self.first = &b;
}

bool
TraceRegistry::dump_all (std::function<bool (const char *str, int str_size)>
                         dump_callback)
{
    return dump (nullptr, dump_callback);
}

bool
TraceRegistry::dump (const char *name,
                     std::function<bool (const char *str, int str_size)>
                     dump_callback)
{
    bool ok = true;
    TraceRegistry &self = get_instance ();
    TraceBufferBase *b = self.first;
    while (b)
    {
        if (!name || std::strcmp (name, b->name_) == 0)
        {
            {
                char buf[128];
                int r = snprintf (buf, sizeof (buf) - 1, "---[%s]---", b->name_);
                if (r >= static_cast<int> (sizeof (buf) - 1))
                    r = sizeof (buf) - 2;
                buf[r++] = '\n';
                buf[r++] = '\0';
                ok = ok && dump_callback (buf, r);
            }
            ok = ok && b->dump (dump_callback);
        }
        b = b->next_;
    }
    return ok;
}

TraceRegistry &
TraceRegistry::get_instance ()
{
    static TraceRegistry instance;
    return instance;
}

} // namespace ucoo
