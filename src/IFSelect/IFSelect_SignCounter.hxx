// Created on: 1994-11-07
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

#ifndef _IFSelect_SignCounter_HeaderFile
#define _IFSelect_SignCounter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <Standard_Integer.hxx>
#include <IFSelect_SignatureList.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Standard_CString.hxx>
class IFSelect_Signature;
class IFSelect_Selection;
class Standard_Transient;
class Interface_InterfaceModel;
class Interface_Graph;
class TCollection_HAsciiString;

class IFSelect_SignCounter;
DEFINE_STANDARD_HANDLE(IFSelect_SignCounter, IFSelect_SignatureList)

//! SignCounter gives the frame to count signatures associated
//! with entities, deducted from them. Ex.: their Dynamic Type.
//!
//! It can sort a set of Entities according a signature, i.e. :
//! - list of different values found for this Signature
//! - for each one, count and list of entities
//! Results are returned as a SignatureList, which can be queried
//! on the count (list of strings, count per signature, or list of
//! entities per signature)
//!
//! A SignCounter can be filled, either directly from lists, or
//! from the result of a Selection : hence, its content can be
//! automatically recomputed as desired
//!
//! SignCounter works by using a Signature in its method AddSign
//!
//! Methods can be redefined to, either
//! - directly compute the value without a Signature
//! - compute the value in the context of a Graph
class IFSelect_SignCounter : public IFSelect_SignatureList
{

public:

  //! Creates a SignCounter, without proper Signature
  //! If <withmap> is True (default), added entities are counted
  //! only if they are not yet recorded in the map
  //! Map control can be set off if the input guarantees uniqueness of data
  //! <withlist> is transmitted to SignatureList (option to list
  //! entities, not only to count them).
  Standard_EXPORT IFSelect_SignCounter(const Standard_Boolean withmap = Standard_True, const Standard_Boolean withlist = Standard_False);

  //! Creates a SignCounter, with a predefined Signature
  //! Other arguments as for Create without Signature.
  Standard_EXPORT IFSelect_SignCounter(const Handle(IFSelect_Signature)& matcher, const Standard_Boolean withmap = Standard_True, const Standard_Boolean withlist = Standard_False);
  
  //! Returns the Signature used to count entities. It can be null.
  Standard_EXPORT Handle(IFSelect_Signature) Signature() const;
  
  //! Changes the control status. The map is not cleared, simply
  //! its use changes
  Standard_EXPORT void SetMap (const Standard_Boolean withmap);
  
  //! Adds an entity by considering its signature, which is given by
  //! call to method AddSign
  //! Returns True if added, False if already in the map (and
  //! map control status set)
  Standard_EXPORT virtual Standard_Boolean AddEntity (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model);
  
  //! Adds an entity (already filtered by Map) with its signature.
  //! This signature can be computed with the containing model.
  //! Its value is provided by the object Signature given at start,
  //! if no Signature is defined, it does nothing.
  //!
  //! Can be redefined (in this case, see also Sign)
  Standard_EXPORT virtual void AddSign (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model);
  
  //! Adds a list of entities by adding each of the items
  Standard_EXPORT void AddList (const Handle(TColStd_HSequenceOfTransient)& list, const Handle(Interface_InterfaceModel)& model);
  
  //! Adds a list of entities in the context given by the graph
  //! Default just call basic AddList
  //! Can be redefined to get a signature computed with the graph
  Standard_EXPORT virtual void AddWithGraph (const Handle(TColStd_HSequenceOfTransient)& list, const Interface_Graph& graph);
  
  //! Adds all the entities contained in a Model
  Standard_EXPORT void AddModel (const Handle(Interface_InterfaceModel)& model);
  
  //! Adds the result determined by a Selection from a Graph
  //! Remark : does not impact at all data from SetSelection & Co
  Standard_EXPORT void AddFromSelection (const Handle(IFSelect_Selection)& sel, const Interface_Graph& G);

  //! Sets a Selection as input : this causes content to be cleared
  //! then the Selection to be ready to compute (but not immediately)
  Standard_EXPORT void SetSelection (const Handle(IFSelect_Selection)& sel);

  //! Returns the selection, or a null Handle
  Standard_EXPORT Handle(IFSelect_Selection) Selection() const;
  
  //! Changes the mode of working with the selection :
  //! -1 just clears optimisation data and nothing else
  //! 0 clears it   1 inhibits it for computing (but no clearing)
  //! 2 sets it active for computing
  //! Default at creation is 0, after SetSelection (not null) is 2
  Standard_EXPORT void SetSelMode (const Standard_Integer selmode);
  
  //! Returns the mode of working with the selection
  Standard_EXPORT Standard_Integer SelMode() const;
  
  //! Computes from the selection result, if selection is active
  //! (mode 2). If selection is not defined (mode 0) or is inhibited
  //! (mode 1) does nothing.
  //! Returns True if computation is done (or optimised), False else
  //! This method is called by ComputeCounter from WorkSession
  //!
  //! If <forced> is True, recomputes systematically
  //! Else (D), if the counter was not cleared and if the former
  //! computed result started from the same total size of Graph and
  //! same count of selected entities : computation is not redone
  //! unless <forced> is given as True
  Standard_EXPORT Standard_Boolean ComputeSelected (const Interface_Graph& G, const Standard_Boolean forced = Standard_False);
  
  //! Determines and returns the value of the signature for an
  //! entity as an HAsciiString. This method works exactly as
  //! AddSign, which is optimized
  //!
  //! Can be redefined, accorded with AddSign
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) Sign (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const;
  
  //! Applies AddWithGraph on one entity, and returns the Signature
  //! Value which has been recorded
  //! To do this, Add is called with SignOnly Mode True during the
  //! call, the returned value is LastValue
  Standard_EXPORT Standard_CString ComputedSign (const Handle(Standard_Transient)& ent, const Interface_Graph& G);

  DEFINE_STANDARD_RTTIEXT(IFSelect_SignCounter,IFSelect_SignatureList)

private:

  Standard_Boolean themapstat;
  TColStd_MapOfTransient themap;
  Handle(IFSelect_Signature) thematcher;
  Handle(IFSelect_Selection) theselect;
  Standard_Integer theselmode;
  Standard_Integer thenbcomp1;
  Standard_Integer thenbcomp2;

};

#endif // _IFSelect_SignCounter_HeaderFile
