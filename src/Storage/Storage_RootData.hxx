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

#ifndef _Storage_RootData_HeaderFile
#define _Storage_RootData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Storage_MapOfPers.hxx>
#include <Storage_Error.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Storage_HSeqOfRoot.hxx>
class Storage_BaseDriver;
class Storage_Root;
class Standard_Persistent;


class Storage_RootData;
DEFINE_STANDARD_HANDLE(Storage_RootData, Standard_Transient)


class Storage_RootData : public Standard_Transient
{

public:

  
  Standard_EXPORT Storage_RootData();

  Standard_EXPORT Standard_Boolean Read (const Handle(Storage_BaseDriver)& theDriver);
  
  //! returns the number of roots.
  Standard_EXPORT Standard_Integer NumberOfRoots() const;
  
  //! add a root to <me>. If a root with same name is present, it
  //! will be replaced by <aRoot>.
  Standard_EXPORT void AddRoot (const Handle(Storage_Root)& aRoot);
  
  Standard_EXPORT Handle(Storage_HSeqOfRoot) Roots() const;
  
  //! find a root with name <aName>.
  Standard_EXPORT Handle(Storage_Root) Find (const TCollection_AsciiString& aName) const;
  
  //! returns Standard_True if <me> contains a root named <aName>
  Standard_EXPORT Standard_Boolean IsRoot (const TCollection_AsciiString& aName) const;
  
  //! remove the root named <aName>.
  Standard_EXPORT void RemoveRoot (const TCollection_AsciiString& aName);
  
  Standard_EXPORT Storage_Error ErrorStatus() const;
  
  Standard_EXPORT TCollection_AsciiString ErrorStatusExtension() const;
  
  Standard_EXPORT void ClearErrorStatus();

  Standard_EXPORT void UpdateRoot (const TCollection_AsciiString& aName, const Handle(Standard_Persistent)& aPers);

friend class Storage_Schema;


  DEFINE_STANDARD_RTTIEXT(Storage_RootData,Standard_Transient)

protected:




private:

  
  
  Standard_EXPORT void SetErrorStatus (const Storage_Error anError);
  
  Standard_EXPORT void SetErrorStatusExtension (const TCollection_AsciiString& anErrorExt);

  Storage_MapOfPers myObjects;
  Storage_Error myErrorStatus;
  TCollection_AsciiString myErrorStatusExt;


};







#endif // _Storage_RootData_HeaderFile
