

// File   OpenGl_GraphicDriver_5.cxx
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

void OpenGl_GraphicDriver::DumpGroup (const Graphic3d_CGroup& ACGroup) {

  Graphic3d_CGroup MyCGroup = ACGroup;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_structure_exploration");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_structure_exploration
    (long (MyCGroup.Struct->Id), long (MyCGroup.LabelBegin), long (MyCGroup.LabelEnd));

}

void OpenGl_GraphicDriver::DumpStructure (const Graphic3d_CStructure& ACStructure) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_structure_exploration");
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_structure_exploration (long (MyCStructure.Id), 0, 0);

}

void OpenGl_GraphicDriver::DumpView (const Graphic3d_CView& ACView) {

  Graphic3d_CView MyCView = ACView;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_view_exploration");
    PrintCView (MyCView, 1);
  }
  call_togl_view_exploration (long (MyCView.ViewId));

}

Standard_Boolean OpenGl_GraphicDriver::ElementExploration (const Graphic3d_CStructure & ACStructure, const Standard_Integer ElementNumber, Graphic3d_VertexNC& AVertex, Graphic3d_Vector& AVector) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  Quantity_Color AColor;
  long AType;

  CALL_DEF_POINT Pt;
  CALL_DEF_NORMAL Npt, Nfa;
  CALL_DEF_COLOR Cpt;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_element_exploration");
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_element_exploration (MyCStructure.Id, ElementNumber, &AType, &Pt, &Npt, &Cpt, &Nfa);

  if (! AType) return (Standard_False);
  else {
    AVertex.SetCoord
      (double (Pt.x), double (Pt.y), double (Pt.z));
    AVertex.SetNormal
      (double (Npt.dx), double (Npt.dy), double (Npt.dz));
    AColor.SetValues
      (double (Cpt.r), double (Cpt.g),
      double (Cpt.b), Quantity_TOC_RGB);
    AVertex.SetColor (AColor);
    AVector.SetCoord
      (double (Nfa.dx), double (Nfa.dy), double (Nfa.dz));
    return (Standard_True);
  }
}

Graphic3d_TypeOfPrimitive OpenGl_GraphicDriver::ElementType (const Graphic3d_CStructure & ACStructure, const Standard_Integer ElementNumber) {

  Graphic3d_CStructure MyCStructure = ACStructure;

  long AType;
  Graphic3d_TypeOfPrimitive top;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_element_type");
    PrintCStructure (MyCStructure, 1);
  }
  call_togl_element_type (MyCStructure.Id, ElementNumber, &AType);

  switch (AType) {

    case 0:
      top = Graphic3d_TOP_UNDEFINED;
      break;
    case 1:
      top = Graphic3d_TOP_POLYLINE;
      break;
    case 2:
      top = Graphic3d_TOP_POLYGON;
      break;
    case 3:
      top = Graphic3d_TOP_TRIANGLEMESH;
      break;
    case 4:
      top = Graphic3d_TOP_QUADRANGLEMESH;
      break;
    case 5:
      top = Graphic3d_TOP_TEXT;
      break;
    case 6:
      top = Graphic3d_TOP_MARKER;
      break;
    default:
      top = Graphic3d_TOP_UNDEFINED;
      break;

  }

  return (top);
}
