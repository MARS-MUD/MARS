/******************************************************************************
*       *****   **    **           **             ***** ***        *******    *         
*    ******  ***** *****        *****          ******  * **      *       ***  *         
*   **   *  *  ***** *****     *  ***         **   *  *  **     *         **  *         
*  *    *  *   * **  * **         ***        *    *  *   **     **        *   *         
*      *  *    *     *           *  **           *  *    *       ***          *         
*     ** **    *     *           *  **          ** **   *       ** ***        *         
*     ** **    *     *          *    **         ** **  *         *** ***      *         
*     ** **    *     *          *    **         ** ****            *** ***    *         
*     ** **    *     *         *      **        ** **  ***           *** ***  *         
*     ** **    *     **        *********        ** **    **            ** *** *         
*     *  **    *     **       *        **       *  **    **             ** ** *         
*        *     *      **      *        **          *     **              * *  *         
*    ****      *      **     *****      **     ****      ***   ***        *   *         
*   *  *****           **   *   ****    ** *  *  ****    **   *  *********    *         
*  *     **                *     **      **  *    **     *   *     *****      *         
*  *                       *                 *               *                *         
*    **                      **                **              **             *         
*                                                                             *
*  MULTI                    ADVENTURER         ROLEPLAYING     SYSTEM         *
*  MARS 0.1b is copyright 2015-2024 Ro Black mars@wr3tch.org                  *
******************************************************************************/
/******************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,           *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.      *
 *                                                                            *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael             *
 *  Chastain, Michael Quan, and Mitchell Tse.                                 *
 *                                                                            *
 *  In order to use any part of this Merc Diku Mud, you must comply with      *
 *  both the original Diku license in 'license.doc' as well the Merc          *
 *  license in 'license.txt'.  In particular, you may not remove either of    *
 *  these copyright notices.                                                  *
 *                                                                            *
 *  Much time and thought has gone into this software and you are             *
 *  benefitting.  We hope that you share your changes too.  What goes         *
 *  around, comes around.                                                     *
 *****************************************************************************/

/******************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			                      *
*	ROM has been brought to you by the ROM consortium		                  *
*	    Russ Taylor (rtaylor@hypercube.org)				                      *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			                  *
*	    Brian Moore (zump@rom.org)					                          *
*	By using this code, you have agreed to follow the terms of the	          *
*	ROM license, in the file Rom24/doc/rom.license			                  *
******************************************************************************/
/* dwatch egate artifice fdmud code copyright 2002-2008 Bob Kircher	  */


/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#include <arpa/telnet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"


/*
 * Malloc debugging stuff.
 */

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif


/*
 * Socket and TCP/IP stuff.
 */
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
        char    keepalive_str   [] = { IAC, NOP, '\0' };

DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/


/*
 * OS-dependent local functions.
 */

void	game_loop_unix		args( ( int control ) );
int	init_socket		args( ( int port, bool fAbort ) );
void	init_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );


/* ANSI codes for foreground text colors */
const char * const ansi_color[] =
{
    "\033[0m\033[30m",	/* AT_BLACK  */
    "\033[0m\033[31m",	/* AT_BLOOD  */
    "\033[0m\033[32m",	/* AT_DGREEN */
    "\033[0m\033[33m",	/* AT_ORANGE */
    "\033[0m\033[34m",	/* AT_DBLUE  */
    "\033[0m\033[35m",	/* AT_PURPLE */
    "\033[0m\033[36m",	/* AT_CYAN   */
    "\033[0m\033[37m",	/* AT_GREY   */
    "\033[1m\033[30m",	/* AT_DGREY  */
    "\033[1m\033[31m",	/* AT_RED    */
    "\033[1m\033[32m",	/* AT_GREEN  */
    "\033[1m\033[33m",	/* AT_YELLOW */
    "\033[1m\033[34m",	/* AT_BLUE   */
    "\033[1m\033[35m",	/* AT_PINK   */
    "\033[1m\033[36m",	/* AT_LBLUE  */
    "\033[1m\033[37m"	/* AT_WHITE  */
};

static const char *bar_chart[] =
{
    "`R      `Y      `G      ",
    "`R=     `Y      `G      ",
    "`R==    `Y      `G      ",
    "`R===   `Y      `G      ",
    "`R====  `Y      `G      ",
    "`R===== `Y      `G      ",
    "`R======`Y      `G      ",
    "`R======`Y=     `G      ",
    "`R======`Y==    `G      ",
    "`R======`Y===   `G      ",
    "`R======`Y====  `G      ",
    "`R======`Y===== `G      ",
    "`R======`Y======`G      ",
    "`R======`Y======`G=     ",
    "`R======`Y======`G==    ",
    "`R======`Y======`G===   ",
    "`R======`Y======`G====  ",
    "`R======`Y======`G===== ",
    "`R======`Y======`G======",
};

struct	timeval	last_time;


/*
 * Other local functions (OS-independent).
 */
void	act_arena		args( ( int AType, const char *format,
                                        CHAR_DATA *ch, const void *arg1,
                                        const void *arg2, int type,
                                        int min_pos ) );
void	hotboot_recover		args( ( void ) );
void	load_startup_time	args( ( void ) );
int	main			args( ( int argc, char **argv ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	reset_idle_event	args( ( DESCRIPTOR_DATA *d ) );
void	save_startup_time	args( ( void ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void    webwho_socket           args( ( void ) );
int
main( int argc, char **argv )
{
    struct timeval now_time;
    bool fHotBoot = FALSE;

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    /*
     * Init time.
     */
    setenv( "TZ", TIMEZONE, TRUE );
    gettimeofday( &now_time, NULL );
    current_time 	= (time_t) now_time.tv_sec;
    boot_time		= current_time;
    startup_time	= current_time;
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Save our binary name
     */
    strcpy( boot_file, argv[0] );

    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = GAME_PORT;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}

	/* Are we recovering from a hotboot? */
 	if ( argc > 2 && argv[2] && argv[2][0] )
 	{
	    fHotBoot = TRUE;
	    control = atoi( argv[3] );
	    load_startup_time( );
 	}
 	else
 	{
	    fHotBoot = FALSE;
	    save_startup_time( );
	}
    }

    /*
     * Run the game.
     */

    if ( !fHotBoot )
	control = init_socket( port, TRUE );

    sprintf( log_buf, "Booting %s", MUD_NAME );
    log_string( log_buf );
    boot_db();

    if ( sys_webwho )
	whosock = init_socket( port + WEBWHO_OFFSET, FALSE );
    else
	whosock = 0;

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;
    if ( !fHotBoot )
	sprintf( log_buf, "%s is ready to rock on port %d.", MUD_NAME, port );
    else
	sprintf( log_buf, "%s continues to rock on port %d.", MUD_NAME, port );
    log_string( log_buf );

    if ( fHotBoot )
    	hotboot_recover( );

    game_loop_unix( control );
    close (control);

    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );
    exit( 0 );
    return 0;
}


int
init_socket( int port, bool fAbort )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close(fd);
	exit( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close(fd);
	    exit( 1 );
	}
    }
#endif

    sa		    = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
	perror( "Init socket: bind" );
	close( fd );
	abort_mud( "Cannot bind to socket." );
	exit( 1 );
    }


    if ( listen( fd, 3 ) < 0 )
    {
	perror("Init socket: listen");
	close(fd);
	exit(1);
    }

    return fd;
}


void
game_loop_unix( int control )
{
    static struct timeval null_time;

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	int maxdesc;

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc	= control;

	if ( whosock != 0 )
	{
	    FD_SET( whosock, &in_set );
	    maxdesc = UMAX( maxdesc, whosock );
	}

	for ( d = descriptor_list; d; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    init_descriptor( control );

	/* Check the web */
	if ( sys_webwho && FD_ISSET( whosock, &in_set ) )
	    webwho_socket( );

	/*
	 * Kick out the freaky folks.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character && d->connected == CON_PLAYING)
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
	    }
	}

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character != NULL && d->connected == CON_PLAYING)
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if (d->character != NULL && d->character->daze > 0)
		--d->character->daze;

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );
		reset_idle_event( d );
		d->riding = FALSE;

		lci++;
		lci %= LCB_SIZE;
		lcb[lci].b_time = current_time;
		lcb[lci].d = d;
		strcpy( lcb[lci].cmd, d->incomm );

		if (d->showstr_point)
		    show_string(d,d->incomm);
		else if ( d->pString )
		    string_add( d->character, d->incomm );
		else if ( d->connected == CON_PLAYING )
		{
		    if ( IS_DEAD( d->character ) )
			check_death_cmds( d, d->incomm );
		    else
			substitute_alias( d, d->incomm );
		}
		else
		    nanny( d, d->incomm );

		d->incomm[0]	= '\0';
	    }
	    else
		check_riding( d );
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
	    &&   FD_ISSET(d->descriptor, &out_set) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL && d->connected == CON_PLAYING)
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 && errno != EINTR )
		{
		    perror( "Game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}


void
init_descriptor( int control )
{
    char		buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *	dnew;
    EVENT_DATA *	event;
    struct sockaddr_in	sock;
    struct hostent *	from;
    int			desc;
    unsigned		size;

    size = sizeof( sock );
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size ) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	return;
    }

    /*
     * Cons a new descriptor.
     */
    dnew = new_descriptor(); /* new_descriptor now also allocates things */
    dnew->descriptor = desc;

    size = sizeof( sock );
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    {
	perror( "New_descriptor: getpeername" );
	dnew->host = str_dup( "(unknown)" );
    }
    else
    {
	/*
	 * Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
	int addr;

	addr = ntohl( sock.sin_addr.s_addr );
	sprintf( buf, "%d.%d.%d.%d",
	    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	    );
	sprintf( log_buf, "Sock.sinaddr:  %s", buf );
	log_string( log_buf );
	if ( addr != 0x7F000101 )
	    wiznet( log_buf, NULL, NULL, WIZ_SITES, 0, IMPLEMENTOR );
	from = gethostbyaddr( (char *) &sock.sin_addr,
	    sizeof(sock.sin_addr), AF_INET );
	dnew->host = str_dup( from ? from->h_name : buf );
	dnew->ip = str_dup( buf );
    }

    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    if ( check_ban(dnew->host,BAN_ALL))
    {
	write_to_descriptor( desc,
	    "Your site has been banned from this mud.\n\r", 0, dnew->ansi );
	close( desc );
	free_descriptor(dnew);
	return;
    }
    /*
     * Init descriptor data.
     */
    dnew->next			= descriptor_list;
    descriptor_list		= dnew;

    /*
     * Queue idle timeout
     */
    event = new_event( );
    event->type = EVENT_DESC_LOGIN;
    event->fun = event_desc_login;
    add_event_desc( event, dnew, 15 * 60 * PULSE_PER_SECOND );

    /*
     * Queue idle timer
     */
    event = new_event( );
    event->type = EVENT_DESC_IDLE;
    event->fun = event_desc_idle;
    add_event_desc( event, dnew, 60 * PULSE_PER_SECOND );

    /*
     * Send the greeting.
     */
    write_to_buffer( dnew, "Do you wish to use ANSI color?  (Yes/No): ", 0 );

    return;
}


void
close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 && dclose->outsize < 32000 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );
    }

    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }

    if ( ( ch = dclose->character ) != NULL )
    {
	sprintf( log_buf, "Closing link to %s.", ch->name );
	log_string( log_buf );
	/* cut down on wiznet spam when rebooting */
	if ( dclose->connected == CON_PLAYING && !merc_down)
	{
	    if ( dclose->original != NULL
	    && !IS_NPC( dclose->original ) )
		REMOVE_BIT( dclose->original->act, PLR_SWITCHED );
	    act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
	    wiznet("Net death has claimed $N.",ch,NULL,WIZ_LINKS,0,0);
	    ch->desc = NULL;

	}
	else
	{
	    if ( ch->pet != NULL && ch->pet->in_room == NULL )
		extract_char( ch->pet, TRUE );
	    if ( ch->mount != NULL && ch->mount->in_room == NULL )
		extract_char( ch->mount, TRUE );

	    free_char(dclose->original ? dclose->original : 
		dclose->character );
	}
    }

    if ( d_next == dclose )
	d_next = d_next->next;   

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d != NULL )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

    close( dclose->descriptor );
    free_descriptor(dclose);
    return;
}



bool
read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf );
	write_to_descriptor( d->descriptor,
	    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0, d->ansi );
	return FALSE;
    }

    /* Snarf input. */
    for ( ; ; )
    {
	int nRead;

	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string( "EOF encountered on read." );
	    return FALSE;
	}
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void
read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
        if ( (unsigned char)d->inbuf[i] == IAC )
        {
            if ( d->inbuf[i+1] == '\0' || d->inbuf[i+2] == '\0' )
                return;
            else
                i += 2;
        }
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0, d->ansi );

	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	{
	    --k;
	    continue;
        }
	else if ( (unsigned char)d->inbuf[i] == IAC )
	{
	    i += 2;
	    continue;
	}
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */

    if ( k > 1 || d->incomm[0] == '!' )
    {
    	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
	    if (++d->repeat >= 25 && d->character
	    &&  d->connected == CON_PLAYING)
	    {
		sprintf( log_buf, "%s input spamming!", d->host );
		log_string( log_buf );
		wiznet("Spam spam spam $N spam spam spam spam spam!",
		       d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
		if (d->incomm[0] == '!')
		    wiznet( "$t", d->character, d->inlast, WIZ_SPAM, 0,
			get_trust(d->character));
		else
		    wiznet( "$t", d->character, d->incomm, WIZ_SPAM, 0,
			get_trust( d->character ) );

		d->repeat = 0;
/*
		write_to_descriptor( d->descriptor,
		    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
		strcpy( d->incomm, "quit" );
*/
	    }
	}
    }


    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}



/*
 * Low level output function.
 */
bool
process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    CHAR_DATA *	rch;
    char *	p;
    char *	q;
    int		bar_index;

    /*
     * Bust a prompt.
     */
    set_char_color( C_DEFAULT, d->original ? d->original : d->character );
    if (!merc_down && d->showstr_point)
	write_to_buffer( d,
/*	"[Hit Return to continue]\n\r", */
  "[Please type (c)ontinue, (r)efresh, (b)ack, (h)elp, (q)uit, or RETURN]:  ",
	0 );
    else if ( d->pString )
	write_to_buffer( d, "> ", 2 );
    else if (fPrompt && !merc_down && d->connected == CON_PLAYING)
    {
   	CHAR_DATA *ch;
	CHAR_DATA *victim;

	ch = d->character;

        /* battle prompt */
        if ((victim = ch->fighting) != NULL && can_see(ch,victim))
	{
            int percent;
            char wound[100];
	    char buf[MAX_STRING_LENGTH];

            if (victim->max_hit > 0)
                percent = victim->hit * 100 / victim->max_hit;
            else
                percent = -1;

            if (percent >= 100)
                sprintf(wound,"is `Wunharmed`X.");
            else if (percent >= 90)
                sprintf(wound,"is `Binjured `Xslightly.");
            else if (percent >= 75)
                sprintf(wound,"is a bit `Obattered`X.");
            else if (percent >= 50)
                sprintf(wound,"is `rdamaged `Xconsiderably`X.");
            else if (percent >= 30)
                sprintf(wound,"is `Pquite hurt`X.");
            else if (percent >= 15)
                sprintf(wound,"is grievously `Rharmed`X.");
            else if (percent >= 0)
                sprintf(wound,"is nearly `Yvanquished`X.");
            else
                sprintf(wound,"is `Cincapacitated`X.");

            sprintf(buf,"%s %s \n\r", 
	            IS_NPC(victim) ? victim->short_descr : victim->name,wound);

	    p = buf + strlen( buf );
	    if ( ch->desc && ch->desc->original )
		rch = ch->desc->original;
	    else
		rch = ch;
	    if ( !IS_NPC( rch ) && IS_SET( rch->act2, PLR_FMETER ) )
	    {
		p = stpcpy( p, "`WYou: [" );
		bar_index = URANGE( 0, ( ch->hit * 18 + ch->max_hit / 36 ) / ch->max_hit, 18 );
		p = stpcpy( p, bar_chart[ bar_index] );
		p = stpcpy( p, "`W] " );

		percent = ( ch->hit * 100 + ch->max_hit / 2 ) / ch->max_hit;
		if ( percent < 34 )
		    p = stpcpy( p, "`R" );
		else if ( percent < 67 )
			p = stpcpy( p, "`Y" );
		else
		    p = stpcpy( p, "`G" );
		p += sprintf( p, "%d", percent );
		p = stpcpy( p, "%\n\r`WOpp: [" );

		bar_index = URANGE( 0, ( victim->hit * 18 + victim->max_hit / 36 ) / victim->max_hit, 18 );
		p = stpcpy( p, bar_chart[ bar_index] );
		p = stpcpy( p, "`W] " );

		percent = ( victim->hit * 100 + victim->max_hit / 2 ) / victim->max_hit;
		if ( percent < 34 )
			p = stpcpy( p, "`R" );
		else if ( percent < 67 )
		    p = stpcpy( p, "`Y" );
		else
		    p = stpcpy( p, "`G" );
		p += sprintf( p, "%d", percent );

		p = stpcpy( p, "%`X" );
	    }

	    q = &buf[0];
	    while ( is_colcode( q ) )
	        q += 2;
	    *q = UPPER( *q );
            write_to_buffer( d, buf, p - buf );
	}


	ch = d->original ? d->original : d->character;
	if (!IS_SET(ch->comm, COMM_COMPACT) )
	    write_to_buffer( d, "\n\r", 2 );


        if ( IS_SET(ch->comm, COMM_PROMPT) )
            bust_a_prompt( d->character );

	if (IS_SET(ch->comm,COMM_TELNET_GA))
	    write_to_buffer(d,go_ahead_str,0);
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
	if (d->character != NULL)
	    write_to_buffer( d->snoop_by, d->character->name,0);
	write_to_buffer( d->snoop_by, "> ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop, d->ansi ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void
bust_a_prompt( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    const char *str;
    const char *i;
    char *point;
    char doors[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    int door;
    struct tm *timebuf;

    if ( ch == NULL || ch->desc == NULL )
        return;

    point = buf;
    str = ch->prompt;
    if (str == NULL || str[0] == '\0')
    {
        sprintf( buf, "<%dhp %dm %dmv> %s",
	    ch->hit,ch->mana,ch->move,ch->prefix);
	send_to_char(buf,ch);
	return;
    }

    if ( IS_SET( ch->comm,COMM_AFK ) )
    {
	send_to_char( "`W<`RAFK`W>`X ",ch );
	return;
    }
    else if ( !IS_NPC( ch ) && ch->pnote != NULL && ch->fighting == NULL )
    {
        ch_printf( ch, "`W<`R%s `Gnote in progress`W>`X ",
                   ch->pnote->board->name );
        return;
    }

    timebuf = localtime( &current_time );

   while( *str != '\0' )
   {
      if( *str != '%' )
      {
         *point++ = *str++;
         continue;
      }
      ++str;
      switch( *str )
      {
         default :
            i = " "; break;
         case 'a' :
            if( ch->level > 9 )
               sprintf( buf2, "%d", ch->alignment );
            else
               sprintf( buf2, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ?
                "evil" : "neutral" );
            i = buf2; break;
 	 case 'c' :
	    sprintf(buf2,"%s","\n\r");
	    i = buf2; break;
        case 'd':
            sprintf( buf2, "%02d/%02d/%02d",
                           timebuf->tm_year % 100,
                           timebuf->tm_mon + 1,
                           timebuf->tm_mday );
            i = buf2; break;
        case 'D':
            strcpy( buf2, IS_AFFECTED ( ch, AFF_HIDE ) ? "D" : "-" );
            i = buf2; break;
	case 'e':
	    found = FALSE;
	    doors[0] = '\0';
	    for (door = 0; door < MAX_DIR; door++)
	    {
		if ((pexit = ch->in_room->exit[door]) != NULL
		&&  pexit ->to_room != NULL
		&&  (can_see_room(ch,pexit->to_room)
		||   (IS_AFFECTED(ch,AFF_INFRARED) 
		&&    !IS_AFFECTED(ch,AFF_BLIND)))
		&&  !IS_SET(pexit->exit_info,EX_CLOSED))
		{
		    found = TRUE;
		    strcat( doors, dir_letter[door] );
		}
	    }
	    if (!found)
	 	strcat( doors, "none" );
	    sprintf(buf2,"%s",doors);
	    i = buf2; break;
         case 'h' :
            sprintf( buf2, "%d", ch->hit );
            i = buf2; break;
         case 'H' :
            sprintf( buf2, "%d", ch->max_hit );
            i = buf2; break;
         case 'i':
            if ( ch->invis_level > 0
            ||	 ch->incog_level > 0
            ||	 IS_AFFECTED( ch, AFF_INVISIBLE ) )
                i = " invis";
            else
                i = "";
            break;
         case 'm' :
            sprintf( buf2, "%d", ch->mana );
            i = buf2; break;
         case 'M' :
            sprintf( buf2, "%d", ch->max_mana );
            i = buf2; break;
	 case 'o' :
	    i = olc_ed_name( ch );
	    break;
	 case 'O' :
	    i = olc_ed_vnum( ch );
	    break;
         case 'p':
             if ( !IS_NPC( ch )
             &&   IS_SET( ch->act2, PLR_POSE )
             &&   !IS_NULLSTR( ch->pcdata->pose ) )
                 i = " posing";
             else
                 i = "";
            break;
         case 'r' :
            if( ch->in_room != NULL )
               sprintf( buf2, "%s", 
		((!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT)) ||
		 (!IS_AFFECTED(ch,AFF_BLIND) && !room_is_dark( ch->in_room )))
		? ROOMNAME( ch->in_room ) : "darkness");
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case 'R' :
            if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
            {
		if ( IS_VIRTUAL( ch->in_room ) )
		    sprintf( buf2, "%d%c(%d,%d)", ch->in_room->vnum,
			     IS_SET( ch->in_room->room_flags, ROOM_VIRTUAL ) ? 'v' : 'r',
			     ( ch->in_room->vnum - ch->in_room->area->min_vnum ) % ch->in_room->area->maxx,
			     ( ch->in_room->vnum - ch->in_room->area->min_vnum ) / ch->in_room->area->maxx );
		else
		    sprintf( buf2, "%d", ch->in_room->vnum );
            }
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case 'S':
             strcpy( buf2, IS_AFFECTED ( ch, AFF_SNEAK ) ? "S" : "-" );
             i = buf2; break;
         case 't':
             sprintf( buf2, "%02d:%02d:%02d",
                            timebuf->tm_hour,
                            timebuf->tm_min,
                            timebuf->tm_sec );
             i = buf2; break;
         case 'v' :
            sprintf( buf2, "%d", ch->move );
            i = buf2; break;
         case 'V' :
            sprintf( buf2, "%d", ch->max_move );
            i = buf2; break;
	 case 'w':
	    sprintf( buf2, "%d%c %d%c %d%c %d%c",
		     ch->money.gold,   GOLD_INITIAL,
		     ch->money.silver, SILVER_INITIAL,
		     ch->money.copper, COPPER_INITIAL,
		     ch->money.fract,  FRACT_INITIAL );
	    i = buf2; break;
         case 'x' :
            sprintf( buf2, "%d", ch->exp );
            i = buf2; break;
	 case 'X' :
	    sprintf(buf2, "%d", IS_NPC(ch) ? 0 :
	    (ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);
	    i = buf2; break;
         case 'z' :
            if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
               sprintf( buf2, "%s", ch->in_room->area->name );
            else
               sprintf( buf2, " " );
            i = buf2; break;
         case '%' :
            sprintf( buf2, "%%" );
            i = buf2; break;
      }
      ++str;
      while( (*point = *i) != '\0' )
         ++point, ++i;
   }
   write_to_buffer( ch->desc, buf, point - buf );

   if (ch->prefix[0] != '\0')
        write_to_buffer(ch->desc,ch->prefix,0);
   return;
}

/*
 * Append onto an output buffer.
 */
void
write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    static const char color_codes[] = ".rgObpcwzRGYBPCW";
    char *dst;
    char  c;
    char  color;
    int   len;

    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen(txt);

    /*
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
	d->outbuf[0]	= '\n';
	d->outbuf[1]	= '\r';
	d->outtop	= 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
	char *outbuf;

        if (d->outsize >= 32000)
	{
	    close_socket(d);
	    bug("Buffer overflow. Closing.\n\r",0);
	    return;
 	}
	outbuf      = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
    }

    /*
     * Set up for color processing
     */
    if ( d->lastcolor > 0
	 && d->lastcolor < ( sizeof( color_codes ) / sizeof( color_codes[0] ) ) )
    {
	color = color_codes[(int)d->lastcolor];
    }
    else
    {
	color = color_codes[C_DEFAULT];
    }
    /*
     * Copy.  This is where we can do `X substitution and color mapping.
     */
/*  strncpy( d->outbuf + d->outtop, txt, length ); */
    dst = d->outbuf + d->outtop;
    for ( len = 0; len < length; len++ )
    {
	*dst++ = ( c = *txt++ );
	if ( c == '`' && len < length - 1 )
	{
	    if ( *txt == 'X' )
	    {
		*dst++ = color;
	    }
	    else if ( *txt == '.' )
	    {
		if ( d->character != NULL  && !IS_NPC( d->character )
		&&   !IS_SET( d->character->act, PLR_BLINK ) )
		    *dst++ = color_codes[number_range( 1, 15 )];
		else
		    *dst++ = *txt;
	    }
	    else
	    {
		*dst++ = *txt;
	    }
	    txt++;
	    len++;
	}
    }
    d->outtop += length;
    return;
}



/************************************************************************/
/*		write_to_descriptor()					*/
/* Lowest level output function.					*/
/* Write a block of text to the file descriptor.			*/
/* If this gives errors on very long blocks (like 'ofind all'),		*/
/*   try lowering the max block size.					*/
/* Color code interpretation is performed here.				*/
/* NOTE: txt may possibly NOT have a '\0' terminator			*/
/************************************************************************/
bool
write_to_descriptor( int desc, char *txt, int length, bool fAnsi )
{
    int		iStart;
    int		nWrite;
    int		nBlock;
    char *	str;
    int		c;
    int		nl;
    int		l;
    char *	buf;
    char *	pt;
    int		color;
    char *	p;
    char	b[ MAX_INPUT_LENGTH ];
    int		buflen;

    if ( length <= 0 )
	length = strlen( txt );
//    *( txt + length ) = '\0';

    /* First, calculate size of output string */
    if ( fAnsi )
	l = 9;
    else
	l = 0;
    str = txt;
    nl = 0;
    while ( ( str - txt ) < length )
    {
	c = *str++;
	if ( c == '`' )
	{
	    switch ( *str++ )
	    {
		case '\0': nl++; str--; break;
		case '-':
		case '`' : nl++; break;
		case 'r':
		case 'g':
		case 'O':
		case 'b':
		case 'p':
		case 'c':
		case 'w': if ( fAnsi ) nl += 9; break;
		case 'z':
		case 'R':
		case 'G':
		case 'Y':
		case 'B':
		case 'P':
		case 'C':
		case 'W': if ( fAnsi ) nl += 9; break;
		case '.': if ( fAnsi ) nl += 11; break;
		default:  nl += 2; break;
	    }
	}
	else
	{
	    nl++;
	}
    }
    buf = alloc_mem( nl );

    /* Now copy txt to buf, expanding color codes to ANSI sequences */
    pt = buf;
    str = txt;
    while ( ( str - txt ) < length )
    {
	c = *str++;
	if ( c == '`' )
	{
	    switch ( *str++ )
	    {
		case '\0': *pt++ = '`'; str--; p = ""; break;
		case '-': p = ""; *pt++ = '~';   break;
		case '`': p = ""; *pt++ = '`';   break;
		//case 'r': p = "\033[0m\033[31m"; break;
        case 'r': p = "\e[0;31m"; break;
		case 'g': p = "\033[0m\033[32m"; break;
		case 'O': p = "\033[0m\033[33m"; break;
		case 'b': p = "\033[0m\033[34m"; break;
		case 'p': p = "\033[0m\033[35m"; break;
		case 'c': p = "\033[0m\033[36m"; break;
		case 'w': p = "\033[0m\033[37m"; break;
		case 'z': p = "\033[1m\033[30m"; break;
		case 'R': p = "\033[1m\033[31m"; break;
		case 'G': p = "\033[1m\033[32m"; break;
		case 'Y': p = "\033[1m\033[33m"; break;
		case 'B': p = "\033[1m\033[34m"; break;
		case 'P': p = "\033[1m\033[35m"; break;
		case 'C': p = "\033[1m\033[36m"; break;
		case 'W': p = "\033[1m\033[37m"; break;
		case '.': 
		    color = number_range( 1, 15 );
		    sprintf( b, "\033[0;%d;5;%dm", (color>7), 30+(color&7) );
		    p = b;
		    break;
		default :
		    *pt++ = '`';
		    *pt++ = *--str;
		    str++; p = "";
		    break;
	    }
	    if ( fAnsi ) 
		pt = stpcpy( pt, p );	
	}
	else
	{
	    *pt++ = c;
	}
    }

    buflen = pt - buf;

    for ( iStart = 0; iStart < buflen; iStart += nWrite )
    {
	nBlock = UMIN( buflen - iStart, 2048 );
	if ( ( nWrite = write( desc, buf + iStart, nBlock ) ) < 0 )
	{
	    perror( "Write_to_descriptor" );
	    free_mem( buf, nl );
	    return FALSE;
	}
    }
    free_mem( buf, nl );
    return TRUE;
}


void
stop_idling( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *	pReturn;

    if ( ch == NULL
    ||   ch->desc == NULL
    ||   ch->desc->connected != CON_PLAYING
    ||   ch->was_in_room == 0 
    ||   ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
	return;

    if ( ( pReturn = get_vroom_index( ch->was_in_room ) ) == NULL )
    {
	bugf( "Stop_idling: room #%d has disappeared for %s!",
	      ch->was_in_room,
	      IS_NPC( ch ) ? ch->short_descr : ch->name );
	pReturn = get_room_index( ROOM_VNUM_LIMBO );
    }
    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, pReturn );
    ch->was_in_room	= 0;
    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}

//void char_puts(const char *txt, CHAR_DATA *ch)     
//{     
//        send_to_char(GETMSG(txt, ch->lang), ch);     
//}     



void reset_idle_event( DESCRIPTOR_DATA *d )
{
    EVENT_DATA *event;

    if ( d == NULL )
	return;

    strip_event_desc( d, EVENT_DESC_IDLE );
    d->idle = 0;
    event = new_event( );
    event->type = EVENT_DESC_IDLE;
    event->fun = event_desc_idle;
    add_event_desc( event, d, 60 * PULSE_PER_SECOND );
    return;
}


/*
 * Set color for subsequent output
 */
int
set_char_color( int AType, CHAR_DATA *ch )
{
    int oldcolor;

    if ( ch && ch->desc )
	oldcolor = ch->desc->lastcolor;
    else
	oldcolor = AT_NONE;

    if ( AType == AT_NONE )
	return oldcolor;

    if ( !ch || !ch->desc || !ch->desc->ansi )
	return oldcolor;

    if ( AType >= sizeof( ansi_color ) / sizeof( ansi_color[0] ) )
	return oldcolor;

    if ( !IS_NPC( ch ) && AType < MAX_CLRMAP )
	AType = ch->pcdata->colormap[AType] % MAX_CLRMAP;

    ch->desc->lastcolor = AType;
    write_to_buffer( ch->desc, ansi_color[AType], 0 );
    return oldcolor;
}


/*
 * Get color set by previous set_char_color()
 */
int
get_char_color( CHAR_DATA *ch )
{
    if ( !ch || !ch->desc || !ch->desc->ansi )
	return AT_NONE;
    return ch->desc->lastcolor;
}


/*
 * Write to one char.
 */
void
send_to_char( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
        write_to_buffer( ch->desc, txt, strlen(txt) );
    return;
}



void
ch_printf( CHAR_DATA *ch, char *fmt, ... )
{
    char buf[MAX_STRING_LENGTH*2];      /* better safe than sorry */
    va_list args;

    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    va_end( args );
    send_to_char( buf, ch );
}

/*
 * Send a page to one char.
 */
void
page_to_char( const char *txt, CHAR_DATA *ch )
{
    if ( txt == NULL || ch->desc == NULL)
	return;

    if (ch->lines == 0 )
    {
	send_to_char(txt,ch);
	return;
    }

    ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
    strcpy(ch->desc->showstr_head,txt);
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string(ch->desc,"");
}


/*
 * string pager
 */
void
show_string( DESCRIPTOR_DATA *d, char *input )
{
    char               *start, *end;
    char                arg[MAX_INPUT_LENGTH];
    int                 lines = 0, pagelen;

    if ( !d ) return;

    /* Set the page length */
    /* ------------------- */

/*
    pagelen = d->original ? d->original->pcdata->pagelen
                          : d->character->pcdata->pagelen;
*/
    pagelen = d->original ? d->original->lines
                          : d->character->lines;

    /* Check for the command entered */
    /* ----------------------------- */

    one_argument( input, arg );

    switch( UPPER( *arg ) )
    {
        /* Show the next page */

        case '\0':
        case 'C': lines = 0;
                  break;

        /* Scroll back a page */

        case 'B': lines = -2 * pagelen;
                  break;

        /* Help for show page */

        case 'H': write_to_buffer( d, "B     - Scroll back one page.\n\r", 0 );
                  write_to_buffer( d, "C     - Continue scrolling.\n\r", 0 );
                  write_to_buffer( d, "H     - This help menu.\n\r", 0 );
                  write_to_buffer( d, "R     - Refresh the current page.\n\r",
                                   0 );
                  write_to_buffer( d, "Enter - Continue Scrolling.\n\r", 0 );
                  return;

        /* refresh the current page */

        case 'R': lines = -1 - pagelen;
                  break;

        /* stop viewing */

        default:  free_mem( d->showstr_head, strlen( d->showstr_head ) + 1 );
                  d->showstr_head  = NULL;
                  d->showstr_point = NULL;
                  return;
    }

    /* do any backing up necessary to find the starting point */
    /* ------------------------------------------------------ */

    if ( lines < 0 )
    {
        for( start= d->showstr_point; start > d->showstr_head && lines < 0;
             start-- )
            if ( *start == '\r' )
                lines++;
    }
    else
        start = d->showstr_point;

    /* Find the ending point based on the page length */
    /* ---------------------------------------------- */

    lines  = 0;

    for ( end= start; *end && lines < pagelen; end++ )
        if ( *end == '\r' )
            lines++;

    d->showstr_point = end;

    if ( end - start )
        write_to_buffer( d, start, end - start );

    /* See if this is the end (or near the end) of the string */
    /* ------------------------------------------------------ */

    for ( ; isspace( *end ); end++ );

    if ( !*end )
    {
        free_mem( d->showstr_head, strlen( d->showstr_head ) + 1 );
        d->showstr_head  = NULL;
        d->showstr_point = NULL;
    }

    return;
}


void
send_to_room( int color, ROOM_INDEX_DATA *pRoom, const char *txt, int min_position )
{
    CHAR_DATA *ch;
    int color_save;

    if ( pRoom == NULL || IS_NULLSTR( txt ) )
	return;

    for ( ch = pRoom->people; ch != NULL; ch = ch->next_in_room )
	if ( ch->position >= min_position )
	{
	    if ( color != AT_NONE )
	    {
		color_save = set_char_color( color, ch );
		send_to_char( txt, ch );
		set_char_color( color_save, ch );
	    }
	    else
		send_to_char( txt, ch );
	}
}


/* quick sex fixer */
void
fix_sex(CHAR_DATA *ch)
{
    if (ch->sex < 0 || ch->sex > 2)
    	ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}


void
act_color( int AType, const char *format, CHAR_DATA *ch,
		const void *arg1, const void *arg2, int type, int min_pos)
{
    int		  lastcolor;

    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *	to;
    CHAR_DATA *	vch = (CHAR_DATA *) arg2;
    OBJ_DATA *	obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *	obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *	point;
    char *	first_char;
    char *	buf_end;
    bool	at2 = FALSE;	/* will be set TRUE if arg2 is text */
    bool	colorcodes;

    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
        return;

    /* discard null rooms and chars */
    if (ch == NULL || ch->in_room == NULL)
	return;

    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
        if ( vch == NULL )
        {
            bug( "Act: null vch with TO_VICT.", 0 );
	    log_string( format );
            return;
        }

	if (vch->in_room == NULL)
	    return;

        to = vch->in_room->people;
    }

    for ( ; to != NULL; to = to->next_in_room )
    {
        if ( /* to->desc == NULL || */ to->position < min_pos )
            continue;

        if ( (type == TO_CHAR) && to != ch )
            continue;
        if ( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if ( type == TO_ROOM && to == ch )
            continue;
        if ( type == TO_NOTVICT && (to == ch || to == vch) )
            continue;

	if ( !is_same_world( ch, to ) )
	    continue;

        point   = buf;
        str     = format;
        while ( *str != '\0' )
        {
            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }
            ++str;

            if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' )
            {
		bug( "Act: missing arg2 for code %d.", *str );
		log_string( format );
		i = " <@@@> ";
            }
            else
            {
                switch ( *str )
                {
                default:  bugf( "Act: bad code %d.", *str );
			  log_string( format );
                          i = " <@@@> ";				break;
                /* Thx alex for 't' idea */
                case 't': i = (char *) arg1;				break;
                case 'T': i = (char *) arg2;	at2 = TRUE;		break;
                case 'n': i = PERS( ch,  to  );				break;
                case 'N': i = PERS( vch, to  );				break;
                case 'e': i = he_she  [URANGE( 0, ch ->sex, 2 )];	break;
                case 'E': i = he_she  [URANGE( 0, vch->sex, 2 )];	break;
                case 'm': i = him_her [URANGE( 0, ch ->sex, 2 )];	break;
                case 'M': i = him_her [URANGE( 0, vch->sex, 2 )];	break;
                case 's': i = his_her [URANGE( 0, ch ->sex, 2 )];	break;
                case 'S': i = his_her [URANGE( 0, vch->sex, 2 )];	break;

                case 'p':
                    i = can_see_obj( to, obj1 )
                            ? obj1->short_descr
                            : "something";
                    break;

                case 'P':
                    i = can_see_obj( to, obj2 )
                            ? obj2->short_descr
                            : "something";
                    break;

                case 'd':
                    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
                    {
                        i = "door";
                    }
                    else
                    {
                        strcpy( fname, (char *)arg2 );
                        i = fname;
                    }
                    break;
                }
            }

            ++str;
            if ( ( colorcodes = has_colcode( i ) ) )
            {
                const char *cp;
                cp = strrchr( i, '`' );
                if ( *(cp + 1) == 'X' )
                    colorcodes = FALSE;
            }
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
            if ( colorcodes )
            {
                *point++ = '`';
                *point++ = 'X';
            }
        }

        buf_end = point;
        *point++ = '\n';
        *point++ = '\r';
        *point = '\0';
	first_char = buf;
	while ( is_colcode( first_char ) )
	    first_char += 2;
        *first_char   = UPPER( *first_char );
	lastcolor = get_char_color( to );
	set_char_color( AType, to );
	if ( to->desc )
	    write_to_buffer( to->desc, buf, point - buf );
	set_char_color( lastcolor, to );
	if ( MOBtrigger )
	{
	    *buf_end = '\0';
	    mprog_act_trigger( buf, to, ch, obj1, at2 ? NULL : vch );
	}
    }

    MOBtrigger = TRUE;

    if ( IS_SET( ch->in_room->area->area_flags, AREA_ARENA )
    &&	 ch->in_room != ch->in_room->area->room_list
    &&	 type != TO_CHAR
    &&	 type != TO_VICT )
        act_arena( AType, format, ch, arg1, arg2, type, min_pos );

    return;
}


void
act_arena(  int AType, const char *format, CHAR_DATA *ch, const void *arg1,
            const void *arg2, int type, int min_pos )
{
    char		buf[MAX_STRING_LENGTH];
    char		fname[MAX_INPUT_LENGTH];
    CHAR_DATA *		to;
    CHAR_DATA *		vch;
    OBJ_DATA *		obj1;
    OBJ_DATA *		obj2;
    const char *	i;
    const char *	str;
    char *		buf_end;
    char *		first_char;
    char *		point;
    int			lastcolor;
    bool		at2;
    bool		colorcodes;

    if ( ( to = ch->in_room->area->room_list->people ) == NULL )
        return;

    vch  = (CHAR_DATA *)arg2;
    obj1 = (OBJ_DATA *)arg1;
    obj2 = (OBJ_DATA *)arg2;
    at2  = FALSE;

    for ( ; to != NULL; to = to->next_in_room )
    {
        if ( to->desc == NULL || to->position < min_pos )
            continue;

        point = buf;
        str = format;
        while ( *str != '\0' )
        {
            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }
            ++str;

            if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' )
            {
		bug( "Act: missing arg2 for code %d.", *str );
		log_string( format );
		i = " <@@@> ";
            }
            else
            {
                switch ( *str )
                {
                default:  bugf( "Act: bad code %d.", *str );
			  log_string( format );
                          i = " <@@@> ";				break;
                /* Thx alex for 't' idea */
                case 't': i = (char *) arg1;				break;
                case 'T': i = (char *) arg2;	at2 = TRUE;		break;
                case 'n': i = PERS( ch,  to  );				break;
                case 'N': i = PERS( vch, to  );				break;
                case 'e': i = he_she  [URANGE( 0, ch ->sex, 2 )];	break;
                case 'E': i = he_she  [URANGE( 0, vch->sex, 2 )];	break;
                case 'm': i = him_her [URANGE( 0, ch ->sex, 2 )];	break;
                case 'M': i = him_her [URANGE( 0, vch->sex, 2 )];	break;
                case 's': i = his_her [URANGE( 0, ch ->sex, 2 )];	break;
                case 'S': i = his_her [URANGE( 0, vch->sex, 2 )];	break;

                case 'p':
                    i = can_see_obj( to, obj1 )
                            ? obj1->short_descr
                            : "something";
                    break;

                case 'P':
                    i = can_see_obj( to, obj2 )
                            ? obj2->short_descr
                            : "something";
                    break;

                case 'd':
                    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
                    {
                        i = "door";
                    }
                    else
                    {
                        strcpy( fname, (char *)arg2 );
                        i = fname;
                    }
                    break;
                }
            }

            ++str;
            if ( ( colorcodes = has_colcode( i ) ) )
            {
                const char *cp;
                cp = strrchr( i, '`' );
                if ( *(cp + 1) == 'X' )
                    colorcodes = FALSE;
            }
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
            if ( colorcodes )
            {
                *point++ = '`';
                *point++ = 'X';
            }
        }

        buf_end = point;
        *point++ = '\n';
        *point++ = '\r';
        *point = '\0';
	first_char = buf;
	while ( is_colcode( first_char ) )
	    first_char += 2;
        *first_char   = UPPER( *first_char );
	write_to_buffer( to->desc, "[Arena] ", 0 );
	lastcolor = get_char_color( to );
	set_char_color( AType, to );
	write_to_buffer( to->desc, buf, point - buf );
	set_char_color( lastcolor, to );
    }

    return;
}


FILE *file_open(const char *path, const char *mode)     
{            
    FILE *fp = NULL;     
    char buf[MAX_STRING_LENGTH];     

    /* If fpReserve is open, attempt to close it. */     
    if (fpReserve)     
    {                             
        /* If fclose returns an error, catch it and abort. */     
        if (fclose(fpReserve))                               
        {     
            /* Error occured closing fpReserve.                                                 
             * If the error is ENOENT (No such file or directory)                               
             * and we are on unix, ignore it. This is because closing                                 
             * a file that was opened on /dev/null nearly always returns                              
             * this error, so it's safe to ignore.                                               
             */                                                                                  
#ifdef unix                                                                                      
            if( errno != ENOENT )                                                                
#endif                                                                                           
            {                                                                                    
                sprintf(buf, "Error (%d) closing fpReserve:", errno);     
                perror(buf);                                                                                               
                abort();                                                                                                   
            }                                                                                                              
        }                                                                                                                  
        fpReserve = NULL;                                                                                                  
    }                                                                                                                      
                                                                                                                           
    if (!(fp = fopen(path,mode)))                                                                                          
    {                                                                                                                      
        sprintf( buf, "file_open(%s,%s):", path, mode );                                                                   
        perror(buf);                                                                                                       
    }                                                                                                                      
                                                                                                                           
    return fp;                                                                                                             
}     
                                                                                                                           
void file_close( FILE *fp )                                                                                                
{                                                                                                                          
    /* Make sure fp is at least not null */                                                                                
    if( !fp )                                                                                                              
        return;                                                                                                            
                                                                                                                           
    /* Attempt to close the file. */                                                                                       
    if( fclose(fp) )                                                                                                       
    {                                                                                                                      
        /* Error occured closing the file. Let's print the error message and exit. */                                      
        perror( "file_close():" );
        abort();                                                                                                           
    }                                                                                                                      
                                                                                                                           
    /* Attempt to reopen fpReserve, if it's not already open */                                                            
    if (!fpReserve)                                                                                                        
    {                                                                                                                      
        if (!(fpReserve = fopen(NULL_FILE, "r")))                                                                          
        {                                                                                                                  
            /* Error occured opening fpReserve. Let's print the error message and exit. */                                 
            perror( "Error opening fpReserve:" );                                                                          
            abort();                                                                                                       
        }                                                                                                                  
    }                                                                                                                      
                                                                                                                           
    return;                                                                                                                
}                                                                                                                          
                                                                                                                           
bool file_exists( const char *path )                                                                                       
{                                                                                                                          
    FILE *fp = NULL;                                                                                                       
    char buf[MAX_STRING_LENGTH];                                                                                           
                                                                                                                           
    /* If fpReserve is open, attempt to close it. */                                                                       
    if (fpReserve)                                                                                                         
    {                                                                                                                      
        /* If fclose returns an error, catch it and abort. */                                                              
        if (fclose(fpReserve))                                                                                             
        {                                                                                                                  
            /* Error occured closing fpReserve.                                                                            
             * If the error is ENOENT (No such file or directory)                                                          
             * and we are on unix, ignore it. This is because closing                                                      
             * a file that was opened on /dev/null nearly always returns                                                   
             * this error, so it's safe to ignore.                                                                         
             */                                                                                                            
#ifdef unix                                                                                                                
            if( errno != ENOENT )                                                                                          
#endif                                                                                                                     
            {                                                                                                              
                sprintf(buf, "Error (%d) closing fpReserve:", errno);                                                      
                perror(buf);                                                                                               
                abort();                                                                                                   
            }                                                                                                              
        }
        fpReserve = NULL;
    }
                                                                                                                           
    if((fp = fopen(path, "r")) != NULL)
    {
  file_close(fp);                                                                                                    
        return TRUE;                                                                                                       
    }                                                                                                                      
                                                                                                                           
    return FALSE;                                                                                                          
}                 
void
count_players( void )
{
    int			julian;
    int			count = 0;
    DESCRIPTOR_DATA *	d;
    FILE *		fp;
    time_t		now;

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
	    count++;
    }
    now = time( NULL );
    julian = localtime( &now ) -> tm_yday;
    if ( julian != today || count > most_on )
    {
	if ( julian != today )
	{
	    most_yesterday = most_on;
	    today = julian;
	    most_on = 0;
	}
	if ( count > most_on )
	    most_on = count;
	fclose( fpReserve );
	if ( ( fp = fopen ( SYSTEM_DIR MAX_PLAYERS, "w" ) ) != NULL )
	{
	    fprintf( fp, "%d %d %d\n", julian, most_on, most_yesterday );
	    fclose( fp );
	}
	fpReserve = fopen( NULL_FILE, "r" );
    }
}


/* Recover from a hotboot - load players */
void
hotboot_recover( void )
{
    DESCRIPTOR_DATA *	d;
    DESCRIPTOR_DATA *	d_last;
    EVENT_DATA *	event;
    CHAR_DATA *		vch;
    FILE *		fp;
    char		name[SHORT_STRING_LENGTH];
    char		host[MAX_STRING_LENGTH];
    char		ip[MAX_INPUT_LENGTH];
    int			ansi;
    int			desc;
    int			retval;
    bool		fOld;

    log_printf( "Hotboot recovery initiated" );

    fp = fopen( HOTBOOT_FILE, "r" );

    if ( !fp ) /* there are some descriptors open which will hang forever then ? */
    {
	perror( "hotboot_recover:fopen" );
	log_printf( "Hotboot file not found. Exiting.\n\r" );
	exit( 1 );
    }

/*  unlink( HOTBOOT_FILE );  In case something crashes - doesn't prevent reading	*/
    rename( HOTBOOT_FILE, HOTBOOT_SAVE ); /* save it for posterity instead of unlinking it */
    d_last = NULL;

    for ( ; ; )
    {
	retval = fscanf( fp, "%d %s %s %s %d\n", &desc, name, ip, host, &ansi );
	if ( retval < 5 || desc == -1 )
	    break;

	/* Write something, and check if it goes error-free */		
	if ( !write_to_descriptor( desc,
	"\n\rEverything is immediately engulfed in thick darkness then utter silence.\n\r",
	0, FALSE ) )
	{
	    close( desc ); /* nope */
	    continue;
	}

	d = new_descriptor( );
	d->descriptor = desc;

	d->host = str_dup( host );
	d->ip = str_dup( ip );
	d->ansi = ( ansi != 0 );
//	d->next = descriptor_list;
//	descriptor_list = d;
	d->next = NULL;
	if ( d_last )
	{
	    d_last->next = d;
	    d_last = d;
	}
	else
	{
	    descriptor_list = d;
	    d_last = d;
	}
	d->connected = CON_HOTBOOT_RECOVER; /* -15, so close_socket frees the char */

	event = new_event( );
	event->type = EVENT_DESC_IDLE;
	event->fun = event_desc_idle;
	add_event_desc( event, d, 60 * PULSE_PER_SECOND );

	/* Now, find the pfile */

	fOld = load_char_obj( d, name );

	if ( !fOld ) /* Player file not found?! */
	{
	    write_to_descriptor( desc, "\n\rSomehow, your character was lost in the hotboot. Sorry.\n\r", 0, d->ansi );
	    close_socket( d );			
	}
	else /* ok! */
	{
	    ROOM_INDEX_DATA *pRoom;

	    write_to_descriptor( desc,
"\n\rThe darkness recedes like a curtain, but something is strangely different.\n\r",
	    0, d->ansi );

	    /* Just In Case */
	    if ( !d->character->in_room )
		d->character->in_room = get_room_index( ROOM_VNUM_TEMPLE );

	    /* Insert in the char_list */
	    d->character->next = char_list;
	    char_list = d->character;

	    if ( ( pRoom = get_vroom_index( d->character->room_vnum ) ) == NULL )
		pRoom = get_room_index( ROOM_VNUM_LIMBO );
	    char_to_room( d->character, pRoom );
	    do_look( d->character, "auto" );
	    for ( vch = d->character->in_room->people; vch; vch = vch->next )
	    {
		if (    d->character != vch
		     && d->character->in_room == vch->in_room
		     && can_see( vch, d->character ) )
		{
		    act ( "$n materializes!", d->character, NULL, vch, TO_VICT );
		}
	    }
	    d->connected = CON_PLAYING;
	    strip_event_desc( d, EVENT_DESC_LOGIN );

	    if ( d->character->pet != NULL )
	    {
		char_to_room( d->character->pet, d->character->in_room );
		act( "$n materializes!", d->character->pet, NULL, NULL, TO_ROOM );
	    }
	    if ( d->character->mount != NULL )
	    {
		char_to_room( d->character->mount, d->character->in_room );
		act( "$n materializes!", d->character->mount, NULL, NULL, TO_ROOM );
		add_follower( d->character->mount, d->character );
		do_mount( d->character, d->character->mount->name );
	    }
	    if ( d->character->pcdata->deathstate == DEATH_SEARCH )
	        create_rift_mob( d->character );
	}

    }

   fclose( fp );
}


void
update_userlist( CHAR_DATA *ch, bool fLogon )
{
    USERLIST_DATA *	pTmp;
    USERLIST_DATA *	pUser;

    if ( IS_NPC( ch ) )
	return;

    pUser = user_lookup( ch->name );
    if ( get_trust( ch ) < 1 )
    {
	if ( pUser == NULL )
	    return;
	else
	{
	    if ( user_first == pUser )
	    {
		user_first = pUser->next;
		if ( user_last == pUser )
		    user_last = NULL;
	    }
	    else
	    {
		for ( pTmp = user_first; pTmp != NULL; pTmp = pTmp->next )
		{
		    if ( pTmp->next == pUser )
		    {
			pTmp->next = pUser->next;
			if ( user_last == pUser )
			    user_last = pTmp;
			break;
		    }
		}
	    }
	}

	free_user_rec( pUser );
	queue_userlist_save( );
	return;
    }
    else
    {
	if ( pUser == NULL )
	{
	    pUser = new_user_rec( );
	    pUser->name = str_dup( ch->name );

	    if ( user_first == NULL
	    ||	 strcasecmp( pUser->name, user_first->name ) < 0 )
	    {
		pUser->next = user_first;
		user_first = pUser;
		if ( user_last == NULL )
		{
		    user_last = pUser;
		}
	    }
	    else if ( strcasecmp( pUser->name, user_last->name ) > 0 )
	    {
		user_last->next = pUser;
		pUser->next = NULL;
		user_last = pUser;
	    }
	    else
	    {
		for ( pTmp = user_first; pTmp != NULL; pTmp = pTmp->next )
		{
		    if ( strcasecmp( pUser->name, pTmp->next->name ) < 0 )
		    {
			pUser->next = pTmp->next;
			pTmp->next = pUser;
			break;
		    }
		}
	    }
	}
    }

    pUser->level = IS_AVATAR( ch ) ? ch->pcdata->avatar->level : ch->level;
    pUser->played = ch->played + current_time - ch->logon;
    pUser->logcount = ch->pcdata->logcount;
    free_string( pUser->race );
    pUser->race = str_dup( race_table[ch->race].who_name );
    free_string( pUser->class );
    pUser->class = str_dup( class_table[ch->class].who_name );

    if ( pUser->id == 0 )
	pUser->id = ch->id;

    if ( fLogon || pUser->lastlogon == 0 )
	pUser->lastlogon = ch->logon;
    if ( fLogon && ch->desc != NULL )
    {
	free_string( pUser->host );
	pUser->host = str_dup( ch->desc->host );
    }

    queue_userlist_save( );
    return;
}


void
queue_userlist_save( void )
{
    EVENT_DATA *	pEvent;

    if ( get_event_game( EVENT_GAME_SAVE_USERLIST ) != NULL )
	return;

    pEvent = new_event( );
    pEvent->fun = event_save_userlist;
    pEvent->type = EVENT_GAME_SAVE_USERLIST;
    add_event_game( pEvent, 5 * PULSE_PER_SECOND );
    return;
}


void
save_userlist( void )
{
    FILE *	fp;
    USERLIST_DATA *	wUser;

    fclose( fpReserve );
    if ( ( fp = fopen( TEMP_DIR USER_FILE, "w" ) ) == NULL )
    {
	fpReserve = fopen( NULL_FILE, "r" );
	bugf( "Update_userlist: unable to open %s", TEMP_DIR USER_FILE );
	return;
    }

    for ( wUser = user_first; wUser != NULL; wUser = wUser->next )
    {
	fprintf( fp, "#USER\n" );
	fprintf( fp, "Name %s\n",	wUser->name );
	fprintf( fp, "Host '%s'\n",	wUser->host );
	fprintf( fp, "Id %ld\n",	wUser->id );
	fprintf( fp, "Last %ld\n",	wUser->lastlogon );
	fprintf( fp, "LogC %d\n",	wUser->logcount );
	fprintf( fp, "Lvl %d\n",	wUser->level );
	fprintf( fp, "Race '%s'\n",     wUser->race );
	fprintf( fp, "Class '%s'\n",    wUser->class );
	fprintf( fp, "Plyd %d\n",	wUser->played );
	fprintf( fp, "End\n\n" );
    }

    fprintf( fp, "#END\n" );
    fclose( fp );
    rename( TEMP_DIR USER_FILE, SYSTEM_DIR USER_FILE );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


void
load_startup_time( void )
{
    FILE *fp;

    if ( ( fp = fopen( SYSTEM_DIR BOOTTIME_FILE, "r" ) ) != NULL )
    {
	if (! fscanf( fp, "%ld", &startup_time ) )
	    bugf( "Load_startup_time: unable to read boottime file.");
	fclose( fp );
    }

    return;
}


void
save_startup_time( void )
{
    FILE *fp;

    if ( ( fp = fopen( SYSTEM_DIR BOOTTIME_FILE, "w" ) ) != NULL )
    {
	fprintf( fp, "%ld %s", startup_time, ctime( &startup_time ) );
	fclose( fp );
    }

    return;
}


void
add_keepalive( CHAR_DATA *ch )
{
    EVENT_DATA *        event;

    if ( ch->desc == NULL )
        return;

    event = new_event( );
    event->type = EVENT_DESC_KEEPALIVE;
    event->fun = event_desc_keepalive;
    add_event_desc( event, ch->desc, KEEPALIVE_TIMER );
}


bool
event_desc_idle( EVENT_DATA *event )
{
    EVENT_DATA *ev_new;

    event->owner.d->idle++;

    ev_new = new_event( );
    ev_new->type = EVENT_DESC_IDLE;
    ev_new->fun = event_desc_idle;
    add_event_desc( ev_new, event->owner.d, 60 * PULSE_PER_SECOND );

    return FALSE;	/* let handler remove this event */
}


bool
event_desc_keepalive( EVENT_DATA *event )
{
    EVENT_DATA *        ev_new;

    write_to_descriptor( event->owner.d->descriptor, keepalive_str, 0, FALSE );

    ev_new = new_event( );
    ev_new->type = EVENT_DESC_KEEPALIVE;
    ev_new->fun = event_desc_keepalive;
    add_event_desc( ev_new, event->owner.d, KEEPALIVE_TIMER );

    return FALSE;       /* Handler will remove event */
}


bool
event_desc_login( EVENT_DATA *event )
{
    close_socket( event->owner.d );
    return TRUE;
}


bool
event_save_userlist( EVENT_DATA *event )
{
    save_userlist( );
    return FALSE;
}


void
webwho_socket( void )
{
    int                 fd;
    unsigned            size;
    struct sockaddr_in  sock;

    size = sizeof( sock );
    if ( ( fd = accept( whosock, (struct sockaddr *)&sock, &size ) ) < 0 )
        return;
    web_who( fd );
    close( fd );
    return;
}

