// File:      OpenGl_graduatedtrihedron.hxx
// Created:   6 March 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE SA 2011

#ifndef __OPENGL_GRADUATEDTRIHEDRON_H_
#define __OPENGL_GRADUATEDTRIHEDRON_H_

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem_view.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <Graphic3d_CGraduatedTrihedron.hxx>

extern TStatus call_graduatedtrihedron_display(int WsId, const Graphic3d_CGraduatedTrihedron &data);
extern TStatus call_graduatedtrihedron_redraw(int WsId);
extern TStatus call_graduatedtrihedron_erase(int WsId);
extern TStatus call_graduatedtrihedron_minmaxvalues(const float xMin,
                                                    const float yMin,
                                                    const float zMin,
                                                    const float xMax,
                                                    const float yMax,
                                                    const float zMax);

#endif /* __OPENGL_GRADUATEDTRIHEDRON_H_ */
