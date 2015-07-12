// Created on: 2003-12-17
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


#include <Font_NameOfFont.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_Tool.hxx>
#include <Precision.hxx>

//================================================================
// Function : CreateAspectFillArea3d
// Purpose  :
//================================================================
Handle( Graphic3d_AspectFillArea3d ) MeshVS_Tool::CreateAspectFillArea3d
(  const Handle(MeshVS_Drawer)& theDr,
   const Graphic3d_MaterialAspect& Mat,
   const Standard_Boolean UseDefaults )
{
  Handle( Graphic3d_AspectFillArea3d ) anAsp;
  if ( theDr.IsNull() )
    return anAsp;

  Aspect_InteriorStyle     anIntStyle    = Aspect_IS_EMPTY;
  Quantity_Color           anIntColor    = Quantity_NOC_CYAN1,
                           anEdgeColor   = Quantity_NOC_WHITE;
  Aspect_TypeOfLine        anEdgeType    = Aspect_TOL_SOLID;
  Standard_Real            anEdgeWidth   = 1.0;
  Aspect_HatchStyle        aHStyle       = Aspect_HS_HORIZONTAL;
  Graphic3d_MaterialAspect aFrMat        = Mat,
                           aBackMat      = Mat;

  Standard_Integer         anIntStyleI   = (Standard_Integer)Aspect_IS_EMPTY;
  Standard_Integer         anEdgeTypeI   = (Standard_Integer)Aspect_TOL_SOLID;
  Standard_Integer         aHStyleI      = (Standard_Integer)Aspect_HS_HORIZONTAL;

  if ( !theDr->GetColor ( MeshVS_DA_InteriorColor, anIntColor ) && !UseDefaults )
    return anAsp;

  Quantity_Color aBackIntColor = anIntColor;
  if ( !theDr->GetColor ( MeshVS_DA_BackInteriorColor, aBackIntColor ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetColor ( MeshVS_DA_EdgeColor, anEdgeColor ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetDouble ( MeshVS_DA_EdgeWidth, anEdgeWidth ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetInteger ( MeshVS_DA_InteriorStyle, anIntStyleI ) && !UseDefaults )
    return anAsp;
  else
    anIntStyle = (Aspect_InteriorStyle) anIntStyleI;

  if ( !theDr->GetInteger ( MeshVS_DA_EdgeType, anEdgeTypeI ) && !UseDefaults )
    return anAsp;
  else
    anEdgeType = (Aspect_TypeOfLine) anEdgeTypeI;

  if ( !theDr->GetInteger ( MeshVS_DA_HatchStyle, aHStyleI ) && !UseDefaults )
    return anAsp;
  else
    aHStyle = (Aspect_HatchStyle) aHStyleI;

  anAsp = new Graphic3d_AspectFillArea3d ( anIntStyle, anIntColor, anEdgeColor, anEdgeType,
    anEdgeWidth, aFrMat, aBackMat );
  anAsp->SetBackInteriorColor ( aBackIntColor );
  anAsp->SetHatchStyle ( aHStyle );

  return anAsp;
}

//================================================================
// Function : CreateAspectFillArea3d
// Purpose  :
//================================================================
Handle( Graphic3d_AspectFillArea3d ) MeshVS_Tool::CreateAspectFillArea3d
(  const Handle(MeshVS_Drawer)& theDr,
   const Standard_Boolean UseDefaults )
{
  Graphic3d_MaterialAspect aFrMat   = Graphic3d_NOM_BRASS;
  Graphic3d_MaterialAspect aBackMat = Graphic3d_NOM_BRASS;
  Standard_Integer aFrMatI = (Standard_Integer)Graphic3d_NOM_BRASS;
  Standard_Integer aBackMatI = (Standard_Integer)Graphic3d_NOM_BRASS;

  if ( !theDr->GetInteger ( MeshVS_DA_FrontMaterial, aFrMatI ) && !UseDefaults )
    return 0;
  else
    aFrMat = (Graphic3d_MaterialAspect)(Graphic3d_NameOfMaterial)aFrMatI;

  if ( !theDr->GetInteger ( MeshVS_DA_BackMaterial, aBackMatI ) && !UseDefaults )
    return 0;
  else
    aBackMat = (Graphic3d_MaterialAspect)(Graphic3d_NameOfMaterial)aBackMatI;

  Handle( Graphic3d_AspectFillArea3d ) aFill =
    CreateAspectFillArea3d ( theDr, aFrMat, UseDefaults );
  aFill->SetBackMaterial ( aBackMat );

  return aFill;
}
//================================================================
// Function : CreateAspectLine3d
// Purpose  :
//================================================================
Handle( Graphic3d_AspectLine3d ) MeshVS_Tool::CreateAspectLine3d
(  const Handle(MeshVS_Drawer)& theDr,
 const Standard_Boolean UseDefaults )
{
  Handle( Graphic3d_AspectLine3d ) anAsp;
  if ( theDr.IsNull() )
    return anAsp;

  Quantity_Color           aBeamColor   = Quantity_NOC_YELLOW;
  Aspect_TypeOfLine        aBeamType    = Aspect_TOL_SOLID;
  Standard_Real            aBeamWidth   = 1.0;
  Standard_Integer         aBeamTypeI   = (Standard_Integer)Aspect_TOL_SOLID;

  if ( !theDr->GetColor ( MeshVS_DA_BeamColor, aBeamColor ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetDouble ( MeshVS_DA_BeamWidth, aBeamWidth ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetInteger ( MeshVS_DA_BeamType, aBeamTypeI ) && !UseDefaults )
    return anAsp;
  else
    aBeamType = (Aspect_TypeOfLine) aBeamTypeI;

  anAsp = new Graphic3d_AspectLine3d ( aBeamColor, aBeamType, aBeamWidth );

  return anAsp;
}

//================================================================
// Function : CreateAspectMarker3d
// Purpose  :
//================================================================
Handle( Graphic3d_AspectMarker3d ) MeshVS_Tool::CreateAspectMarker3d
(  const Handle(MeshVS_Drawer)& theDr,
 const Standard_Boolean UseDefaults )
{
  Handle( Graphic3d_AspectMarker3d ) anAsp;
  if ( theDr.IsNull() )
    return anAsp;

  Quantity_Color           aMColor   = Quantity_NOC_YELLOW;
  Aspect_TypeOfMarker      aMType    = Aspect_TOM_X;
  Standard_Real            aMScale   = 1.0;
  Standard_Integer         aMTypeI   = (Standard_Integer)Aspect_TOM_X;

  if ( !theDr->GetColor ( MeshVS_DA_MarkerColor, aMColor ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetDouble ( MeshVS_DA_MarkerScale, aMScale ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetInteger ( MeshVS_DA_MarkerType, aMTypeI ) && !UseDefaults )
    return anAsp;
  else
    aMType = (Aspect_TypeOfMarker) aMTypeI;

  anAsp = new Graphic3d_AspectMarker3d ( aMType, aMColor, aMScale );

  return anAsp;
}

//================================================================
// Function : CreateAspectText3d
// Purpose  :
//================================================================
Handle( Graphic3d_AspectText3d ) MeshVS_Tool::CreateAspectText3d
(  const Handle(MeshVS_Drawer)& theDr,
   const Standard_Boolean UseDefaults )
{
  Handle( Graphic3d_AspectText3d ) anAsp;
  if ( theDr.IsNull() )
    return anAsp;

  Quantity_Color            aTColor       = Quantity_NOC_YELLOW;
  Standard_Real             anExpFactor   = 1.0,
                            aSpace        = 0.0;
  Standard_CString          aFont         = Font_NOF_ASCII_MONO;
  Aspect_TypeOfStyleText    aStyle        = Aspect_TOST_NORMAL;
  Aspect_TypeOfDisplayText  aDispText     = Aspect_TODT_NORMAL;
  TCollection_AsciiString   aFontString   = Font_NOF_ASCII_MONO;
  Font_FontAspect           aFontAspect   = Font_FA_Regular;
  Standard_Integer          aStyleI       = (Standard_Integer)Aspect_TOST_NORMAL;
  Standard_Integer          aDispTextI    = (Standard_Integer)Aspect_TODT_NORMAL;
  // Bold font is used by default for better text readability
  Standard_Integer          aFontAspectI  = (Standard_Integer)Font_FA_Bold;

  if ( !theDr->GetColor ( MeshVS_DA_TextColor, aTColor ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetDouble ( MeshVS_DA_TextExpansionFactor, anExpFactor ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetDouble ( MeshVS_DA_TextSpace, aSpace ) && !UseDefaults )
    return anAsp;

  if ( !theDr->GetAsciiString ( MeshVS_DA_TextFont, aFontString ) && !UseDefaults )
    return anAsp;
  else
    aFont = aFontString.ToCString();

  if ( !theDr->GetInteger ( MeshVS_DA_TextStyle, aStyleI ) && !UseDefaults )
    return anAsp;
  else
    aStyle = (Aspect_TypeOfStyleText) aStyleI;

  if ( !theDr->GetInteger ( MeshVS_DA_TextDisplayType, aDispTextI ) && !UseDefaults )
    return anAsp;
  else
    aDispText = (Aspect_TypeOfDisplayText) aDispTextI;

  if ( !theDr->GetInteger ( MeshVS_DA_TextFontAspect, aFontAspectI ) && !UseDefaults )
    return anAsp;
  else 
    aFontAspect = (Font_FontAspect) aFontAspectI;

  anAsp = new Graphic3d_AspectText3d ( aTColor, aFont, anExpFactor, aSpace, aStyle, aDispText );
  anAsp->SetTextFontAspect( aFontAspect );
  return anAsp;
}

//================================================================
// Function : GetNormal
// Purpose  :
//================================================================
Standard_Boolean MeshVS_Tool::GetNormal( const TColStd_Array1OfReal& Nodes,
                                        gp_Vec& Norm )
{
  Standard_Integer first = Nodes.Lower(),
    last  = Nodes.Upper(),
    count = (last-first+1)/3, i, j;
  if( first==0 )
  {
    first = 1;
    count = Standard_Integer( Nodes.Value( 0 ) );
  }

  if( count<3 )
    return Standard_False;

  Standard_Boolean res = Standard_True;


  Standard_Real normal[3], first_vec[3], cur_vec[3], xx, yy, zz,
    conf = Precision::Confusion();

  for( i=0; i<3; i++ )
  {
    normal[i] = 0.0;
    first_vec[i] = Nodes.Value( first+3+i ) - Nodes.Value( first+i );
  }

  for( i=2; i<count; i++ )
  {
    for( j=0; j<3; j++ )
      cur_vec[j] = Nodes.Value( first+3*i+j ) - Nodes.Value( first+j );

    xx = first_vec[1] * cur_vec[2] - first_vec[2] * cur_vec[1];
    yy = first_vec[2] * cur_vec[0] - first_vec[0] * cur_vec[2];
    zz = first_vec[0] * cur_vec[1] - first_vec[1] * cur_vec[0];

    cur_vec[0] = xx;
    cur_vec[1] = yy;
    cur_vec[2] = zz;

    if( fabs( cur_vec[0] ) > conf ||
      fabs( cur_vec[1] ) > conf ||
      fabs( cur_vec[2] ) > conf )
    {
      Standard_Real cur = Sqrt( cur_vec[0]*cur_vec[0] + cur_vec[1]*cur_vec[1] + cur_vec[2]*cur_vec[2] );
      for( Standard_Integer k=0; k<3; k++ )
        cur_vec[k] /= cur;
    }

    if( fabs( normal[0] ) <= conf &&
      fabs( normal[1] ) <= conf &&
      fabs( normal[2] ) <= conf )
      for( Standard_Integer k=0; k<3; k++ )
        normal[k] = cur_vec[k];

    if( fabs( normal[0]-cur_vec[0] ) > conf ||
      fabs( normal[1]-cur_vec[1] ) > conf ||
      fabs( normal[2]-cur_vec[2] ) > conf    )
    {
      res = Standard_False;
      break;
    }
  }

  if( res )
    Norm.SetCoord( normal[0], normal[1], normal[2] );

  return res;
}


//================================================================
// Function : GetAverageNormal
// Purpose  :
//================================================================
Standard_Boolean MeshVS_Tool::GetAverageNormal( const TColStd_Array1OfReal& Nodes,
                                               gp_Vec& Norm )
{
  Standard_Integer first = Nodes.Lower(),
    last  = Nodes.Upper(),
    count = (last-first+1)/3, i, j;
  if( first==0 )
  {
    first = 1;
    count = Standard_Integer( Nodes.Value( 0 ) );
  }

  if( count<3 )
    return Standard_False;

  Standard_Boolean res = Standard_True;


  Standard_Real normal[3], first_vec[3], cur_vec[3], xx, yy, zz,
    conf = Precision::Confusion();

  for( i=0; i<3; i++ )
  {
    normal[i] = 0.0;
    first_vec[i] = Nodes.Value( first+3+i ) - Nodes.Value( first+i );
  }

  gp_XYZ* norm_vec = new gp_XYZ[count-2];
  for ( i = 0; i < count-2; i++ )
    norm_vec[i].SetCoord(0, 0, 0);

  for( i=2; i<count; i++ )
  {
    for( j=0; j<3; j++ )
      cur_vec[j] = Nodes.Value( first+3*i+j ) - Nodes.Value( first+j );

    xx = first_vec[1] * cur_vec[2] - first_vec[2] * cur_vec[1];
    yy = first_vec[2] * cur_vec[0] - first_vec[0] * cur_vec[2];
    zz = first_vec[0] * cur_vec[1] - first_vec[1] * cur_vec[0];

    cur_vec[0] = xx;
    cur_vec[1] = yy;
    cur_vec[2] = zz;

    if( fabs( cur_vec[0] ) > conf ||
      fabs( cur_vec[1] ) > conf ||
      fabs( cur_vec[2] ) > conf )
    {
      Standard_Real cur = Sqrt( cur_vec[0]*cur_vec[0] + cur_vec[1]*cur_vec[1] + cur_vec[2]*cur_vec[2] );
      for( Standard_Integer k=0; k<3; k++ )
        cur_vec[k] /= cur;
    }

    norm_vec[i-2].SetCoord(cur_vec[0], cur_vec[1], cur_vec[2]);

    if( fabs( normal[0] ) <= conf &&
      fabs( normal[1] ) <= conf &&
      fabs( normal[2] ) <= conf )
      for( Standard_Integer k=0; k<3; k++ )
        normal[k] = cur_vec[k];

    if( fabs( normal[0]-cur_vec[0] ) > conf ||
      fabs( normal[1]-cur_vec[1] ) > conf ||
      fabs( normal[2]-cur_vec[2] ) > conf    )
    {
      res = Standard_False;
    }
  }

  if( !res ) {
    for( j=0; j<3; j++ ) {
      normal[j] = 0.0;
      for (i = 0; i < count-2; i++)
        normal[j] += norm_vec[i].Coord(j+1);
      normal[j] /= (count-2);
    }
  }
  delete [] norm_vec;

  Norm.SetCoord( normal[0], normal[1], normal[2] );

  return res;
}
