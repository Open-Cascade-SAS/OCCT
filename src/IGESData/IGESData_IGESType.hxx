// Created on: 1992-04-06
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IGESData_IGESType_HeaderFile
#define _IGESData_IGESType_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>


//! taken from directory part of an entity (from file or model),
//! gives "type" and "form" data, used to recognize entity's type
class IGESData_IGESType 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IGESData_IGESType();
  
  Standard_EXPORT IGESData_IGESType(const Standard_Integer atype, const Standard_Integer aform);
  
  //! returns "type" data
  Standard_EXPORT Standard_Integer Type() const;
  
  //! returns "form" data
  Standard_EXPORT Standard_Integer Form() const;
  
  //! compares two IGESTypes, avoiding comparing their fields
  Standard_EXPORT Standard_Boolean IsEqual (const IGESData_IGESType& another) const;
Standard_Boolean operator == (const IGESData_IGESType& another) const
{
  return IsEqual(another);
}
  
  //! resets fields (useful when an IGESType is stored as mask)
  Standard_EXPORT void Nullify();




protected:





private:



  Standard_Integer thetype;
  Standard_Integer theform;


};







#endif // _IGESData_IGESType_HeaderFile
