/* Returns a unique number according to the string passed as 1fst argument */

int osd_getkey(char *name)
{
    int   ii = 0;
    register  char *pp;

        pp = name;
        while( *pp ) ii = ii << 1 ^ *pp++;
        if( ii < 0 ) ii = -ii;

    return ii;
}
