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
	Bit::Window * pWindow = Bit::CreateWindow( );

	// Create the window and check if it's created
	BIT_UINT32 Style = Bit::Window::Style_TitleBar | Bit::Window::Style_Minimize | Bit::Window::Style_Close;

	if( pWindow->Open( Bit::Vector2_ui32( 800, 600 ), 32, "Hello World", Style ) != BIT_OK ||
		!pWindow->IsOpen( ) )
	{
		bitTrace( "Can not create the window." );

		delete pWindow;
		return 0;
	}

	// Change the window title
	pWindow->SetTitle( "Cool. We can now change the window title. Testing swedish characters: åäö ÅÄÖ" );


	// Create a timer and run a main loop for some time
	Bit::Timer Timer;
	Timer.Start( );

	// Run the main loop
	while( Timer.GetLapsedTime( ) < 3.0f && pWindow->IsOpen( ) )
	{
		pWindow->DoEvents( );

	}

	// Destroy the window
	pWindow->Close( );

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
