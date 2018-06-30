/*
	Time wasted
*/

#include <windows.h>
#include <detours.h>
#include <helper.h>
#include <netpacket.h>

class ILuaBase;

typedef void* (*CreateInterfaceFn)(const char*, int);
typedef int (*luaL_loadbufferx_proto)(ILuaBase*, const char*,
	size_t, const char*,
	const char*);

static List* Channels;
static IVEngineClient* IEngineClient;

luaL_loadbufferx_proto luaL_loadbufferx;

int luaL_loadbufferx_hook(ILuaBase* obj, const char* body, size_t size, const char* name, const char* mode) {
	const char* server = "local";
	bool orig = false;

	if(IEngineClient->IsConnected())
		server = Channels->GetChannel()->GetPacket()->GetIp();

	if(*name == '@') {
		++name;
		orig = true;
	}

	std::string root = string_format("%s\\", server);
	std::string comment = string_format("--[[\n%s\n--]]\n", name);

	std::experimental::filesystem::path root_path(root);
	std::experimental::filesystem::path file_path(string_format("%s%s", root.c_str(), name));
	
	if(file_path.has_extension()) {
		if(file_path.extension().string().compare(".lua") == 0) {
			safe_file_write(file_path, body, comment);
		}else {
			root_path += "Unsafe";
			safe_file_append(root_path, body, comment);
		}
	}else {
		root_path += "RunString";
		safe_file_append(root_path, body, comment);
	}

	if(orig)
		--name;

	return luaL_loadbufferx(obj, body, size, name, mode);
}

DWORD WINAPI Thread(LPVOID);

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD hReason, LPVOID lpFree) {
	if (hReason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, NULL, Thread, NULL, NULL, NULL);

	return TRUE;
}

DWORD WINAPI Thread(LPVOID) {
	void* ptr = utils::memory::FindSig(GetModuleHandleA("engine.dll"),
		"\xa1????\xff\x34\xb0\xe8????\x46\x83\xc4\x04\x3b\xf7\x7c\xeb\xa1????\x83\xe8\x01\x5e\xa3????\x75\x04",
		0x1fffff, 0);

	if(ptr) {
		CreateInterfaceFn GetIVEngineClient = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface");
		IEngineClient = (IVEngineClient*)GetIVEngineClient("VEngineClient013", 0);
		luaL_loadbufferx = (luaL_loadbufferx_proto)GetProcAddress(GetModuleHandleA("lua_shared.dll"), "luaL_loadbufferx");
		luaL_loadbufferx = (luaL_loadbufferx_proto)DetourFunction((PBYTE)luaL_loadbufferx, (PBYTE)&luaL_loadbufferx_hook);

		Channels = *(List**)((unsigned int)ptr + 1);
	}else
		MessageBoxA(NULL, "Something went wrong :( Tell me gmod version pls?", "Ow Ow Ow", 0);

	return 0;
}