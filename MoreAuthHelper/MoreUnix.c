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

// When we pass a descriptor, we have to pass at least one byte
// of data along with it, otherwise the recvmsg call will not
// block if the descriptor hasn't been written to the other end
// of the socket yet.

static const char kDummyData = 'D';


extern int MoreUNIXWriteDescriptor(int fd, int fdToWrite)
    // See comment in header.
{
    int 				err;
    struct msghdr 		msg;
    struct iovec		iov;
    struct {
        struct cmsghdr 	hdr;
        int            	fd;
    } 					control;
    ssize_t 			bytesSent;

    assert(fd >= 0);
    assert(fdToWrite >= 0);

    control.hdr.cmsg_len   = sizeof(control);
    control.hdr.cmsg_level = SOL_SOCKET;
    control.hdr.cmsg_type  = SCM_RIGHTS;
    control.fd             = fdToWrite;

    iov.iov_base = (char *) &kDummyData;
    iov.iov_len  = sizeof(kDummyData);

    msg.msg_name       = NULL;
    msg.msg_namelen    = 0;
    msg.msg_iov        = &iov;
    msg.msg_iovlen     = 1;
    msg.msg_control    = (caddr_t) &control;
    msg.msg_controllen = control.hdr.cmsg_len;
    msg.msg_flags	   = 0;
    do {
        bytesSent = sendmsg(fd, &msg, 0);
        if (bytesSent == sizeof(kDummyData)) {
            err = 0;
        } else {
            assert(bytesSent == -1);

            err = errno;
            assert(err != 0);
        }
    } while (err == EINTR);

    return err;
}

extern int MoreUNIXCopyDescriptorToDescriptor(int source, int dest)
    // See comment in header.
{
    int		err;
    bool    done;
    char *  buffer;
    static const size_t kBufferSize = 64 * 1024;

    assert(source >= 0);
    assert(dest   >= 0);

    err = 0;
    buffer = (char *) malloc(kBufferSize);
    if (buffer == NULL) {
        err = ENOMEM;
    }

    if (err == 0) {
        done = false;
        do {
            size_t bytesRead;

            err = MoreUNIXRead(source, buffer, kBufferSize, &bytesRead);
            if (err == EPIPE) {
                done = true;
                err = 0;
            }
            if (err == 0) {
                err = MoreUNIXWrite(dest, buffer, bytesRead, NULL);
            }
        } while (err == 0 && !done);
    }

    free(buffer);

    return err;
}

extern int MoreUNIXReadDescriptor(int fd, int *fdRead)
    // See comment in header.
{
    int 				err;
    struct msghdr 		msg;
    struct iovec		iov;
    struct {
        struct cmsghdr 	hdr;
        int            	fd;
    } 					control;
    char				dummyData;
    ssize_t				bytesReceived;

    assert(fd >= 0);
    assert( fdRead != NULL);
    assert(*fdRead == -1);

    iov.iov_base = (char *) &dummyData;
    iov.iov_len  = sizeof(dummyData);

    msg.msg_name       = NULL;
    msg.msg_namelen    = 0;
    msg.msg_iov        = &iov;
    msg.msg_iovlen     = 1;
    msg.msg_control    = (caddr_t) &control;
    msg.msg_controllen = sizeof(control);
    msg.msg_flags	   = MSG_WAITALL;

    do {
        bytesReceived = recvmsg(fd, &msg, 0);
        if (bytesReceived == sizeof(dummyData)) {
            if (   (dummyData != kDummyData)
                   || (msg.msg_flags != 0)
                   || (msg.msg_control == NULL)
                   || (msg.msg_controllen != sizeof(control))
                   || (control.hdr.cmsg_len != sizeof(control))
                   || (control.hdr.cmsg_level != SOL_SOCKET)
                   || (control.hdr.cmsg_type  != SCM_RIGHTS)
                   || (control.fd < 0) ) {
                err = EINVAL;
            } else {
                *fdRead = control.fd;
                err = 0;
            }
        } else if (bytesReceived == 0) {
            err = EPIPE;
        } else {
            assert(bytesReceived == -1);

            err = errno;
            assert(err != 0);
        }
    } while (err == EINTR);

    assert( (err == 0) == (*fdRead >= 0) );

    return err;
}

#endif
