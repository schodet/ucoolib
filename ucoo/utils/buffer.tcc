#ifndef ucoo_utils_buffer_tcc
#define ucoo_utils_buffer_tcc
// buffer.tcc
//  {{{
//
// Copyright (C) 2016 Nicolas Schodet
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
// Contact :
//        Web: http://ni.fr.eu.org/
//      Email: <nico at ni.fr.eu.org>
// }}}
#include <algorithm>

namespace ucoo {

template<typename T, int max_size>
Buffer<T, max_size>::Buffer ()
    : begin_ (data_), end_ (data_)
{
}

template<typename T, int max_size>
bool
Buffer<T, max_size>::empty () const
{
    return begin_ == end_;
}

template<typename T, int max_size>
bool
Buffer<T, max_size>::full () const
{
    return end_ == &data_[max_size];
}

template<typename T, int max_size>
int
Buffer<T, max_size>::size () const
{
    return end_ - begin_;
}

template<typename T, int max_size>
int
Buffer<T, max_size>::room () const
{
    return &data_[max_size] - end_;
}

template<typename T, int max_size>
const T *
Buffer<T, max_size>::read () const
{
    return begin_;
}

template<typename T, int max_size>
void
Buffer<T, max_size>::drop (int n)
{
    begin_ += n;
    if (empty ())
        begin_ = end_ = &data_[0];
}

template<typename T, int max_size>
const T *
Buffer<T, max_size>::read (int n)
{
    drop (n);
    return read ();
}

template<typename T, int max_size>
T *
Buffer<T, max_size>::write ()
{
    return end_;
}

template<typename T, int max_size>
void
Buffer<T, max_size>::written (int n)
{
    end_ += n;
}

template<typename T, int max_size>
T *
Buffer<T, max_size>::write (int n)
{
    T *r = write ();
    written (n);
    return r;
}

template<typename T, int max_size>
void
Buffer<T, max_size>::rewind ()
{
    if (begin_ != &data_[0])
    {
        std::copy (begin_, end_, &data_[0]);
        end_ -= begin_ - &data_[0];
        begin_ = &data_[0];
    }
}

} // namespace ucoo

#endif // ucoo_utils_buffer_tcc
