#include <Bit/Window/Window.hpp>
#include <Bit/Graphics/GraphicDevice.hpp>
#include <Bit/System/Keyboard.hpp>
#include <Bit/System/Mouse.hpp>
#include <Bit/Graphics/Image.hpp>
#include <Bit/System/Timer.hpp>
#include <Bit/System.hpp>
#include <Bit/System/Vector3.hpp>
#include <Bit/System/MatrixManager.hpp>
#include <Bit/System/ResourceManager.hpp>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>

// Window/graphic device
Bit::Window * pWindow = BIT_NULL;
Bit::GraphicDevice * pGraphicDevice = BIT_NULL;
Bit::Keyboard * pKeyboard = BIT_NULL;
Bit::Mouse * pMouse = BIT_NULL;

// Render data
Bit::VertexObject * pVertexObject = BIT_NULL;
Bit::Texture * pTexture = BIT_NULL;
Bit::ShaderProgram * pShaderProgram = BIT_NULL;
Bit::Shader * pVertexShader = BIT_NULL;
Bit::Shader * pFragmentShader = BIT_NULL;

// Setting varialbes
const Bit::Vector2_ui32 WindowSize( 1024, 768 );

// Global functions
int CloseApplication( const int p_Code );
void InitializeCamera( );
BIT_UINT32 CreateWindow( );
BIT_UINT32 CreateGraphicDevice( );
BIT_UINT32 LoadMatrices( );
BIT_UINT32 LoadRenderData( );
void Render( );

// Main function
int main( int argc, char ** argv )
{
	// Initialize the memory leak detector for Win32 only (ignored by default in linux)
	bitInitMemoryLeak( BIT_NULL );

	// Setting the absolute path in order to read files.
	Bit::SetAbsolutePath( argv[ 0 ] );

	// Initialize the application
	if( CreateWindow( ) != BIT_OK ||
		CreateGraphicDevice( ) != BIT_OK ||
		LoadMatrices( ) != BIT_OK ||
		LoadRenderData( ) != BIT_OK )
	{
		return CloseApplication( 0 );
	}

	// Create a timer and run a main loop for some time
	BIT_FLOAT64 DeltaTime = 0.0f;
	Bit::Timer Timer;
	Timer.Start( );

    bitTrace( "Key count: %i\n", Bit::Keyboard::Key_Count );

	// Run the main loop
	while( pWindow->IsOpen( ) )
	{
		// Get the delta time;
		DeltaTime = Timer.GetLapsedTime( );
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
				    bitTrace( "Keyboard pressed:  %i \n", Event.Key );
				}
				break;
				case Bit::Event::KeyJustPressed:
				{
				    bitTrace( "Keyboard just pressed:  %i \n", Event.Key );

					switch( Event.Key )
					{
					    case Bit::Keyboard::Key_Escape:
					    {
					        return CloseApplication( 0 );
					    }
					    default: break;
					}

				}
				break;
				case Bit::Event::KeyJustReleased:
				{
				    bitTrace( "Keyboard just released:  %i \n", Event.Key );
				}
				break;
				case Bit::Event::ButtonPressed:
				{
				    bitTrace( "Mouse pressed:  %i \n", Event.Button );
				}
				break;
				case Bit::Event::ButtonJustPressed:
				{
				    bitTrace( "Mouse just pressed:  %i \n", Event.Button );
				}
				break;
				case Bit::Event::ButtonJustReleased:
				{
				    bitTrace( "Mouse just released:  %i \n", Event.Button );
				}
				break;
				default:
					break;
			}
		}

		// Update the keyboard
		//pKeyboard->Update( );
		//pMouse->Update( );

		// Button test
		/*if( pMouse->ButtonIsJustPressed( Bit::Mouse::Button_1 ) )
		{
            bitTrace( "Mouse press 2:  %i \n", Bit::Mouse::Button_1 );
		}
		if( pMouse->ButtonIsJustPressed( Bit::Mouse::Button_2 ) )
		{
            bitTrace( "Mouse press 2:  %i \n", Bit::Mouse::Button_2 );
		}
		if( pMouse->ButtonIsJustPressed( Bit::Mouse::Button_3 ) )
		{
            bitTrace( "Mouse press 2:  %i \n", Bit::Mouse::Button_3 );
		}
*/

        // Key test
		/*for( BIT_MEMSIZE i = 0; i < Bit::Keyboard::Key_Count; i++ )
		{
		    if( pKeyboard->KeyIsDown( (Bit::Keyboard::eKey)i ) )
		    {
		        bitTrace( "Keyboard 2:  %i \n", i );
		    }

		}

		// Button test
		for( BIT_MEMSIZE i = 0; i < Bit::Mouse::Button_Count; i++ )
		{
		    if( pMouse->ButtonIsDown( (Bit::Mouse::eButton)i ) )
		    {
		        bitTrace( "Mouse 2:  %i \n", i );
		    }

		}*/

/*
        // SYSTEM KEY TESTS
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_W ) )
        {
            bitTrace( "Pressed W \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_A ) )
        {
            bitTrace( "Pressed A \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_S ) )
        {
            bitTrace( "Pressed S \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_D ) )
        {
            bitTrace( "Pressed D \n" );
        }


        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Alt_L ) )
        {
            bitTrace( "Pressed Alt L \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Alt_R ) )
        {
            bitTrace( "Pressed Alt R \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Backspace ) )
        {
            bitTrace( "Pressed Backspace \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Bracket_L ) )
        {
            bitTrace( "Pressed Bracket L \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Bracket_R ) )
        {
            bitTrace( "Pressed Bracket R \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Break ) )
        {
            bitTrace( "Pressed BreaK \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Capslock ) )
        {
            bitTrace( "Pressed Capslock \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Control_L ) )
        {
            bitTrace( "Pressed Control L \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Control_R ) )
        {
            bitTrace( "Pressed Control R \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Delete ) )
        {
            bitTrace( "Pressed Delete \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_End ) )
        {
            bitTrace( "Pressed End \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::key_Espace ) )
        {
            bitTrace( "Pressed Espace \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Home ) )
        {
            bitTrace( "Pressed Home \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Insert ) )
        {
            bitTrace( "Pressed Insert \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_PageDown ) )
        {
            bitTrace( "Pressed Page Down \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_PageUp ) )
        {
            bitTrace( "Pressed Page Up \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Print ) )
        {
            bitTrace( "Pressed Print \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Return ) )
        {
            bitTrace( "Pressed Return \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_ScrollLock ) )
        {
            bitTrace( "Pressed Scroll lock \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Section ) )
        {
            bitTrace( "Pressed Section \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Shift_L ) )
        {
            bitTrace( "Pressed Shift L\n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Shift_R ) )
        {
            bitTrace( "Pressed Shift R \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Space ) )
        {
            bitTrace( "Pressed Space \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::key_Super_L ) )
        {
            bitTrace( "Pressed Super L \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::key_Super_R ) )
        {
            bitTrace( "Pressed Super R \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Tab ) )
        {
            bitTrace( "Pressed Tab \n" );
        }

        // New keys

        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Asterisk ) )
        {
            bitTrace( "Pressed Asterisk \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Comma ) )
        {
            bitTrace( "Pressed Comma \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Minus ) )
        {
            bitTrace( "Pressed Minus \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Period ) )
        {
            bitTrace( "Pressed Period \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Slash ) )
        {
            bitTrace( "Pressed Slash \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Colon ) )
        {
            bitTrace( "Pressed Colon \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_SemiColon ) )
        {
            bitTrace( "Pressed Semi colon \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Less ) )
        {
            bitTrace( "Pressed Less \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Equal ) )
        {
            bitTrace( "Pressed Equal \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Greater ) )
        {
            bitTrace( "Pressed Greater \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Question ) )
        {
            bitTrace( "Pressed Question \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Underscore ) )
        {
            bitTrace( "Pressed Underscore \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_A_Diaeresis ) )
        {
            bitTrace( "Pressed A Diaeresis Ä \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_A_Ring ) )
        {
            bitTrace( "Pressed A ring Å \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_O_Diaeresis ) )
        {
            bitTrace( "Pressed O Diaeresis Ö \n" );
        }



        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Exclam ) )
        {
            bitTrace( "Pressed Exclam \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Quote ) )
        {
            bitTrace( "Pressed Quote \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Number ) )
        {
            bitTrace( "Pressed Number \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Dollar ) )
        {
            bitTrace( "Pressed Dollar\n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Percent ) )
        {
            bitTrace( "Pressed Percent \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Ampersand ) )
        {
            bitTrace( "Pressed Ampersand \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Apostrophe ) )
        {
            bitTrace( "Pressed Apostrophe \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Parenthesis_Left ) )
        {
            bitTrace( "Pressed Parenthesis Left \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Parenthesis_Right ) )
        {
            bitTrace( "Pressed Parenthesis Right \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Brace_Left ) )
        {
            bitTrace( "Pressed Brace Left \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Bar ) )
        {
            bitTrace( "Pressed Bar \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Brace_Right ) )
        {
            bitTrace( "Pressed Brace Right \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_Tilde ) )
        {
            bitTrace( "Pressed Tilde \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_BackSlash) )
        {
            bitTrace( "Pressed Back Slash \n" );
        }
        if( pKeyboard->KeyIsJustPressed( Bit::Keyboard::Key_NumLock ) )
        {
            bitTrace( "Pressed Num lock \n" );
        }
*/

		// ///////////////////////////////////////////////////////////////////////////////////

		pGraphicDevice->BindDefaultFramebuffer( );
		pGraphicDevice->ClearColor( );
		pGraphicDevice->ClearDepth( );

		pShaderProgram->Bind( );
		pTexture->Bind( 0 );
		pVertexObject->Render( Bit::VertexObject::RenderMode_Triangles );
		pShaderProgram->Unbind( );

		// Present the buffers
		pGraphicDevice->Present( );
	}

	// We are done
	bitTrace( "Closing the program.\n" );
	return CloseApplication( 0 );

	return 0;
}



int CloseApplication( const int p_Code )
{
	// Release the resource manager
	//Bit::ResourceManager::Release( );


	if( pShaderProgram )
	{
		delete pShaderProgram;
		pShaderProgram = BIT_NULL;
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

	if( pTexture )
	{
		delete pTexture;
		pTexture = BIT_NULL;
	}

	if( pVertexObject )
	{
		delete pVertexObject;
		pVertexObject = BIT_NULL;
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
	BIT_UINT32 Style = Bit::Window::Style_TitleBar | Bit::Window::Style_Minimize | Bit::Window::Style_Close;

	// Open the window
	if( pWindow->Open( WindowSize, 32, "Hello World", Style ) != BIT_OK )
	{
		bitTrace( "[Error] Can not open the window\n" );
		return BIT_ERROR;
	}

	// Change the window's title
	pWindow->SetTitle( "Linux Test. Testing swedish characters: והצ ִֵײ" );


    // Create a keyboard
    if( ( pKeyboard = Bit::CreateKeyboard( ) ) == BIT_NULL )
    {
        bitTrace( "[Error] Can not create the keyboard\n" );
		return BIT_ERROR;
    }

    // Create a mouse
    if( ( pMouse = Bit::CreateMouse( ) ) == BIT_NULL )
    {
        bitTrace( "[Error] Can not create the mouse\n" );
		return BIT_ERROR;
    }

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
	pGraphicDevice->DisableFaceCulling( );


    // Initialize the resource manager
	Bit::Texture::eFilter TextureFilters[ ] =
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
	Bit::MatrixManager::LoadPerspective( 45.0f,(BIT_FLOAT32)WindowSize.x / (BIT_FLOAT32)WindowSize.y, 0.001f, 50.0f );

	// View matrix
	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_View );
	Bit::MatrixManager::LoadIdentity( );


	Bit::MatrixManager::LoadLookAt( Bit::Vector3_f32( 0.0f, 0.0f, 3.0f ) , Bit::Vector3_f32( 0.0f, 0.0f, -1.0f ), Bit::Vector3_f32( 0.0f, 1.0f, 0.0f ) );

	Bit::MatrixManager::SetMode( Bit::MatrixManager::Mode_Model );
	Bit::MatrixManager::LoadIdentity( );

	return BIT_OK;
}


BIT_UINT32 LoadRenderData( )
{
	// Vertex object
	// Create the vertex object
	if( ( pVertexObject = pGraphicDevice->CreateVertexObject( ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not create the vertex object\n" );
		return BIT_ERROR;
	}


	BIT_FLOAT32 VertexPositions[ 18 ] =
	{
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	BIT_FLOAT32 VertexTextures[ 12 ] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	if( pVertexObject->AddVertexBuffer( VertexPositions, 3, BIT_TYPE_FLOAT32 ) != BIT_OK )
	{
		bitTrace( "[Error] Can not add vertex position buffer to the vertex object\n" );
		return BIT_ERROR;
	}

	if( pVertexObject->AddVertexBuffer( VertexTextures, 2, BIT_TYPE_FLOAT32 ) != BIT_OK )
	{
		bitTrace( "[Error] Can not add vertex texture buffer to the vertex object\n" );
		return BIT_ERROR;
	}

	if( pVertexObject->Load( 2, 3 ) != BIT_OK )
	{
		bitTrace( "[Error] Can not load the vertex object\n" );
		return BIT_ERROR;
	}


	// Load the texture
	std::string TexturePath = "../../../Data/TestImage.tga";
    if( (pTexture = Bit::ResourceManager::GetTexture( Bit::GetAbsolutePath( TexturePath ) ) ) == BIT_NULL )
	{
		bitTrace( "[Error] Can not get the texture\n" );
		return BIT_ERROR;
	}


	// Shader sources
	static const std::string VertexSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 Position; \n"
		"in vec2 Texture; \n"
		"out vec2 out_Texture; \n"

		"uniform mat4 ProjectionMatrix; \n"
		"uniform mat4 ViewMatrix; \n"

		"void main(void) \n"
		"{ \n"
		"	out_Texture = Texture; \n"
		"	gl_Position = ProjectionMatrix * ViewMatrix * vec4( Position, 1.0 ); \n"
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

	// Set the sources
	pVertexShader->SetSource( VertexSource );
	pFragmentShader->SetSource( FragmentSource );

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


	// Create the shader program
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
	pShaderProgram->Bind( );

	pShaderProgram->SetUniformMatrix4x4f( "ProjectionMatrix", Bit::MatrixManager::GetProjectionMatrix( ) );
	pShaderProgram->SetUniformMatrix4x4f( "ViewMatrix", Bit::MatrixManager::GetViewMatrix( ) );
	pShaderProgram->SetUniform1i( "ColorTexture", 0 );

	pShaderProgram->Unbind( );

	return BIT_OK;
}
