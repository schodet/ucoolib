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
#include "ucoo/base/fs/romfs/romfs.hh"

#include <algorithm>
#include <cstring>

namespace ucoo {


RomFS::RomFSStream::RomFSStream (const char *begin, const char *end)
    : begin_ (begin), end_ (end)
{
}

int
RomFS::RomFSStream::read (char *buf, int count)
{
    int l = std::min<int> (count, end_ - begin_);
    if (!l)
        return -2;
    else
    {
        std::copy (begin_, begin_ + l, buf);
        begin_ += l;
        return l;
    }
}

int
RomFS::RomFSStream::write (const char *buf, int count)
{
    return -1;
}

int
RomFS::RomFSStream::poll ()
{
    return end_ - begin_;
}

RomFS::RomFS (const uint32_t *addr, int size)
{
    ucoo::assert (size >= 12);
    ucoo::assert (addr[0] == 0x466d6f52); // RomF
    files_count_ = addr[1];
    ucoo::assert (size >= 12 + 8 * files_count_);
    filenames_ = &addr[2];
    filecontents_ = &addr[2 + files_count_];
    ucoo::assert (size >= 12 + 8 * files_count_
                  + static_cast<int> (filecontents_[files_count_]));
    data_ = reinterpret_cast<const char *> (&addr[3 + 2 * files_count_]);
}

static int
filename_compare (const char *a, int a_len, const char *b, int b_len)
{
    int bcmp = std::memcmp (a, b, std::min (a_len, b_len));
    if (bcmp == 0)
        return a_len - b_len;
    else
        return bcmp;
}

Stream *
RomFS::open (const char *filename, Mode mode, Error &error)
{
    if (mode == Mode::WRITE)
    {
        error = Error::READ_ONLY;
        return nullptr;
    }
    int filename_len = std::strlen (filename);
    // Dichotomy search.
    int begin = 0;
    int end = files_count_;
    while (begin != end)
    {
        int i = begin + (end - begin) / 2;
        // Compare file name.
        const char *i_name = data_ + filenames_[i];
        int i_len = filenames_[i + 1] - filenames_[i];
        int cmp = filename_compare (filename, filename_len, i_name, i_len);
        if (cmp < 0)
            end = i;
        else if (cmp > 0)
            begin = i + 1;
        else
        {
            // Match.
            const char *begin = data_ + filecontents_[i];
            const char *end = data_ + filecontents_[i + 1];
            RomFSStream *s = pool_.construct (begin, end);
            if (!s)
                error = Error::TOO_MANY_OPEN_FILES;
            return s;
        }
    }
    // Not found.
    error = Error::NO_SUCH_FILE;
    return nullptr;
}

void
RomFS::close (Stream *file)
{
    pool_.destroy (static_cast<RomFSStream *> (file));
}

void
RomFS::unlink (const char *filename)
{
    // This space is left intentionally blank.
}

} // namespace ucoo
