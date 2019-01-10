#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/GrADS>
#include <kvs/StructuredVolumeObject>
#include <kvs/AnyValueArray>
#include <kvs/Math>
#include <kvs/ExternalFaces>
#include <kvs/TransferFunction>
#include <kvs/StructuredVectorToScalar>
#include <kvs/PolygonRenderer>
#include <kvs/PointObject>
#include <kvs/SphereGlyph>
#include <kvs/PointRenderer>
#include <kvs/Matrix22>
#include <kvs/PointExporter>
#include <kvs/PointImporter>
#include <kvs/ColorMap>
#include <fstream>
#include <iostream>
#include <exception>


#include "VectorVolumeCell.h"
#include "Streamline.h"

const int zpad_low = 0;
const int zpad_high = 30;
const int ypad_low = 233;
const int ypad_high = 159;
const int xpad_low = 243;
const int xpad_high = 360;
float g_IgnoreValue = 0;



template <typename T, typename U>
void FitBounds(T& dst, const U& src)
{
    dst.setMinMaxObjectCoords(src.minObjectCoord(), src.maxObjectCoord());
    dst.setMinMaxExternalCoords(src.minExternalCoord(), src.maxExternalCoord());
}

kvs::PointObject* ExtractVortexes(const kvs::StructuredVolumeObject& volume)
{
    const size_t dimx = volume.resolution().x();
    const size_t dimy = volume.resolution().y();
    const size_t dimz = volume.resolution().z();

    MY_ASSERT(xpad_low < (int)dimx - xpad_high);
    MY_ASSERT(ypad_low < (int)dimy - ypad_high);
    MY_ASSERT(zpad_low < (int)dimz - zpad_high);

    std::vector<float> coords;

    VectorVolumeCell cell;
    cell.attach(volume);
    cell.setIgnoreValue(g_IgnoreValue);

    for (size_t k = zpad_low; k < dimz - zpad_high; ++k)
    {
        for (size_t j = ypad_low; j < dimy - ypad_high; ++j)
        {
            for (size_t i = xpad_low; i < dimx - xpad_high; ++i)
            {
                cell.load(i, j, k);
                cell.findCriticalPoint(coords);
            }
        }
    }

    kvs::PointObject* object = new kvs::PointObject();
    object->setCoords(kvs::ValueArray<float>(coords));
    object->setSize(3);
    object->setColor(kvs::RGBColor(255, 255, 0));
    FitBounds(*object, volume);

    return object;
}

kvs::StructuredVolumeObject* ReadGrADSData(char** argv)
{
    // Read GrADS data.
    size_t tindex = 0;

    kvs::GrADS udata(argv[1]);
    kvs::GrADS vdata(argv[2]);
    kvs::GrADS wdata(argv[3]);

    size_t dimx = udata.dataDescriptor().xdef().num;
    size_t dimy = udata.dataDescriptor().ydef().num;
    size_t dimz = udata.dataDescriptor().zdef().num;

    udata.dataList().at(tindex).load();
    vdata.dataList().at(tindex).load();
    wdata.dataList().at(tindex).load();

    const float* u = udata.dataList().at(tindex).values().pointer();
    const float* v = vdata.dataList().at(tindex).values().pointer();
    const float* w = wdata.dataList().at(tindex).values().pointer();

//    kvs::AnyValueArray values;
//    float* pvalues = static_cast<float*>(values.allocate<float>(dimx * dimy * dimz * 3));
    kvs::ValueArray<float> values( dimx * dimy * dimz * 3 );
    float* pvalues = values.data();
    const float ignore_value = udata.dataDescriptor().undef().value;
    g_IgnoreValue = ignore_value;
    double min_value = kvs::Value<double>::Max();
    double max_value = 0;

    size_t t = 0;
    for (size_t k = 0; k < dimz; ++k)
    {
        for (size_t j = 0; j < dimy; ++j)
        {
            for (size_t i = 0; i < dimx; ++i)
            {
                float lu = u[t];
                float lv = v[t];
                float lw = w[t];
                *(pvalues++) = lu;
                *(pvalues++) = lv;
                *(pvalues++) = lw;
                if ((int)i >= xpad_low && (int)i <= (int)dimx - xpad_high
                 && (int)j >= ypad_low && (int)j <= (int)dimy - ypad_high
                 && (int)k >= zpad_low && (int)k <= (int)dimz - zpad_high)
                {
                    if (lu != ignore_value && lv != ignore_value && lw != ignore_value)
                    {
                        double length = std::sqrt(lu*lu+lv*lv+lw*lw);
                        min_value = kvs::Math::Min(min_value, length);
                        max_value = kvs::Math::Max(max_value, length);
                    }
                }
                ++t;
            }
        }
    }

    udata.dataList().at(tindex).free();
    vdata.dataList().at(tindex).free();
    wdata.dataList().at(tindex).free();

    kvs::StructuredVolumeObject* vector_volume = new kvs::StructuredVolumeObject();
    vector_volume->setGridType(kvs::StructuredVolumeObject::Uniform);
    vector_volume->setResolution(kvs::Vector3ui(dimx, dimy, dimz));
    vector_volume->setVeclen(3);
    vector_volume->setValues(values);
    vector_volume->setMinMaxValues(0, max_value);
    vector_volume->updateMinMaxCoords();
    //vector_volume->updateMinMaxValues();

    kvs::Vector3f lower_bounds((float)xpad_low, (float)ypad_low, (float)zpad_low);
    kvs::Vector3f greater_bounds((float)dimx - xpad_high, (float)dimy - ypad_high, (float)dimz - zpad_high);
    vector_volume->setMinMaxObjectCoords(lower_bounds, greater_bounds);

    return vector_volume;
}

kvs::LineObject* MakeStreamLine(const kvs::StructuredVolumeObject& vector_volume, kvs::PointObject& vortexes)
{
    kvs::TransferFunction tfunc;
    kvs::test::Streamline* streamline = new kvs::test::Streamline();
    streamline->setVortexes(&vortexes);
    streamline->setIntegrationInterval(0.1);
    streamline->setTransferFunction(tfunc);
    streamline->setIntegrationMethod(kvs::test::Streamline::RungeKutta4th);
    streamline->setVectorLengthThreshold(0.1);
    //streamline->setEnableIntegrationTimesCondition(false);
    streamline->setIntegrationTimesThreshold(400);
    streamline->setEnableVectorLengthCondition(true);
    streamline->setIgnoreValue(g_IgnoreValue);

    kvs::LineObject* line = streamline->exec(&vector_volume);
    std::vector<float> vortex_radiuses = streamline->radiuses();
    std::vector<kvs::UInt8> colors;
    for (size_t i = 0; i < vortex_radiuses.size(); ++i)
    {
        std::cout << i << ":" << vortex_radiuses[i] << std::endl;
        if (vortex_radiuses[i] > 0)
        {
            colors.push_back(255);
            colors.push_back(0);
            colors.push_back(0);
        }
        else
        {
            colors.push_back(255);
            colors.push_back(255);
            colors.push_back(0);
        }
    }
    vortexes.setColors(kvs::ValueArray<kvs::UInt8>(colors));

    FitBounds(*line, vector_volume);
    return line;
}

int main(int argc, char** argv)
{
    kvs::glut::Application app(argc, argv);

    kvs::StructuredVolumeObject* vector_volume = ReadGrADSData(argv);
    std::cout << "volume" << std::endl;
    vector_volume->print( std::cout );

    kvs::PointObject* vortexes = ExtractVortexes(*vector_volume);
    std::cout << "vortexes" << std::endl;
    vortexes->print( std::cout << std::endl );

    kvs::LineObject* streamline = MakeStreamLine(*vector_volume, *vortexes);
    std::cout << "streamline" << std::endl;
    streamline->print( std::cout << std::endl );

    delete vector_volume;

    kvs::glut::Screen screen(&app);
    screen.setBackgroundColor(kvs::RGBColor(150, 150, 150));
    screen.registerObject(vortexes);
    screen.registerObject(streamline);

    screen.show();
    return app.run();
}
