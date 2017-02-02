// Created on: 2004-05-18
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
#include <BinTools_Curve2dSet.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Stream.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
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
//function : BinTools_Curve2dSet
//purpose  : 
//=======================================================================

BinTools_Curve2dSet::BinTools_Curve2dSet() 
{
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void  BinTools_Curve2dSet::Clear()
{
  myMap.Clear();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Integer  BinTools_Curve2dSet::Add(const Handle(Geom2d_Curve)& S)
{
  return myMap.Add(S);
}


//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)  BinTools_Curve2dSet::Curve2d
       (const Standard_Integer I)const 
{
  return  Handle(Geom2d_Curve)::DownCast(myMap(I));
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer  BinTools_Curve2dSet::Index
  (const Handle(Geom2d_Curve)& S)const 
{
  return myMap.FindIndex(S);
}

//=======================================================================
//function : operator << (gp_Pnt2d)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const gp_Pnt2d P)
		  
{
  BinTools::PutReal(OS, P.X());
  return BinTools::PutReal(OS, P.Y());
}

//=======================================================================
//function : operator << (gp_Dir2d D)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const gp_Dir2d D)
{
  BinTools::PutReal(OS, D.X());
  return BinTools::PutReal(OS, D.Y());
}

//=======================================================================
//function : operator << ((Geom2d_Line)& L)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom2d_Line)& L)
{

  OS << (Standard_Byte)LINE;
  gp_Lin2d C2d = L->Lin2d();
  OS << C2d.Location();//Pnt2d
  OS << C2d.Direction();//Dir2d
  return OS;
}

//=======================================================================
//function : operator << ((Geom2d_Circle)& C)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom2d_Circle)& C)
{
  OS << (Standard_Byte)CIRCLE;
  gp_Circ2d C2d = C->Circ2d();
  OS << C2d.Location();
  OS << C2d.XAxis().Direction();
  OS << C2d.YAxis().Direction();
  BinTools::PutReal(OS, C2d.Radius());
  return OS;
}

//=======================================================================
//function : operator <<(Geom2d_Ellipse)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom2d_Ellipse)& E)
{
  OS << (Standard_Byte)ELLIPSE;
  gp_Elips2d C2d = E->Elips2d();
  OS << C2d.Location();
  OS << C2d.XAxis().Direction();
  OS << C2d.YAxis().Direction();
  BinTools::PutReal(OS, C2d.MajorRadius());
  BinTools::PutReal(OS, C2d.MinorRadius());
  return OS;
}

//=======================================================================
//function :  operator << (Geom2d_Parabola)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom2d_Parabola)& P)
{
  OS << (Standard_Byte)PARABOLA;
  gp_Parab2d C2d = P->Parab2d();
  OS << C2d.Location();//Loc
  OS << C2d.Axis().XAxis().Direction();//XDir
  OS << C2d.Axis().YAxis().Direction();//YDir
  BinTools::PutReal(OS, C2d.Focal());//Focal
  return OS;
}

//=======================================================================
//function : operator <<(Geom2d_Hyperbola)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom2d_Hyperbola)& H)
{
  OS << (Standard_Byte)HYPERBOLA;
  gp_Hypr2d C2d = H->Hypr2d();
  OS << C2d.Location(); //Loc
  OS << C2d.XAxis().Direction();//XDir
  OS << C2d.YAxis().Direction();//YDir
  BinTools::PutReal(OS, C2d.MajorRadius());//MajR
  BinTools::PutReal(OS, C2d.MinorRadius());
  return OS;
}

//=======================================================================
//function : operator <<(Geom2d_BezierCurve)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom2d_BezierCurve)& B)
{
  OS << (Standard_Byte)BEZIER;
  Standard_Boolean aRational = B->IsRational() ? 1:0;
  BinTools::PutBool(OS, aRational); //rational
  // poles and weights
  Standard_Integer i,aDegree = B->Degree(); 
  BinTools::PutExtChar(OS, (Standard_ExtCharacter)aDegree); //Degree
  for (i = 1; i <= aDegree+1; i++) {
    OS << B->Pole(i); //Pnt2d
    if (aRational)
      BinTools::PutReal(OS, B->Weight(i));//Real
  }
  return OS;
}

//=======================================================================
//function : operator <<(Geom2d_BSplineCurve)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom2d_BSplineCurve)& B)
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
  BinTools::PutInteger(OS,  aNbPoles);
  BinTools::PutInteger(OS,  aNbKnots);
  for (i = 1; i <= aNbPoles; i++) {
    OS << B->Pole(i); // Pnt2d
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
//function : operator <<(Geom2d_TrimmedCurve)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom2d_TrimmedCurve)& C)
{
  OS << (Standard_Byte)TRIMMED;
  BinTools::PutReal(OS, C->FirstParameter()); 
  BinTools::PutReal(OS, C->LastParameter());
  BinTools_Curve2dSet::WriteCurve2d(C->BasisCurve(),OS);
  return OS;
}

//=======================================================================
//function : operator <<(Geom2d_OffsetCurve)
//purpose  : 
//=======================================================================

static Standard_OStream& operator <<(Standard_OStream& OS, const Handle(Geom2d_OffsetCurve)& C)
{
  OS << (Standard_Byte)OFFSET;
  BinTools::PutReal(OS,C->Offset());//Offset 
  BinTools_Curve2dSet::WriteCurve2d(C->BasisCurve(),OS);
  return OS;  
}

//=======================================================================
//function : WriteCurve2d
//purpose  : 
//=======================================================================

void BinTools_Curve2dSet::WriteCurve2d(const Handle(Geom2d_Curve)& C,
					Standard_OStream& OS)
{
  Handle(Standard_Type) TheType = C->DynamicType();
  try {
    OCC_CATCH_SIGNALS
    if ( TheType ==  STANDARD_TYPE(Geom2d_Circle)) {
      OS << Handle(Geom2d_Circle)::DownCast(C);
    }
    else if ( TheType ==STANDARD_TYPE(Geom2d_Line)) {
      OS << Handle(Geom2d_Line)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_Ellipse)) {
      OS << Handle(Geom2d_Ellipse)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_Parabola)) {
      OS << Handle(Geom2d_Parabola)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_Hyperbola)) {
      OS << Handle(Geom2d_Hyperbola)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_BezierCurve)) {
      OS << Handle(Geom2d_BezierCurve)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_BSplineCurve)) {
      OS << Handle(Geom2d_BSplineCurve)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
      OS << Handle(Geom2d_TrimmedCurve)::DownCast(C);
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_OffsetCurve)) {
      OS << Handle(Geom2d_OffsetCurve)::DownCast(C);
    }
    else {
      throw Standard_Failure("UNKNOWN CURVE2d TYPE");
    }
  }
  catch(Standard_Failure const& anException) {
    Standard_SStream aMsg;
    aMsg << "EXCEPTION in BinTools_Curve2dSet::WriteCurve2d(..)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }  
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  BinTools_Curve2dSet::Write(Standard_OStream& OS)const 
{
  Standard_Integer i, aNbCurves = myMap.Extent();
  OS << "Curve2ds "<< aNbCurves << "\n";
  for (i = 1; i <= aNbCurves; i++) {
    WriteCurve2d(Handle(Geom2d_Curve)::DownCast(myMap(i)),OS);
  }
}


//=======================================================================
//function : ReadPnt2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Pnt2d& P)
{
  Standard_Real X=0.,Y=0.;
  BinTools::GetReal(IS, X);
  BinTools::GetReal(IS, Y);
  P.SetCoord(X,Y);
  return IS;
}

//=======================================================================
//function : ReadDir2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS, gp_Dir2d& D)
{
  Standard_Real X=0.,Y=0.;
  BinTools::GetReal(IS, X);
  BinTools::GetReal(IS, Y);
  D.SetCoord(X,Y);
  return IS;
}


//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Line)& L)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.);
  IS >> P >> AX;
  L = new Geom2d_Line(P,AX);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================
static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Circle)& C)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.),AY(1.,0.);
  Standard_Real R=0.;
  IS >> P >> AX >> AY;
  BinTools::GetReal(IS, R);
  C = new Geom2d_Circle(gp_Ax22d(P,AX,AY),R);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Ellipse)& E)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.),AY(1.,0.);
  Standard_Real R1=0., R2=0.;
  IS >> P >> AX >> AY;
  BinTools::GetReal(IS, R1);
  BinTools::GetReal(IS, R2);
  E = new Geom2d_Ellipse(gp_Ax22d(P,AX,AY),R1,R2);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Parabola)& C)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.),AY(1.,0.);
  Standard_Real R1=0.;
  IS >> P >> AX >> AY;
  BinTools::GetReal(IS, R1);
  C = new Geom2d_Parabola(gp_Ax22d(P,AX,AY),R1);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_Hyperbola)& H)
{
  gp_Pnt2d P(0.,0.);
  gp_Dir2d AX(1.,0.),AY(1.,0.);
  Standard_Real R1=0.,R2=0.;
  IS >> P >> AX >> AY;
  BinTools::GetReal(IS, R1);
  BinTools::GetReal(IS, R2);
  H = new Geom2d_Hyperbola(gp_Ax22d(P,AX,AY),R1,R2);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_BezierCurve)& B)
{
  Standard_Boolean rational=Standard_False;
  BinTools::GetBool(IS, rational);

// poles and weights
  Standard_Integer i=0,degree=0;
// degree;
  Standard_ExtCharacter aVal='\0';
  BinTools::GetExtChar(IS, aVal);
  degree = (Standard_Integer)aVal;

  TColgp_Array1OfPnt2d poles(1,degree+1);
  TColStd_Array1OfReal weights(1,degree+1);
  
  for (i = 1; i <= degree+1; i++) {
    IS >> poles(i);//Pnt2d
    if (rational)
      BinTools::GetReal(IS, weights(i));
  }

  if (rational)
    B = new Geom2d_BezierCurve(poles,weights);
  else
    B = new Geom2d_BezierCurve(poles);

  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_BSplineCurve)& B)
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

  TColgp_Array1OfPnt2d poles(1,nbpoles);
  TColStd_Array1OfReal weights(1,nbpoles);
  
  for (i = 1; i <= nbpoles; i++) {
    IS >> poles(i);//Pnt2d
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
    B = new Geom2d_BSplineCurve(poles,weights,knots,mults,degree,periodic);
  else
    B = new Geom2d_BSplineCurve(poles,knots,mults,degree,periodic);
  
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_TrimmedCurve)& C)
{
  Standard_Real p1=0.,p2=0.;
  BinTools::GetReal(IS, p1);//FirstParameter
  BinTools::GetReal(IS, p2);//LastParameter
  Handle(Geom2d_Curve) BC;
  BinTools_Curve2dSet::ReadCurve2d(IS,BC);
  C = new Geom2d_TrimmedCurve(BC,p1,p2);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

static Standard_IStream& operator>>(Standard_IStream& IS,
				    Handle(Geom2d_OffsetCurve)& C)
{
  Standard_Real p=0.;
  BinTools::GetReal(IS, p);//Offset
  Handle(Geom2d_Curve) BC;
  BinTools_Curve2dSet::ReadCurve2d(IS,BC);
  C = new Geom2d_OffsetCurve(BC,p);
  return IS;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

Standard_IStream& BinTools_Curve2dSet::ReadCurve2d(Standard_IStream& IS,
						    Handle(Geom2d_Curve)& C)
{
  try {
    OCC_CATCH_SIGNALS
    const Standard_Byte ctype = (Standard_Byte) IS.get();
    switch (ctype) {

    case LINE :
      {
        Handle(Geom2d_Line) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case CIRCLE :
      {
        Handle(Geom2d_Circle) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case ELLIPSE :
      {
        Handle(Geom2d_Ellipse) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case PARABOLA :
      {
        Handle(Geom2d_Parabola) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case HYPERBOLA :
      {
        Handle(Geom2d_Hyperbola) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case BEZIER :
      {
        Handle(Geom2d_BezierCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case BSPLINE :
      {
        Handle(Geom2d_BSplineCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case TRIMMED :
      {
        Handle(Geom2d_TrimmedCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;

    case OFFSET :
      {
        Handle(Geom2d_OffsetCurve) CC;
        IS >> CC;
        C = CC;
      }
      break;
      
    default:
      {
	C = NULL;
        throw Standard_Failure("UNKNOWN CURVE2d TYPE");
      }
      break;
    }
  }
  catch(Standard_Failure const& anException) {
    C = NULL;
    Standard_SStream aMsg;
    aMsg <<"EXCEPTION in BinTools_Curve2dSet::ReadCurve2d(...)" << endl;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  return IS;
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void  BinTools_Curve2dSet::Read(Standard_IStream& IS)
{
  char buffer[255];

  IS >> buffer;
  if (IS.fail() || strcmp(buffer,"Curve2ds")) {
    Standard_SStream aMsg;
    aMsg << "BinTools_Curve2dSet::Read:  Not a Curve2d table"<<endl;
#ifdef OCCT_DEBUG
    cout <<"Curve2dSet buffer: " << buffer << endl;
#endif
    throw Standard_Failure(aMsg.str().c_str());
    return;
  }

  Handle(Geom2d_Curve) C;
  Standard_Integer i, aNbCurves;
  IS >> aNbCurves;
  IS.get();//remove <lf>
  for (i = 1; i <= aNbCurves; i++) {
    BinTools_Curve2dSet::ReadCurve2d(IS,C);
    myMap.Add(C);
  }
}



