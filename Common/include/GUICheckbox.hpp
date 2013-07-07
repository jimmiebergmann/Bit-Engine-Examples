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

#ifndef __GUI_CHECKBOX_HPP__

#include <Bit/DataTypes.hpp>
#include <Bit/System/Vector2.hpp>

class GUICheckbox
{

public:

	// Constructor/destructor
	GUICheckbox( const Bit::Vector2_si32 p_Position, const Bit::Vector2_ui32 p_Size, const BIT_BOOL p_Status );
	~GUICheckbox( );

	// Set functions
	void SetPosition( const Bit::Vector2_si32 p_Position );
	void SetSize( const Bit::Vector2_ui32 p_Size);
	void SetStatus( const BIT_BOOL p_Status );

	// Get functions
	Bit::Vector2_si32 GetPosition( ) const;
	Bit::Vector2_ui32 GetSize( ) const;
	BIT_BOOL GetStatus( ) const;

private:

	// Private variables
	Bit::Vector2_si32 m_Position;
	Bit::Vector2_ui32 m_Size;
	BIT_BOOL m_Status;

};

#endif