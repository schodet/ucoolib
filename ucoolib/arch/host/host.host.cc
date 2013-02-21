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
#include "host.hh"

#include "ucoolib/arch/arch.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <unistd.h>

namespace ucoo {

Host::Host (const std::string &default_instance_name)
    : program_name_ ("program"), option_parsed_ (false)
{
    add_option ('i', "NAME", "instance name", default_instance_name);
    add_option ('h', "display this help message");
}

std::string
Host::get_instance (int strip) const
{
    assert (strip >= 0);
    std::string instance = get_option ('i');
    if (strip)
    {
        std::string::size_type end = std::string::npos;
        do
        {
            end = instance.rfind (':', end - 1);
            if (end == std::string::npos)
                end = 0;
        } while (end != 0 && --strip);
        instance.erase (end);
    }
    return instance;
}

mex::Node &
Host::get_node ()
{
    if (!node_.get ())
        node_.reset (new mex::Node);
    return *node_;
}

void
Host::add_option (char opt, const char *help)
{
    Option o;
    o.metavar = 0;
    o.help = help;
    o.set = false;
    if (!options_.insert (Options::value_type (opt, o)).second)
        assert_unreachable ();
}

void
Host::add_option (char opt, const char *metavar, const char *help,
                  const std::string &default_value)
{
    Option o;
    o.metavar = metavar;
    o.help = help;
    o.set = false;
    o.value = default_value;
    if (!options_.insert (Options::value_type (opt, o)).second)
        assert_unreachable ();
}

bool
Host::is_option_set (char opt) const
{
    assert (option_parsed_);
    Options::const_iterator i = options_.find (opt);
    assert (i != options_.end ());
    return i->second.set;
}

const std::string &
Host::get_option (char opt) const
{
    assert (option_parsed_);
    Options::const_iterator i = options_.find (opt);
    assert (i != options_.end ());
    return i->second.value;
}

void
Host::parse_options ()
{
    // Get program arguments.
    int argc;
    const char **argv;
    arch_get_args (argc, argv);
    // Save program name.
    const char *slash = std::strrchr (argv[0], '/');
    program_name_ = slash ? slash + 1 : argv[0];
    // Parse.
    std::string getoptdesc;
    for (Options::const_iterator i = options_.begin ();
         i != options_.end (); ++i)
    {
        getoptdesc.push_back (i->first);
        if (i->second.metavar)
            getoptdesc.push_back (':');
    }
    int c;
    const char *getoptdescs = getoptdesc.c_str ();
    char **getopt_argv = const_cast<char **> (argv);
    while ((c = getopt (argc, getopt_argv, getoptdescs)) != -1)
    {
        if (c == '?')
            usage (1);
        else
        {
            Options::iterator i = options_.find (c);
            assert (i != options_.end ());
            i->second.set = true;
            if (i->second.metavar)
                i->second.value = optarg;
        }
    }
    if (optind != argc)
    {
        fprintf (stderr, "%s: too many arguments\n", argv[0]);
        usage (1);
    }
    option_parsed_ = true;
    if (is_option_set ('h'))
        usage (0);
}

void
Host::usage (int ret)
{
    FILE *out = ret == 0 ? stdout : stderr;
    fprintf (out, "Usage: %s [options...]\n\nOptions:\n", program_name_);
    size_t max_meta = 0;
    for (Options::const_iterator i = options_.begin ();
         i != options_.end (); ++i)
    {
        if (i->second.metavar)
            max_meta = std::max (max_meta,
                                 std::strlen (i->second.metavar) + 1);
    }
    for (Options::const_iterator i = options_.begin ();
         i != options_.end (); ++i)
    {
        const Option &o = i->second;
        fprintf (out, "  -%c %-*s %s\n", i->first,
                 static_cast<int> (max_meta),
                 o.metavar ? o.metavar : "", o.help);
    }
    exit (ret);
}

} // namespace ucoo
