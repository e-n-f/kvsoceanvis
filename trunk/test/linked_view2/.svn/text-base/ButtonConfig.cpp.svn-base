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

void MouseProcess(const int x, const int y, const int action, const kvs::Vector2f objmin, const kvs::Vector2f objmax, kvs::glut::Screen* screen, const std::string object_name, UpdatePR* pr_updator)
{
    static int x0, x1, y0, y1;
    if (action == int(kvs::MouseButton::Pressed))
    {
        x0 = x;
        y0 = y;
    }
    else if (action == int(kvs::MouseButton::Released))
    {
        x1 = x;
        y1 = y;
        GetPosition(x0, y0, x1, y1, objmin, objmax, screen, object_name, pr_updator);
    }
}

kvs::Vector3d GetObjectPosition3( int x, int y, double depth, kvs::glut::Screen* screen, kvs::ObjectBase* object )
{
    glPushMatrix();
    screen->camera()->update();
    kvs::ObjectManager* om = screen->objectManager();
    object->transform( om->objectCenter(), om->normalize() );

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];

    glGetIntegerv( GL_VIEWPORT, viewport );
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );

    GLdouble posX0 = 0.0, posY0 = 0.0, posZ0 = 0.0;
    GLdouble winx = GLdouble( x );
    GLdouble winy = GLdouble( screen->height() - y );
    gluUnProject( winx, winy, depth, modelview, projection, viewport, &posX0, &posY0, &posZ0 );

    glPopMatrix();

    return kvs::Vector3d( posX0, posY0, posZ0 );
}

kvs::Vector2f GetObjectPosition2( int x, int y, kvs::glut::Screen* screen, const std::string& object_name )
{
    kvs::ObjectManager* om = screen->objectManager();
    kvs::ObjectBase* obj = om->object( object_name );
    if ( obj == NULL )
    { std::cout << "Null Object" << std::endl; }

    kvs::PolygonObject* poly = dynamic_cast<kvs::PolygonObject*>( obj );
    if ( poly == NULL )
    { std::cout << "Not a name of a polygon object" << std::endl; }

    const double obj_z = poly->coords().at( 2 );
    const kvs::Vector3d front = GetObjectPosition3( x, y, 0.0, screen, obj );
    const kvs::Vector3d back = GetObjectPosition3( x, y, 1.0, screen, obj );
    const kvs::Vector3d dir = ( back - front ).normalize();

    const double t = ( obj_z - front.z() ) / dir.z();
    const kvs::Vector3d cross_point = front + t * dir;

    return kvs::Vector2f( float( cross_point.x() ), float( cross_point.y() ) );
}

void GetPosition(const int x0, const int y0, const int x1, const int y1, const kvs::Vector2f objmin, const kvs::Vector2f objmax, kvs::glut::Screen* screen, const std::string object_name, UpdatePR* pr_updator)
{
    kvs::Vector2f p0 = GetObjectPosition2(x0, y0, screen, object_name);
    kvs::Vector2f p1 = GetObjectPosition2(x1, y1, screen, object_name);

    kvs::ObjectManager* om = screen->objectManager();
    kvs::ObjectBase* obj = om->object( object_name );
    if ( obj == NULL )
    { std::cout << "Null Object" << std::endl; }

    kvs::PolygonObject* poly = dynamic_cast<kvs::PolygonObject*>( obj );
    if ( poly == NULL )
    { std::cout << "Not a name of a polygon object" << std::endl; }

    kvs::Vector3f omin = poly->minObjectCoord();
    kvs::Vector3f omax = poly->maxObjectCoord();

    float objx0 = (p0.x() < p1.x()) ? p0.x() : p1.x();
    float objy0 = (p0.y() < p1.y()) ? p0.y() : p1.y();
    float objx1 = (p0.x() > p1.x()) ? p0.x() : p1.x();
    float objy1 = (p0.y() > p1.y()) ? p0.y() : p1.y();

    float xfactor = (objmax.x() - objmin.x())/(omax.x() - omin.x());
    float yfactor = (objmax.y() - objmin.y())/(omax.y() - omin.y());

    PosValue::setRealRange(objmin.x(), objmin.y(), objmax.x(), objmax.y());
    PosValue::setObjRange(omin.x(), omin.y(), omax.x(), omax.y());
    PosValue::setObjPos(objx0, objy0, objx1, objy1);
    int wx0 = (x0 < x1) ? x0 : x1;
    int wy0 = (y0 < y1) ? y0 : y1;
    int wx1 = (x0 > x1) ? x0 : x1;
    int wy1 = (y0 > y1) ? y0 : y1;
    PosValue::setWinPos(wx0, wy0, wx1, wy1);
    PosValue::setWinRange(0, 0, screen->width(), screen->height());

    if(objx0 >= omin.x() && objy0 >= omin.y() && objx1 <= omax.x() && objy1 <= omax.y())
    {
        float px0 = (objx0 - omin.x()) * xfactor + objmin.x();
        float py0 = (objy0 - omin.y()) * yfactor + objmin.y();
        float px1 = (objx1 - omin.x()) * xfactor + objmin.x();
        float py1 = (objy1 - omin.y()) * yfactor + objmin.y();
        std::cout << "( " << px0 << ", " << py0 << " ) -- ( " << px1 << ", " << py1 << " )" << std::endl;
        pr_updator->setPosValue(px0, py0, px1, py1);
        PosValue::setRealPos(px0, py0, px1, py1);
        PosValue::setPaintFlag(true);
        screen->redraw();
    }
    else
    {
        std::cout << "Out of range. Please select again." << std::endl;
    }
}

MousePress::MousePress(kvs::glut::Screen* screen, std::string object_name, kvs::Vector2f objmin, kvs::Vector2f objmax, UpdatePR* pr_updator)
{
    m_screen = screen;
    m_object_name = object_name;
    m_objmin = objmin;
    m_objmax = objmax;
    m_pr_updator = pr_updator;
}

void MousePress::update( kvs::MouseEvent* event )
{
    if (event->button() == int(kvs::MouseButton::Left))
    {
        MouseProcess(event->x(), event->y(), event->action(), m_objmin, m_objmax, m_screen, m_object_name, m_pr_updator);
        PosValue::setPaintFlag(false);
        PosValue::temp[0] = event->x();
        PosValue::temp[1] = event->y();
    }
    else if ( event->button() == int(kvs::MouseButton::Right) )
    {
        if ( !m_screen->isActiveMove( event->x(), event->y() ) ) return;

        m_screen->mousePressFunction( event->x(), event->y(), kvs::Mouse::Translation );
        PosValue::setPaintFlag(false);
        m_screen->redraw();
    }
}

MouseRelease::MouseRelease(kvs::glut::Screen* screen, std::string object_name, kvs::Vector2f objmin, kvs::Vector2f objmax, UpdatePR* pr_updator)
{
    m_screen = screen;
    m_object_name = object_name;
    m_objmin = objmin;
    m_objmax = objmax;
    m_pr_updator = pr_updator;
}

void MouseRelease::update( kvs::MouseEvent* event )
{
    if (event->button() == int(kvs::MouseButton::Left))
    {
        MouseProcess(event->x(), event->y(), event->action(), m_objmin, m_objmax, m_screen, m_object_name, m_pr_updator);
    }
    else if ( event->button() == int(kvs::MouseButton::Right) )
    {
        m_screen->mouseReleaseFunction( event->x(), event->y() );
        m_screen->redraw();
    }
}

MouseMove::MouseMove(kvs::glut::Screen* screen)
{
    m_screen = screen;
}

void MouseMove::update( kvs::MouseEvent* event )
{
    if (event->button() == int(kvs::MouseButton::Left))
    {
        PosValue::temp[2] = event->x();
        PosValue::temp[3] = event->y();
        PosValue::sortValue();
        PosValue::setPaintFlag(true);
        m_screen->redraw();
    }
    else if (event->button() == int(kvs::MouseButton::Right))
    {
        m_screen->mouseMoveFunction(event->x(), event->y());
        m_screen->redraw();
    }
}

void WheelScroll::update( kvs::WheelEvent* event )
{
    PosValue::setPaintFlag(false);
}

UpdatePR::UpdatePR( kvs::glut::Screen* screen, kvs::glut::Screen* screen2, std::string object_name, std::string object_name2, std::string outputfile, kvs::TableObject* reftable, kvs::Vector3i posidx, kvs::Vector3i vindices, bool hidepos, bool hideuvw )
{
    m_screen = screen;
    m_screen2 = screen2;
    m_object_name = object_name;
    m_object_name2 = object_name2;
    m_outputfile = outputfile;
    m_reftable = reftable;
    m_posidx = posidx;
    m_vindices = vindices;
    m_hidepos = hidepos;
    m_hideuvw = hideuvw;
}

void UpdatePR::setPosValue(const float xmin, const float ymin, const float xmax, const float ymax)
{
    m_pos_v0.set(xmin, ymin);
    m_pos_v1.set(xmax, ymax);
}

void UpdatePR::update( kvs::KeyEvent* event )
{
    switch ( event->key() )
    {
        case kvs::Key::p:
            {
                size_t nc = m_reftable->ncolumns();
                std::vector<float> lim(nc*2);
                lim[m_posidx.x()*2] = m_pos_v0.x();
                lim[m_posidx.x()*2+1] = m_pos_v1.x();
                lim[m_posidx.y()*2] = m_pos_v0.y();
                lim[m_posidx.y()*2+1] = m_pos_v1.y();
                for (size_t i=0; i<nc; i++)
                {
                    if (i != size_t(m_posidx.x()) && i != size_t(m_posidx.y()))
                    {
                        lim[i*2] = -9999;
                        lim[i*2+1] = 9999;
                    }
                }

                tof::TabFilter tfilter;
                tfilter.exec(m_reftable, lim, m_outputfile);
                kvs::TableObject* tableout = new kvs::TableImporter( m_outputfile );

                kvs::TableObject* table = new kvs::TableObject();
                if (m_hidepos == true && m_hideuvw == false)
                {
                    for (size_t i=0; i<tableout->ncolumns(); i++)
                    {
                        if (i != size_t(m_posidx.x()) && i != size_t(m_posidx.y()) && i != size_t(m_posidx.z()))
                        {
                            table->addColumn(tableout->column(i), tableout->label(i));
                        }
                    }
                }
                else if(m_hidepos == false && m_hideuvw == true)
                {
                    for (size_t i=0; i<tableout->ncolumns(); i++)
                    {
                        if (i != size_t(m_vindices.x()) && i != size_t(m_vindices.y()) && i != size_t(m_vindices.z()))
                        {
                            table->addColumn(tableout->column(i), tableout->label(i));
                        }
                    }
                }
                else if(m_hidepos == true && m_hideuvw == true)
                {
                    for (size_t i=0; i<tableout->ncolumns(); i++)
                    {
                        if (i != size_t(m_posidx.x()) && i != size_t(m_posidx.y()) && i != size_t(m_posidx.z()) && i != size_t(m_vindices.x()) && i != size_t(m_vindices.y()) && i != size_t(m_vindices.z()))
                        {
                            table->addColumn(tableout->column(i), tableout->label(i));
                        }
                    }
                }
                else
                {
                    for (size_t i=0; i<tableout->ncolumns(); i++)
                    {
                        table->addColumn(tableout->column(i), tableout->label(i));
                    }
                }
                kvs::TableObject* table2 = new kvs::TableObject();
                for (size_t i=0; i<table->ncolumns(); i++)
                {
                    table2->addColumn(table->column(i), table->label(i));
                }
                m_screen->objectManager()->change(m_object_name, table, true);
                m_screen2->objectManager()->change(m_object_name2, table2, true);
                table->setName( m_object_name );
                table2->setName( m_object_name2 );
                m_screen->redraw();
                m_screen2->redraw();
                break;
            }

        default: break;
    }
}

kvs::Vector2f PosValue::m_real_minrange;
kvs::Vector2f PosValue::m_real_maxrange;
kvs::Vector2f PosValue::m_real_minpos;
kvs::Vector2f PosValue::m_real_maxpos;
kvs::Vector2f PosValue::m_obj_minrange;
kvs::Vector2f PosValue::m_obj_maxrange;
kvs::Vector2f PosValue::m_obj_minpos;
kvs::Vector2f PosValue::m_obj_maxpos;
kvs::Vector2i PosValue::m_win_minrange;
kvs::Vector2i PosValue::m_win_maxrange;
kvs::Vector2i PosValue::m_win_minpos;
kvs::Vector2i PosValue::m_win_maxpos;
bool PosValue::m_paintflag;
std::vector<int> PosValue::temp(4);

void PosValue::setRealRange(const float xmin, const float ymin, const float xmax, const float ymax)
{
    m_real_minrange.set(xmin, ymin);
    m_real_maxrange.set(xmax, ymax);
}

void PosValue::setRealPos(const float xmin, const float ymin, const float xmax, const float ymax)
{
    m_real_minpos.set(xmin, ymin);
    m_real_maxpos.set(xmax, ymax);
}

void PosValue::setObjRange(const float xmin, const float ymin, const float xmax, const float ymax)
{
    m_obj_minrange.set(xmin, ymin);
    m_obj_maxrange.set(xmax, ymax);
}

void PosValue::setObjPos(const float xmin, const float ymin, const float xmax, const float ymax)
{
    m_obj_minpos.set(xmin, ymin);
    m_obj_maxpos.set(xmax, ymax);
}

void PosValue::setWinRange(const int xmin, const int ymin, const int xmax, const int ymax)
{
    m_win_minrange.set(xmin, ymin);
    m_win_maxrange.set(xmax, ymax);
}

void PosValue::setWinPos(const int xmin, const int ymin, const int xmax, const int ymax)
{
    m_win_minpos.set(xmin, ymin);
    m_win_maxpos.set(xmax, ymax);
}

void PosValue::setPaintFlag(const bool paintflag)
{
    m_paintflag = paintflag;
}

void PosValue::getRealRange(float* xmin, float* ymin, float* xmax, float* ymax)
{
    *xmin = m_real_minrange.x();
    *ymin = m_real_minrange.y();
    *xmax = m_real_maxrange.x();
    *ymax = m_real_maxrange.y();
}

void PosValue::getRealPos(float* xmin, float* ymin, float* xmax, float* ymax)
{
    *xmin = m_real_minpos.x();
    *ymin = m_real_minpos.y();
    *xmax = m_real_maxpos.x();
    *ymax = m_real_maxpos.y();
}

void PosValue::getObjRange(float* xmin, float* ymin, float* xmax, float* ymax)
{
    *xmin = m_obj_minrange.x();
    *ymin = m_obj_minrange.y();
    *xmax = m_obj_maxrange.x();
    *ymax = m_obj_maxrange.y();
}

void PosValue::getObjPos(float* xmin, float* ymin, float* xmax, float* ymax)
{
    *xmin = m_obj_minpos.x();
    *ymin = m_obj_minpos.y();
    *xmax = m_obj_maxpos.x();
    *ymax = m_obj_maxpos.y();
}

void PosValue::getWinRange(int* xmin, int* ymin, int* xmax, int* ymax)
{
    *xmin = m_win_minrange.x();
    *ymin = m_win_minrange.y();
    *xmax = m_win_maxrange.x();
    *ymax = m_win_maxrange.y();
}

void PosValue::getWinPos(int* xmin, int* ymin, int* xmax, int* ymax)
{
    *xmin = m_win_minpos.x();
    *ymin = m_win_minpos.y();
    *xmax = m_win_maxpos.x();
    *ymax = m_win_maxpos.y();
}

bool PosValue::getPaintFlag()
{
    return m_paintflag;
}

void PosValue::sortValue()
{
    int x0 = PosValue::temp[0];
    int y0 = PosValue::temp[1];
    int x1 = PosValue::temp[2];
    int y1 = PosValue::temp[3];
    int wx0 = (x0 < x1) ? x0 : x1;
    int wy0 = (y0 < y1) ? y0 : y1;
    int wx1 = (x0 > x1) ? x0 : x1;
    int wy1 = (y0 > y1) ? y0 : y1;
    PosValue::setWinPos(wx0, wy0, wx1, wy1);
}

void PaintEvent::update()
{
    if (PosValue::getPaintFlag() == true)
    {
        int x0, y0, x1, y1;
        float cx0, cy0, cx1, cy1;
        int wx0, wy0, wx1, wy1;
        PosValue::getWinPos(&x0, &y0, &x1, &y1);
        PosValue::getWinRange(&wx0, &wy0, &wx1, &wy1);
        cx0 = (x0 - (wx1 - wx0)/2.0f)/((wx1 - wx0)/2.0f);
        cy0 = -(y0 - (wy1 - wy0)/2.0f)/((wy1 - wy0)/2.0f);
        cx1 = (x1 - (wx1 - wx0)/2.0f)/((wx1 - wx0)/2.0f);
        cy1 = -(y1 - (wy1 - wy0)/2.0f)/((wy1 - wy0)/2.0f);
        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        glBegin(GL_LINE_LOOP);
        glColor3ub( 128, 128, 128 );
        glVertex2f( cx0, cy0 );
        glVertex2f( cx0, cy1 );
        glVertex2f( cx1, cy1 );
        glVertex2f( cx1, cy0 );
        glEnd();
        glEnable(GL_DEPTH_TEST);
        glPopMatrix();
        glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
    }
}

UpdatePCP::UpdatePCP()
{
    //null
}

void UpdatePCP::setScreen(kvs::glut::Screen* pcpscreen, kvs::glut::Screen* vrscreen, kvs::glut::Screen* spmscreen)
{
    m_pcpscreen = pcpscreen;
    m_vrscreen = vrscreen;
    m_spmscreen = spmscreen;
}

void UpdatePCP::setObject(const std::string tabobject_name, const std::string volobject_name, const std::string lineobject_name, const std::string tabobject2_name, kvs::TableObject* reftable)
{
    m_tabobject_name = tabobject_name;
    m_volobject_name = volobject_name;
    m_lineobject_name = lineobject_name;
    m_tabobject2_name = tabobject2_name;
    m_reftable = reftable;
}

void UpdatePCP::setRenderer(const std::string pcprenderer_name)
{
    m_pcprenderer_name = pcprenderer_name;
}

void UpdatePCP::setIndex(const kvs::Vector3i posidx, const kvs::Vector3i vindices)
{
    m_posidx = posidx;
    m_vindices = vindices;
}

void UpdatePCP::setBool(const bool inverse, const bool hidepos, const bool hideuvw)
{
    m_inverse = inverse;
    m_hidepos = hidepos;
    m_hideuvw = hideuvw;
}

void UpdatePCP::setFile(const std::string tableout, const std::string volumeout, const std::string volumeout3, const std::string ivreffile)
{
    m_tableout = tableout;
    m_volumeout = volumeout;
    m_volumeout3 = volumeout3;
    m_ivreffile = ivreffile;
}

void UpdatePCP::update( kvs::KeyEvent* event )
{
    switch ( event->key() )
    {
        case kvs::Key::z:
            {
                kvs::TableObject* object = static_cast<kvs::TableObject*>( m_pcpscreen->objectManager()->object( m_tabobject_name ) );
                kvs::ParallelCoordinatesRenderer* renderer = static_cast<kvs::ParallelCoordinatesRenderer*>( m_pcpscreen->rendererManager()->renderer( m_pcprenderer_name ) );

                object->setMinValue( renderer->activeAxis(), object->minRange(renderer->activeAxis()) );
                object->setMaxValue( renderer->activeAxis(), object->maxRange(renderer->activeAxis()) );
                m_pcpscreen->redraw();
                break;
            }

        case kvs::Key::i:
            {
                kvs::TableObject* object = static_cast<kvs::TableObject*>( m_pcpscreen->objectManager()->object( m_tabobject_name ) );
                kvs::ParallelCoordinatesRenderer* renderer = static_cast<kvs::ParallelCoordinatesRenderer*>( m_pcpscreen->rendererManager()->renderer( m_pcprenderer_name ) );

                float min = object->minRange(renderer->activeAxis());
                float max = object->maxRange(renderer->activeAxis());
                std::cout << "Min: " << min << ". Max: " << max << "." << std::endl;
                break;
            }

        case kvs::Key::r:
            {
                kvs::TableObject* object = static_cast<kvs::TableObject*>( m_pcpscreen->objectManager()->object( m_tabobject_name ) );
                kvs::ParallelCoordinatesRenderer* renderer = static_cast<kvs::ParallelCoordinatesRenderer*>( m_pcpscreen->rendererManager()->renderer( m_pcprenderer_name ) );
                size_t nr = object->nrows();
                kvs::ValueArray<float> alpha;
                alpha = object->column(renderer->activeAxis()).asValueArray<float>();
                float min = alpha[0];
                float max = alpha[0];
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
                object->setMinValue( renderer->activeAxis(), min );
                object->setMaxValue( renderer->activeAxis(), max );
                object->setMinRange( renderer->activeAxis(), min );
                object->setMaxRange( renderer->activeAxis(), max );
                m_pcpscreen->redraw();
                break;
            }

        case kvs::Key::m:
            {
                kvs::TableObject* object = static_cast<kvs::TableObject*>( m_pcpscreen->objectManager()->object( m_tabobject_name ) );
                kvs::TableObject* object2 = static_cast<kvs::TableObject*>( m_spmscreen->objectManager()->object( m_tabobject2_name ) );
                assert(object->ncolumns() == object2->ncolumns());
                size_t nc = object->ncolumns();
                for (size_t i=0; i<nc; i++)
                {
                    float min = object->minRange(i);
                    float max = object->maxRange(i);
                    object2->setRange(i, min, max);
                }
                m_spmscreen->redraw();
                break;
            }

        case kvs::Key::l:
            {
                kvs::ParallelCoordinatesRenderer* renderer = static_cast<kvs::ParallelCoordinatesRenderer*>( m_pcpscreen->rendererManager()->renderer( m_pcprenderer_name ) );
                if (m_hidepos == false)
                {
                    if (renderer->activeAxis() == size_t(m_posidx.x()) || renderer->activeAxis() == size_t(m_posidx.y()) || renderer->activeAxis() == size_t(m_posidx.z()))
                    {
                        std::cout << "Please check the selected axis." << std::endl;
                        break;
                    }
                }

                float xmin, ymin, xmax, ymax;
                PosValue::getRealPos(&xmin, &ymin, &xmax, &ymax);
                size_t nc = m_reftable->ncolumns();
                std::vector<float> lim(nc*2);
                lim[m_posidx.x()*2] = xmin;
                lim[m_posidx.x()*2+1] = xmax;
                lim[m_posidx.y()*2] = ymin;
                lim[m_posidx.y()*2+1] = ymax;
                for (size_t i=0; i<nc; i++)
                {
                    if (i != size_t(m_posidx.x()) && i != size_t(m_posidx.y()))
                    {
                        lim[i*2] = -9999;
                        lim[i*2+1] = 9999;
                    }
                }

                tof::TabFilter tfilter;
                tfilter.exec(m_reftable, lim, m_tableout);
                kvs::TableObject* tableout = new kvs::TableImporter( m_tableout );

                ttv::TVConv ttvolume;
                if (m_hidepos == true && m_hideuvw == false)
                {
                    ttvolume.exec(tableout, m_posidx, 1, renderer->activeAxis()+3, m_vindices, m_inverse, m_volumeout);
                    ttvolume.exec(tableout, m_posidx, 3, renderer->activeAxis()+3, m_vindices, m_inverse, m_volumeout3);
                }
                else if (m_hidepos == false && m_hideuvw == true)
                {
                    ttvolume.exec(tableout, m_posidx, 1, renderer->activeAxis()+3, m_vindices, m_inverse, m_volumeout);
                    ttvolume.exec(tableout, m_posidx, 3, renderer->activeAxis()+3, m_vindices, m_inverse, m_volumeout3);
                }
                else if (m_hidepos == true && m_hideuvw == true)
                {
                    ttvolume.exec(tableout, m_posidx, 1, renderer->activeAxis()+6, m_vindices, m_inverse, m_volumeout);
                    ttvolume.exec(tableout, m_posidx, 3, renderer->activeAxis()+6, m_vindices, m_inverse, m_volumeout3);
                }
                else
                {
                    ttvolume.exec(tableout, m_posidx, 1, renderer->activeAxis(), m_vindices, m_inverse, m_volumeout);
                    ttvolume.exec(tableout, m_posidx, 3, renderer->activeAxis(), m_vindices, m_inverse, m_volumeout3);
                }
                kvs::StructuredVolumeObject* volumeout = new kvs::StructuredVolumeImporter( m_volumeout );
                kvs::StructuredVolumeObject* volumeout3 = new kvs::StructuredVolumeImporter( m_volumeout3 );
                delete tableout;

                kvs::GrADS ivref(m_ivreffile);
                const float ignore_value = ivref.dataDescriptor().undef().value;
                kvs::UnstructuredVolumeObject* tensor = new kvsoceanvis::util::VortexPointVolume( volumeout3, ignore_value );
                kvs::PointObject* cpoint = new kvsoceanvis::util::VortexPoint( tensor );
                cpoint->setNormals( kvs::ValueArray<kvs::Real32>(0) ); // delete normal vectors
                delete tensor;
                const float delta = 0.5;
                kvs::PointObject* seed_point = new kvsoceanvis::util::SeedPoint( cpoint, delta );
                delete cpoint;

                kvs::ColorMap cmap( 256 );
                cmap.create();

                kvs::Streamline* streamline = new kvs::Streamline();
                streamline->setSeedPoints( seed_point );
                streamline->setIntegrationInterval( 0.2 );
                streamline->setIntegrationMethod( kvs::Streamline::RungeKutta2nd );
                streamline->setVectorLengthThreshold( 0.2 );
                streamline->setIntegrationDirection( kvs::Streamline::BothDirections );
                streamline->setIntegrationTimesThreshold( 100 );
                streamline->setEnableIntegrationTimesCondition( true );
                streamline->setEnableVectorLengthCondition( false );
                streamline->setColorMap( cmap );
                kvs::LineObject* line;
                line = streamline->exec( volumeout3 );
                delete seed_point;
                delete volumeout3;

                m_vrscreen->objectManager()->change(m_volobject_name, volumeout, true);
                volumeout->setName( m_volobject_name );
                m_vrscreen->objectManager()->change(m_lineobject_name, line, true);
                line->setName( m_lineobject_name );
                m_vrscreen->redraw();

                break;
            }

        default: break;
    }
}
