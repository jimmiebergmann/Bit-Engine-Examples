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

#ifndef __CAMERA_HPP__

#include <Bit/System/MatrixManager.hpp>
#include <Bit/System/Vector2.hpp>
#include <Bit/System/Vector3.hpp>

class Camera
{

public:

	// Constructor
	Camera( );

	// Public functions
	void MoveForwards( );
	void MoveBackwards( );
	void MoveLeft( );
	void MoveRight( );
	void RotateRight( BIT_FLOAT32 p_Amount );
	void RotateLeft( BIT_FLOAT32 p_Amount );
	void RotateUp( BIT_FLOAT32 p_Amount );
	void RotateDown( BIT_FLOAT32 p_Amount );
	BIT_BOOL Update( const BIT_FLOAT64 p_DeltaTime );
	void UpdateMatrix( );

	// Set functions
	void SetPosition( const Bit::Vector3_f32 p_Position );
	void SetDirection( const Bit::Vector3_f32 p_Direction );
	void SetMovementSpeed( const BIT_FLOAT32 p_Speed );
	void SetEyeSpeed( const BIT_FLOAT32 p_Speed );

	// Get functions
	Bit::Matrix4x4 GetMatrix( ) const;
	Bit::Vector3_f32 GetPosition( ) const;
	Bit::Vector3_f32 GetDirection( ) const;

private:

	// Private functions
	void CalculateDirectionFlank( );

	// Private variables
	Bit::Matrix4x4 m_Matrix;
	Bit::Vector3_f32 m_Position;
	Bit::Vector3_f32 m_Direction;
	Bit::Vector3_f32 m_DirectionFlank;
	Bit::Vector2_f32 m_CameraAngles;
	BIT_FLOAT32 m_MovementSpeed;
	BIT_FLOAT32 m_EyeSpeed;

	// Update variables
	BIT_BOOL m_UpdatePosition;
	BIT_BOOL m_UpdateDirection;
	Bit::Vector3_f32 m_NewMovementDirection;
	Bit::Vector2_f32 m_NewEyeDirection;

};

#endif