#include <iostream>
#include <kvs/AnyValueArray>
#include <kvs/TableObject>
#include <kvs/TableImporter>
#include <kvs/AxisObject>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/glut/ParallelAxisRenderer>
#include <kvs/Xorshift128>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <lua5.1/lua.hpp>
#include <util/TableObjectWriter.h>
#include <util/Event.h>

#include "luautils.h"

template <typename Object, typename Renderer>
class UpdatePCP : public kvs::KeyPressEventListener
{
private:

    kvs::glut::Screen* m_screen;
    std::string m_object_name;
    std::string m_renderer_name;
    std::string m_pos;
    float m_factor;

public:

    UpdatePCP( kvs::glut::Screen* screen, std::string object_name, std::string renderer_name )
    {
        m_screen = screen;
        m_object_name = object_name;
        m_renderer_name = renderer_name;
    }

    void setFactor(float factor)
    {
        m_factor = factor;
    }

    void setPos(std::string pos)
    {
        m_pos = pos;
    }

    void update( kvs::KeyEvent* event )
    {
        switch ( event->key() )
        {
            case kvs::Key::c:
                {
                    if (m_pos == "x")
                    {
                        m_pos = "y";
                        m_factor = 0;
                        std::cout << "Present Position of Factor: " << m_pos << std::endl;
                    }
                    else if (m_pos == "y")
                    {
                        m_pos = "x";
                        m_factor = 0;
                        std::cout << "Present Position of Factor: " << m_pos << std::endl;
                    }
                    break;
                }
            case kvs::Key::i:
                {
                    std::cout << "Present Position of Factor: " << m_pos << std::endl;
                    std::cout << "Present Value of Factor: " << m_factor << std::endl;
                    break;
                }
            case kvs::Key::u:
                {
                    kvs::TableObject* object = static_cast<kvs::TableObject*>( screen()->objectManager()->object( m_object_name ) );
                    // kvs::ParallelCoordinatesRenderer* renderer = static_cast<kvs::ParallelCoordinatesRenderer*>( screen()->rendererManager()->renderer( m_renderer_name ) );
                    m_factor = m_factor + 0.1;
                    if (m_factor > 1)
                    {
                        m_factor = 1;
                    }
                    size_t nr = object->nrows();
                    kvs::ValueArray<double> alpha(nr);
                    alpha = object->column(3).asValueArray<double>();
                    if (m_pos == "x")
                    {
                        for(size_t i=0; i<nr; i++)
                        {
                            alpha[i] = object->column(0).to<double>(i) * m_factor + object->column(1).to<double>(i);
                        }
                    }
                    else if (m_pos == "y")
                    {
                        for(size_t i=0; i<nr; i++)
                        {
                            alpha[i] = object->column(0).to<double>(i) + object->column(1).to<double>(i) * m_factor;
                        }
                    }
                    double min = alpha[0];
                    double max = alpha[0];
                    for (size_t i=0; i<nr; i++)
                    {
                        if (alpha[i] < min)
                        {
                            min = alpha[i];
                        }
                        if (alpha[i] > max)
                        {
                            max = alpha[i];
                        }
                    }
                    object->setMinValue( 3, min );
                    object->setMaxValue( 3, max );
                    object->setMinRange( 3, min );
                    object->setMaxRange( 3, max );
                    m_screen->redraw();
                    break;
                }
            case kvs::Key::j:
                {
                    kvs::TableObject* object = static_cast<kvs::TableObject*>( screen()->objectManager()->object( m_object_name ) );
                    // kvs::ParallelCoordinatesRenderer* renderer = static_cast<kvs::ParallelCoordinatesRenderer*>( screen()->rendererManager()->renderer( m_renderer_name ) );
                    m_factor = m_factor - 0.1;
                    if (m_factor < -1)
                    {
                        m_factor = -1;
                    }
                    size_t nr = object->nrows();
                    kvs::ValueArray<double> alpha(nr);
                    alpha = object->column(3).asValueArray<double>();
                    if (m_pos == "x")
                    {
                        for(size_t i=0; i<nr; i++)
                        {
                            alpha[i] = object->column(0).to<double>(i) * m_factor + object->column(1).to<double>(i);
                        }
                    }
                    else if (m_pos == "y")
                    {
                        for(size_t i=0; i<nr; i++)
                        {
                            alpha[i] = object->column(0).to<double>(i) + object->column(1).to<double>(i) * m_factor;
                        }
                    }
                    double min = alpha[0];
                    double max = alpha[0];
                    for (size_t i=0; i<nr; i++)
                    {
                        if (alpha[i] < min)
                        {
                            min = alpha[i];
                        }
                        if (alpha[i] > max)
                        {
                            max = alpha[i];
                        }
                    }
                    object->setMinValue( 3, min );
                    object->setMaxValue( 3, max );
                    object->setMinRange( 3, min );
                    object->setMaxRange( 3, max );
                    m_screen->redraw();
                    break;
                }
            case kvs::Key::o:
                {
                    kvs::TableObject* object = static_cast<kvs::TableObject*>( screen()->objectManager()->object( m_object_name ) );
                    // kvs::ParallelCoordinatesRenderer* renderer = static_cast<kvs::ParallelCoordinatesRenderer*>( screen()->rendererManager()->renderer( m_renderer_name ) );
                    m_factor = m_factor + 0.01;
                    if (m_factor > 1)
                    {
                        m_factor = 1;
                    }
                    size_t nr = object->nrows();
                    kvs::ValueArray<double> alpha(nr);
                    alpha = object->column(3).asValueArray<double>();
                    if (m_pos == "x")
                    {
                        for(size_t i=0; i<nr; i++)
                        {
                            alpha[i] = object->column(0).to<double>(i) * m_factor + object->column(1).to<double>(i);
                        }
                    }
                    else if (m_pos == "y")
                    {
                        for(size_t i=0; i<nr; i++)
                        {
                            alpha[i] = object->column(0).to<double>(i) + object->column(1).to<double>(i) * m_factor;
                        }
                    }
                    double min = alpha[0];
                    double max = alpha[0];
                    for (size_t i=0; i<nr; i++)
                    {
                        if (alpha[i] < min)
                        {
                            min = alpha[i];
                        }
                        if (alpha[i] > max)
                        {
                            max = alpha[i];
                        }
                    }
                    object->setMinValue( 3, min );
                    object->setMaxValue( 3, max );
                    object->setMinRange( 3, min );
                    object->setMaxRange( 3, max );
                    m_screen->redraw();
                    break;
                }
            case kvs::Key::l:
                {
                    kvs::TableObject* object = static_cast<kvs::TableObject*>( screen()->objectManager()->object( m_object_name ) );
                    // kvs::ParallelCoordinatesRenderer* renderer = static_cast<kvs::ParallelCoordinatesRenderer*>( screen()->rendererManager()->renderer( m_renderer_name ) );
                    m_factor = m_factor - 0.01;
                    if (m_factor < -1)
                    {
                        m_factor = -1;
                    }
                    size_t nr = object->nrows();
                    kvs::ValueArray<double> alpha(nr);
                    alpha = object->column(3).asValueArray<double>();
                    if (m_pos == "x")
                    {
                        for(size_t i=0; i<nr; i++)
                        {
                            alpha[i] = object->column(0).to<double>(i) * m_factor + object->column(1).to<double>(i);
                        }
                    }
                    else if (m_pos == "y")
                    {
                        for(size_t i=0; i<nr; i++)
                        {
                            alpha[i] = object->column(0).to<double>(i) + object->column(1).to<double>(i) * m_factor;
                        }
                    }
                    double min = alpha[0];
                    double max = alpha[0];
                    for (size_t i=0; i<nr; i++)
                    {
                        if (alpha[i] < min)
                        {
                            min = alpha[i];
                        }
                        if (alpha[i] > max)
                        {
                            max = alpha[i];
                        }
                    }
                    object->setMinValue( 3, min );
                    object->setMaxValue( 3, max );
                    object->setMinRange( 3, min );
                    object->setMaxRange( 3, max );
                    m_screen->redraw();
                    break;
                }
            default: break;
        }
    }
};

int main(int argc, char** argv)
{
    std::string title = "";

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
  
    //load the given file and run it as a function
    if (luaL_loadfile(L, "config.lua") || lua_pcall(L, 0, 0, 0))
    {
        luaL_error(L, "cannot run configuration file: %s",lua_tostring(L, -1));
    }

    int n;
    double a,b,c;
    int xscale,yscale;
    bool ifout;
    lua_intexpr(L, "Param.n", &n);
    lua_numberexpr(L, "Param.a", &a);
    lua_numberexpr(L, "Param.b", &b);
    lua_numberexpr(L, "Param.c", &c);
    lua_intexpr(L, "Param.xscale", &xscale);
    lua_intexpr(L, "Param.yscale", &yscale);
    ifout = lua_boolexpr(L, "Param.ifout");

    kvs::ValueArray<double> x(n);
    kvs::ValueArray<double> y(n);
    kvs::ValueArray<double> z(n);
    kvs::ValueArray<double> alpha(z.size());
    kvs::Xorshift128 R;
    for (size_t i=0; i<z.size(); i++)
    {
        x[i] = R.rand()*xscale;
        y[i] = R.rand()*yscale;
        z[i] = a*x[i] + b*y[i] + c;
    }
    kvs::TableObject* output = new kvs::TableObject();
    output->addColumn(kvs::AnyValueArray(x), "x");
    output->addColumn(kvs::AnyValueArray(y), "y");
    output->addColumn(kvs::AnyValueArray(z), "z");

    if (ifout == true)
    {
        kvsoceanvis::util::TableObjectWriter* kvsml = new kvsoceanvis::util::TableObjectWriter(output);
        kvsml->write("output.kvsml");
        delete kvsml;
    }

    std::string object_name("TableObject");
    output->setName( object_name );

    kvs::ColorMap cmap( 256 );
    cmap.addPoint(   0, kvs::RGBColor( 0, 0, 255 ) );
    cmap.addPoint( 128, kvs::RGBColor( 255, 255, 255 ) );
    cmap.addPoint( 255, kvs::RGBColor( 255, 0, 0 ) );
    cmap.create();

    int opacity = 160;

    kvs::ParallelCoordinatesRenderer* renderer01 = new kvs::ParallelCoordinatesRenderer();
    std::string renderer_name("PCPRenderer");
    renderer01->setName( renderer_name );
    renderer01->enableAntiAliasing();
    renderer01->setLeftMargin( 30 );
    renderer01->setRightMargin( 30 );
    renderer01->setBottomMargin( 30 );
    renderer01->setColorMap( cmap );
    renderer01->setLineOpacity( opacity );
    renderer01->disableShading();

    kvs::glut::ParallelAxisRenderer* axis = new kvs::glut::ParallelAxisRenderer();
    axis->setLeftMargin( 30 );
    axis->setRightMargin( 30 );
    axis->setBottomMargin( 30 );

    kvsoceanvis::util::Event::Parameter parameter;
    kvsoceanvis::util::Event::KeyPress<kvs::TableObject,kvs::ParallelCoordinatesRenderer> key_press( parameter, object_name, renderer_name );
    kvsoceanvis::util::Event::MousePress<kvs::TableObject,kvs::ParallelCoordinatesRenderer> mouse_press( parameter, object_name, renderer_name );
    kvsoceanvis::util::Event::MouseDoubleClick<kvs::TableObject,kvs::ParallelCoordinatesRenderer> mouse_double_click( parameter, object_name, renderer_name );
    kvsoceanvis::util::Event::MouseMove<kvs::TableObject,kvs::ParallelCoordinatesRenderer> mouse_move( parameter, object_name, renderer_name );
    kvsoceanvis::util::Event::MouseRelease<kvs::TableObject,kvs::ParallelCoordinatesRenderer> mouse_release( parameter, object_name, renderer_name );
    kvsoceanvis::util::Event::Paint<kvs::TableObject,kvs::ParallelCoordinatesRenderer> paint( parameter, object_name, renderer_name );

    screen.registerObject( output, renderer01 );
    screen.registerObject( output, axis );
    screen.addKeyPressEvent( &key_press );
    screen.addMousePressEvent( &mouse_press );
    screen.addMouseDoubleClickEvent( &mouse_double_click );
    screen.addMouseMoveEvent( &mouse_move );
    screen.addMouseReleaseEvent( &mouse_release );
    screen.addPaintEvent( &paint );
    screen.background()->setColor(kvs::RGBAColor(96, 96, 96, 255));
    title += "[Parallel Coordinates]";

    screen.setGeometry( 0, 0, 768, 512 );
    screen.setTitle( title );
    screen.show();

    bool init = true;
    UpdatePCP<kvs::TableObject,kvs::ParallelCoordinatesRenderer> pcp_updator( &screen, object_name, renderer_name );
    if (init == true)
    {
        pcp_updator.setPos("x");
        pcp_updator.setFactor(0.0);
        std::cout << "Present Position of Factor: x" << std::endl;
        std::cout << "Present Value of Factor: 0" << std::endl;
        for (int i=0; i<n; i++)
        {
            alpha[i] = y[i];
        }
        output->addColumn(kvs::AnyValueArray(alpha), "alpha");
        init = false;
    }
    screen.addKeyPressEvent( &pcp_updator );
    lua_close(L);
    return( app.run() );
}
