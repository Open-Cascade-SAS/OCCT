// Created on: 1998-01-28
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IFSelect_SignMultiple_HeaderFile
#define _IFSelect_SignMultiple_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_SequenceOfTransient.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class IFSelect_SignMultiple;
DEFINE_STANDARD_HANDLE(IFSelect_SignMultiple, IFSelect_Signature)

//! Multiple Signature : ordered list of other Signatures
//! It concatenates on a same line the result of its sub-items
//! separated by sets of 3 blanks
//! It is possible to define tabulations between sub-items
//! Moreover, match rules are specific
class IFSelect_SignMultiple : public IFSelect_Signature
{

public:

  
  //! Creates an empty SignMultiple with a Name
  //! This name should take expected tabulations into account
  Standard_EXPORT IFSelect_SignMultiple(const Standard_CString name);
  
  //! Adds a Signature. Width, if given, gives the tabulation
  //! If <maxi> is True, it is a forced tabulation (overlength is
  //! replaced by a final dot)
  //! If <maxi> is False, just 3 blanks follow an overlength
  Standard_EXPORT void Add (const Handle(IFSelect_Signature)& subsign, const Standard_Integer width = 0, const Standard_Boolean maxi = Standard_False);
  
  //! Concatenates the values of sub-signatures, with their
  //! tabulations
  Standard_EXPORT Standard_CString Value (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Specialized Match Rule
  //! If <exact> is False, simply checks if at least one sub-item
  //! matches
  //! If <exact> is True, standard match with Value
  //! (i.e. tabulations must be respected)
  Standard_EXPORT virtual Standard_Boolean Matches (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model, const TCollection_AsciiString& text, const Standard_Boolean exact) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SignMultiple,IFSelect_Signature)

protected:




private:


  TColStd_SequenceOfTransient thesubs;
  TColStd_SequenceOfInteger thetabs;


};







#endif // _IFSelect_SignMultiple_HeaderFile
