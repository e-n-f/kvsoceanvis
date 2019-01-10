/*****************************************************************************/
/**
 *  @file   TableObjectWriter.cpp
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
#include "TableObjectWriter.h"
#include <kvs/File>


namespace kvsoceanvis
{

namespace util
{

TableObjectWriter::TableObjectWriter( const kvs::TableObject* object ):
    m_object( object )
{
}

void TableObjectWriter::write( const std::string filename )
{
    std::ofstream kvsml( filename.c_str() );
    kvsml << "<KVSML>" << std::endl;
    kvsml << "\t<Object type=\"Table\">" << std::endl;

    const size_t nrows = m_object->numberOfRows();
    const size_t ncolumns = m_object->numberOfColumns();
    kvsml << "\t\t<TableObject "
          << "nrows=\"" << nrows << "\" "
          << "ncolumns=\"" << ncolumns << "\">" << std::endl;

    const std::string basename = kvs::File( filename ).baseName();
    for ( size_t i = 0; i < ncolumns; i++ )
    {
        const std::string label = m_object->label(i);
        const std::string type = m_object->column(i).typeInfo()->typeName();
        const float min_value = m_object->minValue(i);
        const float max_value = m_object->maxValue(i);
        const std::string file = basename + "_" + label + ".dat";

        kvsml.setf( std::ios::fixed );
        kvsml << "\t\t\t<Column "
              << "label=\"" << label << "\" "
              << "min_value=\"" << min_value << "\" "
              << "max_value=\"" << max_value << "\">" << std::endl;
        kvsml.unsetf( std::ios::fixed );
        kvsml << "\t\t\t\t<DataArray "
              << "type=\"" << type << "\" "
              << "file=\"" << file << "\" "
              << "format=\"binary\"/>" << std::endl;
        kvsml << "\t\t\t</Column>" << std::endl;

        std::ofstream ofs( file.c_str() );
        ofs.write( (char*)m_object->column(i).data(), m_object->column(i).byteSize() );
        ofs.close();
    }

    kvsml << "\t\t</TableObject>" << std::endl;
    kvsml << "\t</Object>" << std::endl;
    kvsml << "</KVSML>" << std::endl;
    kvsml.close();
}

} // end of namespace util

} // end of namespace kvsoceanvis
