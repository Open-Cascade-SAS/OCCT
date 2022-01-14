// Created on: 1993-10-19
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IFSelect_GeneralModifier_HeaderFile
#define _IFSelect_GeneralModifier_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class IFSelect_Selection;
class IFSelect_Dispatch;
class TCollection_AsciiString;

class IFSelect_GeneralModifier;
DEFINE_STANDARD_HANDLE(IFSelect_GeneralModifier, Standard_Transient)

//! This class gives a frame for Actions which modify the effect
//! of a Dispatch, i.e. :
//! By Selections and Dispatches, an original Model can be
//! split into one or more "target" Models : these Models
//! contain Entities copied from the original one (that is, a
//! part of it). Basically, these dispatched Entities are copied
//! as identical to their original counterparts. Also the copied
//! Models reproduce the Header of the original one.
//!
//! Modifiers allow to change this copied content : this is the
//! way to be used for any kind of alterations, adaptations ...
//! They are exploited by a ModelCopier, which firstly performs
//! the copy operation described by Dispatches, then invokes the
//! Modifiers to work on the result.
//!
//! Each GeneralModifier can be attached to :
//! - all the Models produced
//! - a Dispatch (it will be applied to all the Models obtained
//! from this Dispatch) designated by its Ident in a ShareOut
//! - in addition, to a Selection (facultative) : this adds a
//! criterium, the Modifier is invoked on a produced Model only
//! if this Model contains an Entity copied from one of the
//! Entities designated by this Selection.
//! (for special Modifiers from IFAdapt, while they must work on
//! definite Entities, this Selection is mandatory to run)
//!
//! Remark : this class has no action attached, it only provides
//! a frame to work on criteria. Then, sub-classes will define
//! their kind of action, which can be applied at a precise step
//! of the production of a File : see Modifier, and in the
//! package IFAdapt, EntityModifier and EntityCopier
class IFSelect_GeneralModifier : public Standard_Transient
{

public:

  //! Returns True if this modifier may change the graph of
  //! dependences (aknowledged at creation time)
  Standard_EXPORT Standard_Boolean MayChangeGraph() const;
  
  //! Attaches to a Dispatch. If <disp> is Null, Resets it
  //! (to apply the Modifier on every Dispatch)
  Standard_EXPORT void SetDispatch (const Handle(IFSelect_Dispatch)& disp);
  
  //! Returns the Dispatch to be matched, Null if not set
  Standard_EXPORT Handle(IFSelect_Dispatch) Dispatch() const;
  
  //! Returns True if a Model obtained from the Dispatch <disp>
  //! is to be treated (apart from the Selection criterium)
  //! If Dispatch(me) is Null, returns True. Else, checks <disp>
  Standard_EXPORT Standard_Boolean Applies (const Handle(IFSelect_Dispatch)& disp) const;
  
  //! Sets a Selection : a Model is treated if it contains one or
  //! more Entities designated by the Selection
  Standard_EXPORT void SetSelection (const Handle(IFSelect_Selection)& sel);
  
  //! Resets the Selection : this criterium is not longer active
  Standard_EXPORT void ResetSelection();
  
  //! Returns True if a Selection is set as an additional criterium
  Standard_EXPORT Standard_Boolean HasSelection() const;
  
  //! Returns the Selection, or a Null Handle if not set
  Standard_EXPORT Handle(IFSelect_Selection) Selection() const;
  
  //! Returns a short text which defines the operation performed
  Standard_EXPORT virtual TCollection_AsciiString Label() const = 0;

  DEFINE_STANDARD_RTTIEXT(IFSelect_GeneralModifier,Standard_Transient)

protected:

  //! Sets the Modifier criteria to default Values
  //! (i.e. "always applies")
  //! <maychangegraph> must be provided at creation time, to :
  //! - False if this GeneralModifier surely lets the graph of
  //! dependencies unchanged (NO edition of any reference, BUT
  //! also NO entity added or replaced or removed)
  //! - True if there is a possibility of changing the graph of
  //! dependencies when this modifier is applied
  Standard_EXPORT IFSelect_GeneralModifier(const Standard_Boolean maychangegraph);

private:

  Handle(IFSelect_Selection) thesel;
  Handle(IFSelect_Dispatch) thedisp;
  Standard_Boolean thechgr;

};

#endif // _IFSelect_GeneralModifier_HeaderFile
