/*****************************************************************************/
/**
 *  @file   Command.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: Command.h 394 2014-10-16 06:17:47Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#ifndef COMMAND_H_INCLUDE
#define COMMAND_H_INCLUDE

#include <string>

#define DefineCommandBaseClass( base_class )    \
    typedef base_class BaseClass

#define DefineCommandName( name )               \
    static std::string CommandName( void )      \
    {                                           \
        return std::string( name );             \
    }

#define DefineCommandDescription( description )         \
    static std::string CommandDescription( void )       \
    {                                                   \
        return std::string( description );              \
    }

/*===========================================================================*/
/**
 *  @brief  Command class.
 */
/*===========================================================================*/
class Command
{
private:

    int m_argc; ///< number of arguments
    char** m_argv; ///< argument values

public:

    Command( int argc, char** argv );
    virtual ~Command() {}

    virtual int exec() = 0;
    int argc() const;
    char** argv() const;
    char* argv( const size_t index ) const;
};

#endif // COMMAND_H_INCLUDE
