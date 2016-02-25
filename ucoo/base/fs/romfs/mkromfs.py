#!/usr/bin/python
#
"""Generate a ROM file system."""
#
# Copyright (C) 2015 Nicolas Schodet
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#
import argparse
import struct
import sys

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-o', '--output', type=argparse.FileType('w'),
        default=sys.stdout,
        metavar='FILE', help='output file')
p.add_argument('-i', '--header', metavar='VARNAME',
        help='generate C header file')
p.add_argument('-S', '--not-static', dest='static',
        action='store_const', const='', default='static ',
        help='do not output static keyword')
p.add_argument('file', nargs='*', help='file to put in file system')
options = p.parse_args()

files = options.file
files.sort()

magic = 0x466d6f52
h = struct.pack('<II', magic, len(files))
indexes = [0]
contents = []

index = 0
for f in files:
    index += len(f)
    indexes.append(index)
for f in files:
    with open(f) as fd:
        content = fd.read()
        index += len(content)
        indexes.append(index)
        contents.append(content)

indexes = struct.pack('<%dI' % len(indexes), *indexes)
names = ''.join(files)
contents = ''.join(contents)
fs = ''.join((h, indexes, names, contents))

if options.header:
    pad = (len(fs) + 3) / 4 * 4 - len(fs)
    if pad:
        fs += struct.pack('B', 0) * pad
    words = struct.unpack('<%dI' % (len(fs) / 4), fs)
    lines = [ '/* Auto-generated ROM FS from:' ]
    lines += [ ' *  - %s' % f for f in files ]
    lines += [ ' */', '', '%sconst uint32_t %s[] = {'
            % (options.static, options.header) ]
    for i in xrange(0, len(words), 4):
        w = [ '0x%08x,' % i for i in words[i:i+4] ]
        lines.append('    ' + ' '.join (w))
    lines += [ '};', '', '%sconst int %s_size = sizeof (%s);'
            % (options.static, options.header, options.header) ]
    options.output.write('\n'.join(lines))
else:
    options.output.write(fs)
