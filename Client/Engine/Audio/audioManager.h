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

	/*! \brief              Sets how much the channel should loop for.
	 *  \param channelId:   The channel ID.
	 *  \param isLooping:   If the channel should loop or not.
	 */
	void SetChannelLoopCount(int channelId, bool loopCount);

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

// The ideal interface:
/*

ResourceManager::GetSound("jump"); Returns a `Sound` Object

Functions from the Sound class

Sound.Play();
Sound.Pause();

Sound.LoopFor(int numLoops);
Sound.StopLooping();

Sound.SetVolume(float volume);

Sound.SetSpatialInfo(bool isSpatial, const glm::vec2& position, float range);
Sound.SetSpatial(bool isSpatial);
Sound.SetPosition(const glm::vec2& position);
Sound.SetRange(float range);

Sound.AddToGroup(const std::string& groupName);

Group functions:

*/


struct Sound {
	Sound(AudioManager& am) : audioManager(am) {};
	AudioManager& audioManager;

	int m_Channel = 0;
	int m_NumLoops = -1;
	float m_Volume = 1.0f;
	bool m_Paused = true;
	bool m_Spatial = false;
	glm::vec2 m_Position = { 0, 0 };
	float m_Range = 500.0f;
	float m_Pitch = 1.0f;

	void LoopFor(int numLoops) {
		m_NumLoops = numLoops;
		audioManager.SetChannelLoopCount(m_Channel, numLoops);
	}
	void StopLooping() {
		m_NumLoops = -1;
		audioManager.SetChannelLooping(m_Channel, false);
	}

	void SetVolume(float volume) {
		m_Volume = volume;
		audioManager.SetChannelVolumeMult(m_Channel, volume);
	}

	void SetSpatialinfo(bool isSpatial, const glm::vec2& position, float range) {
		m_Spatial = isSpatial;
		m_Position = position;
		m_Range = range;
		audioManager.SetChannelSpatial(m_Channel, isSpatial);
		audioManager.SetChannelPosition(m_Channel, position);
		audioManager.SetChannelRange(m_Channel, range);
	}

	void SetSpatial(bool isSpatial) {
		m_Spatial = isSpatial;
		audioManager.SetChannelSpatial(m_Channel, isSpatial);
	}

	void SetPosition(const glm::vec2& position) {
		m_Position = position;
		audioManager.SetChannelPosition(m_Channel, position);
	}

	void SetRange(float range) {
		m_Range = range;
		audioManager.SetChannelRange(m_Channel, range);
	}

	void SetPitch(float pitch) {
		m_Pitch = pitch;
		audioManager.SetChannelPitch(m_Channel, pitch);
	}

	void Play(const std::string& source, bool isLooping = false, float pitch = 1.0f, float volumeMultiplier = 1.0f, bool isSpatial = false, const glm::vec2& soundPos = { 0, 0 }, float soundRange = 500.0f)
	{
		m_Channel = audioManager.SoundPlay(source.c_str(), isLooping, pitch, volumeMultiplier, isSpatial, soundPos, soundRange);
		m_NumLoops = isLooping ? 0 : -1;
		m_Pitch = pitch;
		m_Volume = volumeMultiplier;
		m_Spatial = isSpatial;
		m_Position = soundPos;
		m_Range = soundRange;
		m_Paused = false;
	}

	void Pause(bool isPaused)
	{
		m_Paused = isPaused;
		audioManager.SetChannelPaused(m_Channel, isPaused);
	}
};