/*****************************************************************************/
/**
 *  @file   OutOfCoreTableImporter.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: OutOfCoreTableImporter.cpp 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "OutOfCoreTableImporter.h"
#include <kvs/AnyValueArray>
#include <kvs/XMLDocument>
#include <kvs/XMLElement>
#include <kvs/XMLComment>
#include <kvs/KVSMLObjectTable>
#include <Core/FileFormat/KVSML/DataArrayTag.h>
#include <Core/FileFormat/KVSML/TableObjectTag.h>
#include <Core/FileFormat/KVSML/ColumnTag.h>
#include <utility>
#include <kvs/Math>
#include <kvs/Value>


namespace
{

std::pair<kvs::Real64,kvs::Real64> GetMinMax( const kvs::AnyValueArray& array )
{
    kvs::Real64 min_value = kvs::Value<kvs::Real64>::Max();
    kvs::Real64 max_value = kvs::Value<kvs::Real64>::Min();
    const std::type_info& type = array.typeInfo()->type();
    if ( type == typeid( kvs::Int8 ) )
    {
        const kvs::Int8* value = static_cast<const kvs::Int8*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( kvs::UInt8 ) )
    {
        const kvs::UInt8* value = static_cast<const kvs::UInt8*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( kvs::Int16 ) )
    {
        const kvs::Int16* value = static_cast<const kvs::Int16*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( kvs::UInt16 ) )
    {
        const kvs::UInt16* value = static_cast<const kvs::UInt16*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( kvs::Int32 ) )
    {
        const kvs::Int32* value = static_cast<const kvs::Int32*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( kvs::UInt32 ) )
    {
        const kvs::UInt32* value = static_cast<const kvs::UInt32*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( kvs::Int64 ) )
    {
        const kvs::Int64* value = static_cast<const kvs::Int64*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( kvs::UInt64 ) )
    {
        const kvs::UInt64* value = static_cast<const kvs::UInt64*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( kvs::Real32 ) )
    {
        const kvs::Real32* value = static_cast<const kvs::Real32*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( kvs::Real64 ) )
    {
        const kvs::Real64* value = static_cast<const kvs::Real64*>( array.data() );
        for( size_t i = 0; i < array.size(); i++ )
        {
            min_value = kvs::Math::Min( min_value, kvs::Real64( value[i] ) );
            max_value = kvs::Math::Max( max_value, kvs::Real64( value[i] ) );
        }
    }
    else if ( type == typeid( std::string ) )
    {
        min_value = 0.0;
        max_value = 0.0;
    }

    return std::pair<kvs::Real64,kvs::Real64>( min_value, max_value );
}

}

namespace kvsoceanvis
{

namespace pcs
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new OutOfCoreTableImporter class.
 */
/*===========================================================================*/
OutOfCoreTableImporter::OutOfCoreTableImporter()
{
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new OutOfCoreTableImporter class.
 *  @param  filename [in] filename
 */
/*===========================================================================*/
OutOfCoreTableImporter::OutOfCoreTableImporter( const std::string& filename, const kvs::UInt64 cache_size )
{
    if ( kvs::KVSMLObjectTable::CheckExtension( filename ) )
    {
        kvs::KVSMLObjectTable* table = new kvs::KVSMLObjectTable();
//        table->setFilename( filename );
        this->import( table, filename );

        if ( cache_size > 0 )
        {
            this->enableCache( cache_size );
            this->openColumnFiles();
            this->fetch();
            this->closeColumnFiles();
        }

        delete table;

        BaseClass::setSuccess( true );
    }
    else
    {
        kvsMessageError("Cannot import '%'.",filename.c_str());
        BaseClass::setSuccess( false );
    }
}

/*===========================================================================*/
/**
 *  @brief  Imports out-of-core table object.
 *  @param  file_format [in] file format
 *  @return pointer to the imported table object
 */
/*===========================================================================*/
OutOfCoreTableImporter::SuperClass* OutOfCoreTableImporter::exec( const kvs::FileFormatBase* file_format )
{
    return this;
}

void OutOfCoreTableImporter::import( const kvs::KVSMLObjectTable* kvsml, const std::string& filename )
{
    // XML document
    kvs::XMLDocument document;
//    if ( !document.read( kvsml->filename() ) )
    if ( !document.read( filename ) )
    {
        kvsMessageError( "%s", document.ErrorDesc().c_str() );
        return;
    }

    // <KVSML>
    kvs::kvsml::KVSMLTag kvsml_tag;
    if ( !kvsml_tag.read( &document ) )
    {
        kvsMessageError( "Cannot read <%s>.", kvsml_tag.name().c_str() );
        return;
    }

    // <Object>
    kvs::kvsml::ObjectTag object_tag;
    if ( !object_tag.read( kvsml_tag.node() ) )
    {
        kvsMessageError( "Cannot read <%s>.", object_tag.name().c_str() );
        return;
    }

    // <TableObject>
    kvs::kvsml::TableObjectTag table_object_tag;
    if ( !table_object_tag.read( object_tag.node() ) )
    {
        kvsMessageError( "Cannot read <%s>.", table_object_tag.name().c_str() );
        return;
    }

    SuperClass::setNumberOfRows( table_object_tag.nrows() );
    SuperClass::setNumberOfColumns( table_object_tag.ncolumns() );

    // <Column>
    kvs::kvsml::ColumnTag column_tag;
    kvs::XMLNode::SuperClass* node = kvs::XMLNode::FindChildNode( table_object_tag.node(), column_tag.name() );
//    size_t counter = 0;

    SuperClass::Labels labels;
    SuperClass::Values min_values;
    SuperClass::Values max_values;
    SuperClass::Values min_ranges;
    SuperClass::Values max_ranges;
    SuperClass::InsideRangeFlags inside_range_flags;
    size_t nrows = 0;
    while ( node )
    {
        column_tag.read( kvs::XMLNode::ToElement( node ) );

//        if ( counter++ < m_ncolumns )
        {
            // Read one element for determing the data type.
            kvs::AnyValueArray data_array;
            kvs::kvsml::DataArrayTag data_array_tag;
            if ( !data_array_tag.read( node, SuperClass::numberOfRows(), &data_array ) )
            {
                kvsMessageError( "Cannot read <%s>.", data_array_tag.name().c_str() );
            }
            else
            {
                const std::string path = kvs::File( document.filename() ).pathName( true );
                const std::string filename = path + kvs::File::Separator() + data_array_tag.file();

                SuperClass::m_column_types.push_back( data_array_tag.type() );
                SuperClass::m_column_formats.push_back( data_array_tag.format() );
                SuperClass::m_column_files.push_back( filename );

/*
                if ( column_tag.hasMinValue() && column_tag.hasMaxValue() )
                {
                    const kvs::Real64 min_value = column_tag.minValue();
                    const kvs::Real64 max_value = column_tag.maxValue();
                    SuperClass::addColumn( data_array, min_value, max_value, column_tag.label() );
                }
                else
                {
                    SuperClass::addColumn( data_array, column_tag.label() );
                }
*/
//                SuperClass::addColumn( data_array, column_tag.label() );
                labels.push_back( column_tag.label() );

                const std::pair<kvs::Real64,kvs::Real64> min_max = ::GetMinMax( data_array );
                const kvs::Real64 min_value = min_max.first;
                const kvs::Real64 max_value = min_max.second;
                min_values.push_back( min_value );
                max_values.push_back( max_value );
                min_ranges.push_back( min_value );
                max_ranges.push_back( max_value );

                const size_t column_index = labels.size() - 1;
                if ( column_tag.hasMinValue() )
                {
//                    SuperClass::m_min_values[column_index] = SuperClass::m_min_ranges[column_index] = column_tag.minValue();
//                    SuperClass::setMinValue( column_index, column_tag.minValue() );
//                    SuperClass::setMinRange( column_index, column_tag.minValue() );
                    min_values[column_index] = column_tag.minValue();
                    min_ranges[column_index] = column_tag.minValue();
                }
                if ( column_tag.hasMaxValue() )
                {
//                    SuperClass::m_max_values[column_index] = SuperClass::m_max_ranges[column_index] = column_tag.maxValue();
//                    SuperClass::setMaxValue( column_index, column_tag.maxValue() );
//                    SuperClass::setMaxRange( column_index, column_tag.maxValue() );
                    max_values[column_index] = column_tag.maxValue();
                    max_ranges[column_index] = column_tag.maxValue();
                }

                // Delete added column values.
//                SuperClass::m_ncolumns--;
//                SuperClass::m_columns.pop_back();
            }
        }

        node = table_object_tag.node()->IterateChildren( column_tag.name(), node );
    }

    inside_range_flags.resize( nrows, 1 );

    SuperClass::setLabels( labels );
    SuperClass::setMinValues( min_values );
    SuperClass::setMaxValues( max_values );
    SuperClass::setMinRanges( min_ranges );
    SuperClass::setMaxRanges( max_ranges );
    SuperClass::setInsideRangeFlags( inside_range_flags );

//    SuperClass::m_min_ranges = SuperClass::m_min_values;
//    SuperClass::m_max_ranges = SuperClass::m_max_values;
}

} // end of namespace pcs

} // end of namespace kvsoceanvis
