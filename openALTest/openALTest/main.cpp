#include "al.h"
#include "alc.h"
#include "efx.h"
#include "efx-creative.h"
#include "EFX-Util.h"
#include "xram.h"
#include <windows.h>
#include <cstdlib>
#include <iostream>
using namespace std;
static void list_audio_devices(const ALCchar *devices);
void checkError();


struct wavFile {

	char type[4];
	DWORD size, chunkSize;
	short formatType, channels;
	DWORD sampleRate, avgBytesPerSec;
	short bytesPerSample, bitsPerSample;
	DWORD dataSize;
	unsigned char* buf;

};
wavFile openWavFile(char* fileName);
int main()
{
	// Initialization
	
	
	


	/*Open a device(In this case the default device)*/
	ALCdevice* Device; // select the "preferred device"
	Device = alcOpenDevice(nullptr);
	if (!Device)
	{
		perror("Error opening AL device");
		return 1;
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
	ALCcontext *context;
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
	ALuint source;


	alGenSources(1, &source);
	checkError();

	alSourcef(source, AL_PITCH, 1);
	checkError();

	alSourcef(source, AL_GAIN, 1);
	checkError();

	alSource3f(source, AL_POSITION, 0, 0, 0);
	checkError();

	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	checkError();

	alSourcei(source, AL_LOOPING, AL_FALSE);
	checkError();

	/*Buffer Generation this holds the raw audio stream*/
	ALuint buffer;

	alGenBuffers(1, &buffer);
	checkError();

	/*Loading an audio stream to a buffer*/

	ALenum format;
	//ALvoid *data;
	//ALuint frequency;
	ALboolean loop = AL_FALSE;
	wavFile wav = openWavFile("C:\\Users\\Johnny\\Documents\\Visual Studio 2015\\Projects\\openALTest\\openALTest\\test.wav");

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

	alBufferData(buffer, format, (ALvoid*)wav.buf, (ALsizei)wav.dataSize, (ALsizei)wav.sampleRate);
	checkError();

	alSourcei(source, AL_BUFFER, buffer);
	checkError();

	alSourcePlay(source);
	
	ALint source_state;
	alGetSourcei(source, AL_SOURCE_STATE, &source_state);
	checkError();
	while (source_state == AL_PLAYING) 
	{
		alGetSourcei(source, AL_SOURCE_STATE, &source_state);
		checkError();
	}
	
	/*Clean up*/
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	Device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(Device);
	
	while (1);
	return 0;
}
/*Initializes the wav file to be read*/
wavFile openWavFile(char* fileName)
{
	wavFile wav;

	char type[4];
	DWORD size, chunkSize;
	short formatType, channels;
	DWORD sampleRate, avgBytesPerSec;
	short bytesPerSample, bitsPerSample;
	DWORD dataSize;

	FILE *fp = nullptr;
	fopen_s(&fp, fileName, "rb");
	
	
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'R' || type[1] != 'I' || type[3] != 'F' || type[3] != 'F')
	{
		cout << "RIFF PRINT OUT: " << type[0] << type[1] << type[2] << type[3] << endl;
		cout << "No RIFF" << endl;
		//return ;
	}
	
	fread(&size, sizeof(DWORD), 1, fp);
	
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'W' || type[1] != 'A' || type[3] != 'V' || type[3] != 'E')
	{
		cout << "WAVE PRINT OUT: " << type[0] << type[1] << type[2] << type[3] << endl;
		cout << "Not a WAVE" << endl;
		//return ;
	}

	fread(type, sizeof(char), 4, fp);
	
	if (type[0] != 'f' || type[1] != 'm' || type[3] != 't' || type[3] != ' ')
	{
		cout << "FMT PRINT OUT: " << type[0] << type[1] << type[2] << type[3] << endl;
		cout << "No fmt " << endl;
		//return ;
	}

	fread(&chunkSize, sizeof(DWORD),1,fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'd' || type[1] != 'a' || type[3] != 't' || type[3] != 'a')
	{
		cout << "DATA PRINT OUT: " << type[0] << type[1] << type[2] << type[3] << endl;
		cout << "No DATA" << endl;
		//return ;
	}
	
	fread(&dataSize, sizeof(DWORD), 1, fp);

	wav.buf = new unsigned char[dataSize];
	fread(wav.buf, sizeof(BYTE), dataSize, fp);

	wav.size = size;
	wav.chunkSize = chunkSize;
	wav.formatType = formatType;
	wav.channels = channels;
	wav.sampleRate = sampleRate;
	wav.avgBytesPerSec = avgBytesPerSec;
	wav.bytesPerSample = bytesPerSample;
	wav.bitsPerSample = bytesPerSample;
	wav.dataSize = dataSize;

	return wav;

}
void checkError()
{
	ALCenum error;
	error = alGetError();
	if (error != AL_NO_ERROR)
		perror("error: ");
}
static void list_audio_devices(const ALCchar *devices)
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