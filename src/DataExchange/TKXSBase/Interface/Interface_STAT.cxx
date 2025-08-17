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

#include <Interface_STAT.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

static Interface_STAT   statvoid("");
static Interface_STAT   statact("");
static Standard_CString voidname = "";

Interface_STAT::Interface_STAT(const Standard_CString title)
{
  thetitle = new TCollection_HAsciiString(title);
  thetotal = 1.;
}

Interface_STAT::Interface_STAT(const Interface_STAT& other)
{
  other.Internals(thetitle, thetotal, thephnam, thephw, thephdeb, thephfin, thestw);
}

void Interface_STAT::Internals(Handle(TCollection_HAsciiString)&       tit,
                               Standard_Real&                          total,
                               Handle(TColStd_HSequenceOfAsciiString)& phn,
                               Handle(TColStd_HSequenceOfReal)&        phw,
                               Handle(TColStd_HSequenceOfInteger)&     phdeb,
                               Handle(TColStd_HSequenceOfInteger)&     phfin,
                               Handle(TColStd_HSequenceOfReal)&        stw) const
{
  tit   = thetitle;
  total = thetotal;
  phn   = thephnam;
  phw   = thephw;
  phdeb = thephdeb;
  phfin = thephfin;
  stw   = thestw;
}

void Interface_STAT::AddPhase(const Standard_Real weight, const Standard_CString name)
{
  if (thephw.IsNull())
  {
    //  1st time : empty the already noted steps
    thetotal = 0.;
    thephnam = new TColStd_HSequenceOfAsciiString();
    thephw   = new TColStd_HSequenceOfReal();
    thephdeb = new TColStd_HSequenceOfInteger();
    thephfin = new TColStd_HSequenceOfInteger();
    thestw   = new TColStd_HSequenceOfReal();
  }
  thetotal += weight;
  thephnam->Append(TCollection_AsciiString(name));
  thephw->Append(weight);
  thephdeb->Append(thestw->Length() + 1);
  thephfin->Append(0);
  thestw->Append(0.);
}

void Interface_STAT::AddStep(const Standard_Real weight)
{
  if (thephdeb.IsNull())
  {
    // 1st time : for default phase, at least create receptacle for steps
    thephdeb = new TColStd_HSequenceOfInteger();
    thephfin = new TColStd_HSequenceOfInteger();
    thestw   = new TColStd_HSequenceOfReal();
    thephdeb->Append(thestw->Length() + 1);
    thephfin->Append(1);
    thestw->Append(0.);
  }
  //  Now, add this step
  Standard_Integer n0 = thephdeb->Value(thephdeb->Length());
  //  This gives in thestw the number of the cumulative steps
  thestw->ChangeValue(n0) += weight;
  thestw->Append(weight); // we add this step
  thephfin->ChangeValue(thephfin->Length())++;
}

void Interface_STAT::Description(Standard_Integer& nbphases,
                                 Standard_Real&    total,
                                 Standard_CString& title) const
{
  nbphases = (thephw.IsNull() ? 1 : thephw->Length());
  total    = thetotal;
  title    = thetitle->ToCString();
}

void Interface_STAT::Phase(const Standard_Integer num,
                           Standard_Integer&      n0step,
                           Standard_Integer&      nbstep,
                           Standard_Real&         weight,
                           Standard_CString&      name) const
{
  if (thephdeb.IsNull())
  {
    //  No phase, no step ... so only one ...
    n0step = -1;
    nbstep = 1;
    weight = 1.;
    name   = voidname;
  }
  if (thephw.IsNull())
  {
    //  No phase but steps
    weight = 1.;
    name   = voidname;
  }
  else if (num < 1 || num > thephdeb->Length())
    return;
  else
  {
    //  Phase
    weight = thephw->Value(num);
    name   = thephnam->Value(num).ToCString();
    n0step = thephdeb->Value(num);
    nbstep = thephfin->Value(num);
  }

  //  Let's see for this phase
}

Standard_Real Interface_STAT::Step(const Standard_Integer num) const
{
  if (thestw.IsNull())
    return 1.;
  if (num < 1 || num > thestw->Length())
    return 1.;
  return thestw->Value(num);
}

//  ###############  COMPTAGE  ################

//  The counting is done on the following basis :
//  TOTAL  : total of phase weights against which to calculate
//  PHASES : weight of past phases  and  weight of current phase
//    These weights are to be brought back to TOTAL
//  CURRENT PHASE : nb of items  and  nb of declared cycles
//    Nb of items already passed (complete cycle)
//  CURRENT CYCLE  : nb of items  of this cycle, total of step weights
//    Weight of steps already passed, of the current step, n0 current step
//  CURRENT STEP : nb of items already passed

static struct zestat
{
  Standard_CString itle, name;
  Standard_Real    otal, // total of phase weights
    oldph,               // weight of phases already passed
    phw,                 // weight of the current phase
    otph,                // weight of steps in the current phase (current cycle)
    oldst,               // weight of steps already passed (current cycle)
    stw;                 // weight of current step
  Standard_Integer nbph, // total nb of phases
    numph,               // n0 current phase
    n0, n1,              // n0 and nb steps in current phase
    nbitp,               // nb items total phase
    nbcyc,               // nb cycles total phase
    olditp,              // nb items already passed (cycles passed) / phase
    numcyc,              // n0 current cycle / phase
    nbitc,               // nb items current cycle
    numst,               // n0 current step / cycle
    numitem;             // nb items already passed / current step
} TheStat;

void Interface_STAT::Start(const Standard_Integer items, const Standard_Integer cycles) const
{
  statact = *this;
  statact.Description(TheStat.nbph, TheStat.otal, TheStat.itle);
  TheStat.oldph = TheStat.phw = 0.;
  TheStat.numph               = 0;
  NextPhase(items, cycles);
}

void Interface_STAT::StartCount(const Standard_Integer items, const Standard_CString name)
{
  Interface_STAT statcount(name);
  statcount.Start(items);
}

void Interface_STAT::NextPhase(const Standard_Integer items, const Standard_Integer cycles)
{
  //  We accumulate the previous phase to the total, we clear the "local" data
  TheStat.numcyc = TheStat.numst = TheStat.olditp = 0;
  TheStat.oldst = TheStat.stw = 0.;
  if (TheStat.numph >= TheStat.nbph)
  {
    End();
    return;
  }

  TheStat.numph++;
  TheStat.oldph += TheStat.phw; // accumulate on this phase
  TheStat.nbitp = items;
  TheStat.nbcyc = cycles;
  statact.Phase(TheStat.numph, TheStat.n0, TheStat.n1, TheStat.phw, TheStat.name);
  TheStat.otph = (TheStat.n1 > 1 ? statact.Step(TheStat.n0) : 1.);
  //   if a single cycle, we start it; otherwise, wait NextCycle
  TheStat.nbitc = 0;
  if (cycles == 1)
    NextCycle(items);
}

void Interface_STAT::SetPhase(const Standard_Integer items, const Standard_Integer cycles)
{
  TheStat.nbitp = items;
  TheStat.nbcyc = cycles;
}

void Interface_STAT::NextCycle(const Standard_Integer items)
{
  //  accumulation of this cycle on cycles already passed, reset steps
  TheStat.numcyc++;
  TheStat.olditp += TheStat.nbitc;
  //  if (stat.olditem > stat.nbitp) return;
  TheStat.numst   = 1;
  TheStat.oldst   = 0.;
  TheStat.stw     = (TheStat.n1 > 1 ? statact.Step(TheStat.n0 + 1) : TheStat.otph);
  TheStat.nbitc   = items;
  TheStat.numitem = 0;
}

void Interface_STAT::NextStep()
{
  if (TheStat.numst >= TheStat.n1)
    return;
  TheStat.numst++;
  TheStat.oldst += TheStat.stw;
  TheStat.numitem = 0;
  TheStat.stw     = statact.Step(TheStat.n0 + TheStat.numst);
}

void Interface_STAT::NextItem(const Standard_Integer nbitems)
{
  TheStat.numitem += nbitems;
}

void Interface_STAT::End()
{
  TheStat.oldph = TheStat.otal;
  TheStat.phw = TheStat.stw = 0.;
  TheStat.itle = TheStat.name = voidname;
}

// ###########  QUERY  ############

Standard_CString Interface_STAT::Where(const Standard_Boolean phase)
{
  return (phase ? TheStat.name : TheStat.itle);
}

Standard_Integer Interface_STAT::Percent(const Standard_Boolean phase)
{
  if (TheStat.numitem > TheStat.nbitc)
    TheStat.numitem = TheStat.nbitc;
  //  we count the items already passed
  Standard_Real enphase = TheStat.olditp * TheStat.otph + // complete cycles passed
                          TheStat.nbitc * TheStat.oldst + // current cycle, complete steps passed
                          TheStat.numitem * TheStat.stw;  // current step
                                                          //  proportion for this phase
  Standard_Real    prophase = enphase / (TheStat.nbitp * TheStat.otph);
  Standard_Integer res      = Standard_Integer(prophase * 100.);
  if (phase)
    return res;

  //  that's it for this phase
  //  counting in the phases
  Standard_Real encours = (TheStat.oldph + TheStat.phw * prophase) / TheStat.otal;
  res                   = Standard_Integer(encours * 100.);
  return res;
}
