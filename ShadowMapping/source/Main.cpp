#include <Bit/Window/Window.hpp>
#include <Bit/Graphics/GraphicDevice.hpp>
#include <Bit/Graphics/Texture.hpp>
#include <Bit/Graphics/Framebuffer.hpp>
#include <Bit/Graphics/ModelOBJ.hpp>
#include <Bit/Graphics/ShaderProgram.hpp>
#include <Bit/System/Timer.hpp>
#include <Bit/System.hpp>
#include <Bit/System/Vector3.hpp>
#include <Bit/System/MatrixManager.hpp>
#include <Bit/System/ResourceManager.hpp>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>
#include <Camera.hpp>

// Window/graphic device
Bit::Window * pWindow = BIT_NULL;
Bit::GraphicDevice * pGraphicDevice = BIT_NULL;

// Level variables
const std::string LevelModelPath = "../../../Data/Level.obj";
Bit::Model * pLevelModel = BIT_NULL;
Bit::Texture * pLevelColorTexture = BIT_NULL;
Bit::Texture * pLevelDepthTexture = BIT_NULL;
Bit::Framebuffer * pLevelFramebuffer = BIT_NULL;
Bit::ShaderProgram * pLevelShaderProgram = BIT_NULL;
Bit::Shader * pLevelVertexShader = BIT_NULL;
Bit::Shader * pLevelFragmentShader = BIT_NULL;

// Camera variables
Camera Camera;
Bit::Vector2_si32 MousePosition( 0, 0 );
Bit::Vector2_si32 MouseLockPosition( 500, 500 );

// Fullscreen data
Bit::VertexObject * pFullscreenVertexObject = BIT_NULL;
Bit::ShaderProgram * pFullscreenShaderProgram = BIT_NULL;
Bit::Shader * pFullscreenVertexShader = BIT_NULL;
Bit::Shader * pFullscreenFragmentShader = BIT_NULL;

// Framebuffer/renderbuffer/shadow data
Bit::Texture * pShadowDepthTexture = BIT_NULL;
Bit::Framebuffer * pShadowFramebuffer = BIT_NULL;
Bit::ShaderProgram * pShadowShaderProgram = BIT_NULL;
Bit::Shader * pShadowVertexShader = BIT_NULL;
Bit::Shader * pShadowFragmentShader = BIT_NULL;
Bit::Vector3_f32 LightPosition( 24.0f, 11.0f, 9.0f );
Bit::Vector3_f32 LightDirection( -0.817f, -0.508f, -0.271f );
Bit::Matrix4x4 ShadowViewMatrix;
Bit::Matrix4x4 BiasMatrix;

// Setting varialbes
const Bit::Vector2_ui32 WindowSize( 1024, 768 );
BIT_BOOL UseNormalMapping = BIT_TRUE;


// Global functions
int CloseApplication( const int p_Code );
void InitializeCamera( );
BIT_UINT32 CreateWindow( );
BIT_UINT32 CreateGraphicDevice( );
BIT_UINT32 LoadMatrices( );
BIT_UINT32 LoadLevelData( );
BIT_UINT32 LoadFullscreenData( );
BIT_UINT32 LoadShadowData( );
BIT_UINT32 InitializeShadowMap( );
void Render( );

// Main function
int main( int argc, char ** argv )
{
	// Initialize the memory leak detector for Win32 only (ignored by default in linux)
	bitInitMemoryLeak( BIT_NULL );

	// Setting the absolute path in order to read files.
	Bit::SetAbsolutePath( argv[ 0 ] );

	// Initialize the camera
	InitializeCamera( );

	// Initialize the application
	if( CreateWindow( ) != BIT_OK ||
		CreateGraphicDevice( ) != BIT_OK ||
		LoadMatrices( ) != BIT_OK ||
		LoadLevelData( ) != BIT_OK ||
		LoadFullscreenData( ) != BIT_OK ||
		LoadShadowData( ) != BIT_OK ||
		InitializeShadowMap( ) != BIT_OK )
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

		// Do evenets
		pWindow->Update( );

		// Mouse lock
		if( pWindow->IsFocused( ) )
		{
			MousePosition = pWindow->GetCursorScreenPosition(  );
			pWindow->SetCursorPosition( MouseLockPosition );
		}

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
							Camera.MoveForwards( );
						}
						break;
						case Bit::Keyboard::Key_S:
						{
							Camera.MoveBackwards( );
						}
						break;
						case Bit::Keyboard::Key_A:
						{
							Camera.MoveLeft( );
						}
						break;
						case Bit::Keyboard::Key_D:
						{
							Camera.MoveRight( );
						}
						break;
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

							/*// Bind and update the uniform
							pShaderProgram_Model->Bind( );
							pShaderProgram_Model->SetUniform1i( "UseNormalMapping", UseNormalMapping );
							pShaderProgram_Model->Unbind( );*/
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
				case Bit::Event::KeyReleased:
				{
					bitTrace( "[Event] Key Released: %i\n", Event.Key );
				}
				break;
				case Bit::Event::MouseMoved:
				{
					continue;
				}
				break;
				case Bit::Event::MouseButtonPressed:
				{
					continue;
				}
				break;
				case Bit::Event::MouseButtonReleased:
				{
					continue;
				}
				break;
				default:
					break;
			}
		}


		// Update the camera angles
		Bit::Vector2_f32 CameraDiffs;
		CameraDiffs.x = (BIT_FLOAT32)MousePosition.x - (BIT_FLOAT32)MouseLockPosition.x;
		CameraDiffs.y = (BIT_FLOAT32)MousePosition.y - (BIT_FLOAT32)MouseLockPosition.y;
		if( CameraDiffs.y > 0.0f )
		{
			Camera.RotateUp( abs( CameraDiffs.y ) );
		}
		else if( CameraDiffs.y < 0.0f )
		{
			Camera.RotateDown( abs( CameraDiffs.y ) );
		}
		if( CameraDiffs.x > 0.0f  )
		{
			Camera.RotateRight( abs( CameraDiffs.x ) );
		}
		else if( CameraDiffs.x < 0.0f )
		{
			Camera.RotateLeft( abs( CameraDiffs.x ) );
		}


		// ///////////////////////////////////////////////////
		// Render the level to the level framebuffer
		pLevelFramebuffer->Bind( );
		pGraphicDevice->EnableDepthTest( );
		pGraphicDevice->ClearColor( );
		pGraphicDevice->ClearDepth( );

		// Bind the level model shader program
		pLevelShaderProgram->Bind( );
		pShadowDepthTexture->Bind( 0 );

		// Update the camera if needed
		if( Camera.Update( DeltaTime ) )
		{
			pLevelShaderProgram->SetUniformMatrix4x4f( "ViewMatrix", Camera.GetMatrix( ) );
		}

		// Render the model
		pLevelModel->Render( Bit::VertexObject::RenderMode_Triangles );

		// Unbind the level model shader program
		pLevelShaderProgram->Unbind( );


		// ///////////////////////////////////////////////////
		// Render the fullscreen quad
		pGraphicDevice->BindDefaultFramebuffer( );
		pGraphicDevice->DisableDepthTest( );
		pGraphicDevice->ClearColor( );

		// Bind the fullscreen shader program
		pFullscreenShaderProgram->Bind( );
		pLevelColorTexture->Bind( 0 );
		pFullscreenVertexObject->Render( Bit::VertexObject::RenderMode_Triangles );
		pFullscreenShaderProgram->Unbind( );

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


	if( pShadowDepthTexture )
	{
		delete pShadowDepthTexture;
		pShadowDepthTexture = BIT_NULL;
	}

	if( pShadowFramebuffer )
	{
		delete pShadowFramebuffer;
		pShadowFramebuffer = BIT_NULL;
	}

	if( pShadowShaderProgram )
	{
		delete pShadowShaderProgram;
		pShadowShaderProgram = BIT_NULL;
	}

	if( pShadowVertexShader )
	{
		delete pShadowVertexShader;
		pShadowVertexShader = BIT_NULL;
	}

	if( pShadowFragmentShader )
	{
		delete pShadowFragmentShader;
		pShadowFragmentShader = BIT_NULL;
	}

	if( pFullscreenShaderProgram )
	{
		delete pFullscreenShaderProgram;
		pFullscreenShaderProgram = BIT_NULL;
	}

	if( pFullscreenVertexShader )
	{
		delete pFullscreenVertexShader;
		pFullscreenVertexShader = BIT_NULL;
	}

	if( pFullscreenFragmentShader )
	{
		delete pFullscreenFragmentShader;
		pFullscreenFragmentShader = BIT_NULL;
	}

	if( pFullscreenVertexObject )
	{
		delete pFullscreenVertexObject;
		pFullscreenVertexObject = BIT_NULL;
	}

	if( pLevelFragmentShader )
	{
		delete pLevelFragmentShader;
		pLevelFragmentShader = BIT_NULL;
	}

	if( pLevelVertexShader )
	{
		delete pLevelVertexShader;
		pLevelVertexShader = BIT_NULL;
	}

	if( pLevelShaderProgram )
	{
		delete pLevelShaderProgram;
		pLevelShaderProgram = BIT_NULL;
	}

	if( pLevelColorTexture )
	{
		delete pLevelColorTexture;
		pLevelColorTexture = BIT_NULL;
	}

	if( pLevelDepthTexture )
	{
		delete pLevelDepthTexture;
		pLevelDepthTexture = BIT_NULL;
	}

	if( pLevelFramebuffer )
	{
		delete pLevelFramebuffer;
		pLevelFramebuffer = BIT_NULL;
	}

	if( pLevelModel )
	{
		delete pLevelModel;
		pLevelModel = BIT_NULL;
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

	// Calculate the mouse lock position
	Bit::Vector2_si32 WindowPosition = pWindow->GetPosition( );
	MouseLockPosition = WindowPosition + ( pWindow->GetSize( ) / 2 );

	// Change the window's title
	pWindow->SetTitle( "Cool. We can now change the window title. Testing swedish characters: åäö ÅÄÖ" );
	pWindow->ShowCursor( BIT_FALSE );
	pWindow->SetCursorPosition( MouseLockPosition );

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
	//pGraphicDevice->EnableFaceCulling( Bit::GraphicDevice::Culling_BackFace );
	pGraphicDevice->DisableFaceCulling( );

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

BIT_UINT32 LoadMatrices( )
{
	// Initialize the matrix manager data

	// Projection
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_Projection );
	Bit::MatrixManager::LoadPerspective( 45.0f,(BIT_FLOAT32)WindowSize.x / (BIT_FLOAT32)WindowSize.y, 2.0f, 50.0f );

	// View matrix
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_View );
	Bit::MatrixManager::LoadIdentity( );

	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_Model );
	Bit::MatrixManager::LoadIdentity( );

	// Bias matrix
	BiasMatrix.Identity( );
	BiasMatrix.Translate( 0.5f, 0.5f, 0.5f );
	BiasMatrix.m[ 0 ] = 0.5f;
	BiasMatrix.m[ 5 ] = 0.5f;
	BiasMatrix.m[ 10 ] = 0.5f;

	// Shadow view matrix
	ShadowViewMatrix.Identity( );
	ShadowViewMatrix.LookAt( LightPosition, LightDirection, Bit::Vector3_f32( 0.0f, 1.0f, 0.0f ) );

	// Initialize the camera
	InitializeCamera( );



	return BIT_OK;
}

void InitializeCamera( )
{
	Camera.SetPosition( LightPosition );
	Camera.SetDirection( LightDirection );
	Camera.SetMovementSpeed( 40.0f );
	Camera.SetEyeSpeed( 15.0f );
	Camera.UpdateMatrix( );

	/*
		Pos:  23.960457 12.673248 8.509544
		Dir: -0.817315 -0.508349 -0.271252
	*/

	// Set the matrix to the matrix manager
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_View );
	Bit::MatrixManager::SetMatrix( Camera.GetMatrix( ) );

}


BIT_UINT32 LoadLevelData( )
{


	// Level model

	// Create the level model
	if( ( pLevelModel = pGraphicDevice->CreateModel( Bit::Model::Model_OBJ ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the level model\n" );
	}

	// Read the model file
	BIT_UINT32 Status = BIT_OK;
	if( ( Status = pLevelModel->ReadFile( Bit::GetAbsolutePath( LevelModelPath ).c_str( ) ) ) != BIT_OK )
	{
		if( Status == BIT_ERROR_OPEN_FILE )
		{
			bitTrace( "[Error] Can not open the file\n" );
		}
		else
		{
			bitTrace( "[Error] Can not read the level model file\n" );
		}

		return BIT_ERROR;
	}

	// Load the model
	BIT_UINT32 ModelVerteBits = Bit::VertexObject::Vertex_Position | Bit::VertexObject::Vertex_Normal;
	// Set The texture filters
	Bit::Texture::eFilter TextureFilters[ ] =
	{
		Bit::Texture::Filter_Min, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_Mag, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_None, Bit::Texture::Filter_None
	};
	if( pLevelModel->Load( ModelVerteBits, TextureFilters, BIT_TRUE ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the level model\n" );
		return BIT_ERROR;
	}



	// Level textures / framebuffer

	if( ( pLevelColorTexture = pGraphicDevice->CreateTexture( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the level color texture\n" );
		return BIT_ERROR;
	}
	if( ( pLevelDepthTexture = pGraphicDevice->CreateTexture( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the level depth texture\n" );
		return BIT_ERROR;
	}


	// Load the level textures
	if( pLevelColorTexture->Load( WindowSize, BIT_RGB, BIT_RGB, BIT_TYPE_UCHAR8, BIT_NULL ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the level color texture\n" );
		return BIT_ERROR;
	}
	if( pLevelDepthTexture->Load( WindowSize, BIT_DEPTH, BIT_DEPTH, BIT_TYPE_FLOAT32, BIT_NULL ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the level depth texture\n" );
		return BIT_ERROR;
	}

	if( pLevelColorTexture->SetFilters( TextureFilters ) != BIT_OK )
	{
		bitTrace( "[Error] Can not set the level color texture filters\n" );
		return BIT_ERROR;
	}
	if( pLevelDepthTexture->SetFilters( TextureFilters ) != BIT_OK )
	{
		bitTrace( "[Error] Can not set the level depth texture filters\n" );
		return BIT_ERROR;
	}



	// Create the shadow framebuffer
	if( ( pLevelFramebuffer = pGraphicDevice->CreateFramebuffer( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the level framebuffer\n" );
		return BIT_ERROR;
	}

	// Attach the depth shadow texture to the framebuffer
	if( pLevelFramebuffer->Attach( pLevelColorTexture ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the color texture to the level framebuffer\n" );
		return BIT_ERROR;
	}
	if( pLevelFramebuffer->Attach( pLevelDepthTexture ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the depth texture to the level framebuffer\n" );
		return BIT_ERROR;
	}


	// Level shaders

	// Shader sources
	static const std::string VertexSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 Position; \n"
		"in vec3 Normal; \n"

		"out vec3 out_Position; \n"
		"out vec3 out_Normal; \n"

		"uniform mat4 BiasMatrix; \n"
		"uniform mat4 ProjectionMatrix; \n"
		"uniform mat4 ViewMatrix; \n"
		"uniform mat4 ShadowViewMatrix; \n"

		// Shadow data
		"out vec4 LightVertexPosition; \n"

		"void main(void) \n"
		"{ \n"

		// Set some out values
		"	out_Position = Position.xyz; \n"
		"	out_Normal = normalize( Normal ); \n"

		// Set position and shadow light position
		"	LightVertexPosition = BiasMatrix * ProjectionMatrix * ShadowViewMatrix * vec4( Position, 1.0 ); \n"
		"	gl_Position = ProjectionMatrix * ViewMatrix * vec4( Position, 1.0 ); \n"

		"} \n";

	static const std::string FragmentSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 out_Position; \n"
		"in vec3 out_Normal; \n"
		"out vec4 out_Color; \n"

		"uniform vec3 LightPosition; \n"

		// Shadow data
		"uniform sampler2D ShadowTexture; \n"
		"in vec4 LightVertexPosition; \n"


		"void main(void) \n"
		"{ \n"
		// Calculate the lighting

		"	vec4 LightVertexPosition2 = LightVertexPosition; \n"
		"	LightVertexPosition2 /= LightVertexPosition2.w; \n"

		"	float ShadowValue = 0.0; \n"

		"	for( float x = -0.0005; x <= 0.0005; x += 0.0001 ) \n"
		"	{ \n"
		"		for( float y = -0.0005; y <= 0.0005; y += 0.0001 ) \n"
		"		{ \n"

		"			if( texture2D( ShadowTexture, LightVertexPosition2.xy + vec2( x, y ) ).r >= LightVertexPosition2.z ) \n"
		"			{ \n"
		"				ShadowValue += 1.0; \n"
		"			} \n"

		"		} \n"

		"	} \n"

		"	ShadowValue /= 100.0; \n"


		"	vec3 LightDirection = normalize( vec3( LightPosition - out_Position ) ); \n"
		"	float Light = max( dot( out_Normal, LightDirection ) , 0.0f ); \n"
		"	vec4 LightVector = vec4( Light, Light, Light, 1.0 ); \n"
		// Set the output color
		"	out_Color = vec4( ShadowValue, ShadowValue, ShadowValue, 1.0 ) * LightVector; \n"
		"} \n";

	// Load the shaders
	// Create the vertex and fragment shaders
	if( ( pLevelVertexShader = pGraphicDevice->CreateShader( Bit::Shader::Vertex ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the level vertex shader\n" );
		return BIT_ERROR;
	}
	if( ( pLevelFragmentShader = pGraphicDevice->CreateShader( Bit::Shader::Fragment ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the level vertex shader\n" );
		return BIT_ERROR;
	}

	// Set the sources
	pLevelVertexShader->SetSource( VertexSource );
	pLevelFragmentShader->SetSource( FragmentSource );

	// Compile the shaders
	if( pLevelVertexShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the level vertex shader\n" );
		return BIT_ERROR;
	}
	if( pLevelFragmentShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the level fragment shader\n" );
		return BIT_ERROR;
	}


	// Create the shader program
	if( ( pLevelShaderProgram = pGraphicDevice->CreateShaderProgram( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the level shader program\n" );
		return BIT_ERROR;
	}

	// Attach the shaders
	if( pLevelShaderProgram->AttachShaders( pLevelVertexShader ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the level vertex shader\n" );
		return BIT_ERROR;
	}
	if( pLevelShaderProgram->AttachShaders( pLevelFragmentShader ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the level fragment shader\n" );
		return BIT_ERROR;
	}

	// Set attribute locations
	pLevelShaderProgram->SetAttributeLocation( "Position", 0 );
	pLevelShaderProgram->SetAttributeLocation( "Normal", 1 );


	// Link the shaders
	if( pLevelShaderProgram->Link( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not link the level shader program\n" );
		return BIT_ERROR;
	}

	// Set uniforms
	pLevelShaderProgram->Bind( );

	pLevelShaderProgram->SetUniformMatrix4x4f( "BiasMatrix", BiasMatrix );
	pLevelShaderProgram->SetUniformMatrix4x4f( "ProjectionMatrix",
		Bit::MatrixManager::GetMatrix( Bit::MatrixManager::Mode_Projection ) );
	pLevelShaderProgram->SetUniformMatrix4x4f( "ViewMatrix",
		Bit::MatrixManager::GetMatrix( Bit::MatrixManager::Mode_View ) );
	pLevelShaderProgram->SetUniformMatrix4x4f( "ShadowViewMatrix", ShadowViewMatrix );
	pLevelShaderProgram->SetUniform1i( "ShadowTexture", 0 );
	pLevelShaderProgram->SetUniform3f( "LightPosition", LightPosition.x, LightPosition.y, LightPosition.z );

	pLevelShaderProgram->Unbind( );

	return BIT_OK;
}

BIT_UINT32 LoadFullscreenData( )
{
	// Fullscreen vertex object

	// Create the fulscreen vertex object
	if( ( pFullscreenVertexObject = pGraphicDevice->CreateVertexObject( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the fullscreen vertex object\n" );
		return BIT_ERROR;
	}


	BIT_FLOAT32 VertexPositions[ 18 ] =
	{
		0.0f, 0.0f, 0.0f,
		static_cast<const BIT_FLOAT32>(WindowSize.x), 0.0f, 0.0f,
		static_cast<const BIT_FLOAT32>(WindowSize.x), static_cast<const BIT_FLOAT32>(WindowSize.y), 0.0f,

		0.0f, 0.0f, 0.0f,
		static_cast<const BIT_FLOAT32>(WindowSize.x), static_cast<const BIT_FLOAT32>(WindowSize.y), 0.0f,
		0.0f, static_cast<const BIT_FLOAT32>(WindowSize.y), 0.0f
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


	// Load the fullscreen vertex object
	/*if( pFullscreenVertexObject->LoadFullscreenQuad( WindowSize ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the fullscreen vertex object\n" );
		return BIT_ERROR;
	}*/



	// Fullscreen shaders shaders

	// Shader sources
	static const std::string VertexSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 Position; \n"
		"in vec2 Texture; \n"
		"out vec2 out_Texture; \n"

		"uniform mat4 ProjectionMatrix; \n"

		"void main(void) \n"
		"{ \n"

		// Set some out values
		"	out_Texture = Texture; \n"

		// Set the output position
		"	gl_Position = ProjectionMatrix * vec4( Position, 1.0 ); \n"

		"} \n";

	static const std::string FragmentSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec2 out_Texture; \n"
		"out vec4 out_Color; \n"
		"uniform sampler2D ColorTexture; \n"

		"void main(void) \n"
		"{ \n"
		// Calculate the lighting
		"	vec4 TextureColor = texture2D( ColorTexture, out_Texture ); \n"
		// Set the output color
		"	out_Color.xyz = TextureColor.xyz; \n"
		"	out_Color.a = 1.0; \n"
		"} \n";

	// Load the shaders
	// Create the vertex and fragment shaders
	if( ( pFullscreenVertexShader = pGraphicDevice->CreateShader( Bit::Shader::Vertex ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the fullscreen vertex shader\n" );
		return BIT_ERROR;
	}
	if( ( pFullscreenFragmentShader = pGraphicDevice->CreateShader( Bit::Shader::Fragment ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the fullscreen vertex shader\n" );
		return BIT_ERROR;
	}

	// Set the sources
	pFullscreenVertexShader->SetSource( VertexSource );
	pFullscreenFragmentShader->SetSource( FragmentSource );

	// Compile the shaders
	if( pFullscreenVertexShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the fullscreen vertex shader\n" );
		return BIT_ERROR;
	}
	if( pFullscreenFragmentShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the fullscreen fragment shader\n" );
		return BIT_ERROR;
	}


	// Create the shader program
	if( ( pFullscreenShaderProgram = pGraphicDevice->CreateShaderProgram( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the fullscreen shader program\n" );
		return BIT_ERROR;
	}

	// Attach the shaders
	if( pFullscreenShaderProgram->AttachShaders( pFullscreenVertexShader ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the fullscreen vertex shader\n" );
		return BIT_ERROR;
	}
	if( pFullscreenShaderProgram->AttachShaders( pFullscreenFragmentShader ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the fullscreen fragment shader\n" );
		return BIT_ERROR;
	}

	// Set attribute locations
	pFullscreenShaderProgram->SetAttributeLocation( "Position", 0 );
	pFullscreenShaderProgram->SetAttributeLocation( "Texture", 1 );


	// Link the shaders
	if( pFullscreenShaderProgram->Link( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not link the fullscreen shader program\n" );
		return BIT_ERROR;
	}

	// Set uniforms
	pFullscreenShaderProgram->Bind( );

	Bit::Matrix4x4 OrthographicMatrix;
	OrthographicMatrix.Orthographic( 0.0f, WindowSize.x, 0.0f, WindowSize.y, -1.0f, 1.0f );

	pFullscreenShaderProgram->SetUniformMatrix4x4f( "ProjectionMatrix", OrthographicMatrix );
	pFullscreenShaderProgram->SetUniform1i( "ColorTexture", 0 );

	pFullscreenShaderProgram->Unbind( );

	return BIT_OK;
}

BIT_UINT32 LoadShadowData( )
{
	// Shadow texture

	// Create the shadow texture
	if( ( pShadowDepthTexture = pGraphicDevice->CreateTexture( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the shadow texture\n" );
		return BIT_ERROR;
	}

	// Load the shadow texture
	if( pShadowDepthTexture->Load( WindowSize, BIT_DEPTH, BIT_DEPTH, BIT_TYPE_FLOAT32, BIT_NULL ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the shadow texture\n" );
		return BIT_ERROR;
	}
	// Set The texture filters
	Bit::Texture::eFilter TextureFilters[ ] =
	{
		Bit::Texture::Filter_Min, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_Mag, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_None, Bit::Texture::Filter_None
	};
	if( pShadowDepthTexture->SetFilters( TextureFilters ) != BIT_OK )
	{
		bitTrace( "[Error] Can not set the shadow depth texture filters\n" );
		return BIT_ERROR;
	}



	// Shadow framebuffer

	// Create the shadow framebuffer
	if( ( pShadowFramebuffer = pGraphicDevice->CreateFramebuffer( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the shadow framebuffer\n" );
		return BIT_ERROR;
	}

	// Attach the depth shadow texture to the framebuffer
	if( pShadowFramebuffer->Attach( pShadowDepthTexture ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the shadow texture to the shadow framebuffer\n" );
		return BIT_ERROR;
	}


	// Shadow shader

	// Shader sources
	static const std::string VertexSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 Position; \n"
		"uniform mat4 ProjectionMatrix; \n"
		"uniform mat4 ViewMatrix; \n"

		"void main(void) \n"
		"{ \n"

		// Set the output position
		"	gl_Position = ProjectionMatrix * ViewMatrix * vec4( Position, 1.0 ); \n"

		"} \n";

	static const std::string FragmentSource =
		"#version 330 \n"
		"precision highp float; \n"

		"out vec4 out_Color; \n"

		"void main(void) \n"
		"{ \n"
		"	out_Color.a = 1.0; \n"
		"} \n";

	// Load the shaders
	// Create the vertex and fragment shaders
	if( ( pShadowVertexShader = pGraphicDevice->CreateShader( Bit::Shader::Vertex ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the shadow vertex shader\n" );
		return BIT_ERROR;
	}
	if( ( pShadowFragmentShader = pGraphicDevice->CreateShader( Bit::Shader::Fragment ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the shadow vertex shader\n" );
		return BIT_ERROR;
	}

	// Set the sources
	pShadowVertexShader->SetSource( VertexSource );
	pShadowFragmentShader->SetSource( FragmentSource );

	// Compile the shaders
	if( pShadowVertexShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the shadow vertex shader\n" );
		return BIT_ERROR;
	}
	if( pShadowFragmentShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not compile the shadow fragment shader\n" );
		return BIT_ERROR;
	}


	// Create the shader program
	if( ( pShadowShaderProgram = pGraphicDevice->CreateShaderProgram( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the shadow shader program\n" );
		return BIT_ERROR;
	}

	// Attach the shaders
	if( pShadowShaderProgram->AttachShaders( pShadowVertexShader ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the shadow vertex shader\n" );
		return BIT_ERROR;
	}
	if( pShadowShaderProgram->AttachShaders( pShadowFragmentShader ) != BIT_OK )
	{
		bitTrace( "[Error] Can not attach the shadow fragment shader\n" );
		return BIT_ERROR;
	}

	// Set attribute locations
	pShadowShaderProgram->SetAttributeLocation( "Position", 0 );

	// Link the shaders
	if( pShadowShaderProgram->Link( ) != BIT_OK )
	{
		bitTrace( "[Error] Can not link the shadow shader program\n" );
		return BIT_ERROR;
	}

	// Set uniforms
	pShadowShaderProgram->Bind( );
	pShadowShaderProgram->SetUniformMatrix4x4f( "ProjectionMatrix",
		Bit::MatrixManager::GetMatrix( Bit::MatrixManager::Mode_Projection ) );
	pShadowShaderProgram->SetUniformMatrix4x4f( "ViewMatrix", ShadowViewMatrix );
	pShadowShaderProgram->Unbind( );



	return BIT_OK;
}

BIT_UINT32 InitializeShadowMap( )
{
	// Bind the shadow framebuffer
	pShadowFramebuffer->Bind( );
	pGraphicDevice->ClearDepth( );
	pGraphicDevice->EnableFaceCulling( Bit::GraphicDevice::Culling_FrontFace );

	pShadowShaderProgram->Bind( );

	// Render the level model
	pLevelModel->Render( Bit::VertexObject::RenderMode_Triangles );

	pShadowShaderProgram->Unbind( );

	// Finally unbind the shadow framebuffer
	pShadowFramebuffer->Unbind( );

	pGraphicDevice->DisableFaceCulling( );

	return BIT_OK;
}
