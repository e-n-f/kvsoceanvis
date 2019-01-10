/*****************************************************************************/
/**
 *  @file   Interactor.cpp
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
#include "Interactor.h"
#include <kvs/OpenGL>
#include <kvs/ColorImage>
#include <kvs/Camera>


ParallelInteractor::ParallelInteractor()
{
    setEventType( kvs::EventBase::AllEvents );
}

void ParallelInteractor::KeyPressEvent( kvs::KeyEvent* event )
{
    Object* object = static_cast<Object*>( scene()->object( m_object_name ) );
    Renderer* renderer = static_cast<Renderer*>( scene()->renderer( m_renderer_name ) );
    if ( !object || !renderer ) return;

    switch ( event->key() )
    {
    case kvs::Key::Right:
    {
        int axis = renderer->activeAxis() + 1;
        if ( axis > int( object->numberOfColumns() - 1 ) ) axis = int( object->numberOfColumns() - 1 );
        renderer->selectAxis( axis );
        break;
    }
    case kvs::Key::Left:
    {
        int axis = renderer->activeAxis() - 1;
        if ( axis < 0 ) axis = 0;
        renderer->selectAxis( axis );
        break;
    }
    case kvs::Key::s:
    {
        kvs::ColorImage image = scene()->camera()->snapshot();
        image.write("snapshot.bmp");
        break;
    }
    default: break;
    }
}

void ParallelInteractor::mousePressEvent( kvs::MouseEvent* event )
{
    Object* object = static_cast<Object*>( scene()->object( m_object_name ) );
    Renderer* renderer = static_cast<Renderer*>( scene()->renderer( m_renderer_name ) );
    if ( !object || !renderer ) return;

    m_parameter.X0 = event->x();
    m_parameter.Y0 = screen()->height() - event->y();
    m_parameter.pX = m_parameter.X0;
    m_parameter.pY = m_parameter.Y0;

    const int X_min = renderer->leftMargin();
    const int X_max = screen()->width() - renderer->rightMargin();

    const size_t naxes = object->numberOfColumns();
    const float stride = float( X_max - X_min ) / ( naxes - 1 );
    for ( size_t i = 0; i < naxes; i++ )
    {
        const float ex = static_cast<float>( event->x() );
        const float x = renderer->leftMargin() + stride * i;
        if ( x - 5 < ex && ex < x + 5 )
        {
            m_parameter.SelectedAxis = i;
            m_parameter.MovingRange = true;
        }
        else if ( x - 10 < ex && ex < x + 10 )
        {
            m_parameter.SelectedAxis = i;
            m_parameter.SettingRange = true;
        }
    }
}

void ParallelInteractor::mouseDoubleClickEvent( kvs::MouseEvent* event )
{
    Object* object = static_cast<Object*>( scene()->object( m_object_name ) );
    Renderer* renderer = static_cast<Renderer*>( scene()->renderer( m_renderer_name ) );
    if ( !object || !renderer ) return;

    const int X_min = renderer->leftMargin();
    const int X_max = screen()->width() - renderer->rightMargin();

    const size_t naxes = object->numberOfColumns();
    const float stride = float( X_max - X_min ) / ( naxes - 1 );
    for ( size_t i = 0; i < naxes; i++ )
    {
        const float ex = static_cast<float>( event->x() );
        const float x = renderer->leftMargin() + stride * i;
        if ( x - 5 < ex && ex < x + 5 ) { object->resetRange( i ); return; }
    }

    object->resetRange();

    m_parameter.SettingRange = false;
    m_parameter.MovingRange = false;

    screen()->redraw();
}

void ParallelInteractor::mouseReleaseEvent( kvs::MouseEvent* event )
{
    m_parameter.SettingRange = false;
    m_parameter.MovingRange = false;
}

void ParallelInteractor::mouseMoveEvent( kvs::MouseEvent* event )
{
    Object* object = static_cast<Object*>( scene()->object( m_object_name ) );
    Renderer* renderer = static_cast<Renderer*>( scene()->renderer( m_renderer_name ) );
    if ( !object || !renderer ) return;

    m_parameter.X1 = event->x();
    m_parameter.Y1 = screen()->height() - event->y();

    const int Y_min = renderer->bottomMargin();
    const int Y_max = screen()->height() - renderer->topMargin();

    if ( m_parameter.Y1 <= Y_min ) m_parameter.Y1 = Y_min;
    if ( m_parameter.Y1 >= Y_max ) m_parameter.Y1 = Y_max;

    const int axis = m_parameter.SelectedAxis;
    const kvs::Real64 y_min = object->minValue( axis );
    const kvs::Real64 y_max = object->maxValue( axis );

    if ( m_parameter.SettingRange )
    {
        const int Y_rect_min = kvs::Math::Min( m_parameter.Y0, m_parameter.Y1 );
        const int Y_rect_max = kvs::Math::Max( m_parameter.Y0, m_parameter.Y1 );

        const kvs::Real64 y_rect_min = y_min + ( y_max - y_min ) * ( Y_rect_min - Y_min ) / ( Y_max - Y_min );
        const kvs::Real64 y_rect_max = y_min + ( y_max - y_min ) * ( Y_rect_max - Y_min ) / ( Y_max - Y_min );

        object->setMinRange( axis, y_rect_min );
        object->setMaxRange( axis, y_rect_max );
    }

    if ( m_parameter.MovingRange )
    {
        const int Y_delta = m_parameter.Y1 - m_parameter.pY;
        const kvs::Real64 y_delta = ( y_max - y_min ) * Y_delta / ( Y_max - Y_min );

        object->moveRange( axis, y_delta );

        m_parameter.pX = m_parameter.X1;
        m_parameter.pY = m_parameter.Y1;
    }
}

void ParallelInteractor::paintEvent()
{
    Object* object = static_cast<Object*>( scene()->object( m_object_name ) );
    Renderer* renderer = static_cast<Renderer*>( scene()->renderer( m_renderer_name ) );
    if ( !object || !renderer ) return;

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

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    const int X_min = renderer->leftMargin();
    const int X_max = screen()->width() - renderer->rightMargin();
    const int Y_min = renderer->topMargin();
    const int Y_max = screen()->height() - renderer->bottomMargin();

    const int naxes = object->numberOfColumns();
    const float stride = float( X_max - X_min ) / ( naxes - 1 );
    for ( int i = 0; i < naxes; i++ )
    {
        const float X = renderer->leftMargin() + stride * i;

        const kvs::Real64 y_min_range = object->minRange(i);
        const kvs::Real64 y_max_range = object->maxRange(i);
        const kvs::Real64 y_min_value = object->minValue(i);
        const kvs::Real64 y_max_value = object->maxValue(i);

        const float Y_min_range = Y_max - ( Y_max - Y_min ) * ( y_max_range - y_min_value ) / ( y_max_value - y_min_value );
        const float Y_max_range = Y_max - ( Y_max - Y_min ) * ( y_min_range - y_min_value ) / ( y_max_value - y_min_value );

        glBegin( GL_QUADS );
        glColor4ub( 128, 128, 128, 80 );
        glVertex2f( X - 10, Y_min_range );
        glVertex2f( X + 10, Y_min_range );
        glVertex2f( X + 10, Y_max_range );
        glVertex2f( X - 10, Y_max_range );
        glEnd();

        glLineWidth( 2.0f );
        glBegin( GL_LINE_LOOP );
        glColor3ub( 128, 128, 128 );
        glVertex2f( X - 10, Y_min_range );
        glVertex2f( X + 10, Y_min_range );
        glVertex2f( X + 10, Y_max_range );
        glVertex2f( X - 10, Y_max_range );
        glEnd();
    }

    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glPopAttrib();
}


namespace
{

struct Rectangle
{
    double xmin;
    double xmax;
    double ymin;
    double ymax;

    Rectangle( const SliceInteractor::Object* object, const kvs::Camera* camera )
    {
        const double width = camera->windowWidth();
        const double height = camera->windowHeight();
        const double current_aspect_ratio = width / height;
        const double initial_aspect_ratio = object->width() / object->height();
        const double aspect_ratio = current_aspect_ratio / initial_aspect_ratio;
        if ( aspect_ratio >= 1.0 )
        {
            xmin = 0;
            xmax = width - 1;
            double factor = width / object->width();
            ymin = ( height - object->height() * factor ) * 0.5;
            ymax = ymin + object->height() * factor;
        }
        else
        {
            ymin = 0;
            ymax = height - 1;
            double factor = height / object->height();
            xmin = ( width - object->width() * factor ) * 0.5;
            xmax = xmin + object->width() * factor;
        }
    }

    kvs::Vec4ui gridIndices(
        SliceInteractor::Parameter& parameter,
        const SliceInteractor::Object* object,
        const kvs::Camera* camera )
    {
        const double width = camera->windowWidth();
        const double height = camera->windowHeight();
        const double current_aspect_ratio = width / height;
        const double initial_aspect_ratio = object->width() / object->height();
        const double aspect_ratio = current_aspect_ratio / initial_aspect_ratio;

        size_t xmin_index;
        size_t xmax_index;
        size_t ymin_index;
        size_t ymax_index;
        if ( aspect_ratio >= 1.0 )
        {
            xmin_index = ( parameter.X0 / width ) * object->width();
            xmax_index = ( parameter.X1 / width ) * object->width();
            ymin_index = ( ( ( height - parameter.Y0 ) - ymin ) / ( ymax - ymin ) ) * object->height();
            ymax_index = ( ( ( height - parameter.Y1 ) - ymin ) / ( ymax - ymin ) ) * object->height();
        }
        else
        {
            xmin_index = ( ( parameter.X0 - xmin ) / ( xmax - xmin ) ) * object->width();
            xmax_index = ( ( parameter.X1 - xmin ) / ( xmax - xmin ) ) * object->width();
            ymin_index = ( ( height - parameter.Y0 ) / height ) * object->height();
            ymax_index = ( ( height - parameter.Y1 ) / height ) * object->height();
        }

        return kvs::Vec4ui(
            kvs::Math::Min( xmin_index, xmax_index ),
            kvs::Math::Max( xmin_index, xmax_index ),
            kvs::Math::Min( ymin_index, ymax_index ),
            kvs::Math::Max( ymin_index, ymax_index ) );
    }
};

kvs::ColorMap GetColorMap( kvs::TableObject* table, kvs::TableObject* cropped_table, size_t active_axis )
{
    kvs::Real32 min0 = table->minValue( active_axis );
    kvs::Real32 max0 = table->maxValue( active_axis );
    kvs::Real32 min1 = cropped_table->minValue( active_axis );
    kvs::Real32 max1 = cropped_table->maxValue( active_axis );

//            kvs::ColorMap cmap0 = m_parallel_view->colorMap();
    kvs::ColorMap cmap0( 256 );
    cmap0.setRange( min0, max0 );
    cmap0.create();

    kvs::ValueArray<kvs::UInt8> cmap1( 256 * 3 );
    kvs::Real32 stride = ( max1 - min1 ) / 256.0f;
    kvs::Real32 value = min1;
    for ( size_t i = 0; i < 256; i++, value += stride )
    {
        kvs::RGBColor c = cmap0.at( value );
        cmap1[ 3 * i + 0 ] = c.r();
        cmap1[ 3 * i + 1 ] = c.g();
        cmap1[ 3 * i + 2 ] = c.b();
    }

    return kvs::ColorMap( cmap1 );
}

} // end of namespace


SliceInteractor::SliceInteractor( Model* model, ParallelView* parallel_view, VolumeView* volume_view ):
    m_model( model ),
    m_parallel_view( parallel_view ),
    m_volume_view( volume_view )
{
    setEventType( kvs::EventBase::AllEvents );
}

void SliceInteractor::mousePressEvent( kvs::MouseEvent* event )
{
    m_parameter.Rectangle = true;

    m_parameter.X0 = event->x();
    m_parameter.Y0 = event->y();
    m_parameter.pX = m_parameter.X0;
    m_parameter.pY = m_parameter.Y0;

    m_parameter.MovingRange = false;
    m_parameter.Rectangle = false;

    if ( !scene()->hasObject( m_object_name ) ) return;
    Object* object = static_cast<Object*>( scene()->object( m_object_name ) );

    ::Rectangle rect( object, scene()->camera() );
    double X_min = rect.xmin;
    double X_max = rect.xmax;
    double Y_min = rect.ymin;
    double Y_max = rect.ymax;
    if ( X_min <= m_parameter.X0 && m_parameter.X0 <= X_max &&
         Y_min <= m_parameter.Y0 && m_parameter.Y0 <= Y_max )
    {
        m_parameter.Rectangle = true;
    }
}

void SliceInteractor::mouseDoubleClickEvent( kvs::MouseEvent* event )
{
/*
  Object* object = static_cast<Object*>( scene()->object( m_object_name ) );
  object->resetRange();
*/
    m_parameter.MovingRange = false;
    m_parameter.Rectangle = false;
}

void SliceInteractor::mouseReleaseEvent( kvs::MouseEvent* event )
{
    m_parameter.MovingRange = false;

    if ( m_parameter.X0 == event->x() ||
         m_parameter.Y0 == event->y() )
    {
        m_parameter.Rectangle = false;
    }
    else
    {
        m_parameter.Rectangle = false;

        if ( !scene()->hasObject( m_object_name ) ) return;
        Object* object = static_cast<Object*>( scene()->object( m_object_name ) );
        ::Rectangle rect( object, scene()->camera() );
        double X_min = rect.xmin;
        double X_max = rect.xmax;
        double Y_min = rect.ymin;
        double Y_max = rect.ymax;
        if ( X_min <= m_parameter.X0 && m_parameter.X0 <= X_max &&
             Y_min <= m_parameter.Y0 && m_parameter.Y0 <= Y_max )
        {
            m_parameter.Rectangle = true;

            Object* object = static_cast<Object*>( scene()->object( m_object_name ) );
            if ( !object ) return;

            ::Rectangle rect( object, scene()->camera() );
            kvs::Vec4ui indices = rect.gridIndices( m_parameter, object, scene()->camera() );

            float xmin_index = indices[0];
            float xmax_index = indices[1];
            float ymin_index = indices[2];
            float ymax_index = indices[3];
            float zmin_index = 0;
            float zmax_index = m_model->volumeObject()->resolution().z() - 1;

            kvs::Vec3 min_range( xmin_index, ymin_index, zmin_index );
            kvs::Vec3 max_range( xmax_index, ymax_index, zmax_index );

            kvs::StructuredVolumeObject* volume = m_model->croppedVolumeObject( min_range, max_range );
            volume->setMinMaxValues( m_model->volumeObject()->minValue(), m_model->volumeObject()->maxValue() );
            m_volume_view->insertObject( volume );
            m_volume_view->scene()->reset();
            m_volume_view->redraw();

            size_t active_axis = m_parallel_view->activeAxis();
            kvs::TableObject* table = m_model->croppedTableObject( min_range, max_range );
            m_parallel_view->setColorMap( ::GetColorMap( m_model->tableObject(), table, active_axis ) );
            m_parallel_view->insertObject( table );
            m_parallel_view->redraw();
        }
    }
}

void SliceInteractor::mouseMoveEvent( kvs::MouseEvent* event )
{
    m_parameter.X1 = event->x();
    m_parameter.Y1 = event->y();

    if ( !scene()->hasObject( m_object_name ) ) return;
    Object* object = static_cast<Object*>( scene()->object( m_object_name ) );

    ::Rectangle rect( object, scene()->camera() );
    double X_min = rect.xmin;
    double X_max = rect.xmax;
    double Y_min = rect.ymin;
    double Y_max = rect.ymax;
    if ( m_parameter.X1 < X_min ) m_parameter.X1 = X_min;
    if ( m_parameter.X1 > X_max ) m_parameter.X1 = X_max;
    if ( m_parameter.Y1 < Y_min ) m_parameter.Y1 = Y_min;
    if ( m_parameter.Y1 > Y_max ) m_parameter.Y1 = Y_max;
}

void SliceInteractor::paintEvent()
{
    if ( !m_parameter.Rectangle ) return;

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

    const int X_min_range = m_parameter.X0;
    const int X_max_range = m_parameter.X1;
    const int Y_min_range = m_parameter.Y0;
    const int Y_max_range = m_parameter.Y1;

    glLineWidth( 2 );
    glBegin( GL_LINE_LOOP );
    glColor3ub( 128, 128, 128 );
    glVertex2i( X_min_range, Y_min_range );
    glVertex2i( X_max_range, Y_min_range );
    glVertex2i( X_max_range, Y_max_range );
    glVertex2i( X_min_range, Y_max_range );
    glEnd();

    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glPopAttrib();
}
