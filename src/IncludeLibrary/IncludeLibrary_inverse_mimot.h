#define inverse_mimot(a)\
(0 | (( a & 0x000000ff ) << 8  )\
|    (( a & 0x0000ff00 ) >> 8  )\
|    (( a & 0x00ff0000 ) << 8  )\
|    (( a & 0xff000000 ) >> 8  ) )

