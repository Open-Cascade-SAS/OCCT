// Created on: 2007-07-19
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <NIS_TriangulatedDrawer.hxx>
#include <NIS_InteractiveObject.hxx>
#include <NIS_Triangulated.hxx>
#ifdef _DEBUG
#include <Standard_ProgramError.hxx>
#endif
#ifdef WNT
#include <windows.h>
#endif
#include <GL/gl.h>

// Comment this line if you see no hilighting of triangulations due to negative
// polygon offsets. Disabling this macro means that all offsets will be created
// in the positive scale.

// But before changing this macro please play with your OpenGL video card
// settings in the direction of performance improvement. Particularly, I had a
// good result after checking "Enable write combining" option in NVIDIA 6600. 

#define NEGATIVE_POFFSET

IMPLEMENT_STANDARD_HANDLE  (NIS_TriangulatedDrawer, NIS_Drawer)
IMPLEMENT_STANDARD_RTTIEXT (NIS_TriangulatedDrawer, NIS_Drawer)

//=======================================================================
//function : NIS_TriangulatedDrawer()
//purpose  : Constructor
//=======================================================================

NIS_TriangulatedDrawer::NIS_TriangulatedDrawer
                                (const Quantity_Color theNormal,
                                 const Quantity_Color theHilight,
                                 const Quantity_Color theDynHilight)
  : myLineWidth      (1.f),
    myIsDrawPolygons (Standard_False),
    myPolygonType    (NIS_Triangulated::Polygon_Default),
    myPolygonAsLineLoop (Standard_False)
{
  myColor[Draw_Normal]       = theNormal;
  myColor[Draw_Top]          = theNormal;
  myColor[Draw_Transparent]  = theNormal;
  myColor[Draw_Hilighted]    = theHilight;
  myColor[Draw_DynHilighted] = theDynHilight;
}

//=======================================================================
//function : Assign
//purpose  : 
//=======================================================================

void NIS_TriangulatedDrawer::Assign (const Handle_NIS_Drawer& theOther)
{
  if (theOther.IsNull() == Standard_False) {
    NIS_Drawer::Assign (theOther);
    const Handle(NIS_TriangulatedDrawer)& anOther =
      static_cast <const Handle(NIS_TriangulatedDrawer)&> (theOther);
    myColor[Draw_Normal]       = anOther->myColor[Draw_Normal];
    myColor[Draw_Top]          = anOther->myColor[Draw_Top];
    myColor[Draw_Transparent]  = anOther->myColor[Draw_Transparent];
    myColor[Draw_Hilighted]    = anOther->myColor[Draw_Hilighted];
    myColor[Draw_DynHilighted] = anOther->myColor[Draw_DynHilighted];
    myLineWidth                = anOther->myLineWidth;
    myIsDrawPolygons           = anOther->myIsDrawPolygons;
    myPolygonType              = anOther->myPolygonType;
  }
}

static const Standard_Integer nObjPerDrawer = 64;

//=======================================================================
//function : IsEqual
//purpose  : Comparison of two Drawers (for Map impementation)
//=======================================================================

Standard_Boolean NIS_TriangulatedDrawer::IsEqual
                                (const Handle_NIS_Drawer& theOther)const
{
  static const Standard_Real anEpsilon2 (1e-7);
  Standard_Boolean aResult (Standard_False);
  const Handle(NIS_TriangulatedDrawer) anOther =
    Handle(NIS_TriangulatedDrawer)::DownCast (theOther);
  if (NIS_Drawer::IsEqual(theOther))
    aResult = (anOther->myColor[Draw_Normal]
               .SquareDistance (myColor[Draw_Normal]) < anEpsilon2 &&
               anOther->myColor[Draw_Hilighted]
               .SquareDistance (myColor[Draw_Hilighted]) < anEpsilon2 &&
               anOther->myColor[Draw_DynHilighted]
               .SquareDistance (myColor[Draw_DynHilighted]) < anEpsilon2 &&
               (anOther->myLineWidth - myLineWidth) *
               (anOther->myLineWidth - myLineWidth) < 0.01 &&
               anOther->myIsDrawPolygons == myIsDrawPolygons &&
               anOther->myPolygonType == myPolygonType);
  return aResult;
}

//=======================================================================
//function : BeforeDraw
//purpose  : 
//=======================================================================

void NIS_TriangulatedDrawer::BeforeDraw (const DrawType      theType,
                                         const NIS_DrawList&)
{
  Quantity_Parameter   aValue[4];
  Quantity_TypeOfColor bidTC (Quantity_TOC_RGB);
  GLfloat aLineWidth (myLineWidth);
  Standard_Integer anOffsetHilighted = 0;
  switch (theType) {
  case Draw_DynHilighted:
    aLineWidth = myLineWidth + 1.f;
#ifdef NEGATIVE_POFFSET
    anOffsetHilighted = -11;
#endif
  case Draw_Hilighted:
    switch (myPolygonType) {
      default:
      case NIS_Triangulated::Polygon_Default:
        if (myIsDrawPolygons) {
      case NIS_Triangulated::Polygon_Line:
          glEnable(GL_POLYGON_OFFSET_LINE);
        } else {
      case NIS_Triangulated::Polygon_Fill:
          glEnable(GL_POLYGON_OFFSET_FILL);
        }
    }
    if (theType == Draw_Hilighted)
    {
#ifdef NEGATIVE_POFFSET
      anOffsetHilighted = -10;
#else
      anOffsetHilighted = 1;
#endif
    }
    myColor[theType].Values (aValue[0], aValue[1], aValue[2], bidTC);
    glColor3d (aValue[0], aValue[1], aValue[2]);
    break;
  case Draw_Normal:
  case Draw_Top:
  case Draw_Transparent:
#ifndef NEGATIVE_POFFSET
    anOffsetHilighted = 11;
#endif
    myColor[theType].Values (aValue[0], aValue[1], aValue[2], bidTC);
    aValue[3] = 1. - myTransparency;
    if (myTransparency > 0.01) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    glColor4d (aValue[0], aValue[1], aValue[2], aValue[3]);
    break;
  default:
    return;
  }
  if (anOffsetHilighted)
    glPolygonOffset(1.f, static_cast<GLfloat>(anOffsetHilighted));

// myColor[theType].Values (aValue[0], aValue[1], aValue[2], bidTC);
// glColor3d (aValue[0], aValue[1], aValue[2]);
    switch (myPolygonType) {
      default:
      case NIS_Triangulated::Polygon_Default:
        if (myIsDrawPolygons) {
      case NIS_Triangulated::Polygon_Line:
          glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        } else {
      case NIS_Triangulated::Polygon_Fill:
          glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        }
    }
  glEnableClientState (GL_VERTEX_ARRAY);
  glLineWidth (aLineWidth);
  glShadeModel(GL_FLAT);
  glDisable(GL_LIGHTING);
}

//=======================================================================
//function : AfterDraw
//purpose  : 
//=======================================================================

void NIS_TriangulatedDrawer::AfterDraw (const DrawType      theType,
                                        const NIS_DrawList&)
{
  switch (theType) {
    case Draw_Hilighted:
    case Draw_DynHilighted:
    switch (myPolygonType) {
      default:
      case NIS_Triangulated::Polygon_Default:
        if (myIsDrawPolygons) {
      case NIS_Triangulated::Polygon_Line:
          glDisable(GL_POLYGON_OFFSET_LINE);
        } else {
      case NIS_Triangulated::Polygon_Fill:
          glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }
    case Draw_Normal:
    case Draw_Top:
    case Draw_Transparent:
      glDisableClientState(GL_VERTEX_ARRAY);
      if (myTransparency > 0.01)
        glDisable(GL_BLEND);
    default:;
  }
  if (myPolygonType == NIS_Triangulated::Polygon_Line ||
      (myPolygonType == NIS_Triangulated::Polygon_Default && myIsDrawPolygons))
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================

void NIS_TriangulatedDrawer::Draw (const Handle_NIS_InteractiveObject& theObj,
                                   const DrawType                   /*theType*/,
                                   const NIS_DrawList&)
{
  // Assertion for the type of the drawn object
#ifdef _DEBUG
  static const Handle(Standard_Type) ThisType = STANDARD_TYPE(NIS_Triangulated);
  if (theObj->IsKind(ThisType) == Standard_False)
    Standard_ProgramError::Raise ("NIS_Triangulated::Draw: "
                                  "irrelevant object type");
#endif
  const NIS_Triangulated * pObject =
    static_cast <const NIS_Triangulated *> (theObj.operator->());
  glVertexPointer (pObject->myNodeCoord, GL_FLOAT, 0, pObject->Node(0));

  GLenum aType = GL_UNSIGNED_INT;
  if (pObject->myIndexType == 0)
    aType = GL_UNSIGNED_BYTE;
  else if (pObject->myIndexType == 1)
    aType = GL_UNSIGNED_SHORT;

  if (myIsDrawPolygons == Standard_False) {
    if (pObject->IsTriangulation()) {
      glDrawElements (GL_TRIANGLES, pObject->NTriangles()*3,
                      aType, pObject->mypTriangles);
    }
  } else {
    if (pObject->IsPolygons()) {
      GLenum aMode = GL_POLYGON;
      if (myPolygonAsLineLoop &&
          (myPolygonType == NIS_Triangulated::Polygon_Line ||
           (myPolygonType == NIS_Triangulated::Polygon_Default &&
            myIsDrawPolygons)))
        aMode = GL_LINE_LOOP;
      const Standard_Integer nPoly = pObject->NPolygons();
      for (Standard_Integer i = 0; i < nPoly; i++) {
        const Standard_Integer nSize = pObject->NPolygonNodes(i);
        void* anArray;
        if (pObject->myIndexType == 0)
          anArray = reinterpret_cast<unsigned char *>(pObject->mypPolygons[i]) + 1;
        else if (pObject->myIndexType == 1)
          anArray = reinterpret_cast<unsigned short *>(pObject->mypPolygons[i]) + 1;
        else
          anArray = pObject->mypPolygons[i] + 1;
        glDrawElements (aMode, nSize, aType, anArray);
      }
    }
  }
  if (pObject->IsSegments())
    glDrawElements (GL_LINES, pObject->NLineNodes(),
                    aType, pObject->mypLines);
  else {
    Standard_Boolean isLoop;
    if (pObject->IsLine(isLoop))
      if (isLoop) {
//         glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements (GL_LINE_LOOP, pObject->NLineNodes(),
                        aType, pObject->mypLines);
//         glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      } else {
        glDrawElements (GL_LINE_STRIP, pObject->NLineNodes(),
                        aType, pObject->mypLines);
      }
  }

}

