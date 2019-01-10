/*****************************************************************************/
/**
 *  @file   Controller.cpp
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
#include "Controller.h"


/*===========================================================================*/
/**
 *  @brief  Constructs a new Controller class.
 *  @param  model [in] pointer to the model
 *  @param  parallel_view [in] pointer to the parallel view
 *  @param  volume_view [in] pointer to the volume view
 *  @param  slice_view [in] pointer to the slice view
 */
/*===========================================================================*/
Controller::Controller(
    Model* model,
    ParallelView* parallel_view,
    VolumeView* volume_view,
    SliceView* slice_view ):
    m_model( model ),
    m_parallel_view( parallel_view ),
    m_volume_view( volume_view ),
    m_slice_view( slice_view )
{
    m_parallel_interactor = new ParallelInteractor();
    m_parallel_interactor->setObjectName( "Table" );
    m_parallel_interactor->setRendererName( "PCs" );
    m_parallel_view->addEvent( m_parallel_interactor );

    m_slice_interactor = new SliceInteractor( model, parallel_view, volume_view );
    m_slice_interactor->setObjectName( "SliceImage" );
    m_slice_interactor->setRendererName( "SliceRenderer" );
    m_slice_view->addEvent( m_slice_interactor );
}

/*===========================================================================*/
/**
 *  @brief  Destroys the controller class.
 */
/*===========================================================================*/
Controller::~Controller()
{
    delete m_parallel_interactor;
    delete m_slice_interactor;
}
