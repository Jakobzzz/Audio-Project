#pragma once
#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <map>
#include <string>
#include <vector>
#include <math.h>
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace px
{
	struct Implementation 
	{
		Implementation();
		~Implementation();
		void Update();

		FMOD::Studio::System* mpStudioSystem;
		FMOD::System* mpSystem;
		int mnNextChannelId;
		std::map<std::string, FMOD::Studio::Bank*> mBanks;
		std::map<std::string, FMOD::Studio::EventInstance*> mEvents;
		std::map<std::string, FMOD::Sound*> mSounds;
		std::map<int, FMOD::Channel*> mChannels;
	};

	class SoundManager
	{
	public:
		static void Init();
		static void Update();
		static int ErrorCheck(FMOD_RESULT result);

	public:
		void LoadBank(const std::string & strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
		void LoadEvent(const std::string & strEventName);
		void LoadSound(const std::string & strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
		void UnloadSound(const std::string & strSoundName);

	public:
		int Play(const std::string & strSoundName, const Vector3 & vPos = Vector3(0.f), float fVolumedB = 0.f);
		void PlayEvent(const std::string & strEventName);
		//void StopChannel(int nChannelId);
		void StopEvent(const std::string & strEventName, bool bImmediate = false);

	public:
		//void Set3dListenerAndOrientation(const Vector3& vPosition, const Vector3& vLook, const Vector3& vUp);
		void SetEventParameter(const std::string & strEventName, const std::string & strParameterName, float fValue);
		//void StopAllChannels();
		void SetChannel3dPosition(int nChannelId, const Vector3 & vPosition);
		void SetChannelVolume(int nChannelId, float fVolumedB);
		
	public:
		void GetEventParameter(const std::string & strEventName, const std::string & strEventParameter, float* parameter);
		//bool IsPlaying(int nChannelId) const;
		bool IsEventPlaying(const std::string & strEventName) const;

	public:
		float dbToVolume(float dB);
		float VolumeTodB(float volume);
		FMOD_VECTOR VectorToFmod(const Vector3& vPosition);	
	};
}
