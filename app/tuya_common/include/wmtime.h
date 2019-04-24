/*! \file wmtime.h
 *  \brief Time Management Subsystem
 *
 * This module provides the time managment APIs. Internally wireless
 * microcontroller manages time in terms of seconds since epoch (1 Jan 1970
 * 00:00). This representation is known as "posix time". These routines can
 * be used to do conversions between human readable time and posix time as well
 * as to set the system time to desired value.
 *
 */

/*
 *   Copyright 2008-2013, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef _WMTIME_H_
#define _WMTIME_H_

#include "sys_adapter.h"

typedef uint32_t time_t;
typedef long suseconds_t;

/** Abridged version of struct tm (refer 'man gmtime' found in standard C
    library */
struct tm {
	/** seconds [0-59] */
	int tm_sec;
	/** minutes [0-59] */
	int tm_min;
	/** hours [0-23] */
	int tm_hour;
	/** day of the month [1-31] */
	int tm_mday;
	/** month [0-11] */
	int tm_mon;
	/** year. The number of years since 1900 */
	int tm_year;
	/** day of the week [0-6] 0-Sunday...6-Saturday */
	int tm_wday;
};


/** Convert HTTP date format to POSIX time format
 *
 * \param[in] date HTTP date format
 *
 * \return success or failure as:
 *     -WM_FAIL: Conversion failed. Invalid format/data
 *     else valid time_t value
 */
time_t http_date_to_time(const char *date);

/** Set the date and time
 *
 * \param[in] tm The rtc value is updated with the values in tm structure
 * \return success or failure as:
 *     0: Success
 *     -1: Failed validation of tm structure
 */
extern int wmtime_time_set(const struct tm *tm);

/**
 * Get date and time
 *
 * \param[out] tm tm structure is updated to get the current value in rtc
 * \return success or failure as:
 *     0: Success
 *     non-zero: Internal error
 */
extern int wmtime_time_get(struct tm *tm);

/** Set the date and time using posix time
*
*  \param[in] time The rtc value is updated with the value present in time
*  \return success or failure as:
*      0: Success
*      non-zero: Internal error
*/
extern int wmtime_time_set_posix(time_t time);

/**
 * Get date and time in posix format
 *
*  \return time_t value from RTC
 */
extern time_t wmtime_time_get_posix(void);

/**
 * Convert to tm structure from POSIX/Unix time (Seconds since epoch)
 *
 * \param[in] tm This is POSIX time that is to be converted into \ref tm
 * \param[out] result This should point to pre-allocated \ref tm instance
 * \return pointer to struct tm; NULL in case of error
 */
extern struct tm *gmtime_r(const time_t *tm, struct tm *result);

/**
 * Converts to POSIX/Unix time from tm structure
 *
 * \param[in] tm This is \ref tm instance that is to be converted into
 * time_t format
 * \return time_t POSIX/Unix time equivalent
 */
extern time_t mktime(const struct tm *tm);

/**
 * Initialize time subsystem including RTC. Sets system time to 1/1/1970 00:00
 * (i.e. epoch 0)
 *
 * \return WM_SUCCESS on success, zero otherwise
 *
 * \note This function is called by the wm_core_init function. Applications need
 * not explicitly call this function if they have already called wm_core_init().
 */
extern int wmtime_init(void);

#endif
