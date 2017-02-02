// Created on: 2004-05-20
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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


#include <BinTools.hxx>
#include <BinTools_CurveSet.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#define LINE      1
#define CIRCLE    2
#define ELLIPSE   3
#define PARABOLA  4
#define HYPERBOLA 5
#define BEZIER    6
#define BSPLINE   7
#define TRIMMED   8
#define OFFSET    9

//=======================================================================
//function : BinTools_CurveSet
//purpose  : 
//=======================================================================

BinTools_CurveSet::BinTools_CurveSet() 
{
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  BinTools_CurveSet::Clear()
{
  myMap.Clear();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Integer  BinTools_CurveSet::Add(const Handle(Geom_Curve)& C)
{
  return  (C.IsNull()) ? 0 : myMap.Add(C);
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Handle(Geom_Curve)  BinTools_CurveSet::Curve
       (const Standard_Integer I)const 
{
  if  (I == 0) {
	Handle(Geom_Curve) dummy;
	return dummy;
  }
  else
    return Handle(Geom_Curve)::DownCast(myMap(I));
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  BinTools_CurveSet::Index
  (const Handle(Geom_Curve)& S)const 
{
  return S.IsNull() ? 0 : myMap.FindIndex(S);
}


//=======================================================================
//function : operator << (gp_Pnt)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const gp_Pnt P)
{
  BinTools::PutReal(OS, P.X());
  BinTools::PutReal(OS, P.Y());
  BinTools::PutReal(OS, P.Z());
  return OS;
}

//=======================================================================
//function : operator << (gp_Dir)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const gp_Dir D)
{
  BinTools::PutReal(OS, D.X());
  BinTools::PutReal(OS, D.Y());
  BinTools::PutReal(OS, D.Z());
  return OS;
}


//=======================================================================
//function : operator << (Geom_Line)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom_Line)& L)
{
  OS << (Standard_Byte)LINE;
  gp_Lin C = L->Lin();
  OS << C.Location();//Pnt
  OS << C.Direction();//Dir
  return OS;
  
}

//=======================================================================
//function :  operator <<(Geom_Circle)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom_Circle)& CC)
{
  OS << (Standard_Byte)CIRCLE;
  gp_Circ C = CC->Circ();
  OS << C.Location();
  OS << C.Axis().Direction();
  OS << C.XAxis().Direction();
  OS << C.YAxis().Direction();
  BinTools::PutReal(OS, C.Radius());
  return OS;
}

//=======================================================================
//function : operator <<(Geom_Ellipse)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom_Ellipse)& E)
{
  OS << (Standard_Byte)ELLIPSE;
  gp_Elips C = E->Elips();
  OS << C.Location();
  OS << C.Axis().Direction();
  OS << C.XAxis().Direction();
  OS << C.YAxis().Direction();
  BinTools::PutReal(OS, C.MajorRadius());
  BinTools::PutReal(OS, C.MinorRadius());
  return OS;  
}

//=======================================================================
//function : operator <<(Geom_Parabola)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom_Parabola)& P)
{
  OS << (Standard_Byte)PARABOLA;
  gp_Parab C = P->Parab();
  OS << C.Location();
  OS << C.Axis().Direction();
  OS << C.XAxis().Direction();
  OS << C.YAxis().Direction();
  BinTools::PutReal(OS, C.Focal());
  return OS;  
}

//=======================================================================
//function : operator <<(Geom_Hyperbola)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom_Hyperbola)& H)
{
  OS << (Standard_Byte)HYPERBOLA;
  gp_Hypr C = H->Hypr();
  OS << C.Location();
  OS << C.Axis().Direction();
  OS << C.XAxis().Direction();
  OS << C.YAxis().Direction();
  BinTools::PutReal(OS, C.MajorRadius());
  BinTools::PutReal(OS, C.MinorRadius());
  return OS; 
}

//=======================================================================
//function : operator <<(Geom_BezierCurve)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom_BezierCurve)& B)
{
  OS << (Standard_Byte)BEZIER;
  Standard_Boolean aRational = B->IsRational() ? 1:0;
  BinTools::PutBool(OS, aRational); //rational
  // poles and weights
  Standard_Integer i,aDegree = B->Degree(); 
  BinTools::PutExtChar(OS, (Standard_ExtCharacter)aDegree); //<< Degree
  for (i = 1; i <= aDegree+1; i++) {
    OS << B->Pole(i); //Pnt
    if (aRational)
      BinTools::PutReal(OS, B->Weight(i));//Real
  }
  return OS;  
}

//=======================================================================
//function : operator <<(Geom_BSplineCurve)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom_BSplineCurve)& B)
{
  OS << (Standard_Byte)BSPLINE;
  Standard_Boolean aRational = B->IsRational() ? 1:0;
  BinTools::PutBool(OS, aRational); //rational
  Standard_Boolean aPeriodic = B->IsPeriodic() ? 1:0;
  BinTools::PutBool(OS, aPeriodic); //periodic
  // poles and weights
  Standard_Integer i,aDegree,aNbPoles,aNbKnots;
  aDegree = B->Degree();
  aNbPoles = B->NbPoles();
  aNbKnots = B->NbKnots();
  BinTools::PutExtChar(OS, (Standard_ExtCharacter) aDegree);
  BinTools::PutInteger(OS, aNbPoles);
  BinTools::PutInteger(OS, aNbKnots);
  for (i = 1; i <= aNbPoles; i++) {
    OS << B->Pole(i); // Pnt
    if (aRational)
      BinTools::PutReal(OS, B->Weight(i));
  }
  
  for (i = 1; i <= aNbKnots; i++) {
    BinTools::PutReal(OS, B->Knot(i));
    BinTools::PutInteger(OS, B->Multiplicity(i));
  }  
  return OS;
}

//=======================================================================
//function : operator <<(Geom_TrimmedCurve)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom_TrimmedCurve)& C)
{
  OS << (Standard_Byte)TRIMMED;
  BinTools::PutReal(OS, C->FirstParameter()); 
  BinTools::PutReal(OS, C->LastParameter());
  BinTools_CurveSet::WriteCurve(C->BasisCurve(),OS);
  return OS; 
}

//=======================================================================
//function : operator <<(Geom_OffsetCurve)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom_OffsetCurve)& C)
{
  OS << (Standard_Byte)OFFSET;
  BinTools::PutReal(OS,C->Offset());//Offset 
  OS << C->Direction();
  BinTools_CurveSet::WriteCurve(C->BasisCurve(),OS);
  return OS; 
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BinTools_CurveSet::WriteCurve(const Handle(Geom_Curve)& C,
				    Standard_OStream& OS)
{
  Handle(Standard_Type) TheType = C->DynamicType();
  try {
    OCC_CATCH_SIGNALS
    if ( TheType ==STANDARD_TYPE(Geom_Line)) {
      OS << Handle(Geom_Line)::DownCast(C);
    }
    else if ( TheType ==  STANDARD_TYPE(Geom_Circle)) {
      OS << Handle(Geom_Circle)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom_Ellipse)) {
      OS << Handle(Geom_Ellipse)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom_Parabola)) {
      OS << Handle(Geom_Parabola)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom_Hyperbola)) {
      OS << Handle(Geom_Hyperbola)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom_BezierCurve)) {
      OS << Handle(Geom_BezierCurve)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom_BSplineCurve)) {
      OS << Handle(Geom_BSplineCurve)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom_TrimmedCurve)) {
      OS << Handle(Geom_TrimmedCurve)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom_OffsetCurve)) {
      OS << Handle(Geom_OffsetCurve)::DownCast(C);
    }
    else {
      throw Standard_Failure("UNKNOWN CURVE TYPE");
    }
  }
   catch(Standard_Failure const& anException) {
     Standard_SStream aMsg;
     aMsg << "EXCEPTION in BinTools_CurveSet::WriteCurve(..)" << endl;
     aMsg << anException << endl;
     throw Standard_Failure(aMsg.str().c_str());
   }  
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  BinTools_CurveSet::Write(Standard_OStream& OS)const 
{
  Standard_Integer i, nbsurf = myMap.Extent();
  OS << "Curves "<< nbsurf << "\n";
  for (i = 1; i <= nbsurf; i++) {
    WriteCurve(Handle(Geom_Curve)::DownCast(myMap(i)),OS);
  }
}


//=======================================================================
//function : ReadPnt
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Pnt& P)
{
  Standard_Real X=0.,Y=0.,Z=0.;
  BinTools::GetReal(IS, X);
  BinTools::GetReal(IS, Y);
  BinTools::GetReal(IS, Z);
  P.SetCoord(X,Y,Z);
  return IS;
}

//=======================================================================
//function : ReadDir
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Dir& D)
{
  Standard_Real X=0.,Y=0.,Z=0.;
  BinTools::GetReal(IS, X);
  BinTools::GetReal(IS, Y);
  BinTools::GetReal(IS, Z);
  D.SetCoord(X,Y,Z);
  return IS;
}


//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Line)& L)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir AX(1.,0.,0.);
  IS >> P >> AX;
  L = new Geom_Line(P,AX);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Circle)& C)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir A(1.,0.,0.),AX(1.,0.,0.),AY(1.,0.,0.);
  Standard_Real R=0.;
  IS >> P >> A >> AX >> AY;
  BinTools::GetReal(IS, R);
  C = new Geom_Circle(gp_Ax2(P,A,AX),R);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Ellipse)& E)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir A(1.,0.,0.),AX(1.,0.,0.),AY(1.,0.,0.);
  Standard_Real R1=0.,R2=0.;
  IS >> P >> A >> AX >> AY;
  BinTools::GetReal(IS, R1);
  BinTools::GetReal(IS, R2);
  E = new Geom_Ellipse(gp_Ax2(P,A,AX),R1,R2);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Parabola)& C)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir A(1.,0.,0.),AX(1.,0.,0.),AY(1.,0.,0.);
  Standard_Real R1=0.;
  IS >> P >> A >> AX >> AY;
  BinTools::GetReal(IS, R1);
  C = new Geom_Parabola(gp_Ax2(P,A,AX),R1);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_Hyperbola)& H)
{
  gp_Pnt P(0.,0.,0.);
  gp_Dir A(1.,0.,0.),AX(1.,0.,0.),AY(1.,0.,0);
  Standard_Real R1=0.,R2=0.;
  IS >> P >> A >> AX >> AY;
  BinTools::GetReal(IS, R1);
  BinTools::GetReal(IS, R2);
  H = new Geom_Hyperbola(gp_Ax2(P,A,AX),R1,R2);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_BezierCurve)& B)
{
  Standard_Boolean rational=Standard_False;
  BinTools::GetBool(IS, rational);

// poles and weights
  Standard_Integer i=0,degree=0;
// degree;
  Standard_ExtCharacter aVal='\0';
  BinTools::GetExtChar(IS, aVal);
  degree = (Standard_Integer)aVal;

  TColgp_Array1OfPnt poles(1,degree+1);
  TColStd_Array1OfReal weights(1,degree+1);
  
  for (i = 1; i <= degree+1; i++) {
    IS >> poles(i);//Pnt
    if (rational)
// weights(i);
      BinTools::GetReal(IS, weights(i));
  }

  if (rational)
    B = new Geom_BezierCurve(poles,weights);
  else
    B = new Geom_BezierCurve(poles);

  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_BSplineCurve)& B)
{

  Standard_Boolean rational=Standard_False,periodic=Standard_False;
  BinTools::GetBool(IS, rational);
  BinTools::GetBool(IS, periodic);

// poles and weights
  Standard_Integer i=0,degree=0,nbpoles=0,nbknots=0;
  Standard_ExtCharacter aVal='\0';
  BinTools::GetExtChar(IS, aVal);
  degree = (Standard_Integer)aVal;

  BinTools::GetInteger(IS, nbpoles);
  
  BinTools::GetInteger(IS, nbknots);

  TColgp_Array1OfPnt poles(1,nbpoles);
  TColStd_Array1OfReal weights(1,nbpoles);
  
  for (i = 1; i <= nbpoles; i++) {
    IS >> poles(i);//Pnt
    if (rational)
      BinTools::GetReal(IS, weights(i));
  }

  TColStd_Array1OfReal knots(1,nbknots);
  TColStd_Array1OfInteger mults(1,nbknots);

  for (i = 1; i <= nbknots; i++) {
    BinTools::GetReal(IS, knots(i));
    BinTools::GetInteger(IS, mults(i));
  }

  if (rational)
    B = new Geom_BSplineCurve(poles,weights,knots,mults,degree,periodic);
  else
    B = new Geom_BSplineCurve(poles,knots,mults,degree,periodic);
  
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_TrimmedCurve)& C)
{
  Standard_Real p1=0.,p2=0.;
  BinTools::GetReal(IS, p1);//FirstParameter
  BinTools::GetReal(IS, p2);//LastParameter
  Handle(Geom_Curve) BC;
  BinTools_CurveSet::ReadCurve(IS,BC);
  C = new Geom_TrimmedCurve(BC,p1,p2);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom_OffsetCurve)& C)
{
  Standard_Real p=0.;
  BinTools::GetReal(IS, p);//Offset
  gp_Dir D(1.,0.,0.);
  IS >> D;
  Handle(Geom_Curve) BC;
  BinTools_CurveSet::ReadCurve(IS,BC);
  C = new Geom_OffsetCurve(BC,p,D);
  return IS;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

Standard_IStream& BinTools_CurveSet::ReadCurve(Standard_IStream& IS,
						Handle(Geom_Curve)& C)
{
  try {
    OCC_CATCH_SIGNALS
    const Standard_Byte ctype = (Standard_Byte) IS.get();

    switch (ctype) {

    case LINE :
      {
        Handle(Geom_Line) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case CIRCLE :
      {
        Handle(Geom_Circle) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case ELLIPSE :
      {
        Handle(Geom_Ellipse) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case PARABOLA :
      {
        Handle(Geom_Parabola) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case HYPERBOLA :
      {
        Handle(Geom_Hyperbola) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case BEZIER :
      {
        Handle(Geom_BezierCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case BSPLINE :
      {
        Handle(Geom_BSplineCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case TRIMMED :
      {
        Handle(Geom_TrimmedCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case OFFSET :
      {
        Handle(Geom_OffsetCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;
      
    default:
      {
	C = NULL;
	throw Standard_Failure("UNKNOWN CURVE TYPE");
      }
    }
  }
  catch(Standard_Failure const& anException) {
    C = NULL;
    Standard_SStream aMsg;
    aMsg <<"EXCEPTION in BinTools_CurveSet::ReadCurve(..)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  return IS;
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void  BinTools_CurveSet::Read(Standard_IStream& IS)
{
  char buffer[255];
  IS >> buffer;
  if (IS.fail() || strcmp(buffer,"Curves")) {
    Standard_SStream aMsg;
    aMsg << "BinTools_CurveSet::Read:  Not a Curve table"<<endl;
#ifdef OCCT_DEBUG
    cout <<"CurveSet buffer: " << buffer << endl;
#endif
    throw Standard_Failure(aMsg.str().c_str());
    return;
  }

  Handle(Geom_Curve) C;
  Standard_Integer i, nbcurve;
  IS >> nbcurve;

  IS.get();//remove <lf>
  for (i = 1; i <= nbcurve; i++) {
    BinTools_CurveSet::ReadCurve(IS,C);
    myMap.Add(C);
  }
}


