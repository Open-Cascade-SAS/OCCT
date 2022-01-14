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

#ifndef _IGESGraph_LineFontPredefined_HeaderFile
#define _IGESGraph_LineFontPredefined_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>


class IGESGraph_LineFontPredefined;
DEFINE_STANDARD_HANDLE(IGESGraph_LineFontPredefined, IGESData_IGESEntity)

//! defines IGESLineFontPredefined, Type <406> Form <19>
//! in package IGESGraph
//!
//! Provides the ability to specify a line font pattern
//! from a predefined list rather than from
//! Directory Entry Field 4
class IGESGraph_LineFontPredefined : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGraph_LineFontPredefined();
  
  //! This method is used to set the fields of the class
  //! LineFontPredefined
  //! - nbProps              : Number of property values (NP = 1)
  //! - aLineFontPatternCode : Line Font Pattern Code
  Standard_EXPORT void Init (const Standard_Integer nbProps, const Standard_Integer aLineFontPatternCode);
  
  //! returns the number of property values in <me>
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns the Line Font Pattern Code of <me>
  Standard_EXPORT Standard_Integer LineFontPatternCode() const;




  DEFINE_STANDARD_RTTIEXT(IGESGraph_LineFontPredefined,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Integer theLineFontPatternCode;


};







#endif // _IGESGraph_LineFontPredefined_HeaderFile
