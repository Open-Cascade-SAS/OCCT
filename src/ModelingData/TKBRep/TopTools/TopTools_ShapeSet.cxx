// Created on: 1993-07-02
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

// Modified:    02 Nov 2000: BUC60769. JMB, PTV.  In order to be able to read BRep
//              is run, we need the following modifications.
//              On Unix platforms:
//              ------------------
//                In method Read(Standard_IStream &IS), during the version
//                authentication we cut last '\r' in the line (which will
//                be present if file is in DOS coding)

#include <Message_ProgressScope.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeSet.hxx>
#include <Standard_Assert.hxx>

#include <BRep_TFace.hxx>

#include <string.h>

const Standard_CString TopTools_ShapeSet::THE_ASCII_VERSIONS[TopTools_FormatVersion_UPPER + 1] = {
  "",
  "CASCADE Topology V1, (c) Matra-Datavision",
  "CASCADE Topology V2, (c) Matra-Datavision",
  "CASCADE Topology V3, (c) Open Cascade"};

//=================================================================================================

TopTools_ShapeSet::TopTools_ShapeSet()
    : myFormatNb(TopTools_FormatVersion_CURRENT)
{
}

TopTools_ShapeSet::~TopTools_ShapeSet() {}

//=================================================================================================

void TopTools_ShapeSet::SetFormatNb(const Standard_Integer theFormatNb)
{
  Standard_ASSERT_RETURN(theFormatNb >= TopTools_FormatVersion_LOWER
                           && theFormatNb <= TopTools_FormatVersion_UPPER,
                         "Error: unsupported TopTools version.", );

  myFormatNb = theFormatNb;
}

//=================================================================================================

Standard_Integer TopTools_ShapeSet::FormatNb() const
{
  return myFormatNb;
}

//=================================================================================================

void TopTools_ShapeSet::Clear()
{
  myShapes.Clear();
  myLocations.Clear();
}

//=================================================================================================

Standard_Integer TopTools_ShapeSet::Add(const TopoDS_Shape& S)
{
  if (S.IsNull())
    return 0;
  myLocations.Add(S.Location());
  TopoDS_Shape S2 = S;
  S2.Location(TopLoc_Location());
  Standard_Integer index = myShapes.FindIndex(S2);
  if (index == 0)
  {
    AddGeometry(S2);

    for (TopoDS_Iterator its(S2, Standard_False, Standard_False); its.More(); its.Next())
      Add(its.Value());
    index = myShapes.Add(S2);
  }
  return index;
}

//=================================================================================================

const TopoDS_Shape& TopTools_ShapeSet::Shape(const Standard_Integer I) const
{
  return myShapes(I);
}

//=================================================================================================

Standard_Integer TopTools_ShapeSet::Index(const TopoDS_Shape& S) const
{
  return myShapes.FindIndex(S);
}

//=================================================================================================

const TopTools_LocationSet& TopTools_ShapeSet::Locations() const
{
  return myLocations;
}

//=================================================================================================

TopTools_LocationSet& TopTools_ShapeSet::ChangeLocations()
{
  return myLocations;
}

//=================================================================================================

static void PrintShapeEnum(const TopAbs_ShapeEnum T, Standard_OStream& S, Standard_Boolean C)
{
  switch (T)
  {

    case TopAbs_VERTEX:
      if (C)
        S << "Ve";
      else
        S << "VERTEX   ";
      break;

    case TopAbs_EDGE:
      if (C)
        S << "Ed";
      else
        S << "EDGE     ";
      break;

    case TopAbs_WIRE:
      if (C)
        S << "Wi";
      else
        S << "WIRE     ";
      break;

    case TopAbs_FACE:
      if (C)
        S << "Fa";
      else
        S << "FACE     ";
      break;

    case TopAbs_SHELL:
      if (C)
        S << "Sh";
      else
        S << "SHELL    ";
      break;

    case TopAbs_SOLID:
      if (C)
        S << "So";
      else
        S << "SOLID    ";
      break;

    case TopAbs_COMPSOLID:
      if (C)
        S << "CS";
      else
        S << "COMPSOLID";
      break;

    case TopAbs_COMPOUND:
      if (C)
        S << "Co";
      else
        S << "COMPOUND ";
      break;

    case TopAbs_SHAPE:
      if (C)
        S << "Sp";
      else
        S << "SHAPE";
      break;
  }
}

//=================================================================================================

static void PrintOrientation(const TopAbs_Orientation O, Standard_OStream& S, Standard_Boolean C)
{
  switch (O)
  {

    case TopAbs_FORWARD:
      if (C)
        S << "+";
      else
        S << "FORWARD";
      break;

    case TopAbs_REVERSED:
      if (C)
        S << "-";
      else
        S << "REVERSED";
      break;

    case TopAbs_INTERNAL:
      if (C)
        S << "i";
      else
        S << "INTERNAL";
      break;

    case TopAbs_EXTERNAL:
      if (C)
        S << "e";
      else
        S << "EXTERNAL";
      break;
  }
}

//=======================================================================
// function : DumpExtent
// purpose  : Dump number of shapes.
//=======================================================================

Standard_OStream& TopTools_ShapeSet::DumpExtent(Standard_OStream& OS) const
{

  Standard_Integer nbVERTEX    = 0;
  Standard_Integer nbEDGE      = 0;
  Standard_Integer nbWIRE      = 0;
  Standard_Integer nbFACE      = 0;
  Standard_Integer nbSHELL     = 0;
  Standard_Integer nbSOLID     = 0;
  Standard_Integer nbCOMPSOLID = 0;
  Standard_Integer nbCOMPOUND  = 0;
  Standard_Integer nbSHAPE     = myShapes.Extent();

  for (Standard_Integer i = 1; i <= nbSHAPE; i++)
  {

    switch (myShapes(i).ShapeType())
    {

      case TopAbs_VERTEX:
        nbVERTEX++;
        break;

      case TopAbs_EDGE:
        nbEDGE++;
        break;

      case TopAbs_WIRE:
        nbWIRE++;
        break;

      case TopAbs_FACE:
        nbFACE++;
        break;

      case TopAbs_SHELL:
        nbSHELL++;
        break;

      case TopAbs_SOLID:
        nbSOLID++;
        break;

      case TopAbs_COMPSOLID:
        nbCOMPSOLID++;
        break;

      case TopAbs_COMPOUND:
        nbCOMPOUND++;
        break;

      case TopAbs_SHAPE:
        break;
    }
  }

  OS << " VERTEX    : " << nbVERTEX << "\n";
  OS << " EDGE      : " << nbEDGE << "\n";
  OS << " WIRE      : " << nbWIRE << "\n";
  OS << " FACE      : " << nbFACE << "\n";
  OS << " SHELL     : " << nbSHELL << "\n";
  OS << " SOLID     : " << nbSOLID << "\n";
  OS << " COMPSOLID : " << nbCOMPSOLID << "\n";
  OS << " COMPOUND  : " << nbCOMPOUND << "\n";
  OS << " SHAPE     : " << nbSHAPE << "\n";

  return OS;
}

//=======================================================================
// function : DumpExtent
// purpose  : Dump number of shapes.
//=======================================================================

void TopTools_ShapeSet::DumpExtent(TCollection_AsciiString& S) const
{

  Standard_Integer nbVERTEX    = 0;
  Standard_Integer nbEDGE      = 0;
  Standard_Integer nbWIRE      = 0;
  Standard_Integer nbFACE      = 0;
  Standard_Integer nbSHELL     = 0;
  Standard_Integer nbSOLID     = 0;
  Standard_Integer nbCOMPSOLID = 0;
  Standard_Integer nbCOMPOUND  = 0;
  Standard_Integer nbSHAPE     = myShapes.Extent();

  for (Standard_Integer i = 1; i <= nbSHAPE; i++)
  {

    switch (myShapes(i).ShapeType())
    {

      case TopAbs_VERTEX:
        nbVERTEX++;
        break;

      case TopAbs_EDGE:
        nbEDGE++;
        break;

      case TopAbs_WIRE:
        nbWIRE++;
        break;

      case TopAbs_FACE:
        nbFACE++;
        break;

      case TopAbs_SHELL:
        nbSHELL++;
        break;

      case TopAbs_SOLID:
        nbSOLID++;
        break;

      case TopAbs_COMPSOLID:
        nbCOMPSOLID++;
        break;

      case TopAbs_COMPOUND:
        nbCOMPOUND++;
        break;

      case TopAbs_SHAPE:
        break;
    }
  }

  S = S + " VERTEX    : " + TCollection_AsciiString(nbVERTEX) + "\n";
  S = S + " EDGE      : " + TCollection_AsciiString(nbEDGE) + "\n";
  S = S + " WIRE      : " + TCollection_AsciiString(nbWIRE) + "\n";
  S = S + " FACE      : " + TCollection_AsciiString(nbFACE) + "\n";
  S = S + " SHELL     : " + TCollection_AsciiString(nbSHELL) + "\n";
  S = S + " SOLID     : " + TCollection_AsciiString(nbSOLID) + "\n";
  S = S + " COMPSOLID : " + TCollection_AsciiString(nbCOMPSOLID) + "\n";
  S = S + " COMPOUND  : " + TCollection_AsciiString(nbCOMPOUND) + "\n";
  S = S + " SHAPE     : " + TCollection_AsciiString(nbSHAPE) + "\n";
}

//=================================================================================================

void TopTools_ShapeSet::Dump(Standard_OStream& OS) const
{
  //-----------------------------------------
  // dump the shapes
  //-----------------------------------------

  Standard_Integer i, nbShapes = myShapes.Extent();

  OS << "\nDump of " << nbShapes << " TShapes";
  OS << "\n\n-----------------\n\n";

  OS << "Flags : Free, Modified, Checked, Orientable, Closed, Infinite, Convex, Locked";
  OS << "\n\n";

  for (i = nbShapes; i >= 1; i--)
  {

    const TopoDS_Shape& S = myShapes(i);
    OS << "TShape # " << nbShapes - i + 1 << " : ";

    // Type and flags

    PrintShapeEnum(S.ShapeType(), OS, Standard_False);
    OS << " ";
    OS << (S.Free() ? 1 : 0);
    OS << (S.Modified() ? 1 : 0);
    OS << (S.Checked() ? 1 : 0);
    OS << (S.Orientable() ? 1 : 0);
    OS << (S.Closed() ? 1 : 0);
    OS << (S.Infinite() ? 1 : 0);
    OS << (S.Convex() ? 1 : 0);
    OS << (S.Locked() ? 1 : 0);
    OS << " " << (void*)&(*S.TShape()) << "\n";

    // sub-shapes
    OS << "    ";
    TopoDS_Iterator its(S, Standard_False, Standard_False);
    while (its.More())
    {
      const TopoDS_Shape& sub = its.Value();
      PrintOrientation(sub.Orientation(), OS, Standard_True);
      OS << nbShapes - myShapes.FindIndex(sub.Located(TopLoc_Location())) + 1;
      if (!sub.Location().IsIdentity())
        OS << "(L" << myLocations.Index(sub.Location()) << ")";
      OS << " ";
      its.Next();
    }
    OS << "\n";

    // Geometry

    DumpGeometry(S, OS);
  }

  //-----------------------------------------
  // dump the geometry
  //-----------------------------------------

  DumpGeometry(OS);

  //-----------------------------------------
  // dump the locations
  //-----------------------------------------

  myLocations.Dump(OS);

  OS << "\n";
}

//=================================================================================================

void TopTools_ShapeSet::Write(Standard_OStream& OS, const Message_ProgressRange& theProgress)
{
  // always use C locale for writing shapes
  std::locale anOldLocale = OS.imbue(std::locale::classic());

  // use 15-digit precision
  std::streamsize prec = OS.precision(15);

  // write the copyright
  OS << "\n" << THE_ASCII_VERSIONS[myFormatNb] << "\n";

  //-----------------------------------------
  // write the locations
  //-----------------------------------------

  Message_ProgressScope aPS(theProgress, "Writing", 100);

  myLocations.Write(OS, aPS.Next(10));

  if (aPS.UserBreak())
  {
    OS << "Interrupted by the user\n";
    OS.imbue(anOldLocale);
    return;
  }

  //-----------------------------------------
  // write the geometry
  //-----------------------------------------

  WriteGeometry(OS, aPS.Next(75));

  if (aPS.UserBreak())
  {
    OS << "Interrupted by the user\n";
    OS.imbue(anOldLocale);
    return;
  }

  //-----------------------------------------
  // write the shapes
  //-----------------------------------------

  Standard_Integer i, nbShapes = myShapes.Extent();

  OS << "\nTShapes " << nbShapes << "\n";

  // subshapes are written first
  Message_ProgressScope aPS1(aPS.Next(15), "Shapes", nbShapes);
  for (i = 1; i <= nbShapes && aPS1.More(); i++, aPS1.Next())
  {
    const TopoDS_Shape& S = myShapes(i);

    // Type
    PrintShapeEnum(S.ShapeType(), OS, Standard_True);
    OS << "\n";

    // Geometry
    WriteGeometry(S, OS);

    // Flags
    OS << "\n";
    OS << (S.Free() ? 1 : 0);
    OS << (S.Modified() ? 1 : 0);
    OS << (S.Checked() ? 1 : 0);
    OS << (S.Orientable() ? 1 : 0);
    OS << (S.Closed() ? 1 : 0);
    OS << (S.Infinite() ? 1 : 0);
    OS << (S.Convex() ? 1 : 0);
    OS << "\n";

    // sub-shapes

    Standard_Integer l = 0;
    TopoDS_Iterator  its(S, Standard_False, Standard_False);
    while (its.More())
    {
      Write(its.Value(), OS);
      l++;
      if (l == 10)
      {
        OS << "\n";
        l = 0;
      }
      its.Next();
    }
    Write(TopoDS_Shape(), OS); // Null shape to end the list
    OS << "\n";
  }

  OS << "\n";
  OS.precision(prec);
  OS.imbue(anOldLocale);

  if (aPS.UserBreak())
    OS << "Interrupted by the user\n";
}

//=================================================================================================

static TopAbs_ShapeEnum ReadShapeEnum(Standard_IStream& IS)
{
  char buffer[255];
  IS >> buffer;

  switch (buffer[0])
  {

    case 'V':
      return TopAbs_VERTEX;

    case 'E':
      return TopAbs_EDGE;

    case 'W':
      return TopAbs_WIRE;

    case 'F':
      return TopAbs_FACE;

    case 'S':
      if (buffer[1] == 'h')
        return TopAbs_SHELL;
      else
        return TopAbs_SOLID;

    case 'C':
      if (buffer[1] == 'S')
        return TopAbs_COMPSOLID;
      else
        return TopAbs_COMPOUND;
  }
  return TopAbs_COMPOUND;
}

//=================================================================================================

void TopTools_ShapeSet::Read(Standard_IStream& IS, const Message_ProgressRange& theProgress)
{
  // always use C locale for reading shapes
  std::locale anOldLocale = IS.imbue(std::locale::classic());

  Clear();

  // Check the version
  char             vers[101];
  Standard_Boolean anIsSetFormat = Standard_False;
  do
  {
    IS.getline(vers, 100, '\n');
    // BUC60769 PTV 18.10.2000: remove possible '\r' at the end of the line
    // Standard_Integer lv = strlen(vers);
    // char *pm;
    // if(pm = strchr(vers,'\r'))
    //  *pm ='\0';

    Standard_Size lv = strlen(vers);
    if (lv > 0)
    {
      for (lv--; lv > 0 && (vers[lv] == '\r' || vers[lv] == '\n'); lv--)
        vers[lv] = '\0';
    }
    for (Standard_Integer i = TopTools_FormatVersion_LOWER; i <= TopTools_FormatVersion_UPPER; ++i)
    {
      if (!strcmp(vers, THE_ASCII_VERSIONS[i]))
      {
        SetFormatNb(i);
        anIsSetFormat = Standard_True;
        break;
      }
    }
    if (anIsSetFormat)
    {
      break;
    }

  } while (!IS.fail());
  if (IS.fail())
  {
    std::cout << "File was not written with this version of the topology" << std::endl;
    IS.imbue(anOldLocale);
    return;
  }

  //-----------------------------------------
  // read the locations
  //-----------------------------------------

  Message_ProgressScope aPS(theProgress, "Reading", 100);

  myLocations.Read(IS, aPS.Next(10));

  if (aPS.UserBreak())
  {
    std::cout << "Interrupted by the user" << std::endl;
    // on remet le LC_NUMERIC a la precedente valeur
    IS.imbue(anOldLocale);
    return;
  }

  //-----------------------------------------
  // read the geometry
  //-----------------------------------------
  ReadGeometry(IS, aPS.Next(75));

  if (aPS.UserBreak())
  {
    std::cout << "Interrupted by the user" << std::endl;
    IS.imbue(anOldLocale);
    return;
  }

  //-----------------------------------------
  // read the shapes
  //-----------------------------------------

  char buffer[255];
  IS >> buffer;
  if (strcmp(buffer, "TShapes"))
  {
    std::cout << "Not a TShape table" << std::endl;
    // on remet le LC_NUMERIC a la precedente valeur
    IS.imbue(anOldLocale);
    return;
  }

  Standard_Integer i, nbShapes;
  IS >> nbShapes;

  // OCC19559
  Message_ProgressScope aPS1(aPS.Next(15), "Shapes", nbShapes);
  for (i = 1; i <= nbShapes && aPS1.More(); i++, aPS1.Next())
  {
    TopoDS_Shape S;

    // Read type and create empty shape.

    TopAbs_ShapeEnum T = ReadShapeEnum(IS);
    ReadGeometry(T, IS, S);

    // Set the flags
    IS >> buffer;

    // sub-shapes
    TopoDS_Shape SS;
    do
    {
      Read(SS, IS, nbShapes);
      if (!SS.IsNull())
        AddShapes(S, SS);
    } while (!SS.IsNull());

    S.Free(buffer[0] == '1');
    S.Modified(buffer[1] == '1');

    const bool isChecked = myFormatNb >= TopTools_FormatVersion_VERSION_2 && buffer[2] == '1';
    S.Checked(isChecked);

    S.Orientable(buffer[3] == '1');
    S.Closed(buffer[4] == '1');
    S.Infinite(buffer[5] == '1');
    S.Convex(buffer[6] == '1');

    // check

    if (myFormatNb == TopTools_FormatVersion_VERSION_1)
      Check(T, S);

    myShapes.Add(S);
  }

  // on remet le LC_NUMERIC a la precedente valeur
  IS.imbue(anOldLocale);

  if (aPS.UserBreak())
    std::cout << "Interrupted by the user" << std::endl;
}

//=================================================================================================

void TopTools_ShapeSet::Dump(const TopoDS_Shape& S, Standard_OStream& OS) const
{
  if (S.IsNull())
    OS << "Null shape\n";
  OS << "Shape : " << myShapes.FindIndex(S.Located(TopLoc_Location()));
  OS << ", ";
  PrintOrientation(S.Orientation(), OS, Standard_False);
  if (!S.Location().IsIdentity())
    OS << ", location : " << myLocations.Index(S.Location());
  OS << "\n";
}

//=================================================================================================

void TopTools_ShapeSet::Write(const TopoDS_Shape& S, Standard_OStream& OS) const
{
  if (S.IsNull())
    OS << "*";
  else
  {
    PrintOrientation(S.Orientation(), OS, Standard_True);
    OS << myShapes.Extent() - myShapes.FindIndex(S.Located(TopLoc_Location())) + 1;
    OS << " " << myLocations.Index(S.Location()) << " ";
  }
}

//=================================================================================================

void TopTools_ShapeSet::Read(TopoDS_Shape& S, Standard_IStream& IS) const
{
  Read(S, IS, myShapes.Extent());
}

//=================================================================================================

void TopTools_ShapeSet::Read(TopoDS_Shape&          S,
                             Standard_IStream&      IS,
                             const Standard_Integer nbshapes) const
{
  if (nbshapes < 1)
  {
    S = TopoDS_Shape();
    return;
  }
  char buffer[255];
  IS >> buffer;
  if (buffer[0] == '*')
    S = TopoDS_Shape();
  else
  {
    S = myShapes(nbshapes - atoi(buffer + 1) + 1);
    switch (buffer[0])
    {

      case '+':
        S.Orientation(TopAbs_FORWARD);
        break;

      case '-':
        S.Orientation(TopAbs_REVERSED);
        break;

      case 'i':
        S.Orientation(TopAbs_INTERNAL);
        break;

      case 'e':
        S.Orientation(TopAbs_EXTERNAL);
        break;
    }

    Standard_Integer l;
    IS >> l;
    S.Location(myLocations.Location(l), Standard_False);
  }
}

//=================================================================================================

void TopTools_ShapeSet::AddGeometry(const TopoDS_Shape&) {}

//=================================================================================================

void TopTools_ShapeSet::DumpGeometry(Standard_OStream&) const {}

//=================================================================================================

void TopTools_ShapeSet::WriteGeometry(Standard_OStream&, const Message_ProgressRange&) {}

//=================================================================================================

void TopTools_ShapeSet::ReadGeometry(Standard_IStream&, const Message_ProgressRange&) {}

//=================================================================================================

void TopTools_ShapeSet::DumpGeometry(const TopoDS_Shape&, Standard_OStream&) const {}

//=================================================================================================

void TopTools_ShapeSet::WriteGeometry(const TopoDS_Shape&, Standard_OStream&) const {}

//=================================================================================================

void TopTools_ShapeSet::ReadGeometry(const TopAbs_ShapeEnum, Standard_IStream&, TopoDS_Shape&) {}

//=================================================================================================

void TopTools_ShapeSet::AddShapes(TopoDS_Shape&, const TopoDS_Shape&) {}

//=================================================================================================

void TopTools_ShapeSet::Check(const TopAbs_ShapeEnum, TopoDS_Shape&) {}

//=================================================================================================

Standard_Integer TopTools_ShapeSet::NbShapes() const
{
  return myShapes.Extent();
}
