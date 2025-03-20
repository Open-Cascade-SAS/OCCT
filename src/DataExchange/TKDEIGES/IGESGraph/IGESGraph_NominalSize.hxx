// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGraph_NominalSize_HeaderFile
#define _IGESGraph_NominalSize_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>
class TCollection_HAsciiString;

class IGESGraph_NominalSize;
DEFINE_STANDARD_HANDLE(IGESGraph_NominalSize, IGESData_IGESEntity)

//! defines IGESNominalSize, Type <406> Form <13>
//! in package IGESGraph
//!
//! Specifies a value, a name, and optionally a
//! reference to an engineering standard
class IGESGraph_NominalSize : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGraph_NominalSize();

  //! This method is used to set the fields of the class
  //! NominalSize
  //! - nbProps           : Number of property values (2 or 3)
  //! - aNominalSizeValue : NominalSize Value
  //! - aNominalSizeName  : NominalSize Name
  //! - aStandardName     : Name of relevant engineering standard
  Standard_EXPORT void Init(const Standard_Integer                  nbProps,
                            const Standard_Real                     aNominalSizeValue,
                            const Handle(TCollection_HAsciiString)& aNominalSizeName,
                            const Handle(TCollection_HAsciiString)& aStandardName);

  //! returns the number of property values in <me>
  Standard_EXPORT Standard_Integer NbPropertyValues() const;

  //! returns the value of <me>
  Standard_EXPORT Standard_Real NominalSizeValue() const;

  //! returns the name of <me>
  Standard_EXPORT Handle(TCollection_HAsciiString) NominalSizeName() const;

  //! returns True if an engineering Standard is defined for <me>
  //! else, returns False
  Standard_EXPORT Standard_Boolean HasStandardName() const;

  //! returns the name of the relevant engineering standard of <me>
  Standard_EXPORT Handle(TCollection_HAsciiString) StandardName() const;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_NominalSize, IGESData_IGESEntity)

protected:
private:
  Standard_Integer                 theNbPropertyValues;
  Standard_Real                    theNominalSizeValue;
  Handle(TCollection_HAsciiString) theNominalSizeName;
  Handle(TCollection_HAsciiString) theStandardName;
};

#endif // _IGESGraph_NominalSize_HeaderFile
