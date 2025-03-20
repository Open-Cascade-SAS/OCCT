// Created on: 1995-11-08
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

#ifndef _Interface_Category_HeaderFile
#define _Interface_Category_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Integer.hxx>

#include <Interface_GTool.hxx>
class Interface_Protocol;
class Standard_Transient;
class Interface_ShareTool;
class Interface_InterfaceModel;

//! This class manages categories
//! A category is defined by a name and a number, and can be
//! seen as a way of rough classification, i.e. less precise than
//! a cdl type.
//! Hence, it is possible to dispatch every entity in about
//! a dozen of categories, twenty is a reasonable maximum.
//!
//! Basically, the system provides the following categories :
//! Shape (Geometry, BRep, CSG, Features, etc...)
//! Drawing (Drawing, Views, Annotations, Pictures, Sketches ...)
//! Structure (Component & Part, Groups & Patterns ...)
//! Description (Meta-Data : Relations, Properties, Product ...)
//! Auxiliary   (those which do not enter in the above list)
//! and some dedicated categories
//! FEA , Kinematics , Piping , etc...
//! plus Professional  for other dedicated non-classed categories
//!
//! In addition, this class provides a way to compute then quickly
//! query category numbers for an entire model.
//! Values are just recorded as a list of numbers, control must
//! then be done in a wider context (which must provide a Graph)
class Interface_Category
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a Category, with no protocol yet
  Interface_Category()
      : myGTool(new Interface_GTool)
  {
    Init();
  }

  //! Creates a Category with a given protocol
  Interface_Category(const Handle(Interface_Protocol)& theProtocol)
      : myGTool(new Interface_GTool(theProtocol))
  {
    Init();
  }

  //! Creates a Category with a given GTool
  Interface_Category(const Handle(Interface_GTool)& theGTool)
      : myGTool(theGTool)
  {
    Init();
  }

  //! Sets/Changes Protocol
  void SetProtocol(const Handle(Interface_Protocol)& theProtocol)
  {
    myGTool->SetProtocol(theProtocol);
  }

  //! Determines the Category Number for an entity in its context,
  //! by using general service CategoryNumber
  Standard_EXPORT Standard_Integer CatNum(const Handle(Standard_Transient)& theEnt,
                                          const Interface_ShareTool&        theShares);

  //! Clears the recorded list of category numbers for a Model
  void ClearNums() { myNum.Nullify(); }

  //! Computes the Category Number for each entity and records it,
  //! in an array (ent.number -> category number)
  //! Hence, it can be queried by the method Num.
  //! The Model itself is not recorded, this method is intended to
  //! be used in a wider context (which detains also a Graph, etc)
  Standard_EXPORT void Compute(const Handle(Interface_InterfaceModel)& theModel,
                               const Interface_ShareTool&              theShares);

  //! Returns the category number recorded for an entity number
  //! Returns 0 if out of range
  Standard_EXPORT Standard_Integer Num(const Standard_Integer theNumEnt) const;

  //! Records a new Category defined by its names, produces a number
  //! New if not yet recorded
  Standard_EXPORT static Standard_Integer AddCategory(const Standard_CString theName);

  //! Returns the count of recorded categories
  Standard_EXPORT static Standard_Integer NbCategories();

  //! Returns the name of a category, according to its number
  Standard_EXPORT static Standard_CString Name(const Standard_Integer theNum);

  //! Returns the number of a category, according to its name
  Standard_EXPORT static Standard_Integer Number(const Standard_CString theName);

  //! Default initialisation
  //! (protected against several calls : passes only once)
  Standard_EXPORT static void Init();

private:
  Handle(Interface_GTool)          myGTool;
  Handle(TColStd_HArray1OfInteger) myNum;
};

#endif // _Interface_Category_HeaderFile
