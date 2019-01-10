#include <vector>
#include <cassert>
#include <cstring>
#include <kvs/AnyValueArray>
#include <kvs/Vector>
#include <kvs/TableObject>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeExporter>

#include "Tab2Volume.h"

namespace ttv
{
    TVConv::TVConv()
    {
        // null
    }

    void TVConv::exec(const kvs::TableObject* input, const kvs::Vector3i posidx, const int ndim, const int vidx, const kvs::Vector3i vidxes, const bool inverse, const std::string filename)
    {
        int xidx = posidx.x();
        int yidx = posidx.y();
        int zidx = posidx.z();
        int resx = calcres(input, xidx);
        int resy = calcres(input, yidx);
        int resz = calcres(input, zidx);
        int n = resx*resy*resz;
        kvs::ValueArray<float> coords(input->nrows()*3);
        for (size_t i=0; i<input->nrows(); i++)
        {
            coords[i*3] = input->column(xidx).to<float>(i);
            coords[i*3+1] = input->column(yidx).to<float>(i);
            coords[i*3+2] = input->column(zidx).to<float>(i);
        }

        kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
        volume->setCoords(coords);
        if ( ndim == 1 )
        {
            if (inverse == true)
            {
                kvs::ValueArray<float> value(n);
                for (int j=0; j<resz; j++)
                {
                    for (int i=0; i<resx*resy; i++)
                    {
                        value[(resz-j-1)*resx*resy+i] = input->column(vidx).to<float>(j*resx*resy+i);
                    }
                }
                volume->setValues(kvs::AnyValueArray(value));
            }
            else
            {
                volume->setValues(input->column(vidx));
            }

            volume->setGridType(kvs::StructuredVolumeObject::Uniform);
            volume->setVeclen(1);
            volume->setResolution(kvs::Vector3ui(resx,resy,resz));
            volume->updateMinMaxValues();
        }
        else if ( ndim == 3 )
        {
            int uidx = vidxes.x();
            int vidx = vidxes.y();
            int widx = vidxes.z();

            kvs::ValueArray<float> u(input->nrows());
            kvs::ValueArray<float> v(input->nrows());
            kvs::ValueArray<float> w(input->nrows());

            if (inverse == true)
            {
                for (int j=0; j<resz; j++)
                {
                    for (int i=0; i<resx*resy; i++)
                    {
                        u[(resz-j-1)*resx*resy+i] = input->column(uidx).to<float>(j*resx*resy+i);
                        v[(resz-j-1)*resx*resy+i] = input->column(vidx).to<float>(j*resx*resy+i);
                        w[(resz-j-1)*resx*resy+i] = input->column(widx).to<float>(j*resx*resy+i);
                    }
                }
            }
            else
            {
                for (size_t i=0; i<input->nrows(); i++)
                {
                    u[i] = input->column(uidx).to<float>(i);
                    v[i] = input->column(vidx).to<float>(i);
                    w[i] = input->column(widx).to<float>(i);
                }
            }

            kvs::ValueArray<float> value(n*3);
            for (int i=0; i<n; i++)
            {
                value[i*3] = u[i];
                value[i*3+1] = v[i];
                value[i*3+2] = w[i];
            }

            volume->setValues(kvs::AnyValueArray(value));
            volume->setGridType(kvs::StructuredVolumeObject::Uniform);
            volume->setVeclen(3);
            volume->setResolution(kvs::Vector3ui(resx,resy,resz));
            volume->updateMinMaxValues();
        }
        else
        {
            std::cout << "Error: please check the number of dimensions." << std::endl;
        }
        kvs::StructuredVolumeExporter<kvs::KVSMLObjectStructuredVolume> exporter(volume);
        exporter.setWritingDataType(kvs::KVSMLObjectStructuredVolume::ExternalBinary);
        exporter.write(filename);
    }

    int TVConv::calcres(const kvs::TableObject* input, const int index)
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
}
