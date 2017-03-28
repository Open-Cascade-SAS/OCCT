// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _StdLPersistent_Value_HeaderFile
#define _StdLPersistent_Value_HeaderFile

#include <StdObjMgt_Attribute.hxx>
#include <StdLPersistent_HString.hxx>

#include <TDataStd_Integer.hxx>
#include <TDF_TagSource.hxx>
#include <TDF_Reference.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_AsciiString.hxx>


class StdLPersistent_Value
{
  template <class AttribClass>
  class integer : public StdObjMgt_Attribute<AttribClass>::SingleInt
  {
  public:
    //! Import transient attribuite from the persistent data.
    Standard_EXPORT virtual void ImportAttribute();
  };

  template <class AttribClass,
            class HStringClass = StdLPersistent_HString::Extended>
  class string : public StdObjMgt_Attribute<AttribClass>::SingleRef
  {
  public:
    //! Import transient attribuite from the persistent data.
    Standard_EXPORT virtual void ImportAttribute();
  };

public:

  class TagSource : public integer <TDF_TagSource> {
  public:
    Standard_CString PName() const { return "PDF_TagSource"; }
  };

  class Reference : public string <TDF_Reference> {
  public:
    Standard_CString PName() const { return "PDF_Reference"; }
  };

  class Comment : public string <TDataStd_Comment> {
  public:
    Standard_CString PName() const { return "PDF_Comment"; }
  };

  class UAttribute : public string <TDataStd_UAttribute>
  {
  public:
    //! Create an empty transient attribuite
    Standard_EXPORT virtual Handle(TDF_Attribute) CreateAttribute();
    Standard_CString PName() const { return "PDataStd_UAttribute"; }
  };

  class Integer : public integer <TDataStd_Integer>
  {
  public:
    //! Create an empty transient attribuite
    Standard_EXPORT virtual Handle(TDF_Attribute) CreateAttribute();
    Standard_CString PName() const { return "PDataStd_Integer"; }
  };

  class Name : public string <TDataStd_Name>
  {
  public:
    //! Create an empty transient attribuite
    Standard_EXPORT virtual Handle(TDF_Attribute) CreateAttribute();
    Standard_CString PName() const { return "PDataStd_Name"; }
  };

  class AsciiString : public string <TDataStd_AsciiString, StdLPersistent_HString::Ascii>
  {
  public:
    //! Create an empty transient attribuite
    Standard_EXPORT virtual Handle(TDF_Attribute) CreateAttribute();
    Standard_CString PName() const { return "PDataStd_AsciiString"; }
  };
};

template<>
template<>
inline Standard_CString StdObjMgt_Attribute<TDF_TagSource>::Simple<Standard_Integer>::PName() const
  { return "PDF_TagSource"; }

template<>
template<>
inline Standard_CString StdObjMgt_Attribute<TDF_Reference>::Simple<Handle(StdObjMgt_Persistent)>::PName() const
  { return "PDF_Reference"; }

template<>
template<>
inline Standard_CString StdObjMgt_Attribute<TDataStd_Comment>::Simple<Handle(StdObjMgt_Persistent)>::PName() const
  { return "PDataStd_Comment"; }

#endif
