#include <Bit/Window/Window.hpp>
#include <Bit/Graphics/GraphicDevice.hpp>
#include <Bit/System/Timer.hpp>
#include <Bit/System/Randomizer.hpp>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>
#include <iostream>

Bit::Window * pWindow = BIT_NULL;
Bit::GraphicDevice * pGraphicDevice = BIT_NULL;

int CloseApplication( const std::string p_Message, const int p_Code );

int main( )
{
	// Initialize the memory leak detector
	bitInitMemoryLeak( BIT_NULL );


	// Create a window
	if( ( pWindow = Bit::CreateWindow( ) ) == BIT_NULL )
	{
		return CloseApplication( "Can not create the window", 0 );
	}

	// Create the style we want for the window
	BIT_UINT32 Style = Bit::Window::Style_TitleBar | Bit::Window::Style_Minimize |  Bit::Window::Style_Resize | Bit::Window::Style_Close;

	// Open the window
	if( pWindow->Open( Bit::Vector2_ui32( 800, 600 ), 32, "Hello World", Style ) != BIT_OK )
	{
		return CloseApplication( "Can not open the window", 0 );
	}

	// Change the window's title
	pWindow->SetTitle( "Cool. We can now change the window title. Testing swedish characters: åäö ÅÄÖ" );

	// Create a graphic device
	if( ( pGraphicDevice = Bit::CreateGraphicDevice( ) ) == BIT_NULL )
	{
		return CloseApplication( "Can not create the graphic device", 0 );
	}

	// Open the graphic device
	if( pGraphicDevice->Open( *pWindow, 0 ) != BIT_OK )
	{
		//return CloseApplication( "Can not open the graphic device", 0 );
	}


	// Create a timer and run a main loop for some time
	Bit::Timer Timer;
	Timer.Start( );

	// Run the main loop
	while( Timer.GetLapsedTime( ) < 17.0f && pWindow->IsOpen( ) )
	{
		// Do evenets
		pWindow->DoEvents( );

		Bit::Event Event;
		while( pWindow->PollEvent( Event ) )
		{

		    usleep( 1000 );

			switch( Event.Type )
			{
				case Bit::Event::Closed:
				{
					bitTrace( "[Event] Closed\n" );
				}
				break;
                case Bit::Event::Moved:
				{
					bitTrace( "[Event] Moved: %i %i\n",
						Event.Position.x, Event.Position.y );
				}
				break;
				case Bit::Event::Resized:
				{
					bitTrace( "[Event] Resized: %i %i\n",
						Event.Size.x, Event.Size.y );
				}
				break;
				case Bit::Event::GainedFocus:
				{
					bitTrace( "[Event] Gained Focus\n" );
				}
				break;
				case Bit::Event::LostFocus:
				{
					bitTrace( "[Event] Lost Focus\n" );
				}
				break;
				case Bit::Event::KeyPressed:
				{
					bitTrace( "[Event] Key Pressed: %i\n", Event.Key );
				}
				break;
				case Bit::Event::KeyReleased:
				{
					bitTrace( "[Event] Key Released: %i\n", Event.Key );
				}
				break;
				case Bit::Event::MouseMoved:
				{
					bitTrace( "[Event] Mouse Moved: %i %i\n",
						Event.MousePosition.x, Event.MousePosition.y );
				}
				break;
				case Bit::Event::MouseButtonPressed:
				{
					bitTrace( "[Event] Mouse Button Pressed: %i   %i %i\n",
						Event.Button, Event.MousePosition.x, Event.MousePosition.y );
				}
				break;
				case Bit::Event::MouseButtonReleased:
				{
					bitTrace( "[Event] Mouse Button Released: %i   %i %i\n",
						Event.Button, Event.MousePosition.x, Event.MousePosition.y );
				}
				break;
				default:
					break;
			}
		}

		// Clear the buffers
		pGraphicDevice->ClearColor( );
		pGraphicDevice->ClearDepth( );

		// Present the buffers
		pGraphicDevice->Present( );
	}

	// Destroy the window
	bitTrace( "Closing window.\n" );
	pWindow->Close( );

	// Test the random function
	Bit::SeedRandomizer( Bit::Timer::GetSystemTime( ) );
	BIT_UINT32 RandomNumber = Bit::RandomizeNumber( 100, 150 );

	// Do some debug printing
	bitTrace( "This is a random number: %i\n", RandomNumber );
	bitTrace( "Closing the program.\n" );

	// We are done
	return CloseApplication( "", 0 );
}

int CloseApplication( const std::string p_Message, const int p_Code )
{
	// Clean up the window and graphic device.
	if( pWindow )
	{
		delete pWindow;
		pWindow = BIT_NULL;
	}

	if( pGraphicDevice )
	{
		delete pGraphicDevice;
		pGraphicDevice = BIT_NULL;
	}

	// Display the error message if any
	if( p_Message.length( ) )
	{
		bitTrace( "[Error] %s\n", p_Message.c_str( ) );
	}

	// Return the code
	return p_Code;
}
