#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/GrADS>
#include <kvs/Bounds>
#include <kvs/ArrowGlyph>
#include <kvs/SphereGlyph>
#include <kvs/TransferFunction>
#include <kvs/RGBFormulae>
#include <kvs/LineRenderer>
#include <kvs/PolygonRenderer>
#include <util/SeedPoint.h>
#include "Util/Import.h"
#include "Util/Crop.h"
#include "Util/Rescale.h"
#include "Util/Downsize.h"
#include "Util/Arrow.h"
#include "Util/Vortex.h"
#include "Util/Streamline.h"
#include "Util/VorticalStreamline.h"
#include "Util/CoastlJpn.h"

#include <util/OrthoSlice.h>

#include <kvs/glut/Label>
#include <kvs/glut/RadioButton>
#include <kvs/glut/RadioButtonGroup>
#include <kvs/glut/CheckBox>
#include <kvs/glut/CheckBoxGroup>
#include <kvs/ObjectManager>


namespace Widget
{

class ProjectionTypeButtonGroup
{
    class OrthogonalCameraButton : public kvs::glut::RadioButton
    {
    public:

        OrthogonalCameraButton( kvs::glut::Screen* screen ):
            kvs::glut::RadioButton( screen ){ setScene( screen->scene() ); };

        void stateChanged()
        {
            if ( this->state() ) { scene()->camera()->setProjectionTypeToOrthogonal(); }
        }
    };

    class PerspectiveCameraButton : public kvs::glut::RadioButton
    {
    public:

        PerspectiveCameraButton( kvs::glut::Screen* screen ):
            kvs::glut::RadioButton( screen ){ setScene( screen->scene() ); };

        void stateChanged()
        {
            if ( this->state() ) { scene()->camera()->setProjectionTypeToPerspective(); }
        }
    };

    kvs::glut::Label* m_label;
    kvs::glut::RadioButtonGroup m_group;
    OrthogonalCameraButton* m_orthogonal;
    PerspectiveCameraButton* m_perspective;

public:

    ProjectionTypeButtonGroup( kvs::glut::Screen* screen, size_t base_x, size_t base_y )
    {
        m_label = new kvs::glut::Label( screen );
        m_orthogonal = new OrthogonalCameraButton( screen );
        m_perspective = new PerspectiveCameraButton( screen );

        m_label->setX( base_x );
        m_label->setY( base_y );
        m_label->setMargin( 5 );
        m_label->setText( "PROJECTION TYPE" );

        m_orthogonal->setX( m_label->x() );
        m_orthogonal->setY( m_label->y() + 20 );
        m_orthogonal->setMargin( 10 );
        m_orthogonal->setCaption( "Orthogonal" );
        m_orthogonal->setState( true );

        m_perspective->setX( m_orthogonal->x() );
        m_perspective->setY( m_orthogonal->y() + 15 );
        m_perspective->setMargin( 10 );
        m_perspective->setCaption( "Perspective" );

        m_group.add( m_orthogonal );
        m_group.add( m_perspective );
    }

    ~ProjectionTypeButtonGroup()
    {
        delete m_label;
        delete m_perspective;
        delete m_orthogonal;
    }

    void show()
    {
        m_label->show();
        m_group.show();
    }
};

class RenderingObjectCheckButtonGroup
{
    class CheckBox : public kvs::glut::CheckBox
    {
        std::string m_name;

    public:

        CheckBox( kvs::glut::Screen* screen, std::string name ):
            kvs::glut::CheckBox( screen ),
            m_name( name )

        {
            setScene( screen->scene() );
        };

        void stateChanged()
        {
            if ( this->state() )
            {
                scene()->objectManager()->object( m_name )->show();
            }
            else
            {
                scene()->objectManager()->object( m_name )->hide();
            }
        }
    };

    kvs::glut::Label* m_label;
    kvs::glut::CheckBoxGroup m_group;
    CheckBox* m_bounds_lines;
    CheckBox* m_coastal_lines;
    CheckBox* m_stream_lines;
    CheckBox* m_vortex_points;
    CheckBox* m_arrow_glyph;

public:

    RenderingObjectCheckButtonGroup( kvs::glut::Screen* screen, size_t base_x, size_t base_y )
    {
        m_label = new kvs::glut::Label( screen );
        m_bounds_lines = new CheckBox( screen, "BoundsLines" );
        m_coastal_lines = new CheckBox( screen, "CoastalLines" );
        m_stream_lines = new CheckBox( screen, "StreamLines" );
        m_vortex_points = new CheckBox( screen, "VortexPoints" );
        m_arrow_glyph = new CheckBox( screen, "ArrowGlyph" );

        m_label->setX( base_x );
        m_label->setY( base_y );
        m_label->setMargin( 5 );
        m_label->setText( "RENDERING OBJECT" );

        m_bounds_lines->setX( m_label->x() );
        m_bounds_lines->setY( m_label->y() + 20 );
        m_bounds_lines->setMargin( 10 );
        m_bounds_lines->setCaption( "Bounding Box" );
        m_bounds_lines->setState( true );

        m_coastal_lines->setX( m_bounds_lines->x() );
        m_coastal_lines->setY( m_bounds_lines->y() + 20 );
        m_coastal_lines->setMargin( 10 );
        m_coastal_lines->setCaption( "Coastal Lines" );
        m_coastal_lines->setState( true );

        m_stream_lines->setX( m_coastal_lines->x() );
        m_stream_lines->setY( m_coastal_lines->y() + 20 );
        m_stream_lines->setMargin( 10 );
        m_stream_lines->setCaption( "Stream Lines" );
        m_stream_lines->setState( true );

        m_vortex_points->setX( m_stream_lines->x() );
        m_vortex_points->setY( m_stream_lines->y() + 20 );
        m_vortex_points->setMargin( 10 );
        m_vortex_points->setCaption( "Vortex Points" );
        m_vortex_points->setState( true );

        m_arrow_glyph->setX( m_vortex_points->x() );
        m_arrow_glyph->setY( m_vortex_points->y() + 20 );
        m_arrow_glyph->setMargin( 10 );
        m_arrow_glyph->setCaption( "Arrow Glyph" );
        m_arrow_glyph->setState( false );

        m_group.add( m_bounds_lines );
        m_group.add( m_coastal_lines );
        m_group.add( m_stream_lines );
        m_group.add( m_vortex_points );
        m_group.add( m_arrow_glyph );
    }

    ~RenderingObjectCheckButtonGroup()
    {
        delete m_label;
        delete m_bounds_lines;
        delete m_coastal_lines;
        delete m_stream_lines;
        delete m_vortex_points;
        delete m_arrow_glyph;
    }

    void show()
    {
        m_label->show();
        m_bounds_lines->show();
        m_coastal_lines->show();
        m_stream_lines->show();
        m_vortex_points->show();
        m_arrow_glyph->show();
    }
};

} // end of namespace Widget


int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    kvs::GrADS* udata = new kvs::GrADS( argv[1] );
    kvs::GrADS* vdata = new kvs::GrADS( argv[2] );
    kvs::GrADS* wdata = new kvs::GrADS( argv[3] );
    kvs::GrADS* tdata = new kvs::GrADS( argv[4] );

    const float ignore_value = udata->dataDescriptor().undef().value;

    const size_t tindex = 0;
    const size_t vindex = 0;
    std::cout << "IMPORT FILE" << std::endl;
    std::cout << kvs::Indent(4) << udata->data(tindex).filename() << std::endl;
    std::cout << kvs::Indent(4) << vdata->data(tindex).filename() << std::endl;
    std::cout << kvs::Indent(4) << wdata->data(tindex).filename() << std::endl;
    std::cout << kvs::Indent(4) << tdata->data(tindex).filename() << std::endl;

    kvs::StructuredVolumeObject* volume = ISFV2014::Import( udata, vdata, wdata, tindex, vindex );
    volume->print( std::cout << std::endl << "IMPORTED VOLUME (velocity)" << std::endl, kvs::Indent(4) );

    kvs::StructuredVolumeObject* tvolume = ISFV2014::Import( tdata, tindex, vindex );
    tvolume->print( std::cout << std::endl << "IMPORTED VOLUME (temperature)" << std::endl, kvs::Indent(4) );

    delete udata;
    delete vdata;
    delete wdata;
    delete tdata;

    //const kvs::Vec3 min_range( 141, 35, -1000 ); // in (deg, deg, meter)
    const kvs::Vec3 min_range( 141, 33, -1000 ); // in (deg, deg, meter)
    const kvs::Vec3 max_range( 147, 43, 0 ); // in (deg, deg, meter)
//    const kvs::Vec3 min_range( 160, 50, -1000 ); // in (deg, deg, meter)
//    const kvs::Vec3 max_range( 180, 60, 0 ); // in (deg, deg, meter)

    kvs::StructuredVolumeObject* cropped_volume = ISFV2014::Crop( volume, min_range, max_range );
    kvs::StructuredVolumeObject* cropped_tvolume = ISFV2014::Crop( tvolume, min_range, max_range );

    const kvs::Vec3 coord_scale( 1, 1, 400 );
    const kvs::Vec3 value_scale( 1, 1, 400 );
    ISFV2014::Rescale::Coords( cropped_volume, coord_scale );
    ISFV2014::Rescale::Values( cropped_volume, value_scale, true );
    ISFV2014::Rescale::Coords( cropped_tvolume, coord_scale );

    cropped_volume->print( std::cout << std::endl << "CROPPED VOLUME (velocity)" << std::endl, kvs::Indent(4) );
    cropped_tvolume->print( std::cout << std::endl << "CROPPED VOLUME (temperature)" << std::endl, kvs::Indent(4) );
    delete volume;
    delete tvolume;


    const float p = kvs::Math::Mix( cropped_tvolume->minObjectCoord().z(), cropped_tvolume->maxObjectCoord().z(), 0.2f );
    const kvsoceanvis::util::OrthoSlice::AlignedAxis a = kvsoceanvis::util::OrthoSlice::ZAxis;
    const kvs::TransferFunction t( 256 );
    kvs::PolygonObject* ortho_slice = new kvsoceanvis::util::OrthoSlice( cropped_tvolume, p, a, t );

    ortho_slice->print( std::cout << std::endl );

    delete cropped_tvolume;
//    delete ortho_slice;



    kvs::PointObject* vortex_points = ISFV2014::Vortex( cropped_volume, ignore_value );
    vortex_points->setName( "VortexPoints" );
    vortex_points->setColor( kvs::RGBColor( 128, 128, 128 ) );
    vortex_points->print( std::cout << std::endl << "VORTEX POINTS" << std::endl, kvs::Indent(4) );

    const kvs::Vec3ui stride( 3, 3, 3 );
    kvs::StructuredVolumeObject* downsized_volume = ISFV2014::Downsize( cropped_volume, stride );
    downsized_volume->setName( "ArrowGlyph" );
    downsized_volume->print( std::cout << std::endl << "DOWNSIZED VOLUME" << std::endl, kvs::Indent(4) );

    const float delta = 1;
    kvs::PointObject* seed_points = new kvsoceanvis::util::SeedPoint( vortex_points, delta );
    seed_points->print( std::cout << std::endl << "SEED POINTS" << std::endl, kvs::Indent(4) );

    const kvs::TransferFunction transfer_function( kvs::RGBFormulae::Jet( 256 ) );
    ISFV2014::VorticalStreamline* streamline = new ISFV2014::VorticalStreamline();
    streamline->setSeedPoints( seed_points );
    streamline->setIntegrationInterval( 0.1 );
    streamline->setTransferFunction( transfer_function );
    streamline->setIntegrationMethod( ISFV2014::Streamline::RungeKutta4th );
    streamline->setVectorLengthThreshold( 0.01 );
    streamline->setIntegrationDirection( ISFV2014::Streamline::BothDirections );
    streamline->setIntegrationTimesThreshold( 20000 );
    streamline->setEnableIntegrationTimesCondition( true );
    streamline->setEnableVectorLengthCondition( true );

    kvs::LineObject* stream_lines = streamline->exec( cropped_volume );
    stream_lines->setName( "StreamLines" );
    stream_lines->setSize( 4 );
    stream_lines->print( std::cout << std::endl << "STREAMLINES" << std::endl, kvs::Indent(4) );
    delete cropped_volume;
    delete seed_points;

    kvs::glsl::LineRenderer* stream_lines_renderer = new kvs::glsl::LineRenderer();
    stream_lines_renderer->setOutlineWidth( 1.5 );

    ISFV2014::CoastlJpn coast( "../Util/coastl_jpn.csv" );
    kvs::LineObject* coastal_lines = coast.toLineObject( stream_lines->maxObjectCoord().z() );
    coastal_lines->setName( "CoastalLines" );
    coastal_lines->setSize( 2 );
    coastal_lines->setColor( kvs::RGBColor( 128, 128, 128 ) );
    coastal_lines->setMinMaxObjectCoords( stream_lines->minObjectCoord(), stream_lines->maxObjectCoord() );
    coastal_lines->setMinMaxExternalCoords( stream_lines->minExternalCoord(), stream_lines->maxExternalCoord() );

    kvs::glsl::LineRenderer* coastal_lines_renderer = new kvs::glsl::LineRenderer();

    kvs::Bounds* bounds = new kvs::Bounds();
    bounds->setLineWidth( 2.0f );
    bounds->setLineColor( kvs::RGBColor( 255, 50, 50 ) );
    kvs::LineObject* bounds_lines = bounds->outputLineObject( vortex_points );
    bounds_lines->setName( "BoundsLines" );
    delete bounds;

    kvs::SphereGlyph* sphere = new kvs::SphereGlyph();
    sphere->setNumberOfSlices( 20 );
    sphere->setNumberOfStacks( 20 );
    sphere->setScale( 20 );

    ISFV2014::ArrowGlyph* arrow = new ISFV2014::ArrowGlyph();
    const ISFV2014::ArrowGlyph::ArrowType type = ISFV2014::ArrowGlyph::TubeArrow;
    const ISFV2014::ArrowGlyph::SizeMode mode = ISFV2014::ArrowGlyph::SizeByDefault;
    arrow->setTransferFunction( transfer_function );
    arrow->setIgnoreValue( ignore_value );
    arrow->setType( type );
    arrow->setSizeMode( mode );
    arrow->setScale( 45 );

    bounds_lines->show();
    vortex_points->show();
    downsized_volume->hide();
    stream_lines->show();
    coastal_lines->show();

    kvs::glut::Screen screen( &app );
    screen.setSize( 800, 600 );
    screen.scene()->camera()->setProjectionTypeToOrthogonal();
    screen.setTitle( "Streamlines" );
    screen.registerObject( bounds_lines );
    screen.registerObject( vortex_points, sphere );
    screen.registerObject( downsized_volume, arrow );
    screen.registerObject( stream_lines, stream_lines_renderer );
    screen.registerObject( coastal_lines, coastal_lines_renderer );
    screen.registerObject( ortho_slice );
    screen.show();

    Widget::ProjectionTypeButtonGroup projection_type_group( &screen, 10, 10 );
    projection_type_group.show();

    Widget::RenderingObjectCheckButtonGroup rendering_object_group( &screen, 10, 80 );
    rendering_object_group.show();

    return app.run();
}
