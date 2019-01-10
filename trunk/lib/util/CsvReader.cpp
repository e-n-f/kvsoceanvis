/*****************************************************************************/
/**
 *  @file   CsvReader.cpp
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
#include "CsvReader.h"


namespace kvsoceanvis
{

namespace util
{

CsvReader::CsvReader( const std::string filename )
{
    this->read( filename );
}

void CsvReader::read( const std::string filename )
{
    kvs::Csv csv( filename );

    const size_t ncolumns = csv.row(0).size();
    const size_t nrows = csv.nrows();
    for ( size_t i = 0; i < ncolumns; i++ )
    {
        std::string label = csv.row(0).at(i);
//        kvs::AnyValueArray column;
//        kvs::Real32* pcolumn = static_cast<kvs::Real32*>( column.allocate<kvs::Real32>( nrows - 1 ) );
        kvs::ValueArray<kvs::Real32> column( nrows - 1 );
        kvs::Real32* pcolumn = column.data();
        for ( size_t j = 1; j < nrows; j++ )
        {
            const char* svalue = csv.row(j).at(i).c_str();
            const kvs::Real32 value = static_cast<kvs::Real32>( std::atof( svalue ) );
            *(pcolumn++) = value;
        }

        addColumn( kvs::AnyValueArray( column ), label );
    }
}

} // end of namespace util

} // end of namespace kvsoceanvis
