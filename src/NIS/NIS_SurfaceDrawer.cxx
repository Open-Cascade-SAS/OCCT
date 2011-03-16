// File:      NIS_SurfaceDrawer.cpp
// Created:   20.03.08 09:09
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade S.A. 2008

#include <NIS_SurfaceDrawer.hxx>
#include <NIS_Surface.hxx>
#include <NIS_InteractiveObject.hxx>
#include <Standard_ProgramError.hxx>

#ifdef WNT
#include <windows.h>
#endif
#include <GL/gl.h>

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
    myTransparency  (0.0),
    myPolygonOffset (0.f)
{
  myColor[Draw_Normal]       = theNormal;
  myColor[Draw_Transparent]  = theNormal;
  myColor[Draw_Hilighted]    = theHilight;
  myColor[Draw_DynHilighted] = theDynHilight;
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void NIS_SurfaceDrawer::SetColor(const Quantity_Color &theColor,
                                       const Standard_Real   theTransparency)
{
  myColor[Draw_Normal]      = theColor;
  myColor[Draw_Transparent] = theColor;
  myTransparency            = theTransparency;
}

//=======================================================================
//function : Assign
//purpose  : 
//=======================================================================

void NIS_SurfaceDrawer::Assign (const Handle_NIS_Drawer& theOther)
{
  if (theOther.IsNull() == Standard_False) {
    NIS_Drawer::Assign (theOther);
    const Handle(NIS_SurfaceDrawer)& anOther =
      static_cast <const Handle(NIS_SurfaceDrawer)&> (theOther);
    myColor[Draw_Normal]       = anOther->myColor[Draw_Normal];
    myColor[Draw_Transparent]  = anOther->myColor[Draw_Transparent];
    myColor[Draw_Hilighted]    = anOther->myColor[Draw_Hilighted];
    myColor[Draw_DynHilighted] = anOther->myColor[Draw_DynHilighted];
    myBackColor                = anOther->myBackColor;
  }
}

//=======================================================================
//function : IsEqual
//purpose  : Comparison of two Drawers (for Map impementation)
//=======================================================================

Standard_Boolean NIS_SurfaceDrawer::IsEqual
                                (const Handle_NIS_Drawer& theOther)const
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
               (anOther->myTransparency - myTransparency) *
               (anOther->myTransparency - myTransparency) < 0.01);
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
                                  const Handle_NIS_View& theView)
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
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  glEnableClientState (GL_VERTEX_ARRAY);
  glEnable(GL_COLOR_MATERIAL);

  Quantity_Parameter   aValueCol[3];
  Quantity_TypeOfColor bidTC (Quantity_TOC_RGB);
  GLfloat aLineWidth (1.f);
  GLfloat anOffset = myPolygonOffset;
  switch (theType) {
  case Draw_DynHilighted:
    aLineWidth = 3.f;
    myColor[theType].Values (aValueCol[0], aValueCol[1], aValueCol[2], bidTC);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3d (aValueCol[0], aValueCol[1], aValueCol[2]);
    glLineWidth (aLineWidth);
//     glEnable(GL_POLYGON_OFFSET_LINE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.f, -(anOffset + 11.f));
    return;
  case Draw_Hilighted:
    anOffset += 10.f;
  case Draw_Normal:
  case Draw_Transparent:
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.f, -anOffset);
    glEnableClientState (GL_NORMAL_ARRAY);
    myColor[theType].Values (aValueCol[0], aValueCol[1], aValueCol[2], bidTC);
    aValueCol[3] = 1. - myTransparency;
    break;
  default:
    return;
  }
//   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
  static const GLfloat gColorN[4]  = {0.f, 0.f, 0.f, 1.f};
  GLfloat gColorS[4] = {
    0.5f * static_cast<GLfloat> (1. + aValueCol[0]),
    0.5f * static_cast<GLfloat> (1. + aValueCol[1]),
    0.5f * static_cast<GLfloat> (1. + aValueCol[2]),
    1.f
  };
  if (theType == Draw_Hilighted ||
      myBackColor.SquareDistance(myColor[Draw_Normal]) < 1.e-7)
  {
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d (aValueCol[0], aValueCol[1], aValueCol[2], aValueCol[3]);
//    glColor3d (aValueCol[0], aValueCol[1], aValueCol[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &gColorS[0]);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 10);
  } else {
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d (aValueCol[0], aValueCol[1], aValueCol[2], aValueCol[3]);
//    glColor3d (aValueCol[0], aValueCol[1], aValueCol[2]);
    glMaterialfv(GL_FRONT, GL_SPECULAR, &gColorS[0]);
    myBackColor.Values (aValueCol[0], aValueCol[1], aValueCol[2], bidTC);
    glColorMaterial(GL_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3d (aValueCol[0], aValueCol[1], aValueCol[2]);
    glMateriali(GL_FRONT, GL_SHININESS, 10);
    gColorS[0] = 0.9f * static_cast<GLfloat> (aValueCol[0]) + 0.1f;
    gColorS[1] = 0.9f * static_cast<GLfloat> (aValueCol[1]) + 0.1f;
    gColorS[2] = 0.9f * static_cast<GLfloat> (aValueCol[2]) + 0.1f;
    glMaterialfv(GL_BACK, GL_SPECULAR, &gColorS[0]);
    glMateriali(GL_BACK, GL_SHININESS, 5);
  }
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &gColorN[0]);

  if (theType == Draw_Hilighted)
    glColor3d(0.9, 0.9, 0.9);   // for the polygon presentation

  glLineWidth (aLineWidth);
  glShadeModel(GL_SMOOTH);
}

//=======================================================================
//function : AfterDraw
//purpose  : 
//=======================================================================

void NIS_SurfaceDrawer::AfterDraw (const DrawType      theType,
                                    const NIS_DrawList&)
{
  // Reset transformation matrix.
//   glPopMatrix();

  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glDisableClientState(GL_VERTEX_ARRAY);
  switch (theType) {
    case Draw_DynHilighted:
//       glDisable(GL_POLYGON_OFFSET_LINE);
      break;
    case Draw_Hilighted:
    case Draw_Normal:
    case Draw_Transparent:
      glDisable(GL_POLYGON_OFFSET_FILL);
      glDisableClientState(GL_NORMAL_ARRAY);
    default:;
  }
}

//=======================================================================
//function : Draw
//purpose  : 
//=======================================================================

void NIS_SurfaceDrawer::Draw (const Handle_NIS_InteractiveObject& theObj,
                               const DrawType                      theType,
                               const NIS_DrawList&)
{
  // Assertion for the type of the drawn object
#ifdef _DEBUG
  static const Handle(Standard_Type) ThisType = STANDARD_TYPE(NIS_Surface);
  Standard_ProgramError_Raise_if (theObj->IsKind(ThisType) == Standard_False,
                                  "NIS_Surface::Draw: "
                                  "irrelevant object type");
#endif
  const NIS_Surface * pObject =
    static_cast <const NIS_Surface *> (theObj.operator->());
  if (pObject->NTriangles())
  {
    glVertexPointer (3, GL_FLOAT, 0, pObject->Node(0));
    glNormalPointer (GL_FLOAT, 0, pObject->Normal(0));
    glDrawElements (GL_TRIANGLES, pObject->NTriangles()*3,
                    GL_UNSIGNED_INT, pObject->Triangle(0));
  }
}
