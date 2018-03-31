#include <utils/SoundManager.hpp>
#include <Windows.h>
#include <mmsystem.h>
#include <assert.h>

namespace px
{
	SoundManager::SoundManager()
	{
	}

	SoundManager::~SoundManager()
	{
		for (auto sound : m_sounds)
			free(sound.second);	
	}

	void SoundManager::LoadSoundFromFile(const Sound::ID & id, const std::string & filePath)
	{
		FILE * pFile;
		char* buffer;
		long lSize;
		size_t result;

		pFile = fopen(filePath.c_str(), "rb");
		if (pFile == NULL) { fputs("File error", stderr); exit(1); }

		//Obtain file size:
		fseek(pFile, 0, SEEK_END);
		lSize = ftell(pFile);
		rewind(pFile);

		//Allocate memory for the sound buffer
		buffer = (char*)malloc(sizeof(char) * lSize);
		if (buffer == NULL) { fputs("Memory error", stderr); exit(2); }

		//Copy the file into the buffer
		result = fread(buffer, 1, lSize, pFile);
		if (result != lSize) { fputs("Reading error", stderr); exit(3); }
		fclose(pFile);

		//Map the buffer to the ID
		auto inserted = m_sounds.insert(std::make_pair(id, std::move(buffer)));
		assert(inserted.second);
	}

	void SoundManager::Play(const Sound::ID & id, bool repeated)
	{
		auto found = m_sounds.find(id);

		if(repeated)
			sndPlaySound(found->second, SND_MEMORY | SND_ASYNC | SND_LOOP);
		else
			sndPlaySound(found->second, SND_MEMORY | SND_ASYNC);
	}
}