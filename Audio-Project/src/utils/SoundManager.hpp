#pragma once
#include <map>
#include <string>

//Note: need to use waveout functions for multiple sounds or add a sound library for that

namespace px
{
	namespace Sound
	{
		enum ID
		{
			Gun,
			Birds
		};
	}

	class SoundManager
	{
	public:
		SoundManager();
		~SoundManager();

	public:
		void LoadSoundFromFile(const Sound::ID & id, const std::string & filePath);

	public:
		void Play(const Sound::ID & id, bool repeated = false);

	private:
		std::map<Sound::ID, char*> m_sounds;
	};
}
