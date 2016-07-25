#ifndef ucoo_hal_usb_usb_desc_cdc_hh
#define ucoo_hal_usb_usb_desc_cdc_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2016 Nicolas Schodet
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
#include "ucoo/hal/usb/usb_desc.hh"

namespace ucoo {

enum
{
    USB_CLASS_CDC = 0x02,
    USB_CLASS_DATA = 0x0a,
    USB_CDC_SUBCLASS_ACM = 0x02,
    USB_CDC_PROTOCOL_AT = 0x01,
};

/// CDC header descriptor.
struct __attribute__ ((packed, aligned (1))) UsbCdcHeaderDesc
{
    /// Size of this descriptor in bytes.
    uint8_t bLength;
    /// Descriptor type (CS_INTERFACE: 0x24).
    uint8_t bDescriptorType;
    /// Descriptor sub type (header functional descriptor: 0).
    uint8_t bDescriptorSubtype;
    /// USB CDC release number (0x0110 for USB CDC 1.1).
    uint16_t bcdCDC;
};

/// Make a header descriptor.
constexpr UsbCdcHeaderDesc
usb_cdc_header_desc (uint16_t bcdCDC)
{
    return { sizeof (UsbCdcHeaderDesc), 0x24, 0, bcdCDC };
}

/// CDC call management descriptor.
struct __attribute__ ((packed, aligned (1))) UsbCdcCallManagementDesc
{
    /// Size of this descriptor in bytes.
    uint8_t bLength;
    /// Descriptor type (CS_INTERFACE: 0x24).
    uint8_t bDescriptorType;
    /// Descriptor sub type (header functional descriptor: 1).
    uint8_t bDescriptorSubtype;
    /// Capabilities:
    ///  - bit 0: device handles call management itself
    ///  - bit 1: device can send/receive call management information over
    ///  data class interface
    uint8_t bmCapabilities;
    /// Interface number of data class interface used for call management.
    uint8_t bDataInterface;
};

/// Make a call management descriptor.
constexpr UsbCdcCallManagementDesc
usb_cdc_call_management_desc (
    uint8_t bmCapabilities,
    uint8_t bDataInterface)
{
    return { sizeof (UsbCdcCallManagementDesc), 0x24, 1, bmCapabilities,
        bDataInterface };
}

/// CDC abstract control management descriptor.
struct __attribute__ ((packed, aligned (1)))
    UsbCdcAbstractControlManagementDesc
{
    /// Size of this descriptor in bytes.
    uint8_t bLength;
    /// Descriptor type (CS_INTERFACE: 0x24).
    uint8_t bDescriptorType;
    /// Descriptor sub type (header functional descriptor: 2).
    uint8_t bDescriptorSubtype;
    /// Capabilities:
    ///  - bit 0: support Set_Comm_Feature, Clear_Comm_Feature and
    ///  Get_Comm_Feature
    ///  - bit 1: support Set_Line_Coding, Set_Control_Line_State,
    ///  Get_Line_Coding and Serial_State notification
    ///  - bit 2: support Send_Break
    ///  - bit 3: support Network_Connection notification
    uint8_t bmCapabilities;
};

/// Make an abstract control management descriptor.
constexpr UsbCdcAbstractControlManagementDesc
usb_cdc_abstract_control_management_desc (
    uint8_t bmCapabilities)
{
    return { sizeof (UsbCdcAbstractControlManagementDesc), 0x24, 2,
        bmCapabilities };
}

/// CDC union descriptor.
struct __attribute__ ((packed, aligned (1))) UsbCdcUnionDesc
{
    /// Size of this descriptor in bytes.
    uint8_t bLength;
    /// Descriptor type (CS_INTERFACE: 0x24).
    uint8_t bDescriptorType;
    /// Descriptor sub type (header functional descriptor: 6).
    uint8_t bDescriptorSubtype;
    /// Interface number of the communication or data class interface
    /// designated as the controlling interface for the union.
    uint8_t bControlInterface;
    /// Interface number of the first subordinated interface in the union.
    uint8_t bSubordinateInterface0;
    /// Other subordinated interface number follows...
};

namespace details { // {{{

template<typename Desc>
constexpr Desc
usb_cdc_union_desc (Desc desc)
{
    return desc;
}

template<typename Desc, typename... Rest>
constexpr auto
usb_cdc_union_desc (Desc desc, uint8_t bSubordinateInterfaceN,
                    Rest... rest)
    -> decltype (usb_cdc_union_desc (
            Pack<Desc, uint8_t> (desc, 0), rest...))
{
    return usb_cdc_union_desc (
        Pack<Desc, uint8_t> (desc, bSubordinateInterfaceN),
        rest...);
}

} // namespace details }}}

/// Make an union descriptor.
template<typename... SubInterfaces>
constexpr auto
usb_cdc_union_desc (
    uint8_t bControlInterface,
    uint8_t bSubordinateInterface0,
    SubInterfaces... sub_interfaces)
    -> decltype (details::usb_cdc_union_desc (
            (UsbCdcUnionDesc) { 0, 0, 0, 0, 0 }, sub_interfaces...))
{
    using Ret = decltype (details::usb_cdc_union_desc (
            (UsbCdcUnionDesc) { 0, 0, 0, 0, 0 }, sub_interfaces...));
    return details::usb_cdc_union_desc (
        (UsbCdcUnionDesc) { sizeof (Ret), 0x24, 6, bControlInterface,
        bSubordinateInterface0 }, sub_interfaces...);
}

} // namespace ucoo

#endif // ucoo_hal_usb_usb_desc_cdc_hh
