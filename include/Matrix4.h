/* 
* VMMLib - Vector & Matrix Math Lib
*  
* @author Jonas Boesch
* @author Stefan Eilemann
* @author Renato Pajarola
* @author David H. Eberly ( Wild Magic )
* @author Andrew Willmott ( VL )
*
* @license BSD license, check LICENSE
*
* parts of the source code of VMMLib were inspired by David Eberly's 
* Wild Magic and Andrew Willmott's VL.
* 
*/ 


#ifndef _Matrix4_H_
#define _Matrix4_H_

/* 
 *   4x4 Matrix Class
 */ 

#include <math.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <assert.h>

// * * * * * * * * * *
//
// - declaration -
//
// * * * * * * * * * *

namespace vmml
{
    template< typename T > class Matrix3;
    template< typename T > class Vector3;
    template< typename T > class Vector4;

template< typename T > 
class Matrix4
{
public:
    union
    {
        struct
        {
            T   m00, m10, m20, m30, m01, m11, m21, m31, 
                m02, m12, m22, m32, m03, m13, m23, m33;
        };
        T m[4][4]; //cols
        T ml[16]; // linear
    };
    
    Matrix4();
    Matrix4( const Matrix4& mm );
    Matrix4( T v00, T v01, T v02, T v03, 
             T v10, T v11, T v12, T v13,
             T v20, T v21, T v22, T v23,
             T v30, T v31, T v32, T v33 );
    Matrix4( const Vector4<T>& v0, const Vector4<T>& v1, 
             const Vector4<T>& v2, const Vector4<T>& v3, 
             bool columnVectors = false );

    // dangerous, but implemented to allow easy conversion between 
    // Matrix< float > and Matrix< double >
    //the pointer 'values must be a valid 16 component c array of the resp. type
    Matrix4( float* values );
    Matrix4( double* values );
 
    inline Matrix4& operator= ( const Matrix4& mm );
    inline bool operator== ( const Matrix4& mm ) const;
    inline bool operator!= ( const Matrix4& mm ) const;

    void set( const Matrix4& mm );
    // dangerous, but implemented to allow easy conversion between 
    // Matrix< float > and Matrix< double >
    //the pointer 'values must be a valid 16 component c array of the resp. type
    void set( const float* mm );
    void set( const double* mm );
    void set( T v00, T v01, T v02, T v03, T v10, T v11, T v12, T v13, 
              T v20, T v21, T v22, T v23, T v30, T v31, T v32, T v33 );

    Vector4< T > getColumn( const size_t column ) const;
    Vector4< T > getRow( const size_t row ) const; 
    const T& getElement( const size_t row, const size_t col ) const;
    
    // vec3
    void setColumn( const size_t column, const Vector3< T >& columnvec );
    void setRow( const size_t row, const Vector3< T >& rowvec );
    // vec4
    void setColumn( const size_t column, const Vector4< T >& columnvec );
    void setRow( const size_t row, const Vector4< T >& rowvec );

    void setElement( const size_t row, const size_t col, 
                         const T& value ) const;

    // arithmetic operations
    Matrix4 operator+ ( const Matrix4& mm ) const;
    Matrix4 operator- ( const Matrix4& mm ) const;
    Matrix4 operator* ( const Matrix4& mm ) const;
    Matrix4 operator* ( T scalar ) const; // matrix = matrix * scalar 
    inline Matrix4 operator/ ( T scalar ) const
        { scalar = 1.0 / scalar; return operator*(scalar); }; 
    
    // vector = matrix * vector
    Vector3< T > operator* ( const Vector3< T >& vv ) const;
    Vector4< T > operator* ( const Vector4< T >& vv ) const;

    Matrix4& operator+= ( const Matrix4& mm );
    Matrix4& operator-= ( const Matrix4& mm );
    Matrix4& operator*= ( const Matrix4& mm );
    Matrix4& operator*= ( T scalar ); // matrix = matrix * scalar 
    inline Matrix4& operator/= ( T scalar )
        { scalar = 1.0 / scalar; return operator*=( scalar ); };

    Matrix4 negate() const;
    Matrix4 operator-() const;
    
    Matrix4 getTransposed() const;

    T getDeterminant() const;
    inline T det() const;

    Matrix4 getAdjugate() const;
    inline Matrix4 getAdjoint() const;
    
    Matrix4 getInverse( bool& isInvertible, T limit = 0.0000000001 );
    inline bool getInverse( Matrix4& result, T limit = 0.0000000001 );

    void rotateX( const T angle );
    void rotateY( const T angle );
    void rotateZ( const T angle );
    void scale( const T scale[3] );
    void scaleTranslation( const T scale[3] );
    void setTranslation( const T x, const T y, const T z );
    void setTranslation( const T trans[3] );
    void setTranslation( const Vector3<T>& trans );

    void tensor( const Vector3< T >& u, const Vector3< T >& v );
    void tensor( const Vector4< T >& u, const Vector4< T >& v );

    // computes the minor of M, that is, the determinant of an 
    // n-1 x n-1 ( = 3x3 ) submatrix of M
    // specify the indices of the rows/columns to be used 
    T getMinor( const size_t row0, const size_t row1, const size_t row2,
             const size_t col0, const size_t col1, const size_t col2 ) const;
    // specify the indices of the rows/columns to be removed ( slower ) 
    T getMinor( const size_t removeRow, const size_t removeCol ) const;
                   

    friend std::ostream& operator << ( std::ostream& os, const Matrix4& m )
    {
        const std::ios::fmtflags flags = os.flags();
        const int                prec  = os.precision();

        os.setf( std::ios::right, std::ios::adjustfield );
        os.precision( 5 );
        os << std::endl << "|" << std::setw(7) << m.m[0][0] << " " 
           << std::setw(7) << m.m[0][1] << " " 
           << std::setw(7) << m.m[0][2] << " " 
           << std::setw(7) << m.m[0][3] << "|" << std::endl
           << "|" << std::setw(7) << m.m[1][0] << " " 
           << std::setw(7) << m.m[1][1] << " " 
           << std::setw(7) << m.m[1][2] << " " 
           << std::setw(7) << m.m[1][3] << "|" << std::endl
           << "|" << std::setw(7) << m.m[2][0] << " " 
           << std::setw(7) << m.m[2][1] << " " 
           << std::setw(7) << m.m[2][2] << " " 
           << std::setw(7) << m.m[2][3] << "|" << std::endl
           << "|" << std::setw(7) << m.m[3][0] << " " 
           << std::setw(7) << m.m[3][1] << " " 
           << std::setw(7) << m.m[3][2] << " " 
           << std::setw(7) << m.m[3][3] << "|" << std::endl;
        os.precision( prec );
        os.setf( flags );
        return os;
    };  
    
    static const Matrix4 IDENTITY;
    static const Matrix4 ZERO;

};

#ifndef VMMLIB_DISABLE_TYPEDEFS
    typedef Matrix4<float>  Matrix4f;
    typedef Matrix4<double> Matrix4d;
#endif
}
// * * * * * * * * * *
//
// - implementation -
//
// * * * * * * * * * *
#include "Matrix3.h"
#include "Vector3.h"
#include "Vector4.h"

namespace vmml
{

template< typename T > 
const Matrix4< T > Matrix4< T >::IDENTITY( 1, 0, 0, 0, 0, 1, 0, 0,
                                           0, 0, 1, 0, 0, 0, 0, 1 );

template< typename T > 
const Matrix4< T > Matrix4< T >::ZERO( 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0 );


template< typename T > 
Matrix4< T >::Matrix4()
{}

template< typename T > 
Matrix4< T >::Matrix4( const Matrix4< T >& mm )
{
    memcpy(m,mm.m, 16 * sizeof( T ) );
}

template< typename T > 
Matrix4< T >::Matrix4( T v00, T v01, T v02, T v03, 
                       T v10, T v11, T v12, T v13,
                       T v20, T v21, T v22, T v23,
                       T v30, T v31, T v32, T v33 )
    : m00( v00 )
    , m10( v10 )
    , m20( v20 )
    , m30( v30 )
    , m01( v01 )
    , m11( v11 )
    , m21( v21 )
    , m31( v31 )
    , m02( v02 )
    , m12( v12 )
    , m22( v22 )
    , m32( v32 )
    , m03( v03 )
    , m13( v13 )
    , m23( v23 )
    , m33( v33 )
{}
         
template< typename T > 
Matrix4< T >::Matrix4( const Vector4< T >& v0, const Vector4< T >& v1, 
                          const Vector4< T >& v2, const Vector4< T >& v3,
                          bool columnVectors )
{
    if ( columnVectors )
        for ( size_t i = 0; i < 4; ++i )
        {
            m[0][i] = v0[i];
            m[1][i] = v1[i];
            m[2][i] = v2[i];
            m[3][i] = v3[i];
        }
    else
        for ( size_t i = 0; i < 4; ++i )
        {
            m[i][0] = v0[i];
            m[i][1] = v1[i];
            m[i][2] = v2[i];
            m[i][3] = v3[i];
        }
}

template< typename T > 
const T& Matrix4< T >::getElement( const size_t row, const size_t col ) const
{
    return m[col][row];
} 

template< typename T > 
void Matrix4< T >::setElement( const size_t row, const size_t col, 
                         const T& value ) const
{
    m[col][row] = value;
}

template< typename T > 
Matrix4< T >::Matrix4( float* values )
{
    assert( values && "Matrix4: Initialisation of a Matrix from a Nullpointer was requested." );
    for ( size_t i = 0; i < 16; ++i )
        ml[i] = static_cast< T > ( values[i] );
}

template< typename T > 
Matrix4< T >::Matrix4( double* values )
{
    assert( values && "Matrix4: Initialisation of a Matrix from a Nullpointer was requested." );
    for ( size_t i = 0; i < 16; ++i )
        ml[i] = static_cast< T > ( values[i] );
}


template< typename T > 
Matrix4< T >& Matrix4< T >::operator= ( const Matrix4< T >& mm )
{
    memcpy( ml, mm.ml, 16 * sizeof(T) );
    return *this;
}

template< typename T > 
bool Matrix4< T >::operator== (const Matrix4< T >& mm) const
{
    for( size_t i = 0; i < 16; ++i )
    {
        if( ml[i] != mm.ml[i] )
            return false;
    }
    return true;
}

template< typename T > 
inline bool Matrix4< T >::operator!= (const Matrix4< T >& mm) const
{
    return !operator==(mm);
}

template< typename T > 
void Matrix4< T >::set( const Matrix4& mm )
{
    memcpy( ml, mm.ml, 16 * sizeof( T ) );
}

template< typename T > 
void Matrix4< T >::set( const float* values )
{
    assert( values && "Matrix4: Nullpointer argument as source for initialisation!" );
    for ( size_t i = 0; i < 16; ++i )
        ml[i] = static_cast< T > ( values[i] );
}

template< typename T > 
void Matrix4< T >::set( const double* values )
{
    assert( values && "Matrix4: Nullpointer argument as source for initialisation!" );
    for ( size_t i = 0; i < 16; ++i )
        ml[i] = static_cast< T > ( values[i] );
}

template< typename T > 
void Matrix4< T >::set( T v00, T v01, T v02, T v03, T v10, T v11, T v12, T v13, 
              T v20, T v21, T v22, T v23, T v30, T v31, T v32, T v33 )
{
    m00 = v00;
    m10 = v10;
    m20 = v20;
    m30 = v30;
    m01 = v01;
    m11 = v11;
    m21 = v21;
    m31 = v31;
    m02 = v02;
    m12 = v12;
    m22 = v22;
    m32 = v32;
    m03 = v03;
    m13 = v13;
    m23 = v23;
    m33 = v33;

}

template< typename T > 
Vector4< T > Matrix4< T >::getColumn( size_t column ) const
{
    assert( column < 4 && "Matrix4: Requested Column ( getColumn ) with invalid index!" );
    return Vector4< T >( m[column] );
}

template< typename T > 
Vector4< T > Matrix4< T >::getRow( size_t row ) const
{
    assert( row < 4 && "Matrix4: Requested Row ( getRow ) with invalid index!");
    return Vector4< T > ( ml[0+row], ml[4+row], ml[8+row], ml[12+row] );
}

template< typename T > 
void Matrix4< T >::setColumn( size_t column, const Vector4< T >& columnvec )
{
    m[column][0] = columnvec[0];
    m[column][1] = columnvec[1];
    m[column][2] = columnvec[2];
    m[column][3] = columnvec[3];
}

template< typename T > 
void Matrix4< T >::setRow( size_t row, const Vector4< T >& rowvec )
{
    m[0][row] = rowvec[0];
    m[1][row] = rowvec[1];
    m[2][row] = rowvec[2];
    m[3][row] = rowvec[3];
}

template< typename T > 
void Matrix4< T >::setColumn( size_t column, const Vector3< T >& columnvec )
{
    m[column][0] = columnvec[0];
    m[column][1] = columnvec[1];
    m[column][2] = columnvec[2];
}

template< typename T > 
void Matrix4< T >::setRow( size_t row, const Vector3< T >& rowvec )
{
    m[0][row] = rowvec[0];
    m[1][row] = rowvec[1];
    m[2][row] = rowvec[2];
}

template< typename T > 
Matrix4< T > Matrix4< T >::operator+ (const Matrix4< T >& mm) const
{
    Matrix4< T > result;
    for ( size_t i = 0; i < 16; ++i )
        result.ml[i] = ml[i] + mm.ml[i];
    return result;
}

template< typename T > 
Matrix4< T > Matrix4< T >::operator- (const Matrix4< T >& mm) const
{
    Matrix4< T > result;
    for( size_t i = 0; i < 16; ++i )
        result.ml[i] = ml[i] - mm.ml[i];
    return result;
}

template< typename T > 
Matrix4< T > Matrix4< T >::operator* (const Matrix4< T >& o) const
{
    Matrix4< T > r;

    r.m00 = m00*o.m00 + m01*o.m10 + m02*o.m20 + m03*o.m30;
    r.m10 = m10*o.m00 + m11*o.m10 + m12*o.m20 + m13*o.m30;
    r.m20 = m20*o.m00 + m21*o.m10 + m22*o.m20 + m23*o.m30;
    r.m30 = m30*o.m00 + m31*o.m10 + m32*o.m20 + m33*o.m30;

    r.m01 = m00*o.m01 + m01*o.m11 + m02*o.m21 + m03*o.m31;
    r.m11 = m10*o.m01 + m11*o.m11 + m12*o.m21 + m13*o.m31;
    r.m21 = m20*o.m01 + m21*o.m11 + m22*o.m21 + m23*o.m31;
    r.m31 = m30*o.m01 + m31*o.m11 + m32*o.m21 + m33*o.m31;

    r.m02 = m00*o.m02 + m01*o.m12 + m02*o.m22 + m03*o.m32;
    r.m12 = m10*o.m02 + m11*o.m12 + m12*o.m22 + m13*o.m32;
    r.m22 = m20*o.m02 + m21*o.m12 + m22*o.m22 + m23*o.m32;
    r.m32 = m30*o.m02 + m31*o.m12 + m32*o.m22 + m33*o.m32;

    r.m03 = m00*o.m03 + m01*o.m13 + m02*o.m23 + m03*o.m33;
    r.m13 = m10*o.m03 + m11*o.m13 + m12*o.m23 + m13*o.m33;
    r.m23 = m20*o.m03 + m21*o.m13 + m22*o.m23 + m23*o.m33;
    r.m33 = m30*o.m03 + m31*o.m13 + m32*o.m23 + m33*o.m33;

    return r;
}

template< typename T > 
Matrix4< T > Matrix4< T >::operator* ( T scalar ) const
{
    Matrix4< T > result;
    for ( size_t i = 0; i < 16; ++i )
        result.ml[i] = ml[i] * scalar;
    return result;
}

template< typename T > 
Matrix4< T >& Matrix4< T >::operator+= (const Matrix4< T >& mm) 
{
    for ( size_t i = 0; i < 16; ++i )
        ml[i]  += mm.ml[i];
    return *this;
}

template< typename T > 
Matrix4< T >& Matrix4< T >::operator-= ( const Matrix4& mm )
{
    for ( size_t i = 0; i < 16; ++i )
        ml[i]  -= mm.ml[i];
    return *this;
}

template< typename T > 
Matrix4< T >& Matrix4< T >::operator*= ( const Matrix4& o ) 
{
    Matrix4< T > r;

    r.m00 = m00*o.m00 + m01*o.m10 + m02*o.m20 + m03*o.m30;
    r.m10 = m10*o.m00 + m11*o.m10 + m12*o.m20 + m13*o.m30;
    r.m20 = m20*o.m00 + m21*o.m10 + m22*o.m20 + m23*o.m30;
    r.m30 = m30*o.m00 + m31*o.m10 + m32*o.m20 + m33*o.m30;

    r.m01 = m00*o.m01 + m01*o.m11 + m02*o.m21 + m03*o.m31;
    r.m11 = m10*o.m01 + m11*o.m11 + m12*o.m21 + m13*o.m31;
    r.m21 = m20*o.m01 + m21*o.m11 + m22*o.m21 + m23*o.m31;
    r.m31 = m30*o.m01 + m31*o.m11 + m32*o.m21 + m33*o.m31;

    r.m02 = m00*o.m02 + m01*o.m12 + m02*o.m22 + m03*o.m32;
    r.m12 = m10*o.m02 + m11*o.m12 + m12*o.m22 + m13*o.m32;
    r.m22 = m20*o.m02 + m21*o.m12 + m22*o.m22 + m23*o.m32;
    r.m32 = m30*o.m02 + m31*o.m12 + m32*o.m22 + m33*o.m32;

    r.m03 = m00*o.m03 + m01*o.m13 + m02*o.m23 + m03*o.m33;
    r.m13 = m10*o.m03 + m11*o.m13 + m12*o.m23 + m13*o.m33;
    r.m23 = m20*o.m03 + m21*o.m13 + m22*o.m23 + m23*o.m33;
    r.m33 = m30*o.m03 + m31*o.m13 + m32*o.m23 + m33*o.m33;

    *this = r;
    return *this;
}

template< typename T > 
Matrix4< T >& Matrix4< T >::operator*= ( T scalar )
{
    // matrix = matrix * scalar 
    for ( size_t i = 0; i < 16; ++i )
        ml[i]  *= scalar;
    return *this;
}

template< typename T > 
Vector4< T > Matrix4< T >::operator* (const Vector4< T >& vv) const
{
	return Vector4< T >( vv[0] * m00 + vv[1] * m10 + vv[2] * m20 + vv[3] * m30,
                         vv[0] * m01 + vv[1] * m11 + vv[2] * m21 + vv[3] * m31,
                         vv[0] * m02 + vv[1] * m12 + vv[2] * m22 + vv[3] * m32,
                         vv[0] * m03 + vv[1] * m13 + vv[2] * m23 + vv[3] * m33);
}

template< typename T > 
Vector3< T > Matrix4< T >::operator* (const Vector3< T >& vv) const
{
	Vector4< T > result;
    
	result[0] = vv[0] * m00 + vv[1] * m10 + vv[2] * m20 + m30;
	result[1] = vv[0] * m01 + vv[1] * m11 + vv[2] * m21 + m31;
	result[2] = vv[0] * m02 + vv[1] * m12 + vv[2] * m22 + m32;
	result[3] = vv[0] * m03 + vv[1] * m13 + vv[2] * m23 + m33;
	return Vector3<T>( result );
}

template< typename T > 
Matrix4< T > Matrix4< T >::getTransposed() const
{
    Matrix4< T > result;
    for ( size_t i = 0; i < 4; ++i )
        for ( size_t j = 0; j < 4; ++j )
            result.m[i][j] = m[j][i]; 
    return result;
}

template< typename T > 
inline T Matrix4< T >::det() const
{
    return getDeterminant();
}

template< typename T > 
T Matrix4< T >::getDeterminant() const
{
    return m00 * getMinor( 1, 2, 3, 1, 2, 3 ) 
         - m01 * getMinor( 1, 2, 3, 0, 2, 3 ) 
         + m02 * getMinor( 1, 2, 3, 0, 1, 3 ) 
         - m03 * getMinor( 1, 2, 3, 0, 1, 2 ); 
}

template< typename T > 
inline Matrix4< T > Matrix4< T >::getAdjoint() const
{
    return getAdjugate();
}

template< typename T >
Matrix4< T > Matrix4< T >::getAdjugate() const 
{
    return Matrix4(    // rows  // cols 
             getMinor( 1, 2, 3, 1, 2, 3 ), //0,0
            -getMinor( 0, 2, 3, 1, 2, 3 ), //1,0
             getMinor( 0, 1, 3, 1, 2, 3 ), //2,0
            -getMinor( 0, 1, 2, 1, 2, 3 ), //3,0
            -getMinor( 1, 2, 3, 0, 2, 3 ), //0,1
             getMinor( 0, 2, 3, 0, 2, 3 ), //1,1
            -getMinor( 0, 1, 3, 0, 2, 3 ), //2,1
             getMinor( 0, 1, 2, 0, 2, 3 ), //3,1
             getMinor( 1, 2, 3, 0, 1, 3 ), //0,2
            -getMinor( 0, 2, 3, 0, 1, 3 ), //1,2
             getMinor( 0, 1, 3, 0, 1, 3 ), //2,2
            -getMinor( 0, 1, 2, 0, 1, 3 ), //3,2
            -getMinor( 1, 2, 3, 0, 1, 2 ), //0,3
             getMinor( 0, 2, 3, 0, 1, 2 ), //1,3
            -getMinor( 0, 1, 3, 0, 1, 2 ), //2,3
             getMinor( 0, 1, 2, 0, 1, 2 )  //3,3
             );

#if 0 
// not transposed
    return Matrix4(    // rows  // cols 
             getMinor( 1, 2, 3, 1, 2, 3 ), //0,0
            -getMinor( 1, 2, 3, 0, 2, 3 ), //0,1
             getMinor( 1, 2, 3, 0, 1, 3 ), //0,2
            -getMinor( 1, 2, 3, 0, 1, 2 ), //0,3
            -getMinor( 0, 2, 3, 1, 2, 3 ), //1,0
             getMinor( 0, 2, 3, 0, 2, 3 ), //1,1
            -getMinor( 0, 2, 3, 0, 1, 3 ), //1,2
             getMinor( 0, 2, 3, 0, 1, 2 ), //1,3
             getMinor( 0, 1, 3, 1, 2, 3 ), //2,0
            -getMinor( 0, 1, 3, 0, 2, 3 ), //2,1
             getMinor( 0, 1, 3, 0, 1, 3 ), //2,2
            -getMinor( 0, 1, 3, 0, 1, 2 ), //2,3
            -getMinor( 0, 1, 2, 1, 2, 3 ), //3,0
             getMinor( 0, 1, 2, 0, 2, 3 ), //3,1
            -getMinor( 0, 1, 2, 0, 1, 3 ), //3,2
             getMinor( 0, 1, 2, 0, 1, 2 )  //3,3
             );
#endif
}

template< typename T > 
Matrix4< T > Matrix4< T >::getInverse( bool& isInvertible, T limit )
{
    Matrix4< T > tmp; 
    isInvertible = getInverse( tmp, limit );
    return tmp;
}

template< typename T > 
bool Matrix4< T >::getInverse( Matrix4< T >& result, T limit )
{
    T det = getDeterminant();
    if ( fabs(det) <= limit ) 
        return false;
    else
        result = getAdjugate() * ( 1. / det );
    return true;
}

template< typename T >
void Matrix4<T>::rotateX( const T angle )
{
    //matrix multiplication: ml = ml * rotation x axis
    const T sinus = sin(angle);
    const T cosin = cos(angle);

    T temp = m[0][0];
    m[0][0] = m[0][0] * cosin - m[0][2] * sinus;
    m[0][2] = temp    * sinus + m[0][2] * cosin;

    temp = m[1][0];
    m[1][0] = m[1][0] * cosin - m[1][2] * sinus;
    m[1][2] = temp    * sinus + m[1][2] * cosin;

    temp = m[2][0];
    m[2][0] = m[2][0] * cosin - m[2][2] * sinus;
    m[2][2] = temp    * sinus + m[2][2] * cosin;

    temp = m[3][0];
    m[3][0] = m[3][0] * cosin - m[3][2] * sinus;
    m[3][2] = temp    * sinus + m[3][2] * cosin;
}

template<>
inline void Matrix4<float>::rotateX( const float angle )
{
    //matrix multiplication: ml = ml * rotation x axis
    const float sinus = sinf(angle);
    const float cosin = cosf(angle);

    float temp = m[0][0];
    m[0][0] = m[0][0] * cosin - m[0][2] * sinus;
    m[0][2] = temp    * sinus + m[0][2] * cosin;

    temp = m[1][0];
    m[1][0] = m[1][0] * cosin - m[1][2] * sinus;
    m[1][2] = temp    * sinus + m[1][2] * cosin;

    temp = m[2][0];
    m[2][0] = m[2][0] * cosin - m[2][2] * sinus;
    m[2][2] = temp    * sinus + m[2][2] * cosin;

    temp = m[3][0];
    m[3][0] = m[3][0] * cosin - m[3][2] * sinus;
    m[3][2] = temp    * sinus + m[3][2] * cosin;
}

template< typename T >
void Matrix4<T>::rotateY( const T angle )
{
    //matrix multiplication: ml = ml * rotation y axis
    const T sinus = sin(angle);
    const T cosin = cos(angle);

    T temp = m[0][1];
    m[0][1] = m[0][1] *  cosin + m[0][2] * sinus;
    m[0][2] = temp    * -sinus + m[0][2] * cosin;

    temp = m[1][1];
    m[1][1] = m[1][1] *  cosin + m[1][2] * sinus;
    m[1][2] = temp    * -sinus + m[1][2] * cosin;

    temp = m[2][1];
    m[2][1] = m[2][1] *  cosin + m[2][2] * sinus;
    m[2][2] = temp    * -sinus + m[2][2] * cosin;

    temp = m[3][1];
    m[3][1] = m[3][1] *  cosin + m[3][2] * sinus;
    m[3][2] = temp    * -sinus + m[3][2] * cosin;
}

template<>
inline void Matrix4<float>::rotateY( const float angle )
{
    //matrix multiplication: ml = ml * rotation y axis
    const float sinus = sinf(angle);
    const float cosin = cosf(angle);

    float temp = m[0][1];
    m[0][1] = m[0][1] *  cosin + m[0][2] * sinus;
    m[0][2] = temp    * -sinus + m[0][2] * cosin;

    temp = m[1][1];
    m[1][1] = m[1][1] *  cosin + m[1][2] * sinus;
    m[1][2] = temp    * -sinus + m[1][2] * cosin;

    temp = m[2][1];
    m[2][1] = m[2][1] *  cosin + m[2][2] * sinus;
    m[2][2] = temp    * -sinus + m[2][2] * cosin;

    temp = m[3][1];
    m[3][1] = m[3][1] *  cosin + m[3][2] * sinus;
    m[3][2] = temp    * -sinus + m[3][2] * cosin;
}

template< typename T >
void Matrix4<T>::rotateZ( const T angle )
{
    //matrix multiplication: ml = ml * rotation z axis
    const T sinus = sin(angle);
    const T cosin = cos(angle);

    T temp = m[0][0];
    m[0][0] = m[0][0] *  cosin + m[0][1] * sinus;
    m[0][1] = temp    * -sinus + m[0][1] * cosin;

    temp = m[1][0];
    m[1][0] = m[1][0] *  cosin + m[1][1] * sinus;
    m[1][1] = temp    * -sinus + m[1][1] * cosin;

    temp = m[2][0];
    m[2][0] = m[2][0] *  cosin + m[2][1] * sinus;
    m[2][1] = temp    * -sinus + m[2][1] * cosin;

    temp = m[3][0];
    m[3][0] = m[3][0] *  cosin + m[3][1] * sinus;
    m[3][1] = temp    * -sinus + m[3][1] * cosin;
}

template<>
inline void Matrix4<float>::rotateZ( const float angle )
{
    //matrix multiplication: ml = ml * rotation z axis
    const float sinus = sinf(angle);
    const float cosin = cosf(angle);

    float temp = m[0][0];
    m[0][0] = m[0][0] *  cosin + m[0][1] * sinus;
    m[0][1] = temp    * -sinus + m[0][1] * cosin;

    temp = m[1][0];
    m[1][0] = m[1][0] *  cosin + m[1][1] * sinus;
    m[1][1] = temp    * -sinus + m[1][1] * cosin;

    temp = m[2][0];
    m[2][0] = m[2][0] *  cosin + m[2][1] * sinus;
    m[2][1] = temp    * -sinus + m[2][1] * cosin;

    temp = m[3][0];
    m[3][0] = m[3][0] *  cosin + m[3][1] * sinus;
    m[3][1] = temp    * -sinus + m[3][1] * cosin;
}

template< typename T >
void Matrix4<T>::scale( const T scale[3] )
{
    ml[0]  *= scale[0];
    ml[1]  *= scale[0];
    ml[2]  *= scale[0];
    ml[3]  *= scale[0];
    ml[4]  *= scale[1];
    ml[5]  *= scale[1];
    ml[6]  *= scale[1];
    ml[7]  *= scale[1];
    ml[8]  *= scale[2];
    ml[9]  *= scale[2];
    ml[10] *= scale[2];
    ml[11] *= scale[2];
}

template< typename T >
void Matrix4<T>::scaleTranslation( const T scale[3] )
{
    ml[12] *= scale[0];
    ml[13] *= scale[1];
    ml[14] *= scale[2];
}

template< typename T >
void Matrix4<T>::setTranslation( const T x, const T y, const T z )
{
    ml[12] = x;
    ml[13] = y;
    ml[14] = z;
}
template< typename T >
void Matrix4<T>::setTranslation( const T trans[3] )
{
    ml[12] = trans[0];
    ml[13] = trans[1];
    ml[14] = trans[2];
}
template< typename T >
void Matrix4<T>::setTranslation( const Vector3<T>& trans )
{
    ml[12] = trans.x;
    ml[13] = trans.y;
    ml[14] = trans.z;
}


template< typename T > 
void Matrix4< T >::tensor( const Vector3< T >& u, 
                              const Vector3< T >& v )
{
    int i, j;
    for (j = 0; j < 3; j++) 
    {
        for (i = 0; i < 3; i++)
            m[i][j] = u[j] * v[i];
        m[3][j] = u[j];
    }
    for (i = 0; i < 3; i++)
        m[i][3] = v[i];
    m[3][3] = 1.0;    
}

template< typename T > 
void Matrix4< T >::tensor( const Vector4< T >& u,
                              const Vector4< T >& v )
{
    int i, j;
    for (j = 0; j < 4; j++)
        for (i = 0; i < 4; i++)
            m[i][j] = u[j] * v[i];

}

template< typename T > 
T Matrix4< T >::getMinor( const size_t removeRow, const size_t removeCol ) const
{
    size_t col[3], c = 0;
    size_t row[3], r = 0;
    for ( size_t i = 0; i < 3; ++i )
    {
        if ( c == removeCol )
            ++c;
        col[i] = c++;
        if ( r == removeRow )
            ++r;
        row[i] = r++;
    }
    
    Matrix3< T > minorm( m[col[0]][row[0]], m[col[1]][row[0]], m[col[2]][row[0]], 
                         m[col[0]][row[1]], m[col[1]][row[1]], m[col[2]][row[1]],
                         m[col[0]][row[2]], m[col[1]][row[2]], m[col[2]][row[2]] );
    return minorm.det();
}


template< typename T > 
T Matrix4< T >::getMinor( const size_t row0, const size_t row1,
                       const size_t row2, const size_t col0,
                       const size_t col1, const size_t col2 ) const
{
    Matrix3< T > minorm( m[col0][row0], m[col1][row0], m[col2][row0], 
                         m[col0][row1], m[col1][row1], m[col2][row1],
                         m[col0][row2], m[col1][row2], m[col2][row2] );
    return minorm.det();
}


template< typename T > 
Matrix4< T > Matrix4< T >::operator-() const
{
    Matrix4< T > result( *this );
    result *= -1.0;
    return result;
}

template< typename T > 
Matrix4< T > Matrix4< T >::negate() const
{
    Matrix4< T > result( *this );
    result *= -1.0;
    return result;
}
}
#endif
