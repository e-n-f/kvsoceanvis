/*****************************************************************************/
/**
 *  @file   GrADS2Table.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: GrADS2Table.cpp 32 2011-12-26 14:28:45Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "GrADS2Table.h"
#include <iostream>
#include <kvs/GrADS>
#include <kvs/File>
#include <kvs/Math>
#include <kvs/Value>
#include <kvs/CommandLine>
#include <kvs/ValueArray>


namespace
{

/*===========================================================================*/
/**
 *  @brief  Returns value array specified by tindex and vindex.
 *  @param  grads [in] GrADS data
 *  @param  tindex [in] index of time step
 *  @param  vindex [in] index of variable
 *  @param  min_value [out] min. value
 *  @param  max_value [out] max. value
 */
/*===========================================================================*/
kvs::ValueArray<float> GetValues(
    const kvs::GrADS& grads,
    const size_t tindex,
    const size_t vindex,
    float* min_value,
    float* max_value )
{
    const size_t dimx = grads.dataDescriptor().xdef().num;
    const size_t dimy = grads.dataDescriptor().ydef().num;
    const size_t dimz = grads.dataDescriptor().zdef().num;

    const size_t size = dimx * dimy * dimz;
    kvs::ValueArray<float> values( size );
    float* pvalues = values.pointer();

    const float ignore_value = grads.dataDescriptor().undef().value;
    grads.dataList().at(tindex).load();
    {
        const float* src = grads.dataList().at(tindex).values().pointer() + size * vindex;
        for ( size_t i = 0; i < size; i++ )
        {
            pvalues[i] = src[i];

            if ( !kvs::Math::Equal( src[i], ignore_value ) )
            {
                *min_value = kvs::Math::Min( src[i], *min_value );
                *max_value = kvs::Math::Max( src[i], *max_value );
            }
        }
    }
    grads.dataList().at(tindex).free();

    return( values );
}

}

/*===========================================================================*/
/**
 *  @brief  Constructs a new GrADS2Table command class.
 *  @param  argc [in] argumnet count
 *  @param  argv [in] argument values
 */
/*===========================================================================*/
GrADS2Table::GrADS2Table( int argc, char** argv ):
    Command( argc, argv )
{
}

/*===========================================================================*/
/**
 *  @brief  Executes GrADS-to-Table converting command.
 *  @return true if the process is done successfully
 */
/*===========================================================================*/
int GrADS2Table::exec( void )
{
    const std::string name = GrADS2Table::CommandName();
    const std::string desc = GrADS2Table::CommandDescription();
    const std::string command = std::string( BaseClass::argv(0) ) + " -" + name;
    kvs::CommandLine commandline( BaseClass::argc(), BaseClass::argv(), command );
    commandline.addHelpOption("help");
    commandline.addOption( name, desc + "." );
    commandline.addOption( "verbose", "Verbose output.", 0, false );
    commandline.addOption( "o", "Output filename. (default: <basename of input file>.kvsml)", 1, false );
    commandline.addValue( "input data file", false );
    if ( !commandline.parse() ) return( false );

    // Input filename.
    const std::string filename( commandline.value<std::string>() );
    if ( !kvs::File( filename ).isExisted() )
    {
        kvsMessageError( "%s is not existed.", filename.c_str() );
        return( false );
    }

    // Verbose mode.
    const bool verbose = commandline.hasOption("verbose");

    // Read GrADS data file.
    if ( verbose ) std::cout << "Reading " << filename << " ... " << std::flush;
    kvs::GrADS grads( filename );
    if ( grads.isFailure() )
    {
        kvsMessageError( "Cannot read %s.", filename.c_str() );
        return( false );
    }
    if ( verbose ) std::cout << "done." << std::endl;

    // Parameters represents the data size.
    const size_t dimx = grads.dataDescriptor().xdef().num;
    const size_t dimy = grads.dataDescriptor().ydef().num;
    const size_t dimz = grads.dataDescriptor().zdef().num;
    const size_t dimt = grads.dataList().size();
    const size_t nvars = grads.dataDescriptor().vars().values.size();
    if ( verbose )
    {
        std::cout << "  Title: " << grads.dataDescriptor().title().name << std::endl;
        std::cout << "  Number of grid points in the x direction: " << dimx << std::endl;
        std::cout << "  Number of grid points in the y direction: " << dimy << std::endl;
        std::cout << "  Number of grid points in the z direction: " << dimz << std::endl;
        std::cout << "  Number of time steps: " << dimt << std::endl;
        std::cout << "  Number of variables: " << nvars << std::endl;
    }

    std::string ofilename = kvs::File( grads.filename() ).baseName() + ".kvsml";
    if ( commandline.hasOption("o") ) ofilename = commandline.optionValue<std::string>("o");

    std::ofstream kvsml( ofilename.c_str() );
    kvsml << "<KVSML>" << std::endl;
    kvsml << "\t<Object type=\"Table\">" << std::endl;

    const size_t nrows = dimx * dimy * dimz * dimt;
    const size_t ncolumns = nvars + 4; // x + y + z + var0 + var1 + ... + t
    kvsml << "\t\t<TableObject "
          << "nrows=\"" << nrows << "\" "
          << "ncolumns=\"" << ncolumns << "\">" << std::endl;

    // Output x, y and z coordinates.
    {
        // X coordinates.
        if ( verbose ) std::cout << "Reading x coordinates ... " << std::flush;
        kvs::ValueArray<kvs::Real32> xcoords;
        if ( grads.dataDescriptor().xdef().mapping == kvs::grads::XDef::Levels )
        {
            xcoords = grads.dataDescriptor().xdef().values;
        }
        else if ( grads.dataDescriptor().xdef().mapping == kvs::grads::XDef::Linear )
        {
            kvs::Real32 value = grads.dataDescriptor().xdef().values[0];
            const kvs::Real32 increment = grads.dataDescriptor().xdef().values[1];
            xcoords.allocate( dimx );
            for ( size_t i = 0; i < dimx; i++, value += increment ) xcoords[i] = value;
        }
        if ( verbose ) std::cout << "done." << std::endl;

        // Y coordinates.
        if ( verbose ) std::cout << "Reading y coordinates ... " << std::flush;
        kvs::ValueArray<kvs::Real32> ycoords;
        if ( grads.dataDescriptor().ydef().mapping == kvs::grads::YDef::Levels )
        {
            ycoords = grads.dataDescriptor().ydef().values;
        }
        else if ( grads.dataDescriptor().ydef().mapping == kvs::grads::YDef::Linear )
        {
            kvs::Real32 value = grads.dataDescriptor().ydef().values[0];
            const kvs::Real32 increment = grads.dataDescriptor().ydef().values[1];
            ycoords.allocate( dimy );
            for ( size_t i = 0; i < dimy; i++, value += increment ) ycoords[i] = value;
        }
        if ( verbose ) std::cout << "done." << std::endl;

        // Z coordinates.
        if ( verbose ) std::cout << "Reading z coordinates ... " << std::flush;
        kvs::ValueArray<kvs::Real32> zcoords;
        if ( grads.dataDescriptor().zdef().mapping == kvs::grads::ZDef::Levels )
        {
            zcoords = grads.dataDescriptor().zdef().values;
        }
        else if ( grads.dataDescriptor().zdef().mapping == kvs::grads::ZDef::Linear )
        {
            kvs::Real32 value = grads.dataDescriptor().zdef().values[0];
            const kvs::Real32 increment = grads.dataDescriptor().zdef().values[1];
            zcoords.allocate( dimz );
            for ( size_t i = 0; i < dimz; i++, value += increment ) zcoords[i] = value;
        }
        if ( verbose ) std::cout << "done." << std::endl;

        const std::string xvarname = "lon";
        const std::string xfilename = kvs::File( ofilename ).baseName() + "_" + xvarname + ".dat";
        std::ofstream xofs( xfilename.c_str(), std::ios::out | std::ios::trunc );
        xofs.close();

        const std::string yvarname = "lat";
        const std::string yfilename = kvs::File( ofilename ).baseName() + "_" + yvarname + ".dat";
        std::ofstream yofs( yfilename.c_str(), std::ios::out | std::ios::trunc );
        yofs.close();

        const std::string zvarname = "depth";
        const std::string zfilename = kvs::File( ofilename ).baseName() + "_" + zvarname + ".dat";
        std::ofstream zofs( zfilename.c_str(), std::ios::out | std::ios::trunc );
        zofs.close();

        if ( verbose ) std::cout << "Aligning x, y and z coordinates ... " << std::flush;
        const size_t size = dimx * dimy * dimz;
        kvs::Real32* x = new kvs::Real32 [size];
        kvs::Real32* y = new kvs::Real32 [size];
        kvs::Real32* z = new kvs::Real32 [size];
        for ( size_t k = 0, index = 0; k < dimz; ++k )
        {
            for ( size_t j = 0; j < dimy; ++j )
            {
                for ( size_t i = 0; i < dimx; ++i, ++index )
                {
                    x[index] = xcoords[i];
                    y[index] = ycoords[j];
                    z[index] = zcoords[k];
                }
            }
        }
        if ( verbose ) std::cout << "done." << std::endl;

        if ( verbose ) std::cout << "Writing " << xfilename << " ... " << std::flush;
        for ( size_t k = 0; k < dimt; ++k )
        {
            xofs.open( xfilename.c_str(), std::ios::out | std::ios::app );
            xofs.write( (char*)(x), sizeof(float) * size );
            xofs.close();
        }
        if ( verbose ) std::cout << "done." << std::endl;
        delete [] x;

        if ( verbose ) std::cout << "Writing " << yfilename << " ... " << std::flush;
        for ( size_t k = 0; k < dimt; ++k )
        {
            yofs.open( yfilename.c_str(), std::ios::out | std::ios::app );
            yofs.write( (char*)(y), sizeof(float) * size );
            yofs.close();
        }
        if ( verbose ) std::cout << "done." << std::endl;
        delete [] y;

        if ( verbose ) std::cout << "Writing " << zfilename << " ... " << std::flush;
        for ( size_t k = 0; k < dimt; ++k )
        {
            zofs.open( zfilename.c_str(), std::ios::out | std::ios::app );
            zofs.write( (char*)(z), sizeof(float) * size );
            zofs.close();
        }
        if ( verbose ) std::cout << "done." << std::endl;
        delete [] z;

        kvsml << "\t\t\t<Column "
              << "label=\"" << xvarname << "\">" << std::endl;
        kvsml << "\t\t\t\t<DataArray "
              << "type=\"float\" "
              << "file=\"" << xfilename << "\" "
              << "format=\"binary\"/>" << std::endl;
        kvsml << "\t\t\t</Column>" << std::endl;

        kvsml << "\t\t\t<Column "
              << "label=\"" << yvarname << "\">" << std::endl;
        kvsml << "\t\t\t\t<DataArray "
              << "type=\"float\" "
              << "file=\"" << yfilename << "\" "
              << "format=\"binary\"/>" << std::endl;
        kvsml << "\t\t\t</Column>" << std::endl;

        kvsml << "\t\t\t<Column "
              << "label=\"" << zvarname << "\">" << std::endl;
        kvsml << "\t\t\t\t<DataArray "
              << "type=\"float\" "
              << "file=\"" << zfilename << "\" "
              << "format=\"binary\"/>" << std::endl;
        kvsml << "\t\t\t</Column>" << std::endl;
    }

    // Output variables.
    std::list<kvs::grads::Vars::Var>::const_iterator var = grads.dataDescriptor().vars().values.begin();
    for ( size_t vindex = 0; vindex < nvars; ++vindex, ++var )
    {
        const std::string varname = var->varname;
        const std::string vfilename = kvs::File( ofilename ).baseName() + "_" + varname + ".dat";

        std::ofstream ofs( vfilename.c_str(), std::ios::out | std::ios::trunc );
        ofs.close();

        float min_value = kvs::Value<float>::Max();
        float max_value = kvs::Value<float>::Min();
        for ( size_t tindex = 0; tindex < dimt; ++tindex )
        {
            if ( verbose ) std::cout << "Reading " << grads.dataList().at(tindex).filename() << " ... " << std::flush;
            kvs::ValueArray<float> values = ::GetValues( grads, tindex, vindex, &min_value, &max_value );
            if ( verbose ) std::cout << "done." << std::endl;

            if ( verbose ) std::cout << "Appending " << vfilename << " ... " << std::flush;
            ofs.open( vfilename.c_str(), std::ios::out | std::ios::app );
            ofs.write( (char*)(values.pointer()), values.byteSize() );
            ofs.close();
            if ( verbose ) std::cout << "done." << std::endl;
        }
        if ( verbose ) std::cout << "Writing " << vfilename << " ... done." << std::endl;

        kvsml << "\t\t\t<Column "
              << "label=\"" << var->description << "\" "
              << "min_value=\"" << min_value << "\" "
              << "max_value=\"" << max_value << "\">" << std::endl;
        kvsml << "\t\t\t\t<DataArray "
              << "type=\"float\" "
              << "file=\"" << vfilename << "\" "
              << "format=\"binary\"/>" << std::endl;
        kvsml << "\t\t\t</Column>" << std::endl;
    }

    // Output date.
    {
        const std::string varname = "date";
        const std::string dfilename = kvs::File( ofilename ).baseName() + "_" + varname + ".dat";

        std::ofstream ofs( dfilename.c_str(), std::ios::out | std::ios::trunc );
        ofs.close();

        if ( verbose ) std::cout << "Writing " << dfilename << " ... " << std::flush;
        int min_value = kvs::Value<int>::Max();
        int max_value = kvs::Value<int>::Min();
        kvs::grads::TDef tdef = grads.dataDescriptor().tdef();
        for ( size_t t = 0; t < dimt; ++t, ++tdef )
        {
            const int year = tdef.start.year;
            const int month = tdef.start.month;
            const int day = tdef.start.day;
            char date[8]; sprintf( date, "%4d%02d%02d", year, month, day );

            min_value = kvs::Math::Min( min_value, atoi(date) );
            max_value = kvs::Math::Max( max_value, atoi(date) );

            const size_t size = dimx * dimy * dimz;
            int* date_num = new int [size];
            for ( size_t i = 0; i < size; ++i ) date_num[i] = atoi(date);

            ofs.open( dfilename.c_str(), std::ios::out | std::ios::app );
            ofs.write( (char*)(date_num), sizeof(int) * size );
            ofs.close();
            delete [] date_num;
        }
        if ( verbose ) std::cout << "done." << std::endl;

        kvsml << "\t\t\t<Column "
              << "label=\"date\" "
              << "min_value=\"" << min_value << "\" "
              << "max_value=\"" << max_value << "\">" << std::endl;
        kvsml << "\t\t\t\t<DataArray "
              << "type=\"int\" "
              << "file=\"" << dfilename << "\" "
              << "format=\"binary\"/>" << std::endl;
        kvsml << "\t\t\t</Column>" << std::endl;
    }

    kvsml << "\t\t</TableObject>" << std::endl;
    kvsml << "\t</Object>" << std::endl;
    kvsml << "</KVSML>" << std::endl;
    kvsml.close();

    if ( verbose ) std::cout << "Writing " << ofilename << " ... done." << std::endl;

    return( 0 );
}
