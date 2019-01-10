/*****************************************************************************/
/**
 *  @file   GrADS2Table.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: GrADS2Table.h 18 2011-12-18 17:00:59Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef GRADS_TABLE_H_INCLUDE
#define GRADS_TABLE_H_INCLUDE

#include "Command.h"


/*===========================================================================*/
/**
 *  @brief  GrADS2Table command class.
 */
/*===========================================================================*/
class GrADS2Table : public Command
{
public:

    DefineCommandBaseClass( Command );
    DefineCommandName( "GrADS2Table" );
    DefineCommandDescription( "Convert GrADS data to Table data" );

public:

    GrADS2Table( int argc, char** argv );

    int exec( void );
};

#endif // GRADS_TABLE_H_INCLUDE
