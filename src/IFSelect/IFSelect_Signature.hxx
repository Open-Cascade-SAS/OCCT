// Created on: 1994-04-21
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

#ifndef _IFSelect_Signature_HeaderFile
#define _IFSelect_Signature_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <Interface_SignType.hxx>
class Standard_Transient;
class Interface_InterfaceModel;


class IFSelect_Signature;
DEFINE_STANDARD_HANDLE(IFSelect_Signature, Interface_SignType)

//! Signature provides the basic service used by the classes
//! SelectSignature and Counter (i.e. Name, Value), which is :
//! - for an entity in a model, give a characteristic string, its
//! signature
//! This string has not to be unique in the model, but gives a
//! value for such or such important feature.
//! Examples : Dynamic Type; Category; etc
class IFSelect_Signature : public Interface_SignType
{

public:

  
  //! Sets the information data to tell "integer cases" with
  //! possible min and max values
  //! To be called when creating
  Standard_EXPORT void SetIntCase (const Standard_Boolean hasmin, const Standard_Integer valmin, const Standard_Boolean hasmax, const Standard_Integer valmax);
  
  //! Tells if this Signature gives integer values
  //! and returns values from SetIntCase if True
  Standard_EXPORT Standard_Boolean IsIntCase (Standard_Boolean& hasmin, Standard_Integer& valmin, Standard_Boolean& hasmax, Standard_Integer& valmax) const;
  
  //! Adds a possible case
  //! To be called when creating, IF the list of possible cases for
  //! Value is known when starting
  //! For instance, for CDL types, rather do not fill this,
  //! but for a specific enumeration (such as a status), can be used
  Standard_EXPORT void AddCase (const Standard_CString acase);
  
  //! Returns the predefined list of possible cases, filled by AddCase
  //! Null Handle if no predefined list (hence, to be counted)
  //! Useful to filter on  really possible vase, for instance, or
  //! for a help
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) CaseList() const;
  
  //! Returns an identification of the Signature (a word), given at
  //! initialization time
  //! Returns the Signature for a Transient object. It is specific
  //! of each sub-class of Signature. For a Null Handle, it should
  //! provide ""
  //! It can work with the model which contains the entity
  Standard_EXPORT Standard_CString Name() const Standard_OVERRIDE;
  
  //! The label of a Signature uses its name as follow :
  //! "Signature : <name>"
  Standard_EXPORT TCollection_AsciiString Label() const;
  
  //! Tells if the value for <ent> in <model> matches a text, with
  //! a criterium <exact>.
  //! The default definition calls MatchValue
  //! Can be redefined
  Standard_EXPORT virtual Standard_Boolean Matches (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model, const TCollection_AsciiString& text, const Standard_Boolean exact) const;
  
  //! Default procedure to tell if a value <val> matches a text
  //! with a criterium <exact>. <exact> = True requires equality,
  //! else only contained (no reg-exp)
  Standard_EXPORT static Standard_Boolean MatchValue (const Standard_CString val, const TCollection_AsciiString& text, const Standard_Boolean exact);
  
  //! This procedure converts an Integer to a CString
  //! It is a convenient way when the value of a signature has the
  //! form of a simple integer value
  //! The value is to be used immediately (one buffer only, no copy)
  Standard_EXPORT static Standard_CString IntValue (const Standard_Integer val);




  DEFINE_STANDARD_RTTIEXT(IFSelect_Signature,Interface_SignType)

protected:

  
  //! Initializes a Signature with its name
  Standard_EXPORT IFSelect_Signature(const Standard_CString name);

  TCollection_AsciiString thename;


private:


  Standard_Integer thecasi[3];
  Handle(TColStd_HSequenceOfAsciiString) thecasl;


};







#endif // _IFSelect_Signature_HeaderFile
