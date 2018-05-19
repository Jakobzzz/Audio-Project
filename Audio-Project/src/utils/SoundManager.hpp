/*
	Sound manager class based on the FMOD-library
*/

#pragma once
#include <fmod_studio.hpp>
#include <fmod.hpp>
#include <map>
#include <string>
#include <vector>
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace px
{
	namespace Sounds
	{
		enum ID
		{
			Nature,
			Footsteps,
			Churchbell,
			Gun,
			Slot_machine
		};
	}

	class SoundManager
	{
	public:
		SoundManager();
		~SoundManager();

	private:
		struct SoundInfo
		{
			FMOD::Sound* sound;
			std::string filePath;
		};

	public:
		void Update();
		void LoadSound(const Sounds::ID & id, const std::string & strSoundName, bool bLooping = false, bool bStream = false);
		void UnloadSound(const Sounds::ID & id);

	public:
		int Play(const Sounds::ID & id, const Vector3 & vPos = Vector3(0.f), float fVolumedB = 0.f);

	public:
		void SetChannelVolume(int nChannelId, float fVolumedB);

	public:
		float dbToVolume(float dB);
		float VolumeTodB(float volume);
		FMOD_VECTOR VectorToFmod(const Vector3& vPosition);	
		float InverseSqLaw(const Vector3 & noisePos, const Vector3 & listenerPos, const float & startVolume = 1.f);

	private:
		int ErrorCheck(FMOD_RESULT result);
		float Vector3Magnitude(const Vector3 & v);

	private:
		FMOD::Studio::System* m_studioSystem;
		FMOD::System* m_system;
		int m_nextChannelId;
		std::map<Sounds::ID, SoundInfo> m_sounds;
		using Channels = std::map<int, FMOD::Channel*>;
		Channels m_channels;
	};
}
