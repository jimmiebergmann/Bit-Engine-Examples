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
#define __CAMERA_HPP__

#include <Bit/DataTypes.hpp>
#include <Bit/System/MatrixManager.hpp>
#include <Bit/System/Vector2.hpp>
#include <Bit/System/Vector3.hpp>

class Camera
{

public:

	// Public enums
	enum eMovement
	{
		Left = 0,
		Right = 1,
		Forward = 2,
		Backward = 3
	};

	// Constructor
	Camera( );

	// Public functions
	void MoveForwards( );
	void MoveBackwards( );
	void MoveLeft( );
	void MoveRight( );
	void Rotate( Bit::Vector2_si32 p_Directions );
	BIT_BOOL Update( const BIT_FLOAT64 p_DeltaTime );
	void UpdateMatrix( );

	// Set functions
	void SetPosition( const Bit::Vector3_f32 p_Position );
	void SetDirection( Bit::Vector3_f32 p_Direction );
	void SetMovementSpeed( const BIT_FLOAT32 p_Speed );
	void SetRotationSpeed( const BIT_FLOAT32 p_Speed );
	void SetRotationResistance( const BIT_FLOAT32 p_Resistance );
	void SetRotationRollFactor( const BIT_FLOAT32 p_Roll );

	// Get functions
	Bit::Matrix4x4 GetMatrix( ) const;
	Bit::Vector3_f32 GetPosition( ) const;
	Bit::Vector3_f32 GetDirection( ) const;
	Bit::Vector3_f32 GetDirectionFlank( ) const;
	BIT_FLOAT32 GetMovementSpeed( ) const;
	BIT_FLOAT32 GetRotationSpeed( ) const;
	BIT_FLOAT32 GetRotationResistance( ) const;
	BIT_FLOAT32 GetRotationRollFactor( ) const;

private:

	// Private functions
	void CalculateDirectionsFromAngles( );
	void CalculateDirectionFlank( );

	// Private variables
	BIT_BOOL m_MovementFlags[ 4 ];
	Bit::Vector2_si32 m_RotationDirections;
	Bit::Vector2_f32 m_RotationForce;
	Bit::Matrix4x4 m_Matrix;
	Bit::Vector3_f32 m_Position;
	Bit::Vector3_f32 m_Direction;
	Bit::Vector3_f32 m_DirectionFlank;
	Bit::Vector2_f32 m_Angles;
	BIT_FLOAT32 m_MovementSpeed;
	BIT_FLOAT32 m_RotationSpeed;
	BIT_FLOAT32 m_RotationResistance;
	BIT_FLOAT32 m_RotationRollFactor;


};

#endif
