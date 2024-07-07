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


#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "lookup.h"
#include "recycle.h"
#include "tables.h"

/*  Defines by Narn for new mudprog parsing, used as 
    return values from mprog_do_command. */
#define COMMANDOK    1
#define IFTRUE       2
#define IFFALSE      3
#define ORTRUE       4
#define ORFALSE      5
#define FOUNDELSE    6
#define FOUNDENDIF   7
#define IFIGNORED    8
#define ORIGNORED    9

/* Ifstate defines, used to create and access ifstate array
   in mprog_driver. */
#define MAX_IFS     40		/* should always be generous */
#define IN_IF        0
#define IN_ELSE      1
#define DO_IF        2
#define DO_ELSE      3

#define MAX_IF_ARGS	 6
#define MAX_PROG_NEST   20

#define isoperator(c)	((c)=='='||(c)=='<'||(c)=='>'||(c)=='!'||(c)=='&'\
			||(c)=='|')

/*
 * Local functions
 */
static void debug_log_string	args( ( const char *str ) );
bool	eprog_percent_check	args( ( CHAR_DATA *smob, CHAR_DATA *actor,
                                        EXIT_DATA *pExit, void *vo,
                                        bitvector type ) );
CHAR_DATA *eset_supermob	args( ( EXIT_DATA *pExit, ROOM_INDEX_DATA *pRoom ) );
int	mprog_do_command	args( ( char *cmnd, CHAR_DATA *mob,
					CHAR_DATA *actor, OBJ_DATA *obj,
					void *vo, CHAR_DATA *rndm,
					bool ignore, bool ignore_ors ) );
int	mprog_do_ifcheck	args( ( char* ifcheck, CHAR_DATA* mob,
					CHAR_DATA* actor, OBJ_DATA* obj,
					void* vo, CHAR_DATA* rndm ) );
void	mprog_driver		args( ( char* com_list, CHAR_DATA* mob,
					CHAR_DATA* actor, OBJ_DATA* obj,
					void* vo, bool single_step ) );
bool	mprog_keyword_check	args( ( const char *argu, const char *argl ) );
char *	mprog_next_command	args( ( char *clist ) );
bool	mprog_seval		args( ( const char *lhs, char *opr, char *rhs,
					CHAR_DATA *mob ) );
void	mprog_time_check	args( ( CHAR_DATA *mob, CHAR_DATA *actor,
					OBJ_DATA *obj, void *vo,
					bitvector type ) );
void	mprog_translate		args( ( char ch, char* t, CHAR_DATA* mob,
					CHAR_DATA* actor, OBJ_DATA* obj,
					void* vo, CHAR_DATA* rndm ) );
bool	mprog_veval		args( ( int lhs, char *opr, int rhs,
					CHAR_DATA *mob ) );

bool	oprog_percent_check	args( ( CHAR_DATA *smob, CHAR_DATA *actor,
					OBJ_DATA *obj, void *vo, bitvector type ) );
CHAR_DATA *oset_supermob	args( ( OBJ_DATA *obj ) );
void	release_supermob	args( ( CHAR_DATA *smob ) );
bool	rprog_percent_check	args( ( CHAR_DATA *smob, CHAR_DATA *actor,
					ROOM_INDEX_DATA *room, void *vo,
					bitvector type ) );
bool	rprog_roomlist_check	args( ( CHAR_DATA *smob,
					ROOM_INDEX_DATA *here,
					ROOM_INDEX_DATA *there,
					bitvector type ) );
bool	rprog_wordlist_check	args( ( char *arg, CHAR_DATA *smob,
					CHAR_DATA *actor,
					ROOM_INDEX_DATA *room, void *vo,
					bitvector type ) );
CHAR_DATA *rset_supermob	args( ( ROOM_INDEX_DATA *pRoom ) );


/*
 * This function is called from mprog_driver, once for each line in a mud prog.
 * This function checks what the line is, executes if/or checks and calls
 * interpret to perform the the commands.  Written by Narn, Dec 95.
 */
int
mprog_do_command( char *cmnd, CHAR_DATA *mob, CHAR_DATA *actor, 
                  OBJ_DATA *obj, void *vo, CHAR_DATA *rndm, 
                  bool ignore, bool ignore_ors )
{
    char  firstword[MAX_INPUT_LENGTH];
    char *ifcheck;
    char  buf[ MAX_INPUT_LENGTH ];
    char  tmp[ MAX_INPUT_LENGTH ];
    char *point;
    char *str;
    char *i;
    int   validif;
    int   vnum;

    /* Isolate the first word of the line, it gives us a clue what
       we want to do. */
    ifcheck = one_argument( cmnd, firstword );

    if ( !str_cmp( firstword, "if" ) )
    {
	/*
	   Ok, we found an if.  According to the boolean 'ignore', either
	   ignore the ifcheck and report that back to mprog_driver or do
	   the ifcheck and report whether it was successful.
	*/
	if ( ignore )
	    return IFIGNORED;
	else
	    validif = mprog_do_ifcheck( ifcheck, mob, actor, obj, vo, rndm );

	if ( validif == 1 )
	    return IFTRUE;

	if ( validif == 0 )
	    return IFFALSE;

	return BERR;
    }

    if ( !str_cmp( firstword, "or" ) )
    {
	/*
	   Same behavior as with ifs, but use the boolean 'ignore_ors' to
	   decide which way to go.
	*/
	if ( ignore_ors )
	    return ORIGNORED;
	else
	    validif = mprog_do_ifcheck( ifcheck, mob, actor, obj, vo, rndm );

	if ( validif == 1 )
	    return ORTRUE;

	if ( validif == 0 )
	    return ORFALSE;

	return BERR;
    }

    /* For else and endif, just report back what we found.  Mprog_driver
       keeps track of logiclevels. */
    if ( !str_cmp( firstword, "else" ) )
    {
	return FOUNDELSE;
    }

    if ( !str_cmp( firstword, "endif" ) )
    {
	return FOUNDENDIF;
    }

    /*
       Ok, didn't find an if, an or, an else or an endif.  
       If the command is in an if or else section that is not to be 
       performed, the boolean 'ignore' is set to true and we just 
       return.  If not, we try to execute the command.
    */

    if ( ignore )
	return COMMANDOK;

    /* If the command is 'break', that's all folks. */
    if ( !str_cmp( firstword, "break" ) )
	return BERR;

    vnum = mob->pIndexData->vnum;
    point   = buf;
    str     = cmnd;

    /* This chunk of code taken from mprog_process_cmnd. */
    while ( *str != '\0' )
    {
	if ( *str != '$' )
	{
	    *point++ = *str++;
	    continue;
	}
	str++;
	mprog_translate( *str, tmp, mob, actor, obj, vo, rndm );
	i = tmp;
	++str;
	while ( ( *point = *i ) != '\0' )
	    ++point, ++i;
    }
    *point = '\0';

    interpret( mob, buf );  

    /*
       If the mob is mentally unstable and does things like fireball
       itself, let's make sure it's still alive.
    */
    if ( char_died( mob ) )
    {
	bug( "Mob died while executing program, vnum %d.", vnum );
	return BERR;
    }

    return COMMANDOK;
}


/*
 * This function performs the evaluation of the if checks.  It is
 * here that you can add any ifchecks which you so desire. Hopefully
 * it is clear from what follows how one would go about adding your
 * own. The syntax for an if check is: ifcheck ( arg ) [opr val]
 * where the parenthesis are required and the opr and val fields are
 * optional but if one is there then both must be. The spaces are all
 * optional. The evaluation of the opr expressions is farmed out
 * to reduce the redundancy of the mammoth if statement list.
 * If there are errors, then return BERR otherwise return boolean 1,0
 */
int
mprog_do_ifcheck( char *ifcheck, CHAR_DATA *mob, CHAR_DATA *actor,
		      OBJ_DATA *obj, void *vo, CHAR_DATA *rndm )
{
    int		argc = 0;
    char *	argv[MAX_IF_ARGS];
    char	buf[MAX_STRING_LENGTH];
    char *	chck;
    CHAR_DATA *	chkchar = NULL;
    OBJ_DATA *	chkobj = NULL;
    char *	cvar;
    bool	fDone;
    bool	fInvert = FALSE;
    char	opr[MAX_INPUT_LENGTH];
    char * 	p = buf;
    char *	q;
    int		rhsvl = 0;
    char *	rval = "";
    int		i;
/*    int		lhsvl;  I think we're gonna need this someday so I'll just comment it out for now. */

    if ( !*ifcheck )
    {
	progbug( mob, "Null ifcheck" );
	return BERR;
    }

    /*
     * New parsing by Thoric to allow for multiple arguments inside the
     * brackets, ie: if leveldiff($n, $i) > 10
     * It's also smaller, cleaner and probably faster
     */
    for ( i = 0; i < MAX_IF_ARGS; i++ )
        argv[i] = NULL;

    strcpy( buf, ifcheck );
    opr[0] = '\0';
    while ( isspace( *p ) )
	++p;

    if ( *p == '!' )
    {
	fInvert = TRUE;
	p++;
	while ( isspace( *p ) )
	    p++;
    }
    else if ( !strncasecmp( p, "not ", 4 ) )
    {
	fInvert = TRUE;
	p += 4;
	while ( isspace( *p ) )
	    p++;
    }

    argv[argc++] = p;
    while ( isalnum( *p ) )
	++p;
    while ( isspace(*p) )
	*p++ = '\0';
    if ( *p != '(' )
    {
	progbug( mob, "Ifcheck Syntax error (missing left bracket)" );
	return BERR;
    }

    *p++ = '\0';
    /* Need to check for spaces or if name( $n ) isn't legal --Shaddai */
    while ( isspace( *p ) )
	*p++ = '\0';
    fDone = FALSE;

    for ( ; ; )
    {
	argv[argc++] = p;
	while ( *p == '$' || isalnum(*p) )
	    ++p;
	while ( isspace(*p) )
	    *p++ = '\0';
	switch( *p )
	{
	    case ',':
		*p++ = '\0';
		while ( isspace( *p ) )
		    *p++ = '\0';
		if ( argc >= MAX_IF_ARGS )
		{
		    while ( *p && *p != ')' )
			++p;
		    if ( *p )
			*p++ = '\0';
		    while ( isspace(*p) )
			*p++ = '\0';
		    fDone = TRUE;;
		}
		break;
	    case ')':
		*p++ = '\0';
		while ( isspace(*p) ) *p++ = '\0';
	    	fDone = TRUE;;
	    	break;
	    default:
		progbug( mob, "Ifcheck Syntax warning (missing right bracket)" );
		fDone = TRUE;
		break;
	}
	if ( fDone )
	    break;
    }    

    q = p;
    while ( isoperator(*p) ) ++p;
    strncpy(opr, q, p-q);
    opr[p-q] = '\0';
    while ( isspace(*p) ) *p++ = '\0';
    rval = p;
    /*
    while ( *p && !isspace(*p) ) ++p;
    */
    while ( *p ) ++p;
    *p = '\0';

    chck = argv[0] ? argv[0] : "";
    cvar = argv[1] ? argv[1] : "";

    /*
     * chck contains check, cvar is the variable in the (), opr is the
     * operator if there is one, and rval is the value if there was an
     * operator.
     */
    if ( cvar[0] == '$' )
    {
	switch(cvar[1])
	{
	    case 'i':	chkchar = mob;			break;
	    case 'n':	chkchar = actor;		break;
	    case 't':	chkchar = (CHAR_DATA *)vo;	break;
	    case 'r':	chkchar = rndm;			break;
	    case 'o':	chkobj = obj;			break;
	    case 'p':	chkobj = (OBJ_DATA *)vo;	break;
	    case 'q':	chkchar = mob->qmem;		break;
	    default:
		progbug( mob, "Bad argument '%c' to '%s'", cvar[0], chck );
		return BERR;
	}
	if ( chkchar == NULL && chkobj == NULL
	&&   str_cmp( chck, "hour" )
	&&   str_cmp( chck, "isvalid" )
	&&   str_cmp( chck, "month" )
	&&   str_cmp( chck, "sector" ) )
	    return BERR;
    }

    if ( !str_cmp( chck, "cansee" ) )
    {
	if ( chkchar && can_see( mob, chkchar ) )
	    return fInvert ^ TRUE;
	return fInvert ^ FALSE;
    }

    if ( !str_cmp( chck, "clan" ) )
    {
	if ( chkchar )
	{
	    if ( chkchar->clan != NULL )
		return fInvert ^ mprog_seval( chkchar->clan->name, opr, rval, mob );
	    else
		return fInvert ^ FALSE;
	}
	progbug( mob, "Bad argument to 'clan'" );
	return BERR;
    }

    if ( !str_cmp( chck, "class" ) )
    {
	if ( chkchar )
	{
	    if ( !IS_NPC( chkchar ) )
		return fInvert ^ mprog_seval( class_table[chkchar->class].name, opr, rval, mob );
	    else
		return fInvert ^ FALSE;	/* might want to check mob class bits here */
	}
	progbug( mob, "Bad argument to 'class'" );
	return BERR;
    }

    if ( !str_cmp( chck, "countchars" ) )
    {
        ROOM_INDEX_DATA *	room = NULL;
        OBJ_DATA *		obj;
        CHAR_DATA *		rch;
        int			count;

        if ( chkchar != NULL )
            room = chkchar->in_room;
        else if ( chkobj != NULL )
        {
            for ( obj = chkobj; obj->in_obj != NULL; obj = obj->in_obj )
                ;
            if ( obj->in_room != NULL )
                room = obj->in_room;
            else if ( obj->carried_by != NULL )
                room = obj->carried_by->in_room;
        }
        else if ( is_number( cvar ) )
            room = get_room_index( atoi( cvar ) );

        if ( room == NULL )
        {
            progbug( mob, "Room not found in 'countchars'" );
            return BERR;
        }

        if ( IS_NULLSTR( opr ) )
        {
            progbug( mob, "Missing operator in 'countchars'" );
            return BERR;
        }
        if ( !is_number( rval ) )
        {
            progbug( mob, "Bad right hand value in 'countchars'" );
            return BERR;
        }

        count = 0;
        for ( rch = room->people; rch != NULL; rch = rch->next_in_room )
            count++;

        return fInvert ^ mprog_veval( count, opr, atoi( rval ), mob );
    }

    if ( !str_cmp( chck, "countmobs" ) )
    {
        ROOM_INDEX_DATA *	room = NULL;
        OBJ_DATA *		obj;
        CHAR_DATA *		rch;
        int			count;

        if ( chkchar != NULL )
            room = chkchar->in_room;
        else if ( chkobj != NULL )
        {
            for ( obj = chkobj; obj->in_obj != NULL; obj = obj->in_obj )
                ;
            if ( obj->in_room != NULL )
                room = obj->in_room;
            else if ( obj->carried_by != NULL )
                room = obj->carried_by->in_room;
        }
        else if ( is_number( cvar ) )
            room = get_room_index( atoi( cvar ) );

        if ( room == NULL )
        {
            progbug( mob, "Room not found in 'countmobs'" );
            return BERR;
        }

        if ( IS_NULLSTR( opr ) )
        {
            progbug( mob, "Missing operator in 'countmobs'" );
            return BERR;
        }
        if ( !is_number( rval ) )
        {
            progbug( mob, "Bad right hand value in 'countmobs'" );
            return BERR;
        }

        count = 0;
        for ( rch = room->people; rch != NULL; rch = rch->next_in_room )
            if ( IS_NPC( rch ) )
                count++;

        return fInvert ^ mprog_veval( count, opr, atoi( rval ), mob );
    }

    if ( !str_cmp( chck, "countplayers" ) )
    {
        ROOM_INDEX_DATA *	room = NULL;
        OBJ_DATA *		obj;
        CHAR_DATA *		rch;
        int			count;

        if ( chkchar != NULL )
            room = chkchar->in_room;
        else if ( chkobj != NULL )
        {
            for ( obj = chkobj; obj->in_obj != NULL; obj = obj->in_obj )
                ;
            if ( obj->in_room != NULL )
                room = obj->in_room;
            else if ( obj->carried_by != NULL )
                room = obj->carried_by->in_room;
        }
        else if ( is_number( cvar ) )
            room = get_room_index( atoi( cvar ) );

        if ( room == NULL )
        {
            progbug( mob, "Room not found in 'countplayers'" );
            return BERR;
        }

        if ( IS_NULLSTR( opr ) )
        {
            progbug( mob, "Missing operator in 'countplayers'" );
            return BERR;
        }
        if ( !is_number( rval ) )
        {
            progbug( mob, "Bad right hand value in 'countplayers'" );
            return BERR;
        }

        count = 0;
        for ( rch = room->people; rch != NULL; rch = rch->next_in_room )
            if ( !IS_NPC( rch ) )
                count++;

        return fInvert ^ mprog_veval( count, opr, atoi( rval ), mob );
    }

    if ( !str_cmp( chck, "doorclosed" ) )
    {
        int		dir;
        EXIT_DATA *	exit;

        if ( chkchar == NULL )
        {
            progbug( mob, "Bad first argument to \"doorclosed\"" );
            return BERR;
        }
        if ( IS_NULLSTR( argv[2] ) )
        {
            progbug( mob, "Missing second argument to 'doorclosed'" );
            return BERR;
        }
        if ( ( dir = dir_lookup( argv[2] ) ) == DIR_NONE )
        {
            progbug( mob, "Bad second argument \"%s\" to 'doorclosed'", argv[2] );
            return BERR;
        }

        if ( chkchar->in_room == NULL
        ||   ( exit = chkchar->in_room->exit[dir] ) == NULL
        ||   !IS_SET( exit->exit_info, EX_CLOSED ) )
            return fInvert ^ FALSE;
        else
            return fInvert ^ TRUE;
    }

    if ( !str_cmp( chck, "dooropen" ) )
    {
        int		dir;
        EXIT_DATA *	exit;

        if ( chkchar == NULL )
        {
            progbug( mob, "Bad first argument to \"dooropen\"" );
            return BERR;
        }
        if ( IS_NULLSTR( argv[2] ) )
        {
            progbug( mob, "Missing second argument to 'dooropen'" );
            return BERR;
        }
        if ( ( dir = dir_lookup( argv[2] ) ) == DIR_NONE )
        {
            progbug( mob, "Bad second argument \"%s\" to 'dooropen'", argv[2] );
            return BERR;
        }

        if ( chkchar->in_room == NULL
        ||   ( exit = chkchar->in_room->exit[dir] ) == NULL
        ||   IS_SET( exit->exit_info, EX_CLOSED ) )
            return fInvert ^ FALSE;
        else
            return fInvert ^ TRUE;
    }

    if ( !str_cmp( chck, "exit" ) )
    {
        int		dir;
        int		lhsvl;
        EXIT_DATA *	exit;

        if ( chkchar == NULL )
        {
            progbug( mob, "Bad first argument to \"exit\"" );
            return BERR;
        }
        if ( IS_NULLSTR( argv[2] ) )
        {
            progbug( mob, "Missing second argument to 'exit'" );
            return BERR;
        }
        if ( ( dir = dir_lookup( argv[2] ) ) == DIR_NONE )
        {
            progbug( mob, "Bad second argument \"%s\" to 'exit'", argv[2] );
            return BERR;
        }

        if ( IS_NULLSTR( opr ) )
        {
            if ( chkchar->in_room == NULL || chkchar->in_room->exit[dir] == NULL )
                return fInvert ^ FALSE;
            else
                return fInvert ^ TRUE;
        }

        if ( chkchar->in_room == NULL
        ||   ( exit = chkchar->in_room->exit[dir] ) == NULL
        ||   exit->to_room == NULL )
        {
            lhsvl = 0;
        }
        else
        {
            lhsvl = exit->to_room->vnum;
        }

        return fInvert ^ mprog_veval( lhsvl, opr, atoi( rval ), mob );
    }

    if ( !str_cmp( chck, "fighting" ) )
    {
	if ( chkchar != NULL )
	{
	    if ( chkchar->fighting != NULL )
		return fInvert ^ mprog_seval( chkchar->fighting->name, opr, rval, mob );
	    else
		return fInvert;
	}

	progbug( mob, "Bad argument to 'fighting'" );
	return BERR;
    }

    if ( !str_cmp( chck, "furniture" ) )
    {
        int	vnum;
        if ( chkchar != NULL )
        {
            if ( IS_NULLSTR( opr ) )
                return fInvert ^ ( chkchar->on != NULL );
            if ( chkchar->on == NULL )
                vnum = 0;
            else
                vnum = chkchar->on->pIndexData->vnum;
            return fInvert ^ mprog_veval( vnum, opr, atoi( rval ), mob );
	}

	progbug( mob, "Bad argument to 'furniture'" );
	return BERR;
    }

    if ( !str_cmp( chck, "goldamt" ) )
    {
	MONEY	rmoney;

	if ( chkchar )
	{
	    if ( money_value( &rmoney, rval ) == NULL )
	    {
		progbug( mob, "Bad argument to \"goldamt\"" );
		return BERR;
	    }
	    return fInvert ^ mprog_veval( CASH_VALUE( chkchar->money ),
					  opr, CASH_VALUE( rmoney ), mob );
	}
	progbug( mob, "Bad argument to \"goldamt\"" );
	return BERR;
    }

    if ( !str_cmp( chck, "hasdelay" ) )
    {
	bool	fDelay;

	if ( chkchar != NULL )
	{
	    if ( IS_NPC( chkchar ) && chkchar->pIndexData->vnum == MOB_VNUM_SUPERMOB )
	    {
		switch ( chkchar->smob_type )
		{
		    case PTYPE_EXIT:
			fDelay = ( get_event_exit( chkchar->smob_ptr, EVENT_EXIT_MPDELAY ) != NULL );
			break;
		    case PTYPE_OBJ:
			fDelay = ( get_event_obj( chkchar->smob_ptr, EVENT_OBJ_MPDELAY ) != NULL );
			break;
		    case PTYPE_ROOM:
			fDelay = ( get_event_room( chkchar->smob_ptr, EVENT_ROOM_MPDELAY ) != NULL );
			break;
		    default:
			fDelay = ( get_event_char( chkchar, EVENT_CHAR_MPDELAY ) != NULL );
			break;
		}
	    }
	    else
	    {
		fDelay = ( get_event_char( chkchar, EVENT_CHAR_MPDELAY ) != NULL );
	    }
	    return fInvert ^ fDelay;
	}
	progbug( mob, "Bad argument to \"hasdelay\"" );
	return BERR;
    }

    if ( !str_cmp( chck, "hasmemory" ) )
    {
	if ( chkchar != NULL )
	    return fInvert ^ ( chkchar->qmem != NULL );
	progbug( mob, "Bad argument to \"hasmemory\"" );
	return BERR;
    }

    if ( !str_cmp( chck, "hasobj" ) )
    {
        int		vnum;
        OBJ_DATA *	obj;

        if ( chkchar == NULL )
        {
            progbug( mob, "Bad first argument to \"hasobj\"" );
            return BERR;
        }
        if ( IS_NULLSTR( argv[2] ) )
        {
            progbug( mob, "Bad second argument to \"hasobj\"" );
            return BERR;
        }

        if ( is_number( argv[2] ) )
        {
            vnum = atoi( argv[2] );
            for ( obj = chkchar->carrying; obj != NULL; obj = obj->next_content )
                if ( obj->pIndexData->vnum == vnum )
                    return fInvert ^ TRUE;
        }
        else
        {
            for ( obj = chkchar->carrying; obj != NULL; obj = obj->next_content )
                if ( is_name( argv[2], obj->name ) )
                    return fInvert ^ TRUE;
        }

        return fInvert ^ FALSE;
    }

    if ( !str_cmp( chck, "hassecret" ) )
    {
        int		dir;
        EXIT_DATA *	exit;

        if ( chkchar == NULL )
        {
            progbug( mob, "Bad first argument to \"hassecret\"" );
            return BERR;
        }
        if ( IS_NULLSTR( argv[2] ) )
        {
            progbug( mob, "Missing second argument to \"hassecret\"" );
            return BERR;
        }
        if ( ( dir = dir_lookup( argv[2] ) ) == DIR_NONE )
        {
            progbug( mob, "Bad second argument \"%s\" to \"hassecret\"", argv[2] );
            return BERR;
        }

        if ( chkchar->in_room == NULL
        ||   ( exit = chkchar->in_room->exit[dir] ) == NULL
        ||   !IS_SET( exit->exit_info, EX_SECRET ) )
            return fInvert ^ FALSE;
        else
            return fInvert ^ TRUE;
    }

    if ( !str_cmp( chck, "hasspell" ) )
    {
        AFFECT_DATA *	paf;

        if ( chkchar != NULL )
        {
            if ( IS_NULLSTR( argv[2] ) )
                return fInvert ^ ( chkchar->affected != NULL );

            for ( paf = chkchar->affected; paf != NULL; paf = paf->next )
                if ( !str_cmp( argv[2], skill_table[paf->type].name ) )
                    return fInvert ^ TRUE;
            return fInvert ^ FALSE;
        }

        if ( chkobj != NULL )
        {
            if ( IS_NULLSTR( argv[2] ) )
                return fInvert ^ ( chkobj->affected != NULL || chkobj->pIndexData->affected != NULL );

            for ( paf = chkobj->affected; paf != NULL; paf = paf->next )
                if ( !str_cmp( argv[2], skill_table[paf->type].name ) )
                    return fInvert ^ TRUE;
            for ( paf = chkobj->pIndexData->affected; paf != NULL; paf = paf->next )
                if ( !str_cmp( argv[2], skill_table[paf->type].name ) )
                    return fInvert ^ TRUE;
            return fInvert ^ FALSE;
        }

        progbug( mob, "Bad first argument to \"hasspell\"" );
        return BERR;
    }

    if ( !str_cmp( chck, "hitpoints" ) )
    {
	int	lhsvl;

	if ( chkchar == NULL )
	{
	    progbug( mob, "Bad argument to \"hitpoints\"" );
	    return BERR;
	}

	lhsvl = chkchar->hit;
	rhsvl = atoi( rval );
	return fInvert ^ mprog_veval( lhsvl, opr, rhsvl, mob );
    }

    if ( !str_cmp( chck, "hitprcnt" ) )
    {
	int	lhsvl;

	if ( chkchar == NULL )
	{
	    progbug( mob, "Bad argument to \"hitprcnt\"" );
	    return BERR;
	}

	lhsvl = ( 100 * chkchar->hit ) / chkchar->max_hit;
	rhsvl = atoi( rval );
	return fInvert ^ mprog_veval( lhsvl, opr, rhsvl, mob );
    }

    if ( !str_cmp( chck, "hour" ) )
    {
	int hour;

	if ( !str_cmp( rval, "sunrise" ) )
	    hour = hour_sunrise;
        else if ( !str_cmp( rval, "sunset" ) )
            hour = hour_sunset;
        else
            hour = atoi( rval );
	return fInvert ^ mprog_veval( time_info.hour, opr, hour, mob );
    }

    if ( !str_cmp( chck, "inroom" ) )
    {
	if ( chkchar )
	    return fInvert ^ mprog_veval( chkchar->in_room->vnum, opr, atoi( rval ), mob );
	progbug( mob, "Bad argument to 'inroom'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isaffected" ) )
    {
        if ( chkchar == NULL )
        {
            progbug( mob, "Bad first argument to \"isaffected\"" );
            return BERR;
        }
        if ( IS_NULLSTR( argv[2] ) )
        {
            return fInvert ^ ( !xIS_EMPTY( chkchar->affected_by ) | !xIS_EMPTY( chkchar->shielded_by ) );
        }

        if ( is_exact_name( argv[2], affect_bit_name( chkchar->affected_by ) ) )
            return fInvert ^ TRUE;
        else
            return fInvert ^ is_exact_name( argv[2], shield_bit_name( chkchar->shielded_by ) );
    }

    if ( !str_cmp( chck, "iscarrying" ) )
    {
        int		vnum;
        OBJ_DATA *	obj;

        if ( chkchar == NULL )
        {
            progbug( mob, "Bad first argument to \"iscarrying\"" );
            return BERR;
        }
        if ( IS_NULLSTR( argv[2] ) )
        {
            progbug( mob, "Bad second argument to \"iscarrying\"" );
            return BERR;
        }

        if ( is_number( argv[2] ) )
        {
            vnum = atoi( argv[2] );
            for ( obj = chkchar->carrying; obj != NULL; obj = obj->next_content )
                if ( obj->pIndexData->vnum == vnum && obj->wear_loc == WEAR_NONE )
                    return fInvert ^ TRUE;
        }
        else
        {
            for ( obj = chkchar->carrying; obj != NULL; obj = obj->next_content )
                if ( obj->wear_loc == WEAR_NONE && is_name( argv[2], obj->name ) )
                    return fInvert ^ TRUE;
        }

        return fInvert ^ FALSE;
    }

    if ( !str_cmp( chck, "ischarmed" ) )
    {
	if ( chkchar )
	    return fInvert ^ ( IS_AFFECTED( chkchar, AFF_CHARM ) ? TRUE : FALSE );
	progbug( mob, "Bad argument to \"ischarmed\"" );
	return BERR;
    }

    if ( !str_cmp( chck, "isclan" ) )
    {
	if ( chkchar )
	    return fInvert ^ ( chkchar->clan != NULL );
	progbug( mob, "Bad argument to 'isclan'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isevil" ) )
    {
	if ( chkchar )
	    return fInvert ^ IS_EVIL( chkchar );
	progbug( mob, "Bad argument to 'isevil'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isfight" ) )
    {
	if ( chkchar )
	    return fInvert ^ ( chkchar->fighting != NULL );
	progbug( mob, "Bad argument to 'isfight'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isfollow" ) )
    {
	if ( chkchar )
	{
	    CHAR_DATA *lch;

	    if ( chkchar->master == NULL )
		return fInvert ^ FALSE;

	    for ( lch = chkchar->in_room->people; lch != NULL; lch = lch->next_in_room )
		if ( lch != chkchar && lch == chkchar->master )
		    return fInvert ^ TRUE;

	    return fInvert ^ FALSE;
	}
	progbug( mob, "Bad argument to 'isfollow'" );
	return BERR;
    }

    if ( !str_cmp( chck, "ishere" ) )
    {
	if ( chkchar )
	    return fInvert ^ ( chkchar->in_room == mob->in_room );
	progbug( mob, "Bad argument to 'ishere'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isgood" ) )
    {
	if ( chkchar )
	    return fInvert ^ IS_GOOD( chkchar );
	progbug( mob, "Bad argument to 'isgood'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isimmortal" ) )
    {
	if ( chkchar )
	    return fInvert ^ ( IS_IMMORTAL( chkchar ) ? TRUE : FALSE );
	progbug( mob, "Bad argument to 'isimmortal'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isneutral" ) )
    {
	if ( chkchar )
	    return fInvert ^ IS_NEUTRAL( chkchar );
	progbug( mob, "Bad argument to 'isneutral'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isnpc" ) )
    {
	if ( chkchar )
	    return fInvert ^ ( IS_NPC( chkchar ) ? TRUE : FALSE );
	progbug( mob, "Bad argument to 'isnpc'" );
	return BERR;
    }

    if ( !str_cmp( chck, "ispc" ) )
    {
	if ( chkchar )
	    return fInvert ^ ( IS_NPC( chkchar ) ? FALSE : TRUE );
	progbug( mob, "Bad argument to 'ispc'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isspirit" ) )
    {
	if ( chkchar )
	    return fInvert ^ ( IS_DEAD( chkchar ) ? TRUE : FALSE );
	progbug( mob, "Bad argument to 'isspirit'" );
	return BERR;
    }

    if ( !str_cmp( chck, "isvalid" ) )
    {
        bool result = FALSE;

        if ( cvar[0] != '$' )
        {
            progbug( mob, "Bad argument to \"isvalid\"" );
            return BERR;
        }
        switch( cvar[1] )
        {
            case 'i':	result = mob   != NULL;	break;
            case 'n':	result = actor != NULL;	break;
            case 't':	result = vo    != NULL;	break;
            case 'r':	result = rndm  != NULL;	break;
            case 'o':	result = obj   != NULL;	break;
            case 'p':	result = vo    != NULL;	break;
            case 'q':	result = mob   != NULL && mob->qmem != NULL; break;
            default:
                progbug( mob, "Bad argument to \"isvalid\"" );
                return BERR;
        }
        return fInvert ^ result;
    }

    if ( !str_cmp( chck, "iswearing" ) )
    {
        int		vnum;
        OBJ_DATA *	obj;

        if ( chkchar == NULL )
        {
            progbug( mob, "Bad first argument to \"iswearing\"" );
            return BERR;
        }
        if ( IS_NULLSTR( argv[2] ) )
        {
            progbug( mob, "Bad second argument to \"iswearing\"" );
            return BERR;
        }

        if ( is_number( argv[2] ) )
        {
            vnum = atoi( argv[2] );
            for ( obj = chkchar->carrying; obj != NULL; obj = obj->next_content )
                if ( obj->pIndexData->vnum == vnum && obj->wear_loc != WEAR_NONE )
                    return fInvert ^ TRUE;
        }
        else
        {
            for ( obj = chkchar->carrying; obj != NULL; obj = obj->next_content )
                if ( obj->wear_loc != WEAR_NONE && is_name( argv[2], obj->name ) )
                    return fInvert ^ TRUE;
        }

        return fInvert ^ FALSE;
    }

    if ( !str_cmp( chck, "level" ) )
    {
	if ( chkchar )
	    return fInvert ^ mprog_veval( chkchar->level, opr, atoi( rval ), mob );
	if ( chkobj )
	    return fInvert ^ mprog_veval( chkobj->level, opr, atoi( rval ), mob );
	progbug( mob, "Bad argument to 'level'" );
	return BERR;
    }

    if ( !str_cmp( chck, "mobinarea" ) )
    {
	int vnum = atoi( cvar );
	int lhsvl;
	int world_count;
	int found_count;
	CHAR_DATA *tmob;
	MOB_INDEX_DATA *m_index;

	if ( vnum < 1 || vnum > MAX_VNUM )
	{
	    progbug( mob, "Bad vnum to 'mobinarea'" );
	    return BERR;
	}

	m_index = get_mob_index(vnum);

	if(!m_index)
	    world_count = 0;
	else
	    world_count = m_index->count;

	lhsvl = 0;
	found_count = 0;

	for( tmob = char_list; tmob && found_count != world_count;
	     tmob = tmob->next )
	{
	    if ( IS_NPC( tmob ) && tmob->pIndexData->vnum == vnum )
	    {
		found_count++;
		if ( tmob->in_room->area == mob->in_room->area )
		    lhsvl++;
	    }
	}
	rhsvl = atoi( rval );

	/* Changed below from 1 to 0 */
	if ( rhsvl < 0 )
	    rhsvl = 0;
	if ( !*opr )
	    strcpy( opr, ">" );

	return fInvert ^ mprog_veval( lhsvl, opr, rhsvl, mob );
    }

    if ( !str_cmp( chck, "mobinroom" ) )
    {
	int vnum = atoi(cvar);
	int lhsvl;
	CHAR_DATA *oMob;

	if ( vnum < 1 || vnum > MAX_VNUM )
	{
	    progbug( mob, "Bad vnum to 'mobinroom'" );
	    return BERR;
	}
	lhsvl = 0;
	for ( oMob = mob->in_room->people; oMob;
	    oMob = oMob->next_in_room )
	if ( IS_NPC( oMob ) && oMob->pIndexData->vnum == vnum )
	    lhsvl++;
	rhsvl = atoi( rval );
	/* Changed below from 1 to 0 */
	if ( rhsvl < 0 ) rhsvl = 0;
	    if ( !*opr )
		strcpy( opr, ">" );
	return fInvert ^  mprog_veval( lhsvl, opr, rhsvl, mob );
    }

    if ( !str_cmp( chck, "mobinworld" ) )
    {
	int vnum = atoi( cvar );
	int lhsvl;
	MOB_INDEX_DATA *m_index;

	if ( vnum < 1 || vnum > MAX_VNUM )
	{
	    progbug( mob, "Bad vnum to 'mobinworld'" );
	    return BERR;
	}

	m_index = get_mob_index( vnum );

	if( !m_index )
	    lhsvl = 0;
	else
	    lhsvl = m_index->count;

	rhsvl = atoi( rval );

	/* Changed below from 1 to 0 */
	if ( rhsvl < 0 )
	    rhsvl = 0;
	if( !*opr )
	    strcpy( opr, ">" );

	return fInvert ^ mprog_veval( lhsvl, opr, rhsvl, mob );
    }

    if ( !str_cmp( chck, "month" ) )
    {
        return fInvert ^ mprog_veval( time_info.month + 1, opr, atoi( rval ), mob );
    }

    if ( !str_cmp( chck, "number" ) )
    {
	rhsvl = atoi( rval );
	if ( chkchar )
	    return fInvert ^ mprog_veval( IS_NPC( chkchar ) ? chkchar->pIndexData->vnum : 0, opr, rhsvl, mob );
	if ( chkobj )
	    return fInvert ^ mprog_veval( chkobj->pIndexData->vnum, opr, rhsvl, mob );
	progbug( mob, "Bad argument to 'number'" );
	return BERR;
    }

    if ( !str_cmp( chck, "name" ) )
    {
	if ( chkchar )
	    return fInvert ^ mprog_seval( chkchar->name, opr, rval, mob );
	if ( chkobj )
	    return fInvert ^ mprog_seval( chkobj->name, opr, rval, mob );
	progbug( mob, "Bad argument to 'name'" );
	return BERR;
    }

    if ( !str_cmp( chck, "objhere" ) )
    {
        int			vnum;
        int			lhsvl;
        OBJ_DATA *		obj;
        OBJ_DATA *		in_obj;

        vnum = atoi( cvar );
        if ( vnum < 1 || vnum > MAX_VNUM )
        {
            progbug( mob, "Bad vnum to 'objhere'" );
            return BERR;
        }

        lhsvl = 0;

        for ( obj = object_list; obj != NULL; obj = obj->next )
        {
            if ( obj->pIndexData->vnum != vnum )
                continue;

            for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
                ;

            if ( in_obj->in_room != NULL && in_obj->in_room == mob->in_room )
                lhsvl++;
            else if ( in_obj->carried_by != NULL
            && in_obj->carried_by->in_room == mob->in_room )
                lhsvl++;
            else if ( in_obj->stored_in != NULL
            && in_obj->stored_in->owner != NULL
            && mob->in_room != NULL
            && mob->in_room->vnum == in_obj->stored_in->vnum )
                lhsvl++;
        }

        if ( opr[0] == '\0' )
        {
            strcpy( opr, ">" );
            rhsvl = 0;
        }
        else
            rhsvl = atoi( rval );

        return fInvert ^ mprog_veval( lhsvl, opr, rhsvl, mob );
    }

    if ( !str_cmp( chck, "objinarea" ) )
    {
        int			vnum;
        int			lhsvl;
        OBJ_DATA *		obj;
        OBJ_DATA *		in_obj;
        ROOM_INDEX_DATA *	pRoom;

        vnum = atoi( cvar );
        if ( vnum < 1 || vnum > MAX_VNUM )
        {
            progbug( mob, "Bad vnum to 'objinarea'" );
            return BERR;
        }

        lhsvl = 0;

        for ( obj = object_list; obj != NULL; obj = obj->next )
        {
            if ( obj->pIndexData->vnum != vnum )
                continue;

            for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
                ;

            if ( in_obj->in_room != NULL && in_obj->in_room->area == mob->in_room->area )
                lhsvl++;
            else if ( in_obj->carried_by != NULL
            && in_obj->carried_by->in_room->area == mob->in_room->area )
                lhsvl++;
            else if ( in_obj->stored_in != NULL
            && in_obj->stored_in->owner != NULL
            && mob->in_room != NULL
            && ( pRoom = get_room_index( in_obj->stored_in->vnum ) ) != NULL
            && mob->in_room->area == pRoom->area )
                lhsvl++;
        }

        if ( opr[0] == '\0' )
        {
            strcpy( opr, ">" );
            rhsvl = 0;
        }
        else
            rhsvl = atoi( rval );

        return fInvert ^ mprog_veval( lhsvl, opr, rhsvl, mob );
    }

    if ( !str_cmp( chck, "objinroom" ) )
    {
        int		vnum;
        int		lhsvl;
        OBJ_DATA *	obj;

        vnum = atoi( cvar );
        if ( vnum < 1 || vnum > MAX_VNUM )
        {
            progbug( mob, "Bad vnum to 'objinroom'" );
            return BERR;
        }

        lhsvl = 0;
        for ( obj = mob->in_room->contents; obj != NULL; obj = obj->next_content )
            if ( obj->pIndexData->vnum == vnum )
                lhsvl++;

        if ( opr[0] == '\0' )
        {
            strcpy( opr, ">" );
            rhsvl = 0;
        }
        else
            rhsvl = atoi( rval );

        return fInvert ^ mprog_veval( lhsvl, opr, rhsvl, mob );
    }

    if ( !str_cmp( chck, "objinworld" ) )
    {
        int			vnum;
        int			lhsvl;
        OBJ_INDEX_DATA *	o_index;

        vnum = atoi( cvar );
        if ( vnum < 1 || vnum > MAX_VNUM )
        {
            progbug( mob, "Bad vnum to 'objinworld'" );
            return BERR;
        }

        o_index = get_obj_index( vnum );
        if ( o_index == NULL )
            lhsvl = 0;
        else
            lhsvl = o_index->count;

        if ( opr[0] == '\0' )
        {
            strcpy( opr, ">" );
            rhsvl = 0;
        }
        else
            rhsvl = atoi( rval );

        return fInvert ^ mprog_veval( lhsvl, opr, rhsvl, mob );
    }

    if ( !str_cmp( chck, "objtype" ) )
    {
	if ( chkobj )
	    return fInvert ^ mprog_seval( flag_string( item_types, chkobj->item_type ), opr, rval, mob );
	progbug( mob, "Bad argument to 'objtype'" );
	return BERR;
    }

    if ( !str_cmp( chck, "objval0" ) )
    {
	if ( chkobj )
	    return fInvert ^ mprog_veval( chkobj->value[0], opr, atoi( rval ), mob );
	progbug( mob, "Bad argument to 'objval0'" );
	return BERR;
    }

    if ( !str_cmp( chck, "objval1" ) )
    {
	if ( chkobj )
	    return fInvert ^ mprog_veval( chkobj->value[1], opr, atoi( rval ), mob );
	progbug( mob, "Bad argument to 'objval1'" );
	return BERR;
    }

    if ( !str_cmp( chck, "objval2" ) )
    {
	if ( chkobj )
	    return fInvert ^ mprog_veval( chkobj->value[2], opr, atoi( rval ), mob );
	progbug( mob, "Bad argument to 'objval2'" );
	return BERR;
    }

    if ( !str_cmp( chck, "objval3" ) )
    {
	if ( chkobj )
	    return fInvert ^ mprog_veval( chkobj->value[3], opr, atoi( rval ), mob );
	progbug( mob, "Bad argument to 'objval3'" );
	return BERR;
    }

    if ( !str_cmp( chck, "objval4" ) )
    {
	if ( chkobj )
	    return fInvert ^ mprog_veval( chkobj->value[4], opr, atoi( rval ), mob );
	progbug( mob, "Bad argument to 'objval4'" );
	return BERR;
    }

    if ( !str_cmp( chck, "objval5" ) )
    {
	if ( chkobj )
	    return fInvert ^ mprog_veval( chkobj->value[5], opr, atoi( rval ), mob );
	progbug( mob, "Bad argument to 'objval5'" );
	return BERR;
    }

    if ( !str_cmp( chck, "objvnum" ) )
    {
        rhsvl = atoi( rval );
        if ( chkobj )
            return fInvert ^ mprog_veval( chkobj->pIndexData->vnum, opr, rhsvl, mob );
        progbug( mob, "Bad argument to 'objvnum'" );
        return BERR;
    }

    if ( !str_cmp( chck, "position" ) )
    {
	if ( chkchar )
	    return fInvert ^ mprog_seval( flag_string( position_types, chkchar->position ), opr, rval, mob );
	progbug( mob, "Bad argument to 'position'" );
	return BERR;
    }

    if ( !str_cmp( chck, "race" ) )
    {
        if ( chkchar )
        {
            return fInvert ^ mprog_seval( race_table[chkchar->race].name, opr, rval, mob );
        }
        progbug( mob, "Bad argument to 'race'" );
        return BERR;
    }

    if ( !str_cmp( chck, "rand" ) )
    {
	return fInvert ^ ( number_percent( ) <= atoi( cvar ) );
    }

    if ( !str_cmp( chck, "sector" ) )
    {
	char *sector;
	int dir;
	if ( chkchar == NULL )
	{
	    if ( chkobj != NULL )
	    {
		progbug( mob, "Bad argument to \"sector\"" );
		return BERR;
	    }
	    for ( dir = 0; dir < MAX_DIR; dir++ )
		if ( !str_cmp( cvar, dir_name[dir] ) )
		    break;
	    if ( dir >= MAX_DIR )
	    {
		progbug( mob, "Bad argument to \"sector\"" );
		return BERR;
	    }
	    if ( mob->in_room == NULL
            ||	 mob->in_room->exit[dir] == NULL
            ||	 mob->in_room->exit[dir]->to_room == NULL )
                sector = "none";
            else
                sector = flag_string( sector_types,
                         mob->in_room->exit[dir]->to_room->sector_type );
	}
	else
	{
	    sector = flag_string( sector_types,
                     chkchar->in_room != NULL ? chkchar->in_room->sector_type : SECT_NONE );
	}
	return fInvert ^ mprog_seval( sector, opr, rval, mob );
    }

    if ( !str_cmp( chck, "sex" ) )
    {
	if ( chkchar )
	    return fInvert ^ mprog_seval( sex_name( chkchar->sex ), opr, rval, mob );
	progbug( mob, "Bad argument to \"sex\"" );
	return BERR;
    }

    if ( !str_cmp( chck, "timeskilled" ) )
    {
	MOB_INDEX_DATA *pMob;

	if ( chkchar )
	{
	    if ( !IS_NPC( chkchar ) )
	    {
		return fInvert ^ mprog_veval( 0, opr, atoi( rval ), mob );
	    }
	    pMob = chkchar->pIndexData;
	}
	else if ( !( pMob = get_mob_index( atoi( cvar ) ) ) )
	{
	    progbug( mob, "TimesKilled ifcheck: bad vnum" );
	    return BERR;
	}
	return fInvert ^ mprog_veval( pMob->killed, opr, atoi( rval ), mob );
    }

    if ( !str_cmp( chck, "usage" ) )
    {
        CHAR_DATA *	rch;
        int		count;

        if ( chkchar != NULL )
        {
            if ( chkchar->on == NULL || chkchar->in_room == NULL )
                return fInvert ^ mprog_veval( 0, opr, atoi( rval ), mob );
            count = 0;
            for ( rch = chkchar->in_room->people; rch != NULL; rch = rch->next_in_room )
                if ( rch->on == chkchar->on )
                    count++;
            return fInvert ^ mprog_veval( count, opr, atoi( rval ), mob );
        }
        progbug( mob, "Bad argument to \"usage\"" );
        return BERR;
    }

    if ( !str_cmp( chck, "vnum" ) )
    {
	rhsvl = atoi( rval );
	if ( chkchar )
	    return fInvert ^ mprog_veval( IS_NPC( chkchar ) ? chkchar->pIndexData->vnum : 0, opr, rhsvl, mob );
	if ( chkobj )
	    return fInvert ^ mprog_veval( chkobj->pIndexData->vnum, opr, rhsvl, mob );
	progbug( mob, "Bad argument to 'vnum'" );
	return BERR;
    }


    /*
     * Ok... all the ifchecks are done, so if we didnt find ours then something
     * odd happened.  So report the bug and abort the MUDprogram (return error)
     */
    progbug( mob, "Unknown ifcheck" );
    return BERR;
}


/*
 *  The main focus of the MOBprograms.  This routine is called 
 *  whenever a trigger is successful.  It is responsible for parsing
 *  the command list and figuring out what to do. However, like all
 *  complex procedures, everything is farmed out to the other guys.
 *
 *  This function rewritten by Narn for Realms of Despair, Dec/95.
 *
 */
void
mprog_driver ( char *com_list, CHAR_DATA *mob, CHAR_DATA *actor,
		   OBJ_DATA *obj, void *vo, bool single_step)
{
    char tmpcmndlst[ MAX_STRING_LENGTH ];
    char *command_list;
    char *cmnd;
    CHAR_DATA *rndm  = NULL;
    CHAR_DATA *vch   = NULL;
    int count        = 0;
    int ignorelevel  = 0;
    int iflevel;
    int	result;
    bool ifstate[MAX_IFS][ DO_ELSE + 1 ];
    static int prog_nest;

    if IS_AFFECTED( mob, AFF_CHARM )
	return;

    /* Next couple of checks stop program looping. -- Altrag */
    if ( mob == actor )
    {
	progbug( mob, "triggering oneself" );
	return;
    }

    if ( ++prog_nest > MAX_PROG_NEST )
    {
	progbug( mob, "max_prog_nest exceeded" );
	--prog_nest;
	return;
    }

    /* Make sure all ifstate bools are set to FALSE */
    for ( iflevel = 0; iflevel < MAX_IFS; iflevel++ )
    {
	for ( count = 0; count < DO_ELSE; count++ )
	{
	    ifstate[iflevel][count] = FALSE;
	}
    }

    iflevel = 0;

    /*
     * get a random visible player who is in the room with the mob.
     *
     *  If there isn't a random player in the room, rndm stays NULL.
     *  If you do a $r, $R, $j, or $k with rndm = NULL, you'll crash
     *  in mprog_translate.
     *
     *  Adding appropriate error checking in mprog_translate.
     *    -Haus
     *
     * This used to ignore players MAX_LEVEL - 3 and higher (standard
     * Merc has 4 immlevels).  Thought about changing it to ignore all
     * imms, but decided to just take it out.  If the mob can see you, 
     * you may be chosen as the random player. -Narn
     *
     */

    count = 0;
    for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( !IS_NPC( vch )
	   &&  can_see( mob, vch ) )
	{
	    if ( number_range( 0, count ) == 0 )
		rndm = vch;
	    count++;
	}
    }

    strcpy( tmpcmndlst, com_list );
    command_list = tmpcmndlst;
    if ( single_step )
    {
	if ( mob->mpscriptpos > strlen( tmpcmndlst ) )
	    mob->mpscriptpos = 0;
	else
	    command_list += mob->mpscriptpos;
	if ( *command_list == '\0' )
	{
	    command_list = tmpcmndlst;
	    mob->mpscriptpos = 0;
	}
    }

    /* 
       From here on down, the function is all mine.  The original code
       did not support nested ifs, so it had to be redone.  The max 
       logiclevel (MAX_IFS) is defined at the beginning of this file, 
       use it to increase/decrease max allowed nesting.  -Narn 
    */

    for ( ; ; )
    {
	/* With these two lines, cmnd becomes the current line from the prog,
	   and command_list becomes everything after that line. */
	cmnd         = command_list;
	command_list = mprog_next_command( command_list );

	/* Are we at the end? */
	if ( cmnd[0] == '\0' )
	{
	    if ( ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE] )
	    {
		progbug( mob, "Missing endif" );
	    }
	    --prog_nest;
	    return;
	}

	/* Evaluate/execute the command, check what happened. */
	result = mprog_do_command( cmnd, mob, actor, obj, vo, rndm, 
	    ( ifstate[iflevel][IN_IF] && !ifstate[iflevel][DO_IF] )
	    || ( ifstate[iflevel][IN_ELSE] && !ifstate[iflevel][DO_ELSE] ),
	    ( ignorelevel > 0 ) );

	/* Script prog support  -Thoric */
	if ( single_step )
	{
	    mob->mpscriptpos = command_list - tmpcmndlst;
	    --prog_nest;
	    return;
	}

	/* This is the complicated part.  Act on the returned value from
	   mprog_do_command according to the current logic state. */

	switch ( result )
	{
	case COMMANDOK:
	    debug_log_string( "COMMANDOK" );
	    /* Ok, this one's a no-brainer. */
	    continue;
	    break;

	case IFTRUE:
	    debug_log_string( "IFTRUE" );
	    /*
	       An if was evaluated and found true.  Note that we are in an
	       if section and that we want to execute it.
	    */
	    iflevel++;
	    if ( iflevel == MAX_IFS )
	    {
		progbug( mob, "Maximum nested ifs exceeded" );
		--prog_nest;
		return;
	    }

	    ifstate[iflevel][IN_IF] = TRUE; 
	    ifstate[iflevel][DO_IF] = TRUE;
	    break;

	case IFFALSE:
	    debug_log_string( "IFFALSE" );
	    /*
	      An if was evaluated and found false.  Note that we are in an
	      if section and that we don't want to execute it unless we find
	      an or that evaluates to true.
	    */
	    iflevel++;
	    if ( iflevel >= MAX_IFS )
	    {
		progbug( mob, "Maximum nested ifs exceeded" );
		--prog_nest;
		return;
	    }
	    ifstate[iflevel][IN_IF] = TRUE; 
	    ifstate[iflevel][DO_IF] = FALSE;
	    break;

	case ORTRUE:
	    debug_log_string( "ORTRUE" );
	   /*
	      An or was evaluated and found true.  We should already be in an
	      if section, so note that we want to execute it.
	   */
	   if ( !ifstate[iflevel][IN_IF] )
	   {
		progbug( mob, "Unmatched or" );
		--prog_nest;
		return;
	   }
	   ifstate[iflevel][DO_IF] = TRUE;
	   break;

	case ORFALSE:
	    debug_log_string( "ORFALSE" );
	    /*
	       An or was evaluated and found false.  We should already be in an
	       if section, and we don't need to do much.  If the if was true or
	       there were/will be other ors that evaluate(d) to true, they'll
	       set do_if to true.
	    */
	    if ( !ifstate[iflevel][IN_IF] )
	    {
		progbug( mob, "Unmatched or" );
		--prog_nest;
		return;
	    }
		continue;
	    break;

	case FOUNDELSE:
	    debug_log_string( "FOUNDELSE" );
	    /*
	       Found an else; make sure we're in an if section, bug out if not.
	       If this else is not one that we wish to ignore, note that we're
	       now in an else section, and look at whether or not we executed
	       the if section to decide whether to execute the else section.
	       Ca marche bien.
	    */
	    if ( ignorelevel > 0 )
		continue;

	    if ( ifstate[iflevel][IN_ELSE] )
	    {
		progbug( mob, "Found else in an else section" );
		--prog_nest;
		return;
	    }
	    if ( !ifstate[iflevel][IN_IF] )
	    {
		progbug( mob, "Unmatched else" );
		--prog_nest;
		return;
	    }

	    ifstate[iflevel][IN_ELSE] = TRUE;
	    ifstate[iflevel][DO_ELSE] = !ifstate[iflevel][DO_IF];
	    ifstate[iflevel][IN_IF]   = FALSE;
	    ifstate[iflevel][DO_IF]   = FALSE;

	    break;

	case FOUNDENDIF:
	    debug_log_string( "FOUNDENDIF" );
	    /*
	       Hmm, let's see... FOUNDENDIF must mean that we found an endif.
	       So let's make sure we were expecting one, return if not.  If
	       this endif matches the if or else that we're executing, note
	       that we are now no longer executing an if.  If not, keep track
	       of what we're ignoring.
	    */
	    if ( !( ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE] ) )
	    {
		progbug( mob, "Unmatched endif" );
		--prog_nest;
		return;
	    }

	    if ( ignorelevel > 0 )      
	    {
		ignorelevel--;
		continue;
	    }

	    ifstate[iflevel][IN_IF]   = FALSE;
	    ifstate[iflevel][DO_IF]   = FALSE;
	    ifstate[iflevel][IN_ELSE] = FALSE;
	    ifstate[iflevel][DO_ELSE] = FALSE;

	    iflevel--;
	    break;

	case IFIGNORED:
	    debug_log_string( "IFIGNORED" );
	    if ( !( ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE] ) )
	    {
		progbug( mob,
		         "Parse error, ignoring if while not in if or else" );
		--prog_nest;
		return;
	    }
	    ignorelevel++;
	    break;

	case ORIGNORED:
	    debug_log_string( "ORIGNORED" );
	    if ( !( ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE] ) )
	    {
		progbug( mob, "Unmatched or" );
		--prog_nest;
		return;
	    }
	    if ( ignorelevel == 0 )
	    {
		progbug( mob, "Parse error, mistakenly ignoring or" );
		--prog_nest;
		return;
	    }

	    break;

	case BERR:
	    debug_log_string( "BERR" );
	    --prog_nest;
	    return;
	    break;
	}
    }
    --prog_nest;
    return;
}


/*
 * Used to get sequential lines of a multi line string (separated by "\n\r")
 * Thus its like one_argument(), but a trifle different. It is destructive
 * to the multi line string argument, and thus clist must not be shared.
 */
char *
mprog_next_command( char *clist )
{
    char *pointer = clist;

    while ( *pointer != '\n' && *pointer != '\0' )
	pointer++;
    if ( *pointer == '\n' )
	*pointer++ = '\0';
    if ( *pointer == '\r' )
	*pointer++ = '\0';

    return ( pointer );

}


/*
 *  The two functions mprog_seval and mprog_veval do the basic evaluation
 *  of ifcheck operators.
 *  It is important to note that the string operations are not what
 *  you probably expect.  Equality is exact and division is substring.
 *  remember that lhs has been stripped of leading space, but can
 *  still have trailing spaces so be careful when editing since:
 *  "guard" and "guard " are not equal.
 */
bool
mprog_seval( const char *lhs, char *opr, char *rhs, CHAR_DATA *mob )
{
    if ( !str_cmp( opr, "==" ) )
	return ( bool )( !str_cmp( lhs, rhs ) );
    if ( !str_cmp( opr, "!=" ) )
	return ( bool )( str_cmp( lhs, rhs ) );
    if ( !str_cmp( opr, "/" ) )
	return ( bool )( !str_infix( rhs, lhs ) );
    if ( !str_cmp( opr, "!/" ) )
	return ( bool )( str_infix( rhs, lhs ) );

    progbug( mob, "Improper MOBprog operator '%s'", opr );
    return 0;
}


/* the "value evaluator" */
bool
mprog_veval( int lhs, char *opr, int rhs, CHAR_DATA *mob )
{
    if ( !str_cmp( opr, "==" ) )
	return ( lhs == rhs );
    if ( !str_cmp( opr, "!=" ) )
	return ( lhs != rhs );
    if ( !str_cmp( opr, ">" ) )
	return ( lhs > rhs );
    if ( !str_cmp( opr, "<" ) )
	return ( lhs < rhs );
    if ( !str_cmp( opr, "<=" ) )
	return ( lhs <= rhs );
    if ( !str_cmp( opr, ">=" ) )
	return ( lhs >= rhs );
    if ( !str_cmp( opr, "&" ) )
	return ( lhs & rhs );
    if ( !str_cmp( opr, "|" ) )
	return ( lhs | rhs );

    progbug( mob, "Improper MOBprog operator '%s'", opr );

  return 0;
}


/*
 * This routine handles the variables for command expansion.
 * If you want to add any go right ahead, it should be fairly
 * clear how it is done and they are quite easy to do, so you
 * can be as creative as you want. The only catch is to check
 * that your variables exist before you use them. At the moment,
 * using $t when the secondary target refers to an object 
 * i.e. >prog_act drops~<nl>if ispc($t)<nl>sigh<nl>endif<nl>~<nl>
 * probably makes the mud crash (vice versa as well) The cure
 * would be to change act() so that vo becomes vict & v_obj.
 * but this would require a lot of small changes all over the code.
 */

/*
 *  There's no reason to make the mud crash when a variable's
 *  fubared.  I added some ifs.  I'm willing to trade some 
 *  performance for stability. -Haus
 *
 *  Added char_died and obj_extracted checks	-Thoric
 */
void
mprog_translate( char ch, char *t, CHAR_DATA *mob, CHAR_DATA *actor,
		      OBJ_DATA *obj, void *vo, CHAR_DATA *rndm )
{
    CHAR_DATA *	vict	= (CHAR_DATA *) vo;
    OBJ_DATA *	v_obj	= (OBJ_DATA  *) vo;
    CHAR_DATA *	qmem	= mob->qmem;

    if ( vict != NULL && vict->stype != STYPE_CHAR )
        vict = NULL;
    if ( v_obj != NULL && v_obj->stype != STYPE_OBJ )
        v_obj = NULL;

#if 0 /* don't wanna delete this just yet */
/* Fix crash bug :)  SHADDAI */
    if ( v_obj && v_obj->serial )
        vict = NULL;
    else
        v_obj = NULL;
#endif

    *t = '\0';
    switch ( ch ) 
    {

	case 'a':
	    if ( obj && !obj_extracted(obj) ) 
	    {
		strcpy( t, aoran(obj->name) );
/*
                switch ( *( obj->name ) )
	        {
	            case 'a': case 'e': case 'i':
                    case 'o': case 'u': strcpy( t, "an" );
	                      break;
                    default: strcpy( t, "a" );
                }
*/
	    }
	 else
	      strcpy( t, "a" );
	 break;

     case 'A':
         if ( v_obj && !obj_extracted(v_obj) )
	 {
	      strcpy( t, aoran(v_obj->name) );
         }
	 else
	      strcpy( t, "a" );
	 break;

     case 'e':
         if ( actor && !char_died(actor) )
	 {
	   can_see( mob, actor ) ? strcpy( t, he_she[ actor->sex ] )
	                         : strcpy( t, "someone" );
         } 
	 else
	      strcpy( t, "it" );
	 break;

     case 'E':
         if ( vict && !char_died(vict) )
	 {
	   can_see( mob, vict ) ? strcpy( t, he_she[ vict->sex ] )
                                : strcpy( t, "someone" );
         }
	 else
	      strcpy( t, "it" );
	 break;

     case 'i':
	 if ( mob && !char_died(mob) )
	 {
	   if (mob->name)
              one_argument( mob->name, t );
         } else
	    strcpy( t, "someone" );
      break;

     case 'I':
	 if ( mob && !char_died(mob) )
	 {
	   if (mob->short_descr)
	   {
              strcpy( t, mob->short_descr );
           } else {
	      strcpy( t, "someone" );
	   }
         } else
	    strcpy( t, "someone" );
      break;

     case 'j':
	 if (mob && !char_died(mob))
	 {
	    strcpy( t, he_she[ mob->sex ] );
         } else {
	    strcpy( t, "it" );
	 }
	 break;

     case 'J':
         if ( rndm && !char_died(rndm) )
	 {
	   can_see( mob, rndm ) ? strcpy( t, he_she[ rndm->sex ] )
	                        : strcpy( t, "someone" );
         }
	 else
	      strcpy( t, "it" );
	 break;

     case 'k':
	 if( mob && !char_died(mob) )
	 {
	   strcpy( t, him_her[ mob->sex ] );
         } else {
	    strcpy( t, "it" );
	 }
	 break;

     case 'K':
         if ( rndm && !char_died(rndm) )
	 {
	   can_see( mob, rndm ) ? strcpy( t, him_her[ rndm->sex ] )
                                : strcpy( t, "someone's" );
         }
	 else
	      strcpy( t, "its'" );
	 break;

     case 'l':
	 if( mob && !char_died(mob) )
	 {
	   strcpy( t, his_her[ mob->sex ] );
         } else {
	    strcpy( t, "it" );
	 }
	 break;

     case 'L':
         if ( rndm && !char_died(rndm) )
	 {
	   can_see( mob, rndm ) ? strcpy( t, his_her[ rndm->sex ] )
	                        : strcpy( t, "someone" );
         }
	 else
	      strcpy( t, "its" );
	 break;

     case 'm':
         if ( actor && !char_died(actor) )
	 {
	   can_see( mob, actor ) ? strcpy( t, him_her[ actor->sex ] )
                                 : strcpy( t, "someone" );
         }
	 else
	      strcpy( t, "it" );
	 break;

     case 'M':
         if ( vict && !char_died(vict) )
	 {
	   can_see( mob, vict ) ? strcpy( t, him_her[ vict->sex ] )
                                : strcpy( t, "someone" );
         }
	 else
	      strcpy( t, "it" );
	 break;

    case 'n':
        if ( actor && !char_died( actor ) )
	{
	    if ( can_see( mob,actor ) )
		first_arg( actor->name, t, FALSE );
	    if ( !IS_NPC( actor ) )
		*t = UPPER( *t );
        }
	else
	    strcpy( t, "someone" );
	break;

     case 'N':
         if ( actor && !char_died(actor) ) 
	 {
            if ( can_see( mob, actor ) )
	       if ( IS_NPC( actor ) )
		 strcpy( t, actor->short_descr );
	       else
	       {
		   strcpy( t, actor->name );
		   strcat( t, actor->pcdata->title );
	       }
	    else
	      strcpy( t, "someone" );
         } 
	 else
	      strcpy( t, "someone" );
	 break;

     case 'o':
         if ( obj && !obj_extracted(obj) )
         {
           can_see_obj( mob, obj ) ? one_argument( obj->name, t )
                                   : strcpy( t, "something" );
         }
         else
              strcpy( t, "something" );
         break;

     case 'O':
         if ( obj && !obj_extracted(obj) )
	 {
	   can_see_obj( mob, obj ) ? strcpy( t, obj->short_descr )
                                   : strcpy( t, "something" );
         }
	 else
	      strcpy( t, "something" );
	 break;

     case 'p':
         if ( v_obj && !obj_extracted(v_obj) )
	 {
	   can_see_obj( mob, v_obj ) ? one_argument( v_obj->name, t )
                                     : strcpy( t, "something" );
         }
	 else
	      strcpy( t, "something" );
	 break;

     case 'P':
        if ( v_obj && !obj_extracted(v_obj) )
	{
	    can_see_obj( mob, v_obj ) ? strcpy( t, v_obj->short_descr )
                                      : strcpy( t, "something" );
        }
	else
            strcpy( t, "something" );
        break;

    case 'q':
        if ( mob != NULL && !char_died( mob ) && mob->qmem != NULL )
        {
            if ( can_see( mob, mob->qmem ) )
                first_arg( mob->qmem->name, t, FALSE );
            if ( !IS_NPC( mob->qmem ) )
                *t = UPPER( *t );
        }
        else
            strcpy( t, "someone" );
        break;

    case 'Q':
        if ( mob != NULL && !char_died( mob ) && mob->qmem != NULL )
        {
            if ( can_see( mob, mob->qmem ) )
            {
                if ( IS_NPC( mob->qmem ) )
                    strcpy( t, mob->qmem->short_descr );
                else
                {
                    strcpy( t, mob->qmem->name );
                    strcat( t, mob->qmem->pcdata->title );
                }
            }
            else
                strcpy( t, "someone" );
        }
        else
            strcpy( t, "someone" );
        break;

     case 'r':
        if ( rndm && !char_died(rndm) )
	{
	    if ( can_see( mob, rndm ) )
	    {
	        one_argument( rndm->name, t );
            }
            if ( !IS_NPC( rndm ) )
            {
                *t = UPPER( *t );
	    }
	}  
	else
	    strcpy( t, "someone" );
	break;

    case 'R':
	 if ( rndm && !char_died(rndm) )
	 {
            if ( can_see( mob, rndm ) )
	       if ( IS_NPC( rndm ) )
		 strcpy(t,rndm->short_descr);
	       else
	       {
		 strcpy( t, rndm->name );
		 strcat( t, " " );
		 strcat( t, rndm->pcdata->title );
	       }
	    else
	      strcpy( t, "someone" );
         }
	 else 
	      strcpy( t, "someone" );
	 break;

     case 's':
         if ( actor && !char_died(actor) )
	 {
	   can_see( mob, actor ) ? strcpy( t, his_her[ actor->sex ] )
	                         : strcpy( t, "someone's" );
         }
	 else
	      strcpy( t, "its'" );
	 break;

    case 'S':
        if ( vict && !char_died(vict) )
	{
	    can_see( mob, vict ) ? strcpy( t, his_her[ vict->sex ] )
                                 : strcpy( t, "someone's" ); 
        }
	else
            strcpy( t, "its'" );
	break;

    case 't':
        if ( vict && !char_died(vict) )
	{
	    if ( can_see( mob, vict ) )
		one_argument( vict->name, t );
	    if ( !IS_NPC( vict ) )
		*t = UPPER( *t );
        } 
	else 
	    strcpy( t, "someone" );

	break;

     case 'T':
         if ( vict && !char_died(vict) ) 
	 {
            if ( can_see( mob, vict ) )
	       if ( IS_NPC( vict ) )
		 strcpy( t, vict->short_descr );
	       else
	       {
		 strcpy( t, vict->name );
		 strcat( t, " " );
		 strcat( t, vict->pcdata->title );
	       }
	    else
	      strcpy( t, "someone" );
         }
	 else 
	      strcpy( t, "someone" );
	 break;

    case 'u':
        if ( qmem && !char_died( qmem ) )
        {
            can_see( mob, qmem ) ? strcpy( t, he_she[ qmem->sex ] )
				   : strcpy( t, "someone" );
	}
	else
	    strcpy( t, "it" );
	break;

    case 'v':
        if ( qmem && !char_died( qmem ) )
        {
            can_see( mob, qmem ) ? strcpy( t, him_her[ qmem->sex ] )
				   : strcpy( t, "someone" );
	}
	else
	    strcpy( t, "it" );
	break;

    case 'w':
        if ( qmem && !char_died( qmem ) )
        {
            can_see( mob, qmem ) ? strcpy( t, his_her[ qmem->sex ] )
				   : strcpy( t, "someone" );
	}
	else
	    strcpy( t, "its'" );
	break;

     case '$':
         strcpy( t, "$" );
	 break;

     default:
         progbug( mob, "Bad $var '%02X'", ch );
	 break;
     }

    return;
}


/*
 * The next three routines are the basic trigger types. Either trigger
 *  on a certain percent, or trigger on a keyword or word phrase, or time.
 *  To see how this works, look at the various trigger routines..
 */
void
mprog_wordlist_check( char *arg, CHAR_DATA *mob, CHAR_DATA *actor,
			  OBJ_DATA *obj, void *vo, bitvector type )
{

    char	temp1[ MAX_STRING_LENGTH ];
    char	temp2[ MAX_INPUT_LENGTH ];
#if defined( OLD_WORDCHECK )
    char	word[ MAX_INPUT_LENGTH ];
#endif
    MPROG_DATA *mprg;
    char *	list;
    char *	start;
    char *	dupl;
    char *	end;
    int		i;

    for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
    {
	if ( mprg->type == type )
	{
	    strcpy( temp1, mprg->arglist );
	    list = temp1;
	    for ( i = 0; i < strlen( list ); i++ )
		list[i] = LOWER( list[i] );
	    strcpy( temp2, arg );
	    dupl = temp2;
	    for ( i = 0; i < strlen( dupl ); i++ )
		dupl[i] = LOWER( dupl[i] );

	    if ( ( *list == 'p' ) && ( *(list+1) == ' ' ) )
	    {
		list += 2;
		while ( ( start = strstr( dupl, list ) ) )
		if ( ( start == dupl || *(start-1) == ' ' )
		     && ( *(end = start + strlen( list ) ) == ' '
			 || *end == '\n'
			 || *end == '\r'
			 || *end == '\0' ) )
		{
		    mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
		    break;
		}
		else
		    dupl = start+1;
	    }
	    else
	    {
#if defined( OLD_WORDCHECK )
		list = one_argument( list, word );
		for( ; word[0] != '\0'; list = one_argument( list, word ) )
		while ( ( start = strstr( dupl, word ) ) )
		    if ( ( start == dupl || *(start-1) == ' ' )
			&& ( *(end = start + strlen( word ) ) == ' '
			    || *end == '\n'
			    || *end == '\r'
			    || *end == '\0' ) )
		    {
			mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
			break;
		    }
		    else
			dupl = start+1;
#else
                if ( has_same_word( list, dupl ) )
		    mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
#endif
	    } 
	}

    }
    return;

}


void
mprog_percent_check( CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj,
			 void *vo, bitvector type )
{
    MPROG_DATA * mprg;

    for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
    {
	if ( ( mprg->type == type )
	   && ( number_percent( ) <= atoi( mprg->arglist ) ) )
	{
	    mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
	    if ( type != MP_GREET_PROG && type != MP_ALL_GREET_PROG )
		break;
	}

    }
    return;

}


void
mprog_time_check( CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj,
                         void *vo, bitvector type)
{
    MPROG_DATA *mprg;
    bool	trigger_time;
    int		hour;

    for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
    {
        if ( !str_cmp( mprg->arglist, "sunrise" ) )
            hour = hour_sunrise;
        else if ( !str_cmp( mprg->arglist, "sunset" ) )
            hour = hour_sunset;
        else
            hour = atoi( mprg->arglist );
	trigger_time = ( time_info.hour == hour );     

	if ( !trigger_time )
	{
	    if ( mprg->triggered )
	    mprg->triggered = FALSE;
	    continue;
	}

	if ( ( mprg->type == type )
	      && ( ( !mprg->triggered ) || ( mprg->type == MP_HOUR_PROG ) ) )
	{
	    mprg->triggered = TRUE;
	    mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );
	}
    }
    return;
}


bool
eprog_percent_check( CHAR_DATA *smob, CHAR_DATA *actor, EXIT_DATA *pExit,
                     void *vo, bitvector type )
{
    OREPROG_DATA*	prg;

    for ( prg = pExit->eprogs; prg != NULL; prg = prg->next )
    {
        if ( ( prg->type & type ) && number_percent( ) < atoi( prg->arglist ) )
        {
            mprog_driver( prg->comlist, smob, actor, NULL, vo, FALSE );
            return TRUE;
        }
    }
    return FALSE;
}


bool
oprog_percent_check( CHAR_DATA *smob, CHAR_DATA *actor, OBJ_DATA *obj,
			  void *vo, bitvector type )
{
    OREPROG_DATA *	prg;

    for ( prg = obj->pIndexData->oprogs; prg != NULL; prg = prg->next )
    {
	if ( ( prg->type & type ) && number_percent( ) < atoi( prg->arglist ) )
	{
	    mprog_driver( prg->comlist, smob, actor, obj, vo, FALSE );
	    return TRUE;
	}
    }
    return FALSE;
}


bool
rprog_percent_check( CHAR_DATA *smob, CHAR_DATA *actor,
			  ROOM_INDEX_DATA *room,
			  void *vo, bitvector type )
{
    OREPROG_DATA *	prg;

    for ( prg = room->rprogs; prg != NULL; prg = prg->next )
    {
	if ( ( prg->type & type ) && number_percent( ) < atoi( prg->arglist ) )
	{
	    mprog_driver( prg->comlist, smob, actor, NULL, vo, FALSE );
	    return TRUE;
	}
    }
    return FALSE;
}


bool
rprog_roomlist_check( CHAR_DATA *smob, ROOM_INDEX_DATA *here,
		      ROOM_INDEX_DATA *there, bitvector type )
{
    OREPROG_DATA *	prg;
    char		arg[MAX_INPUT_LENGTH];
    char *		arglist;

    if ( here == NULL || there == NULL )
        return FALSE;

    for ( prg = here->rprogs; prg != NULL; prg = prg->next )
    {
        if ( prg->type & type )
        {
            if ( !str_cmp( prg->arglist, "all" ) )
            {
                mprog_driver( prg->comlist, smob, NULL, NULL, NULL, FALSE );
                return TRUE;
            }
            arglist = prg->arglist;
            while ( !IS_NULLSTR( arglist ) )
            {
                arglist = one_argument( arglist, arg );
                if ( atoi( arg ) == there->vnum )
                {
                    mprog_driver( prg->comlist, smob, NULL, NULL, NULL, FALSE );
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}


bool
rprog_wordlist_check( char *arg, CHAR_DATA *smob, CHAR_DATA *actor,
		      ROOM_INDEX_DATA *room, void *vo, bitvector type )
{

    char	temp1[ MAX_STRING_LENGTH ];
    char	temp2[ MAX_INPUT_LENGTH ];
#if defined( OLD_WORDCHECK )
    char	word[ MAX_INPUT_LENGTH ];
#endif
    OREPROG_DATA *prg;
    char *	list;
    char *	start;
    char *	dupl;
    char *	end;
    int		i;

    for ( prg = room->rprogs; prg != NULL; prg = prg->next )
    {
	if ( prg->type == type )
	{
	    strcpy( temp1, prg->arglist );
	    list = temp1;
	    for ( i = 0; i < strlen( list ); i++ )
		list[i] = LOWER( list[i] );
	    strcpy( temp2, arg );
	    dupl = temp2;
	    for ( i = 0; i < strlen( dupl ); i++ )
		dupl[i] = LOWER( dupl[i] );

	    if ( ( *list == 'p' ) && ( *(list+1) == ' ' ) )
	    {
		list += 2;
		while ( ( start = strstr( dupl, list ) ) )
		if ( ( start == dupl || *(start-1) == ' ' )
		     && ( *(end = start + strlen( list ) ) == ' '
			 || *end == '\n'
			 || *end == '\r'
			 || *end == '\0' ) )
		{
		    mprog_driver( prg->comlist, smob, actor, NULL, vo, FALSE );
		    return TRUE;
		}
		else
		    dupl = start+1;
	    }
	    else
	    {
#if defined( OLD_WORDCHECK )
		list = one_argument( list, word );
		for( ; word[0] != '\0'; list = one_argument( list, word ) )
		while ( ( start = strstr( dupl, word ) ) )
		    if ( ( start == dupl || *(start-1) == ' ' )
			&& ( *(end = start + strlen( word ) ) == ' '
			    || *end == '\n'
			    || *end == '\r'
			    || *end == '\0' ) )
		    {
			mprog_driver( prg->comlist, smob, actor, NULL, vo, FALSE );
			return TRUE;
		    }
		    else
			dupl = start+1;
#else
                if ( has_same_word( list, dupl ) )
		{
		    mprog_driver( prg->comlist, smob, actor, NULL, vo, FALSE );
		    return TRUE;
		}
#endif
	    }
	}

    }

    return FALSE;

}


bool
mprog_keyword_check( const char *argu, const char *argl )
{
#if defined( OLD_WORDCHECK )
    char word[MAX_INPUT_LENGTH];
#endif
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *arg, *arglist;
    char *start, *end;

    arg = str_lower( arg1, argu );
    arglist = str_lower( arg2, argl );

    if ( ( arglist[0] == 'p' ) && ( arglist[1] == ' ' ) )
    {
	arglist += 2;
	while ( ( start = strstr( arg, arglist ) ) )
	    if ( (start == arg || *(start-1) == ' ' )
    	    && ( *(end = start + strlen( arglist ) ) == ' '
    	    ||   *end == '\n'
    	    ||   *end == '\r'
    	    ||   *end == '\0' ) )
		return TRUE;
	    else
		arg = start+1;
    }
    else
    {
#if defined( OLD_WORDCHECK )
	arglist = one_argument( arglist, word );
	for ( ; word[0] != '\0'; arglist = one_argument( arglist, word ) )
	    while ( ( start = strstr( arg, word ) ) )
		if ( ( start == arg || *(start-1) == ' ' )
	    	&& ( *(end = start + strlen( word ) ) == ' '
	    	||   *end == '\n'
	    	||   *end == '\r'
	    	||   *end == '\0' ) )
		    return TRUE;
		else
		    arg = start +1;
#else
        if ( has_same_word( arglist, arg ) )
            return TRUE;
#endif
    }
    return FALSE;
}


CHAR_DATA *
eset_supermob( EXIT_DATA *pExit, ROOM_INDEX_DATA *pRoom )
{
    CHAR_DATA *		smob;
    char		buf[MAX_INPUT_LENGTH];
    int			dir;
    char *		dirname;

    smob = create_mobile( get_mob_index( MOB_VNUM_SUPERMOB ) );

    for ( dir = 0; dir < MAX_DIR; dir++ )
        if ( pRoom->exit[dir] == pExit )
            break;
    if ( dir >= MAX_DIR )
        dirname = "(Unknown direction)";
    else
        dirname = dir_name[dir];

    strcpy( buf, capitalize( dirname ) );
    free_string( smob->short_descr );
    smob->short_descr = str_dup( buf );
    sprintf( buf, "Room %d --> %s", pRoom->vnum, dirname );
    free_string( smob->description );
    smob->description = str_dup( buf );

    smob->smob_ptr = (void *)pExit;
    smob->smob_type = PTYPE_EXIT;
    char_to_room( smob, pRoom );
    return smob;
}


CHAR_DATA *oset_supermob( OBJ_DATA *obj )
{
    CHAR_DATA *		smob;
    OBJ_DATA *		in_obj;
    ROOM_INDEX_DATA *	room;
    char		buf[SHORT_STRING_LENGTH];

    smob = create_mobile( get_mob_index( MOB_VNUM_SUPERMOB ) );
    free_string( smob->short_descr );
    free_string( smob->description );
    smob->short_descr = str_dup( obj->short_descr );
    sprintf( buf, "Object #%d", obj->pIndexData->vnum );
    smob->description = str_dup( buf );
    smob->smob_ptr = (void *)obj;
    smob->smob_type = PTYPE_OBJ;
    if ( obj->in_room )
	room = obj->in_room;
    else if ( obj->carried_by && obj->carried_by->in_room )
	room = obj->carried_by->in_room;
    else if ( obj->in_obj )
    {
	for ( in_obj = obj->in_obj; in_obj->in_obj; in_obj = in_obj->in_obj )
	    ;
	if ( in_obj->in_room )
	    room = in_obj->in_room;
	else if ( in_obj->carried_by && in_obj->carried_by->in_room )
	    room = in_obj->carried_by->in_room;
	else
	    room = get_room_index( ROOM_VNUM_LIMBO );
    }
    else
	room = get_room_index( ROOM_VNUM_LIMBO );
    char_to_room( smob, room );
    return smob;
}


void
release_supermob( CHAR_DATA *smob )
{
    extract_char( smob, TRUE );
    return;
}


CHAR_DATA *
rset_supermob( ROOM_INDEX_DATA *pRoom )
{
    CHAR_DATA *		smob;
    char		buf[SHORT_STRING_LENGTH];

    if ( pRoom == NULL )
	return NULL;

    smob = create_mobile( get_mob_index( MOB_VNUM_SUPERMOB ) );
    free_string( smob->short_descr );
    free_string( smob->description );
    smob->short_descr = str_dup( pRoom->name );
    sprintf( buf, "Room #%d", pRoom->vnum );
    smob->description = str_dup( buf );
    smob->smob_ptr = (void *)pRoom;
    smob->smob_type = PTYPE_ROOM;
    char_to_room( smob, pRoom );
    return smob;
}


void
eprog_bash_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_BASH )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_BASH );
        release_supermob( smob );
    }
    return;
}


void
eprog_close_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_CLOSE )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_CLOSE );
        release_supermob( smob );
    }
    return;
}


void
eprog_enter_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *	smob;

    if ( pExit == NULL || room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_ENTER )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_ENTER );
        release_supermob( smob );
    }
    else
    {
	eprog_pass_trigger( pExit, room, ch, TRUE );
    }
}


void
eprog_exit_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *	smob;

    if ( pExit == NULL || room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_EXIT )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_EXIT );
        release_supermob( smob );
    }
    else
    {
	eprog_pass_trigger( pExit, room, ch, FALSE );
    }
}


void
eprog_knock_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_KNOCK )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_KNOCK );
        release_supermob( smob );
    }
    return;
}


bool
eprog_listen_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *	smob;
    bool	result;

    result = FALSE;
    if ( room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return FALSE;

    if ( pExit->progtypes & EXIT_PROG_LISTEN )
    {
        smob = eset_supermob( pExit, room );
        result = eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_LISTEN );
        release_supermob( smob );
    }
    return result;
}


void
eprog_lock_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *smob;

    if ( room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_LOCK )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_LOCK );
        release_supermob( smob );
    }
    return;
}


void
eprog_look_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_LOOK )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_LOOK );
        release_supermob( smob );
    }
    return;
}


void
eprog_open_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_OPEN )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_OPEN );
        release_supermob( smob );
    }
    return;
}


void
eprog_pass_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch,
		    bool fEnter )
{
    CHAR_DATA *smob;

    if ( pExit == NULL || room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_PASS )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_PASS );
        release_supermob( smob );
    }
    else
    {
	if ( fEnter )
	    rprog_enter_trigger( room, ch );
	else
	    rprog_exit_trigger( room, ch );
    }

    return;
}


void
eprog_pick_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_PICK )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_PICK );
        release_supermob( smob );
    }
    return;
}


void
eprog_scry_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( pExit == NULL || room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_SCRY )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_SCRY );
        release_supermob( smob );
    }
    return;
}


void
eprog_unlock_trigger( EXIT_DATA *pExit, ROOM_INDEX_DATA *room,
		      CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *smob;

    if ( room == NULL || IS_SET( room->room_flags, ROOM_VIRTUAL ) )
        return;

    if ( pExit->progtypes & EXIT_PROG_UNLOCK )
    {
        smob = eset_supermob( pExit, room );
        eprog_percent_check( smob, ch, pExit, NULL, EXIT_PROG_UNLOCK );
        release_supermob( smob );
    }
    return;
}


void
mprog_act_trigger( char* buf, CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj,
		   void* vo )
{
    MPROG_ACT_LIST *	tmp_act;
    MPROG_DATA *	mprg;
    bool		found;

    if ( !mob
	 || !mob->in_room
	 || !mob->in_room->area->nplayer )
    {
	return;
    }
    if ( ch == mob )
	return;

    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & MP_ACT_PROG ) )
    {
	/* make sure this is a matching trigger */
	found = FALSE;
	for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
	    if ( mprg->type == MP_ACT_PROG
	    &&   mprog_keyword_check( buf, mprg->arglist ) )
	    {
		found = TRUE;
		break;
	    }
	if ( !found )
	    return;

	tmp_act = new_mpact_data( );
	tmp_act->next = mob->mpact;
	mob->mpact	= tmp_act;
	mob->mpact->buf = str_dup( buf );
	mob->mpact->ch	= ch;
	mob->mpact->obj	= obj;
	mob->mpact->vo	= vo;
	mob->mpactnum++;
    }

    return;
}


void
mprog_bribe_trigger( CHAR_DATA* mob, CHAR_DATA* ch, MONEY *amt )
{
    char	buf[ MAX_STRING_LENGTH ];
    MPROG_DATA *mprg;
    OBJ_DATA *	obj;
    MONEY	arg;
    int		amount;
    int		bribe;

    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & MP_BRIBE_PROG ) )
    {
	bribe = amt->gold    * FRACT_PER_COPPER * COPPER_PER_SILVER * SILVER_PER_GOLD
	       + amt->silver * FRACT_PER_COPPER * COPPER_PER_SILVER
	       + amt->copper   * FRACT_PER_COPPER
	       + amt->fract;
	obj = create_object( get_obj_index( OBJ_VNUM_COINS ), 0 );
	sprintf( buf, obj->short_descr, bribe );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	obj->value[0]    = amt->gold;
	obj->value[1]	 = amt->silver;
	obj->value[2]	 = amt->copper;
	obj->value[3]	 = amt->fract;
	obj_to_char( obj, mob );
	money_subtract( &mob->money, amt, FALSE );

	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	{
	    if ( mprg->type & MP_BRIBE_PROG )
	    {
	        if ( money_value( &arg, mprg->arglist ) == NULL )
	        {
	            progbug( mob, "Bribe_trigger: bad bribe amount: %s", mprg->arglist );
	            return;
                }
                amount = arg.gold   * FRACT_PER_COPPER * COPPER_PER_SILVER * SILVER_PER_GOLD
                       + arg.silver * FRACT_PER_COPPER * COPPER_PER_SILVER
                       + arg.copper * FRACT_PER_COPPER
                       + arg.fract;
                if ( bribe >= amount )
                {
		    mprog_driver( mprg->comlist, mob, ch, obj, NULL, FALSE );
		    break;
                }
	    }
	}
    }

    return;

}


void
mprog_buy_trigger( CHAR_DATA *mob, CHAR_DATA *ch, CHAR_DATA *pet, OBJ_DATA *obj )
{
    char	buf[MAX_INPUT_LENGTH];
    MPROG_DATA *mprg;

    if ( IS_NPC( mob )
    &&	 can_see( mob, ch )
    &&	 ( mob->pIndexData->progtypes & MP_BUY_PROG ) )
    {
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	{
	    one_argument( mprg->arglist, buf );
	    if ( ( mprg->type & MP_BUY_PROG )
		&& ( ( obj != NULL && !str_cmp( obj->name, mprg->arglist ) )
		||   ( pet != NULL && !str_cmp( pet->name, mprg->arglist ) )
		|| ( !str_cmp( "any", buf ) ) ) )
	    {
		mprog_driver( mprg->comlist, mob, ch, obj, pet, FALSE );
		break;
	    }
	}
    }

    return;
}


void
mprog_death_trigger( CHAR_DATA* mob, CHAR_DATA* ch, int dam )
{
    int oldpos;

    if ( IS_NPC( mob )
    && ( mob->pIndexData->progtypes & MP_DEATH_PROG ) )
    {
	oldpos = mob->position;
	mob->position = POS_STANDING;
	mprog_percent_check( mob, ch, NULL, NULL, MP_DEATH_PROG );
	mob->position = oldpos;
    }
    else
    {
	death_cry( mob, dam );
    }

    return;
}


void
mprog_entry_trigger( CHAR_DATA* mob )
{
    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & MP_ENTRY_PROG ) )
    {
	mprog_percent_check( mob, NULL, NULL, NULL, MP_ENTRY_PROG );
    }

    return;
}


void
mprog_fight_trigger( CHAR_DATA* mob, CHAR_DATA* ch )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & MP_FIGHT_PROG ) )
    {
	mprog_percent_check( mob, ch, NULL, NULL, MP_FIGHT_PROG );
    }
    return;
}


void
mprog_give_trigger( CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj )
{
    char	buf[MAX_INPUT_LENGTH];
    MPROG_DATA *mprg;

    if ( IS_NPC( mob )
     && can_see( mob, ch )
     && ( mob->pIndexData->progtypes & MP_GIVE_PROG ) )
    {
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	{
	    one_argument( mprg->arglist, buf );
	    if ( ( mprg->type & MP_GIVE_PROG )
		&& ( ( !str_cmp( obj->name, mprg->arglist ) )
		|| ( !str_cmp( "all", buf ) ) ) )
	    {
		mprog_driver( mprg->comlist, mob, ch, obj, NULL, FALSE );
		break;
	    }
	}
    }

    return;
}


void
mprog_greet_trigger( CHAR_DATA* ch )
{
    CHAR_DATA *vmob;

    if ( ch->in_room == NULL )
	return;

    for ( vmob = ch->in_room->people; vmob != NULL; vmob = vmob->next_in_room )
    {
	if ( IS_NPC( vmob )
	  && ch != vmob
	  && can_see( vmob, ch )
	  && ( vmob->fighting == NULL )
	  && IS_AWAKE( vmob )
	  && ( vmob->pIndexData->progtypes & MP_GREET_PROG) )
	{
	    mprog_percent_check( vmob, ch, NULL, NULL, MP_GREET_PROG );
	}
	else if ( IS_NPC( vmob )
	  && ch != vmob
	  && ( vmob->fighting == NULL )
	  && IS_AWAKE( vmob )
	  && ( vmob->pIndexData->progtypes & MP_ALL_GREET_PROG ) )
	{
	    mprog_percent_check( vmob, ch, NULL, NULL, MP_ALL_GREET_PROG );
	}
     }

    return;
}


void
mprog_hitprcnt_trigger( CHAR_DATA* mob, CHAR_DATA* ch )
{
    MPROG_DATA *mprg;

    if ( IS_NPC( mob )
        && ( mob->pIndexData->progtypes & MP_HITPRCNT_PROG ) )
    {
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	{
	    if ( ( mprg->type & MP_HITPRCNT_PROG )
		&& ( ( 100*mob->hit / mob->max_hit ) < atoi( mprg->arglist ) ) )
	    {
		mprog_driver( mprg->comlist, mob, ch, NULL, NULL, FALSE );
		break;
	    }
	}
    }
    return;
}


void
mprog_list_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    if ( IS_NPC( mob )
    && ( mob->pIndexData->progtypes & MP_LIST_PROG ) )
    {
	mprog_percent_check( mob, ch, NULL, NULL, MP_LIST_PROG );
    }
}


void
mprog_random_trigger( CHAR_DATA *mob )
{
    if ( mob->pIndexData->progtypes & MP_RAND_PROG )
    {
	mprog_percent_check( mob, NULL, NULL, NULL, MP_RAND_PROG );
    }

    return;
}


void
mprog_repop_trigger( CHAR_DATA *mob )
{
    if ( mob->pIndexData->progtypes & MP_REPOP_PROG )
    {
	mprog_percent_check( mob, NULL, NULL, NULL, MP_REPOP_PROG );
    }
}


void
mprog_speech_trigger( char* txt, CHAR_DATA* actor )
{
    CHAR_DATA *vmob;

    for ( vmob = actor->in_room->people; vmob; vmob = vmob->next_in_room )
    {
	if ( IS_NPC( vmob ) && IS_SET( vmob->pIndexData->progtypes, MP_SPEECH_PROG ) && !xIS_SET( vmob->affected_by, AFF_DEAF ) )
	{
//	    if ( IS_NPC( actor ) && actor->pIndexData == vmob->pIndexData )
	    if ( vmob == actor )
		continue;
	    mprog_wordlist_check( txt, vmob, actor, NULL, NULL, MP_SPEECH_PROG );
	}
    }
}


void
mprog_sell_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
    char	buf[MAX_INPUT_LENGTH];
    MPROG_DATA *mprg;

    if ( IS_NPC( mob )
    &&	 can_see( mob, ch )
    &&	 ( mob->pIndexData->progtypes & MP_SELL_PROG ) )
    {
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	{
	    one_argument( mprg->arglist, buf );
	    if ( ( mprg->type & MP_SELL_PROG )
		&& ( ( !str_cmp( obj->name, mprg->arglist ) )
		|| ( !str_cmp( "any", buf ) ) ) )
	    {
		mprog_driver( mprg->comlist, mob, ch, obj, NULL, FALSE );
		break;
	    }
	}
    }

    return;
}


void
mprog_tell_trigger( char *txt, CHAR_DATA *mob, CHAR_DATA *talker )
{
    if ( mob == talker )  /* guard against recursion */
	return;

    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & MP_TELL_PROG ) )
    {
	mprog_wordlist_check( txt, mob, talker, NULL, NULL, MP_TELL_PROG );
    }

    return;
}


void
mprog_time_trigger( void )
{
    CHAR_DATA *	vmob;
    CHAR_DATA *	vmob_next;
    MPROG_DATA *mprg;
    int		arg_hour;

    for ( vmob = char_list; vmob; vmob = vmob_next )
    {
	vmob_next = vmob->next;
	if ( vmob->deleted )
	    continue;
	if ( IS_NPC( vmob ) && ( vmob->pIndexData->progtypes & MP_TIME_PROG ) )
	{
	    for ( mprg = vmob->pIndexData->mobprogs; mprg; mprg = mprg->next )
	    {
		if ( mprg->type & MP_TIME_PROG )
		{
		    if ( !str_cmp( mprg->arglist, "sunrise" ) )
		        arg_hour = hour_sunrise;
                    else if ( !str_cmp( mprg->arglist, "sunset" ) )
                        arg_hour = hour_sunset;
                    else
                        arg_hour = atoi( mprg->arglist );
                    if ( arg_hour == time_info.hour )
		    {
                        mprog_driver( mprg->comlist, vmob, NULL, NULL, NULL, FALSE );
                        break;
                    }
		}
	    }
	}
    }
}


void
mprog_whisper_trigger( char *txt, CHAR_DATA *mob, CHAR_DATA *talker )
{
    if ( mob == talker )  /* guard against recursion */
	return;

    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & MP_WHISPER_PROG ) )
    {
	mprog_wordlist_check( txt, mob, talker, NULL, NULL, MP_WHISPER_PROG );
    }

    return;
}


void
oprog_buy_trigger( OBJ_DATA *obj, CHAR_DATA *ch, CHAR_DATA *vendor )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_BUY )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, vendor, OBJ_PROG_BUY );
	release_supermob( smob );
    }
    return;
}


void
oprog_cast_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_CAST )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_CAST );
	release_supermob( smob );
    }
    return;
}


void
oprog_cast_sn_trigger( OBJ_DATA *obj, CHAR_DATA *ch, int sn, void *vo )
{
}


void
oprog_close_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_CLOSE )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_CLOSE );
	release_supermob( smob );
    }
    return;
}


void
oprog_drop_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_DROP )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_DROP );
	release_supermob( smob );
    }
    return;
}


void
oprog_fill_trigger( OBJ_DATA *obj, CHAR_DATA *ch, OBJ_DATA *spring )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_FILL )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, spring, OBJ_PROG_FILL );
	release_supermob( smob );
    }
    return;
}


void
oprog_get_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_GET )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_GET );
	release_supermob( smob );
    }
    return;
}


void
oprog_get_from_trigger( OBJ_DATA *obj, CHAR_DATA *ch, OBJ_DATA *obj2 )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_GET_FROM )
    {
	smob = oset_supermob( obj );
	if ( !oprog_percent_check( smob, ch, obj, obj2, OBJ_PROG_GET_FROM ) )
	    oprog_get_trigger( obj, ch );
	release_supermob( smob );
    }
    else
	oprog_get_trigger( obj, ch );
    return;
}


void
oprog_give_trigger( OBJ_DATA *obj, CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_GIVE )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, victim, OBJ_PROG_GIVE );
	release_supermob( smob );
    }
    return;
}


void
oprog_invoke_trigger( OBJ_DATA *obj, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_INVOKE )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, vo, OBJ_PROG_INVOKE );
	release_supermob( smob );
    }
    return;
}


void
oprog_join_trigger( OBJ_DATA *obj, CHAR_DATA *ch, OBJ_DATA *obj2 )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_JOIN )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, obj2, OBJ_PROG_JOIN );
	release_supermob( smob );
    }
    return;
}


void
oprog_lock_trigger( OBJ_DATA *obj, CHAR_DATA *ch, OBJ_DATA *key )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_LOCK )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, key, OBJ_PROG_LOCK );
	release_supermob( smob );
    }
    return;
}


void
oprog_look_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_LOOK )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_LOOK );
	release_supermob( smob );
    }
    return;
}


void
oprog_look_in_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_LOOK_IN )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_LOOK_IN );
	release_supermob( smob );
    }
    return;
}


void
oprog_open_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_OPEN )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_OPEN );
	release_supermob( smob );
    }
    return;
}


void
oprog_pick_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_PICK )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_PICK );
	release_supermob( smob );
    }
    return;
}


void
oprog_put_trigger( OBJ_DATA *obj, CHAR_DATA *ch, OBJ_DATA *secondary )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_PUT )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, secondary, OBJ_PROG_PUT );
	release_supermob( smob );
    }
    return;
}


void
oprog_random_trigger( OBJ_DATA *obj )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_RANDOM )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, NULL, obj, NULL, OBJ_PROG_RANDOM );
	release_supermob( smob );
    }
    return;
}


void
oprog_read_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *		smob;
    OREPROG_DATA *	prg;
    int			page;
    char *		arglist;
    char		arg[MAX_INPUT_LENGTH];

    if ( ( obj->pIndexData->progtypes & OBJ_PROG_READ ) && obj->item_type == ITEM_BOOK )
    {
        page = obj->value[0] - 1;
        for ( prg = obj->pIndexData->oprogs; prg != NULL; prg = prg->next )
        {
            if ( prg->type != OBJ_PROG_READ )
                continue;
            arglist = prg->arglist;
            while ( *arglist != '\0' )
            {
                arglist = one_argument( arglist, arg );
                if ( page == atoi( arg ) )
                {
                    smob = oset_supermob( obj );
                    mprog_driver( prg->comlist, smob, ch, obj, NULL, FALSE );
                    release_supermob( smob );
                    return;
                }
            }
        }
    }
    return;
}


void
oprog_remove_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_REMOVE )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_REMOVE );
	release_supermob( smob );
    }
    return;
}


void
oprog_retrieve_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_RETRIEVE )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_RETRIEVE );
	release_supermob( smob );
    }
    return;
}


void
oprog_sell_trigger( OBJ_DATA *obj, CHAR_DATA *ch, CHAR_DATA *vendor )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_SELL )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, vendor, OBJ_PROG_SELL );
	release_supermob( smob );
    }
    return;
}


void
oprog_separate_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_SEPARATE )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_SEPARATE );
	release_supermob( smob );
    }
    return;
}


void
oprog_store_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_STORE )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_STORE );
	release_supermob( smob );
    }
    return;
}


void
oprog_throw_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_THROW )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_THROW );
	release_supermob( smob );
    }
    return;
}


void
oprog_unlock_trigger( OBJ_DATA *obj, CHAR_DATA *ch, OBJ_DATA *key )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_UNLOCK )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, key, OBJ_PROG_UNLOCK );
	release_supermob( smob );
    }
    return;
}


void
oprog_unuse_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *	smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_UNUSE )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_UNUSE );
	release_supermob( smob );
    }
    return;
}


bool
oprog_use_trigger( OBJ_DATA *obj, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *	smob;
    bool	fResult = FALSE;

    if ( obj->pIndexData->progtypes & OBJ_PROG_USE )
    {
	smob = oset_supermob( obj );
	fResult = oprog_percent_check( smob, ch, obj, vo, OBJ_PROG_USE );
	release_supermob( smob );
    }
    return fResult;
}


void
oprog_wear_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( obj->pIndexData->progtypes & OBJ_PROG_WEAR )
    {
	smob = oset_supermob( obj );
	oprog_percent_check( smob, ch, obj, NULL, OBJ_PROG_WEAR );
	release_supermob( smob );
    }
    return;
}


bool
rprog_arrive_trigger( ROOM_INDEX_DATA *here, ROOM_INDEX_DATA *there )
{
    CHAR_DATA *	smob;
    bool	fResult;

    fResult = FALSE;
    if ( here->progtypes & ROOM_PROG_ARRIVE )
    {
	smob = rset_supermob( here );
	rprog_roomlist_check( smob, here, there, ROOM_PROG_ARRIVE );
	release_supermob( smob );
    }
    return fResult;
}


void
rprog_boot_trigger( ROOM_INDEX_DATA *room )
{
    CHAR_DATA *	smob;

    if ( room->progtypes & ROOM_PROG_BOOT )
    {
	smob = rset_supermob( room );
	rprog_percent_check( smob, NULL, room, NULL, ROOM_PROG_BOOT );
	release_supermob( smob );
    }
    return;
}


void
rprog_cast_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room->progtypes & ROOM_PROG_CAST )
    {
	smob = rset_supermob( room );
	rprog_percent_check( smob, ch, room, NULL, ROOM_PROG_CAST );
	release_supermob( smob );
    }
    return;
}


void
rprog_cast_sn_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch, int sn, void *vo )
{
}


void
rprog_death_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room->progtypes & ROOM_PROG_DEATH )
    {
	smob = rset_supermob( room );
	rprog_percent_check( smob, ch, room, NULL, ROOM_PROG_DEATH );
	release_supermob( smob );
    }
    return;
}


bool
rprog_depart_trigger( ROOM_INDEX_DATA *here, ROOM_INDEX_DATA *there )
{
    CHAR_DATA *	smob;
    bool	fResult;

    fResult = FALSE;
    if ( here->progtypes & ROOM_PROG_DEPART )
    {
	smob = rset_supermob( here );
	rprog_roomlist_check( smob, here, there, ROOM_PROG_DEPART );
	release_supermob( smob );
    }
    return fResult;
}


void
rprog_enter_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room->progtypes & ROOM_PROG_ENTER )
    {
	smob = rset_supermob( room );
	rprog_percent_check( smob, ch, room, NULL, ROOM_PROG_ENTER );
	release_supermob( smob );
    }
    else
	rprog_pass_trigger( room, ch );
    return;
}


void
rprog_exit_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room->progtypes & ROOM_PROG_EXIT )
    {
	smob = rset_supermob( room );
	rprog_percent_check( smob, ch, room, NULL, ROOM_PROG_EXIT );
	release_supermob( smob );
    }
    else
	rprog_pass_trigger( room, ch );
    return;
}


bool
rprog_interp_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch, char *txt )
{
    CHAR_DATA *	smob;
    bool	result;

    result = FALSE;

    if ( room->progtypes & ROOM_PROG_INTERP )
    {
	smob = rset_supermob( room );
	result = rprog_wordlist_check( txt, smob, ch, room, NULL,
				       ROOM_PROG_INTERP );
	release_supermob( smob );
    }
    return result;
}


bool
rprog_listen_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch  )
{
    CHAR_DATA *	smob;
    bool	result;

    result = FALSE;
    if ( room->progtypes & ROOM_PROG_LISTEN )
    {
	smob = rset_supermob( room );
	result = rprog_percent_check( smob, ch, room, NULL, ROOM_PROG_LISTEN );
	release_supermob( smob );
    }
    return result;
}


bool
rprog_look_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *	smob;
    bool	result;

    result = FALSE;
    if ( room->progtypes & ROOM_PROG_LOOK )
    {
	smob = rset_supermob( room );
	result = rprog_wordlist_check( arg, smob, ch, room, NULL,
				       ROOM_PROG_LOOK );
	release_supermob( smob );
    }
    return result;
}


void
rprog_pass_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *smob;

    if ( room->progtypes & ROOM_PROG_PASS )
    {
	smob = rset_supermob( room );
	rprog_percent_check( smob, ch, room, NULL, ROOM_PROG_PASS );
	release_supermob( smob );
    }
    return;
}


bool
rprog_pull_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *	smob;
    bool	result;

    if ( room->progtypes & ROOM_PROG_PULL )
    {
	smob = rset_supermob( room );
	result = rprog_wordlist_check( arg, smob, ch, room, NULL,
				       ROOM_PROG_PULL );
	release_supermob( smob );
	return result;
    }
    return FALSE;
}


bool
rprog_push_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *	smob;
    bool	result;

    if ( room->progtypes & ROOM_PROG_PUSH )
    {
	smob = rset_supermob( room );
	result = rprog_wordlist_check( arg, smob, ch, room, NULL,
				       ROOM_PROG_PUSH );
	release_supermob( smob );
	return result;
    }
    return FALSE;
}


void rprog_random_trigger( ROOM_INDEX_DATA *room )
{
    CHAR_DATA *smob;

    if ( room->progtypes & ROOM_PROG_RANDOM )
    {
	smob = rset_supermob( room );
	rprog_percent_check( smob, NULL, room, NULL, ROOM_PROG_RANDOM );
	release_supermob( smob );
    }
    return;
}


bool
rprog_rest_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *	smob;
    bool	fResult;

    fResult = FALSE;
    if ( room->progtypes & ROOM_PROG_REST )
    {
	smob = rset_supermob( room );
	fResult = rprog_percent_check( smob, ch, room, NULL, ROOM_PROG_REST );
	release_supermob( smob );
    }
    return fResult;;
}


bool
rprog_sit_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *	smob;
    bool	fResult;

    fResult = FALSE;
    if ( room->progtypes & ROOM_PROG_SIT )
    {
	smob = rset_supermob( room );
	fResult = rprog_percent_check( smob, ch, room, NULL, ROOM_PROG_SIT );
	release_supermob( smob );
    }
    return fResult;
}


bool
rprog_sleep_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *	smob;
    bool	fResult;

    fResult = FALSE;
    if ( room->progtypes & ROOM_PROG_SLEEP )
    {
	smob = rset_supermob( room );
	rprog_percent_check( smob, ch, room, NULL, ROOM_PROG_SLEEP );
	release_supermob( smob );
    }
    return fResult;
}


bool
rprog_smell_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *	smob;
    bool	result;

    result = FALSE;
    if ( room->progtypes & ROOM_PROG_SMELL )
    {
	smob = rset_supermob( room );
	result = rprog_percent_check( smob, ch, room, NULL,
				       ROOM_PROG_SMELL );
	release_supermob( smob );
    }
    return result;
}


void
rprog_speech_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch, char *txt )
{
    CHAR_DATA *	smob;
    bool	result;

    if ( room->progtypes & ROOM_PROG_SPEECH )
    {
	smob = rset_supermob( room );
	result = rprog_wordlist_check( txt, smob, ch, room, NULL,
				       ROOM_PROG_SPEECH );
	release_supermob( smob );
    }
    return;
}


bool
rprog_stand_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch )
{
    CHAR_DATA *	smob;
    bool	fTriggered;

    fTriggered = FALSE;
    if ( room->progtypes & ROOM_PROG_STAND )
    {
	smob = rset_supermob( room );
	fTriggered = rprog_percent_check( smob, ch, room, NULL,
	                                  ROOM_PROG_STAND );
	release_supermob( smob );
    }
    return fTriggered;;
}


void
rprog_time_trigger( ROOM_INDEX_DATA *room, int hour )
{
    CHAR_DATA *		smob;
    char		stime[MAX_INPUT_LENGTH];
    char		etime[MAX_INPUT_LENGTH];
    int			sint;
    int			eint;
    OREPROG_DATA *	prog;
    char *		arg_p;

    if ( room->progtypes & ROOM_PROG_TIME )
    {
	for ( prog = room->rprogs; prog != NULL; prog = prog->next )
	{
	    if ( prog->type & ROOM_PROG_TIME )
	    {
		arg_p = one_argument( prog->arglist, stime );
		one_argument( arg_p, etime );

		if ( !str_cmp( stime, "sunrise" ) )
		    sprintf( stime, "%d", hour_sunrise );
                else if ( !str_cmp( stime, "sunset" ) )
                    sprintf( stime, "%d", hour_sunset );
                if ( !str_cmp( etime, "sunrise" ) )
                    sprintf( etime, "%d", hour_sunrise );
                else if ( !str_cmp( etime, "sunset" ) )
                    sprintf( etime, "%d", hour_sunset );

		if ( !is_number( stime ) || !is_number( etime )
		||   ( sint = atoi( stime ) ) < 0 || sint >= MAX_HOUR
		||   ( eint = atoi( etime ) ) < 0 || eint >= MAX_HOUR )
		{
		    buildbug( "Rprog time_prog: bad time '%s' or '%s' in room #%d.",
			      stime, etime, room->vnum );
		    continue;
		}
		if ( ( eint >= sint &&   hour <= eint && hour >= sint )
		||   ( eint <  sint && ( hour >= sint || hour <= eint ) ) )
		{
		    smob = rset_supermob( room );
		    mprog_driver( prog->comlist, smob, NULL, NULL, NULL, FALSE );
		    release_supermob( smob );
		}
	    }
	}
    }
}


void
rprog_wake_trigger( ROOM_INDEX_DATA *room, CHAR_DATA *ch, CHAR_DATA *rch )
{
    CHAR_DATA *smob;

    if ( room->progtypes & ROOM_PROG_WAKE )
    {
	smob = rset_supermob( room );
	rprog_percent_check( smob, ch, room, rch, ROOM_PROG_WAKE );
	release_supermob( smob );
    }
    return;
}


void
progbug( CHAR_DATA *mob, const char *fmt, ... )
{
    char	buf[MAX_STRING_LENGTH];
    char	buf1[MAX_STRING_LENGTH];
    int		vnum;
    int		room_vnum;
    va_list	args;

    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    va_end( args );

    room_vnum = mob != NULL ? mob->in_room != NULL ? mob->in_room->vnum : 0 : 0;

    /*
     * Check if we're dealing with supermob, which means the bug occurred
     * in a room or obj prog.
     */
    vnum = mob ? mob->pIndexData ? mob->pIndexData->vnum : 0 : 0;
    if ( vnum == MOB_VNUM_SUPERMOB )
    {
	/*
	 * It's supermob.  In oset_supermob and rset_supermob, the description
	 * was set to indicate the object or room, so we just need to show
	 * the description in the bug message.
	 */
	sprintf( buf1, "%s, %s in room #%d.", buf,
	      mob->description == NULL ? "(unknown)" : mob->description,
	      room_vnum );
    }
    else
    {
	sprintf( buf1, "%s, Mob #%d in room %d.", buf, vnum, room_vnum );
    }

    wiznet( "$t", mob, buf1, WIZ_BUILDBUG, 0, 0 );
    log_string( buf1 );
    return;
}

static void debug_log_string( const char *str )
{
#if defined( DEBUG )
    log_string( str );
#endif
    return;
}


bool
event_exit_mpdelay( EVENT_DATA *event )
{
    CHAR_DATA *		smob;
    EXIT_DATA *		exit;
    ROOM_INDEX_DATA *	room;
    int			dir;

    exit = event->owner.exit;
    if ( ( room = exit->from_room ) == NULL )
    {
        bugf( "Exit_mpdelay: mpdelay event with no room." );
        return FALSE;
    }

    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
        if ( room->exit[dir] == exit )
            break;
    }

    if ( dir >= MAX_DIR )
    {
        bugf( "Exit_mpdelay:  Cannot find exit for room %d", room->vnum );
        return FALSE;
    }

    smob = eset_supermob( room->exit[dir], room );
    interpret( smob, event->argument );
    release_supermob( smob );
    return FALSE;
}


bool
event_obj_mpdelay( EVENT_DATA *event )
{
    CHAR_DATA *smob;

    smob = oset_supermob( event->owner.obj );
    interpret( smob, event->argument );
    release_supermob( smob );
    return FALSE;
}


bool
event_room_mpdelay( EVENT_DATA *event )
{
    CHAR_DATA *smob;

    smob = rset_supermob( event->owner.room );
    interpret( smob, event->argument );
    release_supermob( smob );
    return FALSE;
}


