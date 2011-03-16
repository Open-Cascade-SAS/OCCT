
// File         Graphic3d_Group_8.cxx (PrimitivesAspect)
// Created      Fevrier 1992
// Author       NW,JPB,CAL
// modified:     1/07/97 ; PCT : ajout texture mapping
//              20/07/97 ; PCT : ajout transparence texture
//              08/04/98 ; FGU : Ajout emission surface
//              30/11/98 ; FMN : S4069. Textes always visible.   
//              22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets */

//-Copyright    MatraDatavision 1991,1992

//-Version      

//-Design       Declaration des variables specifiques aux groupes
//              de primitives

//-Warning      Un groupe est defini dans une structure
//              Il s'agit de la plus petite entite editable

//-References   

//-Language     C++ 2.0

//-Declarations

#define OCC1174 //SAV 08/01/03 : Added back face interior color controling


// for the class
#include <Graphic3d_Group.jxx>
#include <Graphic3d_Group.pxx>
#include <Graphic3d_TextureMap.hxx>

//-Methods, in order

void Graphic3d_Group::SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real R, G, B;
  Standard_Real AWidth;
  Quantity_Color AColor;
  Aspect_TypeOfLine ALType;

  CTX->Values (AColor, ALType, AWidth);
  AColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCGroup.ContextLine.Color.r    = float (R);
  MyCGroup.ContextLine.Color.g    = float (G);
  MyCGroup.ContextLine.Color.b    = float (B);
  MyCGroup.ContextLine.LineType   = int (ALType);
  MyCGroup.ContextLine.Width      = float (AWidth);
  MyCGroup.ContextLine.IsDef      = 1;

  int noinsert    = 1;
  MyGraphicDriver->LineContextGroup (MyCGroup, noinsert);

  MyCGroup.ContextLine.IsSet      = 1;

  Update ();

}

void Graphic3d_Group::SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real R, G, B;
  Standard_Real AWidth;
  //Quantity_Color AColor;
  Quantity_Color AIntColor;
  Quantity_Color BackIntColor;
  Quantity_Color AEdgeColor;
  Aspect_TypeOfLine ALType;
  Aspect_InteriorStyle AStyle;

  CTX->Values (AStyle, AIntColor, BackIntColor, AEdgeColor, ALType, AWidth);
  AIntColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCGroup.ContextFillArea.Style          = int (AStyle);
  MyCGroup.ContextFillArea.IntColor.r     = float (R);
  MyCGroup.ContextFillArea.IntColor.g     = float (G);
  MyCGroup.ContextFillArea.IntColor.b     = float (B);

#ifdef OCC1174
  if ( CTX->Distinguish() )
    BackIntColor.Values( R, G, B, Quantity_TOC_RGB );
#endif
  MyCGroup.ContextFillArea.BackIntColor.r = float( R );
  MyCGroup.ContextFillArea.BackIntColor.g = float( G );
  MyCGroup.ContextFillArea.BackIntColor.b = float( B );

  MyCGroup.ContextFillArea.Edge   = (CTX->Edge () ? 1 : 0);
  AEdgeColor.Values (R, G, B, Quantity_TOC_RGB);
  MyCGroup.ContextFillArea.EdgeColor.r    = float (R);
  MyCGroup.ContextFillArea.EdgeColor.g    = float (G);
  MyCGroup.ContextFillArea.EdgeColor.b    = float (B);
  MyCGroup.ContextFillArea.LineType       = int (ALType);
  MyCGroup.ContextFillArea.Width  = float (AWidth);
  MyCGroup.ContextFillArea.Hatch  = int (CTX->HatchStyle ());

  /*** Front and Back face ***/
  MyCGroup.ContextFillArea.Distinguish    =
    ( CTX->Distinguish () ? 1 : 0 );
  MyCGroup.ContextFillArea.BackFace       =
    ( CTX->BackFace () ? 1 : 0 );

  /*** Back Material ***/
  // Light specificity
  MyCGroup.ContextFillArea.Back.Shininess =
    float ((CTX->BackMaterial ()).Shininess ());
  MyCGroup.ContextFillArea.Back.Ambient   =
    float ((CTX->BackMaterial ()).Ambient ());
  MyCGroup.ContextFillArea.Back.Diffuse   =
    float ((CTX->BackMaterial ()).Diffuse ());
  MyCGroup.ContextFillArea.Back.Specular  =
    float ((CTX->BackMaterial ()).Specular ());
  MyCGroup.ContextFillArea.Back.Transparency      =
    float ((CTX->BackMaterial ()).Transparency ());
  MyCGroup.ContextFillArea.Back.Emission  =
    float ((CTX->BackMaterial ()).Emissive ());

  // Reflection mode 
  MyCGroup.ContextFillArea.Back.IsAmbient =
    ( (CTX->BackMaterial ()).ReflectionMode
    (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  MyCGroup.ContextFillArea.Back.IsDiffuse =
    ( (CTX->BackMaterial ()).ReflectionMode
    (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  MyCGroup.ContextFillArea.Back.IsSpecular        =
    ( (CTX->BackMaterial ()).ReflectionMode
    (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  MyCGroup.ContextFillArea.Back.IsEmission        =
    ( (CTX->BackMaterial ()).ReflectionMode
    (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  // Material type                                
  //JR/Hp
  const Graphic3d_MaterialAspect ama = CTX->BackMaterial () ;
  Standard_Boolean amt = ama.MaterialType(Graphic3d_MATERIAL_PHYSIC) ;
  MyCGroup.ContextFillArea.Back.IsPhysic = ( amt ? 1 : 0 );

  // Specular color                                       
  MyCGroup.ContextFillArea.Back.ColorSpec.r       =
    float (((CTX->BackMaterial ()).SpecularColor ()).Red ());
  MyCGroup.ContextFillArea.Back.ColorSpec.g       =
    float (((CTX->BackMaterial ()).SpecularColor ()).Green ());
  MyCGroup.ContextFillArea.Back.ColorSpec.b       =
    float (((CTX->BackMaterial ()).SpecularColor ()).Blue ());


  // Ambient color
  MyCGroup.ContextFillArea.Back.ColorAmb.r        =
    float (((CTX->BackMaterial ()).AmbientColor ()).Red ());
  MyCGroup.ContextFillArea.Back.ColorAmb.g        =
    float (((CTX->BackMaterial ()).AmbientColor ()).Green ());
  MyCGroup.ContextFillArea.Back.ColorAmb.b        =
    float (((CTX->BackMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color
  MyCGroup.ContextFillArea.Back.ColorDif.r        =
    float (((CTX->BackMaterial ()).DiffuseColor ()).Red ());
  MyCGroup.ContextFillArea.Back.ColorDif.g        =
    float (((CTX->BackMaterial ()).DiffuseColor ()).Green ());
  MyCGroup.ContextFillArea.Back.ColorDif.b        =
    float (((CTX->BackMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  MyCGroup.ContextFillArea.Back.ColorEms.r        =
    float (((CTX->BackMaterial ()).EmissiveColor ()).Red ());
  MyCGroup.ContextFillArea.Back.ColorEms.g        =
    float (((CTX->BackMaterial ()).EmissiveColor ()).Green ());
  MyCGroup.ContextFillArea.Back.ColorEms.b        =
    float (((CTX->BackMaterial ()).EmissiveColor ()).Blue ());

  MyCGroup.ContextFillArea.Back.EnvReflexion = 
    float ((CTX->BackMaterial ()).EnvReflexion());

  /*** Front Material ***/
  // Light specificity
  MyCGroup.ContextFillArea.Front.Shininess        =
    float ((CTX->FrontMaterial ()).Shininess ());
  MyCGroup.ContextFillArea.Front.Ambient  =
    float ((CTX->FrontMaterial ()).Ambient ());
  MyCGroup.ContextFillArea.Front.Diffuse  =
    float ((CTX->FrontMaterial ()).Diffuse ());
  MyCGroup.ContextFillArea.Front.Specular =
    float ((CTX->FrontMaterial ()).Specular ());
  MyCGroup.ContextFillArea.Front.Transparency     =
    float ((CTX->FrontMaterial ()).Transparency ());
  MyCGroup.ContextFillArea.Front.Emission =
    float ((CTX->FrontMaterial ()).Emissive ());    

  // Reflection mode      
  MyCGroup.ContextFillArea.Front.IsAmbient        =
    ( (CTX->FrontMaterial ()).ReflectionMode
    (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  MyCGroup.ContextFillArea.Front.IsDiffuse        =
    ( (CTX->FrontMaterial ()).ReflectionMode
    (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  MyCGroup.ContextFillArea.Front.IsSpecular       =
    ( (CTX->FrontMaterial ()).ReflectionMode
    (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  MyCGroup.ContextFillArea.Front.IsEmission       =
    ( (CTX->FrontMaterial ()).ReflectionMode
    (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  // Material type
  //JR/Hp
  const Graphic3d_MaterialAspect amas = CTX->FrontMaterial () ;
  Standard_Boolean amty = amas.MaterialType(Graphic3d_MATERIAL_PHYSIC) ;
  MyCGroup.ContextFillArea.Front.IsPhysic = ( amty ? 1 : 0 );

  // Specular color                               
  MyCGroup.ContextFillArea.Front.ColorSpec.r      =
    float (((CTX->FrontMaterial ()).SpecularColor ()).Red ());
  MyCGroup.ContextFillArea.Front.ColorSpec.g      =
    float (((CTX->FrontMaterial ()).SpecularColor ()).Green ());
  MyCGroup.ContextFillArea.Front.ColorSpec.b      =
    float (((CTX->FrontMaterial ()).SpecularColor ()).Blue ());

  // Ambient color                                
  MyCGroup.ContextFillArea.Front.ColorAmb.r       =
    float (((CTX->FrontMaterial ()).AmbientColor ()).Red ());
  MyCGroup.ContextFillArea.Front.ColorAmb.g       =
    float (((CTX->FrontMaterial ()).AmbientColor ()).Green ());
  MyCGroup.ContextFillArea.Front.ColorAmb.b       =
    float (((CTX->FrontMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color                                
  MyCGroup.ContextFillArea.Front.ColorDif.r       =
    float (((CTX->FrontMaterial ()).DiffuseColor ()).Red ());
  MyCGroup.ContextFillArea.Front.ColorDif.g       =
    float (((CTX->FrontMaterial ()).DiffuseColor ()).Green ());
  MyCGroup.ContextFillArea.Front.ColorDif.b       =
    float (((CTX->FrontMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  MyCGroup.ContextFillArea.Front.ColorEms.r       =
    float (((CTX->FrontMaterial ()).EmissiveColor ()).Red ());
  MyCGroup.ContextFillArea.Front.ColorEms.g       =
    float (((CTX->FrontMaterial ()).EmissiveColor ()).Green ());
  MyCGroup.ContextFillArea.Front.ColorEms.b       =
    float (((CTX->FrontMaterial ()).EmissiveColor ()).Blue ());

  MyCGroup.ContextFillArea.Front.EnvReflexion = 
    float ((CTX->FrontMaterial ()).EnvReflexion());

  MyCGroup.ContextFillArea.IsDef  = 1; // Definition material ok

  /*** Texture map ***/
  Handle(Graphic3d_TextureMap) GroupTextureMap = CTX->TextureMap();
  if (! GroupTextureMap.IsNull() )
    MyCGroup.ContextFillArea.Texture.TexId = GroupTextureMap->TextureId();
  else
    MyCGroup.ContextFillArea.Texture.TexId = -1;

  MyCGroup.ContextFillArea.Texture.doTextureMap =
    CTX->TextureMapState () ? 1 : 0;

  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
  Standard_Integer aPolyMode;
  Standard_Real    aPolyFactor, aPolyUnits;
  CTX->PolygonOffsets(aPolyMode, aPolyFactor, aPolyUnits);
  MyCGroup.ContextFillArea.PolygonOffsetMode   = aPolyMode;
  MyCGroup.ContextFillArea.PolygonOffsetFactor = aPolyFactor;
  MyCGroup.ContextFillArea.PolygonOffsetUnits  = aPolyUnits;
  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 

  int noinsert = 1;
  MyGraphicDriver->FaceContextGroup (MyCGroup, noinsert);

  MyCGroup.ContextFillArea.IsSet  = 1;

  Update ();

}

void Graphic3d_Group::SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real R, G, B;
  Standard_Real AScale;
  Quantity_Color AColor;
  Aspect_TypeOfMarker AMType;

  CTX->Values (AColor, AMType, AScale);
  AColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCGroup.ContextMarker.Color.r  = float (R);
  MyCGroup.ContextMarker.Color.g  = float (G);
  MyCGroup.ContextMarker.Color.b  = float (B);
  MyCGroup.ContextMarker.MarkerType       = int (AMType);
  MyCGroup.ContextMarker.Scale    = float (AScale);
  MyCGroup.ContextMarker.IsDef    = 1;

  int noinsert    = 1;
  if ( AMType == Aspect_TOM_USERDEFINED )
  {
    CTX->GetTextureSize( MyMarkWidth, MyMarkHeight );
    MyMarkArray = CTX->GetTexture();
    MyGraphicDriver->MarkerContextGroup (MyCGroup, noinsert, MyMarkWidth, MyMarkHeight, MyMarkArray);
  }
  else
    MyGraphicDriver->MarkerContextGroup (MyCGroup, noinsert);

  MyCGroup.ContextMarker.IsSet    = 1;

  Update ();

}

void Graphic3d_Group::SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_CString AFont;
  Standard_Real R, G, B;
  Standard_Real Rs, Gs, Bs;
  Standard_Real ASpace;
  Standard_Real AnExpansion;
  Quantity_Color AColor;
  Aspect_TypeOfStyleText  AStyle;
  Aspect_TypeOfDisplayText ADisplayType;
  Quantity_Color AColorSubTitle;
  Standard_Boolean ATextZoomable;
  Standard_Real    ATextAngle;
  OSD_FontAspect   ATextFontAspect;

  CTX->Values (AColor, AFont, AnExpansion, ASpace, AStyle, ADisplayType,AColorSubTitle,ATextZoomable,ATextAngle,ATextFontAspect);
  AColor.Values (R, G, B, Quantity_TOC_RGB);
  AColorSubTitle.Values (Rs, Gs, Bs, Quantity_TOC_RGB);

  MyCGroup.ContextText.Color.r    = float (R);
  MyCGroup.ContextText.Color.g    = float (G);
  MyCGroup.ContextText.Color.b    = float (B);
  MyCGroup.ContextText.Font       = (char*)AFont;
  MyCGroup.ContextText.Expan      = float (AnExpansion);
  MyCGroup.ContextText.Space      = float (ASpace);
  MyCGroup.ContextText.Style      = int (AStyle);
  MyCGroup.ContextText.DisplayType= int (ADisplayType);
  MyCGroup.ContextText.ColorSubTitle.r    = float (Rs);
  MyCGroup.ContextText.ColorSubTitle.g    = float (Gs);
  MyCGroup.ContextText.ColorSubTitle.b    = float (Bs);
  MyCGroup.ContextText.TextZoomable   = ATextZoomable;   
  MyCGroup.ContextText.TextAngle    = ATextAngle;   
  MyCGroup.ContextText.TextFontAspect   = (int)ATextFontAspect;

  MyCGroup.ContextText.IsDef      = 1;

  int noinsert    = 1;
  MyGraphicDriver->TextContextGroup (MyCGroup, noinsert);

  MyCGroup.ContextText.IsSet              = 1;

  Update ();

}

void Graphic3d_Group::SetPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real R, G, B;
  Standard_Real AWidth;
  Quantity_Color AColor;
  Aspect_TypeOfLine ALType;

  CTX->Values (AColor, ALType, AWidth);
  AColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCGroup.ContextLine.Color.r    = float (R);
  MyCGroup.ContextLine.Color.g    = float (G);
  MyCGroup.ContextLine.Color.b    = float (B);
  MyCGroup.ContextLine.LineType   = int (ALType);
  MyCGroup.ContextLine.Width              = float (AWidth);
  MyCGroup.ContextLine.IsDef              = 1;

  int noinsert    = 0;
  MyGraphicDriver->LineContextGroup (MyCGroup, noinsert);

  MyCGroup.ContextLine.IsSet              = 1;

  Update ();

}

void Graphic3d_Group::SetPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real R, G, B;
  Standard_Real AWidth;
  Quantity_Color AIntColor;
  Quantity_Color BackIntColor;
  Quantity_Color AEdgeColor;
  Aspect_TypeOfLine ALType;
  Aspect_InteriorStyle AStyle;

  CTX->Values (AStyle, AIntColor, BackIntColor, AEdgeColor, ALType, AWidth);
  AIntColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCGroup.ContextFillArea.Style          = int (AStyle);
  MyCGroup.ContextFillArea.IntColor.r     = float (R);
  MyCGroup.ContextFillArea.IntColor.g     = float (G);
  MyCGroup.ContextFillArea.IntColor.b     = float (B);
#ifdef OCC1174    
  if ( CTX->Distinguish() )
    BackIntColor.Values( R, G, B, Quantity_TOC_RGB );
#endif
  MyCGroup.ContextFillArea.BackIntColor.r = float( R );
  MyCGroup.ContextFillArea.BackIntColor.g = float( G );
  MyCGroup.ContextFillArea.BackIntColor.b = float( B );

  MyCGroup.ContextFillArea.Edge   = (CTX->Edge () ? 1 : 0);
  AEdgeColor.Values (R, G, B, Quantity_TOC_RGB);
  MyCGroup.ContextFillArea.EdgeColor.r    = float (R);
  MyCGroup.ContextFillArea.EdgeColor.g    = float (G);
  MyCGroup.ContextFillArea.EdgeColor.b    = float (B);
  MyCGroup.ContextFillArea.LineType       = int (ALType);
  MyCGroup.ContextFillArea.Width  = float (AWidth);
  MyCGroup.ContextFillArea.Hatch  = int (CTX->HatchStyle ());
  // Front and Back face
  MyCGroup.ContextFillArea.Distinguish    =
    ( CTX->Distinguish () ? 1 : 0 );
  MyCGroup.ContextFillArea.BackFace       =
    ( CTX->BackFace () ? 1 : 0 );

  /*** Back Material ***/
  // Material state
  MyCGroup.ContextFillArea.Back.Shininess =
    float ((CTX->BackMaterial ()).Shininess ());
  MyCGroup.ContextFillArea.Back.Ambient   =
    float ((CTX->BackMaterial ()).Ambient ());
  MyCGroup.ContextFillArea.Back.Diffuse   =
    float ((CTX->BackMaterial ()).Diffuse ());
  MyCGroup.ContextFillArea.Back.Specular  =
    float ((CTX->BackMaterial ()).Specular ());
  MyCGroup.ContextFillArea.Back.Transparency      =
    float ((CTX->BackMaterial ()).Transparency ());
  MyCGroup.ContextFillArea.Back.Emission  =
    float ((CTX->BackMaterial ()).Emissive ());     

  // Reflection mode      
  MyCGroup.ContextFillArea.Back.IsAmbient =
    ( (CTX->BackMaterial ()).ReflectionMode
    (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  MyCGroup.ContextFillArea.Back.IsDiffuse =
    ( (CTX->BackMaterial ()).ReflectionMode
    (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  MyCGroup.ContextFillArea.Back.IsSpecular        =
    ( (CTX->BackMaterial ()).ReflectionMode
    (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  MyCGroup.ContextFillArea.Back.IsEmission        =
    ( (CTX->BackMaterial ()).ReflectionMode
    (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  // Material type
  //JR/Hp
  const Graphic3d_MaterialAspect ama = CTX->BackMaterial () ;
  Standard_Boolean amt = ama.MaterialType(Graphic3d_MATERIAL_PHYSIC) ;
  MyCGroup.ContextFillArea.Back.IsPhysic = ( amt ? 1 : 0 );

  // Specular color                               
  MyCGroup.ContextFillArea.Back.ColorSpec.r       =
    float (((CTX->BackMaterial ()).SpecularColor ()).Red ());
  MyCGroup.ContextFillArea.Back.ColorSpec.g       =
    float (((CTX->BackMaterial ()).SpecularColor ()).Green ());
  MyCGroup.ContextFillArea.Back.ColorSpec.b       =
    float (((CTX->BackMaterial ()).SpecularColor ()).Blue ());

  // Ambient color                                
  MyCGroup.ContextFillArea.Back.ColorAmb.r        =
    float (((CTX->BackMaterial ()).AmbientColor ()).Red ());
  MyCGroup.ContextFillArea.Back.ColorAmb.g        =
    float (((CTX->BackMaterial ()).AmbientColor ()).Green ());
  MyCGroup.ContextFillArea.Back.ColorAmb.b        =
    float (((CTX->BackMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color                                
  MyCGroup.ContextFillArea.Back.ColorDif.r        =
    float (((CTX->BackMaterial ()).DiffuseColor ()).Red ());
  MyCGroup.ContextFillArea.Back.ColorDif.g        =
    float (((CTX->BackMaterial ()).DiffuseColor ()).Green ());
  MyCGroup.ContextFillArea.Back.ColorDif.b        =
    float (((CTX->BackMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  MyCGroup.ContextFillArea.Back.ColorEms.r        =
    float (((CTX->BackMaterial ()).EmissiveColor ()).Red ());
  MyCGroup.ContextFillArea.Back.ColorEms.g        =
    float (((CTX->BackMaterial ()).EmissiveColor ()).Green ());
  MyCGroup.ContextFillArea.Back.ColorEms.b        =
    float (((CTX->BackMaterial ()).EmissiveColor ()).Blue ());

  MyCGroup.ContextFillArea.Back.EnvReflexion = 
    float ((CTX->BackMaterial ()).EnvReflexion());

  /*** Front Material ***/
  // Ligth specificity
  MyCGroup.ContextFillArea.Front.Shininess        =
    float ((CTX->FrontMaterial ()).Shininess ());
  MyCGroup.ContextFillArea.Front.Ambient  =
    float ((CTX->FrontMaterial ()).Ambient ());
  MyCGroup.ContextFillArea.Front.Diffuse  =
    float ((CTX->FrontMaterial ()).Diffuse ());
  MyCGroup.ContextFillArea.Front.Specular =
    float ((CTX->FrontMaterial ()).Specular ());
  MyCGroup.ContextFillArea.Front.Transparency     =
    float ((CTX->FrontMaterial ()).Transparency ());
  MyCGroup.ContextFillArea.Front.Emission =
    float ((CTX->FrontMaterial ()).Emissive ());    

  // Reflection mode      
  MyCGroup.ContextFillArea.Front.IsAmbient        =
    ( (CTX->FrontMaterial ()).ReflectionMode
    (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  MyCGroup.ContextFillArea.Front.IsDiffuse        =
    ( (CTX->FrontMaterial ()).ReflectionMode
    (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  MyCGroup.ContextFillArea.Front.IsSpecular       =
    ( (CTX->FrontMaterial ()).ReflectionMode
    (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  MyCGroup.ContextFillArea.Front.IsEmission       =
    ( (CTX->FrontMaterial ()).ReflectionMode
    (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  //Material type
  //JR/Hp
  const Graphic3d_MaterialAspect amas = CTX->FrontMaterial () ;
  Standard_Boolean amty = amas.MaterialType(Graphic3d_MATERIAL_PHYSIC) ;
  MyCGroup.ContextFillArea.Front.IsPhysic = ( amty ? 1 : 0 );

  // Specular color                               
  MyCGroup.ContextFillArea.Front.ColorSpec.r      =
    float (((CTX->FrontMaterial ()).SpecularColor ()).Red ());
  MyCGroup.ContextFillArea.Front.ColorSpec.g      =
    float (((CTX->FrontMaterial ()).SpecularColor ()).Green ());
  MyCGroup.ContextFillArea.Front.ColorSpec.b      =
    float (((CTX->FrontMaterial ()).SpecularColor ()).Blue ());

  // Ambient color                                
  MyCGroup.ContextFillArea.Front.ColorAmb.r       =
    float (((CTX->FrontMaterial ()).AmbientColor ()).Red ());
  MyCGroup.ContextFillArea.Front.ColorAmb.g       =
    float (((CTX->FrontMaterial ()).AmbientColor ()).Green ());
  MyCGroup.ContextFillArea.Front.ColorAmb.b       =
    float (((CTX->FrontMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color                                
  MyCGroup.ContextFillArea.Front.ColorDif.r       =
    float (((CTX->FrontMaterial ()).DiffuseColor ()).Red ());
  MyCGroup.ContextFillArea.Front.ColorDif.g       =
    float (((CTX->FrontMaterial ()).DiffuseColor ()).Green ());
  MyCGroup.ContextFillArea.Front.ColorDif.b       =
    float (((CTX->FrontMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  MyCGroup.ContextFillArea.Front.ColorEms.r       =
    float (((CTX->FrontMaterial ()).EmissiveColor ()).Red ());
  MyCGroup.ContextFillArea.Front.ColorEms.g       =
    float (((CTX->FrontMaterial ()).EmissiveColor ()).Green ());
  MyCGroup.ContextFillArea.Front.ColorEms.b       =
    float (((CTX->FrontMaterial ()).EmissiveColor ()).Blue ());

  MyCGroup.ContextFillArea.Front.EnvReflexion = 
    float ((CTX->FrontMaterial ()).EnvReflexion());

  MyCGroup.ContextFillArea.IsDef  = 1; // Material definition ok

  Handle(Graphic3d_TextureMap) GroupTextureMap = CTX->TextureMap();
  if (! GroupTextureMap.IsNull() )
    MyCGroup.ContextFillArea.Texture.TexId = GroupTextureMap->TextureId();
  else
    MyCGroup.ContextFillArea.Texture.TexId = -1;

  MyCGroup.ContextFillArea.Texture.doTextureMap =
    CTX->TextureMapState () ? 1 : 0;

  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
  Standard_Integer aPolyMode;
  Standard_Real    aPolyFactor, aPolyUnits;
  CTX->PolygonOffsets(aPolyMode, aPolyFactor, aPolyUnits);
  MyCGroup.ContextFillArea.PolygonOffsetMode   = aPolyMode;
  MyCGroup.ContextFillArea.PolygonOffsetFactor = aPolyFactor;
  MyCGroup.ContextFillArea.PolygonOffsetUnits  = aPolyUnits;
  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets

  int noinsert    = 0;
  MyGraphicDriver->FaceContextGroup (MyCGroup, noinsert);

  MyCGroup.ContextFillArea.IsSet  = 1;

  Update ();

}

void Graphic3d_Group::SetPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real R, G, B;
  Standard_Real AScale;
  Quantity_Color AColor;
  Aspect_TypeOfMarker AMType;

  CTX->Values (AColor, AMType, AScale);
  AColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCGroup.ContextMarker.Color.r  = float (R);
  MyCGroup.ContextMarker.Color.g  = float (G);
  MyCGroup.ContextMarker.Color.b  = float (B);
  MyCGroup.ContextMarker.MarkerType       = int (AMType);
  MyCGroup.ContextMarker.Scale    = float (AScale);
  MyCGroup.ContextMarker.IsDef    = 1;

  int noinsert    = 0;
  if ( AMType == Aspect_TOM_USERDEFINED )
  {
    CTX->GetTextureSize( MyMarkWidth, MyMarkHeight );
    MyMarkArray = CTX->GetTexture();
    MyGraphicDriver->MarkerContextGroup (MyCGroup, noinsert, MyMarkWidth, MyMarkHeight, MyMarkArray);
  }
  else
    MyGraphicDriver->MarkerContextGroup (MyCGroup, noinsert);

  MyCGroup.ContextMarker.IsSet    = 1;

  Update ();

}

void Graphic3d_Group::SetPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_CString AFont;
  Standard_Real R, G, B;
  Standard_Real Rs, Gs, Bs;
  Standard_Real ASpace;
  Standard_Real AnExpansion;
  Quantity_Color AColor;
  Aspect_TypeOfStyleText  AStyle;
  Aspect_TypeOfDisplayText ADisplayType;
  Quantity_Color AColorSubTitle;  
  Standard_Boolean ATextZoomable;
  Standard_Real ATextAngle;
  OSD_FontAspect ATextFontAspect;

  CTX->Values (AColor, AFont, AnExpansion, ASpace, AStyle, ADisplayType,AColorSubTitle,ATextZoomable,ATextAngle,ATextFontAspect);
  AColor.Values (R, G, B, Quantity_TOC_RGB);
  AColorSubTitle.Values (Rs, Gs, Bs, Quantity_TOC_RGB);

  MyCGroup.ContextText.Color.r    = float (R);
  MyCGroup.ContextText.Color.g    = float (G);
  MyCGroup.ContextText.Color.b    = float (B);
  MyCGroup.ContextText.Font       = (char*)AFont;
  MyCGroup.ContextText.Expan      = float (AnExpansion);
  MyCGroup.ContextText.Space      = float (ASpace);
  MyCGroup.ContextText.Style      = int (AStyle);
  MyCGroup.ContextText.DisplayType= int (ADisplayType);
  MyCGroup.ContextText.ColorSubTitle.r    = float (Rs);
  MyCGroup.ContextText.ColorSubTitle.g    = float (Gs);
  MyCGroup.ContextText.ColorSubTitle.b    = float (Bs);
  MyCGroup.ContextText.TextZoomable   = ATextZoomable;  
  MyCGroup.ContextText.TextAngle    = ATextAngle;   
  MyCGroup.ContextText.TextFontAspect   = (int)ATextFontAspect;   
  MyCGroup.ContextText.IsDef              = 1;

  int noinsert    = 0;
  MyGraphicDriver->TextContextGroup (MyCGroup, noinsert);

  MyCGroup.ContextText.IsSet              = 1;

  Update ();

}

Standard_Boolean Graphic3d_Group::IsGroupPrimitivesAspectSet (const Graphic3d_GroupAspect theAspect) const {
    switch (theAspect) {
        case Graphic3d_ASPECT_LINE:      return MyCGroup.ContextLine.IsSet;
        case Graphic3d_ASPECT_TEXT:      return MyCGroup.ContextText.IsSet;
        case Graphic3d_ASPECT_MARKER:    return MyCGroup.ContextMarker.IsSet;
        case Graphic3d_ASPECT_FILL_AREA: return MyCGroup.ContextFillArea.IsSet;
        default: return Standard_False;
    }
} 


void Graphic3d_Group::GroupPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& CTXL, const Handle(Graphic3d_AspectText3d)& CTXT, const Handle(Graphic3d_AspectMarker3d)& CTXM, const Handle(Graphic3d_AspectFillArea3d)& CTXF) const {

  Standard_Real           R, G, B;
  Standard_Real           Rs, Gs, Bs;
  Quantity_Color          AColor;

  Standard_Real           AWidth;
  Aspect_TypeOfLine       ALType;

  Standard_CString    AFont;
  Standard_Real           ASpace;
  Standard_Real           AnExpansion;

  Standard_Real           AScale;
  Aspect_TypeOfMarker     AMType;

  Quantity_Color          AnIntColor;
  Quantity_Color          AnEdgeColor;
  Aspect_InteriorStyle    AStyle;
  Standard_Boolean        EdgeOn = Standard_False;
  Graphic3d_MaterialAspect Front;
  Graphic3d_MaterialAspect Back;
  Aspect_TypeOfStyleText   AStyleT;
  Aspect_TypeOfDisplayText ADisplayType;
  Quantity_Color          AColorSubTitle;

  if (MyCGroup.ContextLine.IsSet) {
    R   = Standard_Real (MyCGroup.ContextLine.Color.r);
    G   = Standard_Real (MyCGroup.ContextLine.Color.g);
    B   = Standard_Real (MyCGroup.ContextLine.Color.b);
    ALType      = Aspect_TypeOfLine (MyCGroup.ContextLine.LineType);
    AWidth      = Standard_Real (MyCGroup.ContextLine.Width);
  }
  else {
    R   = Standard_Real (MyCGroup.Struct->ContextLine.Color.r);
    G   = Standard_Real (MyCGroup.Struct->ContextLine.Color.g);
    B   = Standard_Real (MyCGroup.Struct->ContextLine.Color.b);
    ALType      = Aspect_TypeOfLine (MyCGroup.Struct->ContextLine.LineType);
    AWidth      = Standard_Real (MyCGroup.Struct->ContextLine.Width);
  }
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  CTXL->SetColor (AColor);
  CTXL->SetType (ALType);
  CTXL->SetWidth (AWidth);

  if (MyCGroup.ContextText.IsSet) {
    R   = Standard_Real (MyCGroup.ContextText.Color.r);
    G   = Standard_Real (MyCGroup.ContextText.Color.g);
    B   = Standard_Real (MyCGroup.ContextText.Color.b);
    Rs  = Standard_Real (MyCGroup.ContextText.ColorSubTitle.r);
    Gs  = Standard_Real (MyCGroup.ContextText.ColorSubTitle.g);
    Bs  = Standard_Real (MyCGroup.ContextText.ColorSubTitle.b);
    AFont       = Standard_CString (MyCGroup.ContextText.Font);
    AnExpansion = Standard_Real (MyCGroup.ContextText.Expan);
    ASpace      = Standard_Real (MyCGroup.ContextText.Space);
    AStyleT     = Aspect_TypeOfStyleText (MyCGroup.ContextText.Style);
    ADisplayType= Aspect_TypeOfDisplayText (MyCGroup.ContextText.DisplayType);
  }
  else {
    R   = Standard_Real (MyCGroup.Struct->ContextText.Color.r);
    G   = Standard_Real (MyCGroup.Struct->ContextText.Color.g);
    B   = Standard_Real (MyCGroup.Struct->ContextText.Color.b);
    Rs  = Standard_Real (MyCGroup.Struct->ContextText.ColorSubTitle.r);
    Gs  = Standard_Real (MyCGroup.Struct->ContextText.ColorSubTitle.g);
    Bs  = Standard_Real (MyCGroup.Struct->ContextText.ColorSubTitle.b);
    AFont       = Standard_CString (MyCGroup.Struct->ContextText.Font);
    AnExpansion = Standard_Real (MyCGroup.Struct->ContextText.Expan);
    ASpace      = Standard_Real (MyCGroup.Struct->ContextText.Space);
    AStyleT     = Aspect_TypeOfStyleText (MyCGroup.Struct->ContextText.Style);
    ADisplayType= Aspect_TypeOfDisplayText (MyCGroup.Struct->ContextText.DisplayType);
  }
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  AColorSubTitle.SetValues (Rs, Gs, Bs, Quantity_TOC_RGB);
  CTXT->SetColor (AColor);
  CTXT->SetFont (AFont);
  CTXT->SetExpansionFactor (AnExpansion);
  CTXT->SetSpace (ASpace);
  CTXT->SetStyle (AStyleT);
  CTXT->SetDisplayType (ADisplayType);
  CTXT->SetColorSubTitle (AColorSubTitle);

  if (MyCGroup.ContextMarker.IsSet) {
    R   = Standard_Real (MyCGroup.ContextMarker.Color.r);
    G   = Standard_Real (MyCGroup.ContextMarker.Color.g);
    B   = Standard_Real (MyCGroup.ContextMarker.Color.b);
    AMType      = Aspect_TypeOfMarker (MyCGroup.ContextMarker.MarkerType);
    AScale      = Standard_Real (MyCGroup.ContextMarker.Scale);
    if( AMType == Aspect_TOM_USERDEFINED )
    {

      CTXM->SetTexture( MyMarkWidth, MyMarkHeight, MyMarkArray );
    }
  }
  else {
    R   = Standard_Real (MyCGroup.Struct->ContextMarker.Color.r);
    G   = Standard_Real (MyCGroup.Struct->ContextMarker.Color.g);
    B   = Standard_Real (MyCGroup.Struct->ContextMarker.Color.b);
    AMType      = Aspect_TypeOfMarker (MyCGroup.Struct->ContextMarker.MarkerType);
    AScale      = Standard_Real (MyCGroup.Struct->ContextMarker.Scale);
  }
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  CTXM->SetColor (AColor);
  CTXM->SetType (AMType);
  CTXM->SetScale (AScale);

  if (MyCGroup.ContextFillArea.IsSet) {
    // Interior
    AStyle      = Aspect_InteriorStyle (MyCGroup.ContextFillArea.Style);
    R   = Standard_Real (MyCGroup.ContextFillArea.IntColor.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.IntColor.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.IntColor.b);
    AnIntColor.SetValues (R, G, B, Quantity_TOC_RGB);
    // Edges
    if (MyCGroup.ContextFillArea.Edge == 1) EdgeOn = Standard_True;
    R   = Standard_Real (MyCGroup.ContextFillArea.EdgeColor.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.EdgeColor.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.EdgeColor.b);
    AnEdgeColor.SetValues (R, G, B, Quantity_TOC_RGB);
    ALType      = Aspect_TypeOfLine (MyCGroup.ContextFillArea.LineType);
    AWidth      = Standard_Real (MyCGroup.ContextFillArea.Width);
    // Back Material
    Back.SetShininess (
      Standard_Real (MyCGroup.ContextFillArea.Back.Shininess));
    Back.SetAmbient (
      Standard_Real (MyCGroup.ContextFillArea.Back.Ambient));
    Back.SetDiffuse (
      Standard_Real (MyCGroup.ContextFillArea.Back.Diffuse));
    Back.SetSpecular (
      Standard_Real (MyCGroup.ContextFillArea.Back.Specular));
    Back.SetTransparency (
      Standard_Real (MyCGroup.ContextFillArea.Back.Transparency));
    Back.SetEmissive (
      Standard_Real (MyCGroup.ContextFillArea.Back.Emission));
    if (MyCGroup.ContextFillArea.Back.IsAmbient == 1)
      Back.SetReflectionModeOn (Graphic3d_TOR_AMBIENT);
    else
      Back.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
    if (MyCGroup.ContextFillArea.Back.IsDiffuse == 1)
      Back.SetReflectionModeOn (Graphic3d_TOR_DIFFUSE);
    else
      Back.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
    if (MyCGroup.ContextFillArea.Back.IsSpecular == 1)
      Back.SetReflectionModeOn (Graphic3d_TOR_SPECULAR);
    else
      Back.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);

    R   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorSpec.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorSpec.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorSpec.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Back.SetSpecularColor (AColor);

    R   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorAmb.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorAmb.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorAmb.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Back.SetAmbientColor (AColor);

    R   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorDif.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorDif.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorDif.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Back.SetDiffuseColor (AColor);

    R   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorEms.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorEms.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.Back.ColorEms.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Back.SetEmissiveColor (AColor);

    Back.SetEnvReflexion (MyCGroup.ContextFillArea.Back.EnvReflexion);
    // Front Material
    Front.SetShininess (
      Standard_Real (MyCGroup.ContextFillArea.Front.Shininess));
    Front.SetAmbient (
      Standard_Real (MyCGroup.ContextFillArea.Front.Ambient));
    Front.SetDiffuse (
      Standard_Real (MyCGroup.ContextFillArea.Front.Diffuse));
    Front.SetSpecular (
      Standard_Real (MyCGroup.ContextFillArea.Front.Specular));
    Front.SetTransparency (
      Standard_Real (MyCGroup.ContextFillArea.Front.Transparency));
    Front.SetEmissive (
      Standard_Real (MyCGroup.ContextFillArea.Back.Emission));
    if (MyCGroup.ContextFillArea.Front.IsAmbient == 1)
      Front.SetReflectionModeOn (Graphic3d_TOR_AMBIENT);
    else
      Front.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
    if (MyCGroup.ContextFillArea.Front.IsDiffuse == 1)
      Front.SetReflectionModeOn (Graphic3d_TOR_DIFFUSE);
    else
      Front.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
    if (MyCGroup.ContextFillArea.Front.IsSpecular == 1)
      Front.SetReflectionModeOn (Graphic3d_TOR_SPECULAR);
    else
      Front.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);

    R   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorSpec.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorSpec.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorSpec.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Front.SetSpecularColor (AColor);

    R   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorAmb.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorAmb.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorAmb.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Front.SetAmbientColor (AColor);

    R   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorDif.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorDif.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorDif.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Front.SetDiffuseColor (AColor);

    R   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorEms.r);
    G   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorEms.g);
    B   = Standard_Real (MyCGroup.ContextFillArea.Front.ColorEms.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Front.SetEmissiveColor (AColor);

    Front.SetEnvReflexion (MyCGroup.ContextFillArea.Front.EnvReflexion);

    // Edges
    if (EdgeOn)
      CTXF->SetEdgeOn ();
    else
      CTXF->SetEdgeOff ();
    // Hatch
    CTXF->SetHatchStyle(Aspect_HatchStyle (MyCGroup.ContextFillArea.Hatch));
    // Materials
    // Front and Back face
    if (MyCGroup.ContextFillArea.Distinguish == 1)
      CTXF->SetDistinguishOn ();
    else
      CTXF->SetDistinguishOff ();
    if (MyCGroup.ContextFillArea.BackFace == 1)
      CTXF->SuppressBackFace ();
    else
      CTXF->AllowBackFace ();
    // Texture
    // Pb sur les textures
    //if (MyCGroup.ContextFillArea.Texture.TexId == -1)
    //else
    if (MyCGroup.ContextFillArea.Texture.doTextureMap == 1)
      CTXF->SetTextureMapOn ();
    else
      CTXF->SetTextureMapOff ();

    // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
    CTXF->SetPolygonOffsets(MyCGroup.ContextFillArea.PolygonOffsetMode, 
      MyCGroup.ContextFillArea.PolygonOffsetFactor,
      MyCGroup.ContextFillArea.PolygonOffsetUnits);
    // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
  }
  else {
    // Interior
    AStyle      = Aspect_InteriorStyle (MyCGroup.Struct->ContextFillArea.Style);
    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.IntColor.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.IntColor.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.IntColor.b);
    AnIntColor.SetValues (R, G, B, Quantity_TOC_RGB);
    // Edges
    if (MyCGroup.Struct->ContextFillArea.Edge == 1) EdgeOn = Standard_True;
    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.EdgeColor.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.EdgeColor.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.EdgeColor.b);
    AnEdgeColor.SetValues (R, G, B, Quantity_TOC_RGB);
    ALType      = Aspect_TypeOfLine (MyCGroup.Struct->ContextFillArea.LineType);
    AWidth      = Standard_Real (MyCGroup.Struct->ContextFillArea.Width);
    // Back Material
    Back.SetShininess (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Back.Shininess));
    Back.SetAmbient (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Back.Ambient));
    Back.SetDiffuse (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Back.Diffuse));
    Back.SetSpecular (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Back.Specular));
    Back.SetTransparency (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Back.Transparency));
    Back.SetEmissive (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Back.Emission));
    if (MyCGroup.Struct->ContextFillArea.Back.IsAmbient == 1)
      Back.SetReflectionModeOn (Graphic3d_TOR_AMBIENT);
    else
      Back.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
    if (MyCGroup.Struct->ContextFillArea.Back.IsDiffuse == 1)
      Back.SetReflectionModeOn (Graphic3d_TOR_DIFFUSE);
    else
      Back.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
    if (MyCGroup.Struct->ContextFillArea.Back.IsSpecular == 1)
      Back.SetReflectionModeOn (Graphic3d_TOR_SPECULAR);
    else
      Back.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);

    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorSpec.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorSpec.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorSpec.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Back.SetSpecularColor (AColor);

    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorAmb.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorAmb.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorAmb.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Back.SetAmbientColor (AColor);

    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorDif.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorDif.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorDif.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Back.SetDiffuseColor (AColor);

    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorEms.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorEms.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.Back.ColorEms.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Back.SetEmissiveColor (AColor);

    Back.SetEnvReflexion (MyCGroup.Struct->ContextFillArea.Back.EnvReflexion);
    // Front Material
    Front.SetShininess (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Front.Shininess));
    Front.SetAmbient (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Front.Ambient));
    Front.SetDiffuse (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Front.Diffuse));
    Front.SetSpecular (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Front.Specular));
    Front.SetTransparency (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Front.Transparency));
    Front.SetEmissive (
      Standard_Real (MyCGroup.Struct->ContextFillArea.Front.Emission));
    if (MyCGroup.Struct->ContextFillArea.Front.IsAmbient == 1)
      Front.SetReflectionModeOn (Graphic3d_TOR_AMBIENT);
    else
      Front.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
    if (MyCGroup.Struct->ContextFillArea.Front.IsDiffuse == 1)
      Front.SetReflectionModeOn (Graphic3d_TOR_DIFFUSE);
    else
      Front.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
    if (MyCGroup.Struct->ContextFillArea.Front.IsSpecular == 1)
      Front.SetReflectionModeOn (Graphic3d_TOR_SPECULAR);
    else
      Front.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);

    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorSpec.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorSpec.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorSpec.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Front.SetSpecularColor (AColor);

    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorAmb.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorAmb.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorAmb.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Front.SetAmbientColor (AColor);

    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorDif.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorDif.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorDif.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Front.SetDiffuseColor (AColor);

    R   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorEms.r);
    G   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorEms.g);
    B   = Standard_Real (MyCGroup.Struct->ContextFillArea.Front.ColorEms.b);
    AColor.SetValues (R, G, B, Quantity_TOC_RGB);
    Front.SetEmissiveColor (AColor);

    Front.SetEnvReflexion (MyCGroup.Struct->ContextFillArea.Front.EnvReflexion);

    // Edges
    if (EdgeOn)
      CTXF->SetEdgeOn ();
    else
      CTXF->SetEdgeOff ();
    // Hatch
    CTXF->SetHatchStyle(Aspect_HatchStyle (MyCGroup.Struct->ContextFillArea.Hatch));
    // Materials
    // Front and Back face
    if (MyCGroup.Struct->ContextFillArea.Distinguish == 1)
      CTXF->SetDistinguishOn ();
    else
      CTXF->SetDistinguishOff ();
    if (MyCGroup.Struct->ContextFillArea.BackFace == 1)
      CTXF->SuppressBackFace ();
    else
      CTXF->AllowBackFace ();
    // Texture
    if (MyCGroup.Struct->ContextFillArea.Texture.doTextureMap == 1)
      CTXF->SetTextureMapOn ();
    else
      CTXF->SetTextureMapOff ();

    // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
    CTXF->SetPolygonOffsets(MyCGroup.Struct->ContextFillArea.PolygonOffsetMode, 
      MyCGroup.Struct->ContextFillArea.PolygonOffsetFactor,
      MyCGroup.Struct->ContextFillArea.PolygonOffsetUnits);
    // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
  }
  CTXF->SetInteriorStyle (AStyle);
  CTXF->SetInteriorColor (AnIntColor);
  CTXF->SetEdgeColor (AnEdgeColor);
  CTXF->SetEdgeLineType (ALType);
  CTXF->SetEdgeWidth (AWidth);
  CTXF->SetBackMaterial (Back);
  CTXF->SetFrontMaterial (Front);
}

void Graphic3d_Group::PrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& CTXL, const Handle(Graphic3d_AspectText3d)& CTXT, const Handle(Graphic3d_AspectMarker3d)& CTXM, const Handle(Graphic3d_AspectFillArea3d)& CTXF) const {

        GroupPrimitivesAspect (CTXL, CTXT, CTXM, CTXF);

}
