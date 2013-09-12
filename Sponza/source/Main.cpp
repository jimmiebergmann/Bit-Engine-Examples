#include <Bit/System.hpp>
#include <Bit/Window/Window.hpp>
#include <Bit/Graphics/GraphicDevice.hpp>
#include <Bit/System/Timer.hpp>
#include <Bit/System/MatrixManager.hpp>
#include <Bit/System/ResourceManager.hpp>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>
#include <Camera.hpp>
#include <GUIManager.hpp>

// Window/graphic device
Bit::Window * pWindow = BIT_NULL;
Bit::GraphicDevice * pGraphicDevice = BIT_NULL;

// Setting varialbes
const Bit::Vector2_ui32 WindowSize( 800, 600 );
BIT_BOOL UseNormalMapping = BIT_TRUE;

// Level variables
const std::string LevelModelPath = "../../../../Sponza/sponza.obj";
Bit::Model * pLevelModel = BIT_NULL;
Bit::ShaderProgram * pShaderProgram_Model = BIT_NULL;
Bit::Shader * pVertexShader_Model = BIT_NULL;
Bit::Shader * pFragmentShader_Model = BIT_NULL;

// Camera variables
Camera ViewCamera;
Bit::Vector2_si32 MousePosition( 0, 0 );
Bit::Vector2_si32 PreviousMousePosition( 0, 0 );
BIT_BOOL HoldingDownMouse = BIT_FALSE;
const BIT_UINT32 RotateMouseButton = 1;

// GUI
GUIManager * GUI = BIT_NULL;
GUICheckbox * Checkbox1 = BIT_NULL;
GUICheckbox * Checkbox2 = BIT_NULL;
GUISlider * Slider1 = BIT_NULL;
GUISlider * Slider2 = BIT_NULL;

// Fullscreen rendering
Bit::Framebuffer * pFramebuffer = BIT_NULL;
Bit::Texture * pColorTexture = BIT_NULL;
Bit::Texture * pDepthTexture = BIT_NULL;
Bit::VertexObject * pFullscreenVertexObject = BIT_NULL;

// Post-Processing varaibles
Bit::PostProcessingBloom * pPostProcessingBloom = BIT_NULL;

// Global functions
int CloseApplication( const int p_Code );
void InitializeMatrixManager( );
void InitializeCamera( );
BIT_UINT32 CreateWindow( );
BIT_UINT32 CreateGraphicDevice( );
BIT_UINT32 CreateFullscreenRendering( );
BIT_UINT32 CreatePostProcessing( );
BIT_UINT32 CreateModel( );
BIT_UINT32 CreateModelShader( );
BIT_UINT32 CreateGUI( );

// Main function
int main( int argc, char ** argv )
{
	// Initialize the memory leak detector for Win32 only (ignored by default in linux)
	bitInitMemoryLeak( BIT_NULL );

	// Setting the absolute path in order to read files.
	Bit::SetAbsolutePath( argv[ 0 ] );

	// Initialize the matrix manager
	InitializeMatrixManager( );

	// Initialize the camera
	InitializeCamera( );

	// Initialize the application
	if( CreateWindow( ) != BIT_OK ||
		CreateGraphicDevice( ) != BIT_OK ||
		CreateFullscreenRendering( ) != BIT_OK ||
		CreatePostProcessing( ) != BIT_OK ||
		CreateModel( ) != BIT_OK  ||
		CreateModelShader( ) != BIT_OK ||
		CreateGUI( ) != BIT_OK )
	{
		return CloseApplication( 0 );
	}

	// Create a timer and run a main loop for some time
	BIT_FLOAT64 DeltaTime = 0.0f;
	Bit::Timer Timer;
	Timer.Start( );

	// Run the main loop
	while( pWindow->IsOpen( ) )
	{
		// Get the delta time;
		DeltaTime = Timer.GetLapsedTime( );
		Timer.Start( );

		//bitTrace( "FPS: %f\n", 1.0f / DeltaTime );

		// Do evenets
		pWindow->Update( );

		// Mouse lock
		/*if( pWindow->IsFocused( ) )
		{
			MousePosition = pWindow->GetCursorScreenPosition(  );
			pWindow->SetCursorPosition( MouseLockPosition );
		}*/

		Bit::Event Event;
		while( pWindow->PollEvent( Event ) )
		{
			// Swtich the event type
			switch( Event.Type )
			{
				case Bit::Event::Closed:
				{
					// Close the application
					return CloseApplication( 0 );
				}
				break;
                case Bit::Event::Moved:
				{
					continue;
				}
				break;
				case Bit::Event::Resized:
				{
					continue;
				}
				break;
				case Bit::Event::GainedFocus:
				{
					pWindow->ShowCursor( BIT_FALSE );
					continue;
				}
				break;
				case Bit::Event::LostFocus:
				{
					pWindow->ShowCursor( BIT_TRUE );
					continue;
				}
				break;
				case Bit::Event::KeyPressed:
				{
					switch( Event.Key )
					{
						// Movement keys
						case Bit::Keyboard::Key_W:
						{
							ViewCamera.MoveForwards( );
						}
						break;
						case Bit::Keyboard::Key_S:
						{
							ViewCamera.MoveBackwards( );
						}
						break;
						case Bit::Keyboard::Key_A:
						{
							ViewCamera.MoveLeft( );
						}
						break;
						case Bit::Keyboard::Key_D:
						{
							ViewCamera.MoveRight( );
						}
						break;
					}

				}
				break;
				case Bit::Event::KeyJustPressed:
				{
					switch( Event.Key )
					{
						// Culling
						case Bit::Keyboard::Key_Z:
						{
							pGraphicDevice->EnableFaceCulling( Bit::GraphicDevice::Culling_BackFace );
						}
						break;
						case Bit::Keyboard::Key_X:
						{
							pGraphicDevice->EnableFaceCulling( Bit::GraphicDevice::Culling_FrontFace );
						}
						break;
						case Bit::Keyboard::Key_C:
						{
							pGraphicDevice->DisableFaceCulling( );
						}
						break;
						// Mouse visibility
						case Bit::Keyboard::Key_V:
						{
							pWindow->ShowCursor( BIT_TRUE );
						}
						break;
						case Bit::Keyboard::Key_B:
						{
							pWindow->ShowCursor( BIT_FALSE );
						}
						break;
						case Bit::Keyboard::Key_M:
						{
							// Flip the flag
							UseNormalMapping = !UseNormalMapping;

							// Bind and update the uniform
							pShaderProgram_Model->Bind( );
							pShaderProgram_Model->SetUniform1i( "UseNormalMapping", UseNormalMapping );
							pShaderProgram_Model->Unbind( );
						}
						break;

						// Exit keys
						case Bit::Keyboard::Key_Escape:
						{
							return CloseApplication( 0 );
						}
						break;
					}
				}
				break;
				case Bit::Event::KeyJustReleased:
				{
					bitTrace( "[Event] Key Released: %i\n", Event.Key );
				}
				break;
				case Bit::Event::MouseMoved:
				{
					// Update the GUI
					GUI->Update( Event );

					// Update the mouse position if we are holding down the mouse button
					if( HoldingDownMouse )
					{
						MousePosition = Event.MousePosition;
					}
				}
				break;
				case Bit::Event::ButtonJustPressed:
				{
					// Update the GUI
					GUI->Update( Event );

					if( Event.Button == RotateMouseButton )
					{
						// Set the mouse position
						MousePosition = Event.MousePosition;

						// Set the previous mouse position as well if we just pressed the mouse button
						if( !HoldingDownMouse )
						{
						    HoldingDownMouse = BIT_TRUE;
							PreviousMousePosition = Event.MousePosition;
						}
					}

				}
				break;
				case Bit::Event::ButtonJustReleased:
				{
					// Update the GUI
					GUI->Update( Event );

					if( Event.Button == RotateMouseButton )
					{
						HoldingDownMouse = BIT_FALSE;
					}
				}
				break;
				default:
					break;
			}
		}

		// Is the mouse button pressed?
		if( HoldingDownMouse )
		{
			// Calculate the mouse difference from the last update
			Bit::Vector2_si32 MouseDiff = MousePosition - PreviousMousePosition;
			PreviousMousePosition = MousePosition;

			// Rotate the camera
			ViewCamera.Rotate( MouseDiff );
		}


		// Update the camera angles
	/*	Bit::Vector2_f32 CameraDiffs;
		CameraDiffs.x = (BIT_FLOAT32)MousePosition.x - (BIT_FLOAT32)MouseLockPosition.x;
		CameraDiffs.y = (BIT_FLOAT32)MousePosition.y - (BIT_FLOAT32)MouseLockPosition.y;
		if( CameraDiffs.y > 0.0f )
		{
			ViewCamera.RotateUp( abs( CameraDiffs.y ) );
		}
		else if( CameraDiffs.y < 0.0f )
		{
			ViewCamera.RotateDown( abs( CameraDiffs.y ) );
		}
		if( CameraDiffs.x > 0.0f  )
		{
			ViewCamera.RotateRight( abs( CameraDiffs.x ) );
		}
		else if( CameraDiffs.x < 0.0f )
		{
			ViewCamera.RotateLeft( abs( CameraDiffs.x ) );
		}*/

		// Bind the framebuffer
		pFramebuffer->Bind( );
		pGraphicDevice->EnableDepthTest( );

		// Clear the buffers
		pGraphicDevice->ClearColor( );
		pGraphicDevice->ClearDepth( );

		// Bind the model shader program
		pShaderProgram_Model->Bind( );

		// Update the camera if needed
		if( ViewCamera.Update( DeltaTime ) )
		{
			pShaderProgram_Model->SetUniformMatrix4x4f( "ViewMatrix", ViewCamera.GetMatrix( ) );
		}

		// Render the model
		pLevelModel->Render( Bit::VertexObject::RenderMode_Triangles );

		// Unbind the shader program
		pShaderProgram_Model->Unbind( );

		// Unbind the framebuffer (binding the standard framebuffer)
		pFramebuffer->Unbind( );

		// Post-processing
		pGraphicDevice->DisableDepthTest( );
		pGraphicDevice->ClearColor( );

		// Apply bloom
		pPostProcessingBloom->Process( );

		// Render the GUI
		//
		GUI->Render( );

		// Present the buffers
		pGraphicDevice->Present( );
	}

	// We are done
	bitTrace( "Closing the program.\n" );
	return CloseApplication( 0 );
}

int CloseApplication( const int p_Code )
{
	// Release the resource manager
	Bit::ResourceManager::Release( );

	if( Slider2 )
	{
		delete Slider2;
		Slider2 = BIT_NULL;
	}

	if( Slider1 )
	{
		delete Slider1;
		Slider1 = BIT_NULL;
	}

	if( Checkbox2 )
	{
		delete Checkbox2;
		Checkbox2 = BIT_NULL;
	}

	if( Checkbox1 )
	{
		delete Checkbox1;
		Checkbox1 = BIT_NULL;
	}

	if( GUI )
	{
		delete GUI;
		GUI = BIT_NULL;
	}


	if( pLevelModel )
	{
		delete pLevelModel;
		pLevelModel = BIT_NULL;
	}

	if( pShaderProgram_Model )
	{
		delete pShaderProgram_Model;
		pShaderProgram_Model = BIT_NULL;
	}

	if( pVertexShader_Model )
	{
		delete pVertexShader_Model;
		pVertexShader_Model = BIT_NULL;
	}

	if( pFragmentShader_Model )
	{
		delete pFragmentShader_Model;
		pFragmentShader_Model = BIT_NULL;
	}

	if( pPostProcessingBloom )
	{
		delete pPostProcessingBloom;
		pPostProcessingBloom = BIT_NULL;
	}

	if( pFramebuffer )
	{
		delete pFramebuffer;
		pFramebuffer = BIT_NULL;
	}

	if( pColorTexture )
	{
		delete pColorTexture;
		pColorTexture = BIT_NULL;
	}

	if( pDepthTexture )
	{
		delete pDepthTexture;
		pDepthTexture = BIT_NULL;
	}

	if( pFullscreenVertexObject )
	{
		delete pFullscreenVertexObject;
		pFullscreenVertexObject = BIT_NULL;
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

void InitializeMatrixManager( )
{
	// Projection
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_Projection );
	Bit::MatrixManager::LoadPerspective( 45.0f,(BIT_FLOAT32)WindowSize.x / (BIT_FLOAT32)WindowSize.y, 2.0f, 4000.0f );

	// Model view
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_View );
	Bit::MatrixManager::LoadIdentity( );

	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_Model );
	Bit::MatrixManager::LoadIdentity( );
}

void InitializeCamera( )
{
	ViewCamera.SetPosition( Bit::Vector3_f32( -900.0f, 600.0f, -200.0f ) );
	ViewCamera.SetDirection( Bit::Vector3_f32( 1.0f, -0.5f, 0.4f ).Normal( ) );
	ViewCamera.SetMovementSpeed( 1000.0f );
	ViewCamera.SetRotationSpeed( 4.0f );
	ViewCamera.SetRotationResistance( 20.0f );
	ViewCamera.SetRotationRollFactor( 0.1f );
	ViewCamera.UpdateMatrix( );

	// Set the matrix to the matrix manager
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_View );
	Bit::MatrixManager::SetMatrix( ViewCamera.GetMatrix( ) );
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

	// Calculate the mouse lock position
	//Bit::Vector2_si32 WindowPosition = pWindow->GetPosition( );
	//MouseLockPosition = WindowPosition + ( pWindow->GetSize( ) / 2 );

	// Change the window's title
	pWindow->SetTitle( "Sponza Scene" );
	//pWindow->ShowCursor( BIT_FALSE );
	//pWindow->SetCursorPosition( MouseLockPosition );

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
	pGraphicDevice->EnableFaceCulling( Bit::GraphicDevice::Culling_BackFace );

	// Initialize the resource manager
	Bit::Texture::eFilter TextureFilters[ ] =
	{
		Bit::Texture::Filter_Min, Bit::Texture::Filter_Linear,
		Bit::Texture::Filter_Mag, Bit::Texture::Filter_Linear,
		Bit::Texture::Filter_Wrap_X, Bit::Texture::Filter_Repeat,
		Bit::Texture::Filter_Wrap_Y, Bit::Texture::Filter_Repeat,
		Bit::Texture::Filter_None, Bit::Texture::Filter_None
	};

	// Initialize the resource manager
	if( Bit::ResourceManager::Initialize( pGraphicDevice, TextureFilters ) != BIT_OK )
	{
		bitTrace( "[Error] Can not initialize the resource manager\n" );
		return BIT_ERROR;
	}

	return BIT_OK;
}

BIT_UINT32 CreateFullscreenRendering( )
{
	// Create the textures
	if( ( pColorTexture = pGraphicDevice->CreateTexture( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the fullscreen color texture\n" );
		return BIT_ERROR;
	}

	if( ( pDepthTexture = pGraphicDevice->CreateTexture( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the fullscreen depth texture\n" );
		return BIT_ERROR;
	}

	// Load the textures
	if( pColorTexture->Load( WindowSize, BIT_RGB, BIT_RGB, BIT_TYPE_UCHAR8, BIT_NULL ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the fullscreen color texture\n" );
		return BIT_ERROR;
	}

	if( pDepthTexture->Load( WindowSize, BIT_DEPTH, BIT_DEPTH, BIT_TYPE_FLOAT32, BIT_NULL ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the fullscreen depth texture\n" );
		return BIT_ERROR;
	}

	// Set texture filters
	Bit::Texture::eFilter TextureFilters[ ] =
	{
		Bit::Texture::Filter_Min, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_Mag, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_Wrap_X, Bit::Texture::Filter_Repeat,
		Bit::Texture::Filter_Wrap_Y, Bit::Texture::Filter_Repeat,
		Bit::Texture::Filter_None, Bit::Texture::Filter_None
	};

	if( pColorTexture->SetFilters( TextureFilters ) != BIT_OK )
	{
		bitTrace( "[Error] Can not set the fullscreen color texture filers\n" );
		return BIT_ERROR;
	}

	if( pDepthTexture->SetFilters( TextureFilters ) != BIT_OK )
	{
		bitTrace( "[Error] Can not set the fullscreen depth texture filers\n" );
		return BIT_ERROR;
	}


	// Create the frame buffer
	if( ( pFramebuffer = pGraphicDevice->CreateFramebuffer( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the framebuffer texture\n" );
		return BIT_ERROR;
	}

	// Attach the texture to the framebuffer
	if( pFramebuffer->Attach( pColorTexture ) != BIT_OK )
	{
		bitTrace( "[Error] Can not create attach the color texture to the framebuffer\n" );
		return BIT_ERROR;
	}

	if( pFramebuffer->Attach( pDepthTexture ) != BIT_OK )
	{
		bitTrace( "[Error] Can not create attach the depth texture to the framebuffer\n" );
		return BIT_ERROR;
	}

	// Create the fullscreen vertex object
	if( ( pFullscreenVertexObject = pGraphicDevice->CreateVertexObject( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the fullscreen vertex object\n" );
		return BIT_ERROR;
	}

	// Load the fullscreen vertex object

	BIT_FLOAT32 VertexPositions[ 18 ] =
	{
		0.0f, 0.0f, 0.0f,
		static_cast<BIT_FLOAT32>(WindowSize.x), 0.0f, 0.0f,
		static_cast<BIT_FLOAT32>(WindowSize.x), static_cast<BIT_FLOAT32>(WindowSize.y), 0.0f,
		0.0f, 0.0f, 0.0f,
		static_cast<BIT_FLOAT32>(WindowSize.x), static_cast<BIT_FLOAT32>(WindowSize.y), 0.0f,
		0.0f, static_cast<BIT_FLOAT32>(WindowSize.y), 0.0f
	};

	BIT_FLOAT32 VertexTextures[ 12 ] =
	{
		0.0f, 0.0f,		1.0f, 0.0f,		1.0f, 1.0f,
		0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f
	};

	// SLOW WAY


	if( pFullscreenVertexObject->AddVertexBuffer( VertexPositions, 3, BIT_TYPE_FLOAT32 ) != BIT_OK )
	{
		bitTrace( "[Error] Can not add vertex position buffer to the fullscreen vertex object\n" );
		return BIT_ERROR;
	}
	if( pFullscreenVertexObject->AddVertexBuffer( VertexTextures, 2, BIT_TYPE_FLOAT32 ) != BIT_OK )
	{
		bitTrace( "[Error] Can not add vertex texture buffer to the fullscreen vertex object\n" );
		return BIT_ERROR;
	}

	if( pFullscreenVertexObject->Load( 2, 3 ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the fullscreen vertex object\n" );
		return BIT_ERROR;
	}
	/*if( pFullscreenVertexObject->LoadFullscreenQuad( WindowSize ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the fullscreen vertex object\n" );
		return BIT_ERROR;
	}*/

	return BIT_OK;
}

BIT_UINT32 CreatePostProcessing( )
{
	// Create the bloom effect
	if( ( pPostProcessingBloom = pGraphicDevice->CreatePostProcessingBloom( pFullscreenVertexObject, pColorTexture ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the bloom post-processing effect.\n" );
		return BIT_ERROR;
	}

	// Load the bloom effect
	if( pPostProcessingBloom->Load( 0.15f, 2, 1.0f ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the bloom post-processing effect.\n" );
		return BIT_ERROR;
	}

	return BIT_OK;
}


BIT_UINT32 CreateModel( )
{
	// Allocate the image
	if( ( pLevelModel = pGraphicDevice->CreateModel( Bit::Model::Model_OBJ ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the model\n" );
	}

	Bit::Timer Timer;
	Timer.Start( );

	BIT_UINT32 Status = BIT_OK;
	if( ( Status = pLevelModel->ReadFile( Bit::GetAbsolutePath( LevelModelPath ).c_str( ) ) ) != BIT_OK )
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

	// Load the model
	BIT_UINT32 ModelVerteBits = Bit::VertexObject::Vertex_Position | Bit::VertexObject::Vertex_Texture
		| Bit::VertexObject::Vertex_Normal | Bit::VertexObject::Vertex_Tangent | Bit::VertexObject::Vertex_Binormal;
	Bit::Texture::eFilter TextureFilters[ ] =
	{
		Bit::Texture::Filter_Min, Bit::Texture::Filter_Linear_Mipmap,
		Bit::Texture::Filter_Mag, Bit::Texture::Filter_Linear_Mipmap,
		Bit::Texture::Filter_None, Bit::Texture::Filter_None
	};
	if( pLevelModel->Load( ModelVerteBits, TextureFilters, BIT_TRUE ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the model\n" );
		return BIT_ERROR;
	}

	Timer.Stop( );
	bitTrace( "Model load time: %f ms.\n", Timer.GetTime( ) * 1000.0f );

	return BIT_OK;
}

BIT_UINT32 CreateModelShader( )
{
	// Shader sources
	static const std::string VertexSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 Position; \n"
		"in vec2 Texture; \n"
		"in vec3 Normal; \n"
		"in vec3 Tangent; \n"
		"in vec3 Binormal; \n"

		"out vec3 out_Position; \n"
		"out vec2 out_Texture; \n"
		"out vec3 out_Normal; \n"
		"out vec3 out_Tangent; \n"
		"out vec3 out_Binormal; \n"
		"out mat3 out_TangentSpace; \n"

		"uniform mat4 ProjectionMatrix; \n"
		"uniform mat4 ViewMatrix; \n"

		"void main(void) \n"
		"{ \n"

		// Set some out values
		"	vec4 NewPosition = vec4( Position, 1.0 ); \n"
		"	out_Position = NewPosition.xyz; \n"
		"	out_Texture = Texture; \n"
		"	out_Normal = normalize( Normal ); \n"
		"	out_Tangent = normalize( Tangent ); \n"
		"	out_Binormal = normalize( Binormal ); \n"

		// Calculate the tangent space matrix
		"	out_TangentSpace[ 0 ] = out_Tangent; \n"
		"	out_TangentSpace[ 1 ] = out_Binormal; \n"
		"	out_TangentSpace[ 2 ] = out_Normal; \n"

		// Set the output position
		"	gl_Position = ProjectionMatrix * ViewMatrix * NewPosition; \n"

		"} \n";

	static const std::string FragmentSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 out_Position; \n"
		"in vec2 out_Texture; \n"
		"in vec3 out_Normal; \n"
		"in vec3 out_Tangent; \n"
		"in vec3 out_Binormal; \n"
		"in vec3 out_LightVec; \n"
		"in mat3 out_TangentSpace; \n"

		"out vec4 out_Color; \n"

		"uniform sampler2D DiffuseTexture; \n"
		"uniform sampler2D NormalTexture; \n"
		"uniform vec3 LightPosition; \n"
		"uniform int UseNormalMapping; \n"


		"void main(void) \n"
		"{ \n"

		// Diffuse color map
		"	vec4 DiffuseMap = texture2D( DiffuseTexture, out_Texture ); \n"
		"	if( DiffuseMap.a == 0.0 ) { discard; } \n"

		"	vec3 Light; \n"

		// Compute the direction of the light source
		"	vec3 LightDirection = normalize( vec3( LightPosition - out_Position ) ); \n"

		// Are we using normal maps?
		"	if( UseNormalMapping == 1 ) \n"
		"	{ \n"
				// Normal color map
		"		vec4 NormalMap = texture2D( NormalTexture, out_Texture ); \n"
		"		NormalMap.y = 1.0 - NormalMap.y; \n"
		"		vec3 OldNormalDirection = 2.0 * NormalMap.rgb - 1.0; \n"

		"		vec3 NormalDirection = normalize( out_TangentSpace * OldNormalDirection ); \n"

		// Compute the light
		"		Light = vec3( max( dot( LightDirection, NormalDirection ), 0.1 ) ); \n"

		"	} \n"
		// Use normal lighting
		"	else \n"
		"	{ \n"
		"		Light = vec3( max( dot( LightDirection, out_Normal ), 0.1 ) ); \n"
		"	} \n"

		// Set the output color
		"	out_Color = DiffuseMap * vec4( Light.xyz, 1.0 ); \n"
		"} \n";

	// Load the shaders
	// Create the vertex and fragment shaders
	if( ( pVertexShader_Model = pGraphicDevice->CreateShader( Bit::Shader::Vertex ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the vertex shader\n" );
		return BIT_ERROR;
	}
	if( ( pFragmentShader_Model = pGraphicDevice->CreateShader( Bit::Shader::Fragment ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the vertex shader\n" );
		return BIT_ERROR;
	}

	// Set the sources
	pVertexShader_Model->SetSource( VertexSource );
	pFragmentShader_Model->SetSource( FragmentSource );

	// Compile the shaders
	if( pVertexShader_Model->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the vertex shader\n" );
		return BIT_ERROR;
	}
	if( pFragmentShader_Model->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the fragment shader\n" );
		return BIT_ERROR;
	}


	// Create the shader program
	if( ( pShaderProgram_Model = pGraphicDevice->CreateShaderProgram( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the shader program\n" );
		return BIT_ERROR;
	}

	// Attach the shaders
	if( pShaderProgram_Model->AttachShaders( pVertexShader_Model ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the vertex shader\n" );
		return BIT_ERROR;
	}
	if( pShaderProgram_Model->AttachShaders( pFragmentShader_Model ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the fragment shader\n" );
		return BIT_ERROR;
	}

	// Set attribute locations
	pShaderProgram_Model->SetAttributeLocation( "Position", 0 );
	pShaderProgram_Model->SetAttributeLocation( "Texture", 1 );
	pShaderProgram_Model->SetAttributeLocation( "Normal", 2 );
	pShaderProgram_Model->SetAttributeLocation( "Tangent", 3 );
	pShaderProgram_Model->SetAttributeLocation( "Binormal", 4 );

	// Link the shaders
	if( pShaderProgram_Model->Link( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not link the shader program\n" );
		return BIT_ERROR;
	}

	// Set uniforms
	pShaderProgram_Model->Bind( );
	pShaderProgram_Model->SetUniform1i( "DiffuseTexture", 0 );
	pShaderProgram_Model->SetUniform1i( "NormalTexture", 1 );
	pShaderProgram_Model->SetUniformMatrix4x4f( "ProjectionMatrix",
		Bit::MatrixManager::GetMatrix( Bit::MatrixManager::Mode_Projection ) );
	pShaderProgram_Model->SetUniformMatrix4x4f( "ViewMatrix",
		Bit::MatrixManager::GetMatrix( Bit::MatrixManager::Mode_View ) );
	pShaderProgram_Model->SetUniform3f( "LightPosition", 1.0f, 100.0f, 0.0f );
	pShaderProgram_Model->SetUniform1i( "UseNormalMapping", UseNormalMapping );
	pShaderProgram_Model->Unbind( );

	return BIT_OK;
}

BIT_UINT32 CreateGUI( )
{
	// Allocate everything
	GUI = new GUIManager( pGraphicDevice );
	Checkbox1 = new GUICheckbox( Bit::Vector2_si32( 400, 400 ), Bit::Vector2_ui32( 32, 32 ), BIT_FALSE );
	Checkbox2 = new GUICheckbox( Bit::Vector2_si32( 700, 600 ), Bit::Vector2_ui32( 16, 16 ), BIT_FALSE );

	// Load the GUI manager
	if( GUI->Load( ) )
	{
		bitTrace( "[Error] Can not load he GUI manager\n" );
		return BIT_ERROR;
	}

	// Add the elements
	GUI->Add( Checkbox1 );
	GUI->Add( Checkbox2 );

	/*

	GUIManager * GUI = BIT_NULL;
	GUICheckbox * Checkbox1 = BIT_NULL;
	GUICheckbox * Checkbox2 = BIT_NULL;
	GUISlider * Slider1 = BIT_NULL;
	GUISlider * Slider2 = BIT_NULL;


	*/



	return BIT_OK;
}
