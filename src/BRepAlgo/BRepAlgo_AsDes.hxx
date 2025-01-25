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

#ifndef _BRepAlgo_AsDes_HeaderFile
#define _BRepAlgo_AsDes_HeaderFile

#include <Standard.hxx>

#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;

class BRepAlgo_AsDes;
DEFINE_STANDARD_HANDLE(BRepAlgo_AsDes, Standard_Transient)

//! SD to store descendants and ascendants of Shapes.
class BRepAlgo_AsDes : public Standard_Transient
{

public:
  //! Creates an empty AsDes.
  Standard_EXPORT BRepAlgo_AsDes();

  Standard_EXPORT void Clear();

  //! Stores <SS> as a futur subshape of <S>.
  Standard_EXPORT void Add(const TopoDS_Shape& S, const TopoDS_Shape& SS);

  //! Stores <SS> as futurs SubShapes of <S>.
  Standard_EXPORT void Add(const TopoDS_Shape& S, const TopTools_ListOfShape& SS);

  Standard_EXPORT Standard_Boolean HasAscendant(const TopoDS_Shape& S) const;

  Standard_EXPORT Standard_Boolean HasDescendant(const TopoDS_Shape& S) const;

  //! Returns the Shape containing <S>.
  Standard_EXPORT const TopTools_ListOfShape& Ascendant(const TopoDS_Shape& S) const;

  //! Returns futur subhapes of <S>.
  Standard_EXPORT const TopTools_ListOfShape& Descendant(const TopoDS_Shape& S) const;

  //! Returns futur subhapes of <S>.
  Standard_EXPORT TopTools_ListOfShape& ChangeDescendant(const TopoDS_Shape& S);

  //! Replace theOldS by theNewS.
  //! theOldS disappear from this.
  Standard_EXPORT void Replace(const TopoDS_Shape& theOldS, const TopoDS_Shape& theNewS);

  //! Remove theS from me.
  Standard_EXPORT void Remove(const TopoDS_Shape& theS);

  //! Returns  True if (S1> and <S2>  has  common
  //! Descendants.  Stores in <LC> the Commons Descendants.
  Standard_EXPORT Standard_Boolean HasCommonDescendant(const TopoDS_Shape&   S1,
                                                       const TopoDS_Shape&   S2,
                                                       TopTools_ListOfShape& LC) const;

  DEFINE_STANDARD_RTTIEXT(BRepAlgo_AsDes, Standard_Transient)

private:
  //! Replace theOldS by theNewS.
  //! theOldS disappear from this.
  Standard_EXPORT void BackReplace(const TopoDS_Shape&         theOldS,
                                   const TopoDS_Shape&         theNewS,
                                   const TopTools_ListOfShape& theL,
                                   const Standard_Boolean      theInUp);

private:
  TopTools_DataMapOfShapeListOfShape up;
  TopTools_DataMapOfShapeListOfShape down;
};

#endif // _BRepAlgo_AsDes_HeaderFile
