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

#ifndef _IFSelect_SelectSignature_HeaderFile
#define _IFSelect_SelectSignature_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <IFSelect_SelectExtract.hxx>
class IFSelect_Signature;
class IFSelect_SignCounter;
class Standard_Transient;
class Interface_Graph;
class Interface_InterfaceModel;

//! A SelectSignature sorts the Entities on a Signature Matching.
//! The signature to match is given at creation time. Also, the
//! required match is given at creation time : exact (IsEqual) or
//! contains (the Type's Name must contain the criterium Text)
//!
//! Remark that no more interpretation is done, it is an
//! alphanumeric signature : for instance, DynamicType is matched
//! as such, super-types are not considered
//!
//! Also, numeric (integer) comparisons are supported : an item
//! can be <val ou <=val or >val or >=val , val being an Integer
//!
//! A SelectSignature may also be created from a SignCounter,
//! which then just gives its LastValue as SignatureValue
class IFSelect_SelectSignature : public IFSelect_SelectExtract
{

public:
  //! Creates a SelectSignature with its Signature and its Text to
  //! Match.
  //! <exact> if True requires exact match,
  //! if False requires <signtext> to be contained in the Signature
  //! of the entity (default is "exact")
  Standard_EXPORT IFSelect_SelectSignature(const occ::handle<IFSelect_Signature>& matcher,
                                           const char*                            signtext,
                                           const bool                             exact = true);

  //! As above with an AsciiString
  Standard_EXPORT IFSelect_SelectSignature(const occ::handle<IFSelect_Signature>& matcher,
                                           const TCollection_AsciiString&         signtext,
                                           const bool                             exact = true);

  //! Creates a SelectSignature with a Counter, more precisely a
  //! SelectSignature. Which is used here to just give a Signature
  //! Value (by SignOnly Mode)
  //! Matching is the default provided by the class Signature
  Standard_EXPORT IFSelect_SelectSignature(const occ::handle<IFSelect_SignCounter>& matcher,
                                           const char*                              signtext,
                                           const bool                               exact = true);

  //! Returns the used Signature, then it is possible to access it,
  //! modify it as required. Can be null, hence see Counter
  Standard_EXPORT occ::handle<IFSelect_Signature> Signature() const;

  //! Returns the used SignCounter. Can be used as alternative for
  //! Signature
  Standard_EXPORT occ::handle<IFSelect_SignCounter> Counter() const;

  //! Returns True for an Entity (model->Value(num)) of which the
  //! signature matches the text given as creation time
  //! May also work with a Counter from the Graph
  Standard_EXPORT bool SortInGraph(const int                              rank,
                                   const occ::handle<Standard_Transient>& ent,
                                   const Interface_Graph&                 G) const override;

  //! Not called, defined only to remove a deferred method here
  Standard_EXPORT bool Sort(const int                                    rank,
                            const occ::handle<Standard_Transient>&       ent,
                            const occ::handle<Interface_InterfaceModel>& model) const override;

  //! Returns Text used to Sort Entity on its Signature or SignCounter
  Standard_EXPORT const TCollection_AsciiString& SignatureText() const;

  //! Returns True if match must be exact
  Standard_EXPORT bool IsExact() const;

  //! Returns a text defining the criterium.
  //! (it refers to the text and exact flag to be matched, and is
  //! qualified by the Name provided by the Signature)
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const override;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectSignature, IFSelect_SelectExtract)

private:
  occ::handle<IFSelect_Signature>               thematcher;
  occ::handle<IFSelect_SignCounter>             thecounter;
  TCollection_AsciiString                       thesigntext;
  int                                           theexact;
  NCollection_Sequence<TCollection_AsciiString> thesignlist;
  NCollection_Sequence<int>                     thesignmode;
};

#endif // _IFSelect_SelectSignature_HeaderFile
