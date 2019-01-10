#include <vector>
#include <cassert>
#include <cstring>
#include <cmath>

#include <kvs/TableImporter>
#include <kvs/ColorMap>
#include <kvs/PointObject>
#include <kvs/PointExporter>

#include "Tab2Point.h"

namespace ttp
{
    TPConv::TPConv()
    {
        // null
    }

    void TPConv::exec(const kvs::TableObject* input, const int xidx, const int yidx, const int vidx, const int psize, const float zvalue, kvs::ColorMap cmap, const std::string filename)
    {
        size_t n = input->nrows();

        kvs::ValueArray<float> xcoords(n);
        kvs::ValueArray<float> ycoords(n);
        kvs::ValueArray<float> values(n);
        for (size_t i=0; i<n; i++)
        {
            xcoords[i] = input->column(xidx).to<float>(i);
            ycoords[i] = input->column(yidx).to<float>(i);
            values[i] = input->column(vidx).to<float>(i);
        }

        float min = values[0];
        float max = values[0];

        for (size_t i=0; i<values.size(); i++)
        {
            if (values[i] < min) { min = values[i]; }
            if (values[i] > max) { max = values[i]; }
        }
        
        kvs::ValueArray<kvs::UInt8> colors(values.size()*3);
        cmap.setRange(min, max);
        for (size_t i=0; i<values.size(); i++)
        {
            kvs::RGBColor color = cmap.at( values[i] );

            colors[i*3] = color.r();
            colors[i*3+1] = color.g();
            colors[i*3+2] = color.b();
        }

        kvs::ValueArray<float> coords(n*3);
        for (size_t i=0; i<values.size(); i++)
        {
            coords[i*3] = xcoords[i];
            coords[i*3+1] = ycoords[i];
            coords[i*3+2] = zvalue;
        }

        kvs::PointObject* point = new kvs::PointObject();
        point->setCoords(coords);
        point->setColors(colors);
        point->setSize(psize);

        kvs::KVSMLObjectPoint* points = new kvs::PointExporter<kvs::KVSMLObjectPoint>( point );
        points->setWritingDataType( kvs::KVSMLObjectPoint::ExternalBinary );
        points->write( filename );
    }
}
