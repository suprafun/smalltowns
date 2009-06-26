
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

const int CLIENT_VERSION = 0x0005;

enum
{
    // Player and Account Communication
    // Connecting
    PAMSG_CONNECT               = 0x0001, // I version
    APMSG_CONNECT_RESPONSE      = 0x0002, // C error

    // Registering
    PAMSG_REGISTER              = 0x0005, // S username, S password
    APMSG_REGISTER_RESPONSE     = 0x0006, // C error

    // Logging In
    PAMSG_LOGIN                 = 0x0008, // S username, S password
    APMSG_LOGIN_RESPONSE        = 0x0009, // C error

    // Character screen
    PAMSG_CHAR_LIST             = 0x0010, //
    APMSG_CHAR_LIST_RESPONSE    = 0x0011, // I num, { I id, S character name, I hair, I level, I rights }
    PAMSG_CHAR_CREATE           = 0x0012, // S character name, I hair
    APMSG_CHAR_CREATE_RESPONSE  = 0x0013, // C error [ I id ]
    PAMSG_CHAR_CHOOSE           = 0x0016, // I slot
    APMSG_CHAR_CHOOSE_RESPONSE  = 0x0017, // C error [ I id ]

    // Game server info
    APMSG_GAME_SERVER           = 0x0100, // S ip/hostname, I port, I tag

    // Player and Game Communication
    // Connecting
    PGMSG_CONNECT               = 0x0800, // I id, I tag
    GPMSG_CONNECT_RESPONSE      = 0x0801, // C error

    // map info
    GPMSG_LOAD_MAP              = 0x0810, // S name of map

};

enum
{
	ERR_NONE            = 0,
	ERR_INVALID_VER     = 1,
	ERR_INVALID_NAME    = 2,
	ERR_INVALID_PASS    = 3,
	ERR_INVALID_EMAIL   = 4,
	ERR_TAKEN_NAME      = 5,

	ERR_TOO_MANY        = 6,
	ERR_INVALID_CHAR    = 7,

	ERR_INVALID_TAG     = 8,
};

#endif

