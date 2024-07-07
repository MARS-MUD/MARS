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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "lookup.h"
#include "magic.h"
#include "olc.h"
#include "recycle.h"
#include "tables.h"


/*
 * Local functions
 */


void
do_dreamedit( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    char		arg2[MAX_INPUT_LENGTH];
    char		arg3[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *	d;
    DREAM_DATA *	pDream;
    DREAM_DATA *	pPrev;
    int			vnum;

    argument = one_argument( argument, arg );

    if ( IS_NPC( ch ) )
    {
        send_to_char( "Dreams are of no concern to you.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char( "DreamEdit:  There is no default dream to edit.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "create" ) )
    {
        if ( dreamedit_create( ch, "" ) )
        {
            ch->desc->editor = ED_DREAM;
            dreamedit_show( ch, "" );
        }
        return;
    }

    if ( !str_cmp( arg, "delete" ) )
    {
        argument = one_argument( argument, arg2 );
        argument = one_argument( argument, arg3 );
        if ( str_cmp( arg2, "dream" ) || !is_number( arg3 ) )
        {
            send_to_char( "Syntax:  dreamedit delete dream <vnum>\n\r", ch );
            return;
        }

        if ( dream_list == NULL )
        {
            send_to_char( "DreamEdit:  There are no dreams to delete.\n\r", ch );
            return;
        }

        vnum = atoi( arg3 );
        if ( vnum < 1 )
        {
            ch_printf( ch, "DreamEdit:  Bad dream vnum %d.\n\r", vnum );
            return;
        }

        if ( ( pDream = get_dream_index( vnum ) ) == NULL )
        {
            send_to_char( "DreamEdit:  That dream vnum does not exist.\n\r", ch );
            return;
        }

        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING
            &&   d != ch->desc
            &&   ( d->pEdit == pDream || d->inEdit == pDream ) )
            {
                send_to_char( "DreamEdit:  Somebody else is editing that dream.\n\r", ch );
                return;
            }
        }

        if ( ch->desc->inEdit == pDream )
            edit_done( ch );
        if ( ch->desc->pEdit == pDream )
            edit_done( ch );

        if ( pDream == dream_list )
        {
            dream_list = pDream->next;
        }
        else
        {
            for ( pPrev = dream_list; pPrev != NULL; pPrev = pPrev->next )
            {
                if ( pPrev->next == pDream )
                {
                    pPrev->next = pDream->next;
                    break;
                }
            }
            if ( pPrev == NULL )
            {
                bugf( "Dream delete: vnum %d not found.", pDream->vnum );
                send_to_char( "DreamEdit:  internal error.\n\r", ch );
                return;
            }
        }

        free_dream( pDream );
        send_to_char( "Dream deleted.\n\r", ch );

        changed_dream = TRUE;

        return;
    }

    if ( !is_number( arg ) || ( pDream = get_dream_index( atoi( arg ) ) ) == NULL )
    {
        send_to_char( "DreamEdit:  That dream does not exist.\n\r", ch );
        return;
    }

    ch->desc->pEdit = (void *)pDream;
    ch->desc->editor = ED_DREAM;
    dreamedit_show( ch, "" );

    return;
}


void
dreamedit( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    int			cmd;
    char		command[MAX_INPUT_LENGTH];
    DREAM_DATA *	pDream;
    int			value;

    EDIT_DREAM( ch, pDream );

    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
        dreamedit_show( ch, "" );
        return;
    }

    if ( !str_cmp( command, "done" ) )
    {
        edit_done( ch );
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    /* Shortcut to dreamedit_edit() */
    if ( is_number( command ) )
    {
        changed_dream |= dreamedit_edit( ch, command );
        return;
    }

    /* Handle flags */
    if ( ( value = flag_value( dream_types, command ) ) != NO_FLAG )
    {
        pDream->type = value;
        send_to_char( "Type set.\n\r", ch );
        changed_dream = TRUE;
        return;
    }

    /* Handle flags */
    if ( ( value = flag_value( dream_flags, command ) ) != NO_FLAG )
    {
        TOGGLE_BIT( pDream->dream_flags, value );
        send_to_char( "Flag toggled.\n\r", ch );
        changed_dream = TRUE;
        return;
    }

    if ( !str_cmp( command, "all" ) )
        value = SEX_EITHER;
    else
        value = sex_lookup( command );
    if ( value != NO_FLAG )
    {
        pDream->sex = value;
        send_to_char( "Gender set.\n\r", ch );
        changed_dream = TRUE;
        return;
    }

    /* Search table and dispatch command */
    for ( cmd = 0; *dreamedit_table[cmd].name != '\0'; cmd++ )
    {
	if ( !str_prefix( command, dreamedit_table[cmd].name ) )
	{
	    changed_dream |= (*dreamedit_table[cmd].olc_fun) ( ch, argument );
	    return;
	}
    }

    /* Default to standard interpreter */
    interpret( ch, arg );
    return;
}


bool
dreamedit_add( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;
    DREAMSEG_DATA*	pSeg;
    DREAMSEG_DATA *	pTmp;

    EDIT_DREAM( ch, pDream );

    pSeg = new_dream_seg( );

    if ( pDream->seg == NULL )
        pDream->seg = pSeg;
    else
    {
        for ( pTmp = pDream->seg; pTmp->next != NULL; pTmp = pTmp->next )
            ;
        pTmp->next = pSeg;
    }

    string_append( ch, &pSeg->text );

    return TRUE;
}


bool
dreamedit_addaffect( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;
    AFFECT_DATA *	pAf;
    char		loc[MAX_INPUT_LENGTH];
    char		mod[MAX_INPUT_LENGTH];
    char		dur[MAX_INPUT_LENGTH];
    int			value;

    EDIT_DREAM( ch, pDream );

    argument = one_argument( argument, loc );
    argument = one_argument( argument, mod );
    argument = one_argument( argument, dur );

    if ( loc[0] == '\0' || mod[0] == '\0' || !is_number( mod )
    ||	 dur[0] == '\0' || !is_number( dur ) )
    {
	send_to_char( "Syntax:  addaffect [location] [#mod] [duration]\n\r", ch );
	return FALSE;
    }

    if ( atoi( dur ) < 1 )
    {
        send_to_char( "DreamEdit:  Duration must be positive value.\n\r", ch );
        return FALSE;
    }

    if ( ( value = flag_value( apply_types, loc ) ) == NO_FLAG )
    {
        send_to_char( "Valid affects are:\n\r", ch );
	show_help( ch, "apply" );
	return FALSE;
    }

    pAf             =   new_affect( );
    pAf->where	    =	TO_AFFECTS;
    pAf->type       =   gsn_dream;
    pAf->level	    =	LEVEL_HERO;
    pAf->location   =   value;
    pAf->duration   =   atoi( dur );
    pAf->modifier   =   atoi( mod );
    pAf->bitvector  =   AFF_DREAM;
    pAf->next       =   pDream->affect;
    pDream->affect  =   pAf;

    send_to_char( "Affect added.\n\r", ch );
    return TRUE;
}


bool
dreamedit_author( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;
    char 		arg[MAX_INPUT_LENGTH];

    EDIT_DREAM( ch, pDream );

    argument = first_arg( argument, arg, FALSE );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  author <author>\n\r", ch );
        return FALSE;
    }

    free_string( pDream->author );

    if ( !str_cmp( argument, "none" ) )
        pDream->author = str_dup( "" );
    else
        pDream->author = str_dup( arg );

    send_to_char( "Author set.\n\r", ch );
    return TRUE;
}


bool
dreamedit_class( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;
    char		arg[MAX_INPUT_LENGTH];
    int			value;

    EDIT_DREAM( ch, pDream );
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  class <class>\n\r", ch );
        return FALSE;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        xCLEAR_BITS( pDream->class_flags );
        send_to_char( "Class flags cleared.\n\r", ch );
        return TRUE;
    }

    if ( ( value = class_lookup( arg ) ) != NO_CLASS )
    {
        xTOGGLE_BIT( pDream->class_flags, value );
        send_to_char( "Class flag toggled.\n\r", ch );
        return TRUE;
    }

    send_to_char( "DreamEdit:  Class not found.\n\r", ch );
    return FALSE;
}


bool
dreamedit_create( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;
    int			vnum;

    /* find a vnum */
    vnum = 1;
    for ( pDream = dream_list; pDream != NULL; pDream = pDream->next )
    {
        if ( vnum < pDream->vnum )
            break;
        vnum++;
    }

    pDream = new_dream( );
    pDream->vnum  = vnum;
    pDream->seg   = NULL;
    pDream->type  = DREAM_NORMAL;

    insert_dream( pDream );

    ch->desc->pEdit = (void *)pDream;
    send_to_char( "Dream created.\n\r", ch );
    return TRUE;
}


bool
dreamedit_delaffect( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;;
    AFFECT_DATA *	pAf;
    AFFECT_DATA *	pAf_next;
    char		affect[MAX_INPUT_LENGTH];
    int			value;
    int			cnt;

    EDIT_DREAM( ch, pDream );

    one_argument( argument, affect );

    if ( !is_number( affect ) || affect[0] == '\0' )
    {
        send_to_char( "Syntax:  delaffect [#affect]\n\r", ch );
        return FALSE;
    }

    if ( ( pAf = pDream->affect ) == NULL )
    {
        send_to_char( "DreamEdit:  Dream has no affects.\n\r", ch );
        return FALSE;
    }

    value = atoi( affect );

    if ( value < 0 )
    {
        send_to_char( "DreamEdit:  no such affect.\n\r", ch );
        return FALSE;
    }

    if ( value == 0 )
    {
        pAf = pDream->affect;
        pDream->affect = pAf->next;
        free_affect( pAf );
    }
    else
    {
	cnt = 0;
	while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
	     pAf = pAf_next;

	if ( pAf_next != NULL )
	{
	    pAf->next = pAf_next->next;
	    free_affect( pAf_next );
	}
	else
	{
	     send_to_char( "DreamEdit:  No such affect.\n\r", ch );
	     return FALSE;
	}
    }

    send_to_char( "Affect removed.\n\r", ch );
    return TRUE;

}


bool
dreamedit_delete( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    DREAM_DATA *	pDream;
    DREAMSEG_DATA *	pSeg;
    DREAMSEG_DATA *	pPrev;
    int			count;
    int			number;

    EDIT_DREAM( ch, pDream );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
        send_to_char( "Syntax:  delete <segment number>\n\r", ch );
        return FALSE;
    }

    if ( pDream->seg == NULL )
    {
        send_to_char( "DreamEdit:  This dream has no segments to delete.\n\r", ch );
        return FALSE;
    }

    number = atoi( arg );

    if ( number == 1 )
    {
        pSeg = pDream->seg;
        pDream->seg = pSeg->next;
        free_dream_seg( pSeg );
        send_to_char( "Dream segment deleted.\n\r", ch );
        return TRUE;
    }

    count = 1;
    for ( pPrev = pDream->seg; pPrev->next != NULL; pPrev = pPrev->next )
    {
        count++;
        if ( count == number )
            break;
    }

    if ( pPrev->next == NULL )
    {
        send_to_char( "DreamEdit:  Dream segment not found.\n\r", ch );
        if ( count == 1 )
            send_to_char( "Available segment to delete: 1.\n\r", ch );
        else
            ch_printf( ch, "Available segments to delete: 1 - %d.\n\r", count );
        return FALSE;
    }

    pSeg = pPrev->next;
    pPrev->next = pSeg->next;
    free_dream_seg( pSeg );
    send_to_char( "Dream segment deleted.\n\r", ch );
    return TRUE;
}


bool
dreamedit_edit( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    DREAM_DATA *	pDream;
    DREAMSEG_DATA *	pSeg;
    int			count;
    int			number;

    EDIT_DREAM( ch, pDream );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
        send_to_char( "Syntax:  edit <segment number>\n\r", ch );
        return FALSE;
    }

    if ( pDream->seg == NULL )
    {
        send_to_char( "DreamEdit:  This dream has no segments to edit.\n\r", ch );
        return FALSE;
    }

    number = atoi( arg );
    count = 0;

    for ( pSeg = pDream->seg; pSeg != NULL; pSeg = pSeg->next )
    {
        count++;
        if ( count == number )
            break;
    }

    if ( pSeg == NULL )
    {
        send_to_char( "DreamEdit:  Dream segment not found.\n\r", ch );
        if ( count == 1 )
            send_to_char( "Available segment to edit: 1.\n\r", ch );
        else
            ch_printf( ch, "Available segments to edit: 1 - %d.\n\r", count );
        return FALSE;
    }

    string_append( ch, &pSeg->text );

    return TRUE;
}


bool
dreamedit_insert( CHAR_DATA *ch, char *argument )
{
    char		arg[MAX_INPUT_LENGTH];
    DREAM_DATA *	pDream;
    DREAMSEG_DATA *	pSeg;
    DREAMSEG_DATA *	pPrev;
    int			count;
    int			number;

    EDIT_DREAM( ch, pDream );

    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )
    {
        send_to_char( "Syntax:  insert <segment number>\n\r", ch );
        return FALSE;
    }

    number = atoi( arg );

    if ( number < 1 )
    {
        ch_printf( ch, "DreamEdit:  Bad value %d.\n\r", number );
        return FALSE;
    }

    if ( number == 1 )
    {
        pSeg = new_dream_seg( );
        pSeg->next = pDream->seg;
        pDream->seg = pSeg;
        string_append( ch, &pSeg->text );
        return TRUE;
    }

    if ( pDream->seg == NULL )
    {
        send_to_char( "DreamEdit:  This dream has no segments to insert in front of.\n\r", ch );
        return FALSE;
    }

    count = 1;
    for ( pPrev = pDream->seg; pPrev->next != NULL; pPrev = pPrev->next )
    {
        count++;
        if ( count == number )
            break;
    }

    if ( pPrev->next == NULL )
    {
        send_to_char( "DreamEdit:  This dream does not have that many segments.\n\r", ch );
        return FALSE;
    }

    pSeg = new_dream_seg( );
    pSeg->next = pPrev->next;
    pPrev->next = pSeg;
    string_append( ch, &pSeg->text );
    return TRUE;
}


bool
dreamedit_race( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;
    char		arg[MAX_INPUT_LENGTH];
    int			value;

    EDIT_DREAM( ch, pDream );
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  race <race>\n\r", ch );
        return FALSE;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        xCLEAR_BITS( pDream->race_flags );
        send_to_char( "Race flags cleared.\n\r", ch );
        return TRUE;
    }

    if ( ( value = race_lookup( arg ) ) != NO_CLASS )
    {
        xTOGGLE_BIT( pDream->race_flags, value );
        send_to_char( "Race flag toggled.\n\r", ch );
        return TRUE;
    }

    send_to_char( "DreamEdit:  Race not found.\n\r", ch );
    return FALSE;
}


bool
dreamedit_show( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;
    BUFFER *		pBuf;

    EDIT_DREAM( ch, pDream );

    pBuf = new_buf( );
    show_dream_info ( pDream, pBuf );
    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return FALSE;
}


bool
dreamedit_title( CHAR_DATA *ch, char *argument )
{
    DREAM_DATA *	pDream;
    char		oldtitle[MAX_INPUT_LENGTH];
    char *		newtitle;
    bool		fChanged;

    EDIT_DREAM( ch, pDream );

    if ( *argument == '\0' )
    {
        send_to_char( "Syntax:  title [title]\n\r", ch );
        return FALSE;
    }

    strcpy( oldtitle, pDream->title );
    if ( !str_cmp( argument, "none" ) )
    {
        free_string( pDream->title );
        pDream->title = &str_empty[0];
        fChanged = TRUE;
    }
    else
    {
        newtitle = string_change( ch, pDream->title, argument );
        if ( newtitle != NULL )
        {
            pDream->title = newtitle;
            fChanged = TRUE;
        }
        else
        {
            pDream->title = str_dup( oldtitle );
            fChanged = FALSE;
        }
    }

    send_to_char( fChanged ? *pDream->title != '\0' ? "Title set.\n\r" : "Title removed." : "Title not changed.\n\r", ch );
    return fChanged;
}


void
show_dream_info( DREAM_DATA *pDream, BUFFER *pBuf )
{
    DREAMSEG_DATA *	pSeg;
    char		buf[MAX_INPUT_LENGTH];
    AFFECT_DATA *	paf;
    char *		p;
    int			count;

    buf_printf( pBuf, "Vnum:      [%d]\n\r", pDream->vnum );
    buf_printf( pBuf, "Author:    [%s]\n\r", IS_NULLSTR( pDream->author ) ? "(none)" : pDream->author );
    buf_printf( pBuf, "Title:     [%s]`X\n\r", IS_NULLSTR( pDream->title ) ? "(none)" : pDream->title );
    buf_printf( pBuf, "Type:      [%s]\n\r", flag_string( dream_types, pDream->type ) );
    buf_printf( pBuf, "Gender:    [%s]\n\r", sex_name( pDream->sex ) );
    buf_printf( pBuf, "Flags:     [%s]\n\r",
                flag_string( dream_flags, pDream->dream_flags ) );

    add_buf( pBuf, "Class:     [" );
    p = buf;
    for ( count = 0; count < MAX_CLASS; count++ )
        if ( xIS_SET( pDream->class_flags, count ) )
            p += sprintf( p, " %s", class_table[count].name );
    if ( p == buf )
        strcpy( buf, " all" );
    buf_printf( pBuf, "%s]\n\r", buf + 1 );

    add_buf( pBuf, "Race:      [" );
    p = buf;
    for ( count = 1; !IS_NULLSTR( race_table[count].name ); count++ )
        if ( xIS_SET( pDream->race_flags, count ) )
            p += sprintf( p, " %s", race_table[count].name );
    if ( p == buf )
        strcpy( buf, " all" );
    buf_printf( pBuf, "%s]\n\r", buf + 1 );

    count = 0;
    for ( paf = pDream->affect; paf != NULL; paf = paf->next )
    {
        if ( count == 0 )
        {
            add_buf( pBuf, "No Modifier Duration Affects\n\r" );
            add_buf( pBuf, "-- -------- -------- -------\n\r" );
        }
        buf_printf( pBuf, "%2d %8d %8d %s\n\r", count,
                    paf->modifier, paf->duration,
                    flag_string( apply_types, paf->location ) );
        count++;
    }
    add_buf( pBuf, "\n\r" );

    count = 0;
    for ( pSeg = pDream->seg; pSeg != NULL; pSeg = pSeg->next )
    {
        p = pSeg->text;
        count++;
        buf_printf( pBuf, "[%2d] %s`X\n\r", count, p );
    }

    if ( count == 0 )
        add_buf( pBuf, "No dream segments currently set.\n\r" );

    return;
}

