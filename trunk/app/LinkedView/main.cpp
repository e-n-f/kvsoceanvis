/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: main.cpp 350 2013-07-01 01:21:25Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include <kvs/qt/Application>
#include "MainWindow.h"


/*===========================================================================*/
/**
 *  @brief  Main function.
 *  @param  argc [in] argument count
 *  @param  argv [in] argument values
 */
/*===========================================================================*/
int main( int argc, char** argv )
{
    kvs::qt::Application app( argc, argv );

    const int width = 1000;
    const int height = 650;
    MainWindow main_window( &app );
    main_window.resize( width, height );
    main_window.show();
    main_window.raise();

    return app.run();
}
