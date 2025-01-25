// Created on: 1995-10-25
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

#ifndef _IFSelect_SelectSent_HeaderFile
#define _IFSelect_SelectSent_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IFSelect_SelectExtract.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;

class IFSelect_SelectSent;
DEFINE_STANDARD_HANDLE(IFSelect_SelectSent, IFSelect_SelectExtract)

//! This class returns entities according sending to a file
//! Once a model has been loaded, further sendings are recorded
//! as status in the graph (for each value, a count of sendings)
//!
//! Hence, it is possible to query entities : sent ones (at least
//! once), non-sent (i.e. remaining) ones, duplicated ones, etc...
//!
//! This selection performs this query
class IFSelect_SelectSent : public IFSelect_SelectExtract
{

public:
  //! Creates a SelectSent :
  //! sentcount = 0 -> remaining (non-sent) entities
  //! sentcount = 1, atleast = True (D) -> sent (at least once)
  //! sentcount = 2, atleast = True -> duplicated (sent least twice)
  //! etc...
  //! sentcount = 1, atleast = False -> sent just once (non-dupl.d)
  //! sentcount = 2, atleast = False -> sent just twice
  //! etc...
  Standard_EXPORT IFSelect_SelectSent(const Standard_Integer sentcount = 1,
                                      const Standard_Boolean atleast   = Standard_True);

  //! Returns the queried count of sending
  Standard_EXPORT Standard_Integer SentCount() const;

  //! Returns the <atleast> status, True for sending at least the
  //! sending count, False for sending exactly the sending count
  //! Remark : if SentCount is 0, AtLeast is ignored
  Standard_EXPORT Standard_Boolean AtLeast() const;

  //! Returns the list of selected entities. It is redefined to
  //! work on the graph itself (not queried by sort)
  //!
  //! An entity is selected if its count complies to the query in
  //! Direct Mode, rejected in Reversed Mode
  //!
  //! Query works on the sending count recorded as status in Graph
  Standard_EXPORT virtual Interface_EntityIterator RootResult(const Interface_Graph& G) const
    Standard_OVERRIDE;

  //! Returns always False because RootResult has done the work
  Standard_EXPORT Standard_Boolean
    Sort(const Standard_Integer                  rank,
         const Handle(Standard_Transient)&       ent,
         const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;

  //! Returns a text defining the criterium : query :
  //! SentCount = 0 -> "Remaining (non-sent) entities"
  //! SentCount = 1, AtLeast = True  -> "Sent entities"
  //! SentCount = 1, AtLeast = False -> "Sent once (no duplicated)"
  //! SentCount = 2, AtLeast = True  -> "Sent several times entities"
  //! SentCount = 2, AtLeast = False -> "Sent twice entities"
  //! SentCount > 2, AtLeast = True  -> "Sent at least <count> times entities"
  //! SentCount > 2, AtLeast = False -> "Sent <count> times entities"
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectSent, IFSelect_SelectExtract)

protected:
private:
  Standard_Integer thecnt;
  Standard_Boolean thelst;
};

#endif // _IFSelect_SelectSent_HeaderFile
