#include <Standard_NullObject.hxx>

#include <AlienImage_MemoryOperations.ixx>


void AlienImage_MemoryOperations::SwapLong( const Standard_Address Data,
					    const Standard_Integer Size )

{
    register char c;
    register char *bp = ( char * )Data ;
    register char *ep = ( char * )Data + Size ;
    register char *sp;

    if ( Data == NULL)
	Standard_NullObject::Raise("AlienImage_MemoryOperations : SwapLong");

    while (bp < ep) {
	sp = bp + 3;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	sp = bp + 1;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	bp += 2;
    }
}

void AlienImage_MemoryOperations::SwapShort( const Standard_Address Data,
					     const Standard_Integer Size )

{
    register char c;
    register char *ep = ( char * )Data + Size ;
    register char *bp = ( char * )Data ;

    if ( Data == NULL)
	Standard_NullObject::Raise("AlienImage_MemoryOperations : SwapLong");

    while (bp < ep) {
	c = *bp;
	*bp = *(bp + 1);
	bp++;
	*bp++ = c;
    }
}

 
