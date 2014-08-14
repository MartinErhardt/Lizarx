#ifndef _H_UTMP
#define _H_UTMP

#define UTMP_FILE        "/etc/utmp"
#define WTMP_FILE        "/var/adm/wtmp"
#define ILOG_FILE        "/etc/.ilog"
#define ut_name  ut_user

struct utmp
{
        char ut_user[256];             /* User login name */
        char ut_id[14];                /* /etc/inittab id */
        char ut_line[64];              /* device name (console, lnxx) */
        pid_t ut_pid;                  /* process id */
        short ut_type;                 /* type of entry */
#if !defined(__64BIT__)
        int __time_t_space;            /* for 32vs64-bit time_t PPC */
#endif
        time_t ut_time;                /* time entry was made */
        struct exit_status
        {
            short e_termination;       /* Process termination status */
            short e_exit;              /* Process exit status */
        }
        ut_exit;                       /* The exit status of a process
                                        * marked as DEAD_PROCESS.
                                        */
        char ut_host[256];             /* host name */
        int __dbl_word_pad;            /* for double word alignment */
        int __reservedA[2];
        int __reservedV[6];
};
                    /*  Definitions for ut_type */
#define EMPTY              0
#define RUN_LVL            1
#define BOOT_TIME          2
#define OLD_TIME           3
#define NEW_TIME           4
#define INIT_PROCESS       5         /* Process spawned by "init"                 */
#define LOGIN_PROCESS      6         /* A "getty" process                         */

                                     /* waitingforlogin                           */
#define USER_PROCESS       7         /* A user process                            */
#define DEAD_PROCESS       8
#define ACCOUNTING         9
#define UTMAXTYPE ACCOUNTING         /* Largest legal value                        */
                                     /* of ut_type                                 */

   /* Special strings or formats used in the          */
   /* "ut_line" field when accounting for             */
   /* something other than a process.                 */
   /* No string for the ut_line field can be more     */
   /* than 11 chars + a NULL in length.               */

#define RUNLVL_MSG          "run-level %c"
#define BOOT_MSG            "system boot"
#define OTIME_MSG           "old time"
#define TIME_MSG            "new time"

#endif                /* _H_UTMP    */
