// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _TDataStd_GenericExtString_HeaderFile
#define _TDataStd_GenericExtString_HeaderFile

#include <TDF_DerivedAttribute.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_GUID.hxx>

class TDF_RelocationTable;

class TDataStd_GenericExtString;
DEFINE_STANDARD_HANDLE(TDataStd_GenericExtString, TDF_Attribute)

//! An ancestor attribute for all attributes which have TCollection_ExtendedString field.
//! If an attribute inherits this one it should not have drivers for persistence.
//! Also this attribute provides functionality to have on the same label same attributes with
//! different IDs.
class TDataStd_GenericExtString : public TDF_Attribute
{

public:
  //! Sets <S> as name. Raises if <S> is not a valid name.
  Standard_EXPORT virtual void Set(const TCollection_ExtendedString& S);

  //! Sets the explicit user defined GUID  to the attribute.
  Standard_EXPORT void SetID(const Standard_GUID& guid) Standard_OVERRIDE;

  //! Returns the name contained in this name attribute.
  Standard_EXPORT virtual const TCollection_ExtendedString& Get() const;

  //! Returns the ID of the attribute.
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       into,
                             const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TDataStd_GenericExtString, TDF_Attribute)

protected:
  //! A string field of the attribute, participated in persistence.
  TCollection_ExtendedString myString;
  //! A private GUID of the attribute.
  Standard_GUID myID;
};

#endif // _TDataStd_GenericExtString_HeaderFile
