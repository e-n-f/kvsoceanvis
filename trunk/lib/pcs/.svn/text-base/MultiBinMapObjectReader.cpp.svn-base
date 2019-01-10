/*****************************************************************************/
/**
 *  @file   MultiBinMapObjectReader.cpp
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
#include "MultiBinMapObjectReader.h"
#include <fstream>
#include <cstdlib>


namespace kvsoceanvis
{

namespace pcs
{

MultiBinMapObjectReader::MultiBinMapObjectReader( const std::string filename )
{
    this->read( filename );
}

void MultiBinMapObjectReader::read( const std::string filename )
{
    std::ifstream ifs( filename.c_str() );

    int nrows = 0;
    ifs.read( (char*)(&nrows), sizeof(int) );
    MultiBinMapObject::setNumberOfRows( nrows );

    int ncolumns = 0;
    ifs.read( (char*)(&ncolumns), sizeof(int) );
    MultiBinMapObject::setNumberOfColumns( ncolumns );

    int npoints = 0;
    ifs.read( (char*)(&npoints), sizeof(int) );
    MultiBinMapObject::m_npoints = npoints;

    int bin_list_size = 0;
    ifs.read( (char*)(&bin_list_size), sizeof(int) );

    for ( size_t i = 0; i < size_t(bin_list_size); i++ )
    {
        int counter = 0;
        ifs.read( (char*)(&counter), sizeof(int) );

        int nindices = 0;
        ifs.read( (char*)(&nindices), sizeof(int) );

        kvs::ValueArray<kvs::UInt16> indices( nindices );
        ifs.read( (char*)(indices.pointer()), indices.byteSize() );

        Bin bin( indices );
        for ( int i = 0; i < counter; i++ ) bin.count();

        MultiBinMapObject::m_bin_list.push_back( bin );
    }

    int labels_size = 0;
    ifs.read( (char*)(&labels_size), sizeof(int) );

    MultiBinMapObject::Labels labels;
    for ( size_t i = 0; i < size_t(labels_size); i++ )
    {
        int label_size = 0;
        ifs.read( (char*)(&label_size), sizeof(int) );

        char* label = new char [label_size];
        ifs.read( label, label_size );

        labels.push_back( std::string( label, label_size ) );
    }
    MultiBinMapObject::setLabels( labels );

    int nbins_size = 0;
    ifs.read( (char*)(&nbins_size), sizeof(int) );
    MultiBinMapObject::m_nbins.allocate( nbins_size );
    ifs.read( (char*)(MultiBinMapObject::m_nbins.data()), m_nbins.byteSize() );

    int min_values_size = 0;
    ifs.read( (char*)(&min_values_size), sizeof(int) );
//    m_min_values.resize( min_values_size );
//    ifs.read( (char*)(&m_min_values[0]), m_min_values.size() * sizeof(kvs::Real64) );
    MultiBinMapObject::Values min_values( min_values_size );
    ifs.read( (char*)(&min_values[0]), min_values.size() * sizeof(kvs::Real64) );
    MultiBinMapObject::setMinValues( min_values );

    int max_values_size = 0;
    ifs.read( (char*)(&max_values_size), sizeof(int) );
//    m_max_values.resize( max_values_size );
//    ifs.read( (char*)(&m_max_values[0]), m_max_values.size() * sizeof(kvs::Real64) );
    MultiBinMapObject::Values max_values( max_values_size );
    ifs.read( (char*)(&max_values[0]), max_values.size() * sizeof(kvs::Real64) );
    MultiBinMapObject::setMaxValues( max_values );

    int min_ranges_size = 0;
    ifs.read( (char*)(&min_ranges_size), sizeof(int) );
//    m_min_ranges.resize( min_ranges_size );
//    ifs.read( (char*)(&m_min_ranges[0]), m_min_ranges.size() * sizeof(kvs::Real64) );
    MultiBinMapObject::Values min_ranges( min_ranges_size );
    ifs.read( (char*)(&min_ranges[0]), min_ranges.size() * sizeof(kvs::Real64) );
    MultiBinMapObject::setMinRanges( min_ranges );

    int max_ranges_size = 0;
//    ifs.read( (char*)(&max_ranges_size), sizeof(int) );
//    m_max_ranges.resize( max_values_size );
//    ifs.read( (char*)(&m_max_ranges[0]), m_max_ranges.size() * sizeof(kvs::Real64) );
    MultiBinMapObject::Values max_ranges( max_ranges_size );
    ifs.read( (char*)(&max_ranges[0]), max_ranges.size() * sizeof(kvs::Real64) );
    MultiBinMapObject::setMaxRanges( max_ranges );
}

} // end of namespace pcs

} // end of namespace kvsoceanvis
