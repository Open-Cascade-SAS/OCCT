// Created on: 1995-04-24
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#include <ChFiDS_FilSpine.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <NCollection_List.hxx>
#include <ElCLib.hxx>
#include <gp_XY.hxx>
#include <Law_Composite.hxx>
#include <Law_Constant.hxx>
#include <Law_Function.hxx>
#include <Law_Interpol.hxx>
#include <Law_S.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ChFiDS_FilSpine, ChFiDS_Spine)

//=================================================================================================

ChFiDS_FilSpine::ChFiDS_FilSpine() = default;

ChFiDS_FilSpine::ChFiDS_FilSpine(const double Tol)
    : ChFiDS_Spine(Tol)
{
}

//=================================================================================================

void ChFiDS_FilSpine::Reset(const bool AllData)
{
  ChFiDS_Spine::Reset(AllData);
  laws.Clear();
  if (AllData)
    parandrad.Clear();
  else // Complete parandrad
  {
    double spinedeb = FirstParameter();
    double spinefin = LastParameter();

    gp_XY FirstUandR = parandrad.First();
    gp_XY LastUandR  = parandrad.Last();
    if (std::abs(spinedeb - FirstUandR.X()) > gp::Resolution())
    {
      FirstUandR.SetX(spinedeb);
      parandrad.Prepend(FirstUandR);
    }
    if (std::abs(spinefin - LastUandR.X()) > gp::Resolution())
    {
      LastUandR.SetX(spinefin);
      parandrad.Append(LastUandR);
    }

    if (IsPeriodic())
      parandrad(parandrad.Length()).SetY(parandrad(1).Y());
  }
}

//=================================================================================================

void ChFiDS_FilSpine::SetRadius(const double Radius, const TopoDS_Edge& E)
{
  splitdone = false;
  int   IE  = Index(E);
  gp_XY FirstUandR(0., Radius), LastUandR(1., Radius);
  SetRadius(FirstUandR, IE);
  SetRadius(LastUandR, IE);
}

//=================================================================================================

void ChFiDS_FilSpine::UnSetRadius(const TopoDS_Edge& E)
{
  splitdone = false;
  int IE    = Index(E);

  double Uf     = FirstParameter(IE);
  double Ul     = LastParameter(IE);
  int    ifirst = 0, ilast = 0;
  for (int i = 1; i <= parandrad.Length(); i++)
  {
    if (std::abs(parandrad(i).X() - Uf) <= gp::Resolution())
      ifirst = i;
    if (std::abs(parandrad(i).X() - Ul) <= gp::Resolution())
      ilast = i;
  }
  if (ifirst != 0 && ilast != 0)
    parandrad.Remove(ifirst, ilast);
}

//=================================================================================================

void ChFiDS_FilSpine::SetRadius(const double Radius, const TopoDS_Vertex& V)
{
  double npar = Absc(V);
  gp_XY  UandR(npar, Radius);
  SetRadius(UandR, 0);
}

//=================================================================================================

void ChFiDS_FilSpine::SetRadius(const double Radius)
{
  parandrad.Clear();
  gp_XY FirstUandR(FirstParameter(), Radius);
  gp_XY LastUandR(LastParameter(), Radius);
  SetRadius(FirstUandR, 0);
  SetRadius(LastUandR, 0);
}

//=================================================================================================

void ChFiDS_FilSpine::SetRadius(const gp_XY& UandR, const int IinC)
{
  double W;
  if (IinC == 0)
    W = UandR.X();
  else
  {
    double Uf = FirstParameter(IinC);
    double Ul = LastParameter(IinC);
    W         = Uf + UandR.X() * (Ul - Uf);
  }

  gp_XY pr(W, UandR.Y());
  int   i;
  for (i = 1; i <= parandrad.Length(); i++)
  {
    if (parandrad.Value(i).X() == W)
    {
      parandrad.ChangeValue(i).SetY(UandR.Y());
      if (!splitdone)
        return;
      else
        break;
    }
    else if (parandrad.Value(i).X() > W)
    {
      parandrad.InsertBefore(i, pr);
      if (!splitdone)
        return;
      else
        break;
    }
  }
  if (i == parandrad.Length() + 1)
    parandrad.Append(pr);

  // si le split est done il faut rejouer la law
  // correspondant au parametre W
  if (splitdone)
  {
    NCollection_List<occ::handle<ChFiDS_ElSpine>>::Iterator It(elspines);
    NCollection_List<occ::handle<Law_Function>>::Iterator   Itl(laws);
    occ::handle<ChFiDS_ElSpine>                             Els = It.Value();
    if (Els->IsPeriodic())
      Itl.ChangeValue() = ComputeLaw(Els);
    else
    {
      for (; It.More(); It.Next(), Itl.Next())
      {
        Els       = It.Value();
        double uf = Els->FirstParameter();
        double ul = Els->LastParameter();
        if (uf <= W && W <= ul)
        {
          Itl.ChangeValue() = ComputeLaw(Els);
        }
      }
    }
  }
}

//=================================================================================================

void ChFiDS_FilSpine::UnSetRadius(const TopoDS_Vertex& V)
{
  double npar = Absc(V);
  for (int i = 1; i <= parandrad.Length(); i++)
  {
    if (parandrad.Value(i).X() == npar)
    {
      parandrad.Remove(i);
      break;
    }
  }
}

//=================================================================================================

void ChFiDS_FilSpine::SetRadius(const occ::handle<Law_Function>& C, const int /*IinC*/)
{
  splitdone                                          = false;
  occ::handle<Law_Composite>                   prout = new Law_Composite();
  NCollection_List<occ::handle<Law_Function>>& lst   = prout->ChangeLaws();
  lst.Append(C);
  parandrad.Clear();
}

//=================================================================================================

bool ChFiDS_FilSpine::IsConstant() const
{
  if (parandrad.IsEmpty())
    return false;

  bool   isconst = true;
  double Radius  = parandrad(1).Y();
  for (int i = 2; i <= parandrad.Length(); i++)
    if (std::abs(Radius - parandrad(i).Y()) > Precision::Confusion())
    {
      isconst = false;
      break;
    }
  return isconst;
}

//=================================================================================================

bool ChFiDS_FilSpine::IsConstant(const int IE) const
{
  double Uf = FirstParameter(IE);
  double Ul = LastParameter(IE);

  double StartRad = 0.0, par, rad;
  int    i;
  for (i = 1; i < parandrad.Length(); i++)
  {
    par            = parandrad(i).X();
    rad            = parandrad(i).Y();
    double nextpar = parandrad(i + 1).X();
    if (std::abs(Uf - par) <= gp::Resolution()
        || (par < Uf && Uf < nextpar && nextpar - Uf > gp::Resolution()))
    {
      StartRad = rad;
      break;
    }
  }
  for (i++; i <= parandrad.Length(); i++)
  {
    par = parandrad(i).X();
    rad = parandrad(i).Y();
    if (std::abs(rad - StartRad) > Precision::Confusion())
      return false;
    if (std::abs(Ul - par) <= gp::Resolution())
      return true;
    if (par > Ul)
      return true;
  }
  return true;
}

//=================================================================================================

double ChFiDS_FilSpine::Radius(const TopoDS_Edge& E) const
{
  int IE = Index(E);
  return Radius(IE);
}

//=================================================================================================

double ChFiDS_FilSpine::Radius(const int IE) const
{
  double Uf = FirstParameter(IE);
  double Ul = LastParameter(IE);

  double StartRad = 0., par, rad;
  int    i;
  for (i = 1; i < parandrad.Length(); i++)
  {
    par            = parandrad(i).X();
    rad            = parandrad(i).Y();
    double nextpar = parandrad(i + 1).X();
    if (std::abs(Uf - par) <= gp::Resolution()
        || (par < Uf && Uf < nextpar && nextpar - Uf > gp::Resolution()))
    {
      StartRad = rad;
      break;
    }
  }
  for (i++; i <= parandrad.Length(); i++)
  {
    par = parandrad(i).X();
    rad = parandrad(i).Y();
    if (std::abs(rad - StartRad) > Precision::Confusion())
      throw Standard_DomainError("Edge is not constant");
    if (std::abs(Ul - par) <= gp::Resolution())
      return StartRad;
    if (par > Ul)
      return StartRad;
  }
  return StartRad;
}

//=================================================================================================

double ChFiDS_FilSpine::Radius() const
{
  if (!IsConstant())
    throw Standard_DomainError("Spine is not constant");
  return parandrad(1).Y();
}

//=================================================================================================

void ChFiDS_FilSpine::AppendElSpine(const occ::handle<ChFiDS_ElSpine>& Els)
{
  ChFiDS_Spine::AppendElSpine(Els);
  AppendLaw(Els);
}

//=================================================================================================

void ChFiDS_FilSpine::AppendLaw(const occ::handle<ChFiDS_ElSpine>& Els)
{
  occ::handle<Law_Composite> l = ComputeLaw(Els);
  laws.Append(l);
}

static void mklaw(NCollection_List<occ::handle<Law_Function>>& res,
                  const NCollection_Sequence<gp_XY>&           pr,
                  const double                                 curdeb,
                  const double                                 curfin,
                  const double                                 Rdeb,
                  const double                                 Rfin,
                  const bool                                   recadre,
                  const double                                 deb,
                  const double                                 fin,
                  const double                                 tol3d)
{
  NCollection_Sequence<gp_XY> npr;
  double                      rad = Rdeb, raf = Rfin;
  bool                        yaunpointsurledeb = false;
  bool                        yaunpointsurlefin = false;
  if (!pr.IsEmpty())
  {
    for (int i = 1; i <= pr.Length(); i++)
    {
      const gp_XY& cur  = pr.Value(i);
      double       wcur = cur.X();
      if (recadre)
        wcur = ElCLib::InPeriod(wcur, deb, fin);
      if (curdeb - tol3d <= wcur && wcur <= curfin + tol3d)
      {
        if (wcur - curdeb < tol3d)
        {
          yaunpointsurledeb = true;
          gp_XY ncur        = cur;
          if (Rdeb < 0.)
            rad = cur.Y();
          ncur.SetCoord(curdeb, rad);
          npr.Append(ncur);
        }
        else if (curfin - wcur < tol3d)
        {
          yaunpointsurlefin = true;
          gp_XY ncur        = cur;
          if (Rfin < 0.)
            raf = cur.Y();
          ncur.SetCoord(curfin, raf);
          npr.Append(ncur);
        }
        else
          npr.Append(gp_XY(wcur, cur.Y()));
      }
    }
  }

  if (npr.IsEmpty())
  {
    if (Rdeb < 0. && Rfin < 0.)
      throw Standard_DomainError("Impossible to create the law");
    else if (Rdeb < 0. || Rfin < 0.)
    {
      double                    r   = (Rfin < 0.) ? Rdeb : Rfin;
      occ::handle<Law_Constant> loi = new Law_Constant();
      loi->Set(r, curdeb, curfin);
      res.Append(loi);
    }
    else
    {
      occ::handle<Law_S> loi = new Law_S();
      loi->Set(curdeb, Rdeb, curfin, Rfin);
      res.Append(loi);
    }
  }
  else
  {
    if (!yaunpointsurledeb && Rdeb >= 0.)
      npr.Append(gp_XY(curdeb, Rdeb));
    if (!yaunpointsurlefin && Rfin >= 0.)
      npr.Append(gp_XY(curfin, Rfin));
    int nbp = npr.Length();
    //    for(int i = 1; i < nbp; i++){
    int i;
    for (i = 1; i < nbp; i++)
    {
      for (int j = i + 1; j <= nbp; j++)
      {
        if (npr.Value(i).X() > npr.Value(j).X())
        {
          gp_XY temp         = npr.Value(i);
          npr.ChangeValue(i) = npr.Value(j);
          npr.ChangeValue(j) = temp;
        }
      }
    }
    // Duplicates are removed.
    bool fini = (nbp <= 1);
    i         = 1;
    while (!fini)
    {
      if (fabs(npr.Value(i).X() - npr.Value(i + 1).X()) < tol3d)
      {
        npr.Remove(i);
        nbp--;
      }
      else
        i++;
      fini = (i >= nbp);
    }

    if (rad < 0.)
    {
      occ::handle<Law_Constant> loi = new Law_Constant();
      loi->Set(npr.First().Y(), curdeb, npr.First().X());
      res.Append(loi);
    }
    if (nbp > 1)
    {
      NCollection_Array1<gp_Pnt2d> tpr(1, nbp);
      for (int l = 1; l <= nbp; l++)
      {
        tpr(l).SetXY(npr.Value(l));
      }
      occ::handle<Law_Interpol> curloi = new Law_Interpol();
      curloi->Set(tpr, 0., 0., false);
      res.Append(curloi);
    }
    if (raf < 0.)
    {
      occ::handle<Law_Constant> loi = new Law_Constant();
      loi->Set(npr.Last().Y(), npr.Last().X(), curfin);
      res.Append(loi);
    }
  }
}

//=================================================================================================

occ::handle<Law_Composite> ChFiDS_FilSpine::ComputeLaw(const occ::handle<ChFiDS_ElSpine>& Els)
{
  double tol3d = Precision::Confusion();
  double deb, fin, curdeb, curfin;
  curdeb = deb = Els->FirstParameter();
  curfin = fin = Els->LastParameter();
  int ideb     = Index(deb, true);
  int ifin     = Index(fin, false);
  int len      = NbEdges();
  // if the spine is periodic, attention to the index and parameters
  double spinedeb = FirstParameter();
  double spinefin = LastParameter();

  int nbed   = ifin - ideb + 1;
  int bidfin = ifin;

  occ::handle<Law_Composite>                   loi  = new Law_Composite();
  NCollection_List<occ::handle<Law_Function>>& list = loi->ChangeLaws();
  double                                       Rdeb = 0., Rfin = 0., Rcur;
  int                                          icur = 1;
  occ::handle<Law_S>                           sl;
  occ::handle<Law_Constant>                    lastloi;
  bool                                         lawencours = false;

  if (IsPeriodic())
  {
    if (deb < 0 && ideb > ifin)
      bidfin += len;
    else if (fin > LastParameter(len) && ideb > ifin)
      bidfin += len;
    nbed = bidfin - ideb + 1;
  }
  NCollection_Array1<int> ind(1, nbed);
  int                     j = 1;
  for (int i = ideb; i <= bidfin; i++)
  {
    ind(j++) = ((i - 1) % len) + 1;
  }

  if (Els->IsPeriodic())
  {
    // A pereodic composite is created at range, which is eventually
    // offset relatively to the elspine, to avoid a single point at
    // origin.
    loi->SetPeriodic();
    // Is there a constant edge?
    //    for(int k = 1; k <= len; k++){
    int k;
    for (k = 1; k <= len; k++)
    {
      if (IsConstant(k))
      { // yes  !
        spinedeb = deb = curdeb = FirstParameter(k);
        spinefin = fin = deb + Period();
        for (int l = 1; l <= len; l++)
        {
          ind(l) = ((k + l - 2) % len) + 1;
        }
        Rdeb = Rfin = Radius(k);
        icur++;
        if (len == 1)
          curfin = LastParameter(k); // because InPeriod will make 0.!!!
        else
          curfin = ElCLib::InPeriod(LastParameter(k), spinedeb, spinefin);
        occ::handle<Law_Constant> curloi = new Law_Constant();
        curloi->Set(Rdeb, curdeb, curfin);
        list.Append(curloi);
        curdeb = curfin;
        break;
      }
    }
    if (k > len)
    { // no !
      if (parandrad.IsEmpty())
        throw Standard_DomainError("Radius not defined");
      int nbp = parandrad.Length();
      if (nbp > 1)
      {
        deb = parandrad.First().X();
        fin = deb + Period();
        if (parandrad.Last().X() - fin < -tol3d)
          nbp++;
      }
      else
        nbp++;
      NCollection_Array1<gp_Pnt2d> pr(1, nbp);
      for (int l = 1; l < nbp; l++)
      {
        pr(l).SetXY(parandrad(l));
      }
      pr(nbp).SetCoord(fin, pr(1).Y());
      occ::handle<Law_Interpol> curloi = new Law_Interpol();
      curloi->Set(pr, true);
      list.Append(curloi);
      return loi;
    }
  }
  else if (IsPeriodic())
  {
    // start radius.
    if (IsConstant(ind(1)))
    {
      Rdeb   = Radius(ind(1));
      curfin = LastParameter(ind(1));
      curfin = ElCLib::InPeriod(curfin, spinedeb + tol3d, spinefin + tol3d);
      curfin = std::min(fin, curfin);
      occ::handle<Law_Constant> curloi = new Law_Constant();
      curloi->Set(Rdeb, curdeb, curfin);
      list.Append(curloi);
      curdeb = curfin;
      icur++;
    }
    else
    {
      // There is inevitably kpart right before!
      int iprec = (ind(1) - 1);
      if (iprec == 0)
        iprec = len;
      if (IsConstant(iprec))
      {
        Rdeb = Radius(iprec);
      }
      else
        throw Standard_DomainError("AppendLaw : previous constant is missing!");
      lawencours = true;
    }
    // the raduis at end.
    if (IsConstant(ind(nbed)))
      Rfin = Radius(ind(nbed));
    else
    {
      // There is inevitably kpart right after!
      int isuiv = (ind(nbed) + 1);
      if (isuiv == len + 1)
        isuiv = 1;
      if (IsConstant(isuiv))
      {
        Rfin = Radius(isuiv);
      }
      else
        throw Standard_DomainError("AppendLaw : next constant is missing!");
    }
  }
  else
  {
    // the radius at start.
    if (IsConstant(ind(1)))
    {
      Rdeb                             = Radius(ind(1));
      curfin                           = std::min(fin, LastParameter(ind(1)));
      occ::handle<Law_Constant> curloi = new Law_Constant();
      curloi->Set(Rdeb, curdeb, curfin);
      list.Append(curloi);
      curdeb = curfin;
      icur++;
    }
    else
    {
      if (ind(1) > 1)
      {
        if (IsConstant(ind(1) - 1))
        {
          Rdeb = Radius(ind(1) - 1);
        }
        else
          throw Standard_DomainError("AppendLaw : previous constant is missing");
      }
      else if (parandrad.IsEmpty())
      {
        throw Standard_DomainError("AppendLaw : no radius on vertex");
      }
      else
        Rdeb = -1.;
      lawencours = true;
    }
    // the radius at end.
    if (IsConstant(ind(nbed)))
      Rfin = Radius(ind(nbed));
    else
    {
      if (ind(nbed) < len)
      {
        if (IsConstant(ind(nbed) + 1))
          Rfin = Radius(ind(nbed) + 1);
        else
          throw Standard_DomainError("AppendLaw : next constant is missing");
      }
      else if (parandrad.IsEmpty())
      {
        throw Standard_DomainError("AppendLaw : no radius on vertex");
      }
      else
        Rfin = -1.;
    }
  }

  // There are infos on the extremities of the elspine,
  // all edges are parsed
  for (; icur <= nbed; icur++)
  {
    if (IsConstant(ind(icur)))
    {
      Rcur = Radius(ind(icur));
      if (lawencours)
      {
        NCollection_List<occ::handle<Law_Function>> temp;
        mklaw(temp, parandrad, curdeb, curfin, Rdeb, Rcur, IsPeriodic(), spinedeb, spinefin, tol3d);
        list.Append(temp);
        lawencours = false;
        curdeb     = curfin;
      }
      curfin = LastParameter(ind(icur));
      if (IsPeriodic())
      {
        curfin = ElCLib::InPeriod(curfin, spinedeb + tol3d, spinefin + tol3d);
        if (ind(icur) == ind(nbed))
        {
          // Attention the curfin can be wrong if the last edge passes
          // above the  origin periodic spline.
          double biddeb = FirstParameter(ind(icur));
          biddeb        = ElCLib::InPeriod(biddeb, spinedeb + tol3d, spinefin + tol3d);
          if (biddeb >= curfin)
            curfin = fin;
          else
            curfin = std::min(fin, curfin);
        }
        else
          curfin = std::min(fin, curfin);
      }
      if ((curfin - curdeb) > tol3d)
      {
        Rdeb                             = Rcur;
        occ::handle<Law_Constant> curloi = new Law_Constant();
        curloi->Set(Rdeb, curdeb, curfin);
        list.Append(curloi);
        curdeb = curfin;
      }
    }
    else
    {
      curfin = LastParameter(ind(icur));
      if (IsPeriodic())
        curfin = ElCLib::InPeriod(curfin, spinedeb + tol3d, spinefin + tol3d);
      curfin     = std::min(fin, curfin);
      lawencours = true;
      if (ind(icur) == ind(nbed))
      {
        // Attention the curfin can be wrong if the last edge passes
        // above the  origin periodic spline.
        if (IsPeriodic())
        {
          double biddeb = FirstParameter(ind(icur));
          curfin        = LastParameter(ind(icur));
          biddeb        = ElCLib::InPeriod(biddeb, spinedeb + tol3d, spinefin + tol3d);
          curfin        = ElCLib::InPeriod(curfin, spinedeb + tol3d, spinefin + tol3d);
          if (biddeb >= curfin)
            curfin = fin;
          else
            curfin = std::min(fin, curfin);
        }
        // or if it is the end of spine with extension.
        else if (ind(icur) == len)
          curfin = fin;
        NCollection_List<occ::handle<Law_Function>> temp;
        mklaw(temp, parandrad, curdeb, curfin, Rdeb, Rfin, IsPeriodic(), spinedeb, spinefin, tol3d);
        list.Append(temp);
      }
    }
  }
  if (!lastloi.IsNull())
    list.Append(lastloi);
  return loi;
}

//=================================================================================================

occ::handle<Law_Composite> ChFiDS_FilSpine::Law(const occ::handle<ChFiDS_ElSpine>& Els) const
{
  NCollection_List<occ::handle<ChFiDS_ElSpine>>::Iterator Itsp(elspines);
  NCollection_List<occ::handle<Law_Function>>::Iterator   Itl(laws);
  for (; Itsp.More(); Itsp.Next(), Itl.Next())
  {
    if (Els == Itsp.Value())
    {
      return occ::down_cast<Law_Composite>(Itl.Value());
    }
  }
  return occ::handle<Law_Composite>();
}

//=================================================================================================

occ::handle<Law_Function>& ChFiDS_FilSpine::ChangeLaw(const TopoDS_Edge& E)
{
  if (!SplitDone())
  {
    throw Standard_DomainError("ChFiDS_FilSpine::ChangeLaw : the limits are not up-to-date");
  }
  int IE = Index(E);
  if (IsConstant(IE))
  {
    throw Standard_DomainError("ChFiDS_FilSpine::ChangeLaw : no law on constant edges");
  }
  occ::handle<ChFiDS_ElSpine> hsp = ElSpine(IE);
  double                      w   = 0.5 * (FirstParameter(IE) + LastParameter(IE));
  occ::handle<Law_Composite>  lc  = Law(hsp);
  return lc->ChangeElementaryLaw(w);
}

//=================================================================================================

double ChFiDS_FilSpine::MaxRadFromSeqAndLaws() const
{
  double MaxRad = 0.;

  for (int i = 1; i <= parandrad.Length(); i++)
    if (parandrad(i).Y() > MaxRad)
      MaxRad = parandrad(i).Y();

  NCollection_List<occ::handle<Law_Function>>::Iterator itl(laws);
  for (; itl.More(); itl.Next())
  {
    occ::handle<Law_Function> law = itl.Value();
    double                    fpar, lpar, par, delta, rad;
    law->Bounds(fpar, lpar);
    delta = (lpar - fpar) * 0.2;
    for (int i = 0; i <= 4; i++)
    {
      par = fpar + i * delta;
      rad = law->Value(par);
      if (rad > MaxRad)
        MaxRad = rad;
    }
    rad = law->Value(lpar);
    if (rad > MaxRad)
      MaxRad = rad;
  }

  return MaxRad;
}
