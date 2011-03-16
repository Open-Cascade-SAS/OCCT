// File:	MeshVS_NodalColorPrsBuilder.cxx
// Created:	Wed Nov 12 2003
// Author:	Alexander SOLOVYOV
// Copyright:	Open CASCADE 2003

#define _POLYGONES_
// if define _POLYGONES_ ColorPrsBuilder use ArrayOfPolygons for drawing faces

#include <MeshVS_NodalColorPrsBuilder.ixx>

#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>

#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>

#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>

#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <MeshVS_Buffer.hxx>

#include <gp_Pnt.hxx>
#include <Image_ColorImage.hxx>
#include <AlienImage_AlienImage.hxx>
#include <AlienImage_BMPAlienImage.hxx>
#include <Graphic3d_Texture2D.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Standard_DefineHandle.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Graphic3d_Array1OfVertexNT.hxx>
#include <AIS_Drawer.hxx>
#include <Quantity_Array1OfColor.hxx>
#include <Aspect_SequenceOfColor.hxx>

/*
  Class       : MeshVS_ImageTexture2D
  Description : Texture for nodal presentation
*/
class MeshVS_ImageTexture2D : public Graphic3d_Texture2D
{
public:
  MeshVS_ImageTexture2D( Handle(Graphic3d_StructureManager) theSM, 
                         const Handle(AlienImage_AlienImage)& theImg );
  virtual ~MeshVS_ImageTexture2D();

public:
  DEFINE_STANDARD_RTTI(MeshVS_ImageTexture2D)
};

DEFINE_STANDARD_HANDLE(MeshVS_ImageTexture2D,Graphic3d_Texture2D)
IMPLEMENT_STANDARD_HANDLE(MeshVS_ImageTexture2D,Graphic3d_Texture2D)
IMPLEMENT_STANDARD_RTTIEXT(MeshVS_ImageTexture2D, Graphic3d_Texture2D)

MeshVS_ImageTexture2D::MeshVS_ImageTexture2D
                        (Handle(Graphic3d_StructureManager) theSM, 
                         const Handle(AlienImage_AlienImage)& theImg)
: Graphic3d_Texture2D( theSM, "", Graphic3d_TOT_2D )
{
  MyCInitTexture.doModulate = 0/*1*/;
  MyCInitTexture.doRepeat = 0;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_MANUAL;
  MyCInitTexture.doLinear = 1;
  MyCInitTexture.sx = 1.0F;
  MyCInitTexture.sy = 1.0F;
  MyCInitTexture.tx = 0.0F;
  MyCInitTexture.ty = 0.0F;
  MyCInitTexture.angle = 0.0F;
  MyCInitTexture.sparams[0] = 0.0F;
  MyCInitTexture.sparams[1] = 0.0F;
  MyCInitTexture.sparams[2] = 0.0F;
  MyCInitTexture.sparams[3] = 0.0F;
  MyCInitTexture.tparams[0] = 0.0F;
  MyCInitTexture.tparams[1] = 0.0F;
  MyCInitTexture.tparams[2] = 0.0F;
  MyCInitTexture.tparams[3] = 0.0F;
  Update();
  LoadTexture(theImg);
}

MeshVS_ImageTexture2D::~MeshVS_ImageTexture2D()
{
}

//================================================================
// Function : getNearestPow2
// Purpose  : Returns the nearest power of two greater than the 
//            argument value
//================================================================
static inline Standard_Integer getNearestPow2( Standard_Integer theValue )
{
  // Precaution against overflow
  Standard_Integer aHalfMax = IntegerLast() >> 1, aRes = 1;
  if ( theValue > aHalfMax ) theValue = aHalfMax;
  while ( aRes < theValue ) aRes <<= 1;
  return aRes;
}

/*
  Class       : MeshVS_NodalColorPrsBuilder
  Description : This class provides methods to create presentation of 
                nodes with assigned color (See hxx for more description )
*/

//================================================================
// Function : Constructor MeshVS_NodalColorPrsBuilder
// Purpose  :
//================================================================
MeshVS_NodalColorPrsBuilder::MeshVS_NodalColorPrsBuilder ( const Handle(MeshVS_Mesh)& Parent,
                                                           const MeshVS_DisplayModeFlags& Flags,
                                                           const Handle (MeshVS_DataSource)& DS,
                                                           const Standard_Integer Id,
                                                           const MeshVS_BuilderPriority& Priority )
: MeshVS_PrsBuilder ( Parent, Flags, DS, Id, Priority ),
  myUseTexture( Standard_False ), 
  myInvalidColor( Quantity_NOC_GRAY )
{
  SetExcluding ( Standard_True );
}

//================================================================
// Function : Build
// Purpose  :
//================================================================
void MeshVS_NodalColorPrsBuilder::Build ( const Handle(Prs3d_Presentation)& Prs,
                                          const TColStd_PackedMapOfInteger& IDs,
                                          TColStd_PackedMapOfInteger& IDsToExclude,
                                          const Standard_Boolean IsElement,
                                          const Standard_Integer DisplayMode) const
{
  Handle (MeshVS_DataSource) aSource = GetDataSource();
  Handle (MeshVS_Drawer)     aDrawer = GetDrawer();
  if ( aSource.IsNull() || aDrawer.IsNull() )
    return;

  Standard_Integer aMaxFaceNodes;
  if ( !aDrawer->GetInteger ( MeshVS_DA_MaxFaceNodes, aMaxFaceNodes ) || aMaxFaceNodes <= 0 )
    return;

  MeshVS_Buffer aCoordsBuf (3*aMaxFaceNodes*sizeof(Standard_Real));
  TColStd_Array1OfReal aCoords ( aCoordsBuf, 1, 3 * aMaxFaceNodes );
  Standard_Integer NbNodes;
  MeshVS_EntityType aType;

  if ( !( DisplayMode & GetFlags() ) || !IsElement )
    return;

  if ( myUseTexture && ( !myTextureCoords.Extent() || !myTextureColorMap.Length() ) || 
       !myUseTexture && !myNodeColorMap.Extent() )
    return;

  // subtract the hidden elements and ids to exclude (to minimise allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign( IDs );
  Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
  if ( !aHiddenElems.IsNull() )
    anIDs.Subtract( aHiddenElems->Map() );
  anIDs.Subtract( IDsToExclude );

  Standard_Boolean IsReflect = Standard_False, IsMeshSmoothShading = Standard_False;
  aDrawer->GetBoolean( MeshVS_DA_ColorReflection, IsReflect );
  aDrawer->GetBoolean( MeshVS_DA_SmoothShading,   IsMeshSmoothShading );
  
  // NOTE: Lighting effect not tested with textures, so turning off reflectance when textures are used.
  // This can be improved if necessary...
  IsReflect = IsReflect && !myUseTexture;

  // Following parameter are used for texture presentation only
  int nbColors = 0; // Number of colors from color map
  int nbTextureColors = 0; // Number of colors in texture (it will be pow of 2)
  if ( myUseTexture )
  {
    nbColors = myTextureColorMap.Length();
    nbTextureColors = getNearestPow2( nbColors );
  }
  
  Standard_Integer aSize = anIDs.Extent();

  // Calculate maximum possible number of vertices and bounds
  Handle( MeshVS_HArray1OfSequenceOfInteger ) aTopo;
  Standard_Integer PolygonVerticesFor3D = 0, PolygonBoundsFor3D = 0;
  TColStd_MapIteratorOfPackedMapOfInteger it (anIDs);
  for( ; it.More(); it.Next() )
  {
    Standard_Integer aKey = it.Key();
    if ( aSource->Get3DGeom( aKey, NbNodes, aTopo ) )
      MeshVS_MeshPrsBuilder::HowManyPrimitives
      ( aTopo, Standard_True, Standard_False, NbNodes,
      PolygonVerticesFor3D, PolygonBoundsFor3D );
  }

  // Draw faces with nodal color
  // OCC20644 Use "plastic" material as it is "non-physic" and so it is easier
  // to get the required colors (see TelUpdateMaterial() function in OpenGl_attri.c)
  Graphic3d_MaterialAspect aMaterial[ 2 ];          
  aMaterial[ 0 ] = Graphic3d_MaterialAspect( Graphic3d_NOM_PLASTIC );
  aMaterial[ 1 ] = Graphic3d_MaterialAspect( Graphic3d_NOM_PLASTIC );
  Standard_Integer i;
  for ( i = 0; i < 2; i++ )
  {
    if ( !IsReflect )
    {
      aMaterial[ i ].SetReflectionModeOff( Graphic3d_TOR_SPECULAR );
      aMaterial[ i ].SetReflectionModeOff( Graphic3d_TOR_AMBIENT );
      aMaterial[ i ].SetReflectionModeOff( Graphic3d_TOR_DIFFUSE );
      aMaterial[ i ].SetReflectionModeOff( Graphic3d_TOR_EMISSION );
    }
    else{
      // OCC20644 Using the material with reflection properties same as in
      // ElementalColorPrsBuilder, to get the same colors.
      // Additionally, ambient and diffuse coefficients are used below to scale incoming colors,
      // to simulate TelUpdateMaterial() function from OpenGl_attri.c.
      // This is mandatory, as these "scaled" colors are then passed directly to OpenGL
      // as ambient and diffuse colors of the current material using glColorMaterial().
      // In ElementalColorPrsBuilder we do not need to do scale the colors, as this
      // is done by TelUpdateMaterial().
      // 0.5 is used to have the colors in 3D maximally similar to those in the color scale.
      // This is possible when the sum of all coefficient is equal to 1.
      aMaterial[i].SetAmbient( .5 );
      aMaterial[i].SetDiffuse( .5 );
      aMaterial[i].SetSpecular( 0. );
      aMaterial[i].SetEmissive( 0. );
    }
  
 } 


  // Create array of polygons for interior presentation of faces and volumes
  Handle(Graphic3d_ArrayOfPolygons) aCPolyArr = new Graphic3d_ArrayOfPolygons
    ( aMaxFaceNodes * aSize + PolygonVerticesFor3D, aSize + PolygonBoundsFor3D, 
    0, myUseTexture || IsReflect, !myUseTexture, Standard_False, myUseTexture );

  // Create array of polylines for presentation of edges 
  // (used for optimization insted of SetEdgeOn method call) 
  Handle(Graphic3d_ArrayOfPolylines) aPolyL = new Graphic3d_ArrayOfPolylines
    ( ( aMaxFaceNodes + 1 ) * aSize + PolygonVerticesFor3D, aSize + PolygonBoundsFor3D );

  gp_Pnt P, Start;
  Standard_Real aMin = gp::Resolution() * gp::Resolution();
  gp_Dir aDefNorm( 0., 0., 1. );

  // Prepare for scaling the incoming colors
  Standard_Real anColorRatio = aMaterial[0].Ambient();

  for( it.Reset(); it.More(); it.Next() )
  {
    Standard_Integer aKey = it.Key();
    if ( aSource->GetGeom  ( aKey, Standard_True, aCoords, NbNodes, aType ) )
    {
      MeshVS_Buffer aNodesBuf (NbNodes*sizeof(Standard_Integer));
      TColStd_Array1OfInteger aNodes(aNodesBuf, 1, NbNodes);
      if ( !aSource->GetNodesByElement ( aKey, aNodes, NbNodes ) )
        continue;

      Quantity_Color aNColor;

      Standard_Boolean isValid = Standard_True;
      Standard_Integer i;
      if ( myUseTexture )
      {
        for ( i = 1; i <= NbNodes && isValid; i++ )
          isValid = myTextureCoords.IsBound( aNodes( i ) );
      }
      else
      {
        for ( i = 1; i <= NbNodes && isValid; i++ )
          isValid = GetColor ( aNodes( i ), aNColor );
      }

      if ( !isValid )
        continue;

      // Preparing normal(s) to show reflections if requested
      Handle(TColStd_HArray1OfReal) aNormals;
      Standard_Boolean hasNormals = 
        ( IsReflect && aSource->GetNormalsByElement( aKey, IsMeshSmoothShading, aMaxFaceNodes, aNormals ) );

      if ( aType == MeshVS_ET_Face )
      {
        aCPolyArr->AddBound ( NbNodes );
        aPolyL->AddBound ( NbNodes + 1 );

        for ( i = 1; i <= NbNodes; i++)
        {
          P = gp_Pnt( aCoords( 3 * i - 2 ), aCoords( 3 * i - 1 ), aCoords( 3 * i ) );
          if ( myUseTexture )
          {
            int anId = aNodes( i );
            double aTexCoord = myTextureCoords( anId );

            // transform texture coordinate in accordance with number of colors specified 
            // by upper level and real size of Gl texture
            aTexCoord = aTexCoord * nbColors / nbTextureColors;

            aCPolyArr->AddVertex( P, aDefNorm, 
              gp_Pnt2d( aTexCoord, aTexCoord >= 0 && aTexCoord <= 1 ? 1 : 0 ) );
          }
          else
          {
            GetColor ( aNodes( i ), aNColor );
           
            if ( IsReflect )
            { 
              // Simulating TelUpdateMaterial() from OpenGl_attri.c
              // to get the same colors in elemental and nodal color prs builders
              aNColor.SetValues(anColorRatio * aNColor.Red(),
                                anColorRatio * aNColor.Green(),
                                anColorRatio * aNColor.Blue(), 
                                Quantity_TOC_RGB);
              
              if ( hasNormals )
              {
                gp_Vec aNorm(aNormals->Value( 3 * i - 2 ), 
                  aNormals->Value( 3 * i - 1 ), 
                  aNormals->Value( 3 * i     ));
                aNorm.SquareMagnitude() > aMin ?
                  aCPolyArr->AddVertex(P, gp_Dir( aNorm ), aNColor ) : 
                aCPolyArr->AddVertex(P, aDefNorm       , aNColor );
              }
              else
                aCPolyArr->AddVertex(P, aDefNorm, aNColor );
            }
            else
              aCPolyArr->AddVertex( P, aNColor );
          }
          aPolyL->AddVertex ( P );
          if ( i == 1 )
            Start = P;
        }
        aPolyL->AddVertex ( Start );

        // if IsExcludingOn then presentation must not be built by other builders
        if ( IsExcludingOn() )
          IDsToExclude.Add( aKey );
      }
      else if ( aType == MeshVS_ET_Volume )
      {
        if ( !aSource->Get3DGeom( aKey, NbNodes, aTopo ) )
          continue;

        // iterate through faces of volume 
        for ( Standard_Integer k = aTopo->Lower(), last = aTopo->Upper(), normIndex = 1; k <= last; k++, normIndex++ )
        {
          const TColStd_SequenceOfInteger& aSeq = aTopo->Value( k );
          Standard_Integer m = aSeq.Length(), ind;

          // build polygon & polylines for current face
          aCPolyArr->AddBound( m );
          aPolyL->AddBound( m + 1 );
          for ( Standard_Integer j = 1; j <= m; j++ )
          {
            ind = aSeq.Value( j );
            P = gp_Pnt( aCoords( 3 * ind + 1 ), 
                        aCoords( 3 * ind + 2 ), 
                        aCoords( 3 * ind + 3 ) );
            if ( myUseTexture )
            {
              Standard_Integer anId = aNodes( ind + 1 );
              Standard_Real aTexCoord = myTextureCoords( anId );

              // transform texture coordinate in accordance with number of colors specified 
              // by upper level and real size of Gl texture
              aTexCoord = aTexCoord * nbColors / nbTextureColors;

              gp_Pnt2d aTP( aTexCoord, aTexCoord >= 0 && aTexCoord <= 1 ? 1 : 0 );
              aCPolyArr->AddVertex( P, aDefNorm, aTP );
            }
            else
            {
              GetColor( aNodes( ind + 1 ), aNColor );
              if  ( IsReflect )
              { 
                // Simulating TelUpdateMaterial() from OpenGl_attri.c
                // to get the same colors in elemental and nodal color prs builders
                aNColor.SetValues(anColorRatio * aNColor.Red(),
                                  anColorRatio * aNColor.Green(),
                                  anColorRatio * aNColor.Blue(), 
                                  Quantity_TOC_RGB);
                
                if ( hasNormals )
                {
                  gp_Vec aNorm(aNormals->Value( 3 * normIndex - 2 ), 
                    aNormals->Value( 3 * normIndex - 1 ), 
                    aNormals->Value( 3 * normIndex ));
                  aNorm.SquareMagnitude() > aMin ? 
                    aCPolyArr->AddVertex( P, gp_Dir( aNorm ), aNColor ) : 
                  aCPolyArr->AddVertex( P, aDefNorm       , aNColor );
                }
                else
                  aCPolyArr->AddVertex( P, aDefNorm, aNColor );
              }
              else
                aCPolyArr->AddVertex( P, aNColor );
            }
            aPolyL->AddVertex ( P );
            if ( j == 1 )
              Start = P;
          }
          aPolyL->AddVertex ( Start );
        }

        // if IsExcludingOn then presentation must not be built by other builders
        if ( IsExcludingOn() )
          IDsToExclude.Add( aKey );
      } 
    } 
  } // for ( ...

  Handle(Graphic3d_AspectFillArea3d) anAsp;

//  Aspect_InteriorStyle  aStyle;
//  Standard_Integer      aStyleInt;
  Aspect_TypeOfLine     anEdgeType = Aspect_TOL_SOLID;
  Standard_Integer      anEdgeInt;
  Standard_Real         anEdgeWidth;
  Quantity_Color        anInteriorColor;
  Quantity_Color        anEdgeColor, aLineColor;
  Standard_Boolean      aShowEdges = Standard_True;

  aDrawer->GetColor  ( MeshVS_DA_InteriorColor, anInteriorColor );
  aDrawer->GetColor  ( MeshVS_DA_EdgeColor, anEdgeColor );
  aDrawer->GetColor  ( MeshVS_DA_BeamColor, aLineColor );
  aDrawer->GetDouble ( MeshVS_DA_EdgeWidth, anEdgeWidth );
  aDrawer->GetBoolean( MeshVS_DA_ShowEdges, aShowEdges );

  if ( aDrawer->GetInteger ( MeshVS_DA_EdgeType, anEdgeInt ) )
    anEdgeType = (Aspect_TypeOfLine) anEdgeInt;

  if ( myUseTexture )
  {
    Handle(AIS_Drawer) anAISDrawer =  myParentMesh->Attributes();
    if ( anAISDrawer.IsNull() )
      return;
    
    anAISDrawer->SetShadingAspect( new Prs3d_ShadingAspect() );
    anAsp = anAISDrawer->ShadingAspect()->Aspect();
    if ( anAsp.IsNull() )
      return;

    anAsp->SetFrontMaterial( aMaterial[ 0 ] );
    anAsp->SetBackMaterial( aMaterial[ 1 ] );
  

    Handle(Graphic3d_Texture2D) aTexture = CreateTexture();
    if ( aTexture.IsNull() )
      return;

    anAsp->SetTextureMapOn();  
    anAsp->SetTextureMap( aTexture );
  }
  else
  {
//    if ( aDrawer->GetInteger ( MeshVS_DA_InteriorStyle, aStyleInt ) )
//      aStyle = (Aspect_InteriorStyle)aStyleInt;

    anAsp = new Graphic3d_AspectFillArea3d ( 
      Aspect_IS_SOLID, Quantity_NOC_GRAY, anEdgeColor,
      anEdgeType, anEdgeWidth, aMaterial[ 0 ], aMaterial[ 1 ] );
  }

  anAsp->SetDistinguishOff();
  anAsp->SetEdgeOff();
  anAsp->SetInteriorColor( Quantity_NOC_GRAY );

  Handle(Graphic3d_AspectLine3d) anLAsp = 
    new Graphic3d_AspectLine3d( anEdgeColor, anEdgeType, anEdgeWidth );
  
  Prs3d_Root::NewGroup ( Prs );
  Handle(Graphic3d_Group) aGroup1 = Prs3d_Root::CurrentGroup ( Prs );
  
  aGroup1->SetPrimitivesAspect( anAsp );
  aGroup1->BeginPrimitives();
  aGroup1->AddPrimitiveArray( aCPolyArr );
  aGroup1->EndPrimitives();

  if (aShowEdges)
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle(Graphic3d_Group) aGroup2 = Prs3d_Root::CurrentGroup ( Prs );

    anAsp->SetEdgeOff();
    anAsp->SetTextureMapOff();  
    aGroup2->SetPrimitivesAspect( anAsp );
    aGroup2->SetPrimitivesAspect( anLAsp );
    aGroup2->BeginPrimitives();
    aGroup2->AddPrimitiveArray( aPolyL );
    aGroup2->EndPrimitives();
    anAsp->SetEdgeOn();
  }
}

//================================================================
// Function : SetColors
// Purpose  :
//================================================================
void MeshVS_NodalColorPrsBuilder::SetColors ( 
  const MeshVS_DataMapOfIntegerColor& theColorMap )
{
  myNodeColorMap = theColorMap;
}

//================================================================
// Function : GetColors
// Purpose  :
//================================================================
const MeshVS_DataMapOfIntegerColor& MeshVS_NodalColorPrsBuilder::GetColors() const
{
  return myNodeColorMap;
}

//================================================================
// Function : HasColors
// Purpose  :
//================================================================
Standard_Boolean MeshVS_NodalColorPrsBuilder::HasColors () const
{
  return ( myNodeColorMap.Extent() >0 );
}

//================================================================
// Function : GetColor
// Purpose  :
//================================================================
Standard_Boolean MeshVS_NodalColorPrsBuilder::GetColor ( const Standard_Integer ID,
                                                         Quantity_Color& theColor ) const
{
  Standard_Boolean aRes = myNodeColorMap.IsBound ( ID );
  if ( aRes )
    theColor = myNodeColorMap.Find ( ID );
  return aRes;
}

//================================================================
// Function : SetColor
// Purpose  :
//================================================================
void MeshVS_NodalColorPrsBuilder::SetColor ( const Standard_Integer theID,
                                             const Quantity_Color& theCol )
{
  Standard_Boolean aRes = myNodeColorMap.IsBound ( theID );
  if ( aRes )
    myNodeColorMap.ChangeFind ( theID ) = theCol;
  else
    myNodeColorMap.Bind ( theID, theCol );
}

//================================================================
// Function : UseTexture
// Purpose  : Specify whether texture must be used to build presentation
//================================================================
void MeshVS_NodalColorPrsBuilder::UseTexture( const Standard_Boolean theToUse )
{
  myUseTexture = theToUse;
  if ( myUseTexture )
    myNodeColorMap.Clear();
  else
    myTextureColorMap.Clear();
}

//================================================================
// Function : IsUseTexture
// Purpose  : Verify whether texture is used to build presentation
//================================================================
Standard_Boolean MeshVS_NodalColorPrsBuilder::IsUseTexture() const
{
  return myUseTexture;
}

//================================================================
// Function : SetColorMap
// Purpose  : Set colors to be used for texrture presentation. 
//            Generate texture in accordance with given parameters
//================================================================
void MeshVS_NodalColorPrsBuilder::SetColorMap( const Aspect_SequenceOfColor& theColors )
{
  myTextureColorMap = theColors;
}

//================================================================
// Function : GetColorMap
// Purpose  : Return colors used for texrture presentation 
//================================================================
const Aspect_SequenceOfColor& MeshVS_NodalColorPrsBuilder::GetColorMap() const
{
  return myTextureColorMap;
}

//================================================================
// Function : SetInvalidColor
// Purpose  : Set color representing invalid texture coordinate 
//            (laying outside range [0, 1])
//================================================================
void MeshVS_NodalColorPrsBuilder::SetInvalidColor( 
  const Quantity_Color& theInvalidColor )
{
  myInvalidColor = theInvalidColor;
}

//================================================================
// Function : GetInvalidColor
// Purpose  : Return color representing invalid texture coordinate
//            (laying outside range [0, 1])
//================================================================
Quantity_Color MeshVS_NodalColorPrsBuilder::GetInvalidColor() const
{
  return myInvalidColor;
}

//================================================================
// Function : SetTextureCoords
// Purpose  : Specify correspondence between node IDs and texture 
//            coordinates (range [0, 1])
//================================================================
void MeshVS_NodalColorPrsBuilder::SetTextureCoords ( 
  const TColStd_DataMapOfIntegerReal& theMap )
{
  myTextureCoords = theMap;
}

//================================================================
// Function : GetTextureCoords
// Purpose  : Get correspondence between node IDs and texture 
//            coordinates (range [0, 1]) 
//================================================================
const TColStd_DataMapOfIntegerReal& MeshVS_NodalColorPrsBuilder::GetTextureCoords() const
{
  return myTextureCoords;
}

//================================================================
// Function : SetTextureCoord
// Purpose  : Specify correspondence between node ID and texture 
//            coordinate (range [0, 1]) 
//================================================================
void MeshVS_NodalColorPrsBuilder::SetTextureCoord( const Standard_Integer theID,
                                                   const Standard_Real theCoord )
{
  myTextureCoords.Bind( theID, theCoord );
}

//================================================================
// Function : GetTextureCoord
// Purpose  : Return correspondence between node IDs and texture 
//            coordinate (range [0, 1]) 
//================================================================
Standard_Real MeshVS_NodalColorPrsBuilder::GetTextureCoord( const Standard_Integer theID )
{
  return myTextureCoords.IsBound( theID ) ? myTextureCoords( theID ) : -1;
}

//================================================================
// Function : CreateTexture
// Purpose  : Create texture in accordance with myTextureColorMap
//================================================================
Handle(Graphic3d_Texture2D) MeshVS_NodalColorPrsBuilder::CreateTexture() const
{
  Handle(Graphic3d_Texture2D) aTexture;

  int nbColors = myTextureColorMap.Length();
  if ( nbColors == 0 )
    return aTexture;

  Handle(PrsMgr_PresentationManager3d) aPrsMgr = GetPresentationManager();
  if ( aPrsMgr.IsNull() )
    return aTexture;

  int nbTextureColors = getNearestPow2( nbColors );

  // Create and fill image with colors
  Handle(Image_ColorImage) aCImage = 
    new Image_ColorImage( 0, 0, nbTextureColors, 2 );

  if ( nbColors > 0 )
  {
    int i;
    for ( i = 0; i < nbColors; i++ )
    {
      aCImage->SetPixel( i, 0, Aspect_ColorPixel( myTextureColorMap( i + 1 ) ) ); 
      aCImage->SetPixel( i, 1, Aspect_ColorPixel( myInvalidColor ) ); 
    }
    Quantity_Color aLastColor = myTextureColorMap( nbColors );
    for ( i = nbColors; i < nbTextureColors; i++ )
    {
      aCImage->SetPixel( i, 0, aLastColor ); 
      aCImage->SetPixel( i, 1, Aspect_ColorPixel( myInvalidColor ) ); 
    }
  }

  // Convert image to bmp
  Handle(AlienImage_BMPAlienImage) aTextureImage = new AlienImage_BMPAlienImage();
  aTextureImage->FromImage( aCImage );  

  // Create texture
  aTexture = new MeshVS_ImageTexture2D( aPrsMgr->StructureManager(), aTextureImage );
  return aTexture;
}
