// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_View.hxx>

#include <Visual3d_Layer.hxx>

#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_Workspace.hxx>

/*----------------------------------------------------------------------*/

//TelProjectionRaster in OpenGl_telem_util.cxx
Standard_Boolean OpenGl_View::ProjectObjectToRaster (const Standard_Integer w, const Standard_Integer h,
                                                    const Standard_ShortReal x, const Standard_ShortReal y, const Standard_ShortReal z,
                                                    Standard_ShortReal &xr, Standard_ShortReal &yr)
{
  int i, j, k;

  GLdouble modelMatrix[16];
  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      modelMatrix[k] = ( GLdouble )myOrientationMatrix[i][j];

  GLdouble projMatrix[16];
  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      projMatrix[k] = ( GLdouble )myMappingMatrix[i][j];

  GLint viewport[4];
  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = w;
  viewport[3] = h;

  /*
  * glGetIntegerv (GL_VIEWPORT, viewport);
  * glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
  * glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
  */

  GLdouble winx, winy, winz;
  if (gluProject (( GLdouble )x, ( GLdouble )y, ( GLdouble )z, modelMatrix, projMatrix, viewport, &winx, &winy, &winz))
  {
    xr = ( Standard_ShortReal )winx;
    yr = ( Standard_ShortReal )winy;
    return Standard_True;
  }

  xr = 0.F;
  yr = 0.F;
  return Standard_False;
}

/*----------------------------------------------------------------------*/
//TelUnProjectionRaster
Standard_Boolean OpenGl_View::ProjectRasterToObject (const Standard_Integer w, const Standard_Integer h,
                                                    const Standard_Integer xr, const Standard_Integer yr,
                                                    Standard_ShortReal &x, Standard_ShortReal &y, Standard_ShortReal &z)
{
  int i, j, k;

  GLdouble modelMatrix[16];
  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      modelMatrix[k] = ( GLdouble )myOrientationMatrix[i][j];

  GLdouble projMatrix[16];
  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      projMatrix[k] = ( GLdouble )myMappingMatrix[i][j];

  GLint viewport[4];
  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = w;
  viewport[3] = h;

  /*
  * glGetIntegerv (GL_VIEWPORT, viewport);
  * glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
  * glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
  */

  GLdouble objx, objy, objz;
  if (gluUnProject (( GLdouble )xr, ( GLdouble )yr, 0.0, modelMatrix, projMatrix, viewport, &objx, &objy, &objz))
  {
    x = ( Standard_ShortReal )objx;
    y = ( Standard_ShortReal )objy;
    z = ( Standard_ShortReal )objz;
    return Standard_True;
  }

  x = 0.F;
  y = 0.F;
  z = 0.F;
  return Standard_False;
}

/*----------------------------------------------------------------------*/
//TelUnProjectionRasterWithRay
Standard_Boolean OpenGl_View::ProjectRasterToObjectWithRay (const Standard_Integer w, const Standard_Integer h,
                                                           const Standard_Integer xr, const Standard_Integer yr,
                                                           Standard_ShortReal &x, Standard_ShortReal &y, Standard_ShortReal &z,
                                                           Standard_ShortReal &dx, Standard_ShortReal &dy, Standard_ShortReal &dz)
{
  int i, j, k;

  GLdouble modelMatrix[16];
  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      modelMatrix[k] = ( GLdouble )myOrientationMatrix[i][j];

  GLdouble projMatrix[16];
  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      projMatrix[k] = ( GLdouble )myMappingMatrix[i][j];

  GLint viewport[4];
  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = w;
  viewport[3] = h;

  /*
  * glGetIntegerv (GL_VIEWPORT, viewport);
  * glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
  * glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
  */

  const GLdouble winx = ( GLdouble )xr;
  const GLdouble winy = ( GLdouble )yr;

  GLdouble objx, objy, objz;
  if (gluUnProject (winx, winy, 0.0, modelMatrix, projMatrix, viewport, &objx, &objy, &objz))
  {
    GLdouble objx1, objy1, objz1;
    if (gluUnProject (winx, winy, -10.0, modelMatrix, projMatrix, viewport, &objx1, &objy1, &objz1))
    {
      x = ( Standard_ShortReal )objx;
      y = ( Standard_ShortReal )objy;
      z = ( Standard_ShortReal )objz;
      dx = ( Standard_ShortReal )(objx-objx1);
      dy = ( Standard_ShortReal )(objy-objy1);
      dz = ( Standard_ShortReal )(objz-objz1);
      return Standard_True;
    }
  }

  x = 0.F;
  y = 0.F;
  z = 0.F;
  dx = 0.F;
  dy = 0.F;
  dz = 0.F;
  return Standard_False;
}

/*----------------------------------------------------------------------*/

//call_togl_inquiremat
void OpenGl_View::GetMatrices (TColStd_Array2OfReal&  theMatOrient,
                               TColStd_Array2OfReal&  theMatMapping,
                               const Standard_Boolean theIsCustom) const
{
  int i, j;
  if (theIsCustom)
  {
    // OCC18942: Trying to return the current matrices instead of calculating them anew.
    // This in particular allows using application-defined matrices.
    for (i = 0; i < 4; ++i)
    {
      for (j = 0; j < 4; ++j)
      {
        theMatOrient  (i, j) = myOrientationMatrix[j][i];
        theMatMapping (i, j) = myMappingMatrix[j][i];
      }
    }
    return;
  }

  int anErr = 0;
  Tmatrix3 aMatOri;
  Tmatrix3 aMatMapping;

  view_map3 aViewMap;
  memcpy (&aViewMap.win,     &myExtra.map.window,   sizeof(Tlimit));
  memcpy (&aViewMap.proj_vp, &myExtra.map.viewport, sizeof(Tlimit3));
  switch (myExtra.map.proj)
  {
    default:
    case TelParallel:    aViewMap.proj_type = TYPE_PARAL;    break;
    case TelPerspective: aViewMap.proj_type = TYPE_PERSPECT; break;
  }
  aViewMap.proj_ref_point.x = myExtra.map.prp[0];
  aViewMap.proj_ref_point.y = myExtra.map.prp[1];
  aViewMap.proj_ref_point.z = myExtra.map.prp[2];
  aViewMap.view_plane  = myExtra.map.vpd;
  aViewMap.back_plane  = myExtra.map.bpd;
  aViewMap.front_plane = myExtra.map.fpd;

  call_func_eval_ori_matrix3 ((const point3* )myExtra.vrp,
                              (const vec3*   )myExtra.vpn,
                              (const vec3*   )myExtra.vup,
                              &anErr, aMatOri);
  if (anErr == 0)
    call_func_eval_map_matrix3 (&aViewMap, &anErr, aMatMapping);

  if (anErr == 0)
  {
    for (i = 0; i < 4; ++i)
    {
      for (j = 0; j < 4; ++j)
      {
        theMatOrient  (i, j) = aMatOri[j][i];
        theMatMapping (i, j) = aMatMapping[j][i];
      }
    }
    return;
  }

  // return just identity matrices
  for (i = 0; i < 4; ++i)
  {
    for (j = 0; j < 4; ++j)
    {
      if (i == j) {
        theMatMapping (i, j) = 1.0;
        theMatOrient  (i, j) = 1.0;
      }
      else {
        theMatMapping (i, j)  = 0.0;
        theMatOrient  (i, j)  = 0.0;
      }
    }
  }
}

/*----------------------------------------------------------------------*/
