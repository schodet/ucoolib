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
#include "ucoo/intf/file_system.hh"
#ifdef TARGET_newlib
# include "ucoo/arch/syscalls.newlib.hh"
# include "ucoo/common.hh"
#endif

namespace ucoo {

void
FileSystem::enable (bool root)
{
    disable ();
    enabled_ = true;
    root_ = root;
#ifdef TARGET_newlib
    if (root_)
    {
        assert (!syscalls_file_system);
        syscalls_file_system = this;
    }
#endif
}

void
FileSystem::disable ()
{
#ifdef TARGET_newlib
    if (enabled_ && root_)
        syscalls_file_system = nullptr;
#endif
    enabled_ = false;
    root_ = false;
}

} // namespace ucoo
