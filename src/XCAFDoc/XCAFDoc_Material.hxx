// Created on: 2003-03-05
// Created by: Sergey KUUL
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_Material_HeaderFile
#define _XCAFDoc_Material_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <TDF_Attribute.hxx>
class TCollection_HAsciiString;
class Standard_GUID;
class TDF_Label;
class TDF_RelocationTable;


class XCAFDoc_Material;
DEFINE_STANDARD_HANDLE(XCAFDoc_Material, TDF_Attribute)

//! attribute to store material
class XCAFDoc_Material : public TDF_Attribute
{

public:

  
  Standard_EXPORT XCAFDoc_Material();
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT static Handle(XCAFDoc_Material) Set (const TDF_Label& label, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Standard_Real aDensity, const Handle(TCollection_HAsciiString)& aDensName, const Handle(TCollection_HAsciiString)& aDensValType);
  
  Standard_EXPORT void Set (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Standard_Real aDensity, const Handle(TCollection_HAsciiString)& aDensName, const Handle(TCollection_HAsciiString)& aDensValType);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetName() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetDescription() const;
  
  Standard_EXPORT Standard_Real GetDensity() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetDensName() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetDensValType() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XCAFDoc_Material,TDF_Attribute)

protected:




private:


  Handle(TCollection_HAsciiString) myName;
  Handle(TCollection_HAsciiString) myDescription;
  Standard_Real myDensity;
  Handle(TCollection_HAsciiString) myDensName;
  Handle(TCollection_HAsciiString) myDensValType;


};







#endif // _XCAFDoc_Material_HeaderFile
