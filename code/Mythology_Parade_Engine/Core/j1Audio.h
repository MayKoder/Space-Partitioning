#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "SDL_mixer/include/SDL_mixer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_VOLUME 150
#define MAX_SFX 80

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME, int volume = 200);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(int channel, unsigned int fx, int repeat = 0);

	// Clean all fxs to change scene
	bool CleanFxs();
private:

	_Mix_Music*			music = NULL;
	int sfxList[MAX_SFX];
	std::list<Mix_Chunk*>	fx;
	int MusicVolume;
};

#endif // __j1AUDIO_H__