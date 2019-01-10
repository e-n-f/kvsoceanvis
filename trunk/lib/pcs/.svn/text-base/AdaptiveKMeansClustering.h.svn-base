/*****************************************************************************/
/**
 *  @file   AdaptiveKMeansClustering.h
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
/*----------------------------------------------------------------------------
 *
 * References:
 * [1] C. Sugar and J. Gareth, Finding the number of clusters in a data set :
 *     An information theoretic approach, Journal of the American Statistical
 *     Association, vol. 98, pp. 750-763, 2003.
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__PCS__ADAPTIVE_KMEANS_CLUSTERING_H_INCLUDE
#define KVSOCEANVIS__PCS__ADAPTIVE_KMEANS_CLUSTERING_H_INCLUDE

#include <kvs/Module>
#include <kvs/FilterBase>
#include <kvs/TableObject>
#include <kvs/ValueArray>


namespace kvsoceanvis
{

namespace pcs
{

/*===========================================================================*/
/**
 *  @brief  Adaptive k-means clustering class.
 */
/*===========================================================================*/
class AdaptiveKMeansClustering : public kvs::FilterBase, public kvs::TableObject
{
    kvsModuleName( kvsoceanvis::pcs::AdaptiveKMeansClustering );
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::TableObject );

private:

    size_t m_nclusters; ///< number of clusters
    size_t m_max_iterations; ///< maximum number of interations
    float m_tolerance; ///< tolerance of distance
    size_t m_max_nclusters; ///< maximum number of clusters for finding the best k
    kvs::ValueArray<kvs::Real32> m_distortions; ///< distortions for finding the best k

public:

    AdaptiveKMeansClustering();
    AdaptiveKMeansClustering( const kvs::ObjectBase* object );
    AdaptiveKMeansClustering( const kvs::ObjectBase* object, const size_t max_iterations, const float tolerance );
    AdaptiveKMeansClustering( const kvs::ObjectBase* object, const size_t max_nclusters );
    AdaptiveKMeansClustering( const kvs::ObjectBase* object, const size_t max_nclusters, const size_t max_iterations, const float tolerance );

public:

    SuperClass* exec( const kvs::ObjectBase* object );

public:

    void setMaxNumberOfClusters( const size_t max_nclusters );
    void setMaxIterations( const size_t max_iterations );
    void setTolerance( const float tolerance );

    size_t numberOfClusters() const;
    size_t maxNumberOfClusters() const;
    const kvs::ValueArray<kvs::Real32>& distortions() const;
    kvs::ValueArray<kvs::Real32> jumps() const;
};

} // end of namespace pcs

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__PCS__ADAPTIVE_KMEANS_CLUSTERING_H_INCLUDE
