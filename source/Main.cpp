#include <Bit/Window.hpp>
#include <Bit/System/Timer.hpp>
#include <Bit/System/Randomizer.hpp>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>
#include <iostream>

int main( )
{
	// Initialize the memory leak detector
	bitInitMemoryLeak( BIT_NULL );

	// Declare a new window
	Bit::Window * pWindow = new Bit::Window( );

	// Create the window and check if it's created
	BIT_UINT32 Style = Bit::Window::Style_TitleBar | Bit::Window::Style_Minimize | Bit::Window::Style_Close;

	if( pWindow->Create( Bit::Vector2_ui32( 800, 600 ), 32, "Hello World", Style ) != BIT_OK ||
		!pWindow->IsCreated( ) )
	{
		bitTrace( "Can not create the window." );

		delete pWindow;
		return 0;
	}

	// Change the window title
	//pWindow->SetTitle( "Cool. We can now change the window title. Testing swedish characters: åäö ÅÄÖ" );

	// Create a timer and run a main loop for some time
	Bit::Timer Timer;
	Timer.Start( );

	// Run the main loop
	while( Timer.GetLapsedTime( ) < 3.0f )
	{
		if( pWindow->DoEvents( ) != BIT_OK )
		{
		    break;
		}
	}

	// Destroy the window
	pWindow->Destroy( );

	// Delete the window
	delete pWindow;
	pWindow = BIT_NULL;


	// Test the random function
	Bit::SeedRandomizer( Bit::Timer::GetSystemTime( ) );
	BIT_UINT32 RandomNumber = Bit::RandomizeNumber( 100, 150 );

	// Do some debug printing
	bitTrace( "This is a random number: %i\n", RandomNumber );
	bitTrace( "Closing the program.\n" );

	// We are done
	return 0;
}
