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

		ErrorCheck(m_studioSystem->update());
	}

	void SoundManager::LoadSound(const Sound::ID & id, const std::string & strSoundName, bool b3d, bool bLooping, bool bStream)
	{
		SoundInfo info;
		info.filePath = strSoundName;

		//Load 2D and 3D sounds
		FMOD_MODE eMode = FMOD_DEFAULT;
		eMode |= b3d ? FMOD_3D : FMOD_2D;
		eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
		FMOD::Sound* pSound = nullptr;
		ErrorCheck(m_system->createSound(info.filePath.c_str(), eMode, nullptr, &pSound));
		info.sound = std::move(pSound);

		auto inserted = m_sounds.insert(std::make_pair(id, info));
		assert(inserted.second);
	}

	void SoundManager::UnloadSound(const Sound::ID & id)
	{
		auto tFoundIt = m_sounds.find(id);
		if (tFoundIt == m_sounds.end())
			return;

		ErrorCheck(tFoundIt->second.sound->release());
		m_sounds.erase(tFoundIt);
	}

	int SoundManager::Play(const Sound::ID & id, const Vector3 & vPosition, float fVolumedB)
	{
		int nChannelId = m_nextChannelId++;
		auto tFoundIt = m_sounds.find(id);

		FMOD::Channel* pChannel = nullptr;
		ErrorCheck(m_system->playSound(tFoundIt->second.sound, nullptr, true, &pChannel));
		if (pChannel)
		{
			FMOD_MODE currMode;
			tFoundIt->second.sound->getMode(&currMode);
			if (currMode & FMOD_3D) 
			{
				FMOD_VECTOR position = VectorToFmod(vPosition);
				ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
			}
			ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
			ErrorCheck(pChannel->setPaused(false));
			m_channels[nChannelId] = pChannel;
		}

		return nChannelId;
	}

	void SoundManager::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
	{
		auto tFoundIt = m_banks.find(strBankName);
		if (tFoundIt != m_banks.end())
			return;

		FMOD::Studio::Bank* pBank;
		ErrorCheck(m_studioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
		if (pBank)
			m_banks[strBankName] = pBank;
	}

	void SoundManager::LoadEvent(const std::string& strEventName) 
	{
		auto tFoundit = m_events.find(strEventName);
		if (tFoundit != m_events.end())
			return;

		FMOD::Studio::EventDescription* pEventDescription = NULL;
		ErrorCheck(m_studioSystem->getEvent(strEventName.c_str(), &pEventDescription));
		if (pEventDescription) 
		{
			FMOD::Studio::EventInstance* pEventInstance = NULL;
			ErrorCheck(pEventDescription->createInstance(&pEventInstance));
			if (pEventInstance)
				m_events[strEventName] = pEventInstance;
		}
	}

	void SoundManager::PlayEvent(const std::string &strEventName)
	{
		auto tFoundit = m_events.find(strEventName);
		if (tFoundit == m_events.end()) 
		{
			LoadEvent(strEventName);
			tFoundit = m_events.find(strEventName);
			if (tFoundit == m_events.end())
				return;
		}

		tFoundit->second->start();
	}

	void SoundManager::StopEvent(const std::string &strEventName, bool bImmediate)
	{
		auto tFoundIt = m_events.find(strEventName);
		if (tFoundIt == m_events.end())
			return;

		FMOD_STUDIO_STOP_MODE eMode;
		eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		ErrorCheck(tFoundIt->second->stop(eMode));
	}

	bool SoundManager::IsEventPlaying(const std::string &strEventName) const
	{
		auto tFoundIt = m_events.find(strEventName);
		if (tFoundIt == m_events.end())
			return false;

		FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
		if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
			return true;

		return false;
	}

	void SoundManager::SetChannel3dPosition(int nChannelId, const Vector3& vPosition)
	{
		auto tFoundIt = m_channels.find(nChannelId);
		if (tFoundIt == m_channels.end())
			return;

		FMOD_VECTOR position = VectorToFmod(vPosition);
		ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
	}

	void SoundManager::SetChannelVolume(int nChannelId, float fVolumedB)
	{
		auto tFoundIt = m_channels.find(nChannelId);
		if (tFoundIt == m_channels.end())
			return;

		ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
	}

	void SoundManager::SetEventParameter(const std::string &strEventName, const std::string &strParameterName, float fValue)
	{
		auto tFoundIt = m_events.find(strEventName);
		if (tFoundIt == m_events.end())
			return;

		FMOD::Studio::ParameterInstance* pParameter = NULL;
		ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
		ErrorCheck(pParameter->setValue(fValue));
	}


	void SoundManager::GetEventParameter(const std::string &strEventName, const std::string &strParameterName, float* parameter)
	{
		auto tFoundIt = m_events.find(strEventName);
		if (tFoundIt == m_events.end())
			return;
		FMOD::Studio::ParameterInstance* pParameter = NULL;

		ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
		ErrorCheck(pParameter->getValue(parameter));
	}

	FMOD_VECTOR SoundManager::VectorToFmod(const Vector3& vPosition)
	{
		FMOD_VECTOR fVec;
		fVec.x = vPosition.x;
		fVec.y = vPosition.y;
		fVec.z = vPosition.z;
		return fVec;
	}

	float SoundManager::dbToVolume(float dB)
	{
		return powf(10.0f, 0.05f * dB);
	}

	float SoundManager::VolumeTodB(float volume)
	{
		return 20.0f * log10f(volume);
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