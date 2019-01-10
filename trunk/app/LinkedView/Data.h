/*****************************************************************************/
/**
 *  @file   Data.h
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
#ifndef DATA_H_INCLUDE
#define DATA_H_INCLUDE

#include <string>
#include <list>


class Data
{
public:

    class Item;
    class Row;
    typedef std::list<Item> Column;
    typedef std::list<Row> Table;

private:

    Table* m_table;

public:

    Data();
    ~Data();

    size_t numberOfVariables() const;
    size_t numberOfTimeSteps() const;

    void insert( const Row& data );
    void insert( const Column& data );
    void remove( const int year, const int month, const int day );
    void remove( const std::string& variable );
    void sort();
    const Row* find( const Row& row ) const;
};

class Data::Item
{
private:

    std::string m_variable;
    std::string m_description;
    std::string m_filename;

public:

    Item( const std::string& variable, const std::string& description, const std::string& filename );

    bool operator == ( const Item& rhs ) const;
};

class Data::Row
{
public:

    typedef std::list<Data::Item> DataType;

private:

    int m_year;
    int m_month;
    int m_day;
    DataType m_data;

public:

    Row( const int year, const int month, const int day );

    void insert( const Data::Item& item );
    void remove( const Data::Item& item );
    const Data::Item* find( const Data::Item& item ) const;

    bool operator == ( const Data::Row& rhs ) const;
    bool operator < ( const Data::Row& rhs ) const;
};

#endif // DATA_H_INCLUDE
