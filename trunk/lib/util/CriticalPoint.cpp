/*****************************************************************************/
/**
 *  @file   CriticalPoint.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: CriticalPoint.cpp 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "CriticalPoint.h"
#include <kvs/Matrix33>


namespace
{

float NewtonMethod(float a0, float a1, float a2, float a3)
{
    const float eps = 1.0e-4;
    const int max_times = 10000;

    float x = 1000;
    for ( int count = 0; count < max_times; count++ )
    {
        float fx = a3 * x * x * x + a2 * x * x + a1 * x + a0;
        float dfx = 3 * a3 * x * x + 2 * a2 * x + a1;
        float diff = fx / dfx;
        if ( kvs::Math::Abs( diff ) < eps ) break;
        x -= diff;
    }

    return x;
}

kvs::Vector3f CalcEigenVector(const kvs::Matrix33f& mat, float eigen_value)
{
    kvs::Vector3f ret;
    kvs::Matrix33f temp;
    temp[0][0] -= eigen_value;
    temp[1][1] -= eigen_value;
    temp[2][2] -= eigen_value;

    if (temp[0][0] != 0)
    {
        temp[0] /= temp[0][0];
        temp[1] -= temp[1][0] * temp[0];
        temp[2] -= temp[2][0] * temp[0];

        if (temp[1][1] != 0)
        {
            temp[1] /= temp[1][1];
            temp[0] -= temp[0][1] * temp[1];
            temp[2] -= temp[2][1] * temp[1];

            ret.x() = -temp[0][2];
            ret.y() = -temp[1][2];
            ret.z() = 1;
        }
        else
        {
            std::cout << "error" << std::endl;
            return kvs::Vector3f(1,0,0);
        }
    }
    else
    {
        std::cout << "error" << std::endl;
        return kvs::Vector3f(1,0,0);
    }
    ret.normalize();
    return ret;
}

}


namespace kvsoceanvis
{

namespace util
{

CriticalPoint::CriticalPoint( const kvs::UnstructuredVolumeObject* volume )
{
    this->exec( volume );
}

CriticalPoint::SuperClass* CriticalPoint::exec( const kvs::ObjectBase* object )
{
    const kvs::UnstructuredVolumeObject* volume = kvs::UnstructuredVolumeObject::DownCast( object );
    if ( volume->cellType() != kvs::UnstructuredVolumeObject::Point )
    {
        kvsMessageError("Cell type isn't point element.");
        return this;
    }

    if ( volume->veclen() != 9 )
    {
        kvsMessageError("Data isn't tensor value.");
    }

    const float* pvalues = static_cast<const float*>( volume->values().data() );
    const size_t nnodes = volume->numberOfNodes();

    kvs::ValueArray<float> eigen_values( nnodes );
    kvs::ValueArray<float> eigen_vectors( nnodes * 3 );
    for ( size_t i = 0; i < nnodes; i++ )
    {
        const float m00 = *(pvalues++);
        const float m01 = *(pvalues++);
        const float m02 = *(pvalues++);
        const float m10 = *(pvalues++);
        const float m11 = *(pvalues++);
        const float m12 = *(pvalues++);
        const float m20 = *(pvalues++);
        const float m21 = *(pvalues++);
        const float m22 = *(pvalues++);

        const kvs::Matrix33f mat(
            m00, m01, m02,
            m10, m11, m12,
            m20, m21, m22 );

        const float a3 = 1;
        const float a2 = - mat.trace();
        const float a1 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]
            + mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]
            + mat[2][2] * mat[0][0] - mat[2][0] * mat[0][2];
        const float a0 = - mat.determinant();

        const float eigen_value = NewtonMethod( a0, a1, a2, a3 );
        const kvs::Vector3f eigen_vector = CalcEigenVector( mat, eigen_value );

        eigen_values[i] = eigen_value;
        eigen_vectors[3*i+0] = eigen_vector[0];
        eigen_vectors[3*i+1] = eigen_vector[1];
        eigen_vectors[3*i+2] = eigen_vector[2];
    }

    float min_eigen_value = kvs::Value<float>::Max();
    float max_eigen_value = kvs::Value<float>::Min();
    for ( size_t i = 0; i < eigen_values.size(); i++ )
    {
        min_eigen_value = kvs::Math::Min( min_eigen_value, eigen_values[i] );
        max_eigen_value = kvs::Math::Max( max_eigen_value, eigen_values[i] );
    }

    kvs::ColorMap cmap = BaseClass::transferFunction().colorMap();
    cmap.setRange( min_eigen_value, max_eigen_value );

    kvs::ValueArray<kvs::UInt8> colors( eigen_values.size() * 3 );
    kvs::UInt8* pcolors = colors.pointer();
    for ( size_t i = 0; i < eigen_values.size(); i++ )
    {
        kvs::RGBColor c = cmap.at( eigen_values[i] );
        *(pcolors++) = c.r();
        *(pcolors++) = c.g();
        *(pcolors++) = c.b();
    }

    SuperClass::setCoords( volume->coords() );
    SuperClass::setNormals( eigen_vectors );
    SuperClass::setColors( colors );
    SuperClass::setSize( 1 );
    SuperClass::setMinMaxObjectCoords( volume->minObjectCoord(), volume->maxObjectCoord() );
    SuperClass::setMinMaxExternalCoords( volume->minExternalCoord(), volume->maxExternalCoord() );

    return this;
}

} // end of namespace util

} // end of namespace kvsoceanvis
