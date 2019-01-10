/*****************************************************************************/
/**
 *  @file   Command.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: Command.cpp 18 2011-12-18 17:00:59Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "Command.h"


/*===========================================================================*/
/**
 *  @brief  Constructs a new Command class.
 *  @param  argc [in] number of arguments
 *  @param  argv [in] argument values
 */
/*===========================================================================*/
Command::Command( int argc, char** argv ):
    m_argc( argc ),
    m_argv( argv )
{
}

/*===========================================================================*/
/**
 *  @brief  Returns the number of arguments.
 *  @return number of argument
 */
/*===========================================================================*/
int Command::argc( void ) const
{
    return m_argc;
}

/*===========================================================================*/
/**
 *  @brief  Returns the argument values.
 *  @return argument values
 */
/*===========================================================================*/
char** Command::argv( void ) const
{
    return m_argv;
}

/*===========================================================================*/
/**
 *  @brief  Returns the specified argument value.
 *  @param  index [in] index of argument value
 *  @return specified argument value
 */
/*===========================================================================*/
char* Command::argv( const size_t index ) const
{
    return m_argv[ index ];
}
