// Created on: 1999-07-12
// Created by: Denis PASCAL
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

#ifndef _TDocStd_Modified_HeaderFile
#define _TDocStd_Modified_HeaderFile

#include <Standard.hxx>

#include <TDF_Label.hxx>
#include <NCollection_Map.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class Standard_GUID;
class TDF_RelocationTable;

//! Transient attribute which register modified labels.
//! This attribute is attached to root label.
class TDocStd_Modified : public TDF_Attribute
{

public:
  //! API class methods
  //! =================
  Standard_EXPORT static bool IsEmpty(const TDF_Label& access);

  Standard_EXPORT static bool Add(const TDF_Label& alabel);

  Standard_EXPORT static bool Remove(const TDF_Label& alabel);

  Standard_EXPORT static bool Contains(const TDF_Label& alabel);

  //! if <IsEmpty> raise an exception.
  Standard_EXPORT static const NCollection_Map<TDF_Label>& Get(const TDF_Label& access);

  //! remove all modified labels. becomes empty
  Standard_EXPORT static void Clear(const TDF_Label& access);

  //! Modified methods
  //! ================
  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT TDocStd_Modified();

  Standard_EXPORT bool IsEmpty() const;

  Standard_EXPORT void Clear();

  //! add <L> as modified
  Standard_EXPORT bool AddLabel(const TDF_Label& L);

  //! remove <L> as modified
  Standard_EXPORT bool RemoveLabel(const TDF_Label& L);

  //! returns modified label map
  Standard_EXPORT const NCollection_Map<TDF_Label>& Get() const;

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& With) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       Into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& anOS) const override;

  DEFINE_STANDARD_RTTIEXT(TDocStd_Modified, TDF_Attribute)

private:
  NCollection_Map<TDF_Label> myModified;
};

#endif // _TDocStd_Modified_HeaderFile
