#include <Bit/Window/Window.hpp>
#include <Bit/Graphics/GraphicDevice.hpp>
#include <Bit/Graphics/Image.hpp>
#include <Bit/Graphics/Model.hpp>
#include <Bit/System/Timer.hpp>
#include <Bit/System.hpp>
#include <Bit/System/Randomizer.hpp>
#include <Bit/System/Vector3.hpp>
#include <Bit/System/Line.hpp>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>
#include <iostream>

// Global variables
Bit::Window * pWindow = BIT_NULL;
Bit::GraphicDevice * pGraphicDevice = BIT_NULL;
Bit::Model * pModel = BIT_NULL;
Bit::VertexObject * pVertexObject = BIT_NULL;
Bit::Shader * pVertexShader = BIT_NULL;
Bit::Shader * pFragmentShader = BIT_NULL;
Bit::ShaderProgram * pShaderProgram = BIT_NULL;
Bit::Image * pImage = BIT_NULL;
Bit::Texture * pTexture = BIT_NULL;
Bit::Vector2_ui32 WindowSize( 800, 600 );

// Global functions
int CloseApplication( const int p_Code );
BIT_UINT32 CreateWindow( );
BIT_UINT32 CreateGraphicDevice( );
BIT_UINT32 CreateModel( );
BIT_UINT32 CreateImage( );
BIT_UINT32 CreateTexture( );
BIT_UINT32 CreateVertexObject( );
BIT_UINT32 CreateShaders( std::string p_Argv );
BIT_UINT32 CreateShaderProgram( );


// Main function
int main( int argc, char ** argv )
{
	// Doing some tests:
	Bit::Line Line1( Bit::Vector2_f32( 0.0f, 0.0f ), Bit::Vector2_f32( 0.0f, 100.0f ) );
	Bit::Line Line2( Bit::Vector2_f32( -100.0f, 100.0f  ), Bit::Vector2_f32( 100.0f, 100.0f  ) );
	Bit::Vector3_f32 Intersection;

	if( Line1.IntersectionLine2( Line2, Intersection ) )
	{
		bitTrace( "Intersection: %f  %f  %f\n\n\n",
			Intersection.x, Intersection.y, Intersection.z );
	}
	else
	{
		bitTrace( "Intersection: NONE\n\n\n" );
	}




	// Initialize the memory leak detector for Win32 only (ignored by default in linux)
	bitInitMemoryLeak( BIT_NULL );

	// Setting the absolute path in order to read files.
	Bit::SetAbsolutePath( argv[ 0 ] );

	// Initialize the application
	if( CreateWindow( ) != BIT_OK ||
		CreateGraphicDevice( ) != BIT_OK ||
		CreateModel( ) != BIT_OK ||
		CreateImage( ) != BIT_OK ||
		CreateTexture( ) != BIT_OK ||
		CreateVertexObject( ) != BIT_OK ||
		CreateShaders( argv[ 0 ] ) != BIT_OK ||
		CreateShaderProgram( ) != BIT_OK )
	{
		return CloseApplication( 0 );
	}


	// Create a timer and run a main loop for some time
	Bit::Timer Timer;
	Timer.Start( );

	// Run the main loop
	while( /*Timer.GetLapsedTime( ) < 10.0f &&*/ pWindow->IsOpen( ) )
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
		pShaderProgram->Bind( );

		// Update the view matrix. Rotate the model
		Bit::Matrix4x4 ViewMatrix;
		ViewMatrix.Identity( );
		ViewMatrix.Translate( 0.0f, 0.0f, -3.2f );
		ViewMatrix.RotateY( Timer.GetLapsedTime( ) * 100.0f );
		pShaderProgram->SetUniformMatrix4x4f( "ViewMatrix", ViewMatrix );

		pTexture->Bind( 0 );
		pVertexObject->Render( Bit::VertexObject::RenderMode_Triangles );
		pTexture->Unbind( );
		pShaderProgram->Unbind( );

		// Present the buffers
		pGraphicDevice->Present( );
	}

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
	if( pTexture )
	{
		delete pTexture;
		pTexture = BIT_NULL;
	}

	if( pImage )
	{
		delete pImage;
		pImage = BIT_NULL;
	}

	if( pFragmentShader )
	{
		delete pFragmentShader;
		pFragmentShader = BIT_NULL;
	}

	if( pVertexShader )
	{
		delete pVertexShader;
		pVertexShader = BIT_NULL;
	}

	if( pShaderProgram )
	{
		delete pShaderProgram;
		pShaderProgram = BIT_NULL;
	}

	if( pVertexObject )
	{
		delete pVertexObject;
		pVertexObject = BIT_NULL;
	}

	if( pModel )
	{
		delete pModel;
		pModel = BIT_NULL;
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
	BIT_UINT32 Style = Bit::Window::Style_TitleBar | Bit::Window::Style_Minimize |
		/* Bit::Window::Style_Resize | */ Bit::Window::Style_Close;

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
	pGraphicDevice->EnableTexture( );
	pGraphicDevice->EnableAlpha( );

	return BIT_OK;
}

BIT_UINT32 CreateModel( )
{
	// Allocate the image
	pModel = new Bit::Model( );

	BIT_UINT32 Status = BIT_OK;
	if( ( Status = pModel->ReadFile( Bit::GetAbsolutePath( "../../../Data/monkey.obj" ) ) ) != BIT_OK )
	{
		if( Status == BIT_ERROR_OPEN_FILE )
		{
			bitTrace( "[Error] Can not open the file\n" );
		}
		else
		{
			bitTrace( "[Error] Can not read the model file\n" );
		}

		return BIT_ERROR;
	}

	bitTrace( "Model vertex count: %i\n", pModel->GetVertexPositionCount( ) );

	return BIT_OK;
}

BIT_UINT32 CreateImage( )
{
	// Allocate the image
	pImage = new Bit::Image( );

	// Open the image via a file
	if( pImage->ReadFile( Bit::GetAbsolutePath( "../../../Data/Image.tga" ) ) != BIT_OK )
	{
		bitTrace( "[Error] Can not read the image file\n" );
		return BIT_ERROR;
	}

	// Print image information
	bitTrace( "[Note] Image: %i %i %i\n",
		pImage->GetSize( ).x, pImage->GetSize( ).y, pImage->GetDepth( ) );

	// Get the first pixel
	Bit::Pixel Pixel = pImage->GetPixel( 0 );

	bitTrace( "[Note] Pixel: %i %i %i %i\n",
		Pixel.m_R, Pixel.m_G, Pixel.m_B, Pixel.m_A );

	return BIT_OK;
}

BIT_UINT32 CreateTexture( )
{
	// Create the texture
	if( ( pTexture = pGraphicDevice->CreateTexture( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the texture\n" );
		return BIT_ERROR;
	}

	// Load the texture
	if( pTexture->Load( *pImage ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the texture\n" );
		return BIT_ERROR;
	}

	// Set texture filers
	Bit::Texture::eFilter Filters[ ] =
	{
		Bit::Texture::Filter_Min, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_Mag, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_Wrap_X, Bit::Texture::Filter_Repeat,
		Bit::Texture::Filter_Wrap_Y, Bit::Texture::Filter_Repeat,
		Bit::Texture::Filter_None, Bit::Texture::Filter_None
	};

	if( pTexture->SetFilters( Filters ) )
	{
		bitTrace( "[Error] Can not set the texture filters\n" );
		return BIT_ERROR;
	}

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

	// Load the vertex object via the model
	if( pModel->LoadVertexObject( *pVertexObject ) != BIT_OK )
	{
		bitTrace( "[Error] Can notload the vertex object\n" );
		return BIT_ERROR;
	}



	// Create vertex array
	/*BIT_FLOAT32 VertexCoordData[ 18 ] =
	{
		-1.0f, -1.0f, 0.0f,		1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 0.0f,		-1.0f, 1.0f, 0.0f
	};
	BIT_FLOAT32 VertexTexData[ 12 ] =
	{
		0, 0,	1, 0,	1, 1,
		0, 0,	1, 1,	0, 1
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
	if( pVertexObject->Load( 2, 3 ) == BIT_ERROR )
	{
		bitTrace( "[Error] Can not load the vertex object\n" );
		return BIT_ERROR;
	}*/

	// Update the tex coo buffer
	/*BIT_FLOAT32 pNewData[ 12 ] =
	{
		0, 0,	1, 0,	1, 1,
		0, 0,	1, 1,	0, 1
	};
	if( pVertexObject->UpdateVertexBuffer( 1, pNewData, 0, 12 ) == BIT_ERROR )
	{
		bitTrace( "[Error] Can not create the vertex object\n" );
		return BIT_ERROR;
	}*/

	return BIT_OK;
}

BIT_UINT32 CreateShaders( std::string p_Argv )
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
	if( pVertexShader->ReadFile( Bit::GetAbsolutePath( "../../../Data/VertexShader.txt" ) ) != BIT_OK )
	{
		bitTrace( "[Error] Can not read the vertex shader file\n" );
		return BIT_ERROR;
	}
	if( pFragmentShader->ReadFile(Bit::GetAbsolutePath( "../../../Data/FragmentShader.txt" ) ) != BIT_OK )
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
	BIT_UINT32 AttributeIndex = 0;
	if( pModel->GetVertexPositionCount( ) )
	{
		pShaderProgram->SetAttributeLocation( "Position", AttributeIndex );
		AttributeIndex++;
	}
	if( pModel->GetVertexTextureCount( ) )
	{
		pShaderProgram->SetAttributeLocation( "Texture", AttributeIndex );
		AttributeIndex++;
	}
	if( pModel->GetVertexNormalCount( ) )
	{
		pShaderProgram->SetAttributeLocation( "Normal", AttributeIndex );
		AttributeIndex++;
	}


	// Link the shaders
	if( pShaderProgram->Link( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not link the shader program\n" );
		return BIT_ERROR;
	}

	// Set uniforms
	Bit::Matrix4x4 ProjectionMatrix;
	Bit::Matrix4x4 ViewMatrix;
	ProjectionMatrix.Perspective( 45.0f,(BIT_FLOAT32)WindowSize.x / (BIT_FLOAT32)WindowSize.y,
		0.001f, 100.0f );
	ViewMatrix.Identity( );
	ViewMatrix.Translate( 0.0f, 0.0f, -3.2f );

	// Bind and finally set the uniforms
	pShaderProgram->Bind( );
	//pShaderProgram->SetUniform1i( "Texture", 0 );
	pShaderProgram->SetUniformMatrix4x4f( "ProjectionMatrix", ProjectionMatrix );
	pShaderProgram->SetUniformMatrix4x4f( "ViewMatrix", ViewMatrix );
	pShaderProgram->Unbind( );

	return BIT_OK;
}
