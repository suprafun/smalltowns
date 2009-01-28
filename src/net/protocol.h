
/**
 * Protocol used for packets
 * P = Player
 * A = Account Server
 * G = Game Server
 *
 * I = Integer
 * S = String
 * C = Char
 *
 * $Id$
 */

#ifndef ST_PROTOCOL_H
#define ST_PROTOCOL_H

enum
{
    // Player and Account Communication
    // Connecting
    PAMSG_CONNECT           = 0x0001, // I version
    APMSG_CONNECT_RESPONSE  = 0x0002, // C error

    // Registering
    PAMSG_REGISTER          = 0x0005,
    APMSG_REGISTER_RESPONSE = 0x0006,

    // Logging In
    PAMSG_LOGIN             = 0x0008,
    APMSG_LOGIN_RESPONSE    = 0x0009,

};

enum
{
    NOERROR             = 0,
    ERROR_INVALID_VER   = 1,
    ERROR_INVALID_NAME  = 2,
    ERROR_INVALID_PASS  = 3,
    ERROR_INVALID_EMAIL = 4,
};

#endif

