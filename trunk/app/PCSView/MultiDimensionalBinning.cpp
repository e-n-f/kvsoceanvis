/*****************************************************************************/
/**
 *  @file   MultiDimensionalBinning.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: MultiDimensionalBinning.cpp 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "MultiDimensionalBinning.h"
#include <kvs/CommandLine>
#include <kvs/File>
#include <kvs/Timer>
#include <kvs/Tokenizer>
#include <kvs/TableImporter>

#include <pcs/OutOfCoreTableObject.h>
#include <pcs/OutOfCoreTableImporter.h>
#include <pcs/OutOfCoreMultiBinMapping.h>
#include <pcs/MultiBinMapping.h>
#include <pcs/MultiBinMapObject.h>
#include <pcs/MultiBinMapObjectWriter.h>


using namespace kvsoceanvis;

namespace
{

/*===========================================================================*/
/**
 *  @brief  Returns number of bins calculated by using Squre-root Choice.
 *  @param  table [in] pointer to the table
 *  @param  index [in] column index
 *  @return number of bins
 */
/*===========================================================================*/
template <typename TableObject>
size_t SquareRootChoice( const TableObject* table, const size_t index )
{
    const kvs::AnyValueArray array = table->column(index);
    const kvs::Real64 n = array.size();

    return( size_t( std::sqrt( n ) ) );
}

/*===========================================================================*/
/**
 *  @brief  Returns number of bins calculated by using Sturges' Formula.
 *  @param  table [in] pointer to the table
 *  @param  index [in] column index
 *  @return number of bins
 */
/*===========================================================================*/
template <typename TableObject>
size_t SturgesFormula( const TableObject* table, const size_t index )
{
    const kvs::AnyValueArray array = table->column(index);
    const kvs::Real64 n = array.size();

//    return( std::ceil( log2(n) + 1 ) );
    return( size_t( std::ceil( std::log(n) / std::log(2.0) + 1 ) ) );
}

/*===========================================================================*/
/**
 *  @brief  Returns number of bins calculated by using Scott's Choice.
 *  @param  table [in] pointer to the table
 *  @param  index [in] column index
 *  @return number of bins
 */
/*===========================================================================*/
template <typename TableObject>
size_t ScottChoice( const TableObject* table, const size_t index )
{
    const kvs::AnyValueArray& array = table->column(index);
    const size_t nvalues = array.size();

    // Calculate mean value.
    kvs::Real64 mean = 0.0;
    for ( size_t i = 0; i < nvalues; i++ ) { mean += array.at<kvs::Real64>(i); }
    mean = mean / nvalues;

    // Calculate standard deviation.
    kvs::Real64 sdev = 0.0;
    for ( size_t i = 0; i < nvalues; i++ )
    {
        const kvs::Real64 value = array.at<kvs::Real64>(i);
        sdev += ( value - mean ) * ( value - mean );
    }
    sdev = std::sqrt( sdev / nvalues );

    const kvs::Real64 h = 3.5 * sdev / std::pow( nvalues, 1.0 / 3.0 );
    const kvs::Real64 min = table->minRange(index);
    const kvs::Real64 max = table->maxRange(index);

    return( std::ceil( ( max - min ) / h ) );
}

// Binning functions.
typedef size_t (*BinningFunction)( const kvs::TableObject* table, const size_t index );
BinningFunction Binning[] = {
    SquareRootChoice<kvs::TableObject>,
    SturgesFormula<kvs::TableObject>,
    ScottChoice<kvs::TableObject>
};

// Out-Of-Core binning functions.
typedef size_t (*OutOfCoreBinningFunction)( const pcs::OutOfCoreTableObject* table, const size_t index );
OutOfCoreBinningFunction OutOfCoreBinning[] = {
    SquareRootChoice<pcs::OutOfCoreTableObject>,
    SturgesFormula<pcs::OutOfCoreTableObject>,
    ScottChoice<pcs::OutOfCoreTableObject>
};

}

/*===========================================================================*/
/**
 *  @brief  Constructs a new MultiDimensionalBinning command class.
 *  @param  argc [in] argument count
 *  @param  argv [in] argument values
 */
/*===========================================================================*/
MultiDimensionalBinning::MultiDimensionalBinning( int argc, char** argv ):
    Command( argc, argv )
{
}

/*===========================================================================*/
/**
 *  @brief  Executes multi-dimensional binning command.
 *  @return true if the process is done successfully
 */
/*===========================================================================*/
int MultiDimensionalBinning::exec( void )
{
    const std::string name = MultiDimensionalBinning::CommandName();
    const std::string desc = MultiDimensionalBinning::CommandDescription();
    const std::string command = std::string( BaseClass::argv(0) ) + " -" + name;
    kvs::CommandLine commandline( BaseClass::argc(), BaseClass::argv(), command );
    commandline.addHelpOption("help");
    commandline.addOption( name, desc + "." );
    commandline.addOption( "verbose", "Verbose output.", 0, false );
    commandline.addOption( "o", "Output filename. (default: <basename of input file>.mbin)", 1, false );
    commandline.addOption( "out_of_core", "Out-of-Core processing.", 0, false );
    commandline.addOption( "cache", "Cache size [mega-byte]. (default: 0)", 1, false );
    commandline.addOption( "nbins", "Number of bins. (default: none)", 1, false );
    commandline.addOption( "binning", "Binning method. (default: 0)\n"
                           "\t      0 = Square-root Choice\n"
                           "\t      1 = Sturges' Formula\n"
                           "\t      2 = Scott's Choice", 1, false );
    commandline.addValue( "input data file", false );
    if ( !commandline.parse() ) return( false );

    const std::string filename( commandline.value<std::string>() );
    if ( !kvs::File( filename ).isExisted() )
    {
        kvsMessageError( "%s is not existed.", filename.c_str() );
        return( false );
    }

    // Verbose mode.
    const bool verbose = commandline.hasOption("verbose");

    kvs::TableObject* table = NULL;
    if ( commandline.hasOption("out_of_core") )
    {
        if ( verbose ) std::cout << "Importing (Out-of-Core) " << filename << " ... " << std::flush;
        size_t cache_size = commandline.hasOption("cache") ? commandline.optionValue<size_t>("cache") : 0;
        table = new pcs::OutOfCoreTableImporter( filename, kvs::UInt64( cache_size * 1024 * 1024 ) );
        if ( !table )
        {
            kvsMessageError( "Cannot create table object." );
            return( false );
        }
        if ( verbose ) std::cout << "done." << std::endl;
    }
    else
    {
        if ( verbose ) std::cout << "Importing " << filename << " ... " << std::flush;
        table = new kvs::TableImporter( filename );
        if ( !table )
        {
            kvsMessageError( "Cannot create table object." );
            return( false );
        }
        if ( verbose ) std::cout << "done." << std::endl;
    }

    if ( verbose )
    {
        std::cout << "  Number of columns: " << table->numberOfColumns() << std::endl;
        std::cout << "  Number of nrows: " << table->numberOfRows() << std::endl;
    }

    // Number of bins.
    const size_t ncolumns = table->numberOfColumns();
    kvs::ValueArray<kvs::UInt32> nbins( ncolumns );
    for ( size_t i = 0; i < ncolumns; i++ ) { nbins[i] = 1; }
    if ( commandline.hasOption("nbins") )
    {
        // Set by the specified values.
        std::vector<kvs::UInt32> temp;
        kvs::Tokenizer t( commandline.optionValue<std::string>("nbins"), ",\n\t\0" );
        while ( !t.isLast() )
        {
            const std::string value = t.token();
            temp.push_back( kvs::UInt32( std::atoi( value.c_str() ) ) );
        }

        if ( temp.size() == 1 )
        {
            const kvs::UInt32 value = temp[0];
            for ( size_t i = 0; i < ncolumns; i++ ) { nbins[i] = value; }
        }
        else if ( temp.size() > 1 )
        {
            const size_t size = temp.size() > ncolumns ? ncolumns : temp.size();
            for ( size_t i = 0; i < size; i++ ) { nbins[i] = temp[i]; }
        }
    }
    else
    {
        // Estimate the number of bins by the specified method.
        size_t method = 0;
        if ( commandline.hasOption("binning") )
        {
            method = commandline.optionValue<size_t>("binning");
            if ( method > 2 ) method = 2;
        }

        if ( commandline.hasOption("out_of_core") )
        {
            pcs::OutOfCoreTableObject* t = static_cast<pcs::OutOfCoreTableObject*>(table);
            for ( size_t i = 0; i < ncolumns; i++ ) { nbins[i] = ::OutOfCoreBinning[method]( t, i ); }
        }
        else
        {
            for ( size_t i = 0; i < ncolumns; i++ ) { nbins[i] = ::Binning[method]( table, i ); }
        }
    }

    size_t total_nbins = nbins[0];
    for ( size_t i = 1; i < nbins.size(); i++ ) total_nbins *= nbins[i];

    if ( verbose )
    {
        std::cout << "  Number of bins: " << total_nbins << " (";
        for ( size_t i = 0; i < ncolumns; i++ )
        {
            std::cout << table->label(i) << ":" << nbins[i];
            if ( i < ncolumns - 1 ) std::cout << ",";
        }
        std::cout << ")" << std::endl;
    }

    // Binning.
    pcs::MultiBinMapObject* object = NULL;
    if ( commandline.hasOption("out_of_core") )
    {
        if ( verbose ) std::cout << "Binning (Out-of-Core) ... " << std::flush;
        kvs::Timer timer( kvs::Timer::Start );
        object = new pcs::OutOfCoreMultiBinMapping( table, nbins );
        timer.stop();
        delete table;
        if ( verbose ) std::cout << "done. [" << timer.msec() << " msec]" << std::endl;
    }
    else
    {
        if ( verbose ) std::cout << "Binning ... " << std::flush;
        kvs::Timer timer( kvs::Timer::Start );
        object = new pcs::MultiBinMapping( table, nbins );
        timer.stop();
        delete table;
        if ( verbose ) std::cout << "done. [" << timer.msec() << " msec]" << std::endl;
    }

    // Writting the binned data.
    std::string ofilename = kvs::File( filename ).baseName() + ".mbin";
    if ( commandline.hasOption("o") ) ofilename = commandline.optionValue<std::string>("o");

    if ( verbose ) std::cout << "Writting " << ofilename << " ... " << std::flush;
    pcs::MultiBinMapObjectWriter writer( object );
    writer.write( ofilename );
    if ( verbose ) std::cout << "done." << std::endl;

    if ( verbose )
    {
        size_t active_bins = object->binList().size();
        std::cout << "  Number of sample points: " << object->npoints() << std::endl;
        std::cout << "  Number of active bins: " << active_bins << std::endl;
        std::cout << "  Bin occupancy: " << float( active_bins ) * 100.0 / total_nbins << " %" << std::endl;
    }

    delete object;

    return( 0 );
}
