#include "MoreUnix.h"

#if MORE_DEBUG

// There's a macro version of this in the header.

extern int MoreUNIXErrno(int result)
{
    int err;

    err = 0;
    if (result < 0) {
        err = errno;
        assert(err != 0);
    }
    return err;
}

extern int MoreUNIXIgnoreSIGPIPE(void)
    // See comment in header.
{
    int err;
    struct sigaction signalState;

    err = sigaction(SIGPIPE, NULL, &signalState);
    err = MoreUNIXErrno(err);
    if (err == 0) {
        signalState.sa_handler = SIG_IGN;

        err = sigaction(SIGPIPE, &signalState, NULL);
        err = MoreUNIXErrno(err);
    }

    return err;
}

#endif
