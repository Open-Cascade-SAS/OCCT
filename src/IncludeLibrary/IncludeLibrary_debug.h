
#ifdef UNX
#define DEBUG           0   /* false */
#elif defined(WNT)
#define DEBUG           0   /* false */
#else
#define DEBUG           0   /* false */
#endif

#if DEBUG                    /* AVEC DEBUG */

#ifdef CSF1
#define DO_MALLOC 0          /* True : Pas de malloc(libc) */
#define RPC_TRACE 0          /* True : Trace dans Serpc */
#define NTD_TRACE 0
#define OSD_TRACE 0
#define ACS_TRACE 0
#define FRONT_TRACE 0

#else                        /* Euclid */
#ifdef UNX
#define TKBD_TRACE 0         /* false */
#define NTD_TRACE 0
#define OSD_TRACE 0
#define RPC_TRACE 0
#define ACS_TRACE 0
#else
#define TKBD_TRACE 0         /* false */
#define NTD_TRACE 0
#define OSD_TRACE 0
#define RPC_TRACE 0
#define ACS_TRACE 0
#endif

#endif

#else                        /* Pas debug */
#ifdef CSF1
#define DO_MALLOC 0          /* True : Pas de malloc(libc) */
#define RPC_TRACE 0
#define NTD_TRACE 0
#define OSD_TRACE 0
#define ACS_TRACE 0

#else                        /* Euclid sans debug */
#define TKBD_TRACE 0
#define RPC_TRACE 0
#define NTD_TRACE 0
#define OSD_TRACE 0
#define ACS_TRACE 0
#endif

#endif

