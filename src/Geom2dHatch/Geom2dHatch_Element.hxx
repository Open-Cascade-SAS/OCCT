// Created on: 1993-11-10
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Geom2dHatch_Element_HeaderFile
#define _Geom2dHatch_Element_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <TopAbs_Orientation.hxx>
class Geom2dAdaptor_Curve;



class Geom2dHatch_Element 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dHatch_Element();
  
  //! Magic constructor.
  Standard_EXPORT Geom2dHatch_Element(const Geom2dHatch_Element& Other);
  
  //! Creates an element.
  Standard_EXPORT Geom2dHatch_Element(const Geom2dAdaptor_Curve& Curve, const TopAbs_Orientation Orientation = TopAbs_FORWARD);
  
  //! Returns the curve associated to the element.
  Standard_EXPORT const Geom2dAdaptor_Curve& Curve() const;
  
  //! Returns the curve associated to the element.
  Standard_EXPORT Geom2dAdaptor_Curve& ChangeCurve();
  
  //! Sets the orientation of the element.
  Standard_EXPORT void Orientation (const TopAbs_Orientation Orientation);
  
  //! Returns the orientation of the element.
  Standard_EXPORT TopAbs_Orientation Orientation() const;




protected:





private:



  Geom2dAdaptor_Curve myCurve;
  TopAbs_Orientation myOrientation;


};







#endif // _Geom2dHatch_Element_HeaderFile
