/*****************************************************************************/
/**
 *  @file   OutOfCoreKMeansClusterMapping.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: OutOfCoreKMeansClusterMapping.h 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__PCS__OUT_OF_CORE_K_MEANS_CLUSTER_MAPPING_H_INCLUDE
#define KVSOCEANVIS__PCS__OUT_OF_CORE_K_MEANS_CLUSTER_MAPPING_H_INCLUDE

#include <kvs/Module>
#include <kvs/FilterBase>
#include "ClusterMapObject.h"


namespace kvsoceanvis
{

namespace pcs
{

/*===========================================================================*/
/**
 *  @brief  Out-of-core k-means clustering class.
 */
/*===========================================================================*/
class OutOfCoreKMeansClusterMapping : public kvs::FilterBase, public pcs::ClusterMapObject
{
    kvsModuleName( kvsoceanvis::pcs::OutOfCoreKMeansClusterMapping );
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( pcs::ClusterMapObject );

protected:

    size_t m_nclusters; ///< number of clusters
    size_t m_max_iterations; ///< maximum number of interations
    double m_tolerance; ///< tolerance of distance

public:

    OutOfCoreKMeansClusterMapping();
    OutOfCoreKMeansClusterMapping( const kvs::ObjectBase* object, const size_t nclusters );
    OutOfCoreKMeansClusterMapping( const kvs::ObjectBase* object, const size_t nclusters, const size_t max_iterations, const double torelance );

public:

    SuperClass* exec( const kvs::ObjectBase* object );

public:

    void setNumberOfClusters( const size_t nclusters );
    void setMaxInterations( const size_t max_iterations );
    void setTolerance( const double tolerance );
};

} // end of namespace pcs

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__PCS__OUT_OF_CORE_K_MEANS_CLUSTER_MAPPING_H_INCLUDE
