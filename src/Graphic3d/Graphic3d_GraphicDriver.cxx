// Created on: 1997-01-28
// Created by: CAL
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

#include <Graphic3d_GraphicDriver.hxx>

#include <Graphic3d_Layer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_GraphicDriver, Standard_Transient)

// =======================================================================
// function : Graphic3d_GraphicDriver
// purpose  :
// =======================================================================
Graphic3d_GraphicDriver::Graphic3d_GraphicDriver(const Handle(Aspect_DisplayConnection)& theDisp)
    : myDisplayConnection(theDisp)
{
  // default layers are always presented in display layer sequence and cannot be removed
  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetName("UNDERLAY");
    aSettings.SetImmediate(Standard_False);
    aSettings.SetRaytracable(Standard_False);
    aSettings.SetEnvironmentTexture(Standard_False);
    aSettings.SetEnableDepthTest(Standard_False);
    aSettings.SetEnableDepthWrite(Standard_False);
    aSettings.SetClearDepth(Standard_False);
    aSettings.SetPolygonOffset(Graphic3d_PolygonOffset());
    Handle(Graphic3d_Layer) aLayer =
      new Graphic3d_Layer(Graphic3d_ZLayerId_BotOSD, Handle(BVH_Builder3d)());
    aLayer->SetLayerSettings(aSettings);
    myLayers.Append(aLayer);
    myLayerIds.Bind(aLayer->LayerId(), aLayer);
  }

  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetName("DEFAULT");
    aSettings.SetImmediate(Standard_False);
    aSettings.SetRaytracable(Standard_True);
    aSettings.SetEnvironmentTexture(Standard_True);
    aSettings.SetEnableDepthTest(Standard_True);
    aSettings.SetEnableDepthWrite(Standard_True);
    aSettings.SetClearDepth(Standard_False);
    aSettings.SetPolygonOffset(Graphic3d_PolygonOffset());
    Handle(Graphic3d_Layer) aLayer =
      new Graphic3d_Layer(Graphic3d_ZLayerId_Default, Handle(BVH_Builder3d)());
    aLayer->SetLayerSettings(aSettings);
    myLayers.Append(aLayer);
    myLayerIds.Bind(aLayer->LayerId(), aLayer);
  }

  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetName("TOP");
    aSettings.SetImmediate(Standard_True);
    aSettings.SetRaytracable(Standard_False);
    aSettings.SetEnvironmentTexture(Standard_True);
    aSettings.SetEnableDepthTest(Standard_True);
    aSettings.SetEnableDepthWrite(Standard_True);
    aSettings.SetClearDepth(Standard_False);
    aSettings.SetPolygonOffset(Graphic3d_PolygonOffset());
    Handle(Graphic3d_Layer) aLayer =
      new Graphic3d_Layer(Graphic3d_ZLayerId_Top, Handle(BVH_Builder3d)());
    aLayer->SetLayerSettings(aSettings);
    myLayers.Append(aLayer);
    myLayerIds.Bind(aLayer->LayerId(), aLayer);
  }

  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetName("TOPMOST");
    aSettings.SetImmediate(Standard_True);
    aSettings.SetRaytracable(Standard_False);
    aSettings.SetEnvironmentTexture(Standard_True);
    aSettings.SetEnableDepthTest(Standard_True);
    aSettings.SetEnableDepthWrite(Standard_True);
    aSettings.SetClearDepth(Standard_True);
    aSettings.SetPolygonOffset(Graphic3d_PolygonOffset());
    Handle(Graphic3d_Layer) aLayer =
      new Graphic3d_Layer(Graphic3d_ZLayerId_Topmost, Handle(BVH_Builder3d)());
    aLayer->SetLayerSettings(aSettings);
    myLayers.Append(aLayer);
    myLayerIds.Bind(aLayer->LayerId(), aLayer);
  }

  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetName("OVERLAY");
    aSettings.SetImmediate(Standard_True);
    aSettings.SetRaytracable(Standard_False);
    aSettings.SetEnvironmentTexture(Standard_False);
    aSettings.SetEnableDepthTest(Standard_False);
    aSettings.SetEnableDepthWrite(Standard_False);
    aSettings.SetClearDepth(Standard_False);
    aSettings.SetPolygonOffset(Graphic3d_PolygonOffset());
    Handle(Graphic3d_Layer) aLayer =
      new Graphic3d_Layer(Graphic3d_ZLayerId_TopOSD, Handle(BVH_Builder3d)());
    aLayer->SetLayerSettings(aSettings);
    myLayers.Append(aLayer);
    myLayerIds.Bind(aLayer->LayerId(), aLayer);
  }
}

// =======================================================================
// function : GetDisplayConnection
// purpose  :
// =======================================================================
const Handle(Aspect_DisplayConnection)& Graphic3d_GraphicDriver::GetDisplayConnection() const
{
  return myDisplayConnection;
}

// =======================================================================
// function : NewIdentification
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_GraphicDriver::NewIdentification()
{
  return myStructGenId.Next();
}

// =======================================================================
// function : RemoveIdentification
// purpose  :
// =======================================================================
void Graphic3d_GraphicDriver::RemoveIdentification(const Standard_Integer theId)
{
  myStructGenId.Free(theId);
}

//=================================================================================================

const Graphic3d_ZLayerSettings& Graphic3d_GraphicDriver::ZLayerSettings(
  const Graphic3d_ZLayerId theLayerId) const
{
  const Handle(Graphic3d_Layer)* aLayer = myLayerIds.Seek(theLayerId);
  if (aLayer == NULL)
  {
    throw Standard_OutOfRange(
      "Graphic3d_GraphicDriver::ZLayerSettings, Layer with theLayerId does not exist");
  }
  return (*aLayer)->LayerSettings();
}

//=================================================================================================

void Graphic3d_GraphicDriver::ZLayers(TColStd_SequenceOfInteger& theLayerSeq) const
{
  theLayerSeq.Clear();

  // append normal layers
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers); aLayerIter.More();
       aLayerIter.Next())
  {
    const Handle(Graphic3d_Layer)& aLayer = aLayerIter.Value();
    if (!aLayer->IsImmediate())
    {
      theLayerSeq.Append(aLayer->LayerId());
    }
  }

  // append immediate layers
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers); aLayerIter.More();
       aLayerIter.Next())
  {
    const Handle(Graphic3d_Layer)& aLayer = aLayerIter.Value();
    if (aLayer->IsImmediate())
    {
      theLayerSeq.Append(aLayer->LayerId());
    }
  }
}

//=================================================================================================

void Graphic3d_GraphicDriver::InsertLayerBefore(const Graphic3d_ZLayerId        theNewLayerId,
                                                const Graphic3d_ZLayerSettings& theSettings,
                                                const Graphic3d_ZLayerId        theLayerAfter)
{
  Standard_ASSERT_RAISE(
    theNewLayerId > 0,
    "Graphic3d_GraphicDriver::InsertLayerBefore, negative and zero IDs are reserved");
  Standard_ASSERT_RAISE(
    !myLayerIds.IsBound(theNewLayerId),
    "Graphic3d_GraphicDriver::InsertLayerBefore, Layer with theLayerId already exists");

  Handle(Graphic3d_Layer) aNewLayer = new Graphic3d_Layer(theNewLayerId, Handle(BVH_Builder3d)());
  aNewLayer->SetLayerSettings(theSettings);

  Handle(Graphic3d_Layer) anOtherLayer;
  if (theLayerAfter != Graphic3d_ZLayerId_UNKNOWN && myLayerIds.Find(theLayerAfter, anOtherLayer))
  {
    for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers);
         aLayerIter.More();
         aLayerIter.Next())
    {
      if (aLayerIter.Value() == anOtherLayer)
      {
        myLayers.InsertBefore(aNewLayer, aLayerIter);
        break;
      }
    }
  }
  else
  {
    myLayers.Prepend(aNewLayer);
  }
  myLayerIds.Bind(theNewLayerId, aNewLayer);
}

//=================================================================================================

void Graphic3d_GraphicDriver::InsertLayerAfter(const Graphic3d_ZLayerId        theNewLayerId,
                                               const Graphic3d_ZLayerSettings& theSettings,
                                               const Graphic3d_ZLayerId        theLayerBefore)
{
  Standard_ASSERT_RAISE(
    theNewLayerId > 0,
    "Graphic3d_GraphicDriver::InsertLayerAfter, negative and zero IDs are reserved");
  Standard_ASSERT_RAISE(
    !myLayerIds.IsBound(theNewLayerId),
    "Graphic3d_GraphicDriver::InsertLayerAfter, Layer with theLayerId already exists");

  Handle(Graphic3d_Layer) aNewLayer = new Graphic3d_Layer(theNewLayerId, Handle(BVH_Builder3d)());
  aNewLayer->SetLayerSettings(theSettings);

  Handle(Graphic3d_Layer) anOtherLayer;
  if (theLayerBefore != Graphic3d_ZLayerId_UNKNOWN && myLayerIds.Find(theLayerBefore, anOtherLayer))
  {
    for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter(myLayers);
         aLayerIter.More();
         aLayerIter.Next())
    {
      if (aLayerIter.Value() == anOtherLayer)
      {
        myLayers.InsertAfter(aNewLayer, aLayerIter);
        break;
      }
    }
  }
  else
  {
    myLayers.Append(aNewLayer);
  }
  myLayerIds.Bind(theNewLayerId, aNewLayer);
}

//=================================================================================================

void Graphic3d_GraphicDriver::RemoveZLayer(const Graphic3d_ZLayerId theLayerId)
{
  Standard_ASSERT_RAISE(theLayerId > 0,
                        "Graphic3d_GraphicDriver::RemoveZLayer, negative and zero IDs are reserved "
                        "and cannot be removed");

  Handle(Graphic3d_Layer) aLayerDef;
  myLayerIds.Find(theLayerId, aLayerDef);
  Standard_ASSERT_RAISE(
    !aLayerDef.IsNull(),
    "Graphic3d_GraphicDriver::RemoveZLayer, Layer with theLayerId does not exist");
  myLayers.Remove(aLayerDef);
  myLayerIds.UnBind(theLayerId);
}

//=================================================================================================

void Graphic3d_GraphicDriver::SetZLayerSettings(const Graphic3d_ZLayerId        theLayerId,
                                                const Graphic3d_ZLayerSettings& theSettings)
{
  Handle(Graphic3d_Layer) aLayerDef;
  myLayerIds.Find(theLayerId, aLayerDef);
  Standard_ASSERT_RAISE(
    !aLayerDef.IsNull(),
    "Graphic3d_GraphicDriver::SetZLayerSettings, Layer with theLayerId does not exist");
  aLayerDef->SetLayerSettings(theSettings);
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_GraphicDriver::DumpJson(Standard_OStream& theOStream,
                                       Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myStructGenId)

  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator anIter(myLayers); anIter.More();
       anIter.Next())
  {
    const Handle(Graphic3d_Layer)& aLayer = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aLayer.get())
  }
}
