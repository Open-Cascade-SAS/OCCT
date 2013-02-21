// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

/***********************************************************************

FONCTION :
--------
Classe Visual3d_Layer :

HISTORIQUE DES MODIFICATIONS :
----------------------------
Novembre 1998 : CAL : Creation.
??-??-?? : ??? ; 

************************************************************************/

// for the class
#include <Visual3d_Layer.ixx>

#include <Graphic3d_TypeOfPrimitive.hxx>
#include <Visual3d_ViewManager.hxx>

//-Aliases
#define MyViewManager ((Visual3d_ViewManager *) MyPtrViewManager)

//-Global data definitions
static Standard_Boolean theLayerState = Standard_False;
static Graphic3d_TypeOfPrimitive theTypeOfPrimitive = Graphic3d_TOP_UNDEFINED;

#define NO_TRACE_CALL
#define NO_DEBUG

//
//-Constructors
//

Visual3d_Layer::Visual3d_Layer (const Handle(Visual3d_ViewManager)& AManager, const Aspect_TypeOfLayer ALayerType, const Standard_Boolean AFlag) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::Visual3d_Layer" << endl;
#endif
  MyPtrViewManager  = (void *) AManager.operator->();

  MyGraphicDriver = MyViewManager->GraphicDriver();

  MyCLayer.layerType  = int (ALayerType);
  MyCLayer.ptrLayer = NULL;
  MyCLayer.ortho[0] = -1.0;
  MyCLayer.ortho[1] =  1.0;
  MyCLayer.ortho[2] = -1.0;
  MyCLayer.ortho[3] =  1.0;
  MyCLayer.attach   = 0; // Aspect_TOC_BOTTOM_LEFT
  MyCLayer.sizeDependent  = (AFlag ? 1 : 0);

  MyViewManager->SetLayer (this);
  MyGraphicDriver->Layer (MyCLayer);
  MyCLayer.ptrLayer->layerData = this;

#ifdef DEBUG
  cout << "Visual3d_Layer::Visual3d_Layer" << endl;
  call_def_ptrLayer ptrLayer;
  ptrLayer = (call_def_ptrLayer) MyCLayer.ptrLayer;
  if (ptrLayer == NULL)
    cout << "\tptrLayer == NULL" << endl;
  else
    cout << "\tptrLayer->listIndex = " << ptrLayer->listIndex << endl;
#endif
}

//
//-Destructors
//

void Visual3d_Layer::Destroy () {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::Destroy" << endl;
#endif
  MyGraphicDriver->RemoveLayer (MyCLayer);
}

//-Methods, in order

void Visual3d_Layer::Begin () {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::Begin" << endl;
#endif
  if (theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is already open !");

  MyGraphicDriver->BeginLayer (MyCLayer);
  theLayerState = Standard_True;
}

void Visual3d_Layer::End () {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::End" << endl;
#endif
#ifdef DEBUG
  cout << "Visual3d_Layer::End" << endl;
  call_def_ptrLayer ptrLayer;
  ptrLayer = (call_def_ptrLayer) MyCLayer.ptrLayer;
  if (ptrLayer == NULL)
    cout << "\tptrLayer == NULL" << endl;
  else
    cout << "\tptrLayer->listIndex = " << ptrLayer->listIndex << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");

  // Flush all graphics
  MyGraphicDriver->EndLayer ();
  theLayerState = Standard_False;
#ifdef DEBUG
  cout << "Visual3d_Layer::End" << endl;
  ptrLayer = (call_def_ptrLayer) MyCLayer.ptrLayer;
  if (ptrLayer == NULL)
    cout << "\tptrLayer == NULL" << endl;
  else
    cout << "\tptrLayer->listIndex = " << ptrLayer->listIndex << endl;
#endif
}

void Visual3d_Layer::Clear () {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::Clear" << endl;
#endif
  if (theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is open !");

  MyGraphicDriver->ClearLayer (MyCLayer);
}

//
//-Graphic definition methods
//

void Visual3d_Layer::BeginPolyline () {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::BeginPolyline" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");
  if( theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
    Visual3d_LayerDefinitionError::Raise
    ("One primitive is already open !");

  theTypeOfPrimitive = Graphic3d_TOP_POLYLINE;

  MyGraphicDriver->BeginPolyline2d ();
}

void Visual3d_Layer::BeginPolygon () {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::BeginPolygon" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");
  if( theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
    Visual3d_LayerDefinitionError::Raise
    ("One primitive is already open !");

  theTypeOfPrimitive = Graphic3d_TOP_POLYGON;

  MyGraphicDriver->BeginPolygon2d ();
}

void Visual3d_Layer::AddVertex (const Standard_Real X, const Standard_Real Y, const Standard_Boolean AFlag) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::AddVertex" << endl;
#endif
  Standard_ShortReal x = Standard_ShortReal(X);
  Standard_ShortReal y = Standard_ShortReal(Y);

  switch (theTypeOfPrimitive) {
    case Graphic3d_TOP_POLYLINE :
      if( AFlag ) MyGraphicDriver->Draw (x, y);
      else        MyGraphicDriver->Move (x, y);
      break;
    case Graphic3d_TOP_POLYGON :
      MyGraphicDriver->Edge (x, y);
      break;
    default:
      Visual3d_LayerDefinitionError::Raise
        ("Bad Primitive type!");
  }
}

void Visual3d_Layer::ClosePrimitive () {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::ClosePrimitive" << endl;
#endif
  switch (theTypeOfPrimitive) {
    case Graphic3d_TOP_POLYLINE :
      MyGraphicDriver->EndPolyline2d ();
      break;
    case Graphic3d_TOP_POLYGON :
      MyGraphicDriver->EndPolygon2d ();
      break;
    default:
      Visual3d_LayerDefinitionError::Raise
        ("Bad Primitive type!");
  }

  theTypeOfPrimitive = Graphic3d_TOP_UNDEFINED;
}

void Visual3d_Layer::DrawRectangle (const Standard_Real X, const Standard_Real Y, const Standard_Real Width, const Standard_Real Height) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::DrawRectangle" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");
  if (theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
    Visual3d_LayerDefinitionError::Raise
    ("One primitive is already open !");

  Standard_ShortReal x = Standard_ShortReal (X);
  Standard_ShortReal y = Standard_ShortReal (Y);
  Standard_ShortReal width = Standard_ShortReal (Width);
  Standard_ShortReal height = Standard_ShortReal (Height);
  MyGraphicDriver->Rectangle (x, y, width, height);
}

#ifdef WNT
#undef DrawText
#endif
void Visual3d_Layer::DrawText (const Standard_CString AText, const Standard_Real X, const Standard_Real Y, const Standard_Real AHeight) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::DrawText" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");
  if (theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
    Visual3d_LayerDefinitionError::Raise
    ("One primitive is already open !");

  Standard_ShortReal x = Standard_ShortReal (X);
  Standard_ShortReal y = Standard_ShortReal (Y);
  Standard_ShortReal height = Standard_ShortReal (AHeight);
  MyGraphicDriver->Text (AText, x, y, height);
}

void Visual3d_Layer::SetColor (const Quantity_Color& AColor) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::SetColor" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");

  Standard_ShortReal r = Standard_ShortReal (AColor.Red());
  Standard_ShortReal g = Standard_ShortReal (AColor.Green());
  Standard_ShortReal b = Standard_ShortReal (AColor.Blue());
#ifdef TRACE_CALL
  cout << "\tSetColor : " << r << ", " << g << ", " << b << endl;
#endif
  MyGraphicDriver->SetColor (r, g, b);
}

void Visual3d_Layer::SetTransparency (const Standard_ShortReal ATransparency) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::SetTransparency" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");

#ifdef TRACE_CALL
  cout << "\tSetTransparency : " << ATransparency << endl;
#endif
  MyGraphicDriver->SetTransparency (ATransparency);
}

void Visual3d_Layer::UnsetTransparency () {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::UnsetTransparency" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");

#ifdef TRACE_CALL
  cout << "\tUnsetTransparency" << endl;
#endif
  MyGraphicDriver->UnsetTransparency ();
}

#ifdef WNT
#undef DrawText
#endif
void Visual3d_Layer::TextSize (const Standard_CString AText,  const Standard_Real AHeight, Standard_Real& AWidth, Standard_Real& AnAscent, Standard_Real& ADescent) const {
  Standard_ShortReal aWidth, anAscent, aDescent;

#ifdef TRACE_CALL
  cout << "Visual3d_Layer::TextSize" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");
  if (theTypeOfPrimitive != Graphic3d_TOP_UNDEFINED )
    Visual3d_LayerDefinitionError::Raise
    ("One primitive is already open !");

  MyGraphicDriver->TextSize (AText, (Standard_ShortReal)AHeight, aWidth, anAscent, aDescent);
  AWidth = aWidth;
  AnAscent = anAscent;
  ADescent = aDescent;
}

void Visual3d_Layer::SetLineAttributes (const Aspect_TypeOfLine AType, const Standard_Real AWidth) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::SetLineAttributes" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");

  Standard_Integer type = AType;
  Standard_ShortReal width = Standard_ShortReal (AWidth);
#ifdef TRACE_CALL
  cout << "\tSetLineAttributes : " << type << ", " << width << endl;
#endif
  MyGraphicDriver->SetLineAttributes (type, width);
}

void Visual3d_Layer::SetTextAttributes (const Standard_CString AFont, const Aspect_TypeOfDisplayText AType, const Quantity_Color& AColor) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::SetTextAttributes" << endl;
#endif
  if (! theLayerState)
    Visual3d_LayerDefinitionError::Raise
    ("Layer is not open !");

  Standard_CString font = (char*)AFont;
  Standard_Integer type = AType;
  Standard_ShortReal r = Standard_ShortReal (AColor.Red());
  Standard_ShortReal g = Standard_ShortReal (AColor.Green());
  Standard_ShortReal b = Standard_ShortReal (AColor.Blue());
#ifdef TRACE_CALL
  cout << "\tSetTextAttributes : " << font << ", " << type << endl;
  cout << "\t                    " << r << ", " << g << ", " << b << endl;
#endif
  MyGraphicDriver->SetTextAttributes (font, type, r, g, b);
}

void Visual3d_Layer::SetOrtho (const Standard_Real Left, const Standard_Real Right, const Standard_Real Bottom, const Standard_Real Top, const Aspect_TypeOfConstraint Attach) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::SetOrtho" << endl;
#endif

  MyCLayer.ortho[0] = Standard_ShortReal (Left);
  MyCLayer.ortho[1] = Standard_ShortReal (Right);
  MyCLayer.ortho[2] = Standard_ShortReal (Bottom);
  MyCLayer.ortho[3] = Standard_ShortReal (Top);
  MyCLayer.attach   = Standard_Integer (Attach);
}

void Visual3d_Layer::SetViewport (const Standard_Integer Width, const Standard_Integer Height) {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::SetViewport" << endl;
#endif

  MyCLayer.viewport[0]  = float (Width);
  MyCLayer.viewport[1]  = float (Height);
}

Aspect_CLayer2d Visual3d_Layer::CLayer () const {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::CLayer" << endl;
#endif
#ifdef DEBUG
  cout << "Visual3d_Layer::CLayer" << endl;
  call_def_ptrLayer ptrLayer;
  ptrLayer = (call_def_ptrLayer) MyCLayer.ptrLayer;
  if (ptrLayer == NULL)
    cout << "\tptrLayer == NULL" << endl;
  else
    cout << "\tptrLayer->listIndex = " << ptrLayer->listIndex << endl;
#endif
  return MyCLayer;
}

Aspect_TypeOfLayer Visual3d_Layer::Type () const {
#ifdef TRACE_CALL
  cout << "Visual3d_Layer::Type" << endl;
#endif
  return Aspect_TypeOfLayer (MyCLayer.layerType);
}


void Visual3d_Layer::AddLayerItem( const Handle(Visual3d_LayerItem&) Item )
{
  MyListOfLayerItems.Append( Item );
}

void Visual3d_Layer::RemoveLayerItem( const Handle(Visual3d_LayerItem&) Item )
{
  if ( MyListOfLayerItems.IsEmpty() )
    return;

  Visual3d_NListOfLayerItem::Iterator it( MyListOfLayerItems );
  for( ; it.More(); it.Next() ) {
    if ( it.Value() == Item ) {
      MyListOfLayerItems.Remove( it );
      break;
    }
  }
}

void Visual3d_Layer::RemoveAllLayerItems()
{
  MyListOfLayerItems.Clear();
}

const Visual3d_NListOfLayerItem& Visual3d_Layer::GetLayerItemList() const
{
  return MyListOfLayerItems;
}

void Visual3d_Layer::RenderLayerItems() const
{
  theLayerState = Standard_True;

  const Visual3d_NListOfLayerItem& items = GetLayerItemList();
  Visual3d_NListOfLayerItem::Iterator it(items);
  for(; it.More(); it.Next() ) {
    it.Value()->RedrawLayerPrs();
  }

  theLayerState = Standard_False;
}
