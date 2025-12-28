// Created on: 2000-08-17
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

#include <IGESCAFControl.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESGraph_Color.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESBasic_Name.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Sequence.hxx>
#include <TDocStd_Document.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_TransientListBinder.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_LengthUnit.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>
#include <XCAFPrs_Style.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <XSControl_WorkSession.hxx>
#include <UnitsMethods.hxx>

namespace
{
typedef NCollection_DataMap<TopoDS_Shape, TCollection_ExtendedString> DataMapOfShapeNames;

void CollectShapeNames(const TDF_Label&                  theLabel,
                       const TopLoc_Location&            theLocation,
                       const occ::handle<TDataStd_Name>& thePrevName,
                       DataMapOfShapeNames&              theMapOfShapeNames)
{
  bool      hasReferredShape = false;
  bool      hasComponents    = false;
  TDF_Label aReferredLabel;

  occ::handle<TDataStd_Name> aName;
  theLabel.FindAttribute(TDataStd_Name::GetID(), aName);

  if (XCAFDoc_ShapeTool::GetReferredShape(theLabel, aReferredLabel))
  {
    TopLoc_Location aSubLocation = theLocation.Multiplied(XCAFDoc_ShapeTool::GetLocation(theLabel));
    CollectShapeNames(aReferredLabel, aSubLocation, aName, theMapOfShapeNames);
    hasReferredShape = true;
  }

  NCollection_Sequence<TDF_Label> aSeq;
  if (XCAFDoc_ShapeTool::GetComponents(theLabel, aSeq))
  {
    for (int anIter = 1; anIter <= aSeq.Length(); anIter++)
    {
      CollectShapeNames(aSeq.Value(anIter), theLocation, aName, theMapOfShapeNames);
    }
    hasComponents = true;
  }

  aSeq.Clear();
  if (XCAFDoc_ShapeTool::GetSubShapes(theLabel, aSeq))
  {
    for (int anIter = 1; anIter <= aSeq.Length(); anIter++)
    {
      TopoDS_Shape aShape;
      if (!XCAFDoc_ShapeTool::GetShape(aSeq.Value(anIter), aShape))
        continue;
      if (!aSeq.Value(anIter).FindAttribute(TDataStd_Name::GetID(), aName))
        continue;
      theMapOfShapeNames.Bind(aShape, aName->Get());
    }
  }

  if (!hasReferredShape && !hasComponents && !thePrevName.IsNull())
  {
    TopoDS_Shape aShape;
    if (!XCAFDoc_ShapeTool::GetShape(theLabel, aShape))
      return;
    aShape.Move(theLocation, false);
    theMapOfShapeNames.Bind(aShape, thePrevName->Get());
  }
}
} // namespace

//=================================================================================================

IGESCAFControl_Writer::IGESCAFControl_Writer()
    : myColorMode(true),
      myNameMode(true),
      myLayerMode(true)
{
}

//=================================================================================================

IGESCAFControl_Writer::IGESCAFControl_Writer(const occ::handle<XSControl_WorkSession>& WS,
                                             const bool /*scratch*/)
{
  // this code does things in a wrong way, it should be vice-versa
  WS->SetModel(Model());
  WS->SetMapWriter(TransferProcess());
  myColorMode = true;
  myNameMode  = true;
  myLayerMode = true;

  //  SetWS (WS,scratch); // this should be the only required command here
}

//=================================================================================================

IGESCAFControl_Writer::IGESCAFControl_Writer(const occ::handle<XSControl_WorkSession>& WS,
                                             const char*                               theUnit)
    : IGESControl_Writer(theUnit)
{

  WS->SetModel(Model());
  WS->SetMapWriter(TransferProcess());
  myColorMode = true;
  myNameMode  = true;
  myLayerMode = true;
}

//=================================================================================================

bool IGESCAFControl_Writer::Transfer(const occ::handle<TDocStd_Document>& doc,
                                     const Message_ProgressRange&         theProgress)
{
  // translate free top-level shapes of the DECAF document
  occ::handle<XCAFDoc_ShapeTool> STool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
  if (STool.IsNull())
    return false;

  NCollection_Sequence<TDF_Label> labels;
  STool->GetFreeShapes(labels);
  return Transfer(labels, theProgress);
}

//=================================================================================================

bool IGESCAFControl_Writer::Transfer(const TDF_Label&             label,
                                     const Message_ProgressRange& theProgress)
{
  NCollection_Sequence<TDF_Label> labels;
  labels.Append(label);
  return Transfer(labels, theProgress);
}

//=================================================================================================

bool IGESCAFControl_Writer::Transfer(const NCollection_Sequence<TDF_Label>& labels,
                                     const Message_ProgressRange&           theProgress)
{
  if (labels.Length() <= 0)
    return false;
  prepareUnit(labels.First()); // set local length unit to the model
  Message_ProgressScope aPS(theProgress, "Labels", labels.Length());
  for (int i = 1; i <= labels.Length() && aPS.More(); i++)
  {
    TopoDS_Shape shape = XCAFDoc_ShapeTool::GetShape(labels.Value(i));
    if (!shape.IsNull())
      AddShape(shape, aPS.Next());
    //      IGESControl_Writer::Transfer ( shape );
  }

  // write colors
  if (GetColorMode())
    WriteAttributes(labels);

  // write layers
  if (GetLayerMode())
    WriteLayers(labels);

  // write names
  if (GetNameMode())
    WriteNames(labels);

  // refresh graph
  //  WS()->ComputeGraph ( true );
  ComputeModel();

  return true;
}

//=================================================================================================

bool IGESCAFControl_Writer::Perform(const occ::handle<TDocStd_Document>& doc,
                                    const char*                          filename,
                                    const Message_ProgressRange&         theProgress)
{
  if (!Transfer(doc, theProgress))
    return false;
  return Write(filename) == IFSelect_RetDone;
}

//=================================================================================================

bool IGESCAFControl_Writer::Perform(const occ::handle<TDocStd_Document>& doc,
                                    const TCollection_AsciiString&       filename,
                                    const Message_ProgressRange&         theProgress)
{
  if (!Transfer(doc, theProgress))
    return false;
  return Write(filename.ToCString()) == IFSelect_RetDone;
}

//=================================================================================================

bool IGESCAFControl_Writer::WriteAttributes(const NCollection_Sequence<TDF_Label>& labels)
{
  // Iterate on labels
  if (labels.Length() <= 0)
    return false;
  for (int i = 1; i <= labels.Length(); i++)
  {
    TDF_Label L = labels.Value(i);

    // collect color settings
    NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher> settings;
    TopLoc_Location                                                                  loc;
    XCAFPrs::CollectStyleSettings(L, loc, settings);
    if (settings.Extent() <= 0)
      continue;

    // get a target shape and try to find corresponding context
    // (all the colors set under that label will be put into that context)
    TopoDS_Shape S;
    if (!XCAFDoc_ShapeTool::GetShape(L, S))
      continue;

    // iterate on subshapes and create IGES styles
    NCollection_DataMap<XCAFPrs_Style, occ::handle<Standard_Transient>> colors;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>              Map;
    const XCAFPrs_Style                                                 inherit;
    MakeColors(S, settings, colors, Map, inherit);
  }

  return true;
}

//=================================================================================================

void IGESCAFControl_Writer::MakeColors(
  const TopoDS_Shape&                                                                     S,
  const NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher>& settings,
  NCollection_DataMap<XCAFPrs_Style, occ::handle<Standard_Transient>>&                    colors,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                                 Map,
  const XCAFPrs_Style&                                                                    inherit)
{
  // skip already processed shapes
  if (!Map.Add(S))
    return;

  // check if shape has its own style (or inherits from ancestor)
  XCAFPrs_Style style = inherit;
  if (settings.Contains(S))
  {
    const XCAFPrs_Style& own = settings.FindFromKey(S);
    if (own.IsSetColorCurv())
      style.SetColorCurv(own.GetColorCurv());
    if (own.IsSetColorSurf())
      style.SetColorSurf(own.GetColorSurf());
    style.SetMaterial(own.Material());
  }

  // analyze whether current entity should get a color
  bool           hasColor = false;
  Quantity_Color col;
  if (S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_SOLID)
  {
    if (style.IsSetColorSurf())
    {
      hasColor = true;
      col      = style.GetColorSurf();
    }
    else if (!style.Material().IsNull() && !style.Material()->IsEmpty())
    {
      hasColor = true;
      col      = style.Material()->BaseColor().GetRGB();
    }
  }
  else if (S.ShapeType() == TopAbs_EDGE || S.ShapeType() == TopAbs_WIRE)
  {
    if (style.IsSetColorCurv())
    {
      hasColor = true;
      col      = style.GetColorCurv();
    }
  }

  // if color has to be assigned, try to do this
  if (hasColor)
  {
    occ::handle<IGESGraph_Color> colent;
    int                          rank = IGESCAFControl::EncodeColor(col);
    if (!rank)
    {
      XCAFPrs_Style c; // style used as key in the map
      c.SetColorSurf(col);
      if (colors.IsBound(c))
      {
        colent = occ::down_cast<IGESGraph_Color>(colors.Find(c));
      }
      else
      {
        occ::handle<TCollection_HAsciiString> str =
          new TCollection_HAsciiString(col.StringName(col.Name()));
        colent = new IGESGraph_Color;
        NCollection_Vec3<double> aColor_sRGB;
        col.Values(aColor_sRGB.r(), aColor_sRGB.g(), aColor_sRGB.b(), Quantity_TOC_sRGB);
        colent->Init(aColor_sRGB.r() * 100., aColor_sRGB.g() * 100., aColor_sRGB.b() * 100., str);
        AddEntity(colent);
        colors.Bind(c, colent);
      }
    }
    occ::handle<Transfer_FinderProcess>   FP = TransferProcess();
    occ::handle<IGESData_IGESEntity>      ent;
    occ::handle<TransferBRep_ShapeMapper> mapper = TransferBRep::ShapeMapper(FP, S);
    occ::handle<TransferBRep_ShapeMapper> aNoLocMapper =
      TransferBRep::ShapeMapper(FP, S.Located(TopLoc_Location()));
    if (FP->FindTypedTransient(mapper, STANDARD_TYPE(IGESData_IGESEntity), ent)
        || FP->FindTypedTransient(aNoLocMapper, STANDARD_TYPE(IGESData_IGESEntity), ent))
    {
      ent->InitColor(colent, rank);
      occ::handle<IGESSolid_Face> ent_f = occ::down_cast<IGESSolid_Face>(ent);
      if (!ent_f.IsNull())
      {
        if (!ent_f->Surface().IsNull())
          ent_f->Surface()->InitColor(colent, rank);
      }
    }
    else
    {
      // may be S was split during shape process
      occ::handle<Transfer_Binder> bnd = FP->Find(mapper);
      if (!bnd.IsNull())
      {
        occ::handle<Transfer_TransientListBinder> TransientListBinder =
          // occ::down_cast<Transfer_TransientListBinder>( bnd->Next(true) );
          occ::down_cast<Transfer_TransientListBinder>(bnd);
        int i = 0, nb = 0;
        if (!TransientListBinder.IsNull())
        {
          nb = TransientListBinder->NbTransients();
          for (i = 1; i <= nb; i++)
          {
            occ::handle<Standard_Transient> t = TransientListBinder->Transient(i);
            ent                               = occ::down_cast<IGESData_IGESEntity>(t);
            if (!ent.IsNull())
            {
              ent->InitColor(colent, rank);
              occ::handle<IGESSolid_Face> ent_f = occ::down_cast<IGESSolid_Face>(ent);
              if (!ent_f.IsNull())
              {
                if (!ent_f->Surface().IsNull())
                  ent_f->Surface()->InitColor(colent, rank);
              }
            }
          }
        }
        /* // alternative: consider recursive mapping S -> compound -> entities
        else {
          TopoDS_Shape comp = TransferBRep::ShapeResult(bnd);
          if ( ! comp.IsNull() && comp.ShapeType() < S.ShapeType() )
            for ( TopoDS_Iterator it(comp); it.More(); it.Next() ) {
              MakeColors ( it.Value(), settings, colors, Map, style );
            }
        }
        */
      }
    }
  }

  // iterate on subshapes (except vertices :)
  if (S.ShapeType() == TopAbs_EDGE)
    return;
  for (TopoDS_Iterator it(S); it.More(); it.Next())
  {
    MakeColors(it.Value(), settings, colors, Map, style);
  }
}

static void AttachLayer(const occ::handle<Transfer_FinderProcess>& FP,
                        const occ::handle<XCAFDoc_LayerTool>&      LTool,
                        const TopoDS_Shape&                        aSh,
                        const int                                  localIntName)
{

  NCollection_Sequence<TopoDS_Shape> shseq;
  if (aSh.ShapeType() == TopAbs_COMPOUND)
  {
    TopoDS_Iterator aShIt(aSh);
    for (; aShIt.More(); aShIt.Next())
    {
      const TopoDS_Shape&                                            newSh = aShIt.Value();
      occ::handle<NCollection_HSequence<TCollection_ExtendedString>> shLayers =
        new NCollection_HSequence<TCollection_ExtendedString>;
      if (!LTool->GetLayers(newSh, shLayers) || newSh.ShapeType() == TopAbs_COMPOUND)
        AttachLayer(FP, LTool, newSh, localIntName);
    }
    return;
  }
  else if (aSh.ShapeType() == TopAbs_SOLID || aSh.ShapeType() == TopAbs_SHELL)
  {
    for (TopExp_Explorer exp(aSh, TopAbs_FACE); exp.More(); exp.Next())
    {
      TopoDS_Face entSh = TopoDS::Face(exp.Current());
      shseq.Append(entSh);
    }
  }
  else
  {
    shseq.Append(aSh);
  }

  for (int i = 1; i <= shseq.Length(); i++)
  {
    const TopoDS_Shape&                   localShape = shseq.Value(i);
    occ::handle<IGESData_IGESEntity>      Igesent;
    occ::handle<TransferBRep_ShapeMapper> mapper = TransferBRep::ShapeMapper(FP, localShape);
    if (FP->FindTypedTransient(mapper, STANDARD_TYPE(IGESData_IGESEntity), Igesent))
    {
      Igesent->InitLevel(0, localIntName);
    }
#ifdef OCCT_DEBUG
    else
      std::cout << "Warning: Can't find entity for shape in mapper" << std::endl;
#endif
  }
}

static void MakeLayers(const occ::handle<Transfer_FinderProcess>& FP,
                       const occ::handle<XCAFDoc_ShapeTool>&      STool,
                       const occ::handle<XCAFDoc_LayerTool>&      LTool,
                       const NCollection_Sequence<TDF_Label>&     aShapeLabels,
                       const int                                  localIntName)
{
  for (int j = 1; j <= aShapeLabels.Length(); j++)
  {
    TDF_Label    aShapeLabel = aShapeLabels.Value(j);
    TopoDS_Shape aSh;
    if (!STool->GetShape(aShapeLabel, aSh))
      continue;
    AttachLayer(FP, LTool, aSh, localIntName);
  }
}

//=================================================================================================

bool IGESCAFControl_Writer::WriteLayers(const NCollection_Sequence<TDF_Label>& labels)
{
  if (labels.Length() <= 0)
    return false;
  occ::handle<XCAFDoc_ShapeTool> STool = XCAFDoc_DocumentTool::ShapeTool(labels(1));
  if (STool.IsNull())
    return false;
  occ::handle<XCAFDoc_LayerTool> LTool = XCAFDoc_DocumentTool::LayerTool(labels(1));
  if (LTool.IsNull())
    return false;

  int                             globalIntName = 0;
  NCollection_Sequence<TDF_Label> aLayerLabels;
  LTool->GetLayerLabels(aLayerLabels);

  occ::handle<Transfer_FinderProcess> FP = TransferProcess();
  for (int i = 1; i <= aLayerLabels.Length(); i++)
  {
    TDF_Label aOneLayerL = aLayerLabels.Value(i);
    if (aOneLayerL.IsNull())
      continue;
    TCollection_ExtendedString localName;
    LTool->GetLayer(aOneLayerL, localName);
    int                     localIntName = 0;
    TCollection_AsciiString asciiName(localName, '?');
    if (asciiName.IsIntegerValue())
    {
      localIntName = asciiName.IntegerValue();
      if (globalIntName < localIntName)
        globalIntName = localIntName;

      NCollection_Sequence<TDF_Label> aShapeLabels;
      LTool->GetShapesOfLayer(aOneLayerL, aShapeLabels);
      if (aShapeLabels.Length() <= 0)
        continue;
      MakeLayers(FP, STool, LTool, aShapeLabels, localIntName);
    }
  }

  for (int i1 = 1; i1 <= aLayerLabels.Length(); i1++)
  {
    TDF_Label aOneLayerL = aLayerLabels.Value(i1);
    if (aOneLayerL.IsNull())
      continue;
    TCollection_ExtendedString localName;
    LTool->GetLayer(aOneLayerL, localName);
    int                     localIntName = 0;
    TCollection_AsciiString asciiName(localName, '?');
    if (asciiName.IsIntegerValue())
      continue;
    NCollection_Sequence<TDF_Label> aShapeLabels;
    LTool->GetShapesOfLayer(aOneLayerL, aShapeLabels);
    if (aShapeLabels.Length() <= 0)
      continue;
    localIntName = ++globalIntName;
    MakeLayers(FP, STool, LTool, aShapeLabels, localIntName);
  }

  return true;
}

//=================================================================================================

bool IGESCAFControl_Writer::WriteNames(const NCollection_Sequence<TDF_Label>& theLabels)
{
  if (theLabels.Length() <= 0)
    return false;

  DataMapOfShapeNames aMapOfShapeNames;

  for (int anIter = 1; anIter <= theLabels.Length(); anIter++)
  {
    TDF_Label aLabel = theLabels.Value(anIter);

    TopoDS_Shape               aShape;
    occ::handle<TDataStd_Name> aName;
    if (!XCAFDoc_ShapeTool::GetShape(aLabel, aShape))
      continue;
    if (!aLabel.FindAttribute(TDataStd_Name::GetID(), aName))
      continue;

    aMapOfShapeNames.Bind(aShape, aName->Get());

    // Collect names for subshapes
    TopLoc_Location aLocation;
    CollectShapeNames(aLabel, aLocation, aName, aMapOfShapeNames);
  }

  for (DataMapOfShapeNames::Iterator anIter(aMapOfShapeNames); anIter.More(); anIter.Next())
  {
    const TopoDS_Shape&               aShape = anIter.Key();
    const TCollection_ExtendedString& aName  = anIter.Value();

    occ::handle<Transfer_FinderProcess>   aFinderProcess = TransferProcess();
    occ::handle<IGESData_IGESEntity>      anIGESEntity;
    occ::handle<TransferBRep_ShapeMapper> aShapeMapper =
      TransferBRep::ShapeMapper(aFinderProcess, aShape);

    if (aFinderProcess->FindTypedTransient(aShapeMapper,
                                           STANDARD_TYPE(IGESData_IGESEntity),
                                           anIGESEntity))
    {
      occ::handle<TCollection_HAsciiString> anAsciiName = new TCollection_HAsciiString("        ");
      int                                   aNameLength = 8 - aName.Length();
      if (aNameLength < 0)
        aNameLength = 0;
      for (int aCharPos = 1; aNameLength < 8; aCharPos++, aNameLength++)
      {
        anAsciiName->SetValue(aNameLength + 1,
                              IsAnAscii(aName.Value(aCharPos)) ? (char)aName.Value(aCharPos) : '?');
      }
      anIGESEntity->SetLabel(anAsciiName);

      // Set long IGES name using 406 form 15 entity
      occ::handle<IGESBasic_Name>              aLongNameEntity = new IGESBasic_Name;
      occ::handle<TCollection_HExtendedString> aTmpStr = new TCollection_HExtendedString(aName);
      aLongNameEntity->Init(1, new TCollection_HAsciiString(aTmpStr, '_'));

      anIGESEntity->AddProperty(aLongNameEntity);
      AddEntity(aLongNameEntity);
    }
  }

  return true;
}

//=================================================================================================

void IGESCAFControl_Writer::prepareUnit(const TDF_Label& theLabel)
{
  occ::handle<XCAFDoc_LengthUnit> aLengthAttr;
  if (!theLabel.IsNull() && theLabel.Root().FindAttribute(XCAFDoc_LengthUnit::GetID(), aLengthAttr))
  {
    Model()->ChangeGlobalSection().SetCascadeUnit(aLengthAttr->GetUnitValue() * 1000);
  }
  else
  {
    XSAlgo_ShapeProcessor::PrepareForTransfer(); // update unit info
    Model()->ChangeGlobalSection().SetCascadeUnit(UnitsMethods::GetCasCadeLengthUnit());
  }
}

//=================================================================================================

void IGESCAFControl_Writer::SetColorMode(const bool colormode)
{
  myColorMode = colormode;
}

//=================================================================================================

bool IGESCAFControl_Writer::GetColorMode() const
{
  return myColorMode;
}

//=================================================================================================

void IGESCAFControl_Writer::SetNameMode(const bool namemode)
{
  myNameMode = namemode;
}

//=================================================================================================

bool IGESCAFControl_Writer::GetNameMode() const
{
  return myNameMode;
}

//=================================================================================================

void IGESCAFControl_Writer::SetLayerMode(const bool layermode)
{
  myLayerMode = layermode;
}

//=================================================================================================

bool IGESCAFControl_Writer::GetLayerMode() const
{
  return myLayerMode;
}
