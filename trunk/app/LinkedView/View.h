/*****************************************************************************/
/**
 *  @file   View.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: View.h 395 2014-10-24 09:00:30Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#ifndef VIEW_H_INCLUDE
#define VIEW_H_INCLUDE

#include <kvs/qt/Screen>
#include <kvs/qt/Application>
#include <kvs/RendererBase>
#include <kvs/ObjectBase>
#include <kvs/ImageObject>
#include <kvs/OpenGL>
#include <kvs/Texture2D>
#include <kvs/RGBColor>
#include <kvs/Camera>
#include <pcs/MultiBinMapObject.h>
#include <pcs/MultiBinMappedParallelCoordinatesRenderer.h>


/*===========================================================================*/
/**
 *  @brief  View class.
 */
/*===========================================================================*/
class View : public kvs::qt::Screen
{
public:

    View( kvs::qt::Application* app, const kvs::RGBColor& color );
    virtual void insertObject( kvs::ObjectBase* object ){};
};

class SliceView : public View
{
public:

    typedef kvs::ImageObject Object;

    class Renderer : public kvs::RendererBase
    {
        kvsModule( Renderer, Renderer );
        kvsModuleBaseClass( kvs::RendererBase );

    private:

        double m_initial_aspect_ratio; ///< initial aspect ratio
        double m_left; ///< screen left position
        double m_right; ///< screen right position
        double m_bottom; ///< screen bottom position
        double m_top; ///< screen top position
        size_t m_margin; ///< margin in pixel
        kvs::Texture2D m_texture; ///< texture image

    public:

        Renderer( const size_t margin = 0 ) : m_margin( margin ) {}
        virtual ~Renderer() { m_texture.release(); }

        void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* )
        {
            kvs::ImageObject* image = kvs::ImageObject::DownCast( object );
            if ( !image ) return;

            if ( !m_texture.isCreated() ) { this->create_texture( image ); }
#if 1
            else
            {
                m_texture.release();
                this->create_texture( image );
            }
#endif

            this->center_alignment( camera->windowWidth(), camera->windowHeight() );

            KVS_GL_CALL( glClearDepth( 1.0 ) );
            kvs::OpenGL::Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            kvs::OpenGL::WithPushedAttrib p( GL_ALL_ATTRIB_BITS );
            {
                kvs::OpenGL::Disable( GL_DEPTH_TEST );
                kvs::OpenGL::Disable( GL_TEXTURE_1D );
                kvs::OpenGL::Disable( GL_TEXTURE_3D );
                kvs::OpenGL::Enable( GL_TEXTURE_2D );

                kvs::Texture::GuardedBinder unit( m_texture );
                kvs::OpenGL::WithPushedMatrix p1( GL_MODELVIEW );
                p1.loadIdentity();
                {
                    kvs::OpenGL::WithPushedMatrix p2( GL_PROJECTION );
                    p2.loadIdentity();
                    {
                        kvs::OpenGL::SetOrtho( m_left, m_right, m_bottom, m_top, -1, 1 );
                        glBegin( GL_QUADS );
                        glTexCoord2f( 0.0, 0.0 ); glVertex2f(  0.0,  1.0 );
                        glTexCoord2f( 0.0, 1.0 ); glVertex2f(  0.0,  0.0 );
                        glTexCoord2f( 1.0, 1.0 ); glVertex2f(  1.0,  0.0 );
                        glTexCoord2f( 1.0, 0.0 ); glVertex2f(  1.0,  1.0 );
                        glEnd();
                    }
                }
            }
        }

    private:

        void create_texture( const kvs::ImageObject* image )
        {
            const double width  = image->width();
            const double height = image->height();
            m_initial_aspect_ratio = width / height;
            m_left = 0.0;
            m_right = 1.0;
            m_bottom = 0.0;
            m_top = 1.0;

            if ( image->pixelType() == kvs::ImageObject::Gray8 )
            {
                const size_t nchannels = 1;
                const size_t bytes_per_channel = 1;
                m_texture.setPixelFormat( nchannels, bytes_per_channel );
            }
            else if ( image->pixelType() == kvs::ImageObject::Gray16 )
            {
                const size_t nchannels = 1;
                const size_t bytes_per_channel = 2;
                m_texture.setPixelFormat( nchannels, bytes_per_channel );
            }
            else if ( image->pixelType() == kvs::ImageObject::Color24 )
            {
                const size_t nchannels = 3;
                const size_t bytes_per_channel = 1;
                m_texture.setPixelFormat( nchannels, bytes_per_channel );
            }
            else if ( image->pixelType() == kvs::ImageObject::Color32 )
            {
                const size_t nchannels = 4;
                const size_t bytes_per_channel = 1;
                m_texture.setPixelFormat( nchannels, bytes_per_channel );
            }
            else
            {
                kvsMessageError("Unknown pixel color type.");
            }

            kvs::Texture::SetEnv( GL_TEXTURE_ENV_MODE, GL_REPLACE );
            m_texture.create( image->width(), image->height(), image->pixels().data() );
        }

        void center_alignment( const double width, const double height )
        {
            const double current_aspect_ratio = width / height;
            const double aspect_ratio = current_aspect_ratio / m_initial_aspect_ratio;
            if( aspect_ratio >= 1.0 )
            {
                m_left = ( 1.0 - aspect_ratio ) * 0.5;
                m_right = ( 1.0 + aspect_ratio ) * 0.5;
                m_bottom = 0.0;
                m_top = 1.0;
            }
            else
            {
                m_left = 0.0;
                m_right = 1.0;
                m_bottom = ( 1.0 - 1.0 / aspect_ratio ) * 0.5;
                m_top = ( 1.0 + 1.0 / aspect_ratio ) * 0.5;
            }

/*
            m_left -= m_margin / ( width - 1 );
            m_right += m_margin / ( width - 1 );
            m_bottom -= m_margin / ( height - 1 );
            m_top += m_margin / ( height - 1 );
*/
        }
    };

private:
    size_t m_index;
    float m_ignore_value;

public:

    SliceView( kvs::qt::Application* app, const kvs::RGBColor& color );
    void setIndex( const size_t index ) { m_index = index; }
    void setIgnoreValue( const float value ) { m_ignore_value = value; }
    void insertObject( kvs::ObjectBase* object );
};

#include <kvs/RayCastingRenderer>
#include <kvs/StructuredVolumeObject>

class VolumeView : public View
{
public:
    typedef kvs::StructuredVolumeObject Object;
    typedef kvs::glsl::RayCastingRenderer Renderer;

public:

    VolumeView( kvs::qt::Application* app, const kvs::RGBColor& color );
    void insertObject( kvs::ObjectBase* object );
};

class ParallelView : public View
{
public:
    typedef kvsoceanvis::pcs::MultiBinMapObject Object;
    typedef kvsoceanvis::pcs::MultiBinMappedParallelCoordinatesRenderer Renderer;

private:
    size_t m_active_axis;
    kvs::ColorMap m_color_map;

public:

    ParallelView( kvs::qt::Application* app, const kvs::RGBColor& color );
    size_t activeAxis() const { return m_active_axis; }
    kvs::ColorMap colorMap() const { return m_color_map; }
    void setActiveAxis( const size_t axis ) { m_active_axis = axis; }
    void setColorMap( const kvs::ColorMap& color_map ) { m_color_map = color_map; }
    void insertObject( kvs::ObjectBase* object );
};

#endif // VIEW_H_INCLUDE
