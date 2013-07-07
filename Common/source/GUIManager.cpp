// ///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013 Jimmie Bergmann - jimmiebergmann@gmail.com
//
// This software is provided 'as-is', without any express or
// implied warranty. In no event will the authors be held
// liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute
// it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but
//    is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any
//    source distribution.
// ///////////////////////////////////////////////////////////////////////////

#include <GUIManager.hpp>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>


// Construtor/destructor
GUIManager::GUIManager( Bit::GraphicDevice * p_pGraphicDevice ) :
	m_Loaded( BIT_FALSE ),
	m_pGraphicDevice( p_pGraphicDevice ),
	m_pVertexObject( BIT_NULL ),
	m_pShaderProgram( BIT_NULL ),
	m_pVertexShader( BIT_NULL ),
	m_pFragmentShader( BIT_NULL ),
	m_ButtonIsPressed( BIT_FALSE )
{

}

GUIManager::~GUIManager( )
{
	Unload( );
}

// Public funcions
BIT_UINT32 GUIManager::Load( )
{
	if( m_Loaded )
	{
		bitTrace( "[GUIManager::Load] Already loaded\n" );
		return BIT_ERROR;
	}

	if( m_pGraphicDevice == BIT_NULL )
	{
		bitTrace( "[GUIManager::Load] Graphic device is NULL\n" );
		return BIT_ERROR;
	}

	if( LoadVertexObject( ) != BIT_OK )
	{
		bitTrace( "[GUIManager::Load] Can not load the vertex object\n" );
		return BIT_ERROR;
	}

	if( LoadShaders( ) != BIT_OK )
	{
		bitTrace( "[GUIManager::Load] Can not load the shaders\n" );
		return BIT_ERROR;
	}

	if( LoadTextures( ) != BIT_OK )
	{
		bitTrace( "[GUIManager::Load] Can not load the textures\n" );
		return BIT_ERROR;
	}

	m_Loaded = BIT_TRUE;
	return BIT_OK;
}

void GUIManager::Unload( )
{
	m_pGraphicDevice = BIT_NULL;

	// Delete the render variables
	if( m_pVertexObject )
	{
		delete m_pVertexObject;
		m_pVertexObject = BIT_NULL;
	}

	if( m_pShaderProgram )
	{
		delete m_pShaderProgram;
		m_pShaderProgram = BIT_NULL;
	}

	if( m_pVertexShader )
	{
		delete m_pVertexShader;
		m_pVertexShader = BIT_NULL;
	}

	if( m_pFragmentShader )
	{
		delete m_pFragmentShader;
		m_pFragmentShader = BIT_NULL;
	}

	// Clear the GUI elements
	m_Checkboxes.clear( );
	m_Sliders.clear( );

	// Reset some flags
	m_ButtonIsPressed = BIT_FALSE;
	m_Loaded = BIT_FALSE;
}

BIT_UINT32 GUIManager::Update( Bit::Event & p_Event )
{
	return BIT_OK;
}

void GUIManager::Render( )
{
	if( !m_Loaded )
	{
		bitTrace( "[GUIManager::Render] Is not loaded yet\n" );
		return;
	}

	// Render the checkboxes
	// ..
	for( BIT_MEMSIZE i = 0; i < m_Checkboxes.size( ); i++ )
	{
		m_pShaderProgram->Bind( );
		m_pShaderProgram->SetUniform2f( "VertexPosition", m_Checkboxes[ i ]->GetPosition( ).x,
			m_Checkboxes[ i ]->GetPosition( ).y );
		m_pShaderProgram->SetUniform2f( "VertexSize", m_Checkboxes[ i ]->GetSize( ).x,
			m_Checkboxes[ i ]->GetSize( ).y );

		m_pVertexObject->Render( Bit::VertexObject::RenderMode_Triangles );
		m_pShaderProgram->Unbind( );
	}


	// Render the sliders
	// ..

}

BIT_UINT32 GUIManager::Add( const GUICheckbox * p_pCheckbox )
{
	if( p_pCheckbox == BIT_NULL )
	{
		bitTrace( "[GUIManager::Add(Checkbox)] NULL param\n" );
		return BIT_ERROR;
	}

	m_Checkboxes.push_back( p_pCheckbox );
	return BIT_OK;
}

BIT_UINT32 GUIManager::Add( const GUISlider * p_pSlider )
{
	if( p_pSlider == BIT_NULL )
	{
		bitTrace( "[GUIManager::Add(Slider)] NULL param\n" );
		return BIT_ERROR;
	}

	m_Sliders.push_back( p_pSlider );
	return BIT_OK;
}

BIT_UINT32 GUIManager::Remove( const GUICheckbox * p_pCheckbox )
{
	// Search for the pointer
	// ...

	return BIT_OK;
}

BIT_UINT32 GUIManager::Remove( const GUISlider * p_pSlider )
{
	// Search for the pointer
	// ...

	return BIT_OK;
}

// Set functions
void GUIManager::SetCheckboxImagePath( const std::string p_FilePath )
{
	m_CheckboxImagePath = p_FilePath;
}

void GUIManager::SetSliderImagePath( const std::string p_FilePath )
{
	m_SliderImagePath = p_FilePath;
}

// Get functions
std::string GUIManager::GetCheckboxImagePath( ) const
{
	return m_CheckboxImagePath;
}

std::string GUIManager::GetSliderImagePath( ) const
{
	return m_SliderImagePath;
}

BIT_UINT32 GUIManager::LoadVertexObject( )
{
	// Create the fullscreen vertex object
	if( ( m_pVertexObject = m_pGraphicDevice->CreateVertexObject( ) ) == BIT_NULL )
	{
		bitTrace( "[GUIManager::LoadVertexObject] Can not create the vertex object via the graphic device\n" );
		return BIT_ERROR;
	}

	// Load the fullscreen vertex object
	
	BIT_FLOAT32 VertexPositions[ 18 ] =
	{
		0.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f
	};

	BIT_FLOAT32 VertexTextures[ 12 ] =
	{
		0.0f, 0.0f,		1.0f, 0.0f,		1.0f, 1.0f,	
		0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f
	};

	// SLOW WAY 


	if( m_pVertexObject->AddVertexBuffer( VertexPositions, 3, BIT_TYPE_FLOAT32 ) != BIT_OK )
	{
		bitTrace( "[GUIManager::LoadVertexObject] Can not add vertex position buffer\n" );
		return BIT_ERROR;
	}
	if( m_pVertexObject->AddVertexBuffer( VertexTextures, 2, BIT_TYPE_FLOAT32 ) != BIT_OK )
	{
		bitTrace( "[GUIManager::LoadVertexObject] Can not add vertex texture buffert\n" );
		return BIT_ERROR;
	}

	if( m_pVertexObject->Load( 2, 3 ) != BIT_OK )
	{
		bitTrace( "[GUIManager::LoadVertexObject] Can not load the vertex object\n" );
		return BIT_ERROR;
	}


	return BIT_OK;
}

BIT_UINT32 GUIManager::LoadShaders( )
{
		// Shader sources
	static const std::string VertexSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 Position; \n"
		"in vec2 Texture; \n"

		"out vec3 out_Position; \n"
		"out vec2 out_Texture; \n"

		"uniform mat4 ProjectionMatrix; \n"
		"uniform vec2 VertexPosition; \n"
		"uniform vec2 VertexSize; \n"

		"void main(void) \n"
		"{ \n"

		// Set the output position
		"	gl_Position = ProjectionMatrix * vec4( Position * vec3( VertexSize, 0.0 ) + vec3( VertexPosition, 0.0 ), 1.0 ); \n"

		"} \n";

	static const std::string FragmentSource =
		"#version 330 \n"
		"precision highp float; \n"

		"in vec3 out_Position; \n"
		"in vec2 out_Texture; \n"
		"out vec4 out_Color; \n"

		"uniform sampler2D Texture; \n"

		"void main(void) \n"
		"{ \n"

		// Set the output color
		"	out_Color =  vec4( 1.0, 0.0, 0.0, 1.0 ); \n"
		"} \n";

	// Load the shaders
	// Create the vertex and fragment shaders
	if( ( m_pVertexShader = m_pGraphicDevice->CreateShader( Bit::Shader::Vertex ) ) == BIT_NULL )
	{
		bitTrace( "[GUIManager::LoadShaders] Can not create the vertex shader\n" );
		return BIT_ERROR;
	}
	if( ( m_pFragmentShader = m_pGraphicDevice->CreateShader( Bit::Shader::Fragment ) ) == BIT_NULL )
	{
		bitTrace( "[GUIManager::LoadShaders] Can not create the vertex shader\n" );
		return BIT_ERROR;
	}

	// Set the sources
	m_pVertexShader->SetSource( VertexSource );
	m_pFragmentShader->SetSource( FragmentSource );

	// Compile the shaders
	if( m_pVertexShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[GUIManager::LoadShaders] Can not compile the vertex shader\n" );
		return BIT_ERROR;
	}
	if( m_pFragmentShader->Compile( ) != BIT_OK )
	{
		bitTrace( "[GUIManager::LoadShaders] Can not compile the fragment shader\n" );
		return BIT_ERROR;
	}


	// Create the shader program
	if( ( m_pShaderProgram = m_pGraphicDevice->CreateShaderProgram( ) ) == BIT_NULL )
	{
		bitTrace( "[GUIManager::LoadShaders] Can not create the shader program\n" );
		return BIT_ERROR;
	}

	// Attach the shaders
	if( m_pShaderProgram->AttachShaders( m_pVertexShader ) != BIT_OK )
	{
		bitTrace( "[GUIManager::LoadShaders] Can not attach the vertex shader\n" );
		return BIT_ERROR;
	}
	if( m_pShaderProgram->AttachShaders( m_pFragmentShader ) != BIT_OK )
	{
		bitTrace( "[GUIManager::LoadShaders] Can not attach the fragment shader\n" );
		return BIT_ERROR;
	}

	// Set attribute locations
	m_pShaderProgram->SetAttributeLocation( "Position", 0 );
	m_pShaderProgram->SetAttributeLocation( "Texture", 1 );

	// Link the shaders
	if( m_pShaderProgram->Link( ) != BIT_OK )
	{
		bitTrace( "[GUIManager::LoadShaders] Can not link the shader program\n" );
		return BIT_ERROR;
	}

	// Set uniforms
	Bit::Matrix4x4 Matrix;
	Matrix.Orthographic( 0.0f, m_pGraphicDevice->GetViewportHigh( ).x,
		0.0f, m_pGraphicDevice->GetViewportHigh( ).y, -1.0f, 1.0f );

	m_pShaderProgram->Bind( );
	m_pShaderProgram->SetUniform1i( "Texture", 0 );
	m_pShaderProgram->SetUniformMatrix4x4f( "ProjectionMatrix", Matrix );

	m_pShaderProgram->Unbind( );




















	return BIT_OK;
}

BIT_UINT32 GUIManager::LoadTextures( )
{
	return BIT_OK;
}
