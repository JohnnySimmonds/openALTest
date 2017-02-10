#include "Audio.h"

Audio::Audio()
{
}


Audio::~Audio()
{
	cleanUpAudio(wav);
	cleanUpAudio(wav2);
	cleanUpAudio(wav3);
	cleanUpAudio(wav4);

}

void Audio::initAudio()
{
	// Initialization

	/*Open a device(In this case the default device)*/
	
	Device = alcOpenDevice(nullptr);
	if (!Device)
	{
		perror("Error opening AL device");

	}
	/*Check to see if there are other devices to access*/
	ALboolean enumeration;
	enumeration = alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
	{
		cout << "Enumeration not supported" << endl;
	}
	else
	{
		cout << "Lets Enumerate" << endl;
		list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	}

	/*Reset the error stack has a depth of 1 error.  Do this to help determine where the errors are*/
	checkError();


	/*Initializing a context to render an audio scene*/
	
	context = alcCreateContext(Device, NULL);
	if (!alcMakeContextCurrent(context))
		cout << "Failed to make context current" << endl;

	checkError();

	/*Configuring a listener*/
	ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

	alListener3f(AL_POSITION, 0, 0, 1.0f);
	checkError();

	alListener3f(AL_VELOCITY, 0, 0, 0);
	checkError();

	alListenerfv(AL_ORIENTATION, listenerOri);
	checkError();

	/*Source Generation creating an audio source object which is the origin of the sound*/
	

	alGenSources(1, &wav.source);
	checkError();

	alGenSources(1, &wav2.source);
	checkError();

	alGenSources(1, &wav3.source);
	checkError();

	alGenSources(1, &wav4.source);
	checkError();


	wav.source = sourceSetup(wav.source);
	wav2.source = sourceSetup(wav2.source);
	wav3.source = sourceSetup(wav3.source);
	wav4.source = sourceSetup(wav4.source);
	

	/*Buffer Generation this holds the raw audio stream*/
	

	alGenBuffers(1, &wav.buffer);
	checkError();

	alGenBuffers(1, &wav2.buffer);
	checkError();

	alGenBuffers(1, &wav3.buffer);
	checkError();

	alGenBuffers(1, &wav4.buffer);
	checkError();

	wav = openWavFile("test.wav", wav);
	wav2 = openWavFile("test2.wav", wav2);
	wav3 = openWavFile("Bats.wav", wav3);
	wav4 = openWavFile("Inferno.wav", wav4);

	wav.format = formatWav(wav);
	wav2.format = formatWav(wav2);
	wav3.format = formatWav(wav3);
	wav4.format = formatWav(wav4);

}
void Audio::playAudio(int choice)
{
	wavFile* toPlay;
	switch (choice)
	{
		case 1:
			toPlay = &wav;
			break;
		case 2:
			toPlay = &wav2;
			break;
		case 3:
			toPlay = &wav3;
			break;
		case 4:
			toPlay = &wav4;
			break;
		default:
			cout << "invalid choice of song" << endl;
			return;
		
	}

	alBufferData(toPlay->buffer, toPlay->format, (ALvoid*)toPlay->songBuf, (ALsizei)toPlay->dataSize, (ALsizei)toPlay->sampleRate);
	checkError();

	alSourcei(toPlay->source, AL_BUFFER, toPlay->buffer);
	checkError();

	alSourcePlay(toPlay->source);

	/*This just forces it to play one song until the song is finished*/
	/*ALint source_state;

	alGetSourcei(toPlay->source, AL_SOURCE_STATE, &source_state);
	checkError();
	while (source_state == AL_PLAYING) 
	{
		alGetSourcei(toPlay->source, AL_SOURCE_STATE, &source_state);
		checkError();
	}
*/
}
void Audio::cleanUpAudio(wavFile wav)
{
	/*Clean up*/
	alDeleteSources(1, &wav.source);
	alDeleteBuffers(1, &wav.buffer);
	Device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(Device);
}
ALuint Audio::sourceSetup(ALuint source)
{

	alSourcef(source, AL_PITCH, 1);
	checkError();

	alSourcef(source, AL_GAIN, 1);
	checkError();

	alSource3f(source, AL_POSITION, 0, 0, 0);
	checkError();

	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	checkError();

	alSourcei(source, AL_LOOPING, AL_TRUE);
	checkError();

	return source;
}

ALenum Audio::formatWav(wavFile wav)
{
	ALenum format;
	if (wav.bitsPerSample == 8)
	{
		if (wav.channels == 1)
			format = AL_FORMAT_MONO8;
		else if (wav.channels == 2)
			format = AL_FORMAT_STEREO8;
	}
	if (wav.bitsPerSample == 16)
	{
		if (wav.channels == 1)
			format = AL_FORMAT_MONO16;
		else if (wav.channels == 2)
			format = AL_FORMAT_STEREO16;
	}
	return format;

}
/*Initializes the wav file to be read*/
wavFile Audio::openWavFile(char* fileName, wavFile toPlay)
{
	wavFile wav = toPlay;

	char type[4];
	DWORD size, chunkSize;
	short formatType, channels;
	DWORD sampleRate, avgBytesPerSec;
	short bytesPerSample, bitsPerSample;
	DWORD dataSize;
	unsigned char* buf;

	FILE *fp = nullptr;
	fopen_s(&fp, fileName, "rb");


	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F')
	{
		cout << "RIFF PRINT OUT: " << type[0] << type[1] << type[2] << type[3] << endl;
		cout << "No RIFF" << endl;
	}

	fread(&size, sizeof(DWORD), 1, fp);

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E')
	{
		cout << "WAVE PRINT OUT: " << type[0] << type[1] << type[2] << type[3] << endl;
		cout << "Not a WAVE" << endl;
	}

	fread(type, sizeof(char), 4, fp);

	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ')
	{
		cout << "FMT PRINT OUT: " << type[0] << type[1] << type[2] << type[3] << endl;
		cout << "No fmt " << endl;
	}

	fread(&chunkSize, sizeof(DWORD), 1, fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a')
	{
		cout << "DATA PRINT OUT: " << type[0] << type[1] << type[2] << type[3] << endl;
		cout << "No DATA" << endl;
	}

	fread(&dataSize, sizeof(DWORD), 1, fp);

	buf = new unsigned char[dataSize];
	fread(buf, sizeof(BYTE), dataSize, fp);

	wav.size = size;
	wav.chunkSize = chunkSize;
	wav.formatType = formatType;
	wav.channels = channels;
	wav.sampleRate = sampleRate;
	wav.avgBytesPerSec = avgBytesPerSec;
	wav.bytesPerSample = bytesPerSample;
	wav.bitsPerSample = bitsPerSample;
	wav.dataSize = dataSize;
	wav.songBuf = buf;
	return wav;

}
/*openAL apparently needs us to check for errors constantly, this just makes it easier*/
void Audio::checkError()
{
	ALCenum error;
	error = alGetError();
	if (error != AL_NO_ERROR)
		perror("CHECK ERROR");
}
/*Probably not need just shows what audio devices are available*/
void Audio::list_audio_devices(const ALCchar *devices)
{
	const ALCchar *device = devices, *next = devices + 1;
	size_t len = 0;

	fprintf(stdout, "Devices list:\n");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		fprintf(stdout, "%s\n", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	fprintf(stdout, "----------\n");
}
