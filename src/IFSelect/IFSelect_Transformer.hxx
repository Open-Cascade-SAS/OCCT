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

#ifndef _IFSelect_Transformer_HeaderFile
#define _IFSelect_Transformer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class Interface_Graph;
class Interface_Protocol;
class Interface_CheckIterator;
class Interface_InterfaceModel;
class TCollection_AsciiString;

class IFSelect_Transformer;
DEFINE_STANDARD_HANDLE(IFSelect_Transformer, Standard_Transient)

//! A Transformer defines the way an InterfaceModel is transformed
//! (without sending it to a file).
//! In order to work, each type of Transformer defines it method
//! Perform, it can be parametred as needed.
//!
//! It receives a Model (the data set) as input. It then can :
//! - edit this Model on the spot
//!   (i.e. alter its content: by editing entities, or adding/replacing some ...)
//! - produce a copied Model, which detains the needed changes
//!   (typically on the same type, but some or all entities being
//!   rebuilt or converted; or converted from a protocol to another one)
class IFSelect_Transformer : public Standard_Transient
{

public:

  //! Performs a Transformation (defined by each sub-class) :
  //! <G> gives the input data (especially the starting model) and
  //! can be used for queries (by Selections, etc...)
  //! <protocol> allows to work with General Services as necessary
  //! (it applies to input data)
  //! If the change corresponds to a conversion to a new protocol,
  //! see also the method ChangeProtocol
  //! <checks> stores produced checks messages if any
  //! <newmod> gives the result of the transformation :
  //! - if it is Null (i.e. has not been affected), the transformation
  //! has been made on the spot, it is assumed to cause no change
  //! to the graph of dependances
  //! - if it equates the starting Model, it has been transformed on
  //! the spot (possibiliy some entities were replaced inside it)
  //! - if it is new, it corresponds to a new data set which replaces
  //! the starting one
  //!
  //! <me> is mutable to allow results for ChangeProtocol to be
  //! memorized if needed, and to store information useful for
  //! the method Updated
  //!
  //! Returns True if Done, False if an Error occurred:
  //! in this case, if a new data set has been produced, the transformation is ignored,
  //! else data may be corrupted.
  Standard_EXPORT virtual Standard_Boolean Perform (const Interface_Graph& G, const Handle(Interface_Protocol)& protocol, Interface_CheckIterator& checks, Handle(Interface_InterfaceModel)& newmod) = 0;
  
  //! This methods allows to declare that the Protocol applied to
  //! the new Model has changed. It applies to the last call to
  //! Perform.
  //!
  //! Returns True if the Protocol has changed, False else.
  //! The provided default keeps the starting Protocol. This method
  //! should be redefined as required by the effect of Perform.
  Standard_EXPORT virtual Standard_Boolean ChangeProtocol (Handle(Interface_Protocol)& newproto) const;
  
  //! This method allows to know what happened to a starting
  //! entity after the last Perform. If <entfrom> (from starting
  //! model) has one and only one known item which corresponds in
  //! the new produced model, this method must return True and
  //! fill the argument <entto>. Else, it returns False.
  Standard_EXPORT virtual Standard_Boolean Updated (const Handle(Standard_Transient)& entfrom, Handle(Standard_Transient)& entto) const = 0;
  
  //! Returns a text which defines the way a Transformer works
  //! (to identify the transformation it performs)
  Standard_EXPORT virtual TCollection_AsciiString Label() const = 0;

  DEFINE_STANDARD_RTTIEXT(IFSelect_Transformer,Standard_Transient)


};

#endif // _IFSelect_Transformer_HeaderFile
