#ifndef ucoo_hal_usb_usb_desc_hh
#define ucoo_hal_usb_usb_desc_hh
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
#include "ucoo/common.hh"

namespace ucoo {

namespace details { // {{{

/// Pack several elements in a new types (std::tuple constructor is not
/// constexpr in C++11).
template<typename... Elements>
struct Pack;

template<typename Head, typename... Tail>
struct __attribute__ ((packed, aligned (1))) Pack<Head, Tail...>
{
    Head head;
    Pack<Tail...> tail;
    constexpr Pack (Head head_, Tail... tail_)
        : head (head_), tail (tail_...) { }
};

template<typename Head>
struct __attribute__ ((packed, aligned (1))) Pack<Head>
{
    Head head;
    constexpr Pack (Head head_) : head (head_) { }
};

/// Take a pack, unpack the Nth value.
template<int N, typename Pack>
struct Unpack;

template<typename Head, typename... Tail>
struct Unpack<0, Pack<Head, Tail...>>
{
    static constexpr const Head &unpack (const Pack<Head, Tail...> &pack)
    {
        return pack.head;
    }
};

template<int N, typename Head, typename... Tail>
struct Unpack<N, Pack<Head, Tail...>>
{
    static constexpr auto unpack (const Pack<Head, Tail...> &pack)
        -> decltype (Unpack<N - 1, Pack<Tail...>>::unpack (pack.tail))
    {
        return Unpack<N - 1, Pack<Tail...>>::unpack (pack.tail);
    }
};

/// Type whose only purpose is to have a sequence of integer as template
/// parameter.
template<int ...>
struct Range
{
};

template<int N, int... Next>
struct RangeGen
{
    using Type = typename RangeGen<N - 1, N - 1, Next...>::Type;
};

template<int... Next>
struct RangeGen<0, Next...>
{
    using Type = Range<Next...>;
};

} // namespace details }}}

/// Device descriptor.
struct __attribute__ ((packed, aligned (1))) UsbDeviceDesc
{
    /// Size of this descriptor in bytes.
    uint8_t bLength;
    /// Device descriptor type (1).
    uint8_t bDescriptorType;
    /// USB spec release number (0x0200 for USB 2.0).
    uint16_t bcdUSB;
    /// Class code assigned by USB-IF (0x00: each interface defines its own
    /// class, 0xff: vendor defined).
    uint8_t bDeviceClass;
    /// SubClass code assigned by USB-IF.
    uint8_t bDeviceSubClass;
    /// Protocol code assigned by USB-IF.
    uint8_t bDeviceProtocol;
    /// Max packet size for endpoint 0 (8, 16, 32 or 64).
    uint8_t bMaxPacketSize0;
    /// Vendor ID.
    uint16_t idVendor;
    /// Product ID.
    uint16_t idProduct;
    /// Device release number.
    uint16_t bcdDevice;
    /// Index of string descriptor describing manufacturer (0 if no string).
    uint8_t iManufacturer;
    /// Index of string descriptor describing product (0 if no string).
    uint8_t iProduct;
    /// Index of string descriptor describing device serial number (0 if no
    /// string).
    uint8_t iSerialNumber;
    /// Number of possible configurations.
    uint8_t bNumConfigurations;
};

/// Make a device descriptor.
constexpr UsbDeviceDesc
usb_device_desc (
    uint16_t bcdUSB,
    uint8_t bDeviceClass,
    uint8_t bDeviceSubClass,
    uint8_t bDeviceProtocol,
    uint8_t bMaxPacketSize0,
    uint16_t idVendor,
    uint16_t idProduct,
    uint16_t bcdDevice,
    uint8_t iManufacturer,
    uint8_t iProduct,
    uint8_t iSerialNumber,
    uint8_t bNumConfigurations)
{
    return { sizeof (UsbDeviceDesc), 1, bcdUSB, bDeviceClass, bDeviceSubClass,
        bDeviceProtocol, bMaxPacketSize0, idVendor, idProduct, bcdDevice,
        iManufacturer, iProduct, iSerialNumber, bNumConfigurations };
}

/// Configuration descriptor.
struct __attribute__ ((packed, aligned (1))) UsbConfigurationDesc
{
    /// Size of this descriptor in bytes.
    uint8_t bLength;
    /// Configuration descriptor type (2).
    uint8_t bDescriptorType;
    /// Total number of bytes in this descriptor and all the following
    /// descriptors.
    uint16_t wTotalLength;
    /// Number of interfaces supported by this configuration.
    uint8_t bNumInterfaces;
    /// Value used by Set Configuration to select this configuration.
    uint8_t bConfigurationValue;
    /// Index of string descriptor describing configuration (0 if no string).
    uint8_t iConfiguration;
    /// Attributes:
    ///  - bit 7: must be set
    ///  - bit 6: self-powered
    ///  - bit 5: remote wakeup
    ///  - others: must be 0
    uint8_t bmAttributes;
    /// Maximum current drawn by device in this configuration in units of
    /// 2 mA.
    uint8_t bMaxPower;
};

/// Make a configuration descriptor.
template<typename... SubDesc>
constexpr details::Pack<UsbConfigurationDesc, SubDesc...>
usb_configuration_desc (
    uint8_t bNumInterfaces,
    uint8_t bConfigurationValue,
    uint8_t iConfiguration,
    uint8_t bmAttributes,
    uint8_t bMaxPower,
    SubDesc... sub_descs)
{
    return details::Pack<UsbConfigurationDesc, SubDesc...> (
        { sizeof (UsbConfigurationDesc), 2,
        sizeof (details::Pack<UsbConfigurationDesc, SubDesc...>),
        bNumInterfaces, bConfigurationValue, iConfiguration, bmAttributes,
        bMaxPower }, sub_descs...);
}

/// Interface descriptor.
struct __attribute__ ((packed, aligned (1))) UsbInterfaceDesc
{
    /// Size of this descriptor in bytes.
    uint8_t bLength;
    /// Interface descriptor type (4).
    uint8_t bDescriptorType;
    /// Number identifying this interface (start at 0).
    uint8_t bInterfaceNumber;
    /// Value used to select this alternate setting for this interface (0 for
    /// first and default setting).
    uint8_t bAlternateSetting;
    /// Number of endpoints used by this interface.
    uint8_t bNumEndpoints;
    /// Class code assigned by USB-IF.
    uint8_t bInterfaceClass;
    /// SubClass Code assigned by USB-IF.
    uint8_t bInterfaceSubClass;
    /// Protocol Code assigned by USB-IF.
    uint8_t bInterfaceProtocol;
    /// Index of string descriptor describing interface (0 if no string).
    uint8_t iInterface;
};

/// Make an interface descriptor.
template<typename... SubDesc>
constexpr details::Pack<UsbInterfaceDesc, SubDesc...>
usb_interface_desc (
    uint8_t bInterfaceNumber,
    uint8_t bAlternateSetting,
    uint8_t bNumEndpoints,
    uint8_t bInterfaceClass,
    uint8_t bInterfaceSubClass,
    uint8_t bInterfaceProtocol,
    uint8_t iInterface,
    SubDesc... sub_descs)
{
    return details::Pack<UsbInterfaceDesc, SubDesc...> (
        { sizeof (UsbInterfaceDesc), 4, bInterfaceNumber, bAlternateSetting,
        bNumEndpoints, bInterfaceClass, bInterfaceSubClass,
        bInterfaceProtocol, iInterface }, sub_descs...);
}

/// Endpoint descriptor.
struct __attribute__ ((packed, aligned (1))) UsbEndpointDesc
{
    /// Size of this descriptor in bytes.
    uint8_t bLength;
    /// Endpoint descriptor type (5).
    uint8_t bDescriptorType;
    /// The address of this endpoint within the device.
    ///  - bit 7: direction: 0: OUT, 1: IN
    ///  - bit 3-0: endpoint number
    uint8_t bEndpointAddress;
    /// Attributes:
    ///  - bit 7-6: must be 0
    ///  - bit 5-4: usage type:
    ///     - 00: data endpoint
    ///     - 01: feedback endpoint
    ///     - 10: implicit feedback data endpoint
    ///  - bit 3-2: synchronisation type
    ///     - 00: none
    ///     - 01: asynchronous
    ///     - 10: adaptive
    ///     - 11: synchronous
    ///  - bit 1-0: transfer type
    ///     - 00: control
    ///     - 01: isochronous
    ///     - 10: bulk
    ///     - 11: interrupt
    uint8_t bmAttributes;
    /// Maximum packet size this endpoint can send or receive when this
    /// configuration is selected.
    uint16_t wMaxPacketSize;
    /// Interval for polling endpoint for data transfers. Expressed in frames
    /// (ms) for low/full speed.
    uint8_t bInterval;
};

/// Make an endpoint descriptor.
template<typename... SubDesc>
constexpr details::Pack<UsbEndpointDesc, SubDesc...>
usb_endpoint_desc (
    uint8_t bEndpointAddress,
    uint8_t bmAttributes,
    uint16_t wMaxPacketSize,
    uint8_t bInterval,
    SubDesc... sub_descs)
{
    return details::Pack<UsbEndpointDesc, SubDesc...> (
        { sizeof (UsbEndpointDesc), 5, bEndpointAddress, bmAttributes,
        wMaxPacketSize, bInterval }, sub_descs...);
}

/// Make a bulk endpoint descriptor, shortcut.
template<typename... SubDesc>
constexpr details::Pack<UsbEndpointDesc, SubDesc...>
usb_endpoint_desc_bulk (
    uint8_t bEndpointAddress,
    uint16_t wMaxPacketSize,
    SubDesc... sub_descs)
{
    return usb_endpoint_desc (bEndpointAddress, 0x02, wMaxPacketSize, 0,
                              sub_descs...);
}

enum
{
    USB_LANGUAGE_EN_US = 0x0409,
};

/// String descriptor.
template<int Length>
struct __attribute__ ((packed, aligned (1))) UsbStringDesc
{
    /// Size of this descriptor in bytes.
    uint8_t bLength;
    /// String descriptor type (3).
    uint8_t bDescriptorType;
    /// Unicode encoded string.
    char16_t bString[Length];
};

namespace details { // {{{

template<int LengthWithNull, int... Rng>
constexpr UsbStringDesc<LengthWithNull - 1>
usb_string_desc (const char16_t (&string)[LengthWithNull],
                 const Range<Rng...> &)
{
    return { sizeof (UsbStringDesc<LengthWithNull - 1>), 3,
        { string[Rng]... } };
}

} // namespace details }}}

/// Make a string descriptor.
template<int LengthWithNull>
constexpr UsbStringDesc<LengthWithNull - 1>
usb_string_desc (const char16_t (&string)[LengthWithNull])
{
    return details::usb_string_desc (
        string, typename details::RangeGen<LengthWithNull - 1>::Type ());
}

/// Make the first string descriptor (language descriptor).
template<typename... Languages>
constexpr UsbStringDesc<sizeof... (Languages)>
usb_string_desc (Languages... languages)
{
    return { sizeof (UsbStringDesc<sizeof... (Languages)>), 3,
        { static_cast<char16_t> (languages)... } };
}

/// Make a descriptors pack.
template<typename... Descs>
constexpr details::Pack<Descs...>
usb_descs_pack (Descs... descs)
{
    return details::Pack<Descs...> (descs...);
}

namespace details {

/// Descriptor with its size.
struct UsbDesc
{
    const char *desc;
    int desc_size;
    template<typename Desc>
    constexpr UsbDesc (const Desc &desc_)
        : desc (reinterpret_cast<const char *> (&desc_)),
          desc_size (sizeof (desc_)) { }
};

/// Descriptors with their size.
template<typename... Elements>
struct UsbDescs;

template<typename Head>
struct UsbDescs<Head>
{
    UsbDesc head;
    constexpr UsbDescs (const Pack<Head> &pack)
        : head (pack.head) { }
};

template<typename Head, typename... Tail>
struct UsbDescs<Head, Tail...>
{
    UsbDesc head;
    UsbDescs<Tail...> tail;
    constexpr UsbDescs (const Pack<Head, Tail...> &pack)
        : head (pack.head), tail (pack.tail) { }
};

} // namespace details }}}

/// Make an object with descriptors and their sizes.
template<typename... Descs>
constexpr details::UsbDescs<Descs...>
usb_descs (const details::Pack<Descs...> &descs_pack)
{
    return descs_pack;
}

} // namespace ucoo

#endif // ucoo_hal_usb_usb_desc_hh
