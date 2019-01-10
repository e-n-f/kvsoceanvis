/*****************************************************************************/
/**
 *  @file   ClusterMappedParallelCoordinatesRenderer.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: ClusterMappedParallelCoordinatesRenderer.cpp 353 2013-07-05 09:34:53Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "ClusterMappedParallelCoordinatesRenderer.h"
#include <cstring>
#include <kvs/OpenGL>
#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/ObjectBase>
#include <kvs/AnyValueArray>
#include "ClusterMapObject.h"


namespace
{

void BeginDraw()
{
    GLint vp[4]; glGetIntegerv( GL_VIEWPORT, vp );
    const GLint left = vp[0];
    const GLint bottom = vp[1];
    const GLint right = vp[2];
    const GLint top = vp[3];

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glMatrixMode( GL_MODELVIEW );  glPushMatrix(); glLoadIdentity();
    glMatrixMode( GL_PROJECTION ); glPushMatrix(); glLoadIdentity();
    glOrtho( left, right, top, bottom, -1, 1 ); // The origin is upper-left.
    glDisable( GL_DEPTH_TEST );
}

void EndDraw()
{
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glPopAttrib();
}

} // end of namespace


namespace kvsoceanvis
{

namespace pcs
{

ClusterMappedParallelCoordinatesRenderer::ClusterMappedParallelCoordinatesRenderer():
    m_top_margin( 20 ),
    m_bottom_margin( 20 ),
    m_left_margin( 30 ),
    m_right_margin( 30 ),
    m_enable_anti_aliasing( false ),
    m_enable_multisample_anti_aliasing( false ),
    m_active_axis( 0 ),
    m_cluster_opacity( 255 ),
    m_cluster_edge_width( 0.0f ),
    m_color_map( 256 )
{
    m_color_map.create();
}

/*===========================================================================*/
/**
 *  @brief  Sets top margin.
 *  @param  top_margin [in] margin
 */
/*===========================================================================*/
void ClusterMappedParallelCoordinatesRenderer::setTopMargin( const int top_margin )
{
    m_top_margin = top_margin;
}

/*===========================================================================*/
/**
 *  @brief  Sets bottom margin.
 *  @param  bottom_margin [in] margin
 */
/*===========================================================================*/
void ClusterMappedParallelCoordinatesRenderer::setBottomMargin( const int bottom_margin )
{
    m_bottom_margin = bottom_margin;
}

/*===========================================================================*/
/**
 *  @brief  Sets left margin.
 *  @param  left_margin [in] left margin
 */
/*===========================================================================*/
void ClusterMappedParallelCoordinatesRenderer::setLeftMargin( const int left_margin )
{
    m_left_margin = left_margin;
}

/*===========================================================================*/
/**
 *  @brief  Sets right margin.
 *  @param  right_margin [in] right margin
 */
/*===========================================================================*/
void ClusterMappedParallelCoordinatesRenderer::setRightMargin( const int right_margin )
{
    m_right_margin = right_margin;
}

/*===========================================================================*/
/**
 *  @brief  Enables anti-aliasing mode.
 *  @param  multisample [in] if true, multisampling is available
 */
/*===========================================================================*/
void ClusterMappedParallelCoordinatesRenderer::enableAntiAliasing( const bool multisample ) const
{
    m_enable_anti_aliasing = true;
    m_enable_multisample_anti_aliasing = multisample;
}

/*===========================================================================*/
/**
 *  @brief  Disables anti-aliasing mode.
 */
/*===========================================================================*/
void ClusterMappedParallelCoordinatesRenderer::disableAntiAliasing() const
{
    m_enable_anti_aliasing = false;
    m_enable_multisample_anti_aliasing = false;
}

void ClusterMappedParallelCoordinatesRenderer::setClusterOpacity( const kvs::UInt8 opacity )
{
    m_cluster_opacity = opacity;
}

void ClusterMappedParallelCoordinatesRenderer::setClusterEdgeWidth( const kvs::Real32 width )
{
    m_cluster_edge_width = width;
}

void ClusterMappedParallelCoordinatesRenderer::setColorMap( const kvs::ColorMap& color_map )
{
    m_color_map = color_map;
}

void ClusterMappedParallelCoordinatesRenderer::selectAxis( const size_t index )
{
    m_active_axis = index;
}

/*===========================================================================*/
/**
 *  @brief  Returns top margin.
 *  @return top margin
 */
/*===========================================================================*/
int ClusterMappedParallelCoordinatesRenderer::topMargin() const
{
    return m_top_margin;
}

/*===========================================================================*/
/**
 *  @brief  Returns bottom margin.
 *  @return bottom margin
 */
/*===========================================================================*/
int ClusterMappedParallelCoordinatesRenderer::bottomMargin() const
{
    return m_bottom_margin;
}

/*===========================================================================*/
/**
 *  @brief  Returns left margin.
 *  @return left margin
 */
/*===========================================================================*/
int ClusterMappedParallelCoordinatesRenderer::leftMargin() const
{
    return m_left_margin;
}

/*===========================================================================*/
/**
 *  @brief  Returns right margin.
 *  @return right margin
 */
/*===========================================================================*/
int ClusterMappedParallelCoordinatesRenderer::rightMargin() const
{
    return m_right_margin;
}

size_t ClusterMappedParallelCoordinatesRenderer::activeAxis() const
{
    return m_active_axis;
}

kvs::UInt8 ClusterMappedParallelCoordinatesRenderer::clusterOpacity() const
{
    return m_cluster_opacity;
}

kvs::Real32 ClusterMappedParallelCoordinatesRenderer::clusterEdgeWidth() const
{
    return m_cluster_edge_width;
}

void ClusterMappedParallelCoordinatesRenderer::exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::IgnoreUnusedVariable( light );

    pcs::ClusterMapObject* cluster_map_object = static_cast<pcs::ClusterMapObject*>( object );

    glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );

    // Anti-aliasing.
    if ( m_enable_anti_aliasing )
    {
#if defined ( GL_MULTISAMPLE )
        if ( m_enable_multisample_anti_aliasing )
        {
            GLint buffers = 0;
            GLint samples = 0;
            glGetIntegerv( GL_SAMPLE_BUFFERS, &buffers );
            glGetIntegerv( GL_SAMPLES, &samples );
            if ( buffers > 0 && samples > 1 ) glEnable( GL_MULTISAMPLE );
        }
        else
#endif
        {
            glEnable( GL_LINE_SMOOTH );
            glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
        }
    }

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    ::BeginDraw();

    const int x0 = m_left_margin;
    const int x1 = camera->windowWidth() - m_right_margin;
    const int y0 = m_top_margin;
    const int y1 = camera->windowHeight() - m_bottom_margin;

    const kvs::Real64 min_value = cluster_map_object->minValue( m_active_axis );
    const kvs::Real64 max_value = cluster_map_object->maxValue( m_active_axis );
    m_color_map.setRange( min_value, max_value );

    const size_t naxes = cluster_map_object->naxes();
    const float stride = float( x1 - x0 ) / ( naxes - 1 );

    GLfloat* vertex = new GLfloat [ naxes * 4 ]; memset( vertex, 0, sizeof(GLfloat) * naxes * 4 );
    pcs::ClusterMapObject::ClusterList::const_iterator cluster = cluster_map_object->clusterList().begin();
    pcs::ClusterMapObject::ClusterList::const_iterator last = cluster_map_object->clusterList().end();
    while ( cluster != last )
    {
        if ( cluster->counter() > 0 )
        {
            const kvs::Real64 active_min_value = cluster->minValue( m_active_axis );
            const kvs::Real64 active_max_value = cluster->maxValue( m_active_axis );
            const kvs::Real64 active_ave_value = ( active_min_value + active_max_value ) * 0.5;
            const kvs::RGBColor color = m_color_map.at( active_ave_value );

            bool draw = true;
            for ( size_t i = 0; i < naxes; i++ )
            {
                const float x = m_left_margin + stride * i;
                const kvs::Real64 axis_min_value = cluster_map_object->minValue(i);
                const kvs::Real64 axis_max_value = cluster_map_object->maxValue(i);
                const kvs::Real64 cluster_min_value = cluster->minValue(i);
                const kvs::Real64 cluster_max_value = cluster->maxValue(i);

                const kvs::Real64 axis_min_range = cluster_map_object->minRange(i);
                const kvs::Real64 axis_max_range = cluster_map_object->maxRange(i);
                if ( axis_max_range < cluster_max_value ||
                     axis_min_range > cluster_min_value )
                {
                    draw = false;
                    break;
                }

                const kvs::Real64 axis_range = axis_max_value - axis_min_value;
                const kvs::Real64 min_value = ( cluster_min_value - axis_min_value ) / axis_range;
                const kvs::Real64 max_value = ( cluster_max_value - axis_min_value ) / axis_range;

                const float ya = y1 - min_value * ( y1 - y0 );
                const float yb = y1 - max_value * ( y1 - y0 );
                vertex[ 4 * i + 0 ] = x;
                vertex[ 4 * i + 1 ] = ya;
                vertex[ 4 * i + 2 ] = x;
                vertex[ 4 * i + 3 ] = yb;
            }

            if ( draw )
            {
                glBegin( GL_QUAD_STRIP );
                glColor4ub( color.r(), color.g(), color.b(), m_cluster_opacity );
                for ( size_t i = 0; i < naxes; i++ )
                {
                    glVertex2fv( vertex + 4 * i );
                    glVertex2fv( vertex + 4 * i + 2 );
                }
                glEnd();
            }

            if ( draw )
            {
                if ( m_cluster_edge_width > 0.0f )
                {
                    const GLubyte r = color.r() * 0.8 + 0.5;
                    const GLubyte g = color.g() * 0.8 + 0.5;
                    const GLubyte b = color.b() * 0.8 + 0.5;
                    const GLubyte a = m_cluster_opacity;
                    glLineWidth( m_cluster_edge_width );
                    for ( size_t i = 0; i < naxes - 1; i++ )
                    {
                        glBegin( GL_LINE_LOOP );
                        glColor4ub( r, g, b, a );
                        glVertex2f( vertex[4*i+0], vertex[4*i+1] );
                        glVertex2f( vertex[4*i+2], vertex[4*i+3] );
                        glVertex2f( vertex[4*i+6], vertex[4*i+7] );
                        glVertex2f( vertex[4*i+4], vertex[4*i+5] );
                        glEnd();
                    }
                }
            }
        }

        cluster++;
    }
    delete [] vertex;

    ::EndDraw();

    glPopAttrib();
}

} // end of namespace pcs

} // end of namespace kvsoceanvis
