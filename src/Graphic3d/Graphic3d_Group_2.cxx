
// File   Graphic3d_Group_2.cxx (Inquire)
// Created  Fevrier 1992
// Author NW,JPB,CAL

//-Copyright  MatraDatavision 1991,1992

//-Version  

//-Design Declaration des variables specifiques aux groupes
//    de primitives

//-Warning  Un groupe est defini dans une structure
//    Il s'agit de la plus petite entite editable

//-References 

//-Language C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_Group.jxx>
#include <Graphic3d_Group.pxx>

#include <Graphic3d_StructureManager.hxx>

// Structures c necessaires a l'interfacage avec les routines c
// Routines C a declarer en extern
//-Methods, in order

Standard_Boolean Graphic3d_Group::IsDeleted () const {

  if ( (MyCGroup.IsDeleted) || (MyStructure->IsDeleted ()) )
    return (Standard_True);
  else
    return (Standard_False);

}

Standard_Boolean Graphic3d_Group::ContainsFacet () const {

  return (MyContainsFacet);

}

Standard_Boolean Graphic3d_Group::IsEmpty () const {

  if (IsDeleted ()) return (Standard_True);

  Standard_ShortReal RL = ShortRealLast ();
  Standard_ShortReal RF = ShortRealFirst ();
  Standard_Boolean Result = ((MyBounds.XMin == RL) && (MyBounds.YMin == RL) &&
    (MyBounds.ZMin == RL) && (MyBounds.XMax == RF) &&
    (MyBounds.YMax == RF) && (MyBounds.ZMax == RF));

  if (Result != MyIsEmpty)
    cout << "MyIsEmpty != IsEmpty ()\n" << flush;

  return (Result);

}

void Graphic3d_Group::SetMinMaxValues (const Standard_Real XMin, const Standard_Real YMin, const Standard_Real ZMin, const Standard_Real XMax, const Standard_Real YMax, const Standard_Real ZMax) {

  MyBounds.XMin = Standard_ShortReal (XMin);
  MyBounds.YMin = Standard_ShortReal (YMin);
  MyBounds.ZMin = Standard_ShortReal (ZMin);
  MyBounds.XMax = Standard_ShortReal (XMax);
  MyBounds.YMax = Standard_ShortReal (YMax);
  MyBounds.ZMax = Standard_ShortReal (ZMax);

}

void Graphic3d_Group::MinMaxValues (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) const {

  MinMaxCoord (XMin, YMin, ZMin, XMax, YMax, ZMax);

}

Handle(Graphic3d_Structure) Graphic3d_Group::Structure () const {

  return MyStructure;

}

void Graphic3d_Group::MinMaxCoord (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) const {

  if (IsEmpty ()) {
    // Groupe vide
    XMin  = YMin  = ZMin  = ShortRealFirst ();
    XMax  = YMax  = ZMax  = ShortRealLast ();
  }
  else {
    XMin  = Standard_Real (MyBounds.XMin);
    YMin  = Standard_Real (MyBounds.YMin);
    ZMin  = Standard_Real (MyBounds.ZMin);
    XMax  = Standard_Real (MyBounds.XMax);
    YMax  = Standard_Real (MyBounds.YMax);
    ZMax  = Standard_Real (MyBounds.ZMax);
  }

}

void Graphic3d_Group::Labels (Standard_Integer& LB, Standard_Integer& LE) const {

  LB  = Standard_Integer (MyCGroup.LabelBegin);
  LE  = Standard_Integer (MyCGroup.LabelEnd);

}

void Graphic3d_Group::Exploration () const {

  if (IsDeleted ()) return;

  MyGraphicDriver->DumpGroup (MyCGroup);

}

void Graphic3d_Group::Update () const {

  if (IsDeleted ()) return;

  if ( (MyStructure->StructureManager ())->UpdateMode () == Aspect_TOU_ASAP )
    (MyStructure->StructureManager ())->Update ();

}
