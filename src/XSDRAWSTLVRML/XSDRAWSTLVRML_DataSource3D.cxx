// Created on: 2014-08-04
// Created by: Artem NOVIKOV
// Copyright (c) 2014 OPEN CASCADE SAS
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


#include <Standard_Type.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapOfIntegerReal.hxx>
#include <XSDRAWSTLVRML_DataSource3D.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSDRAWSTLVRML_DataSource3D,MeshVS_DataSource)

//================================================================
// Function : Constructor
// Purpose  :
//================================================================
XSDRAWSTLVRML_DataSource3D::XSDRAWSTLVRML_DataSource3D()
{
  for (Standard_Integer aNodeID = 1; aNodeID <= 16; aNodeID++)
  {
    myNodes.Add( aNodeID );
  }

  for (Standard_Integer anElemID = 1; anElemID <= 5; anElemID++)
  {
    myElements.Add( anElemID );
  }

  myNodeCoords = new TColStd_HArray2OfReal(1, 16, 1, 3);

  myNodeCoords->SetValue( 1, 1, 5 );
  myNodeCoords->SetValue( 1, 2, 5 );
  myNodeCoords->SetValue( 1, 3, 20 );

  myNodeCoords->SetValue( 2, 1, 0 );
  myNodeCoords->SetValue( 2, 2, 10 );
  myNodeCoords->SetValue( 2, 3, 10 );

  myNodeCoords->SetValue( 3, 1, 10 );
  myNodeCoords->SetValue( 3, 2, 0 );
  myNodeCoords->SetValue( 3, 3, 10 );

  myNodeCoords->SetValue( 4, 1, 0 );
  myNodeCoords->SetValue( 4, 2, 0 );
  myNodeCoords->SetValue( 4, 3, 10 );

  myNodeCoords->SetValue( 5, 1, -10 );
  myNodeCoords->SetValue( 5, 2, 0 );
  myNodeCoords->SetValue( 5, 3, 10 );

  myNodeCoords->SetValue( 6, 1, -10 );
  myNodeCoords->SetValue( 6, 2, 10 );
  myNodeCoords->SetValue( 6, 3, 10 );

  myNodeCoords->SetValue( 7, 1, -10 );
  myNodeCoords->SetValue( 7, 2, 10 );
  myNodeCoords->SetValue( 7, 3, 0 );

  myNodeCoords->SetValue( 8, 1, -10 );
  myNodeCoords->SetValue( 8, 2, 0 );
  myNodeCoords->SetValue( 8, 3, 0 );

  myNodeCoords->SetValue( 9, 1, 0 );
  myNodeCoords->SetValue( 9, 2, 0 );
  myNodeCoords->SetValue( 9, 3, 0 );

  myNodeCoords->SetValue( 10, 1, 0 );
  myNodeCoords->SetValue( 10, 2, 10 );
  myNodeCoords->SetValue( 10, 3, 0 );

  myNodeCoords->SetValue( 11, 1, 0 );
  myNodeCoords->SetValue( 11, 2, -10 );
  myNodeCoords->SetValue( 11, 3, 10 );

  myNodeCoords->SetValue( 12, 1, 10 );
  myNodeCoords->SetValue( 12, 2, -10 );
  myNodeCoords->SetValue( 12, 3, 10 );

  myNodeCoords->SetValue( 13, 1, 10 );
  myNodeCoords->SetValue( 13, 2, -10 );
  myNodeCoords->SetValue( 13, 3, 0 );

  myNodeCoords->SetValue( 14, 1, 0 );
  myNodeCoords->SetValue( 14, 2, -10 );
  myNodeCoords->SetValue( 14, 3, 0 );

  myNodeCoords->SetValue( 15, 1, 10 );
  myNodeCoords->SetValue( 15, 2, 0 );
  myNodeCoords->SetValue( 15, 3, 0 );

  myNodeCoords->SetValue( 16, 1, 5 );
  myNodeCoords->SetValue( 16, 2, 5 );
  myNodeCoords->SetValue( 16, 3, -10 );

  myElemNbNodes = new TColStd_HArray1OfInteger(1, 5);

  myElemNbNodes->SetValue( 1, 4 );
  myElemNbNodes->SetValue( 2, 8 );
  myElemNbNodes->SetValue( 3, 6 );
  myElemNbNodes->SetValue( 4, 8 );
  myElemNbNodes->SetValue( 5, 4 );

  myElemNodes = new TColStd_HArray2OfInteger(1, 5, 1, 8);

  myElemNodes->SetValue(1, 1, 1);
  myElemNodes->SetValue(1, 2, 2);
  myElemNodes->SetValue(1, 3, 3);
  myElemNodes->SetValue(1, 4, 4);

  myElemNodes->SetValue(2, 1, 2);
  myElemNodes->SetValue(2, 2, 4);
  myElemNodes->SetValue(2, 3, 5);
  myElemNodes->SetValue(2, 4, 6);
  myElemNodes->SetValue(2, 5, 7);
  myElemNodes->SetValue(2, 6, 8);
  myElemNodes->SetValue(2, 7, 9);
  myElemNodes->SetValue(2, 8, 10);

  myElemNodes->SetValue(3, 1, 2);
  myElemNodes->SetValue(3, 2, 3);
  myElemNodes->SetValue(3, 3, 4);
  myElemNodes->SetValue(3, 4, 10);
  myElemNodes->SetValue(3, 5, 15);
  myElemNodes->SetValue(3, 6, 9);

  myElemNodes->SetValue(4, 1, 4);
  myElemNodes->SetValue(4, 2, 3);
  myElemNodes->SetValue(4, 3, 12);
  myElemNodes->SetValue(4, 4, 11);
  myElemNodes->SetValue(4, 5, 14);
  myElemNodes->SetValue(4, 6, 13);
  myElemNodes->SetValue(4, 7, 15);
  myElemNodes->SetValue(4, 8, 9);

  myElemNodes->SetValue(5, 1, 16);
  myElemNodes->SetValue(5, 2, 15);
  myElemNodes->SetValue(5, 3, 10);
  myElemNodes->SetValue(5, 4, 9);
}

//================================================================
// Function : GetGeom
// Purpose  :
//================================================================
Standard_Boolean XSDRAWSTLVRML_DataSource3D::GetGeom
( const Standard_Integer theID, const Standard_Boolean theIsElement,
 TColStd_Array1OfReal& theCoords, Standard_Integer& theNbNodes,
 MeshVS_EntityType& theType ) const
{
  if (theIsElement)
  {
    if (theID >= 1 && theID <= myElements.Extent())
    {
      theType = MeshVS_ET_Volume;
      theNbNodes = myElemNbNodes->Value(theID);

      for (Standard_Integer aNodeI = 1, aGlobCoordI = 1; aNodeI <= theNbNodes; aNodeI++)
      {
        Standard_Integer anIdxNode = myElemNodes->Value(theID, aNodeI);
        for(Standard_Integer aCoordI = 1; aCoordI <= 3; aCoordI++, aGlobCoordI++ )
          theCoords(aGlobCoordI) = myNodeCoords->Value(anIdxNode, aCoordI);
      }

      return Standard_True;
    }
    else
      return Standard_False;
  }
  else
    if (theID >= 1 && theID <= myNodes.Extent())
    {
      theType = MeshVS_ET_Node;
      theNbNodes = 1;

      theCoords( 1 ) = myNodeCoords->Value(theID, 1);
      theCoords( 2 ) = myNodeCoords->Value(theID, 2);
      theCoords( 3 ) = myNodeCoords->Value(theID, 3);
      return Standard_True;
    }
    else
      return Standard_False;
}

//================================================================
// Function : Get3DGeom
// Purpose  :
//================================================================
Standard_Boolean XSDRAWSTLVRML_DataSource3D::Get3DGeom
( const Standard_Integer theID, Standard_Integer& theNbNodes,
 Handle(MeshVS_HArray1OfSequenceOfInteger)& theData ) const
{
  Handle(MeshVS_HArray1OfSequenceOfInteger) aMeshData;
  if (theID == 1 || theID == 5)
  {
    aMeshData = new MeshVS_HArray1OfSequenceOfInteger(1,4);
    theNbNodes = 4;
    for (Standard_Integer anElemI = 1; anElemI <= 4; anElemI++)
    {
      aMeshData->ChangeValue(anElemI).Append( (anElemI - 1) % 4 );
      aMeshData->ChangeValue(anElemI).Append( anElemI % 4 );
      aMeshData->ChangeValue(anElemI).Append( (anElemI + 1) % 4 );
    }
    theData = aMeshData;
    return Standard_True;
  }

  if (theID == 2 || theID == 4)
  {
    aMeshData = new MeshVS_HArray1OfSequenceOfInteger(1,6);
    theNbNodes = 8;
    for (Standard_Integer anElemI = 1, k = 1; anElemI <= 4; anElemI++)
    {
      aMeshData->ChangeValue(anElemI).Append( (k - 1) % 8 );
      aMeshData->ChangeValue(anElemI).Append( k % 8 );
      aMeshData->ChangeValue(anElemI).Append( (k + 1) % 8 );
      aMeshData->ChangeValue(anElemI).Append( (k + 2) % 8 );
      k+=2;
    }

    aMeshData->ChangeValue(5).Append( 0 );
    aMeshData->ChangeValue(5).Append( 3 );
    aMeshData->ChangeValue(5).Append( 4 );
    aMeshData->ChangeValue(5).Append( 7 );

    aMeshData->ChangeValue(6).Append( 1 );
    aMeshData->ChangeValue(6).Append( 2 );
    aMeshData->ChangeValue(6).Append( 5 );
    aMeshData->ChangeValue(6).Append( 6 );

    theData = aMeshData;
    return Standard_True;
  }

  if (theID == 3)
  {
    aMeshData = new MeshVS_HArray1OfSequenceOfInteger(1,5);
    theNbNodes = 6;
    for (Standard_Integer anElemI = 1; anElemI <= 2; anElemI++)
    {
      aMeshData->ChangeValue(anElemI).Append( (anElemI - 1) * 3 );
      aMeshData->ChangeValue(anElemI).Append( (anElemI - 1) * 3 + 1 );
      aMeshData->ChangeValue(anElemI).Append( (anElemI - 1) * 3 + 2 );
    }
    for (Standard_Integer anElemI = 1; anElemI <= 3; anElemI++)
    {
      aMeshData->ChangeValue(2 + anElemI).Append( (anElemI - 1) % 3 );
      aMeshData->ChangeValue(2 + anElemI).Append( anElemI % 3 );
      aMeshData->ChangeValue(2 + anElemI).Append( anElemI % 3 + 3 );
      aMeshData->ChangeValue(2 + anElemI).Append( (anElemI - 1) % 3 + 3 );
    }
    theData = aMeshData;
    return Standard_True;
  }

  return Standard_False;
}

//================================================================
// Function : GetGeomType
// Purpose  :
//================================================================
Standard_Boolean XSDRAWSTLVRML_DataSource3D::GetGeomType
( const Standard_Integer theID,
 const Standard_Boolean theIsElement,
 MeshVS_EntityType& theType ) const
{
  if (theIsElement)
  {
    if (theID >= 1 && theID <= myElements.Extent())
    {
      theType = MeshVS_ET_Volume;
      return Standard_True;
    }
  }
  else
    if (theID >= 1 && theID <= myNodes.Extent())
    {
      theType = MeshVS_ET_Node;
      return Standard_True;
    }

  return Standard_False;
}

//================================================================
// Function : GetAddr
// Purpose  :
//================================================================
Standard_Address XSDRAWSTLVRML_DataSource3D::GetAddr
( const Standard_Integer, const Standard_Boolean ) const
{
  return NULL;
}

//================================================================
// Function : GetNodesByElement
// Purpose  :
//================================================================
Standard_Boolean XSDRAWSTLVRML_DataSource3D::GetNodesByElement
( const Standard_Integer theID,
 TColStd_Array1OfInteger& theNodeIDs,
 Standard_Integer& theNbNodes ) const
{
  Standard_Integer aLow;
  if (theID == 1 || theID == 5)
  {
    theNbNodes = 4;
    aLow = theNodeIDs.Lower();
    theNodeIDs (aLow)     = myElemNodes->Value(theID, 1 );
    theNodeIDs (aLow + 1) = myElemNodes->Value(theID, 2 );
    theNodeIDs (aLow + 2) = myElemNodes->Value(theID, 3 );
    theNodeIDs (aLow + 3) = myElemNodes->Value(theID, 4 );
    return Standard_True;
  }

  if (theID == 2 || theID == 4)
  {
    theNbNodes = 8;
    aLow = theNodeIDs.Lower();
    theNodeIDs (aLow)     = myElemNodes->Value(theID, 1 );
    theNodeIDs (aLow + 1) = myElemNodes->Value(theID, 2 );
    theNodeIDs (aLow + 2) = myElemNodes->Value(theID, 3 );
    theNodeIDs (aLow + 3) = myElemNodes->Value(theID, 4 );
    theNodeIDs (aLow + 4) = myElemNodes->Value(theID, 5 );
    theNodeIDs (aLow + 5) = myElemNodes->Value(theID, 6 );
    theNodeIDs (aLow + 6) = myElemNodes->Value(theID, 7 );
    theNodeIDs (aLow + 7) = myElemNodes->Value(theID, 8 );
    return Standard_True;
  }

    if (theID == 3)
  {
    theNbNodes = 6;
    aLow = theNodeIDs.Lower();
    theNodeIDs (aLow)     = myElemNodes->Value(theID, 1 );
    theNodeIDs (aLow + 1) = myElemNodes->Value(theID, 2 );
    theNodeIDs (aLow + 2) = myElemNodes->Value(theID, 3 );
    theNodeIDs (aLow + 3) = myElemNodes->Value(theID, 4 );
    theNodeIDs (aLow + 4) = myElemNodes->Value(theID, 5 );
    theNodeIDs (aLow + 5) = myElemNodes->Value(theID, 6 );
    return Standard_True;
  }

  return Standard_False;
}

//================================================================
// Function : GetAllNodes
// Purpose  :
//================================================================
const TColStd_PackedMapOfInteger& XSDRAWSTLVRML_DataSource3D::GetAllNodes() const
{
  return myNodes;
}

//================================================================
// Function : GetAllElements
// Purpose  :
//================================================================
const TColStd_PackedMapOfInteger& XSDRAWSTLVRML_DataSource3D::GetAllElements() const
{
  return myElements;
}

//================================================================
// Function : GetNormal
// Purpose  :
//================================================================
Standard_Boolean XSDRAWSTLVRML_DataSource3D::GetNormal
( const Standard_Integer /*theID*/, const Standard_Integer /*theMax*/,
 Standard_Real& /*theNx*/, Standard_Real& /*theNy*/,Standard_Real& /*theNz*/ ) const
{
  return Standard_False;
}
