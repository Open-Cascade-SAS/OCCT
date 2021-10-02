// Created on: 1998-07-02
// Created by: Joelle CHAUVET
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

#ifndef _BRepFill_CompatibleWires_HeaderFile
#define _BRepFill_CompatibleWires_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_SequenceOfShape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Edge;


//! Constructs a sequence of Wires (with good orientation
//! and origin) agreed each other so that the surface passing
//! through these sections is not twisted
class BRepFill_CompatibleWires 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_CompatibleWires();
  
  Standard_EXPORT BRepFill_CompatibleWires(const TopTools_SequenceOfShape& Sections);
  
  Standard_EXPORT void Init (const TopTools_SequenceOfShape& Sections);
  
  Standard_EXPORT void SetPercent (const Standard_Real percent = 0.01);
  
  //! Performs  CompatibleWires According  to  the orientation
  //! and the origin of  each other
  Standard_EXPORT void Perform (const Standard_Boolean WithRotation = Standard_True);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! returns the generated sequence.
  Standard_EXPORT const TopTools_SequenceOfShape& Shape() const;
  
  //! Returns   the  shapes  created  from   a  subshape
  //! <SubSection> of a section.
  Standard_EXPORT const TopTools_ListOfShape& GeneratedShapes (const TopoDS_Edge& SubSection) const;
  
  Standard_EXPORT const TopTools_DataMapOfShapeListOfShape& Generated() const;

  Standard_EXPORT Standard_Boolean IsDegeneratedFirstSection() const;

  Standard_EXPORT Standard_Boolean IsDegeneratedLastSection() const;



protected:





private:

  
  //! Insert cutting  points  on  closed wires to  have same
  //! number of edges. The sequence of shapes must
  //! be a sequence of wires.
  Standard_EXPORT void SameNumberByPolarMethod (const Standard_Boolean WithRotation = Standard_True);
  
  //! Insert cutting  points  on  open wires to  have same
  //! number of edges. The sequence of shapes must
  //! be a sequence of wires.
  Standard_EXPORT void SameNumberByACR (const Standard_Boolean report);
  
  //! Computes  origins and orientation  on closed wires to
  //! avoid twisted results. The sequence of shapes must
  //! be a sequence of wires. <polar> must be true
  //! if SameNumberByPolarMethod was used before.
  Standard_EXPORT void ComputeOrigin (const Standard_Boolean polar);
  
  //! Computes  origins and orientation  on open wires to
  //! avoid twisted results. The sequence of shapes must
  //! be a sequence of wires.
  Standard_EXPORT void SearchOrigin();


  TopTools_SequenceOfShape myInit;
  TopTools_SequenceOfShape myWork;
  Standard_Real myPercent;
  Standard_Boolean myDegen1;
  Standard_Boolean myDegen2;
  Standard_Boolean myIsDone;
  TopTools_DataMapOfShapeListOfShape myMap;


};







#endif // _BRepFill_CompatibleWires_HeaderFile
