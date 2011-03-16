#include <AIS2D_PrimitiveArchit.ixx>

AIS2D_PrimitiveArchit::AIS2D_PrimitiveArchit( const Handle(Graphic2d_Primitive)& aPrim,
                                              const Standard_Integer ind ) : 
  myPrimitive( aPrim ),
  myInd( ind )
{
}

Handle(Graphic2d_Primitive) AIS2D_PrimitiveArchit::GetPrimitive() const {
   return myPrimitive;
}

Standard_Integer AIS2D_PrimitiveArchit::GetIndex() const {
   return myInd;
}
