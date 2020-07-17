// Created on: 1998-06-03
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeAnalysis_Shell_HeaderFile
#define _ShapeAnalysis_Shell_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Shape;
class TopoDS_Compound;


//! This class provides operators to analyze edges orientation
//! in the shell.
class ShapeAnalysis_Shell 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_Shell();
  
  //! Clears data about loaded shells and performed checks
  Standard_EXPORT void Clear();
  
  //! Adds shells contained in the <shape> to the list of loaded shells
  Standard_EXPORT void LoadShells (const TopoDS_Shape& shape);
  
  //! Checks if shells fulfill orientation condition, i.e. if each
  //! edge is, either present once (free edge) or twice (connected
  //! edge) but with different orientations (FORWARD/REVERSED)
  //! Edges which do not fulfill these conditions are bad
  //!
  //! If <alsofree> is True free edges are considered.
  //! Free edges can be queried but are not bad
  Standard_EXPORT Standard_Boolean CheckOrientedShells (const TopoDS_Shape& shape, const Standard_Boolean alsofree = Standard_False, const Standard_Boolean checkinternaledges = Standard_False);
  
  //! Tells if a shape is loaded (only shells are checked)
  Standard_EXPORT Standard_Boolean IsLoaded (const TopoDS_Shape& shape) const;
  
  //! Returns the actual number of loaded shapes (i.e. shells)
  Standard_EXPORT Standard_Integer NbLoaded() const;
  
  //! Returns a loaded shape specified by its rank number.
  //! Returns null shape if <num> is out of range
  Standard_EXPORT TopoDS_Shape Loaded (const Standard_Integer num) const;
  
  //! Tells if at least one edge is recorded as bad
  Standard_EXPORT Standard_Boolean HasBadEdges() const;
  
  //! Returns the list of bad edges as a Compound
  //! It is empty (not null) if no edge are recorded as bad
  Standard_EXPORT TopoDS_Compound BadEdges() const;
  
  //! Tells if at least one edge is recorded as free (not connected)
  Standard_EXPORT Standard_Boolean HasFreeEdges() const;
  
  //! Returns the list of free (not connected) edges as a Compound
  //! It is empty (not null) if no edge are recorded as free
  Standard_EXPORT TopoDS_Compound FreeEdges() const;
  
  //! Tells if at least one edge is connected (shared twice or more)
  Standard_EXPORT Standard_Boolean HasConnectedEdges() const;




protected:





private:



  TopTools_IndexedMapOfShape myShells;
  TopTools_IndexedMapOfShape myBad;
  TopTools_IndexedMapOfShape myFree;
  Standard_Boolean myConex;


};







#endif // _ShapeAnalysis_Shell_HeaderFile
