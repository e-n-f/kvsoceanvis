/*****************************************************************************/
/**
 *  @file   CsvReader.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: CsvReader.h 287 2012-06-15 14:20:06Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__UTIL__CSV_READER_H_INCLUDE
#define KVSOCEANVIS__UTIL__CSV_READER_H_INCLUDE

#include <string>
#include <fstream>
#include <cstdlib>
#include <kvs/Type>
#include <kvs/Csv>
#include <kvs/TableObject>


namespace kvsoceanvis
{

namespace util
{

class CsvReader : public kvs::TableObject
{
public:

    CsvReader( const std::string filename );

    void read( const std::string filename );
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__CSV_READER_H_INCLUDE
