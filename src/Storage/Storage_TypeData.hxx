// Created on: 1997-02-06
// Created by: Kernel
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Storage_TypeData_HeaderFile
#define _Storage_TypeData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Storage_PType.hxx>
#include <Storage_Error.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
class Standard_NoSuchObject;
class Storage_Schema;
class Storage_BaseDriver;
class TCollection_AsciiString;


class Storage_TypeData;
DEFINE_STANDARD_HANDLE(Storage_TypeData, Standard_Transient)


class Storage_TypeData : public Standard_Transient
{

public:

  
  Standard_EXPORT Storage_TypeData();

  Standard_EXPORT Standard_Boolean Read (const Handle(Storage_BaseDriver)& theDriver);
  
  Standard_EXPORT Standard_Integer NumberOfTypes() const;

  //! add a type to the list
  Standard_EXPORT void AddType (const TCollection_AsciiString& aName, const Standard_Integer aTypeNum);
  
  //! returns the name of the type with number <aTypeNum>
  Standard_EXPORT TCollection_AsciiString Type (const Standard_Integer aTypeNum) const;

  //! returns the name of the type with number <aTypeNum>
  Standard_EXPORT Standard_Integer Type (const TCollection_AsciiString& aTypeName) const;
  
  Standard_EXPORT Standard_Boolean IsType (const TCollection_AsciiString& aName) const;
  
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) Types() const;
  
  Standard_EXPORT Storage_Error ErrorStatus() const;
  
  Standard_EXPORT TCollection_AsciiString ErrorStatusExtension() const;
  
  Standard_EXPORT void ClearErrorStatus();
  
  Standard_EXPORT void Clear();


friend class Storage_Schema;


  DEFINE_STANDARD_RTTIEXT(Storage_TypeData,Standard_Transient)

protected:




private:

  
  Standard_EXPORT void SetErrorStatus (const Storage_Error anError);
  
  Standard_EXPORT void SetErrorStatusExtension (const TCollection_AsciiString& anErrorExt);

  Storage_PType myPt;
  Storage_Error myErrorStatus;
  TCollection_AsciiString myErrorStatusExt;


};







#endif // _Storage_TypeData_HeaderFile
