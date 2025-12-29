// Created on: 1997-12-16
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

#ifndef _TDataStd_Expression_HeaderFile
#define _TDataStd_Expression_HeaderFile

#include <Standard.hxx>

#include <TCollection_ExtendedString.hxx>
#include <TDF_Attribute.hxx>
#include <NCollection_List.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_RelocationTable;

//! Expression attribute.
//! ====================
//!
//! * Data Structure of the Expression is stored in a
//! string and references to variables used by the string
//!
//! Warning: To be consistent, each Variable referenced by the
//! expression must have its equivalent in the string
class TDataStd_Expression : public TDF_Attribute
{

public:
  //! class methods
  //! =============
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Find, or create, an Expression attribute.
  //! Expressionmethods
  //! ============
  Standard_EXPORT static occ::handle<TDataStd_Expression> Set(const TDF_Label& label);

  Standard_EXPORT TDataStd_Expression();

  //! build and return the expression name
  Standard_EXPORT TCollection_ExtendedString Name() const;

  Standard_EXPORT void SetExpression(const TCollection_ExtendedString& E);

  Standard_EXPORT const TCollection_ExtendedString& GetExpression() const;

  Standard_EXPORT NCollection_List<occ::handle<TDF_Attribute>>& GetVariables();

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& With) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       Into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& anOS) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TDataStd_Expression, TDF_Attribute)

protected:
  TCollection_ExtendedString                   myExpression;
  NCollection_List<occ::handle<TDF_Attribute>> myVariables;
};

#endif // _TDataStd_Expression_HeaderFile
