/*****************************************************************************/
/**
 *  @file   LinkedView.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: LinkedView.h 29 2011-12-22 07:29:14Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef LINKED_VIEW_H_INCLUDE
#define LINKED_VIEW_H_INCLUDE

#include "Command.h"


/*===========================================================================*/
/**
 *  @brief  Linked view command class.
 */
/*===========================================================================*/
class LinkedView : public Command
{
public:

    DefineCommandBaseClass( Command );
    DefineCommandName( "LinkedView" );
    DefineCommandDescription( "Linked view" );

public:

    LinkedView( int argc, char** argv );

    int exec( void );
};

#endif // LINKED_VIEW_H_INCLUDE
