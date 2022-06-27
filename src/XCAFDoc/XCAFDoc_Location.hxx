// Created on: 2000-08-15
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_Location_HeaderFile
#define _XCAFDoc_Location_HeaderFile

#include <Standard.hxx>

#include <TopLoc_Location.hxx>
#include <TDF_Attribute.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_RelocationTable;


class XCAFDoc_Location;
DEFINE_STANDARD_HANDLE(XCAFDoc_Location, TDF_Attribute)

//! attribute to store TopLoc_Location
class XCAFDoc_Location : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  Standard_EXPORT XCAFDoc_Location();
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Find, or create, a Location attribute and set it's value
  //! the Location attribute is returned.
  //! Location methods
  //! ===============
  Standard_EXPORT static Handle(XCAFDoc_Location) Set (const TDF_Label& label, const TopLoc_Location& Loc);
  
  Standard_EXPORT void Set (const TopLoc_Location& Loc);
  
  //! Returns True if there is a reference on the same label
  Standard_EXPORT const TopLoc_Location& Get() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XCAFDoc_Location,TDF_Attribute)

protected:




private:


  TopLoc_Location myLocation;


};







#endif // _XCAFDoc_Location_HeaderFile
