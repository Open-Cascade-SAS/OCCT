// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Aspect_GradientBackground.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_RectangularGrid.hxx>
#include <Aspect_Window.hxx>
#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Vector.hxx>
#include <Quantity_Color.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_CircularGrid.hxx>
#include <V3d_Light.hxx>
#include <V3d_RectangularGrid.hxx>
#include <V3d_UnMapped.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

#define MYEPSILON1 0.0001		// Comparison with 0.0
#define MYEPSILON2 M_PI / 180.	// Delta between 2 angles

//=============================================================================
//function : SetGrid
//purpose  :
//=============================================================================
void V3d_View::SetGrid (const gp_Ax3& aPlane, const Handle(Aspect_Grid)& aGrid)
{
  MyPlane	= aPlane;
  MyGrid	= aGrid;

  Standard_Real xl, yl, zl;
  Standard_Real xdx, xdy, xdz;
  Standard_Real ydx, ydy, ydz;
  Standard_Real dx, dy, dz;
  aPlane.Location ().Coord (xl, yl, zl);
  aPlane.XDirection ().Coord (xdx, xdy, xdz);
  aPlane.YDirection ().Coord (ydx, ydy, ydz);
  aPlane.Direction ().Coord (dx, dy, dz);

  Standard_Real CosAlpha = Cos (MyGrid->RotationAngle ());
  Standard_Real SinAlpha = Sin (MyGrid->RotationAngle ());

  TColStd_Array2OfReal Trsf1 (1, 4, 1, 4);
  Trsf1 (4, 4) = 1.0;
  Trsf1 (4, 1) = Trsf1 (4, 2) = Trsf1 (4, 3) = 0.0;
  // Translation
  Trsf1 (1, 4) = xl,
  Trsf1 (2, 4) = yl,
  Trsf1 (3, 4) = zl;
  // Transformation change of marker
  Trsf1 (1, 1) = xdx,
  Trsf1 (2, 1) = xdy,
  Trsf1 (3, 1) = xdz,
  Trsf1 (1, 2) = ydx,
  Trsf1 (2, 2) = ydy,
  Trsf1 (3, 2) = ydz,
  Trsf1 (1, 3) = dx,
  Trsf1 (2, 3) = dy,
  Trsf1 (3, 3) = dz;

  TColStd_Array2OfReal Trsf2 (1, 4, 1, 4);
  Trsf2 (4, 4) = 1.0;
  Trsf2 (4, 1) = Trsf2 (4, 2) = Trsf2 (4, 3) = 0.0;
  // Translation of the origin
  Trsf2 (1, 4) = -MyGrid->XOrigin (),
  Trsf2 (2, 4) = -MyGrid->YOrigin (),
  Trsf2 (3, 4) = 0.0;
  // Rotation Alpha around axis -Z
  Trsf2 (1, 1) = CosAlpha,
  Trsf2 (2, 1) = -SinAlpha,
  Trsf2 (3, 1) = 0.0,
  Trsf2 (1, 2) = SinAlpha,
  Trsf2 (2, 2) = CosAlpha,
  Trsf2 (3, 2) = 0.0,
  Trsf2 (1, 3) = 0.0,
  Trsf2 (2, 3) = 0.0,
  Trsf2 (3, 3) = 1.0;

  Standard_Real valuetrsf;
  Standard_Real valueoldtrsf;
  Standard_Real valuenewtrsf;
  Standard_Integer i, j, k;
  // Calculation of the product of matrices
  for (i=1; i<=4; i++)
      for (j=1; j<=4; j++) {
    MyTrsf (i, j) = 0.0;
    for (k=1; k<=4; k++) {
        valueoldtrsf = Trsf1 (i, k);
        valuetrsf	 = Trsf2 (k, j);
        valuenewtrsf = MyTrsf (i, j) + valueoldtrsf * valuetrsf;
        MyTrsf (i, j) = valuenewtrsf;
    }
     }
}

//=============================================================================
//function : SetGridActivity
//purpose  :
//=============================================================================
void V3d_View::SetGridActivity (const Standard_Boolean AFlag)
{
  if (AFlag) MyGrid->Activate ();
  else MyGrid->Deactivate ();
}

//=============================================================================
//function : toPolarCoords
//purpose  :
//=============================================================================
void toPolarCoords (const Standard_Real theX, const Standard_Real theY, 
                          Standard_Real& theR, Standard_Real& thePhi)
{
  theR = Sqrt (theX * theX + theY * theY);
  thePhi = ATan2 (theY, theX);  
}

//=============================================================================
//function : toCartesianCoords
//purpose  :
//=============================================================================
void toCartesianCoords (const Standard_Real theR, const Standard_Real thePhi, 
                              Standard_Real& theX, Standard_Real& theY)
{
  theX = theR * Cos (thePhi);
  theY = theR * Sin (thePhi);
}

//=============================================================================
//function : Compute
//purpose  :
//=============================================================================
Graphic3d_Vertex V3d_View::Compute (const Graphic3d_Vertex & AVertex) const
{
  Graphic3d_Vertex CurPoint, NewPoint;
  Standard_Real X1, Y1, Z1, X2, Y2, Z2;
  Standard_Real XPp, YPp;
  Handle(Graphic3d_Camera) aCamera = Camera();

  gp_Dir aRefPlane = aCamera->Direction().Reversed();
  X1 = aRefPlane.X(); Y1 = aRefPlane.Y(); Z1 = aRefPlane.Z();
  MyPlane.Direction ().Coord (X2, Y2, Z2);

  gp_Dir VPN (X1, Y1, Z1);
  gp_Dir GPN (X2, Y2, Z2);

  AVertex.Coord (X1, Y1, Z1);
  Project (X1, Y1, Z1, XPp, YPp);

  // Casw when the plane of the grid and the plane of the view
  // are perpendicular to MYEPSILON2 close radians
  if (Abs (VPN.Angle (GPN) - M_PI / 2.) < MYEPSILON2) {
    NewPoint.SetCoord (X1, Y1, Z1);
    return NewPoint;
  }

  Standard_Boolean IsRectangular = 
    MyGrid->IsKind (STANDARD_TYPE (Aspect_RectangularGrid));

  Graphic3d_Vertex P1;

  Standard_Real x0, y0, z0, x1, y1, z1, x2, y2, z2;
    
  P1.SetCoord (0.0, 0.0, 0.0);
  CurPoint = V3d_View::TrsPoint (P1, MyTrsf);
  CurPoint.Coord (x0, y0, z0);
    
  // get grid axes in world space
  P1.SetCoord (1.0, 0.0, 0.0);
  CurPoint = V3d_View::TrsPoint (P1, MyTrsf);
  CurPoint.Coord (x1, y1, z1);
  gp_Vec aGridX (gp_Pnt (x0, y0, z0), gp_Pnt (x1, y1, z1));
  aGridX.Normalize();

  P1.SetCoord (0.0, 1.0, 0.0);
  CurPoint = V3d_View::TrsPoint (P1, MyTrsf);
  CurPoint.Coord (x2, y2, z2);
  gp_Vec aGridY (gp_Pnt (x0, y0, z0), gp_Pnt (x2, y2, z2));
  aGridY.Normalize();

  // get grid normal
  MyPlane.Direction().Coord (x2, y2, z2);
  gp_Vec aPlaneNormal (x2, y2, z2);

  gp_Vec aPointOnPlane = gp_Vec (0.0, 0.0, 0.0);

  AVertex.Coord (x1, y1, z1);
    
  // project ray from camera onto grid plane
  gp_Vec aProjection  = aCamera->IsOrthographic()
                      ? gp_Vec (aCamera->Direction())
                      : gp_Vec (aCamera->Eye(), gp_Pnt (x1, y1, z1)).Normalized();
  gp_Vec aPointOrigin = gp_Vec (gp_Pnt (x1, y1, z1), gp_Pnt (x0, y0, z0));
  Standard_Real aT    = aPointOrigin.Dot (aPlaneNormal) / aProjection.Dot (aPlaneNormal);
  aPointOnPlane       = gp_Vec (x1, y1, z1) + aProjection * aT;

  if (IsRectangular) {
    Standard_Real XS, YS;
    Handle(Aspect_RectangularGrid) theGrid =
      Handle(Aspect_RectangularGrid)::DownCast (MyGrid);
    XS = theGrid->XStep (), YS = theGrid->YStep ();

    // project point on plane to grid local space
    gp_Vec aToPoint (gp_Pnt (x0, y0, z0), 
                     gp_Pnt (aPointOnPlane.X(), aPointOnPlane.Y(), aPointOnPlane.Z()));
    Standard_Real anXSteps = Round (aGridX.Dot (aToPoint) / XS);
    Standard_Real anYSteps = Round (aGridY.Dot (aToPoint) / YS);

    // clamp point to grid
    gp_Vec aResult = aGridX * anXSteps * XS + aGridY * anYSteps * YS + gp_Vec (x0, y0, z0);
    NewPoint.SetCoord (aResult.X(), aResult.Y(), aResult.Z());

  } 
  else // IsCircular
  {
    Standard_Real RS;
    Standard_Integer DN;
    Standard_Real Alpha;
    Handle(Aspect_CircularGrid) theGrid =
      Handle(Aspect_CircularGrid)::DownCast (MyGrid);
    RS = theGrid->RadiusStep ();
    DN = theGrid->DivisionNumber ();
    Alpha = M_PI / Standard_Real (DN);

    // project point on plane to grid local space
    gp_Vec aToPoint (gp_Pnt (x0, y0, z0), 
                     gp_Pnt (aPointOnPlane.X(), aPointOnPlane.Y(), aPointOnPlane.Z()));

    Standard_Real anR = 0.0, aPhi = 0.0;
    Standard_Real aLocalX = aGridX.Dot (aToPoint);
    Standard_Real aLocalY = aGridY.Dot (aToPoint);
    toPolarCoords (aLocalX, aLocalY, anR, aPhi);

    // clamp point to grid
    Standard_Real anRSteps  = Round (anR / RS);
    Standard_Real aPhiSteps = Round (aPhi / Alpha);
    toCartesianCoords (anRSteps * RS, aPhiSteps * Alpha, aLocalX, aLocalY);

    gp_Vec aResult = aGridX * aLocalX + aGridY * aLocalY + gp_Vec (x0, y0, z0);
    NewPoint.SetCoord (aResult.X(), aResult.Y(), aResult.Z());
  }

  return NewPoint;
}

//=============================================================================
//function : ZBufferTriedronSetup
//purpose  :
//=============================================================================
void V3d_View::ZBufferTriedronSetup(const Quantity_NameOfColor theXColor,
                                    const Quantity_NameOfColor theYColor,
                                    const Quantity_NameOfColor theZColor,
                                    const Standard_Real        theSizeRatio,
                                    const Standard_Real        theAxisDiametr,
                                    const Standard_Integer     theNbFacettes)
{
  myView->ZBufferTriedronSetup (theXColor,
                                theYColor,
                                theZColor,
                                theSizeRatio,
                                theAxisDiametr,
                                theNbFacettes);
}

//=============================================================================
//function : TriedronDisplay
//purpose  :
//=============================================================================
void V3d_View::TriedronDisplay (const Aspect_TypeOfTriedronPosition thePosition,
                                const Quantity_NameOfColor theColor,
                                const Standard_Real theScale,
                                const V3d_TypeOfVisualization theMode)
{
  myView->TriedronDisplay (thePosition, theColor, theScale, (theMode == V3d_WIREFRAME));
}

//=============================================================================
//function : TriedronErase
//purpose  :
//=============================================================================
void V3d_View::TriedronErase()
{
  myView->TriedronErase();
}

//=============================================================================
//function : TriedronEcho
//purpose  :
//=============================================================================
void V3d_View::TriedronEcho (const Aspect_TypeOfTriedronEcho theType)
{
  myView->TriedronEcho (theType);
}

//=============================================================================
//function : GetGraduatedTrihedron
//purpose  :
//=============================================================================
const Graphic3d_GraduatedTrihedron& V3d_View::GetGraduatedTrihedron() const
{
  return myView->GetGraduatedTrihedron();
}

//=============================================================================
//function : GraduatedTrihedronDisplay
//purpose  :
//=============================================================================
void V3d_View::GraduatedTrihedronDisplay(const Graphic3d_GraduatedTrihedron& theTrihedronData)
{
  myView->GraduatedTrihedronDisplay (theTrihedronData);
}

//=============================================================================
//function : GraduatedTrihedronErase
//purpose  :
//=============================================================================
void V3d_View::GraduatedTrihedronErase()
{
  myView->GraduatedTrihedronErase();
}
