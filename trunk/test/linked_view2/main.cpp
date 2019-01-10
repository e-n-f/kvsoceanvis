#include <iostream>
#include <cmath>
#include <cstring>
#include <kvs/Vector2>
#include <kvs/Vector3>
#include <kvs/AnyValueArray>
#include <kvs/GrADS>
#include <kvs/Bounds>
#include <kvs/TableObject>
#include <kvs/TableImporter>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/PointObject>
#include <kvs/PointImporter>
#include <kvs/AxisObject>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/glut/ParallelAxisRenderer>
#include <kvs/ColorMap>
#include <kvs/Background>
#include <kvs/PointRenderer>
#include <kvs/Streamline>
#include <kvs/RayCastingRenderer>
#include <kvs/OrthoSlice>
#include <kvs/PolygonRenderer>
#include <kvs/LineRenderer>
#include <kvs/TransferFunction>
#include <kvs/Camera>
#include <kvs/Mouse>
#include <kvs/MousePressEventListener>
#include <kvs/MouseReleaseEventListener>
#include <kvs/MouseMoveEventListener>
#include <kvs/MouseEvent>
#include <kvs/MouseButton>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <Event.h>
#include <Widget.h>
#include <VortexPoint.h>
#include <VortexPointVolume.h>
#include <StructuredVolumeImporter.h>
#include <SeedPoint.h>
#include <Vorticity.h>
#include <Timer.h>
#include <TransferFunctionEditor.h>

#include "TableFilter.h"
#include "Tab2Volume.h"
#include "Tab2Point.h"
#include "ButtonConfig.h"
#include "ScatterPlotMatrixRenderer.h"
#include "Axis2DMatrix.h"

#include <lua5.1/lua.hpp>
#include "luautils.h"

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

    int opacity;
    int xidx, yidx, zidx, vidx, muidx, mvidx, mwidx;
    int pvidx, psize;
    double depth;
    bool inverse;
    bool hidepos, hideuvw, showpcp, showvr, showspm;

    lua_intexpr(L, "Param.opacity", &opacity);
    lua_intexpr(L, "Param.posindex.x", &xidx);
    lua_intexpr(L, "Param.posindex.y", &yidx);
    lua_intexpr(L, "Param.posindex.z", &zidx);
    lua_intexpr(L, "Param.vindex", &vidx);
    lua_intexpr(L, "Param.vindices.u", &muidx);
    lua_intexpr(L, "Param.vindices.v", &mvidx);
    lua_intexpr(L, "Param.vindices.w", &mwidx);

    lua_intexpr(L, "Param.pvindex", &pvidx);
    lua_intexpr(L, "Param.psize", &psize);

    lua_numberexpr(L, "Param.depth", &depth);
    inverse = lua_boolexpr(L, "Param.inverse");
    hidepos = lua_boolexpr(L, "Param.hidepos");
    hideuvw = lua_boolexpr(L, "Param.hideuvw");
    showpcp = lua_boolexpr(L, "Param.showpcp");
    showvr = lua_boolexpr(L, "Param.showvr");
    showspm = lua_boolexpr(L, "Param.showspm");

    kvs::Vector3i posidx;
    kvs::Vector3i mids;
    posidx.set(xidx, yidx, zidx);
    mids.set(muidx, mvidx, mwidx);

    std::string inputfile(lua_stringexpr(L, "Param.inputfile", 0));
    kvs::TableObject* input = new kvs::TableImporter(inputfile);
    std::string object_name("TableObject");
    input->setName( object_name );
    double xmin = input->minValue(xidx);
    double xmax = input->maxValue(xidx);
    double ymin = input->minValue(yidx);
    double ymax = input->maxValue(yidx);
    //double zmin = input->minValue(zidx);
    //double zmax = input->maxValue(zidx);

    kvs::Vector2f objmin;
    objmin.set(float(xmin), float(ymin));
    kvs::Vector2f objmax;
    objmax.set(float(xmax), float(ymax));

    std::string cpuefile(lua_stringexpr(L, "Param.cpuefile", 0));
    kvs::TableObject* cpueinput = new kvs::TableImporter(cpuefile);

    std::string volumeout(lua_stringexpr(L, "Param.volumeout", 0));
    ttv::TVConv ttvolume;
    ttvolume.exec(input, posidx, 1, vidx, mids, inverse, volumeout);
    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeImporter( volumeout );
    std::string object_name03("VolumeObject");
    volume->setName( object_name03 );
    float xres = volume->resolution().x();
    float yres = volume->resolution().y();
    //float zres = volume->resolution().z();
    float xfactor = xres/(xmax - xmin);
    float yfactor = yres/(ymax - ymin);

    kvs::glut::Screen screen02( &app );
    std::string title02 = "";
    std::string object_name01("TableObject2");
    std::string tableout(lua_stringexpr(L, "Param.tableout", 0));

    kvs::TableObject* table = new kvs::TableObject();
    if(hidepos == true && hideuvw == false)
    {
        for (size_t i=0; i<input->ncolumns(); i++)
        {
            if (i != size_t(xidx) && i != size_t(yidx) && i != size_t(zidx))
            {
                table->addColumn(input->column(i), input->label(i));
            }
        }
    }
    else if(hidepos == false && hideuvw == true)
    {
        for (size_t i=0; i<input->ncolumns(); i++)
        {
            if (i != size_t(muidx) && i != size_t(mvidx) && i != size_t(mwidx))
            {
                table->addColumn(input->column(i), input->label(i));
            }
        }
    }
    else if(hidepos == true && hideuvw == true)
    {
        for (size_t i=0; i<input->ncolumns(); i++)
        {
            if (i != size_t(xidx) && i != size_t(yidx) && i != size_t(zidx) && i != size_t(muidx) && i != size_t(mvidx) && i != size_t(mwidx))
            {
                table->addColumn(input->column(i), input->label(i));
            }
        }
    }
    else
    {
        for (size_t i=0; i<input->ncolumns(); i++)
        {
            table->addColumn(input->column(i), input->label(i));
        }
    }
    table->setName( object_name01 );

    kvs::ColorMap cmap( 256 );
    cmap.addPoint(   0, kvs::RGBColor( 0, 0, 255 ) );
    cmap.addPoint( 128, kvs::RGBColor( 255, 255, 255 ) );
    cmap.addPoint( 255, kvs::RGBColor( 255, 0, 0 ) );
    cmap.create();

    kvs::ColorMap cmap02( 256 );
    cmap02.create();

    kvs::ColorMap cmap03( 256 );
    cmap03.addPoint(   0, kvs::RGBColor( 0, 255, 192 ) );
    cmap03.addPoint( 128, kvs::RGBColor( 255, 255, 255 ) );
    cmap03.addPoint( 255, kvs::RGBColor( 192, 0, 255 ) );
    cmap03.create();

    kvs::ParallelCoordinatesRenderer* renderer03 = new kvs::ParallelCoordinatesRenderer();
    std::string renderer_name03("PCPRenderer");
    renderer03->setName( renderer_name03 );
    renderer03->enableAntiAliasing();
    renderer03->setLeftMargin( 42 );
    renderer03->setRightMargin( 42 );
    renderer03->setTopMargin( 45 );
    renderer03->setBottomMargin( 36 );
    renderer03->setColorMap( cmap );
    renderer03->setLineOpacity( opacity );
    renderer03->disableShading();

    kvs::glut::ParallelAxisRenderer* axis = new kvs::glut::ParallelAxisRenderer();
    axis->setLeftMargin( 42 );
    axis->setRightMargin( 42 );
    axis->setTopMargin( 45 );
    axis->setBottomMargin( 36 );

    kvsoceanvis::util::Event::Parameter parameter;
    kvsoceanvis::util::Event::KeyPress<kvs::TableObject,kvs::ParallelCoordinatesRenderer> key_press( parameter, object_name01, renderer_name03 );
    kvsoceanvis::util::Event::MousePress<kvs::TableObject,kvs::ParallelCoordinatesRenderer> mouse_press( parameter, object_name01, renderer_name03 );
    kvsoceanvis::util::Event::MouseDoubleClick<kvs::TableObject,kvs::ParallelCoordinatesRenderer> mouse_double_click( parameter, object_name01, renderer_name03 );
    kvsoceanvis::util::Event::MouseMove<kvs::TableObject,kvs::ParallelCoordinatesRenderer> mouse_move( parameter, object_name01, renderer_name03 );
    kvsoceanvis::util::Event::MouseRelease<kvs::TableObject,kvs::ParallelCoordinatesRenderer> mouse_release( parameter, object_name01, renderer_name03 );
    kvsoceanvis::util::Event::Paint<kvs::TableObject,kvs::ParallelCoordinatesRenderer> paint( parameter, object_name01, renderer_name03 );

    screen02.registerObject( table, renderer03 );
    screen02.registerObject( table, axis );
    screen02.addKeyPressEvent( &key_press );
    screen02.addMousePressEvent( &mouse_press );
    screen02.addMouseDoubleClickEvent( &mouse_double_click );
    screen02.addMouseMoveEvent( &mouse_move );
    screen02.addMouseReleaseEvent( &mouse_release );
    screen02.addPaintEvent( &paint );
    screen02.background()->setColor(kvs::RGBAColor(96, 96, 96, 1.0f));
    title02 += "[Parallel Coordinates]";

    screen02.setGeometry( 0, 0, 768, 192 );
    screen02.setTitle( title02 );
    if (showpcp == true)
    {
        screen02.show();
    }

    kvsoceanvis::util::Widget::AxisSelector<kvs::TableObject,kvs::ParallelCoordinatesRenderer> selector( &screen02, object_name01, renderer_name03 );
    selector.setLeftMargin( 42 );
    selector.setRightMargin( 42 );
    selector.create();
    selector.select( renderer03->activeAxis() );
    selector.show();

    const float dparam = volume->resolution().z() * float(depth);
    const kvs::OrthoSlice::AlignedAxis aaxis = kvs::OrthoSlice::ZAxis;
    kvs::TransferFunction ostfunc( 256 );
    ostfunc.setColorMap( cmap02 );
    kvs::PolygonObject* object = new kvs::OrthoSlice( volume, dparam, aaxis, ostfunc );
    std::string object_name02("PolygonObject");
    object->setName( object_name02 );

    std::string pointout(lua_stringexpr(L, "Param.pointout", 0));
    ttp::TPConv ttpoint;
    ttpoint.exec(cpueinput, xidx, yidx, pvidx, psize, (dparam+0.1), cmap, pointout);
    kvs::PointObject* point = new kvs::PointImporter( pointout );
    point->setSize(psize);
    size_t pnum = cpueinput->nrows();
    kvs::ValueArray<float> pcoords(pnum*3);
    for (size_t i=0; i<pnum; i++)
    {
        pcoords[i*3] = (cpueinput->column(xidx).to<float>(i) - xmin)*xfactor;
        pcoords[i*3+1] = (cpueinput->column(yidx).to<float>(i) - ymin)*yfactor;
        pcoords[i*3+2] = dparam + 0.1;
    }
    point->setCoords(pcoords);

    kvs::PolygonRenderer* renderer01 = new kvs::PolygonRenderer();
    renderer01 -> disableShading();
    screen.registerObject( object, renderer01 );

    kvs::PointRenderer* renderer02 = new kvs::PointRenderer();
    renderer02 -> disableShading();
    screen.registerObject( point, renderer02 );

    title += "[2D Map]";

    screen.setGeometry( 0, 0, 512, 512 );
    screen.background()->setColor(kvs::RGBAColor(0, 0, 0, 1.0f));
    //screen.camera()->setProjectionType( kvs::Camera::Orthogonal );
    screen.setTitle( title );
    screen.show();

    kvs::glut::Screen screen03( &app );
    std::string object_name04("LineObject");
    std::string renderer_name04("RayCastingRenderer");
    std::string ivreffile(lua_stringexpr(L, "Param.ivreffile", 0));
    std::string volumeout3(lua_stringexpr(L, "Param.volumeout3", 0));

    kvs::TransferFunction tfunc( 256 );
    tfunc.setColorMap( cmap02 );
    kvs::RayCastingRenderer* renderer04 = new kvs::RayCastingRenderer();
    renderer04 -> setName(renderer_name04);
    renderer04 -> disableShading();
    renderer04 -> enableLODControl(12);
    renderer04 -> setTransferFunction(tfunc);
    kvs::LineRenderer* renderer05 = new kvs::LineRenderer();
    renderer05 -> disableShading();

    kvsoceanvis::util::TransferFunctionEditor<kvs::RayCastingRenderer> editor( &screen03, renderer_name04 );
    editor.setVolumeObject( volume );
    editor.setTransferFunction( tfunc );

    if (showpcp == true && showvr == true)
    {
        std::string title03 = "";

        kvs::GrADS ivref(ivreffile);
        const float ignore_value = ivref.dataDescriptor().undef().value;
        ttv::TVConv ttvolume3;
        ttvolume3.exec(input, posidx, 3, vidx, mids, inverse, volumeout3);
        kvs::StructuredVolumeObject* volume3 = new kvs::StructuredVolumeImporter( volumeout3 );
        kvs::UnstructuredVolumeObject* tensor = new kvsoceanvis::util::VortexPointVolume( volume3, ignore_value );
        kvs::PointObject* cpoint = new kvsoceanvis::util::VortexPoint( tensor );
        cpoint->setSize( 3 );
        cpoint->setNormals( kvs::ValueArray<kvs::Real32>(0) ); // delete normal vectors
        cpoint->setColor( kvs::RGBColor( 0, 0, 0 ) );
        delete tensor;
        const float delta = 0.5;
        kvs::PointObject* seed_point = new kvsoceanvis::util::SeedPoint( cpoint, delta );
        delete cpoint;

        kvs::Streamline* streamline = new kvs::Streamline();
        streamline->setSeedPoints( seed_point );
        streamline->setIntegrationInterval( 0.2 );
        streamline->setIntegrationMethod( kvs::Streamline::RungeKutta2nd );
        streamline->setVectorLengthThreshold( 0.2 );
        streamline->setIntegrationDirection( kvs::Streamline::BothDirections );
        streamline->setIntegrationTimesThreshold( 100 );
        streamline->setEnableIntegrationTimesCondition( true );
        streamline->setEnableVectorLengthCondition( false );
        streamline->setColorMap( cmap02 );
        kvs::LineObject* line;
        line = streamline->exec( volume3 );
        line->setName(object_name04);
        delete seed_point;
        delete volume3;

        title03 += "[Volume Rendering]";
        screen03.setGeometry( 0, 0, 512, 512 );
        screen03.background()->setColor(kvs::RGBAColor(160, 160, 160, 1.0f));
        screen03.setTitle( title03 );
        //screen03.registerObject( new kvs::Bounds( line ) );
        screen03.registerObject( line, renderer05 );
        //screen03.registerObject( cpoint );
        screen03.registerObject( volume, renderer04 );
        screen03.show();

        editor.show();
    }

    kvs::glut::Screen screen04( &app );
    kvs::TableObject* table2 = new kvs::TableObject();
    for (size_t i=0; i<table->ncolumns(); i++)
    {
        table2->addColumn(table->column(i), table->label(i));
    }
    std::string object_name05("TableObject3");
    table2 -> setName(object_name05);
    kvs::ScatterPlotMatrixRenderer* renderer06 = new kvs::ScatterPlotMatrixRenderer();
    renderer06->setBackgroundColor(kvs::RGBAColor(96, 96, 96, 0.5f));
    std::string renderer_name06("ScatterPlotMatrixRenderer");
    renderer06 -> setName(renderer_name06);
    renderer06 -> setMargin( 10 );
    renderer06 -> setColorMap(cmap03);

    kvs::glut::Axis2DMatrix* axisspm = new kvs::glut::Axis2DMatrix();
    axisspm->setMargin( 10 );
    axisspm->setAxisWidth( 3.0 );
    axisspm->setLabelColor( kvs::RGBColor(255, 255, 255) );
    axisspm->setValueColor( kvs::RGBColor(255, 255, 255) );

    if (showspm == true)
    {
        std::string title04 = "";
        title04 += "[Scatter Plot Matrix]";
        screen04.setGeometry( 0, 0, 768, 512 );
        screen04.background()->setColor(kvs::RGBAColor(0, 0, 0, 1.0f));
        screen04.registerObject( table2, renderer06 );
        screen04.registerObject( table2, axisspm );
        screen04.setTitle( title04 );
        screen04.show();
    }

    UpdatePR pr_updator( &screen02, &screen04, object_name01, object_name05, tableout, input, posidx, mids, hidepos, hideuvw );
    MousePress mp_updator( &screen, object_name02, objmin, objmax, &pr_updator );
    MouseRelease mr_updator( &screen, object_name02, objmin, objmax, &pr_updator );
    MouseMove mm_updator( &screen );
    PaintEvent pr_selectregion;
    WheelScroll pr_wsupdator;
    screen.setMousePressEvent( &mp_updator );
    screen.setMouseReleaseEvent( &mr_updator );
    screen.setMouseMoveEvent( &mm_updator );
    screen.addKeyPressEvent( &pr_updator );
    screen.addPaintEvent( &pr_selectregion );
    screen.addWheelEvent( &pr_wsupdator );

    UpdatePCP pcp_updator;
    pcp_updator.setScreen(&screen02, &screen03, &screen04);
    pcp_updator.setObject(object_name01, object_name03, object_name04, object_name05, input);
    pcp_updator.setRenderer(renderer_name03);
    pcp_updator.setIndex(posidx, mids);
    pcp_updator.setBool(inverse, hidepos, hideuvw);
    pcp_updator.setFile(tableout, volumeout, volumeout3, ivreffile);
    screen02.addKeyPressEvent( &pcp_updator );

    lua_close(L);
    return( app.run() );
}
