/*****************************************************************************/
/**
 *  @file   MultiDimensionalBinning.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: MultiDimensionalBinning.h 18 2011-12-18 17:00:59Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef MULTI_DIMENSIONAL_BINNING_H_INCLUDE
#define MULTI_DIMENSIONAL_BINNING_H_INCLUDE

#include "Command.h"


/*===========================================================================*/
/**
 *  @brief  MultiDimensionalBinning command class.
 */
/*===========================================================================*/
class MultiDimensionalBinning : public Command
{
public:

    DefineCommandBaseClass( Command );
    DefineCommandName( "MultiDimensionalBinning" );
    DefineCommandDescription( "Multi-dimensional binning" );

public:

    MultiDimensionalBinning( int argc, char** argv );

    int exec( void );
};

#endif // MULTI_DIMENSIONAL_BINNING_H_INCLUDE
