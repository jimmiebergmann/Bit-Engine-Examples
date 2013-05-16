#include <Bit/Window/Window.hpp>
#include <Bit/Graphics/GraphicDevice.hpp>
#include <Bit/System/Timer.hpp>
#include <Bit/System/Randomizer.hpp>
#include <Bit/System/Vector3.hpp>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>
#include <iostream>

// Global variables
Bit::Window * pWindow = BIT_NULL;
Bit::GraphicDevice * pGraphicDevice = BIT_NULL;
Bit::VertexObject * pVertexObject = BIT_NULL;
Bit::Shader * pVertexShader = BIT_NULL;
Bit::Shader * pFragmentShader = BIT_NULL;
Bit::ShaderProgram * pShaderProgram = BIT_NULL;
Bit::Vector2_ui32 WindowSize( 800, 600 );

// Global functions
int CloseApplication( const int p_Code );
BIT_UINT32 CreateWindow( );
BIT_UINT32 CreateGraphicDevice( );
BIT_UINT32 CreateVertexObject( );
BIT_UINT32 CreateShaders( );
BIT_UINT32 CreateShaderProgram( );

// Main function
int main( )
{
	// Initialize the memory leak detector
	bitInitMemoryLeak( BIT_NULL );


	// Initialize the application
	if( CreateWindow( ) != BIT_OK ||
		CreateGraphicDevice( ) != BIT_OK ||
		CreateVertexObject( ) != BIT_OK ||
		CreateShaders( ) != BIT_OK ||
		CreateShaderProgram( ) != BIT_OK )
	{
		return CloseApplication( 0 );
	}


	// Create a timer and run a main loop for some time
	Bit::Timer Timer;
	Timer.Start( );

	// Run the main loop
	while( Timer.GetLapsedTime( ) < 16.0f && pWindow->IsOpen( ) )
	{
		// Do evenets
		pWindow->Update( );

		Bit::Event Event;
		while( pWindow->PollEvent( Event ) )
		{

			switch( Event.Type )
			{
				case Bit::Event::Closed:
				{
					bitTrace( "[Event] Closed\n" );
					return CloseApplication( 0 );
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

					// Pressed ESC key
					if( Event.Key == 27 || Event.Key == 65307 )
					{
						return CloseApplication( 0 );
					}
				}
				break;
				case Bit::Event::KeyReleased:
				{
					bitTrace( "[Event] Key Released: %i\n", Event.Key );
				}
				break;
				/*case Bit::Event::MouseMoved:
				{
					bitTrace( "[Event] Mouse Moved: %i %i\n",
						Event.MousePosition.x, Event.MousePosition.y );
				}
				break;*/
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

		// Render the vertex object
		// !NOTE! Wont be visible because of the lack of shaders!

		pShaderProgram->Bind( );
		pVertexObject->Render( Bit::VertexObject::RenderMode_Triangles );
		pShaderProgram->Unbind( );

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
	return CloseApplication( 0 );
}

int CloseApplication( const int p_Code )
{
	if( pVertexObject )
	{
		delete pVertexObject;
		pVertexObject = BIT_NULL;
	}

	if( pVertexShader )
	{
		delete pVertexShader;
		pVertexShader = BIT_NULL;
	}
	if( pFragmentShader )
	{
		delete pFragmentShader;
		pFragmentShader = BIT_NULL;
	}
	if( pShaderProgram )
	{
		delete pShaderProgram;
		pShaderProgram = BIT_NULL;
	}

	if( pGraphicDevice )
	{
		delete pGraphicDevice;
		pGraphicDevice = BIT_NULL;
	}

	if( pWindow )
	{
		delete pWindow;
		pWindow = BIT_NULL;
	}

	// Return the code
	return p_Code;
}

BIT_UINT32 CreateWindow( )
{
	// Create a window
	if( ( pWindow = Bit::CreateWindow( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the window\n" );
		return BIT_ERROR;
	}

	// Create the style we want for the window
	BIT_UINT32 Style = Bit::Window::Style_TitleBar | Bit::Window::Style_Minimize |  Bit::Window::Style_Resize | Bit::Window::Style_Close;

	// Open the window
	if( pWindow->Open( WindowSize, 32, "Hello World", Style ) != BIT_OK )
	{
		bitTrace( "[Error] Can not open the window\n" );
		return BIT_ERROR;
	}

	// Change the window's title
	pWindow->SetTitle( "Cool. We can now change the window title. Testing swedish characters: ��� ���" );

	return BIT_OK;
}

BIT_UINT32 CreateGraphicDevice( )
{
	// Create a graphic device
	if( ( pGraphicDevice = Bit::CreateGraphicDevice( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the graphic device\n" );
		return BIT_ERROR;
	}

	// Open the graphic device
	if( pGraphicDevice->Open( *pWindow, 0 ) != BIT_OK )
	{
		bitTrace( "[Error] Can not open the graphic device\n" );
		return BIT_ERROR;
	}

	// Set the clear color
	pGraphicDevice->SetClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	pGraphicDevice->SetViewport( 0, 0, WindowSize.x, WindowSize.y );
	pGraphicDevice->EnableDepthTest( );

	return BIT_OK;
}

BIT_UINT32 CreateVertexObject( )
{
	// Create a vertex object via the graphic device
	if( ( pVertexObject = pGraphicDevice->CreateVertexObject( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the vertex object\n" );
		return BIT_ERROR;
	}

	// Create vertex array
	BIT_FLOAT32 VertexCoordData[ 9 ] =
	{
		100, 100, 0,	200, 100, 0,	200, 200, 0
	};
	BIT_FLOAT32 VertexTexData[ 6 ] =
	{
		0, 0,	1, 0,	1, 1
	};

	// Add vertex buffer
	if( pVertexObject->AddVertexBuffer( VertexCoordData, 3, BIT_TYPE_FLOAT32 ) == BIT_ERROR )
	{
		bitTrace( "[Error] Can not add vertex coord data to the vertex object\n" );
		return BIT_ERROR;
	}
	if( pVertexObject->AddVertexBuffer( VertexTexData, 2, BIT_TYPE_FLOAT32 ) == BIT_ERROR )
	{
		bitTrace( "[Error] Can not add vertex texture data to the vertex object\n" );
		return BIT_ERROR;
	}

	// Load the vertex object
	if( pVertexObject->Load( 1, 3 ) == BIT_ERROR )
	{
		bitTrace( "[Error] Can not load the vertex object\n" );
		return BIT_ERROR;
	}

	// Update the tex coo buffer
	BIT_FLOAT32 pNewData[ 6 ] =
	{
		1, 2, 3, 4, 5, 6
	};
	if( pVertexObject->UpdateVertexBuffer( 1, pNewData, 0, 6 ) == BIT_ERROR )
	{
		bitTrace( "[Error] Can not create the vertex object\n" );
		return BIT_ERROR;
	}

	return BIT_OK;
}

BIT_UINT32 CreateShaders( )
{
	// Create the vertex and fragment shaders
	if( ( pVertexShader = pGraphicDevice->CreateShader( Bit::Shader::Vertex ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the vertex shader\n" );
		return BIT_ERROR;
	}
	if( ( pFragmentShader = pGraphicDevice->CreateShader( Bit::Shader::Fragment ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the vertex shader\n" );
		return BIT_ERROR;
	}

	// Read the sources
	if( pVertexShader->Read( "../../../Data/VertexShader.txt" ) != BIT_OK )
	{
		bitTrace( "[Error] Can not read the vertex shader file\n" );
		return BIT_ERROR;
	}
	if( pFragmentShader->Read( "../../../Data/FragmentShader.txt" ) != BIT_OK )
	{
		bitTrace( "[Error] Can not read the fragment shader file\n" );
		return BIT_ERROR;
	}

	// Compile the shaders
	if( pVertexShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the vertex shader\n" );
		return BIT_ERROR;
	}
	if( pFragmentShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the fragment shader\n" );
		return BIT_ERROR;
	}


	return BIT_OK;
}

BIT_UINT32 CreateShaderProgram( )
{
	// Create the vertex and fragment shaders
	if( ( pShaderProgram = pGraphicDevice->CreateShaderProgram( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the shader program\n" );
		return BIT_ERROR;
	}

	// Attach the shaders
	if( pShaderProgram->AttachShaders( pVertexShader ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the vertex shader\n" );
		return BIT_ERROR;
	}
	if( pShaderProgram->AttachShaders( pFragmentShader ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the fragment shader\n" );
		return BIT_ERROR;
	}

	// Set attribute locations
	pShaderProgram->SetAttributeLocation( "Position", 0 );
	pShaderProgram->SetAttributeLocation( "Texture", 1 );
	
	// Link the shaders
	if( pShaderProgram->Link( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not link the shader program\n" );
		return BIT_ERROR;
	}

	// Set uniforms
	Bit::Matrix4x4 ProjectionMatrix;
	Bit::Matrix4x4 ViewMatrix;
	ProjectionMatrix.Orthographic( 0.0f, WindowSize.x, 0.0f, WindowSize.y, -1.0f, 1.0f );
	ViewMatrix.Identity( );

	// Bind and finally set the uniforms
	pShaderProgram->Bind( );
	pShaderProgram->SetUniform1i( "Texture", 0 );
	pShaderProgram->SetUniformMatrix4x4f( "ProjectionMatrix", ProjectionMatrix );
	pShaderProgram->SetUniformMatrix4x4f( "ViewMatrix", ViewMatrix );
	pShaderProgram->Unbind( );

	return BIT_OK;
}
