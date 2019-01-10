/*****************************************************************************/
/**
 *  @file   CreateRandomTable.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: CreateRandomTable.h 393 2014-10-16 06:16:37Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__UTIL__CREATE_RANDOM_TABLE_H_INCLUDE
#define KVSOCEANVIS__UTIL__CREATE_RANDOM_TABLE_H_INCLUDE

#include <cstdio>
#include <kvs/MersenneTwister>
#include <kvs/ValueArray>
#include <kvs/TableObject>


namespace kvsoceanvis
{

namespace util
{

inline kvs::TableObject CreateRandomTable( const size_t nrows, const size_t ncolumns, const size_t seed )
{
    kvs::TableObject table;

    kvs::MersenneTwister R( seed );
    for ( size_t i = 0; i < ncolumns; i++ )
    {
        kvs::ValueArray<kvs::Int32> array( nrows );
        for ( size_t j = 0; j < nrows; j++ )
        {
            array[j] = static_cast<kvs::Int32>( 100 * R() );
        }

        table.addColumn( kvs::AnyValueArray( array ) );
    }

    return table;
}

inline kvs::TableObject CreateRandomTable( const size_t nrows, const size_t ncolumns )
{
    return CreateRandomTable( nrows, ncolumns, time( NULL ) );
}

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__CREATE_RANDOM_TABLE_H_INCLUDE
