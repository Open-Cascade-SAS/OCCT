// Created on: 2008-03-20
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <NIS_SurfaceDrawer.hxx>
#include <NIS_Surface.hxx>
#include <NIS_InteractiveObject.hxx>
#include <Standard_ProgramError.hxx>

#include <OpenGl_GlCore11.hxx>

static void setColor(GLenum               theFace,
                     Quantity_Parameter * theAmbient,
                     const Standard_Real  theSpecularity,
                     GLint                theShininess);

IMPLEMENT_STANDARD_HANDLE  (NIS_SurfaceDrawer, NIS_Drawer)
IMPLEMENT_STANDARD_RTTIEXT (NIS_SurfaceDrawer, NIS_Drawer)

//=======================================================================
//function : NIS_SurfaceDrawer()
//purpose  : Constructor
//=======================================================================

NIS_SurfaceDrawer::NIS_SurfaceDrawer
                                (const Quantity_Color   &theNormal,
                                 const Quantity_Color   &theHilight,
                                 const Quantity_Color   &theDynHilight)
  : myBackColor     (theNormal),
    myPolygonOffset (0.f),
    myIsWireframe   (Standard_False)
{
  myColor[Draw_Normal]       = theNormal;
  myColor[Draw_Top]          = theNormal;
  myColor[Draw_Transparent]  = theNormal;
  myColor[Draw_Hilighted]    = theHilight;
  myColor[Draw_DynHilighted] = theDynHilight;
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::SetColor(const Quantity_Color &theColor)
{
  myColor[Draw_Normal]      = theColor;
  myColor[Draw_Top]         = theColor;
  myColor[Draw_Transparent] = theColor;
}

//=======================================================================
//function : Assign
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::Assign (const Handle(NIS_Drawer)& theOther)
{
  if (theOther.IsNull() == Standard_False) {
    NIS_Drawer::Assign (theOther);
    const Handle(NIS_SurfaceDrawer)& anOther =
      static_cast <const Handle(NIS_SurfaceDrawer)&> (theOther);
    myColor[Draw_Normal]       = anOther->myColor[Draw_Normal];
    myColor[Draw_Top]          = anOther->myColor[Draw_Top];
    myColor[Draw_Transparent]  = anOther->myColor[Draw_Transparent];
    myColor[Draw_Hilighted]    = anOther->myColor[Draw_Hilighted];
    myColor[Draw_DynHilighted] = anOther->myColor[Draw_DynHilighted];
    myBackColor                = anOther->myBackColor;
    myPolygonOffset            = anOther->myPolygonOffset;
    myIsWireframe              = anOther->myIsWireframe;
  }
}

//=======================================================================
//function : IsEqual
//purpose  : Comparison of two Drawers (for Map impementation)
//=======================================================================

Standard_Boolean NIS_SurfaceDrawer::IsEqual
                                (const Handle(NIS_Drawer)& theOther)const
{
  static const Standard_Real       anEpsilon2 (1e-7);
  Standard_Boolean                 aResult (Standard_False);
  const Handle(NIS_SurfaceDrawer) anOther =
                        Handle(NIS_SurfaceDrawer)::DownCast (theOther);
  if (NIS_Drawer::IsEqual(theOther))
    aResult = (anOther->myColor[Draw_Normal]
               .SquareDistance (myColor[Draw_Normal]) < anEpsilon2 &&
               anOther->myColor[Draw_Hilighted]
               .SquareDistance (myColor[Draw_Hilighted]) < anEpsilon2 &&
               anOther->myColor[Draw_DynHilighted]
               .SquareDistance (myColor[Draw_DynHilighted]) < anEpsilon2 &&
               anOther->myBackColor.SquareDistance(myBackColor) < anEpsilon2 &&
               fabs(anOther->myPolygonOffset - myPolygonOffset) < 0.999 &&
               myIsWireframe == anOther->myIsWireframe);
  if (aResult) {
    // Arbitrary point for test
    gp_XYZ aPnt[2] = {
      gp_XYZ(113., -31.3, 29.19),
      gp_XYZ(113., -31.3, 29.19)
    };
    anOther->myTrsf.Transforms(aPnt[0]);
    myTrsf.Transforms(aPnt[1]);
    if ((aPnt[0] - aPnt[1]).SquareModulus() > anEpsilon2)
      aResult = Standard_False;
  }
  return aResult;
}

//=======================================================================
//function : redraw
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::redraw  (const DrawType         theType,
                                 const Handle(NIS_View)& theView)
{
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();

  GLdouble aMatrix[16] = {
    myTrsf.Value(1,1), myTrsf.Value(2,1), myTrsf.Value(3,1),  0.,
    myTrsf.Value(1,2), myTrsf.Value(2,2), myTrsf.Value(3,2),  0.,
    myTrsf.Value(1,3), myTrsf.Value(2,3), myTrsf.Value(3,3),  0.,
    myTrsf.Value(1,4), myTrsf.Value(2,4), myTrsf.Value(3,4),  1.
  };

  glMultMatrixd( aMatrix );

  NIS_Drawer::redraw(theType, theView);

  glPopMatrix();
}

//=======================================================================
//function : BeforeDraw
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::BeforeDraw (const DrawType      theType,
                                    const NIS_DrawList&)
{
  glEnable(GL_LIGHTING);
  //  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
  glEnableClientState (GL_VERTEX_ARRAY);
  if (myIsWireframe == Standard_False) {
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glShadeModel(GL_SMOOTH);
  }

  Quantity_Parameter   aValueCol[4] = {0., 0., 0., 1.};
  Quantity_TypeOfColor bidTC (Quantity_TOC_RGB);
  GLfloat aLineWidth (1.f);
  GLfloat anOffset = myPolygonOffset;
  static const GLfloat gColorN[4]  = {0.f, 0.f, 0.f, 1.f};

  switch (theType) {
  case Draw_DynHilighted:
    aLineWidth = 3.f;
    myColor[theType].Values (aValueCol[0], aValueCol[1], aValueCol[2], bidTC);
    setColor(GL_FRONT_AND_BACK, &aValueCol[0], 0.1, 5);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &gColorN[0]);
    glLineWidth (aLineWidth);
    if (myIsWireframe == Standard_False)
      glPolygonOffset(1.f, -(anOffset + 11.f));
    return;
  case Draw_Hilighted:
    anOffset += 10.f;
  case Draw_Normal:
  case Draw_Top:
  case Draw_Transparent:
    if (myIsWireframe == Standard_False) {
      glPolygonOffset(1.f, -anOffset);
      glEnableClientState (GL_NORMAL_ARRAY);
    }
    myColor[theType].Values (aValueCol[0], aValueCol[1], aValueCol[2], bidTC);
    aValueCol[3] = 1. - myTransparency;
    if (theType == Draw_Transparent) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      // don't write triangles into depth test
      glDepthMask(GL_FALSE);
    }
    break;
  default:
    return;
  }
  //  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
  if (theType == Draw_Hilighted ||
      myBackColor.SquareDistance(myColor[Draw_Normal]) < 1.e-7)
  {
    setColor(GL_FRONT_AND_BACK, &aValueCol[0], 0.5, 10);
  } else {
    setColor(GL_FRONT, &aValueCol[0], 0.4, 10);
    myBackColor.Values (aValueCol[0], aValueCol[1], aValueCol[2], bidTC);
    setColor(GL_BACK, &aValueCol[0], 0.8, 5);
  }
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &gColorN[0]);
  glLineWidth (aLineWidth);
}

//=======================================================================
//function : AfterDraw
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::AfterDraw (const DrawType      theType,
                                   const NIS_DrawList&)
{
  glDisable(GL_LIGHTING);
  glDisableClientState(GL_VERTEX_ARRAY);
  switch (theType) {
    case Draw_Transparent:
      glDisable(GL_BLEND);
      glDepthMask(GL_TRUE);
    case Draw_Hilighted:
    case Draw_Normal:
    case Draw_Top:
      if (myIsWireframe == Standard_False)
        glDisableClientState(GL_NORMAL_ARRAY);
    default:;
  }
  if (myIsWireframe == Standard_False)
    glDisable(GL_POLYGON_OFFSET_FILL);
}

//=======================================================================
//function : Draw
//purpose  :
//=======================================================================

void NIS_SurfaceDrawer::Draw (const Handle(NIS_InteractiveObject)& theObj,
                              const DrawType                      theType,
                              const NIS_DrawList&)
{
  // Assertion for the type of the drawn object
  Standard_ProgramError_Raise_if (! theObj->IsKind(STANDARD_TYPE(NIS_Surface)),
                                  "NIS_Surface::Draw: irrelevant object type");

  const NIS_Surface * pObject =
    static_cast <const NIS_Surface *> (theObj.operator->());
  glVertexPointer (3, GL_FLOAT, 0, pObject->Node(0));

  // In Highlited mode the shape must be shown as wireframe
  Standard_Boolean isWireframe(myIsWireframe);
  if (isWireframe == Standard_False && theType == Draw_DynHilighted)
    if (pObject->NEdges() > 0)
      isWireframe = Standard_True;

  if (isWireframe)
  {
    for (Standard_Integer i = 0; i < pObject->NEdges(); i++) {
      const GLint * pEdge = static_cast<const GLint *> (pObject->Edge(i));
      glDrawElements (GL_LINE_STRIP, pEdge[0], GL_UNSIGNED_INT, &pEdge[1]);
    }
  } else {
    if (pObject->NTriangles()) {
      if (theType != Draw_DynHilighted)
        glNormalPointer (GL_FLOAT, 0, pObject->Normal(0));
      glDrawElements (GL_TRIANGLES, pObject->NTriangles()*3,
                      GL_UNSIGNED_INT, pObject->Triangle(0));
    }
  }
}

//=======================================================================
//function : setColor
//purpose  :
//=======================================================================

void setColor(GLenum               theFace,
              Quantity_Parameter * theAmbient,
              const Standard_Real  theSpecularity,
              GLint                theShininess)
{
  GLfloat aSpec = static_cast<GLfloat>(theSpecularity);
  GLfloat aValueCol[4] = {
    GLfloat(theAmbient[0]),
    GLfloat(theAmbient[1]),
    GLfloat(theAmbient[2]),
    GLfloat(theAmbient[3])
  };
  glMaterialfv(theFace, GL_AMBIENT_AND_DIFFUSE, &aValueCol[0]);
  aValueCol[0] = aSpec * (aValueCol[0] - 1.f) + 1.f;
  aValueCol[1] = aSpec * (aValueCol[1] - 1.f) + 1.f;
  aValueCol[2] = aSpec * (aValueCol[2] - 1.f) + 1.f;
  aValueCol[3] = 1.f;
  glMaterialfv(theFace, GL_SPECULAR, &aValueCol[0]);
  glMateriali(theFace, GL_SHININESS, theShininess);
}

