// Created on: 1999-09-28
// Created by: Denis PASCAL
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TNaming_Selector_HeaderFile
#define _TNaming_Selector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_Label.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_AttributeMap.hxx>
class TDF_Label;
class TopoDS_Shape;
class TNaming_NamedShape;


//! This class provides a single API for selection of shapes.
//! This involves both identification and selection of
//! shapes in the data framework.
//! If the selected shape is modified, this selector will
//! solve its identifications.
//! This class is the user interface for topological
//! naming resources.
//! * The   <IsIdentified> method returns  (if exists)
//! the NamedShape which  contains a given shape. The
//! definition of  an  identified shape is :   a Shape
//! handled by a NamedShape  (this shape  is the only
//! one stored) , which  has the TNaming_PRImITIVE evolution
//!
//! *  The   <Select> method  returns   ALWAYS a  new
//! NamedShape at the given  label, which contains the
//! argument  selected  shape.    When  calling  this
//! method, the sub-hierarchy of <label> is first cleared,
//! then a TNaming_NamedShape   is ALWAYS created  at
//! this <label>, with the TNaming_SELECTED evolution.
//! The <Naming attribute> is associated to the selected
//! shape which store the arguments of the selection .
//! If the given selected shape was already identified
//! (method IsIdentified)   , this   Naming attribute
//! contains  the reference (Identity  code)  to the
//! argument shape.
//!
//! * The <Solve> method  update the current value of
//! the NamedShape, according to the <Naming> attribute.
//! A boolean status  is    returned to say  if  the
//! algorithm succeed   or not.  To read   the current
//! value    of the selected    Named  Shape  use the
//! TNaming_Tool::GetShape    method,    as  for  any
//! NamedShape attribute.
class TNaming_Selector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! To know if a shape is already identified (not selected)
  //! =======================================================
  //!
  //! The label access defines the point of access to the data framework.
  //! selection is the shape for which we want to know
  //! whether it is identified or not.
  //! If true, NS is returned as the identity of selection.
  //! If Geometry is true, NS will be the named shape
  //! containing the first appearance of selection and
  //! not any other shape. In other words, selection
  //! must be the only shape stored in NS.
  Standard_EXPORT static Standard_Boolean IsIdentified (const TDF_Label& access, const TopoDS_Shape& selection, Handle(TNaming_NamedShape)& NS, const Standard_Boolean Geometry = Standard_False);
  
  //! Create a selector on this label
  //! to select a shape.
  //! ==================
  Standard_EXPORT TNaming_Selector(const TDF_Label& aLabel);
  

  //! Creates a topological naming on the label
  //! aLabel given as an argument at construction time.
  //! If successful, the shape Selection - found in the
  //! shape Context - is now identified in the named
  //! shape returned in NamedShape.
  //! If Geometry is true, NamedShape contains the
  //! first appearance of Selection.
  //! This syntax is more robust than the previous
  //! syntax for this method.
  Standard_EXPORT Standard_Boolean Select (const TopoDS_Shape& Selection, const TopoDS_Shape& Context, const Standard_Boolean Geometry = Standard_False, const Standard_Boolean KeepOrientatation = Standard_False) const;
  

  //! Creates a topological naming on the label
  //! aLabel given as an argument at construction time.
  //! If successful, the shape Selection is now
  //! identified in the named shape returned in NamedShape.
  //! If Geometry is true, NamedShape contains the
  //! first appearance of Selection.
  Standard_EXPORT Standard_Boolean Select (const TopoDS_Shape& Selection, const Standard_Boolean Geometry = Standard_False, const Standard_Boolean KeepOrientatation = Standard_False) const;
  

  //! Updates the topological naming on the label
  //! aLabel given as an argument at construction time.
  //! The underlying shape returned in the method
  //! NamedShape is updated.
  //! To read this shape, use the method TNaming_Tool::GetShape
  Standard_EXPORT Standard_Boolean Solve (TDF_LabelMap& Valid) const;
  
  //! Returns the attribute list args.
  //! This list contains the named shape on which the topological naming was built.
  Standard_EXPORT void Arguments (TDF_AttributeMap& args) const;
  
  //! Returns the NamedShape build or under construction,
  //! which contains the topological naming..
  Standard_EXPORT Handle(TNaming_NamedShape) NamedShape() const;




protected:





private:



  TDF_Label myLabel;


};







#endif // _TNaming_Selector_HeaderFile
