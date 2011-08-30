
// File   OpenGl_GraphicDriver_Layer.cxx
// Created  Mardi 3 novembre 1998
// Author CAL

//-Copyright  MatraDatavision 1998

//-Version  

//-Design Declaration des variables specifiques aux Drivers

//-Warning  Un driver encapsule les Pex et OpenGl drivers

//-References 

//-Language C++ 2.0

//-Declarations

// for the class
#include <OpenGl_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>

#include <OpenGl_tgl_funcs.hxx>

//-Aliases
#define NO_DEBUG

//-Global data definitions

//-Methods, in order

void OpenGl_GraphicDriver::Layer (Aspect_CLayer2d& ACLayer) {
  Aspect_CLayer2d MyCLayer = ACLayer;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_layer2d");
    PrintInteger ("ALayerType", int (ACLayer.layerType));
  }
  call_togl_layer2d (&MyCLayer);
  ACLayer = MyCLayer;
#ifdef DEBUG
  cout << "OpenGl_GraphicDriver::Layer" << endl;
  call_def_ptrLayer ptrLayer;
  ptrLayer = (call_def_ptrLayer) MyCLayer.ptrLayer;
  if (ptrLayer == NULL)
    cout << "\tptrLayer == NULL" << endl;
  else
    cout << "\tptrLayer->listIndex = " << ptrLayer->listIndex << endl;
#endif
}

void OpenGl_GraphicDriver::RemoveLayer (const Aspect_CLayer2d& ACLayer) {
  Aspect_CLayer2d MyCLayer = ACLayer;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_removelayer2d");
    PrintInteger ("ALayerType", int (ACLayer.layerType));
  }
  call_togl_removelayer2d (&MyCLayer);
}

void OpenGl_GraphicDriver::BeginLayer (const Aspect_CLayer2d& ACLayer) {
  Aspect_CLayer2d MyCLayer = ACLayer;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_begin_layer2d");
    PrintInteger ("ALayerType", int (ACLayer.layerType));
  }
  call_togl_begin_layer2d (&MyCLayer);
}

void OpenGl_GraphicDriver::BeginPolygon2d () {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_begin_polygon2d");
  }
  call_togl_begin_polygon2d ();
}

void OpenGl_GraphicDriver::BeginPolyline2d () {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_begin_polyline2d");
  }
  call_togl_begin_polyline2d ();
}

void OpenGl_GraphicDriver::ClearLayer (const Aspect_CLayer2d& ACLayer) {
  Aspect_CLayer2d MyCLayer = ACLayer;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_clear_layer2d");
    PrintInteger ("ALayerType", int (ACLayer.layerType));
  }
  call_togl_clear_layer2d (&MyCLayer);
}

void OpenGl_GraphicDriver::Draw (const Standard_ShortReal X, const Standard_ShortReal Y) {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_draw2d");
    PrintShortReal ("X", X);
    PrintShortReal ("Y", Y);
  }
  call_togl_draw2d (X, Y);
}

void OpenGl_GraphicDriver::Edge (const Standard_ShortReal X, const Standard_ShortReal Y) {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_edge2d");
    PrintShortReal ("X", X);
    PrintShortReal ("Y", Y);
  }
  call_togl_edge2d (X, Y);
}

void OpenGl_GraphicDriver::EndLayer () {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_end_layer2d");
  }
  call_togl_end_layer2d ();
}

void OpenGl_GraphicDriver::EndPolygon2d () {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_end_polygon2d");
  }
  call_togl_end_polygon2d ();
}

void OpenGl_GraphicDriver::EndPolyline2d () {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_end_polyline2d");
  }
  call_togl_end_polyline2d ();
}

void OpenGl_GraphicDriver::Move (const Standard_ShortReal X, const Standard_ShortReal Y) {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_move2d");
    PrintShortReal ("X", X);
    PrintShortReal ("Y", Y);
  }
  call_togl_move2d (X, Y);
}

void OpenGl_GraphicDriver::Rectangle (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Width, const Standard_ShortReal Height) {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_rectangle2d");
    PrintShortReal ("X", X);
    PrintShortReal ("Y", Y);
    PrintShortReal ("Width", Width);
    PrintShortReal ("Height", Height);
  }
  call_togl_rectangle2d (X, Y, Width, Height);
}

void OpenGl_GraphicDriver::SetColor (const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B) {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_set_color");
    PrintShortReal ("R", R);
    PrintShortReal ("G", G);
    PrintShortReal ("B", B);
  }
  call_togl_set_color (R, G, B);
}

void OpenGl_GraphicDriver::SetTransparency (const Standard_ShortReal ATransparency) {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_set_transparency");
    PrintShortReal ("Transparency", ATransparency);
  }
  call_togl_set_transparency (ATransparency);
}

void OpenGl_GraphicDriver::UnsetTransparency () {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_unset_transparency");
  }
  call_togl_unset_transparency ();
}

void OpenGl_GraphicDriver::SetLineAttributes (const Standard_Integer Type, const Standard_ShortReal Width) {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_set_line_attributes");
    PrintInteger ("Type", Type);
    PrintShortReal ("Width", Width);
  }
  call_togl_set_line_attributes (Type, Width);
}

void OpenGl_GraphicDriver::SetTextAttributes (const Standard_CString Font, const Standard_Integer AType, const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B) {
  if (MyTraceLevel) {
    PrintFunction ("call_togl_set_text_attributes");
    PrintString ("Font", Font);
    PrintInteger ("Type", AType);
    PrintShortReal ("R", R);
    PrintShortReal ("G", G);
    PrintShortReal ("B", B);
  }
  call_togl_set_text_attributes ((Tchar*)Font, AType, R, G, B);
}   

void OpenGl_GraphicDriver::Text (const Standard_CString AText, const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Height)
{
  if (MyTraceLevel) {
    PrintFunction ("call_togl_text2d");
    PrintString ("Text", AText);
    PrintShortReal ("X", X);
    PrintShortReal ("Y", Y);
    PrintShortReal ("Height", Height);
  }
  const Standard_ShortReal h = (Height < 0)? DefaultTextHeight() : Height;
  TCollection_ExtendedString estr(AText);
  call_togl_text2d ((Techar *)estr.ToExtString(), X, Y, h);
}

void OpenGl_GraphicDriver::TextSize (const Standard_CString AText, const Standard_ShortReal AHeight, Standard_ShortReal& AWidth, Standard_ShortReal& AnAscent, Standard_ShortReal& ADescent) const
{
  TCollection_ExtendedString estr(AText);
  call_togl_textsize2d ((Techar *)estr.ToExtString(), AHeight, &AWidth, &AnAscent, &ADescent);
  if (MyTraceLevel) {
    PrintFunction ("call_togl_textsize2d");
    PrintString ("Text", AText);
    PrintShortReal ("AHeight", AHeight);
    PrintShortReal ("AWidth", AWidth);
    PrintShortReal ("AnAscent", AnAscent);
    PrintShortReal ("ADescent", ADescent);
  }
}
