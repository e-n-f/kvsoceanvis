/*****************************************************************************/
/**
 *  @file   Model.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
#ifndef MODEL_H_INCLUDE
#define MODEL_H_INCLUDE

#include <string>
#include <kvs/StructuredVolumeObject>
#include <kvs/TableObject>
#include "DataBase.h"

namespace kvs { class GrADS; }
namespace kvs { class ColorMap; }
class View;

/*===========================================================================*/
/**
 *  @brief  Model class.
 */
/*===========================================================================*/
class Model
{
public:

    typedef kvs::StructuredVolumeObject VolumeObject;
    typedef kvs::TableObject TableObject;

private:

    DataBase m_data_base; ///< data base in which GrADS datasets are stored
    VolumeObject* m_volume_object; ///< pointer to the rendered volume object
    TableObject* m_table_object; ///< pointer to the rendered table object

public:

    Model();

    DataBase& dataBase() { return m_data_base; }
    void read( const std::string& filename );

    VolumeObject* volumeObject();
    TableObject* tableObject();
    bool importVolumeObject( const std::string& varname, const size_t tindex );
    bool importTableObject( const size_t tindex );
    VolumeObject* croppedVolumeObject( const kvs::Vec3 min_range, const kvs::Vec3 max_range );
    TableObject* croppedTableObject( const kvs::Vec3 min_range, const kvs::Vec3 max_range );
};

#endif // MODEL_H_INCLUDE
