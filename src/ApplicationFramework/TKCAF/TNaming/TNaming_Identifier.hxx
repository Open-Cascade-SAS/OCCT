// Created on: 1997-03-03
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TNaming_Identifier_HeaderFile
#define _TNaming_Identifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_Label.hxx>
#include <TNaming_NameType.hxx>
#include <TNaming_NamedShape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
class TNaming_NamedShape;
class TNaming_Localizer;

class TNaming_Identifier
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TNaming_Identifier(const TDF_Label&       Lab,
                                     const TopoDS_Shape&    S,
                                     const TopoDS_Shape&    Context,
                                     const bool Geom);

  Standard_EXPORT TNaming_Identifier(const TDF_Label&                  Lab,
                                     const TopoDS_Shape&               S,
                                     const occ::handle<TNaming_NamedShape>& ContextNS,
                                     const bool            Geom);

  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT TNaming_NameType Type() const;

  Standard_EXPORT bool IsFeature();

  Standard_EXPORT occ::handle<TNaming_NamedShape> Feature() const;

  Standard_EXPORT void InitArgs();

  Standard_EXPORT bool MoreArgs() const;

  Standard_EXPORT void NextArg();

  Standard_EXPORT bool ArgIsFeature() const;

  Standard_EXPORT occ::handle<TNaming_NamedShape> FeatureArg();

  Standard_EXPORT TopoDS_Shape ShapeArg();

  Standard_EXPORT TopoDS_Shape ShapeContext() const;

  Standard_EXPORT occ::handle<TNaming_NamedShape> NamedShapeOfGeneration() const;

  Standard_EXPORT void AncestorIdentification(TNaming_Localizer&  Localizer,
                                              const TopoDS_Shape& Context);

  Standard_EXPORT void PrimitiveIdentification(TNaming_Localizer&                Localizer,
                                               const occ::handle<TNaming_NamedShape>& NS);

  Standard_EXPORT void GeneratedIdentification(TNaming_Localizer&                Localizer,
                                               const occ::handle<TNaming_NamedShape>& NS);

  Standard_EXPORT void Identification(TNaming_Localizer&                Localizer,
                                      const occ::handle<TNaming_NamedShape>& NS);

private:
  Standard_EXPORT void Init(const TopoDS_Shape& Context);

  TDF_Label                  myTDFAcces;
  TopoDS_Shape               myShape;
  bool           myDone;
  bool           myIsFeature;
  TNaming_NameType           myType;
  occ::handle<TNaming_NamedShape> myFeature;
  NCollection_List<occ::handle<TNaming_NamedShape>>   myPrimitiveArgs;
  NCollection_List<TopoDS_Shape>       myShapeArgs;
  occ::handle<TNaming_NamedShape> myNSContext;
};

#endif // _TNaming_Identifier_HeaderFile
