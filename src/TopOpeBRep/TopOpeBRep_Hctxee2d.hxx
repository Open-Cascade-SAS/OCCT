// Created on: 1998-10-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRep_Hctxee2d_HeaderFile
#define _TopOpeBRep_Hctxee2d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Edge.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <IntRes2d_Domain.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Edge;
class BRepAdaptor_Surface;
class TopoDS_Shape;
class Geom2dAdaptor_Curve;
class IntRes2d_Domain;


class TopOpeBRep_Hctxee2d;
DEFINE_STANDARD_HANDLE(TopOpeBRep_Hctxee2d, Standard_Transient)


class TopOpeBRep_Hctxee2d : public Standard_Transient
{

public:

  
  Standard_EXPORT TopOpeBRep_Hctxee2d();
  
  Standard_EXPORT void SetEdges (const TopoDS_Edge& E1, const TopoDS_Edge& E2, const BRepAdaptor_Surface& BAS1, const BRepAdaptor_Surface& BAS2);
  
  Standard_EXPORT const TopoDS_Shape& Edge (const Standard_Integer I) const;
  
  Standard_EXPORT const Geom2dAdaptor_Curve& Curve (const Standard_Integer I) const;
  
  Standard_EXPORT const IntRes2d_Domain& Domain (const Standard_Integer I) const;




  DEFINE_STANDARD_RTTIEXT(TopOpeBRep_Hctxee2d,Standard_Transient)

protected:




private:


  TopoDS_Edge myEdge1;
  Geom2dAdaptor_Curve myCurve1;
  IntRes2d_Domain myDomain1;
  TopoDS_Edge myEdge2;
  Geom2dAdaptor_Curve myCurve2;
  IntRes2d_Domain myDomain2;


};







#endif // _TopOpeBRep_Hctxee2d_HeaderFile
