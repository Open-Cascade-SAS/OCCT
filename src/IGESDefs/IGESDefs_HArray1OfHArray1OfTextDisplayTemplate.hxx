// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDefs_HArray1OfHArray1OfTextDisplayTemplate_HeaderFile
#define _IGESDefs_HArray1OfHArray1OfTextDisplayTemplate_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_Array1OfTransient.hxx>
#include <Standard_Transient.hxx>
#include <IGESGraph_HArray1OfTextDisplayTemplate.hxx>
#include <Standard_Integer.hxx>

class IGESDefs_HArray1OfHArray1OfTextDisplayTemplate;
DEFINE_STANDARD_HANDLE(IGESDefs_HArray1OfHArray1OfTextDisplayTemplate, Standard_Transient)

class IGESDefs_HArray1OfHArray1OfTextDisplayTemplate : public Standard_Transient
{

public:
  Standard_EXPORT IGESDefs_HArray1OfHArray1OfTextDisplayTemplate(const Standard_Integer low,
                                                                 const Standard_Integer up);

  Standard_EXPORT Standard_Integer Lower() const;

  Standard_EXPORT Standard_Integer Upper() const;

  Standard_EXPORT Standard_Integer Length() const;

  Standard_EXPORT void SetValue(const Standard_Integer                                num,
                                const Handle(IGESGraph_HArray1OfTextDisplayTemplate)& val);

  Standard_EXPORT Handle(IGESGraph_HArray1OfTextDisplayTemplate) Value(
    const Standard_Integer num) const;

  DEFINE_STANDARD_RTTI_INLINE(IGESDefs_HArray1OfHArray1OfTextDisplayTemplate, Standard_Transient)

protected:
private:
  TColStd_Array1OfTransient thelist;
};

#endif // _IGESDefs_HArray1OfHArray1OfTextDisplayTemplate_HeaderFile
