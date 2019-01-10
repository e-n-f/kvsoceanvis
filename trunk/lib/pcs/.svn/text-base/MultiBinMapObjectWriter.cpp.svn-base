/*****************************************************************************/
/**
 *  @file   MultiBinMapObjectWriter.cpp
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
#include "MultiBinMapObjectWriter.h"
#include <kvs/File>
#include <fstream>


namespace kvsoceanvis
{

namespace pcs
{

MultiBinMapObjectWriter::MultiBinMapObjectWriter( const pcs::MultiBinMapObject* object ):
    m_object( object )
{
}

void MultiBinMapObjectWriter::write( const std::string filename )
{
    std::ofstream ofs( filename.c_str() );

    int nrows = m_object->numberOfRows();
    ofs.write( (char*)(&nrows), sizeof(int) );

    int ncolumns = m_object->numberOfColumns();
    ofs.write( (char*)(&ncolumns), sizeof(int) );

    int npoints = m_object->npoints();
    ofs.write( (char*)(&npoints), sizeof(int) );

    int bin_list_size = m_object->binList().size();
    ofs.write( (char*)(&bin_list_size), sizeof(int) );

    pcs::MultiBinMapObject::BinList::const_iterator bin = m_object->binList().begin();
    pcs::MultiBinMapObject::BinList::const_iterator last = m_object->binList().end();
    while ( bin != last )
    {
        int counter = bin->counter();
        ofs.write( (char*)(&counter), sizeof(int) );

        int nindices = bin->indices().size();
        ofs.write( (char*)(&nindices), sizeof(int) );
        ofs.write( (char*)(bin->indices().pointer()), bin->indices().byteSize() );

        bin++;
    }

    int labels_size = m_object->labels().size();
    ofs.write( (char*)(&labels_size), sizeof(int) );
    for ( size_t i = 0; i < m_object->labels().size(); i++ )
    {
        int label_size = m_object->label(i).size();
        ofs.write( (char*)(&label_size), sizeof(int) );
        ofs.write( m_object->label(i).c_str(), m_object->label(i).size() );
    }

    int nbins_size = m_object->nbins().size();
    ofs.write( (char*)(&nbins_size), sizeof(int) );
    ofs.write( (char*)(m_object->nbins().pointer()), m_object->nbins().byteSize() );

    int min_values_size = m_object->minValues().size();
    ofs.write( (char*)(&min_values_size), sizeof(int) );
    ofs.write( (char*)(&m_object->minValues()[0]), min_values_size * sizeof(kvs::Real64) );

    int max_values_size = m_object->maxValues().size();
    ofs.write( (char*)(&max_values_size), sizeof(int) );
    ofs.write( (char*)(&m_object->maxValues()[0]), max_values_size * sizeof(kvs::Real64) );

    int min_ranges_size = m_object->minRanges().size();
    ofs.write( (char*)(&min_ranges_size), sizeof(int) );
    ofs.write( (char*)(&m_object->minRanges()[0]), min_ranges_size * sizeof(kvs::Real64) );

    int max_ranges_size = m_object->maxRanges().size();
    ofs.write( (char*)(&max_ranges_size), sizeof(int) );
    ofs.write( (char*)(&m_object->maxRanges()[0]), max_ranges_size * sizeof(kvs::Real64) );

    ofs.close();
}

} // end of namespace pcs

} // end of namespace kvsoceanvis
