/*****************************************************************************/
/**
 *  @file   MultiBinMappedParallelCoordinatesRenderer.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: MultiBinMappedParallelCoordinatesRenderer.h 287 2012-06-15 14:20:06Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__PCS__MULTI_BIN_MAPPED_PARALLEL_COORDINATES_RENDERER_H_INCLUDE
#define KVSOCEANVIS__PCS__MULTI_BIN_MAPPED_PARALLEL_COORDINATES_RENDERER_H_INCLUDE

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
 *  @brief  Multiple bin mapped parallel coordinates renderer class.
 */
/*===========================================================================*/
class MultiBinMappedParallelCoordinatesRenderer : public kvs::RendererBase
{
    // Class name.
    kvsClassName( kvsoceanvis::pcs::MultiBinMappedParallelCoordinatesRenderer );

    // Module information.
    kvsModuleCategory( Renderer );
    kvsModuleBaseClass( kvs::RendererBase );

protected:

    int m_top_margin; ///< top margin
    int m_bottom_margin; ///< bottom margin
    int m_left_margin; ///< left margin
    int m_right_margin; ///< right margin
    mutable bool m_enable_anti_aliasing; ///< flag for anti-aliasing (AA)
    mutable bool m_enable_multisample_anti_aliasing; ///< flag for multisample anti-aliasing (MSAA)
    size_t m_active_axis; ///< index of active axis
    kvs::UInt8 m_bin_opacity; ///< bin opacity
    kvs::Real32 m_bin_edge_width; ///< bin edge width
    kvs::ColorMap m_color_map; ///< color map

public:

    MultiBinMappedParallelCoordinatesRenderer();

public:

    void setTopMargin( const int top_margin );
    void setBottomMargin( const int bottom_margin );
    void setLeftMargin( const int left_margin );
    void setRightMargin( const int right_margin );
    void enableAntiAliasing( const bool multisample = false ) const;
    void disableAntiAliasing() const;
    void setBinOpacity( const kvs::UInt8 opacity );
    void setBinEdgeWidth( const kvs::Real32 width );
    void setColorMap( const kvs::ColorMap& color_map );
    void selectAxis( const size_t index );

    int topMargin() const;
    int bottomMargin() const;
    int leftMargin() const;
    int rightMargin() const;
    size_t activeAxis() const;
    kvs::UInt8 binOpacity() const;
    kvs::Real32 binEdgeWidth() const;

public:

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );
};

} // end of namespace pcs

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__PCS__MULTI_BIN_MAPPED_PARALLEL_COORDINATES_RENDERER_H_INCLUDE
