/*****************************************************************************/
/**
 *  @file   Controller.h
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
#ifndef CONTROLLER_H_INCLUDE
#define CONTROLLER_H_INCLUDE

#include "Model.h"
#include "View.h"
#include "Interactor.h"

/*===========================================================================*/
/**
 *  @brief  Controller class.
 */
/*===========================================================================*/
class Controller
{
private:

    Model* m_model; ///< pointer to the model
    ParallelView* m_parallel_view; ///< pointer to the parallel view
    VolumeView* m_volume_view; ///< pointer to the volume view
    SliceView* m_slice_view; ///< pointer to the slice view

    ParallelInteractor* m_parallel_interactor; ///< interactor for parallel view
    SliceInteractor* m_slice_interactor; ///< interactor for slice view

public:

    Controller(
        Model* model,
        ParallelView* parallel_view,
        VolumeView* volume_view,
        SliceView* slice_view );

    ~Controller();
};

#endif // CONTROLLER_H_INCLUDE
