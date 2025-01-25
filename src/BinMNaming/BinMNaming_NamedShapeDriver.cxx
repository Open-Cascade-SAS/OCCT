// Created on: 2004-04-08
// Created by: Sergey ZARITCHNY
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

#include <BinMNaming_NamedShapeDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinTools_LocationSet.hxx>
#include <BinTools_ShapeSet.hxx>
#include <BinTools_ShapeWriter.hxx>
#include <BinTools_ShapeReader.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_FormatVersion.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMNaming_NamedShapeDriver, BinMDF_ADriver)

#define SHAPESET "SHAPE_SECTION"

//=======================================================================
static Standard_Character EvolutionToChar(const TNaming_Evolution theEvol)
{
  switch (theEvol)
  {
    case TNaming_PRIMITIVE:
      return 'P';
    case TNaming_GENERATED:
      return 'G';
    case TNaming_MODIFY:
      return 'M';
    case TNaming_DELETE:
      return 'D';
    case TNaming_SELECTED:
      return 'S';
      // clang-format off
    case TNaming_REPLACE      : return 'M'; // for compatibility case TNaming_REPLACE      : return 'R';
      // clang-format on
    default:
      throw Standard_DomainError("TNaming_Evolution:: Evolution Unknown");
  }
}

//=======================================================================
static TNaming_Evolution EvolutionToEnum(const Standard_Character theEvol)
{
  switch (theEvol)
  {
    case 'P':
      return TNaming_PRIMITIVE;
    case 'G':
      return TNaming_GENERATED;
    case 'M':
      return TNaming_MODIFY;
    case 'D':
      return TNaming_DELETE;
    case 'S':
      return TNaming_SELECTED;
    case 'R':
      return TNaming_MODIFY; // for compatibility //TNaming_REPLACE;
    default:
      throw Standard_DomainError("TNaming_Evolution:: Evolution Unknown");
  }
}

//=======================================================================
static Standard_Character OrientationToChar(const TopAbs_Orientation theOrient)
{
  switch (theOrient)
  {
    case TopAbs_FORWARD:
      return 'F';
    case TopAbs_REVERSED:
      return 'R';
    case TopAbs_INTERNAL:
      return 'I';
    case TopAbs_EXTERNAL:
      return 'E';
    default:
      throw Standard_DomainError("TopAbs_Orientation:: Orientation Unknown");
  }
}

//=======================================================================
static TopAbs_Orientation CharToOrientation(const Standard_Character theCharOrient)
{
  switch (theCharOrient)
  {
    case 'F':
      return TopAbs_FORWARD;
    case 'R':
      return TopAbs_REVERSED;
    case 'I':
      return TopAbs_INTERNAL;
    case 'E':
      return TopAbs_EXTERNAL;
    default:
      throw Standard_DomainError("TopAbs_Orientation:: Orientation Unknown");
  }
}

//=======================================================================
static void TranslateTo(const TopoDS_Shape&   theShape,
                        BinObjMgt_Persistent& theResult,
                        BinTools_ShapeSet*    theShapeSet)
{
  // Check for empty shape
  if (theShape.IsNull())
  {
    theResult.PutInteger(-1);
    theResult.PutInteger(-1);
    theResult.PutInteger(-1);
    return;
  }
  // Add to shape set both TShape and Location contained in <theShape>
  const Standard_Integer aTShapeID = theShapeSet->Add(theShape);
  const Standard_Integer aLocID    = theShapeSet->Locations().Index(theShape.Location());

  // Fill theResult with shape parameters: TShape ID, Location, Orientation
  theResult << aTShapeID;
  theResult << aLocID;
  theResult << OrientationToChar(theShape.Orientation());
}

//=======================================================================
static int TranslateFrom(const BinObjMgt_Persistent& theSource,
                         TopoDS_Shape&               theResult,
                         BinTools_ShapeSet*          theShapeSet)
{
  Standard_Integer   aShapeID, aLocID;
  Standard_Character aCharOrient;
  Standard_Boolean   Ok = theSource >> aShapeID; // TShapeID;
  if (!Ok)
    return 1;
  // Read TShape and Orientation
  if (aShapeID <= 0 || aShapeID > theShapeSet->NbShapes())
    return 1;
  Ok = theSource >> aLocID;
  if (!Ok)
    return 1;
  Ok = theSource >> aCharOrient;
  if (!Ok)
    return 1;
  TopAbs_Orientation anOrient = CharToOrientation(aCharOrient);

  theResult.TShape(theShapeSet->Shape(aShapeID).TShape());                       // TShape
  theResult.Location(theShapeSet->Locations().Location(aLocID), Standard_False); // Location
  theResult.Orientation(anOrient);                                               // Orientation
  return 0;
}

//=================================================================================================

BinMNaming_NamedShapeDriver::BinMNaming_NamedShapeDriver(
  const Handle(Message_Messenger)& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TNaming_NamedShape)->Name()),
      myShapeSet(NULL),
      myWithTriangles(Standard_False),
      myWithNormals(Standard_False),
      myIsQuickPart(Standard_False)
{
}

//=================================================================================================

Handle(TDF_Attribute) BinMNaming_NamedShapeDriver::NewEmpty() const
{
  return new TNaming_NamedShape();
}

//=======================================================================
// function : Paste
// purpose  : persistent => transient (retrieve)
//=======================================================================

Standard_Boolean BinMNaming_NamedShapeDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                    const Handle(TDF_Attribute)& theTarget,
                                                    BinObjMgt_RRelocationTable&) const
{
  Handle(TNaming_NamedShape) aTAtt = Handle(TNaming_NamedShape)::DownCast(theTarget);
  Standard_Integer           aNbShapes;
  theSource >> aNbShapes;
  TDF_Label        aLabel = theTarget->Label();
  TNaming_Builder  aBuilder(aLabel);
  Standard_Integer aVer;
  Standard_Boolean ok = theSource >> aVer;
  if (!ok)
    return Standard_False;
  aTAtt->SetVersion(aVer); // Version
  Standard_Character aCharEvol;
  ok = theSource >> aCharEvol;
  if (!ok)
    return Standard_False;
  TNaming_Evolution anEvol = EvolutionToEnum(aCharEvol); // Evolution
  aTAtt->SetVersion(anEvol);

  BinTools_ShapeSetBase* aShapeSet =
    const_cast<BinMNaming_NamedShapeDriver*>(this)->ShapeSet(Standard_True);
  Standard_IStream* aDirectStream = NULL;
  if (myIsQuickPart) // enables direct reading of shapes from the stream
    aDirectStream = const_cast<BinObjMgt_Persistent*>(&theSource)->GetIStream();

  NCollection_List<TopoDS_Shape> anOldShapes, aNewShapes;
  for (Standard_Integer i = 1; i <= aNbShapes; i++)
  {
    TopoDS_Shape anOldShape, aNewShape;

    if (anEvol != TNaming_PRIMITIVE)
    {
      if (myIsQuickPart)
        aShapeSet->Read(*aDirectStream, anOldShape);
      else if (TranslateFrom(theSource, anOldShape, static_cast<BinTools_ShapeSet*>(aShapeSet)))
        return Standard_False;
    }

    if (anEvol != TNaming_DELETE)
    {
      if (myIsQuickPart)
        aShapeSet->Read(*aDirectStream, aNewShape);
      else if (TranslateFrom(theSource, aNewShape, static_cast<BinTools_ShapeSet*>(aShapeSet)))
        return Standard_False;
    }

    // Here we add shapes in reverse order because TNaming_Builder also adds them in reverse order.
    anOldShapes.Prepend(anOldShape);
    aNewShapes.Prepend(aNewShape);
  }

  for (NCollection_List<TopoDS_Shape>::Iterator anOldIt(anOldShapes), aNewIt(aNewShapes);
       anOldIt.More() && aNewIt.More();
       anOldIt.Next(), aNewIt.Next())
  {
    switch (anEvol)
    {
      case TNaming_PRIMITIVE:
        aBuilder.Generated(aNewIt.Value());
        break;
      case TNaming_GENERATED:
        aBuilder.Generated(anOldIt.Value(), aNewIt.Value());
        break;
      case TNaming_MODIFY:
        aBuilder.Modify(anOldIt.Value(), aNewIt.Value());
        break;
      case TNaming_DELETE:
        aBuilder.Delete(anOldIt.Value());
        break;
      case TNaming_SELECTED:
        aBuilder.Select(aNewIt.Value(), anOldIt.Value());
        break;
      case TNaming_REPLACE:
        // clang-format off
        aBuilder.Modify (anOldIt.Value(), aNewIt.Value()); // for compatibility aBuilder.Replace(anOldShape, aNewShape);
        // clang-format on
        break;
      default:
        throw Standard_DomainError("TNaming_Evolution:: Evolution Unknown");
    }
  }
  return Standard_True;
}

//=======================================================================
// function : Paste
// purpose  : transient => persistent (store)
//=======================================================================

void BinMNaming_NamedShapeDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                        BinObjMgt_Persistent&        theTarget,
                                        BinObjMgt_SRelocationTable&) const
{
  Handle(TNaming_NamedShape) aSAtt = Handle(TNaming_NamedShape)::DownCast(theSource);

  //--------------------------------------------------------------
  Standard_Integer NbShapes = 0;
  for (TNaming_Iterator SItr(aSAtt); SItr.More(); SItr.Next())
    NbShapes++;
  //--------------------------------------------------------------

  BinTools_ShapeSetBase* aShapeSet =
    const_cast<BinMNaming_NamedShapeDriver*>(this)->ShapeSet(Standard_False);
  TNaming_Evolution anEvol = aSAtt->Evolution();

  theTarget << NbShapes;
  theTarget << aSAtt->Version();
  theTarget << EvolutionToChar(anEvol);

  Standard_OStream* aDirectStream = NULL;
  if (myIsQuickPart) // enables direct writing of shapes to the stream
    aDirectStream = theTarget.GetOStream();

  for (TNaming_Iterator SIterator(aSAtt); SIterator.More(); SIterator.Next())
  {
    const TopoDS_Shape& anOldShape = SIterator.OldShape();
    const TopoDS_Shape& aNewShape  = SIterator.NewShape();

    if (anEvol != TNaming_PRIMITIVE)
    {
      if (myIsQuickPart)
        aShapeSet->Write(anOldShape, *aDirectStream);
      else
        TranslateTo(anOldShape, theTarget, static_cast<BinTools_ShapeSet*>(aShapeSet));
    }

    if (anEvol != TNaming_DELETE)
    {
      if (myIsQuickPart)
        aShapeSet->Write(aNewShape, *aDirectStream);
      else
        TranslateTo(aNewShape, theTarget, static_cast<BinTools_ShapeSet*>(aShapeSet));
    }
  }
}

//=================================================================================================

void BinMNaming_NamedShapeDriver::WriteShapeSection(Standard_OStream&            theOS,
                                                    const Standard_Integer       theDocVer,
                                                    const Message_ProgressRange& theRange)
{
  myIsQuickPart = Standard_False;
  theOS << SHAPESET;
  if (theDocVer >= TDocStd_FormatVersion_VERSION_11)
  {
    ShapeSet(Standard_False)->SetFormatNb(BinTools_FormatVersion_VERSION_4);
  }
  else
  {
    ShapeSet(Standard_False)->SetFormatNb(BinTools_FormatVersion_VERSION_1);
  }
  ShapeSet(Standard_False)->Write(theOS, theRange);
  ShapeSet(Standard_False)->Clear();
}

//=================================================================================================

void BinMNaming_NamedShapeDriver::Clear()
{
  if (myShapeSet)
  {
    myShapeSet->Clear();
    delete myShapeSet;
    myShapeSet = NULL;
  }
}

//=================================================================================================

void BinMNaming_NamedShapeDriver::ReadShapeSection(Standard_IStream&            theIS,
                                                   const Message_ProgressRange& theRange)
{
  myIsQuickPart = Standard_False;
  // check section title string; note that some versions of OCCT (up to 6.3.1)
  // might avoid writing shape section if it is empty
  std::streamoff          aPos = theIS.tellg();
  TCollection_AsciiString aSectionTitle;
  theIS >> aSectionTitle;
  if (aSectionTitle.Length() > 0 && aSectionTitle == SHAPESET)
  {
    BinTools_ShapeSetBase* aShapeSet = ShapeSet(Standard_True);
    aShapeSet->Clear();
    aShapeSet->Read(theIS, theRange);
  }
  else
    theIS.seekg(aPos); // no shape section is present, try to return to initial point
}

//=================================================================================================

BinTools_ShapeSetBase* BinMNaming_NamedShapeDriver::ShapeSet(const Standard_Boolean theReading)
{
  if (!myShapeSet)
  {
    if (myIsQuickPart)
    {
      if (theReading)
        myShapeSet = new BinTools_ShapeReader();
      else
        myShapeSet = new BinTools_ShapeWriter();
    }
    else
      myShapeSet = new BinTools_ShapeSet();
    myShapeSet->SetWithTriangles(myWithTriangles);
    myShapeSet->SetWithNormals(myWithNormals);
  }
  return myShapeSet;
}

//=================================================================================================

BinTools_LocationSet& BinMNaming_NamedShapeDriver::GetShapesLocations() const
{
  BinTools_ShapeSetBase* aShapeSet =
    const_cast<BinMNaming_NamedShapeDriver*>(this)->ShapeSet(Standard_False);
  return static_cast<BinTools_ShapeSet*>(aShapeSet)->ChangeLocations();
}
