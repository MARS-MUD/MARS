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


#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"


#define DECLARE_DEBUG_FUN( fun ) static DO_FUN fun

struct debug_type
{
    char * const	name;
    DO_FUN *		do_fun;
};

DECLARE_DEBUG_FUN( d_demo	);
DECLARE_DEBUG_FUN( d_foo	);

static const struct debug_type debug_table[] =
{
    {	"demo",		d_demo	        },
    {	"foo",		d_foo		},	/* a test to see if hotboot is in fact working. */
    {   "",             NULL            }
};

void
do_debug( CHAR_DATA *ch, char *argument )
{
    int  cmd;
    char command[ MAX_INPUT_LENGTH ];

    if ( !IS_CODER( ch ) || *argument == '\0' )
    {
	send_to_char( "Debug what?\n\r", ch );
	return;
    }

    argument = one_argument( argument, command );

    for ( cmd = 0; *debug_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, debug_table[cmd].name ) )
	{
	    (*debug_table[cmd].do_fun) ( ch, argument );
	    return;
	}
    }

    send_to_char( "Debug what?\n\r", ch );
    return;
}


static void
d_demo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "The debug command is working.\n\r", ch );
    return;
}

static void
d_foo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "The foo command is working.  Now you can take it out again.\n\r", ch );
    return;
}

