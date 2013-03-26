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
#include "adc.host.hh"

#include <string>

namespace ucoo {

/// Shared context between instances.
class AdcHostShared
{
  public:
    /// Constructor, connect to mex node.
    AdcHostShared (Host &host);
    /// Register a new instance.
    void register_instance (Host &host, AdcHost &instance, const char *name);
  private:
    /// Handle value from Mex.
    void handle_adc_channel (mex::Msg &msg);
  private:
    Host &host_;
    typedef std::map<std::string, AdcHost *> Instances;
    Instances instances_;
};

AdcHostShared::AdcHostShared (Host &host)
    : host_ (host)
{
    std::string instance = host_.get_instance ();
    mex::Node &node = host_.get_node ();
    mex::mtype_t mtype = node.reserve (instance + ":adc_channel");
    node.handler_register (mtype, *this, &AdcHostShared::handle_adc_channel);
}

void
AdcHostShared::register_instance (Host &host, AdcHost &instance,
                                  const char *name)
{
    assert (&host == &host_);
    std::pair<Instances::iterator, bool> r =
        instances_.insert (Instances::value_type (name, &instance));
    assert (r.second);
}

void
AdcHostShared::handle_adc_channel (mex::Msg &msg)
{
    int ivalue, namelen;
    msg.pop ("l") >> ivalue;
    namelen = msg.len ();
    std::string name (msg.pop (namelen), namelen);
    Instances::iterator i = instances_.find (name);
    assert (i != instances_.end ());
    double fvalue = ivalue / (double) (1u << 31);
    i->second->value_ = fvalue * i->second->resolution_;
}

AdcHostShared *AdcHost::shared_;

AdcHost::AdcHost (Host &host, const char *name, int resolution)
    : resolution_ (resolution), value_ (0)
{
    if (!shared_)
    {
        static AdcHostShared shared (host);
        shared_ = &shared;
    }
    shared_->register_instance (host, *this, name);
}

} // namespace ucoo
