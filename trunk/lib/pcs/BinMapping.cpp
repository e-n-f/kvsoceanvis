/*****************************************************************************/
/**
 *  @file   BinMapping.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: BinMapping.cpp 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "BinMapping.h"
#include <kvs/AnyValueArray>
#include <kvs/ValueArray>


namespace kvsoceanvis
{

namespace pcs
{

BinMapping::BinMapping()
{
}

BinMapping::BinMapping( const kvs::ObjectBase* object )
{
    this->exec( object );
}

BinMapping::SuperClass* BinMapping::exec( const kvs::ObjectBase* object )
{
    if ( !object )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input object is NULL.");
        return NULL;
    }

    const kvs::TableObject* table = kvs::TableObject::DownCast( object );

    // Calculate number of binns for each axis.
    const size_t ncolumns = table->numberOfColumns();

    kvs::ValueArray<kvs::UInt32> nbins( ncolumns );
    for ( size_t i = 0; i < ncolumns; i++ )
    {
        const size_t n = this->get_nbins_by_sturges_formula( table, i );
//        const size_t n = this->get_nbins_by_scott_choice( table, i );
        nbins[i] = n;
    }

    // Bin mapping (2D binning).
    for ( size_t i = 0; i < ncolumns - 1; i++ )
    {
        const kvs::AnyValueArray& x = table->column(i);
        const kvs::AnyValueArray& y = table->column(i+1);

//        const kvs::Real64 xmin = table->minRange(i);
//        const kvs::Real64 xmax = table->maxRange(i);
//        const kvs::Real64 ymin = table->minRange(i+1);
//        const kvs::Real64 ymax = table->maxRange(i+1);

        const kvs::Real64 xmin = table->minValue(i);
        const kvs::Real64 xmax = table->maxValue(i);
        const kvs::Real64 ymin = table->minValue(i+1);
        const kvs::Real64 ymax = table->maxValue(i+1);

        // Count.
        const kvs::UInt32 xsize = nbins[i];
        const kvs::UInt32 ysize = nbins[i+1];
        kvs::ValueArray<kvs::UInt32> bin_map( xsize * ysize );
        bin_map.fill( 0x00 );

        const size_t nvalues = x.size();
        for ( size_t j = 0; j < nvalues; j++ )
        {
            const size_t xindex = size_t( ( xsize - 1 ) * ( this->get_value(x,j) - xmin ) / ( xmax - xmin ) );
            const size_t yindex = size_t( ( ysize - 1 ) * ( this->get_value(y,j) - ymin ) / ( ymax - ymin ) );

            const size_t index = xindex + yindex * xsize;
            bin_map[index]++;
        }

        SuperClass::addBinMap( bin_map );
    }

    return this;
}

size_t BinMapping::get_nbins_by_sturges_formula( const kvs::TableObject* table, const size_t index )
{
    const kvs::AnyValueArray& array = table->column(index);
    const kvs::Real64 n = array.size();
//    return std::ceil( log2(n) + 1 );
    return size_t( std::ceil( std::log(n) / std::log(2.0) + 1 ) );
}

size_t BinMapping::get_nbins_by_scott_choice( const kvs::TableObject* table, const size_t index )
{
    const kvs::AnyValueArray& array = table->column(index);
    const kvs::Real64 h = this->get_bin_width_by_scott_choice(array);
    const kvs::Real64 min = table->minRange(index);
    const kvs::Real64 max = table->maxRange(index);
    return size_t( std::ceil( ( max - min ) / h ) );
}

kvs::Real64 BinMapping::get_bin_width_by_scott_choice( const kvs::AnyValueArray& array )
{
    // Scott's choice.
    const kvs::Real64 s = this->get_standard_deviation( array );
    const kvs::Real64 n = array.size();

    return 3.5 * s / std::pow( n, 1.0 / 3.0 );
}

kvs::Real64 BinMapping::get_standard_deviation( const kvs::AnyValueArray& array )
{
    const kvs::Real64 mean = this->get_mean( array );
    const size_t nvalues = array.size();
    kvs::Real64 sum = 0.0;
    for ( size_t i = 0; i < nvalues; i++ )
    {
        const kvs::Real64 value = this->get_value( array, i );
        sum += ( value - mean ) * ( value - mean );
    }

    return std::sqrt( sum / nvalues );
}

kvs::Real64 BinMapping::get_mean( const kvs::AnyValueArray& array )
{
    const size_t nvalues = array.size();
    kvs::Real64 sum = 0.0;
    for ( size_t i = 0; i < nvalues; i++ )
    {
        sum += this->get_value( array, i );
    }

    return sum / nvalues;
}

kvs::Real64 BinMapping::get_value( const kvs::AnyValueArray& array, const size_t index )
{
    /* The following methods will be implemented in kvs::AnyValueArray and kvs::AnyValue class.
     *     isInt8, isInt16, ...
     *     toInt8, toInt16, ...
     */

    const std::type_info& type = array.typeInfo()->type();
    if ( type == typeid( kvs::Int8   ) ) return static_cast<kvs::Real64>( array.at<kvs::Int8>(index) );
    if ( type == typeid( kvs::Int16  ) ) return static_cast<kvs::Real64>( array.at<kvs::Int16>(index) );
    if ( type == typeid( kvs::Int32  ) ) return static_cast<kvs::Real64>( array.at<kvs::Int32>(index) );
    if ( type == typeid( kvs::Int64  ) ) return static_cast<kvs::Real64>( array.at<kvs::Int64>(index) );
    if ( type == typeid( kvs::UInt8  ) ) return static_cast<kvs::Real64>( array.at<kvs::UInt8>(index) );
    if ( type == typeid( kvs::UInt16 ) ) return static_cast<kvs::Real64>( array.at<kvs::UInt16>(index) );
    if ( type == typeid( kvs::UInt32 ) ) return static_cast<kvs::Real64>( array.at<kvs::UInt32>(index) );
    if ( type == typeid( kvs::UInt64 ) ) return static_cast<kvs::Real64>( array.at<kvs::UInt64>(index) );
    if ( type == typeid( kvs::Real32 ) ) return static_cast<kvs::Real64>( array.at<kvs::Real32>(index) );
    if ( type == typeid( kvs::Real64 ) ) return static_cast<kvs::Real64>( array.at<kvs::Real64>(index) );

    kvsMessageError("Unsupported data type.");
    return 0.0;
}

} // end of namespace pcs

} // end of namespace kvsoceanvis
