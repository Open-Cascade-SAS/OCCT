// Created on: 2004-01-15
// Created by: Sergey KUUL
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_Datum_HeaderFile
#define _XCAFDoc_Datum_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
class TCollection_HAsciiString;
class Standard_GUID;
class TDF_Label;
class TDF_RelocationTable;
class XCAFDimTolObjects_DatumObject;

// resolve name collisions with WinAPI headers
#ifdef GetObject
  #undef GetObject
#endif

class XCAFDoc_Datum;
DEFINE_STANDARD_HANDLE(XCAFDoc_Datum, TDF_Attribute)

//! attribute to store datum
class XCAFDoc_Datum : public TDF_Attribute
{

public:

  
  Standard_EXPORT XCAFDoc_Datum();
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT static Handle(XCAFDoc_Datum) Set (const TDF_Label& label, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(TCollection_HAsciiString)& anIdentification);

  Standard_EXPORT static   Handle(XCAFDoc_Datum) Set (const TDF_Label& theLabel);
  
  Standard_EXPORT void Set (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(TCollection_HAsciiString)& anIdentification);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetName() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetDescription() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetIdentification() const;
  
  //! Returns dimension object data taken from the paren's label and its sub-labels.
  Standard_EXPORT Handle(XCAFDimTolObjects_DatumObject) GetObject() const;
  
  //! Updates parent's label and its sub-labels with data taken from theDatumObject.
  //! Old data associated with the label will be lost.
  Standard_EXPORT void SetObject(const Handle(XCAFDimTolObjects_DatumObject)& theDatumObject);
      
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_Datum,TDF_Attribute)

private:


  Handle(TCollection_HAsciiString) myName;
  Handle(TCollection_HAsciiString) myDescription;
  Handle(TCollection_HAsciiString) myIdentification;

};

#endif // _XCAFDoc_Datum_HeaderFile
