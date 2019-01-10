/*****************************************************************************/
/**
 *  @file   TransferFunctionEditor.h
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
#ifndef KVSOCEANVIS__UTIL__TRANSFER_FUNCTION_H_INCLUDE
#define KVSOCEANVIS__UTIL__TRANSFER_FUNCTION_H_INCLUDE

#include <kvs/glut/Screen>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RendererManager>
#include <string>


namespace kvsoceanvis
{

namespace util
{

template <typename Renderer>
class TransferFunctionEditor : public kvs::glut::TransferFunctionEditor
{
    std::string m_renderer_name;

public:

    TransferFunctionEditor( kvs::glut::Screen* screen, std::string renderer_name ):
        kvs::glut::TransferFunctionEditor( screen )
    {
        m_renderer_name = renderer_name;
    }

    void apply( void )
    {
        kvs::glut::Screen* glut_screen = dynamic_cast<kvs::glut::Screen*>( screen() );
        if ( glut_screen )
        {
            kvs::RendererManager* manager = glut_screen->scene()->rendererManager();
            Renderer* renderer = static_cast<Renderer*>( manager->renderer(m_renderer_name) );
            renderer->setTransferFunction( transferFunction() );
        }
        screen()->redraw();
    }
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__TRANSFER_FUNCTION_H_INCLUDE
