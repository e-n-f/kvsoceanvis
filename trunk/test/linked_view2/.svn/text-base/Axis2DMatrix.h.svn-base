/*****************************************************************************/
/**
 *  @file   Axis2DMatrix.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: Axis2DMatrix.h 1407 2012-12-10 11:24:32Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#ifndef KVS__GLUT__AXIS_2D_MATRIX_H_INCLUDE
#define KVS__GLUT__AXIS_2D_MATRIX_H_INCLUDE

#include <kvs/RendererBase>
#include <kvs/Module>
#include <kvs/RGBColor>
#include <kvs/RGBAColor>


namespace kvs
{

class ObjectBase;
class Camera;
class Light;

namespace glut
{

/*===========================================================================*/
/**
 *  @brief  Axis2DMatrix class.
 */
/*===========================================================================*/
class Axis2DMatrix : public kvs::RendererBase
{
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

protected:

    int m_top_margin; ///< top margin
    int m_bottom_margin; ///< bottom margin
    int m_left_margin; ///< left margin
    int m_right_margin; ///< right margin
    int m_margin; ///< margin
    kvs::Real32   m_axis_width; ///< axis width
    kvs::RGBColor m_axis_color; ///< axis color
    kvs::RGBColor m_value_color; ///< value color
    kvs::RGBColor m_label_color; ///< label color

public:

    Axis2DMatrix( void );

public:

    void setTopMargin( const int top_margin );

    void setBottomMargin( const int bottom_margin );

    void setLeftMargin( const int left_margin );

    void setRightMargin( const int right_margin );

    void setMargin( const int margin );

    void setAxisWidth( const kvs::Real32 axis_width );

    void setAxisColor( const kvs::RGBColor axis_color );

    void setValueColor( const kvs::RGBColor value_color );

    void setLabelColor( const kvs::RGBColor label_color );

    const int topMargin( void ) const;

    const int bottomMargin( void ) const;

    const int leftMargin( void ) const;

    const int rightMargin( void ) const;

    const kvs::Real32 axisWidth( void ) const;

    const kvs::RGBColor axisColor( void ) const;

    const kvs::RGBColor valueColor( void ) const;

    const kvs::RGBColor labelColor( void ) const;

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );
};

} // end of namespace glut

} // end of namespace kvs

#endif // KVS__GLUT__AXIS_2D_MATRIX_H_INCLUDE
