/* ============================================================
 * Control compile time options.
 * ============================================================
 *
 * Compile time options have moved to config.mk.
 */


/* ============================================================
 * Compatibility defines
 *
 * Generally for Windows native support.
 * ============================================================ */
#ifdef WIN32
#define snprintf sprintf_s
#  ifndef strcasecmp
#    define strcasecmp strcmpi
#  endif
#define strtok_r strtok_s
#define strerror_r(e, b, l) strerror_s(b, l, e)
#endif


#define uthash_malloc(sz) _mosquitto_malloc(sz)
#define uthash_free(ptr,sz) _mosquitto_free(ptr)

#ifndef EPROTO
#  define EPROTO ECONNABORTED
#endif
