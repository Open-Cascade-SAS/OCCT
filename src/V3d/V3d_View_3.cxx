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
#include <Aspect_Window.hxx>
#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Vector.hxx>
#include <Quantity_Color.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_TypeMismatch.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_Light.hxx>
#include <V3d_UnMapped.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

//=============================================================================
//function : Move
//purpose  :
//=============================================================================
void V3d_View::Move (const Standard_Real Dx,
                     const Standard_Real Dy,
                     const Standard_Real Dz,
                     const Standard_Boolean Start)
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  if( Start ) 
  {
    myCamStartOpEye = aCamera->Eye();

    gp_Dir aReferencePlane (aCamera->Direction().Reversed());
    gp_Dir anUp (aCamera->Up());
    if (!ScreenAxis (aReferencePlane, anUp, myXscreenAxis, myYscreenAxis, myZscreenAxis))
    {
          throw V3d_BadValue("V3d_View::Translate, alignment of Eye,At,Up");
    }
  }

  Standard_Real XX, XY, XZ, YX, YY, YZ, ZX, ZY, ZZ;

  myXscreenAxis.Coord (XX,XY,XZ);
  myYscreenAxis.Coord (YX,YY,YZ);
  myZscreenAxis.Coord (ZX,ZY,ZZ);

  aCamera->SetEye (myCamStartOpEye);

  aCamera->SetEye (aCamera->Eye().XYZ()
    + Dx * gp_Pnt (XX, XY, XZ).XYZ()
    + Dy * gp_Pnt (YX, YY, YZ).XYZ()
    + Dz * gp_Pnt (ZX, ZY, ZZ).XYZ()
    );

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : Move
//purpose  :
//=============================================================================
void V3d_View::Move (const Standard_Real Length, const Standard_Boolean Start)
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  if( Start )
  {
    myCamStartOpEye = aCamera->Eye();
  }
  aCamera->SetEye (myCamStartOpEye);

  Standard_Real Vx, Vy, Vz;
  MyDefaultViewAxis.Coord (Vx, Vy, Vz) ;

  aCamera->SetEye (aCamera->Eye().XYZ() + Length * gp_Pnt (Vx, Vy, Vz).XYZ());

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : Move
//purpose  :
//=============================================================================
void V3d_View::Move (const V3d_TypeOfAxe Axe , const Standard_Real Length, const Standard_Boolean Start)
{
  switch (Axe) {
  case V3d_X :
    Move(Length,0.,0.,Start);
    break ;
  case V3d_Y :
    Move(0.,Length,0.,Start);
    break ;
  case V3d_Z :
    Move(0.,0.,Length,Start);
    break ;
  }
}

//=============================================================================
//function : Translate
//purpose  :
//=============================================================================
void V3d_View::Translate (const Standard_Real Dx,
                          const Standard_Real Dy,
                          const Standard_Real Dz,
                          const Standard_Boolean Start)
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  if( Start )
  {
    myCamStartOpEye = aCamera->Eye();
    myCamStartOpCenter = aCamera->Center();

    gp_Dir aReferencePlane (aCamera->Direction().Reversed());
    gp_Dir anUp (aCamera->Up());
    if (!ScreenAxis (aReferencePlane, anUp,
		  myXscreenAxis,myYscreenAxis,myZscreenAxis))
	      throw V3d_BadValue("V3d_View::Translate, alignment of Eye,At,Up");
  }

  Standard_Real XX, XY, XZ, YX, YY, YZ, ZX, ZY, ZZ;

  myXscreenAxis.Coord (XX,XY,XZ);
  myYscreenAxis.Coord (YX,YY,YZ);
  myZscreenAxis.Coord (ZX,ZY,ZZ);

  aCamera->SetEye (myCamStartOpEye);
  aCamera->SetCenter (myCamStartOpCenter);

  aCamera->SetCenter (aCamera->Center().XYZ()
    - Dx * gp_Pnt (XX, XY, XZ).XYZ()
    - Dy * gp_Pnt (YX, YY, YZ).XYZ()
    - Dz * gp_Pnt (ZX, ZY, ZZ).XYZ()
    );

  aCamera->SetEye (aCamera->Eye().XYZ()
    - Dx * gp_Pnt (XX, XY, XZ).XYZ()
    - Dy * gp_Pnt (YX, YY, YZ).XYZ()
    - Dz * gp_Pnt (ZX, ZY, ZZ).XYZ()
    );

  AutoZFit();

  ImmediateUpdate();
}

//=============================================================================
//function : Translate
//purpose  :
//=============================================================================
void V3d_View::Translate(const V3d_TypeOfAxe Axe, const Standard_Real Length,const Standard_Boolean Start) {

  switch (Axe) {
  case V3d_X :
    Translate(Length,0.,0., Start);
    break ;
  case V3d_Y :
    Translate(0.,Length,0., Start);
    break ;
  case V3d_Z :
    Translate(0.,0.,Length, Start);
    break ;
  }
}

//=======================================================================
//function : Place
//purpose  :
//=======================================================================
void V3d_View::Place (const Standard_Integer theXp,
                      const Standard_Integer theYp,
                      const Standard_Real theZoomFactor)
{
  Standard_Integer aWinWidth  = 0;
  Standard_Integer aWinHeight = 0;
  View()->Window()->Size (aWinWidth, aWinHeight);

  Standard_Integer aWinCXp = aWinWidth  / 2;
  Standard_Integer aWinCYp = aWinHeight / 2;
  Pan (aWinCXp - theXp, -(aWinCYp - theYp), theZoomFactor / Scale());
}

//=======================================================================
//function : Place
//purpose  :
//=======================================================================
void V3d_View::Translate (const Standard_Real theLength, const Standard_Boolean theStart)
{
  Handle(Graphic3d_Camera) aCamera = Camera();

  Standard_Real aVx, aVy, aVz;
  if (theStart) 
  {
    myCamStartOpCenter = aCamera->Center() ;
  }
  MyDefaultViewAxis.Coord (aVx, aVy, aVz);
  gp_Pnt aNewCenter (myCamStartOpCenter.XYZ() - gp_Pnt (aVx, aVy, aVz).XYZ() * theLength);
  aCamera->SetCenter (aNewCenter);

  AutoZFit();

  ImmediateUpdate();
}
