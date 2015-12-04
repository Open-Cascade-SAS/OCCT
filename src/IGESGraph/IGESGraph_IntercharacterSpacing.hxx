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

#ifndef _IGESGraph_IntercharacterSpacing_HeaderFile
#define _IGESGraph_IntercharacterSpacing_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>


class IGESGraph_IntercharacterSpacing;
DEFINE_STANDARD_HANDLE(IGESGraph_IntercharacterSpacing, IGESData_IGESEntity)

//! defines IGESIntercharacterSpacing, Type <406> Form <18>
//! in package IGESGraph
//!
//! Specifies the gap between letters when fixed-pitch
//! spacing is used
class IGESGraph_IntercharacterSpacing : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGraph_IntercharacterSpacing();
  
  //! This method is used to set the fields of the class
  //! IntercharacterSpacing
  //! - nbProps  : Number of property values (NP = 1)
  //! - anISpace : Intercharacter spacing percentage
  Standard_EXPORT void Init (const Standard_Integer nbProps, const Standard_Real anISpace);
  
  //! returns the number of property values in <me>
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns the Intercharacter Space of <me> in percentage
  //! of the text height (Range = 0..100)
  Standard_EXPORT Standard_Real ISpace() const;




  DEFINE_STANDARD_RTTIEXT(IGESGraph_IntercharacterSpacing,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Real theISpace;


};







#endif // _IGESGraph_IntercharacterSpacing_HeaderFile
