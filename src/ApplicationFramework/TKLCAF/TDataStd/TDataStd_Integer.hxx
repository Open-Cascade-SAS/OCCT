// Created on: 1997-02-06
// Created by: Denis PASCAL
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

#ifndef _TDataStd_Integer_HeaderFile
#define _TDataStd_Integer_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDF_Label;
class TDF_RelocationTable;

//! The basis to define an integer attribute.
class TDataStd_Integer : public TDF_Attribute
{

public:
  //! class methods
  //! =============
  //! Returns the GUID for integers.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds, or creates, an Integer attribute and sets <value>
  //! the Integer attribute is returned.
  Standard_EXPORT static occ::handle<TDataStd_Integer> Set(const TDF_Label& label, const int value);

  //! Finds, or creates, an Integer attribute with explicit user defined <guid> and sets <value>.
  //! The Integer attribute is returned.
  Standard_EXPORT static occ::handle<TDataStd_Integer> Set(const TDF_Label&     label,
                                                           const Standard_GUID& guid,
                                                           const int            value);

  //! Integer methods
  //! ===============
  Standard_EXPORT void Set(const int V);

  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID(const Standard_GUID& guid) override;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() override;

  //! Returns the integer value contained in the attribute.
  Standard_EXPORT int Get() const;

  //! Returns True if there is a reference on the same label
  Standard_EXPORT bool IsCaptured() const;

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& With) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       Into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const override;

  Standard_EXPORT TDataStd_Integer();

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TDataStd_Integer, TDF_Attribute)

private:
  int           myValue;
  Standard_GUID myID;
};

#endif // _TDataStd_Integer_HeaderFile
