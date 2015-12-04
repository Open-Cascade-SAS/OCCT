// Created on: 2003-09-19
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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


#include <Aspect_TypeOfLine.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfTriangleFans.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <MeshVS_Buffer.hxx>
#include <MeshVS_DataMapIteratorOfDataMapOfIntegerVector.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_EntityType.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_VectorPrsBuilder.hxx>
#include <Precision.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_VectorPrsBuilder,MeshVS_PrsBuilder)

//================================================================
// Function : Constructor MeshVS_VectorPrsBuilder
// Purpose  :
//================================================================
MeshVS_VectorPrsBuilder::MeshVS_VectorPrsBuilder ( const Handle(MeshVS_Mesh)& Parent,
                                                   const Standard_Real MaxLength,
                                                   const Quantity_Color& VectorColor,
                                                   const MeshVS_DisplayModeFlags& Flags,
                                                   const Handle (MeshVS_DataSource)& DS,
                                                   const Standard_Integer Id,
                                                   const MeshVS_BuilderPriority& Priority,
                                                   const Standard_Boolean IsSimplePrs )
: MeshVS_PrsBuilder ( Parent, Flags, DS, Id, Priority ),
myIsSimplePrs( IsSimplePrs ),
mySimpleWidthPrm( 2.5 ),
mySimpleStartPrm( 0.85 ),
mySimpleEndPrm( 0.95 )
{
  Handle ( MeshVS_Drawer ) aDrawer = GetDrawer();
  if ( !aDrawer.IsNull() )
  {
    aDrawer->SetDouble ( MeshVS_DA_VectorMaxLength, MaxLength );
    aDrawer->SetColor  ( MeshVS_DA_VectorColor, VectorColor );
    aDrawer->SetDouble ( MeshVS_DA_VectorArrowPart, 0.1 );
  }
}

//================================================================
// Function : GetVectors
// Purpose  :
//================================================================
const MeshVS_DataMapOfIntegerVector& MeshVS_VectorPrsBuilder::GetVectors
  ( const Standard_Boolean IsElements ) const
{
  if ( IsElements )
    return myElemVectorMap;
  else
    return myNodeVectorMap;
}

//================================================================
// Function : SetVectors
// Purpose  :
//================================================================
void MeshVS_VectorPrsBuilder::SetVectors ( const Standard_Boolean IsElements,
                                           const MeshVS_DataMapOfIntegerVector& theMap )
{
  if ( IsElements )
    myElemVectorMap = theMap;
  else
    myNodeVectorMap = theMap;
}

//================================================================
// Function : HasVectors
// Purpose  :
//================================================================
Standard_Boolean MeshVS_VectorPrsBuilder::HasVectors ( const Standard_Boolean IsElement ) const
{
  Standard_Boolean aRes = (myNodeVectorMap.Extent()>0);
  if ( IsElement )
    aRes = (myElemVectorMap.Extent()>0);
  return aRes;

}

//================================================================
// Function : GetVector
// Purpose  :
//================================================================
Standard_Boolean MeshVS_VectorPrsBuilder::GetVector ( const Standard_Boolean IsElement,
                                                      const Standard_Integer ID,
                                                      gp_Vec& Vect ) const
{
  const MeshVS_DataMapOfIntegerVector* aMap = &myNodeVectorMap;
  if ( IsElement )
    aMap = &myElemVectorMap;

  Standard_Boolean aRes = aMap->IsBound ( ID );
  if ( aRes )
    Vect = aMap->Find ( ID );

  return aRes;
}

//================================================================
// Function : SetVector
// Purpose  :
//================================================================
void MeshVS_VectorPrsBuilder::SetVector ( const Standard_Boolean IsElement,
                                          const Standard_Integer ID,
                                          const gp_Vec& Vect )
{
  MeshVS_DataMapOfIntegerVector* aMap = &myNodeVectorMap;
  if ( IsElement )
    aMap = &myElemVectorMap;

  Standard_Boolean aRes = aMap->IsBound ( ID );
  if ( aRes )
    aMap->ChangeFind ( ID ) = Vect;
  else
    aMap->Bind ( ID, Vect );
}

//================================================================
// Function : GetMaxVectorValue
// Purpose  :
//================================================================
void MeshVS_VectorPrsBuilder::GetMinMaxVectorValue ( const Standard_Boolean IsElement,
                                                     Standard_Real& MinValue,
                                                     Standard_Real& MaxValue ) const
{
  const MeshVS_DataMapOfIntegerVector* aMap = &myNodeVectorMap;
  if ( IsElement )
    aMap = &myElemVectorMap;

  MeshVS_DataMapIteratorOfDataMapOfIntegerVector anIt ( *aMap );
  if ( anIt.More() )
    MinValue = MaxValue = anIt.Value().Magnitude();

  Standard_Real aCurValue;

  for ( ; anIt.More(); anIt.Next() )
  {
    aCurValue = anIt.Value().Magnitude();
    if ( MinValue > aCurValue )
      MinValue = aCurValue;
    if ( MaxValue < aCurValue )
      MaxValue = aCurValue;
  }
}

//================================================================
// Function : Build
// Purpose  :
//================================================================

#define NB_VERTICES 2
#define NB_BOUNDS 1
#define NB_TRIANGLES 6
#define NB_FANS 1

void MeshVS_VectorPrsBuilder::Build ( const Handle(Prs3d_Presentation)& Prs,
                                      const TColStd_PackedMapOfInteger& IDs,
                                      TColStd_PackedMapOfInteger& IDsToExclude,
                                      const Standard_Boolean IsElement,
                                      const Standard_Integer theDisplayMode ) const
{
  Handle ( MeshVS_Drawer ) aDrawer = GetDrawer();
  Handle (MeshVS_DataSource) aSource = GetDataSource();
  if ( aSource.IsNull() || aDrawer.IsNull() || !HasVectors( IsElement ) ||
       ( theDisplayMode & GetFlags() )==0 )
    return;

  Standard_Integer aMaxFaceNodes;
  Standard_Real aMaxLen, anArrowPart = 0.1;

  if ( !aDrawer->GetInteger ( MeshVS_DA_MaxFaceNodes, aMaxFaceNodes ) ||
       aMaxFaceNodes <= 0 ||
       !aDrawer->GetDouble  ( MeshVS_DA_VectorMaxLength, aMaxLen )    ||
       aMaxLen <= 0       ||
       !aDrawer->GetDouble ( MeshVS_DA_VectorArrowPart, anArrowPart ) ||
       anArrowPart <= 0
     )
    return;

  MeshVS_Buffer aCoordsBuf (3*aMaxFaceNodes*sizeof(Standard_Real));
  TColStd_Array1OfReal aCoords (aCoordsBuf, 1, 3*aMaxFaceNodes);
  Standard_Integer NbNodes;
  MeshVS_EntityType aType;

  // DECLARE ARRAYS OF PRIMITIVES
  const MeshVS_DataMapOfIntegerVector& aMap = GetVectors ( IsElement );
  Standard_Integer aNbVectors = aMap.Extent();

  if ( aNbVectors <= 0 )
    return;

  // polylines
  Standard_Integer aNbVertices = aNbVectors  * NB_VERTICES;

  Handle(Graphic3d_ArrayOfPrimitives) aLineArray = new Graphic3d_ArrayOfSegments (aNbVertices);
  Handle(Graphic3d_ArrayOfPrimitives) aArrowLineArray = new Graphic3d_ArrayOfSegments (aNbVertices);

  Handle(Graphic3d_ArrayOfPrimitives) aTriangleArray = new Graphic3d_ArrayOfSegments (
    aNbVectors * 8 /* vertices per arrow */, aNbVectors * 12 /* segments per arrow */ * 2 /* indices per segment */);

  TColgp_Array1OfPnt anArrowPnt(1,8);
  Standard_Real k, b, aMaxValue, aMinValue, aValue, X, Y, Z;
  
  Standard_Real aMinLength = calculateArrow( anArrowPnt, aMaxLen, anArrowPart );
  gp_Vec aVec; gp_Trsf aTrsf;

  GetMinMaxVectorValue ( IsElement, aMinValue, aMaxValue );

  if ( aMaxValue - aMinValue > Precision::Confusion() )
  {
    k = 0.8 * aMaxLen / ( aMaxValue - aMinValue );
    b = aMaxLen - k * aMaxValue;
  }
  else
  {
    k = 0;
    b = aMaxLen;
  }

  TColStd_PackedMapOfInteger aCustomElements;

  // subtract the hidden elements and ids to exclude (to minimize allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign( IDs );
  if ( IsElement )
  {
    Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
    if ( !aHiddenElems.IsNull() )
      anIDs.Subtract( aHiddenElems->Map() );
  }
  anIDs.Subtract( IDsToExclude );

  TColStd_MapIteratorOfPackedMapOfInteger it (anIDs);
  for( ; it.More(); it.Next() )
  {
    Standard_Integer aKey = it.Key();
      if( GetVector ( IsElement, aKey, aVec ) )
      {
        aValue = aVec.Magnitude();

        if ( Abs( aValue ) < Precision::Confusion() )
          continue;

        if( aSource->GetGeom ( aKey, IsElement, aCoords, NbNodes, aType ) )
        {
          if( aType == MeshVS_ET_Node )
          {
            X = aCoords(1);
            Y = aCoords(2);
            Z = aCoords(3);
          }
          else if( aType == MeshVS_ET_Link || 
                   aType == MeshVS_ET_Face || 
                   aType == MeshVS_ET_Volume )
          {
            if( IsElement && IsExcludingOn() )
              IDsToExclude.Add( aKey );
            X = Y = Z = 0;
            for ( Standard_Integer i=1; i<=NbNodes; i++ )
            {
              X += aCoords (3*i-2);
              Y += aCoords (3*i-1);
              Z += aCoords (3*i);
            }
            X /= Standard_Real ( NbNodes );
            Y /= Standard_Real ( NbNodes );
            Z /= Standard_Real ( NbNodes );
          }
          else
          {
            aCustomElements.Add( aKey );
            continue;
          }

          aTrsf.SetDisplacement ( gp_Ax3 ( gp_Pnt ( 0, 0, 0 ), gp_Dir(0, 0, 1)),
                                  gp_Ax3 ( gp_Pnt ( X, Y, Z ), aVec ) );

          DrawVector ( aTrsf, Max( k * fabs( aValue ) + b, aMinLength), aMaxLen,
                       anArrowPnt, aLineArray, aArrowLineArray, aTriangleArray );
        }
      }
  }

  Prs3d_Root::NewGroup ( Prs );
  Handle (Graphic3d_Group) aVGroup = Prs3d_Root::CurrentGroup ( Prs );

  Quantity_Color aColor;
  aDrawer->GetColor ( MeshVS_DA_VectorColor, aColor );

  // Add primitive arrays to group
  Handle(Graphic3d_AspectLine3d) aLinAspect =
    new Graphic3d_AspectLine3d ( aColor, Aspect_TOL_SOLID, 1.5 );

  aVGroup->SetPrimitivesAspect( aLinAspect );
  aVGroup->AddPrimitiveArray( aLineArray );

  if ( !myIsSimplePrs )
  {
    Handle(Graphic3d_AspectLine3d) anArrowLinAspect =
      new Graphic3d_AspectLine3d (aColor, Aspect_TOL_SOLID, mySimpleWidthPrm);

    aVGroup->SetPrimitivesAspect( anArrowLinAspect );
    aVGroup->AddPrimitiveArray( aTriangleArray );
  }
  else
  {
    Handle(Graphic3d_AspectLine3d) anArrowLinAspect =
      new Graphic3d_AspectLine3d ( aColor, Aspect_TOL_SOLID, mySimpleWidthPrm * 1.5 );

    aVGroup->SetPrimitivesAspect( anArrowLinAspect );
    aVGroup->AddPrimitiveArray( aArrowLineArray );
  }

  if( !aCustomElements.IsEmpty() )
    CustomBuild( Prs, aCustomElements, IDsToExclude, theDisplayMode );
}

//=======================================================================
// name    : DrawVector
// Purpose : Fill arrays of primitives for drawing force
//=======================================================================
void MeshVS_VectorPrsBuilder::DrawVector ( const gp_Trsf& theTrsf,
                                           const Standard_Real theLength,
                                           const Standard_Real theMaxLength,
                                           const TColgp_Array1OfPnt& theArrowPoints,
                                           const Handle(Graphic3d_ArrayOfPrimitives)& theLines,
                                           const Handle(Graphic3d_ArrayOfPrimitives)& theArrowLines,
                                           const Handle(Graphic3d_ArrayOfPrimitives)& theTriangles) const
{
  const int PntNum = 8;

  const Standard_Real aMinLength = theMaxLength * ( 1 - mySimpleStartPrm );
  const Standard_Real aLocalLength = ( !myIsSimplePrs || theLength > aMinLength ? theLength : aMinLength );
  // draw line
  gp_Pnt aLinePnt[ 2 ] = { gp_Pnt( 0, 0, 0 ) , gp_Pnt( 0, 0, aLocalLength ) };
  theTrsf.Transforms (aLinePnt[0].ChangeCoord());
  theTrsf.Transforms (aLinePnt[1].ChangeCoord());

  theLines->AddVertex (aLinePnt[0]);
  theLines->AddVertex (aLinePnt[1]);

  // draw arrow
  if (!myIsSimplePrs)
  {
    Standard_Integer aLower = theArrowPoints.Lower(),
                     aUpper = theArrowPoints.Upper();

    if (aUpper - aLower < PntNum - 1)
      return;

    TColgp_Array1OfPnt anArrowPnt (aLower, aUpper);
    for (Standard_Integer aPntIdx = aLower; aPntIdx < aLower + PntNum; ++aPntIdx)
    {
      anArrowPnt (aPntIdx).ChangeCoord() = theArrowPoints (aPntIdx).Coord() + gp_XYZ (0, 0, aLocalLength);
      theTrsf.Transforms (anArrowPnt (aPntIdx).ChangeCoord());
    }

    const Standard_Integer aVrtOffset = theTriangles->VertexNumber() + 1;

    for (Standard_Integer aPntIdx = 0; aPntIdx < PntNum; ++aPntIdx)
    {
      theTriangles->AddVertex (anArrowPnt (aLower + aPntIdx));
    }

    for (Standard_Integer aPntIdx = 1; aPntIdx <= PntNum - 2; ++aPntIdx)
    {
      theTriangles->AddEdge (aVrtOffset);
      theTriangles->AddEdge (aVrtOffset + aPntIdx);

      theTriangles->AddEdge (aVrtOffset + aPntIdx);
      theTriangles->AddEdge (aVrtOffset + aPntIdx + 1);
    }
  }
  else
  {
    const Standard_Real aEndPos = aLocalLength - theMaxLength * ( 1 - mySimpleEndPrm );
    const Standard_Real aArrowLength = theMaxLength * ( mySimpleEndPrm - mySimpleStartPrm );
    gp_Pnt aArrowPnt[ 2 ] = { gp_Pnt( 0, 0, aEndPos - aArrowLength ),
                              gp_Pnt( 0, 0, aEndPos ) };
    theTrsf.Transforms (aArrowPnt[0].ChangeCoord());
    theTrsf.Transforms (aArrowPnt[1].ChangeCoord());

    theArrowLines->AddVertex (aArrowPnt[0]);
    theArrowLines->AddVertex (aArrowPnt[1]);
  }
}

//=======================================================================
// name    : calculateArrow
// Purpose : Calculate points of arrow ( 8 pnts )
//=======================================================================
Standard_Real MeshVS_VectorPrsBuilder::calculateArrow ( TColgp_Array1OfPnt& Points,
                                                        const Standard_Real Length,
                                                        const Standard_Real ArrowPart )
{
  Standard_Real h = Length * ArrowPart;
  Standard_Real w = h / 5.;

  Standard_Integer f = Points.Lower();
  Points( f   ) = gp_Pnt( 0,  0, 0 );
  Points( f+1 ) = gp_Pnt( 0, -w, -h );
  Points( f+2 ) = gp_Pnt( w * 0.866, -w * 0.5, -h );
  Points( f+3 ) = gp_Pnt( w * 0.866,  w * 0.5, -h );
  Points( f+4 ) = gp_Pnt( 0 , w, -h );
  Points( f+5 ) = gp_Pnt( -w * 0.866,  w * 0.5, -h );
  Points( f+6 ) = gp_Pnt( -w * 0.866, -w * 0.5, -h );
  Points( f+7 ) = gp_Pnt( 0, -w, -h );

  return h;
}

//=======================================================================
// name    : SetSimplePrsMode
// Purpose : 
//=======================================================================
void MeshVS_VectorPrsBuilder::SetSimplePrsMode( const Standard_Boolean IsSimpleArrow )
{
  myIsSimplePrs = IsSimpleArrow;
}

//=======================================================================
// name    : SetSimplePrsParams
// Purpose : 
//=======================================================================
void MeshVS_VectorPrsBuilder::SetSimplePrsParams( const Standard_Real theLineWidthParam,
                                                  const Standard_Real theStartParam,
                                                  const Standard_Real theEndParam )
{
  mySimpleWidthPrm = theLineWidthParam;
  mySimpleStartPrm = theStartParam;
  mySimpleEndPrm   = theEndParam;
}
