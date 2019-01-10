/*****************************************************************************/
/**
 *  @file   View.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: View.cpp 395 2014-10-24 09:00:30Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "View.h"
#include <kvs/qt/Application>
#include <kvs/RGBColor>
#include <kvs/ValueArray>
#include <kvs/ObjectManager>
#include <kvs/RendererManager>
#include <kvs/IDManager>
#include <kvs/Background>
#include <kvs/ObjectBase>
#include <kvs/RendererBase>
#include <kvs/ImageObject>
#include <kvs/ImageRenderer>
#include <kvs/StructuredVolumeObject>
#include <kvs/TableObject>
#include <kvs/RayCastingRenderer>
#include <kvs/Bounds>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/glut/ParallelAxis>
#include <pcs/MultiBinMapping.h>
#include <pcs/MultiBinMapObject.h>
#include <pcs/MultiBinMappedParallelCoordinatesRenderer.h>
using namespace kvsoceanvis;


#include <kvs/MapperBase>
#include <kvs/RendererBase>
#include <kvs/OpenGL>
#include <kvs/Texture2D>

namespace
{

class SliceImage : public kvs::MapperBase, public kvs::ImageObject
{
    kvsModule( SliceImage, Mapper );
    kvsModuleBaseClass( kvs::MapperBase );
    kvsModuleSuperClass( kvs::ImageObject );

public:

    enum AlignedAxis
    {
        XAxis = 0,
        YAxis = 1,
        ZAxis = 2
    };

private:

    size_t m_index; ///< grid index on the axis
    AlignedAxis m_aligned_axis; ///< aligned axis
    float m_ignore_value;
    kvs::RGBColor m_ignore_value_color;

public:

    SliceImage(
        const kvs::ObjectBase* object,
        const size_t index,
        const AlignedAxis aligned_axis,
        const float ignore_value,
        const kvs::RGBColor ignore_value_color,
        const kvs::TransferFunction& transfer_function ):
        kvs::MapperBase( transfer_function ),
        kvs::ImageObject()
    {
        m_index = index;
        m_aligned_axis = aligned_axis;
        m_ignore_value = ignore_value;
        m_ignore_value_color = ignore_value_color;
        this->exec( object );
    }

    SuperClass* exec( const kvs::ObjectBase* object )
    {
        const kvs::VolumeObjectBase* volume_base = kvs::VolumeObjectBase::DownCast( object );
        if ( !volume_base ) { BaseClass::setSuccess( false ); return NULL; }

        const kvs::StructuredVolumeObject* volume = kvs::StructuredVolumeObject::DownCast( volume_base );
        if ( !volume ) { BaseClass::setSuccess( false ); return NULL; }

        switch ( m_aligned_axis )
        {
        case XAxis: return this->slice_x( volume );
        case YAxis: return this->slice_y( volume );
        case ZAxis: return this->slice_z( volume );
        default: return NULL;
        }
    }

    kvs::ImageObject* slice_x( const kvs::StructuredVolumeObject* /* volume */ )
    {
        return NULL;
    }

    kvs::ImageObject* slice_y( const kvs::StructuredVolumeObject* /* volume */ )
    {
        return NULL;
    }

    kvs::ImageObject* slice_z( const kvs::StructuredVolumeObject* volume )
    {
        const size_t min_index = 0;
        const size_t max_index = volume->resolution().z() - 1;
        const size_t index = kvs::Math::Clamp( m_index, min_index, max_index );

        const float min_value = volume->minValue();
        const float max_value = volume->maxValue();
        const kvs::ValueArray<float> values = volume->values().asValueArray<float>();

        const size_t width = volume->resolution().x();
        const size_t height = volume->resolution().y();
        const float* pvalues = values.data() + width * height * index;
        const kvs::ColorMap& cmap = transferFunction().colorMap();

        kvs::ValueArray<kvs::UInt8> pixels( width * height * 3 );
        kvs::UInt8* ppixels = pixels.data();
        for ( size_t j = 0; j < height; j++ )
        {
            for ( size_t i = 0; i < width; i++ )
            {
                const float value = *(pvalues++);
                if ( kvs::Math::Equal( value, m_ignore_value ) ||
                     value < min_value || max_value < value )
                {
                    *(ppixels++) = m_ignore_value_color.r();
                    *(ppixels++) = m_ignore_value_color.g();
                    *(ppixels++) = m_ignore_value_color.b();
                }
                else
                {
                    const float nvalue = ( value - min_value ) / ( max_value - min_value );
                    const size_t cindex = kvs::Math::Round( nvalue * ( cmap.resolution() - 1 ) );
                    const kvs::RGBColor color = cmap[ cindex ];
                    *(ppixels++) = color.r();
                    *(ppixels++) = color.g();
                    *(ppixels++) = color.b();
                }
            }
        }

        const size_t stride = width * 3;
        const size_t end_line = height / 2;
        ppixels = pixels.data();
        for ( size_t i = 0; i < end_line; i++ )
        {
            kvs::UInt8* src = ppixels + ( i * stride );
            kvs::UInt8* dst = ppixels + ( ( height - i - 1 ) * stride );
            for ( size_t j = 0; j < stride; j++ )
            {
                std::swap( *src, *dst );
                src++; dst++;
            }
        }

        SuperClass::setSize( width, height );
        SuperClass::setPixels( pixels, kvs::ImageObject::Color24 );

        return this;
    }
};

class SliceRenderer : public kvs::RendererBase
{
    kvsModule( SliceRenderer, Renderer );
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

    SliceRenderer( const size_t margin = 0 ) : m_margin( margin ) {}
    virtual ~SliceRenderer() { m_texture.release(); }

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

        m_left -= m_margin / width;
        m_right += m_margin / width;
        m_bottom -= m_margin / height;
        m_top += m_margin / height;
    }
};

class SliceAxis : public kvs::RendererBase
{
    kvsModule( SliceAxis, Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

private:

    double m_initial_aspect_ratio; ///< initial aspect ratio
    double m_left; ///< screen left position
    double m_right; ///< screen right position
    double m_bottom; ///< screen bottom position
    double m_top; ///< screen top position
    size_t m_margin;
    kvs::Real32 m_axis_width; ///< axis width
    kvs::RGBColor m_axis_color; ///< axis color

public:

    SliceAxis( const size_t margin = 0 ) :
        m_margin( margin ),
        m_axis_width( 1.0 ),
        m_axis_color( kvs::RGBColor::Black() ) {}

    void setMargin( const size_t margin ) { m_margin = margin; }
    void setAxisWidth( const kvs::Real32 width ) { m_axis_width = width; }
    void setAxisColor( const kvs::RGBColor color ) { m_axis_color = color; }
    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
    {
        kvs::ImageObject* image = kvs::ImageObject::DownCast( object );
        if ( !image ) return;

        this->center_alignment( image, camera->windowWidth(), camera->windowHeight() );

        kvs::OpenGL::WithPushedAttrib p( GL_ALL_ATTRIB_BITS );
        {
            kvs::OpenGL::Disable( GL_DEPTH_TEST );

            kvs::OpenGL::WithPushedMatrix p1( GL_MODELVIEW );
            p1.loadIdentity();
            {
                kvs::OpenGL::WithPushedMatrix p2( GL_PROJECTION );
                p2.loadIdentity();
                {
                    kvs::OpenGL::SetOrtho( m_left, m_right, m_bottom, m_top, -1, 1 );
                    glLineWidth( m_axis_width );
                    glBegin( GL_LINES );
                    glColor4ub( m_axis_color.r(), m_axis_color.g(), m_axis_color.b(), 255 );
                    glColor3ub( 0, 0, 0 );
                    glVertex2f( 0.0, 0.0 ); glVertex2f( 1.0, 0.0 ); // X axis
                    glVertex2f( 0.0, 0.0 ); glVertex2f( 0.0, 1.0 ); // Y axis
                    glEnd();
                }
            }
        }
    }

private:

    void center_alignment( const kvs::ImageObject* image, const double width, const double height )
    {
        const double initial_aspect_ratio = double( image->width() ) / double( image->height() );
        const double current_aspect_ratio = width / height;
        const double aspect_ratio = current_aspect_ratio / initial_aspect_ratio;
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

//        m_left -= m_margin / width;
//        m_right += m_margin / width;
//        m_bottom -= m_margin / height;
//        m_top += m_margin / height;
    }
};

}

/*===========================================================================*/
/**
 *  @brief  Constructs a new View class.
 *  @param  app [in] pointer to the application
 *  @param  color [in] background color
 */
/*===========================================================================*/
View::View( kvs::qt::Application* app, const kvs::RGBColor& color ):
    kvs::qt::Screen( app )
{
    setBackgroundColor( color );
    create();
}

SliceView::SliceView( kvs::qt::Application* app, const kvs::RGBColor& color ):
    View( app, color ),
    m_index( 0 ),
    m_ignore_value( 0 )
{
}

void SliceView::insertObject( kvs::ObjectBase* object )
{
    if ( object->objectType() != kvs::ObjectBase::Volume ) { return; }

    const size_t index = m_index;
    const ::SliceImage::AlignedAxis axis = ::SliceImage::ZAxis;
    const float value = m_ignore_value;
    const kvs::RGBColor color = kvs::RGBColor::White();
    const kvs::TransferFunction tfunc( 256 );
    const size_t margin = 20;

    // Object.
    Object* image = new ::SliceImage( object, index, axis, value, color, tfunc );
    image->setName( "SliceImage" );

    // Renderer.
    Renderer* renderer = new Renderer( margin );
    renderer->setName( "SliceRenderer" );

    // Object for rendering axis.
//    Object* copied_image = new Object();
//    copied_image->shallowCopy( *image );
//    copied_image->setName( "SliceAxis" );

    if ( scene()->hasObject( "SliceImage" ) )
    {
//        scene()->replaceObject( "SliceAxis", copied_image, true );
        scene()->replaceObject( "SliceImage", image, true );
        scene()->replaceRenderer( "SliceRenderer", renderer, true );
    }
    else
    {
//        ::SliceAxis* axis = new ::SliceAxis( margin );
//        axis->setAxisWidth( 3 );
//        axis->setAxisColor( kvs::RGBColor::Black() );

        scene()->registerObject( image, renderer );
//        scene()->registerObject( copied_image, axis );
    }
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new VolumeView class.
 *  @param  app [in] pointer to the application
 *  @param  color [in] background color
 */
/*===========================================================================*/
VolumeView::VolumeView( kvs::qt::Application* app, const kvs::RGBColor& color ):
    View( app, color )
{
}

/*===========================================================================*/
/**
 *  @brief  Inserts volume object.
 *  @param  object [in] pointer to the object
 */
/*===========================================================================*/
void VolumeView::insertObject( kvs::ObjectBase* object )
{
    if ( object->objectType() != kvs::ObjectBase::Volume ) { return; }

    // Object.
    Object* volume = kvs::StructuredVolumeObject::DownCast( object );
    volume->setName( "VolumeObject" );

    // Renderer.
    Renderer* renderer = new Renderer();
    renderer->setName( "VolumeRenderer" );

    // Object for rendering bounding box.
    Object* copied_volume = new Object();
    copied_volume->shallowCopy( *volume );
    copied_volume->setName( "BoundsObject" );

    kvs::Bounds* bounds = new kvs::Bounds();
    bounds->setName( "BoundsRenderer" );
    bounds->setLineWidth( 1.2f );
    bounds->enableAntiAliasing();

    if ( scene()->hasObject("VolumeObject") )
    {
        scene()->replaceObject( "VolumeObject", volume, true );
        scene()->replaceRenderer( "VolumeRenderer", renderer, true );

        scene()->replaceObject( "BoundsObject", copied_volume, true );
        scene()->replaceRenderer( "BoundsRenderer", bounds, true );
    }
    else
    {
        scene()->registerObject( copied_volume, bounds );
        scene()->registerObject( volume, renderer );
    }
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new ParallelCoordinatesView class.
 *  @param  app [in] pointer to the application
 *  @param  color [in] background color
 */
/*===========================================================================*/
ParallelView::ParallelView( kvs::qt::Application* app, const kvs::RGBColor& color ):
    View( app, color ),
    m_active_axis( 0 ),
    m_color_map( 256 )
{
    m_color_map.create();
}

/*===========================================================================*/
/**
 *  @brief  Inserts table object.
 *  @param  object [in] pointer to the object
 */
/*===========================================================================*/
void ParallelView::insertObject( kvs::ObjectBase* object )
{
    if ( object->objectType() != kvs::ObjectBase::Table ) { return; }

    // Object which is generated with multi-dimensional binning.
    size_t ncolumns = kvs::TableObject::DownCast( object )->numberOfColumns();
    kvs::ValueArray<kvs::UInt32> nbins( ncolumns ); nbins.fill( 10 );
    Object* table = new pcs::MultiBinMapping( object, nbins );
    table->setName( "Table" );
//    delete object;

    // Renderer.
    const int top_margin = 50;
    const int bottom_margin = 40;
    const int left_margin = 40;
    const int right_margin = 40;
    Renderer* renderer = new Renderer();
    renderer->setName( "PCs" );
    renderer->setTopMargin( top_margin );
    renderer->setBottomMargin( bottom_margin );
    renderer->setLeftMargin( left_margin );
    renderer->setRightMargin( right_margin );
    renderer->setBinEdgeWidth( 1.5f );
    renderer->setBinOpacity( 100 );
    renderer->enableAntiAliasing();
    renderer->selectAxis( m_active_axis );
    renderer->setColorMap( m_color_map );

    // Object for rendering axis.
    Object* copied_table = new Object();
    copied_table->shallowCopy( *table );
    copied_table->setName( "AxisObject" );

    kvs::glut::ParallelAxis* axis = new kvs::glut::ParallelAxis();
    axis->setName("AxisRenderer");
    axis->setTopMargin( top_margin );
    axis->setBottomMargin( bottom_margin );
    axis->setLeftMargin( left_margin );
    axis->setRightMargin( right_margin );
    axis->setAxisWidth( 4.0f );
    axis->setAxisColor( kvs::RGBColor( 0, 0, 0 ) );
    axis->setValueColor( kvs::RGBColor( 0, 0, 0 ) );
    axis->setLabelColor( kvs::RGBColor( 200, 100, 100 ) );

    if ( scene()->hasObject( "Table" ) )
    {
        scene()->replaceObject( "Table", table, true );
        scene()->replaceRenderer( "PCs", renderer, true );

        scene()->replaceObject( "AxisObject", copied_table, true );
        scene()->replaceRenderer( "AxisRenderer", axis, true );
    }
    else
    {
        scene()->registerObject( table, renderer );
        scene()->registerObject( copied_table, axis );
    }
}
