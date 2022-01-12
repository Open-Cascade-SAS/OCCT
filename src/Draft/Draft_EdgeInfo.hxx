// Created on: 1994-08-31
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Draft_EdgeInfo_HeaderFile
#define _Draft_EdgeInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <gp_Pnt.hxx>
class Geom_Curve;
class Geom2d_Curve;



class Draft_EdgeInfo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Draft_EdgeInfo();
  
  Standard_EXPORT Draft_EdgeInfo(const Standard_Boolean HasNewGeometry);
  
  Standard_EXPORT void Add (const TopoDS_Face& F);
  
  Standard_EXPORT void RootFace (const TopoDS_Face& F);
  
  Standard_EXPORT void Tangent (const gp_Pnt& P);
  
  Standard_EXPORT Standard_Boolean IsTangent (gp_Pnt& P) const;
  
  Standard_EXPORT Standard_Boolean NewGeometry() const;
  
  Standard_EXPORT void SetNewGeometry (const Standard_Boolean NewGeom);
  
  Standard_EXPORT const Handle(Geom_Curve)& Geometry() const;
  
  Standard_EXPORT const TopoDS_Face& FirstFace() const;
  
  Standard_EXPORT const TopoDS_Face& SecondFace() const;
  
  Standard_EXPORT const Handle(Geom2d_Curve)& FirstPC() const;
  
  Standard_EXPORT const Handle(Geom2d_Curve)& SecondPC() const;
  
  Standard_EXPORT Handle(Geom_Curve)& ChangeGeometry();
  
  Standard_EXPORT Handle(Geom2d_Curve)& ChangeFirstPC();
  
  Standard_EXPORT Handle(Geom2d_Curve)& ChangeSecondPC();
  
  Standard_EXPORT const TopoDS_Face& RootFace() const;
  
  Standard_EXPORT void Tolerance (const Standard_Real tol);
  
  Standard_EXPORT Standard_Real Tolerance() const;




protected:





private:



  Standard_Boolean myNewGeom;
  Handle(Geom_Curve) myGeom;
  TopoDS_Face myFirstF;
  TopoDS_Face mySeconF;
  Handle(Geom2d_Curve) myFirstPC;
  Handle(Geom2d_Curve) mySeconPC;
  TopoDS_Face myRootFace;
  Standard_Boolean myTgt;
  gp_Pnt myPt;
  Standard_Real myTol;


};







#endif // _Draft_EdgeInfo_HeaderFile
