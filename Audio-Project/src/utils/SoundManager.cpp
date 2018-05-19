#include <utils/SoundManager.hpp>
#include <iostream>
#include <assert.h>

namespace px
{
	SoundManager::SoundManager() : m_studioSystem(NULL), m_system(NULL)
	{
		//Init FMOD sound engine
		ErrorCheck(FMOD::Studio::System::create(&m_studioSystem));
		ErrorCheck(m_studioSystem->initialize(32, NULL, NULL, NULL));
		ErrorCheck(m_studioSystem->getLowLevelSystem(&m_system));
	}

	SoundManager::~SoundManager()
	{
		//Release sounds
		for (auto & sound : m_sounds)
			sound.second.sound->release();

		ErrorCheck(m_studioSystem->unloadAll());
		ErrorCheck(m_studioSystem->release());
	}

	void SoundManager::Update() 
	{
		//Check if a channel has stopped playing and destroy it
		std::vector<Channels::iterator> pStoppedChannels;

		for (auto it = m_channels.begin(), itEnd = m_channels.end(); it != itEnd; ++it)
		{
			bool bIsPlaying = false;
			it->second->isPlaying(&bIsPlaying);
			if (!bIsPlaying)
				pStoppedChannels.push_back(it);
		}

		for (auto & it : pStoppedChannels)
			m_channels.erase(it);

		ErrorCheck(m_system->update());
		ErrorCheck(m_studioSystem->update());
	}

	void SoundManager::LoadSound(const Sounds::ID & id, const std::string & strSoundName, bool bLooping, bool bStream)
	{
		SoundInfo info;
		info.filePath = strSoundName;

		//Load 2D sounds
		FMOD_MODE eMode = FMOD_DEFAULT;
		eMode |= FMOD_2D;
		eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
		FMOD::Sound* pSound = nullptr;
		ErrorCheck(m_system->createSound(info.filePath.c_str(), eMode, nullptr, &pSound));
		info.sound = std::move(pSound);

		auto inserted = m_sounds.insert(std::make_pair(id, info));
		assert(inserted.second);
	}

	void SoundManager::UnloadSound(const Sounds::ID & id)
	{
		auto tFoundIt = m_sounds.find(id);
		if (tFoundIt == m_sounds.end())
			return;

		ErrorCheck(tFoundIt->second.sound->release());
		m_sounds.erase(tFoundIt);
	}

	int SoundManager::Play(const Sounds::ID & id, const Vector3 & vPosition, float fVolumedB)
	{
		int nChannelId = m_nextChannelId++;
		auto tFoundIt = m_sounds.find(id);

		FMOD::Channel* pChannel = nullptr;
		ErrorCheck(m_system->playSound(tFoundIt->second.sound, nullptr, true, &pChannel));
		if (pChannel)
		{
			ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
			ErrorCheck(pChannel->setPaused(false));
			m_channels[nChannelId] = pChannel;
		}

		return nChannelId;
	}

	void SoundManager::SetChannelVolume(int nChannelId, float fVolumedB)
	{
		auto tFoundIt = m_channels.find(nChannelId);
		if (tFoundIt == m_channels.end())
			return;

		ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
	}

	FMOD_VECTOR SoundManager::VectorToFmod(const Vector3& vPosition)
	{
		FMOD_VECTOR fVec;
		fVec.x = vPosition.x;
		fVec.y = vPosition.y;
		fVec.z = vPosition.z;
		return fVec;
	}

	float SoundManager::InverseSqLaw(const Vector3 & noisePos, const Vector3 & listenerPos, const float & startVolume)
	{
		//Apply inverse square law for attenuation (3D sound)
		return VolumeTodB(startVolume) - VolumeTodB(Vector3Magnitude(noisePos - listenerPos));
	}

	float SoundManager::dbToVolume(float dB)
	{
		return powf(10.0f, 0.05f * dB);
	}

	float SoundManager::VolumeTodB(float volume)
	{
		return 20.0f * log10f(volume);
	}

	float SoundManager::Vector3Magnitude(const Vector3 & v)
	{
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	int SoundManager::ErrorCheck(FMOD_RESULT result)
	{
		if (result != FMOD_OK)
		{
			std::cout << "FMOD ERROR " << result << std::endl;
			return 1;
		}
		return 0;
	}
}