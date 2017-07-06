// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepShape_BoxDomain_HeaderFile
#define _StepShape_BoxDomain_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
class StepGeom_CartesianPoint;


class StepShape_BoxDomain;
DEFINE_STANDARD_HANDLE(StepShape_BoxDomain, Standard_Transient)


class StepShape_BoxDomain : public Standard_Transient
{

public:

  
  //! Returns a BoxDomain
  Standard_EXPORT StepShape_BoxDomain();
  
  Standard_EXPORT void Init (const Handle(StepGeom_CartesianPoint)& aCorner, const Standard_Real aXlength, const Standard_Real aYlength, const Standard_Real aZlength);
  
  Standard_EXPORT void SetCorner (const Handle(StepGeom_CartesianPoint)& aCorner);
  
  Standard_EXPORT Handle(StepGeom_CartesianPoint) Corner() const;
  
  Standard_EXPORT void SetXlength (const Standard_Real aXlength);
  
  Standard_EXPORT Standard_Real Xlength() const;
  
  Standard_EXPORT void SetYlength (const Standard_Real aYlength);
  
  Standard_EXPORT Standard_Real Ylength() const;
  
  Standard_EXPORT void SetZlength (const Standard_Real aZlength);
  
  Standard_EXPORT Standard_Real Zlength() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_BoxDomain,Standard_Transient)

protected:




private:


  Handle(StepGeom_CartesianPoint) corner;
  Standard_Real xlength;
  Standard_Real ylength;
  Standard_Real zlength;


};







#endif // _StepShape_BoxDomain_HeaderFile
