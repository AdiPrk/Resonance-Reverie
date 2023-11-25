#include <PCH/pch.h>

#include "AudioManager.h"
#include <Source/ResourceManager/resourceManager.h>

AudioManager::AudioManager()
{
    FMOD_RESULT result;

    // Create the main system object.
    result = FMOD::System_Create(&system);
    LogError(result);

    // Init fmod
    result = system->init(512, FMOD_INIT_NORMAL, 0);
    LogError(result);
}

AudioManager::~AudioManager()
{
    system->close();
    system->release();
}

void AudioManager::FilterStoppedChannels()
{
    // Create iterator for the channel map
    std::vector<ChannelMap::iterator> stoppedChannels;

    for (auto it = Channels.begin(), itEnd = Channels.end(); it != itEnd; it++)
    {
        bool isPlaying = false;

        // Check if the channel is playing
        it->second.channel->isPlaying(&isPlaying);

        if (!isPlaying) {
            // If the channel is not playing, add its iterator to stoppedChannels
            stoppedChannels.push_back(it);
        }
    }

    // Iterate through the stopped channels and erase them from the Channels map
    for (auto& it : stoppedChannels)
    {
        Channels.erase(it);
    }
}

void AudioManager::UpdateSpatialChannels()
{
    for (auto& pair : Channels) {
        Channel& channel = pair.second;

        if (!channel.isSpatial) {
            continue;
        }

        float distance = glm::distance(channel.position, origin);
        float volume = std::max(std::min(distance, channel.audioRange), 0.f);
        volume = 1 - (volume / channel.audioRange);

        float dx = origin.x - channel.position.x;
        float pan = dx / channel.audioRange;

        channel.channel->setPan(pan);
        channel.channel->setVolume(volume * channel.volumeMultiplier);

        if (channel.initSpatial) {
            channel.channel->setPaused(false);
            channel.initSpatial = false;
        }
    }
}

void AudioManager::Update()
{
    FilterStoppedChannels();
    UpdateSpatialChannels();

    FMOD_RESULT result;
    result = system->update();
    LogError(result);
}

void AudioManager::LogError(FMOD_RESULT result) {
    if (result != FMOD_OK)
    {
        auto errorString = FMOD_ErrorString(result);
        printf("FMOD error! (%d) %s\n", result, errorString);

        assert(0);
    }
}

void AudioManager::LoadSound(const std::string& soundName, bool isStream)
{
    auto soundExists = Sounds.find(soundName);

    // return if sound already exists
    if (soundExists != Sounds.end()) {
        return;
    }

    // Set sound mode
    FMOD_MODE mode = FMOD_DEFAULT;
    mode |= /*is3D ? FMOD_3D : */FMOD_2D; // we aren't doing 3d sounds pretty sure
    mode |= /*isLooping ?*/ FMOD_LOOP_NORMAL;// if !isLooping, loop only once (aka play only once) : FMOD_LOOP_OFF;
    mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    // Create sound
    FMOD::Sound* sound = nullptr;

    system->createSound(soundName.c_str(), mode, 0, &sound);
    if (sound) {
        Sounds[soundName] = sound;
    }
}

void AudioManager::UnloadSound(const std::string& soundName)
{
    auto soundExists = Sounds.find(soundName);

    // return if sound doesn't exist
    if (soundExists == Sounds.end()) {
        return;
    }

    // Release sound
    FMOD_RESULT result;
    result = soundExists->second->release();
    LogError(result);

    Sounds.erase(soundExists);
}

int AudioManager::SoundPlay(const char* source, bool isLooping, float pitchMod, float volumeMultiplier, bool isSpatial, const glm::vec2& soundPos, float soundRange)
{
    // Get current channel ID and increment next channel id counter.
    int channelID = nextChannelID++ % 100000;

    auto soundExists = Sounds.find(source);

    // Create sound if it doesn't exist
    if (soundExists == Sounds.end())
    {
        LoadSound(source);
        soundExists = Sounds.find(source);

        // If it couldn't be created then return
        if (soundExists == Sounds.end())
        {
            return channelID;
        }
    }

    // Play sound
    FMOD::Channel* newChannel = nullptr;
    FMOD_RESULT result;
    result = system->playSound(soundExists->second, nullptr, true, &newChannel);
    LogError(result);

    if (newChannel)
    {
        // Add to channels map
        Channel ch;
        ch.channel = newChannel;
        Channel& channel = Channels[channelID] = ch;

        SetChannelVolumeMult(channelID, volumeMultiplier);
        SetChannelLooping(channelID, isLooping);
        SetChannelPitch(channelID, pitchMod);
        if (isSpatial) {
            // Set spatial settings
            SetChannelPosition(channelID, soundPos);
            SetChannelRange(channelID, soundRange);

            channel.initSpatial = true;
        }
        else {
            // Set volume and unpause sound
            SetChannelVolumeMult(channelID, volumeMultiplier);
            FMOD_RESULT result = channel.channel->setPaused(false);
            LogError(result);
        }
    }

    return channelID;
}

void AudioManager::StopChannel(int channelId)
{
    auto channelExists = Channels.find(channelId);

    // If the channel doesn't exist, return
    if (channelExists == Channels.end()) {
        return;
    }

    channelExists->second.channel->stop();
}

void AudioManager::StopAllChannels()
{
    for (auto it = Channels.begin(), itEnd = Channels.end(); it != itEnd; it++)
    {
        it->second.channel->stop();
    }
}

void AudioManager::SetChannelPosition(int channelId, const glm::vec2& vPosition)
{
    auto channelExists = Channels.find(channelId);

    // If the channel doesn't exist, return
    if (channelExists == Channels.end()) {
        return;
    }

    //FMOD_VECTOR vPos = vPosition.toFMOD();
    //result = channelExists->second.channel->set3DAttributes(&vPos, NULL);
    channelExists->second.position = vPosition;
    //LogError(result);
}

void AudioManager::SetChannelVolumeMult(int channelId, float volumeMult)
{
    auto channelExists = Channels.find(channelId);

    // If the channel doesn't exist, return
    if (channelExists == Channels.end()) {
        return;
    }

    channelExists->second.volumeMultiplier = volumeMult;
}

void AudioManager::SetChannelLooping(int channelId, bool isLooping)
{
    auto channelExists = Channels.find(channelId);

    // If the channel doesn't exist, return
    if (channelExists == Channels.end()) {
        return;
    }

    if (!isLooping) {
        channelExists->second.channel->setLoopCount(0);
    }
    else {
        channelExists->second.channel->setLoopCount(-1);
    }
}

void AudioManager::SetChannelPitch(int channelId, float pitch)
{
    auto channelExists = Channels.find(channelId);

    // If the channel doesn't exist, return
    if (channelExists == Channels.end()) {
        return;
    }

    channelExists->second.channel->setPitch(pitch);

}

void AudioManager::SetChannelSpatial(int channelId, bool isSpatial)
{
    auto channelExists = Channels.find(channelId);

    // If the channel doesn't exist, return
    if (channelExists == Channels.end()) {
        return;
    }

    channelExists->second.isSpatial = isSpatial;
}

void AudioManager::SetChannelRange(int channelId, float soundRange)
{
    auto channelExists = Channels.find(channelId);

    // If the channel doesn't exist, return
    if (channelExists == Channels.end()) {
        return;
    }

    channelExists->second.audioRange = soundRange;
}

void AudioManager::SetChannelPaused(int channelId, bool isPaused)
{
    auto channelExists = Channels.find(channelId);

    // If the channel doesn't exist, return
    if (channelExists == Channels.end()) {
        return;
    }

    FMOD_RESULT result;
    result = channelExists->second.channel->setPaused(isPaused);
}

bool AudioManager::IsPlaying(int channelId) const
{
    auto channelExists = Channels.find(channelId);

    // If the channel doesn't exist, return
    if (channelExists == Channels.end()) {
        return false;
    }

    FMOD_RESULT result;
    bool paused;
    result = channelExists->second.channel->getPaused(&paused);
    return paused;
}

void AudioManager::SetListenerLocation(const glm::vec2& vPosition)
{
    origin = vPosition;
}