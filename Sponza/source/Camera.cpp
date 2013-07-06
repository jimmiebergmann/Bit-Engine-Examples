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
	m_RotationDirections( 0, 0 ),
	m_Position( 0.0f ),
	m_Direction( 0.0f, 0.0f, 1.0f ),
	m_DirectionFlank( 1.0f, 0.0f, 0.0f ),
	m_Angles( 0.0f, 0.0f ),
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
		m_Angles.y -= m_RotationDirections.x * m_RotationSpeed * p_DeltaTime;

		// Set the matrix update flag.
		MatrixUpdate = BIT_TRUE;
	}

	if( m_RotationDirections.y )
	{
		m_Angles.x -= m_RotationDirections.y * m_RotationSpeed * p_DeltaTime;

		// Set the matrix update flag.
		MatrixUpdate = BIT_TRUE;
	}

	// Update the matrix
	if( MatrixUpdate )
	{
		// Calculate the directions
		CalculateDirectionsFromAngles( );

		// Update the matrix
		UpdateMatrix( );
	}


	// Clear the movement flags and the rotation
	for( BIT_MEMSIZE i = 0; i < 4; i++ )
	{
		m_MovementFlags[ i ] = BIT_FALSE;
	}
	m_RotationDirections.x = 0;
	m_RotationDirections.y = 0;


	// Return the matrix update state
	return MatrixUpdate;
}

void Camera::UpdateMatrix( )
{
	// Rotate and translate the camera matrix
	m_Matrix.Identity( );
	m_Matrix.RotateX( m_Angles.x );
	m_Matrix.RotateY( m_Angles.y );
	m_Matrix.Translate( -m_Position.x, -m_Position.y, -m_Position.z );
}

// Set functions
void Camera::SetPosition( const Bit::Vector3_f32 p_Position )
{
	m_Position = p_Position;
}

void Camera::SetDirection( Bit::Vector3_f32 p_Direction )
{ 
	// normalize the direction
	p_Direction.Normalize( );

	// Calculate the Y angle ( vertical spinning, (360 degrees) )
	m_Angles.y = Bit::Vector3_f32::AngleBetweenVectors( Bit::Vector3_f32( p_Direction.x, 0.0f, p_Direction.z ).Normal( ), Bit::Vector3_f32( 0.0f, 0.0f, -1.0f ) );

	if( p_Direction.x > 0.0f )
	{
		m_Angles.y = 360.0f - m_Angles.y;
	}

	// Calculate the X angle ( camera up and down )
	m_Angles.x = Bit::Vector3_f32::AngleBetweenVectors( p_Direction, Bit::Vector3_f32( p_Direction.x, 0.0f, p_Direction.z ).Normal( ) );

	if( p_Direction.y < 0.0f )
	{
		m_Angles.x = -m_Angles.x;
	}

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

BIT_FLOAT32 Camera::GetMovementSpeed( ) const
{
	return m_MovementSpeed;
}

BIT_FLOAT32 Camera::GetRotationSpeed( ) const
{
	return m_RotationSpeed;
}

// Private functions
void Camera::CalculateDirectionsFromAngles( )
{
	// Calculate the view direction
	m_Direction = Bit::Vector3_f32( 0.0f, 0.0f, -1.0f );
	m_Direction.RotateX( m_Angles.x );
	m_Direction.RotateY( -m_Angles.y );

	// Calculate the flank direction
	CalculateDirectionFlank( );
}

void Camera::CalculateDirectionFlank( )
{
	m_DirectionFlank = Bit::Vector3_f32( 0.0f, 0.0f, -1.0f );
	m_DirectionFlank.RotateY( -m_Angles.y + 90.0f );
}