/*****************************************************************************/
/**
 *  @file   MultiBinMapObjectWriter.h
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
#ifndef KVSOCEANVIS__PCS__MULTI_BIN_MAP_OBJECT_WRITER_H_INCLUDE
#define KVSOCEANVIS__PCS__MULTI_BIN_MAP_OBJECT_WRITER_H_INCLUDE

#include <string>
#include "MultiBinMapObject.h"


namespace kvsoceanvis
{

namespace pcs
{

class MultiBinMapObjectWriter
{
protected:

    const pcs::MultiBinMapObject* m_object;

public:

    MultiBinMapObjectWriter( const pcs::MultiBinMapObject* object );

    void write( const std::string filename );
};

} // end of namespace pcs

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__PCS__MULTI_BIN_MAP_OBJECT_WRITER_H_INCLUDE
