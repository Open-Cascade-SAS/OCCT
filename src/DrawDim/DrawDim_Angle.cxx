// Created on: 1996-05-28
// Created by: Denis PASCAL
// Copyright (c) 1996-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Draw_Display.hxx>
#include <DrawDim.hxx>
#include <DrawDim_Angle.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawDim_Angle,DrawDim_Dimension)

//=======================================================================
//function : DrawDim_Angle
//purpose  : 
//=======================================================================
DrawDim_Angle::DrawDim_Angle(const TopoDS_Face& plane1, const TopoDS_Face& plane2)
{
  myPlane1 = plane1;
  myPlane2 = plane2;
}

//=======================================================================
//function : Plane1
//purpose  : 
//=======================================================================

const TopoDS_Face& DrawDim_Angle::Plane1() const 
{
  return myPlane1;
}

//=======================================================================
//function : Plane1
//purpose  : 
//=======================================================================

void DrawDim_Angle::Plane1(const TopoDS_Face& plane)
{
  myPlane1 = plane;
}

//=======================================================================
//function : Plane2
//purpose  : 
//=======================================================================

const TopoDS_Face& DrawDim_Angle::Plane2() const 
{  
  return myPlane2;
}

//=======================================================================
//function : Plane2
//purpose  : 
//=======================================================================

void DrawDim_Angle::Plane2(const TopoDS_Face& plane)
{
  myPlane2 = plane;
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawDim_Angle::DrawOn(Draw_Display& ) const 
{

  // input
  TopoDS_Shape myFShape = myPlane1;
  TopoDS_Shape mySShape = myPlane2;  
  Standard_Real myVal = GetValue();
  gp_Ax1 myAxis;


  // output
  gp_Pnt myFAttach;
  gp_Pnt mySAttach;
  gp_Pnt myPosition(0.,0.,0.);
  gp_Pnt myCenter;
  gp_Dir myFDir;
  gp_Dir mySDir;
  Standard_Boolean myAutomaticPosition = Standard_True;

  // calculation of myAxis
  gp_Pln pln1, pln2;
  if (!DrawDim::Pln(myPlane1,pln1)) return;
  if (!DrawDim::Pln(myPlane2,pln2)) return; 
  IntAna_QuadQuadGeo ip (pln1,pln2,Precision::Confusion(), Precision::Angular());
  if (!ip.IsDone()) return;
 
  gp_Pnt curpos;
  gp_Ax1 AxePos = myAxis;      
  gp_Dir theAxisDir = AxePos.Direction();
  gp_Lin theaxis= gp_Lin (myAxis);
      
  if (myAutomaticPosition) {
    TopExp_Explorer explo1(myFShape,TopAbs_VERTEX);
    Standard_Real curdist = 0;
    while  (explo1.More()) {
      TopoDS_Vertex vertref = TopoDS::Vertex(explo1.Current());
      gp_Pnt curpt = BRep_Tool::Pnt(vertref);
      if (theaxis.Distance(curpt) > curdist) {
	curdist = theaxis.Distance(curpt);
	myFAttach = BRep_Tool::Pnt(vertref);
      }
      explo1.Next();
    }
    curpos = myFAttach.Rotated(AxePos,myVal/2.);
    myCenter = ElCLib::Value(ElCLib::Parameter(theaxis,curpos),theaxis);
    Standard_Real thedista = myCenter.Distance(myFAttach);
    if (thedista > Precision::Confusion()) {
      curpos.Scale(myCenter,1.05);
    }
    myPosition = curpos;
    myAutomaticPosition = Standard_True;      
  }
  else {
    curpos = myPosition;
    //myFAttach  = the point of myFShape closest to curpos (except for the case when this is a point on the axis)
    Standard_Real dist = RealLast(); 
    TopExp_Explorer explo1(myFShape,TopAbs_VERTEX);
    gp_Pnt  AxePosition = AxePos.Location();
    gp_Vec  AxeVector (theAxisDir);
    gp_XYZ  AxeXYZ  = AxeVector.XYZ();
    while (explo1.More()) {
      gp_Pnt curpt = BRep_Tool::Pnt(TopoDS::Vertex(explo1.Current()));
      gp_Vec curvec (AxePosition, curpt);
      gp_XYZ curXYZ = curvec.XYZ();
      gp_XYZ Norm (curXYZ.Crossed(AxeXYZ));

      if (Norm.Modulus() > gp::Resolution()) {
	Standard_Real curdist = curpos.Distance (curpt);
	if (curdist < dist) {
	  myFAttach = curpt;
	  dist = curdist;
	}
      }
      explo1.Next();
    }
    myCenter = ElCLib::Value(ElCLib::Parameter(theaxis,myFAttach),theaxis);
  }

  mySAttach = myFAttach.Rotated(AxePos,myVal);
      
  gp_Vec FVec (myCenter, myFAttach);
  myFDir.SetXYZ (FVec.XYZ());
  gp_Vec SVec (myCenter, mySAttach);
  mySDir.SetXYZ (SVec.XYZ());

  if (!myAutomaticPosition) {
    //Projection of the position on the plane defined by myFDir mySDir and normal theAxisDir
    gp_Pln aPln (myCenter, theAxisDir);
    Standard_Real U,V;
    ElSLib::Parameters (aPln, curpos, U, V);
    curpos = ElSLib::Value (U, V, aPln);
  }

  // DISPLAY
  // Add (myVal, myText,myCenter,myFAttach,mySAttach,myFDir,mySDir,theAxisDir,curpos)

          
}









