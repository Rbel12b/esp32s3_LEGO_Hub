#pragma once
#ifndef _LPF2_SERIAL_DEFINITIONS_H_
#define _LPF2_SERIAL_DEFINITIONS_H_

#define   BYTE_SYNC                     0x00                            // Synchronisation byte
#define   BYTE_ACK                      0x04                            // Acknowledge byte
#define   BYTE_NACK                     0x02                            // Not acknowledge byte

#define   MESSAGE_SYS                   0x00                            // System message
#define   MESSAGE_CMD                   0x40                            // Command message
#define   MESSAGE_INFO                  0x80                            // Info message
#define   MESSAGE_DATA                  0xC0                            // Data message
#define   GET_MESSAGE_TYPE(B)           (B & 0xC0)                      // Get message type

#define   CMD_TYPE                      0x00                            // CMD command - TYPE     (device type for VM reference)
#define   CMD_MODES                     0x01                            // CMD command - MODES    (number of supported modes 0=1)
#define   CMD_SPEED                     0x02                            // CMD command - SPEED    (maximun communication speed)
#define   CMD_SELECT                    0x03                            // CMD command - SELECT   (select mode)
#define   CMD_WRITE                     0x04                            // CMD command - WRITE    (write to device)
#define   GET_CMD_COMMAND(B)            (B & 0x07)                      // Get CMD command

#define   GET_MODE(B)                   (B & 0x07)                      // Get mode

#define   GET_MESSAGE_LENGTH(B)         (1 << ((B >> 3) & 0x07))

#define   MAKE_CMD_COMMAND(C,LC)        (MESSAGE_CMD + (C & 0x07) + ((LC & 0x07) << 3))
#endif