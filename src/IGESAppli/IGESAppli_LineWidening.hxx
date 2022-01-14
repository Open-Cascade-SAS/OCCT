// Created on: 1993-01-11
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

#ifndef _IGESAppli_LineWidening_HeaderFile
#define _IGESAppli_LineWidening_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>


class IGESAppli_LineWidening;
DEFINE_STANDARD_HANDLE(IGESAppli_LineWidening, IGESData_IGESEntity)

//! defines LineWidening, Type <406> Form <5>
//! in package IGESAppli
//! Defines the characteristics of entities when they are
//! used to define locations of items.
class IGESAppli_LineWidening : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESAppli_LineWidening();
  
  //! This method is used to set the fields of the class
  //! LineWidening
  //! - nbPropVal   : Number of property values = 5
  //! - aWidth      : Width of metalization
  //! - aCornering  : Cornering codes
  //! 0 = rounded
  //! 1 = squared
  //! - aExtnFlag   : Extension Flag
  //! 0 = No Extension
  //! 1 = One-half width extension
  //! 2 = Extn set by ExtnVal
  //! - aJustifFlag : Justification flag
  //! 0 = Center justified
  //! 1 = left justified
  //! 2 = right justified
  //! - aExtnVal    : Extension value if aExtnFlag = 2
  Standard_EXPORT void Init (const Standard_Integer nbPropVal, const Standard_Real aWidth, const Standard_Integer aCornering, const Standard_Integer aExtnFlag, const Standard_Integer aJustifFlag, const Standard_Real aExtnVal);
  
  //! returns the number of property values
  //! is always 5
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns the width of metallization
  Standard_EXPORT Standard_Real WidthOfMetalization() const;
  
  //! returns the cornering code
  //! 0 = Rounded  /   1 = Squared
  Standard_EXPORT Standard_Integer CorneringCode() const;
  
  //! returns the extension flag
  //! 0 = No extension
  //! 1 = One-half width extension
  //! 2 = Extension set by theExtnVal
  Standard_EXPORT Standard_Integer ExtensionFlag() const;
  
  //! returns the justification flag
  //! 0 = Centre justified
  //! 1 = Left justified
  //! 2 = Right justified
  Standard_EXPORT Standard_Integer JustificationFlag() const;
  
  //! returns the Extension Value
  //! Present only if theExtnFlag = 2
  Standard_EXPORT Standard_Real ExtensionValue() const;




  DEFINE_STANDARD_RTTIEXT(IGESAppli_LineWidening,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Real theWidth;
  Standard_Integer theCorneringCode;
  Standard_Integer theExtensionFlag;
  Standard_Integer theJustificationFlag;
  Standard_Real theExtensionValue;


};







#endif // _IGESAppli_LineWidening_HeaderFile
