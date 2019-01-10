/*****************************************************************************/
/**
 *  @file   OutOfCoreBinMapping.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: OutOfCoreBinMapping.h 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__PCS__OUT_OF_CORE_BIN_MAPPING_H_INCLUDE
#define KVSOCEANVIS__PCS__OUT_OF_CORE_BIN_MAPPING_H_INCLUDE

#include "BinMapping.h"
#include "OutOfCoreTableObject.h"


namespace kvsoceanvis
{

namespace pcs
{

/*===========================================================================*/
/**
 *  @brief  Out-of-core bin mapping class.
 */
/*===========================================================================*/
class OutOfCoreBinMapping : public pcs::BinMapping
{
    kvsModuleName( kvsoceanvis::pcs::OutOfCoreBinMapping );

public:

    OutOfCoreBinMapping();
    OutOfCoreBinMapping( const kvs::ObjectBase* object );

public:

    SuperClass* exec( const kvs::ObjectBase* object );

protected:

    size_t get_nbins_by_scott_choice( const pcs::OutOfCoreTableObject* table, const size_t index );
    size_t get_nbins_by_sturges_formula( const pcs::OutOfCoreTableObject* table, const size_t index );
};

} // end of namespace pcs

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__PCS__OUT_OF_CORE_BIN_MAPPING_H_INCLUDE
