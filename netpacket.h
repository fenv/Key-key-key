#include <stdio.h>
#include <sigscan.h>

class INetChannel;

enum netadrtype_t {
	NA_NULL = 0,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
};

struct netpacket_t {
	netadrtype_t type;
	unsigned char ip[4];
	unsigned int port;

	const char* GetIp() {
		static char buff[16];
		sprintf_s(buff, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
		return (const char*)&buff;
	}
};

class INetChannel {
public:
	__declspec(noinline) const char* __thiscall GetAddress() {
		return getvfunc<const char*(*)(void)>(this, 0x4)();
	}

	__declspec(noinline) netpacket_t* __thiscall GetPacket() {
		return (netpacket_t*)((unsigned int)this + 0x98);
	}
};

class IVEngineClient {
public:
	bool IsConnected() {
		return getvfunc<bool(*)(void)>(this, 0x6c)();
	}

	INetChannel* GetNetChannel() {
		return getvfunc<INetChannel*(*)(void)>(this, 0x120)();
	}
};

class List {
public:
	__declspec(noinline) INetChannel* __thiscall GetChannel() {
		return ***(INetChannel****)this;
	}
};