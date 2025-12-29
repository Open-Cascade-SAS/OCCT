// Created on: 1993-07-21
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <DBRep.hxx>

#include <BRep_TEdge.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BinTools.hxx>
#include <DBRep_DrawableShape.hxx>
#include <Draw_Appli.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Message_ProgressRange.hxx>
#include <Draw_Segment3D.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Vector.hxx>
#include <OSD_FileSystem.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

#include <cstdio>
// memory management
#ifdef _WIN32
extern Draw_Viewer dout;
#endif

#define Characters(IArg) (strspn(Arg[IArg], "0123456789.+-eE") != strlen(Arg[IArg]))
#define Float(IArg) (strspn(Arg[IArg], "0123456789+-") != strlen(Arg[IArg]))

//==========================================
// useful methods
//==========================================

Standard_EXPORT void DBRep_WriteColorOrientation()
{
  std::cout << "\nrouge  FORWARD";
  std::cout << "\nbleu   REVERSED";
  std::cout << "\nrose   EXTERNAL";
  std::cout << "\norange INTERNAL" << std::endl;
}

Standard_EXPORT Draw_Color DBRep_ColorOrientation(const TopAbs_Orientation Or)
{
  Draw_Color col;
  switch (Or)
  {

    case TopAbs_FORWARD:
      col = Draw_rouge;
      break;

    case TopAbs_REVERSED:
      col = Draw_bleu;
      break;

    case TopAbs_EXTERNAL:
      col = Draw_rose;
      break;

    case TopAbs_INTERNAL:
      col = Draw_orange;
      break;
  }
  return col;
}

//=================================================================================================

DBRep_Params& DBRep::Parameters()
{
  static DBRep_Params aParams;
  return aParams;
}

//=======================================================================
// isos
//=======================================================================

static int isos(Draw_Interpretor& di, int NbArg, const char** Arg)
{
  DBRep_Params& aParams = DBRep::Parameters();
  NbArg--;
  if (NbArg <= 0)
  {
    di << "Current number of isos : " << aParams.NbIsos << "\n";
    return 0;
  }

  int  aNbIsos = 0;
  bool Change  = false;
  if (!Characters(NbArg) && Float(NbArg))
    return 1;
  if (!Characters(NbArg))
  {
    aNbIsos = Draw::Atoi(Arg[NbArg]);
    NbArg--;
    Change = true;
  }

  if (NbArg <= 0)
  {
    aParams.NbIsos = aNbIsos;
    di << "New current number of isos : " << aParams.NbIsos << "\n";
  }
  else
  {
    for (int IArg = 1; IArg <= NbArg; IArg++)
    {
      occ::handle<Draw_Drawable3D> Shape1 = Draw::Get(Arg[IArg]);
      if (!Shape1.IsNull())
      {
        occ::handle<DBRep_DrawableShape> Shape2 = occ::down_cast<DBRep_DrawableShape>(Shape1);
        if (!Shape2.IsNull())
        {
          if (Change)
          {
            Shape2->ChangeNbIsos(aNbIsos);
          }
          else
          {
            di << "Number of isos for " << Arg[IArg] << " : " << Shape2->NbIsos() << "\n";
          }
        }
      }
    }
    if (Change)
      dout.RepaintAll();
  }

  return 0;
}

//=======================================================================
// hlr
//=======================================================================

static int hlr(Draw_Interpretor& di, int n, const char** a)
{
  DBRep_Params& aParams = DBRep::Parameters();
  if (n <= 1)
  {
    if (aParams.WithHLR)
    {
      di << " HLR";
      di << (aParams.WithRgN ? " RgNLines" : (aParams.WithRg1 ? " Rg1Lines" : " no RegLines"));
      di << (aParams.WithHid ? " HiddenLines" : " no HiddenLines");
      di << "\n";
      if (aParams.WithHLR)
      {
        di << "Angle of discretization : ";
        di << aParams.HLRAngle * 180 / M_PI << " degrees\n";
      }
    }
    else
      di << " wireframe";
    di << "\n";
    return 0;
  }

  if (n == 2)
  {
    if (!strcasecmp(a[1], "nohlr"))
    {
      aParams.WithHLR = false;
    }
    else if (!strcasecmp(a[1], "hlr"))
    {
      aParams.WithHLR = true;
    }
    else if (!strcasecmp(a[1], "nohid"))
    {
      aParams.WithHid = false;
    }
    else if (!strcasecmp(a[1], "hid"))
    {
      aParams.WithHLR = true;
      aParams.WithHid = true;
    }
    else if (!strcasecmp(a[1], "norg1"))
    {
      aParams.WithRg1 = false;
      aParams.WithRgN = false;
    }
    else if (!strcasecmp(a[1], "rg1"))
    {
      aParams.WithHLR = true;
      aParams.WithRg1 = true;
      aParams.WithRgN = false;
    }
    else if (!strcasecmp(a[1], "norgn"))
    {
      aParams.WithRgN = false;
    }
    else if (!strcasecmp(a[1], "rgn"))
    {
      aParams.WithHLR = true;
      aParams.WithRg1 = true;
      aParams.WithRgN = true;
    }
    else if (!strcasecmp(a[1], "ang"))
    {
      di << "Angle of discretization : ";
      di << aParams.HLRAngle * 180 / M_PI << " degrees\n";
    }
    else
      return 1;
  }

  int nFirst = 2;

  if (n >= 3 && !strcasecmp(a[1], "ang"))
  {
    nFirst = 3;
    if (n == 3)
    {
      double ang       = Draw::Atof(a[2]);
      aParams.HLRAngle = ang * M_PI / 180;
      if (aParams.HLRAngle < aParams.HAngMin)
      {
        aParams.HLRAngle = aParams.HAngMin;
      }
      if (aParams.HLRAngle > aParams.HAngMax)
      {
        aParams.HLRAngle = aParams.HAngMax;
      }
    }
    di << "Angle of discretization : ";
    di << aParams.HLRAngle * 180 / M_PI << " degrees\n";
  }

  for (int i = nFirst; i < n; i++)
  {
    occ::handle<Draw_Drawable3D>     D = Draw::Get(a[i]);
    occ::handle<DBRep_DrawableShape> S = occ::down_cast<DBRep_DrawableShape>(D);
    if (S.IsNull())
    {
      continue;
    }

    bool   localHLR = false, localRg1 = false, localRgN = false, localHid = false;
    double localAng = 0.0;
    S->GetDisplayHLR(localHLR, localRg1, localRgN, localHid, localAng);
    if (!strcasecmp(a[1], "nohlr"))
    {
      localHLR = false;
    }
    else if (!strcasecmp(a[1], "hlr"))
    {
      localHLR = true;
    }
    else if (!strcasecmp(a[1], "nohid"))
    {
      localHid = false;
    }
    else if (!strcasecmp(a[1], "hid"))
    {
      localHLR = true;
      localHid = true;
    }
    else if (!strcasecmp(a[1], "norg1"))
    {
      localRg1 = false;
      localRgN = false;
    }
    else if (!strcasecmp(a[1], "rg1"))
    {
      localHLR = true;
      localRg1 = true;
      localRgN = false;
    }
    else if (!strcasecmp(a[1], "norgn"))
    {
      localRgN = false;
    }
    else if (!strcasecmp(a[1], "rgn"))
    {
      localHLR = true;
      localRg1 = true;
      localRgN = true;
    }
    else if (!strcasecmp(a[1], "ang"))
    {
      double ang = Draw::Atof(a[2]);
      localAng   = ang * M_PI / 180;
    }
    else
    {
      di << "Syntax error";
      return 1;
    }

    S->DisplayHLR(localHLR, localRg1, localRgN, localHid, localAng);
  }
  dout.RepaintAll();
  return 0;
}

//=======================================================================
// dispor, dispcon
//=======================================================================

static int dispor(Draw_Interpretor&, int n, const char** a)
{
  bool d = !strcasecmp(a[0], "vori");
  if (d)
  {
    DBRep_WriteColorOrientation();
  }

  for (int i = 1; i < n; i++)
  {
    occ::handle<Draw_Drawable3D> d1 = Draw::Get(a[i]);
    if (occ::handle<DBRep_DrawableShape> d2 = occ::down_cast<DBRep_DrawableShape>(d1))
    {
      d2->DisplayOrientation(d);
      Draw::Repaint();
    }
  }
  return 0;
}

//=======================================================================
// discretisation
//=======================================================================

static int discretisation(Draw_Interpretor& di, int n, const char** a)
{
  DBRep_Params& aParams = DBRep::Parameters();
  if (n <= 1)
  {
    di << "Current number of points : " << aParams.Discretization << "\n";
  }
  else
  {
    aParams.Discretization = Draw::Atoi(a[1]);
  }
  return 0;
}

//=======================================================================
// triangles
//=======================================================================

static int triangles(Draw_Interpretor&, int n, const char** a)
{
  DBRep_Params& aParams = DBRep::Parameters();
  if (n < 1)
  {
    return 1;
  }
  if (n == 1)
  {
    aParams.DispTriangles = !aParams.DispTriangles;
  }
  else
  {
    for (int i = 1; i <= n - 1; i++)
    {
      occ::handle<Draw_Drawable3D> d1 = Draw::Get(a[i]);
      if (occ::handle<DBRep_DrawableShape> d2 = occ::down_cast<DBRep_DrawableShape>(d1))
      {
        d2->DisplayTriangulation(!(d2->DisplayTriangulation()));
      }
    }
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
// tclean
//=======================================================================

static int tclean(Draw_Interpretor& di, int n, const char** a)
{
  bool toRemoveGeometry = false;
  bool isForceClean     = false;

  if (n <= 1)
  {
    di.PrintHelp(a[0]);
    return 1;
  }
  TopoDS_Compound aCompound;
  BRep_Builder().MakeCompound(aCompound);
  for (int anArgIter = 1; anArgIter < n; ++anArgIter)
  {
    if (strcmp(a[anArgIter], "-geom") == 0)
    {
      toRemoveGeometry = true;
    }
    else if (strcmp(a[anArgIter], "-force") == 0)
    {
      isForceClean = true;
    }
    else
    {
      TopoDS_Shape aShape = DBRep::Get(a[anArgIter]);
      if (aShape.IsNull())
      {
        di << "Syntax error : NULL shape '" << a[anArgIter] << "'";
        return 1;
      }
      BRep_Builder().Add(aCompound, aShape);
    }
  }
  if (aCompound.NbChildren() == 0)
  {
    di << "Syntax error : wrong number of arguments";
    di.PrintHelp(a[0]);
    return 1;
  }
  if (isForceClean && toRemoveGeometry)
  {
    di << "Syntax error: wrong usage of arguments: do not use 'force' and 'geom' flags together";
    return 1;
  }

  if (toRemoveGeometry)
    BRepTools::CleanGeometry(aCompound);
  else
    BRepTools::Clean(aCompound, isForceClean);
  return 0;
}

//=======================================================================
// polygons
//=======================================================================

static int polygons(Draw_Interpretor&, int n, const char** a)
{
  DBRep_Params& aParams = DBRep::Parameters();
  if (n < 1)
  {
    return 1;
  }
  if (n == 1)
  {
    aParams.DisplayPolygons = !aParams.DisplayPolygons;
  }
  else
  {
    for (int i = 1; i <= n - 1; i++)
    {
      occ::handle<Draw_Drawable3D> d1 = Draw::Get(a[i]);
      if (occ::handle<DBRep_DrawableShape> d2 = occ::down_cast<DBRep_DrawableShape>(d1))
      {
        d2->DisplayPolygons(!(d2->DisplayPolygons()));
      }
    }
  }
  Draw::Repaint();
  return 0;
}

//=======================================================================
// compound
//=======================================================================

static int compound(Draw_Interpretor&, int n, const char** a)
{
  if (n <= 1)
    return 1;
  BRep_Builder    B;
  TopoDS_Compound C;
  B.MakeCompound(C);
  for (int i = 1; i < n - 1; i++)
  {
    TopoDS_Shape S2 = DBRep::Get(a[i]);
    if (!S2.IsNull())
      B.Add(C, S2);
  }
  DBRep::Set(a[n - 1], C);
  return 0;
}

//=======================================================================
// emptycopy
//=======================================================================

static int emptycopy(Draw_Interpretor&, int n, const char** a)
{
  if (n <= 1)
    return 1;
  TopoDS_Shape S = DBRep::Get(a[(n == 2) ? 1 : 2]);
  if (S.IsNull())
    return 1;
  S.EmptyCopy();
  DBRep::Set(a[1], S);
  return 0;
}

//=======================================================================
// add
//=======================================================================

static int add(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;
  BRep_Builder B;
  TopoDS_Shape S1 = DBRep::Get(a[1]);
  if (S1.IsNull())
    return 1;
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  if (S2.IsNull())
    return 1;
  B.Add(S2, S1);
  DBRep::Set(a[2], S2);
  return 0;
}

//=======================================================================
// explode
//=======================================================================

static int explode(Draw_Interpretor& di, int n, const char** a)
{
  if (n <= 1)
    return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull())
    return 0;
  char newname[1024];
  strcpy(newname, a[1]);
  char* p = newname;
  while (*p != '\0')
    p++;
  *p = '_';
  p++;
  int i = 0;
  if (n == 2)
  {
    TopoDS_Iterator itr(S);
    while (itr.More())
    {
      i++;
      Sprintf(p, "%d", i);
      DBRep::Set(newname, itr.Value());
      di.AppendElement(newname);
      itr.Next();
    }
  }
  else
  {
    // explode a type
    TopAbs_ShapeEnum typ;
    switch (a[2][0])
    {

      case 'C':
      case 'c':
        if ((a[2][1] == 'd') || (a[2][1] == 'D'))
          typ = TopAbs_COMPOUND;
        else
          typ = TopAbs_COMPSOLID;
        break;

      case 'S':
      case 's':
        if ((a[2][1] == 'O') || (a[2][1] == 'o'))
          typ = TopAbs_SOLID;
        else if ((a[2][1] == 'H') || (a[2][1] == 'h'))
          typ = TopAbs_SHELL;
        else
          return 1;
        break;

      case 'F':
      case 'f':
        typ = TopAbs_FACE;
        break;

      case 'W':
      case 'w':
        typ = TopAbs_WIRE;
        break;

      case 'E':
      case 'e':
        typ = TopAbs_EDGE;
        break;

      case 'V':
      case 'v':
        typ = TopAbs_VERTEX;
        break;

      default:
        return 1;
    }

    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> M;
    M.Add(S);
    TopExp_Explorer ex(S, typ);
    for (; ex.More(); ex.Next())
    {
      const TopoDS_Shape& Sx    = ex.Current();
      bool                added = M.Add(Sx);
      if (added)
      {
        i++;
        Sprintf(p, "%d", i);
        DBRep::Set(newname, Sx);
        di.AppendElement(newname);
      }
    }
  }
  return 0;
}

//=======================================================================
// nexplode : stable numbered explode (from Serguey Nizhny)
//=======================================================================

static int nexplode(Draw_Interpretor& di, int n, const char** a)
{
  if (n <= 2)
    return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull())
    return 0;
  char newname[1024];
  strcpy(newname, a[1]);
  char* p = newname;
  while (*p != '\0')
    p++;
  *p = '_';
  p++;
  TopAbs_ShapeEnum typ;
  // explode a type
  switch (a[2][0])
  {
    case 'F':
    case 'f':
      typ = TopAbs_FACE;
      break;

    case 'E':
    case 'e':
      typ = TopAbs_EDGE;
      break;

    case 'V':
    case 'v':
      typ = TopAbs_VERTEX;
      break;

    default:
      return 1;
  }
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> IMOStmp;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>        MShape;
  IMOStmp.Add(S);
  TopExp::MapShapes(S, typ, IMOStmp);
  TopExp_Explorer Exp(S, typ);
  int             MaxShapes, Index = 0;
  MaxShapes = IMOStmp.Extent() - 1;
  NCollection_Array1<TopoDS_Shape> aShapes(1, MaxShapes);

  // explode
  while (Exp.More())
  {
    if (MShape.Add(Exp.Current()))
    {
      Index++;
      aShapes.SetValue(Index, Exp.Current());
    }
    Exp.Next();
  }
  //
  NCollection_Array1<int>    OrderInd(1, MaxShapes);
  gp_Pnt                     GPoint;
  GProp_GProps               GPr;
  int                        aTemp;
  NCollection_Array1<double> MidXYZ(1, MaxShapes); // X,Y,Z;
  bool                       NoSort = true;
  //
  // Computing of CentreOfMass for edge and face
  // and for vertex use its point
  for (Index = 1; Index <= MaxShapes; Index++)
  {
    OrderInd.SetValue(Index, Index);
    const TopoDS_Shape& aS = aShapes(Index);
    if (aS.ShapeType() != TopAbs_VERTEX)
    {
      BRepGProp::LinearProperties(aS, GPr);
      GPoint = GPr.CentreOfMass();
    }
    else
    {
      GPoint = BRep_Tool::Pnt(TopoDS::Vertex(aS));
    }
    MidXYZ.SetValue(Index, GPoint.X() * 999 + GPoint.Y() * 99 + GPoint.Z() * 0.9);
  }
  // Sorting
  while (NoSort)
  {
    NoSort = false;
    for (Index = 1; Index < MaxShapes; Index++)
    {
      if (MidXYZ(OrderInd(Index)) > MidXYZ(OrderInd(Index + 1)))
      {
        aTemp               = OrderInd(Index);
        OrderInd(Index)     = OrderInd(Index + 1);
        OrderInd(Index + 1) = aTemp;
        NoSort              = true;
      }
    }
  }
  // Check equality of MidXYZ
  for (Index = 1; Index < MaxShapes; Index++)
  {
    if (MidXYZ(OrderInd(Index + 1)) == MidXYZ(OrderInd(Index)))
      di << "Warning! For this shape the results may be incorrect.\n";
  }

  for (Index = 1; Index <= MaxShapes; Index++)
  {
    Sprintf(p, "%d", Index);
    DBRep::Set(newname, aShapes(OrderInd(Index)));
    di.AppendElement(newname);
  }

  return 0;
}

//=======================================================================
// exwire
//=======================================================================

static int exwire(Draw_Interpretor&, int n, const char** a)
{
  if (n <= 1)
    return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull())
    return 0;
  if (S.ShapeType() != TopAbs_WIRE)
    return 0;
  char newname[1024];
  strcpy(newname, a[1]);
  char* p = newname;
  while (*p != '\0')
    p++;
  *p = '_';
  p++;
  int                    i = 0;
  BRepTools_WireExplorer ex(TopoDS::Wire(S));
  while (ex.More())
  {
    i++;
    Sprintf(p, "%d", i);
    DBRep::Set(newname, ex.Current());
    ex.Next();
  }
  return 0;
}

//=======================================================================
// invert
//=======================================================================

static int invert(Draw_Interpretor&, int n, const char** a)
{
  if (n <= 1)
    return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull())
    return 0;

  BRep_Builder B;
  TopoDS_Shape NS = S.EmptyCopied();
  NS.Closed(S.Closed());

  TopoDS_Iterator itr(S);
  while (itr.More())
  {
    B.Add(NS, itr.Value().Reversed());
    itr.Next();
  }
  DBRep::Set(a[1], NS);

  return 0;
}

//=======================================================================
// orientation, reverse, complement
//=======================================================================

static int orientation(Draw_Interpretor&, int n, const char** a)
{
  if (n <= 1)
    return 1;
  int                cas  = 0;
  TopAbs_Orientation ori  = TopAbs_FORWARD;
  int                last = n;
  if (!strcasecmp(a[0], "orientation"))
  {
    if (n <= 2)
      return 1;
    last--;
    switch (*a[n - 1])
    {

      case 'F':
        ori = TopAbs_FORWARD;
        break;

      case 'R':
        ori = TopAbs_REVERSED;
        break;

      case 'I':
        ori = TopAbs_INTERNAL;
        break;

      case 'E':
        ori = TopAbs_EXTERNAL;
        break;
    }
  }

  else if (!strcasecmp(a[0], "treverse"))
  {
    cas = -1;
  }

  else if (!strcasecmp(a[0], "complement"))
  {
    cas = -2;
  }

  for (int i = 1; i < last; i++)
  {
    TopoDS_Shape S = DBRep::Get(a[i]);
    if (!S.IsNull())
    {
      if (cas == -2)
        S.Complement();
      else if (cas == -1)
        S.Reverse();
      else
        S.Orientation(ori);
      DBRep::Set(a[i], S);
    }
  }
  return 0;
}

#include <TCollection_AsciiString.hxx>

//=======================================================================
// numshapes same as nbshapes but the output is cout
//=======================================================================

static int numshapes(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
    return 1;

  int             i;
  TopExp_Explorer ex;
  for (i = 1; i < n; i++)
  {
    TopoDS_Shape S = DBRep::Get(a[i]);
    if (!S.IsNull())
    {
      BRepTools_ShapeSet BS;
      BS.Add(S);
      di << "Number of shapes in " << a[i] << "\n";
      TCollection_AsciiString Astr;
      BS.DumpExtent(Astr);
      di << Astr.ToCString();
      di << "\n";
    }
  }

  return 0;
}

//=======================================================================
// function : DumpExtent
// purpose  : Dumps the number of sub-shapes in <aStr>.
//=======================================================================
static void DumpExtent(const TopoDS_Shape& aS, TCollection_AsciiString& aStr)
{
  const int        aNbTypes             = 8;
  const char*      pNames[aNbTypes + 1] = {" SHAPE     : ",
                                           " COMPOUND  : ",
                                           " COMPSOLID : ",
                                           " SOLID     : ",
                                           " SHELL     : ",
                                           " FACE      : ",
                                           " WIRE      : ",
                                           " EDGE      : ",
                                           " VERTEX    : "};
  int              i, aNb, aNbSh;
  TopAbs_ShapeEnum aType;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aM;
  //
  aNbSh = 0;
  //
  for (i = aNbTypes - 1; i >= 0; --i)
  {
    aM.Clear();
    aType = (TopAbs_ShapeEnum)i;
    TopExp::MapShapes(aS, aType, aM);
    aNb  = aM.Extent();
    aStr = aStr + pNames[i + 1] + TCollection_AsciiString(aNb) + "\n";
    aNbSh += aNb;
  }
  aStr = aStr + pNames[0] + TCollection_AsciiString(aNbSh) + "\n";
}

//=======================================================================
// nbshapes
//=======================================================================

static int nbshapes(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
    return 1;

  int             i;
  bool            aTotal;
  TopExp_Explorer ex;
  //
  aTotal = !strcmp(a[n - 1], "-t") ? true : false;
  //
  for (i = 1; i < n; i++)
  {
    TopoDS_Shape S = DBRep::Get(a[i]);
    if (!S.IsNull())
    {
      di << "Number of shapes in " << a[i] << "\n";
      TCollection_AsciiString Astr;
      if (aTotal)
      {
        DumpExtent(S, Astr);
      }
      else
      {
        BRepTools_ShapeSet BS;
        BS.Add(S);
        BS.DumpExtent(Astr);
      }
      di << Astr.ToCString();
    }
  }

  return 0;
}

//=======================================================================
//
//=======================================================================

static int countshapes(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
    return 1;

  int             i;
  TopExp_Explorer ex;
  for (i = 1; i < n; i++)
  {
    TopoDS_Shape Sh     = DBRep::Get(a[i]);
    int          nbElem = 0;
    if (!Sh.IsNull())
    {
      di << "Number of shapes in " << a[i] << "\n";
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> M;

      for (ex.Init(Sh, TopAbs_VERTEX); ex.More(); ex.Next())
      {
        const TopoDS_Shape& S     = ex.Current();
        bool                added = M.Add(S);
        if (added)
        {
          nbElem++;
        }
      }
      di << " VERTEX     : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init(Sh, TopAbs_EDGE); ex.More(); ex.Next())
      {
        const TopoDS_Shape& S     = ex.Current();
        bool                added = M.Add(S);
        if (added)
        {
          nbElem++;
        }
      }
      di << " EDGE       : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init(Sh, TopAbs_WIRE); ex.More(); ex.Next())
      {
        const TopoDS_Shape& S     = ex.Current();
        bool                added = M.Add(S);
        if (added)
        {
          nbElem++;
        }
      }
      di << " WIRE       : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init(Sh, TopAbs_FACE); ex.More(); ex.Next())
      {
        const TopoDS_Shape& S     = ex.Current();
        bool                added = M.Add(S);
        if (added)
        {
          nbElem++;
        }
      }
      di << " FACE       : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init(Sh, TopAbs_SHELL); ex.More(); ex.Next())
      {
        const TopoDS_Shape& S     = ex.Current();
        bool                added = M.Add(S);
        if (added)
        {
          nbElem++;
        }
      }
      di << " SHELL      : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init(Sh, TopAbs_SOLID); ex.More(); ex.Next())
      {
        const TopoDS_Shape& S     = ex.Current();
        bool                added = M.Add(S);
        if (added)
        {
          nbElem++;
        }
      }
      di << " SOLID      : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init(Sh, TopAbs_COMPSOLID); ex.More(); ex.Next())
      {
        const TopoDS_Shape& S     = ex.Current();
        bool                added = M.Add(S);
        if (added)
        {
          nbElem++;
        }
      }
      di << " COMPSOLID  : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init(Sh, TopAbs_COMPOUND); ex.More(); ex.Next())
      {
        const TopoDS_Shape& S     = ex.Current();
        bool                added = M.Add(S);
        if (added)
        {
          nbElem++;
        }
      }
      di << " COMPOUND   : " << nbElem << "\n";
      nbElem = 0;

      di << " SHAPE      : " << M.Extent() << "\n";
      di << "\n";
    }
  }

  return 0;
}

//=======================================================================
//
//=======================================================================
void setProp(TopoDS_Shape Sh, const char** a, int n)
{
  int i;
  for (i = 2; i < n; i++)
  {
    if (strstr(a[i], "free"))
    {
      if (a[i][0] == '-')
      {
        Sh.Free(false);
      }
      else
      {
        Sh.Free(true);
      }
    }
    if (strstr(a[i], "modified"))
    {
      if (a[i][0] == '-')
      {
        Sh.Modified(false);
      }
      else
      {
        Sh.Modified(true);
      }
    }
    if (strstr(a[i], "checked"))
    {
      if (a[i][0] == '-')
      {
        Sh.Checked(false);
      }
      else
      {
        Sh.Checked(true);
      }
    }
    if (strstr(a[i], "orientable"))
    {
      if (a[i][0] == '-')
      {
        Sh.Orientable(false);
      }
      else
      {
        Sh.Orientable(true);
      }
    }
    if (strstr(a[i], "closed"))
    {
      if (a[i][0] == '-')
      {
        Sh.Closed(false);
      }
      else
      {
        Sh.Closed(true);
      }
    }
    if (strstr(a[i], "infinite"))
    {
      if (a[i][0] == '-')
      {
        Sh.Infinite(false);
      }
      else
      {
        Sh.Infinite(true);
      }
    }
    if (strstr(a[i], "convex"))
    {
      if (a[i][0] == '-')
      {
        Sh.Convex(false);
      }
      else
      {
        Sh.Convex(true);
      }
    }
    if (strstr(a[i], "locked"))
    {
      if (a[i][0] == '-')
      {
        Sh.Locked(false);
      }
      else
      {
        Sh.Locked(true);
      }
    }
  }
}

//=======================================================================
//
//=======================================================================
static int setFlags(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;

  TopExp_Explorer ex;
  TopoDS_Shape    Sh = DBRep::Get(a[1]);

  if (Sh.IsNull())
    return 1;

  setProp(Sh, a, n);
  for (ex.Init(Sh, TopAbs_VERTEX); ex.More(); ex.Next())
  {
    const TopoDS_Shape& S = ex.Current();
    setProp(S, a, n);
  }

  for (ex.Init(Sh, TopAbs_EDGE); ex.More(); ex.Next())
  {
    const TopoDS_Shape& S = ex.Current();
    setProp(S, a, n);
  }

  for (ex.Init(Sh, TopAbs_FACE); ex.More(); ex.Next())
  {
    const TopoDS_Shape& S = ex.Current();
    setProp(S, a, n);
  }

  return 0;
}

//=======================================================================
// memory management
//=======================================================================
static int purgemmgt(Draw_Interpretor&, int, const char**)
{
  Standard::Purge();
  return 0;
}

//=======================================================================

//=======================================================================
// check
//=======================================================================

static int check(Draw_Interpretor&, int n, const char** a)
{
  if (n < 2)
    return 1;

  int             i;
  TopExp_Explorer ex;
  for (i = 1; i < n; i++)
  {
    TopoDS_Shape S = DBRep::Get(a[i]);
    TopoDS_Shape C;
    if (S.IsNull())
      continue;
    for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next())
    {
      C = ex.Current();
      C.Checked(false);
      BRepTools::Update(C);
    }
  }

  return 0;
}

//=======================================================================
// normals
//=======================================================================
static int normals(Draw_Interpretor& theDI, int theArgNum, const char** theArgs)
{
  if (theArgNum < 2)
  {
    std::cout << "Syntax error: wrong number of arguments!\n";
    theDI.PrintHelp(theArgs[0]);
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(theArgs[1]);
  if (aShape.IsNull())
  {
    std::cout << "Error: shape with name " << theArgs[1] << " is not found\n";
    return 1;
  }

  bool   toUseMesh = false;
  double aLength   = 10.0;
  int    aNbAlongU = 1, aNbAlongV = 1;
  bool   bPrint = false;
  for (int anArgIter = 2; anArgIter < theArgNum; ++anArgIter)
  {
    TCollection_AsciiString aParam(theArgs[anArgIter]);
    aParam.LowerCase();
    if (anArgIter == 2 && aParam.IsRealValue())
    {
      aLength = aParam.RealValue();
      if (std::abs(aLength) <= gp::Resolution())
      {
        std::cout << "Syntax error: length should not be zero\n";
        return 1;
      }
    }
    else if (aParam == "-usemesh" || aParam == "-mesh")
    {
      toUseMesh = true;
    }
    else if (aParam == "-length" || aParam == "-len")
    {
      ++anArgIter;
      aLength = anArgIter < theArgNum ? Draw::Atof(theArgs[anArgIter]) : 0.0;
      if (std::abs(aLength) <= gp::Resolution())
      {
        std::cout << "Syntax error: length should not be zero\n";
        return 1;
      }
    }
    else if (aParam == "-nbalongu" || aParam == "-nbu")
    {
      ++anArgIter;
      aNbAlongU = anArgIter < theArgNum ? Draw::Atoi(theArgs[anArgIter]) : 0;
      if (aNbAlongU < 1)
      {
        std::cout << "Syntax error: NbAlongU should be >=1\n";
        return 1;
      }
    }
    else if (aParam == "-nbalongv" || aParam == "-nbv")
    {
      ++anArgIter;
      aNbAlongV = anArgIter < theArgNum ? Draw::Atoi(theArgs[anArgIter]) : 0;
      if (aNbAlongV < 1)
      {
        std::cout << "Syntax error: NbAlongV should be >=1\n";
        return 1;
      }
    }
    else if (aParam == "-nbalong" || aParam == "-nbuv")
    {
      ++anArgIter;
      aNbAlongU = anArgIter < theArgNum ? Draw::Atoi(theArgs[anArgIter]) : 0;
      aNbAlongV = aNbAlongU;
      if (aNbAlongU < 1)
      {
        std::cout << "Syntax error: NbAlong should be >=1\n";
        return 1;
      }
    }
    else if (aParam == "-print")
    {
      bPrint = true;
    }
    else
    {
      std::cout << "Syntax error: unknown argument '" << aParam << "'\n";
      return 1;
    }
  }

  DBRep_WriteColorOrientation();

  NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>> aNormals;
  if (toUseMesh)
  {
    DBRep_DrawableShape::addMeshNormals(aNormals, aShape, aLength);
  }
  else
  {
    DBRep_DrawableShape::addSurfaceNormals(aNormals, aShape, aLength, aNbAlongU, aNbAlongV);
  }

  for (NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>>::Iterator
         aFaceIt(aNormals);
       aFaceIt.More();
       aFaceIt.Next())
  {
    bool               bReverse = false;
    TopAbs_Orientation aFaceOri = aFaceIt.Key().Orientation();
    const Draw_Color   aColor   = DBRep_ColorOrientation(aFaceOri);
    if (aFaceOri == TopAbs_REVERSED)
      bReverse = true;

    for (NCollection_Vector<std::pair<gp_Pnt, gp_Pnt>>::Iterator aNormalsIt(aFaceIt.Value());
         aNormalsIt.More();
         aNormalsIt.Next())
    {
      const std::pair<gp_Pnt, gp_Pnt>& aVec = aNormalsIt.Value();
      occ::handle<Draw_Segment3D>      aSeg = new Draw_Segment3D(aVec.first, aVec.second, aColor);
      dout << aSeg;
      if (bPrint)
      {
        // Make the normal vector from the points
        gp_Vec aV(aVec.first, aVec.second);
        if (bReverse)
          aV.Reverse();

        // Print values of the vector avoiding printing "-0" values
        theDI << "(" << (aV.X() == 0 ? 0 : aV.X()) << ", " << (aV.Y() == 0 ? 0 : aV.Y()) << ", "
              << (aV.Z() == 0 ? 0 : aV.Z()) << ")\n";
      }
    }
  }

  return 0;
}

//=================================================================================================

void DBRep::Set(const char* theName, const TopoDS_Shape& theShape)
{
  DBRep_Params&                    aParams    = DBRep::Parameters();
  occ::handle<DBRep_DrawableShape> aDrawShape = new DBRep_DrawableShape(theShape,
                                                                        Draw_vert,
                                                                        Draw_jaune,
                                                                        Draw_rouge,
                                                                        Draw_bleu,
                                                                        aParams.Size,
                                                                        aParams.NbIsos,
                                                                        aParams.Discretization);
  aDrawShape->DisplayTriangulation(aParams.DispTriangles);
  aDrawShape->DisplayPolygons(aParams.DisplayPolygons);
  aDrawShape->DisplayHLR(aParams.WithHLR,
                         aParams.WithRg1,
                         aParams.WithRgN,
                         aParams.WithHid,
                         aParams.HLRAngle);
  Draw::Set(theName, aDrawShape);
}

//=================================================================================================

TopoDS_Shape DBRep::getShape(const char*& theName, TopAbs_ShapeEnum theType, bool theToComplain)
{
  const bool                       toPick = theName[0] == '.';
  occ::handle<DBRep_DrawableShape> aDrawable =
    occ::down_cast<DBRep_DrawableShape>(Draw::Get(theName));
  if (aDrawable.IsNull())
  {
    return TopoDS_Shape();
  }

  TopoDS_Shape aShape = aDrawable->Shape();
  if (theType != TopAbs_SHAPE && theType != aShape.ShapeType() && toPick)
  {
    // try to find prom pick
    double u, v;
    DBRep_DrawableShape::LastPick(aShape, u, v);
  }
  if (theType != TopAbs_SHAPE && theType != aShape.ShapeType())
  {
    if (theToComplain)
    {
      std::cout << theName << " is not a ";
      TopAbs::Print(theType, std::cout);
      std::cout << " but a ";
      TopAbs::Print(aShape.ShapeType(), std::cout);
      std::cout << std::endl;
    }
    return TopoDS_Shape();
  }
  return aShape;
}

static int XProgress(Draw_Interpretor& di, int argc, const char** argv)
{
  for (int i = 1; i < argc; i++)
  {
    bool turn = true;
    if (argv[i][0] == '-')
      turn = false;
    else if (argv[i][0] != '+')
      continue;

    TCollection_AsciiString anArgCase(argv[i]);
    anArgCase.LowerCase();
    if (argv[i][1] == 't')
      Draw_ProgressIndicator::DefaultTclMode() = turn;
    else if (argv[i][1] == 'c')
      Draw_ProgressIndicator::DefaultConsoleMode() = turn;
    else if (argv[i][1] == 'g')
      Draw_ProgressIndicator::DefaultGraphMode() = turn;
    else if (!strcmp(argv[i], "-stop") && i + 1 < argc)
    {
      void* aPtr = nullptr;
      if (sscanf(argv[++i], "%p", &aPtr) == 1)
        Draw_ProgressIndicator::StopIndicator() = aPtr;
      return 0;
    }
  }
  di << "Progress Indicator defaults: tcl mode is ";
  if (Draw_ProgressIndicator::DefaultTclMode())
  {
    di << "ON";
  }
  else
  {
    di << "OFF";
  }
  di << ", console mode is ";
  if (Draw_ProgressIndicator::DefaultConsoleMode())
  {
    di << "ON";
  }
  else
  {
    di << "OFF";
  }
  di << ", graphical mode is ";
  if (Draw_ProgressIndicator::DefaultGraphMode())
  {
    di << "ON";
  }
  else
  {
    di << "OFF";
  }
  di << "\n";
  return 0;
}

//=======================================================================
// writebrep
//=======================================================================
static int writebrep(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
{
  int                     aVersion = -1;
  TCollection_AsciiString aShapeName, aFileName;
  TopoDS_Shape            aShape;
  bool                    isBinaryFormat(false);
  bool                    isWithTriangles(true);
  bool                    isWithNormals(false);
  if (!strcasecmp(theArgVec[0], "binsave"))
  {
    isBinaryFormat = true;
  }

  for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString aParam(theArgVec[anArgIter]);
    aParam.LowerCase();
    if (aParam == "-binary")
    {
      isBinaryFormat = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aParam == "-version" && anArgIter + 1 < theNbArgs)
    {
      aVersion = Draw::Atoi(theArgVec[++anArgIter]);
      if (aVersion <= 0)
      {
        theDI << "Syntax error: unknown version";
        return 1;
      }
    }
    else if (aParam == "-notriangles" || aParam == "-triangles")
    {
      isWithTriangles = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aParam == "-nonormals" || aParam == "-normals")
    {
      isWithNormals = Draw::ParseOnOffIterator(theNbArgs, theArgVec, anArgIter);
      if (aParam == "-nonormals")
      {
        isWithNormals = !isWithNormals;
      }
    }
    else if (aShapeName.IsEmpty())
    {
      aShapeName = theArgVec[anArgIter];
      aShape     = DBRep::Get(aShapeName);
      if (aShape.IsNull())
      {
        theDI << "Syntax error: " << aShapeName << " is not a shape";
        return 1;
      }
    }
    else if (aFileName.IsEmpty())
    {
      aFileName = theArgVec[anArgIter];
    }
    else
    {
      theDI << "Syntax error: unknown argument '" << aParam << "'";
      return 1;
    }
  }
  if (aFileName.IsEmpty())
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(theDI);
  if (isBinaryFormat)
  {
    if (aVersion > BinTools_FormatVersion_UPPER)
    {
      theDI << "Syntax error: unknown format version";
      return 1;
    }
    if (isWithNormals && aVersion > 0 && aVersion < BinTools_FormatVersion_VERSION_4)
    {
      theDI << "Error: vertex normals require binary format version 4 or later";
      return 1;
    }

    BinTools_FormatVersion aBinToolsVersion =
      aVersion > 0 ? static_cast<BinTools_FormatVersion>(aVersion) : BinTools_FormatVersion_CURRENT;
    if (!BinTools::Write(aShape,
                         aFileName.ToCString(),
                         isWithTriangles,
                         isWithNormals,
                         aBinToolsVersion,
                         aProgress->Start()))
    {
      theDI << "Cannot write to the file " << aFileName;
      return 1;
    }
  }
  else
  {
    if (aVersion > TopTools_FormatVersion_UPPER)
    {
      theDI << "Syntax error: unknown format version";
      return 1;
    }
    if (isWithNormals && aVersion > 0 && aVersion < TopTools_FormatVersion_VERSION_3)
    {
      theDI << "Error: vertex normals require ascii format version 3 or later";
      return 1;
    }

    TopTools_FormatVersion aTopToolsVersion =
      aVersion > 0 ? static_cast<TopTools_FormatVersion>(aVersion) : TopTools_FormatVersion_CURRENT;
    if (!BRepTools::Write(aShape,
                          aFileName.ToCString(),
                          isWithTriangles,
                          isWithNormals,
                          aTopToolsVersion,
                          aProgress->Start()))
    {
      theDI << "Cannot write to the file " << aFileName;
      return 1;
    }
  }
  theDI << aShapeName;
  return 0;
}

//=======================================================================
// readbrep
//=======================================================================
static int readbrep(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 3)
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  const char* aFileName      = theArgVec[1];
  const char* aShapeName     = theArgVec[2];
  bool        isBinaryFormat = true;
  {
    // probe file header to recognize format
    const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
    std::shared_ptr<std::istream>      aFile =
      aFileSystem->OpenIStream(aFileName, std::ios::in | std::ios::binary);
    if (aFile.get() == nullptr)
    {
      theDI << "Error: cannot read the file '" << aFileName << "'";
      return 1;
    }

    char aStringBuf[255] = {};
    aFile->read(aStringBuf, 255);
    if (aFile->fail())
    {
      theDI << "Error: cannot read the file '" << aFileName << "'";
      return 1;
    }
    isBinaryFormat = !(::strncmp(aStringBuf, "DBRep_DrawableShape", 19) == 0);
  }

  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(theDI);
  TopoDS_Shape                        aShape;
  if (isBinaryFormat)
  {
    if (!BinTools::Read(aShape, aFileName, aProgress->Start()))
    {
      theDI << "Error: cannot read from the file '" << aFileName << "'";
      return 1;
    }
  }
  else
  {
    if (!BRepTools::Read(aShape, aFileName, BRep_Builder(), aProgress->Start()))
    {
      theDI << "Error: cannot read from the file '" << aFileName << "'";
      return 1;
    }
  }

  DBRep::Set(aShapeName, aShape);
  theDI << aShapeName;
  return 0;
}

//=======================================================================
// removeinternals
//=======================================================================
static int removeInternals(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(a[1]);
  if (aShape.IsNull())
  {
    di << a[1] << "is a null shape\n";
    return 1;
  }

  bool isForce = false;
  if (n > 2)
  {
    isForce = (Draw::Atoi(a[2]) != 0);
  }

  BRepTools::RemoveInternals(aShape, isForce);

  DBRep::Set(a[1], aShape);

  return 0;
}

//=================================================================================================

static bool done = false;

void DBRep::BasicCommands(Draw_Interpretor& theCommands)
{
  if (done)
    return;
  done = true;
  Draw::Commands(theCommands);

  // Register save/restore tools
  DBRep_DrawableShape::RegisterFactory();

  const char* g = "Basic shape commands";

  theCommands.Add("isos", "isos [name1 ...] [nbisos]", __FILE__, isos, g);
  theCommands.Add("hlr", "[no]hlr, rg1, rgn, hid, ang", __FILE__, hlr, g);
  theCommands.Add("vori",
                  "vori [name1 ...], edges are colored by orientation (see vconn)",
                  __FILE__,
                  dispor,
                  g);
  theCommands.Add("triangles",
                  "triangles [name1]..., display triangles of shapes if exists",
                  __FILE__,
                  triangles,
                  g);
  theCommands.Add(
    "tclean",
    "tclean [-force] [-geom] [name1]..., depending on using or not key -geom, \n"
    "\t\t -geom  : erase geometry\n"
    "\t\t          if [-geom] is omitted - erase triangulations and \n"
    "\t\t          polygons on triangulations from shapes \n"
    "\t\t -force : force delete all representations not relevant to the given shape \n",
    __FILE__,
    tclean,
    g);
  theCommands.Add("polygons",
                  "polygons [name1]..., display polygons of shapes if exists",
                  __FILE__,
                  polygons,
                  g);
  theCommands.Add("vconn",
                  "vconn [name1 ...] , edges are colored by number of faces (see vori)",
                  __FILE__,
                  dispor,
                  g);
  theCommands.Add("discretisation", "discretisation [nbpoints]", __FILE__, discretisation, g);
  theCommands.Add("compound", "compound [name1 name2 ..] compound", __FILE__, compound, g);
  theCommands.Add("add",
                  "add what where"
                  "\n  adds shape \"what\" to shape \"where\" ",
                  __FILE__,
                  add,
                  g);
  theCommands.Add("explode", "explode name [Cd/C/So/Sh/F/W/E/V]", __FILE__, explode, g);
  theCommands.Add("nexplode",
                  "stable numbered explode for vertex, edge and face: nexplode name [V/E/F]",
                  __FILE__,
                  nexplode,
                  g);
  theCommands.Add("exwire", "exwire wirename", __FILE__, exwire, g);
  theCommands.Add("emptycopy", "emptycopy [copyshape] originalshape", __FILE__, emptycopy, g);
  theCommands.Add("check", "check shape1 shape2 ...", __FILE__, check, g);

  theCommands.Add("orientation", "orientation name1 name2.. F/R/E/I", __FILE__, orientation, g);
  theCommands.Add("treverse", "treverse name1 name2 ...", __FILE__, orientation, g);
  theCommands.Add("complement", "complement name1 name2 ...", __FILE__, orientation, g);
  theCommands.Add("invert", "invert name, reverse subshapes", __FILE__, invert, g);
  theCommands.Add("normals",
                  "normals shape [Length {10}] [-NbAlongU {1}] [-NbAlongV {1}] [-UseMesh] "
                  "[-print], display normals",
                  __FILE__,
                  normals,
                  g);
  theCommands.Add("nbshapes",
                  "\n nbshapes s - shows the number of sub-shapes in <s>;\n nbshapes s -t - shows "
                  "the number of sub-shapes in <s> counting the same sub-shapes with different "
                  "location as different sub-shapes.",
                  __FILE__,
                  nbshapes,
                  g);
  theCommands.Add("numshapes", "numshapes s; size of shape", __FILE__, numshapes, g);
  theCommands.Add("countshapes", "countshapes s; count of shape", __FILE__, countshapes, g);
  theCommands.Add("setflags",
                  "setflags shape_name flag1[flag2...]\n sets flags for shape(free, modified, "
                  "checked, orientable, closed, infinite, convex, locked), for example <setflags a "
                  "free> or <setflags a -free> if necessary unflag ",
                  __FILE__,
                  setFlags,
                  g);

  theCommands.Add("purgemmgt",
                  "returns the free memory from the system to the memory manager",
                  __FILE__,
                  purgemmgt,
                  g);

  // Add command for DRAW-specific ProgressIndicator
  theCommands.Add("XProgress",
                  "XProgress [+|-t] [+|-c] [+|-g]"
                  "\n\t\t The options are:"
                  "\n\t\t   +|-t :  switch on/off output to tcl of Progress Indicator"
                  "\n\t\t   +|-c :  switch on/off output to cout of Progress Indicator"
                  "\n\t\t   +|-g :  switch on/off graphical mode of Progress Indicator",
                  __FILE__,
                  XProgress,
                  "DE: General");
  theCommands.Add(
    "writebrep",
    "writebrep shape filename [-binary {0|1}]=0 [-version Version]=4"
    "\n\t\t:                          [-triangles {0|1}]=1 [-normals {0|1}]=0"
    "\n\t\t: Save the shape in the ASCII (default) or binary format file."
    "\n\t\t:  -binary  write into the binary format (ASCII when unspecified)"
    "\n\t\t:  -version a number of format version to save;"
    "\n\t\t:           ASCII  versions: 1, 2 and 3    (3 for ASCII  when unspecified);"
    "\n\t\t:           Binary versions: 1, 2, 3 and 4 (4 for Binary when unspecified)."
    "\n\t\t:  -triangles write triangulation data (TRUE when unspecified)."
    "\n\t\t:           Ignored (always written) if face defines only triangulation (no surface)."
    "\n\t\t:  -normals include vertex normals while writing triangulation data (FALSE when "
    "unspecified).",
    __FILE__,
    writebrep,
    g);
  theCommands.Add("readbrep",
                  "readbrep filename shape"
                  "\n\t\t: Restore the shape from the binary or ASCII format file.",
                  __FILE__,
                  readbrep,
                  g);
  theCommands.Add("binsave", "binsave shape filename", __FILE__, writebrep, g);
  theCommands.Add("binrestore", "alias to readbrep command", __FILE__, readbrep, g);

  theCommands.Add(
    "removeinternals",
    "removeinternals shape [force flag {0/1}]"
    "\n\t\t             Removes sub-shapes with internal orientation from the shape.\n"
    "\n\t\t             Force flag disables the check on topological connectivity and"
    "removes all internal sub-shapes\n",
    __FILE__,
    removeInternals,
    g);
}
