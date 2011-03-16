// File   OpenGl_GraphicDriver_8.cxx
// Created  Mardi 28 janvier 1997
// Author CAL

//-Copyright  MatraDatavision 1997

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

//-Global data definitions

//-Methods, in order


Standard_Boolean OpenGl_GraphicDriver::BeginAddMode (const Graphic3d_CView& ACView) 
{

  Graphic3d_CView MyCView = ACView;
  Standard_Boolean Result;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_begin_ajout_mode");
    PrintCView (MyCView, 1);
  }
  Result = call_togl_begin_ajout_mode (&MyCView);
  if (MyTraceLevel) {
    PrintIResult ("call_togl_begin_ajout_mode", Result);
  }
  return Result;

}

void OpenGl_GraphicDriver::EndAddMode (void) 
{

  if (MyTraceLevel) {
    PrintFunction ("call_togl_end_ajout_mode");
  }
  call_togl_end_ajout_mode ();

}

Standard_Boolean OpenGl_GraphicDriver::BeginImmediatMode (const Graphic3d_CView& ACView, const Aspect_CLayer2d& ACUnderLayer, const Aspect_CLayer2d& ACOverLayer, const Standard_Boolean DoubleBuffer, const Standard_Boolean RetainMode) 
{

  Graphic3d_CView MyCView = ACView;
  Aspect_CLayer2d MyCUnderLayer = ACUnderLayer;
  Aspect_CLayer2d MyCOverLayer = ACOverLayer;
  Standard_Boolean Result;


  if (MyTraceLevel) {
    PrintFunction ("call_togl_begin_immediat_mode");
    PrintCView (MyCView, 1);
    PrintBoolean ("DoubleBuffer", DoubleBuffer);
    PrintBoolean ("RetainMode", RetainMode);
  }
  Result = call_togl_begin_immediat_mode
    (&MyCView, &MyCUnderLayer, &MyCOverLayer,
    (DoubleBuffer ? 1 : 0), (RetainMode ? 1 : 0));
  if (MyTraceLevel) {
    PrintIResult ("call_togl_begin_immediat_mode", Result);
  }
  return Result;

}

void OpenGl_GraphicDriver::BeginPolyline () 
{

  if (MyTraceLevel) {
    PrintFunction ("call_togl_begin_polyline");
  }
  call_togl_begin_polyline ();

}

void OpenGl_GraphicDriver::ClearImmediatMode (const Graphic3d_CView& ACView,
                                              const Standard_Boolean aFlush) 
{

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_clear_immediat_mode");
    PrintCView (MyCView, 1);
  }
  call_togl_clear_immediat_mode (&MyCView, aFlush);

}

void OpenGl_GraphicDriver::Draw (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z) 
{


  if (MyTraceLevel) {
    PrintFunction ("call_togl_draw");
    PrintShortReal ("X", X);
    PrintShortReal ("Y", Y);
    PrintShortReal ("Z", Z);
  }
  call_togl_draw (X, Y, Z);

}

void OpenGl_GraphicDriver::DrawStructure (const Graphic3d_CStructure& ACStructure) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_draw_structure");
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_draw_structure (ACStructure.Id);

}

void OpenGl_GraphicDriver::EndImmediatMode (const Standard_Integer Synchronize) 
{

  if (MyTraceLevel) {
    PrintFunction ("call_togl_end_immediat_mode");
    PrintInteger ("Synchronize", Synchronize);
  }
  call_togl_end_immediat_mode (int (Synchronize));

}

void OpenGl_GraphicDriver::EndPolyline () 
{

  if (MyTraceLevel) {
    PrintFunction ("call_togl_end_polyline");
  }
  call_togl_end_polyline ();

}

void OpenGl_GraphicDriver::Move (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Z) 
{

  if (MyTraceLevel) {
    PrintFunction ("call_togl_move");
    PrintShortReal ("X", X);
    PrintShortReal ("Y", Y);
    PrintShortReal ("Z", Z);
  }
  call_togl_move (X, Y, Z);

}

void OpenGl_GraphicDriver::SetLineColor (const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B) 
{

  if (MyTraceLevel) {
    PrintFunction ("call_togl_set_linecolor");
    PrintShortReal ("R", R);
    PrintShortReal ("G", G);
    PrintShortReal ("B", B);
  }
  call_togl_set_linecolor (R, G, B);

}

void OpenGl_GraphicDriver::SetLineType (const Standard_Integer Type) 
{

  if (MyTraceLevel) {
    PrintFunction ("call_togl_set_linetype");
    PrintInteger ("Type", Type);
  }
  call_togl_set_linetype (long (Type));

}

void OpenGl_GraphicDriver::SetLineWidth (const Standard_ShortReal Width)
{

  if (MyTraceLevel) {
    PrintFunction ("call_togl_set_linewidth");
    PrintShortReal ("Width", Width);
  }
  call_togl_set_linewidth (float (Width));

}

void OpenGl_GraphicDriver::SetMinMax (const Standard_ShortReal X1, const Standard_ShortReal Y1, const Standard_ShortReal Z1, const Standard_ShortReal X2, const Standard_ShortReal Y2, const Standard_ShortReal Z2)
{

  if (MyTraceLevel) {
    PrintFunction ("call_togl_set_minmax");
    PrintShortReal ("X1", X1);
    PrintShortReal ("Y1", Y1);
    PrintShortReal ("Z1", Z1);
    PrintShortReal ("X2", X2);
    PrintShortReal ("Y2", Y2);
    PrintShortReal ("Z2", Z2);
  }
  call_togl_set_minmax (X1, Y1, Z1, X2, Y2, Z2);

}

void OpenGl_GraphicDriver::Transform (const TColStd_Array2OfReal& AMatrix, const Graphic3d_TypeOfComposition AType)
{

  float theMatrix[4][4];
  Standard_Integer i, j;
  Standard_Integer lr, lc;

  lr = AMatrix.LowerRow ();
  lc = AMatrix.LowerCol ();

  for (i=0; i<=3; i++)
    for (j=0; j<=3; j++)
      theMatrix[i][j] = float (AMatrix (i+lr, j+lc));

  if (MyTraceLevel) {
    PrintFunction ("call_togl_transform");
    PrintMatrix ("AMatrix", AMatrix);
  }
  call_togl_transform (theMatrix, (AType == Graphic3d_TOC_REPLACE) ? 1 : 0);

}
