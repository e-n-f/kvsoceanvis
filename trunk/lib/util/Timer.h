/*****************************************************************************/
/**
 *  @file   Timer.h
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
#ifndef KVSOCEANVIS__UTIL__TIMER_H_INCLUDE
#define KVSOCEANVIS__UTIL__TIMER_H_INCLUDE

#include <kvs/Timer>


namespace kvsoceanvis
{

namespace util
{

class Timer
{
    kvs::Timer m_timer;
    float m_total;

public:

    Timer( void );
    Timer( const std::string& message );

    void start( const std::string& message );
    void stop( void );
    void reset( void );
    float total( void ) const;
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__TIMER_H_INCLUDE
