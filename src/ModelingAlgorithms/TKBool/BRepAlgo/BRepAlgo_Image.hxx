// Created on: 1995-10-26
// Created by: Yves FRICAUD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepAlgo_Image_HeaderFile
#define _BRepAlgo_Image_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;

//! Stores link between a shape <S> and a shape <NewS>
//! obtained from <S>. <NewS> is an image of <S>.
class BRepAlgo_Image
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepAlgo_Image();

  Standard_EXPORT void SetRoot(const TopoDS_Shape& S);

  //! Links <NewS> as image of <OldS>.
  Standard_EXPORT void Bind(const TopoDS_Shape& OldS, const TopoDS_Shape& NewS);

  //! Links <NewS> as image of <OldS>.
  Standard_EXPORT void Bind(const TopoDS_Shape& OldS, const TopTools_ListOfShape& NewS);

  //! Add <NewS> to the image of <OldS>.
  Standard_EXPORT void Add(const TopoDS_Shape& OldS, const TopoDS_Shape& NewS);

  //! Add <NewS> to the image of <OldS>.
  Standard_EXPORT void Add(const TopoDS_Shape& OldS, const TopTools_ListOfShape& NewS);

  Standard_EXPORT void Clear();

  //! Remove <S> to set of images.
  Standard_EXPORT void Remove(const TopoDS_Shape& S);

  //! Removes the root <theRoot> from the list of roots and up and down maps.
  Standard_EXPORT void RemoveRoot(const TopoDS_Shape& Root);

  //! Replaces the <OldRoot> with the <NewRoot>, so all images
  //! of the <OldRoot> become the images of the <NewRoot>.
  //! The <OldRoot> is removed.
  Standard_EXPORT void ReplaceRoot(const TopoDS_Shape& OldRoot, const TopoDS_Shape& NewRoot);

  Standard_EXPORT const TopTools_ListOfShape& Roots() const;

  Standard_EXPORT Standard_Boolean IsImage(const TopoDS_Shape& S) const;

  //! Returns the generator of <S>
  Standard_EXPORT const TopoDS_Shape& ImageFrom(const TopoDS_Shape& S) const;

  //! Returns the upper generator of <S>
  Standard_EXPORT const TopoDS_Shape& Root(const TopoDS_Shape& S) const;

  Standard_EXPORT Standard_Boolean HasImage(const TopoDS_Shape& S) const;

  //! Returns the Image of <S>.
  //! Returns <S> in the list if HasImage(S) is false.
  Standard_EXPORT const TopTools_ListOfShape& Image(const TopoDS_Shape& S) const;

  //! Stores in <L> the images of images of...images of <S>.
  //! <L> contains only <S> if  HasImage(S) is false.
  Standard_EXPORT void LastImage(const TopoDS_Shape& S, TopTools_ListOfShape& L) const;

  //! Keeps only the link between roots and lastimage.
  Standard_EXPORT void Compact();

  //! Deletes in the images the shape of type <ShapeType>
  //! which are not in <S>.
  //! Warning:  Compact() must be call before.
  Standard_EXPORT void Filter(const TopoDS_Shape& S, const TopAbs_ShapeEnum ShapeType);

protected:
private:
  TopTools_ListOfShape               roots;
  TopTools_DataMapOfShapeShape       up;
  TopTools_DataMapOfShapeListOfShape down;
};

#endif // _BRepAlgo_Image_HeaderFile
