// File         Graphic3d_Structure.cxx
// Created      Mars 1992
// Author       NW,JPB,CAL
// Modified      1/08/97 ; PCT : ajout texture mapping
//              20/08/97 ; PCT : ajout transparence pour texture
//              11/97 ; CAL : gestion du GraphicClear
//              11/97 ; CAL : ajout pointer StructPtr
//              11/97 ; CAL : amelioration de l'effacement SetManager
//      CAL : 15 janvier 1998
//              Ajout de la methode IsTransformed dans une Structure.
//                      (Permet la transmission de cette Trsf vers
//                      la structure COMPUTED si necessaire)
//
//              Ajout de la methode IsRotated dans une Structure.
//                      (Permet de connaitre le type de transformation
//                      pour declencher le calcul des parties cachees
//                      de maniere plus sioux)
//
//              Ajout de 3 nouvelles methodes Compute virtual dans Structure.
//                      (Permet des Compute un peu plus specialises)
//
//              Ajout des 2 methodes sur Owner dans Structure.
//                      (evite de calculer 2 fois les parties cachees
//                      pour 2 representations differentes affichees
//                      d'un meme Presentable Object (Owner))
//
//              Ajout des 2 methodes sur HLRValidation dans Structure.
//                      (permet de valider ou d'invalider le calcul des
//                      parties cachees sur certaines representations)
//
//              Ajout de la transmission de la transformation d'une structure
//              vers sa COMPUTED.
//              24/2/98 ; CAL : Modification de la gestion des champs
//      CAL : 26 mars 1998
//              Patch, erreur dans la gestion du nombre de Group avec
//              facettes dans Graphic3d_Structure avec une operation
//              de Clear. (PRO12795)
//      FGU : 30 Avril 1998
//              Ajout emission
//      CAL : 20 mai 1998
//              Perfs. Connection entre structures COMPUTED.
//      30/11/98 ; FMN : S4069. Textes always visible.
//      22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets

#define G003    //EUG 26/01/00 Degeneration management

#define BUC60918        //GG 31/05/01 A transparente structure priority must have the
//                      MAX_PRIORITY value so, the highlighted structure must have
//                      MAX_PRIORITY-1 value.
//                      Add ResetDisplayPriority() method.

#define OCC1174 // SAV 08/01/03 Added back face interior color controling


//
//-Copyright    MatraDatavision 1991,1992,1993,1994,1995

//-Version

//-Design       Declaration of variables specific to graphic structures

//-Warning      A structure is defined in a manager
//              This is a sequence of groups of primitives

//-References

//-Language     C++ 2.0

//-Declarations

// for the class

#define XDOWNCAST
#define XDESTROY
#define XDEBUG
#define XTRACE
#define XTRACE_ISTRSF

#include <stdio.h>

#include <Graphic3d_Structure.ixx>
#include <Graphic3d_Structure.pxx>

#include <Graphic3d_GraphicDevice.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_MaterialAspect.hxx>

#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>

#include <Quantity_Color.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Graphic3d_TextureMap.hxx>

// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
#include <Aspect_PolygonOffsetMode.hxx>
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets

//-Aliases

//-Global data definitions

//-Constructors

Graphic3d_Structure::Graphic3d_Structure (const Handle(Graphic3d_StructureManager)& AManager):
MyGroupGenId (Group_IDMIN, Group_IDMAX),
MyAncestors (),
MyDescendants (),
MyGroups (),
MyHighlightColor (Quantity_NOC_WHITE)
{

  Standard_Integer i, j;

  MyOwner = NULL;

  Handle(Graphic3d_AspectLine3d) aAspectLine3d =
    new Graphic3d_AspectLine3d ();
  Handle(Graphic3d_AspectText3d) aAspectText3d =
    new Graphic3d_AspectText3d ();
  Handle(Graphic3d_AspectMarker3d) aAspectMarker3d =
    new Graphic3d_AspectMarker3d ();
  Handle(Graphic3d_AspectFillArea3d) aAspectFillArea3d =
    new Graphic3d_AspectFillArea3d ();

  // Return default values
  AManager->PrimitivesAspect (aAspectLine3d, aAspectText3d,
    aAspectMarker3d, aAspectFillArea3d);

  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
  // It is necessary to set default polygon offsets for a new presentation
  aAspectFillArea3d->SetPolygonOffsets( Aspect_POM_Fill, 1., 0. );
  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets

  // Update the associated CStructure
  UpdateStructure (aAspectLine3d, aAspectText3d,
    aAspectMarker3d, aAspectFillArea3d);

  MyFirstPtrStructureManager = MyPtrStructureManager =
    (void *) AManager.operator->();
  MyCStructure.Id         = int (AManager->NewIdentification ());

  MyCStructure.IsOpen     = 0;
  MyCStructure.IsDeleted  = 0;

  MyCStructure.Composition        = 0; /* Graphic3d_TOC_REPLACE */

  for (i=0; i<=3; i++)
    for (j=0; j<=3; j++)
      if (i == j)
        MyCStructure.Transformation[i][j] = float (1.0);
      else
        MyCStructure.Transformation[i][j] = float (0.0);

  MyCStructure.Priority           = int (Structure_MAX_PRIORITY / 2);
  MyCStructure.PreviousPriority   = MyCStructure.Priority;
  MyCStructure.stick              = 0,
  MyCStructure.highlight          = 0,
  MyCStructure.visible            = 1,
  MyCStructure.pick               = 1;
  MyCStructure.ContainsFacet      = 0,
  MyCStructure.IsInfinite         = 0,
  MyCStructure.HLRValidation      = 0;

  MyCStructure.GroupBegin = Structure_GROUPBEGIN;
  MyCStructure.GroupEnd   = Structure_GROUPEND;

  MyCStructure.ContextLine.IsDef         = 1,
  MyCStructure.ContextFillArea.IsDef     = 1,
  MyCStructure.ContextMarker.IsDef       = 1,
  MyCStructure.ContextText.IsDef         = 1;

  MyCStructure.ContextLine.IsSet         = 0,
  MyCStructure.ContextFillArea.IsSet     = 0,
  MyCStructure.ContextMarker.IsSet       = 0,
  MyCStructure.ContextText.IsSet         = 0;

  MyVisual        = Graphic3d_TOS_ALL;
  MyComputeVisual = Graphic3d_TOS_ALL;

  /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  /*MyCStructure.TransformPersistence.IsDef = 1;*/
  MyCStructure.TransformPersistence.IsSet = 0;
  MyCStructure.TransformPersistence.Flag = Graphic3d_TMF_None;
  MyCStructure.TransformPersistence.Point.x = 0.0;
  MyCStructure.TransformPersistence.Point.y = 0.0;
  MyCStructure.TransformPersistence.Point.z = 0.0;
  /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  Handle(Aspect_GraphicDriver) agd =
    (AManager->GraphicDevice ())->GraphicDriver ();

  MyGraphicDriver = *(Handle(Graphic3d_GraphicDriver) *) &agd;

  MyGraphicDriver->Structure (MyCStructure);

#ifdef TRACE
  cout << "Graphic3d_Structure::Create (" << MyCStructure.Id << ")\n";
#endif

}

//-Destructors

void Graphic3d_Structure::Destroy () {

#ifdef DESTROY
  cout << "Graphic3d_Structure::Destroy (" << MyCStructure.Id << ")\n";
#endif

  // as MyFirstPtrStructureManager can be already destroyed,
  // avoid attempts to access it
  MyFirstPtrStructureManager = 0;
  Remove ();

}

//-Methods, in order

void Graphic3d_Structure::Clear (const Standard_Boolean WithDestruction) {

  if (IsDeleted ()) return;

  if (WithDestruction) {
    MyGroupGenId.Free ();
    MyGroups.Clear ();
  }

  MyCStructure.ContainsFacet      = 0;

  GraphicClear (WithDestruction);
  MyStructureManager->Clear (this, WithDestruction);

  Update ();

}

void Graphic3d_Structure::Remove () {

  if (IsDeleted ()) return;

#ifdef DESTROY
  if (MyDescendants.Length () != 0) {
    cout << "The structure " << MyCStructure.Id << " have "
      << MyDescendants.Length () << " descendants.\n" << flush;
  }
  if (MyAncestors.Length () != 0) {
    cout << "The structure " << MyCStructure.Id << " have "
      << MyAncestors.Length () << " ancestors.\n" << flush;
  }
#endif

  Standard_Integer i, Length;
  //        Standard_Address APtr = (void *) This ().operator->();
  Standard_Address APtr = (void *) this;
  // It is necessary to remove the eventual pointer on the structure
  // that can be destroyed, in the list of descendants
  // of ancesters of this structure and in the list of ancesters
  // of descendants of the same structure.

  Length  = MyDescendants.Length ();
  for (i=1; i<=Length; i++)
    ((Graphic3d_Structure *)
    (MyDescendants.Value (i)))->Remove (APtr, Graphic3d_TOC_ANCESTOR);

  Length  = MyAncestors.Length ();
  for (i=1; i<=Length; i++)
    ((Graphic3d_Structure *)
    (MyAncestors.Value (i)))->Remove (APtr, Graphic3d_TOC_DESCENDANT);

  MyCStructure.ContainsFacet      = 0;
  MyCStructure.IsDeleted  = 1;

  // Destruction of me in the graphic library
  MyGraphicDriver->RemoveStructure (MyCStructure);

  // Liberation of the identification if the destroyed structure
  // in the first manager that performs creation of the structure.
  if ( MyFirstPtrStructureManager )
    MyFirstStructureManager->Remove (Standard_Integer (MyCStructure.Id));

}

void Graphic3d_Structure::Display () {

  if (IsDeleted ()) return;

  if (! MyCStructure.stick) {
    MyCStructure.stick      = 1;
    MyStructureManager->Display (this);
  }

}

void Graphic3d_Structure::Display (const Standard_Integer Priority) {

  if (IsDeleted ()) return;

  SetDisplayPriority (Priority);

  if (! MyCStructure.stick) {
    MyCStructure.stick      = 1;
    MyStructureManager->Display (this);
  }

}

void Graphic3d_Structure::SetDisplayPriority (const Standard_Integer Priority) {

  if (IsDeleted ()) return;

#ifdef BUC60918
  if( Priority == MyCStructure.Priority ) return;
#endif

  MyCStructure.PreviousPriority   = MyCStructure.Priority;
  MyCStructure.Priority   = int (Priority);

  if (MyCStructure.Priority != MyCStructure.PreviousPriority) {
    if ( (MyCStructure.Priority > Structure_MAX_PRIORITY) ||
      (MyCStructure.Priority < Structure_MIN_PRIORITY) )
      Graphic3d_PriorityDefinitionError::Raise
      ("Bad value for StructurePriority");

    if (MyCStructure.stick)
      MyStructureManager->ChangeDisplayPriority (this,
      Standard_Integer (MyCStructure.PreviousPriority),
      Standard_Integer (MyCStructure.Priority));
  }

}

#ifdef BUC60918
void Graphic3d_Structure::ResetDisplayPriority () {

  if (IsDeleted ()) return;

  if (MyCStructure.Priority != MyCStructure.PreviousPriority) {
    Standard_Integer priority = MyCStructure.Priority;
    MyCStructure.Priority = MyCStructure.PreviousPriority;

    if (MyCStructure.stick)
      MyStructureManager->ChangeDisplayPriority (this,
      priority,
      Standard_Integer (MyCStructure.Priority));
  }

}
#endif

Standard_Integer Graphic3d_Structure::DisplayPriority () const {

  return Standard_Integer (MyCStructure.Priority);

}

void Graphic3d_Structure::Erase () {

  if (IsDeleted ()) return;

  if (MyCStructure.stick) {
    MyCStructure.stick = 0;
    MyStructureManager->Erase (this);
  }

}

void Graphic3d_Structure::Highlight (const Aspect_TypeOfHighlightMethod AMethod) {

  if (IsDeleted ()) return;

  // Highlight on already Highlighted structure.
  if (MyCStructure.highlight) {

    Aspect_TypeOfUpdate UpdateMode = MyStructureManager->UpdateMode ();
    if (UpdateMode == Aspect_TOU_WAIT)
      UnHighlight ();
    else {
      // To avoid call of method : Update ()
      // Not useful and can be costly.
      MyStructureManager->SetUpdateMode (Aspect_TOU_WAIT);
      UnHighlight ();
      MyStructureManager->SetUpdateMode (UpdateMode);
    }
  }

#ifdef BUC60918
  SetDisplayPriority (Standard_Integer (Structure_MAX_PRIORITY)-1);
#else
  SetDisplayPriority (Standard_Integer (Structure_MAX_PRIORITY));
#endif

  GraphicHighlight (AMethod);
  MyStructureManager->Highlight (this, AMethod);

  Update ();

}

void Graphic3d_Structure::SetHighlightColor (const Quantity_Color& AColor) {

  if (IsDeleted ()) return;

  if (! MyCStructure.highlight)
    MyHighlightColor = AColor;
  else {
    // Change highlight color on already Highlighted structure.

    Aspect_TypeOfUpdate UpdateMode  = MyStructureManager->UpdateMode ();
    if (UpdateMode == Aspect_TOU_WAIT)
      UnHighlight ();
    else {
      // To avoid call of method : Update ()
      // Not useful and can be costly.
      MyStructureManager->SetUpdateMode (Aspect_TOU_WAIT);
      UnHighlight ();
      MyStructureManager->SetUpdateMode (UpdateMode);
    }
    MyHighlightColor = AColor;
    Highlight (MyHighlightMethod);
  }

}

void Graphic3d_Structure::SetVisible (const Standard_Boolean AValue) {

  if (IsDeleted ()) return;

  MyCStructure.visible = AValue ? 1:0;

  MyGraphicDriver->NameSetStructure (MyCStructure);

  if (AValue)
    MyStructureManager->Visible (this);
  else
    MyStructureManager->Invisible (this);

  Update ();

}

void Graphic3d_Structure::SetPick (const Standard_Boolean AValue) {

  if (IsDeleted ()) return;

  MyCStructure.pick = AValue ? 1:0;

  MyGraphicDriver->NameSetStructure (MyCStructure);

  if (AValue)
    MyStructureManager->Detectable (this);
  else
    MyStructureManager->Undetectable (this);

  Update ();

}

void Graphic3d_Structure::UnHighlight () {

  if (IsDeleted ()) return;

  if (MyCStructure.highlight) {
    MyCStructure.highlight = 0;

    GraphicUnHighlight ();
    MyStructureManager->UnHighlight (this);
#ifdef BUC60918
    ResetDisplayPriority();
#else
    SetDisplayPriority
      (Standard_Integer (MyCStructure.PreviousPriority));
#endif
    Update ();
  }

}

Quantity_Color Graphic3d_Structure::HighlightColor () const {

  return (MyHighlightColor);

}

Standard_Boolean Graphic3d_Structure::IsDisplayed () const {

  if (MyCStructure.stick)
    return (Standard_True);
  else
    return (Standard_False);

}

Standard_Boolean Graphic3d_Structure::IsDeleted () const {

  if (MyCStructure.IsDeleted)
    return (Standard_True);
  else
    return (Standard_False);

}

Standard_Boolean Graphic3d_Structure::IsHighlighted () const {

  if (MyCStructure.highlight)
    return (Standard_True);
  else
    return (Standard_False);

}

Standard_Boolean Graphic3d_Structure::IsSelectable () const {

  if (MyCStructure.pick)
    return (Standard_True);
  else
    return (Standard_False);

}

Standard_Boolean Graphic3d_Structure::IsVisible () const {

  if (MyCStructure.visible)
    return (Standard_True);
  else
    return (Standard_False);

}

Standard_Boolean Graphic3d_Structure::IsRotated () const {

  // A somewhat light test !
  return   ( MyCStructure.Transformation[0][1] != 0.
    || MyCStructure.Transformation[0][2] != 0.
    || MyCStructure.Transformation[1][0] != 0.
    || MyCStructure.Transformation[1][2] != 0.
    || MyCStructure.Transformation[2][0] != 0.
    || MyCStructure.Transformation[2][1] != 0. );

}

Standard_Boolean Graphic3d_Structure::IsTransformed () const {

  Standard_Boolean Result = Standard_False;
  Standard_Integer i, j;

  for (i=0; i<=3 && ! Result; i++)
    for (j=0; j<=3 && ! Result; j++)
      if (i == j)
        Result = MyCStructure.Transformation[i][j] != 1.;
      else
        Result = MyCStructure.Transformation[i][j] != 0.;

#ifdef TRACE_ISTRSF
  cout << "Structure " << Identification ();
  if (Result)
    cout << " is transformed\n" << flush;
  else
    cout << " is not transformed\n" << flush;
#endif

  return Result;

}

Standard_Boolean Graphic3d_Structure::ContainsFacet () const {

  if (IsDeleted ()) return (Standard_False);

  // A structure contains at least one facet :
  // if one of these groups contains at least one facet.

  Standard_Boolean Result1 = Standard_False;
  Standard_Boolean Result2 = Standard_False;
  Standard_Integer i, Length;

  Result1 = (MyCStructure.ContainsFacet > 0);

  // If one of groups contains at least one facet, the structure contains it too.
  if (Result1) return (Result1);

  Length  = MyDescendants.Length ();

  // Stop at the first descendant  containing at least one facet.
  for (i=1; i<=Length && ! Result2; i++)
    Result2 = ((Graphic3d_Structure *)
    (MyDescendants.Value (i)))->ContainsFacet ();

  return (Result2);

}

Standard_Boolean Graphic3d_Structure::IsEmpty () const {

  if (IsDeleted ()) return (Standard_True);

  // A structure is empty :
  // if all these groups are empty or if all
  // groups are empty and all their descendants
  // are empty or if all its descendants are empty.

  Standard_Boolean Result1   = Standard_True;
  Standard_Integer i, Length = MyGroups.Length ();

  // Stop at the first non-empty group
  for (i=1; i<=Length && Result1; i++)
    Result1 = (MyGroups.Value (i))->IsEmpty ();

  // If a group is non-empty the structure is also non-empty.
  if (! Result1) return (Standard_False);

  Standard_Boolean Result2 = Standard_True;

  Length  = MyDescendants.Length ();

  // Stop at the first non-empty descendant
  for (i=1; i<=Length && Result2; i++)
    Result2 = ((Graphic3d_Structure *)
    (MyDescendants.Value (i)))->IsEmpty ();

  return (Result2);

}

void Graphic3d_Structure::PrimitivesAspect (Handle(Graphic3d_AspectLine3d)& CTXL, Handle(Graphic3d_AspectText3d)& CTXT, Handle(Graphic3d_AspectMarker3d)& CTXM, Handle(Graphic3d_AspectFillArea3d)& CTXF) const {

  CTXL     = Line3dAspect ();
  CTXT     = Text3dAspect ();
  CTXM     = Marker3dAspect ();
  CTXF     = FillArea3dAspect ();

}

void Graphic3d_Structure::GroupsWithFacet (const Standard_Integer ADelta) {

  MyCStructure.ContainsFacet = MyCStructure.ContainsFacet + ADelta;
  if (MyCStructure.ContainsFacet < 0) MyCStructure.ContainsFacet = 0;

#ifdef DEBUG
  cout << "MyCStructure.ContainsFacet " << MyCStructure.ContainsFacet;
  cout << endl << flush;
#endif

}

Handle(Graphic3d_Structure) Graphic3d_Structure::Compute (const Handle(Graphic3d_DataStructureManager)& ) {

  // Implemented by Presentation
  return this;
}

Handle(Graphic3d_Structure) Graphic3d_Structure::Compute (const Handle(Graphic3d_DataStructureManager)& , const TColStd_Array2OfReal& ) {

  // Implemented by Presentation
  return this;
}

void Graphic3d_Structure::Compute (const Handle(Graphic3d_DataStructureManager)& , Handle(Graphic3d_Structure)& ) {

  // Implemented by Presentation
}

void Graphic3d_Structure::Compute (const Handle(Graphic3d_DataStructureManager)& , const TColStd_Array2OfReal& , Handle(Graphic3d_Structure)& ) {

  // Implemented by Presentation
}

void Graphic3d_Structure::ReCompute () {

  MyStructureManager->ReCompute (this);

}

void Graphic3d_Structure::ReCompute (const Handle(Graphic3d_DataStructureManager)& aProjector) {

  MyStructureManager->ReCompute (this, aProjector);

}

void Graphic3d_Structure::SetInfiniteState (const Standard_Boolean AValue) {

  MyCStructure.IsInfinite = AValue ? 1:0;

}

Standard_Boolean Graphic3d_Structure::IsInfinite () const {

  if (IsDeleted ()) return (Standard_True);

  if (MyCStructure.IsInfinite)
    return (Standard_True);
  else
    return (Standard_False);

}

void Graphic3d_Structure::GraphicClear (const Standard_Boolean WithDestruction) {

  if (WithDestruction)
    /*
    * Dans ce cas l'appelant dans faire :
    * void Prs3d_Presentation::Clear () {
    *   Graphic3d_Structure::Clear ();
    *   myCurrentGroup = new Graphic3d_Group (this);
    * }
    */
    MyGraphicDriver->ClearStructure (MyCStructure);
  else {
    /*
    * Dans ce cas l'appelant dans faire :
    * void Prs3d_Presentation::Clear () {
    *   Graphic3d_Structure::Clear ();
    *   // myCurrentGroup = new Graphic3d_Group (this);
    * }
    */
    Standard_Integer Length = MyGroups.Length ();

    for (Standard_Integer i=1; i<=Length; i++)
      (MyGroups.Value (i))->Clear ();
  }

}

void Graphic3d_Structure::GraphicConnect (const Handle(Graphic3d_Structure)& ADaughter) {

  MyGraphicDriver->Connect
    (MyCStructure, *((CALL_DEF_STRUCTURE *)ADaughter->CStructure()));

}

void Graphic3d_Structure::GraphicDisconnect (const Handle(Graphic3d_Structure)& ADaughter) {

  MyGraphicDriver->Disconnect
    (MyCStructure, *((CALL_DEF_STRUCTURE *)ADaughter->CStructure()));

}

Handle(Graphic3d_AspectLine3d) Graphic3d_Structure::Line3dAspect () const {

  Standard_Real           R, G, B;
  Standard_Real           AWidth;
  Quantity_Color          AColor;
  Aspect_TypeOfLine       ALType;

  // ContextLine
  R       = Standard_Real (MyCStructure.ContextLine.Color.r);
  G       = Standard_Real (MyCStructure.ContextLine.Color.g);
  B       = Standard_Real (MyCStructure.ContextLine.Color.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  ALType  = Aspect_TypeOfLine (MyCStructure.ContextLine.LineType);
  AWidth  = Standard_Real (MyCStructure.ContextLine.Width);

  Handle(Graphic3d_AspectLine3d) CTXL =
    new Graphic3d_AspectLine3d (AColor, ALType, AWidth);

  return CTXL;

}

Handle(Graphic3d_AspectText3d) Graphic3d_Structure::Text3dAspect () const {

  Standard_Real             R, G, B;
  Standard_CString          AFont;
  Standard_Real             ASpace;
  Standard_Real             AnExpansion;
  Quantity_Color            AColor;
  Aspect_TypeOfStyleText    AStyle;
  Aspect_TypeOfDisplayText  ADisplayType;

  // ContextText
  R       = Standard_Real (MyCStructure.ContextText.Color.r);
  G       = Standard_Real (MyCStructure.ContextText.Color.g);
  B       = Standard_Real (MyCStructure.ContextText.Color.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  AFont   = Standard_CString (MyCStructure.ContextText.Font);
  AnExpansion     = Standard_Real (MyCStructure.ContextText.Expan);
  ASpace  = Standard_Real (MyCStructure.ContextText.Space);
  AStyle  = Aspect_TypeOfStyleText (MyCStructure.ContextText.Style);
  ADisplayType = Aspect_TypeOfDisplayText (MyCStructure.ContextText.DisplayType);

  Handle(Graphic3d_AspectText3d) CTXT =
    new Graphic3d_AspectText3d (AColor, AFont, AnExpansion, ASpace,AStyle,ADisplayType);

  return CTXT;

}

Handle(Graphic3d_AspectMarker3d) Graphic3d_Structure::Marker3dAspect () const {

  Standard_Real           R, G, B;
  Standard_Real           AScale;
  Quantity_Color          AColor;
  Aspect_TypeOfMarker     AMType;

  // ContextMarker
  R       = Standard_Real (MyCStructure.ContextMarker.Color.r);
  G       = Standard_Real (MyCStructure.ContextMarker.Color.g);
  B       = Standard_Real (MyCStructure.ContextMarker.Color.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  AMType  = Aspect_TypeOfMarker (MyCStructure.ContextMarker.MarkerType);
  AScale  = Standard_Real (MyCStructure.ContextMarker.Scale);

  Handle(Graphic3d_AspectMarker3d) CTXM =
    new Graphic3d_AspectMarker3d (AMType, AColor, AScale);

  return CTXM;

}

Handle(Graphic3d_AspectFillArea3d) Graphic3d_Structure::FillArea3dAspect () const {

  Standard_Real             R, G, B;
  Standard_Real             AWidth;
  Quantity_Color            AColor;
  Quantity_Color            AnIntColor;
  Quantity_Color            AnEdgeColor;
  Aspect_TypeOfLine         ALType;
  Aspect_InteriorStyle      AStyle;
  Standard_Boolean          EdgeOn = Standard_False;
  Graphic3d_MaterialAspect  Front;
  Graphic3d_MaterialAspect  Back;

  // ContextFillArea
  AStyle  = Aspect_InteriorStyle (MyCStructure.ContextFillArea.Style);
  R       = Standard_Real (MyCStructure.ContextFillArea.IntColor.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.IntColor.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.IntColor.b);
  AnIntColor.SetValues (R, G, B, Quantity_TOC_RGB);
  // Edges
  if (MyCStructure.ContextFillArea.Edge == 1) EdgeOn = Standard_True;
  R       = Standard_Real (MyCStructure.ContextFillArea.EdgeColor.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.EdgeColor.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.EdgeColor.b);
  AnEdgeColor.SetValues (R, G, B, Quantity_TOC_RGB);
  ALType  = Aspect_TypeOfLine (MyCStructure.ContextFillArea.LineType);
  AWidth  = Standard_Real (MyCStructure.ContextFillArea.Width);
  // Back Material
  Back.SetShininess (
    Standard_Real (MyCStructure.ContextFillArea.Back.Shininess));
  Back.SetAmbient (
    Standard_Real (MyCStructure.ContextFillArea.Back.Ambient));
  Back.SetDiffuse (
    Standard_Real (MyCStructure.ContextFillArea.Back.Diffuse));
  Back.SetSpecular (
    Standard_Real (MyCStructure.ContextFillArea.Back.Specular));
  Back.SetTransparency (
    Standard_Real (MyCStructure.ContextFillArea.Back.Transparency));
  Back.SetEmissive (
    Standard_Real (MyCStructure.ContextFillArea.Back.Emission));
  if (MyCStructure.ContextFillArea.Back.IsAmbient == 1)
    Back.SetReflectionModeOn (Graphic3d_TOR_AMBIENT);
  else
    Back.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
  if (MyCStructure.ContextFillArea.Back.IsDiffuse == 1)
    Back.SetReflectionModeOn (Graphic3d_TOR_DIFFUSE);
  else
    Back.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
  if (MyCStructure.ContextFillArea.Back.IsSpecular == 1)
    Back.SetReflectionModeOn (Graphic3d_TOR_SPECULAR);
  else
    Back.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
  if (MyCStructure.ContextFillArea.Back.IsEmission == 1)
    Back.SetReflectionModeOn (Graphic3d_TOR_EMISSION);
  else
    Back.SetReflectionModeOff (Graphic3d_TOR_EMISSION);

  R       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorSpec.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorSpec.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorSpec.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  Back.SetSpecularColor (AColor);

  R       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorAmb.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorAmb.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorAmb.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  Back.SetAmbientColor (AColor);

  R       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorDif.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorDif.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorDif.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  Back.SetDiffuseColor (AColor);

  R       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorEms.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorEms.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.Back.ColorEms.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  Back.SetEmissiveColor (AColor);

  Back.SetEnvReflexion (MyCStructure.ContextFillArea.Back.EnvReflexion);

  Graphic3d_TypeOfMaterial mType =
    MyCStructure.ContextFillArea.Back.IsPhysic ?
Graphic3d_MATERIAL_PHYSIC : Graphic3d_MATERIAL_ASPECT;
  Back.SetMaterialType(mType);

  // Front Material
  Front.SetShininess (
    Standard_Real (MyCStructure.ContextFillArea.Front.Shininess));
  Front.SetAmbient (
    Standard_Real (MyCStructure.ContextFillArea.Front.Ambient));
  Front.SetDiffuse (
    Standard_Real (MyCStructure.ContextFillArea.Front.Diffuse));
  Front.SetSpecular (
    Standard_Real (MyCStructure.ContextFillArea.Front.Specular));
  Front.SetTransparency (
    Standard_Real (MyCStructure.ContextFillArea.Front.Transparency));
  Front.SetEmissive (
    Standard_Real (MyCStructure.ContextFillArea.Front.Emission));
  if (MyCStructure.ContextFillArea.Front.IsAmbient == 1)
    Front.SetReflectionModeOn (Graphic3d_TOR_AMBIENT);
  else
    Front.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
  if (MyCStructure.ContextFillArea.Front.IsDiffuse == 1)
    Front.SetReflectionModeOn (Graphic3d_TOR_DIFFUSE);
  else
    Front.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
  if (MyCStructure.ContextFillArea.Front.IsSpecular == 1)
    Front.SetReflectionModeOn (Graphic3d_TOR_SPECULAR);
  else
    Front.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
  if (MyCStructure.ContextFillArea.Front.Emission == 1)
    Front.SetReflectionModeOn (Graphic3d_TOR_EMISSION);
  else
    Front.SetReflectionModeOff (Graphic3d_TOR_EMISSION);

  R       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorSpec.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorSpec.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorSpec.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  Front.SetSpecularColor (AColor);

  R       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorAmb.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorAmb.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorAmb.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  Front.SetAmbientColor (AColor);

  R       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorDif.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorDif.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorDif.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  Front.SetDiffuseColor (AColor);

  R       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorEms.r);
  G       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorEms.g);
  B       = Standard_Real (MyCStructure.ContextFillArea.Front.ColorEms.b);
  AColor.SetValues (R, G, B, Quantity_TOC_RGB);
  Front.SetEmissiveColor (AColor);

  Front.SetEnvReflexion (MyCStructure.ContextFillArea.Front.EnvReflexion);

  mType = MyCStructure.ContextFillArea.Front.IsPhysic ? Graphic3d_MATERIAL_PHYSIC : Graphic3d_MATERIAL_ASPECT;
  Front.SetMaterialType(mType);

  Handle(Graphic3d_AspectFillArea3d) CTXF =
    new Graphic3d_AspectFillArea3d (AStyle, AnIntColor, AnEdgeColor, ALType, AWidth, Front, Back);

  // Edges
  if (EdgeOn)
    CTXF->SetEdgeOn ();
  else
    CTXF->SetEdgeOff ();
  // Hatch
  CTXF->SetHatchStyle(Aspect_HatchStyle (MyCStructure.ContextFillArea.Hatch));
  // Materials
  // Front and Back face
  if (MyCStructure.ContextFillArea.Distinguish == 1)
    CTXF->SetDistinguishOn ();
  else
    CTXF->SetDistinguishOff ();
  if (MyCStructure.ContextFillArea.BackFace == 1)
    CTXF->SuppressBackFace ();
  else
    CTXF->AllowBackFace ();
  // Texture
  // Pb sur les textures
  //if (MyCStructure.ContextFillArea.Texture.TexId == -1)
  //else
  if (MyCStructure.ContextFillArea.Texture.doTextureMap == 1)
    CTXF->SetTextureMapOn ();
  else
    CTXF->SetTextureMapOff ();
#ifdef G003
  Aspect_TypeOfDegenerateModel dMode = Aspect_TypeOfDegenerateModel(
    MyCStructure.ContextFillArea.DegenerationMode);
  Quantity_Ratio dRatio =
    MyCStructure.ContextFillArea.SkipRatio;
  CTXF->SetDegenerateModel(dMode,dRatio);
#endif  // G003

  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
  CTXF->SetPolygonOffsets(MyCStructure.ContextFillArea.PolygonOffsetMode,
    MyCStructure.ContextFillArea.PolygonOffsetFactor,
    MyCStructure.ContextFillArea.PolygonOffsetUnits);
  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets

  return CTXF;

}

const Graphic3d_SequenceOfGroup& Graphic3d_Structure::Groups() const {
  return MyGroups;
}

Standard_Integer Graphic3d_Structure::NumberOfGroups () const {
  return (MyGroups.Length ());
}

void Graphic3d_Structure::SetPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real           R, G, B;
  Standard_Real           AWidth;
  Quantity_Color          AColor;
  Aspect_TypeOfLine       ALType;

  CTX->Values (AColor, ALType, AWidth);
  AColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCStructure.ContextLine.Color.r        = float (R);
  MyCStructure.ContextLine.Color.g        = float (G);
  MyCStructure.ContextLine.Color.b        = float (B);
  MyCStructure.ContextLine.LineType       = int (ALType);
  MyCStructure.ContextLine.Width          = float (AWidth);
  MyCStructure.ContextLine.IsDef          = 1;

  MyGraphicDriver->ContextStructure (MyCStructure);

  // CAL 14/04/95
  // Attributes are "IsSet" during the first update
  // of context (line, marker...)
  MyCStructure.ContextLine.IsSet          = 1;
  MyCStructure.ContextFillArea.IsSet      = 1;
  MyCStructure.ContextMarker.IsSet        = 1;
  MyCStructure.ContextText.IsSet          = 1;

  Update ();

}

void Graphic3d_Structure::SetPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real           R, G, B;
  Standard_Real           AWidth;
  Quantity_Color          AnIntColor;
  Quantity_Color          BackIntColor;
  Quantity_Color          AnEdgeColor;
  Aspect_TypeOfLine       ALType;
  Aspect_InteriorStyle    AStyle;

  CTX->Values (AStyle, AnIntColor, BackIntColor, AnEdgeColor, ALType, AWidth);
  AnIntColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCStructure.ContextFillArea.Style      = int (AStyle);
  MyCStructure.ContextFillArea.IntColor.r = float (R);
  MyCStructure.ContextFillArea.IntColor.g = float (G);
  MyCStructure.ContextFillArea.IntColor.b = float (B);

#ifdef OCC1174
  if ( CTX->Distinguish() )
    BackIntColor.Values( R, G, B, Quantity_TOC_RGB );
#endif
  MyCStructure.ContextFillArea.BackIntColor.r     = float( R );
  MyCStructure.ContextFillArea.BackIntColor.g     = float( G );
  MyCStructure.ContextFillArea.BackIntColor.b     = float( B );


  // Edges
  MyCStructure.ContextFillArea.Edge       = CTX->Edge () ? 1:0;
  AnEdgeColor.Values (R, G, B, Quantity_TOC_RGB);
  MyCStructure.ContextFillArea.EdgeColor.r        = float (R);
  MyCStructure.ContextFillArea.EdgeColor.g        = float (G);
  MyCStructure.ContextFillArea.EdgeColor.b        = float (B);
  MyCStructure.ContextFillArea.LineType           = int (ALType);
  MyCStructure.ContextFillArea.Width              = float (AWidth);
  MyCStructure.ContextFillArea.Hatch              = int (CTX->HatchStyle ());
#ifdef G003
  Quantity_Ratio ratio;
  MyCStructure.ContextFillArea.DegenerationMode =
    int (CTX->DegenerateModel(ratio));
  MyCStructure.ContextFillArea.SkipRatio = float (ratio);
#endif  // G003

  /*** Front and Back face ***/
  MyCStructure.ContextFillArea.Distinguish        = CTX->Distinguish () ? 1:0;
  MyCStructure.ContextFillArea.BackFace           = CTX->BackFace () ? 1:0;

  /*** Back Material ***/
  // Light specificity
  MyCStructure.ContextFillArea.Back.Shininess     =
    float ((CTX->BackMaterial ()).Shininess ());
  MyCStructure.ContextFillArea.Back.Ambient       =
    float ((CTX->BackMaterial ()).Ambient ());
  MyCStructure.ContextFillArea.Back.Diffuse       =
    float ((CTX->BackMaterial ()).Diffuse ());
  MyCStructure.ContextFillArea.Back.Specular      =
    float ((CTX->BackMaterial ()).Specular ());
  MyCStructure.ContextFillArea.Back.Transparency  =
    float ((CTX->BackMaterial ()).Transparency ());
  MyCStructure.ContextFillArea.Back.Emission      =
    float ((CTX->BackMaterial ()).Emissive ());

  // Reflection mode
  MyCStructure.ContextFillArea.Back.IsAmbient     =
    ( (CTX->BackMaterial ()).ReflectionMode (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  MyCStructure.ContextFillArea.Back.IsDiffuse     =
    ( (CTX->BackMaterial ()).ReflectionMode (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  MyCStructure.ContextFillArea.Back.IsSpecular    =
    ( (CTX->BackMaterial ()).ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  MyCStructure.ContextFillArea.Back.IsEmission    =
    ( (CTX->BackMaterial ()).ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  // Material type
  //JR/Hp
  const Graphic3d_MaterialAspect ama = CTX->BackMaterial () ;
  Standard_Boolean amt = ama.MaterialType(Graphic3d_MATERIAL_PHYSIC) ;
  MyCStructure.ContextFillArea.Back.IsPhysic = ( amt ? 1 : 0 );

  // Specular Color
  MyCStructure.ContextFillArea.Back.ColorSpec.r   =
    float (((CTX->BackMaterial ()).SpecularColor ()).Red ());
  MyCStructure.ContextFillArea.Back.ColorSpec.g   =
    float (((CTX->BackMaterial ()).SpecularColor ()).Green ());
  MyCStructure.ContextFillArea.Back.ColorSpec.b   =
    float (((CTX->BackMaterial ()).SpecularColor ()).Blue ());

  // Ambient color
  MyCStructure.ContextFillArea.Back.ColorAmb.r    =
    float (((CTX->BackMaterial ()).AmbientColor ()).Red ());
  MyCStructure.ContextFillArea.Back.ColorAmb.g    =
    float (((CTX->BackMaterial ()).AmbientColor ()).Green ());
  MyCStructure.ContextFillArea.Back.ColorAmb.b    =
    float (((CTX->BackMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color
  MyCStructure.ContextFillArea.Back.ColorDif.r    =
    float (((CTX->BackMaterial ()).DiffuseColor ()).Red ());
  MyCStructure.ContextFillArea.Back.ColorDif.g    =
    float (((CTX->BackMaterial ()).DiffuseColor ()).Green ());
  MyCStructure.ContextFillArea.Back.ColorDif.b    =
    float (((CTX->BackMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  MyCStructure.ContextFillArea.Back.ColorEms.r    =
    float (((CTX->BackMaterial ()).EmissiveColor ()).Red ());
  MyCStructure.ContextFillArea.Back.ColorEms.g    =
    float (((CTX->BackMaterial ()).EmissiveColor ()).Green ());
  MyCStructure.ContextFillArea.Back.ColorEms.b    =
    float (((CTX->BackMaterial ()).EmissiveColor ()).Blue ());

  MyCStructure.ContextFillArea.Back.EnvReflexion =
    float ((CTX->BackMaterial ()).EnvReflexion());

  /*** Front Material ***/
  // Light specificity
  MyCStructure.ContextFillArea.Front.Shininess    =
    float ((CTX->FrontMaterial ()).Shininess ());
  MyCStructure.ContextFillArea.Front.Ambient      =
    float ((CTX->FrontMaterial ()).Ambient ());
  MyCStructure.ContextFillArea.Front.Diffuse      =
    float ((CTX->FrontMaterial ()).Diffuse ());
  MyCStructure.ContextFillArea.Front.Specular     =
    float ((CTX->FrontMaterial ()).Specular ());
  MyCStructure.ContextFillArea.Front.Transparency =
    float ((CTX->FrontMaterial ()).Transparency ());
  MyCStructure.ContextFillArea.Front.Emission     =
    float ((CTX->FrontMaterial ()).Emissive ());

  // Reflection mode
  MyCStructure.ContextFillArea.Front.IsAmbient    =
    ( (CTX->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  MyCStructure.ContextFillArea.Front.IsDiffuse    =
    ( (CTX->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  MyCStructure.ContextFillArea.Front.IsSpecular   =
    ( (CTX->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  MyCStructure.ContextFillArea.Front.IsEmission   =
    ( (CTX->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  // Materail type
  //JR/Hp
  const Graphic3d_MaterialAspect amas = CTX->FrontMaterial () ;
  Standard_Boolean amty = amas.MaterialType(Graphic3d_MATERIAL_PHYSIC) ;
  MyCStructure.ContextFillArea.Front.IsPhysic = ( amty ? 1 : 0 );

  // Specular Color
  MyCStructure.ContextFillArea.Front.ColorSpec.r  =
    float (((CTX->FrontMaterial ()).SpecularColor ()).Red ());
  MyCStructure.ContextFillArea.Front.ColorSpec.g  =
    float (((CTX->FrontMaterial ()).SpecularColor ()).Green ());
  MyCStructure.ContextFillArea.Front.ColorSpec.b  =
    float (((CTX->FrontMaterial ()).SpecularColor ()).Blue ());

  // Ambient color
  MyCStructure.ContextFillArea.Front.ColorAmb.r   =
    float (((CTX->FrontMaterial ()).AmbientColor ()).Red ());
  MyCStructure.ContextFillArea.Front.ColorAmb.g   =
    float (((CTX->FrontMaterial ()).AmbientColor ()).Green ());
  MyCStructure.ContextFillArea.Front.ColorAmb.b   =
    float (((CTX->FrontMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color
  MyCStructure.ContextFillArea.Front.ColorDif.r   =
    float (((CTX->FrontMaterial ()).DiffuseColor ()).Red ());
  MyCStructure.ContextFillArea.Front.ColorDif.g   =
    float (((CTX->FrontMaterial ()).DiffuseColor ()).Green ());
  MyCStructure.ContextFillArea.Front.ColorDif.b   =
    float (((CTX->FrontMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  MyCStructure.ContextFillArea.Front.ColorEms.r   =
    float (((CTX->FrontMaterial ()).EmissiveColor ()).Red ());
  MyCStructure.ContextFillArea.Front.ColorEms.g   =
    float (((CTX->FrontMaterial ()).EmissiveColor ()).Green ());
  MyCStructure.ContextFillArea.Front.ColorEms.b   =
    float (((CTX->FrontMaterial ()).EmissiveColor ()).Blue ());

  MyCStructure.ContextFillArea.Front.EnvReflexion =
    float ((CTX->FrontMaterial ()).EnvReflexion());

  MyCStructure.ContextFillArea.IsDef      = 1; // Definition material ok

  Handle(Graphic3d_TextureMap) TempTextureMap = CTX->TextureMap();
  if (! TempTextureMap.IsNull() )
    MyCStructure.ContextFillArea.Texture.TexId = TempTextureMap->TextureId();
  else
    MyCStructure.ContextFillArea.Texture.TexId = -1;

  MyCStructure.ContextFillArea.Texture.doTextureMap = CTX->TextureMapState() ? 1:0;

  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
  Standard_Integer aPolyMode;
  Standard_Real    aPolyFactor, aPolyUnits;
  CTX->PolygonOffsets(aPolyMode, aPolyFactor, aPolyUnits);
  MyCStructure.ContextFillArea.PolygonOffsetMode   = aPolyMode;
  MyCStructure.ContextFillArea.PolygonOffsetFactor = aPolyFactor;
  MyCStructure.ContextFillArea.PolygonOffsetUnits  = aPolyUnits;
  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets

  MyGraphicDriver->ContextStructure (MyCStructure);
#ifdef G003
  MyGraphicDriver -> DegenerateStructure (MyCStructure);
#endif

  // CAL 14/04/95
  // Attributes are "IsSet" during the first update
  // of context (line, marker...)
  MyCStructure.ContextLine.IsSet          = 1;
  MyCStructure.ContextFillArea.IsSet      = 1;
  MyCStructure.ContextMarker.IsSet        = 1;
  MyCStructure.ContextText.IsSet          = 1;

  Update ();

}

void Graphic3d_Structure::SetPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real             R, G, B;
  Standard_Real             Rs, Gs, Bs;
  Standard_CString          AFont;
  Standard_Real             ASpace;
  Standard_Real             AnExpansion;
  Quantity_Color            AColor;
  Aspect_TypeOfStyleText    AStyle;
  Aspect_TypeOfDisplayText  ADisplayType;
  Quantity_Color            AColorSubTitle;
  Standard_Boolean          ATextZoomable;
  Standard_Real             ATextAngle;
  OSD_FontAspect            ATextFontAspect;

  CTX->Values (AColor, AFont, AnExpansion, ASpace, AStyle, ADisplayType,AColorSubTitle,ATextZoomable,ATextAngle,ATextFontAspect);
  AColor.Values (R, G, B, Quantity_TOC_RGB);
  AColorSubTitle.Values (Rs, Gs, Bs, Quantity_TOC_RGB);

  MyCStructure.ContextText.Color.r          = float (R);
  MyCStructure.ContextText.Color.g          = float (G);
  MyCStructure.ContextText.Color.b          = float (B);
  MyCStructure.ContextText.Font             = (char*) (AFont);
  MyCStructure.ContextText.Expan            = float (AnExpansion);
  MyCStructure.ContextText.Space            = float (ASpace);
  MyCStructure.ContextText.Style            = int (AStyle);
  MyCStructure.ContextText.DisplayType      = int (ADisplayType);
  MyCStructure.ContextText.ColorSubTitle.r  = float (Rs);
  MyCStructure.ContextText.ColorSubTitle.g  = float (Gs);
  MyCStructure.ContextText.ColorSubTitle.b  = float (Bs);
  MyCStructure.ContextText.TextZoomable     = ATextZoomable;
  MyCStructure.ContextText.TextAngle        = ATextAngle;
  MyCStructure.ContextText.TextFontAspect   = (int)ATextFontAspect;

  MyCStructure.ContextText.IsDef          = 1;

  MyGraphicDriver->ContextStructure (MyCStructure);

  // CAL 14/04/95
  // Attributes are "IsSet" during the first update of a context (line, marker...)
  MyCStructure.ContextLine.IsSet          = 1;
  MyCStructure.ContextFillArea.IsSet      = 1;
  MyCStructure.ContextMarker.IsSet        = 1;
  MyCStructure.ContextText.IsSet          = 1;

  Update ();

}

void Graphic3d_Structure::SetPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& CTX) {

  if (IsDeleted ()) return;

  Standard_Real           R, G, B;
  Standard_Real           AScale;
  Quantity_Color          AColor;
  Aspect_TypeOfMarker     AMType;

  CTX->Values (AColor, AMType, AScale);
  AColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCStructure.ContextMarker.Color.r      = float (R);
  MyCStructure.ContextMarker.Color.g      = float (G);
  MyCStructure.ContextMarker.Color.b      = float (B);
  MyCStructure.ContextMarker.MarkerType   = int (AMType);
  MyCStructure.ContextMarker.Scale        = float (AScale);
  MyCStructure.ContextMarker.IsDef        = 1;

  MyGraphicDriver->ContextStructure (MyCStructure);

  // Attributes are "IsSet" during the first update of a context (line, marker...)
  MyCStructure.ContextLine.IsSet          = 1;
  MyCStructure.ContextFillArea.IsSet      = 1;
  MyCStructure.ContextMarker.IsSet        = 1;
  MyCStructure.ContextText.IsSet          = 1;

  Update ();

}

void Graphic3d_Structure::SetVisual (const Graphic3d_TypeOfStructure AVisual) {

  if (IsDeleted ()) return;
  if (MyVisual == AVisual) return;

  if (! MyCStructure.stick) {
    MyVisual        = AVisual;
    SetComputeVisual (AVisual);
  }
  else {

    Aspect_TypeOfUpdate UpdateMode  = MyStructureManager->UpdateMode ();
    if (UpdateMode == Aspect_TOU_WAIT) {
      Erase ();
      MyVisual        = AVisual;
      SetComputeVisual (AVisual);
      Display ();
    }
    else {
      // To avoid calling method : Update ()
      // Not useful and can be costly.
      MyStructureManager->SetUpdateMode (Aspect_TOU_WAIT);
      Erase ();
      MyVisual        = AVisual;
      SetComputeVisual (AVisual);
      MyStructureManager->SetUpdateMode (UpdateMode);
      Display ();
    }
  }

}

void Graphic3d_Structure::SetZoomLimit (const Standard_Real LimitInf, const Standard_Real LimitSup) {

  if (LimitInf <= 0.0)
    Graphic3d_StructureDefinitionError::Raise
    ("Bad value for ZoomLimit inf");

  if (LimitSup <= 0.0)
    Graphic3d_StructureDefinitionError::Raise
    ("Bad value for ZoomLimit sup");

  if (LimitSup < LimitInf)
    Graphic3d_StructureDefinitionError::Raise
    ("ZoomLimit sup < ZoomLimit inf");

}

Graphic3d_TypeOfStructure Graphic3d_Structure::Visual () const {

  return (MyVisual);

}

Standard_Boolean Graphic3d_Structure::AcceptConnection (const Handle(Graphic3d_Structure)& AStructure1, const Handle(Graphic3d_Structure)& AStructure2, const Graphic3d_TypeOfConnection AType) {

  Graphic3d_MapOfStructure ASet;

  // cycle detection
  Graphic3d_Structure::Network (AStructure2, AType, ASet);

  return (! ASet.Contains (AStructure1));

}

void Graphic3d_Structure::Ancestors (Graphic3d_MapOfStructure &SG) const {

  Standard_Integer i, Length = MyAncestors.Length ();

  for (i=1; i<=Length; i++)
    SG.Add ((Graphic3d_Structure *) (MyAncestors.Value (i)));


}

void Graphic3d_Structure::SetOwner (const Standard_Address Owner) {

  MyOwner = Owner;

}

Standard_Address Graphic3d_Structure::Owner () const {

  return MyOwner;

}

void Graphic3d_Structure::Descendants (Graphic3d_MapOfStructure& SG) const {

  Standard_Integer i, Length = MyDescendants.Length ();

  for (i=1; i<=Length; i++)
    SG.Add ((Graphic3d_Structure *) (MyDescendants.Value (i)));

}

void Graphic3d_Structure::Connect (const Handle(Graphic3d_Structure)& AStructure, const Graphic3d_TypeOfConnection AType, const Standard_Boolean WithCheck) {

  if (IsDeleted ()) return;

  if (WithCheck)
    // cycle detection
    if (! Graphic3d_Structure::AcceptConnection
      (this, AStructure, AType))
      return;

  // connection
  Standard_Integer i;
  switch (AType)
  {

  case Graphic3d_TOC_DESCENDANT :
    {
      Standard_Integer indexD = 0;
      Standard_Integer LengthD = MyDescendants.Length ();
      for (i=1; i<=LengthD && indexD==0; i++)
        if ((void *) (MyDescendants.Value (i)) ==
          (void *) (AStructure.operator->())) indexD  = i;

      if (indexD == 0) {
        MyDescendants.Append ((void *) AStructure.operator->());
        AStructure->Connect (this, Graphic3d_TOC_ANCESTOR);

        GraphicConnect (AStructure);
        MyStructureManager->Connect (this, AStructure);

        Update ();
      }
    }
    break;

  case Graphic3d_TOC_ANCESTOR :
    {
      Standard_Integer indexA = 0;
      Standard_Integer LengthA        = MyAncestors.Length ();
      for (i=1; i<=LengthA && indexA==0; i++)
        if ((void *) (MyAncestors.Value (i)) ==
          (void *) (AStructure.operator->())) indexA  = i;

      if (indexA == 0) {
        MyAncestors.Append ((void *) AStructure.operator->());
        AStructure->Connect (this, Graphic3d_TOC_DESCENDANT);

        // MyGraphicDriver->Connect is called in case
        // if connection between parent and child
      }
    }
    break;
  }

}

void Graphic3d_Structure::Disconnect (const Handle(Graphic3d_Structure)& AStructure) {

  if (IsDeleted ()) return;

  Standard_Integer i;

  Standard_Integer indexD = 0;
  Standard_Integer LengthD        = MyDescendants.Length ();
  for (i=1; i<=LengthD && indexD==0; i++)
    if ((void *) (MyDescendants.Value (i)) ==
      (void *) (AStructure.operator->())) indexD  = i;

  // Search in the Descendants
  if (indexD != 0) {
    MyDescendants.Remove (indexD);
    AStructure->Disconnect (this);

    GraphicDisconnect (AStructure);
    MyStructureManager->Disconnect (this, AStructure);

    Update ();
  }
  else {
    Standard_Integer indexA = 0;
    Standard_Integer LengthA        = MyAncestors.Length ();
    for (i=1; i<=LengthA && indexA==0; i++)
      if ((void *) (MyAncestors.Value (i)) ==
        (void *) (AStructure.operator->())) indexA  = i;

    // Search in the Ancestors
    if (indexA != 0) {
      MyAncestors.Remove (indexA);
      AStructure->Disconnect (this);

      // No call of MyGraphicDriver->Disconnect
      // in case of an ancestor
    }
  }

}

void Graphic3d_Structure::DisconnectAll (const Graphic3d_TypeOfConnection AType) {

  if (IsDeleted ()) return;

  Standard_Integer i, Length;

  // disconnection
  switch (AType)
  {
  case Graphic3d_TOC_DESCENDANT :
    Length      = MyDescendants.Length ();
    for (i=1; i<=Length; i++)
      // Value (1) instead of Value (i) as MyDescendants
      // is modified by :
      // Graphic3d_Structure::Disconnect (AStructure)
      // that takes AStructure from MyDescendants
      ((Graphic3d_Structure *)
      (MyDescendants.Value (1)))->Disconnect (this);
    break;
  case Graphic3d_TOC_ANCESTOR :
    Length      = MyAncestors.Length ();
    for (i=1; i<=Length; i++)
      // Value (1) instead of Value (i) as MyAncestors
      // is modified by :
      // Graphic3d_Structure::Disconnect (AStructure)
      // that takes AStructure from MyAncestors
      ((Graphic3d_Structure *)
      (MyAncestors.Value (1)))->Disconnect (this);
    break;
  }

}

Graphic3d_TypeOfComposition Graphic3d_Structure::Composition () const {

  if (MyCStructure.Composition)
    return (Graphic3d_TOC_POSTCONCATENATE);
  else
    return (Graphic3d_TOC_REPLACE);

}

void Graphic3d_Structure::SetTransform (const TColStd_Array2OfReal& AMatrix, const Graphic3d_TypeOfComposition AType) {

  if (IsDeleted ()) return;

  Standard_Integer lr, ur, lc, uc;
  Standard_Real valuetrsf;
  Standard_Real valueoldtrsf;
  Standard_Real valuenewtrsf;
  Standard_Integer i, j, k;
  TColStd_Array2OfReal TheTrsf (0, 3, 0, 3);
  TColStd_Array2OfReal NewTrsf (0, 3, 0, 3);
  TColStd_Array2OfReal AMatrix44 (0, 3, 0, 3);

  // Assign the new transformation in an array [0..3][0..3]
  // Avoid problemes if the user has defined matrice [1..4][1..4]
  //                                              or [3..6][-1..2] !!
  lr      = AMatrix.LowerRow ();
  ur      = AMatrix.UpperRow ();
  lc      = AMatrix.LowerCol ();
  uc      = AMatrix.UpperCol ();

  if ( (ur - lr + 1 != 4) || (uc - lc + 1 != 4) )
    Graphic3d_TransformError::Raise
    ("Transform : not a 4x4 matrix");

  if (AType == Graphic3d_TOC_REPLACE) {
    MyCStructure.Composition = 0;
    // Update of CStructure
    for (i=0; i<=3; i++)
      for (j=0; j<=3; j++) {
        MyCStructure.Transformation[i][j] =
          float (AMatrix (lr + i, lc + j));
        NewTrsf (i, j) = AMatrix (lr + i, lc + j);
      }
  }

  if (AType == Graphic3d_TOC_POSTCONCATENATE) {
    MyCStructure.Composition = 1;
    // To simplify management of indices
    for (i=0; i<=3; i++)
      for (j=0; j<=3; j++)
        AMatrix44 (i, j) = AMatrix (lr + i, lc + j);
    // Calculation of the product of matrices
    for (i=0; i<=3; i++)
      for (j=0; j<=3; j++) {
        NewTrsf (i, j) = 0.0;
        for (k=0; k<=3; k++) {
          valueoldtrsf = MyCStructure.Transformation[i][k];
          valuetrsf    = AMatrix44 (k, j);
          valuenewtrsf = NewTrsf (i, j) +
            valueoldtrsf * valuetrsf;
          NewTrsf (i, j) = valuenewtrsf;
        }
      }
      // Update of CStructure
      for (i=0; i<=3; i++)
        for (j=0; j<=3; j++)
          MyCStructure.Transformation[i][j] = float (NewTrsf (i, j));
  }

  // If transformation, no validation of hidden already calculated parts.
  if (IsRotated ())
    ReCompute ();

  GraphicTransform (NewTrsf);
  MyStructureManager->SetTransform (this, NewTrsf);

  Update ();

}

void Graphic3d_Structure::Transform (TColStd_Array2OfReal& AMatrix) const {

  Standard_Integer lr     = AMatrix.LowerRow ();
  Standard_Integer ur     = AMatrix.UpperRow ();
  Standard_Integer lc     = AMatrix.LowerCol ();
  Standard_Integer uc     = AMatrix.UpperCol ();

  if ( (ur - lr + 1 != 4) || (uc - lc + 1 != 4) )
    Graphic3d_TransformError::Raise
    ("Transform : not a 4x4 matrix");

  for (Standard_Integer i=0; i<=3; i++)
    for (Standard_Integer j=0; j<=3; j++)
      AMatrix (lr + i, lc + j) = MyCStructure.Transformation[i][j];

}

void Graphic3d_Structure::MinMaxValues (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) const {

  Standard_Real RL = RealLast ();
  Standard_Real RF = RealFirst ();

  Standard_Real XTMin, YTMin, ZTMin, XTMax, YTMax, ZTMax, U, V, W;

  MinMaxCoord (XTMin, YTMin, ZTMin, XTMax, YTMax, ZTMax);
  if ((XTMin == RF) && (YTMin == RF) &&
      (ZTMin == RF) && (XTMax == RL) &&
      (YTMax == RL) && (ZTMax == RL)) {
      // Case impossible as it would mean that
      // the structure is empty
      XMin = RF;
      YMin = RF;
      ZMin = RF;

      XMax = RL;
      YMax = RL;
      ZMax = RL;
    }
  else {
    Standard_Integer i, j;
    TColStd_Array2OfReal TheTrsf (0, 3, 0, 3);

    for (i=0; i<=3; i++)
      for (j=0; j<=3; j++)
        TheTrsf (i, j) = MyCStructure.Transformation[i][j];

    Graphic3d_Structure::Transforms
      (TheTrsf, XTMin, YTMin, ZTMin, XMin, YMin, ZMin);
    Graphic3d_Structure::Transforms
      (TheTrsf, XTMax, YTMax, ZTMax, XMax, YMax, ZMax);
    Graphic3d_Structure::Transforms
      (TheTrsf, XTMin, YTMin, ZTMax, U, V, W);
    XMin = Min(U,XMin) ; XMax = Max(U,XMax) ;
    YMin = Min(V,YMin) ; YMax = Max(V,YMax) ;
    ZMin = Min(W,ZMin) ; ZMax = Max(W,ZMax) ;
    Graphic3d_Structure::Transforms
      (TheTrsf, XTMax, YTMin, ZTMax, U, V, W);
    XMin = Min(U,XMin) ; XMax = Max(U,XMax) ;
    YMin = Min(V,YMin) ; YMax = Max(V,YMax) ;
    ZMin = Min(W,ZMin) ; ZMax = Max(W,ZMax) ;
    Graphic3d_Structure::Transforms
      (TheTrsf, XTMax, YTMin, ZTMin, U, V, W);
    XMin = Min(U,XMin) ; XMax = Max(U,XMax) ;
    YMin = Min(V,YMin) ; YMax = Max(V,YMax) ;
    ZMin = Min(W,ZMin) ; ZMax = Max(W,ZMax) ;
    Graphic3d_Structure::Transforms
      (TheTrsf, XTMax, YTMax, ZTMin, U, V, W);
    XMin = Min(U,XMin) ; XMax = Max(U,XMax) ;
    YMin = Min(V,YMin) ; YMax = Max(V,YMax) ;
    ZMin = Min(W,ZMin) ; ZMax = Max(W,ZMax) ;
    Graphic3d_Structure::Transforms
      (TheTrsf, XTMin, YTMax, ZTMax, U, V, W);
    XMin = Min(U,XMin) ; XMax = Max(U,XMax) ;
    YMin = Min(V,YMin) ; YMax = Max(V,YMax) ;
    ZMin = Min(W,ZMin) ; ZMax = Max(W,ZMax) ;
    Graphic3d_Structure::Transforms
      (TheTrsf, XTMin, YTMax, ZTMin, U, V, W);
    XMin = Min(U,XMin) ; XMax = Max(U,XMax) ;
    YMin = Min(V,YMin) ; YMax = Max(V,YMax) ;
    ZMin = Min(W,ZMin) ; ZMax = Max(W,ZMax) ;
  }
}

void Graphic3d_Structure::GroupLabels (Standard_Integer& LB, Standard_Integer& LE) {

  LB      = MyGroupGenId.Next ();
  LE      = MyGroupGenId.Next ();

}

Standard_Integer Graphic3d_Structure::Identification () const {

  Standard_Integer Result = MyCStructure.Id;

  return Result;

}

void Graphic3d_Structure::SetTransformPersistence( const Graphic3d_TransModeFlags& AFlag )
{
  SetTransformPersistence( AFlag, gp_Pnt( 0, 0, 0 ) );
}

void Graphic3d_Structure::SetTransformPersistence( const Graphic3d_TransModeFlags& AFlag,
                                                  const gp_Pnt& APoint )
{
  if (IsDeleted ()) return;

  MyCStructure.TransformPersistence.Flag = AFlag;
  MyCStructure.TransformPersistence.Point.x = APoint.X();
  MyCStructure.TransformPersistence.Point.y = APoint.Y();
  MyCStructure.TransformPersistence.Point.z = APoint.Z();
  //MyStructureManager->Update ();
  //Update();
  MyGraphicDriver->ContextStructure( MyCStructure );

  MyCStructure.TransformPersistence.IsSet = 1;
}

Graphic3d_TransModeFlags Graphic3d_Structure::TransformPersistenceMode() const
{
  return MyCStructure.TransformPersistence.Flag;
}

gp_Pnt Graphic3d_Structure::TransformPersistencePoint() const
{
  gp_Pnt aPnt( 0., 0., 0. );
  aPnt.SetX( MyCStructure.TransformPersistence.Point.x );
  aPnt.SetY( MyCStructure.TransformPersistence.Point.y );
  aPnt.SetZ( MyCStructure.TransformPersistence.Point.z );

  return aPnt;
}

void Graphic3d_Structure::Add (const Handle(Graphic3d_Group)& AGroup) {

  // Method called only by the constructor of Graphic3d_Group
  // It is easy to check presence of <AGroup>
  // in sequence MyGroups.
  MyGroups.Append (AGroup);

}

void Graphic3d_Structure::Remove (const Standard_Address APtr, const Graphic3d_TypeOfConnection AType) {

  Standard_Integer i, index, length;

  switch (AType)
  {
  case Graphic3d_TOC_DESCENDANT :
    index   = 0;
    length  = MyDescendants.Length ();
    for (i=1; i<=length && index==0; i++)
      if ((void *) (MyDescendants.Value (i)) == APtr)
        index   = i;
    if (index != 0)
      MyDescendants.Remove (index);
#ifdef TRACE
    else
      cout << "Error, Graphic3d_Structure::Remove\n";
#endif
    break;

  case Graphic3d_TOC_ANCESTOR :
    index   = 0;
    length  = MyAncestors.Length ();
    for (i=1; i<=length && index==0; i++)
      if ((void *) (MyAncestors.Value (i)) == APtr)
        index   = i;
    if (index != 0)
      MyAncestors.Remove (index);
#ifdef TRACE
    else
      cout << "Error, Graphic3d_Structure::Remove\n";
#endif
    break;
  }

}

void Graphic3d_Structure::Remove (const Handle(Graphic3d_Group)& AGroup) {

  Standard_Integer index = 0;
  Standard_Integer Length = MyGroups.Length ();
  for (Standard_Integer i=1; i<=Length && index==0; i++)
    if (MyGroups.Value (i) == AGroup) index = i;

  // Search in Groups
  if (index != 0) {
    Standard_Integer GroupLabelBegin, GroupLabelEnd;
    AGroup->Labels (GroupLabelBegin, GroupLabelEnd);
    MyGroupGenId.Free (GroupLabelBegin);
    MyGroupGenId.Free (GroupLabelEnd);
    MyGroups.Remove (index);
  }

}

Handle(Graphic3d_StructureManager) Graphic3d_Structure::StructureManager () const {

#ifdef DOWNCAST
  return
    (Handle(Graphic3d_StructureManager)::DownCast(Handle(Standard_Transient)
    ((Standard_Transient*) MyPtrStructureManager)));
#else
  return MyStructureManager;
#endif

}

Graphic3d_TypeOfPrimitive Graphic3d_Structure::Type (const Standard_Integer ElementNumber) const {

  if (IsDeleted ()) return (Graphic3d_TOP_UNDEFINED);

  Graphic3d_TypeOfPrimitive Result;

  Result  = MyGraphicDriver->ElementType
    (MyCStructure, ElementNumber);

  return (Result);

}

Standard_Boolean Graphic3d_Structure::Exploration (const Standard_Integer ElementNumber, Graphic3d_VertexNC& AVertex, Graphic3d_Vector& AVector) const {

  Standard_Boolean Result = Standard_False;

  if (IsDeleted ()) return (Result);

  Result  = MyGraphicDriver->ElementExploration
    (MyCStructure, ElementNumber, AVertex, AVector);

  return (Result);

}

void Graphic3d_Structure::Exploration () const {

  if (IsDeleted ()) return;

  MyGraphicDriver->DumpStructure (MyCStructure);

}

void Graphic3d_Structure::MinMaxCoord (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) const {

  Standard_Real RL = RealLast ();
  Standard_Real RF = RealFirst ();

  Standard_Real Xm, Ym, Zm, XM, YM, ZM;

  //Bounding borders of infinite line has been calculated as own point
  //in center of this line
  if (IsEmpty () || IsInfinite ()) {
    if( IsInfinite ()){
      for (int i=1; i<=MyGroups.Length (); i++)
        if (! (MyGroups.Value (i))->IsEmpty () ) {
          (MyGroups.Value (i))->MinMaxValues(Xm, Ym, Zm, XM, YM, ZM);
          Graphic3d_Vertex vertex1(Xm, Ym, Zm);
          Graphic3d_Vertex vertex2(XM, YM, ZM);
          Standard_Real distance = vertex1.Distance( vertex1,vertex2 );
          if( distance >= 500000.0){
            XMin = XMax = (Xm+ XM)/2.0;
            YMin = YMax = (Ym+ YM)/2.0;
            ZMin = ZMax = (Zm+ ZM)/2.0;
            return;
          }
        }
    }
    XMin = RF;
    YMin = RF;
    ZMin = RF;

    XMax = RL;
    YMax = RL;
    ZMax = RL;
  }
  else {
    XMin = RL;
    YMin = RL;
    ZMin = RL;

    XMax = RF;
    YMax = RF;
    ZMax = RF;
    Standard_Integer i, Length;

    Length  = MyGroups.Length ();
    for (i=1; i<=Length; i++)
      if (! (MyGroups.Value (i))->IsEmpty () ) {
        (MyGroups.Value (i))->MinMaxValues(Xm, Ym, Zm, XM, YM, ZM);
        if (Xm < XMin) XMin = Xm;
        if (Ym < YMin) YMin = Ym;
        if (Zm < ZMin) ZMin = Zm;
        if (XM > XMax) XMax = XM;
        if (YM > YMax) YMax = YM;
        if (ZM > ZMax) ZMax = ZM;
      }

      Length  = MyDescendants.Length ();
      for (i=1; i<=Length; i++)
        if (! ((Graphic3d_Structure *)
          (MyDescendants.Value (i)))->IsEmpty () ) {
            ((Graphic3d_Structure *)
              (MyDescendants.Value (i)))->MinMaxValues (Xm, Ym, Zm, XM, YM, ZM);

            if (Xm < XMin) XMin = Xm;
            if (Ym < YMin) YMin = Ym;
            if (Zm < ZMin) ZMin = Zm;
            if (XM > XMax) XMax = XM;
            if (YM > YMax) YMax = YM;
            if (ZM > ZMax) ZMax = ZM;
          }

          if ((XMin == RL) && (YMin == RL) &&
              (ZMin == RL) && (XMax == RF) &&
              (YMax == RF) && (ZMax == RF)) {
              // Case impossible as it would mean
              // that the structure is empty
              XMin    = RF;
              YMin    = RF;
              ZMin    = RF;

              XMax    = RL;
              YMax    = RL;
              ZMax    = RL;
            }

  }

}

void Graphic3d_Structure::Transforms (const TColStd_Array2OfReal& ATrsf, const Standard_Real X, const Standard_Real Y, const Standard_Real Z, Standard_Real& NewX, Standard_Real& NewY, Standard_Real& NewZ) {

  Standard_Real A, B, C, D;

  Standard_Real RL        = RealLast ();
  Standard_Real RF        = RealFirst ();

  if ((X == RF) || (Y == RF) || (Z == RF) ||
    (X == RL) || (Y == RL) || (Z == RL)) {
      NewX    = X;
      NewY    = Y;
      NewZ    = Z;
    }
  else {
    A       = ATrsf (0, 0);
    B       = ATrsf (0, 1);
    C       = ATrsf (0, 2);
    D       = ATrsf (0, 3);
    NewX    = A * X + B * Y + C * Z + D;
    A       = ATrsf (1, 0);
    B       = ATrsf (1, 1);
    C       = ATrsf (1, 2);
    D       = ATrsf (1, 3);
    NewY    = A * X + B * Y + C * Z + D;
    A       = ATrsf (2, 0);
    B       = ATrsf (2, 1);
    C       = ATrsf (2, 2);
    D       = ATrsf (2, 3);
    NewZ    = A * X + B * Y + C * Z + D;
  }

}

Graphic3d_Vector Graphic3d_Structure::Transforms (const TColStd_Array2OfReal& ATrsf, const Graphic3d_Vector& Coord) {

  Standard_Real NewX, NewY, NewZ;
  Graphic3d_Vector Result;

  Graphic3d_Structure::Transforms
    (ATrsf, Coord.X (), Coord.Y (), Coord.Z (), NewX, NewY, NewZ);
  Result.SetCoord (NewX, NewY, NewZ);

  return (Result);

}

Graphic3d_Vertex Graphic3d_Structure::Transforms (const TColStd_Array2OfReal& ATrsf, const Graphic3d_Vertex& Coord) {

  Standard_Real NewX, NewY, NewZ;
  Graphic3d_Vertex Result;

  Graphic3d_Structure::Transforms
    (ATrsf, Coord.X (), Coord.Y (), Coord.Z (), NewX, NewY, NewZ);
  Result.SetCoord (NewX, NewY, NewZ);

  return (Result);

}

void Graphic3d_Structure::Network (const Handle(Graphic3d_Structure)& AStructure, const Graphic3d_TypeOfConnection AType, Graphic3d_MapOfStructure& ASet) {


  Graphic3d_MapOfStructure h1;
  Graphic3d_MapOfStructure h2;
  AStructure->Descendants (h1);
  AStructure->Ancestors (h2);

  Graphic3d_MapIteratorOfMapOfStructure IteratorD (h1);
  Graphic3d_MapIteratorOfMapOfStructure IteratorA (h2);

  ASet.Add (AStructure);

  // exploration
  switch (AType)
  {

  case Graphic3d_TOC_DESCENDANT :
    while (IteratorD.More ()) {
      Graphic3d_Structure::Network
        (IteratorD.Key (), AType, ASet);
      // IteratorD.Next () is located on the next structure
      IteratorD.Next ();
    }
    break;

  case Graphic3d_TOC_ANCESTOR :
    while (IteratorA.More ()) {
      Graphic3d_Structure::Network
        (IteratorA.Key (), AType, ASet);
      // IteratorA.Next () is located on the next structure
      IteratorA.Next ();
    }
    break;
  }

}

void Graphic3d_Structure::PrintNetwork (const Handle(Graphic3d_Structure)& AStructure, const Graphic3d_TypeOfConnection AType)
{

  Graphic3d_MapOfStructure ASet;

  Graphic3d_MapIteratorOfMapOfStructure IteratorASet (ASet);

  Graphic3d_Structure::Network (AStructure, AType, ASet);

  while (IteratorASet.More ()) {
    cout << "\tIdent "
      << (IteratorASet.Key ())->Identification () << "\n";
    IteratorASet.Next ();
  }
  cout << flush;

}

void Graphic3d_Structure::Update () const
{

  if (IsDeleted ()) return;

  if (MyStructureManager->UpdateMode () == Aspect_TOU_ASAP)
    MyStructureManager->Update ();

}

void Graphic3d_Structure::UpdateStructure (const Handle(Graphic3d_AspectLine3d)& CTXL, const Handle(Graphic3d_AspectText3d)& CTXT, const Handle(Graphic3d_AspectMarker3d)& CTXM, const Handle(Graphic3d_AspectFillArea3d)& CTXF)
{

  Standard_Real             R, G, B;
  Standard_Real             Rs, Gs, Bs;
  Standard_CString          AFont;
  Standard_Real             ASpace;
  Standard_Real             AnExpansion;
  Standard_Real             AWidth;
  Standard_Real             AScale;
  Quantity_Color            AColor;
  Quantity_Color            AnIntColor;
  Quantity_Color            BackIntColor;
  Quantity_Color            AnEdgeColor;
  Aspect_TypeOfLine         ALType;
  Aspect_TypeOfMarker       AMType;
  Aspect_InteriorStyle      AStyle;
  Aspect_TypeOfStyleText    AStyleT;
  Aspect_TypeOfDisplayText  ADisplayType;
  Quantity_Color            AColorSubTitle;
  Standard_Boolean          ATextZoomable;
  Standard_Real             ATextAngle;
  OSD_FontAspect            ATextFontAspect;


  CTXL->Values (AColor, ALType, AWidth);
  AColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCStructure.ContextLine.Color.r        = float (R);
  MyCStructure.ContextLine.Color.g        = float (G);
  MyCStructure.ContextLine.Color.b        = float (B);
  MyCStructure.ContextLine.LineType       = int (ALType);
  MyCStructure.ContextLine.Width          = float (AWidth);

  CTXM->Values (AColor, AMType, AScale);
  AColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCStructure.ContextMarker.Color.r      = float (R);
  MyCStructure.ContextMarker.Color.g      = float (G);
  MyCStructure.ContextMarker.Color.b      = float (B);
  MyCStructure.ContextMarker.MarkerType   = int (AMType);
  MyCStructure.ContextMarker.Scale        = float (AScale);

  CTXT->Values (AColor, AFont, AnExpansion, ASpace,AStyleT,ADisplayType,AColorSubTitle,ATextZoomable,ATextAngle,ATextFontAspect);
  AColor.Values (R, G, B, Quantity_TOC_RGB);
  AColorSubTitle.Values (Rs, Gs, Bs, Quantity_TOC_RGB);

  MyCStructure.ContextText.Color.r          = float (R);
  MyCStructure.ContextText.Color.g          = float (G);
  MyCStructure.ContextText.Color.b          = float (B);
  MyCStructure.ContextText.Font             = (char*) (AFont);
  MyCStructure.ContextText.Expan            = float (AnExpansion);
  MyCStructure.ContextText.Style            = int (AStyleT);
  MyCStructure.ContextText.DisplayType      = int (ADisplayType);
  MyCStructure.ContextText.Space            = float (ASpace);
  MyCStructure.ContextText.ColorSubTitle.r  = float (Rs);
  MyCStructure.ContextText.ColorSubTitle.g  = float (Gs);
  MyCStructure.ContextText.ColorSubTitle.b  = float (Bs);
  MyCStructure.ContextText.TextZoomable     = ATextZoomable;
  MyCStructure.ContextText.TextAngle        = ATextAngle;
  MyCStructure.ContextText.TextFontAspect   = (int)ATextFontAspect;



  CTXF->Values (AStyle, AnIntColor, BackIntColor, AnEdgeColor, ALType, AWidth);
  AnIntColor.Values (R, G, B, Quantity_TOC_RGB);

  MyCStructure.ContextFillArea.Style      = int (AStyle);
  MyCStructure.ContextFillArea.IntColor.r = float (R);
  MyCStructure.ContextFillArea.IntColor.g = float (G);
  MyCStructure.ContextFillArea.IntColor.b = float (B);
#ifdef OCC1174
  if ( CTXF->Distinguish() )
    BackIntColor.Values( R, G, B, Quantity_TOC_RGB );
#endif
  MyCStructure.ContextFillArea.BackIntColor.r     = float( R );
  MyCStructure.ContextFillArea.BackIntColor.g     = float( G );
  MyCStructure.ContextFillArea.BackIntColor.b     = float( B );

  // Edges
  MyCStructure.ContextFillArea.Edge       = CTXF->Edge () ? 1:0;
  AnEdgeColor.Values (R, G, B, Quantity_TOC_RGB);
  MyCStructure.ContextFillArea.EdgeColor.r        = float (R);
  MyCStructure.ContextFillArea.EdgeColor.g        = float (G);
  MyCStructure.ContextFillArea.EdgeColor.b        = float (B);
  MyCStructure.ContextFillArea.LineType           = int (ALType);
  MyCStructure.ContextFillArea.Width              = float (AWidth);
  MyCStructure.ContextFillArea.Hatch              = int (CTXF->HatchStyle ());
#ifdef G003
  Quantity_Ratio ratio;
  MyCStructure.ContextFillArea.DegenerationMode =
    int (CTXF->DegenerateModel(ratio));
  MyCStructure.ContextFillArea.SkipRatio = float (ratio);
#endif  // G003


  /*** Front and Back face ***/
  MyCStructure.ContextFillArea.Distinguish = CTXF->Distinguish () ? 1:0;
  MyCStructure.ContextFillArea.BackFace    = CTXF->BackFace () ? 1:0;
  /*** Back Material ***/
  // Light specificity
  MyCStructure.ContextFillArea.Back.Shininess     =
    float ((CTXF->BackMaterial ()).Shininess ());
  MyCStructure.ContextFillArea.Back.Ambient       =
    float ((CTXF->BackMaterial ()).Ambient ());
  MyCStructure.ContextFillArea.Back.Diffuse       =
    float ((CTXF->BackMaterial ()).Diffuse ());
  MyCStructure.ContextFillArea.Back.Specular      =
    float ((CTXF->BackMaterial ()).Specular ());
  MyCStructure.ContextFillArea.Back.Transparency  =
    float ((CTXF->BackMaterial ()).Transparency ());
  MyCStructure.ContextFillArea.Back.Emission      =
    float ((CTXF->BackMaterial ()).Emissive ());

  // Reflection mode
  MyCStructure.ContextFillArea.Back.IsAmbient     =
    ( (CTXF->BackMaterial ()).ReflectionMode (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  MyCStructure.ContextFillArea.Back.IsDiffuse     =
    ( (CTXF->BackMaterial ()).ReflectionMode (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  MyCStructure.ContextFillArea.Back.IsSpecular    =
    ( (CTXF->BackMaterial ()).ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  MyCStructure.ContextFillArea.Back.IsEmission    =
    ( (CTXF->BackMaterial ()).ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  // Material type
  //JR/Hp
  const Graphic3d_MaterialAspect ama = CTXF->BackMaterial () ;
  Standard_Boolean amt = ama.MaterialType(Graphic3d_MATERIAL_PHYSIC) ;
  MyCStructure.ContextFillArea.Back.IsPhysic = ( amt ? 1 : 0 );

  // Specular color
  MyCStructure.ContextFillArea.Back.ColorSpec.r   =
    float (((CTXF->BackMaterial ()).SpecularColor ()).Red ());
  MyCStructure.ContextFillArea.Back.ColorSpec.g   =
    float (((CTXF->BackMaterial ()).SpecularColor ()).Green ());
  MyCStructure.ContextFillArea.Back.ColorSpec.b   =
    float (((CTXF->BackMaterial ()).SpecularColor ()).Blue ());

  // Ambient color
  MyCStructure.ContextFillArea.Back.ColorAmb.r    =
    float (((CTXF->BackMaterial ()).AmbientColor ()).Red ());
  MyCStructure.ContextFillArea.Back.ColorAmb.g    =
    float (((CTXF->BackMaterial ()).AmbientColor ()).Green ());
  MyCStructure.ContextFillArea.Back.ColorAmb.b    =
    float (((CTXF->BackMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color
  MyCStructure.ContextFillArea.Back.ColorDif.r    =
    float (((CTXF->BackMaterial ()).DiffuseColor ()).Red ());
  MyCStructure.ContextFillArea.Back.ColorDif.g    =
    float (((CTXF->BackMaterial ()).DiffuseColor ()).Green ());
  MyCStructure.ContextFillArea.Back.ColorDif.b    =
    float (((CTXF->BackMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  MyCStructure.ContextFillArea.Back.ColorEms.r    =
    float (((CTXF->BackMaterial ()).EmissiveColor ()).Red ());
  MyCStructure.ContextFillArea.Back.ColorEms.g    =
    float (((CTXF->BackMaterial ()).EmissiveColor ()).Green ());
  MyCStructure.ContextFillArea.Back.ColorEms.b    =
    float (((CTXF->BackMaterial ()).EmissiveColor ()).Blue ());

  MyCStructure.ContextFillArea.Back.EnvReflexion =
    float ((CTXF->BackMaterial ()).EnvReflexion());

  /*** Front Material ***/
  // Light specificity
  MyCStructure.ContextFillArea.Front.Shininess    =
    float ((CTXF->FrontMaterial ()).Shininess ());
  MyCStructure.ContextFillArea.Front.Ambient      =
    float ((CTXF->FrontMaterial ()).Ambient ());
  MyCStructure.ContextFillArea.Front.Diffuse      =
    float ((CTXF->FrontMaterial ()).Diffuse ());
  MyCStructure.ContextFillArea.Front.Specular     =
    float ((CTXF->FrontMaterial ()).Specular ());
  MyCStructure.ContextFillArea.Front.Transparency =
    float ((CTXF->FrontMaterial ()).Transparency ());
  MyCStructure.ContextFillArea.Front.Emission     =
    float ((CTXF->FrontMaterial ()).Emissive ());

  // Reflection mode
  MyCStructure.ContextFillArea.Front.IsAmbient    =
    ( (CTXF->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  MyCStructure.ContextFillArea.Front.IsDiffuse    =
    ( (CTXF->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  MyCStructure.ContextFillArea.Front.IsSpecular   =
    ( (CTXF->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  MyCStructure.ContextFillArea.Front.IsEmission   =
    ( (CTXF->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  // Material type
  //JR/Hp
  const Graphic3d_MaterialAspect amas = CTXF->FrontMaterial () ;
  Standard_Boolean amty = amas.MaterialType(Graphic3d_MATERIAL_PHYSIC) ;
  MyCStructure.ContextFillArea.Front.IsPhysic = ( amty ? 1 : 0 );

  // Specular color
  MyCStructure.ContextFillArea.Front.ColorSpec.r  =
    float (((CTXF->FrontMaterial ()).SpecularColor ()).Red ());
  MyCStructure.ContextFillArea.Front.ColorSpec.g  =
    float (((CTXF->FrontMaterial ()).SpecularColor ()).Green ());
  MyCStructure.ContextFillArea.Front.ColorSpec.b  =
    float (((CTXF->FrontMaterial ()).SpecularColor ()).Blue ());

  // Ambient color
  MyCStructure.ContextFillArea.Front.ColorAmb.r   =
    float (((CTXF->FrontMaterial ()).AmbientColor ()).Red ());
  MyCStructure.ContextFillArea.Front.ColorAmb.g   =
    float (((CTXF->FrontMaterial ()).AmbientColor ()).Green ());
  MyCStructure.ContextFillArea.Front.ColorAmb.b   =
    float (((CTXF->FrontMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color
  MyCStructure.ContextFillArea.Front.ColorDif.r   =
    float (((CTXF->FrontMaterial ()).DiffuseColor ()).Red ());
  MyCStructure.ContextFillArea.Front.ColorDif.g   =
    float (((CTXF->FrontMaterial ()).DiffuseColor ()).Green ());
  MyCStructure.ContextFillArea.Front.ColorDif.b   =
    float (((CTXF->FrontMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  MyCStructure.ContextFillArea.Front.ColorEms.r   =
    float (((CTXF->FrontMaterial ()).EmissiveColor ()).Red ());
  MyCStructure.ContextFillArea.Front.ColorEms.g   =
    float (((CTXF->FrontMaterial ()).EmissiveColor ()).Green ());
  MyCStructure.ContextFillArea.Front.ColorEms.b   =
    float (((CTXF->FrontMaterial ()).EmissiveColor ()).Blue ());

  MyCStructure.ContextFillArea.Front.EnvReflexion =
    float ((CTXF->FrontMaterial ()).EnvReflexion());

  Handle(Graphic3d_TextureMap) TempTextureMap = CTXF->TextureMap();
  if (! TempTextureMap.IsNull() )
    MyCStructure.ContextFillArea.Texture.TexId = TempTextureMap->TextureId();
  else
    MyCStructure.ContextFillArea.Texture.TexId = -1;

  MyCStructure.ContextFillArea.Texture.doTextureMap = CTXF->TextureMapState() ? 1:0;

  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
  Standard_Integer aPolyMode;
  Standard_Real    aPolyFactor, aPolyUnits;
  CTXF->PolygonOffsets(aPolyMode, aPolyFactor, aPolyUnits);
  MyCStructure.ContextFillArea.PolygonOffsetMode   = aPolyMode;
  MyCStructure.ContextFillArea.PolygonOffsetFactor = aPolyFactor;
  MyCStructure.ContextFillArea.PolygonOffsetUnits  = aPolyUnits;
  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
}

void Graphic3d_Structure::GraphicHighlight (const Aspect_TypeOfHighlightMethod AMethod) {

  Standard_Real XMin, YMin, ZMin, XMax, YMax, ZMax;
  Standard_Real R, G, B;

  MyCStructure.highlight  = 1;
  MyHighlightMethod       = AMethod;

  switch (AMethod)
  {
  case Aspect_TOHM_COLOR :
    MyHighlightColor.Values (R, G, B, Quantity_TOC_RGB);
    MyGraphicDriver->HighlightColor
      (MyCStructure, float (R), float (G),
      float (B) , Standard_True);
    MyGraphicDriver->NameSetStructure (MyCStructure);
    break;
  case Aspect_TOHM_BLINK :
    MyGraphicDriver->Blink (MyCStructure, Standard_True);
    MyGraphicDriver->NameSetStructure (MyCStructure);
    break;
  case Aspect_TOHM_BOUNDBOX :
    if (IsEmpty () || IsInfinite ()) {
      // Empty or infinite structure
      XMin = YMin = ZMin = 0.;
      XMax = YMax = ZMax = 0.;
    }
    else {
      MinMaxCoord
        (XMin, YMin, ZMin, XMax, YMax, ZMax);
    }
    MyCStructure.BoundBox.Pmin.x    = float (XMin);
    MyCStructure.BoundBox.Pmin.y    = float (YMin);
    MyCStructure.BoundBox.Pmin.z    = float (ZMin);
    MyCStructure.BoundBox.Pmax.x    = float (XMax);
    MyCStructure.BoundBox.Pmax.y    = float (YMax);
    MyCStructure.BoundBox.Pmax.z    = float (ZMax);
    MyHighlightColor.Values (R, G, B, Quantity_TOC_RGB);
    MyCStructure.BoundBox.Color.r   = float (R);
    MyCStructure.BoundBox.Color.g   = float (G);
    MyCStructure.BoundBox.Color.b   = float (B);
    MyGraphicDriver->BoundaryBox (MyCStructure, Standard_True);
    break;
  }

}

void Graphic3d_Structure::GraphicTransform (const TColStd_Array2OfReal& AMatrix) {

  Standard_Integer i, j;

  for (i=0; i<=3; i++)
    for (j=0; j<=3; j++)
      MyCStructure.Transformation[i][j] = float (AMatrix (i, j));

  MyGraphicDriver->TransformStructure (MyCStructure);

}

void Graphic3d_Structure::GraphicUnHighlight () {

  MyCStructure.highlight  = 0;

  switch (MyHighlightMethod)
  {
  case Aspect_TOHM_COLOR :
    MyGraphicDriver->HighlightColor
      (MyCStructure, 0.0, 0.0, 0.0 , Standard_False);
    MyGraphicDriver->NameSetStructure (MyCStructure);
    break;
  case Aspect_TOHM_BLINK :
    MyGraphicDriver->Blink (MyCStructure, Standard_False);
    MyGraphicDriver->NameSetStructure (MyCStructure);
    break;
  case Aspect_TOHM_BOUNDBOX :
    MyGraphicDriver->BoundaryBox
      (MyCStructure, Standard_False);
    MyGraphicDriver->NameSetStructure (MyCStructure);
    break;
  }

}

Graphic3d_TypeOfStructure Graphic3d_Structure::ComputeVisual () const {

  return (MyComputeVisual);

}

void Graphic3d_Structure::SetComputeVisual (const Graphic3d_TypeOfStructure AVisual) {

  // The ComputeVisual is saved only if the structure is
  // declared TOS_ALL, TOS_WIREFRAME or TOS_SHADING.
  // This declaration permits to calculate
  // proper representation of the structure calculated by Compute
  // instead of passage to TOS_COMPUTED.
  if (AVisual != Graphic3d_TOS_COMPUTED)
    MyComputeVisual = AVisual;

}

void Graphic3d_Structure::Plot (const Handle(Graphic3d_Plotter)& ) {

}

void Graphic3d_Structure::SetManager (const Handle(Graphic3d_StructureManager)& AManager, const Standard_Boolean WithPropagation) {

  // All connected structures should follow ?
#ifdef IMPLEMENTED
  if (WithPropagation) {
    Standard_Integer i, Length;
    Length  = MyDescendants.Length ();
    for (i=1; i<=Length; i++)
      ((Graphic3d_Structure *)
      (MyDescendants.Value (i)))->SetStructureManager (AManager);

    Length  = MyAncestors.Length ();
    for (i=1; i<=Length; i++)
      ((Graphic3d_Structure *)
      (MyAncestors.Value (i)))->SetStructureManager (AManager);
  }
#endif

  // change of identification ?
  // MyStructureManager->Remove (Standard_Integer (MyCStructure.Id));
  // AManager->NewIdentification ();
  // MyCStructure.Id      = int (AManager->NewIdentification ());

  Aspect_TypeOfUpdate UpdateMode = MyStructureManager->UpdateMode ();
  Aspect_TypeOfUpdate NewUpdateMode = AManager->UpdateMode ();
  MyStructureManager->SetUpdateMode (Aspect_TOU_WAIT);
  AManager->SetUpdateMode (Aspect_TOU_WAIT);

  if (MyCStructure.stick) {
    MyStructureManager->Erase (this);
    AManager->Display (this);
  }

  if (MyCStructure.highlight) {
  }

  if (MyCStructure.visible) {
    MyStructureManager->Invisible (this);
    AManager->Visible (this);
  }

  if (MyCStructure.pick) {
    MyStructureManager->Undetectable (this);
    AManager->Detectable (this);
  }

  MyStructureManager->SetUpdateMode (UpdateMode);
  AManager->SetUpdateMode (NewUpdateMode);

  // New manager
  MyPtrStructureManager   = (void *) AManager.operator->();

}

void Graphic3d_Structure::SetHLRValidation (const Standard_Boolean AFlag) {

  MyCStructure.HLRValidation      = AFlag ? 1:0;

}

Standard_Boolean Graphic3d_Structure::HLRValidation () const {

  // Hidden parts stored in <me> are valid if :
  // 1/ the owner is defined.
  // 2/ they are not invalid.

  Standard_Boolean Result = MyOwner != NULL && MyCStructure.HLRValidation != 0;

  return Result;

}

Standard_Address Graphic3d_Structure::CStructure () const {

  return Standard_Address (&MyCStructure);

}
