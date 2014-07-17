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

/***********************************************************************
 
     FONCTION :
     ----------
        Classe V3d_View_3.cxx :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
     00-09-92 : GG  ; Creation.
     24-12-97 : FMN ; Suppression de GEOMLITE
     13-06-98 : FMN ; PRO14896: Correction sur la gestion de la perspective (cf Programming Guide)

************************************************************************/

#define IMP020300       //GG Don't use ZFitAll in during Rotation
//                      for perf improvment

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V3d_View.jxx>
#include <V3d_Static.hxx>

#define Zmargin 1.

/*----------------------------------------------------------------------*/

void V3d_View::Move(const Standard_Real Dx, const Standard_Real Dy, const Standard_Real Dz, const Standard_Boolean Start) {
  
  if( Start ) 
  {
    myCamStartOpEye = myCamera->Eye();

    gp_Dir aReferencePlane (myCamera->Direction().Reversed());
    gp_Dir anUp (myCamera->Up());
    if (!ScreenAxis (aReferencePlane, anUp, MyXscreenAxis, MyYscreenAxis, MyZscreenAxis))
    {
          V3d_BadValue::Raise ("V3d_View::Translate, alignment of Eye,At,Up");
    }
  }

  Standard_Real XX, XY, XZ, YX, YY, YZ, ZX, ZY, ZZ;

  MyXscreenAxis.Coord (XX,XY,XZ); 
  MyYscreenAxis.Coord (YX,YY,YZ); 
  MyZscreenAxis.Coord (ZX,ZY,ZZ);

  myCamera->SetEye (myCamStartOpEye);

  myCamera->SetEye (myCamera->Eye().XYZ()
    + Dx * gp_Pnt (XX, XY, XZ).XYZ()
    + Dy * gp_Pnt (YX, YY, YZ).XYZ()
    + Dz * gp_Pnt (ZX, ZY, ZZ).XYZ()
    );

  View()->AutoZFit();

  ImmediateUpdate();
}

void V3d_View::Move(const Standard_Real Length, const Standard_Boolean Start) {

  if( Start ) 
  {
    myCamStartOpEye = myCamera->Eye();
  }
  myCamera->SetEye (myCamStartOpEye);

  Standard_Real Vx, Vy, Vz;
  MyDefaultViewAxis.Coord (Vx, Vy, Vz) ;

  myCamera->SetEye (myCamera->Eye().XYZ() + Length * gp_Pnt (Vx, Vy, Vz).XYZ());

  View()->AutoZFit();

  ImmediateUpdate();
}

void V3d_View::Move(const V3d_TypeOfAxe Axe , const Standard_Real Length, const Standard_Boolean Start) {


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

void V3d_View::Translate(const Standard_Real Dx, const Standard_Real Dy, const Standard_Real Dz, const Standard_Boolean Start) {

  if( Start ) 
  {
    myCamStartOpEye = myCamera->Eye();
    myCamStartOpCenter = myCamera->Center();

    gp_Dir aReferencePlane (myCamera->Direction().Reversed());
    gp_Dir anUp (myCamera->Up());
    if (!ScreenAxis (aReferencePlane, anUp,
		  MyXscreenAxis,MyYscreenAxis,MyZscreenAxis))
	      V3d_BadValue::Raise ("V3d_View::Translate, alignment of Eye,At,Up");
  }

  Standard_Real XX, XY, XZ, YX, YY, YZ, ZX, ZY, ZZ;

  MyXscreenAxis.Coord (XX,XY,XZ); 
  MyYscreenAxis.Coord (YX,YY,YZ); 
  MyZscreenAxis.Coord (ZX,ZY,ZZ);

  myCamera->SetEye (myCamStartOpEye);
  myCamera->SetCenter (myCamStartOpCenter);

  myCamera->SetCenter (myCamera->Center().XYZ()
    - Dx * gp_Pnt (XX, XY, XZ).XYZ()
    - Dy * gp_Pnt (YX, YY, YZ).XYZ()
    - Dz * gp_Pnt (ZX, ZY, ZZ).XYZ()
    );

  myCamera->SetEye (myCamera->Eye().XYZ()
    - Dx * gp_Pnt (XX, XY, XZ).XYZ()
    - Dy * gp_Pnt (YX, YY, YZ).XYZ()
    - Dz * gp_Pnt (ZX, ZY, ZZ).XYZ()
    );

  View()->AutoZFit();

  ImmediateUpdate();
}

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
                      const Quantity_Factor theZoomFactor)
{
  Standard_Integer aWinWidth  = 0;
  Standard_Integer aWinHeight = 0;
  View()->Window()->Size (aWinWidth, aWinHeight);

  Standard_Integer aWinCXp = aWinWidth  / 2;
  Standard_Integer aWinCYp = aWinHeight / 2;
  Pan (aWinCXp - theXp, aWinCYp - theYp, theZoomFactor / Scale());
}

void V3d_View::Translate(const Standard_Real theLength, const Standard_Boolean theStart) {

  Standard_Real aVx, aVy, aVz ;
  if (theStart) 
  {
    myCamStartOpCenter = myCamera->Center() ;
  }
  MyDefaultViewAxis.Coord (aVx, aVy, aVz);
  gp_Pnt aNewCenter (myCamStartOpCenter.XYZ() - gp_Pnt (aVx, aVy, aVz).XYZ() * theLength);
  myCamera->SetCenter (aNewCenter);

  View()->AutoZFit();

  ImmediateUpdate();
}
