// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Anand NATRAJAN )
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

#ifndef _IGESAppli_PWBDrilledHole_HeaderFile
#define _IGESAppli_PWBDrilledHole_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>


class IGESAppli_PWBDrilledHole;
DEFINE_STANDARD_HANDLE(IGESAppli_PWBDrilledHole, IGESData_IGESEntity)

//! defines PWBDrilledHole, Type <406> Form <26>
//! in package IGESAppli
//! Used to identify an entity that locates a drilled hole
//! and to specify the characteristics of the drilled hole
class IGESAppli_PWBDrilledHole : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESAppli_PWBDrilledHole();
  
  //! This method is used to set the fields of the class
  //! PWBDrilledHole
  //! - nbPropVal  : number of property values, always = 3
  //! - aDrillDia  : Drill diameter size
  //! - aFinishDia : Finish diameter size
  //! - aCode      : Function code for drilled hole
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Standard_Real aDrillDia, const Standard_Real aFinishDia, const Standard_Integer aCode);
  
  //! returns number of property values, always = 3
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns Drill diameter size
  Standard_EXPORT Standard_Real DrillDiameterSize() const;
  
  //! returns Finish diameter size
  Standard_EXPORT Standard_Real FinishDiameterSize() const;
  
  //! returns Function code for drilled hole
  //! is 0, 1, 2, 3, 4, 5 or 5001-9999
  Standard_EXPORT Standard_Integer FunctionCode() const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_PWBDrilledHole,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Real theDrillDiameter;
  Standard_Real theFinishDiameter;
  Standard_Integer theFunctionCode;


};







#endif // _IGESAppli_PWBDrilledHole_HeaderFile
