// Created on: 1996-09-25
// Created by: Christian CAILLET
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

#ifndef _IFSelect_SelectSignedShared_HeaderFile
#define _IFSelect_SelectSignedShared_HeaderFile

#include <Standard.hxx>

#include <TCollection_AsciiString.hxx>
#include <IFSelect_SelectExplore.hxx>
#include <Standard_Integer.hxx>
class IFSelect_Signature;
class Standard_Transient;
class Interface_Graph;
class Interface_EntityIterator;


class IFSelect_SelectSignedShared;
DEFINE_STANDARD_HANDLE(IFSelect_SelectSignedShared, IFSelect_SelectExplore)

//! In the graph, explore the Shareds of the input entities,
//! until it encounters some which match a given Signature
//! (for a limited level, filters the returned list)
//! By default, fitted for any level
class IFSelect_SelectSignedShared : public IFSelect_SelectExplore
{

public:

  
  //! Creates a SelectSignedShared, defaulted for any level
  //! with a given Signature and text to match
  Standard_EXPORT IFSelect_SelectSignedShared(const Handle(IFSelect_Signature)& matcher, const Standard_CString signtext, const Standard_Boolean exact = Standard_True, const Standard_Integer level = 0);
  
  //! Returns the used Signature, then it is possible to access it,
  //! modify it as required
  Standard_EXPORT Handle(IFSelect_Signature) Signature() const;
  
  //! Returns Text used to Sort Entity on its Signature
  Standard_EXPORT const TCollection_AsciiString& SignatureText() const;
  
  //! Returns True if match must be exact
  Standard_EXPORT Standard_Boolean IsExact() const;
  
  //! Explores an entity : its Shared entities
  //! <ent> to take if it matches the Signature
  //! At level max, filters the result. Else gives all Shareds
  Standard_EXPORT Standard_Boolean Explore (const Standard_Integer level, const Handle(Standard_Transient)& ent, const Interface_Graph& G, Interface_EntityIterator& explored) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium.
  //! (it refers to the text and exact flag to be matched, and is
  //! qualified by the Name provided by the Signature)
  Standard_EXPORT TCollection_AsciiString ExploreLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectSignedShared,IFSelect_SelectExplore)

protected:




private:


  Handle(IFSelect_Signature) thematcher;
  TCollection_AsciiString thesigntext;
  Standard_Boolean theexact;


};







#endif // _IFSelect_SelectSignedShared_HeaderFile
