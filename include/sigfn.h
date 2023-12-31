/*
** Copyright 2023 John R. Patek Sr.
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

#ifndef SIGFN_H
#define SIGFN_H

/**
 * @file sigfn.h
 * @brief SigFn header file
 */

#define SIGFN_SUCCESS 0
#define SIGFN_INVALID_SIGNUM 1
#define SIGFN_INVALID_HANDLER 2

#include <stdlib.h>
#include <signal.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief signal handler function type
     *
     * @param signum signal number
     * @param userdata pointer to user defined data, can be NULL
     */
    typedef void (*sigfn_handler_func)(int signum, void *userdata);

    /**
     * @brief attach handler to specific signal
     *
     * @param signum signal to be handled
     * @param handler function associated with this signal
     * @param userdata optional user data to be passed to the function
     */
    int sigfn_handle(int signum, sigfn_handler_func handler, void *userdata);

    /**
     * @brief ignore a specific signal
     *
     * @param signum signal to be ignored
     */
    int sigfn_ignore(int signum);

    /**
     * @brief reset a specific signal to its default behavior
     *
     * @param signum signal to be reset
     */
    int sigfn_reset(int signum);

#ifdef __cplusplus
}
#endif

#endif