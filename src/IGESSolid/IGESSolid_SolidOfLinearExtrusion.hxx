// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESSolid_SolidOfLinearExtrusion_HeaderFile
#define _IGESSolid_SolidOfLinearExtrusion_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESData_IGESEntity;
class gp_XYZ;
class gp_Dir;


class IGESSolid_SolidOfLinearExtrusion;
DEFINE_STANDARD_HANDLE(IGESSolid_SolidOfLinearExtrusion, IGESData_IGESEntity)

//! defines SolidOfLinearExtrusion, Type <164> Form Number <0>
//! in package IGESSolid
//! Solid of linear extrusion is defined by translatin an
//! area determined by a planar curve
class IGESSolid_SolidOfLinearExtrusion : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_SolidOfLinearExtrusion();
  
  //! This method is used to set the fields of the class
  //! SolidOfLinearExtrusion
  //! - aCurve     : the planar curve that is to be translated
  //! - aLength    : the length of extrusion
  //! - aDirection : the vector specifying the direction of extrusion
  //! default (0,0,1)
  Standard_EXPORT void Init (const Handle(IGESData_IGESEntity)& aCurve, const Standard_Real aLength, const gp_XYZ& aDirection);
  
  //! returns the planar curve that is to be translated
  Standard_EXPORT Handle(IGESData_IGESEntity) Curve() const;
  
  //! returns the Extrusion Length
  Standard_EXPORT Standard_Real ExtrusionLength() const;
  
  //! returns the Extrusion direction
  Standard_EXPORT gp_Dir ExtrusionDirection() const;
  
  //! returns ExtrusionDirection after applying TransformationMatrix
  Standard_EXPORT gp_Dir TransformedExtrusionDirection() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_SolidOfLinearExtrusion,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_IGESEntity) theCurve;
  Standard_Real theLength;
  gp_XYZ theDirection;


};







#endif // _IGESSolid_SolidOfLinearExtrusion_HeaderFile
