#include <utils/SoundManager.hpp>
#include <iostream>
#include <memory>

namespace px
{
	//Struct implementation
	Implementation::Implementation() : mpStudioSystem(NULL), mpSystem(NULL)
	{
		SoundManager::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
		SoundManager::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));
		SoundManager::ErrorCheck(mpStudioSystem->getLowLevelSystem(&mpSystem));
	}

	Implementation::~Implementation() 
	{
		SoundManager::ErrorCheck(mpStudioSystem->unloadAll());
		SoundManager::ErrorCheck(mpStudioSystem->release());
	}

	void Implementation::Update() 
	{
		//Check if a channel has stopped playing and destroy it
		std::vector<std::map<int, FMOD::Channel*>::iterator> pStoppedChannels;

		for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
		{
			bool bIsPlaying = false;
			it->second->isPlaying(&bIsPlaying);
			if (!bIsPlaying)
				pStoppedChannels.push_back(it);
		}

		for (auto & it : pStoppedChannels)
			mChannels.erase(it);

		SoundManager::ErrorCheck(mpStudioSystem->update());
	}

	std::unique_ptr<Implementation> sgpImplementation;

	//Class implementation
	void SoundManager::Init() 
	{
		sgpImplementation = std::make_unique<Implementation>();
	}

	void SoundManager::Update() 
	{
		sgpImplementation->Update();
	}

	void SoundManager::LoadSound(const std::string & strSoundName, bool b3d, bool bLooping, bool bStream)
	{
		//Load 2D and 3D sounds
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt != sgpImplementation->mSounds.end())
			return;

		FMOD_MODE eMode = FMOD_DEFAULT;
		eMode |= b3d ? FMOD_3D : FMOD_2D;
		eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
		FMOD::Sound* pSound = nullptr;
		SoundManager::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));

		if (pSound)
			sgpImplementation->mSounds[strSoundName] = pSound;
	}

	void SoundManager::UnloadSound(const std::string & strSoundName)
	{
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt == sgpImplementation->mSounds.end())
			return;
		SoundManager::ErrorCheck(tFoundIt->second->release());
		sgpImplementation->mSounds.erase(tFoundIt);
	}

	int SoundManager::Play(const std::string & strSoundName, const Vector3 & vPosition, float fVolumedB)
	{
		int nChannelId = sgpImplementation->mnNextChannelId++;
		auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt == sgpImplementation->mSounds.end())
		{
			LoadSound(strSoundName);
			tFoundIt = sgpImplementation->mSounds.find(strSoundName);
			if (tFoundIt == sgpImplementation->mSounds.end())
				return nChannelId;
		}

		FMOD::Channel* pChannel = nullptr;
		SoundManager::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
		if (pChannel)
		{
			FMOD_MODE currMode;
			tFoundIt->second->getMode(&currMode);
			if (currMode & FMOD_3D) 
			{
				FMOD_VECTOR position = VectorToFmod(vPosition);
				SoundManager::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
			}
			SoundManager::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
			SoundManager::ErrorCheck(pChannel->setPaused(false));
			sgpImplementation->mChannels[nChannelId] = pChannel;
		}

		return nChannelId;
	}

	void SoundManager::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
	{
		auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
		if (tFoundIt != sgpImplementation->mBanks.end())
			return;

		FMOD::Studio::Bank* pBank;
		SoundManager::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
		if (pBank)
			sgpImplementation->mBanks[strBankName] = pBank;
	}

	void SoundManager::LoadEvent(const std::string& strEventName) 
	{
		auto tFoundit = sgpImplementation->mEvents.find(strEventName);
		if (tFoundit != sgpImplementation->mEvents.end())
			return;

		FMOD::Studio::EventDescription* pEventDescription = NULL;
		SoundManager::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
		if (pEventDescription) 
		{
			FMOD::Studio::EventInstance* pEventInstance = NULL;
			SoundManager::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
			if (pEventInstance)
				sgpImplementation->mEvents[strEventName] = pEventInstance;
		}
	}

	void SoundManager::PlayEvent(const std::string &strEventName)
	{
		auto tFoundit = sgpImplementation->mEvents.find(strEventName);
		if (tFoundit == sgpImplementation->mEvents.end()) 
		{
			LoadEvent(strEventName);
			tFoundit = sgpImplementation->mEvents.find(strEventName);
			if (tFoundit == sgpImplementation->mEvents.end())
				return;
		}

		tFoundit->second->start();
	}

	void SoundManager::StopEvent(const std::string &strEventName, bool bImmediate)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;

		FMOD_STUDIO_STOP_MODE eMode;
		eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		SoundManager::ErrorCheck(tFoundIt->second->stop(eMode));
	}

	bool SoundManager::IsEventPlaying(const std::string &strEventName) const
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return false;

		FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
		if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
			return true;

		return false;
	}

	void SoundManager::SetChannel3dPosition(int nChannelId, const Vector3& vPosition)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;

		FMOD_VECTOR position = VectorToFmod(vPosition);
		SoundManager::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
	}

	void SoundManager::SetChannelVolume(int nChannelId, float fVolumedB)
	{
		auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
		if (tFoundIt == sgpImplementation->mChannels.end())
			return;

		SoundManager::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
	}

	void SoundManager::SetEventParameter(const std::string &strEventName, const std::string &strParameterName, float fValue)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;

		FMOD::Studio::ParameterInstance* pParameter = NULL;
		SoundManager::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
		SoundManager::ErrorCheck(pParameter->setValue(fValue));
	}


	void SoundManager::GetEventParameter(const std::string &strEventName, const std::string &strParameterName, float* parameter)
	{
		auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
		if (tFoundIt == sgpImplementation->mEvents.end())
			return;
		FMOD::Studio::ParameterInstance* pParameter = NULL;

		SoundManager::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
		SoundManager::ErrorCheck(pParameter->getValue(parameter));
	}

	FMOD_VECTOR SoundManager::VectorToFmod(const Vector3& vPosition)
	{
		FMOD_VECTOR fVec;
		fVec.x = vPosition.x;
		fVec.y = vPosition.y;
		fVec.z = vPosition.z;
		return fVec;
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

	float SoundManager::dbToVolume(float dB)
	{
		return powf(10.0f, 0.05f * dB);
	}

	float SoundManager::VolumeTodB(float volume)
	{
		return 20.0f * log10f(volume);
	}
}