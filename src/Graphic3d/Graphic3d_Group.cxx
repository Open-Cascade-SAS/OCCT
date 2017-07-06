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
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_GroupDefinitionError.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_Structure.hxx>
#include "Graphic3d_Structure.pxx"
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_String.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Group,Standard_Transient)

// =======================================================================
// function : Graphic3d_Group
// purpose  :
// =======================================================================
Graphic3d_Group::Graphic3d_Group (const Handle(Graphic3d_Structure)& theStruct)
: myStructure     (theStruct.operator->()),
  myIsClosed      (false),
  myContainsFacet (false)
{
  //
}

// =======================================================================
// function : ~Graphic3d_Group
// purpose  :
// =======================================================================
Graphic3d_Group::~Graphic3d_Group()
{
  // tell graphics driver to clear internal resources of the group
  Clear (Standard_False);
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Graphic3d_Group::Clear (Standard_Boolean theUpdateStructureMgr)
{
  if (IsDeleted())
  {
    return;
  }

  myBounds.Clear();

  if (myContainsFacet)
  {
    myStructure->GroupsWithFacet (-1);
    myContainsFacet = false;
  }

  // clear method could be used on Graphic3d_Structure destruction,
  // and its structure manager could be already destroyed, in that
  // case we don't need to update it;
  if (theUpdateStructureMgr)
  {
    Update();
  }
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
void Graphic3d_Group::Remove()
{
  if (IsDeleted())
  {
    return;
  }

  if (myContainsFacet)
  {
    myStructure->GroupsWithFacet (-1);
    myContainsFacet = false;
  }
  myStructure->Remove (this);

  Update();

  myBounds.Clear();
}

// =======================================================================
// function : IsDeleted
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_Group::IsDeleted() const
{
  return myStructure == NULL
      || myStructure->IsDeleted();
}

// =======================================================================
// function : IsEmpty
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_Group::IsEmpty() const
{
  if (IsDeleted())
  {
    return Standard_True;
  }

  return !myStructure->IsInfinite()
      && !myBounds.IsValid();
}

// =======================================================================
// function : SetMinMaxValues
// purpose  :
// =======================================================================
void Graphic3d_Group::SetMinMaxValues (const Standard_Real theXMin, const Standard_Real theYMin, const Standard_Real theZMin,
                                       const Standard_Real theXMax, const Standard_Real theYMax, const Standard_Real theZMax)
{
  myBounds = Graphic3d_BndBox4f (Graphic3d_Vec4 (static_cast<Standard_ShortReal> (theXMin),
                                                 static_cast<Standard_ShortReal> (theYMin),
                                                 static_cast<Standard_ShortReal> (theZMin),
                                                 1.0f),
                                 Graphic3d_Vec4 (static_cast<Standard_ShortReal> (theXMax),
                                                 static_cast<Standard_ShortReal> (theYMax),
                                                 static_cast<Standard_ShortReal> (theZMax),
                                                 1.0f));
}

// =======================================================================
// function : Structure
// purpose  :
// =======================================================================
Handle(Graphic3d_Structure) Graphic3d_Group::Structure() const
{
  return myStructure;
}

// =======================================================================
// function : MinMaxValues
// purpose  :
// =======================================================================
void Graphic3d_Group::MinMaxValues (Standard_Real& theXMin, Standard_Real& theYMin, Standard_Real& theZMin,
                                    Standard_Real& theXMax, Standard_Real& theYMax, Standard_Real& theZMax) const
{
  if (IsEmpty())
  {
    // Empty Group
    theXMin = theYMin = theZMin = ShortRealFirst();
    theXMax = theYMax = theZMax = ShortRealLast();
  }
  else if (myBounds.IsValid())
  {
    const Graphic3d_Vec4& aMinPt = myBounds.CornerMin();
    const Graphic3d_Vec4& aMaxPt = myBounds.CornerMax();
    theXMin = Standard_Real (aMinPt.x());
    theYMin = Standard_Real (aMinPt.y());
    theZMin = Standard_Real (aMinPt.z());
    theXMax = Standard_Real (aMaxPt.x());
    theYMax = Standard_Real (aMaxPt.y());
    theZMax = Standard_Real (aMaxPt.z());
  }
  else
  {
    // for consistency with old API
    theXMin = theYMin = theZMin = ShortRealLast();
    theXMax = theYMax = theZMax = ShortRealFirst();
  }
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void Graphic3d_Group::Update() const
{
  if (IsDeleted())
  {
    return;
  }

  myStructure->StructureManager()->Update();
}

// =======================================================================
// function : IsGroupPrimitivesAspectSet
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_Group::IsGroupPrimitivesAspectSet (const Graphic3d_GroupAspect theAspect) const
{
  switch (theAspect)
  {
    case Graphic3d_ASPECT_LINE:      return !LineAspect().IsNull();
    case Graphic3d_ASPECT_TEXT:      return !TextAspect().IsNull();
    case Graphic3d_ASPECT_MARKER:    return !MarkerAspect().IsNull();
    case Graphic3d_ASPECT_FILL_AREA: return !FillAreaAspect().IsNull();
    default:                         return Standard_False;
  }
}

// =======================================================================
// function : GroupPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::GroupPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)&     theAspLine,
                                             const Handle(Graphic3d_AspectText3d)&     theAspText,
                                             const Handle(Graphic3d_AspectMarker3d)&   theAspMarker,
                                             const Handle(Graphic3d_AspectFillArea3d)& theAspFill) const
{
  if (!theAspLine.IsNull())
  {
    Handle(Graphic3d_AspectLine3d) aLineAspect = LineAspect();
    if (!aLineAspect.IsNull())
    {
      *theAspLine.operator->() = *aLineAspect;
    }
  }

  if (!theAspText.IsNull())
  {
    Handle(Graphic3d_AspectText3d) aTextAspect = TextAspect();
    if (!aTextAspect.IsNull())
    {
      *theAspText.operator->() = *aTextAspect;
    }
  }

  if (!theAspMarker.IsNull())
  {
    Handle(Graphic3d_AspectMarker3d) aMarkerAspect = MarkerAspect();
    if (!aMarkerAspect.IsNull())
    {
      *theAspMarker.operator->() = *aMarkerAspect;
    }
  }

  if (!theAspFill.IsNull())
  {
    Handle(Graphic3d_AspectFillArea3d) aFillAspect = FillAreaAspect();
    if (!aFillAspect.IsNull())
    {
      *theAspFill.operator->() = *aFillAspect;
    }
  }
}

// =======================================================================
// function : AddPrimitiveArray
// purpose  :
// =======================================================================
void Graphic3d_Group::AddPrimitiveArray (const Handle(Graphic3d_ArrayOfPrimitives)& thePrim,
                                         const Standard_Boolean                     theToEvalMinMax)
{
  if (IsDeleted()
  || !thePrim->IsValid())
  {
    return;
  }

  AddPrimitiveArray (thePrim->Type(), thePrim->Indices(), thePrim->Attributes(), thePrim->Bounds(), theToEvalMinMax);
}

// =======================================================================
// function : AddPrimitiveArray
// purpose  :
// =======================================================================
void Graphic3d_Group::AddPrimitiveArray (const Graphic3d_TypeOfPrimitiveArray theType,
                                         const Handle(Graphic3d_IndexBuffer)& ,
                                         const Handle(Graphic3d_Buffer)&      theAttribs,
                                         const Handle(Graphic3d_BoundBuffer)& ,
                                         const Standard_Boolean               theToEvalMinMax)
{
  if (IsDeleted()
   || theAttribs.IsNull())
  {
    return;
  }

  if (!myContainsFacet
    && theType != Graphic3d_TOPA_POLYLINES
    && theType != Graphic3d_TOPA_SEGMENTS
    && theType != Graphic3d_TOPA_POINTS)
  {
    myStructure->GroupsWithFacet (1);
    myContainsFacet = true;
  }

  if (theToEvalMinMax)
  {
    const Standard_Integer aNbVerts = theAttribs->NbElements;
    for (Standard_Integer anAttribIter = 0; anAttribIter < theAttribs->NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = theAttribs->Attribute (anAttribIter);
      if (anAttrib.Id != Graphic3d_TOA_POS)
      {
        continue;
      }

      const size_t anOffset = theAttribs->AttributeOffset (anAttribIter);
      switch (anAttrib.DataType)
      {
        case Graphic3d_TOD_VEC2:
        {
          for (Standard_Integer aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
          {
            const Graphic3d_Vec2& aVert = *reinterpret_cast<const Graphic3d_Vec2* >(theAttribs->value (aVertIter) + anOffset);
            myBounds.Add (Graphic3d_Vec4 (aVert.x(), aVert.y(), 0.0f, 1.0f));
          }
          break;
        }
        case Graphic3d_TOD_VEC3:
        case Graphic3d_TOD_VEC4:
        {
          for (Standard_Integer aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
          {
            const Graphic3d_Vec3& aVert = *reinterpret_cast<const Graphic3d_Vec3* >(theAttribs->value (aVertIter) + anOffset);
            myBounds.Add (Graphic3d_Vec4 (aVert.x(), aVert.y(), aVert.z(), 1.0f));
          }
          break;
        }
        default: break;
      }
      break;
    }
  }

  Update();
}

// =======================================================================
// function : Marker
// purpose  :
// =======================================================================
void Graphic3d_Group::Marker (const Graphic3d_Vertex& thePoint,
                              const Standard_Boolean  theToEvalMinMax)
{
  Handle(Graphic3d_ArrayOfPoints) aPoints = new Graphic3d_ArrayOfPoints (1);
  aPoints->AddVertex (thePoint.X(), thePoint.Y(), thePoint.Z());
  AddPrimitiveArray (aPoints, theToEvalMinMax);
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const Standard_CString                  /*theText*/,
                            const Graphic3d_Vertex&                 thePoint,
                            const Standard_Real                     /*theHeight*/,
                            const Standard_Real                     /*theAngle*/,
                            const Graphic3d_TextPath                /*theTp*/,
                            const Graphic3d_HorizontalTextAlignment /*theHta*/,
                            const Graphic3d_VerticalTextAlignment   /*theVta*/,
                            const Standard_Boolean                  theToEvalMinMax)
{
  if (IsDeleted())
  {
    return;
  }

  if (theToEvalMinMax)
  {
    Standard_ShortReal x, y, z;
    thePoint.Coord (x, y, z);
    myStructure->CStructure()->Is2dText = Standard_True;
    myBounds.Add (Graphic3d_Vec4 (static_cast<Standard_ShortReal> (x),
                                  static_cast<Standard_ShortReal> (y),
                                  static_cast<Standard_ShortReal> (z),
                                  1.0f));
  }
  Update();
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const Standard_CString  theText,
                            const Graphic3d_Vertex& thePoint,
                            const Standard_Real     theHeight,
                            const Standard_Boolean  theToEvalMinMax)
{
  Text (theText, thePoint, theHeight, 0.0,
        Graphic3d_TP_RIGHT, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM, theToEvalMinMax);
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const TCollection_ExtendedString&       theText,
                            const Graphic3d_Vertex&                 thePoint,
                            const Standard_Real                     theHeight,
                            const Standard_Real                     theAngle,
                            const Graphic3d_TextPath                theTp,
                            const Graphic3d_HorizontalTextAlignment theHta,
                            const Graphic3d_VerticalTextAlignment   theVta,
                            const Standard_Boolean                  theToEvalMinMax)
{
  const NCollection_String aText (theText.ToExtString());
  Text (aText.ToCString(), thePoint, theHeight, theAngle,
        theTp, theHta, theVta, theToEvalMinMax);
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const TCollection_ExtendedString&       theText,
                            const gp_Ax2&                           theOrientation,
                            const Standard_Real                     theHeight,
                            const Standard_Real                     theAngle,
                            const Graphic3d_TextPath                theTP,
                            const Graphic3d_HorizontalTextAlignment theHTA,
                            const Graphic3d_VerticalTextAlignment   theVTA,
                            const Standard_Boolean                  theToEvalMinMax,
                            const Standard_Boolean                  theHasOwnAnchor)
{
  const NCollection_String aText (theText.ToExtString());
  Text (aText.ToCString(),
        theOrientation,
        theHeight,
        theAngle,
        theTP,
        theHTA,
        theVTA,
        theToEvalMinMax,
        theHasOwnAnchor);
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const Standard_CString                  /*theText*/,
                            const gp_Ax2&                           theOrientation,
                            const Standard_Real                     /*theHeight*/,
                            const Standard_Real                     /*theAngle*/,
                            const Graphic3d_TextPath                /*theTp*/,
                            const Graphic3d_HorizontalTextAlignment /*theHta*/,
                            const Graphic3d_VerticalTextAlignment   /*theVta*/,
                            const Standard_Boolean                  theToEvalMinMax,
                            const Standard_Boolean                  /*theHasOwnAnchor*/)
{
  if (IsDeleted())
  {
    return;
  }

  if (theToEvalMinMax)
  {
    myStructure->CStructure()->Is2dText = Standard_False;
    myBounds.Add (Graphic3d_Vec4 (static_cast<Standard_ShortReal> (theOrientation.Location().X()),
                                  static_cast<Standard_ShortReal> (theOrientation.Location().Y()),
                                  static_cast<Standard_ShortReal> (theOrientation.Location().Z()),
                                  1.0f));
  }
  Update();
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const TCollection_ExtendedString& theText,
                            const Graphic3d_Vertex&           thePoint,
                            const Standard_Real               theHeight,
                            const Standard_Boolean            theToEvalMinMax)
{
  const NCollection_String aText (theText.ToExtString());
  Text (aText.ToCString(), thePoint, theHeight, 0.0,
        Graphic3d_TP_RIGHT, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM, theToEvalMinMax);
}
