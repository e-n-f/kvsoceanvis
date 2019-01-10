/*****************************************************************************/
/**
 *  @file   KMeansClustering.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: KMeansClustering.cpp 376 2014-05-27 06:41:31Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "KMeansClustering.h"
#include <kvs/Value>


namespace
{

kvs::Real32 GetEuclideanDistance(
    const kvs::TableObject* table,
    const kvs::UInt16 row_index,
    const kvs::ValueArray<kvs::Real32>& center )
{
    kvs::Real32 distance = 0.0;
    for ( size_t i = 0; i < table->numberOfColumns(); i++ )
    {
        const kvs::Real32 x0 = center[i];
        const kvs::Real32 x1 = table->column(i).at<kvs::Real32>( row_index );
        distance += ( x1 - x0 ) * ( x1 - x0 );
    }

    return distance;
}

kvs::Real32 GetEuclideanDistance(
    const kvs::TableObject* table,
    const kvs::ValueArray<kvs::Real32>& center_old,
    const kvs::ValueArray<kvs::Real32>& center_new )
{
    kvs::Real32 distance = 0.0;
    for ( size_t i = 0; i < table->numberOfColumns(); i++ )
    {
        const kvs::Real32 x0 = center_old[i];
        const kvs::Real32 x1 = center_new[i];
        distance += ( x1 - x0 ) * ( x1 - x0 );
    }

    return distance;
}
}

namespace
{
/*===========================================================================*/
/**
 *  @brief  Calculates the cluster centroid.
 *  @param  table [in] table object
 *  @param  cluster_id [in] target cluster ID
 *  @param  ids [in] cluster ID array
 *  @param  center [out] cluster centroid
 */
/*===========================================================================*/
void CalculateCenter(
    const kvs::TableObject* table,
    const kvs::UInt16 cluster_id,
    const kvs::ValueArray<kvs::UInt16>& ids,
    kvs::ValueArray<kvs::Real32>* center )
{
    for ( size_t j = 0; j < table->numberOfColumns(); j++ ) { center->at(j) = 0.0; }

    size_t counter = 0;
    for ( size_t j = 0; j < table->numberOfRows(); j++ )
    {
        if ( ids[j] == cluster_id )
        {
            for ( size_t k = 0; k < table->numberOfColumns(); k++ )
            {
                center->at(k) += table->column(k).at<kvs::Real32>(j);
            }
            counter++;
        }
    }
    if ( counter != 0 )
    {
        for ( size_t k = 0; k < table->numberOfColumns(); k++ ) { center->at(k) /= counter; }
    }
}

/*===========================================================================*/
/**
 *  @brief  Initialize centers of clusters with random seeding method.
 *  @param  table [in] pointer to table
 *  @param  nclusters [in] number of clusters
 *  @param  ids [i]
 *  @param  centers [in/out] pointer to center array
 */
/*===========================================================================*/
void InitializeCentersWithRandomSeeding(
    const kvs::TableObject* table,
    const size_t nclusters,
    const kvs::ValueArray<kvs::UInt16>& ids,
    kvs::ValueArray<kvs::Real32>* centers )
{
    for ( size_t i = 0; i < nclusters; i++ )
    {
        CalculateCenter( table, i, ids, &centers[i] );
    }
}

/*===========================================================================*/
/**
 *  @brief  Initialize centers of clusters with k-means++.
 *  @param  table [in] pointer to table
 *  @param  nclusters [in] number of clusters
 *  @param  ids [i]
 *  @param  centers [in/out] pointer to center array
 */
/*===========================================================================*/
void InitializeCentersWithSmartSeeding(
    const kvs::TableObject* table,
    const size_t nclusters,
    const kvs::ValueArray<kvs::UInt16>& ids,
    kvs::ValueArray<kvs::Real32>* centers )
{
    CalculateCenter( table, 0, ids, &(centers[0]) );

    for ( size_t i = 1; i < nclusters; i++ )
    {
        kvs::Real32 S = 0.0;
        kvs::Real32 D[ table->numberOfRows() ];
        for ( size_t j = 0; j < table->numberOfRows(); j++ )
        {
            kvs::Real32 distance = kvs::Value<kvs::Real32>::Max();
            for ( size_t k = 0; k < nclusters; k++ )
            {
                const kvs::Real32 d = ::GetEuclideanDistance( table, j, centers[k] );
                if ( d < distance ) { distance = d; }
            }
            D[j] = distance;
            S += distance * distance;
        }

        size_t index = 0;
        kvs::Real32 P = 0.0;
        for ( size_t j = 0; j < table->numberOfRows(); j++ )
        {
            const kvs::Real32 DD = D[j] * D[j];
            if ( P < kvs::Math::Max( P, DD / S ) )
            {
                P = DD / S;
                index = j;
            }
        }

        for ( size_t j = 0; j < table->numberOfColumns(); j++ )
        {
            centers[i].at(j) = table->column(j).at<kvs::Real32>(index);
        }
    }
}

}


namespace kvsoceanvis
{

namespace pcs
{

KMeansClustering::KMeansClustering():
    m_seeding_method( KMeansClustering::RandomSeeding ),
    m_nclusters( 1 ),
    m_max_iterations( 100 ),
    m_tolerance( 1.e-6 ),
    m_centers( NULL )
{
}

KMeansClustering::KMeansClustering( const kvs::ObjectBase* object, const size_t nclusters ):
    m_seeding_method( KMeansClustering::RandomSeeding ),
    m_nclusters( nclusters ),
    m_max_iterations( 100 ),
    m_tolerance( 1.e-6 ),
    m_centers( NULL )
{
    this->exec( object );
}

KMeansClustering::KMeansClustering( const kvs::ObjectBase* object, const size_t nclusters, const size_t max_iterations, const float tolerance ):
    m_seeding_method( KMeansClustering::RandomSeeding ),
    m_nclusters( nclusters ),
    m_max_iterations( max_iterations ),
    m_tolerance( tolerance ),
    m_centers( NULL )
{
    this->exec( object );
}

KMeansClustering::~KMeansClustering()
{
    if ( m_centers ) delete [] m_centers;
}

KMeansClustering::SuperClass* KMeansClustering::exec( const kvs::ObjectBase* object )
{
    if ( !object )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input object is NULL.");
        return NULL;
    }

    const kvs::TableObject* table = static_cast<const kvs::TableObject*>( object );

    // Allocate memory for the cluster center.
    m_centers = new kvs::ValueArray<kvs::Real32> [ m_nclusters ];
    for ( size_t i = 0; i < m_nclusters; i++ ) { m_centers[i].allocate( table->numberOfColumns() ); }

    // Assign initial cluster IDs to each row of the input table randomly.
    kvs::ValueArray<kvs::UInt16> IDs( table->numberOfRows() );
    for ( size_t i = 0; i < table->numberOfRows(); i++ ) IDs[i] = kvs::UInt16( m_nclusters * m_random() );

    // Calculate the center of cluster.
    switch ( m_seeding_method )
    {
    case RandomSeeding:
        ::InitializeCentersWithRandomSeeding( table, m_nclusters, IDs, m_centers );
        break;
    case SmartSeeding:
        ::InitializeCentersWithSmartSeeding( table, m_nclusters, IDs, m_centers );
        break;
    default:
        ::InitializeCentersWithRandomSeeding( table, m_nclusters, IDs, m_centers );
        break;
    }

    // Cluster center used for convergence test.
    kvs::ValueArray<kvs::Real32> center_new( table->numberOfColumns() );

    // Clustering.
    bool converged = false;
    size_t counter = 0;
    while ( !converged )
    {
        // Calculate euclidean distance between the center of cluster and the point, and update the IDs.
        for ( size_t i = 0; i < table->numberOfRows(); i++ )
        {
            size_t id = 0;
            kvs::Real32 distance = kvs::Value<kvs::Real32>::Max();
            for ( size_t j = 0; j < m_nclusters; j++ )
            {
                const kvs::Real32 d = ::GetEuclideanDistance( table, i, m_centers[j] );
                if ( d < distance ) { distance = d; id = j; }
            }
            IDs[i] = id;
        }

        // Convergence test.
        converged = true;
        for ( size_t i = 0; i < m_nclusters; i++ )
        {
            ::CalculateCenter( table, i, IDs, &center_new );

            const kvs::Real32 distance = ::GetEuclideanDistance( table, m_centers[i], center_new );
            if ( !( distance < m_tolerance ) )
            {
                converged = false;
                break;
            }
        }

        if ( counter++ > m_max_iterations ) break;

        // Calculate the center of cluster.
        if ( !converged )
        {
            for ( size_t i = 0; i < m_nclusters; i++ )
            {
                ::CalculateCenter( table, i, IDs, &(m_centers[i]) );
            }
        }

    } // end of while

    // Set the results to the output table object.
    const size_t ncolumns = table->numberOfColumns();
    for ( size_t i = 0; i < ncolumns; i++ )
    {
        const std::string label = table->label(i);
        const kvs::AnyValueArray& column = table->column(i);
        SuperClass::addColumn( column, label );
    }
    SuperClass::addColumn( kvs::AnyValueArray( IDs ), "Cluster ID" );

    return this;
}

void KMeansClustering::setSeedingMethod( SeedingMethod seeding_method )
{
    m_seeding_method = seeding_method;
}

void KMeansClustering::setSeed( const size_t seed )
{
    m_random.setSeed( seed );
}

void KMeansClustering::setNumberOfClusters( const size_t nclusters )
{
    m_nclusters = nclusters;
}

void KMeansClustering::setMaxInterations( const size_t max_iterations )
{
    m_max_iterations = max_iterations;
}

void KMeansClustering::setTolerance( const float tolerance )
{
    m_tolerance = tolerance;
}

const kvs::ValueArray<kvs::Real32>& KMeansClustering::center( const size_t index )
{
    return m_centers[ index ];
}

} // end of namespace pcs

} // end of namespace kvsoceanvis
