// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_RealList_HeaderFile
#define _TDataStd_RealList_HeaderFile

#include <Standard.hxx>

#include <TColStd_ListOfReal.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDF_Label;
class TDF_RelocationTable;

class TDataStd_RealList;
DEFINE_STANDARD_HANDLE(TDataStd_RealList, TDF_Attribute)

//! Contains a list of doubles.
class TDataStd_RealList : public TDF_Attribute
{

public:
  //! Static methods
  //! ==============
  //! Returns the ID of the list of doubles attribute.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds or creates a list of double values attribute.
  Standard_EXPORT static Handle(TDataStd_RealList) Set(const TDF_Label& label);

  //! Finds or creates a list of double values attribute with explicit user defined <guid>.
  Standard_EXPORT static Handle(TDataStd_RealList) Set(const TDF_Label&     label,
                                                       const Standard_GUID& theGuid);

  Standard_EXPORT TDataStd_RealList();

  Standard_EXPORT Standard_Boolean IsEmpty() const;

  Standard_EXPORT Standard_Integer Extent() const;

  Standard_EXPORT void Prepend(const Standard_Real value);

  Standard_EXPORT void Append(const Standard_Real value);

  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID(const Standard_GUID& theGuid) Standard_OVERRIDE;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() Standard_OVERRIDE;

  //! Inserts the <value> before the first meet of <before_value>.
  Standard_EXPORT Standard_Boolean InsertBefore(const Standard_Real value,
                                                const Standard_Real before_value);

  //! Inserts the <value> before the <index> position.
  //! The indices start with 1 .. Extent().
  Standard_EXPORT Standard_Boolean InsertBeforeByIndex(const Standard_Integer index,
                                                       const Standard_Real    before_value);

  //! Inserts the <value> after the first meet of <after_value>.
  Standard_EXPORT Standard_Boolean InsertAfter(const Standard_Real value,
                                               const Standard_Real after_value);

  //! Inserts the <value> after the <index> position.
  //! The indices start with 1 .. Extent().
  Standard_EXPORT Standard_Boolean InsertAfterByIndex(const Standard_Integer index,
                                                      const Standard_Real    after_value);

  //! Removes the first meet of the <value>.
  Standard_EXPORT Standard_Boolean Remove(const Standard_Real value);

  //! Removes a value at <index> position.
  Standard_EXPORT Standard_Boolean RemoveByIndex(const Standard_Integer index);

  Standard_EXPORT void Clear();

  Standard_EXPORT Standard_Real First() const;

  Standard_EXPORT Standard_Real Last() const;

  Standard_EXPORT const TColStd_ListOfReal& List() const;

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& With) Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       Into,
                             const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TDataStd_RealList, TDF_Attribute)

protected:
private:
  TColStd_ListOfReal myList;
  Standard_GUID      myID;
};

#endif // _TDataStd_RealList_HeaderFile
