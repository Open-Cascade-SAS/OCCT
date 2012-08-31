// Created on: 2004-04-08
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BinMNaming_NamedShapeDriver.ixx>
#include <Standard_DomainError.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Evolution.hxx>
#include <TNaming_Builder.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Iterator.hxx>
#include <CDM_MessageDriver.hxx>
#include <TopoDS_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <BinTools_ShapeSet.hxx>
#include <TopAbs_Orientation.hxx>

#define SHAPESET "SHAPE_SECTION"
#define FORMAT_NUMBER 3
//=======================================================================
static Standard_Character EvolutionToChar(const TNaming_Evolution theEvol)
{
  switch(theEvol) {
    case TNaming_PRIMITIVE    : return 'P';
    case TNaming_GENERATED    : return 'G';
    case TNaming_MODIFY       : return 'M';
    case TNaming_DELETE       : return 'D';
    case TNaming_SELECTED     : return 'S';
      //    case TNaming_REPLACE      : return 'R';
  default:
    Standard_DomainError::Raise("TNaming_Evolution:: Evolution Unknown");
  }
  return 'P'; // To avoid compilation error message.
}

//=======================================================================
static TNaming_Evolution EvolutionToEnum(const Standard_Character theEvol)
{
  switch(theEvol) {
    case 'P': return TNaming_PRIMITIVE;
    case 'G': return TNaming_GENERATED;
    case 'M': return TNaming_MODIFY;
    case 'D': return TNaming_DELETE;
    case 'S': return TNaming_SELECTED;
    case 'R': return TNaming_MODIFY; //for compatibility //TNaming_REPLACE;
  default:
    Standard_DomainError::Raise("TNaming_Evolution:: Evolution Unknown");
  }
  return TNaming_PRIMITIVE; // To avoid compilation error message.
}
//=======================================================================
static Standard_Character OrientationToChar(const TopAbs_Orientation theOrient)
{
  switch(theOrient) {
    case TopAbs_FORWARD    : return 'F';
    case TopAbs_REVERSED   : return 'R';
    case TopAbs_INTERNAL   : return 'I';
    case TopAbs_EXTERNAL   : return 'E';
  default:
    Standard_DomainError::Raise("TopAbs_Orientation:: Orientation Unknown");
  }
  return 'F'; // To avoid compilation error message.
}
//=======================================================================
static TopAbs_Orientation CharToOrientation(const Standard_Character  theCharOrient)
{
  switch(theCharOrient) {
    case 'F':  return TopAbs_FORWARD;
    case 'R':  return TopAbs_REVERSED;
    case 'I':  return TopAbs_INTERNAL;
    case 'E':  return TopAbs_EXTERNAL;
  default:
    Standard_DomainError::Raise("TopAbs_Orientation:: Orientation Unknown");
  }
  return TopAbs_FORWARD; // To avoid compilation error message.
}

//=======================================================================
static void TranslateTo (const TopoDS_Shape&            theShape,
                         BinObjMgt_Persistent&          theResult,
                         BinTools_ShapeSet&            theShapeSet)
{
  // Check for empty shape
  if (theShape.IsNull()) {
    theResult.PutInteger(-1);
    theResult.PutInteger(-1);
    theResult.PutInteger(-1);
    return;
  }
  // Add to shape set both TShape and Location contained in <theShape>
  const Standard_Integer aTShapeID = theShapeSet.Add (theShape);
  const Standard_Integer aLocID =
    theShapeSet.Locations().Index (theShape.Location());

  // Fill theResult with shape parameters: TShape ID, Location, Orientation
  theResult << aTShapeID;
  theResult << aLocID;
  theResult << OrientationToChar(theShape.Orientation());
}
//=======================================================================
static int TranslateFrom  (const BinObjMgt_Persistent&  theSource,
                         TopoDS_Shape&                  theResult,
                         BinTools_ShapeSet&            theShapeSet)
{
  Standard_Integer aShapeID, aLocID;
  Standard_Character aCharOrient;
  Standard_Boolean Ok = theSource >> aShapeID; //TShapeID;
  if(!Ok) return 1;
  // Read TShape and Orientation
  if (aShapeID <= 0 || aShapeID > theShapeSet.NbShapes())
    return 1;
  Ok = theSource >> aLocID;
  if(!Ok) return 1;
  Ok = theSource >> aCharOrient;
  if(!Ok) return 1;
  TopAbs_Orientation anOrient = CharToOrientation(aCharOrient);

  theResult.TShape      (theShapeSet.Shape(aShapeID).TShape());//TShape
  theResult.Location    (theShapeSet.Locations().Location (aLocID)); //Location
  theResult.Orientation (anOrient);//Orientation
  return 0;
}

//=======================================================================
//function : BinMNaming_NamedShapeDriver
//purpose  : Constructor
//=======================================================================

BinMNaming_NamedShapeDriver::BinMNaming_NamedShapeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TNaming_NamedShape)->Name()), myShapeSet(Standard_False),myFormatNb(FORMAT_NUMBER)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMNaming_NamedShapeDriver::NewEmpty() const
{
  return new TNaming_NamedShape();
}

//=======================================================================
//function : Paste
//purpose  : persistent => transient (retrieve)
//=======================================================================

Standard_Boolean BinMNaming_NamedShapeDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TNaming_NamedShape) aTAtt= Handle(TNaming_NamedShape)::DownCast(theTarget);
  Standard_Integer aNbShapes;
  theSource >> aNbShapes;
  TDF_Label aLabel = theTarget->Label ();
  TNaming_Builder   aBuilder   (aLabel);
  if (aNbShapes == 0) return Standard_False;
  Standard_Integer aVer;
  Standard_Boolean ok = theSource >> aVer;
  if(!ok) return Standard_False;
  aTAtt->SetVersion(aVer); //Version
  Standard_Character aCharEvol;
  ok = theSource >> aCharEvol;
  if(!ok) return Standard_False;
  TNaming_Evolution anEvol  = EvolutionToEnum(aCharEvol); //Evolution
  aTAtt->SetVersion(anEvol);

  BinTools_ShapeSet& aShapeSet = (BinTools_ShapeSet&) myShapeSet;

  for (Standard_Integer i = 1; i <= aNbShapes; i++) {
    TopoDS_Shape anOldShape, aNewShape;
    
    if ( anEvol != TNaming_PRIMITIVE ) 
      if(TranslateFrom(theSource, anOldShape, aShapeSet)) return Standard_False;

    if (anEvol != TNaming_DELETE) 
      if(TranslateFrom(theSource, aNewShape, aShapeSet)) return Standard_False;

    switch (anEvol) {
    case TNaming_PRIMITIVE    : 
      aBuilder.Generated(aNewShape); 
      break;
    case TNaming_GENERATED    : 
      aBuilder.Generated(anOldShape, aNewShape); 
      break;
    case TNaming_MODIFY       : 
      aBuilder.Modify(anOldShape, aNewShape); 
      break;
    case TNaming_DELETE       : 
      aBuilder.Delete (anOldShape); 
      break;
    case TNaming_SELECTED     : 
      aBuilder.Select(aNewShape, anOldShape); 
      break;
      //    case TNaming_REPLACE      :
      //      aBuilder.Replace(anOldShape, aNewShape); 
      //      break;
      default :
        Standard_DomainError::Raise("TNaming_Evolution:: Evolution Unknown");
    }
    anOldShape.Nullify();
    aNewShape.Nullify();
  }
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient => persistent (store)
//=======================================================================

void BinMNaming_NamedShapeDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                         BinObjMgt_Persistent&        theTarget,
                                         BinObjMgt_SRelocationTable&  ) const
{
  Handle(TNaming_NamedShape) aSAtt= Handle(TNaming_NamedShape)::DownCast(theSource);

  //--------------------------------------------------------------
  Standard_Integer NbShapes = 0;
  for (TNaming_Iterator SItr (aSAtt); SItr.More (); SItr.Next ()) NbShapes++;
  //--------------------------------------------------------------

  if (NbShapes == 0) return;

  BinTools_ShapeSet& aShapeSet = (BinTools_ShapeSet&) myShapeSet;
  TNaming_Evolution anEvol = aSAtt->Evolution();
  
  theTarget << NbShapes;
  theTarget << aSAtt->Version();
  theTarget << EvolutionToChar(anEvol);
 

  Standard_Integer i = 1;  
  for (TNaming_Iterator SIterator(aSAtt) ;SIterator.More(); SIterator.Next()) {
    const TopoDS_Shape& OldShape = SIterator.OldShape();
    const TopoDS_Shape& NewShape = SIterator.NewShape();
    
    if ( anEvol != TNaming_PRIMITIVE ) 
      TranslateTo (OldShape, theTarget, aShapeSet); 

    if (anEvol != TNaming_DELETE) 
      TranslateTo (NewShape, theTarget, aShapeSet);
    
    i++;
  }

}


//=======================================================================
//function : WriteShapeSection
//purpose  : 
//=======================================================================

void BinMNaming_NamedShapeDriver::WriteShapeSection (Standard_OStream& theOS)
{
  theOS << SHAPESET; 
  myShapeSet.SetFormatNb(myFormatNb);
  myShapeSet.Write (theOS);
  myShapeSet.Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BinMNaming_NamedShapeDriver::Clear()
{
  myShapeSet.Clear();
}

//=======================================================================
//function : ReadShapeSection
//purpose  : 
//=======================================================================

void BinMNaming_NamedShapeDriver::ReadShapeSection (Standard_IStream& theIS)
{
  // check section title string; note that some versions of OCCT (up to 6.3.1) 
  // might avoid writing shape section if it is empty
  Standard_Size aPos = theIS.tellg();
  TCollection_AsciiString aSectionTitle;
  theIS >> aSectionTitle;
  if(aSectionTitle.Length() > 0 && aSectionTitle == SHAPESET) {
    myShapeSet.Clear();
    myShapeSet.Read (theIS);
    SetFormatNb(myShapeSet.FormatNb());
  }
  else
    theIS.seekg(aPos); // no shape section is present, try to return to initial point
}

