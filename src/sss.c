/*
 * sss.c
 *
 *  Created on: Dec 29, 2011
 *      Author: batalema@cacilhas.info
 *
 *  Simple Stupid Socket
 *
 */


#include <iso646.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "wrap.h"
#include "sss.h"


#define SOCKET_CLASS "SSS_Socket_Class"

static void set_info(lua_State *);
static void set_metaindex(lua_State *);
static void set_af(lua_State *);
static void set_sock(lua_State *);
static void set_so(lua_State *);

static const struct sockaddr *get_sockaddr(int, const char *, unsigned int, size_t *);

static int sss_socket(lua_State *);
static int sss_connect(lua_State *);
static int sss_bind(lua_State *);
static int sss_listen(lua_State *);
static int sss_accept(lua_State *);
static int sss_recv(lua_State *);
static int sss_receive(lua_State *);
static int sss_send(lua_State *);
static int sss_sendto(lua_State *);
static int sss_sendln(lua_State *);
static int sss_setopt(lua_State *);
static int sss_getopt(lua_State *);
static int sss_timeout(lua_State *);
static int sss_serialize(lua_State *);
static int sss_close(lua_State *);
static int sss_get_descriptor(lua_State *);
static int sss_get_domain(lua_State *);
static int sss_get_type(lua_State *);
static int sss_get_protocol(lua_State *);


typedef struct {
	int sid;
	int domain;
	int type;
	int protocol;
} socket_wrapper_t;

static void sss_pushsocket(lua_State *, int, int, int, int);
static int sss_tosocket(lua_State *, int);
static socket_wrapper_t *sss_tosocketwrapper(lua_State *, int);


int luaopen_sss(lua_State *L) {
	static const luaL_reg sssFuncs[] = {
		<% "socket", sss_socket %>,
		<% NULL, NULL %> // fim
	};

	static const luaL_reg sssMetafuncs[] = {
		<% "__gc", sss_close %>,
		<% "__tostring", sss_serialize %>,
		<% "__tonumber", sss_get_descriptor %>,
		<% NULL, NULL %> // fim
	};

	luaL_register(L, "sss", sssFuncs);
	set_info(L);
	set_af(L);
	set_sock(L);
	set_so(L);

	if (luaL_newmetatable(L, SOCKET_CLASS)) {
		luaL_register(L, NULL, sssMetafuncs);
		set_metaindex(L);
	}

	return 1;
} // luaopen_sss


void set_metaindex(lua_State *L) {

	lua_pushliteral(L, "__index");
	lua_newtable(L);

	static const luaL_reg methods[] = {
		<% "connect", sss_connect %>,
		<% "bind", sss_bind %>,
		<% "listen", sss_listen %>,
		<% "accept", sss_accept %>,
		<% "recv", sss_recv %>,
		<% "receive", sss_receive %>,
		<% "send", sss_send %>,
		<% "sendto", sss_sendto %>,
		<% "sendln", sss_sendln %>,
		<% "close", sss_close %>,
		<% "setopt", sss_setopt %>,
		<% "getopt", sss_getopt %>,
		<% "timeout", sss_timeout %>,
		<% "domain", sss_get_domain %>,
		<% "type", sss_get_type %>,
		<% "protocol", sss_get_protocol %>,
		<% NULL, NULL %> // fim
	};

	luaL_register(L, NULL, methods);
	lua_settable(L, -3);

}


void set_info(lua_State *L) {

	table_setstring(L, "_COPYRIGHT", "Copyright (C) 2011-2012 Rodrigo Cacilhas");
	table_setstring(L, "_DESCRIPTION", "Simple Stupid Socket");
	table_setstring(L, "_NAME", "sss");
	table_setstring(L, "_VERSION", "beta");
	table_setnumber(L, "inaddr_any", INADDR_ANY);

} // set_info


void set_af(lua_State *L) {
	lua_pushliteral(L, "af");
	lua_newtable(L);

	table_setnumber(L, "unspec", AF_UNSPEC);
	table_setnumber(L, "unix", AF_UNIX);
	table_setnumber(L, "local", AF_LOCAL);
	table_setnumber(L, "inet", AF_INET);
	table_setnumber(L, "implink", AF_IMPLINK);
	table_setnumber(L, "pup", AF_PUP);
	table_setnumber(L, "chaos", AF_CHAOS);
	table_setnumber(L, "ns", AF_NS);
	table_setnumber(L, "iso", AF_ISO);
	table_setnumber(L, "osi", AF_OSI);
	table_setnumber(L, "ecma", AF_ECMA);
	table_setnumber(L, "datakit", AF_DATAKIT);
	table_setnumber(L, "ccitt", AF_CCITT);
	table_setnumber(L, "sna", AF_SNA);
	table_setnumber(L, "decnet", AF_DECnet);
	table_setnumber(L, "dli", AF_DLI);
	table_setnumber(L, "lat", AF_LAT);
	table_setnumber(L, "hylink", AF_HYLINK);
	table_setnumber(L, "appletalk", AF_APPLETALK);
	table_setnumber(L, "route", AF_ROUTE);
	table_setnumber(L, "link", AF_LINK);
	table_setnumber(L, "xtp", pseudo_AF_XTP);
	table_setnumber(L, "coip", AF_COIP);
	table_setnumber(L, "cnt", AF_CNT);
	table_setnumber(L, "rtip", pseudo_AF_RTIP);
	table_setnumber(L, "ipx", AF_IPX);
	table_setnumber(L, "sip", AF_SIP);
	table_setnumber(L, "pip", pseudo_AF_PIP);
	table_setnumber(L, "ndrv", AF_NDRV);
	table_setnumber(L, "isdn", AF_ISDN);
	table_setnumber(L, "e164", AF_E164);
	table_setnumber(L, "key", pseudo_AF_KEY);
	table_setnumber(L, "inet6", AF_INET6);
	table_setnumber(L, "natm", AF_NATM);
	table_setnumber(L, "hdrcmplt", pseudo_AF_HDRCMPLT);
	table_setnumber(L, "ieee80211", AF_IEEE80211);
	table_setnumber(L, "max", AF_MAX);

#ifdef __APPLE__
	table_setnumber(L, "system", AF_SYSTEM);
	table_setnumber(L, "netbios", AF_NETBIOS);
	table_setnumber(L, "ppp", AF_PPP);
#else
	table_setnumber(L, "atm", AF_ATM);
#endif

	lua_settable(L, -3);
} // set_pf


void set_sock(lua_State *L) {
	lua_pushliteral(L, "sock");
	lua_newtable(L);

	table_setnumber(L, "stream", SOCK_STREAM);
	table_setnumber(L, "dgram",  SOCK_DGRAM);
	table_setnumber(L, "raw", SOCK_RAW);
	table_setnumber(L, "rdm", SOCK_RDM);
	table_setnumber(L, "seqpacket", SOCK_SEQPACKET);

	lua_settable(L, -3);
} //set_sock


void set_so(lua_State *L) {
	lua_pushliteral(L, "so");
	lua_newtable(L);

	table_setnumber(L, "debug", SO_DEBUG);
	table_setnumber(L, "acceptconn", SO_ACCEPTCONN);
	table_setnumber(L, "reuseaddr", SO_REUSEADDR);
	table_setnumber(L, "keepalive", SO_KEEPALIVE);
	table_setnumber(L, "dontroute", SO_DONTROUTE);
	table_setnumber(L, "broadcast", SO_BROADCAST);
	table_setnumber(L, "useloopback", SO_USELOOPBACK);
	table_setnumber(L, "linger", SO_LINGER);
	table_setnumber(L, "oobinline", SO_OOBINLINE);
	table_setnumber(L, "reuseport", SO_REUSEPORT);
	table_setnumber(L, "timestamp", SO_TIMESTAMP);

#ifdef __APPLE__
	table_setnumber(L, "donttrunc", SO_DONTTRUNC);
	table_setnumber(L, "wantmore", SO_WANTMORE);
	table_setnumber(L, "wantoobflag", SO_WANTOOBFLAG);
#else
	table_setnumber(L, "acceptfilter", SO_ACCEPTFILTER);
#endif

	table_setnumber(L, "sndbuf", SO_SNDBUF);
	table_setnumber(L, "rcvbuf", SO_RCVBUF);
	table_setnumber(L, "sndlowat", SO_SNDLOWAT);
	table_setnumber(L, "rcvlowat", SO_RCVLOWAT);
	table_setnumber(L, "sndtimeo", SO_SNDTIMEO);
	table_setnumber(L, "rcvtimeo", SO_RCVTIMEO);
	table_setnumber(L, "error", SO_ERROR);
	table_setnumber(L, "type", SO_TYPE);

	lua_settable(L, -3);
} // set_so


const struct sockaddr *get_sockaddr(int domain, const char *host, unsigned int port, size_t *size) {
	if (domain == AF_INET) {
		struct sockaddr_in *addr = (struct sockaddr_in *)
		                           malloc(sizeof(struct sockaddr_in));
		addr->sin_family = AF_INET;

		if (strcmp(host, "0.0.0.0") * strcmp(host, "*") == 0)
			addr->sin_addr.s_addr = INADDR_ANY;
		else
			inet_aton(host, &addr->sin_addr);

		addr->sin_port = htons(port);
		if (size)
			*size = sizeof(*addr);

		return (const struct sockaddr *) addr;

	} else if (domain == AF_INET6) {
		struct sockaddr_in6 *addr = (struct sockaddr_in6 *)
		                            malloc(sizeof(struct sockaddr_in6));
		addr->sin6_family = AF_INET6;

		if (strcmp(host, "*") == 0)
			inet_pton(AF_INET6, "::", &addr->sin6_addr);
		else
			inet_pton(AF_INET6, host, &addr->sin6_addr);

		addr->sin6_port = htons(port);
		if (size)
			*size = sizeof(*addr);

		return (const struct sockaddr *) addr;
	}

	return 0;
}


void sss_pushsocket(lua_State *L, int sid, int domain, int type, int protocol) {
	socket_wrapper_t *envelop = (socket_wrapper_t *)
		lua_newuserdata(L, sizeof(socket_wrapper_t));
	envelop->sid = sid;
	envelop->domain = domain;
	envelop->type = type;
	envelop->protocol = protocol;
	luaL_getmetatable(L, SOCKET_CLASS);
	lua_setmetatable(L, -2);
} // sss_pushsocket


int sss_tosocket(lua_State *L, int index) {
	socket_wrapper_t *envelop = sss_tosocketwrapper(L, index);

	if (envelop)
		return envelop->sid;
	else
		return -1;
} // sss_tosocket


socket_wrapper_t *sss_tosocketwrapper(lua_State *L, int index) {
	return (socket_wrapper_t *) luaL_checkudata(L, index, SOCKET_CLASS);
} // sss_tosocketwrapper


int sss_socket(lua_State *L) {
	int domain = (int) luaL_checkint(L, 1);
	int type = (int) luaL_checkint(L, 2);
	int protocol = (lua_isnumber(L, 3)) ? (int) lua_tonumber(L, 3) : 0;
	int sid = socket(domain, type, protocol);

	if (sid == -1)
		return luaL_error(L, strerror(errno));

	sss_pushsocket(L, sid, domain, type, protocol);
	return 1;
}


int sss_connect(lua_State *L) {
	socket_wrapper_t *skt = sss_tosocketwrapper(L, 1);

	lua_getfield(L, 2, "host");
	const char *host = luaL_checkstring(L, -1);

	lua_getfield(L, 2, "port");
	unsigned int port = (unsigned int) luaL_checkint(L, -1);

	size_t size;
	const struct sockaddr *addr = get_sockaddr(skt->domain, host, port, &size);
	if (!addr)
		return luaL_error(L, strerror(errno));

	int status = connect(skt->sid, addr, size);
	free((void *) addr);

	if (status == -1)
		return luaL_error(L, strerror(errno));

	lua_pushnumber(L, status);
	return 1;
} // sss_connect


int sss_bind(lua_State *L) {
	socket_wrapper_t *skt = sss_tosocketwrapper(L, 1);

	lua_getfield(L, 2, "host");
	const char *host = luaL_checkstring(L, -1);

	lua_getfield(L, 2, "port");
	unsigned int port = (unsigned int) luaL_checkint(L, -1);

	size_t size;
	const struct sockaddr *addr = get_sockaddr(skt->domain, host, port, &size);

	if (!addr)
		return luaL_error(L, strerror(errno));

	int status = bind(skt->sid, addr, size);
	free((void *) addr);

	if (status == -1)
		return luaL_error(L, strerror(errno));

	lua_pushnumber(L, status);
	return 1;
} // sss_bind


int sss_listen(lua_State *L) {
	int sid = sss_tosocket(L, 1);
	if (sid == -1)
		return luaL_error(L, "socket expected");

	int backlog = (lua_isnumber(L, 2)) ? (int) lua_tonumber(L, 2) : 1;

	if (listen(sid, backlog) == -1)
		return luaL_error(L, strerror(errno));

	lua_pushboolean(L, 1);
	return 1;
} // sss_listen


int sss_accept(lua_State *L) {
	socket_wrapper_t *skt = sss_tosocketwrapper(L, 1);

	struct sockaddr address;
	socklen_t sin_size = sizeof(struct sockaddr);

	int new_sid = accept(skt->sid, &address, &sin_size);
	if (new_sid == -1)
		return luaL_error(L, strerror(errno));

	if (new_sid == skt->sid)
		return luaL_error(L, "same socket? how can it be?");

	const char *host;
	int port;

	if (skt->domain == AF_INET6) {
		struct sockaddr_in6 *address_p = (struct sockaddr_in6 *) &address;
		char *aux = (char *) malloc(INET6_ADDRSTRLEN);

		inet_ntop(AF_INET6, &address_p->sin6_addr, aux, INET6_ADDRSTRLEN);
		host = (const char *) aux;
		port = ntohs(address_p->sin6_port);

	} else {
		struct sockaddr_in *address_p = (struct sockaddr_in *) &address;
		host = (const char *) inet_ntoa(address_p->sin_addr);
		port = ntohs(address_p->sin_port);
	}

	sss_pushsocket(L, new_sid, skt->domain, skt->type, skt->protocol);
	lua_pushstring(L, host);
	lua_pushnumber(L, port);
	return 3;

} // sss_accept


int sss_recv(lua_State *L) {
	int sid = sss_tosocket(L, 1);
	int bsize = (lua_isnumber(L, 2)) ? (int) lua_tonumber(L, 2) : 1024;
	char buf[bsize + 1];
	int count = recv(sid, buf, bsize, 0);

	char *r = (char *) malloc(sizeof(char) * (count + 1));
	strcpy(r, buf);
	r[count] = 0;

	lua_pushstring(L, r);
	return 1;
} // sss_recv


int sss_receive(lua_State *L) {
	int sid = sss_tosocket(L, 1);

	char buf[4096];
	memset(buf, 0, 4096);
	char aux[2];
	int i = 0;

	while ((i < 4095) and (recv(sid, aux, 1, 0) > 0)) {
		buf[i++] = aux[0];
		if (aux[0] == '\n')
			break;
	}

	char *r = (char *) malloc(sizeof(char) * (strlen(buf) + 1));
	strcpy(r, buf);

	lua_pushstring(L, r);
	return 1;
} // sss_receive


int sss_send(lua_State *L) {
	int sid = sss_tosocket(L, 1);

	const char *s = lua_tostring(L, 2);
	int count = send(sid, s, strlen(s), 0);

	if (count == -1)
		return luaL_error(L, strerror(errno));

	lua_pushboolean(L, 1);
	return 1;
} // sss_send


int sss_sendto(lua_State *L) {
	socket_wrapper_t *skt = sss_tosocketwrapper(L, 1);
	const char *s = luaL_checkstring(L, 3);

	lua_getfield(L, 2, "host");
	const char *host = luaL_checkstring(L, -1);

	lua_getfield(L, 2, "port");
	unsigned int port = (unsigned int) luaL_checkint(L, -1);

	size_t size;
	const struct sockaddr *addr = get_sockaddr(skt->domain, host, port, &size);
	if (!addr)
		return luaL_error(L, strerror(errno));

	int count = sendto(skt->sid, s, strlen(s), 0, addr, size);
	free((void *) addr);

	if (count == -1)
		return luaL_error(L, strerror(errno));

	lua_pushboolean(L, 1);
	return 1;
} // sss_send


int sss_sendln(lua_State *L) {
	int sid = sss_tosocket(L, 1);
	const char *s = lua_tostring(L, 2);
	char buf[strlen(s) + 3];

	strcpy(buf, s);
	strcat(buf, "\r\n");
	int count = send(sid, buf, strlen(buf), 0);

	if (count == -1)
		return luaL_error(L, strerror(errno));

	lua_pushboolean(L, 1);
	return 1;
} // sss_sendln


int sss_close(lua_State *L) {
	socket_wrapper_t *skt = sss_tosocketwrapper(L, 1);

	if (skt->sid != -1) {
		close(skt->sid);
		skt->sid = -1;
		lua_pushboolean(L, 1);

	} else lua_pushboolean(L, 0);

	return 1;
} // sss_close


int sss_serialize(lua_State *L) {
	int sid = sss_tosocket(L, 1);
	char buf[1024];
	sprintf(buf, "<socket %d>", sid);
	char *s = (char *) malloc(sizeof(char) * (strlen(buf) + 1));
	strcpy(s, buf);

	lua_pushstring(L, s);
	return 1;
} // sss_serialize


int sss_setopt(lua_State *L) {
	int sid = sss_tosocket(L, 1);
	int optname = lua_tonumber(L, 2);
	int value = 1;
	if (lua_isnumber(L, 3))
		value = lua_tonumber(L, 3);

	int status = setsockopt(sid, SOL_SOCKET, optname, &value, sizeof(value));

	if (status == -1)
		return luaL_error(L, strerror(errno));

	lua_pushboolean(L, 1);
	return 1;

} // sss_setopt


int sss_getopt(lua_State *L) {
	int sid = sss_tosocket(L, 1);
	int optname = lua_tonumber(L, 2);
	int value = 0;
	socklen_t sizeof_int = sizeof(int);

	int status = getsockopt(sid, SOL_SOCKET, optname, (void *) &value, &sizeof_int);

	if (status == -1)
		return luaL_error(L, strerror(errno));

	lua_pushnumber(L, value);
	return 1;
} // sss_getopt


int sss_timeout(lua_State *L) {
	int sid = sss_tosocket(L, 1);

	if (lua_isnumber(L, 2)) {
		float timeout = lua_tonumber(L, 2);
		struct timeval value;
		value.tv_sec = (int) (timeout / 1000);
		value.tv_usec = ((int) (timeout * 1000)) % 1000000;

		int status = setsockopt(sid, SOL_SOCKET, SO_RCVTIMEO, &value, sizeof(value)) +
					setsockopt(sid, SOL_SOCKET, SO_SNDTIMEO, &value, sizeof(value));

		if (status == -1)
			return luaL_error(L, strerror(errno));
	}

	socklen_t sizeof_timeval = sizeof(struct timeval);
	struct timeval timeouts[2];

	// [0] = rcvtimeo, [1] = sndtimeo
	int r = getsockopt(sid, SOL_SOCKET, SO_RCVTIMEO, timeouts, &sizeof_timeval) +
			getsockopt(sid, SOL_SOCKET, SO_SNDTIMEO, (timeouts + 1), &sizeof_timeval);

	if (r != 0)
		return luaL_error(L, strerror(errno));

	lua_pushnumber(L, (timeouts[0].tv_sec * 1000.L) + (timeouts[0].tv_usec / 1000.0L));
	lua_pushnumber(L, (timeouts[1].tv_sec * 1000.L) + (timeouts[1].tv_usec / 1000.0L));
	return 2;
} // sss_timeout


int sss_get_descriptor(lua_State *L) {
	int sid = sss_tosocket(L, 1);
	lua_pushnumber(L, sid);
	return 1;
} // sss_get_descriptor


int sss_get_domain(lua_State *L) {
	socket_wrapper_t *skt = sss_tosocketwrapper(L, 1);
	lua_pushnumber(L, skt->domain);
	return 1;
} // sss_get_domain


int sss_get_type(lua_State *L) {
	socket_wrapper_t *skt = sss_tosocketwrapper(L, 1);
	lua_pushnumber(L, skt->type);
	return 1;
} // sss_get_type


int sss_get_protocol(lua_State *L) {
	socket_wrapper_t *skt = sss_tosocketwrapper(L, 1);
	lua_pushnumber(L, skt->protocol);
	return 1;
} // sss_get_protocol
