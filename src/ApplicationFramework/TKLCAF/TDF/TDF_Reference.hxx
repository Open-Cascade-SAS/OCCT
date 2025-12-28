// Created on: 2000-03-01
// Created by: Denis PASCAL
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

#ifndef _TDF_Reference_HeaderFile
#define _TDF_Reference_HeaderFile

#include <Standard.hxx>

#include <TDF_Label.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_RelocationTable;
class TDF_DataSet;

//! This attribute is used to store in the framework a
//! reference to an other label.
class TDF_Reference : public TDF_Attribute
{

public:
  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT static occ::handle<TDF_Reference> Set(const TDF_Label& I,
                                                        const TDF_Label& Origin);

  Standard_EXPORT void Set(const TDF_Label& Origin);

  Standard_EXPORT TDF_Label Get() const;

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& With) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       Into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT void References(const occ::handle<TDF_DataSet>& DS) const override;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& anOS) const override;

  Standard_EXPORT TDF_Reference();

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TDF_Reference, TDF_Attribute)

private:
  TDF_Label myOrigin;
};

#endif // _TDF_Reference_HeaderFile
