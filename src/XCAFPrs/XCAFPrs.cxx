// Created on: 2000-08-15
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <BRep_Builder.hxx>
#include <TColStd_HSequenceOfExtendedString.hxx>
#include <TDF_AttributeSequence.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopLoc_IndexedMapOfLocation.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>
#include <XCAFPrs_Style.hxx>

static Standard_Boolean viewnameMode = Standard_False;

static Standard_Boolean getShapesOfSHUO (TopLoc_IndexedMapOfLocation& theaPrevLocMap,
                                         const Handle(XCAFDoc_ShapeTool)& theSTool,
                                         const TDF_Label& theSHUOlab,
                                         TopTools_SequenceOfShape& theSHUOShapeSeq)
{
  Handle(XCAFDoc_GraphNode) SHUO;
  TDF_LabelSequence aLabSeq;
  theSTool->GetSHUONextUsage( theSHUOlab, aLabSeq );
  if (aLabSeq.Length() >= 1)
    for (Standard_Integer i = 1; i <= aLabSeq.Length(); i++) {
      TDF_Label aSubCompL = aLabSeq.Value( i );
      TopLoc_Location compLoc = XCAFDoc_ShapeTool::GetLocation ( aSubCompL.Father() );
      // create new map of laocation (to not merge locations from different shapes)
      TopLoc_IndexedMapOfLocation aNewPrevLocMap;
      for (Standard_Integer m = 1; m <= theaPrevLocMap.Extent(); m++)
        aNewPrevLocMap.Add( theaPrevLocMap.FindKey( m ) );
      aNewPrevLocMap.Add( compLoc );
      // got for the new sublocations and corresponding shape
      getShapesOfSHUO( aNewPrevLocMap, theSTool, aSubCompL, theSHUOShapeSeq );
    }
  else {
    TopoDS_Shape aSHUO_NUSh = theSTool->GetShape ( theSHUOlab.Father() );
    if ( aSHUO_NUSh.IsNull() ) return Standard_False;
    // cause got shape with location already.
    TopLoc_Location nullLoc;
    aSHUO_NUSh.Location ( nullLoc );
    // multiply the locations
    Standard_Integer intMapLenght = theaPrevLocMap.Extent();
    if ( intMapLenght < 1 )
      return Standard_False; // should not be, but to avoid exception...?
    TopLoc_Location SupcompLoc;
    SupcompLoc = theaPrevLocMap.FindKey( intMapLenght );
    if (intMapLenght > 1) {
      Standard_Integer l = intMapLenght - 1;
      while (l >= 1) {
        SupcompLoc = theaPrevLocMap.FindKey( l ).Multiplied( SupcompLoc );
        l--;
      }
    }
    aSHUO_NUSh.Location( SupcompLoc );
    theSHUOShapeSeq.Append( aSHUO_NUSh );
  }
  return (theSHUOShapeSeq.Length() > 0);
}

//=======================================================================
//function : CollectStyleSettings
//purpose  : 
//=======================================================================

void XCAFPrs::CollectStyleSettings (const TDF_Label& theLabel,
				                            const TopLoc_Location& theLoc,
				                            XCAFPrs_DataMapOfShapeStyle& theSettings)
{
  // for references, first collect colors of referred shape
  {
    TDF_Label aLabelRef;
    if (XCAFDoc_ShapeTool::GetReferredShape (theLabel, aLabelRef))
    {
      TopLoc_Location aLocSub = theLoc.Multiplied (XCAFDoc_ShapeTool::GetLocation (theLabel));
      CollectStyleSettings (aLabelRef, aLocSub, theSettings);
    }
  }

  // for assemblies, first collect colors defined in components
  {
    TDF_LabelSequence aComponentLabSeq;
    if (XCAFDoc_ShapeTool::GetComponents (theLabel, aComponentLabSeq)
    && !aComponentLabSeq.IsEmpty())
    {
      for (TDF_LabelSequence::Iterator aComponentIter (aComponentLabSeq); aComponentIter.More(); aComponentIter.Next())
      {
        const TDF_Label& aComponentLab = aComponentIter.Value();
        CollectStyleSettings (aComponentLab, theLoc, theSettings);
      }
    }
  }

  // collect settings on subshapes
  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool(theLabel);
  TDF_LabelSequence aLabSeq;
  XCAFDoc_ShapeTool::GetSubShapes (theLabel, aLabSeq);
  // and add the shape itself
  aLabSeq.Append (theLabel);
  for (TDF_LabelSequence::Iterator aLabIter (aLabSeq); aLabIter.More(); aLabIter.Next())
  {
    const TDF_Label& aLabel = aLabIter.Value();
    XCAFPrs_Style aStyle;

    Standard_Boolean isVisible = aColorTool->IsVisible (aLabel);
    if (isVisible)
    {
      Handle(XCAFDoc_LayerTool) aLayerTool = XCAFDoc_DocumentTool::LayerTool (aLabel);
      Handle(TColStd_HSequenceOfExtendedString) aLayerNames = new TColStd_HSequenceOfExtendedString();
      aLayerTool->GetLayers (aLabel, aLayerNames);
      Standard_Integer aNbHidden = 0;
      for (TColStd_HSequenceOfExtendedString::Iterator aLayerIter (*aLayerNames); aLayerIter.More(); aLayerIter.Next())
      {
        const TCollection_ExtendedString& aLayerName = aLayerIter.Value();
        if (!aLayerTool->IsVisible (aLayerTool->FindLayer (aLayerName)))
        {
          ++aNbHidden;
        }
      }
      isVisible = aNbHidden == 0
               || aNbHidden != aLayerNames->Length();
    }

    if (!isVisible)
    {
      aStyle.SetVisibility (Standard_False);
    }
    else
    {
      Quantity_ColorRGBA aColor;
      if (aColorTool->GetColor (aLabel, XCAFDoc_ColorGen, aColor))
      {
        aStyle.SetColorCurv (aColor.GetRGB());
        aStyle.SetColorSurf (aColor);
      }
      if (aColorTool->GetColor (aLabel, XCAFDoc_ColorSurf, aColor))
      {
        aStyle.SetColorSurf (aColor);
      }
      if (aColorTool->GetColor (aLabel, XCAFDoc_ColorCurv, aColor))
      {
        aStyle.SetColorCurv (aColor.GetRGB());
      }
    }

    // PTV try to set color from SHUO structure
    Handle(XCAFDoc_ShapeTool) aShapeTool = aColorTool->ShapeTool();
    if (aShapeTool->IsComponent (aLabel))
    {
      TDF_AttributeSequence aShuoAttribSeq;
      aShapeTool->GetAllComponentSHUO (aLabel, aShuoAttribSeq);
      for (TDF_AttributeSequence::Iterator aShuoAttribIter (aShuoAttribSeq); aShuoAttribIter.More(); aShuoAttribIter.Next())
      {
        Handle(XCAFDoc_GraphNode) aShuoNode = Handle(XCAFDoc_GraphNode)::DownCast (aShuoAttribIter.Value());
        if (aShuoNode.IsNull())
        {
          continue;
        }

        const TDF_Label aShuolab = aShuoNode->Label();
        {
          TDF_LabelSequence aShuoLabSeq;
          aShapeTool->GetSHUONextUsage (aShuolab, aShuoLabSeq);
          if (aShuoLabSeq.IsEmpty())
          {
            continue;
          }
        }

        Quantity_ColorRGBA aColor;
        XCAFPrs_Style aShuoStyle;
        if (!aColorTool->IsVisible (aShuolab))
        {
          aShuoStyle.SetVisibility (Standard_False);
        }
        else
        {
          if (aColorTool->GetColor (aShuolab, XCAFDoc_ColorGen, aColor))
          {
            aShuoStyle.SetColorCurv (aColor.GetRGB());
            aShuoStyle.SetColorSurf (aColor);
          }
          if (aColorTool->GetColor (aShuolab, XCAFDoc_ColorSurf, aColor))
          {
            aShuoStyle.SetColorSurf (aColor);
          }
          if (aColorTool->GetColor (aShuolab, XCAFDoc_ColorCurv, aColor))
          {
            aShuoStyle.SetColorCurv (aColor.GetRGB());
          }
        }
        if (!aShuoStyle.IsSetColorCurv()
         && !aShuoStyle.IsSetColorSurf()
         &&  aShuoStyle.IsVisible())
        {
          continue;
        }

        // set style for all component from Next Usage Occurrence.
      #ifdef OCCT_DEBUG
        cout << "Set the style for SHUO next_usage-occurrance" << endl;
      #endif
        /* 
        // may be work, but static it returns excess shapes. It is more faster to use OLD version.
        // PTV 14.02.2003 NEW version using API of ShapeTool
        TopTools_SequenceOfShape aShuoShapeSeq;
        aShapeTool->GetAllStyledComponents (aShuoNode, aShuoShapeSeq);
        for (TopTools_SequenceOfShape::Iterator aShuoShapeIter (aShuoShapeSeq); aShuoShapeIter.More(); aShuoShapeIter.Next())
        {
          const TopoDS_Shape& aShuoShape = aShuoShapeIter.Value();
          if (!aShuoShape.IsNull())
            theSettings.Bind (aShuoShape, aShuoStyle);
        }*/
        // OLD version that was written before ShapeTool API, and it FASTER for presentation
        // get TOP location of SHUO component
        TopLoc_Location compLoc = XCAFDoc_ShapeTool::GetLocation (aLabel);
        TopLoc_IndexedMapOfLocation aPrevLocMap;
        // get previous set location
        if (!theLoc.IsIdentity())
        {
          aPrevLocMap.Add (theLoc);
        }
        aPrevLocMap.Add (compLoc);

        // get shapes of SHUO Next_Usage components
        TopTools_SequenceOfShape aShuoShapeSeq;
        getShapesOfSHUO (aPrevLocMap, aShapeTool, aShuolab, aShuoShapeSeq);
        for (TopTools_SequenceOfShape::Iterator aShuoShapeIter (aShuoShapeSeq); aShuoShapeIter.More(); aShuoShapeIter.Next())
        {
          const TopoDS_Shape& aShuoShape = aShuoShapeIter.Value();
          theSettings.Bind (aShuoShape, aShuoStyle);
        }
        continue;
      }
    }

    if (!aStyle.IsSetColorCurv()
     && !aStyle.IsSetColorSurf()
     &&  aStyle.IsVisible())
    {
      continue;
    }

    TopoDS_Shape aSubshape = XCAFDoc_ShapeTool::GetShape (aLabel);
    if (aSubshape.ShapeType() == TopAbs_COMPOUND)
    {
      const TopoDS_Iterator aShapeIter (aSubshape);
      if (!aShapeIter.More())
      {
        continue;
      }
    }
    aSubshape.Move (theLoc);
    theSettings.Bind (aSubshape, aStyle);
  }
}

//=======================================================================
//function : SetViewNameMode
//purpose  : 
//=======================================================================

void XCAFPrs::SetViewNameMode(const Standard_Boolean aNameMode )
{
  viewnameMode = aNameMode;
}

//=======================================================================
//function : GetViewNameMode
//purpose  : 
//=======================================================================

Standard_Boolean XCAFPrs::GetViewNameMode()
{
  return viewnameMode;
}
