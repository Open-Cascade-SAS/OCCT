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

#include <MoniTool_Stat.hxx>
#include <TCollection_HAsciiString.hxx>

// static MoniTool_Stat Statvoid("");
// static MoniTool_Stat Statact ("");
// not Used
// static const char* voidname = "";
MoniTool_Stat::MoniTool_Stat(const char* title)
{
  thetit = new TCollection_HAsciiString(title);
  thelev = 0;
  thetot = new NCollection_HArray1<int>(1, 20);
  thetot->Init(0);
  thedone = new NCollection_HArray1<int>(1, 20);
  thetot->Init(0);
  thecurr = new NCollection_HArray1<int>(1, 20);
  thetot->Init(0);
}

MoniTool_Stat::MoniTool_Stat(const MoniTool_Stat&)
    : thelev(0)
{
}

MoniTool_Stat& MoniTool_Stat::Current()
{
  static MoniTool_Stat thecur;
  return thecur;
}

int MoniTool_Stat::Open(const int nb)
{
  thelev++;
  thetot->SetValue(thelev, nb);
  thedone->SetValue(thelev, 0);
  thecurr->SetValue(thelev, 0);
  return thelev;
}

void MoniTool_Stat::OpenMore(const int id, const int nb)
{
  if (id <= 0 || id > thelev)
    return;
  thetot->SetValue(id, thetot->Value(id) + nb);
}

void MoniTool_Stat::Add(const int nb)
{
  thedone->SetValue(thelev, thedone->Value(thelev) + nb);
  thecurr->SetValue(thelev, 0);
}

void MoniTool_Stat::AddSub(const int nb)
{
  thecurr->SetValue(thelev, nb);
}

void MoniTool_Stat::AddEnd()
{
  thedone->SetValue(thelev, thedone->Value(thelev) + thecurr->Value(thelev));
  thecurr->SetValue(thelev, 0);
}

void MoniTool_Stat::Close(const int id)
{
  if (id < thelev)
    Close(id + 1);
  AddEnd();
  thelev--;
}

int MoniTool_Stat::Level() const
{
  return thelev;
}

double MoniTool_Stat::Percent(const int fromlev) const
{
  if (fromlev > thelev)
    return 0;
  double    r1, r2, r3;
  int tot  = thetot->Value(fromlev);
  int done = thedone->Value(fromlev);
  if (done >= tot)
    return 100.;
  if (fromlev == thelev)
  {
    r1 = tot;
    r2 = done;
    return (r2 * 100) / r1;
  }
  int cur = thecurr->Value(fromlev);
  r1                   = tot;
  r2                   = done;
  r3                   = 0;
  if (cur > 0)
  {
    r3 = cur;
    r3 = cur / tot;
    r3 = r3 * Percent(fromlev + 1);
  }
  if (r1 == 0)
    return 1;
  return (r2 * 100) / r1 + r3;
}
