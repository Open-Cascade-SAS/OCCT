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

#include <XCAFDimTolObjects_Tool.hxx>
#include <XCAFDimTolObjects_DimensionObjectSequence.hxx>
#include <TDF_ChildIterator.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDimTolObjects_GeomToleranceObjectSequence.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDimTolObjects_DatumObjectSequence.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_Datum.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDocStd_Document.hxx>
#include <NCollection_DataMap.hxx>
#include <XCAFDimTolObjects_DataMapOfToleranceDatum.hxx>

//=======================================================================
//function : XCAFDimTolObjects_Tool
//purpose  : 
//=======================================================================

XCAFDimTolObjects_Tool::XCAFDimTolObjects_Tool(const Handle(TDocStd_Document)& theDoc)
{
  myDimTolTool = XCAFDoc_DocumentTool::DimTolTool(theDoc->Main());
}

//=======================================================================
//function : GetDimensions
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_Tool::GetDimensions(XCAFDimTolObjects_DimensionObjectSequence& theDimensionObjectSequence) const
{
  theDimensionObjectSequence.Clear();
  TDF_ChildIterator aChildIterator( myDimTolTool->Label() ); 
  for (; aChildIterator.More(); aChildIterator.Next()) {
    TDF_Label aL = aChildIterator.Value();
    Handle(XCAFDoc_Dimension) aDimension;
    if(aL.FindAttribute(XCAFDoc_Dimension::GetID(),aDimension)) {
      theDimensionObjectSequence.Append(aDimension->GetObject());
    }
  }
}

//=======================================================================
//function : GetGeomTolerances
//purpose  : 
//=======================================================================

void XCAFDimTolObjects_Tool::GetGeomTolerances(XCAFDimTolObjects_GeomToleranceObjectSequence& theGeomToleranceObjectSequence,
                                               XCAFDimTolObjects_DatumObjectSequence& theDatumSequence,
                                               XCAFDimTolObjects_DataMapOfToleranceDatum& theMap) const
{
  theGeomToleranceObjectSequence.Clear();
  TDF_ChildIterator aChildIterator( myDimTolTool->Label() ); 
  for (; aChildIterator.More(); aChildIterator.Next()) {
    TDF_Label aL = aChildIterator.Value();
    Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
    if(aL.FindAttribute(XCAFDoc_GeomTolerance::GetID(),aGeomTolerance)) {
      theGeomToleranceObjectSequence.Append(aGeomTolerance->GetObject());
      TDF_LabelSequence aSeq;
      if(myDimTolTool->GetDatumOfTolerLabels(aGeomTolerance->Label(), aSeq))
      {
        for(Standard_Integer i = 1; i <= aSeq.Length(); i++)
        {
          Handle(XCAFDoc_Datum) aDatum;
          if(aSeq.Value(i).FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
          {
            theDatumSequence.Append(aDatum->GetObject());
            theMap.Bind(theGeomToleranceObjectSequence.Last(), theDatumSequence.Last());
          }
        }
      }
    }
  }
}

//=======================================================================
//function : GetRefDimensions
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDimTolObjects_Tool::GetRefDimensions(const TopoDS_Shape& theShape,
                                                      XCAFDimTolObjects_DimensionObjectSequence& theDimensionObjectSequence) const
{
  theDimensionObjectSequence.Clear();
  TDF_Label aShapeL;
  myDimTolTool->ShapeTool()->Search(theShape, aShapeL);
  if(!aShapeL.IsNull())
  {
    TDF_LabelSequence aSeq;
    if( myDimTolTool->GetRefDimensionLabels(aShapeL, aSeq) ) {
      for(Standard_Integer i = 1; i <= aSeq.Length(); i++)
      {
        Handle(XCAFDoc_Dimension) aDimension;
        if( aSeq.Value(i).FindAttribute(XCAFDoc_Dimension::GetID(),aDimension))
          theDimensionObjectSequence.Append(aDimension->GetObject());
      }
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : GetRefGeomTolerances
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDimTolObjects_Tool::GetRefGeomTolerances(const TopoDS_Shape& theShape,
                                                     XCAFDimTolObjects_GeomToleranceObjectSequence& theGeomToleranceObjectSequence,
                                                     XCAFDimTolObjects_DatumObjectSequence& theDatumSequence,
                                                     XCAFDimTolObjects_DataMapOfToleranceDatum& theMap) const
{
  theGeomToleranceObjectSequence.Clear();
  TDF_Label aShapeL;
  myDimTolTool->ShapeTool()->Search(theShape, aShapeL);
  if(!aShapeL.IsNull())
  {
    TDF_LabelSequence aSeq;
    if( myDimTolTool->GetRefGeomToleranceLabels(aShapeL, aSeq) ) {
      for(Standard_Integer i = 1; i <= aSeq.Length(); i++)
      {
        Handle(XCAFDoc_GeomTolerance) aGeomTolerance;
        if( aSeq.Value(i).FindAttribute(XCAFDoc_GeomTolerance::GetID(),aGeomTolerance))
        {
          theGeomToleranceObjectSequence.Append(aGeomTolerance->GetObject());
          TDF_LabelSequence aLocalSeq;
          if(myDimTolTool->GetDatumOfTolerLabels(aGeomTolerance->Label(), aLocalSeq))
          {
            for(Standard_Integer j = 1; j <= aLocalSeq.Length(); j++)
            {
              Handle(XCAFDoc_Datum) aDatum;
              if(aLocalSeq.Value(j).FindAttribute(XCAFDoc_Datum::GetID(), aDatum))
              {
                theDatumSequence.Append(aDatum->GetObject());
                theMap.Bind(theGeomToleranceObjectSequence.Last(), theDatumSequence.Last());
              }
            }
          }
        }
      }
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : GetRefDatum
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDimTolObjects_Tool::GetRefDatum(const TopoDS_Shape& theShape,
                                                     Handle(XCAFDimTolObjects_DatumObject)& theDatumObject) const
{
  TDF_Label aShapeL;
  myDimTolTool->ShapeTool()->Search(theShape, aShapeL);
  if(!aShapeL.IsNull())
  {
    TDF_LabelSequence aDatumL;
    if(myDimTolTool->GetRefDatumLabel(aShapeL, aDatumL))
    {
      Handle(XCAFDoc_Datum) aDatum;
      if( aDatumL.First().FindAttribute(XCAFDoc_Datum::GetID(),aDatum)){
        theDatumObject = aDatum->GetObject();
        return Standard_True;
      }
    }
  }
  return Standard_False;
}
