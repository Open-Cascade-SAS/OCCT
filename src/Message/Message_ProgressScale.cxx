#include <Message_ProgressScale.ixx>

static const Standard_Real ZERO = 1e-10;
static const Standard_Real INFINITE = 1e100;
  
//=======================================================================
//function : Message_ProgressScale
//purpose  : 
//=======================================================================

Message_ProgressScale::Message_ProgressScale () :
     myMin(0.), myMax(100.), myStep(1.), myInfinite(Standard_False),
     myFirst(0.), myLast(1.)
{
}

//=======================================================================
//function : LocalToBase
//purpose  : 
//=======================================================================

Standard_Real Message_ProgressScale::LocalToBase (const Standard_Real val) const
{
  if ( val <= myMin ) return myFirst;
  if ( myMax - myMin <= ZERO ) return myLast;
  
  if ( ! myInfinite ) {
    if ( val >= myMax ) return myLast;
    return myFirst + ( myLast - myFirst ) * ( val - myMin ) / ( myMax - myMin );
  }
  Standard_Real x = ( val - myMin ) / ( myMax - myMin );
//  return myFirst + ( myLast - myFirst ) * ( 1. - exp ( -x ) ); // exponent
  return myFirst + ( myLast - myFirst ) * x / ( 1. + x );     // hyperbola
}

//=======================================================================
//function : BaseToLocal
//purpose  : 
//=======================================================================

Standard_Real Message_ProgressScale::BaseToLocal (const Standard_Real val) const
{
  if ( myLast - val <= ZERO ) 
    return myInfinite ? INFINITE : myMax;
  if ( ! myInfinite )
    return myMin + ( myMax - myMin ) * ( val - myFirst ) / ( myLast - myFirst );
//  Standard_Real x = log ( ( val - myFirst ) / ( myLast - val ) ); // exponent
  Standard_Real x = ( val - myFirst ) / ( myLast - val );         // hyperbola
  return myMin + x * ( myMax - myMin );
}
