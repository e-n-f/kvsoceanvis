/*****************************************************************************/
/**
 *  @file   MultiDimensionalScaling.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
#include "MultiDimensionalScaling.h"
#include <kvs/Matrix>
#include <kvs/EigenDecomposer>
#include <kvs/Timer>


namespace
{

kvs::Real32 GetEuclideanDistance( const kvs::TableObject* table, const size_t i, const size_t j )
{
    kvs::Real32 distance = 0.0f;
    const size_t ncolumns = table->numberOfColumns();
    for ( size_t index = 0; index < ncolumns; index++ )
    {
        const kvs::Real32 xi = table->column(index).at<kvs::Real32>(i);
        const kvs::Real32 xj = table->column(index).at<kvs::Real32>(j);
        const kvs::Real32 diff = xi - xj;
        distance += diff * diff;
    }

    return distance;
}

void ToIdentify( kvs::Matrix<kvs::Real32>& m )
{
    m.identity();
}

void ToOnes( kvs::Matrix<kvs::Real32>& m )
{
    const size_t ncolumns = m.columnSize();
    const size_t nrows = m.rowSize();
    for ( size_t i = 0; i < ncolumns; i++ ) for ( size_t j = 0; j < nrows; j++ ) m[i][j] = 1.0f;
}

void ToSquare( kvs::Matrix<kvs::Real32>& m )
{
    const size_t ncolumns = m.columnSize();
    const size_t nrows = m.rowSize();
    for ( size_t i = 0; i < ncolumns; i++ ) for ( size_t j = 0; j < nrows; j++ ) m[i][j] = m[i][j] * m[i][j];

}

}

namespace kvsoceanvis
{

namespace pcs
{

MultiDimensionalScaling::MultiDimensionalScaling( void )
{
}

MultiDimensionalScaling::MultiDimensionalScaling( const kvs::TableObject* object )
{
    this->exec( object );
}

MultiDimensionalScaling::MultiDimensionalScaling( const kvs::Matrix<kvs::Real32>& distance_matrix )
{
    this->setDistanceMatrix( distance_matrix );
    this->exec( NULL );
}

void MultiDimensionalScaling::setDistanceMatrix( const kvs::Matrix<kvs::Real32>& distance_matrix )
{
    m_distance_matrix = distance_matrix;
}

MultiDimensionalScaling::SuperClass* MultiDimensionalScaling::exec( const kvs::ObjectBase* object )
{
    if ( object )
    {
        const kvs::TableObject* table = static_cast<const kvs::TableObject*>( object );
        const size_t N = table->numberOfRows();

        // Dissimilarity (distance) matrix (D)
        m_distance_matrix.setSize( N, N );
        m_distance_matrix.zero();
        kvs::Matrix<kvs::Real32>& D = m_distance_matrix;
        for ( size_t i = 0; i < N; i++ )
        {
            for ( size_t j = i + 1; j < N; j++ )
            {
                D[i][j] = std::sqrt( ::GetEuclideanDistance( table, i, j ) );
                D[j][i] = D[i][j];
            }
        }
    }

    kvs::Matrix<kvs::Real32>& D = m_distance_matrix;
    const size_t N = m_distance_matrix.rowSize();

    // Inner product matrix (P) can be solved by Young-Householder transformation.
    kvs::Matrix<kvs::Real32> I( N, N ); ::ToIdentify( I );
    kvs::Matrix<kvs::Real32> O( N, N ); ::ToOnes( O );
    kvs::Matrix<kvs::Real32> DD( D ); ::ToSquare( DD );
    kvs::Matrix<kvs::Real32> H = I - ( 1.0f / N ) * O; // centering matrix
    kvs::Matrix<kvs::Real32> P = -0.5f * H * DD * H; // inner product matrix

    // Eigen value decomposition.
    kvs::EigenDecomposer<kvs::Real32> eigen( P, kvs::EigenDecomposer<kvs::Real32>::Symmetric );
    const kvs::Vector<kvs::Real32>& evalues = eigen.eigenValues();
    const kvs::Matrix<kvs::Real32>& evectors = eigen.eigenVectors();

    // Location (X) in a 2D plane.
    kvs::Matrix<kvs::Real32> A( 2, 2 );
    A[0][0] = std::sqrt( evalues[0] ); A[0][1] = 0.0f;
    A[1][0] = 0.0f; A[1][1] = std::sqrt( evalues[1] );

    kvs::Matrix<kvs::Real32> Q( 2, evectors.columnSize() );
    Q[0] = evectors[0].normalized();
    Q[1] = evectors[1].normalized();

    kvs::Matrix<kvs::Real32> X = A * Q;
/*
    kvs::AnyValueArray x;
    kvs::AnyValueArray y;
    kvs::Real32* px = static_cast<kvs::Real32*>( x.allocate<kvs::Real32>(N) );
    kvs::Real32* py = static_cast<kvs::Real32*>( y.allocate<kvs::Real32>(N) );
*/
    kvs::ValueArray<kvs::Real32> x( N );
    kvs::ValueArray<kvs::Real32> y( N );
    kvs::Real32* px = x.data();
    kvs::Real32* py = y.data();
    for ( size_t i = 0; i < N; i++ )
    {
        px[i] = X[0][i];
        py[i] = X[1][i];
    }

    SuperClass::addColumn( kvs::AnyValueArray(x) );
    SuperClass::addColumn( kvs::AnyValueArray(y) );

    return this;
}

} // end of namespace pcs

} // end of namespace kvsoceanvis
