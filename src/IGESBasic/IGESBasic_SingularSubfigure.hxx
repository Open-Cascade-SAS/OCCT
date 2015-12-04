// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESBasic_SingularSubfigure_HeaderFile
#define _IGESBasic_SingularSubfigure_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESBasic_SubfigureDef;
class gp_XYZ;


class IGESBasic_SingularSubfigure;
DEFINE_STANDARD_HANDLE(IGESBasic_SingularSubfigure, IGESData_IGESEntity)

//! defines SingularSubfigure, Type <408> Form <0>
//! in package IGESBasic
//! Defines the occurrence of a single instance of the
//! defined Subfigure.
class IGESBasic_SingularSubfigure : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESBasic_SingularSubfigure();
  
  //! This method is used to set the fields of the class
  //! SingularSubfigure
  //! - aSubfigureDef : the Subfigure Definition entity
  //! - aTranslation  : used to store the X,Y,Z coord
  //! - hasScale      : Indicates the presence of scale factor
  //! - aScale        : Used to store the scale factor
  Standard_EXPORT void Init (const Handle(IGESBasic_SubfigureDef)& aSubfigureDef, const gp_XYZ& aTranslation, const Standard_Boolean hasScale, const Standard_Real aScale);
  
  //! returns the subfigure definition entity
  Standard_EXPORT Handle(IGESBasic_SubfigureDef) Subfigure() const;
  
  //! returns the X, Y, Z coordinates
  Standard_EXPORT gp_XYZ Translation() const;
  
  //! returns the scale factor
  //! if hasScaleFactor is False, returns 1.0 (default)
  Standard_EXPORT Standard_Real ScaleFactor() const;
  
  //! returns a boolean indicating whether scale factor
  //! is present or not
  Standard_EXPORT Standard_Boolean HasScaleFactor() const;
  
  //! returns the Translation after transformation
  Standard_EXPORT gp_XYZ TransformedTranslation() const;




  DEFINE_STANDARD_RTTIEXT(IGESBasic_SingularSubfigure,IGESData_IGESEntity)

protected:




private:


  Handle(IGESBasic_SubfigureDef) theSubfigureDef;
  gp_XYZ theTranslation;
  Standard_Real theScaleFactor;
  Standard_Boolean hasScaleFactor;


};







#endif // _IGESBasic_SingularSubfigure_HeaderFile
