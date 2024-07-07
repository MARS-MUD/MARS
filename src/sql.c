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


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mysql/mysql.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "tables.h"


void
do_sqldump( CHAR_DATA *ch, char *argument )
{
    MYSQL	conn;
    char	arg[MAX_INPUT_LENGTH];
    char	query[MAX_STRING_LENGTH];
    int		index;

    if ( !sql_flag )
    {
        send_to_char( "MySQL not available.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    if ( !str_cmp( arg, "commands" ) )
    {
        char	name[SHORT_STRING_LENGTH];
        char	logflags[SHORT_STRING_LENGTH];
        char	cmdflags[SHORT_STRING_LENGTH];
        char	secflags[SHORT_STRING_LENGTH];

        mysql_init( &conn );
        if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd, sql_db,
			         sql_port, sql_socket, 0 ) == NULL )
        {
	    bugf( "Sqldump: mysql_real_connect: connect failed." );
	    log_string( mysql_error( &conn ) );
	    return;
        }

        strcpy( query, "DROP TABLE IF EXISTS commands" );
        if ( mysql_query( &conn, query ) != 0 )
        {
            bugf( "Sqldump: drop table query failed" );
            log_string( mysql_error( &conn ) );
            mysql_close( &conn );
            return;
        }

        strcpy( query, "CREATE TABLE commands ("
                       "name varchar(16) NOT NULL default '',"
                       "position varchar(8) default 'rest',"
                       "level int(11) default 0,"
                       "log varchar(12) default 'normal',"
                       "flags varchar(36) default '',"
                       "security varchar(36) default '')" );
        if ( mysql_query( &conn, query ) != 0 )
        {
            bugf( "Sqldump: create table query failed" );
            log_string( mysql_error( &conn ) );
            mysql_close( &conn );
            return;
        }

        for ( index = 0; !IS_NULLSTR( cmd_table[index].name ); index++ )
        {
            mysql_escape_string( name, cmd_table[index].name, strlen( cmd_table[index].name ) );
            strcpy( logflags, flag_string( log_types, cmd_table[index].log ) );
            strcpy( cmdflags, print_flags( cmd_table[index].flags ) );
            if ( cmdflags[0] == '0' )
                cmdflags[0] = '\0';
            strcpy( secflags, print_flags( cmd_table[index].sec_flags ) );
            if ( secflags[0] == '0' )
                secflags[0] = '\0';
            sprintf( query, "INSERT INTO commands "
                     "(name, position, level, log, flags, security) "
                     "VALUES ('%s','%s',%d,'%s','%s','%s')",
                     name,
                     position_table[cmd_table[index].position].short_name,
                     cmd_table[index].level,
                     logflags,
                     cmdflags,
                     secflags );
            if ( mysql_query( &conn, query ) != 0 )
            {
                bugf( "Sqldump: insert query failed" );
                log_string( mysql_error( &conn ) );
                mysql_close( &conn );
                return;
            }
        }

        mysql_close( &conn );
        return;
    }

    if ( !str_cmp( arg, "spells" ) || !str_cmp( arg, "skills" ) )
    {
        char *	p;
        char	buf[MAX_STRING_LENGTH];
        int	iClass;
        int	len = 0;

        mysql_init( &conn );
        if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd, sql_db,
			         sql_port, sql_socket, 0 ) == NULL )
        {
	    bugf( "Sqldump: mysql_real_connect: connect failed." );
	    log_string( mysql_error( &conn ) );
	    return;
        }

        strcpy( query, "DROP TABLE IF EXISTS skills" );
        if ( mysql_query( &conn, query ) != 0 )
        {
            bugf( "Sqldump: drop table query failed" );
            log_string( mysql_error( &conn ) );
            mysql_close( &conn );
            return;
        }

        p = stpcpy( query, "CREATE TABLE skills ("
                "name varchar(32),"
                "id int(4),"
                "spell char(6)," );

        for ( index = 0; index < MAX_CLASS; index++ )
            p += sprintf( p, "%s_lev int(3) default 513,", class_table[index].who_name );
        for ( index = 0; index < MAX_CLASS; index++ )
            p += sprintf( p, "%s_rate int(3) default 0,", class_table[index].who_name );
        for ( index = 0; index < MAX_PREREQ; index++ )
            p += sprintf( p, "Prereq%d int(7),", index );

        p = stpcpy( p, "target varchar(16)," );
        p = stpcpy( p, "position varchar(18)," );
        p = stpcpy( p, "mana int(3),beats int(3), forget int(3)," );
        p = stpcpy( p, "noun_damage varchar(12)," );
        p = stpcpy( p, "msg_off varchar(64)," );
        p = stpcpy( p, "msg_obj varchar(64)," );
        p = stpcpy( p, "msg_room varchar(64) )" );
        if ( mysql_query( &conn, query ) != 0 )
        {
            bugf( "Sqldump: create table query failed" );
            log_string( mysql_error( &conn ) );
            mysql_close( &conn );
            return;
        }

        for ( index = 0; !IS_NULLSTR( skill_table[index].name ); index++ )
        {
            p = stpcpy( query, "INSERT INTO skills VALUES (" );
            p += sprintf( p, "'%s',%d,", skill_table[index].name, index );
            p = stpcpy( p, skill_table[index].spell_fun == spell_null ? "'skill'," : "'spell'," );

            for ( iClass = 0; iClass < MAX_CLASS;  iClass++ )
                p += sprintf( p, "%d,", skill_table[index].skill_level[iClass] );
            for ( iClass = 0; iClass < MAX_CLASS;  iClass++ )
                p += sprintf( p, "%d,", skill_table[index].rating[iClass] );
            for ( iClass = 0; iClass < MAX_PREREQ; iClass++ )
                p += sprintf( p, "%d,", skill_table[index].prereq[iClass] );

            p += sprintf( p, "'%s',", flag_string( target_types, skill_table[index].target ) );
            p += sprintf( p, "'%s',", position_table[skill_table[index].minimum_position].name );
            p += sprintf( p, "%d,%d,%d,", skill_table[index].min_mana,
                                          skill_table[index].beats,
                                          skill_table[index].forget );
            if ( !IS_NULLSTR( skill_table[index].noun_damage ) ) /* noun_damage */
                mysql_escape_string( buf, skill_table[index].noun_damage,
                                     strlen( skill_table[index].noun_damage ) );
            else
                buf[0] = '\0';
            p += sprintf( p, "'%s',", buf );
            if ( !IS_NULLSTR( skill_table[index].msg_off ) )  /* msg_off */
                mysql_escape_string( buf, skill_table[index].msg_off,
                                     strlen( skill_table[index].msg_off ) );
            else
                buf[0] = '\0';
            p += sprintf( p, "'%s',", buf );
            if ( !IS_NULLSTR( skill_table[index].msg_obj ) )  /* msg_obj */
                mysql_escape_string( buf, skill_table[index].msg_obj,
                                     strlen( skill_table[index].msg_obj ) );
            else
                buf[0] = '\0';
            p += sprintf( p, "'%s',", buf );
            if ( !IS_NULLSTR( skill_table[index].msg_room ) ) /* msg_room */
                mysql_escape_string( buf, skill_table[index].msg_room,
                                     strlen( skill_table[index].msg_room ) );
            else
                buf[0] = '\0';
            p += sprintf( p, "'%s')", buf );

            len = UMAX( len, strlen( query ) );
            if ( mysql_query( &conn, query ) != 0 )
            {
                bugf( "Sqldump: insert query failed" );
                log_string( mysql_error( &conn ) );
                mysql_close( &conn );
                return;
            }
        }

        ch_printf( ch, "Longest query %d.\n\r", len );

        mysql_close( &conn );
        return;
    }

    /* option not found -- display help */
    send_to_char( "Syntax: sqldump commands - rebuild commands table.\n\r", ch );
    send_to_char( "        sqldump spells   - rebuild skills table.\n\r", ch );
    send_to_char( "        sqldump skills   - (same)\n\r", ch );

    return;
}


void
sql_add_user( CHAR_DATA *ch )
{
    MYSQL	conn;
    char	query[MAX_STRING_LENGTH];
    char	pwd[SHORT_STRING_LENGTH];

    if ( IS_NPC( ch ) )
	return;

    if ( !sql_flag )
	return;

    mysql_escape_string( pwd, ch->pcdata->pwd, strlen( ch->pcdata->pwd ) );
    mysql_init( &conn );
    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd,
	 sql_db, sql_port, sql_socket, 0 ) == NULL )
	return;

    sprintf( query, "INSERT INTO users(name, class, password, host, id, "
		    "last_log, level, logout, race) VALUES ("
		    "'%s', '%s', '%s', '%s', %ld, "
		    "%ld, %d, %ld, '%s')",
	     capitalize( ch->name ), class_table[ch->class].who_name, pwd,
	     ch->desc != NULL ? ch->desc->host : "(unknown)",
	     ch->id, ch->logon, ch->level, current_time, race_table[ch->race].who_name );
    if ( mysql_query( &conn, query ) != 0 )
    {
	bugf( "Sql_add_user: mysql_query: query failed." );
	log_string( mysql_error( &conn ) );
    }

    mysql_close( &conn );
    return;
}


void
sql_remove_user( CHAR_DATA *ch )
{
    MYSQL	conn;
    char	query[MAX_INPUT_LENGTH];

    if ( !sql_flag )
	return;

    mysql_init( &conn );
    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd,
	 sql_db, sql_port, sql_socket, 0 ) == NULL )
	return;
    sprintf( query, "DELETE FROM users WHERE name = '%s'", ch->name );
    mysql_query( &conn, query );
    mysql_close( &conn );
    return;
}


void
sql_save_char( CHAR_DATA *ch )
{
    MYSQL	 conn;
    char	 query[MAX_INPUT_LENGTH];
    char	 pwd[SHORT_STRING_LENGTH];
    my_ulonglong rows;

    if ( !sql_flag || ch == NULL || IS_NPC( ch ) )
	return;

    mysql_escape_string( pwd, ch->pcdata->pwd, strlen( ch->pcdata->pwd ) );
    mysql_init( &conn );
    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd, sql_db,
			     sql_port, sql_socket, 0 ) == NULL )
    {
	bugf( "Sql_save_char: mysql_real_connect: connect failed." );
	log_string( mysql_error( &conn ) );
	return;
    }

    sprintf( query, "UPDATE users SET "
	     "password = '%s', "
	     "id = %ld, "
	     "last_log = %ld, "
	     "level = %d, "
	     "logout = %ld "
	     "WHERE name = '%s'",

	     pwd,
	     ch->id,
	     ch->logon,
	     IS_AVATAR( ch ) ? ch->pcdata->avatar->level : ch->level,
	     current_time,
	     ch->name );

    if ( mysql_query( &conn, query ) != 0 )
    {
	bugf( "Sql_save_char: mysql_query: query failed." );
	log_string( mysql_error( &conn ) );
    }

    rows = mysql_affected_rows( &conn );

    if ( ch->desc != NULL && !IS_NULLSTR( ch->desc->host ) )
    {
	sprintf( query, "UPDATE users SET host = '%s' "
			"WHERE name = '%s'",
		 ch->desc->host, ch->name );
	if ( mysql_query( &conn, query ) != 0 )
	{
	    bugf( "Sql_save_char: mysql_query: host update failed." );
	    log_string( mysql_error( &conn ) );
	}
    }

    if ( !IS_SET( ch->act2, PLR_PLOADED ) )
    {
	sprintf( query, "UPDATE users SET logout = %ld WHERE name = '%s'",
		 current_time, ch->name );
	if ( mysql_query( &conn, query ) != 0 )
	{
	    bugf( "Sql_save_char: mysql_query: logout update failed." );
	    log_string( mysql_error( &conn ) );
	}
    }

    mysql_close( &conn );

    if ( rows == (my_ulonglong)0 )
        sql_add_user( ch );

    return;
}


void
sql_update_level( CHAR_DATA *ch )
{
    MYSQL	conn;
    char	query[MAX_INPUT_LENGTH];

    if ( !sql_flag || ch == NULL || IS_NPC( ch ) )
	return;

    if ( IS_AVATAR( ch ) )
        return;

    mysql_init( &conn );
    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd, sql_db,
			     sql_port, sql_socket, 0 ) == NULL )
    {
	bugf( "Sql_update_level: mysql_real_connect: connect failed." );
	log_string( mysql_error( &conn ) );
	return;
    }

    sprintf( query, "UPDATE users SET level = %d WHERE name = '%s'",
	     ch->level, ch->name );
    if ( mysql_query( &conn, query ) != 0 )
    {
	bugf( "Sql_update_level: mysql_query: query failed." );
	log_string( mysql_error( &conn ) );
    }

    mysql_close( &conn );
    return;
}


void
sql_update_logout( CHAR_DATA *ch )
{
    MYSQL	conn;
    char	query[MAX_INPUT_LENGTH];

    if ( !sql_flag || ch == NULL || IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act2, PLR_PLOADED ) )
	return;

    mysql_init( &conn );
    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd, sql_db,
			     sql_port, sql_socket, 0 ) == NULL )
    {
	bugf( "Sql_update_logout: mysql_real_connect: connect failed." );
	log_string( mysql_error( &conn ) );
	return;
    }

    sprintf( query, "UPDATE users SET logout = %ld WHERE name = '%s'",
	     current_time, ch->name );
    if ( mysql_query( &conn, query ) != 0 )
    {
	bugf( "Sql_update_logout: mysql_query: query failed." );
	log_string( mysql_error( &conn ) );
    }

    mysql_close( &conn );
    return;
}


void
sql_update_name( const char *oldname, const char *newname )
{
    MYSQL	conn;
    char	query[MAX_INPUT_LENGTH];

    if ( !sql_flag || IS_NULLSTR( oldname ) || IS_NULLSTR( newname ) )
        return;

    mysql_init( &conn );
    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd, sql_db,
			     sql_port, sql_socket, 0 ) == NULL )
    {
	bugf( "Sql_update_name: mysql_real_connect: connect failed." );
	log_string( mysql_error( &conn ) );
	return;
    }

    sprintf( query, "UPDATE users SET name = '%s' WHERE name = '%s'",
	     newname, oldname );
    if ( mysql_query( &conn, query ) != 0 )
    {
	bugf( "Sql_update_name: mysql_query: query failed." );
	log_string( mysql_error( &conn ) );
    }

    mysql_close( &conn );
    return;
}


void
sql_update_password( CHAR_DATA *ch )
{
    MYSQL	conn;
    char	query[MAX_INPUT_LENGTH];
    char	pwd[SHORT_STRING_LENGTH];

    if ( !sql_flag || ch == NULL || IS_NPC( ch ) )
	return;

    mysql_escape_string( pwd, ch->pcdata->pwd, strlen( ch->pcdata->pwd ) );
    mysql_init( &conn );
    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd, sql_db,
			     sql_port, sql_socket, 0 ) == NULL )
    {
	bugf( "Sql_update_password: mysql_real_connect: connect failed." );
	log_string( mysql_error( &conn ) );
	return;
    }

    sprintf( query, "UPDATE users SET password = '%s' WHERE name = '%s'",
	     pwd, ch->name );
    if ( mysql_query( &conn, query ) != 0 )
    {
	bugf( "Sql_update_password: mysql_query: query failed." );
	log_string( mysql_error( &conn ) );
    }

    mysql_close( &conn );
    return;
}


void
sql_update_player_clan( CHAR_DATA *ch )
{
    MYSQL	conn;
    char	query[MAX_INPUT_LENGTH];
    char *	clan;
    int		clvl;

    if ( !sql_flag || ch == NULL || IS_NPC( ch ) )
	return;

    if ( ch->clan != NULL )
    {
        clan = ch->clan->name;
        clvl = ch->clvl;
    }
    else
    {
        clan = "None";
        clvl = 0;
    }

    mysql_init( &conn );
    if ( mysql_real_connect( &conn, sql_host, sql_user, sql_passwd, sql_db,
			     sql_port, sql_socket, 0 ) == NULL )
    {
	bugf( "Sql_update_player_clan: mysql_real_connect: connect failed." );
	log_string( mysql_error( &conn ) );
	return;
    }

    sprintf( query, "UPDATE users SET clan = '%s', clvl = %d WHERE name = '%s'",
             clan, clvl, ch->name );
    if ( mysql_query( &conn, query ) != 0 )
    {
	bugf( "Sql_update_player_clan: mysql_query: query failed." );
	log_string( mysql_error( &conn ) );
    }

    mysql_close( &conn );
    return;
}

