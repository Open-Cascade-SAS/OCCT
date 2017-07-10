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
#include <Aspect_CircularGrid.hxx>

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
Graphic3d_Vertex V3d_View::Compute (const Graphic3d_Vertex& theVertex) const
{
  const Handle(Graphic3d_Camera)& aCamera = Camera();
  gp_Dir VPN = aCamera->Direction().Reversed(); // RefPlane
  gp_Dir GPN = MyPlane.Direction();

  Standard_Real XPp = 0.0, YPp = 0.0;
  Project (theVertex.X(), theVertex.Y(), theVertex.Z(), XPp, YPp);

  // Casw when the plane of the grid and the plane of the view
  // are perpendicular to MYEPSILON2 close radians
  if (Abs (VPN.Angle (GPN) - M_PI / 2.) < MYEPSILON2)
  {
    return theVertex;
  }

  const gp_XYZ aPnt0 = V3d_View::TrsPoint (Graphic3d_Vertex (0.0, 0.0, 0.0), MyTrsf);
    
  // get grid axes in world space
  const gp_XYZ aPnt1 = V3d_View::TrsPoint (Graphic3d_Vertex (1.0, 0.0, 0.0), MyTrsf);
  gp_Vec aGridX (aPnt0, aPnt1);
  aGridX.Normalize();

  const gp_XYZ aPnt2 = V3d_View::TrsPoint (Graphic3d_Vertex (0.0, 1.0, 0.0), MyTrsf);
  gp_Vec aGridY (aPnt0, aPnt2);
  aGridY.Normalize();

  // project ray from camera onto grid plane
  const gp_Vec aProjection  = aCamera->IsOrthographic()
                            ? gp_Vec (aCamera->Direction())
                            : gp_Vec (aCamera->Eye(), gp_Pnt (theVertex.X(), theVertex.Y(), theVertex.Z())).Normalized();
  const gp_Vec aPointOrigin = gp_Vec (gp_Pnt (theVertex.X(), theVertex.Y(), theVertex.Z()), aPnt0);
  const Standard_Real aT    = aPointOrigin.Dot (MyPlane.Direction()) / aProjection.Dot (MyPlane.Direction());
  const gp_XYZ aPointOnPlane = gp_XYZ (theVertex.X(), theVertex.Y(), theVertex.Z()) + aProjection.XYZ() * aT;

  if (Handle(Aspect_RectangularGrid) aRectGrid = Handle(Aspect_RectangularGrid)::DownCast (MyGrid))
  {
    // project point on plane to grid local space
    const gp_Vec aToPoint (aPnt0, aPointOnPlane);
    const Standard_Real anXSteps = Round (aGridX.Dot (aToPoint) / aRectGrid->XStep());
    const Standard_Real anYSteps = Round (aGridY.Dot (aToPoint) / aRectGrid->YStep());

    // clamp point to grid
    const gp_Vec aResult = aGridX * anXSteps * aRectGrid->XStep()
                         + aGridY * anYSteps * aRectGrid->YStep()
                         + gp_Vec (aPnt0);
    return Graphic3d_Vertex (aResult.X(), aResult.Y(), aResult.Z());
  } 
  else // IsCircular
  {
    Handle(Aspect_CircularGrid) aCircleGrid = Handle(Aspect_CircularGrid)::DownCast (MyGrid);
    const Standard_Real anAlpha = M_PI / Standard_Real (aCircleGrid->DivisionNumber());

    // project point on plane to grid local space
    const gp_Vec aToPoint (aPnt0, aPointOnPlane);
    Standard_Real aLocalX = aGridX.Dot (aToPoint);
    Standard_Real aLocalY = aGridY.Dot (aToPoint);
    Standard_Real anR = 0.0, aPhi = 0.0;
    toPolarCoords (aLocalX, aLocalY, anR, aPhi);

    // clamp point to grid
    const Standard_Real anRSteps  = Round (anR / aCircleGrid->RadiusStep());
    const Standard_Real aPhiSteps = Round (aPhi / anAlpha);
    toCartesianCoords (anRSteps * aCircleGrid->RadiusStep(), aPhiSteps * anAlpha, aLocalX, aLocalY);

    const gp_Vec aResult = aGridX * aLocalX + aGridY * aLocalY + gp_Vec (aPnt0);
    return Graphic3d_Vertex (aResult.X(), aResult.Y(), aResult.Z());
  }
}

//=============================================================================
//function : ZBufferTriedronSetup
//purpose  :
//=============================================================================
void V3d_View::ZBufferTriedronSetup(const Quantity_Color&  theXColor,
                                    const Quantity_Color&  theYColor,
                                    const Quantity_Color&  theZColor,
                                    const Standard_Real    theSizeRatio,
                                    const Standard_Real    theAxisDiametr,
                                    const Standard_Integer theNbFacettes)
{
  myTrihedron->SetArrowsColor   (theXColor, theYColor, theZColor);
  myTrihedron->SetSizeRatio     (theSizeRatio);
  myTrihedron->SetNbFacets      (theNbFacettes);
  myTrihedron->SetArrowDiameter (theAxisDiametr);
}

//=============================================================================
//function : TriedronDisplay
//purpose  :
//=============================================================================
void V3d_View::TriedronDisplay (const Aspect_TypeOfTriedronPosition thePosition,
                                const Quantity_Color& theColor,
                                const Standard_Real theScale,
                                const V3d_TypeOfVisualization theMode)
{
  myTrihedron->SetLabelsColor (theColor);
  myTrihedron->SetScale       (theScale);
  myTrihedron->SetPosition    (thePosition);
  myTrihedron->SetWireframe   (theMode == V3d_WIREFRAME);

  myTrihedron->Display (*this);
}

//=============================================================================
//function : TriedronErase
//purpose  :
//=============================================================================
void V3d_View::TriedronErase()
{
  myTrihedron->Erase();
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
