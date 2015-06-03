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
