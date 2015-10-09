#ifndef ucoo_base_fs_romfs_romfs_hh
#define ucoo_base_fs_romfs_romfs_hh
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
#include "ucoo/utils/pool.hh"
#include "ucoo/common.hh"

namespace ucoo {

/// Read only file system, read data from a buffer.
///
/// Buffer structure, little endian:
///  u32: 'RomF'
///  u32: number of files
///  u32[n]: indexes in data buffer of file names, sorted by file names
///  u32[n]: indexes in data buffer of file content
///  u32: index in data buffer to first unused byte
///  char[][n]: file names, not zero terminated
///  char[][n]: file contents
class RomFS : public FileSystem
{
    /// Stream from a romfs.
    class RomFSStream : public Stream
    {
      public:
        /// Constructor from a memory chunk.
        RomFSStream (const char *begin, const char *end);
        /// See Stream::read.
        int read (char *buf, int count) override;
        /// See Stream::write.
        int write (const char *buf, int count) override;
        /// See Stream::poll.
        int poll () override;
      private:
        const char *begin_, *end_;
    };
  public:
    /// Constructor, takes romfs address and size.
    RomFS (const uint32_t *addr, int size);
    /// See FileSystem::open.
    Stream *open (const char *filename, Mode mode, Error &error) override;
    Stream *open (const char *filename, Mode mode = Mode::READ)
    {
        return FileSystem::open (filename, mode);
    }
    /// See FileSystem::close.
    void close (Stream *file) override;
    /// See FileSystem::unlink, this is a no-op.
    void unlink (const char *filename) override;
  private:
    /// Number of files.
    int files_count_;
    /// File names indexes.
    const uint32_t *filenames_;
    /// File contents indexes.
    const uint32_t *filecontents_;
    /// Byte array with file names and contents.
    const char *data_;
    /// Pool of stream.
    Pool<RomFSStream, 5> pool_;
};

} // namespace ucoo

#endif // ucoo_base_fs_romfs_romfs_hh
