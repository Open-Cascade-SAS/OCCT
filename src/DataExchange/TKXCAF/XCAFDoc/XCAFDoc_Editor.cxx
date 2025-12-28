// Created on: 2015-05-14
// Created by: data exchange team
// Copyright (c) 2000-2015 OPEN CASCADE SAS
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

#include <XCAFDoc_Editor.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Message.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFNoteObjects_NoteObject.hxx>
#include <XCAFDoc_AssemblyItemRef.hxx>
#include <XCAFDoc_AssemblyTool.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_Location.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_NoteBalloon.hxx>
#include <XCAFDoc_NoteBinData.hxx>
#include <XCAFDoc_NoteComment.hxx>
#include <XCAFDoc_NotesTool.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>
#include <XCAFDoc_Volume.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>

//=======================================================================
// function : Expand
// purpose  : Convert Shape to assembly
//=======================================================================
bool XCAFDoc_Editor::Expand(const TDF_Label& theDoc,
                            const TDF_Label& theShape,
                            const bool       theRecursively)
{
  if (theDoc.IsNull() || theShape.IsNull())
  {
    return false;
  }
  occ::handle<XCAFDoc_ShapeTool> aShapeTool   = XCAFDoc_DocumentTool::ShapeTool(theDoc);
  bool                           isAutoNaming = aShapeTool->AutoNaming();
  aShapeTool->SetAutoNaming(false);

  TDF_Label aCompoundPartL = theShape;
  if (aShapeTool->IsReference(theShape))
    aShapeTool->GetReferredShape(aCompoundPartL, aCompoundPartL);

  TopoDS_Shape aS = aShapeTool->GetShape(aCompoundPartL);
  if (aShapeTool->Expand(aCompoundPartL))
  {
    // move attributes
    for (TDF_ChildIterator aPartIter(aCompoundPartL, true); aPartIter.More(); aPartIter.Next())
    {
      TDF_Label aChild = aPartIter.Value();
      // get part
      TDF_Label aPart;
      if (aShapeTool->GetReferredShape(aChild, aPart))
      {
        CloneMetaData(aChild, aPart, NULL);
        // remove unnecessary links
        TopoDS_Shape aShape = aShapeTool->GetShape(aChild);
        if (!aShapeTool->GetShape(aPart.Father()).IsNull())
        {
          aPart.ForgetAttribute(XCAFDoc::ShapeRefGUID());
          if (aShapeTool->GetShape(aPart.Father()).ShapeType() == TopAbs_COMPOUND)
          {
            aShapeTool->SetShape(aPart, aShape);
          }
          aPart.ForgetAttribute(XCAFDoc_ShapeMapTool::GetID());
          aChild.ForgetAllAttributes(false);
        }
        aChild.ForgetAttribute(TNaming_NamedShape::GetID());
        aChild.ForgetAttribute(XCAFDoc_ShapeMapTool::GetID());
      }
      else
      {
        // If new original shape is not created, try to process this child
        // as subshape of new part
        NCollection_Sequence<TDF_Label> aUsers;
        if (aShapeTool->GetUsers(aChild, aUsers) > 0)
        {
          for (NCollection_Sequence<TDF_Label>::Iterator anIter(aUsers); anIter.More();
               anIter.Next())
          {
            TDF_Label aSubLabel = anIter.Value();
            // remove unnecessary links
            aSubLabel.ForgetAttribute(XCAFDoc::ShapeRefGUID());
            aSubLabel.ForgetAttribute(XCAFDoc_ShapeMapTool::GetID());
            CloneMetaData(aChild, aSubLabel, NULL);
          }
          aChild.ForgetAllAttributes(false);
        }
      }
    }
    // if assembly contains compound, expand it recursively(if flag theRecursively is true)
    if (theRecursively)
    {
      for (TDF_ChildIterator aPartIter(aCompoundPartL); aPartIter.More(); aPartIter.Next())
      {
        TDF_Label aPart = aPartIter.Value();
        if (aShapeTool->GetReferredShape(aPart, aPart))
        {
          TopoDS_Shape aPartShape = aShapeTool->GetShape(aPart);
          if (!aPartShape.IsNull() && aPartShape.ShapeType() == TopAbs_COMPOUND)
            Expand(theDoc, aPart, theRecursively);
        }
      }
    }
    aShapeTool->SetAutoNaming(isAutoNaming);
    return true;
  }
  aShapeTool->SetAutoNaming(isAutoNaming);
  return false;
}

//=======================================================================
// function : Expand
// purpose  : Convert all compounds in Doc to assembly
//=======================================================================
bool XCAFDoc_Editor::Expand(const TDF_Label& theDoc, const bool theRecursively)
{
  if (theDoc.IsNull())
  {
    return false;
  }
  bool                            aResult = false;
  NCollection_Sequence<TDF_Label> aLabels;
  occ::handle<XCAFDoc_ShapeTool>  aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc);
  aShapeTool->GetFreeShapes(aLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator anIter(aLabels); anIter.More(); anIter.Next())
  {
    const TDF_Label    aLabel = anIter.Value();
    const TopoDS_Shape aS     = aShapeTool->GetShape(aLabel);
    if (!aS.IsNull() && aS.ShapeType() == TopAbs_COMPOUND && !aShapeTool->IsAssembly(aLabel))
    {
      if (Expand(theDoc, aLabel, theRecursively))
      {
        aResult = true;
      }
    }
  }
  return aResult;
}

//=================================================================================================

bool XCAFDoc_Editor::Extract(const NCollection_Sequence<TDF_Label>& theSrcLabels,
                             const TDF_Label&                       theDstLabel,
                             const bool                             theIsNoVisMat)
{
  if (theDstLabel.IsNull())
  {
    return false;
  }

  occ::handle<XCAFDoc_ShapeTool> aDstShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDstLabel);
  NCollection_DataMap<occ::handle<XCAFDoc_VisMaterial>, occ::handle<XCAFDoc_VisMaterial>>
    aVisMatMap;
  for (NCollection_Sequence<TDF_Label>::Iterator aNewRootIter(theSrcLabels); aNewRootIter.More();
       aNewRootIter.Next())
  {
    // Shape
    TopLoc_Location                           aLoc;
    NCollection_DataMap<TDF_Label, TDF_Label> aMap;
    const TDF_Label                           aSrcLabel = aNewRootIter.Value();
    occ::handle<XCAFDoc_ShapeTool> aSrcShapeTool = XCAFDoc_DocumentTool::ShapeTool(aSrcLabel);
    occ::handle<XCAFDoc_Location>  aLocationAttr;
    if (aSrcLabel.FindAttribute(XCAFDoc_Location::GetID(), aLocationAttr))
    {
      aLoc = aLocationAttr->Get();
    }
    TDF_Label aCompLabel = aSrcLabel;
    aSrcShapeTool->GetReferredShape(aSrcLabel, aCompLabel);
    TDF_Label aResLabel     = CloneShapeLabel(aCompLabel, aSrcShapeTool, aDstShapeTool, aMap);
    TDF_Label aNewCompLabel = aDstShapeTool->AddComponent(theDstLabel, aResLabel, aLoc);
    if (aNewCompLabel.IsNull())
    {
      TopoDS_Shape aNewShape = aDstShapeTool->GetShape(aResLabel);
      aNewShape.Move(aLoc);
      aNewCompLabel = aDstShapeTool->AddShape(aNewShape, false);
    }
    aMap.Bind(aSrcLabel, aNewCompLabel);
    aMap.Bind(aCompLabel, aResLabel);
    aDstShapeTool->UpdateAssemblies();
    // Attributes
    for (NCollection_DataMap<TDF_Label, TDF_Label>::Iterator aLabelIter(aMap); aLabelIter.More();
         aLabelIter.Next())
    {
      CloneMetaData(aLabelIter.Key(),
                    aLabelIter.Value(),
                    &aVisMatMap,
                    true,
                    true,
                    true,
                    !theIsNoVisMat);
    }
  }
  return true;
}

//=================================================================================================

bool XCAFDoc_Editor::Extract(const TDF_Label& theSrcLabel,
                             const TDF_Label& theDstLabel,
                             const bool       theIsNoVisMat)
{
  NCollection_Sequence<TDF_Label> aSeq;
  aSeq.Append(theSrcLabel);
  return Extract(aSeq, theDstLabel, theIsNoVisMat);
}

//=================================================================================================

TDF_Label XCAFDoc_Editor::CloneShapeLabel(const TDF_Label&                      theSrcLabel,
                                          const occ::handle<XCAFDoc_ShapeTool>& theSrcShapeTool,
                                          const occ::handle<XCAFDoc_ShapeTool>& theDstShapeTool,
                                          NCollection_DataMap<TDF_Label, TDF_Label>& theMap)
{
  TDF_Label aNewShL;
  if (theMap.Find(theSrcLabel, aNewShL))
  {
    return aNewShL;
  }

  // Location for main assembly
  if (theSrcShapeTool->IsAssembly(theSrcLabel))
  {
    // add assembly and iterate all its components
    TopoDS_Compound aComp;
    BRep_Builder().MakeCompound(aComp);
    aNewShL = theDstShapeTool->AddShape(aComp);
    theMap.Bind(theSrcLabel, aNewShL);

    NCollection_Sequence<TDF_Label> aComponents;
    theSrcShapeTool->GetComponents(theSrcLabel, aComponents);
    for (NCollection_Sequence<TDF_Label>::Iterator aCompIter(aComponents); aCompIter.More();
         aCompIter.Next())
    {
      TDF_Label aCompL = aCompIter.Value();
      TDF_Label aRefL;
      theSrcShapeTool->GetReferredShape(aCompL, aRefL);
      TDF_Label aCompOriginalL = CloneShapeLabel(aRefL, theSrcShapeTool, theDstShapeTool, theMap);
      occ::handle<XCAFDoc_Location> aLocationAttr;
      aCompL.FindAttribute(XCAFDoc_Location::GetID(), aLocationAttr);
      TDF_Label aNewCompL =
        theDstShapeTool->AddComponent(aNewShL, aCompOriginalL, aLocationAttr->Get());
      theMap.Bind(aCompIter.Value(), aNewCompL);
    }
    return aNewShL;
  }

  // add part
  TopoDS_Shape aShape = theSrcShapeTool->GetShape(theSrcLabel);
  aNewShL             = theDstShapeTool->AddShape(aShape, false);
  theMap.Bind(theSrcLabel, aNewShL);

  // get original instead of auxiliary instance
  TDF_Label anOldOriginalL = theSrcLabel;
  if (theSrcShapeTool->IsReference(theSrcLabel))
  {
    theSrcShapeTool->GetReferredShape(theSrcLabel, anOldOriginalL);
    theDstShapeTool->GetReferredShape(aNewShL, aNewShL);
    theMap.Bind(anOldOriginalL, aNewShL);
  }
  // copy subshapes
  NCollection_Sequence<TDF_Label> anOldSubShapes;
  theSrcShapeTool->GetSubShapes(anOldOriginalL, anOldSubShapes);
  for (NCollection_Sequence<TDF_Label>::Iterator aSubIter(anOldSubShapes); aSubIter.More();
       aSubIter.Next())
  {
    TopoDS_Shape aSubShape     = theSrcShapeTool->GetShape(aSubIter.Value());
    TDF_Label    aNewSubShapeL = theDstShapeTool->AddSubShape(aNewShL, aSubShape);
    theMap.Bind(aSubIter.Value(), aNewSubShapeL);
  }
  return aNewShL;
}

//=================================================================================================

void XCAFDoc_Editor::CloneMetaData(
  const TDF_Label& theSrcLabel,
  const TDF_Label& theDstLabel,
  NCollection_DataMap<occ::handle<XCAFDoc_VisMaterial>, occ::handle<XCAFDoc_VisMaterial>>*
             theVisMatMap,
  const bool theToCopyColor,
  const bool theToCopyLayer,
  const bool theToCopyMaterial,
  const bool theToCopyVisMaterial,
  const bool theToCopyAttribute)
{
  if (theSrcLabel == theDstLabel || theSrcLabel.IsNull() || theDstLabel.IsNull())
  {
    return;
  }
  const bool toCopyColor = theToCopyColor && XCAFDoc_DocumentTool::CheckColorTool(theSrcLabel);
  const bool toCopyLayer = theToCopyLayer && XCAFDoc_DocumentTool::CheckLayerTool(theSrcLabel);
  const bool toCopyMaterial =
    theToCopyMaterial && XCAFDoc_DocumentTool::CheckMaterialTool(theSrcLabel);
  const bool toCopyVisMaterial = XCAFDoc_DocumentTool::CheckVisMaterialTool(theSrcLabel);
  // Colors
  if (toCopyColor)
  {
    occ::handle<XCAFDoc_ColorTool> aSrcColorTool = XCAFDoc_DocumentTool::ColorTool(theSrcLabel);
    occ::handle<XCAFDoc_ColorTool> aDstColorTool = XCAFDoc_DocumentTool::ColorTool(theDstLabel);
    const XCAFDoc_ColorType aTypes[] = {XCAFDoc_ColorGen, XCAFDoc_ColorSurf, XCAFDoc_ColorCurv};
    for (int anInd = 0; anInd < 3; anInd++)
    {
      TDF_Label aColorL;
      aSrcColorTool->GetColor(theSrcLabel, aTypes[anInd], aColorL);
      if (!aColorL.IsNull())
      {
        Quantity_ColorRGBA aColor;
        aSrcColorTool->GetColor(aColorL, aColor);
        aDstColorTool->SetColor(theDstLabel, aColor, aTypes[anInd]);
      }
    }
    aDstColorTool->SetVisibility(theDstLabel, aSrcColorTool->IsVisible(theSrcLabel));
  }
  // Layers
  if (toCopyLayer)
  {
    occ::handle<XCAFDoc_LayerTool>  aSrcLayerTool = XCAFDoc_DocumentTool::LayerTool(theSrcLabel);
    occ::handle<XCAFDoc_LayerTool>  aDstLayerTool = XCAFDoc_DocumentTool::LayerTool(theDstLabel);
    NCollection_Sequence<TDF_Label> aLayers;
    aSrcLayerTool->GetLayers(theSrcLabel, aLayers);
    for (NCollection_Sequence<TDF_Label>::Iterator aLayerIter(aLayers); aLayerIter.More();
         aLayerIter.Next())
    {
      TCollection_ExtendedString aLayerName;
      aSrcLayerTool->GetLayer(aLayerIter.Value(), aLayerName);
      aDstLayerTool->SetLayer(theDstLabel, aLayerName);
    }
  }
  // Materials
  if (toCopyMaterial)
  {
    occ::handle<TDataStd_TreeNode> aMatNode;
    if (theSrcLabel.FindAttribute(XCAFDoc::MaterialRefGUID(), aMatNode) && aMatNode->HasFather())
    {
      TDF_Label aMaterialL = aMatNode->Father()->Label();
      if (!aMaterialL.IsNull())
      {
        occ::handle<XCAFDoc_MaterialTool> aSrcMaterialTool =
          XCAFDoc_DocumentTool::MaterialTool(theSrcLabel);
        occ::handle<XCAFDoc_MaterialTool> aDstMaterialTool =
          XCAFDoc_DocumentTool::MaterialTool(theDstLabel);
        double                                aDensity = 0.0;
        occ::handle<TCollection_HAsciiString> aName, aDescription, aDensName, aDensValType;
        if (aSrcMaterialTool
              ->GetMaterial(aMaterialL, aName, aDescription, aDensity, aDensName, aDensValType)
            && !aName.IsNull() && aName->Length() != 0)
        {
          aDstMaterialTool
            ->SetMaterial(theDstLabel, aName, aDescription, aDensity, aDensName, aDensValType);
        }
      }
    }
  }
  // Visual Materials
  if (toCopyVisMaterial && (theToCopyVisMaterial || toCopyColor))
  {
    occ::handle<XCAFDoc_VisMaterialTool> aSrcVisMatTool =
      XCAFDoc_DocumentTool::VisMaterialTool(theSrcLabel);
    TDF_Label aVisMaterialL;
    aSrcVisMatTool->GetShapeMaterial(theSrcLabel, aVisMaterialL);
    if (!aVisMaterialL.IsNull())
    {
      occ::handle<XCAFDoc_VisMaterialTool> aDstVisMatTool;
      occ::handle<XCAFDoc_ColorTool>       aDstColorTool;
      if (theToCopyVisMaterial)
      {
        aDstVisMatTool = XCAFDoc_DocumentTool::VisMaterialTool(theDstLabel);
      }
      else
      {
        aDstColorTool = XCAFDoc_DocumentTool::ColorTool(theDstLabel);
      }
      occ::handle<XCAFDoc_VisMaterial> aVisMatSrc = aSrcVisMatTool->GetMaterial(aVisMaterialL);
      if (theToCopyVisMaterial)
      {
        occ::handle<XCAFDoc_VisMaterial> aVisMatDst;
        if (theVisMatMap != NULL)
        {
          if (!theVisMatMap->Find(aVisMatSrc, aVisMatDst))
          {
            aVisMatDst = new XCAFDoc_VisMaterial();
            aVisMatDst->SetCommonMaterial(aVisMatSrc->CommonMaterial());
            aVisMatDst->SetPbrMaterial(aVisMatSrc->PbrMaterial());
            aVisMatDst->SetAlphaMode(aVisMatSrc->AlphaMode(), aVisMatSrc->AlphaCutOff());
            aVisMatDst->SetFaceCulling(aVisMatSrc->FaceCulling());
            TCollection_AsciiString    aName;
            occ::handle<TDataStd_Name> aNodeName;
            if (aVisMatSrc->Label().FindAttribute(TDataStd_Name::GetID(), aNodeName))
            {
              aName = aNodeName->Get();
            }
            aDstVisMatTool->AddMaterial(aVisMatDst, aName);
            theVisMatMap->Bind(aVisMatSrc, aVisMatDst);
          }
        }
        else
        {
          aVisMatDst = aVisMatSrc; // consider the same document
        }
        aDstVisMatTool->SetShapeMaterial(theDstLabel, aVisMatDst->Label());
      }
      else
      {
        aDstColorTool->SetColor(theDstLabel, aVisMatSrc->BaseColor(), XCAFDoc_ColorGen);
      }
    }
  }
  // Other attributes
  if (theToCopyAttribute)
  {
    // Finds the target attributes or creates them empty
    for (TDF_AttributeIterator anAttIter(theSrcLabel); anAttIter.More(); anAttIter.Next())
    {
      const occ::handle<TDF_Attribute> anAttSrc = anAttIter.Value();
      // protect against color and layer coping without link to colors and layers
      if (const TDataStd_TreeNode* aTreeNode =
            dynamic_cast<const TDataStd_TreeNode*>(anAttSrc.get()))
      {
        (void)aTreeNode;
        continue;
      }
      else if (const XCAFDoc_GraphNode* aGraphNode =
                 dynamic_cast<const XCAFDoc_GraphNode*>(anAttSrc.get()))
      {
        (void)aGraphNode;
        continue;
      }
      else if (const TNaming_NamedShape* aShapeAttr =
                 dynamic_cast<const TNaming_NamedShape*>(anAttSrc.get()))
      {
        (void)aShapeAttr;
        continue;
      }
      else if (const XCAFDoc_ShapeMapTool* aShMapTool =
                 dynamic_cast<const XCAFDoc_ShapeMapTool*>(anAttSrc.get()))
      {
        (void)aShMapTool;
        continue;
      }
      else if (const XCAFDoc_Location* aLocAttr =
                 dynamic_cast<const XCAFDoc_Location*>(anAttSrc.get()))
      {
        (void)aLocAttr;
        continue;
      }
      occ::handle<TDF_Attribute> anAttDst;
      if (!theDstLabel.FindAttribute(anAttSrc->ID(), anAttDst))
      {
        anAttDst = anAttSrc->NewEmpty();
        theDstLabel.AddAttribute(anAttDst);
      }
      occ::handle<TDF_RelocationTable> aRT = new TDF_RelocationTable();
      anAttSrc->Paste(anAttDst, aRT);
    }
  }
  // Name
  occ::handle<TDataStd_Name> aNameAttr;
  theSrcLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttr);
  occ::handle<XCAFDoc_ShapeTool> aDstShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDstLabel);
  if (!aNameAttr.IsNull())
  {
    TDF_Label aRefLabel;
    if (aNameAttr->Get().Search("=>") < 0)
    {
      TDataStd_Name::Set(theDstLabel, aNameAttr->Get());
    }
    else if (aDstShapeTool->GetReferredShape(theDstLabel, aRefLabel))
    {
      TCollection_AsciiString aRefName;
      TDF_Tool::Entry(aRefLabel, aRefName);
      aRefName.Insert(1, "=>");
      TDataStd_Name::Set(theDstLabel, aRefName);
    }
  }
}

//=======================================================================
// function : rescaleDimensionRefLabels
// purpose  : Applies geometrical scale to dimension's reference shapes
//           not belonging to the assembly graph
//=======================================================================

static void rescaleDimensionRefLabels(const NCollection_Sequence<TDF_Label>&    theRefLabels,
                                      BRepBuilderAPI_Transform&                 theBRepTrsf,
                                      const occ::handle<XCAFDoc_AssemblyGraph>& theGraph,
                                      const TCollection_AsciiString&            theEntryDimension)
{
  for (NCollection_Sequence<TDF_Label>::Iterator anIt(theRefLabels); anIt.More(); anIt.Next())
  {
    const TDF_Label& aL = anIt.Value();
    if (!theGraph->GetNodes().Contains(aL))
    {
      occ::handle<TNaming_NamedShape> aNS;
      if (aL.FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Shape aShape = aNS->Get();
        theBRepTrsf.Perform(aShape, true, true);
        if (!theBRepTrsf.IsDone())
        {
          Standard_SStream aSS;
          aSS << "Dimension PMI " << theEntryDimension << " is not scaled.";
          Message::SendWarning(aSS.str().c_str());
        }
        else
        {
          TopoDS_Shape    aScaledShape = theBRepTrsf.Shape();
          TNaming_Builder aBuilder(aL);
          aBuilder.Generated(aShape, aScaledShape);
        }
      }
    }
  }
}

//=======================================================================
// function : shouldRescaleAndCheckRefLabels
// purpose  : Checks if all PMI reference shapes belong to the assembly
//           graph. Returns true if at least one reference shape belongs
//           to the assembly graph.
//=======================================================================

static bool shouldRescaleAndCheckRefLabels(const occ::handle<TDF_Data>&              theData,
                                           const NCollection_Sequence<TDF_Label>&    theRefLabels,
                                           const occ::handle<XCAFDoc_AssemblyGraph>& theGraph,
                                           bool&                                     theAllInG)
{
  theAllInG           = true;
  bool aShouldRescale = false;
  for (NCollection_Sequence<TDF_Label>::Iterator anIt1(theRefLabels); anIt1.More(); anIt1.Next())
  {
    const TDF_Label& aL = anIt1.Value();
    if (theGraph->GetNodes().Contains(aL))
    {
      aShouldRescale = true;
    }
    else
    {
      occ::handle<XCAFDoc_AssemblyItemRef> anItemRefAttr;
      if (!aL.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), anItemRefAttr))
      {
        theAllInG = false;
        continue;
      }
      const XCAFDoc_AssemblyItemId& anItemId = anItemRefAttr->GetItem();
      if (anItemId.IsNull())
      {
        theAllInG = false;
        continue;
      }
      TDF_Label aLRef;
      TDF_Tool::Label(theData, anItemId.GetPath().Last(), aLRef, false);
      if (aLRef.IsNull() || !theGraph->GetNodes().Contains(aLRef))
      {
        theAllInG = false;
        continue;
      }
      aShouldRescale = true;
    }
  }
  return aShouldRescale;
}

//=================================================================================================

void XCAFDoc_Editor::GetChildShapeLabels(const TDF_Label&            theLabel,
                                         NCollection_Map<TDF_Label>& theRelatedLabels)
{
  if (theLabel.IsNull() || !XCAFDoc_ShapeTool::IsShape(theLabel))
  {
    return;
  }
  if (!theRelatedLabels.Add(theLabel))
  {
    return; // Label already processed
  }
  if (XCAFDoc_ShapeTool::IsAssembly(theLabel) || XCAFDoc_ShapeTool::IsSimpleShape(theLabel))
  {
    for (TDF_ChildIterator aChildIter(theLabel); aChildIter.More(); aChildIter.Next())
    {
      const TDF_Label& aChildLabel = aChildIter.Value();
      GetChildShapeLabels(aChildLabel, theRelatedLabels);
    }
  }
  if (XCAFDoc_ShapeTool::IsReference(theLabel))
  {
    TDF_Label aRefLabel;
    XCAFDoc_ShapeTool::GetReferredShape(theLabel, aRefLabel);
    GetChildShapeLabels(aRefLabel, theRelatedLabels);
  }
}

//=================================================================================================

void XCAFDoc_Editor::GetParentShapeLabels(const TDF_Label&            theLabel,
                                          NCollection_Map<TDF_Label>& theRelatedLabels)
{
  if (theLabel.IsNull() || !XCAFDoc_ShapeTool::IsShape(theLabel))
  {
    return;
  }
  if (!theRelatedLabels.Add(theLabel))
  {
    return; // Label already processed
  }
  if (XCAFDoc_ShapeTool::IsSubShape(theLabel) || XCAFDoc_ShapeTool::IsComponent(theLabel))
  {
    TDF_Label aFatherLabel = theLabel.Father();
    GetParentShapeLabels(aFatherLabel, theRelatedLabels);
  }
  else
  {
    NCollection_Sequence<TDF_Label> aUsers;
    XCAFDoc_ShapeTool::GetUsers(theLabel, aUsers);
    if (!aUsers.IsEmpty())
    {
      for (NCollection_Sequence<TDF_Label>::Iterator aUserIter(aUsers); aUserIter.More();
           aUserIter.Next())
      {
        const TDF_Label& aUserLabel = aUserIter.Value();
        GetParentShapeLabels(aUserLabel, theRelatedLabels);
      }
    }
  }
}

//=================================================================================================

bool XCAFDoc_Editor::FilterShapeTree(const occ::handle<XCAFDoc_ShapeTool>& theShapeTool,
                                     const NCollection_Map<TDF_Label>&     theLabelsToKeep)
{
  if (theLabelsToKeep.IsEmpty())
  {
    return false;
  }
  occ::handle<NCollection_BaseAllocator> anAllocator = new NCollection_IncAllocator();
  NCollection_Map<TDF_Label>             aLabelsToKeep(theLabelsToKeep.Size(), anAllocator);
  for (NCollection_Map<TDF_Label>::Iterator aLabelIter(theLabelsToKeep); aLabelIter.More();
       aLabelIter.Next())
  {
    GetChildShapeLabels(aLabelIter.Key(), aLabelsToKeep);
  }
  NCollection_Map<TDF_Label> aInternalLabels(1, anAllocator);
  for (NCollection_Map<TDF_Label>::Iterator aLabelIter(theLabelsToKeep); aLabelIter.More();
       aLabelIter.Next())
  {
    GetParentShapeLabels(aLabelIter.Key(), aInternalLabels);
    NCollection_MapAlgo::Unite(aLabelsToKeep, aInternalLabels);
    aInternalLabels.Clear(false);
  }
  for (TDF_ChildIterator aLabelIter(theShapeTool->Label(), true); aLabelIter.More();
       aLabelIter.Next())
  {
    const TDF_Label& aLabel = aLabelIter.Value();
    if (!aLabelsToKeep.Contains(aLabel))
    {
      aLabel.ForgetAllAttributes(false);
    }
  }
  theShapeTool->UpdateAssemblies();
  return true;
}

//=======================================================================
// function : RescaleGeometry
// purpose  : Applies geometrical scale to all assembly parts, component
//           locations and related attributes
//=======================================================================
bool XCAFDoc_Editor::RescaleGeometry(const TDF_Label& theLabel,
                                     const double     theScaleFactor,
                                     const bool       theForceIfNotRoot)
{
  if (theLabel.IsNull())
  {
    Message::SendFail("Null label.");
    return false;
  }

  if (std::abs(theScaleFactor) <= gp::Resolution())
  {
    Message::SendFail("Scale factor is too small.");
    return false;
  }

  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theLabel);
  if (aShapeTool.IsNull())
  {
    Message::SendFail("Couldn't find XCAFDoc_ShapeTool attribute.");
    return false;
  }

  if (!theForceIfNotRoot && aShapeTool->Label() != theLabel)
  {
    NCollection_Sequence<TDF_Label> aFreeLabels;
    aShapeTool->GetFreeShapes(aFreeLabels);
    bool aFound = false;
    for (NCollection_Sequence<TDF_Label>::Iterator anIt(aFreeLabels); anIt.More(); anIt.Next())
    {
      if (theLabel == anIt.Value())
      {
        aFound = true;
        break;
      }
    }
    if (!aFound)
    {
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(theLabel, anEntry);
      Standard_SStream aSS;
      aSS << "Label " << anEntry << " is not a root. Set ForceIfNotRoot true to rescale forcibly.";
      Message::SendFail(aSS.str().c_str());
      return false;
    }
  }

  occ::handle<XCAFDoc_AssemblyGraph> aG = new XCAFDoc_AssemblyGraph(theLabel);
  if (aG.IsNull())
  {
    Message::SendFail("Couldn't create assembly graph.");
    return false;
  }

  bool anIsDone = true;

  gp_Trsf aTrsf;
  aTrsf.SetScaleFactor(theScaleFactor);
  BRepBuilderAPI_Transform aBRepTrsf(aTrsf);

  XCAFDoc_AssemblyTool::Traverse(
    aG,
    [](const occ::handle<XCAFDoc_AssemblyGraph>& theGraph, const int theNode) -> bool {
      const XCAFDoc_AssemblyGraph::NodeType aNodeType = theGraph->GetNodeType(theNode);
      return (aNodeType == XCAFDoc_AssemblyGraph::NodeType_Part)
             || (aNodeType == XCAFDoc_AssemblyGraph::NodeType_Occurrence);
    },
    [&](const occ::handle<XCAFDoc_AssemblyGraph>& theGraph, const int theNode) -> bool {
      const TDF_Label&                      aLabel    = theGraph->GetNode(theNode);
      const XCAFDoc_AssemblyGraph::NodeType aNodeType = theGraph->GetNodeType(theNode);

      if (aNodeType == XCAFDoc_AssemblyGraph::NodeType_Part)
      {
        const TopoDS_Shape aShape = aShapeTool->GetShape(aLabel);
        aBRepTrsf.Perform(aShape, true, true);
        if (!aBRepTrsf.IsDone())
        {
          Standard_SStream        aSS;
          TCollection_AsciiString anEntry;
          TDF_Tool::Entry(aLabel, anEntry);
          aSS << "Shape " << anEntry << " is not scaled!";
          Message::SendFail(aSS.str().c_str());
          anIsDone = false;
          return false;
        }
        TopoDS_Shape aScaledShape = aBRepTrsf.Shape();
        aShapeTool->SetShape(aLabel, aScaledShape);

        // Update sub-shapes
        NCollection_Sequence<TDF_Label> aSubshapes;
        aShapeTool->GetSubShapes(aLabel, aSubshapes);
        for (NCollection_Sequence<TDF_Label>::Iterator anItSs(aSubshapes); anItSs.More();
             anItSs.Next())
        {
          const TDF_Label&   aLSs = anItSs.Value();
          const TopoDS_Shape aSs  = aShapeTool->GetShape(aLSs);
          const TopoDS_Shape aSs1 = aBRepTrsf.ModifiedShape(aSs);
          aShapeTool->SetShape(aLSs, aSs1);
        }

        occ::handle<XCAFDoc_Area> aArea;
        if (aLabel.FindAttribute(XCAFDoc_Area::GetID(), aArea))
        {
          aArea->Set(aArea->Get() * theScaleFactor * theScaleFactor);
        }

        occ::handle<XCAFDoc_Centroid> aCentroid;
        if (aLabel.FindAttribute(XCAFDoc_Centroid::GetID(), aCentroid))
        {
          aCentroid->Set(aCentroid->Get().XYZ() * theScaleFactor);
        }

        occ::handle<XCAFDoc_Volume> aVolume;
        if (aLabel.FindAttribute(XCAFDoc_Volume::GetID(), aVolume))
        {
          aVolume->Set(aVolume->Get() * theScaleFactor * theScaleFactor * theScaleFactor);
        }
      }
      else if (aNodeType == XCAFDoc_AssemblyGraph::NodeType_Occurrence)
      {
        TopLoc_Location aLoc  = aShapeTool->GetLocation(aLabel);
        gp_Trsf         aTrsf = aLoc.Transformation();
        aTrsf.SetTranslationPart(aTrsf.TranslationPart() * theScaleFactor);
        XCAFDoc_Location::Set(aLabel, aTrsf);
      }

      return true;
    });

  if (!anIsDone)
  {
    return false;
  }

  aShapeTool->UpdateAssemblies();

  occ::handle<XCAFDoc_DimTolTool> aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(theLabel);
  if (!aDimTolTool.IsNull())
  {
    NCollection_Sequence<TDF_Label> aDimensions;
    aDimTolTool->GetDimensionLabels(aDimensions);
    for (NCollection_Sequence<TDF_Label>::Iterator anItD(aDimensions); anItD.More(); anItD.Next())
    {
      const TDF_Label& aDimension = anItD.Value();

      TCollection_AsciiString anEntryDimension;
      TDF_Tool::Entry(aDimension, anEntryDimension);

      occ::handle<XCAFDoc_Dimension> aDimAttr;
      if (aDimension.FindAttribute(XCAFDoc_Dimension::GetID(), aDimAttr))
      {
        bool                            aShouldRescale = false;
        bool                            aFirstLInG     = true;
        bool                            aSecondLInG    = true;
        NCollection_Sequence<TDF_Label> aShapeLFirst, aShapeLSecond;
        bool aHasShapeRefs = aDimTolTool->GetRefShapeLabel(aDimension, aShapeLFirst, aShapeLSecond);
        if (aHasShapeRefs)
        {
          aShouldRescale =
            shouldRescaleAndCheckRefLabels(theLabel.Data(), aShapeLFirst, aG, aFirstLInG)
            || shouldRescaleAndCheckRefLabels(theLabel.Data(), aShapeLSecond, aG, aSecondLInG);
        }

        if (!aShouldRescale)
        {
          Standard_SStream aSS;
          aSS << "Dimension PMI " << anEntryDimension << " is not scaled!";
          Message::SendWarning(aSS.str().c_str());
          continue;
        }

        occ::handle<XCAFDimTolObjects_DimensionObject> aDimObj = aDimAttr->GetObject();

        if (aDimObj->HasTextPoint())
        {
          aDimObj->SetPointTextAttach(aDimObj->GetPointTextAttach().XYZ() * theScaleFactor);
        }

        if (aDimObj->HasPoint())
        {
          aDimObj->SetPoint(aDimObj->GetPoint().XYZ() * theScaleFactor);
        }

        if (aDimObj->HasPoint2())
        {
          aDimObj->SetPoint2(aDimObj->GetPoint2().XYZ() * theScaleFactor);
        }

        if (aDimObj->HasPlane())
        {
          gp_Ax2 aPln = aDimObj->GetPlane();
          aPln.SetLocation(aPln.Location().XYZ() * theScaleFactor);
          aDimObj->SetPlane(aPln);
        }

        occ::handle<NCollection_HArray1<double>> aValues = aDimObj->GetValues();
        if (!aValues.IsNull())
        {
          if (!aFirstLInG || !aSecondLInG)
          {
            Standard_SStream aSS;
            aSS << "Dimension PMI " << anEntryDimension
                << " base shapes do not belong to the rescaled assembly!";
            Message::SendWarning(aSS.str().c_str());
            continue;
          }
          bool                        aRescaleOtherValues = false;
          NCollection_Array1<double>& anArray             = aValues->ChangeArray1();
          switch (aDimObj->GetType())
          {
            case XCAFDimTolObjects_DimensionType_Location_None:
            case XCAFDimTolObjects_DimensionType_Location_CurvedDistance: {
              Standard_SStream aSS;
              aSS << "Dimension PMI " << anEntryDimension << " is not scaled.";
              Message::SendWarning(aSS.str().c_str());
            }
            break;
            case XCAFDimTolObjects_DimensionType_Location_LinearDistance:
            case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToOuter:
            case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToInner:
            case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToCenter:
            case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToOuter:
            case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToInner:
            case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToCenter:
            case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToOuter:
            case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToInner:
              anArray.ChangeFirst() *= theScaleFactor;
              aRescaleOtherValues = true;
              break;
            case XCAFDimTolObjects_DimensionType_Location_Angular:
            case XCAFDimTolObjects_DimensionType_Size_Angular:
              break;
            case XCAFDimTolObjects_DimensionType_Location_Oriented:
            case XCAFDimTolObjects_DimensionType_Location_WithPath: {
              Standard_SStream aSS;
              aSS << "Dimension PMI " << anEntryDimension << " is not scaled.";
              Message::SendWarning(aSS.str().c_str());
            }
            break;
            case XCAFDimTolObjects_DimensionType_Size_CurveLength:
            case XCAFDimTolObjects_DimensionType_Size_Diameter:
            case XCAFDimTolObjects_DimensionType_Size_SphericalDiameter:
            case XCAFDimTolObjects_DimensionType_Size_Radius:
            case XCAFDimTolObjects_DimensionType_Size_SphericalRadius:
            case XCAFDimTolObjects_DimensionType_Size_ToroidalMinorDiameter:
            case XCAFDimTolObjects_DimensionType_Size_ToroidalMajorDiameter:
            case XCAFDimTolObjects_DimensionType_Size_ToroidalMinorRadius:
            case XCAFDimTolObjects_DimensionType_Size_ToroidalMajorRadius:
            case XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorDiameter:
            case XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorDiameter:
            case XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorRadius:
            case XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorRadius:
            case XCAFDimTolObjects_DimensionType_Size_Thickness:
            case XCAFDimTolObjects_DimensionType_Size_WithPath:
              anArray.ChangeFirst() *= theScaleFactor;
              aRescaleOtherValues = true;
              break;
            case XCAFDimTolObjects_DimensionType_CommonLabel:
            case XCAFDimTolObjects_DimensionType_DimensionPresentation: {
              Standard_SStream aSS;
              aSS << "Dimension PMI " << anEntryDimension << " is not scaled.";
              Message::SendWarning(aSS.str().c_str());
            }
            break;
            default: {
              Standard_SStream aSS;
              aSS << "Dimension PMI of unsupported type " << anEntryDimension << " is not scaled.";
              Message::SendWarning(aSS.str().c_str());
            }
          }
          rescaleDimensionRefLabels(aShapeLFirst, aBRepTrsf, aG, anEntryDimension);
          rescaleDimensionRefLabels(aShapeLSecond, aBRepTrsf, aG, anEntryDimension);
          if (aRescaleOtherValues)
          {
            for (int i = anArray.Lower() + 1; i <= anArray.Upper(); ++i)
              anArray.ChangeValue(i) *= theScaleFactor;

            occ::handle<TCollection_HAsciiString> aName = aDimObj->GetSemanticName();
            if (!aName.IsNull())
            {
              aName->AssignCat(" (Rescaled to ");
              Standard_SStream aSS;
              aSS << aValues->First();
              aName->AssignCat(aSS.str().c_str());
              aName->AssignCat(")");
            }
          }
        }
        else
        {
          Standard_SStream aSS;
          aSS << "Dimension PMI values " << anEntryDimension << " are not scaled.";
          Message::SendWarning(aSS.str().c_str());
        }

        aDimAttr->SetObject(aDimObj);
      }
    }

    NCollection_Sequence<TDF_Label> aDatums;
    aDimTolTool->GetDatumLabels(aDatums);
    for (NCollection_Sequence<TDF_Label>::Iterator anIt(aDatums); anIt.More(); anIt.Next())
    {
      const TDF_Label& aDatum = anIt.Value();

      TCollection_AsciiString anEntryDatum;
      TDF_Tool::Entry(aDatum, anEntryDatum);

      occ::handle<XCAFDoc_Datum> aDatumAttr;
      if (aDatum.FindAttribute(XCAFDoc_Datum::GetID(), aDatumAttr))
      {
        occ::handle<XCAFDimTolObjects_DatumObject> aDatumObj = aDatumAttr->GetObject();

        if (aDatumObj->HasDatumTargetParams())
        {
          gp_Ax2 anAxis = aDatumObj->GetDatumTargetAxis();
          anAxis.SetLocation(anAxis.Location().XYZ() * theScaleFactor);
          aDatumObj->SetDatumTargetAxis(anAxis);
          // TODO: Should we rescale target length and width?
          Standard_SStream aSS;
          aSS << "Datum PMI target length and width " << anEntryDatum << " are not scaled.";
          Message::SendWarning(aSS.str().c_str());
          // aDatumObj->SetDatumTargetLength(aDatumObj->GetDatumTargetLength() * theScaleFactor);
          // aDatumObj->SetDatumTargetWidth(aDatumObj->GetDatumTargetWidth() * theScaleFactor);
        }

        if (aDatumObj->HasPointText())
        {
          aDatumObj->SetPointTextAttach(aDatumObj->GetPointTextAttach().XYZ() * theScaleFactor);
        }

        if (aDatumObj->HasPoint())
        {
          aDatumObj->SetPoint(aDatumObj->GetPoint().XYZ() * theScaleFactor);
        }

        if (aDatumObj->HasPlane())
        {
          gp_Ax2 aPln = aDatumObj->GetPlane();
          aPln.SetLocation(aPln.Location().XYZ() * theScaleFactor);
          aDatumObj->SetPlane(aPln);
        }

        aDatumAttr->SetObject(aDatumObj);
      }
    }

    NCollection_Sequence<TDF_Label> aDimTols;
    aDimTolTool->GetDimTolLabels(aDimTols);
    for (NCollection_Sequence<TDF_Label>::Iterator anIt(aDimTols); anIt.More(); anIt.Next())
    {
      const TDF_Label& aDimTol = anIt.Value();

      TCollection_AsciiString anEntryDimTol;
      TDF_Tool::Entry(aDimTol, anEntryDimTol);

      occ::handle<XCAFDoc_DimTol> aDimTolAttr;
      if (aDimTol.FindAttribute(XCAFDoc_DimTol::GetID(), aDimTolAttr))
      {
        Standard_SStream aSS;
        aSS << "DimTol PMI " << anEntryDimTol << " is not scaled.";
        Message::SendWarning(aSS.str().c_str());
      }
    }
  }

  occ::handle<XCAFDoc_NotesTool> aNotesTool = XCAFDoc_DocumentTool::NotesTool(theLabel);
  if (!aNotesTool.IsNull())
  {
    NCollection_Sequence<TDF_Label> aNotes;
    aNotesTool->GetNotes(aNotes);
    for (NCollection_Sequence<TDF_Label>::Iterator anIt(aNotes); anIt.More(); anIt.Next())
    {
      const TDF_Label& aNote = anIt.Value();

      occ::handle<XCAFDoc_Note> aNoteAttr;
      if (aNote.FindAttribute(XCAFDoc_NoteComment::GetID(), aNoteAttr)
          || aNote.FindAttribute(XCAFDoc_NoteBalloon::GetID(), aNoteAttr)
          || aNote.FindAttribute(XCAFDoc_NoteBinData::GetID(), aNoteAttr))
      {
        occ::handle<XCAFNoteObjects_NoteObject> aNoteObj = aNoteAttr->GetObject();

        if (aNoteObj->HasPointText())
        {
          aNoteObj->SetPointText(aNoteObj->GetPointText().XYZ() * theScaleFactor);
        }

        if (aNoteObj->HasPoint())
        {
          aNoteObj->SetPoint(aNoteObj->GetPoint().XYZ() * theScaleFactor);
        }

        if (aNoteObj->HasPlane())
        {
          gp_Ax2 aPln = aNoteObj->GetPlane();
          aPln.SetLocation(aPln.Location().XYZ() * theScaleFactor);
          aNoteObj->SetPlane(aPln);
        }

        aNoteAttr->SetObject(aNoteObj);
      }
    }
  }

  return anIsDone;
}
