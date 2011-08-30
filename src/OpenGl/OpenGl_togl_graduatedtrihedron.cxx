// File:      OpenGl_togl_graduatedtrihedron.cxx
// Created:   6 March 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE SA 2011

#include <OpenGl_graduatedtrihedron.hxx>

#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem_view.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

void EXPORT call_togl_graduatedtrihedron_display(CALL_DEF_VIEW* view, 
                                                 const Graphic3d_CGraduatedTrihedron &cubic)
{
    call_graduatedtrihedron_display(view->WsId, cubic);
}

void EXPORT call_togl_graduatedtrihedron_erase(CALL_DEF_VIEW* view)
{
    call_graduatedtrihedron_erase(view->WsId);
}

void EXPORT call_togl_graduatedtrihedron_minmaxvalues(float xmin, float ymin, float zmin,
                                                      float xmax, float ymax, float zmax)
{
    call_graduatedtrihedron_minmaxvalues(xmin, ymin, zmin, xmax, ymax, zmax);
}
