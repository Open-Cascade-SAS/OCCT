// Created on: 1994-05-31
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IGESSelect_DispPerSingleView_HeaderFile
#define _IGESSelect_DispPerSingleView_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Dispatch.hxx>
#include <Standard_Boolean.hxx>
class IGESSelect_ViewSorter;
class TCollection_AsciiString;
class Interface_Graph;
class IFGraph_SubPartsIterator;
class Interface_EntityIterator;


class IGESSelect_DispPerSingleView;
DEFINE_STANDARD_HANDLE(IGESSelect_DispPerSingleView, IFSelect_Dispatch)

//! This type of dispatch defines sets of entities attached to
//! distinct single views. This information appears in the
//! Directory Part. Drawings are taken into account too,
//! because of their frames (proper lists of annotations)
//!
//! Remaining data concern entities not attached to a single view.
class IGESSelect_DispPerSingleView : public IFSelect_Dispatch
{

public:

  
  //! Creates a DispPerSingleView
  Standard_EXPORT IGESSelect_DispPerSingleView();
  
  //! Returns as Label, "One File per single View or Drawing Frame"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;
  
  //! Computes the list of produced Packets. Packets are computed
  //! by a ViewSorter (SortSingleViews with also frames).
  Standard_EXPORT void Packets (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const Standard_OVERRIDE;
  
  //! Returns True, because of entities attached to no view.
  Standard_EXPORT virtual Standard_Boolean CanHaveRemainder() const Standard_OVERRIDE;
  
  //! Returns Remainder which is a set of Entities.
  //! It is supposed to be called once Packets has been called.
  Standard_EXPORT virtual Interface_EntityIterator Remainder (const Interface_Graph& G) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_DispPerSingleView,IFSelect_Dispatch)

protected:




private:


  Handle(IGESSelect_ViewSorter) thesorter;


};







#endif // _IGESSelect_DispPerSingleView_HeaderFile
