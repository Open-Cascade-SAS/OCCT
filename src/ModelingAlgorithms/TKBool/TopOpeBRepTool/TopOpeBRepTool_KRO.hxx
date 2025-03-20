// Created on: 1996-10-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_KRO_HeaderFile
#define _TopOpeBRepTool_KRO_HeaderFile
#ifdef OCCT_DEBUG
  #include <OSD_Chronometer.hxx>
  #include <TCollection_AsciiString.hxx>
  #include <Standard_OStream.hxx>

// POP pour NT
class TOPKRO : public OSD_Chronometer
{
public:
  TOPKRO(const TCollection_AsciiString& n)
      : myname(n),
        mystart(0),
        mystop(0)
  {
    myname.RightJustify(30, ' ');
  }

  virtual void Start()
  {
    mystart = 1;
    OSD_Chronometer::Start();
  }

  virtual void Stop()
  {
    OSD_Chronometer::Stop();
    mystop = 1;
  }

  void Print(Standard_OStream& OS)
  {
    Standard_Real s;
    Show(s);
    OS << myname << " : ";
    if (!mystart)
      OS << "(inactif)";
    else
    {
      OS << s << " secondes";
      if (!mystop)
        OS << " (run)";
    }
  }

private:
  TCollection_AsciiString myname;
  Standard_Integer        mystart, mystop;
};
#endif
#endif
