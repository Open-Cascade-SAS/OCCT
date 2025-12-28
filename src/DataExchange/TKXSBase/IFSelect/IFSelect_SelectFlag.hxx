// Created on: 1995-09-05
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _IFSelect_SelectFlag_HeaderFile
#define _IFSelect_SelectFlag_HeaderFile

#include <Standard.hxx>

#include <TCollection_AsciiString.hxx>
#include <IFSelect_SelectExtract.hxx>
#include <Standard_Integer.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class Standard_Transient;
class Interface_InterfaceModel;

//! A SelectFlag queries a flag noted in the bitmap of the Graph.
//! The Flag is designated by its Name. Flag Names are defined
//! by Work Session and, as necessary, other functional objects
//!
//! WorkSession from IFSelect defines flag "Incorrect"
//! Objects which control application running define some others
class IFSelect_SelectFlag : public IFSelect_SelectExtract
{

public:
  //! Creates a Select Flag, to query a flag designated by its name
  Standard_EXPORT IFSelect_SelectFlag(const char* flagname);

  //! Returns the name of the flag
  Standard_EXPORT const char* FlagName() const;

  //! Returns the list of selected entities. It is redefined to
  //! work on the graph itself (not queried by sort)
  //!
  //! An entity is selected if its flag is True on Direct mode,
  //! False on Reversed mode
  //!
  //! If flag does not exist for the given name, returns an empty
  //! result, whatever the Direct/Reversed sense
  Standard_EXPORT Interface_EntityIterator RootResult(
    const Interface_Graph& G) const override;

  //! Returns always False because RootResult has done the work
  Standard_EXPORT bool Sort(const int                                    rank,
                            const occ::handle<Standard_Transient>&       ent,
                            const occ::handle<Interface_InterfaceModel>& model) const override;

  //! Returns a text defining the criterium, includes the flag name
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const override;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectFlag, IFSelect_SelectExtract)

private:
  TCollection_AsciiString thename;
};

#endif // _IFSelect_SelectFlag_HeaderFile
