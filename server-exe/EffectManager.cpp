﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "EffectManager.h"


EffectManager::~EffectManager()
{
  clearEffects();
}

bool EffectManager::storeEffect(const RETCDeviceType& deviceType, int effectType, RZEFFECTID* pEffectID, efsize_t effectSize, const char effectData[]) {
	internalEffectData effData;
	effData.type = effectType;
	effData.deviceType = deviceType;
	effData.data = new char[static_cast<size_t>(effectSize)];
	std::move(effectData, effectData + static_cast<size_t>(effectSize), effData.data);

	RZEFFECTID newEffectID;
	if (!createUniqueEffectID(&newEffectID)) {
		return false;
	}

	*pEffectID = newEffectID;

	m_effectMap.insert(std::make_pair(newEffectID, effData));

	return true;
}

bool EffectManager::deleteEffect(const RZEFFECTID& pEffectID) {
	auto it = m_effectMap.find(pEffectID);
	if (it == m_effectMap.end()) {
		return false;
	}

	delete[] it->second.data;
	m_effectMap.erase(it);

	return true;
}

void EffectManager::clearEffects()
{
  std::for_each(m_effectMap.begin(), m_effectMap.end(), [](const auto &effect) -> void
  {
    delete [] effect.second.data;
  });

  m_effectMap.clear();
}

const internalEffectData* EffectManager::getEffect(const RZEFFECTID& pEffectID) {
	const auto& entry = m_effectMap.find(pEffectID);

	if (entry == m_effectMap.end()) {
		return nullptr;
	}

	return &entry->second;
}
