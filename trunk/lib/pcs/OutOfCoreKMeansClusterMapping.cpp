/*****************************************************************************/
/**
 *  @file   OutOfCoreKMeansClusterMapping.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: OutOfCoreKMeansClusterMapping.cpp 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "OutOfCoreKMeansClusterMapping.h"
#include <kvs/MersenneTwister>
#include <kvs/Value>
#include <kvs/KVSMLObjectTable>
#include "OutOfCoreTableObject.h"


namespace
{

kvs::Real64 GetEuclideanDistance(
    const kvsoceanvis::pcs::OutOfCoreTableObject* table,
    const kvs::UInt16 row_index,
    const kvs::ValueArray<kvs::Real64>& means )
{
    kvs::Real64 distance = 0.0;
    for ( size_t i = 0; i < table->numberOfColumns(); i++ )
    {
        const kvs::Real64 x0 = means[i];
        const kvs::Real64 x1 = table->readValue( row_index, i );
        distance += ( x1 - x0 ) * ( x1 - x0 );
    }

    return distance;
}

kvs::Real64 GetEuclideanDistance(
    const kvsoceanvis::pcs::OutOfCoreTableObject* table,
    const kvs::ValueArray<kvs::Real64>& mean_old,
    const kvs::ValueArray<kvs::Real64>& mean_new )
{
    kvs::Real64 distance = 0.0;
    for ( size_t i = 0; i < table->numberOfColumns(); i++ )
    {
        const kvs::Real64 x0 = mean_old[i];
        const kvs::Real64 x1 = mean_new[i];
        distance += ( x1 - x0 ) * ( x1 - x0 );
    }

    return distance;
}
}

namespace
{
/*===========================================================================*/
/**
 *  @brief  Calculates the cluster centroids.
 *  @param  table [in] table object
 *  @param  cluster_id [in] target cluster ID
 *  @param  ids [in] cluster ID array
 *  @param  means [out] cluster centroids (means)
 */
/*===========================================================================*/
void CalculateMeans(
    const kvsoceanvis::pcs::OutOfCoreTableObject* table,
    const kvs::UInt16 cluster_id,
    const kvs::ValueArray<kvs::UInt16>& ids,
    kvs::ValueArray<kvs::Real64>* means )
{
    for ( size_t j = 0; j < table->numberOfColumns(); j++ ) { means->at(j) = 0.0; }

    size_t counter = 0;
    for ( size_t j = 0; j < table->numberOfRows(); j++ )
    {
        if ( ids[j] == cluster_id )
        {
            for ( size_t k = 0; k < table->numberOfColumns(); k++ )
            {
                means->at(k) += table->readValue( j, k );
            }
            counter++;
        }
    }
    if ( counter != 0 )
    {
        for ( size_t k = 0; k < table->numberOfColumns(); k++ ) { means->at(k) /= counter; }
    }
}

}


namespace kvsoceanvis
{

namespace pcs
{

OutOfCoreKMeansClusterMapping::OutOfCoreKMeansClusterMapping()
{
}

OutOfCoreKMeansClusterMapping::OutOfCoreKMeansClusterMapping( const kvs::ObjectBase* object, const size_t nclusters ):
    m_nclusters( nclusters ),
    m_max_iterations( 100 ),
    m_tolerance( 1.e-6 )
{
    this->exec( object );
}

OutOfCoreKMeansClusterMapping::OutOfCoreKMeansClusterMapping( const kvs::ObjectBase* object, const size_t nclusters, const size_t max_iterations, const double tolerance ):
    m_nclusters( nclusters ),
    m_max_iterations( max_iterations ),
    m_tolerance( tolerance )
{
    this->exec( object );
}

OutOfCoreKMeansClusterMapping::SuperClass* OutOfCoreKMeansClusterMapping::exec( const kvs::ObjectBase* object )
{
    if ( !object )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input object is NULL.");
        return NULL;
    }

    const pcs::OutOfCoreTableObject* table = reinterpret_cast<const pcs::OutOfCoreTableObject*>( object );
    const size_t nrows = table->numberOfRows();
    const size_t ncolumns = table->numberOfColumns();
    const size_t nclusters = m_nclusters;

    // Assign initial cluster IDs to each row of the input table randomly.
    kvs::MersenneTwister R;
    kvs::ValueArray<kvs::UInt16> IDs( nrows );
    for ( size_t i = 0; i < nrows; i++ ) IDs[i] = kvs::UInt16( nclusters * R() );

    // Allocate memory for the mean values.
    kvs::ValueArray<kvs::Real64>* means = new kvs::ValueArray<kvs::Real64> [ nclusters ];
    for ( size_t i = 0; i < nclusters; i++ ) { means[i].allocate( ncolumns ); }

    // Mean value used for convergence test.
    kvs::ValueArray<kvs::Real64> mean_new( ncolumns );

   // Clustering.
    table->openColumnFiles();
    bool converged = false;
    size_t iterations = 0;
    while ( !converged )
    {
        // Calculate the center of cluster.
        for ( size_t i = 0; i < nclusters; i++ )
        {
            ::CalculateMeans( table, i, IDs, &(means[i]) );
        }

        // Calculate euclidean distance between the center of cluster and the point, and update the IDs.
        for ( size_t i = 0; i < nrows; i++ )
        {
            size_t id = 0;
            kvs::Real64 distance = kvs::Value<kvs::Real64>::Max();
            for ( size_t j = 0; j < nclusters; j++ )
            {
                const kvs::Real64 d = ::GetEuclideanDistance( table, i, means[j] );
                if ( d < distance ) { distance = d; id = j; }
            }
            IDs[i] = id;
        }

        // Convergence test.
        converged = true;
        for ( size_t i = 0; i < nclusters; i++ )
        {
            ::CalculateMeans( table, i, IDs, &mean_new );

            const kvs::Real64 distance = ::GetEuclideanDistance( table, means[i], mean_new );

            if ( !( distance < m_tolerance ) )
            {
                converged = false;
                break;
            }
        }

        if ( iterations++ > m_max_iterations ) break;
    } // end of while

    delete [] means;


    const size_t index = ncolumns;
    const size_t naxes = index;

    // Cluster parameters.
    size_t* counter = new size_t [ nclusters ];
    kvs::ValueArray<kvs::Real64>* min_values = new kvs::ValueArray<kvs::Real64> [ nclusters ];
    kvs::ValueArray<kvs::Real64>* max_values = new kvs::ValueArray<kvs::Real64> [ nclusters ];

    // Initialize the parameters.
    std::memset( counter, 0, sizeof(size_t) * nclusters );
    for ( size_t i = 0; i < nclusters; i++ )
    {
        min_values[i].allocate( naxes );
        max_values[i].allocate( naxes );
        for ( size_t j = 0; j < naxes; j++ )
        {
            min_values[i][j] = kvs::Value<kvs::Real64>::Max();
            max_values[i][j] = kvs::Value<kvs::Real64>::Min();
        }
    }

    // Cluster mapping.
    for ( size_t i = 0; i < nrows; i++ )
    {
        const size_t id = IDs[i];
        for ( size_t j = 0; j < naxes; j++ )
        {
            const kvs::Real64 value = table->readValue( i, j );
            min_values[id][j] = kvs::Math::Min( min_values[id][j], value );
            max_values[id][j] = kvs::Math::Max( max_values[id][j], value );
        }
        counter[id]++;
    }

    // Set the clusters.
    for ( size_t i = 0; i < nclusters; i++ )
    {
        SuperClass::Cluster cluster;
        cluster.setID( i );
        cluster.setCounter( counter[i] );
        cluster.setMinValues( min_values[i] );
        cluster.setMaxValues( max_values[i] );

        SuperClass::m_cluster_list.push_back( cluster );
    }

    delete [] counter;
    delete [] min_values;
    delete [] max_values;

    // Sorting.
    SuperClass::m_cluster_list.sort();

    // Number of axes.
    SuperClass::m_naxes = naxes;
    SuperClass::setNumberOfColumns( naxes );

    // Number of points.
    SuperClass::m_npoints = nrows;
    SuperClass::setNumberOfRows( nrows );

    // Min/Max value.
/*
    for ( size_t i = 0; i < naxes; i++ )
    {
        SuperClass::m_min_values.push_back( table->minValue(i) );
        SuperClass::m_max_values.push_back( table->maxValue(i) );
    }
*/
    SuperClass::setMinValues( table->minValues() );
    SuperClass::setMaxValues( table->maxValues() );
    SuperClass::setLabels( table->labels() );

    // Min/Max range.
//    SuperClass::m_min_ranges.deepCopy( m_min_values );
//    SuperClass::m_max_ranges.deepCopy( m_max_values );
//    SuperClass::m_min_ranges = m_min_values;
//    SuperClass::m_max_ranges = m_max_values;
    SuperClass::setMinRanges( table->minValues() );
    SuperClass::setMaxRanges( table->maxValues() );

    return this;
}

void OutOfCoreKMeansClusterMapping::setNumberOfClusters( const size_t nclusters )
{
    m_nclusters = nclusters;
}

void OutOfCoreKMeansClusterMapping::setMaxInterations( const size_t max_iterations )
{
    m_max_iterations = max_iterations;
}

void OutOfCoreKMeansClusterMapping::setTolerance( const double tolerance )
{
    m_tolerance = tolerance;
}

} // end of namespace pcs

} // end of namespace kvsoceanvis
