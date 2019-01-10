#pragma once

#include <kvs/GlyphBase>
#include <kvs/Module>
#include <kvs/Type>
#include <kvs/OpenGL>
#include <kvs/ObjectBase>
#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/VolumeObjectBase>
#include <kvs/TransferFunction>
#include <kvs/RGBColor>


namespace
{

const kvs::Real32 LineVertices[12] =
{
     0.0f, 1.0f, 0.0f,
     0.0f, 0.0f, 0.0f,
    -0.2f, 0.8f, 0.0f,
     0.2f, 0.8f, 0.0f
};
const kvs::UInt32 LineConnections[6] =
{
    0, 1,
    0, 2,
    0, 3
};

const kvs::Vector3f ConeTranslation = kvs::Vector3f( 0.0f, 0.0f, 0.7f );
const kvs::Real32 ConeHeight = 0.3f;
const kvs::Real32 ConeRadius = 0.15f;
const size_t ConeSlices = 20;
const size_t ConeStacks = 5;

const kvs::Vector3f CylinderTranslation = kvs::Vector3f( 0.0f, 0.0f, 0.0f );
const kvs::Real32 CylinderHeight = 0.7f;
const kvs::Real32 CylinderRadius = 0.07f;
const size_t CylinderSlices = 20;
const size_t CylinderStacks = 2;

void DrawLineElement( const kvs::RGBColor& color, const kvs::UInt8 opacity )
{
    glColor4ub( color.r(), color.g(), color.b(), opacity );

    glBegin( GL_LINES );
    for ( size_t i = 0; i < 6; i++ )
    {
        const kvs::Real32* vertex = LineVertices + LineConnections[i] * 3;
        glVertex3fv( vertex );
    }
    glEnd();
}

void DrawTubeElement( GLUquadricObj* cylinder, const kvs::RGBColor& color, const kvs::UInt8 opacity )
{
    glColor4ub( color.r(), color.g(), color.b(), opacity );

    glPushMatrix();
    glRotated( -90.0, 1.0, 0.0, 0.0 );

    // Cone.
    glPushMatrix();
    glTranslatef( ConeTranslation.x(), ConeTranslation.y(), ConeTranslation.z() );
    gluCylinder( cylinder, ConeRadius, 0.0, ConeHeight, ConeSlices, ConeStacks );
    glPopMatrix();

    // Cylinder.
    glPushMatrix();
    glTranslatef( CylinderTranslation.x(), CylinderTranslation.y(), CylinderTranslation.z() );
    gluCylinder( cylinder, CylinderRadius, CylinderRadius, CylinderHeight, CylinderSlices, CylinderStacks );
    glPopMatrix();

    glPopMatrix();
}

}; // end of namespace


namespace ISFV2014
{

class ArrowGlyph : public kvs::GlyphBase
{
    kvsModule( ISFV2014::ArrowGlyph, Renderer );
    kvsModuleBaseClass( kvs::GlyphBase );

public:

    enum ArrowType
    {
        LineArrow = 0,
        TubeArrow
    };

private:

    bool m_has_ignore_value;
    float m_ignore_value; ///< ignore value
    GLUquadricObj* m_cylinder; ///< glyph primitive
    ArrowType m_type; ///< arrow type
    const kvs::VolumeObjectBase* m_volume; ///< pointer to the volume object (reference)

public:

    ArrowGlyph():
        kvs::GlyphBase(),
        m_has_ignore_value( false ),
        m_ignore_value( 0.0f ),
        m_cylinder( NULL ),
        m_type( LineArrow ),
        m_volume( NULL )
    {
    }

    virtual ~ArrowGlyph()
    {
        gluDeleteQuadric( m_cylinder );
    }

    void setIgnoreValue( const float ignore_value )
    {
        m_ignore_value = ignore_value;
        m_has_ignore_value = true;
    }

    void setType( const ArrowType type )
    {
        m_type = type;
    }

    ArrowType type() const
    {
        return m_type;
    }

    void exec( kvs::ObjectBase* object, kvs::Camera*, kvs::Light* )
    {
        const kvs::VolumeObjectBase* volume = kvs::VolumeObjectBase::DownCast( object );
        if ( !volume ) { kvsMessageError("Input object is not volume dat."); return; }
        if ( m_volume != volume ) { this->attach_volume( volume ); }

        glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );

        glEnable( GL_DEPTH_TEST );
        this->initialize();
        this->draw();
        glDisable( GL_DEPTH_TEST );

        glPopAttrib();
    }

private:

    void attach_volume( const kvs::VolumeObjectBase* volume )
    {
        m_volume = volume;

        const std::type_info& type = volume->values().typeInfo()->type();
        if ( type != typeid( kvs::Real32 ) ) { kvsMessageError("Not supported data type."); return; }

        BaseClass::calculateCoords( volume );
        BaseClass::calculateSizes<kvs::Real32>( volume );
        BaseClass::calculateDirections<kvs::Real32>( volume );
        BaseClass::calculateColors<kvs::Real32>( volume );
        BaseClass::calculateOpacities<kvs::Real32>( volume );
    }

    void draw()
    {
        if ( !m_cylinder )
        {
            m_cylinder = gluNewQuadric();
            gluQuadricDrawStyle( m_cylinder, GLU_FILL );
            gluQuadricNormals( m_cylinder, GLU_SMOOTH );
        }

        switch ( m_type )
        {
        case LineArrow: this->draw_lines(); break;
        case TubeArrow: this->draw_tubes(); break;
        default: break;
        }
    }

    void draw_lines()
    {
        const size_t npoints = BaseClass::coords().size() / 3;
        const kvs::ValueArray<kvs::Real32> coords = BaseClass::coords();
        const kvs::ValueArray<kvs::UInt8> colors = BaseClass::colors();
        const kvs::ValueArray<kvs::Real32> sizes = BaseClass::sizes();
        const kvs::ValueArray<kvs::UInt8> opacities = BaseClass::opacities();

        if ( BaseClass::directions().size() == 0 )
        {
            for ( size_t i = 0, index = 0; i < npoints; i++, index += 3 )
            {
                const kvs::Vector3f position( coords.data() + index );
                const kvs::Real32 size = sizes[i];
                const kvs::RGBColor color( colors.data() + index );
                const kvs::UInt8 opacity = opacities[i];
                glPushMatrix();
                {
                    BaseClass::transform( position, size );
                    ::DrawLineElement( color, opacity );
                }
                glPopMatrix();
            }
        }
        else
        {
            for( size_t i = 0, index = 0; i < npoints; i++, index += 3 )
            {
                const kvs::Vector3f position( coords.data() + index );
                const kvs::Vector3f direction( BaseClass::directions().data() + index );
                const kvs::Real32 size = sizes[i];
                const kvs::RGBColor color( colors.data() + index );
                const kvs::UInt8 opacity = opacities[i];

                if ( m_has_ignore_value && kvs::Math::Equal( direction.x(), m_ignore_value ) ) continue;

                if ( direction.length() > 0.0f )
                {
                    glPushMatrix();
                    {
                        BaseClass::transform( position, direction, size );
                        ::DrawLineElement( color, opacity );
                    }
                    glPopMatrix();
                }
            }
        }
    }

    void draw_tubes()
    {
        const size_t npoints = BaseClass::coords().size() / 3;
        const kvs::ValueArray<kvs::Real32> coords = BaseClass::coords();
        const kvs::ValueArray<kvs::UInt8> colors = BaseClass::colors();
        const kvs::ValueArray<kvs::Real32> sizes = BaseClass::sizes();
        const kvs::ValueArray<kvs::UInt8> opacities = BaseClass::opacities();

        if ( BaseClass::directions().size() == 0 )
        {
            for ( size_t i = 0, index = 0; i < npoints; i++, index += 3 )
            {
                const kvs::Vector3f position( coords.data() + index );
                const kvs::Real32 size = sizes[i];
                const kvs::RGBColor color( colors.data() + index );
                const kvs::UInt8 opacity = opacities[i];
                glPushMatrix();
                {
                    BaseClass::transform( position, size );
                    ::DrawTubeElement( m_cylinder, color, opacity );
                }
                glPopMatrix();
            }
        }
        else
        {
            for ( size_t i = 0, index = 0; i < npoints; i++, index += 3 )
            {
                const kvs::Vector3f position( coords.data() + index );
                const kvs::Vector3f direction( BaseClass::directions().data() + index );
                const kvs::Real32 size = sizes[i];
                const kvs::RGBColor color( colors.data() + index );
                const kvs::UInt8 opacity = opacities[i];

                if ( m_has_ignore_value && kvs::Math::Equal( direction.x(), m_ignore_value ) ) continue;

                if ( direction.length() > 0.0f )
                {
                    glPushMatrix();
                    {
                        BaseClass::transform( position, direction, size );
                        ::DrawTubeElement( m_cylinder, color, opacity );
                    }
                    glPopMatrix();
                }
            }
        }
    }

    void initialize()
    {
        glDisable( GL_LINE_SMOOTH );

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        glShadeModel( GL_SMOOTH );

        glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
        glEnable( GL_COLOR_MATERIAL );

        if ( m_type == ArrowGlyph::LineArrow )
        {
            glDisable( GL_NORMALIZE );
            glDisable( GL_LIGHTING );
        }
        else
        {
            if ( !BaseClass::isEnabledShading() )
            {
                glDisable( GL_NORMALIZE );
                glDisable( GL_LIGHTING );
            }
            else
            {
                glEnable( GL_NORMALIZE );
                glEnable( GL_LIGHTING );
            }
        }

        glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
    }
};

} // end of namespace ISFV2014
