/*****************************************************************************/
/**
 *  @file   Data.cpp
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
#include "Data.h"
#include <algorithm>
#include <kvs/Date>


Data::Item::Item(
    const std::string& variable,
    const std::string& description,
    const std::string& filename ):
    m_variable( variable ),
    m_description( description ),
    m_filename( filename )
{
}

bool Data::Item::operator == ( const Data::Item& rhs ) const
{
    return m_variable == rhs.m_variable;
}

Data::Row::Row(
    const int year,
    const int month,
    const int day ):
    m_year( year ),
    m_month( month ),
    m_day( day )
{
}

void Data::Row::insert( const Data::Item& item )
{
    m_data.push_back( item );
}

void Data::Row::remove( const Data::Item& item )
{
    m_data.remove( item );
}

const Data::Item* Data::Row::find( const Data::Item& key ) const
{
    DataType::const_iterator v = std::find( m_data.begin(), m_data.end(), key );
    if ( v == m_data.end() ) return NULL;

    return &*v;
}

bool Data::Row::operator == ( const Data::Row& rhs ) const
{
    return kvs::Date( m_year, m_month, m_day ) == kvs::Date( rhs.m_year, rhs.m_month, rhs.m_day );
}

bool Data::Row::operator < ( const Data::Row& rhs ) const
{
    return kvs::Date( m_year, m_month, m_day ) < kvs::Date( rhs.m_year, rhs.m_month, rhs.m_day );
}

Data::Data()
{
    m_table = new Data::Table();
}

Data::~Data()
{
    delete m_table;
}

void Data::insert( const Data::Row& data )
{
    m_table->push_back( data );
}

void Data::insert( const Data::Column& data )
{
    if ( data.size() == m_table->size() )
    {
        Column::const_iterator item = data.begin();
        Table::iterator row = m_table->begin();
        while ( row != m_table->end() )
        {
            row->insert( *item );
            row++;
            item++;
        }
    }
}

void Data::remove( const int year, const int month, const int day )
{
    Data::Row data( year, month, day );
    m_table->remove( data );
}

void Data::remove( const std::string& variable )
{
    Item item( variable, "", "" );
    Table::iterator row = m_table->begin();
    while ( row != m_table->end() )
    {
        row->remove( item );
        row++;
    }
}

void Data::sort()
{
    m_table->sort();
}

const Data::Row* Data::find( const Data::Row& key ) const
{
    Data::Table::const_iterator v = std::find( m_table->begin(), m_table->end(), key );
    if ( v == m_table->end() ) return NULL;

    return &*v;
}
