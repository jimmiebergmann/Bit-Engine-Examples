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
	m_Position( 0.0f, 0.0f, 0.0f ),
	m_Direction( 0.0f, 0.0f, 1.0f ),
	m_DirectionFlank( 1.0f, 0.0f, 0.0f ),
	m_CameraAngles( 0.0f, 0.0f ),
	m_MovementSpeed( 1.0f ),
	m_EyeSpeed( 1.0f ),
	m_UpdatePosition( BIT_FALSE ),
	m_UpdateDirection( BIT_FALSE ),
	m_NewMovementDirection( 0.0f, 0.0f, 0.0f ),
	m_NewEyeDirection( 0.0f, 0.0f )
{
	CalculateDirectionFlank( );
}


// Public functions
void Camera::MoveForwards( )
{
	m_UpdatePosition = BIT_TRUE;
	m_NewMovementDirection += m_Direction;
}

void Camera::MoveBackwards( )
{
	m_UpdatePosition = BIT_TRUE;
	m_NewMovementDirection -= m_Direction;
}

void Camera::MoveLeft( )
{
	m_UpdatePosition = BIT_TRUE;
	m_NewMovementDirection -= m_DirectionFlank;
}

void Camera::MoveRight( )
{
	m_UpdatePosition = BIT_TRUE;
	m_NewMovementDirection += m_DirectionFlank;
}

void Camera::RotateRight( BIT_FLOAT32 p_Amount )
{
	m_UpdateDirection = BIT_TRUE;
	m_NewEyeDirection.y -= p_Amount;
}

void Camera::RotateLeft( BIT_FLOAT32 p_Amount )
{
	m_UpdateDirection = BIT_TRUE;
	m_NewEyeDirection.y += p_Amount;
}

void Camera::RotateUp( BIT_FLOAT32 p_Amount )
{
	m_UpdateDirection = BIT_TRUE;
	m_NewEyeDirection.x -= p_Amount;
}

void Camera::RotateDown( BIT_FLOAT32 p_Amount )
{
	m_UpdateDirection = BIT_TRUE;
	m_NewEyeDirection.x += p_Amount;
}


BIT_BOOL Camera::Update( const BIT_FLOAT64 p_DeltaTime )
{
	// Update the position
	if( m_UpdatePosition )
	{
		// Normalize the new direction
		m_NewMovementDirection.Normalize( );

		// Set the new position
		m_Position += m_NewMovementDirection * m_MovementSpeed * p_DeltaTime;

		// Reset the mew movement direction.
		m_NewMovementDirection.x = 0.0f;
		m_NewMovementDirection.y = 0.0f;
		m_NewMovementDirection.z = 0.0f;
	}

	// Update the direction
	if( m_UpdateDirection )
	{
		m_CameraAngles.x += m_NewEyeDirection.x * m_EyeSpeed * p_DeltaTime;
		m_CameraAngles.y += m_NewEyeDirection.y * m_EyeSpeed * p_DeltaTime;

		if( m_CameraAngles.x >= 89.99f ) m_CameraAngles.x = 89.99f;
		if( m_CameraAngles.x <= -89.99f ) m_CameraAngles.x = -89.99f;

		if( m_CameraAngles.y >= 360.0f ) m_CameraAngles.y -= 360.0f;
		if( m_CameraAngles.y <= 0.0f ) m_CameraAngles.y += 360.0f;

		// Update the direction flank
		CalculateDirectionFlank( );

		m_NewEyeDirection.x = 0.0f;
		m_NewEyeDirection.y = 0.0f;	

		m_Direction = Bit::Vector3_f32( 0.0f, 0.0f, 1.0f );
		m_Direction.RotateX( -m_CameraAngles.x );
		m_Direction.RotateY( -m_CameraAngles.y );
	}

	// Check if we actually changed the matrix
	BIT_BOOL UpdateStatus = BIT_FALSE;

	// Update the matrix
	if( m_UpdatePosition || m_UpdateDirection )
	{
		UpdateStatus = BIT_TRUE;

		// Update the matrix
		UpdateMatrix( );
	}

	// Reset the position and direction update flag
	m_UpdatePosition = BIT_FALSE;
	m_UpdateDirection = BIT_FALSE;

	return UpdateStatus;
}

void Camera::UpdateMatrix( )
{
	m_Matrix.Identity( );
	m_Matrix.LookAt( m_Position, m_Position + m_Direction, Bit::Vector3_f32( 0.0f, 1.0f, 0.0f ) );
	/*m_Matrix.Translate( -m_Position.x, -m_Position.y, -m_Position.z );
	m_Matrix.RotateX( m_CameraAngles.x );
	m_Matrix.RotateY( m_CameraAngles.y );
	m_Matrix.Translate( m_Position.x, m_Position.y, m_Position.z );*/
	
}


// Set functions
void Camera::SetPosition( const Bit::Vector3_f32 p_Position )
{
	m_Position = p_Position;
}

void Camera::SetDirection( const Bit::Vector3_f32 p_Direction )
{
	m_Direction = p_Direction.Normal( );
	CalculateDirectionFlank( );
}

void Camera::SetMovementSpeed( const BIT_FLOAT32 p_Speed )
{
	m_MovementSpeed = p_Speed;
}
void Camera::SetEyeSpeed( const BIT_FLOAT32 p_Speed )
{
	m_EyeSpeed = p_Speed;
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

// Private functions
void Camera::CalculateDirectionFlank( )
{
	m_DirectionFlank = m_Direction.Cross( Bit::Vector3_f32( 0.0f, 1.0f, 0.0f ) ).Normal( );
}