// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "SDKLoader.h"

SDKLoader::SDKLoader() {

}

SDKLoader::~SDKLoader() {
	destroy();
}

void SDKLoader::destroy() {
	for (auto inst : m_dllInstances) {
		FreeLibrary(inst.second);
	}

	m_dllInstances.clear();
}

bool SDKLoader::load(HINSTANCE& inst, const std::string& dllName, functionList& fList) {
	// Force user to call reload.
	if (isLoaded(dllName)) {
		LOG_D("DLL already loaded you should call reload!");
		return false;
	}

	inst = LoadLibraryA(dllName.c_str());

	if (!inst) {
		LOG_E("LoadLibraryA({0}) failed with code {1}", dllName, GetLastError());
		return false;
	}

	for (auto& func : fList) {
		const char * funcPtrName = func.first.c_str();
		void* fptr = GetProcAddress(inst, funcPtrName);
		if (!fptr) {
			LOG_E("Could not find required method {0}, please check {1}.", funcPtrName, dllName);
			FreeLibrary(inst);
			return false;
		}

		func.second = fptr;
	}

	m_dllInstances.insert(make_pair(dllName, inst));
	return true;
}

bool SDKLoader::reload(HINSTANCE& inst, const std::string& dllName, functionList& fList) {
	auto res = unload(dllName);
	if (!res) {
		res = unload(inst);
	}

	if (!res) {
		return false;
	}

	return load(inst, dllName, fList);
}

bool SDKLoader::isLoaded(const std::string& name) {
	return m_dllInstances.find(name) != m_dllInstances.end();
}

bool SDKLoader::unload(const std::string& name) {
	auto elem = m_dllInstances.find(name);

	if (elem == m_dllInstances.end()) {
		return false;
	}

	FreeLibrary(elem->second);
	m_dllInstances.erase(elem);

	return true;
}

bool SDKLoader::unload(HINSTANCE& inst) {
	for (auto& func : m_dllInstances) {
		if (func.second != inst) {
			continue;
		}

		FreeLibrary(func.second);
		m_dllInstances.erase(func.first);
		return true;
	}

	return false;
}
