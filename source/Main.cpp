#include <Bit/Window/Window.hpp>
#include <Bit/Graphics/GraphicDevice.hpp>
#include <Bit/Graphics/Image.hpp>
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

// Global variables
Bit::Window * pWindow = BIT_NULL;
Bit::GraphicDevice * pGraphicDevice = BIT_NULL;
Bit::Model * pModel = BIT_NULL;
Bit::ShaderProgram * pShaderProgram_Model = BIT_NULL;
Bit::Shader * pVertexShader_Model = BIT_NULL;
Bit::Shader * pFragmentShader_Model = BIT_NULL;
Bit::Vector2_ui32 WindowSize( 1600, 1000 );
Bit::Vector2_si32 MousePosition( 0, 0 );
Bit::Vector2_si32 OldMousePosition( 0, 0 );
Camera Camera;

// Global functions
int CloseApplication( const int p_Code );
void InitializeMatrixManager( );
void InitializeCamera( );
BIT_UINT32 CreateWindow( );
BIT_UINT32 CreateGraphicDevice( );
BIT_UINT32 CreateModel( );
BIT_UINT32 CreateModelShader( );

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
		CreateModel( ) != BIT_OK  ||
		CreateModelShader( ) != BIT_OK )
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

		// Store the old mouse position
		OldMousePosition = MousePosition;

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
					continue;
				}
				break;
				case Bit::Event::LostFocus:
				{
					continue;
				}
				break;
				case Bit::Event::KeyPressed:
				{
					switch( Event.Key )
					{
						// Movement keys
						case 'W':
						{
							Camera.MoveForwards( );
						}
						break;
						case 'S':
						{
							Camera.MoveBackwards( );
						}
						break;
						case 'A':
						{
							Camera.MoveLeft( );
						}
						break;
						case 'D':
						{
							Camera.MoveRight( );
						}
						break;

						// Exit keys
						case 27:
						{
							return CloseApplication( 0 );
						}
						break;
						case 65307:
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
					MousePosition = Event.MousePosition;
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
		CameraDiffs.x = (BIT_FLOAT32)MousePosition.x - (BIT_FLOAT32)OldMousePosition.x;
		CameraDiffs.y = (BIT_FLOAT32)MousePosition.y - (BIT_FLOAT32)OldMousePosition.y;
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


		// Clear the buffers
		pGraphicDevice->ClearColor( );
		pGraphicDevice->ClearDepth( );

		// Bind the model shader program
		pShaderProgram_Model->Bind( );

		// Update the camera if needed
		if( Camera.Update( DeltaTime ) )
		{
			pShaderProgram_Model->SetUniformMatrix4x4f( "ViewMatrix", Camera.GetMatrix( ) );
		}

		// Render the model
		pModel->Render( Bit::VertexObject::RenderMode_Triangles );

		// Unbind the shader program
		pShaderProgram_Model->Unbind( );


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

	if( pModel )
	{
		delete pModel;
		pModel = BIT_NULL;
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
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_ModelView );
	Bit::MatrixManager::LoadIdentity( );
}

void InitializeCamera( )
{
	Camera.SetPosition( Bit::Vector3_f32( 0.0f, 0.0f, 0.0f ) );
	Camera.SetDirection( Bit::Vector3_f32( 0.0f, 0.0f, 1.0f ) );
	Camera.SetMovementSpeed( 1000.0f );
	Camera.SetEyeSpeed( 15.0f );
	Camera.UpdateMatrix( );

	// Set the matrix to the matrix manager
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_ModelView );
	Bit::MatrixManager::SetMatrix( Camera.GetMatrix( ) );

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

BIT_UINT32 CreateModel( )
{
	// Allocate the image
	if( ( pModel = pGraphicDevice->CreateModel( Bit::Model::Model_OBJ ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the model\n" );
	}

	Bit::Timer Timer;
	Timer.Start( );

	BIT_UINT32 Status = BIT_OK;
	if( ( Status = pModel->ReadFile( Bit::GetAbsolutePath( "../../../Data/Sponza/sponza2.obj").c_str( ) ) ) != BIT_OK )
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
	BIT_UINT32 ModelVerteBits = 0;
	Bit::Texture::eFilter TextureFilters[ ] =
	{
		Bit::Texture::Filter_None, Bit::Texture::Filter_None
	};
	if( pModel->Load( ModelVerteBits, TextureFilters ) != BIT_OK )
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
		"out vec3 out_Position; \n"
		"out vec2 out_Texture; \n"
		"out vec3 out_Normal; \n"
		
		"uniform mat4 ProjectionMatrix; \n"
		"uniform mat4 ViewMatrix; \n"

		"void main(void) \n"
		"{ \n"
		"	gl_Position = ProjectionMatrix * ViewMatrix * vec4( Position, 1.0 ); \n"
		"	out_Position = Position; \n"
		"	out_Texture = Texture; \n"
		"	out_Normal = Normal; \n"
		"} \n";

	static const std::string FragmentSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 out_Position; \n"
		"in vec2 out_Texture; \n"
		"in vec3 out_Normal; \n"
		"out vec4 out_Color; \n"

		"uniform sampler2D DiffuseTexture; \n"
		"uniform sampler2D NormalTexture; \n"
		"uniform vec3 LightPosition; \n"

		"void main(void) \n"
		"{ \n"
		"	vec4 DiffuseColor = texture2D( DiffuseTexture, out_Texture );\n"

		"	vec3 LightDirection = normalize( LightPosition - out_Position ); \n"
		"	float DiffuseLight = max( dot( out_Normal, LightDirection ), 0.2 ); \n"
		"	vec4 DiffuseLightVector = vec4( DiffuseLight, DiffuseLight, DiffuseLight, 1.0 ); \n"

		"	out_Color = DiffuseColor * DiffuseLightVector; \n"
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
		Bit::MatrixManager::GetMatrix( Bit::MatrixManager::Mode_ModelView ) );
	pShaderProgram_Model->SetUniform3f( "LightPosition", 0.0f, 100.0f, 0.0f );
	pShaderProgram_Model->Unbind( );
	
	return BIT_OK;
}