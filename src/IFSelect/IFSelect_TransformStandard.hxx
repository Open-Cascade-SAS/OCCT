// Created on: 1994-05-27
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

#ifndef _IFSelect_TransformStandard_HeaderFile
#define _IFSelect_TransformStandard_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <IFSelect_SequenceOfGeneralModifier.hxx>
#include <IFSelect_Transformer.hxx>
#include <Standard_Integer.hxx>
class IFSelect_Selection;
class Interface_CopyControl;
class IFSelect_Modifier;
class Interface_Graph;
class Interface_Protocol;
class Interface_CheckIterator;
class Interface_InterfaceModel;
class Interface_CopyTool;
class Standard_Transient;
class TCollection_AsciiString;


class IFSelect_TransformStandard;
DEFINE_STANDARD_HANDLE(IFSelect_TransformStandard, IFSelect_Transformer)

//! This class runs transformations made by Modifiers, as
//! the ModelCopier does when it produces files (the same set
//! of Modifiers can then be used, as to transform the starting
//! Model, as at file sending time).
//!
//! First, considering the resulting model, two options :
//! - modifications are made directly on the starting model
//! (OnTheSpot option), or
//! - data are copied by the standard service Copy, only the
//! remaining (not yet sent in a file) entities are copied
//! (StandardCopy option)
//!
//! If a Selection is set, it forces the list of Entities on which
//! the Modifiers are applied. Else, each Modifier is considered
//! its Selection. By default, it is for the whole Model
//!
//! Then, the Modifiers are sequentially applied
//! If at least one Modifier "May Change Graph", or if the option
//! StandardCopy is selected, the graph will be recomputed
//! (by the WorkSession, see method RunTransformer)
//!
//! Remark that a TransformStandard with option StandardCopy
//! and no Modifier at all has the effect of computing the
//! remaining data (those not yet sent in any output file).
//! Moreover, the Protocol is not changed
class IFSelect_TransformStandard : public IFSelect_Transformer
{

public:

  
  //! Creates a TransformStandard, option StandardCopy, no Modifier
  Standard_EXPORT IFSelect_TransformStandard();
  
  //! Sets the Copy option to a new value :
  //! - True for StandardCopy  - False for OnTheSpot
  Standard_EXPORT void SetCopyOption (const Standard_Boolean option);
  
  //! Returns the Copy option
  Standard_EXPORT Standard_Boolean CopyOption() const;
  
  //! Sets a Selection (or unsets if Null)
  //! This Selection then defines the list of entities on which the
  //! Modifiers will be applied
  //! If it is set, it has priority on Selections of Modifiers
  //! Else, for each Modifier its Selection is evaluated
  //! By default, all the Model is taken
  Standard_EXPORT void SetSelection (const Handle(IFSelect_Selection)& sel);
  
  //! Returns the Selection, Null by default
  Standard_EXPORT Handle(IFSelect_Selection) Selection() const;
  
  //! Returns the count of recorded Modifiers
  Standard_EXPORT Standard_Integer NbModifiers() const;
  
  //! Returns a Modifier given its rank in the list
  Standard_EXPORT Handle(IFSelect_Modifier) Modifier (const Standard_Integer num) const;
  
  //! Returns the rank of a Modifier in the list, 0 if unknown
  Standard_EXPORT Standard_Integer ModifierRank (const Handle(IFSelect_Modifier)& modif) const;
  
  //! Adds a Modifier to the list :
  //! - <atnum> = 0 (default) : at the end of the list
  //! - <atnum> > 0 : at rank <atnum>
  //! Returns True if done, False if <atnum> is out of range
  Standard_EXPORT Standard_Boolean AddModifier (const Handle(IFSelect_Modifier)& modif, const Standard_Integer atnum = 0);
  
  //! Removes a Modifier from the list
  //! Returns True if done, False if <modif> not in the list
  Standard_EXPORT Standard_Boolean RemoveModifier (const Handle(IFSelect_Modifier)& modif);
  
  //! Removes a Modifier from the list, given its rank
  //! Returns True if done, False if <num> is out of range
  Standard_EXPORT Standard_Boolean RemoveModifier (const Standard_Integer num);
  
  //! Performs the Standard Transformation, by calling Copy then
  //! ApplyModifiers (which can return an error status)
  Standard_EXPORT Standard_Boolean Perform (const Interface_Graph& G, const Handle(Interface_Protocol)& protocol, Interface_CheckIterator& checks, Handle(Interface_InterfaceModel)& newmod) Standard_OVERRIDE;
  
  //! This the first operation. It calls StandardCopy or OnTheSpot
  //! according the option
  Standard_EXPORT void Copy (const Interface_Graph& G, Interface_CopyTool& TC, Handle(Interface_InterfaceModel)& newmod) const;
  
  //! This is the standard action of Copy : its takes into account
  //! only the remaining entities (noted by Graph Status positive)
  //! and their proper dependances of course. Produces a new model.
  Standard_EXPORT void StandardCopy (const Interface_Graph& G, Interface_CopyTool& TC, Handle(Interface_InterfaceModel)& newmod) const;
  
  //! This is the OnTheSpot action : each entity is bound with ...
  //! itself. The produced model is the same as the starting one.
  Standard_EXPORT void OnTheSpot (const Interface_Graph& G, Interface_CopyTool& TC, Handle(Interface_InterfaceModel)& newmod) const;
  
  //! Applies the modifiers sequencially.
  //! For each one, prepares required data (if a Selection is
  //! associated as a filter).
  //! For the option OnTheSpot, it determines if the graph may be
  //! changed and updates <newmod> if required
  //! If a Modifier causes an error (check "HasFailed"),
  //! ApplyModifier stops : the following Modifiers are ignored
  Standard_EXPORT Standard_Boolean ApplyModifiers (const Interface_Graph& G, const Handle(Interface_Protocol)& protocol, Interface_CopyTool& TC, Interface_CheckIterator& checks, Handle(Interface_InterfaceModel)& newmod) const;
  
  //! This methods allows to know what happened to a starting
  //! entity after the last Perform. It reads result from the map
  //! which was filled by Perform.
  Standard_EXPORT Standard_Boolean Updated (const Handle(Standard_Transient)& entfrom, Handle(Standard_Transient)& entto) const Standard_OVERRIDE;
  
  //! Returns a text which defines the way a Transformer works :
  //! "On the spot edition" or "Standard Copy" followed by
  //! "<nn> Modifiers"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_TransformStandard,IFSelect_Transformer)

protected:




private:


  Standard_Boolean thecopy;
  Handle(IFSelect_Selection) thesel;
  IFSelect_SequenceOfGeneralModifier themodifs;
  Handle(Interface_CopyControl) themap;


};







#endif // _IFSelect_TransformStandard_HeaderFile
