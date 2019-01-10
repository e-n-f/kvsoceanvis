#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <assert.h>
#include <kvs/AnyValueArray>
#include <kvs/TableObject>
#include <kvs/TableImporter>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeExporter>
#include <kvs/RayCastingRenderer>
#include <kvs/TransferFunction>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/OrthoSlice>
#include <kvs/PolygonRenderer>
#include <kvs/glew/PolygonRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <lua5.1/lua.hpp>

#include "luautils.h"
#include "VortexQuant.h"

int calcres(const kvs::TableObject* input, const int index)
{
    int count = 0;
    for (size_t i=0; i<input->nrows(); i++)
    {
        if (input->column(index).to<float>(i) == input->column(index).to<float>(0))
        {
            count += 1;
        }
    }
    int output = input->nrows()/count;
    return output;
}

void pos2dist(const float lon0, const float lat0, const float lon1, const float lat1, float* lon, float* lat)
{
    float pi = 3.1415926535897932384626;
    float earthradius = 6378137.0;
    float radlon0 = (lon0 / 180) * pi;
    float radlat0 = (lat0 / 180) * pi;
    float radlon1 = (lon1 / 180) * pi;
    float radlat1 = (lat1 / 180) * pi;
    float a = radlat1 - radlat0;
    float b = radlon1 - radlon0;
    float m = 2*earthradius*asin(sqrt(pow(sin(0), 2) + cos(radlat0)*cos(radlat1)*pow(sin(b/2), 2))); // Haversine Formula
    float n = 2*earthradius*asin(sqrt(pow(sin(a/2), 2) + cos(radlat0)*cos(radlat1)*pow(sin(0), 2))); // Haversine Formula
    *lon = m;
    *lat = n;
}

class TransferFunctionEditor : public kvs::glut::TransferFunctionEditor
{
public:

    TransferFunctionEditor( kvs::glut::Screen* screen ):
        kvs::glut::TransferFunctionEditor( screen ){}

    void apply( void )
    {
        const kvs::RendererBase* base = screen()->rendererManager()->renderer();
        kvs::RayCastingRenderer* renderer = (kvs::RayCastingRenderer*)base;
        renderer->setTransferFunction( transferFunction() );
        screen()->redraw();
    }
};

int main(int argc, char** argv)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
  
    //load the given file and run it as a function
    if (luaL_loadfile(L, "config.lua") || lua_pcall(L, 0, 0, 0))
    {
        luaL_error(L, "cannot run configuration file: %s",lua_tostring(L, -1));
    }

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    std::string title = "Q Value";

    std::string tablefile(lua_stringexpr(L, "Param.tablefile", 0));
    kvs::TableObject* table = new kvs::TableImporter( tablefile );

    int xidx;
    int yidx;
    int zidx;
    int uidx;
    int vidx;
    lua_intexpr(L, "Param.posindex.x", &xidx);
    lua_intexpr(L, "Param.posindex.y", &yidx);
    lua_intexpr(L, "Param.posindex.z", &zidx);
    lua_intexpr(L, "Param.vindex.u", &uidx);
    lua_intexpr(L, "Param.vindex.v", &vidx);

    kvs::ValueArray<float> coords(table->nrows()*3);
    for (size_t i=0; i<table->nrows(); i++)
    {
        coords[i*3] = table->column(xidx).to<float>(i);
        coords[i*3+1] = table->column(yidx).to<float>(i);
        coords[i*3+2] = table->column(zidx).to<float>(i);
    }

    int xres = calcres(table, xidx);
    int yres = calcres(table, yidx);
    int zres = calcres(table, zidx);

    kvs::ValueArray<float> values(table->nrows()*2);
    for (size_t i=0; i<table->nrows(); i++)
    {
        values[i*2] = table->column(uidx).to<float>(i);
        values[i*2+1] = table->column(vidx).to<float>(i);
    }
    float lat0 = coords[0];
    float lon0 = coords[1];
    bool convdist = lua_boolexpr(L, "Param.convdist");
    if (convdist == true)
    {
        for (size_t i=0; i<table->nrows(); i++)
        {
            float lat, lon;
            pos2dist(lat0, lon0, coords[i*3], coords[i*3+1], &lat, &lon);
            coords[i*3] = lat;
            coords[i*3+1] = lon;
        }
    }

    int iteration;
    double clevel;
    lua_intexpr(L, "Param.iteration", &iteration);
    lua_numberexpr(L, "Param.clevel", &clevel);
    bool filter = lua_boolexpr(L, "Param.filter");
    bool ifinv = lua_boolexpr(L, "Param.inverse");

    vqt::VQuant qvl;
    kvs::ValueArray<float> qvalues = qvl.QValue(coords, values, xres, yres, zres, ifinv);
    kvs::Vector3ui res;
    res.set(xres, yres, zres);

    std::string outputfile(lua_stringexpr(L, "Param.outputfile", 0));
    kvs::StructuredVolumeObject* output = new kvs::StructuredVolumeObject;
    output->setCoords(coords);
    if (filter == true)
    {
        kvs::ValueArray<float> cqe = qvl.exec(qvalues, iteration, clevel, xres, yres);
        output->setValues(kvs::AnyValueArray(cqe));
    }
    else
    {
        output->setValues(kvs::AnyValueArray(qvalues));
    }
    output->setGridType(kvs::StructuredVolumeObject::Uniform);
    output->setVeclen(1);
    output->setResolution(res);
    output->updateMinMaxValues();
    kvs::StructuredVolumeExporter<kvs::KVSMLObjectStructuredVolume> exporter(output);
    exporter.setWritingDataType(kvs::KVSMLObjectStructuredVolume::ExternalBinary);
    exporter.write(outputfile);

    int vmode;
    lua_intexpr(L, "Param.vmode", &vmode);
    if (vmode == 1)
    {
        kvs::RayCastingRenderer* renderer01 = new kvs::RayCastingRenderer();
        renderer01 -> disableShading();
        screen.registerObject( output, renderer01 );
        // kvs::TransferFunction tfunc(256);
        // tfunc.setRange(-100, 0);
        // renderer01->setTransferFunction(tfunc);

        TransferFunctionEditor* editor = new TransferFunctionEditor( &screen );
        editor->setVolumeObject( output );
        editor->show();
    }
    else if (vmode == 2)
    {
        double opflevel;
        lua_numberexpr(L, "Param.opflevel", &opflevel);
        const float p = output->resolution().z() * opflevel;
        const kvs::OrthoSlice::AlignedAxis a = kvs::OrthoSlice::ZAxis;
        const kvs::TransferFunction t( 256 );
        kvs::PolygonObject* object = new kvs::OrthoSlice( output, p, a, t );
        kvs::PolygonRenderer* renderer02 = new kvs::PolygonRenderer();
        renderer02 -> disableShading();
        screen.registerObject( object, renderer02 );
    }

    lua_close(L);

    screen.setGeometry( 0, 0, 768, 512 );
    screen.setTitle( title );
    screen.show();
    return( app.run() );
}
