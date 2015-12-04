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

#ifndef _IFSelect_CheckCounter_HeaderFile
#define _IFSelect_CheckCounter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SignatureList.hxx>
#include <Standard_Boolean.hxx>
class MoniTool_SignText;
class Interface_CheckIterator;
class Interface_InterfaceModel;


class IFSelect_CheckCounter;
DEFINE_STANDARD_HANDLE(IFSelect_CheckCounter, IFSelect_SignatureList)

//! A CheckCounter allows to see a CheckList (i.e. CheckIterator)
//! not per entity, its messages, but per message, the entities
//! attached (count and list). Because many messages can be
//! repeated if they are due to systematic errors
class IFSelect_CheckCounter : public IFSelect_SignatureList
{

public:

  
  //! Creates a CheckCounter, empty ready to work
  Standard_EXPORT IFSelect_CheckCounter(const Standard_Boolean withlist = Standard_False);
  
  //! Sets a specific signature
  //! Else, the current SignType (in the model) is used
  Standard_EXPORT void SetSignature (const Handle(MoniTool_SignText)& sign);
  
  //! Returns the Signature;
  Standard_EXPORT Handle(MoniTool_SignText) Signature() const;
  
  //! Analyses a CheckIterator according a Model (which detains the
  //! entities for which the CheckIterator has messages), i.e.
  //! counts messages for entities
  //! If <original> is True, does not consider final messages but
  //! those before interpretation (such as inserting variables :
  //! integers, reals, strings)
  //! If <failsonly> is True, only Fails are considered
  //! Remark : global messages are recorded with a Null entity
  Standard_EXPORT void Analyse (const Interface_CheckIterator& list, const Handle(Interface_InterfaceModel)& model, const Standard_Boolean original = Standard_False, const Standard_Boolean failsonly = Standard_False);




  DEFINE_STANDARD_RTTIEXT(IFSelect_CheckCounter,IFSelect_SignatureList)

protected:




private:


  Handle(MoniTool_SignText) thesign;


};







#endif // _IFSelect_CheckCounter_HeaderFile
