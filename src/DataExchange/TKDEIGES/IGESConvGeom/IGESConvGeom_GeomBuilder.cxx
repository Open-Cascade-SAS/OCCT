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

#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESConvGeom_GeomBuilder.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Standard_DomainError.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

static double epsl = 1.E-10;
static double epsa = 1.E-10;

IGESConvGeom_GeomBuilder::IGESConvGeom_GeomBuilder()
{
  Clear();
}

void IGESConvGeom_GeomBuilder::Clear()
{
  theXYZ = new NCollection_HSequence<gp_XYZ>();
  theVec = new NCollection_HSequence<gp_XYZ>();
  gp_Trsf trid;
  thepos = trid;
}

void IGESConvGeom_GeomBuilder::AddXY(const gp_XY& val)
{
  gp_XYZ aval(val.X(), val.Y(), 0.);
  theXYZ->Append(aval);
  aval.SetCoord(0., 0., 0.);
  theVec->Append(aval);
}

void IGESConvGeom_GeomBuilder::AddXYZ(const gp_XYZ& val)
{
  theXYZ->Append(val);
  theVec->Append(gp_XYZ(0., 0., 0.));
}

void IGESConvGeom_GeomBuilder::AddVec(const gp_XYZ& val)
{
  if (!theVec->IsEmpty())
    theVec->SetValue(theVec->Length(), val);
}

int IGESConvGeom_GeomBuilder::NbPoints() const
{
  return theXYZ->Length();
}

gp_XYZ IGESConvGeom_GeomBuilder::Point(const int num) const
{
  return theXYZ->Value(num);
}

occ::handle<IGESGeom_CopiousData> IGESConvGeom_GeomBuilder::MakeCopiousData(
  const int  datatype,
  const bool polyline) const
{
  int num, nb = theXYZ->Length();
  if (datatype < 1 || datatype > 3 || nb == 0 || (polyline && datatype == 3))
    throw Standard_DomainError("IGESConvGeom_GeomBuilder : MakeCopiousData");

  int nbd = datatype + 1; // 1->2  2->3   and   3->6
  if (datatype == 3)
    nbd = 6;
  occ::handle<NCollection_HArray1<double>> data = new NCollection_HArray1<double>(1, nb * nbd);
  double                                   CZ   = 0.;
  for (num = 1; num <= nb; num++)
  {
    const gp_XYZ& pnt = theXYZ->Value(num);
    data->SetValue((num - 1) * nbd + 1, pnt.X());
    data->SetValue((num - 1) * nbd + 2, pnt.Y());
    if (datatype > 1)
      data->SetValue((num - 1) * nbd + 3, pnt.Z());
    else
      CZ += pnt.Z();
    if (datatype < 3)
      continue;
    const gp_XYZ& vec = theVec->Value(num);
    data->SetValue((num - 1) * nbd + 4, vec.X());
    data->SetValue((num - 1) * nbd + 5, vec.Y());
    data->SetValue((num - 1) * nbd + 6, vec.Z());
  }
  if (datatype == 1)
    CZ /= nb;

  occ::handle<IGESGeom_CopiousData> res = new IGESGeom_CopiousData;
  res->Init(datatype, CZ, data);
  res->SetPolyline(polyline);
  return res;
}

gp_Trsf IGESConvGeom_GeomBuilder::Position() const
{
  return thepos;
}

void IGESConvGeom_GeomBuilder::SetPosition(const gp_Trsf& pos)
{
  thepos = pos;
}

void IGESConvGeom_GeomBuilder::SetPosition(const gp_Ax3& pos)
{
  gp_Ax3  orig(gp::XOY());
  gp_Trsf ps;
  ps.SetTransformation(pos, orig);
  thepos = ps;
}

void IGESConvGeom_GeomBuilder::SetPosition(const gp_Ax2& pos)
{
  gp_Ax3 a3(pos);
  SetPosition(a3);
}

void IGESConvGeom_GeomBuilder::SetPosition(const gp_Ax1& pos)
{
  const gp_Pnt& p = pos.Location();
  const gp_Dir& d = pos.Direction();
  gp_Ax3        a3(p, d);
  SetPosition(a3);
}

bool IGESConvGeom_GeomBuilder::IsIdentity() const
{
  if (thepos.Form() == gp_Identity)
    return true;
  //   otherwise, look more closely  ...
  if (!IsTranslation())
    return false;
  if (!thepos.TranslationPart().IsEqual(gp_XYZ(0., 0., 0.), epsl))
    return false;
  return true;
}

bool IGESConvGeom_GeomBuilder::IsTranslation() const
{
  if (thepos.Form() == gp_Identity || thepos.Form() == gp_Translation)
    return true;
  //   otherwise, look more closely  ...

  int i, j;
  for (i = 1; i <= 3; i++)
    for (j = 1; j <= 3; j++)
    {
      double cons = (i == j ? 1. : 0.);
      double val  = thepos.Value(i, j);
      if (val > cons + epsa || val < cons - epsa)
        return false;
    }
  return true;
}

bool IGESConvGeom_GeomBuilder::IsZOnly() const
{
  if (!IsTranslation())
    return false;
  gp_XYZ t = thepos.TranslationPart();
  t.SetZ(0.0);
  if (!t.IsEqual(gp_XYZ(0., 0., 0.), epsl))
    return false;
  return true;
}

void IGESConvGeom_GeomBuilder::EvalXYZ(const gp_XYZ& val, double& X, double& Y, double& Z) const
{
  val.Coord(X, Y, Z);
  thepos.Inverted().Transforms(X, Y, Z);
}

occ::handle<IGESGeom_TransformationMatrix> IGESConvGeom_GeomBuilder::MakeTransformation(
  const double unit) const
{
  occ::handle<NCollection_HArray2<double>> data = new NCollection_HArray2<double>(1, 3, 1, 4);
  int                                      i, j;
  for (i = 1; i <= 3; i++)
    for (j = 1; j <= 4; j++)
      data->SetValue(i, j, (j == 4 ? thepos.Value(i, j) / unit : thepos.Value(i, j)));
  occ::handle<IGESGeom_TransformationMatrix> rs = new IGESGeom_TransformationMatrix;
  rs->Init(data);
  if (thepos.IsNegative())
    rs->SetFormNumber(1);
  return rs;
}
