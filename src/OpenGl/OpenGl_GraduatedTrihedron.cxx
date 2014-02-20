// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif
#ifdef HAVE_STRING_H
  #include <string.h>
#endif

#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_AspectLine.hxx>

const OpenGl_AspectLine myDefaultAspectLine;

static const OpenGl_TextParam THE_LABEL_PARAMS =
{
  16, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM
};

/* Bounding box */
float xmin = 0.0f, ymin = 0.0f, zmin = 0.0f, xmax = 100.0f, ymax = 100.0f, zmax = 100.0f;

/* Normal of the view (not normalized!) */
static float getNormal(float* normal)
{
  GLint viewport[4];
  GLdouble model_matrix[16], proj_matrix[16];

  glGetDoublev(GL_MODELVIEW_MATRIX,  model_matrix);
  glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix);
  glGetIntegerv(GL_VIEWPORT, viewport);

  double x1, y1, z1, x2, y2, z2, x3, y3, z3;
  gluUnProject(viewport[0], viewport[1], 0., model_matrix, proj_matrix, viewport, &x1, &y1, &z1);
  gluUnProject(viewport[0] + viewport[2], viewport[1], 0., model_matrix, proj_matrix, viewport, &x2, &y2, &z2);
  gluUnProject(viewport[0], viewport[1] + viewport[3], 0., model_matrix, proj_matrix, viewport, &x3, &y3, &z3);

  /* Normal out of user is p1p3^p1p2 */
  const double dx1 = x3 - x1;
  const double dy1 = y3 - y1;
  const double dz1 = z3 - z1;
  const double dx2 = x2 - x1;
  const double dy2 = y2 - y1;
  const double dz2 = z2 - z1;
  normal[0] = (float) (dy1 * dz2 - dz1 * dy2);
  normal[1] = (float) (dz1 * dx2 - dx1 * dz2);
  normal[2] = (float) (dx1 * dy2 - dy1 * dx2);

  /* Distance corresponding to 1 pixel */
  const float width = (float) sqrt(dx2 * dx2 + dy2 * dy2 + dz2 * dz2);
  return width / (float) viewport[2];
}

static float getDistance2Corner(float* normal, float* center, float x, float y, float z)
{
    return normal[0] * (x - center[0]) + normal[1] * (y - center[1]) + normal[2] * (z - center[2]);
}

static char getFarestCorner(float d000, float d100, float d010, float d001,
                            float d110, float d101, float d011, float d111)
{
    if (d000 > 0.0f &&
        d000 > d100 && d000 > d010 && d000 > d001 && d000 > d110 &&
        d000 > d101 && d000 > d011 && d000 > d111)
    {
        return 1;
    }
    else if (d100 > 0.0f &&
             d100 > d000 && d100 > d010 && d100 > d001 && d100 > d110 &&
             d100 > d101 && d100 > d011 && d100 > d111)
    {
        return 2;
    }
    else if (d010 > 0.0f &&
             d010 > d000 && d010 > d100 && d010 > d001 && d010 > d110 &&
             d010 > d101 && d010 > d011 && d010 > d111)
    {
        return 3;
    }
    else if (d001 > 0.0f &&
             d001 > d000 && d001 > d100 && d001 > d010 && d001 > d110 &&
             d001 > d101 && d001 > d011 && d001 > d111)
    {
        return 4;
    }
    else if (d110 > 0.0f &&
             d110 > d000 && d110 > d100 && d110 > d010 && d110 > d001 &&
             d110 > d101 && d110 > d011 && d110 > d111)
    {
        return 5;
    }
    else if (d101 > 0.0f &&
             d101 > d000 && d101 > d100 && d101 > d010 && d101 > d001 &&
             d101 > d110 && d101 > d011 && d101 > d111)
    {
        return 6;
    }
    else if (d011 > 0.0f &&
             d011 > d000 && d011 > d100 && d011 > d010 && d011 > d001 &&
             d011 > d110 && d011 > d101 && d011 > d111)
    {
        return 7;
    }
    return 8; /* d111 */
}

static void drawArrow(float x1, float y1, float z1,
                      float x2, float y2, float z2,
                      float xn, float yn, float zn)
{
    float h, r;
    float xa, ya, za;
    float x0, y0, z0;
    float xr, yr, zr;
    float xa1, ya1, za1, xa2, ya2, za2;

    /* Start of arrow: at 10% from the end */
    x0 = x1 + 0.9f * (x2 - x1); y0 = y1 + 0.9f * (y2 - y1); z0 = z1 + 0.9f * (z2 - z1);

    /* Base of the arrow */
    xa = (x2 - x0); ya = (y2 - y0); za = (z2 - z0);

    /* Height of the arrow */
    h = sqrtf(xa * xa + ya * ya + za * za);
    if (h <= 0.0f)
        return;
    xa = xa / h; ya = ya / h; za = za / h;

    /* Radial direction to the arrow */
    xr = ya * zn - za * yn;
    yr = za * xn - xa * zn;
    zr = xa * yn - ya * xn;

    /* Normalize the radial vector */
    r = sqrtf(xr * xr + yr * yr + zr * zr);
    if (r <= 0.0f)
        return;
    xr = xr / r; yr = yr / r; zr = zr / r;

    /* First point of the base of the arrow */
    r = 0.2f * h;
    xr = r * xr; yr = r * yr; zr = r * zr;
    xa1 = x0 + xr; ya1 = y0 + yr; za1 = z0 + zr;

    /* Second point of the base of the arrow */
    xa2 = x0 - xr; ya2 = y0 - yr; za2 = z0 - zr;

    /* Draw a line to the arrow */
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x0, y0, z0);
    glEnd();

    /* Draw a triangle of the arrow */
    glBegin(GL_LINE_LOOP);
        glVertex3f(xa1, ya1, za1);
        glVertex3f(xa2, ya2, za2);
        glVertex3f(x2,  y2,  z2);
    glEnd();
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::Release (const Handle(OpenGl_Context)& theCtx)
{
  myLabelX.Release (theCtx);
  myLabelY.Release (theCtx);
  myLabelZ.Release (theCtx);
  myLabelValues.Release (theCtx);
}

OpenGl_GraduatedTrihedron::OpenGl_GraduatedTrihedron (const Graphic3d_CGraduatedTrihedron& theData)
: myLabelX (theData.xname, OpenGl_Vec3(1.0f, 0.0f, 0.0f), THE_LABEL_PARAMS),
  myLabelY (theData.yname, OpenGl_Vec3(0.0f, 1.0f, 0.0f), THE_LABEL_PARAMS),
  myLabelZ (theData.zname, OpenGl_Vec3(0.0f, 0.0f, 1.0f), THE_LABEL_PARAMS),
  myToDrawXName (theData.xdrawname == Standard_True),
  myToDrawYName (theData.ydrawname == Standard_True),
  myToDrawZName (theData.zdrawname == Standard_True),
  myToDrawXValues (theData.xdrawvalues == Standard_True),
  myToDrawYValues (theData.ydrawvalues == Standard_True),
  myToDrawZValues (theData.zdrawvalues == Standard_True),
  myToDrawGrid (theData.drawgrid == Standard_True),
  myToDrawAxes (theData.drawaxes == Standard_True),
  myNbX (theData.nbx),
  myNbY (theData.nby),
  myNbZ (theData.nbz),
  myXOffset (theData.xoffset),
  myYOffset (theData.yoffset),
  myZOffset (theData.zoffset),
  myXAxisOffset (theData.xaxisoffset),
  myYAxisOffset (theData.yaxisoffset),
  myZAxisOffset (theData.zaxisoffset),
  myDrawXTickmarks (theData.xdrawtickmarks),
  myDrawYTickmarks (theData.ydrawtickmarks),
  myDrawZTickmarks (theData.zdrawtickmarks),
  myXTickmarkLength (theData.xtickmarklength),
  myYTickmarkLength (theData.ytickmarklength),
  myZTickmarkLength (theData.ztickmarklength),
  myCbCubicAxes (theData.cbCubicAxes),
  myPtrVisual3dView (theData.ptrVisual3dView)
{
  myAspectLabels.ChangeFontName() = theData.fontOfNames;
  myAspectValues.ChangeFontName() = theData.fontOfValues;
  myAspectLabels.SetFontAspect (theData.styleOfNames);
  myAspectValues.SetFontAspect (theData.styleOfValues);
  myLabelX.SetFontSize (NULL, theData.sizeOfNames);
  myLabelY.SetFontSize (NULL, theData.sizeOfNames);
  myLabelZ.SetFontSize (NULL, theData.sizeOfNames);
  myLabelValues.SetFontSize (NULL, theData.sizeOfValues);

  // Grid color
  myGridColor[0] = (float) theData.gridcolor.Red();
  myGridColor[1] = (float) theData.gridcolor.Green();
  myGridColor[2] = (float) theData.gridcolor.Blue();
  // X name color
  myXNameColor.rgb[0] = (float )theData.xnamecolor.Red();
  myXNameColor.rgb[1] = (float )theData.xnamecolor.Green();
  myXNameColor.rgb[2] = (float )theData.xnamecolor.Blue();
  myXNameColor.rgb[3] = 1.0f;
  // Y name color
  myYNameColor.rgb[0] = (float )theData.ynamecolor.Red();
  myYNameColor.rgb[1] = (float )theData.ynamecolor.Green();
  myYNameColor.rgb[2] = (float )theData.ynamecolor.Blue();
  myYNameColor.rgb[3] = 1.0f;
  // Z name color
  myZNameColor.rgb[0] = (float )theData.znamecolor.Red();
  myZNameColor.rgb[1] = (float )theData.znamecolor.Green();
  myZNameColor.rgb[2] = (float )theData.znamecolor.Blue();
  myZNameColor.rgb[3] = 1.0f;
  // X color of axis and values
  myXColor.rgb[0] = (float )theData.xcolor.Red();
  myXColor.rgb[1] = (float )theData.xcolor.Green();
  myXColor.rgb[2] = (float )theData.xcolor.Blue();
  myXColor.rgb[3] = 1.0f;
  // Y color of axis and values
  myYColor.rgb[0] = (float )theData.ycolor.Red();
  myYColor.rgb[1] = (float )theData.ycolor.Green();
  myYColor.rgb[2] = (float )theData.ycolor.Blue();
  myYColor.rgb[3] = 1.0f;
  // Z color of axis and values
  myZColor.rgb[0] = (float )theData.zcolor.Red();
  myZColor.rgb[1] = (float )theData.zcolor.Green();
  myZColor.rgb[2] = (float )theData.zcolor.Blue();
  myZColor.rgb[3] = 1.0f;
}

OpenGl_GraduatedTrihedron::~OpenGl_GraduatedTrihedron()
{
  //
}

//call_graduatedtrihedron_redraw
void OpenGl_GraduatedTrihedron::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const OpenGl_AspectLine *oldAspectLine = theWorkspace->SetAspectLine(&myDefaultAspectLine);
  theWorkspace->AspectLine(Standard_True);

  /* Update boundary box */
  if (myCbCubicAxes)
    myCbCubicAxes(myPtrVisual3dView);

  /* Disable lighting for lines */
  GLboolean light = glIsEnabled(GL_LIGHTING);
  if (light)
    glDisable(GL_LIGHTING);

  /* Find the farest point of bounding box */

  /* Get normal of the view out of user. */
  /* Also, the method return distance corresponding to 1 pixel */
  float normal[3];
  float dpix = getNormal(normal);

  /* Normalize normal */
  float d = sqrtf(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
  normal[0] /= d;
  normal[1] /= d;
  normal[2] /= d;

  /* Get central point of bounding box */
  float center[3];
  center[0] = 0.5f * (xmin + xmax);
  center[1] = 0.5f * (ymin + ymax);
  center[2] = 0.5f * (zmin + zmax);

  /* Check distance to corners of bounding box along the normal */
  float d000 = getDistance2Corner(normal, center, xmin, ymin, zmin);
  float d100 = getDistance2Corner(normal, center, xmax, ymin, zmin);
  float d010 = getDistance2Corner(normal, center, xmin, ymax, zmin);
  float d001 = getDistance2Corner(normal, center, xmin, ymin, zmax);
  float d110 = getDistance2Corner(normal, center, xmax, ymax, zmin);
  float d101 = getDistance2Corner(normal, center, xmax, ymin, zmax);
  float d011 = getDistance2Corner(normal, center, xmin, ymax, zmax);
  float d111 = getDistance2Corner(normal, center, xmax, ymax, zmax);
  unsigned char farestCorner = getFarestCorner(d000, d100, d010, d001, d110, d101, d011, d111);

  /* Choose axes for the grid. */
  float LX1[6], LX2[6], LX3[6]; /* Lines along X direction */
  float LY1[6], LY2[6], LY3[6]; /* Lines along Y direction */
  float LZ1[6], LZ2[6], LZ3[6]; /* Lines along Z direction */
  unsigned char LX1draw = 0, LX2draw = 0, LX3draw = 0; /* Allows drawing of X-line (000 - 100 is forbidden) */
  unsigned char LY1draw = 0, LY2draw = 0, LY3draw = 0; /* Allows drawing of Y-line (000 - 010 is forbidden) */
  unsigned char LZ1draw = 0, LZ2draw = 0, LZ3draw = 0; /* Allows drawing of Z-line (000 - 001 is forbidden) */

  /* The first axis will be used for drawing the text and the values. */
  switch (farestCorner)
  {
    case 1: /* d000 */
    {
      /* 001 - 101 */
      LX1draw = 1;
      LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmax; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmax;
      /* 000 - 100 */
      LX2draw = 0; /* forbidden! */
      LX2[0] = xmin; LX2[1] = ymin; LX2[2] = zmin; LX2[3] = xmax; LX2[4] = ymin; LX2[5] = zmin;
      /* 010 - 110 */
      LX3draw = 1;
      LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmin; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmin;

      /* 100 - 110 */
      LY1draw = 1;
      LY1[0] = xmax; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmax; LY1[4] = ymax; LY1[5] = zmin;
      /* 000 - 010 */
      LY2draw = 0; /* forbidden! */
      LY2[0] = xmin; LY2[1] = ymin; LY2[2] = zmin; LY2[3] = xmin; LY2[4] = ymax; LY2[5] = zmin;
      /* 001 - 011 */
      LY3draw = 1;
      LY3[0] = xmin; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmin; LY3[4] = ymax; LY3[5] = zmax;

      /* 100 - 101 */
      LZ1draw = 1;
      LZ1[0] = xmax; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymin; LZ1[5] = zmax;
      /* 000 - 001 */
      LZ2draw = 0; /* forbidden! */
      LZ2[0] = xmin; LZ2[1] = ymin; LZ2[2] = zmin; LZ2[3] = xmin; LZ2[4] = ymin; LZ2[5] = zmax;
      /* 010 - 011 */
      LZ3draw = 1;
      LZ3[0] = xmin; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymax; LZ3[5] = zmax;

      break;
    }
    case 2: /* d100 */
    {
      /* 001 - 101 */
      LX1draw = 1;
      LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmax; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmax;
      /* 000 - 100 */
      LX2draw = 0; /* forbidden! */
      LX2[0] = xmin; LX2[1] = ymin; LX2[2] = zmin; LX2[3] = xmax; LX2[4] = ymin; LX2[5] = zmin;
      /* 010 - 110 */
      LX3draw = 1;
      LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmin; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmin;

      /* 000 - 010 */
      LY1draw = 0; /* forbidden! */
      LY1[0] = xmin; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmin; LY1[4] = ymax; LY1[5] = zmin;
      /* 100 - 110 */
      LY2draw = 1;
      LY2[0] = xmax; LY2[1] = ymin; LY2[2] = zmin; LY2[3] = xmax; LY2[4] = ymax; LY2[5] = zmin;
      /* 101 - 111 */
      LY3draw = 1;
      LY3[0] = xmax; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmax; LY3[4] = ymax; LY3[5] = zmax;

      /* 000 - 001 */
      LZ1draw = 0; /* forbidden! */
      LZ1[0] = xmin; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmin; LZ1[4] = ymin; LZ1[5] = zmax;
      /* 100 - 101 */
      LZ2draw = 1;
      LZ2[0] = xmax; LZ2[1] = ymin; LZ2[2] = zmin; LZ2[3] = xmax; LZ2[4] = ymin; LZ2[5] = zmax;
      /* 110 - 111 */
      LZ3draw = 1;
      LZ3[0] = xmax; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmax; LZ3[4] = ymax; LZ3[5] = zmax;

      break;
    }
    case 3: /* d010 */
    {
      /* 000 - 100 */
      LX1draw = 0; /* forbidden */
      LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmin;
      /* 010 - 110 */
      LX2draw = 1;
      LX2[0] = xmin; LX2[1] = ymax; LX2[2] = zmin; LX2[3] = xmax; LX2[4] = ymax; LX2[5] = zmin;
      /* 011 - 111 */
      LX3draw = 1;
      LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmax;

      /* 100 - 110 */
      LY1draw = 1;
      LY1[0] = xmax; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmax; LY1[4] = ymax; LY1[5] = zmin;
      /* 000 - 010 */
      LY2draw = 0; /* forbidden */
      LY2[0] = xmin; LY2[1] = ymin; LY2[2] = zmin; LY2[3] = xmin; LY2[4] = ymax; LY2[5] = zmin;
      /* 001 - 011 */
      LY3draw = 1;
      LY3[0] = xmin; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmin; LY3[4] = ymax; LY3[5] = zmax;

      /* 110 - 111 */
      LZ1draw = 1;
      LZ1[0] = xmax; LZ1[1] = ymax; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymax; LZ1[5] = zmax;
      /* 010 - 011 */
      LZ2draw = 1;
      LZ2[0] = xmin; LZ2[1] = ymax; LZ2[2] = zmin; LZ2[3] = xmin; LZ2[4] = ymax; LZ2[5] = zmax;
      /* 000 - 001 */
      LZ3draw = 0; /* forbidden */
      LZ3[0] = xmin; LZ3[1] = ymin; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymin; LZ3[5] = zmax;

      break;
    }
    case 4: /* d001 */
    {
      /* 000 - 100 */
      LX1draw = 0; /* forbidden */
      LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmin;
      /* 001 - 101 */
      LX2draw = 1;
      LX2[0] = xmin; LX2[1] = ymin; LX2[2] = zmax; LX2[3] = xmax; LX2[4] = ymin; LX2[5] = zmax;
      /* 011 - 111 */
      LX3draw = 1;
      LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmax;

      /* 000 - 010 */
      LY1draw = 0; /* forbidden */
      LY1[0] = xmin; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmin; LY1[4] = ymax; LY1[5] = zmin;
      /* 001 - 011 */
      LY2draw = 1;
      LY2[0] = xmin; LY2[1] = ymin; LY2[2] = zmax; LY2[3] = xmin; LY2[4] = ymax; LY2[5] = zmax;
      /* 101 - 111 */
      LY3draw = 1;
      LY3[0] = xmax; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmax; LY3[4] = ymax; LY3[5] = zmax;

      /* 100 - 101 */
      LZ1draw = 1;
      LZ1[0] = xmax; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymin; LZ1[5] = zmax;
      /* 000 - 001 */
      LZ2draw = 0; /* forbidden */
      LZ2[0] = xmin; LZ2[1] = ymin; LZ2[2] = zmin; LZ2[3] = xmin; LZ2[4] = ymin; LZ2[5] = zmax;
      /* 010 - 011 */
      LZ3draw = 1;
      LZ3[0] = xmin; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymax; LZ3[5] = zmax;

      break;
    }
    case 5: /* d110 */
    {
      /* 000 - 100 */
      LX1draw = 0; /* forbidden */
      LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmin;
      /* 010 - 110 */
      LX2draw = 1;
      LX2[0] = xmin; LX2[1] = ymax; LX2[2] = zmin; LX2[3] = xmax; LX2[4] = ymax; LX2[5] = zmin;
      /* 011 - 111 */
      LX3draw = 1;
      LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmax;

      /* 000 - 010 */
      LY1draw = 0; /* forbidden */
      LY1[0] = xmin; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmin; LY1[4] = ymax; LY1[5] = zmin;
      /* 100 - 110 */
      LY2draw = 1;
      LY2[0] = xmax; LY2[1] = ymin; LY2[2] = zmin; LY2[3] = xmax; LY2[4] = ymax; LY2[5] = zmin;
      /* 101 - 111 */
      LY3draw = 1;
      LY3[0] = xmax; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmax; LY3[4] = ymax; LY3[5] = zmax;

      /* 100 - 101 */
      LZ1draw = 1;
      LZ1[0] = xmax; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymin; LZ1[5] = zmax;
      /* 110 - 111 */
      LZ2draw = 1;
      LZ2[0] = xmax; LZ2[1] = ymax; LZ2[2] = zmin; LZ2[3] = xmax; LZ2[4] = ymax; LZ2[5] = zmax;
      /* 010 - 011 */
      LZ3draw = 1;
      LZ3[0] = xmin; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymax; LZ3[5] = zmax;

      break;
    }
    case 6: /* d101 */
    {
      /* 000 - 100 */
      LX1draw = 0; /* forbidden */
      LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmin;
      /* 001 - 101 */
      LX2draw = 1;
      LX2[0] = xmin; LX2[1] = ymin; LX2[2] = zmax; LX2[3] = xmax; LX2[4] = ymin; LX2[5] = zmax;
      /* 011 - 111 */
      LX3draw = 1;
      LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmax;

      /* 100 - 110 */
      LY1draw = 1;
      LY1[0] = xmax; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmax; LY1[4] = ymax; LY1[5] = zmin;
      /* 101 - 111 */
      LY2draw = 1;
      LY2[0] = xmax; LY2[1] = ymin; LY2[2] = zmax; LY2[3] = xmax; LY2[4] = ymax; LY2[5] = zmax;
      /* 001 - 011 */
      LY3draw = 1;
      LY3[0] = xmin; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmin; LY3[4] = ymax; LY3[5] = zmax;

      /* 000 - 001 */
      LZ1draw = 0; /* forbidden */
      LZ1[0] = xmin; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmin; LZ1[4] = ymin; LZ1[5] = zmax;
      /* 100 - 101 */
      LZ2draw = 1;
      LZ2[0] = xmax; LZ2[1] = ymin; LZ2[2] = zmin; LZ2[3] = xmax; LZ2[4] = ymin; LZ2[5] = zmax;
      /* 110 - 111 */
      LZ3draw = 1;
      LZ3[0] = xmax; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmax; LZ3[4] = ymax; LZ3[5] = zmax;

      break;
    }
    case 7: /* d011 */
    {
      /* 001 - 101 */
      LX1draw = 1;
      LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmax; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmax;
      /* 011 - 111 */
      LX2draw = 1;
      LX2[0] = xmin; LX2[1] = ymax; LX2[2] = zmax; LX2[3] = xmax; LX2[4] = ymax; LX2[5] = zmax;
      /* 010 - 110 */
      LX3draw = 1;
      LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmin; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmin;

      /* 000 - 010 */
      LY1draw = 0; /* forbidden */
      LY1[0] = xmin; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmin; LY1[4] = ymax; LY1[5] = zmin;
      /* 001 - 011 */
      LY2draw = 1;
      LY2[0] = xmin; LY2[1] = ymin; LY2[2] = zmax; LY2[3] = xmin; LY2[4] = ymax; LY2[5] = zmax;
      /* 101 - 111 */
      LY3draw = 1;
      LY3[0] = xmax; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmax; LY3[4] = ymax; LY3[5] = zmax;

      /* 000 - 001 */
      LZ1draw = 0; /* forbidden */
      LZ1[0] = xmin; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmin; LZ1[4] = ymin; LZ1[5] = zmax;
      /* 010 - 011 */
      LZ2draw = 1;
      LZ2[0] = xmin; LZ2[1] = ymax; LZ2[2] = zmin; LZ2[3] = xmin; LZ2[4] = ymax; LZ2[5] = zmax;
      /* 110 - 111 */
      LZ3draw = 1;
      LZ3[0] = xmax; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmax; LZ3[4] = ymax; LZ3[5] = zmax;

      break;
    }
    case 8: /* d111 */
    {
      /* 010 - 110 */
      LX1draw = 1;
      LX1[0] = xmin; LX1[1] = ymax; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymax; LX1[5] = zmin;
      /* 011 - 111 */
      LX2draw = 1;
      LX2[0] = xmin; LX2[1] = ymax; LX2[2] = zmax; LX2[3] = xmax; LX2[4] = ymax; LX2[5] = zmax;
      /* 001 - 101 */
      LX3draw = 1;
      LX3[0] = xmin; LX3[1] = ymin; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymin; LX3[5] = zmax;

      /* 100 - 110 */
      LY1draw = 1;
      LY1[0] = xmax; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmax; LY1[4] = ymax; LY1[5] = zmin;
      /* 101 - 111 */
      LY2draw = 1;
      LY2[0] = xmax; LY2[1] = ymin; LY2[2] = zmax; LY2[3] = xmax; LY2[4] = ymax; LY2[5] = zmax;
      /* 001 - 011 */
      LY3draw = 1;
      LY3[0] = xmin; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmin; LY3[4] = ymax; LY3[5] = zmax;

      /* 100 - 101 */
      LZ1draw = 1;
      LZ1[0] = xmax; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymin; LZ1[5] = zmax;
      /* 110 - 111 */
      LZ2draw = 1;
      LZ2[0] = xmax; LZ2[1] = ymax; LZ2[2] = zmin; LZ2[3] = xmax; LZ2[4] = ymax; LZ2[5] = zmax;
      /* 010 - 011 */
      LZ3draw = 1;
      LZ3[0] = xmin; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymax; LZ3[5] = zmax;

      break;
    }
  }

  // Draw the graduated trihedron
  unsigned int i, offset;
  float m1[3], m2[3];
  float step, dx, dy, dz;

  // Grid
  if (myToDrawGrid)
  {
    glColor3fv(myGridColor);
    glBegin(GL_LINES);
    // Boundary grid-lines
    if (LX1draw == 1)
    {
      glVertex3fv(&(LX1[0]));
      glVertex3fv(&(LX1[3]));
    }
    if (LX2draw == 1)
    {
      glVertex3fv(&(LX2[0]));
      glVertex3fv(&(LX2[3]));
    }
    if (LX3draw == 1)
    {
      glVertex3fv(&(LX3[0]));
      glVertex3fv(&(LX3[3]));
    }
    if (LY1draw == 1)
    {
      glVertex3fv(&(LY1[0]));
      glVertex3fv(&(LY1[3]));
    }
    if (LY2draw == 1)
    {
      glVertex3fv(&(LY2[0]));
      glVertex3fv(&(LY2[3]));
    }
    if (LY3draw == 1)
    {
      glVertex3fv(&(LY3[0]));
      glVertex3fv(&(LY3[3]));
    }
    if (LZ1draw == 1)
    {
      glVertex3fv(&(LZ1[0]));
      glVertex3fv(&(LZ1[3]));
    }
    if (LZ2draw == 1)
    {
      glVertex3fv(&(LZ2[0]));
      glVertex3fv(&(LZ2[3]));
    }
    if (LZ3draw == 1)
    {
      glVertex3fv(&(LZ3[0]));
      glVertex3fv(&(LZ3[3]));
    }
    glEnd();

    /* Intermediate grid-lines */
    /* X-Grid lines */
    if (myNbX > 0)
    {
      i = myToDrawAxes ? 1 : 0;
      step = fabsf(LX1[3] - LX1[0]) / (float) myNbX;
      while (i < myNbX)
      {
        glBegin(GL_LINE_STRIP);
        glVertex3f(LX1[0] + i * step, LX1[1], LX1[2]);
        glVertex3f(LX2[0] + i * step, LX2[1], LX2[2]);
        glVertex3f(LX3[0] + i * step, LX3[1], LX3[2]);
        glEnd();
        i++;
      }
    }
    /* Y-Grid lines */
    if (myNbY > 0)
    {
      i = myToDrawAxes ? 1 : 0;
      step = fabsf(LY1[4] - LY1[1]) / (float) myNbY;
      while (i < myNbY)
      {
        glBegin(GL_LINE_STRIP);
        glVertex3f(LY1[0], LY1[1] + i * step, LY1[2]);
        glVertex3f(LY2[0], LY2[1] + i * step, LY2[2]);
        glVertex3f(LY3[0], LY3[1] + i * step, LY3[2]);
        glEnd();
        i++;
      }
    }
    /* Z-Grid lines */
    if (myNbZ > 0)
    {
      i = myToDrawAxes ? 1 : 0;
      step = fabsf(LZ1[5] - LZ1[2]) / (float) myNbZ;

      while (i < myNbZ)
      {
        glBegin(GL_LINE_STRIP);
        glVertex3f(LZ1[0], LZ1[1], LZ1[2] + i * step);
        glVertex3f(LZ2[0], LZ2[1], LZ2[2] + i * step);
        glVertex3f(LZ3[0], LZ3[1], LZ3[2] + i * step);
        glEnd();
        i++;
      }
    }
  }

  // Axes (arrows)
  if (myToDrawAxes)
  {
    // X-axis
    glColor3fv(myXColor.rgb);
    drawArrow(xmin, ymin, zmin, xmax, ymin, zmin, normal[0], normal[1], normal[2]);

    // Y-axis
    glColor3fv(myYColor.rgb);
    drawArrow(xmin, ymin, zmin, xmin, ymax, zmin, normal[0], normal[1], normal[2]);

    // Z-axis
    glColor3fv(myZColor.rgb);
    drawArrow(xmin, ymin, zmin, xmin, ymin, zmax, normal[0], normal[1], normal[2]);
  }

  // Names of axes & values
  char textValue[128];

  if (myToDrawXName || myToDrawXValues)
  {
    // Middle point of the first X-axis
    m1[0] = 0.5f * (LX1[0] + LX1[3]);
    m1[1] = 0.5f * (LX1[1] + LX1[4]);
    m1[2] = 0.5f * (LX1[2] + LX1[5]);

    // Middle point of the second X-axis
    m2[0] = 0.5f * (LX2[0] + LX2[3]);
    m2[1] = 0.5f * (LX2[1] + LX2[4]);
    m2[2] = 0.5f * (LX2[2] + LX2[5]);

    // Apply offset to m1
    dy = m1[1] - m2[1];
    if (fabsf(dy) > 1.e-7f)
    {
      dy = (dy > 0.0f)? 1.0f : -1.0f;
    }
    dz = m1[2] - m2[2];
    if (fabsf(dz) > 1.e-7f)
    {
      dz = (dz > 0.0f)? 1.0f : -1.0f;
    }
    m2[1] = dpix * dy;
    m2[2] = dpix * dz;

    // Name of X-axis
    if (myToDrawXName)
    {
      offset = myXAxisOffset + myXTickmarkLength;

      // draw axes labels
      myAspectLabels.ChangeColor() = myXNameColor;
      const OpenGl_AspectText* aPrevAspectText = theWorkspace->SetAspectText (&myAspectLabels);
      myLabelX.SetPosition (OpenGl_Vec3(m1[0], m1[1] + offset * m2[1], m1[2] + offset * m2[2]));
      myLabelX.Render (theWorkspace);
      theWorkspace->SetAspectText (aPrevAspectText);
    }

    // X-values
    if (myToDrawXValues && myNbX > 0)
    {
      myAspectValues.ChangeColor() = myXColor;
      const OpenGl_AspectText* aPrevAspectText = theWorkspace->SetAspectText (&myAspectValues);

      step = fabsf(LX1[3] - LX1[0]) / (float) myNbX;
      offset = myXOffset + myXTickmarkLength;
      for (unsigned int anIter = 0; anIter <= myNbX; ++anIter)
      {
        sprintf (textValue, "%g", LX1[0] + anIter * step);
        myLabelValues.Init (theWorkspace->GetGlContext(), textValue,
                            OpenGl_Vec3(LX1[0] + anIter * step, m1[1] + offset * m2[1], m1[2] + offset * m2[2]));
        myLabelValues.Render (theWorkspace);
      }
      theWorkspace->SetAspectText (aPrevAspectText);
    }

    // X-tickmark
    if (myDrawXTickmarks && myNbX > 0)
    {
      glColor3fv(myGridColor);

      step = fabsf(LX1[3] - LX1[0]) / (float) myNbX;
      for (unsigned int anIter = 0; anIter <= myNbX; ++anIter)
      {
        glBegin(GL_LINES);
        glVertex3f(LX1[0] + anIter * step, m1[1],                             m1[2]);
        glVertex3f(LX1[0] + anIter * step, m1[1] + myXTickmarkLength * m2[1], m1[2] + myXTickmarkLength * m2[2]);
        glEnd();
      }
    }
  }

  if (myToDrawYName || myToDrawYValues)
  {
    // Middle point of the first Y-axis
    m1[0] = 0.5f * (LY1[0] + LY1[3]);
    m1[1] = 0.5f * (LY1[1] + LY1[4]);
    m1[2] = 0.5f * (LY1[2] + LY1[5]);

    // Middle point of the second Y-axis
    m2[0] = 0.5f * (LY2[0] + LY2[3]);
    m2[1] = 0.5f * (LY2[1] + LY2[4]);
    m2[2] = 0.5f * (LY2[2] + LY2[5]);

    // Apply offset to m1
    dx = m1[0] - m2[0];
    if (fabsf(dx) > 1.e-7f)
    {
      dx = (dx > 0.0f)? 1.0f : -1.0f;
    }
    dz = m1[2] - m2[2];
    if (fabsf(dz) > 1.e-7f)
    {
      dz = (dz > 0.0f)? 1.0f : -1.0f;
    }

    m2[0] = dpix * dx;
    m2[2] = dpix * dz;

    // Name of Y-axis
    if (myToDrawYName)
    {
      offset = myYAxisOffset + myYTickmarkLength;

      myAspectLabels.ChangeColor() = myYNameColor;
      const OpenGl_AspectText* aPrevAspectText = theWorkspace->SetAspectText (&myAspectLabels);
      myLabelY.SetPosition (OpenGl_Vec3(m1[0] + offset * m2[0], m1[1], m1[2] + offset * m2[2]));
      myLabelY.Render (theWorkspace);
      theWorkspace->SetAspectText (aPrevAspectText);
    }

    // Y-values
    if (myToDrawYValues && myNbY > 0)
    {
      myAspectValues.ChangeColor() = myYColor;
      const OpenGl_AspectText* aPrevAspectText = theWorkspace->SetAspectText (&myAspectValues);

      step = fabsf(LY1[4] - LY1[1]) / (float) myNbY;
      offset = myYOffset + myYTickmarkLength;
      for (unsigned int anIter = 0; anIter <= myNbY; ++anIter)
      {
        sprintf (textValue, "%g", LY1[1] + anIter * step);
        myLabelValues.Init (theWorkspace->GetGlContext(), textValue,
                            OpenGl_Vec3(m1[0] + offset * m2[0], LY1[1] + anIter * step, m1[2] + offset * m2[2]));
        myLabelValues.Render (theWorkspace);
      }
      theWorkspace->SetAspectText (aPrevAspectText);
    }

    // Y-tickmark
    if (myDrawYTickmarks && myNbY > 0)
    {
      glColor3fv(myGridColor);

      i = 0;
      step = fabsf(LY1[4] - LY1[1]) / (float) myNbY;
      while (i <= myNbY)
      {
        glBegin(GL_LINES);
        glVertex3f(m1[0],                             LY1[1] + i * step, m1[2]);
        glVertex3f(m1[0] + myYTickmarkLength * m2[0], LY1[1] + i * step, m1[2] + myYTickmarkLength * m2[2]);
        glEnd();
        i++;
      }
    }
  }

  if (myToDrawZName || myToDrawZValues)
  {
    // Middle point of the first Z-axis
    m1[0] = 0.5f * (LZ1[0] + LZ1[3]);
    m1[1] = 0.5f * (LZ1[1] + LZ1[4]);
    m1[2] = 0.5f * (LZ1[2] + LZ1[5]);

    // Middle point of the second Z-axis
    m2[0] = 0.5f * (LZ2[0] + LZ2[3]);
    m2[1] = 0.5f * (LZ2[1] + LZ2[4]);
    m2[2] = 0.5f * (LZ2[2] + LZ2[5]);

    // Apply offset to m1
    dx = m1[0] - m2[0];
    if (fabsf(dx) > 1.e-7f)
    {
      dx = (dx > 0.0f)? 1.0f : -1.0f;
    }
    dy = m1[1] - m2[1];
    if (fabsf(dy) > 1.e-7f)
    {
      dy = (dy > 0.0f)? 1.0f : -1.0f;
    }

    m2[0] = dpix * dx;
    m2[1] = dpix * dy;

    // Name of Z-axis
    if (myToDrawZName)
    {
      offset = myZAxisOffset + myZTickmarkLength;

      myAspectLabels.ChangeColor() = myZNameColor;
      const OpenGl_AspectText* aPrevAspectText = theWorkspace->SetAspectText (&myAspectLabels);
      myLabelZ.SetPosition (OpenGl_Vec3(m1[0] + offset * m2[0], m1[1] + offset * m2[1], m1[2]));
      myLabelZ.Render (theWorkspace);
      theWorkspace->SetAspectText (aPrevAspectText);
    }

    // Z-values
    if (myToDrawZValues && myNbZ > 0)
    {
      myAspectValues.ChangeColor() = myZColor;
      const OpenGl_AspectText* aPrevAspectText = theWorkspace->SetAspectText (&myAspectValues);

      step = fabsf(LZ1[5] - LZ1[2]) / (float) myNbZ;
      offset = myZOffset + myZTickmarkLength;
      for (unsigned int anIter = 0; anIter <= myNbZ; ++anIter)
      {
        sprintf (textValue, "%g", LZ1[2] + anIter * step);
        myLabelValues.Init (theWorkspace->GetGlContext(), textValue,
                            OpenGl_Vec3(m1[0] + offset * m2[0], m1[1] + offset * m2[1], LZ1[2] + anIter * step));
        myLabelValues.Render (theWorkspace);
      }
      theWorkspace->SetAspectText (aPrevAspectText);
    }

    // Z-tickmark
    if (myDrawZTickmarks && myNbZ > 0)
    {
      glColor3fv(myGridColor);

      i = 0;
      step = fabsf(LZ1[5] - LZ1[2]) / (float) myNbZ;
      while (i <= myNbZ)
      {
        glBegin(GL_LINES);
        glVertex3f(m1[0],                             m1[1],                             LZ1[2] + i * step);
        glVertex3f(m1[0] + myZTickmarkLength * m2[0], m1[1] + myZTickmarkLength * m2[1], LZ1[2] + i * step);
        glEnd();
        i++;
      }
    }
  }

  // Activate the lighting if it was turned off by this method call
  if (light)
    glEnable(GL_LIGHTING);

  theWorkspace->SetAspectLine(oldAspectLine);
}

//call_graduatedtrihedron_minmaxvalues
void OpenGl_GraduatedTrihedron::SetMinMax (const Standard_ShortReal xMin, const Standard_ShortReal yMin, const Standard_ShortReal zMin,
                                          const Standard_ShortReal xMax, const Standard_ShortReal yMax, const Standard_ShortReal zMax)
{
  xmin = xMin;
  ymin = yMin;
  zmin = zMin;
  xmax = xMax;
  ymax = yMax;
  zmax = zMax;
}
