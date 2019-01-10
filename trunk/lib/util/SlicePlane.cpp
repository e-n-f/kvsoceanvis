/*****************************************************************************/
/**
 *  @file   SlicePlane.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: SlicePlane.cpp 391 2014-07-24 07:42:56Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "SlicePlane.h"
#include <kvs/MarchingCubesTable>


namespace kvsoceanvis
{

namespace util
{

SlicePlane::SlicePlane(
    const kvs::VolumeObjectBase* volume,
    const kvs::Vector4f&         coefficients,
    const kvs::TransferFunction& transfer_function ):
    kvs::MapperBase( transfer_function ),
    kvs::PolygonObject()
{
    this->setPlane( coefficients );
    this->exec( volume );
}

SlicePlane::SlicePlane(
    const kvs::VolumeObjectBase* volume,
    const kvs::Vector3f&         point,
    const kvs::Vector3f&         normal,
    const kvs::TransferFunction& transfer_function ):
    kvs::MapperBase( transfer_function ),
    kvs::PolygonObject()
{
    this->setPlane( point, normal );
    this->exec( volume );
}

void SlicePlane::setPlane( const kvs::Vector4f& coefficients )
{
    m_coefficients = coefficients;
}

void SlicePlane::setPlane( const kvs::Vector3f& point, const kvs::Vector3f& normal )
{
    m_coefficients = kvs::Vector4f( normal, -point.dot( normal ) );
}

SlicePlane::SuperClass* SlicePlane::exec( const kvs::ObjectBase* object )
{
    const kvs::StructuredVolumeObject* volume = kvs::StructuredVolumeObject::DownCast( object );
    const kvs::Real32* xcoords = volume->coords().data();
    const kvs::Real32* ycoords = xcoords + volume->resolution().x();
    const kvs::Real32* zcoords = ycoords + volume->resolution().y();

    // Attach the pointer to the volume object.
    BaseClass::attachVolume( volume );
    BaseClass::setRange( volume );
    BaseClass::setMinMaxCoords( volume, this );

    // Calculated the coordinate data array and the normal vector array.
    std::vector<kvs::Real32> coords;
    std::vector<kvs::Real32> normals;
    std::vector<kvs::UInt8>  colors;

    // Calculate min/max values of the node data.
    if ( !volume->hasMinMaxValues() )
    {
        volume->updateMinMaxValues();
    }

    // Calculate a normalize_factor.
    const kvs::Real64 min_value( volume->minValue() );
    const kvs::Real64 max_value( volume->maxValue() );
    const kvs::Real64 normalize_factor( 255.0 / ( max_value - min_value ) );

    const kvs::Vector3ui ncells( volume->resolution() - kvs::Vector3ui::All(1) );
    const kvs::UInt32    line_size( volume->numberOfNodesPerLine() );
    const kvs::ColorMap& color_map( BaseClass::transferFunction().colorMap() );

    // Extract surfaces.
    size_t index = 0;
    for ( kvs::UInt32 z = 0; z < ncells.z(); ++z )
    {
        for ( kvs::UInt32 y = 0; y < ncells.y(); ++y )
        {
            for ( kvs::UInt32 x = 0; x < ncells.x(); ++x )
            {
                const kvs::Real32 x0 = xcoords[x];
                const kvs::Real32 y0 = ycoords[y];
                const kvs::Real32 z0 = zcoords[z];
                const kvs::Real32 xdiff = xcoords[x+1] - xcoords[x];
                const kvs::Real32 ydiff = ycoords[y+1] - ycoords[y];
                const kvs::Real32 zdiff = zcoords[z+1] - zcoords[z];

                // Calculate the index of the reference table.
                const size_t table_index = this->calculate_table_index( x0, y0, z0, xdiff, ydiff, zdiff );
                if ( table_index == 0 ) continue;
                if ( table_index == 255 ) continue;

                // Calculate the triangle polygons.
                for ( size_t i = 0; kvs::MarchingCubesTable::TriangleID[ table_index ][i] != -1; i += 3 )
                {
                    // Refer the edge IDs from the TriangleTable by using the table_index.
                    const int e0 = kvs::MarchingCubesTable::TriangleID[table_index][i];
                    const int e1 = kvs::MarchingCubesTable::TriangleID[table_index][i+2];
                    const int e2 = kvs::MarchingCubesTable::TriangleID[table_index][i+1];

                    // Determine vertices for each edge.
                    const kvs::Vector3f i0(
                        static_cast<float>( x + kvs::MarchingCubesTable::VertexID[e0][0][0] ),
                        static_cast<float>( y + kvs::MarchingCubesTable::VertexID[e0][0][1] ),
                        static_cast<float>( z + kvs::MarchingCubesTable::VertexID[e0][0][2] ) );

                    const kvs::Vector3f i1(
                        static_cast<float>( x + kvs::MarchingCubesTable::VertexID[e0][1][0] ),
                        static_cast<float>( y + kvs::MarchingCubesTable::VertexID[e0][1][1] ),
                        static_cast<float>( z + kvs::MarchingCubesTable::VertexID[e0][1][2] ) );

                    const kvs::Vector3f i2(
                        static_cast<float>( x + kvs::MarchingCubesTable::VertexID[e1][0][0] ),
                        static_cast<float>( y + kvs::MarchingCubesTable::VertexID[e1][0][1] ),
                        static_cast<float>( z + kvs::MarchingCubesTable::VertexID[e1][0][2] ) );

                    const kvs::Vector3f i3(
                        static_cast<float>( x + kvs::MarchingCubesTable::VertexID[e1][1][0] ),
                        static_cast<float>( y + kvs::MarchingCubesTable::VertexID[e1][1][1] ),
                        static_cast<float>( z + kvs::MarchingCubesTable::VertexID[e1][1][2] ) );

                    const kvs::Vector3f i4(
                        static_cast<float>( x + kvs::MarchingCubesTable::VertexID[e2][0][0] ),
                        static_cast<float>( y + kvs::MarchingCubesTable::VertexID[e2][0][1] ),
                        static_cast<float>( z + kvs::MarchingCubesTable::VertexID[e2][0][2] ) );

                    const kvs::Vector3f i5(
                        static_cast<float>( x + kvs::MarchingCubesTable::VertexID[e2][1][0] ),
                        static_cast<float>( y + kvs::MarchingCubesTable::VertexID[e2][1][1] ),
                        static_cast<float>( z + kvs::MarchingCubesTable::VertexID[e2][1][2] ) );


                    const kvs::Vector3f v0(
                        static_cast<float>( x0 + xdiff * kvs::MarchingCubesTable::VertexID[e0][0][0] ),
                        static_cast<float>( y0 + ydiff * kvs::MarchingCubesTable::VertexID[e0][0][1] ),
                        static_cast<float>( z0 + zdiff * kvs::MarchingCubesTable::VertexID[e0][0][2] ) );

                    const kvs::Vector3f v1(
                        static_cast<float>( x0 + xdiff * kvs::MarchingCubesTable::VertexID[e0][1][0] ),
                        static_cast<float>( y0 + ydiff * kvs::MarchingCubesTable::VertexID[e0][1][1] ),
                        static_cast<float>( z0 + zdiff * kvs::MarchingCubesTable::VertexID[e0][1][2] ) );

                    const kvs::Vector3f v2(
                        static_cast<float>( x0 + xdiff * kvs::MarchingCubesTable::VertexID[e1][0][0] ),
                        static_cast<float>( y0 + ydiff * kvs::MarchingCubesTable::VertexID[e1][0][1] ),
                        static_cast<float>( z0 + zdiff * kvs::MarchingCubesTable::VertexID[e1][0][2] ) );

                    const kvs::Vector3f v3(
                        static_cast<float>( x0 + xdiff * kvs::MarchingCubesTable::VertexID[e1][1][0] ),
                        static_cast<float>( y0 + ydiff * kvs::MarchingCubesTable::VertexID[e1][1][1] ),
                        static_cast<float>( z0 + zdiff * kvs::MarchingCubesTable::VertexID[e1][1][2] ) );

                    const kvs::Vector3f v4(
                        static_cast<float>( x0 + xdiff * kvs::MarchingCubesTable::VertexID[e2][0][0] ),
                        static_cast<float>( y0 + ydiff * kvs::MarchingCubesTable::VertexID[e2][0][1] ),
                        static_cast<float>( z0 + zdiff * kvs::MarchingCubesTable::VertexID[e2][0][2] ) );

                    const kvs::Vector3f v5(
                        static_cast<float>( x0 + xdiff * kvs::MarchingCubesTable::VertexID[e2][1][0] ),
                        static_cast<float>( y0 + ydiff * kvs::MarchingCubesTable::VertexID[e2][1][1] ),
                        static_cast<float>( z0 + zdiff * kvs::MarchingCubesTable::VertexID[e2][1][2] ) );

                    // Calculate coordinates of the vertices which are composed
                    // of the triangle polygon.
                    const kvs::Vector3f vertex0( this->interpolate_vertex( v0, v1 ) );
                    coords.push_back( vertex0.x() );
                    coords.push_back( vertex0.y() );
                    coords.push_back( vertex0.z() );

                    const kvs::Vector3f vertex1( this->interpolate_vertex( v2, v3 ) );
                    coords.push_back( vertex1.x() );
                    coords.push_back( vertex1.y() );
                    coords.push_back( vertex1.z() );

                    const kvs::Vector3f vertex2( this->interpolate_vertex( v4, v5 ) );
                    coords.push_back( vertex2.x() );
                    coords.push_back( vertex2.y() );
                    coords.push_back( vertex2.z() );

                    const double value0 = this->interpolate_value( volume, i0, i1, v0, v1 );
                    const double value1 = this->interpolate_value( volume, i2, i3, v2, v3 );
                    const double value2 = this->interpolate_value( volume, i4, i5, v4, v5 );

                    const kvs::UInt8 color0 =
                        static_cast<kvs::UInt8>( normalize_factor * ( value0 - min_value ) + 0.5 );
                    colors.push_back( color_map[ color0 ].r() );
                    colors.push_back( color_map[ color0 ].g() );
                    colors.push_back( color_map[ color0 ].b() );

                    const kvs::UInt8 color1 =
                        static_cast<kvs::UInt8>( normalize_factor * ( value1 - min_value ) + 0.5 );
                    colors.push_back( color_map[ color1 ].r() );
                    colors.push_back( color_map[ color1 ].g() );
                    colors.push_back( color_map[ color1 ].b() );

                    const kvs::UInt8 color2 =
                        static_cast<kvs::UInt8>( normalize_factor * ( value2 - min_value ) + 0.5 );
                    colors.push_back( color_map[ color2 ].r() );
                    colors.push_back( color_map[ color2 ].g() );
                    colors.push_back( color_map[ color2 ].b() );

                    // Calculate a normal vector for the triangle polygon.
                    const kvs::Vector3f normal( -( vertex2 - vertex0 ).cross( vertex1 - vertex0 ) );
                    normals.push_back( normal.x() );
                    normals.push_back( normal.y() );
                    normals.push_back( normal.z() );
                } // end of loop-triangle
            } // end of loop-x
            ++index;
        } // end of loop-y
        index += line_size;
    } // end of loop-z

    SuperClass::setCoords( kvs::ValueArray<kvs::Real32>( coords ) );
    SuperClass::setColors( kvs::ValueArray<kvs::UInt8>( colors ) );
    SuperClass::setNormals( kvs::ValueArray<kvs::Real32>( normals ) );
    SuperClass::setOpacity( 255 );
    SuperClass::setPolygonType( kvs::PolygonObject::Triangle );
    SuperClass::setColorType( kvs::PolygonObject::VertexColor );
    SuperClass::setNormalType( kvs::PolygonObject::PolygonNormal );

    return this;
}

const size_t SlicePlane::calculate_table_index(
    const size_t x,
    const size_t y,
    const size_t z ) const
{
    size_t table_index = 0;
    if ( this->substitute_plane_equation( x  , y  , z   ) > 0.0f ) { table_index |=   1; }
    if ( this->substitute_plane_equation( x+1, y  , z   ) > 0.0f ) { table_index |=   2; }
    if ( this->substitute_plane_equation( x+1, y+1, z   ) > 0.0f ) { table_index |=   4; }
    if ( this->substitute_plane_equation( x  , y+1, z   ) > 0.0f ) { table_index |=   8; }
    if ( this->substitute_plane_equation( x  , y  , z+1 ) > 0.0f ) { table_index |=  16; }
    if ( this->substitute_plane_equation( x+1, y  , z+1 ) > 0.0f ) { table_index |=  32; }
    if ( this->substitute_plane_equation( x+1, y+1, z+1 ) > 0.0f ) { table_index |=  64; }
    if ( this->substitute_plane_equation( x  , y+1, z+1 ) > 0.0f ) { table_index |= 128; }

    return( table_index );
}

const size_t SlicePlane::calculate_table_index(
    const float x,
    const float y,
    const float z,
    const float xdiff,
    const float ydiff,
    const float zdiff ) const
{
    size_t table_index = 0;
    if ( this->substitute_plane_equation( x,       y,       z       ) > 0.0f ) { table_index |=   1; }
    if ( this->substitute_plane_equation( x+xdiff, y,       z       ) > 0.0f ) { table_index |=   2; }
    if ( this->substitute_plane_equation( x+xdiff, y+ydiff, z       ) > 0.0f ) { table_index |=   4; }
    if ( this->substitute_plane_equation( x,       y+ydiff, z       ) > 0.0f ) { table_index |=   8; }
    if ( this->substitute_plane_equation( x,       y,       z+zdiff ) > 0.0f ) { table_index |=  16; }
    if ( this->substitute_plane_equation( x+xdiff, y,       z+zdiff ) > 0.0f ) { table_index |=  32; }
    if ( this->substitute_plane_equation( x+xdiff, y+ydiff, z+zdiff ) > 0.0f ) { table_index |=  64; }
    if ( this->substitute_plane_equation( x,       y+ydiff, z+zdiff ) > 0.0f ) { table_index |= 128; }

    return( table_index );
}

const float SlicePlane::substitute_plane_equation(
    const size_t x,
    const size_t y,
    const size_t z ) const
{
    return( m_coefficients.x() * x +
            m_coefficients.y() * y +
            m_coefficients.z() * z +
            m_coefficients.w() );
}

const float SlicePlane::substitute_plane_equation(
    const kvs::Vector3f& vertex ) const
{
    return( m_coefficients.x() * vertex.x() +
            m_coefficients.y() * vertex.y() +
            m_coefficients.z() * vertex.z() +
            m_coefficients.w() );
}

const kvs::Vector3f SlicePlane::interpolate_vertex(
    const kvs::Vector3f& vertex0,
    const kvs::Vector3f& vertex1 ) const
{
    const float value0 = this->substitute_plane_equation( vertex0 );
    const float value1 = this->substitute_plane_equation( vertex1 );
    const float ratio = kvs::Math::Abs( value0 / ( value1 - value0 ) );

    return( ( 1.0f - ratio ) * vertex0 + ratio * vertex1 );
}

const double SlicePlane::interpolate_value(
    const kvs::StructuredVolumeObject* volume,
    const kvs::Vector3f& index0,
    const kvs::Vector3f& index1,
    const kvs::Vector3f& vertex0,
    const kvs::Vector3f& vertex1 ) const
{
    const kvs::Real32* const values = static_cast<const kvs::Real32*>( volume->values().data() );

    const size_t line_size  = volume->numberOfNodesPerLine();
    const size_t slice_size = volume->numberOfNodesPerSlice();

    const float value0 = this->substitute_plane_equation( vertex0 );
    const float value1 = this->substitute_plane_equation( vertex1 );
    const float ratio = kvs::Math::Abs( value0 / ( value1 - value0 ) );

    const double x0 = index0.x();
    const double y0 = index0.y();
    const double z0 = index0.z();
    const double x1 = index1.x();
    const double y1 = index1.y();
    const double z1 = index1.z();
    const size_t v0_index = static_cast<size_t>( x0 + y0 * line_size + z0 * slice_size );
    const size_t v1_index = static_cast<size_t>( x1 + y1 * line_size + z1 * slice_size );

    return( values[ v0_index ] + ratio * ( values[ v1_index ] - values[ v0_index ] ) );
}

} // end of namespace util

} // end of namespace kvsoceanvis
