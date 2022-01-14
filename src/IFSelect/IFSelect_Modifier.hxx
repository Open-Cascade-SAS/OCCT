// Created on: 1993-08-26
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

#ifndef _IFSelect_Modifier_HeaderFile
#define _IFSelect_Modifier_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_GeneralModifier.hxx>
class IFSelect_ContextModif;
class Interface_InterfaceModel;
class Interface_Protocol;
class Interface_CopyTool;


class IFSelect_Modifier;
DEFINE_STANDARD_HANDLE(IFSelect_Modifier, IFSelect_GeneralModifier)

//! This class gives a frame for Actions which can work globally
//! on a File once completely defined (i.e. afterwards)
//!
//! Remark : if no Selection is set as criterium, the Modifier is
//! set to work and should consider all the content of the Model
//! produced.
class IFSelect_Modifier : public IFSelect_GeneralModifier
{

public:

  
  //! This deferred method defines the action specific to each class
  //! of Modifier. It is called by a ModelCopier, once the Model
  //! generated and filled. ModelCopier has already checked the
  //! criteria (Dispatch, Model Rank, Selection) before calling it.
  //!
  //! <ctx> detains information about original data and selection.
  //! The result of copying, on which modifications are to be done,
  //! is <target>.
  //! <TC> allows to run additional copies as required
  //!
  //! In case of Error, use methods CCheck from the ContextModif
  //! to aknowledge an entity Check or a Global Check with messages
  Standard_EXPORT virtual void Perform (IFSelect_ContextModif& ctx, const Handle(Interface_InterfaceModel)& target, const Handle(Interface_Protocol)& protocol, Interface_CopyTool& TC) const = 0;




  DEFINE_STANDARD_RTTIEXT(IFSelect_Modifier,IFSelect_GeneralModifier)

protected:

  
  //! Calls inherited Initialize, transmits to it the information
  //! <maychangegraph>
  Standard_EXPORT IFSelect_Modifier(const Standard_Boolean maychangegraph);



private:




};







#endif // _IFSelect_Modifier_HeaderFile
