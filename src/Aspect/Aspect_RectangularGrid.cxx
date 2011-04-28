// Modified	23/02/98 : FMN ; Remplacement PI par Standard_PI

#define CSR577	//GG 25/09/00 Avoid to have unaccuracy coordinates computation
//		when the grid is activated.

#define xTRACE

#include <Aspect_RectangularGrid.ixx>


Aspect_RectangularGrid::Aspect_RectangularGrid(
                               const Quantity_Length aXStep,
                               const Quantity_Length aYStep,
                               const Quantity_Length anXOrigin,
                               const Quantity_Length anYOrigin,
                               const Quantity_PlaneAngle aFirstAngle,
                               const Quantity_PlaneAngle aSecondAngle,
                               const Quantity_PlaneAngle aRotationAngle)
:Aspect_Grid(anXOrigin,anYOrigin,aRotationAngle),myXStep(aXStep),myYStep(aYStep),myFirstAngle(aFirstAngle),mySecondAngle(aSecondAngle)

{
  Standard_NumericError_Raise_if(!CheckAngle (aFirstAngle,mySecondAngle),
                                 "networks are parallel");

  Standard_NegativeValue_Raise_if(aXStep < 0. , "invalid x step");
  Standard_NegativeValue_Raise_if(aYStep < 0. , "invalid y step");
  Standard_NullValue_Raise_if(aXStep == 0. , "invalid x step");
  Standard_NullValue_Raise_if(aYStep == 0. , "invalid y step");
}



void Aspect_RectangularGrid::SetXStep(const Quantity_Length aStep) {
  Standard_NegativeValue_Raise_if(aStep < 0. , "invalid x step");
  Standard_NullValue_Raise_if(aStep == 0. , "invalid y step");
  myXStep = aStep;
  Init();
  UpdateDisplay();
}

void Aspect_RectangularGrid::SetYStep(const Quantity_Length aStep) {
  Standard_NegativeValue_Raise_if(aStep < 0. , "invalid x step");
  Standard_NullValue_Raise_if(aStep == 0. , "invalid y step");
  myYStep = aStep;
  Init();
  UpdateDisplay();
}

void Aspect_RectangularGrid::SetAngle(const Quantity_PlaneAngle anAngle1,
                                            const Quantity_PlaneAngle anAngle2){

  Standard_NumericError_Raise_if(!CheckAngle (anAngle1,anAngle2 ),
                                 "axis are parallel");
  myFirstAngle = anAngle1;
  mySecondAngle = anAngle2;
  Init();
  UpdateDisplay();
}

void Aspect_RectangularGrid::SetGridValues(
	const Quantity_Length theXOrigin,
	const Quantity_Length theYOrigin,
	const Quantity_Length theXStep,
	const Quantity_Length theYStep,
	const Quantity_PlaneAngle theRotationAngle) {

  myXOrigin = theXOrigin;
  myYOrigin = theYOrigin;
  Standard_NegativeValue_Raise_if(theXStep < 0. , "invalid x step");
  Standard_NullValue_Raise_if(theXStep == 0. , "invalid x step");
  myXStep = theXStep;
  Standard_NegativeValue_Raise_if(theYStep < 0. , "invalid y step");
  Standard_NullValue_Raise_if(theYStep == 0. , "invalid y step");
  myYStep = theYStep;
  myRotationAngle = theRotationAngle;
  Init();
  UpdateDisplay();
}

void Aspect_RectangularGrid::Compute(const Quantity_Length X,
                         const Quantity_Length Y,
                         Quantity_Length& gridX,
                         Quantity_Length& gridY) const {
    Standard_Real D1 = b1 * X - a1 * Y - c1;
    Standard_Real D2 = b2 * X - a2 * Y - c2;
    Standard_Integer n1 = Standard_Integer ( Abs(D1)/myXStep + 0.5);
    Standard_Integer n2 = Standard_Integer ( Abs(D2)/myYStep + 0.5);
    Standard_Real offset1 = c1 + Standard_Real(n1) * Sign (myXStep , D1);
    Standard_Real offset2 = c2 + Standard_Real(n2) * Sign (myYStep , D2);
#ifdef CSR577
    Standard_Real Delta = a1*b2 - b1*a2;
    gridX = ( offset2*a1 - offset1*a2) /Delta;
    gridY = ( offset2*b1 - offset1*b2) /Delta;
#else
    Standard_Real Delta = b1*a2 - a1*b2;
    gridX = ( offset1*a2 - offset2*a1) /Delta;
    gridY = ( offset1*b2 - offset2*b1) /Delta;
#endif
#ifdef TRACE
    cout << "Aspect_RectangularGrid::Compute (" << Quantity_Length (X) << ", "
         << Quantity_Length (Y) << ", " << Quantity_Length (gridX) << ", "
         << Quantity_Length (gridY) << ")" << endl;
#endif
}

Quantity_Length Aspect_RectangularGrid::XStep() const {
  return myXStep;
}

Quantity_Length Aspect_RectangularGrid::YStep() const {
  return myYStep;
}

Quantity_Length Aspect_RectangularGrid::FirstAngle() const {
  return myFirstAngle;
}

Quantity_Length Aspect_RectangularGrid::SecondAngle() const {
  return mySecondAngle;
}

void Aspect_RectangularGrid::Init () {

//+zov Fixing CTS17856
//  a1 = Cos (myFirstAngle + RotationAngle() ); 
//  b1 = Sin (myFirstAngle + RotationAngle() );
//  c1 = XOrigin() * b1 - YOrigin() * a1;
//
//  a2 = Cos (mySecondAngle + RotationAngle() + Standard_PI/2.); 
//  b2 = Sin (mySecondAngle + RotationAngle() + Standard_PI/2.);
//  c2 = XOrigin() * b2 - YOrigin() * a2;

#ifdef CSR577
  Standard_Real angle1 = myFirstAngle + RotationAngle();
  Standard_Real angle2 = mySecondAngle + RotationAngle();
  if ( angle1 != 0. ) {
    a1 = -Sin (angle1); 
    b1 = Cos (angle1);
    c1 = XOrigin() * b1 - YOrigin() * a1;
  } else {
    a1 = 0.; b1 = 1.; c1 = XOrigin();
  }

  if ( angle2 != 0. ) {
    angle2 += Standard_PI/2.;
    a2 = -Sin (angle2); 
    b2 = Cos (angle2);
    c2 = XOrigin() * b2 - YOrigin() * a2;
  } else {
    a2 = -1.; b2 = 0.; c2 = YOrigin();
  }
#else
  a1 = -Sin (myFirstAngle + RotationAngle()); 
  b1 = Cos (myFirstAngle + RotationAngle());
  c1 = XOrigin() * b1 - YOrigin() * a1;

  a2 = -Sin (mySecondAngle + RotationAngle() + Standard_PI/2.); 
  b2 = Cos (mySecondAngle + RotationAngle() + Standard_PI/2.);
  c2 = XOrigin() * b2 - YOrigin() * a2;
#endif
//-zov
}

Standard_Boolean Aspect_RectangularGrid::CheckAngle(const Standard_Real alpha,
                                            const Standard_Real beta) const {
  return (Abs( Sin(alpha) * Cos(beta+Standard_PI/2.) - Cos(alpha) * Sin(beta+Standard_PI/2.)) != 0) ;
}


