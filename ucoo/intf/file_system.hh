#ifndef ucoo_intf_file_system_hh
#define ucoo_intf_file_system_hh
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
#include "ucoo/intf/stream.hh"

namespace ucoo {

/// Interface to a simple file system, can be bound to newlib syscalls.
class FileSystem
{
  public:
    /// Open mode.
    enum class Mode {
        /// Read only.
        READ,
        /// Write only.  If file exists, it is truncated, else it is created.
        WRITE,
    };
    /// Error code.
    enum class Error {
        /// Access not allowed.
        ACCESS_DENIED,
        /// The maximum number of open files is exceeded.
        TOO_MANY_OPEN_FILES,
        /// File name is too long.
        NAME_TOO_LONG,
        /// File does not exists.
        NO_SUCH_FILE,
        /// No space left on device.
        NO_SPACE_LEFT,
        /// Read only file system.
        READ_ONLY,
    };
  public:
    /// Enable (and register to newlib syscalls).
    virtual void enable (bool root = true);
    /// Disable (and unregister from newlib syscalls).
    virtual void disable ();
    /// Open file and return an open stream or nullptr on failure.
    virtual Stream *open (const char *filename, Mode mode, Error &error) = 0;
    /// Open file and return an open stream or nullptr on failure, no error
    /// code.
    Stream *open (const char *filename, Mode mode = Mode::READ)
    {
        Error error;
        return open (filename, mode, error);
    }
    /// Close a previously open file.
    virtual void close (Stream *file) = 0;
    /// Remove a file, does not complain if the file does not exists.
    virtual void unlink (const char *filename) = 0;
  private:
    /// Is it enabled?
    bool enabled_;
    /// Is this the root file system?
    bool root_;
};

} // namespace ucoo

#endif // ucoo_intf_file_system_hh
