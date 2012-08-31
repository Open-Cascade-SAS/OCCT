// Created on: 2002-04-25
// Created by: Michael PONIKAROV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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



#include <QADNaming.hxx>
#include <TDF_Label.hxx>
#include <Draw_Interpretor.hxx>
#include <TNaming_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>

static Standard_Integer BuildNamedShape (Draw_Interpretor& di,
					 Standard_Integer nb, 
					 const char** arg) {
  if (nb > 4) { 
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
	  di<<"For this type of evolution number of shapes must be even"<<"\n";
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
	di<<"Unknown evolution type"<<"\n";
	return 1;
      }
    }
    
//     if (nb >= 4) {
//       OnlyModif = atoi(arg[3]);
//     }
  } else {
    di<<"Usage: BuildName df entry evolution(P[RIMITIVE] G[ENERATED] M[ODIFY] D[ELETE] R[EPLACE] S[ELECTED]) shape1 [shape2 ...]"<<"\n";
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
		  "BuildNamedShape df entry evolution(P[RIMITIVE] G[ENERATED] M[ODIFY] D[ELETE] R[EPLACE] S[ELECTED]) shape1 [shape2 ...]",
		  __FILE__,BuildNamedShape,g);
}
