/*****************************************************************************/
/**
 *  @file   DataBase.h
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
#ifndef DATABASE_H_INCLUDE
#define DATABASE_H_INCLUDE

#include <vector>
#include <kvs/GrADS>


class DataBase
{
public:

    typedef kvs::GrADS Data;
    typedef std::vector<Data> DataList;

private:

    DataList m_data_list;

public:

    Data& data( const size_t index ) { return m_data_list[ index ]; }
    const size_t size() const { return m_data_list.size(); }
    void insert( const Data& data );
};

#endif // DATABASE_H_INCLUDE
