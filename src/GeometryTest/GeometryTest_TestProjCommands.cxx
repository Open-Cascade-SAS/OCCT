// Created on: 2011-06-30
// Created by: jgv@ROLEX
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <GeometryTest.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Marker3D.hxx>

#include <stdio.h>
#ifdef WNT
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
//function : xdistcs
//purpose  : 
//=======================================================================
static Standard_Integer xdistcs(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if (n<5) {
    cout<<" Use xdistcs c s t1 t2 nbp"<<endl;
    return 0;
  }
  //
  Standard_Boolean bRet;
  Standard_Integer i, aNbP, iSize;
  Standard_Real aTol, aD, aT, aT1, aT2, dT;
  gp_Pnt aP;
  Handle(Geom_Curve) aC;
  Handle(Geom_Surface) aS;
  GeomAPI_ProjectPointOnSurf aPPS;
  Handle(Draw_Marker3D) aMr;
  Draw_Color aColor(Draw_rouge);
  //
  aTol=1.e-7;
  //
  aC=DrawTrSurf::GetCurve(a[1]);
  if (aC.IsNull()) {
    cout<<a[1]<<" is null curve"<<endl;
    return 0;
  }
  //
  aS=DrawTrSurf::GetSurface(a[2]);
  if (aS.IsNull()) {
    cout<<a[2]<<" is null"<<endl;
    return 0;
  }
  //
  aT1=Draw::Atof(a[3]);
  aT2=Draw::Atof(a[4]);
  //
  aNbP=10;
  if (n>5) {
    aNbP=Draw::Atoi(a[5]);
  }
  //
  iSize=3;
  //
  dT=(aT2-aT1)/(aNbP-1);
  for (i=0; i<aNbP; ++i) {
    aT=aT1+i*dT;
    if (i==aNbP-1) {
      aT=aT2;
    }
    //
    aC->D0(aT, aP);
    aPPS.Init(aP, aS, aTol);
    bRet=aPPS.IsDone();
    if (!bRet) {
      cout<<" GeomAPI_ProjectPointOnSurf failed"<<endl;
      return 0;
    }
    //
    aD=aPPS.LowerDistance();
    printf(" T=%lg\tD=%lg\n", aT, aD);
    //
    aMr=new Draw_Marker3D(aP, Draw_Plus, aColor, iSize);
    dout << aMr;
  }
  //
  return 0;
}

//=======================================================================
//function : TestProjCommands
//purpose  : 
//=======================================================================

void  GeometryTest::TestProjCommands(Draw_Interpretor& theCommands)
{
  
  static Standard_Boolean loaded = Standard_False;
  if (loaded) return;
  loaded = Standard_True;
  
  DrawTrSurf::BasicCommands(theCommands);
  
  const char* g;
  
  g = "Testing of projection (geometric objects)";
  
  theCommands.Add("xdistcs", "xdistcs c s t1 t2 nbp", __FILE__, xdistcs, g);
}
