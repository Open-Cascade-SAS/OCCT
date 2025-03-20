// Created on: 2015-08-06
// Created by: Ilya Novikov
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDimTolObjects_Tool_HeaderFile
#define _XCAFDimTolObjects_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XCAFDoc_DimTolTool.hxx>
#include <TDocStd_Document.hxx>
#include <Standard_Boolean.hxx>
#include <XCAFDimTolObjects_DimensionObjectSequence.hxx>
#include <XCAFDimTolObjects_GeomToleranceObjectSequence.hxx>
#include <XCAFDimTolObjects_DatumObjectSequence.hxx>
#include <XCAFDimTolObjects_DataMapOfToleranceDatum.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
class TDocStd_Document;
class TopoDS_Shape;

class XCAFDimTolObjects_Tool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT XCAFDimTolObjects_Tool(const Handle(TDocStd_Document)& theDoc);

  //! Returns a sequence of Dimensions currently stored
  //! in the GD&T table
  Standard_EXPORT void GetDimensions(
    XCAFDimTolObjects_DimensionObjectSequence& theDimensionObjectSequence) const;

  //! Returns all Dimensions defined for Shape
  Standard_EXPORT Standard_Boolean
    GetRefDimensions(const TopoDS_Shape&                        theShape,
                     XCAFDimTolObjects_DimensionObjectSequence& theDimensions) const;

  //! Returns a sequence of Tolerances currently stored
  //! in the GD&T table
  Standard_EXPORT void GetGeomTolerances(
    XCAFDimTolObjects_GeomToleranceObjectSequence& theGeomToleranceObjectSequence,
    XCAFDimTolObjects_DatumObjectSequence&         theDatumObjectSequence,
    XCAFDimTolObjects_DataMapOfToleranceDatum&     theMap) const;

  //! Returns all GeomTolerances defined for Shape
  Standard_EXPORT Standard_Boolean GetRefGeomTolerances(
    const TopoDS_Shape&                            theShape,
    XCAFDimTolObjects_GeomToleranceObjectSequence& theGeomToleranceObjectSequence,
    XCAFDimTolObjects_DatumObjectSequence&         theDatumObjectSequence,
    XCAFDimTolObjects_DataMapOfToleranceDatum&     theMap) const;

  //! Returns DatumObject defined for Shape
  Standard_EXPORT Standard_Boolean
    GetRefDatum(const TopoDS_Shape&                    theShape,
                Handle(XCAFDimTolObjects_DatumObject)& theDatum) const;

private:
  Handle(XCAFDoc_DimTolTool) myDimTolTool;
};

#endif // _XCAFDimTolObjects_Tool_HeaderFile
