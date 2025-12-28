// Created on: 1996-12-23
// Created by: Alexander BRIVIN
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

#ifndef _Vrml_SFImage_HeaderFile
#define _Vrml_SFImage_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Vrml_SFImageNumber.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>

//! defines SFImage type of VRML field types.
class Vrml_SFImage : public Standard_Transient
{

public:
  Standard_EXPORT Vrml_SFImage();

  Standard_EXPORT Vrml_SFImage(const int                  aWidth,
                               const int                  aHeight,
                               const Vrml_SFImageNumber                aNumber,
                               const occ::handle<NCollection_HArray1<int>>& anArray);

  Standard_EXPORT void SetWidth(const int aWidth);

  Standard_EXPORT int Width() const;

  Standard_EXPORT void SetHeight(const int aHeight);

  Standard_EXPORT int Height() const;

  Standard_EXPORT void SetNumber(const Vrml_SFImageNumber aNumber);

  Standard_EXPORT Vrml_SFImageNumber Number() const;

  Standard_EXPORT void SetArray(const occ::handle<NCollection_HArray1<int>>& anArray);

  Standard_EXPORT occ::handle<NCollection_HArray1<int>> Array() const;

  Standard_EXPORT bool ArrayFlag() const;

  DEFINE_STANDARD_RTTIEXT(Vrml_SFImage, Standard_Transient)

private:
  int                 myWidth;
  int                 myHeight;
  Vrml_SFImageNumber               myNumber;
  occ::handle<NCollection_HArray1<int>> myArray;
  bool                 myArrayFlag;
};

#endif // _Vrml_SFImage_HeaderFile
