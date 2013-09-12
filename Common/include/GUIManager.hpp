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

#ifndef __GUI_MANAGER_HPP__
#define __GUI_MANAGER_HPP__

#include <Bit/DataTypes.hpp>
#include <Bit/Window/Event.hpp>
#include <Bit/Graphics/GraphicDevice.hpp>
#include <GUICheckbox.hpp>
#include <GUISlider.hpp>
#include <vector>
#include <string>

class GUIManager
{

public:

	// Construtor/destructor
	GUIManager( Bit::GraphicDevice * p_pGraphicDevice );
	~GUIManager( );

	// Public funcions
	BIT_UINT32 Load( );
	void Unload( );
	BIT_UINT32 Update( Bit::Event & p_Event );
	void Render( );
	BIT_UINT32 Add( const GUICheckbox * p_pCheckbox );
	BIT_UINT32 Add( const GUISlider * p_pSlider );
	BIT_UINT32 Remove( const GUICheckbox * p_pCheckbox );
	BIT_UINT32 Remove( const GUISlider * p_pSlider );

	// Set functions
	void SetCheckboxImagePath( const std::string p_FilePath );
	void SetSliderImagePath( const std::string p_FilePath );

	// Get functions
	std::string GetCheckboxImagePath( ) const;
	std::string GetSliderImagePath( ) const;

private:

	// Private functions
	BIT_UINT32 LoadVertexObject( );
	BIT_UINT32 LoadShaders( );
	BIT_UINT32 LoadTextures( );
	
	// Private variables
	BIT_BOOL m_Loaded;
	Bit::GraphicDevice * m_pGraphicDevice;
	Bit::VertexObject * m_pVertexObject;
	Bit::ShaderProgram * m_pShaderProgram;
	Bit::Shader * m_pVertexShader;
	Bit::Shader * m_pFragmentShader;
	std::string m_CheckboxImagePath;
	std::string m_SliderImagePath;
	std::vector< const GUICheckbox * > m_Checkboxes;
	std::vector< const GUISlider * > m_Sliders;
	BIT_BOOL m_ButtonIsPressed;

	// Private render variables

};

#endif