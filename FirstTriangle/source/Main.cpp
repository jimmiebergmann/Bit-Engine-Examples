#include <Bit/Window/RenderWindow.hpp>
#include <Bit/Graphics/OpenGL/OpenGLGraphicDevice.hpp>
#include <Bit/Graphics/VertexArrayObject.hpp>
#include <Bit/Graphics/VertexBufferObject.hpp>
#include <Bit/Graphics/ShaderProgram.hpp>
#include <Bit/Graphics/Shader.hpp>
#include <Bit/Graphics/Texture.hpp>
#include <Bit/Graphics/Image.hpp>
#include <Bit/System/Timer.hpp>
#include <Bit/System/Vector3.hpp>
#include <iostream>
#include <Bit/System/MemoryLeak.hpp>

// Win32 only.
#ifdef CreateWindow
#undef CreateWindow
#endif

// Window/graphic device
Bit::RenderWindow * pWindow = NULL;
Bit::GraphicDevice * pGraphicDevice = NULL;

// Render data
Bit::VertexBufferObject * pPositionVBO = NULL;
Bit::VertexBufferObject * pColorVBO = NULL;
Bit::VertexArrayObject * pVertexArray = NULL;
Bit::Texture * pTexture = NULL;
Bit::ShaderProgram * pShaderProgram = NULL;
Bit::Shader * pVertexShader = NULL;
Bit::Shader * pFragmentShader = NULL;
Bit::Matrix4x4f32 projectionMatrix;
Bit::Matrix4x4f32 viewMatrix;

// Setting varialbes
const Bit::Vector2u32 windowSize( 1024, 768 );

// Global functions
int CloseApplication( const int p_Code );
void InitializeCamera( );
Bit::Bool CreateWindow( );
Bit::Bool CreateGraphicDevice( );
Bit::Bool LoadMatrices( );
Bit::Bool LoadRenderData( );
void Render( );

// Main function
int main( int argc, char ** argv )
{
	// Initialize the memory leak detector for Win32 only (ignored by default in linux)
	BitInitMemoryLeak( NULL );

	// Setting the absolute path in order to read files.
	//Bit::SetAbsolutePath( argv[ 0 ] );

	// Initialize the application
	if( CreateWindow( ) != true ||
		CreateGraphicDevice( ) != true ||
		LoadMatrices( ) != true ||
		LoadRenderData( ) != true )
	{
		return CloseApplication( 0 );
	}

	// Create a timer and run a main loop for some time
	Bit::Float64 DeltaTime = 0.0f;
	Bit::Timer Timer;
	Timer.Start( );

	std::cout << "Key count: " << Bit::Keyboard::KeyCount<< std::endl;

	// Run the main loop
	while( pWindow->IsOpen( ) )
	{
		// Get the delta time;
		DeltaTime = Timer.GetLapsedTime( ).AsSeconds( );
		Timer.Start( );

		// Do evenets
		pWindow->Update( );

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
				case Bit::Event::KeyPressed:
				{
					if( Event.Key == Bit::Keyboard::Unknown )
				    {
				        std::cout << "Unknown Key pressed: " << Event.Key << std::endl;
				    }
				    else
				    {
                       std::cout << "Key pressed: " << Event.Key << std::endl;
                    }
				}
				break;
				case Bit::Event::KeyJustPressed:
				{
				    std::cout << "Key just pressed: " << Event.Key << std::endl;

					switch( Event.Key )
					{
					    case Bit::Keyboard::Escape:
					    {
					        return CloseApplication( 0 );
					    }
					    default: break;
					}
				}
				break;
				case Bit::Event::KeyJustReleased:
				{
				    std::cout << "Key just released: " << Event.Key << std::endl;
				}
				break;
				case Bit::Event::MousePressed:
				{
				    std::cout << "Button pressed: " << Event.Button << std::endl;
				}
				break;
				case Bit::Event::MouseJustPressed:
				{
				    std::cout << "Button just pressed: " << Event.Button << std::endl;
				}
				break;
				case Bit::Event::MouseJustReleased:
				{
				    std::cout << "Button just released: " << Event.Button << std::endl;
				}
				break;
				default:
					break;
			}
		}



		//pGraphicDevice->BindDefaultFramebuffer( );
		pGraphicDevice->ClearColor( );
		pGraphicDevice->ClearDepth( );

		pShaderProgram->Bind( );
		pTexture->Bind( 0 );
		pVertexArray->Render( Bit::PrimitiveMode::Triangles );
		pShaderProgram->Unbind( );

		// Present the buffers
		pGraphicDevice->Present( );
	}

	// We are done
	std::cout << "Closing the program.\n";
	return CloseApplication( 0 );
}



int CloseApplication( const int p_Code )
{
	// Release the resource manager
	//Bit::ResourceManager::Release( );


	if( pShaderProgram )
	{
		delete pShaderProgram;
		pShaderProgram = NULL;
	}

	if( pVertexShader )
	{
		delete pVertexShader;
		pVertexShader = NULL;
	}

	if( pFragmentShader )
	{
		delete pFragmentShader;
		pFragmentShader = NULL;
	}

	if( pTexture )
	{
		delete pTexture;
		pTexture = NULL;
	}

	if( pPositionVBO )
	{
		delete pPositionVBO;
		pPositionVBO = NULL;
	}

	if( pColorVBO )
	{
		delete pColorVBO;
		pColorVBO = NULL;
	}

	if( pVertexArray )
	{
		delete pVertexArray;
		pVertexArray = NULL;
	}

	if( pGraphicDevice )
	{
		delete pGraphicDevice;
		pGraphicDevice = NULL;
	}

	if( pWindow )
	{
		delete pWindow;
		pWindow = NULL;
	}

	// Return the code
	return p_Code;
}

Bit::Bool CreateWindow( )
{
	// Create the style we want for the window
	const Bit::Uint32 style = Bit::Style::TitleBar | Bit::Style::Resize | Bit::Style::Minimize | Bit::Style::Close;
	const Bit::VideoMode videoMode( windowSize );

	// Create the window(render window)
	pWindow = new Bit::RenderWindow( videoMode, "title", style );

	if( pWindow->IsOpen( ) == false )
	{
		std::cout << "[Error] Could not open the window." << std::endl;
		return false;
	}

	// Change the window's title
	pWindow->SetTitle( "Bit Engine - First Triangle" );

	return true;
}

Bit::Bool CreateGraphicDevice( )
{
	// Create a graphic device
	pGraphicDevice = new Bit::OpenGLGraphicDevice;

	// Open the graphic device
	if( pGraphicDevice->Open( *pWindow, Bit::GraphicDevice::Version( 3, 1 ) ) != true )
	{
		std::cout <<  "[Error] Can not open the graphic device\n";
		return false;
	}

	// Set the clear color
	pGraphicDevice->SetClearColor( 0, 0, 0, 255 );
	pGraphicDevice->SetViewport( Bit::Vector2u32( 0, 0 ),  windowSize );
	//pGraphicDevice->EnableDepthTest( );
	//pGraphicDevice->DisableFaceCulling( );


    // Initialize the resource manager
	/*Bit::Texture::eFilter TextureFilters[ ] =
	{
		Bit::Texture::Filter_Min, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_Mag, Bit::Texture::Filter_Nearest,
		Bit::Texture::Filter_Wrap_X, Bit::Texture::Filter_Repeat,
		Bit::Texture::Filter_Wrap_Y, Bit::Texture::Filter_Repeat,
		Bit::Texture::Filter_None, Bit::Texture::Filter_None
	};
	
	// Initialize the resource manager
	if( Bit::ResourceManager::Initialize( pGraphicDevice, TextureFilters ) != BIT_OK )
	{
		std::cout <<  "[Error] Can not initialize the resource manager\n";
		return false;
	}*/

	return true;
}

Bit::Bool LoadMatrices( )
{
	// Initialize the matrix manager data

	// Projection
/*	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_Projection );
	Bit::MatrixManager::LoadPerspective( 45.0f,(BIT_FLOAT32)WindowSize.x / (BIT_FLOAT32)WindowSize.y, 0.001f, 50.0f );

	// View matrix
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_ModelView );
	Bit::MatrixManager::LoadIdentity( );
	Bit::MatrixManager::LoadLookAt( Bit::Vector3_f32( 0.0f, 0.0f, 3.0f ) , Bit::Vector3_f32( 0.0f, 0.0f, -1.0f ), Bit::Vector3_f32( 0.0f, 1.0f, 0.0f ) );
	*/

	projectionMatrix.Perspective(  45.0f,(Bit::Float32)windowSize.x / (Bit::Float32)windowSize.y, 0.001f, 50.0f );
	viewMatrix.LookAt( Bit::Vector3f32( 0.0f, 0.0f, 3.0f ) , Bit::Vector3f32( 0.0f, 0.0f, -1.0f ), Bit::Vector3f32( 0.0f, 1.0f, 0.0f ) );

	return true;
}


Bit::Bool LoadRenderData( )
{
	// Create the position vertex buffer
	if( ( pPositionVBO = pGraphicDevice->CreateVertexBufferObject( ) )== NULL )
	{
		std::cout << "[Error] Can not create the position vertex buffer.\n";
		return false;
	}

	// Load the buffer
	Bit::Float32 vertexPositions[ 18 ] =
	{
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	pPositionVBO->Load( 18 * 4, vertexPositions );


	// Create the color vertex buffer
	if( ( pColorVBO = pGraphicDevice->CreateVertexBufferObject( ) )== NULL )
	{
		std::cout << "[Error] Can not create the color vertex buffer.\n";
		return false;
	}

	// Load the buffer
	Bit::Float32 vertexColor[ 12 ] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
	pColorVBO->Load( 12 * 4, vertexColor );

	// Create the vertex array
	if( ( pVertexArray = pGraphicDevice->CreateVertexArrayObject( ) )== NULL )
	{
		std::cout << "[Error] Can not create the vertex array.\n";
		return false;
	}

	// Add VBOs
	pVertexArray->AddVertexBuffer( *pPositionVBO, 3 );
	pVertexArray->AddVertexBuffer( *pColorVBO, 2 );


	// Create the texture
	if( ( pTexture = pGraphicDevice->CreateTexture( ) )== NULL )
	{
		std::cout << "[Error] Can not create the texture.\n";
		return false;
	}

	// Load the texture
	std::string texturePath = "../../../Data/TestImage.tga";
	if( pTexture->LoadFromFile( texturePath ) == false )
	{
		std::cout << "[Error] Can not load the texture.\n";
		return false;
	}

	
	// Shader sources
	static const std::string VertexSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 Position; \n"
		"in vec2 Texture; \n"
		"out vec2 out_Texture; \n"

		"uniform mat4 ProjectionMatrix; \n"
		"uniform mat4 ModelViewMatrix; \n"

		"void main(void) \n"
		"{ \n"
		"	out_Texture = Texture; \n"
		"	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4( Position, 1.0 ); \n"
		"} \n";

	static const std::string FragmentSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec2 out_Texture; \n"
		"out vec4 out_Color; \n"

		"uniform sampler2D ColorTexture; \n"

		"void main(void) \n"
		"{ \n"
		" 	vec4 TextureColor = texture2D( ColorTexture, out_Texture ); \n"
		"	out_Color = TextureColor; \n"
		"} \n";

	// Load the shaders
	// Create the vertex and fragment shaders
	if( ( pVertexShader = pGraphicDevice->CreateShader( Bit::ShaderType::Vertex ) ) == NULL )
	{
		std::cout << "[Error] Can not create the vertex shader\n";
		return false;
	}
	if( ( pFragmentShader = pGraphicDevice->CreateShader( Bit::ShaderType::Fragment ) ) == NULL )
	{
		std::cout << "[Error] Can not create the vertex shader\n";
		return false;
	}

	// Set the sources
	pVertexShader->CompileFromMemory( VertexSource );
	pFragmentShader->CompileFromMemory( FragmentSource );

	// Create the shader program
	if( ( pShaderProgram = pGraphicDevice->CreateShaderProgram( ) ) == NULL )
	{
		std::cout << "[Error] Can not create the shader program\n";
		return false;
	}

	// Attach the shaders
	if( pShaderProgram->AttachShader( *pVertexShader ) != true )
	{
		std::cout << "[Error] Can not attach the vertex shader\n";
		return false;
	}
	if( pShaderProgram->AttachShader( *pFragmentShader ) != true )
	{
		std::cout << "[Error] Can not attach the fragment shader\n";
		return false;
	}

	// Set attribute locations
	pShaderProgram->SetAttributeLocation( "Position", 0 );
	pShaderProgram->SetAttributeLocation( "Texture", 1 );

	// Link the shaders
	if( pShaderProgram->Link( ) != true )
	{
		std::cout << "[Error] Can not link the shader program\n";
		return false;
	}

	// Set uniforms
	pShaderProgram->Bind( );
	pShaderProgram->SetUniformMatrix4x4f( "ProjectionMatrix", projectionMatrix );
	pShaderProgram->SetUniformMatrix4x4f( "ModelViewMatrix", viewMatrix );
	pShaderProgram->SetUniform1i( "ColorTexture", 0 );
	pShaderProgram->Unbind( );
	
	return true;
}
