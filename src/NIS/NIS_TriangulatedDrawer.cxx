// File:      NIS_TriangulatedDrawer.cxx
// Created:   19.07.07 12:09
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007

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
    myIsDrawPolygons (Standard_False)
{
  myColor[Draw_Normal]       = theNormal;
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
    myColor[Draw_Transparent]  = anOther->myColor[Draw_Transparent];
    myColor[Draw_Hilighted]    = anOther->myColor[Draw_Hilighted];
    myColor[Draw_DynHilighted] = anOther->myColor[Draw_DynHilighted];
    myLineWidth                = anOther->myLineWidth;
    myIsDrawPolygons           = anOther->myIsDrawPolygons;
  }
}

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
               anOther->myIsDrawPolygons == myIsDrawPolygons);
  return aResult;
}

//=======================================================================
//function : BeforeDraw
//purpose  : 
//=======================================================================

void NIS_TriangulatedDrawer::BeforeDraw (const DrawType      theType,
                                         const NIS_DrawList&)
{
  Quantity_Parameter   aValue[3];
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
    if (myIsDrawPolygons)
      glEnable(GL_POLYGON_OFFSET_LINE);
    else
      glEnable(GL_POLYGON_OFFSET_FILL);
    if (theType == Draw_Hilighted)
    {
#ifdef NEGATIVE_POFFSET
      anOffsetHilighted = -10;
#else
      anOffsetHilighted = 1;
#endif
    }
    break;
  case Draw_Normal:
  case Draw_Transparent:
#ifndef NEGATIVE_POFFSET
    anOffsetHilighted = 11;
#endif
    break;
  default:
    return;
  }
  if (anOffsetHilighted)
    glPolygonOffset(1.f, static_cast<GLfloat>(anOffsetHilighted));

  myColor[theType].Values (aValue[0], aValue[1], aValue[2], bidTC);
  glColor3d (aValue[0], aValue[1], aValue[2]);
  if (myIsDrawPolygons)
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
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
    if (myIsDrawPolygons)
      glDisable(GL_POLYGON_OFFSET_LINE);
    else
      glDisable(GL_POLYGON_OFFSET_FILL);
    case Draw_Normal:
    case Draw_Transparent:
      glDisableClientState(GL_VERTEX_ARRAY);
    default:;
  }
  if (myIsDrawPolygons)
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
  glVertexPointer (3, GL_FLOAT, 0, pObject->Node(0));
  if (myIsDrawPolygons == Standard_False) {
    if (pObject->IsTriangulation())
      glDrawElements (GL_TRIANGLES, pObject->NTriangles()*3,
                      GL_UNSIGNED_INT, pObject->Triangle(0));
  } else {
    if (pObject->IsPolygons()) {
      const Standard_Integer nPoly = pObject->NPolygons();
      for (Standard_Integer i = 0; i < nPoly; i++) {
        Standard_Integer * arrNodes;
        const Standard_Integer nSize = pObject->Polygon (i, arrNodes);
        glDrawElements (GL_LINE_LOOP, nSize, GL_UNSIGNED_INT, arrNodes);
      }
    }
  }
  if (pObject->IsSegments())
    glDrawElements (GL_LINES, pObject->NLineNodes(),
                    GL_UNSIGNED_INT, pObject->LineNode(0));
  else {
    Standard_Boolean isLoop;
    if (pObject->IsLine(isLoop))
      if (isLoop) {
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements (GL_LINE_LOOP, pObject->NLineNodes(),
                        GL_UNSIGNED_INT, pObject->LineNode(0));
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      } else {
        glDrawElements (GL_LINE_STRIP, pObject->NLineNodes(),
                        GL_UNSIGNED_INT, pObject->LineNode(0));
      }
  }
}
