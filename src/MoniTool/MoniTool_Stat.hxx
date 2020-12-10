// Created on: 1996-02-15
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _MoniTool_Stat_HeaderFile
#define _MoniTool_Stat_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>
class TCollection_HAsciiString;


//! This class manages Statistics to be queried asynchronously.
//!
//! It is organized as a stack of counters, identified by their
//! levels, from one to ... . Each one has a total account of
//! items to be counted, a count of already passed items, plus a
//! count of "current items". The counters of higher level play on
//! these current items.
//! For instance, if a counter has been opened for 100 items, 40
//! already passed, 20 current, its own percent is 40, but there
//! is the contribution of higher level counters, rated for 20 %
//! of this counter.
//!
//! Hence, a counter is opened, items are added. Also items can be
//! add for sub-counter (of higher level), they will be added
//! definitively when the sub-counter will be closed. When the
//! count has ended, this counter is closed, the counter of
//! lower level cumulates it and goes on. As follows :
//!
//! Way of use :
//! Open(nbitems);
//! Add(..)  :  direct adding
//! Add(..)
//! AddSub (nsub)  :  for sub-counter
//! Open (nbsubs)  :  nbsubs for this sub-counter
//! Add (..)
//! Close        : the sub-counter
//! AddEnd()
//! etc...
//! Close          : the starting counter
//!
//! This means that a counter can be opened in a Stat, regardless
//! to the already opened ones :: this will be cumulated
//!
//! A Current Stat is available, but it is possible to have others
class MoniTool_Stat 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Stat form. At start, one default phase is defined,
  //! with one default step. Then, it suffises to start with a
  //! count of items (and cycles if several) then record items,
  //! to have a queryable report.
  Standard_EXPORT MoniTool_Stat(const Standard_CString title = "");
  
  //! used when starting
  Standard_EXPORT MoniTool_Stat(const MoniTool_Stat& other);
  
  Standard_EXPORT static MoniTool_Stat& Current();
  
  //! Opens a new counter with a starting count of items
  Standard_EXPORT Standard_Integer Open (const Standard_Integer nb = 100);
  
  //! Adds more items to be counted by Add... on current level
  Standard_EXPORT void OpenMore (const Standard_Integer id, const Standard_Integer nb);
  
  //! Directly adds items
  Standard_EXPORT void Add (const Standard_Integer nb = 1);
  
  //! Declares a count of items to be added later. If a sub-counter
  //! is opened, its percentage multiplies this sub-count to compute
  //! the percent of current level
  Standard_EXPORT void AddSub (const Standard_Integer nb = 1);
  
  //! Ends the AddSub and cumulates the sub-count to current level
  Standard_EXPORT void AddEnd();
  
  Standard_EXPORT void Close (const Standard_Integer id);
  
  Standard_EXPORT Standard_Integer Level() const;
  
  Standard_EXPORT Standard_Real Percent (const Standard_Integer fromlev = 0) const;




protected:





private:



  Handle(TCollection_HAsciiString) thetit;
  Standard_Integer thelev;
  Handle(TColStd_HArray1OfInteger) thetot;
  Handle(TColStd_HArray1OfInteger) thedone;
  Handle(TColStd_HArray1OfInteger) thecurr;


};







#endif // _MoniTool_Stat_HeaderFile
