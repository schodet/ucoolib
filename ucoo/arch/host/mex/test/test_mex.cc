// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include "ucoo/arch/host/mex/mex_msg.hh"
#include "ucoo/arch/host/mex/mex_node.hh"
#include "ucoo/arch/arch.hh"
#include "ucoo/common.hh"

#include <cstdio>
#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace ucoo::mex;

/// Global error number, should be zero at end of test.
static int error;

/// Helper function, print a buffer.
static void
printbuf (const char *head, const char *buf, int count)
{
    printf ("%s:", head);
    for (int i = 0; i < count; i++)
        printf (" %02x", (unsigned char) buf[i]);
    printf ("\n");
}

/// Helper function, check for equality.
static void
check (int64_t a, int64_t b, const char *name = NULL)
{
    printf ("%s: %Lx\n", name ? name : "pop", a);
    if (a != b)
    {
        printf (" expected: %Lx\n", b);
        error++;
    }
}

/// Test writer, will check what is written and display it.
class TestWriter : public MsgWriter
{
    const char *exbuf_;
    int excount_;
  public:
    template<int count>
    TestWriter (const char (&buf)[count])
        : exbuf_ (buf), excount_ (count) { }
    void write (const char *buf, int count)
    {
        printbuf ("write", buf, count);
        if (count != excount_ || memcmp (buf, exbuf_, count) != 0)
        {
            printbuf (" expected", exbuf_, excount_);
            error++;
        }
    }
};

/// Test reader, will return a predefined message and display it.
class TestReader : public MsgReader
{
    const char *buf_;
    int count_;
  public:
    template<int count>
    TestReader (const char (&buf)[count])
        : buf_ (buf), count_ (count) { }
    int size () { return count_; }
    void read (char *buf)
    {
        printbuf ("read", buf_, count_);
        memcpy (buf, buf_, count_);
    }
};

/// Test message class.
void
test_msg ()
{
    {
        Msg m (MTYPE_DATE);
        const char w1b[] = { 0x01 };
        TestWriter w1 (w1b);
        m.write (w1);
        m.push ("BHl") << 1 << 2 << 3;
        m.push ("\x04\x05", 2);
        const char w2b[] = { 0x01, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03,
            0x04, 0x05 };
        TestWriter w2 (w2b);
        m.write (w2);
    }
    {
        uint32_t ua, ub, uc;
        int32_t a, b, c;
        const char rb[] = { 0x01, 0xff, 0xfe, 0xfd, 0xfc, 1, 2, 3, 4 };
        TestReader r (rb);
        Msg m (r);
        check (m.mtype (), 1, "mtype");
        check (m.len (), 8, "len");
        m.pop ("BBH") >> ua >> ub >> uc;
        check (ua, 0xff); check (ub, 0xfe); check (uc, 0xfdfc);
        m.pop ("BBH") >> ua >> ub >> uc;
        check (ua, 1); check (ub, 2); check (uc, 0x304);
        m = Msg (r);
        m.pop ("bbh") >> a >> b >> c;
        check (a, -1); check (b, -2); check (c, -0x204);
        m.pop ("bbh") >> a >> b >> c;
        check (a, 1); check (b, 2); check (c, 0x304);
        m = Msg (r);
        m.pop ("LL") >> ua >> ub;
        check (ua, 0xfffefdfc); check (ub, 0x01020304);
        m = Msg (r);
        m.pop ("ll") >> a >> b;
        check (a, -0x010204); check (b, 0x01020304);
        m = Msg (r);
        const char *s;
        s = m.pop (8);
        printbuf ("pop", s, 8);
        if (memcmp (s, &rb[1], 8) != 0)
        {
            printbuf (" expected", &rb[1], 8);
            error++;
        }
    }
    {
        Msg m (static_cast<mtype_t> (0x20));
        m.push ("bb") << 1 << 2;
        m.encapsulate (static_cast<mtype_t> (0x21), "b") << 5;
        check (m.mtype (), 0x21, "mtype");
        check (m.len (), 4, "len");
        const char w1b[] = { 0x21, 0x05, 0x20, 0x01, 0x02 };
        TestWriter w1 (w1b);
        m.write (w1);
        m.pop ("b");
        m.decapsulate ();
        check (m.mtype (), 0x20, "mtype");
        check (m.len (), 2, "len");
    }
}

void
test_node_1 ()
{
    Node node;
    mtype_t hello_mtype, world_mtype;
    hello_mtype = node.reserve ("hello");
    world_mtype = node.reserve ("world");
    node.wait (42);
    Msg hello (hello_mtype);
    hello.push ("l") << 42;
    node.send (hello);
    Msg world (world_mtype);
    world.push ("bh") << 12 << 5678;
    std::auto_ptr<Msg> resp (node.request (world));
    int sum;
    resp->pop ("l") >> sum;
    ucoo::assert (sum == 12 + 5678);
    node.wait (42 * 2);
}

void
test_node_2_handle_hello (Node &, Msg &msg)
{
    int pass;
    msg.pop ("l") >> pass;
    ucoo::assert (pass == 42);
    printf ("hello\n");
}

void
test_node_2_handle_world (Node &node, Msg &msg)
{
    int a, b;
    msg.pop ("bh") >> a >> b;
    printf ("world\n");
    Msg resp (msg.mtype ());
    resp.push ("l") << a + b;
    node.response (resp);
}

void
test_node_2 ()
{
    Node node;
    mtype_t hello_mtype, world_mtype;
    hello_mtype = node.reserve ("hello");
    world_mtype = node.reserve ("world");
    node.handler_register (hello_mtype, test_node_2_handle_hello);
    node.handler_register (world_mtype, test_node_2_handle_world);
    node.wait ();
}

/// Test node operation, need a Mex hub to work.
void
test_node ()
{
    int pid;
    pid = fork ();
    ucoo::assert_perror (pid != -1);
    if (pid)
    {
        test_node_1 ();
        int status;
        waitpid (pid, &status, 0);
    }
    else
    {
        test_node_2 ();
        ucoo::assert_unreachable ();
    }
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    test_msg ();
    test_node ();
    printf ("%s\n", error ? "FAIL" : "PASS");
    return error ? 1 : 0;
}
