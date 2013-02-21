#ifndef ucoolib_arch_host_host_hh
#define ucoolib_arch_host_host_hh
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
#include "ucoolib/arch/host/mex/mex_node.hh"

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

#endif // ucoolib_arch_host_host_hh
