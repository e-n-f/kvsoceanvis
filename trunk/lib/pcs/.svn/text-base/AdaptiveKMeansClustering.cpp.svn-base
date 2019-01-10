/*****************************************************************************/
/**
 *  @file   AdaptiveKMeansClustering.cpp
 *  @author Zhang Chi, Naohisa Sakamoto
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
#include "AdaptiveKMeansClustering.h"
#include "FastKMeansClustering.h"
#include <kvs/Matrix>
#include <cmath>


namespace
{

/*===========================================================================*/
/**
 *  @brief  Returns Mahalanobis distance.
 *  @param  x [in] multivariant vector
 *  @param  u [in] mean values
 *  @return Mahalanobis distance
 */
/*===========================================================================*/
kvs::Real32 GetMahalanobisDistance(
    const kvs::ValueArray<kvs::Real32>& x,
    const kvs::ValueArray<kvs::Real32>& u )
{
    const size_t dim = x.size();
    kvs::Matrix<kvs::Real32> d( dim, 1 );
    kvs::Matrix<kvs::Real32> dt( 1, dim );
    for ( size_t i = 0; i < dim; i++ ) { d[i][0] = dt[0][i] = x[i] - u[i]; }

    // Covariance matrix (identity matrix)
    kvs::Matrix<kvs::Real32> S( dim, dim );
    S.identity();

    // In this function, the Mahalanobis distance reduces to the Euclidean
    // distance since the covariance matrix is the identity matrix.
    return ( dt * S * d )[0][0];
}

/*===========================================================================*/
/**
 *  @brief  Returns row array.
 *  @param  table [in] table object
 *  @param  row_index [in] index of the row
 *  @return row array
 */
/*===========================================================================*/
kvs::ValueArray<kvs::Real32> GetRowArray(
    const kvs::TableObject* table,
    const size_t row_index )
{
    const size_t ncolumns = table->numberOfColumns();
    kvs::ValueArray<kvs::Real32> row( ncolumns );
    for ( size_t i = 0; i < ncolumns; i++ )
    {
        row[i] = table->column(i).at<kvs::Real32>( row_index );
    }

    return row;
}

}

namespace kvsoceanvis
{

namespace pcs
{

AdaptiveKMeansClustering::AdaptiveKMeansClustering():
    m_nclusters( 0 ),
    m_max_iterations( 100 ),
    m_tolerance( 1.e-6 ),
    m_max_nclusters( 10 )
{
}

AdaptiveKMeansClustering::AdaptiveKMeansClustering( const kvs::ObjectBase* object ):
    m_nclusters( 0 ),
    m_max_iterations( 100 ),
    m_tolerance( 1.e-6 ),
    m_max_nclusters( 10 )
{
    this->exec( object );
}

AdaptiveKMeansClustering::AdaptiveKMeansClustering(
    const kvs::ObjectBase* object,
    const size_t max_iterations,
    const float torelance ):
    m_nclusters( 0 ),
    m_max_iterations( max_iterations ),
    m_tolerance( torelance ),
    m_max_nclusters( 10 )
{
    this->exec( object );
}

AdaptiveKMeansClustering::AdaptiveKMeansClustering( const kvs::ObjectBase* object, const size_t max_nclusters ):
    m_nclusters( 0 ),
    m_max_iterations( 100 ),
    m_tolerance( 1.e-6 ),
    m_max_nclusters( max_nclusters )
{
    this->exec( object );
}

AdaptiveKMeansClustering::AdaptiveKMeansClustering(
    const kvs::ObjectBase* object,
    const size_t max_nclusters,
    const size_t max_iterations,
    const float torelance ):
    m_nclusters( 0 ),
    m_max_iterations( max_iterations ),
    m_tolerance( torelance ),
    m_max_nclusters( max_nclusters )
{
    this->exec( object );
}

AdaptiveKMeansClustering::SuperClass* AdaptiveKMeansClustering::exec( const kvs::ObjectBase* object )
{
    if ( !object )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input object is NULL.");
        return NULL;
    }

    const kvs::TableObject* table = static_cast<const kvs::TableObject*>( object );
    const size_t nrows = table->numberOfRows();
    const size_t ncolumns = table->numberOfColumns();

    const size_t K = m_max_nclusters; // number of clusters
    const size_t p = ncolumns; // p-dimension
    const kvs::Real32 Y = p * 0.5f; // transformation power

    size_t nclusters = 1; // number of clusters (best k)
    kvs::Real32 Jmax = 0.0f; // maximum jump
    kvs::ValueArray<kvs::UInt32> IDs; // cluster IDs with the best k
    kvs::ValueArray<kvs::Real32> d( K + 1 ); d[0] = 0.0f; // distortion
    for ( size_t k = 1; k < K + 1; k++ )
    {
        // k-means clustering.
        kvsoceanvis::pcs::FastKMeansClustering* clustered_table = new kvsoceanvis::pcs::FastKMeansClustering();
        clustered_table->setSeedingMethod( kvsoceanvis::pcs::FastKMeansClustering::SmartSeeding );
        clustered_table->setNumberOfClusters( k );
        clustered_table->setMaxIterations( m_max_iterations );
        clustered_table->setTolerance( m_tolerance );
        clustered_table->exec( table );

        // Calculate the distortions (averaged Mahalanobis distance per dimension).
        d[k] = 0.0f;
        for ( size_t i = 0; i < nrows; i++ )
        {
            kvs::ValueArray<kvs::Real32> cx = clustered_table->center(0);
            kvs::ValueArray<kvs::Real32> x = ::GetRowArray( table, i );
            kvs::Real32 distance = ::GetMahalanobisDistance( x, cx );
            for ( size_t j = 1; j < k; j++ )
            {
                cx = clustered_table->center(j);
                distance = kvs::Math::Min( distance, ::GetMahalanobisDistance( x, cx ) );
            }
            d[k] += distance;
        }
        d[k] = ( 1.0f / p ) * ( ( 1.0f / nrows ) * d[k] );

        // Calculate jump in transformed distortion.
        kvs::Real32 Jk = std::pow( d[k], -Y ) - std::pow( d[k-1], -Y );
        if ( Jk > Jmax )
        {
            // Update the number of clusters and store the clustering result (IDs).
            // Finally, the number of clusters can be calculated as the largest jump (argmax J[k]).
            nclusters = k;
            Jmax = Jk;
            IDs = clustered_table->column( p ).asValueArray<kvs::UInt32>();
        }

        delete clustered_table;
    }

    m_nclusters = nclusters;
    m_distortions = d;

    // Set the results to the output table object.
    for ( size_t i = 0; i < ncolumns; i++ )
    {
        const std::string label = table->label(i);
        const kvs::AnyValueArray& column = table->column(i);
        SuperClass::addColumn( column, label );
    }
    SuperClass::addColumn( kvs::AnyValueArray( IDs ), "Cluster ID" );

    return this;
}

void AdaptiveKMeansClustering::setMaxNumberOfClusters( const size_t max_nclusters )
{
    m_max_nclusters = max_nclusters;
}

void AdaptiveKMeansClustering::setMaxIterations( const size_t max_iterations )
{
    m_max_iterations = max_iterations;
}

void AdaptiveKMeansClustering::setTolerance( const float tolerance )
{
    m_tolerance = tolerance;
}

size_t AdaptiveKMeansClustering::numberOfClusters() const
{
    return m_nclusters;
}

size_t AdaptiveKMeansClustering::maxNumberOfClusters() const
{
    return m_max_nclusters;
}

const kvs::ValueArray<kvs::Real32>& AdaptiveKMeansClustering::distortions() const
{
    return m_distortions;
}

kvs::ValueArray<kvs::Real32> AdaptiveKMeansClustering::jumps() const
{
    if ( m_distortions.size() != m_max_nclusters + 1 ) return kvs::ValueArray<kvs::Real32>();

    const kvs::ValueArray<kvs::Real32>& d = m_distortions;
    const size_t p = SuperClass::numberOfColumns() - 1;
    const kvs::Real32 Y = p * 0.5f;
    kvs::ValueArray<kvs::Real32> J( m_max_nclusters + 1 ); J[0] = 0.0f;
    for ( size_t k = 1; k < m_max_nclusters + 1; k++ )
    {
        J[k] = std::pow( d[k], -Y ) - std::pow( d[k-1], -Y );
    }

    return J;
}

} // end of namespace pcs

} // end of namespace kvsoceanvis
