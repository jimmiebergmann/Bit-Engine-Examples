#include <Bit/Audio/OpenAL/OpenALAudioDevice.hpp>
#include <Bit/Audio/SoundBuffer.hpp>
#include <Bit/Audio/Sound.hpp>
#include <Bit/System/Keyboard.hpp>
#include <Bit/System/Vector3.hpp>
#include <Bit/System/Sleep.hpp>
#include <iostream>
#include <Bit/System/MemoryLeak.hpp>

// Audio varaibles
Bit::AudioDevice * pAudioDevice = NULL;
Bit::Sound * pSound = NULL;
Bit::SoundBuffer * pSoundBuffer = NULL;
Bit::Keyboard keyboard;

// Settings
const std::string soundFilePath = "../../../Data/PowerUp1.wav";


// Global functions
int CloseApplication( const int p_Code );
Bit::Bool LoadAudio( );

// Main function
int main( int argc, char ** argv )
{
	// Initialize the memory leak detector for Win32 only (ignored by default in linux)
	BitInitMemoryLeak( NULL );

	// Initialize the application
	if( LoadAudio( ) != true )
	{
		return CloseApplication( 0 );
	}

	std::cout << "Press 'P' to play sound.\nPress 'Escape' to close.\n";

	// Main loop.
	while( 1 )
	{

		// Update the keyboard
		keyboard.Update( );

		// Should we exit the program?
		if( keyboard.KeyIsJustReleased( Bit::Keyboard::Escape ) )
		{
			break;
		}

		// Play sound
		if( keyboard.KeyIsJustReleased( Bit::Keyboard::P ) )
		{
			pSound->Play( );
		}

	}

	// We are done
	return CloseApplication( 0 );
}

int CloseApplication( const int p_Code )
{
	if( pSoundBuffer )
	{
		delete pSoundBuffer;
		pSoundBuffer = NULL;
	}

	if( pSound )
	{
		delete pSound;
		pSound = NULL;
	}
	
	if( pAudioDevice )
	{
		delete pAudioDevice;
		pAudioDevice = NULL;
	}

	// Return the code
	std::cout << "Closing the program.";
	Bit::Sleep( Bit::Seconds( 0.66f ) );
	std::cout << ".";
	Bit::Sleep( Bit::Seconds( 0.66f ) );
	std::cout << ".";
	Bit::Sleep( Bit::Seconds( 0.66f ) );
	return p_Code;
}

Bit::Bool LoadAudio( )
{
	// Create the audio device
	 pAudioDevice = new Bit::OpenALAudioDevice;

	// Open the audio device
	if( pAudioDevice->Open( ) != true )
	{
		std::cout << "[Error] Can not open the audio device\n";
		return false;
	}
	pAudioDevice->SetGlobalVolume( 1.0f );
	pAudioDevice->SetListenerPosition( Bit::Vector3f32( 0.0f, 0.0f, 0.0f ) );
	pAudioDevice->SetListenerTarget( Bit::Vector3f32( 0.0f, 0.0f, 1.0f ) );
	pAudioDevice->SetListenerVelocity( Bit::Vector3f32( 0.0f, 0.0f, 0.0f ) );

	// Create the audio buffer
	if( ( pSoundBuffer = pAudioDevice->CreateSoundBuffer( ) ) == NULL )
	{
		std::cout << "[Error] Can not create the sound buffer.\n";
		return false;
	}
	if( pSoundBuffer->LoadFromFile( soundFilePath ) != true )
	{
		std::cout << "[Error] Can not load the sound buffer from the given filepath.\n";
		return false;
	}

	// Create the sound
	if( ( pSound = pAudioDevice->CreateSound( ) ) == NULL )
	{
		std::cout << "[Error] Can not create the sound.\n";
		return false;
	}

	// Load the sound
	if( pSound->LoadFromBuffer( *pSoundBuffer ) != true )
	{
		std::cout << "[Error] Can not load the sound from the given sound buffer.\n";
		return false;
	}
	pSound->SetRelative( true );

	return true;
}