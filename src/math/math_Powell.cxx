// Copyright (c) 1997-1999 Matra Datavision
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

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math_BracketMinimum.hxx>
#include <math_BrentMinimum.hxx>
#include <math_Function.hxx>
#include <math_Matrix.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_Powell.hxx>
#include <Standard_DimensionError.hxx>
#include <StdFail_NotDone.hxx>

namespace {
static inline Standard_Real SQR (const Standard_Real a)
{
    return a * a;
}
}

class DirFunctionBis : public math_Function {

  math_Vector *P0;
  math_Vector *Dir;
  math_Vector *P;
  math_MultipleVarFunction *F;

  public :
    DirFunctionBis(math_Vector& V1,
		math_Vector& V2,
		math_Vector& V3,
		math_MultipleVarFunction& f);

    void Initialize(const math_Vector& p0, const math_Vector& dir);

    virtual Standard_Boolean Value(const Standard_Real x, Standard_Real& fval);
};

 DirFunctionBis::DirFunctionBis(math_Vector& V1,
			  math_Vector& V2,
			  math_Vector& V3,
			  math_MultipleVarFunction& f) {
   P0 = &V1;
   Dir = &V2;
   P = &V3;
   F = &f;
 }


void DirFunctionBis::Initialize(const math_Vector& p0,
			     const math_Vector& dir) {

  *P0 = p0;
  *Dir = dir;
}

Standard_Boolean DirFunctionBis::Value(const Standard_Real x, Standard_Real& fval) {
  *P = *Dir;
  P->Multiply(x);
  P->Add(*P0);

  fval = 0.0;
  return F->Value(*P, fval);
}


static Standard_Boolean MinimizeDirection(math_Vector& P,
				 math_Vector& Dir,
				 Standard_Real& Result,
				 DirFunctionBis& F) {

  Standard_Real ax;
  Standard_Real xx;
  Standard_Real bx;


  F.Initialize(P, Dir);

  math_BracketMinimum Bracket(F, 0.0, 1.0);
  if (Bracket.IsDone()) {
    Bracket.Values(ax, xx, bx);
    math_BrentMinimum Sol(1.0e-10);
    Sol.Perform(F, ax, xx, bx);
    if (Sol.IsDone()) {
      Standard_Real Scale = Sol.Location();
      Result = Sol.Minimum();
      Dir.Multiply(Scale);
      P.Add(Dir);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : math_Powell
//purpose  : Constructor
//=======================================================================
math_Powell::math_Powell(const math_MultipleVarFunction& theFunction,
                         const Standard_Real             theTolerance,
                         const Standard_Integer          theNbIterations,
                         const Standard_Real             theZEPS)
: TheLocation     (1, theFunction.NbVariables()),
  TheMinimum      (RealLast()),
  TheLocationError(RealLast()),
  PreviousMinimum (RealLast()),
  XTol            (theTolerance),
  EPSZ            (theZEPS),
  Done            (Standard_False),
  Iter            (0),
  TheStatus       (math_NotBracketed),
  TheDirections   (1, theFunction.NbVariables(), 1, theFunction.NbVariables()),
  State           (0),
  Itermax         (theNbIterations)
{
}

//=======================================================================
//function : ~math_Powell
//purpose  : Destructor
//=======================================================================
math_Powell::~math_Powell()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void math_Powell::Perform(math_MultipleVarFunction& F,
                          const math_Vector& StartingPoint,
                          const math_Matrix& StartingDirections)
{
  Done = Standard_False;
  Standard_Integer i, ibig, j;
  Standard_Real t, fptt, del;
  Standard_Integer n = TheLocation.Length();
  math_Vector pt(1,n);
  math_Vector ptt(1,n);
  math_Vector xit(1,n);
  math_Vector Temp1(1, n);
  math_Vector Temp2(1, n);
  math_Vector Temp3(1, n);
  DirFunctionBis F_Dir(Temp1, Temp2, Temp3, F);

  TheLocation = StartingPoint;
  TheDirections = StartingDirections;
  pt = TheLocation;  //sauvegarde du point initial


  for(Iter = 1; Iter<= Itermax; Iter++) {
    F.Value(TheLocation, PreviousMinimum);
    ibig = 0;
    del = 0.0;
    for (i = 1; i <= n; i++){
      for(j =1; j<= n; j++) xit(j) = TheDirections(j,i);
      F.Value(TheLocation, fptt); 
      Standard_Boolean IsGood = MinimizeDirection(TheLocation, xit, 
					 TheMinimum, F_Dir);

      if (!IsGood) {
	Done = Standard_False;
	TheStatus = math_DirectionSearchError;
	return;
      }

      if (fabs(fptt - TheMinimum)> del) {
	del = fabs(fptt- TheMinimum);
	ibig = i;
      }
    }

    if (IsSolutionReached(F)) {
      //Termination criterion
      State = F.GetStateNumber();
      Done = Standard_True;
      TheStatus = math_OK;
      return;
    }

    if (Iter == Itermax) {
      Done = Standard_False;
      TheStatus = math_TooManyIterations;
      return;
    }

      ptt = 2.0 * TheLocation - pt;
      xit = TheLocation - pt;
      pt = TheLocation;
    
    // Valeur de la fonction au point extrapole:

    F.Value(ptt, fptt);

    if (fptt < PreviousMinimum) {
      t = 2.0 *(PreviousMinimum -2.0*TheMinimum +fptt)*
	SQR(PreviousMinimum-TheMinimum -del)-del*
	  SQR(PreviousMinimum-fptt);
      if (t <0.0) {
	//Minimisation along the direction
	Standard_Boolean IsGood = MinimizeDirection(TheLocation, xit,
					   TheMinimum, F_Dir);
	if(!IsGood) {
	  Done = Standard_False;
	  TheStatus = math_FunctionError;
	  return;
	}
	
        for(j =1; j <= n; j++) {
	  TheDirections(j, ibig)=xit(j);
        }
      }
    }
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void math_Powell::Dump(Standard_OStream& o) const
{
  o << "math_Powell resolution:";
  if(Done) {
    o << " Status = Done \n";
    o << " Location Vector = "<< TheLocation << "\n";
    o << " Minimum value = " << TheMinimum <<"\n";
    o << " Number of iterations = " << Iter <<"\n";
  }
  else {
    o << " Status = not Done because " << (Standard_Integer)TheStatus << "\n";
  }
}
