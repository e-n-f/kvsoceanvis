#include <iostream>
#include <kvs/AnyValueArray>
#include <kvs/TableObject>
#include <kvs/TableImporter>
#include <kvs/ColorMap>
#include <kvs/Background>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/glut/ParallelAxisRenderer>
#include <kvs/ScatterPlotRenderer>
#include <kvs/glut/Axis2DRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <lua5.1/lua.hpp>
#include <util/InformationTheoreticApproach.h>

int main(int argc, char** argv)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
  
    //load the given file and run it as a function
    if (luaL_loadfile(L, "config.lua") || lua_pcall(L, 0, 0, 0))
    {
        luaL_error(L, "cannot run configuration file: %s",lua_tostring(L, -1));
    }

    int k;
    int n;
    bool show;
    lua_getglobal(L, "Param");
    lua_pushstring(L, "k");
    lua_gettable(L, -2);
    k = (int)lua_tonumber(L,-1);
    lua_remove(L,-1);
    lua_pushstring(L, "n");
    lua_gettable(L, -2);
    n = (int)lua_tonumber(L,-1);
    lua_remove(L,-1);
    kvs::ValueArray<double> x(n);
    kvs::ValueArray<double> y(n);
    lua_pushstring(L, "show");
    lua_gettable(L, -2);
    show = lua_toboolean(L,-1);
    lua_remove(L,-1);

    lua_pushstring(L, "x");
    lua_gettable(L, -2);
    for (int i=0; i<n; i++)
    {
        lua_pushnumber(L, i+1);
        lua_gettable(L, -2);
        x[i] = (double)lua_tonumber(L, -1);
        lua_remove(L,-1);
    }

    lua_remove(L,-1);
    lua_pushstring(L, "y");
    lua_gettable(L, -2);
    for (int i=0; i<n; i++)
    {
        lua_pushnumber(L, i+1);
        lua_gettable(L, -2);
        y[i] = (double)lua_tonumber(L, -1);
        lua_remove(L,-1);
    }

    kvs::TableObject* input = new kvs::TableObject();
    input->addColumn(kvs::AnyValueArray(x));
    input->addColumn(kvs::AnyValueArray(y));
    ita::ITApproach cnumber;
    kvs::Timer timer;
    timer.start();
    int c = cnumber.exec(*input, k);
    timer.stop();
    std::cout << "Duration Time: " << timer.msec() << " [msec]" << std::endl;
    std::cout << "Number of Clusters: " << c << std::endl;

    if (show == true)
    {
        std::string title = "Scatter Plot";

        kvs::glut::Application app( argc, argv );
        kvs::glut::Screen screen( &app );
        kvs::ScatterPlotRenderer* renderer01 = new kvs::ScatterPlotRenderer();
        kvs::glut::Axis2DRenderer* renderer02 = new kvs::glut::Axis2DRenderer();
        renderer01 -> disableShading();
        renderer01 -> setPointSize(6);
        renderer02 -> disableShading();
        screen.registerObject( input, renderer01 );
        screen.registerObject( input, renderer02 );
        screen.setGeometry( 0, 0, 768, 512 );
        screen.setTitle( title );
        screen.show();

        lua_close(L);       
        return( app.run() );
    }
    else
    {
        delete input;
        lua_close(L);
        return 0;
    }
}
