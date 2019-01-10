/*****************************************************************************/
/**
 *  @file   Widget.h
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
#ifndef KVSOCEANVIS__UTIL__WIDGET_H_INCLUDE
#define KVSOCEANVIS__UTIL__WIDGET_H_INCLUDE

#include <kvs/glut/CheckBox>
#include <kvs/glut/CheckBoxGroup>
#include <kvs/glut/RadioButton>
#include <kvs/glut/RadioButtonGroup>
#include <kvs/ScreenBase>

#if KVS_SUPPORT_GLUT
#include <kvs/glut/Screen>
#endif

#if KVS_SUPPORT_QT && defined( QT_VERSION )
#include <kvs/qt/Screen>
#endif


namespace kvsoceanvis
{

namespace util
{

namespace Widget
{

template <typename Object>
Object* GetObject( kvs::ScreenBase* screen_base, const std::string& object_name )
{
    kvs::ObjectManager* manager = NULL;

#if KVS_SUPPORT_GLUT
    kvs::glut::Screen* glut_screen = dynamic_cast<kvs::glut::Screen*>( screen_base );
    if ( glut_screen ) { manager = glut_screen->scene()->objectManager(); }
#endif

#if KVS_SUPPORT_QT && defined( QT_VERSION )
    if ( !glut_screen )
    {
        kvs::qt::Screen* qt_screen = dynamic_cast<kvs::qt::screen*>( screen_base );
        if ( qt_screen ) { manager = qt_screen->scene()->objectManager(); }
    }
#endif

    if ( !manager ) return NULL;
    else return static_cast<Object*>( manager->object( object_name ) );
}

template <typename Renderer>
Renderer* GetRenderer( kvs::ScreenBase* screen_base, const std::string& renderer_name )
{
    kvs::RendererManager* manager = NULL;

#if KVS_SUPPORT_GLUT
    kvs::glut::Screen* glut_screen = dynamic_cast<kvs::glut::Screen*>( screen_base );
    if ( glut_screen ) { manager = glut_screen->scene()->rendererManager(); }
#endif

#if KVS_SUPPORT_QT && defined( QT_VERSION )
    if ( !glut_screen )
    {
        kvs::qt::Screen* qt_screen = dynamic_cast<kvs::qt::screen*>( screen_base );
        if ( qt_screen ) { manager = qt_screen->scene()->rendererManager(); }
    }
#endif

    if ( !manager ) return NULL;
    else return static_cast<Renderer*>( manager->renderer( renderer_name ) );
}

template <typename Object, typename Renderer>
class AxisSelectButton : public kvs::glut::RadioButton
{
private:

    std::string m_object_name;
    std::string m_renderer_name;
    size_t m_id;
    size_t m_left_margin;
    size_t m_right_margin;

public:

    AxisSelectButton(
        kvs::ScreenBase* screen,
        std::string object_name,
        std::string renderer_name ):
        kvs::glut::RadioButton( screen )
    {
        m_object_name = object_name;
        m_renderer_name = renderer_name;
        m_id = 0;
        m_left_margin = 0;
        m_right_margin = 0;
        setCaption( "" );
    }

    void setID( const size_t id )
    {
        m_id = id;
    }

    void setLeftMargin( const size_t left_margin )
    {
        m_left_margin = left_margin;
    }

    void setRightMargin( const size_t right_margin )
    {
        m_right_margin = right_margin;
    }

    void stateChanged( void )
    {
        if ( state() )
        {
            Renderer* renderer = kvsoceanvis::util::Widget::GetRenderer<Renderer>( screen(), m_renderer_name );
            if ( renderer ) renderer->selectAxis( m_id );
        }
    }

    void screenResized( void )
    {
        Object* object = kvsoceanvis::util::Widget::GetObject<Object>( screen(), m_object_name );
        if ( object )
        {
            const size_t naxes = object->numberOfColumns();
            const size_t stride = ( screen()->width() - m_left_margin - m_right_margin ) / ( naxes - 1 );
            setX( m_left_margin + stride * m_id - 3 );
            setY( 10 );
        }
    }
};

template <typename Object, typename Renderer>
class AxisSelector : public kvs::glut::RadioButtonGroup
{
private:

    kvs::ScreenBase* m_screen;
    std::string m_object_name;
    std::string m_renderer_name;
    size_t m_left_margin;
    size_t m_right_margin;

public:

    AxisSelector(
        kvs::ScreenBase* screen,
        std::string object_name,
        std::string renderer_name ):
        kvs::glut::RadioButtonGroup( screen )
    {
        m_screen = screen;
        m_object_name = object_name;
        m_renderer_name = renderer_name;
        m_left_margin = 0;
        m_right_margin = 0;
    }

    void setLeftMargin( const size_t left_margin )
    {
        m_left_margin = left_margin;
    }

    void setRightMargin( const size_t right_margin )
    {
        m_right_margin = right_margin;
    }

    void create( void )
    {
        clear();

        Object* object = kvsoceanvis::util::Widget::GetObject<Object>( screen(), m_object_name );
        if ( !object ) return;

        const size_t naxes = object->numberOfColumns();
        for ( size_t id = 0; id < naxes; id++ )
        {
            typedef AxisSelectButton<Object,Renderer> Button;
            Button* button = new Button( m_screen, m_object_name, m_renderer_name );
            button->setID( id );
            button->setLeftMargin( m_left_margin );
            button->setRightMargin( m_right_margin );
            button->screenResized();
            add( button );
        }
    }

    void clear( void )
    {
        std::list<kvs::glut::RadioButton*>::iterator button = m_buttons.begin();
        std::list<kvs::glut::RadioButton*>::const_iterator last = m_buttons.end();
        while ( button != last )
        {
            if ( *button ) delete *button;
            ++button;
        }

        m_buttons.clear();
    }

    void select( const size_t index )
    {
        size_t counter = 0;

        std::list<kvs::glut::RadioButton*>::const_iterator button = radioButtons().begin();
        std::list<kvs::glut::RadioButton*>::const_iterator last = radioButtons().end();
        while ( button != last )
        {
            if ( counter == index )
            {
                (*button)->setState( true );
            }

            counter++;
            button++;
        }
    }
};

} // end of namespace Widget

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__WIDGET_H_INCLUDE
