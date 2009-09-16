#include <stdlib.h>

/////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// Macros that act like functions to convert OSStatus error codes to errno-style 
// error codes, and vice versa.  Right now these are just pass throughs because 
// OSStatus errors are 32 bit signed values that are generally negative, and 
// errno errors are 32 bit signed values that are small positive.

#define OSStatusToEXXX(os) ((int) (os))
#define EXXXToOSStatus(ex) ((OSStatus) (ex))

// A mechanism to extra errno if a function fails.  You typically use this as 
//
//   fd = open(...);
//   err = MoreUNIXErrno(fd);
//
// or
//
//   err = setuid(0);
//   err = MoreUNIXErrno(err);

#if MORE_DEBUG

extern int MoreUNIXErrno(int result);

#else

#define MoreUNIXErrno(err) (((err) < 0) ? errno : 0)

#endif

#ifdef __cplusplus
}
#endif
