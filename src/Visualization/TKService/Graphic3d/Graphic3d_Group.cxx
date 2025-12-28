// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Graphic3d_Group.hxx>

#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_GroupDefinitionError.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Text.hxx>
#include <NCollection_String.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Group, Standard_Transient)

//=================================================================================================

Graphic3d_Group::Graphic3d_Group(const occ::handle<Graphic3d_Structure>& theStruct)
    : myStructure(theStruct.operator->()),
      myIsClosed(false)
{
  //
}

//=================================================================================================

Graphic3d_Group::~Graphic3d_Group()
{
  // tell graphics driver to clear internal resources of the group
  Clear(false);
}

//=================================================================================================

void Graphic3d_Group::Clear(bool theUpdateStructureMgr)
{
  if (IsDeleted())
  {
    return;
  }

  myBounds.Clear();

  // clear method could be used on Graphic3d_Structure destruction,
  // and its structure manager could be already destroyed, in that
  // case we don't need to update it;
  if (theUpdateStructureMgr)
  {
    Update();
  }
}

//=================================================================================================

void Graphic3d_Group::Remove()
{
  if (IsDeleted())
  {
    return;
  }

  myStructure->Remove(this);

  Update();

  myBounds.Clear();
}

//=================================================================================================

bool Graphic3d_Group::IsDeleted() const
{
  return myStructure == nullptr || myStructure->IsDeleted();
}

//=================================================================================================

bool Graphic3d_Group::IsEmpty() const
{
  if (IsDeleted())
  {
    return true;
  }

  return !myStructure->IsInfinite() && !myBounds.IsValid();
}

//=================================================================================================

void Graphic3d_Group::SetTransformPersistence(
  const occ::handle<Graphic3d_TransformPers>& theTrsfPers)
{
  if (myTrsfPers != theTrsfPers)
  {
    myTrsfPers = theTrsfPers;
    if (!IsDeleted() && !theTrsfPers.IsNull())
    {
      myStructure->CStructure()->SetGroupTransformPersistence(true);
    }
  }
}

//=================================================================================================

void Graphic3d_Group::SetMinMaxValues(const double theXMin,
                                      const double theYMin,
                                      const double theZMin,
                                      const double theXMax,
                                      const double theYMax,
                                      const double theZMax)
{
  myBounds = Graphic3d_BndBox4f(NCollection_Vec4<float>(static_cast<float>(theXMin),
                                                        static_cast<float>(theYMin),
                                                        static_cast<float>(theZMin),
                                                        1.0f),
                                NCollection_Vec4<float>(static_cast<float>(theXMax),
                                                        static_cast<float>(theYMax),
                                                        static_cast<float>(theZMax),
                                                        1.0f));
}

//=================================================================================================

occ::handle<Graphic3d_Structure> Graphic3d_Group::Structure() const
{
  return myStructure;
}

//=================================================================================================

void Graphic3d_Group::MinMaxValues(double& theXMin,
                                   double& theYMin,
                                   double& theZMin,
                                   double& theXMax,
                                   double& theYMax,
                                   double& theZMax) const
{
  if (IsEmpty())
  {
    // Empty Group
    theXMin = theYMin = theZMin = ShortRealFirst();
    theXMax = theYMax = theZMax = ShortRealLast();
  }
  else if (myBounds.IsValid())
  {
    const NCollection_Vec4<float>& aMinPt = myBounds.CornerMin();
    const NCollection_Vec4<float>& aMaxPt = myBounds.CornerMax();
    theXMin                               = double(aMinPt.x());
    theYMin                               = double(aMinPt.y());
    theZMin                               = double(aMinPt.z());
    theXMax                               = double(aMaxPt.x());
    theYMax                               = double(aMaxPt.y());
    theZMax                               = double(aMaxPt.z());
  }
  else
  {
    // for consistency with old API
    theXMin = theYMin = theZMin = ShortRealLast();
    theXMax = theYMax = theZMax = ShortRealFirst();
  }
}

//=================================================================================================

void Graphic3d_Group::Update() const
{
  if (IsDeleted())
  {
    return;
  }

  myStructure->StructureManager()->Update();
}

//=================================================================================================

void Graphic3d_Group::AddPrimitiveArray(const occ::handle<Graphic3d_ArrayOfPrimitives>& thePrim,
                                        const bool theToEvalMinMax)
{
  if (IsDeleted() || !thePrim->IsValid())
  {
    return;
  }

  AddPrimitiveArray(thePrim->Type(),
                    thePrim->Indices(),
                    thePrim->Attributes(),
                    thePrim->Bounds(),
                    theToEvalMinMax);
}

//=================================================================================================

void Graphic3d_Group::AddPrimitiveArray(const Graphic3d_TypeOfPrimitiveArray theType,
                                        const occ::handle<Graphic3d_IndexBuffer>&,
                                        const occ::handle<Graphic3d_Buffer>& theAttribs,
                                        const occ::handle<Graphic3d_BoundBuffer>&,
                                        const bool theToEvalMinMax)
{
  (void)theType;
  if (IsDeleted() || theAttribs.IsNull())
  {
    return;
  }

  if (!theToEvalMinMax)
  {
    Update();
    return;
  }

  const int      aNbVerts       = theAttribs->NbElements;
  int            anAttribIndex  = 0;
  size_t         anAttribStride = 0;
  const uint8_t* aDataPtr =
    theAttribs->AttributeData(Graphic3d_TOA_POS, anAttribIndex, anAttribStride);
  if (aDataPtr == nullptr)
  {
    Update();
    return;
  }

  switch (theAttribs->Attribute(anAttribIndex).DataType)
  {
    case Graphic3d_TOD_VEC2: {
      for (int aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
      {
        const NCollection_Vec2<float>& aVert =
          *reinterpret_cast<const NCollection_Vec2<float>*>(aDataPtr + anAttribStride * aVertIter);
        myBounds.Add(NCollection_Vec4<float>(aVert.x(), aVert.y(), 0.0f, 1.0f));
      }
      break;
    }
    case Graphic3d_TOD_VEC3:
    case Graphic3d_TOD_VEC4: {
      for (int aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
      {
        const NCollection_Vec3<float>& aVert =
          *reinterpret_cast<const NCollection_Vec3<float>*>(aDataPtr + anAttribStride * aVertIter);
        myBounds.Add(NCollection_Vec4<float>(aVert.x(), aVert.y(), aVert.z(), 1.0f));
      }
      break;
    }
    default:
      break;
  }
  Update();
}

//=================================================================================================

void Graphic3d_Group::Marker(const Graphic3d_Vertex& thePoint, const bool theToEvalMinMax)
{
  occ::handle<Graphic3d_ArrayOfPoints> aPoints = new Graphic3d_ArrayOfPoints(1);
  aPoints->AddVertex(thePoint.X(), thePoint.Y(), thePoint.Z());
  AddPrimitiveArray(aPoints, theToEvalMinMax);
}

//=================================================================================================

void Graphic3d_Group::Text(const char*             theText,
                           const Graphic3d_Vertex& thePoint,
                           const double            theHeight,
                           const double /*theAngle*/,
                           const Graphic3d_TextPath /*theTp*/,
                           const Graphic3d_HorizontalTextAlignment theHta,
                           const Graphic3d_VerticalTextAlignment   theVta,
                           const bool                              theToEvalMinMax)
{
  occ::handle<Graphic3d_Text> aText = new Graphic3d_Text((float)theHeight);
  aText->SetText(theText);
  aText->SetPosition(gp_Pnt(thePoint.X(), thePoint.Y(), thePoint.Z()));
  aText->SetHorizontalAlignment(theHta);
  aText->SetVerticalAlignment(theVta);
  AddText(aText, theToEvalMinMax);
}

//=================================================================================================

void Graphic3d_Group::Text(const char*             theText,
                           const Graphic3d_Vertex& thePoint,
                           const double            theHeight,
                           const bool              theToEvalMinMax)
{
  occ::handle<Graphic3d_Text> aText = new Graphic3d_Text((float)theHeight);
  aText->SetText(theText);
  aText->SetPosition(gp_Pnt(thePoint.X(), thePoint.Y(), thePoint.Z()));
  AddText(aText, theToEvalMinMax);
}

//=================================================================================================

void Graphic3d_Group::Text(const TCollection_ExtendedString& theText,
                           const Graphic3d_Vertex&           thePoint,
                           const double                      theHeight,
                           const double /*theAngle*/,
                           const Graphic3d_TextPath /*theTp*/,
                           const Graphic3d_HorizontalTextAlignment theHta,
                           const Graphic3d_VerticalTextAlignment   theVta,
                           const bool                              theToEvalMinMax)
{
  occ::handle<Graphic3d_Text> aText = new Graphic3d_Text((float)theHeight);
  aText->SetText(theText.ToExtString());
  aText->SetPosition(gp_Pnt(thePoint.X(), thePoint.Y(), thePoint.Z()));
  aText->SetHorizontalAlignment(theHta);
  aText->SetVerticalAlignment(theVta);
  AddText(aText, theToEvalMinMax);
}

//=================================================================================================

void Graphic3d_Group::Text(const TCollection_ExtendedString& theText,
                           const gp_Ax2&                     theOrientation,
                           const double                      theHeight,
                           const double /*theAngle*/,
                           const Graphic3d_TextPath /*theTP*/,
                           const Graphic3d_HorizontalTextAlignment theHta,
                           const Graphic3d_VerticalTextAlignment   theVta,
                           const bool                              theToEvalMinMax,
                           const bool                              theHasOwnAnchor)
{
  occ::handle<Graphic3d_Text> aText = new Graphic3d_Text((float)theHeight);
  aText->SetText(theText.ToExtString());
  aText->SetOrientation(theOrientation);
  aText->SetOwnAnchorPoint(theHasOwnAnchor);
  aText->SetHorizontalAlignment(theHta);
  aText->SetVerticalAlignment(theVta);
  AddText(aText, theToEvalMinMax);
}

//=================================================================================================

void Graphic3d_Group::Text(const char*   theText,
                           const gp_Ax2& theOrientation,
                           const double  theHeight,
                           const double /*theAngle*/,
                           const Graphic3d_TextPath /*theTp*/,
                           const Graphic3d_HorizontalTextAlignment theHta,
                           const Graphic3d_VerticalTextAlignment   theVta,
                           const bool                              theToEvalMinMax,
                           const bool                              theHasOwnAnchor)
{
  occ::handle<Graphic3d_Text> aText = new Graphic3d_Text((float)theHeight);
  aText->SetText(theText);
  aText->SetOrientation(theOrientation);
  aText->SetOwnAnchorPoint(theHasOwnAnchor);
  aText->SetHorizontalAlignment(theHta);
  aText->SetVerticalAlignment(theVta);
  AddText(aText, theToEvalMinMax);
}

//=================================================================================================

void Graphic3d_Group::Text(const TCollection_ExtendedString& theText,
                           const Graphic3d_Vertex&           thePoint,
                           const double                      theHeight,
                           const bool                        theToEvalMinMax)
{
  occ::handle<Graphic3d_Text> aText = new Graphic3d_Text((float)theHeight);
  aText->SetText(theText.ToExtString());
  aText->SetPosition(gp_Pnt(thePoint.X(), thePoint.Y(), thePoint.Z()));
  AddText(aText, theToEvalMinMax);
}

//=================================================================================================

void Graphic3d_Group::AddText(const occ::handle<Graphic3d_Text>& theTextParams,
                              const bool                         theToEvalMinMax)
{
  if (IsDeleted())
  {
    return;
  }

  if (theToEvalMinMax)
  {
    myStructure->CStructure()->Is2dText = !theTextParams->HasPlane();

    gp_Pnt aPosition = theTextParams->Position();
    myBounds.Add(NCollection_Vec4<float>((float)aPosition.X(),
                                         (float)aPosition.Y(),
                                         (float)aPosition.Z(),
                                         1.0f));
  }

  Update();
}

//=================================================================================================

void Graphic3d_Group::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, this)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTrsfPers.get())

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myStructure)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBounds)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsClosed)
}
