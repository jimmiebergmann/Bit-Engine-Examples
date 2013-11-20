#include <Bit/Audio/AudioDevice.hpp>
#include <Bit/Audio/AudioBuffer.hpp>
#include <Bit/Audio/Audio.hpp>
#include <Bit/System/Keyboard.hpp>
#include <Bit/System.hpp>
#include <Bit/System/Vector3.hpp>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>

// Audio varaibles
Bit::AudioDevice * pAudioDevice = BIT_NULL;
Bit::Audio * pAudio = BIT_NULL;
Bit::Keyboard * pKeyboard = BIT_NULL;

// Settings
const std::string SoundFilePath = "../../../Data/PowerUp1.wav";


// Global functions
int CloseApplication( const int p_Code );
BIT_UINT32 LoadAudio( );
BIT_UINT32 LoadInput( );

// Main function
int main( int argc, char ** argv )
{
	// Initialize the memory leak detector for Win32 only (ignored by default in linux)
	bitInitMemoryLeak( BIT_NULL );

	// Setting the absolute path in order to read files.
	Bit::SetAbsolutePath( argv[ 0 ] );

	// Initialize the application
	if( LoadAudio( ) != BIT_OK ||
		LoadInput( ) != BIT_OK )
	{
		return CloseApplication( 0 );
	}


	while( 1 )
	{

		// Update the keyboard
		pKeyboard->Update( );

		// Should we exit the program?
		if( pKeyboard->KeyIsJustReleased( Bit::Keyboard::Key_Escape ) )
		{
			break;
		}

		// Play sound
		if( pKeyboard->KeyIsJustReleased( Bit::Keyboard::Key_P ) )
		{
			pAudio->Play( );
		}

	}

	// We are done
	bitTrace( "Closing the program.\n" );
	return CloseApplication( 0 );
}

int CloseApplication( const int p_Code )
{
	if( pAudio )
	{
		delete pAudio;
		pAudio = BIT_NULL;
	}
	
	if( pAudioDevice )
	{
		delete pAudioDevice;
		pAudioDevice = BIT_NULL;
	}

	if( pKeyboard )
	{
		delete pKeyboard;
		pKeyboard = BIT_NULL;
	}

	// Return the code
	return p_Code;
}

BIT_UINT32 LoadAudio( )
{
	// Create the audio device
	if( ( pAudioDevice = Bit::CreateAudioDevice( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the audio device\n" );
		return BIT_ERROR;
	}

	// Open the audio device
	if( pAudioDevice->Open( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not open the audio device\n" );
		return BIT_ERROR;
	}
	pAudioDevice->SetGlobalVolume( 1.0f );
	pAudioDevice->SetListenerPosition( Bit::Vector3_f32( 0.0f, 0.0f, 0.0f ) );
	pAudioDevice->SetListenerTarget( Bit::Vector3_f32( 0.0f, 0.0f, 1.0f ) );
	pAudioDevice->SetListenerVelocity( Bit::Vector3_f32( 0.0f, 0.0f, 0.0f ) );

	// Load the audio buffer
	Bit::AudioBuffer AudioBuffer;
	if( AudioBuffer.Read( Bit::GetAbsolutePath( SoundFilePath ).c_str( ) ) != BIT_OK )
	{
		bitTrace( "[Error] Can not open the sound file\n" );
		return BIT_ERROR;
	}

	// Create the sound
	if( ( pAudio = pAudioDevice->CreateAudio( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the audio \n" );
		return BIT_ERROR;
	}

	// Load the sound
	if( pAudio->Load( AudioBuffer ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the sound \n" );
		return BIT_ERROR;
	}
	pAudio->SetRelative( BIT_TRUE );

	return BIT_OK;
}

BIT_UINT32 LoadInput( )
{
	if( ( pKeyboard = Bit::CreateKeyboard( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the keyboard.\n" );
		return BIT_ERROR;
	}

	return BIT_OK;
}