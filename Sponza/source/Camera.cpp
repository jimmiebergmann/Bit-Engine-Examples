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

#include <Camera.hpp>
#include <iostream>
#include <Bit/System/Debugger.hpp>
#include <Bit/System/MemoryLeak.hpp>

// Constructor
Camera::Camera( ) :
	m_Position( 0.0f ),
	m_Direction( 0.0f, 0.0f, 1.0f ),
	m_DirectionFlank( 1.0f, 0.0f, 0.0f ),
	m_DirectionUp( 0.0f, 1.0f, 0.0f ),
	m_RotationDirections( 0, 0 ),
	m_MovementSpeed( 1.0f ),
	m_RotationSpeed( 1.0f )
{
	CalculateDirectionFlank( );
	UpdateMatrix( );

	// Clear the movement flags
	for( BIT_MEMSIZE i = 0; i < 4; i++ )
	{
		m_MovementFlags[ i ] = BIT_FALSE;
	}
}


// Public functions
void Camera::MoveForwards( )
{
	m_MovementFlags[ Forward ] = BIT_TRUE;
}

void Camera::MoveBackwards( )
{
	m_MovementFlags[ Backward ] = BIT_TRUE;
}

void Camera::MoveLeft( )
{
	m_MovementFlags[ Left ] = BIT_TRUE;
}

void Camera::MoveRight( )
{
	m_MovementFlags[ Right ] = BIT_TRUE;
}

void Camera::Rotate( Bit::Vector2_si32 p_Directions )
{
	m_RotationDirections = p_Directions;
}

BIT_BOOL Camera::Update( const BIT_FLOAT64 p_DeltaTime )
{
	// Make sure that we aren't moving in two opposite direcions at the the same time
	if( m_MovementFlags[ Forward ] && m_MovementFlags[ Backward ] )
	{
		m_MovementFlags[ Forward ] = m_MovementFlags[ Backward ] = BIT_FALSE;
	}

	if( m_MovementFlags[ Left ] && m_MovementFlags[ Right ] )
	{
		m_MovementFlags[ Left ] = m_MovementFlags[ Right ] = BIT_FALSE;
	}

	// Create a new movement flag in order to track if we should update the matrix
	BIT_BOOL MatrixUpdate = BIT_FALSE;
	
	// Check which direction we are moving in.
	if( m_MovementFlags[ Forward ] )
	{
		m_Position += m_Direction * m_MovementSpeed * p_DeltaTime;
		MatrixUpdate = BIT_TRUE;
	}
	else if( m_MovementFlags[ Backward ] )
	{
		m_Position -= m_Direction * m_MovementSpeed * p_DeltaTime;
		MatrixUpdate = BIT_TRUE;
	}

	if( m_MovementFlags[ Left ] )
	{
		m_Position -= m_DirectionFlank * m_MovementSpeed * p_DeltaTime;
		MatrixUpdate = BIT_TRUE;
	}
	else if( m_MovementFlags[ Right ] )
	{
		m_Position += m_DirectionFlank * m_MovementSpeed * p_DeltaTime;
		MatrixUpdate = BIT_TRUE;
	}

	// Update the rotation
	if( m_RotationDirections.x )
	{
		//m_Direction.RotateY( m_RotationDirections.x );
		m_Direction.Rotate( m_RotationDirections.x, m_DirectionUp.x, m_DirectionUp.y, m_DirectionUp.z );
		m_Direction.Normalize( );

		// Set the matrix update flag.
		MatrixUpdate = BIT_TRUE;
	}

	if( m_RotationDirections.y )
	{
		m_Direction.Rotate( m_RotationDirections.y, m_DirectionFlank.x, m_DirectionFlank.y, m_DirectionFlank.z );
		m_Direction.Normalize( );

		CalculateDirectionFlank( );

		// Set the matrix update flag.
		MatrixUpdate = BIT_TRUE;
	}


	// Clear the movement flags and the rotation
	for( BIT_MEMSIZE i = 0; i < 4; i++ )
	{
		m_MovementFlags[ i ] = BIT_FALSE;
	}
	m_RotationDirections.x = 0;
	m_RotationDirections.y = 0;

	// Update the matrix
	if( MatrixUpdate )
	{
		UpdateMatrix( );
	}

	return MatrixUpdate;
}

void Camera::UpdateMatrix( )
{
	m_Matrix.Identity( );
	m_Matrix.LookAt( m_Position, m_Position + m_Direction, m_DirectionUp );
}

// Set functions
void Camera::SetPosition( const Bit::Vector3_f32 p_Position )
{
	m_Position = p_Position;
}

void Camera::SetDirection( Bit::Vector3_f32 p_Direction )
{
	m_Direction = p_Direction;
	CalculateDirectionFlank( );
}

void Camera::SetMovementSpeed( const BIT_FLOAT32 p_Speed )
{
	m_MovementSpeed = p_Speed;
}
void Camera::SetRotationSpeed( const BIT_FLOAT32 p_Speed )
{
	m_RotationSpeed = p_Speed;
}

// Get functions
Bit::Matrix4x4 Camera::GetMatrix( ) const
{
	return m_Matrix;
}

Bit::Vector3_f32 Camera::GetPosition( ) const
{
	return m_Position;
}

Bit::Vector3_f32 Camera::GetDirection( ) const
{
	return m_Direction;
}

Bit::Vector3_f32 Camera::GetDirectionFlank( ) const
{
	return m_DirectionFlank;
}

Bit::Vector3_f32 Camera::GetDirectionUp( ) const
{
	return m_DirectionUp;
}

BIT_FLOAT32 Camera::GetMovementSpeed( ) const
{
	return m_MovementSpeed;
}

BIT_FLOAT32 Camera::GetRotationSpeed( ) const
{
	return m_RotationSpeed;
}

// Private functions
void Camera::CalculateDirectionFlank( )
{
	m_DirectionFlank = m_Direction.Cross( m_DirectionUp ).Normal( );
}

void Camera::CalculateDirectionUp( )
{
	m_DirectionUp = m_Direction.Cross( -m_DirectionFlank ).Normal( );
}