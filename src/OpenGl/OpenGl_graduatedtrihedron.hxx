#ifndef __OPENGL_GRADUATEDTRIHEDRON_H_
#define __OPENGL_GRADUATEDTRIHEDRON_H_

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem_view.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

extern TStatus call_graduatedtrihedron_get(int WsId, CALL_DEF_GRADUATEDTRIHEDRON* data);
extern TStatus call_graduatedtrihedron_display(int WsId, CALL_DEF_GRADUATEDTRIHEDRON* data);
extern TStatus call_graduatedtrihedron_redraw(int WsId);
extern TStatus call_graduatedtrihedron_erase(int WsId);
extern TStatus call_graduatedtrihedron_minmaxvalues(const float xMin,
                                                    const float yMin,
                                                    const float zMin,
                                                    const float xMax,
                                                    const float yMax,
                                                    const float zMax);

#endif /* __OPENGL_GRADUATEDTRIHEDRON_H_ */
