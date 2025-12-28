// Created on: 1999-06-11
// Created by: Sergey RUIN
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TDataStd_UAttribute_HeaderFile
#define _TDataStd_UAttribute_HeaderFile

#include <Standard.hxx>

#include <Standard_GUID.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class TDF_RelocationTable;
class TDF_DataSet;

class TDataStd_UAttribute : public TDF_Attribute
{

public:
  //! api class methods
  //! =============
  //! Find, or create, a UAttribute attribute with <LocalID> as Local GUID.
  //! The UAttribute attribute is returned.
  //! UAttribute methods
  //! ============
  Standard_EXPORT static occ::handle<TDataStd_UAttribute> Set(const TDF_Label&     label,
                                                              const Standard_GUID& LocalID);

  Standard_EXPORT TDataStd_UAttribute();

  Standard_EXPORT void SetID(const Standard_GUID& LocalID) override;

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& with) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT void References(const occ::handle<TDF_DataSet>& DS) const override;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& anOS) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TDataStd_UAttribute, TDF_Attribute)

private:
  Standard_GUID myID;
};

#endif // _TDataStd_UAttribute_HeaderFile
