// Created on: 2004-01-09
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

#ifndef _XCAFDoc_DimTol_HeaderFile
#define _XCAFDoc_DimTol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDF_Attribute.hxx>
class TCollection_HAsciiString;
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;


class XCAFDoc_DimTol;
DEFINE_STANDARD_HANDLE(XCAFDoc_DimTol, TDF_Attribute)

//! attribute to store dimension and tolerance
class XCAFDoc_DimTol : public TDF_Attribute
{

public:

  
  Standard_EXPORT XCAFDoc_DimTol();
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT static Handle(XCAFDoc_DimTol) Set (const TDF_Label& label, const Standard_Integer kind, const Handle(TColStd_HArray1OfReal)& aVal, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription);
  
  Standard_EXPORT void Set (const Standard_Integer kind, const Handle(TColStd_HArray1OfReal)& aVal, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription);
  
  Standard_EXPORT Standard_Integer GetKind() const;
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetVal() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetName() const;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) GetDescription() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XCAFDoc_DimTol,TDF_Attribute)

protected:




private:


  Standard_Integer myKind;
  Handle(TColStd_HArray1OfReal) myVal;
  Handle(TCollection_HAsciiString) myName;
  Handle(TCollection_HAsciiString) myDescription;


};







#endif // _XCAFDoc_DimTol_HeaderFile
