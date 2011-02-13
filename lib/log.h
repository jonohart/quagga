/*
 * $Id$
 *
 * Zebra logging funcions.
 * Copyright (C) 1997, 1998, 1999 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef _ZEBRA_LOG_H
#define _ZEBRA_LOG_H

#include <syslog.h>
#include <stdio.h>
#include "vargs.h"
#include "pthread_safe.h"

/* Here is some guidance on logging levels to use:
 *
 * LOG_DEBUG	- For all messages that are enabled by optional debugging
 *		  features, typically preceded by "if (IS...DEBUG...)"
 * LOG_INFO	- Information that may be of interest, but everything seems
 *		  to be working properly.
 * LOG_NOTICE	- Only for message pertaining to daemon startup or shutdown.
 * LOG_WARNING	- Warning conditions: unexpected events, but the daemon believes
 *		  it can continue to operate correctly.
 * LOG_ERR	- Error situations indicating malfunctions.  Probably require
 *		  attention.
 *
 * Note: LOG_CRIT, LOG_ALERT, and LOG_EMERG are currently not used anywhere,
 * please use LOG_ERR instead.
 */

typedef enum
{
  ZLOG_NONE,
  ZLOG_DEFAULT,
  ZLOG_ZEBRA,
  ZLOG_RIP,
  ZLOG_BGP,
  ZLOG_OSPF,
  ZLOG_RIPNG,
  ZLOG_OSPF6,
  ZLOG_ISIS,
  ZLOG_MASC
} zlog_proto_t;

/* If maxlvl is set to ZLOG_DISABLED, then no messages will be sent
   to that logging destination. */
#define ZLOG_DISABLED	(LOG_EMERG-1)

typedef enum
{
  ZLOG_DEST_SYSLOG = 0,
  ZLOG_DEST_STDOUT,
  ZLOG_DEST_MONITOR,
  ZLOG_DEST_FILE
} zlog_dest_t;
#define ZLOG_NUM_DESTS		(ZLOG_DEST_FILE+1)

struct zlog
{
  const char *ident;	/* daemon name (first arg to openlog) */
  zlog_proto_t protocol;
  int maxlvl[ZLOG_NUM_DESTS];	/* maximum priority to send to associated
  				   logging destination */
  int default_lvl;	/* maxlvl to use if none is specified */
  FILE *fp;
  char *filename;
  int facility;		/* as per syslog facility */
  int record_priority;	/* should messages logged through stdio include the
  			   priority of the message? */
  int syslog_options;	/* 2nd arg to openlog */
  int timestamp_precision;	/* # of digits of subsecond precision */
};

/* Message structure. */
struct message
{
  int key;
  const char *str;
};

/* module initialization */
extern void zlog_init_r(void);
extern void zlog_destroy_r(void);

/* Default logging structure. */
extern struct zlog *zlog_default;

/* Open zlog function */
extern struct zlog *openzlog (const char *progname, zlog_proto_t protocol,
		              int syslog_options, int syslog_facility);

/* Close zlog function. */
extern void closezlog (struct zlog *zl);

/* Generic function for zlog. */
extern void zlog (struct zlog *zl, int priority, const char *format, ...)
  PRINTF_ATTRIBUTE(3, 4);
/* assumed locked version for close friends */
extern void uzlog (struct zlog *zl, int priority, const char *format, ...)
  PRINTF_ATTRIBUTE(3, 4);

/* Handy zlog functions. */
extern void zlog_err (const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
extern void zlog_warn (const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
extern void zlog_info (const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
extern void zlog_notice (const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
extern void zlog_debug (const char *format, ...) PRINTF_ATTRIBUTE(1, 2);

/* For bgpd's peer oriented log. */
extern void plog_err (struct zlog *, const char *format, ...)
  PRINTF_ATTRIBUTE(2, 3);
extern void plog_warn (struct zlog *, const char *format, ...)
  PRINTF_ATTRIBUTE(2, 3);
extern void plog_info (struct zlog *, const char *format, ...)
  PRINTF_ATTRIBUTE(2, 3);
extern void plog_notice (struct zlog *, const char *format, ...)
  PRINTF_ATTRIBUTE(2, 3);
extern void plog_debug (struct zlog *, const char *format, ...)
  PRINTF_ATTRIBUTE(2, 3);

/* Set logging level for the given destination.  If the log_level
   argument is ZLOG_DISABLED, then the destination is disabled.
   This function should not be used for file logging (use zlog_set_file
   or zlog_reset_file instead). */
extern void zlog_set_level (struct zlog *zl, zlog_dest_t, int log_level);

/* Set logging to the given filename at the specified level. */
extern int zlog_set_file (struct zlog *zl, const char *filename, int log_level);
/* Disable file logging. */
extern int zlog_reset_file (struct zlog *zl);

/* Rotate log. */
extern int zlog_rotate (struct zlog *);

/* getters & setters */
extern int zlog_get_default_lvl (struct zlog *zl);
extern void zlog_set_default_lvl (struct zlog *zl, int level);
extern void zlog_set_default_lvl_dest (struct zlog *zl, int level);
extern int zlog_get_maxlvl (struct zlog *zl, zlog_dest_t dest);
extern int zlog_get_facility (struct zlog *zl);
extern void zlog_set_facility (struct zlog *zl, int facility);
extern int zlog_get_record_priority (struct zlog *zl);
extern void zlog_set_record_priority (struct zlog *zl, int record_priority);
extern int zlog_get_timestamp_precision (struct zlog *zl);
extern void zlog_set_timestamp_precision (struct zlog *zl, int timestamp_precision);
extern const char * zlog_get_ident (struct zlog *zl);
extern char * zlog_get_filename (struct zlog *zl);
extern int zlog_is_file (struct zlog *zl);
extern const char * zlog_get_proto_name (struct zlog *zl);
extern const char * uzlog_get_proto_name (struct zlog *zl);

/* For hackey massage lookup and check */
#define LOOKUP(x, y) mes_lookup(x, x ## _max, y, "(no item found)")

extern const char *lookup (const struct message *, int);
extern const char *mes_lookup (const struct message *meslist,
                               int max, int index,
                               const char *no_item);

extern const char *zlog_priority[];
extern const char *zlog_proto_names[];

/* Safe version of strerror -- never returns NULL. */
extern const char *safe_strerror(int errnum);

/* To be called when a fatal signal is caught. */
extern void zlog_signal(int signo, const char *action
#ifdef SA_SIGINFO
			, siginfo_t *siginfo, void *program_counter
#endif
		       );

/* Ring down the curtain -- turn of SIGABRT handler and abort()         */
extern void zabort_abort(void)  __attribute__ ((noreturn)) ;

/* Log a backtrace. */
extern void zlog_backtrace(int priority);

/* Log a backtrace, but in an async-signal-safe way.  Should not be
   called unless the program is about to exit or abort, since it messes
   up the state of zlog file pointers.  If program_counter is non-NULL,
   that is logged in addition to the current backtrace. */
extern void zlog_backtrace_sigsafe(int priority, void *program_counter);

/* Puts a current timestamp in buf and returns the number of characters
 * written (not including the terminating NUL).  The purpose of
 * this function is to avoid calls to localtime appearing all over the code.
 * It caches the most recent localtime result and can therefore
 * avoid multiple calls within the same second.
 *
 * The buflen MUST be > 1 and the buffer address MUST NOT be NULL.
 *
 * If buflen is too small, writes buflen-1 characters followed by '\0'.
 *
 * Time stamp is rendered in the form: %Y/%m/%d %H:%M:%S
 *
 * This has a fixed length (leading zeros are included) of 19 characters
 * (unless this code is still in use beyond the year 9999 !)
 *
 * Which may be followed by "." and a number of decimal digits, usually 1..6.
 *
 * So the maximum time stamp is 19 + 1 + 6 = 26.  Adding the trailing '\n', and
 * rounding up for good measure -- buffer size = 32.
 */
#define TIMESTAMP_FORM "%Y/%m/%d %H:%M:%S"

enum { timestamp_buffer_len = 32 } ;

extern size_t quagga_timestamp(int timestamp_precision /* # subsecond digits */,
			       char *buf, size_t buflen);

/* Generate line to be logged
 *
 * Structure used to hold line for log output -- so that need be generated
 * just once even if output to multiple destinations.
 *
 * Note that the buffer length is a hard limit (including terminating '\n''\0'
 * or '\r''\n''\0').  Do not wish to malloc any larger buffer while logging.
 */
enum { logline_buffer_len = 1008 } ;
enum ll_term
{
  llt_nul   = 0,        /* NB: also length of the terminator            */
  llt_lf    = 1,
  llt_crlf  = 2,
} ;

struct logline {
  char*   p_nl ;        /* address of the terminator                    */

  enum ll_term term ;   /* how line is terminated                       */

  size_t  len ;         /* length including either '\r''\n' or '\n'     */

  char line[logline_buffer_len]; /* buffer                              */
} ;

extern void
uvzlog_line(struct logline* ll, struct zlog *zl, int priority,
                            const char *format, va_list va, enum ll_term term) ;

/* Defines for use in command construction: */

#define LOG_LEVELS "(emergencies|alerts|critical|errors|" \
                              "warnings|notifications|informational|debugging)"

#define LOG_LEVEL_DESC \
  "System is unusable\n" \
  "Immediate action needed\n" \
  "Critical conditions\n" \
  "Error conditions\n" \
  "Warning conditions\n" \
  "Normal but significant conditions\n" \
  "Informational messages\n" \
  "Debugging messages\n"

#define LOG_FACILITIES "(kern|user|mail|daemon|auth|syslog|lpr|news|uucp|cron|local0|local1|local2|local3|local4|local5|local6|local7)"

#define LOG_FACILITY_DESC \
       "Kernel\n" \
       "User process\n" \
       "Mail system\n" \
       "System daemons\n" \
       "Authorization system\n" \
       "Syslog itself\n" \
       "Line printer system\n" \
       "USENET news\n" \
       "Unix-to-Unix copy system\n" \
       "Cron/at facility\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n" \
       "Local use\n"

#endif /* _ZEBRA_LOG_H */
