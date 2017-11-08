// Created on: 1995-03-10
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Draw_Box.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Box,Draw_Drawable3D)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
Draw_Box::Draw_Box(const Bnd_OBB& theOBB,
                   const Draw_Color& theColor) :myOBB(theOBB), myColor(theColor)
{
}

//=======================================================================
//function : ToWCS
//purpose  : 
//=======================================================================
void Draw_Box::ToWCS(const Standard_Real theX,
                     const Standard_Real theY,
                     const Standard_Real theZ,
                     gp_Pnt& theP) const
{
  const gp_XYZ & aC = myOBB.Center();
  const gp_XYZ aXDir = myOBB.XDirection(),
               aYDir = myOBB.YDirection(),
               aZDir = myOBB.ZDirection();

  theP.SetXYZ(aC + theX*aXDir + theY*aYDir + theZ*aZDir);
}

//=======================================================================
//function : MoveX
//purpose  : 
//=======================================================================
void Draw_Box::MoveX(const Standard_Real theShift, gp_Pnt& thePt) const
{
  const gp_XYZ aXDir = myOBB.XDirection();
  thePt.SetXYZ(thePt.XYZ() + theShift*aXDir);
}

//=======================================================================
//function : MoveY
//purpose  : 
//=======================================================================
void Draw_Box::MoveY(const Standard_Real theShift, gp_Pnt& thePt) const
{
  const gp_XYZ aYDir = myOBB.YDirection();
  thePt.SetXYZ(thePt.XYZ() + theShift*aYDir);
}

//=======================================================================
//function : MoveZ
//purpose  : 
//=======================================================================
void Draw_Box::MoveZ(const Standard_Real theShift, gp_Pnt& thePt) const
{
  const gp_XYZ aZDir = myOBB.ZDirection();
  thePt.SetXYZ(thePt.XYZ() + theShift*aZDir);
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================
void Draw_Box::DrawOn(Draw_Display& theDIS) const 
{
  if(myOBB.IsVoid())
  {
    return;
  }

  theDIS.SetColor(myColor);
  
  const Standard_Real aHx = myOBB.XHSize(),
                      aHy = myOBB.YHSize(),
                      aHz = myOBB.ZHSize();

  gp_Pnt aP;
  ToWCS(-aHx, -aHy, -aHz, aP);
  theDIS.MoveTo(aP);

  for(Standard_Integer i = 0; i<2; i++)
  {
    MoveX(2.0*aHx, aP);
    theDIS.DrawTo(aP);
    MoveY(2.0*aHy, aP);
    theDIS.DrawTo(aP);
    MoveX(-2.0*aHx, aP);
    theDIS.DrawTo(aP);
    MoveY(-2.0*aHy, aP);
    theDIS.DrawTo(aP);

    ToWCS(-aHx, -aHy, aHz, aP);
    theDIS.MoveTo(aP);
  }
  
  for(Standard_Integer i = 0; i < 4; i++)
  {
    switch(i)
    {
      case 0: ToWCS(-aHx, -aHy, -aHz, aP); break;
      case 1: ToWCS(aHx, -aHy, -aHz, aP); break;
      case 2: ToWCS(aHx, aHy, -aHz, aP); break;
      case 3: ToWCS(-aHx, aHy, -aHz, aP); break;
      default: break;
    }

    theDIS.MoveTo(aP);
    MoveZ(2.0*aHz, aP);
    theDIS.DrawTo(aP);
  }
}

