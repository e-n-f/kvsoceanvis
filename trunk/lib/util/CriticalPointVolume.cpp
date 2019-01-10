/*****************************************************************************/
/**
 *  @file   CriticalPointVolume.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: CriticalPointVolume.cpp 376 2014-05-27 06:41:31Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "CriticalPointVolume.h"
#include <kvs/Matrix33>
#include <algorithm>
#include <vector>


namespace
{

float Interpolate2( float a, float b, float factor )
{
    return ( 1 - factor ) * a + factor * b;
}

float Interpolate( const float* values, const kvs::Vector3f& local )
{
    float value0 = Interpolate2( values[0], values[1], local.x() );
    float value1 = Interpolate2( values[2], values[3], local.x() );
    float value2 = Interpolate2( value0, value1, local.y() );
    float value3 = Interpolate2( values[4], values[5], local.x() );
    float value4 = Interpolate2( values[6], values[7], local.x() );
    float value5 = Interpolate2( value3, value4, local.y() );
    float value = Interpolate2( value2, value5, local.z() );
    return value;
}

float Dudx( const float* u, const kvs::Vector3f& local )
{
    float dudx0 = Interpolate2( ( - u[0] + u[1] ), ( - u[2] + u[3] ), local.y() );
    float dudx1 = Interpolate2( ( - u[4] + u[5] ), ( - u[6] + u[7] ), local.y() );
    float dudx = Interpolate2( dudx0, dudx1, local.z() );
    return dudx;
}

float Dudy( const float* u, const kvs::Vector3f& local )
{
    float dudy0 = Interpolate2( ( - u[0] + u[2] ), ( - u[4] + u[6] ), local.z() );
    float dudy1 = Interpolate2( ( - u[1] + u[3] ), ( - u[5] + u[7] ), local.z() );
    float dudy = Interpolate2( dudy0, dudy1, local.x() );
    return dudy;
}

float Dudz( const float* u, const kvs::Vector3f& local )
{
    float dudz0 = Interpolate2( ( - u[0] + u[4] ), ( - u[1] + u[5] ), local.x() );
    float dudz1 = Interpolate2( ( - u[2] + u[6] ), ( - u[3] + u[7] ), local.x() );
    float dudz = Interpolate2( dudz0, dudz1, local.y() );
    return dudz;
}

float Dvdx( const float* v, const kvs::Vector3f& local )
{
    float dvdx0 = Interpolate2( ( - v[0] + v[1] ), ( - v[2] + v[3] ), local.y() );
    float dvdx1 = Interpolate2( ( - v[4] + v[5] ), ( - v[6] + v[7] ), local.y() );
    float dvdx = Interpolate2( dvdx0, dvdx1, local.z() );
    return dvdx;
}

float Dvdy( const float* v, const kvs::Vector3f& local )
{
    float dvdy0 = Interpolate2( ( - v[0] + v[2] ), ( - v[4] + v[6] ), local.z() );
    float dvdy1 = Interpolate2( ( - v[1] + v[3] ), ( - v[5] + v[7] ), local.z() );
    float dvdy = Interpolate2( dvdy0, dvdy1, local.x() );
    return dvdy;
}

float Dvdz( const float* v, const kvs::Vector3f& local )
{
    float dvdz0 = Interpolate2( ( - v[0] + v[4] ), ( - v[1] + v[5] ), local.x() );
    float dvdz1 = Interpolate2( ( - v[2] + v[6] ), ( - v[3] + v[7] ), local.x() );
    float dvdz = Interpolate2( dvdz0, dvdz1, local.y() );
    return dvdz;
}

float Dwdx( const float* w, const kvs::Vector3f& local )
{
    float dwdx0 = Interpolate2( ( - w[0] + w[1] ), ( - w[2] + w[3] ), local.y() );
    float dwdx1 = Interpolate2( ( - w[4] + w[5] ), ( - w[6] + w[7] ), local.y() );
    float dwdx = Interpolate2( dwdx0, dwdx1, local.z() );
    return dwdx;
}

float Dwdy( const float* w, const kvs::Vector3f& local )
{
    float dwdy0 = Interpolate2( ( - w[0] + w[2] ), ( - w[4] + w[6] ), local.z() );
    float dwdy1 = Interpolate2( ( - w[1] + w[3] ), ( - w[5] + w[7] ), local.z() );
    float dwdy = Interpolate2( dwdy0, dwdy1, local.x() );
    return dwdy;
}

float Dwdz( const float* w, const kvs::Vector3f& local )
{
    float dwdz0 = Interpolate2( ( - w[0] + w[4] ), ( - w[1] + w[5] ), local.x() );
    float dwdz1 = Interpolate2( ( - w[2] + w[6] ), ( - w[3] + w[7] ), local.x() );
    float dwdz = Interpolate2( dwdz0, dwdz1, local.y() );
    return dwdz;
}

bool IsIncluded( const kvs::Vector3f& p )
{
    return p.x() >= 0 && p.x() < 1 &&
           p.y() >= 0 && p.y() < 1 &&
           p.z() >= 0 && p.z() < 1;
}

bool IsVortex( const kvs::Matrix33f& mat )
{
    const float a3 = 1;
    const float a2 = - mat.trace();
    const float a1 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]
        + mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]
        + mat[2][2] * mat[0][0] - mat[2][0] * mat[0][2];
    const float a0 = - mat.determinant();

    return - 4 * (a1 * a1 * a1) * a3
        + (a1 * a1) * (a2 * a2)
        - 4 * a0 * (a2 * a2 * a2)
        + 18 * a0 * a1 * a2 * a3
        - 27 * (a0 * a0) * (a3 * a3) < 0;
}

kvs::Matrix33f VelocityGradientTensor(const float* values, const kvs::Vector3f& local)
{
    const float* u = values + 0;
    const float* v = values + 8;
    const float* w = values + 16;

    kvs::Matrix33f ret;
    ret[0][0] = Dudx( u, local );
    ret[0][1] = Dudy( u, local );
    ret[0][2] = Dudz( u, local );
    ret[1][0] = Dvdx( v, local );
    ret[1][1] = Dvdy( v, local );
    ret[1][2] = Dvdz( v, local );
    ret[2][0] = Dwdx( w, local );
    ret[2][1] = Dwdy( w, local );
    ret[2][2] = Dwdz( w, local );

    return ret;
}

kvs::Vector3f InterpolatedVector(const float* values, const kvs::Vector3f& local)
{
    const float* u = values + 0;
    const float* v = values + 8;
    const float* w = values + 16;

    kvs::Vector3f ret;
    ret[0] = Interpolate( u, local );
    ret[1] = Interpolate( v, local );
    ret[2] = Interpolate( w, local );

    return ret;
}

bool FindCriticalPoint( const float* values, kvs::Vector3f* point, kvs::Matrix33f* tensor )
{
    const int max_loop = 1000;

    kvs::Vector3f current( 0.5f, 0.5f, 0.5f );
    for (int i = 0; i < max_loop; ++i)
    {
        const kvs::Matrix33f mat = VelocityGradientTensor( values, current );
        const kvs::Vector3f vec = InterpolatedVector( values, current );
        const kvs::Vector3f diff = -mat.inverted() * vec;
        if ( diff.length() < 0.001 )
        {
//            if ( !IsIncluded( current ) ) return false;
            if ( !IsIncluded( current ) || !IsVortex( mat ) ) return false;

            *point = current;
            *tensor = mat;
            return true;
        }

        current += diff;
    }

    return false;
}

}


namespace kvsoceanvis
{

namespace util
{

CriticalPointVolume::CriticalPointVolume( const kvs::StructuredVolumeObject* volume ):
    m_has_ignore_value( false ),
    m_ignore_value( 0.0f )
{
    this->exec( volume );
}

CriticalPointVolume::CriticalPointVolume( const kvs::StructuredVolumeObject* volume, const float ignore_value )
{
    this->setIgnoreValue( ignore_value );
    this->exec( volume );
}

void CriticalPointVolume::setIgnoreValue( const float ignore_value )
{
    m_has_ignore_value = true;
    m_ignore_value = ignore_value;
}

CriticalPointVolume::SuperClass* CriticalPointVolume::exec( const kvs::ObjectBase* object )
{
    const kvs::StructuredVolumeObject* volume = kvs::StructuredVolumeObject::DownCast( object );

    const size_t dimx = volume->resolution().x();
    const size_t dimy = volume->resolution().y();
    const size_t dimz = volume->resolution().z();

    std::vector<float> coords;
    std::vector<float> values;

    const size_t veclen = volume->veclen();
    const float* pvalues = static_cast<const float*>( volume->values().data() );
    if ( volume->gridType() == kvs::StructuredVolumeObject::Uniform )
    {
        for ( size_t k = 0; k < dimz - 1; k++ )
        {
            const float z0 = k;

            for ( size_t j = 0; j < dimy - 1; j++ )
            {
                const float y0 = j;
                for ( size_t i = 0; i < dimx - 1; i++ )
                {
                    const float x0 = i;

                    /*  p2 ------- p3
                     *   |         |    p0: (x0, y0)
                     *   |         |    p1: (x1, y1)
                     *   |         |    p2: (x2, y2)
                     *  p0 ------- p1   p3: (x3, y3)
                     */

                    size_t index[8];
                    index[0] = i + dimx * j + dimx * dimy * k;  // index of (x0, y0) dimx * j + i
                    index[1] = index[0] + 1;    // index of (x1, y0)
                    index[2] = index[0] + dimx; // index of (x0, y1)
                    index[3] = index[2] + 1;    // index of (x1, y1)
                    index[4] = index[0] + dimx * dimy; // index of (x1, y1)
                    index[5] = index[4] + 1;    // index of (x1, y1)
                    index[6] = index[4] + dimx; // index of (x1, y1)
                    index[7] = index[6] + 1;    // index of (x1, y1)

                    float data[24];
                    float* u = data + 0;
                    float* v = data + 8;
                    float* w = data + 16;
                    for (size_t a = 0; a < 8; ++a)
                    {
                        u[a] = pvalues[ index[a] * veclen + 0 ];
                        v[a] = pvalues[ index[a] * veclen + 1 ];
                        w[a] = pvalues[ index[a] * veclen + 2 ];
                    }
                    if ( this->include_ignore_value( data ) ) continue;

                    kvs::Vector3f local;
                    kvs::Matrix33f tensor;
                    if ( ::FindCriticalPoint( data, &local, &tensor ) )
                    {
                        kvs::Vector3f global = kvs::Vector3f(x0,y0,z0) + local;
                        coords.push_back( global[0] );
                        coords.push_back( global[1] );
                        coords.push_back( global[2] );
                        values.push_back( tensor[0][0] );
                        values.push_back( tensor[0][1] );
                        values.push_back( tensor[0][2] );
                        values.push_back( tensor[1][0] );
                        values.push_back( tensor[1][1] );
                        values.push_back( tensor[1][2] );
                        values.push_back( tensor[2][0] );
                        values.push_back( tensor[2][1] );
                        values.push_back( tensor[2][2] );
                    }
                }
            }
        }
    }
    else if ( kvs::StructuredVolumeObject::Rectilinear )
    {
        const kvs::Real32* xcoords = volume->coords().pointer();
        const kvs::Real32* ycoords = xcoords + volume->resolution().x();
        const kvs::Real32* zcoords = ycoords + volume->resolution().y();

        for ( size_t k = 0; k < dimz - 1; k++ )
        {
            const float z0 = zcoords[ k ];
            const float zdiff = zcoords[ k + 1 ] - zcoords[ k ];
            for ( size_t j = 0; j < dimy - 1; j++ )
            {
                const float y0 = ycoords[ j ];
                const float ydiff = ycoords[ j + 1 ] - ycoords[ j ];
                for ( size_t i = 0; i < dimx - 1; i++ )
                {
                    const float x0 = xcoords[ i ];
                    const float xdiff = xcoords[ i + 1 ] - xcoords[ i ];

                    /*  p2 ------- p3
                     *   |         |    p0: (x0, y0)
                     *   |         |    p1: (x1, y1)
                     *   |         |    p2: (x2, y2)
                     *  p0 ------- p1   p3: (x3, y3)
                     */

                    size_t index[8];
                    index[0] = i + dimx * j + dimx * dimy * k;  // index of (x0, y0) dimx * j + i
                    index[1] = index[0] + 1;    // index of (x1, y0)
                    index[2] = index[0] + dimx; // index of (x0, y1)
                    index[3] = index[2] + 1;    // index of (x1, y1)
                    index[4] = index[0] + dimx * dimy; // index of (x1, y1)
                    index[5] = index[4] + 1;    // index of (x1, y1)
                    index[6] = index[4] + dimx; // index of (x1, y1)
                    index[7] = index[6] + 1;    // index of (x1, y1)

                    float data[24];
                    float* u = data + 0;
                    float* v = data + 8;
                    float* w = data + 16;
                    for (size_t a = 0; a < 8; ++a)
                    {
                        u[a] = pvalues[ index[a] * veclen + 0 ];
                        v[a] = pvalues[ index[a] * veclen + 1 ];
                        w[a] = pvalues[ index[a] * veclen + 2 ];
                    }
                    if ( this->include_ignore_value( data ) ) continue;

                    kvs::Vector3f local;
                    kvs::Matrix33f tensor;
                    if ( ::FindCriticalPoint( data, &local, &tensor ) )
                    {
                        kvs::Vector3f d = kvs::Vector3f( local.x() * xdiff, local.y() * ydiff, local.z() * zdiff );
                        kvs::Vector3f global = kvs::Vector3f(x0,y0,z0) + d;
                        coords.push_back( global[0] );
                        coords.push_back( global[1] );
                        coords.push_back( global[2] );
                        values.push_back( tensor[0][0] );
                        values.push_back( tensor[0][1] );
                        values.push_back( tensor[0][2] );
                        values.push_back( tensor[1][0] );
                        values.push_back( tensor[1][1] );
                        values.push_back( tensor[1][2] );
                        values.push_back( tensor[2][0] );
                        values.push_back( tensor[2][1] );
                        values.push_back( tensor[2][2] );
                    }
                }
            }
        }
    }

    SuperClass::setVeclen( 9 );
    SuperClass::setCellType( kvs::UnstructuredVolumeObject::Point );
    SuperClass::setNumberOfNodes( coords.size() / 3 );
    SuperClass::setNumberOfCells( coords.size() / 3 );
    SuperClass::setCoords( kvs::ValueArray<float>( coords ) );
    SuperClass::setValues( kvs::AnyValueArray( kvs::ValueArray<float>( values ) ) );
    SuperClass::setMinMaxObjectCoords( volume->minObjectCoord(), volume->maxObjectCoord() );
    SuperClass::setMinMaxExternalCoords( volume->minExternalCoord(), volume->maxExternalCoord() );

    return this;
}

bool CriticalPointVolume::include_ignore_value( const float* values ) const
{
    if ( m_has_ignore_value )
    {
        return std::find( values, values + 24, m_ignore_value) != (values + 24);
    }

    return false;
}

} // end of namespace util

} // end of namespace kvsoceanvis
