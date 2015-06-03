#ifndef ucoo_arch_host_host_hh
#define ucoo_arch_host_host_hh
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
#include "ucoo/arch/host/mex/mex_node.hh"

#include <string>
#include <list>
#include <memory>

namespace ucoo {

/// Host support class.
class Host
{
  public:
    /// Constructor, with default instance name.
    Host (const std::string &default_instance_name);
    /// Get instance name, strip a number of tail components if requested.
    std::string get_instance (int strip = 0) const;
    /// Get Mex node.  Connect if not done yet.
    mex::Node &get_node ();
    /// Declare a command line option without value.
    void add_option (char opt, const char *help);
    /// Declare a command line option with value.
    void add_option (char opt, const char *metavar, const char *help,
                     const std::string &default_value);
    /// Test if option was set.
    bool is_option_set (char opt) const;
    /// Get value for option.
    const std::string &get_option (char opt) const;
    /// To be done once, parse options.
    void parse_options ();
  private:
    /// Display usage message and exit.
    void usage (int ret);
  private:
    /// Instance name, from default or from command line.
    std::string instance_name_;
    /// Mex node, or 0 if not connected yet.
    std::auto_ptr<mex::Node> node_;
    /// Store options definitions and values.
    struct Option
    {
        const char *metavar;
        const char *help;
        bool set;
        std::string value;
    };
    typedef std::map<char, Option> Options;
    Options options_;
    /// Program name, used for messages.
    const char *program_name_;
    /// Option parsing is done.
    bool option_parsed_;
};

} // namespace ucoo

#endif // ucoo_arch_host_host_hh
