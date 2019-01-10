/*****************************************************************************/
/**
 *  @file   OutOfCoreMultiBinMapping.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: OutOfCoreMultiBinMapping.cpp 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "OutOfCoreMultiBinMapping.h"
#include <kvs/AnyValueArray>


namespace
{

/*===========================================================================*/
/**
 *  @brief  Bin map class.
 */
/*===========================================================================*/
class BinMap
{
public:

    typedef kvs::UInt32 Key;
    typedef kvsoceanvis::pcs::MultiBinMapObject::Bin Value;
    typedef std::multimap<Key,Value> Bucket;

protected:

    Bucket m_bucket;

public:

    BinMap() {}

public:

    const Bucket& bucket() const
    {
        return m_bucket;
    }

    const size_t size() const
    {
        return m_bucket.size();
    }

    void insert( const kvs::ValueArray<kvs::UInt16>& indices )
    {
        const Key key( std::accumulate( indices.begin(), indices.end(), 0 ) );
        const Value value( indices );

        Bucket::iterator b = m_bucket.find( key );
        Bucket::const_iterator last = m_bucket.end();
        if ( b != last )
        {
            Bucket::const_iterator upper = m_bucket.upper_bound( key );
            do
            {
                if ( b->second == value )
                {
                    b->second.count();
                    return;
                }
                b++;
            } while ( b != upper );
        }

        m_bucket.insert( std::make_pair( key, value ) );
    }
};

} // end of namespace


namespace kvsoceanvis
{

namespace pcs
{

OutOfCoreMultiBinMapping::OutOfCoreMultiBinMapping()
{
}

OutOfCoreMultiBinMapping::OutOfCoreMultiBinMapping( const kvs::ObjectBase* object )
{
    this->exec( object );
}

OutOfCoreMultiBinMapping::OutOfCoreMultiBinMapping( const kvs::ObjectBase* object, const kvs::ValueArray<kvs::UInt32>& nbins )
{
    SuperClass::m_nbins = nbins;
    this->exec( object );
}

OutOfCoreMultiBinMapping::SuperClass* OutOfCoreMultiBinMapping::exec( const kvs::ObjectBase* object )
{
    if ( !object )
    {
        BaseClass::setSuccess( false );
        kvsMessageError("Input object is NULL.");
        return NULL;
    }

    const pcs::OutOfCoreTableObject* table = static_cast<const pcs::OutOfCoreTableObject*>( object );

    // Calculate number of binns for each axis.
    const size_t ncolumns = table->numberOfColumns();
    if ( m_nbins.size() < ncolumns )
    {
        m_nbins.allocate( ncolumns );
        for ( size_t i = 0; i < ncolumns; i++ )
        {
            const size_t nbins = this->get_nbins_by_sturges_formula( table, i );
//        const size_t nbins = this->get_nbins_by_scott_choice( table, i );
            m_nbins[i] = nbins;
        }
    }

    // Multi bin mapping.
    ::BinMap bin_map;
    table->openColumnFiles();
    const size_t nrows = table->numberOfRows();
    for ( size_t i = 0; i < nrows; i++ )
    {
        bool ignore = false;
        kvs::ValueArray<kvs::UInt16> indices( ncolumns );
        for ( size_t j = 0; j < ncolumns; j++ )
        {
            const size_t nbins = m_nbins[j];
            const kvs::Real64 min_value = table->minValue(j);
            const kvs::Real64 max_value = table->maxValue(j);
            const kvs::Real64 value = table->readValue( i, j );
            if ( value < min_value || max_value < value ) { ignore = true; break; }

            const kvs::UInt16 index = kvs::UInt16( kvs::Math::Round( ( nbins - 1 ) * ( value - min_value ) / ( max_value - min_value ) ) );
            indices[j] = index;
        }

        if ( !ignore ) bin_map.insert( indices );
    }
    table->closeColumnFiles();

    SuperClass::setNumberOfRows( table->numberOfRows() );
    SuperClass::setNumberOfColumns( table->numberOfColumns() );
    SuperClass::setLabels( table->labels() );

//    SuperClass::m_min_values = kvs::ValueArray<kvs::Real64>( table->minValueList() );
//    SuperClass::m_max_values = kvs::ValueArray<kvs::Real64>( table->maxValueList() );
    SuperClass::setMinValues( table->minValues() );
    SuperClass::setMaxValues( table->maxValues() );

//    SuperClass::m_min_ranges = kvs::ValueArray<kvs::Real64>( table->minRangeList() );
//    SuperClass::m_max_ranges = kvs::ValueArray<kvs::Real64>( table->maxRangeList() );
    SuperClass::setMinRanges( table->minRanges() );
    SuperClass::setMaxRanges( table->maxRanges() );

    // Serialize.
    ::BinMap::Bucket::const_iterator bin = bin_map.bucket().begin();
    ::BinMap::Bucket::const_iterator last = bin_map.bucket().end();
    while ( bin != last ) { m_bin_list.push_back( bin->second ); m_npoints += bin->second.counter(); bin++; }

    // Sorting.
    m_bin_list.sort();

    return this;
}

size_t OutOfCoreMultiBinMapping::get_nbins_by_sturges_formula( const pcs::OutOfCoreTableObject* table, const size_t index )
{
    const kvs::AnyValueArray& array = table->column(index);
    const kvs::Real64 n = array.size();
//    return std::ceil( log2(n) + 1 );
    return size_t( std::ceil( std::log(n) / std::log(2.0) + 1 ) );
}

size_t OutOfCoreMultiBinMapping::get_nbins_by_scott_choice( const pcs::OutOfCoreTableObject* table, const size_t index )
{
    const kvs::AnyValueArray& array = table->column(index);
    const kvs::Real64 h = this->get_bin_width_by_scott_choice(array);
    const kvs::Real64 min = table->minRange(index);
    const kvs::Real64 max = table->maxRange(index);
    return size_t( std::ceil( ( max - min ) / h ) );
}

kvs::Real64 OutOfCoreMultiBinMapping::get_bin_width_by_scott_choice( const kvs::AnyValueArray& array )
{
    // Scott's choice.
    const kvs::Real64 s = this->get_standard_deviation( array );
    const kvs::Real64 n = array.size();

    return 3.5 * s / std::pow( n, 1.0 / 3.0 );
}

kvs::Real64 OutOfCoreMultiBinMapping::get_standard_deviation( const kvs::AnyValueArray& array )
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

kvs::Real64 OutOfCoreMultiBinMapping::get_mean( const kvs::AnyValueArray& array )
{
    const size_t nvalues = array.size();
    kvs::Real64 sum = 0.0;
    for ( size_t i = 0; i < nvalues; i++ )
    {
        sum += this->get_value( array, i );
    }

    return sum / nvalues;
}

kvs::Real64 OutOfCoreMultiBinMapping::get_value( const kvs::AnyValueArray& array, const size_t index )
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
