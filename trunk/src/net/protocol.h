
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

const int CLIENT_VERSION = 0x0002;

enum
{
	// Player and Account Communication
	// Connecting
	PAMSG_CONNECT           = 0x0001, // I version
	APMSG_CONNECT_RESPONSE  = 0x0002, // C error

	// Registering
	PAMSG_REGISTER          = 0x0005, // S username, S password
	APMSG_REGISTER_RESPONSE = 0x0006, // C error

	// Logging In
	PAMSG_LOGIN             = 0x0008, // S username, S password
	APMSG_LOGIN_RESPONSE    = 0x0009, // C error

};

enum
{
	ERR_NONE            = 0,
	ERR_INVALID_VER     = 1,
	ERR_INVALID_NAME    = 2,
	ERR_INVALID_PASS    = 3,
	ERR_INVALID_EMAIL   = 4,
	ERR_TAKEN_NAME      = 5,
};

#endif

