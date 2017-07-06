// Created on: 1997-02-05
// Created by: Alexander BRIVIN and Dmitry TARASOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Vrml_Coordinate3_HeaderFile
#define _Vrml_Coordinate3_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColgp_HArray1OfVec.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>


class Vrml_Coordinate3;
DEFINE_STANDARD_HANDLE(Vrml_Coordinate3, Standard_Transient)

//! defines a Coordinate3 node of VRML specifying
//! properties of geometry and its appearance.
//! This node defines a set of 3D coordinates to be used by a subsequent IndexedFaceSet,
//! IndexedLineSet, or PointSet node. This node does not produce a visible result
//! during rendering; it simply replaces the current coordinates in the rendering
//! state for subsequent nodes to use.
class Vrml_Coordinate3 : public Standard_Transient
{

public:

  
  Standard_EXPORT Vrml_Coordinate3(const Handle(TColgp_HArray1OfVec)& aPoint);
  
  Standard_EXPORT Vrml_Coordinate3();
  
  Standard_EXPORT void SetPoint (const Handle(TColgp_HArray1OfVec)& aPoint);
  
  Standard_EXPORT Handle(TColgp_HArray1OfVec) Point() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




  DEFINE_STANDARD_RTTIEXT(Vrml_Coordinate3,Standard_Transient)

protected:




private:


  Handle(TColgp_HArray1OfVec) myPoint;


};







#endif // _Vrml_Coordinate3_HeaderFile
