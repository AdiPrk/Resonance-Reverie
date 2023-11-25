#pragma once

#include <PCH/pch.h>
class SoundSource;

struct Channel {
	FMOD::Channel* channel;
	glm::vec2 position;
	float volumeMultiplier;
	bool isSpatial;
	float audioRange;
	bool initSpatial = false;
};

class AudioManager
{
public:
	/*! \brief              Initializes the Audio Manager
	 */
	AudioManager();

	/*! \brief              Updates the Audio Manager
	 */
	void Update();

	/*! \brief              Shuts down the Audio Manager.
	 */
	~AudioManager();

	/*! \brief              Checks if something went wrong and exits the program if it did
	 */
	void LogError(FMOD_RESULT result);

	/*! \brief              Loads a sound into the audio manager
	 *  \param soundName:   The name of the sound file.
	 *  \returns            The distance between point and Rectangle2D.
	 */
	void LoadSound(const std::string& soundName, bool isStream = false);

	/*! \brief              Unloads a sound from the audio manager
	 *  \param soundName:   The name of the sound file.
	 */
	void UnloadSound(const std::string& soundName);

	/*! \brief                   Plays a sound. If the sound doesn't exist, it gets created with default settings and then played.
	 *  \param soundName:        The name of the sound file.
	 *  \param isLooping:        If the sound loops or not.
	 *  \param volumeMultiplier: What the volume of the sound should be multiplied by.
	 *  \param isSpacial:        Whether the sound has a position in space or not.
	 *  \param soundPos:         The position of the sound in the world.
	 *  \param soundRange:       The distance over which the sound can be heard.
	 *  \returns                 The channel that the sound was played in.
	 */
	int SoundPlay(const char* source, bool isLooping, float pitchMod = 1.0f, float volumeMultiplier = 1.f, bool isSpatial = false, const glm::vec2& soundPos = { 0, 0 }, float soundRange = 500.f);

	/*! \brief              Stops the channel from playing.
	 *  \param channelId:   The channel ID.
	 */
	void StopChannel(int channelId);

	/*! \brief              Stops all channels.
	 */
	void StopAllChannels();

	/*! \brief              Filters out all channels that have stopped. Doesn't need to be done manually.
	 */
	void FilterStoppedChannels();

	/*! \brief              Updates the volumes of the channels based on position. Doesn't need to be done manually.
	 */
	void UpdateSpatialChannels();

	/*! \brief              Sets the position of a channel in 2d
	 *  \param channelId:   The channel ID.
	 *  \param vPosition:   The position to set the channel to.
	 */
	void SetChannelPosition(int channelId, const glm::vec2& vPosition);

	/*! \brief              Sets the volume of a channel.
	 *  \param channelId:   The channel ID.
	 *  \param volumeMult:  The new multiplier for the sound volume.
	 */
	void SetChannelVolumeMult(int channelId, float volumeMult);

	/*! \brief              Sets the pitch of a channel.
	 *  \param channelId:   The channel ID.
	 *  \param pitch:		The new pitch for the sound
	 */
	void SetChannelPitch(int channelId, float pitch);

	/*! \brief              Sets whether the channel is looping or not.
	 *  \param channelId:   The channel ID.
	 *  \param isLooping:   If the channel should loop or not.
	 */
	void SetChannelLooping(int channelId, bool isLooping);

	/*! \brief              Sets whether the channel is spatial or not.
	 *  \param channelId:   The channel ID.
	 *  \param isSpatial:   If the channel is spatial or not.
	 */
	void SetChannelSpatial(int channelId, bool isSpatial);

	/*! \brief              Sets the range of the sound.
	 *  \param channelId:   The channel ID.
	 *  \param soundRange:  The range of the sound.
	 */
	void SetChannelRange(int channelId, float soundRange);

	/*! \brief              Sets if the channel is paused or not.
	 *  \param channelId:   The channel ID.
	 *  \param soundRange:  If the channel is paused or not.
	 */
	void SetChannelPaused(int channelId, bool isPaused);

	/*! \brief              Checks if a channel is playing or not.
	 *  \param channelId:   The channel ID.
	 *  \returns            Whether the channel is playing or not.
	 */
	bool IsPlaying(int channelId) const;

	/*! \brief              Sets the position of the listener
	 *  \param vPosition:   The position to set the channel to.
	 */
	void SetListenerLocation(const glm::vec2& vPosition);


	inline FMOD::System* GetAudioSystem() { return system; }

private:
	FMOD::System* system = NULL;

	typedef std::map<std::string, FMOD::Sound*> SoundMap;
	typedef std::map<int, Channel> ChannelMap;
	SoundMap Sounds;
	ChannelMap Channels;

	glm::vec2 origin;

	int nextChannelID;
};


struct Sound {
	// Source of sound instance
	const char* source = nullptr;

	// Name of sound component.	
	std::string name = "";

	// Default settings
	bool isStream = false;  // Should the sound be streamed? Good for longer sound files.
	bool isLooping = false; // Should the sound loop after being played?

	float volumeMultiplier = 1.f; // How loud the volume of the sound is

	// Spatial settings
	bool isSpatial = false;       // If the sound has a position in space
	glm::vec2 soundPos = { 0, 0 }; // The position of the sound in space
	float soundRange = 500.f;     // How far from its position the sound can be heard from 

	float pitch = 1.0f;

	// Channel
	int channel = 0;
	bool paused = true;
	bool playOnInit = false;

	void SoundPlay(AudioManager& audioManager)
	{
		channel = audioManager.SoundPlay(name.c_str(), isLooping, pitch, volumeMultiplier, isSpatial, soundPos, soundRange);
		audioManager.SetChannelLooping(channel, isLooping);
		audioManager.SetChannelVolumeMult(channel, volumeMultiplier);
		audioManager.SetChannelSpatial(channel, isSpatial);
		audioManager.SetChannelRange(channel, soundRange);
		audioManager.SetChannelPitch(channel, pitch);
		paused = false;
	}

	void SoundPause(bool isPaused, AudioManager& audioManager)
	{
		paused = isPaused;
		audioManager.SetChannelPaused(channel, isPaused);
	}
};
