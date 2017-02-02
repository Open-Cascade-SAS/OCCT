// Created on: 2002-04-25
// Created by: Michael PONIKAROV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <QADNaming.hxx>
#include <TDF_Label.hxx>
#include <Draw_Interpretor.hxx>
#include <TNaming_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>

static Standard_Integer BuildNamedShape (Draw_Interpretor& di,
					 Standard_Integer nb, 
					 const char** arg) {
  if (nb >= 4) { 
    TDF_Label aLabel;
    if (!QADNaming::Entry(arg, aLabel)) return 1;
    char anEvolution = arg[3][0];
    Standard_Integer a,anInc = (anEvolution == 'P' || anEvolution == 'D') ? 1 : 2;
//     if (anEvolution == 'G') 
//       for(a=1;arg[3][a]!=0 && arg[3][a]!='\n';a++) if (arg[3][a]=='2') anEvolution = '2';
    TNaming_Builder aBuilder(aLabel);
    TopoDS_Shape aShape1,aShape2;

    for(a = 4;a < nb;a += anInc) {
      aShape1 = DBRep::Get (arg[a]);
      if (anInc == 2) {
	if (a == nb - 1) {
	  di<<"For this type of evolution number of shapes must be even\n";
	  return 1;
	}
	aShape2 = DBRep::Get(arg[a+1]);
      }

      switch (anEvolution) {
      case 'P':
	aBuilder.Generated(aShape1);
	break;
      case 'G':
	aBuilder.Generated(aShape1,aShape2);
	break;
      case 'M':
	aBuilder.Modify(aShape1,aShape2);
	break;
      case 'D':
	aBuilder.Delete(aShape1);
	break;
      case 'R':
//	aBuilder.Replace(aShape1,aShape2);
	aBuilder.Modify(aShape1,aShape2);
	break;
      case 'S':
	aBuilder.Select(aShape1,aShape2);
	break;
      default:
	di<<"Unknown evolution type\n";
	return 1;
      }
    }
    
//     if (nb >= 4) {
//       OnlyModif = Draw::Atoi(arg[3]);
//     }
  } else {
    di<<"Usage: BuildName df entry evolution(P[RIMITIVE] G[ENERATED] M[ODIFY] D[ELETE] R[EPLACE] S[ELECTED]) [shape1 shape2 ...]\n";
    return 1;
  }
  return 0;
}


void QADNaming::BuilderCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "Naming builder commands";

  theCommands.Add("BuildNamedShape",
		  "BuildNamedShape df entry evolution(P[RIMITIVE] G[ENERATED] M[ODIFY] D[ELETE] R[EPLACE] S[ELECTED]) [shape1 shape2 ...]",
		  __FILE__,BuildNamedShape,g);
}
