/*****************************************************************************/
/**
 *  @file   BinMappedParallelCoordinatesRenderer.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: BinMappedParallelCoordinatesRenderer.h 287 2012-06-15 14:20:06Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__PCS__BIN_MAPPED_PARALLEL_COORDINATES_RENDERER_H_INCLUDE
#define KVSOCEANVIS__PCS__BIN_MAPPED_PARALLEL_COORDINATES_RENDERER_H_INCLUDE

#include <kvs/RendererBase>
#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/ColorMap>


namespace kvsoceanvis
{

namespace pcs
{

class ObjectBase;
class Camera;
class Light;

/*===========================================================================*/
/**
 *  @brief  Binned parallel coordinates renderer class.
 */
/*===========================================================================*/
class BinMappedParallelCoordinatesRenderer : public kvs::RendererBase
{
    // Class name.
    kvsClassName( kvsoceanvis::pcs::BinMappedParallelCoordinatesRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

private:

    kvs::ColorMap m_color_map; ///< color map

public:

    BinMappedParallelCoordinatesRenderer();

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );
};

} // end of namespace pcs

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__PCS__BIN_MAPPED_PARALLEL_COORDINATES_RENDERER_H_INCLUDE
