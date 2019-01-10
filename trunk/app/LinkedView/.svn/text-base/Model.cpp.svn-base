/*****************************************************************************/
/**
 *  @file   Model.cpp
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
#include "Model.h"
#include "View.h"
#include <kvs/GrADS>
#include <kvs/TableObject>
#include <kvs/PolygonObject>
#include <kvs/StructuredVolumeObject>
#include <kvs/ColorMap>
#include <kvs/Version>
#include <kvs/File>
#include <kvs/Vector3>
#include <kvs/TableImporter>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/ExternalFaces>
#include <kvs/ObjectManager>
#include <kvs/LineObject>
#include <kvs/LineRenderer>
#include <kvs/PolygonRenderer>
#include <kvs/Bounds>
#include <kvs/glut/ParallelAxis>
#include <util/CsvReader.h>
#include <util/CropVolume.h>
#include <util/StructuredVolumeImporter.h>

using namespace kvsoceanvis;


Model::Model():
    m_volume_object( 0 ),
    m_table_object( 0 )
{
}

void Model::read( const std::string& filename )
{
    DataBase::Data data( filename );
    m_data_base.insert( data );
}

Model::VolumeObject* Model::volumeObject()
{
    return m_volume_object;
}

Model::TableObject* Model::tableObject()
{
    return m_table_object;
}

bool Model::importVolumeObject( const std::string& varname, const size_t tindex )
{
    if ( m_data_base.size() == 0 ) return false;

    for ( size_t i = 0; i < m_data_base.size(); i++ )
    {
        const DataBase::Data& data = m_data_base.data(i);
        const int index = data.dataDescriptor().vars().indexOf( varname );
        if ( index >= 0 )
        {
            const size_t vindex = static_cast<size_t>( index );
            const bool zflip = true;
            const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
            m_volume_object = new util::StructuredVolumeImporter( &data, vindex, tindex, zflip, grid_type );
            return true;
        }
    }

    return false;
}

bool Model::importTableObject( const size_t tindex )
{
    if ( m_data_base.size() == 0 ) return false;

    const size_t dimx = m_data_base.data(0).dataDescriptor().xdef().num;
    const size_t dimy = m_data_base.data(0).dataDescriptor().ydef().num;
    const size_t dimz = m_data_base.data(0).dataDescriptor().zdef().num;
    const kvs::Vector3ui dim( dimx, dimy, dimz );

    kvs::ValueTable<kvs::Real32> table;
    TableObject::Labels labels;
    std::vector<kvs::Real32> min_values;
    std::vector<kvs::Real32> max_values;
    for ( size_t i = 0; i < m_data_base.size(); i++ )
    {
        typedef std::list<kvs::grads::Vars::Var> VarList;
        const DataBase::Data& data = m_data_base.data(i);
        VarList::const_iterator var = data.dataDescriptor().vars().values.begin();
        VarList::const_iterator end = data.dataDescriptor().vars().values.end();
        size_t vindex = 0;
        const kvs::Real32 ignore_value = data.dataDescriptor().undef().value;
        while ( var != end )
        {
            data.data( tindex ).load();
            kvs::ValueArray<kvs::Real32> column = data.data( tindex ).values( vindex, dim );
            data.data( tindex ).free();

            kvs::Real32 min_value = kvs::Value<kvs::Real32>::Max();
            kvs::Real32 max_value = kvs::Value<kvs::Real32>::Min();
            for ( size_t j = 0; j < column.size(); j++ )
            {
                // NOTE: "column[j] > -999" is a condition for the 'w' column.
                // "!kvs::Math::Equal( column[j], ignore_value )" is valid condition,
                // but this condition dosen't work well for the w column. I don't
                // why it is like that.
                //
                // if ( !kvs::Math::Equal( column[j], ignore_value ) )
                if ( !kvs::Math::Equal( column[j], ignore_value ) && column[j] > -999 )
                {
                    min_value = kvs::Math::Min( column[j], min_value );
                    max_value = kvs::Math::Max( column[j], max_value );
                }
            }

            table.pushBackColumn( column );
            labels.push_back( var->varname );
            min_values.push_back( min_value );
            max_values.push_back( max_value );

            var++;
            vindex++;
        }
    }

    if ( table.columnSize() > 0 )
    {
        m_table_object = new TableObject();
        for ( size_t i = 0; i < table.columnSize(); i++ )
        {
            const std::string label = i < labels.size() ? labels[i] : "";
            m_table_object->addColumn( table.column(i), label );
        }
        for ( size_t i = 0; i < table.columnSize(); i++ )
        {
            const kvs::Real32 min_value = min_values[i];
            const kvs::Real32 max_value = max_values[i];

            m_table_object->setMinRange( i, min_value );
            m_table_object->setMaxRange( i, max_value );
            m_table_object->setMinValue( i, min_value );
            m_table_object->setMaxValue( i, max_value );
        }
        return true;
    }

    return false;
}

Model::VolumeObject* Model::croppedVolumeObject( const kvs::Vec3 min_range, const kvs::Vec3 max_range )
{
    return new util::CropVolume( m_volume_object, min_range, max_range );
}

Model::TableObject* Model::croppedTableObject( const kvs::Vec3 min_range, const kvs::Vec3 max_range )
{
    TableObject* table = new TableObject();
    for ( size_t i = 0; i < m_table_object->columns().size(); i++ )
    {
        VolumeObject* volume = new VolumeObject();
        volume->shallowCopy( *m_volume_object );
        volume->setValues( m_table_object->column(i) );

        VolumeObject* cropped_volume = new util::CropVolume( volume, min_range, max_range );
        table->addColumn( cropped_volume->values(), m_table_object->label(i) );

        const DataBase::Data& data = m_data_base.data(i);
        const kvs::Real32 ignore_value = data.dataDescriptor().undef().value;
        kvs::Real32 min_value = kvs::Value<kvs::Real32>::Max();
        kvs::Real32 max_value = kvs::Value<kvs::Real32>::Min();
        const kvs::ValueArray<kvs::Real32> values = cropped_volume->values().asValueArray<kvs::Real32>();
        for ( size_t j = 0; j < values.size(); j++ )
        {
            if ( !kvs::Math::Equal( values[j], ignore_value ) && values[j] > -999 )
            {
                min_value = kvs::Math::Min( values[j], min_value );
                max_value = kvs::Math::Max( values[j], max_value );
            }
        }

        table->setMinRange( i, min_value );
        table->setMaxRange( i, max_value );
        table->setMinValue( i, min_value );
        table->setMaxValue( i, max_value );

        delete volume;
        delete cropped_volume;
    }

    return table;
}
