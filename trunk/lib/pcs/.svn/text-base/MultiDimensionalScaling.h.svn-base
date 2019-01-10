/*****************************************************************************/
/**
 *  @file   MultiDimensionalScaling.h
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
#ifndef KVSOCEANVIS__PCS__MULTI_DIMENSIONAL_SCALING_H_INCLUDE
#define KVSOCEANVIS__PCS__MULTI_DIMENSIONAL_SCALING_H_INCLUDE

#include <kvs/Module>
#include <kvs/FilterBase>
#include <kvs/TableObject>
#include <kvs/Matrix>


namespace kvsoceanvis
{

namespace pcs
{

class MultiDimensionalScaling : public kvs::FilterBase, public kvs::TableObject
{
    kvsModuleName( kvsoceanvis::pcs::MultiDimensionalScaling );
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::TableObject );

private:

    kvs::Matrix<kvs::Real32> m_distance_matrix;

public:

    MultiDimensionalScaling( void );
    MultiDimensionalScaling( const kvs::TableObject* object );
    MultiDimensionalScaling( const kvs::Matrix<kvs::Real32>& distance_matrix );

    void setDistanceMatrix( const kvs::Matrix<kvs::Real32>& distance_matrix );

    SuperClass* exec( const kvs::ObjectBase* object );
};

} // end of namespace pcs

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__PCS__MULTI_DIMENSIONAL_SCALING_H_INCLUDE
