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

#include <Interface_STAT.ixx>
#include <TCollection_AsciiString.hxx>

static Interface_STAT statvoid("");
static Interface_STAT statact ("");
static Standard_CString voidname = "";

    Interface_STAT::Interface_STAT (const Standard_CString title)
{
  thetitle = new TCollection_HAsciiString(title);
  thetotal = 1.;
}

    Interface_STAT::Interface_STAT (const Interface_STAT& other)
{  other.Internals (thetitle,thetotal, thephnam,thephw, thephdeb,thephfin, thestw);  }

    void  Interface_STAT::Internals
  (Handle(TCollection_HAsciiString)& tit, Standard_Real& total,
   Handle(TColStd_HSequenceOfAsciiString)& phn,
   Handle(TColStd_HSequenceOfReal)& phw,
   Handle(TColStd_HSequenceOfInteger)& phdeb,
   Handle(TColStd_HSequenceOfInteger)& phfin,
   Handle(TColStd_HSequenceOfReal)& stw) const
{
  tit   = thetitle;  total = thetotal;  phn = thephnam;  phw = thephw;
  phdeb = thephdeb;  phfin = thephfin;  stw = thestw;
}

    void  Interface_STAT::AddPhase
  (const Standard_Real weight, const Standard_CString name)
{
  if (thephw.IsNull()) {
//  1re fois : vider les steps deja notees
    thetotal = 0.;
    thephnam = new TColStd_HSequenceOfAsciiString();
    thephw    = new TColStd_HSequenceOfReal();
    thephdeb  = new TColStd_HSequenceOfInteger();
    thephfin  = new TColStd_HSequenceOfInteger();
    thestw    = new TColStd_HSequenceOfReal();
  }
  thetotal += weight;
  thephnam->Append (TCollection_AsciiString (name));
  thephw->Append    (weight);
  thephdeb->Append (thestw->Length()+1);
  thephfin->Append (0);
  thestw->Append   (0.);
}

    void  Interface_STAT::AddStep (const Standard_Real weight)
{
  if (thephdeb.IsNull()) {
// 1re fois : pour default phase, au moins creer receptacle des steps
    thephdeb  = new TColStd_HSequenceOfInteger();
    thephfin  = new TColStd_HSequenceOfInteger();
    thestw    = new TColStd_HSequenceOfReal();
    thephdeb->Append (thestw->Length()+1);
    thephfin->Append (1);
    thestw->Append   (0.);
  }
//  A present, ajouter cette etape
  Standard_Integer n0 = thephdeb->Value (thephdeb->Length());
//  Ceci donne dans thestw le numero du cumul des etapes
  thestw->ChangeValue (n0) += weight;
  thestw->Append   (weight);  // on ajoute cette etape
  thephfin->ChangeValue (thephfin->Length()) ++;
}


    void  Interface_STAT::Description
  (Standard_Integer& nbphases,
   Standard_Real& total, Standard_CString& title) const
{
  nbphases = (thephw.IsNull() ? 1 : thephw->Length());
  total = thetotal;
  title = thetitle->ToCString();
}

    void  Interface_STAT::Phase
  (const Standard_Integer num,
   Standard_Integer& n0step, Standard_Integer& nbstep,
   Standard_Real& weight, Standard_CString& name) const
{
  if (thephdeb.IsNull()) {
//  Pas de phase, pas d etape ... donc une seule ...
    n0step = -1;  nbstep = 1;  weight = 1.;  name = voidname;
  }
  if (thephw.IsNull()) {
//  Pas de phase mais des etapes
    weight = 1.;  name = voidname;
  } else if (num < 1 || num > thephdeb->Length()) return;
  else  {
//  Phase
    weight = thephw->Value(num);  name = thephnam->Value(num).ToCString();
    n0step = thephdeb->Value(num);
    nbstep = thephfin->Value(num);
  }

//  Voyons pour cette phase
}

    Standard_Real  Interface_STAT::Step (const Standard_Integer num) const
{
  if (thestw.IsNull()) return 1.;
  if (num < 1 || num > thestw->Length()) return 1.;
  return thestw->Value(num);
}

//  ###############  COMPTAGE  ################

//  Le comptage se fait sur la base suivante :
//  TOTAL  : total des poids des phases par rapport auquel calculer
//  PHASES : poids des phases passees  et  poids de la phase en cours
//    Ces poids sont a ramener au TOTAL
//  PHASE COURANTE : nb d items  et  nb de cycles declares
//    Nb d items deja passes (cycle complet)
//  CYCLE COURANT  : nb d items  de ce cycle, total des poids des etapes
//    Poids des etapes deja passees, de l etape en cours, n0 etape en cours
//  ETAPE COURANTE : nb d items deja passes

static struct zestat {
 Standard_CString itle, name;
 Standard_Real otal,  // total des poids des phases
  oldph,  // poids des phases deja passes
  phw,    // poids de la phase en cours
  otph,   // poids des etapes de la phase en cours (cycle en cours)
  oldst,  // poids des etapes deja passees (cycle en cours)
  stw;    // poids etape en cours
 Standard_Integer nbph, // total nb de phases
  numph,  // n0 phase en cours
  n0, n1, // n0 et nb etapes dans phase en cours
  nbitp,  // nb items total phase
  nbcyc,  // nb cycles total phase
  olditp, // nb items deja passes (cycles passes) / phase
  numcyc, // n0 cycle en cours / phase
  nbitc,  // nb items cycle en cours
  numst,  // n0 etape en cours / cycle
  numitem; // nb items deja passes / etape courante
} stat;


    void  Interface_STAT::Start
  (const Standard_Integer items, const Standard_Integer cycles) const
{
  statact = *this;
  statact.Description (stat.nbph,stat.otal,stat.itle);
  stat.oldph = stat.phw = 0.;  stat.numph = 0;
  NextPhase (items,cycles);
}

    void  Interface_STAT::StartCount
  (const Standard_Integer items, const Standard_CString name)
{
  Interface_STAT statcount(name);
  statcount.Start (items);
}

    void  Interface_STAT::NextPhase
  (const Standard_Integer items, const Standard_Integer cycles)
{
//  On cumule la phase precedente au total, on efface les donnees "locales"
  stat.numcyc  = stat.numst = stat.olditp = 0;  stat.oldst = stat.stw = 0.;
  if (stat.numph >= stat.nbph) {  End();  return;  }

  stat.numph ++;  stat.oldph += stat.phw;   // cumule sur cette phase
  stat.nbitp = items;  stat.nbcyc = cycles;
  statact.Phase(stat.numph, stat.n0,stat.n1,stat.phw,stat.name);
  stat.otph = (stat.n1 > 1 ? statact.Step (stat.n0) : 1.);
//   si un seul cycle, on le demarre; sinon, attendre NextCycle
  stat.nbitc = 0;
  if (cycles == 1) NextCycle (items);
}

    void  Interface_STAT::SetPhase
  (const Standard_Integer items, const Standard_Integer cycles)
      {  stat.nbitp = items;  stat.nbcyc = cycles;  }

    void  Interface_STAT::NextCycle (const Standard_Integer items)
{
//  cumul de ce cycle sur les cycles deja passes, raz etapes
  stat.numcyc ++;  stat.olditp += stat.nbitc;
//  if (stat.olditem > stat.nbitp) return;
  stat.numst = 1;
  stat.oldst = 0.;
  stat.stw   = (stat.n1 > 1 ? statact.Step(stat.n0 + 1) : stat.otph);
  stat.nbitc = items;  stat.numitem = 0;
}

    void  Interface_STAT::NextStep ()
{
  if (stat.numst >= stat.n1) return;
  stat.numst ++;  stat.oldst += stat.stw;
  stat.numitem = 0;
  stat.stw = statact.Step (stat.n0 + stat.numst);
}

    void  Interface_STAT::NextItem (const Standard_Integer nbitems)
      {  stat.numitem += nbitems;  }

    void  Interface_STAT::End ()
{  stat.oldph = stat.otal;  stat.phw = stat.stw = 0.;  stat.itle = stat.name = voidname;  }

// ###########  QUERY  ############

    Standard_CString  Interface_STAT::Where   (const Standard_Boolean phase)
      {  return (phase ? stat.name : stat.itle);  }

    Standard_Integer  Interface_STAT::Percent (const Standard_Boolean phase)
{
  if (stat.numitem > stat.nbitc) stat.numitem = stat.nbitc;
//  on compte les items deja passes
  Standard_Real enphase =
    stat.olditp  * stat.otph  +  // cycles complets passes
    stat.nbitc   * stat.oldst +  // cycle courant, etapes completes passees
    stat.numitem * stat.stw;     // etape courante
//  proportion pour cette phase
  Standard_Real prophase = enphase / (stat.nbitp * stat.otph);
  Standard_Integer res = Standard_Integer (prophase*100.);
  if (phase) return res;

//  voila pour cette phase
//  comptage dans les phases
  Standard_Real encours = (stat.oldph + stat.phw * prophase) / stat.otal;
  res = Standard_Integer (encours * 100.);
  return res;
}
