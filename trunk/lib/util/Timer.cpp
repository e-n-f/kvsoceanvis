/*****************************************************************************/
/**
 *  @file   Timer.cpp
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
#include "Timer.h"
#include <iostream>


namespace kvsoceanvis
{

namespace util
{

Timer::Timer( void )
{
    this->reset();
}

Timer::Timer( const std::string& message )
{
    this->reset();
    this->start( message );
}

void Timer::start( const std::string& message )
{
    std::cout << message << " ... " << std::flush;
    m_timer.start();
}

void Timer::stop( void )
{
    m_timer.stop();

    const float msec = m_timer.msec();
    std::cout << "done. [" << msec << " msec]" << std::endl; \

    m_total += msec;
}

void Timer::reset( void )
{
    m_total = 0.0f;
}

float Timer::total( void ) const
{
    return m_total;
}

} // end of namespace util

} // end of namespace kvsoceanvis
