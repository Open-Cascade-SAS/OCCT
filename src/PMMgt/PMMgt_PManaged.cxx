#include <PMMgt_PManaged.ixx>
#include <Standard_TypeMismatch.hxx>

#ifdef TRACE
#define  PRINT(X) cout X
#define  SHALLOWDUMP(X) X.ShallowDump(cout)
#else
#define  PRINT(X) 
#define  SHALLOWDUMP(X)
#endif

PMMgt_PManaged::PMMgt_PManaged()
{
}

//============================================================================
#ifndef CSFDB
class _Oid_PMMgt_PManaged : public Oid_PMMgt_PManaged {
    friend class Handle_PMMgt_PManaged;
};

void Handle_PMMgt_PManaged::Delete()
{
//  PMMgt_PManaged* aMng;
  _Oid_PMMgt_PManaged *aOid;
  aOid = (_Oid_PMMgt_PManaged *) operator->();

  Standard_TypeMismatch_Raise_if(!aOid->IsKind(STANDARD_TYPE(PMMgt_PManaged)),
				 "Handle(PMMgt_PManaged)::TypeMisPMatch");

//  aMng = (PMMgt_PManaged*) aOid->UpdateAccess();
  aOid->UpdateAccess();
  aOid->EndAccess();

#ifdef OBJY
  //==== Remove the "Oid" from MDTV Cache ===================================
  // void CacheRemove(const Standard_Persistent*);
  // CacheRemove(aMng);
#endif 

  Nullify();
//  delete aMng;
}
#endif
#ifdef OBJY
void PHandle_PMMgt_PManaged::Delete()
{
}

void PMMgt_PManaged::operator delete (void *, size_t)
{
}
#endif
