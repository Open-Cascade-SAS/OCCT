// Created on: 1996-12-24
// Created by: Yves FRICAUD
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

#ifndef _TNaming_Tool_HeaderFile
#define _TNaming_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_LabelMap.hxx>
#include <TNaming_MapOfNamedShape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TDF_LabelList.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
class TNaming_Localizer;
class TNaming_NamedShape;
class TNaming_OldShapeIterator;
class TopoDS_Shape;
class TDF_Label;
class TNaming_UsedShapes;


//! A tool to get information on the topology of a
//! named shape attribute.
//! This information is typically a TopoDS_Shape object.
//! Using this tool, relations between named shapes
//! are also accessible.
class TNaming_Tool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the last Modification of <NS>.
  //! Returns the shape CurrentShape contained in
  //! the named shape attribute NS.
  //! CurrentShape is the current state of the entities
  //! if they have been modified in other attributes of the same data structure.
  //! Each call to this function creates a new compound.
  Standard_EXPORT static TopoDS_Shape CurrentShape (const Handle(TNaming_NamedShape)& NS);
  
  //! Returns the shape CurrentShape contained in
  //! the named shape attribute NS, and present in
  //! the updated attribute map Updated.
  //! CurrentShape is the current state of the entities
  //! if they have been modified in other attributes of the same data structure.
  //! Each call to this function creates a new compound.
  //! Warning
  //! Only the contents of Updated are searched.R
  Standard_EXPORT static TopoDS_Shape CurrentShape (const Handle(TNaming_NamedShape)& NS, const TDF_LabelMap& Updated);
  
  //! Returns the NamedShape of the last Modification of <NS>.
  //! This shape is identified by a label.
  Standard_EXPORT static Handle(TNaming_NamedShape) CurrentNamedShape (const Handle(TNaming_NamedShape)& NS, const TDF_LabelMap& Updated);
  
  //! Returns NamedShape the last Modification of <NS>.
  Standard_EXPORT static Handle(TNaming_NamedShape) CurrentNamedShape (const Handle(TNaming_NamedShape)& NS);
  
  //! Returns the named shape attribute defined by
  //! the shape aShape and the label anAccess.
  //! This attribute is returned as a new shape.
  //! You call this function, if you need to create a
  //! topological attribute for existing data.
  //! Example
  //! class MyPkg_MyClass
  //! {
  //! public: Standard_Boolean
  //! SameEdge(const
  //! Handle(OCafTest_Line)& , const
  //! Handle(CafTest_Line)& );
  //! };
  //!
  //! Standard_Boolean
  //! MyPkg_MyClass::SameEdge
  //! (const Handle(OCafTest_Line)& L1
  //! const Handle(OCafTest_Line)& L2)
  //! { Handle(TNaming_NamedShape)
  //! NS1 = L1->NamedShape();
  //! Handle(TNaming_NamedShape)
  //! NS2 = L2->NamedShape();
  //!
  //! return
  //! BRepTools::Compare(NS1->Get(),NS2->Get());
  //! }
  //! In the example above, the function SameEdge is
  //! created to compare the edges having two lines
  //! for geometric supports. If these edges are found
  //! by BRepTools::Compare to be within the same
  //! tolerance, they are considered to be the same.
  //! Warning
  //! To avoid sharing of names, a SELECTED
  //! attribute will not be returned. Sharing of names
  //! makes it harder to manage the data structure.
  //! When the user of the name is removed, for
  //! example, it is difficult to know whether the name
  //! should be destroyed.
  Standard_EXPORT static Handle(TNaming_NamedShape) NamedShape (const TopoDS_Shape& aShape, const TDF_Label& anAcces);
  
  //! Returns the entities stored in the named shape attribute NS.
  //! If there is only one old-new pair, the new shape
  //! is returned. Otherwise, a Compound is returned.
  //! This compound is made out of all the new shapes found.
  //! Each call to this function creates a new compound.
  Standard_EXPORT static TopoDS_Shape GetShape (const Handle(TNaming_NamedShape)& NS);
  
  //! Returns the shape contained as OldShape in <NS>
  Standard_EXPORT static TopoDS_Shape OriginalShape (const Handle(TNaming_NamedShape)& NS);
  
  //! Returns the shape generated from S or by a
  //! modification of S and contained in the named
  //! shape Generation.
  Standard_EXPORT static TopoDS_Shape GeneratedShape (const TopoDS_Shape& S, const Handle(TNaming_NamedShape)& Generation);
  
  Standard_EXPORT static void Collect (const Handle(TNaming_NamedShape)& NS, TNaming_MapOfNamedShape& Labels, const Standard_Boolean OnlyModif = Standard_True);
  
  //! Returns True if <aShape> appears under a label.(DP)
  Standard_EXPORT static Standard_Boolean HasLabel (const TDF_Label& access, const TopoDS_Shape& aShape);
  
  //! Returns  the label  of   the first apparition  of
  //! <aShape>.  Transdef  is a value of the transaction
  //! of the first apparition of <aShape>.
  Standard_EXPORT static TDF_Label Label (const TDF_Label& access, const TopoDS_Shape& aShape, Standard_Integer& TransDef);
  

  //! Returns the shape created from the shape
  //! aShape contained in the attribute anAcces.
  Standard_EXPORT static TopoDS_Shape InitialShape (const TopoDS_Shape& aShape, const TDF_Label& anAcces, TDF_LabelList& Labels);
  
  //! Returns the last transaction where the creation of S
  //! is valid.
  Standard_EXPORT static Standard_Integer ValidUntil (const TDF_Label& access, const TopoDS_Shape& S);
  
  //! Returns the current shape (a Wire or a Shell) built (in the data framework)
  //! from the the shapes of the argument named shape.
  //! It is used for IDENTITY name type computation.
  Standard_EXPORT static void FindShape (const TDF_LabelMap& Valid, const TDF_LabelMap& Forbiden, const Handle(TNaming_NamedShape)& Arg, TopoDS_Shape& S);


friend class TNaming_Localizer;
friend class TNaming_NamedShape;
friend class TNaming_OldShapeIterator;


protected:





private:

  
  //! Returns True if <aShape> appears under a label.
  Standard_EXPORT static Standard_Boolean HasLabel (const Handle(TNaming_UsedShapes)& Shapes, const TopoDS_Shape& aShape);
  
  //! Returns the last transaction where the creation of S
  //! is valid.
  Standard_EXPORT static Standard_Integer ValidUntil (const TopoDS_Shape& S, const Handle(TNaming_UsedShapes)& US);
  
  //! Returns  the label  of   the first apparition  of
  //! <aShape>.  Transdef  is a value of the transaction
  //! of the first apparition of <aShape>.
  Standard_EXPORT static TDF_Label Label (const Handle(TNaming_UsedShapes)& Shapes, const TopoDS_Shape& aShape, Standard_Integer& TransDef);
  
  Standard_EXPORT static void FirstOlds (const Handle(TNaming_UsedShapes)& Shapes, const TopoDS_Shape& S, TNaming_OldShapeIterator& it, TopTools_IndexedMapOfShape& MS, TDF_LabelList& Labels);




};







#endif // _TNaming_Tool_HeaderFile
