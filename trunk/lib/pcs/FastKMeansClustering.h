/*****************************************************************************/
/**
 *  @file   FastKMeansClustering.h
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
 * [1] Greg Hamerly, Making k-means even faster, In proceedings of the 2010 SIAM
 *     international conference on data mining (SDM 2010), April 2010.
 * [2] D. Arthur and S. Vassilvitskii, k-means++ : The Advantages of Careful
 *     Seeding, in Proceedings of the eighteenth annual ACM-SIAM symposium on
 *     Discrete algorithms, 2007, pp. 1027-1035.
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__PCS__FAST_K_MEANS_CLUSTERING_H_INCLUDE
#define KVSOCEANVIS__PCS__FAST_K_MEANS_CLUSTERING_H_INCLUDE

#include <kvs/Module>
#include <kvs/FilterBase>
#include <kvs/TableObject>
#include <kvs/MersenneTwister>


namespace kvsoceanvis
{

namespace pcs
{

/*===========================================================================*/
/**
 *  @brief  Fast K-means clustering class.
 */
/*===========================================================================*/
class FastKMeansClustering : public kvs::FilterBase, public kvs::TableObject
{
    kvsModuleName( kvsoceanvis::pcs::FastKMeansClustering );
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::TableObject );

public:

    enum SeedingMethod
    {
        RandomSeeding,
        SmartSeeding
    };

protected:

    kvs::MersenneTwister m_random; ///< random number generator
    SeedingMethod m_seeding_method; ///< seeding method
    size_t m_nclusters; ///< number of clusters
    size_t m_max_iterations; ///< maximum number of interations
    float m_tolerance; ///< tolerance of distance
    kvs::ValueArray<kvs::Real32>* m_centers; ///< cluster centers

public:

    FastKMeansClustering();
    FastKMeansClustering( const kvs::ObjectBase* object, const size_t nclusters );
    FastKMeansClustering( const kvs::ObjectBase* object, const size_t nclusters, const size_t max_iterations, const float torelance );
    virtual ~FastKMeansClustering();

public:

    SuperClass* exec( const kvs::ObjectBase* object );

public:

    void setSeedingMethod( SeedingMethod seeding_method );
    void setSeed( const size_t seed );
    void setNumberOfClusters( const size_t nclusters );
    void setMaxIterations( const size_t max_iterations );
    void setTolerance( const float tolerance );

    size_t numberOfClusters() const;
    const kvs::ValueArray<kvs::Real32>& center( const size_t index ) const;
};

} // end of namespace pcs

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__PCS__FAST_K_MEANS_CLUSTERING_H_INCLUDE
