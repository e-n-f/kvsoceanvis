#include <kvs/GrADS>
#include <kvs/File>
#include <kvs/Bounds>
#include <kvs/RGBFormulae>
#include <kvs/RendererManager>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/OrientationAxis>
#include <kvs/glut/LegendBar>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RayCastingRenderer>
#include <util/StructuredVolumeImporter.h>
#include <util/CropVolume.h>
#include <util/Timer.h>

using namespace kvsoceanvis;

class OrientationAxis : public kvs::glut::OrientationAxis
{
public:

    OrientationAxis( kvs::glut::Screen* screen ):
        kvs::glut::OrientationAxis( screen )
    {
        setMargin( 10 );
        setSize( 90 );
        setBoxType( kvs::glut::OrientationAxis::SolidBox );
        enableAntiAliasing();
    }

    void screenResized( void )
    {
        setY( screen()->height() - height() );
    }
};

class LegendBar : public kvs::glut::LegendBar
{
public:

    LegendBar( kvs::ScreenBase* screen ):
        kvs::glut::LegendBar( screen )
    {
        setWidth( 200 );
        setHeight( 50 );
    }

    void screenResized( void )
    {
        setX( screen()->width() - width() );
        setY( screen()->height() - height() );
    }
};

class TransferFunctionEditor : public kvs::glut::TransferFunctionEditor
{
    std::string m_renderer_name;
    kvs::glut::LegendBar* m_legend_bar;

public:

    TransferFunctionEditor( kvs::glut::Screen* screen, std::string renderer_name ):
        kvs::glut::TransferFunctionEditor( screen )
    {
        m_renderer_name = renderer_name;
    }

    void setLegendBar( kvs::glut::LegendBar* legend_bar )
    {
        m_legend_bar = legend_bar;
    }

    void apply( void )
    {
        m_legend_bar->setColorMap( transferFunction().colorMap() );

        kvs::glut::Screen* glut_screen = dynamic_cast<kvs::glut::Screen*>( screen() );
        if ( glut_screen )
        {
            typedef kvs::glsl::RayCastingRenderer Renderer;
            kvs::RendererManager* manager = glut_screen->scene()->rendererManager();
            Renderer* renderer = static_cast<Renderer*>( manager->renderer( m_renderer_name ) );
            renderer->setTransferFunction( transferFunction() );
        }
        screen()->redraw();
    }
};

int main( int argc, char** argv )
{
    const size_t vindex = 0; // index of the value stored in the given GrADS dataset
    const size_t tindex = 0; // index of the time step stored in the given GrADS dataset
    const bool zflip = true; // flag whether the data is flipped along the z-axis or not
    const kvs::Vector3f min_range( 250, 200, 0 ); // minimum cropping range of the dataset
    const kvs::Vector3f max_range( 350, 300, 20 ); // maximum cropping range of the dataset
    const kvs::RGBColor background_color( 255, 255, 255 ); // background of the screen

    kvs::glut::Application app( argc, argv );

    // Read GrADS dataset
    util::Timer timer;
    timer.start("Read data");
    kvs::GrADS data( argv[1] );
    timer.stop();

    // Import the dataset as a structured volume object
    timer.start("Import data");
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    kvs::StructuredVolumeObject* volume = new util::StructuredVolumeImporter( &data, vindex, tindex, zflip, grid_type );
    timer.stop();

    timer.start("Crop volume");
    kvs::StructuredVolumeObject* cropped_volume = new util::CropVolume( volume, min_range, max_range );
    delete volume; volume = cropped_volume;
    timer.stop();

    // Calculate minimum and maxmum value without the ignore value specified in the dataset
    float min_value = kvs::Value<float>::Max();
    float max_value = kvs::Value<float>::Min();
    const float ignore_value = data.dataDescriptor().undef().value;
    const float* values = static_cast<const float*>( volume->values().data() );
    for ( size_t i = 0; i < volume->values().size(); i++ )
    {
        const float value = *(values++);
        if ( value != ignore_value )
        {
            min_value = kvs::Math::Min( min_value, value );
            max_value = kvs::Math::Max( max_value, value );
        }
    }
    volume->setMinMaxValues( min_value, max_value );

    // Initial color map
    kvs::ColorMap color_map = kvs::RGBFormulae::Rainbow( 256 );
    color_map.setRange( min_value, max_value );

    // Transfer function
    kvs::TransferFunction transfer_function( 256 );
    transfer_function.setRange( min_value, max_value );
    transfer_function.setColorMap( color_map );

    // Ray casting renderer with GPU
    std::string renderer_name("RCR");
    kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
    renderer->setName( renderer_name );
    renderer->setTransferFunction( transfer_function );

    // Setup rendering screen
    kvs::File file( data.dataList().at(tindex).filename() );
    kvs::glut::Screen screen( &app );
    screen.setTitle( file.fileName() );
    screen.setBackgroundColor( background_color );
    screen.registerObject( volume, new kvs::Bounds() );
    screen.registerObject( volume, renderer );
    screen.show();

    OrientationAxis orientation_axis( &screen );
    orientation_axis.show();

    LegendBar legend_bar( &screen );
    legend_bar.setColorMap( color_map );
    legend_bar.show();

    TransferFunctionEditor editor( &screen, renderer_name );
    editor.setLegendBar( &legend_bar );
    editor.setVolumeObject( volume );
    editor.setTransferFunction( transfer_function );
    editor.show();

    return app.run();
}
