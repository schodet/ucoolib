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
#include "avrisp_proto.hh"

#include "ucoolib/utils/bytes.hh"

#include "config/dev/avrisp.hh"

#include <cstring>
#include <algorithm>

namespace ucoo {

/// Protocol commands.
enum AvrIspProtoCmd
{
    AVRISP_PROTO_CMD_SIGN_ON = 0x01,
    AVRISP_PROTO_CMD_SET_PARAMETER = 0x02,
    AVRISP_PROTO_CMD_GET_PARAMETER = 0x03,
    AVRISP_PROTO_CMD_SET_DEVICE_PARAMETERS = 0x04,
    AVRISP_PROTO_CMD_OSCCAL = 0x05,
    AVRISP_PROTO_CMD_LOAD_ADDRESS = 0x06,
    AVRISP_PROTO_CMD_FIRMWARE_UPGRADE = 0x07,
    AVRISP_PROTO_CMD_ENTER_PROGMODE_ISP = 0x10,
    AVRISP_PROTO_CMD_LEAVE_PROGMODE_ISP = 0x11,
    AVRISP_PROTO_CMD_CHIP_ERASE_ISP = 0x12,
    AVRISP_PROTO_CMD_PROGRAM_FLASH_ISP = 0x13,
    AVRISP_PROTO_CMD_READ_FLASH_ISP = 0x14,
    AVRISP_PROTO_CMD_PROGRAM_EEPROM_ISP = 0x15,
    AVRISP_PROTO_CMD_READ_EEPROM_ISP = 0x16,
    AVRISP_PROTO_CMD_PROGRAM_FUSE_ISP = 0x17,
    AVRISP_PROTO_CMD_READ_FUSE_ISP = 0x18,
    AVRISP_PROTO_CMD_PROGRAM_LOCK_ISP = 0x19,
    AVRISP_PROTO_CMD_READ_LOCK_ISP = 0x1a,
    AVRISP_PROTO_CMD_READ_SIGNATURE_ISP = 0x1b,
    AVRISP_PROTO_CMD_READ_OSCCAL_ISP = 0x1c,
    AVRISP_PROTO_CMD_SPI_MULTI = 0x1d,
};

/// Protocol command status.
enum AvrIspProtoStatus
{
    AVRISP_PROTO_STATUS_CMD_OK = 0x00,

    AVRISP_PROTO_STATUS_CMD_TOUT = 0x80,
    AVRISP_PROTO_STATUS_RDY_BSY_TOUT = 0x81,
    AVRISP_PROTO_STATUS_SET_PARAM_MISSING = 0x82,

    AVRISP_PROTO_STATUS_CMD_FAILED = 0xc0,
    AVRISP_PROTO_STATUS_CKSUM_ERROR = 0xc1,
    AVRISP_PROTO_STATUS_CMD_UNKNOWN = 0xc9,
};

/// Protocol parameters.
enum AvrIspProtoParam
{
    AVRISP_PROTO_PARAM_BUILD_NUMBER_LOW = 0x80,
    AVRISP_PROTO_PARAM_BUILD_NUMBER_HIGH = 0x81,
    AVRISP_PROTO_PARAM_HW_VER = 0x90,
    AVRISP_PROTO_PARAM_SW_MAJOR = 0x91,
    AVRISP_PROTO_PARAM_SW_MINOR = 0x92,
    AVRISP_PROTO_PARAM_VTARGET = 0x94,
    AVRISP_PROTO_PARAM_VADJUST = 0x95,
    AVRISP_PROTO_PARAM_OSC_PSCALE = 0x96,
    AVRISP_PROTO_PARAM_OSC_CMATCH = 0x97,
    AVRISP_PROTO_PARAM_SCK_DURATION = 0x98,
    AVRISP_PROTO_PARAM_TOPCARD_DETECT = 0x9a,
    AVRISP_PROTO_PARAM_STATUS = 0x9c,
    AVRISP_PROTO_PARAM_DATA = 0x9d,
    AVRISP_PROTO_PARAM_RESET_POLARITY = 0x9e,
    AVRISP_PROTO_PARAM_CONTROLLER_INIT = 0x9f,
};

static inline AvrIspProtoStatus
avrisp_proto_isp_status (AvrIspResult isp_status)
{
    if (isp_status == AVRISP_OK)
        return AVRISP_PROTO_STATUS_CMD_OK;
    else if (isp_status == AVRISP_TIMEOUT)
        return AVRISP_PROTO_STATUS_CMD_TOUT;
    else
        return AVRISP_PROTO_STATUS_CMD_FAILED;
}

AvrIspProto::AvrIspProto (AvrIsp &isp)
    : isp_ (isp), sck_duration_us_ (1)
{
}

int
AvrIspProto::accept (uint8_t *data, int len)
{
    uint8_t r;
    AvrIspProtoStatus status;
    AvrIspResult isp_status;
    uint16_t size;
    /* Decode command. */
    switch (data[0])
    {
    case AVRISP_PROTO_CMD_SIGN_ON:
        if (len != 1) break;
        data[1] = AVRISP_PROTO_STATUS_CMD_OK;
        data[2] = sizeof (UCOO_CONFIG_DEV_AVRISP_PROTO_SIGNATURE);
        memcpy (&data[3], UCOO_CONFIG_DEV_AVRISP_PROTO_SIGNATURE,
                sizeof (UCOO_CONFIG_DEV_AVRISP_PROTO_SIGNATURE));
        return 3 + sizeof (UCOO_CONFIG_DEV_AVRISP_PROTO_SIGNATURE);
    case AVRISP_PROTO_CMD_SET_PARAMETER:
        if (len != 3) break;
        status = AVRISP_PROTO_STATUS_CMD_OK;
        switch (data[1])
        {
        case AVRISP_PROTO_PARAM_SCK_DURATION:
            sck_duration_us_ = std::max (data[2], static_cast<uint8_t> (1));
            break;
        case AVRISP_PROTO_PARAM_RESET_POLARITY:
            if (data[2] != 1)
                status = AVRISP_PROTO_STATUS_CMD_FAILED;
            break;
        default:
            status = AVRISP_PROTO_STATUS_CMD_FAILED;
            break;
        }
        data[1] = status;
        return 2;
    case AVRISP_PROTO_CMD_GET_PARAMETER:
        if (len != 2) break;
        status = AVRISP_PROTO_STATUS_CMD_OK;
        switch (data[1])
        {
        case AVRISP_PROTO_PARAM_SCK_DURATION:
            data[2] = sck_duration_us_;
            break;
        case AVRISP_PROTO_PARAM_RESET_POLARITY:
            data[2] = 1;
            break;
        case AVRISP_PROTO_PARAM_BUILD_NUMBER_LOW:
            data[2] = bytes_unpack (UCOO_CONFIG_DEV_AVRISP_PROTO_BUILD_NUMBER, 0);
            break;
        case AVRISP_PROTO_PARAM_BUILD_NUMBER_HIGH:
            data[2] = bytes_unpack (UCOO_CONFIG_DEV_AVRISP_PROTO_BUILD_NUMBER, 1);
            break;
        case AVRISP_PROTO_PARAM_HW_VER:
            data[2] = UCOO_CONFIG_DEV_AVRISP_PROTO_HW_VERSION;
            break;
        case AVRISP_PROTO_PARAM_SW_MAJOR:
            data[2] = bytes_unpack (UCOO_CONFIG_DEV_AVRISP_PROTO_SW_VERSION, 1);
            break;
        case AVRISP_PROTO_PARAM_SW_MINOR:
            data[2] = bytes_unpack (UCOO_CONFIG_DEV_AVRISP_PROTO_SW_VERSION, 0);
            break;
        default:
            status = AVRISP_PROTO_STATUS_CMD_FAILED;
            break;
        }
        data[1] = status;
        return status == AVRISP_PROTO_STATUS_CMD_OK ? 3 : 2;
    case AVRISP_PROTO_CMD_LOAD_ADDRESS:
        if (len != 5) break;
        isp_.load_address (bytes_pack (data[1], data[2], data[3], data[4]));
        data[1] = AVRISP_PROTO_STATUS_CMD_OK;
        return 2;
    case AVRISP_PROTO_CMD_ENTER_PROGMODE_ISP:
        if (len != 12) break;
        isp_status = isp_.enter_progmode (sck_duration_us_, data[1], data[2],
                                          data[3], data[4], data[5], data[6],
                                          data[7], &data[8]);
        data[1] = avrisp_proto_isp_status (isp_status);
        return 2;
    case AVRISP_PROTO_CMD_LEAVE_PROGMODE_ISP:
        if (len != 3) break;
        isp_.leave_progmode (data[1], data[2]);
        data[1] = AVRISP_PROTO_STATUS_CMD_OK;
        return 2;
    case AVRISP_PROTO_CMD_CHIP_ERASE_ISP:
        if (len != 7) break;
        isp_status = isp_.chip_erase (data[1], data[2], &data[3]);
        data[1] = avrisp_proto_isp_status (isp_status);
        return 2;
    case AVRISP_PROTO_CMD_PROGRAM_FLASH_ISP:
    case AVRISP_PROTO_CMD_PROGRAM_EEPROM_ISP:
        if (len < 10) break;
        size = bytes_pack (data[1], data[2]);
        if (len != 10 + size) break;
        isp_status = isp_.program_begin (size, data[3], data[4], data[5], data[6],
                                         data[7], &data[8], data[0]
                                         == AVRISP_PROTO_CMD_PROGRAM_FLASH_ISP);
        if (isp_status == AVRISP_OK)
            isp_status = isp_.program_continue (&data[10], size);
        if (isp_status == AVRISP_OK)
            isp_status = isp_.program_end ();
        data[1] = avrisp_proto_isp_status (isp_status);
        return 2;
    case AVRISP_PROTO_CMD_READ_FLASH_ISP:
    case AVRISP_PROTO_CMD_READ_EEPROM_ISP:
        if (len != 4) break;
        size = bytes_pack (data[1], data[2]);
        isp_status = isp_.read_begin (size, data[3], data[0]
                                      == AVRISP_PROTO_CMD_READ_FLASH_ISP);
        if (isp_status == AVRISP_OK)
            isp_status = isp_.read_continue (&data[2], size);
        if (isp_status == AVRISP_OK)
            isp_status = isp_.read_end ();
        if (isp_status == AVRISP_OK)
        {
            data[1] = AVRISP_PROTO_STATUS_CMD_OK;
            data[2 + size] = AVRISP_PROTO_STATUS_CMD_OK;
            return 2 + size + 1;
        }
        else
        {
            data[1] = AVRISP_PROTO_STATUS_CMD_FAILED;
            return 2;
        }
    case AVRISP_PROTO_CMD_PROGRAM_FUSE_ISP:
    case AVRISP_PROTO_CMD_PROGRAM_LOCK_ISP:
        if (len != 5) break;
        isp_.program_misc (&data[1]);
        data[1] = AVRISP_PROTO_STATUS_CMD_OK;
        data[2] = AVRISP_PROTO_STATUS_CMD_OK;
        return 3;
    case AVRISP_PROTO_CMD_READ_FUSE_ISP:
    case AVRISP_PROTO_CMD_READ_LOCK_ISP:
    case AVRISP_PROTO_CMD_READ_SIGNATURE_ISP:
    case AVRISP_PROTO_CMD_READ_OSCCAL_ISP:
        if (len != 6) break;
        r = isp_.read_misc (data[1], &data[2]);
        data[1] = AVRISP_PROTO_STATUS_CMD_OK;
        data[2] = r;
        data[3] = AVRISP_PROTO_STATUS_CMD_OK;
        return 4;
    case AVRISP_PROTO_CMD_SPI_MULTI:
        if (len < 4) break;
        size = data[2];
        if (len != 4 + size) break;
        isp_.multi (data[1], data[2], data[3], &data[4], &data[2]);
        data[1] = AVRISP_PROTO_STATUS_CMD_OK;
        data[2 + size] = AVRISP_PROTO_STATUS_CMD_OK;
        return 2 + size + 1;
    default:
        /* Unknown. */
        data[1] = AVRISP_PROTO_STATUS_CMD_UNKNOWN;
        return 2;
    }
    data[1] = AVRISP_PROTO_STATUS_CMD_FAILED;
    return 2;
}

} // namespace ucoo
