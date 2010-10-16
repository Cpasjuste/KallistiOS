/* KallistiOS ##version##
 
   include/kos/recursive_lock.h
   Copyright (C) 2008, 2010 Lawrence Sebald
 
*/

/** \file   kos/recursive_lock.h
    \brief  Definitions for a recursive mutex.

    This file defines a recursive lock mechanism, similar to a mutex, but that a
    single thread can obtain as many times as it wants. A single thread is still
    limited to holding the lock at a time, but it can hold it an "unlimited"
    number of times (actually limited to INT_MAX, but who's counting).

    \author Lawrence Sebald
*/

#ifndef __KOS_RECURSIVE_LOCK_H
#define __KOS_RECURSIVE_LOCK_H

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <sys/queue.h>
#include <kos/thread.h>

/** \brief  Recursive lock structure.

    All members of this structure should be considered to be private, it is not
    safe to change anything in here yourself.

    \headerfile kos/recursive_lock.h
*/
typedef struct recursive_lock {
    /** \brief  The thread that currently holds the lock, if any. */
    kthread_t *holder;

    /** \brief  The number of times the holding thread has locked this lock. */
    int count;
} recursive_lock_t;

/** \brief  Allocate a new recursive lock.

    This function allocates a new recurisve lock that is initially not locked.

    \return The created lock, or NULL on failure (errno will be set to ENOMEM to
            indicate that the system appears to be out of memory).
*/
recursive_lock_t *rlock_create();

/** \brief  Destroy a recursive lock.

    This function cleans up a recursive lock. It is an error to attempt to
    destroy a locked recursive lock.

    \param  l       The recursive lock to destroy. It must be unlocked.
*/
void rlock_destroy(recursive_lock_t *l);

/** \brief  Lock a recursive lock.

    This function attempts to lock the requested lock, and if it cannot it will
    block until that is possible.

    \param  l       The recursive lock to lock.
    \retval -1      On error, errno will be set to EPERM if this function is
                    called inside an interrupt, or EINTR if it is interrupted.
    \retval 0       On success.
    \sa rlock_trylock
    \sa rlock_lock_timed
*/
int rlock_lock(recursive_lock_t *l);

/** \brief  Lock a recursive lock (with a timeout).

    This function attempts to lock the requested lock, and if it cannot it will
    block until either it is possible to acquire the lock or timeout
    milliseconds have elapsed.

    \param  l       The recursive lock to lock.
    \param  timeout The maximum number of milliseconds to wait. 0 is an
                    unlimited timeout (equivalent to rlock_lock).
    \retval -1      On error, errno will be set to EPERM if this function is
                    called inside an interrupt, EINTR if the function is
                    interrupted, or EAGAIN if the timeout expires.
    \retval 0       On success.
    \sa rlock_trylock
    \sa rlock_lock_timed
*/
int rlock_lock_timed(recursive_lock_t *l, int timeout);

/** \brief  Unlock a recursive lock.

    This function releases the lock one time from the current thread.

    \param  l       The recursive lock to unlock.
    \retval -1      On error, errno will be set to EPERM if the lock is not held
                    by the calling thread.
    \retval 0       On success.
*/
int rlock_unlock(recursive_lock_t *l);

/** \brief  Attempt to lock a recursive lock without blocking.

    This function attempts to lock a recursive lock without blocking. This
    function, unlike rlock_lock and rlock_lock_timed is safe to call inside an
    interrupt.

    \param  l       The recursive lock to lock.
    \retval -1      On error, errno will be set to EWOULDBLOCK if the lock is
                    currently held by another thread.
    \retval 0       On success.
    \sa rlock_lock
    \sa rlock_lock_timed
*/
int rlock_trylock(recursive_lock_t *l);

/** \brief  Check if a recursive lock is currently held by any thread.

    This function checks whether or not a lock is currently held by any thread,
    including the calling thread. Note that this is <b>NOT</b> a safe way to
    check if a lock <em>will</em> be held by the time you get around to locking
    it.

    \retval TRUE    If the lock is held by any thread.
    \retval FALSE   If the lock is not currently held by any thread.
*/
int rlock_is_locked(recursive_lock_t *l);

/* Init / shutdown */
/** \cond */
int rlock_init();
void rlock_shutdown();
/** \endcond */

__END_DECLS

#endif /* !__KOS_RECURSIVE_LOCK_H */