/*****************************************************************************/
/**
 *  @file   KMeansClustering.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: KMeansClustering.h 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*----------------------------------------------------------------------------
 *
 * References:
 * [1] D. Arthur and S. Vassilvitskii, k-means++ : The Advantages of Careful
 *     Seeding, in Proceedings of the eighteenth annual ACM-SIAM symposium on
 *     Discrete algorithms, 2007, pp. 1027-1035.
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__PCS__K_MEANS_CLUSTERING_H_INCLUDE
#define KVSOCEANVIS__PCS__K_MEANS_CLUSTERING_H_INCLUDE

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
 *  @brief  K-means clustering class.
 */
/*===========================================================================*/
class KMeansClustering : public kvs::FilterBase, public kvs::TableObject
{
    kvsModuleName( kvsoceanvis::pcs::KMeansClustering );
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

    KMeansClustering();
    KMeansClustering( const kvs::ObjectBase* object, const size_t nclusters );
    KMeansClustering( const kvs::ObjectBase* object, const size_t nclusters, const size_t max_iterations, const float torelance );
    virtual ~KMeansClustering();

public:

    SuperClass* exec( const kvs::ObjectBase* object );

public:

    void setSeedingMethod( SeedingMethod seeding_method );
    void setSeed( const size_t seed );
    void setNumberOfClusters( const size_t nclusters );
    void setMaxInterations( const size_t max_iterations );
    void setTolerance( const float tolerance );

    const kvs::ValueArray<kvs::Real32>& center( const size_t index );
};

} // end of namespace pcs

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__PCS__K_MEANS_CLUSTERING_H_INCLUDE
