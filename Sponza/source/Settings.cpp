#include <Settings.hpp>
#include <fstream>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>


// Constructor/destructor
Settings::Settings( ) :
	m_WindowSize( 0, 0 )
{
}

Settings::~Settings( )
{
}

// Public functions
BIT_UINT32 Settings::Open( const char * p_FilePath )
{
	// Open the file
	std::ifstream fin( p_FilePath, std::ifstream::in );

	// Is the file open?
	if( !fin.is_open( ) )
	{
		bitTrace( "[Settings::Open] Can not open the file.\n" );
		return BIT_ERROR_OPEN_FILE;
	}

	// Read the screen resolution
	if( !fin.eof( ) )
	{
		fin >> m_WindowSize.x;
	}
	if( !fin.eof( ) )
	{
		fin >> m_WindowSize.y;
	}
	if( !fin.eof( ) )
	{
		fin >> m_UseNormalMapping;
	}

	// Error check the widnow size
	if( m_WindowSize.x > 4096 || m_WindowSize.y > 4096 )
	{
		bitTrace( "[Settings::Open] The window size is way too large.\n" );
		return BIT_ERROR;
	}
	
	// Everything is ok
	return BIT_OK;
}

// Set functions
void Settings::SetWindowSize( const Bit::Vector2_ui32 p_WindowSize )
{
	m_WindowSize = p_WindowSize;
}

void Settings::SetUseNormalMapping( const BIT_BOOL p_Status )
{
	m_UseNormalMapping = p_Status;
}

// Get functions
Bit::Vector2_ui32 Settings::GetWindowSize( ) const
{
	return m_WindowSize;
}

BIT_BOOL Settings::GetUseNormalMapping( ) const
{
	return m_UseNormalMapping;
}