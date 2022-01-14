// Created on: 1994-12-21
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

#ifndef _IFSelect_DispPerSignature_HeaderFile
#define _IFSelect_DispPerSignature_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Dispatch.hxx>
#include <Standard_Integer.hxx>
class IFSelect_SignCounter;
class TCollection_AsciiString;
class Interface_Graph;
class IFGraph_SubPartsIterator;


class IFSelect_DispPerSignature;
DEFINE_STANDARD_HANDLE(IFSelect_DispPerSignature, IFSelect_Dispatch)

//! A DispPerSignature sorts input Entities according to a
//! Signature : it works with a SignCounter to do this.
class IFSelect_DispPerSignature : public IFSelect_Dispatch
{

public:

  
  //! Creates a DispPerSignature with no SignCounter (by default,
  //! produces only one packet)
  Standard_EXPORT IFSelect_DispPerSignature();
  
  //! Returns the SignCounter used for splitting
  Standard_EXPORT Handle(IFSelect_SignCounter) SignCounter() const;
  
  //! Sets a SignCounter for sort
  //! Remark : it is set to record lists of entities, not only counts
  Standard_EXPORT void SetSignCounter (const Handle(IFSelect_SignCounter)& sign);
  
  //! Returns the name of the SignCounter, which caracterises the
  //! sorting criterium for this Dispatch
  Standard_EXPORT Standard_CString SignName() const;
  
  //! Returns as Label, "One File per Signature <name>"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;
  
  //! Returns True, maximum count is given as <nbent>
  Standard_EXPORT virtual Standard_Boolean LimitedMax (const Standard_Integer nbent, Standard_Integer& max) const Standard_OVERRIDE;
  
  //! Computes the list of produced Packets. It defines Packets from
  //! the SignCounter, which sirts the input Entities per Signature
  //! (specific of the SignCounter).
  Standard_EXPORT void Packets (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_DispPerSignature,IFSelect_Dispatch)

protected:




private:


  Handle(IFSelect_SignCounter) thesign;


};







#endif // _IFSelect_DispPerSignature_HeaderFile
