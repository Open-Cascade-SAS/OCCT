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

#ifndef _IGESSelect_DispPerDrawing_HeaderFile
#define _IGESSelect_DispPerDrawing_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Dispatch.hxx>
#include <Standard_Boolean.hxx>
class IGESSelect_ViewSorter;
class TCollection_AsciiString;
class Interface_Graph;
class IFGraph_SubPartsIterator;
class Interface_EntityIterator;


class IGESSelect_DispPerDrawing;
DEFINE_STANDARD_HANDLE(IGESSelect_DispPerDrawing, IFSelect_Dispatch)

//! This type of dispatch defines sets of entities attached to
//! distinct drawings. This information is taken from attached
//! views which appear in the Directory Part. Also Drawing Frames
//! are considered when Drawings are part of input list.
//!
//! Remaining data concern entities not attached to a drawing.
class IGESSelect_DispPerDrawing : public IFSelect_Dispatch
{

public:

  
  //! Creates a DispPerDrawing
  Standard_EXPORT IGESSelect_DispPerDrawing();
  
  //! Returns as Label, "One File per Drawing"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;
  
  //! Computes the list of produced Packets. Packets are computed
  //! by a ViewSorter (SortDrawings with also frames).
  Standard_EXPORT void Packets (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const Standard_OVERRIDE;
  
  //! Returns True, because of entities attached to no view.
  Standard_EXPORT virtual Standard_Boolean CanHaveRemainder() const Standard_OVERRIDE;
  
  //! Returns Remainder which is a set of Entities.
  //! It is supposed to be called once Packets has been called.
  Standard_EXPORT virtual Interface_EntityIterator Remainder (const Interface_Graph& G) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_DispPerDrawing,IFSelect_Dispatch)

protected:




private:


  Handle(IGESSelect_ViewSorter) thesorter;


};







#endif // _IGESSelect_DispPerDrawing_HeaderFile
