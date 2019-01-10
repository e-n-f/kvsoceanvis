/*****************************************************************************/
/**
 *  @file   Interactor.h
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
#pragma once

#include <string>
#include <kvs/EventListener>
#include "Model.h"
#include "View.h"

class ParallelInteractor : public kvs::EventListener
{
public:

    typedef ParallelView::Object Object;
    typedef ParallelView::Renderer Renderer;

    struct Parameter
    {
        int X0;
        int X1;
        int Y0;
        int Y1;
        int pX;
        int pY;
        int SelectedAxis;
        bool SettingRange;
        bool MovingRange;

        Parameter():
            X0(0),
            X1(0),
            Y0(0),
            Y1(0),
            pX(0),
            pY(0),
            SelectedAxis(0),
            SettingRange(false),
            MovingRange(false){}
    };

private:

    Parameter m_parameter;
    std::string m_object_name;
    std::string m_renderer_name;

public:

    ParallelInteractor();
    void setObjectName( const std::string& name ) { m_object_name = name; }
    void setRendererName( const std::string& name ) { m_renderer_name = name; }
    void KeyPressEvent( kvs::KeyEvent* event );
    void mousePressEvent( kvs::MouseEvent* event );
    void mouseDoubleClickEvent( kvs::MouseEvent* event );
    void mouseReleaseEvent( kvs::MouseEvent* event );
    void mouseMoveEvent( kvs::MouseEvent* event );
    void paintEvent();
};

class SliceInteractor : public kvs::EventListener
{
public:

    typedef SliceView::Object Object;
    typedef SliceView::Renderer Renderer;

    struct Parameter
    {
        int X0;
        int X1;
        int Y0;
        int Y1;
        int pX;
        int pY;
        bool Rectangle;
        bool MovingRange;

        Parameter():
            X0(0),
            X1(0),
            Y0(0),
            Y1(0),
            pX(0),
            pY(0),
            Rectangle(false),
            MovingRange(false){}
    };

private:

    Model* m_model;
    ParallelView* m_parallel_view;
    VolumeView* m_volume_view;
    Parameter m_parameter;
    std::string m_object_name;
    std::string m_renderer_name;

public:

    SliceInteractor( Model* model, ParallelView* parallel_view, VolumeView* volume_view );
    void setObjectName( const std::string& name ) { m_object_name = name; }
    void setRendererName( const std::string& name ) { m_renderer_name = name; }
    void mousePressEvent( kvs::MouseEvent* event );
    void mouseDoubleClickEvent( kvs::MouseEvent* event );
    void mouseReleaseEvent( kvs::MouseEvent* event );
    void mouseMoveEvent( kvs::MouseEvent* event );
    void paintEvent();
};
