// Created on: 1992-02-03
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

#ifndef _Transfer_TransientMapper_HeaderFile
#define _Transfer_TransientMapper_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Transfer_Finder.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
class Standard_Transient;
class Transfer_DataInfo;
class Transfer_Finder;


class Transfer_TransientMapper;
DEFINE_STANDARD_HANDLE(Transfer_TransientMapper, Transfer_Finder)


class Transfer_TransientMapper : public Transfer_Finder
{

public:

  
  Standard_EXPORT Transfer_TransientMapper(const Handle(Standard_Transient)& akey);
  
  Standard_EXPORT const Handle(Standard_Transient)& Value() const;
  
  Standard_EXPORT Standard_Boolean Equates (const Handle(Transfer_Finder)& other) const;
  
  Standard_EXPORT virtual Handle(Standard_Type) ValueType() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_CString ValueTypeName() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(Transfer_TransientMapper,Transfer_Finder)

protected:




private:


  Handle(Standard_Transient) theval;


};







#endif // _Transfer_TransientMapper_HeaderFile
