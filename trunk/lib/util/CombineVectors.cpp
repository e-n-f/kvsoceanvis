/*****************************************************************************/
/**
 *  @file   CombineVectors.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: CombineVectors.cpp 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "CombineVectors.h"


namespace kvsoceanvis
{

namespace util
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new CombineVectors class.
 *  @param  uvolume [in] fisrt component value
 *  @param  vvolume [in] second component value
 *  @param  wvolume [in] third component value
 */
/*===========================================================================*/
CombineVectors::CombineVectors(
    const kvs::StructuredVolumeObject* uvolume,
    const kvs::StructuredVolumeObject* vvolume,
    const kvs::StructuredVolumeObject* wvolume )
{
    m_volume2 = vvolume;
    m_volume3 = wvolume;
    this->exec( uvolume );
}

/*===========================================================================*/
/**
 *  @brief  Main method.
 *  @param  object [in] pointer to the volume object
 *  @return pointer to the combined volume object
 */
/*===========================================================================*/
CombineVectors::SuperClass* CombineVectors::exec( const kvs::ObjectBase* object )
{
    const kvs::StructuredVolumeObject* uvolume = kvs::StructuredVolumeObject::DownCast( object );
    const kvs::StructuredVolumeObject* vvolume = m_volume2;
    const kvs::StructuredVolumeObject* wvolume = m_volume3;

    const size_t nnodes = uvolume->numberOfNodes();

//    kvs::AnyValueArray values;
//    float* pvalues = static_cast<float*>( values.allocate<float>( nnodes * 3 ) );
    kvs::ValueArray<float> values( nnodes * 3 );
    float* pvalues = values.data();

    // Combine the vector components
    const float* u = static_cast<const float*>(uvolume->values().data());
    const float* v = static_cast<const float*>(vvolume->values().data());
    const float* w = static_cast<const float*>(wvolume->values().data());
    for ( size_t i = 0; i < nnodes; i++ )
    {
        *(pvalues++) = *(u++);
        *(pvalues++) = *(v++);
        *(pvalues++) = *(w++);
    }

    SuperClass::setGridType( uvolume->gridType() );
    SuperClass::setResolution( uvolume->resolution() );
    SuperClass::setVeclen( 3 );
    SuperClass::setValues( kvs::AnyValueArray( values ) );
    SuperClass::updateMinMaxValues();
    if ( uvolume->gridType() == kvs::StructuredVolumeObject::Rectilinear )
    {
        SuperClass::setCoords( uvolume->coords() );
        SuperClass::setMinMaxObjectCoords( uvolume->minObjectCoord(), uvolume->maxObjectCoord() );
        SuperClass::setMinMaxExternalCoords( uvolume->minExternalCoord(), uvolume->maxExternalCoord() );
    }
    else
    {
        SuperClass::updateMinMaxCoords();
    }

    return this;
}

} // end of namespace util

} // end of namespace kvsoceanvis
