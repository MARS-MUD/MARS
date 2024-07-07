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
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdarg.h>
#include <unistd.h>

#include "merc.h"
#include "db.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"


/* externals for counting purposes */
extern	OBJ_DATA	*obj_free;
extern	CHAR_DATA	*char_free;
extern  DESCRIPTOR_DATA *descriptor_free;
extern	PC_DATA		*pcdata_free;
extern  AFFECT_DATA	*affect_free;
int               reboot_counter;

/*
 * Globals.
 */
KILL_DATA		kill_table	[MAX_LEVEL];



/*
 * Locals.
 */
MOB_INDEX_DATA *	mob_index_hash		[MAX_KEY_HASH];
OBJ_INDEX_DATA *	obj_index_hash		[MAX_KEY_HASH];
ROOM_INDEX_DATA *	room_index_hash		[MAX_KEY_HASH];
char *			string_hash		[MAX_KEY_HASH];

char *			string_space;
char *			top_string;
int			top_str_dup;
int			top_str_len;
char			str_empty	[1];

int			top_affect;
int			top_ed;
int 			mobile_count = 0;
int			newmobs = 0;
int			newobjs = 0;

static int		dup_count = 0;
static int		dup_len = 0;

/*
 * Memory management.
 * Increase MAX_STRING if you have to.
 * Tune the others only if you understand what you're doing.
 */
#define			MAX_STRING	8000000
#define			MAX_PERM_BLOCK	131072
#define			MAX_MEM_LIST	12

void *			rgFreeList	[MAX_MEM_LIST];
const int		rgSizeList	[MAX_MEM_LIST]	=
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768, 65536-64
};

int			nAllocString;
int			sAllocString;
int			nAllocPerm;
int			sAllocPerm;

int			last_nAllocPerm;
int			last_sAllocPerm;


/*
 * Semi-locals.
 */
int			area_version;
bool			fBootDb;
FILE *			fpArea;
char			strArea[MAX_INPUT_LENGTH];


void	check_mem_change	args( ( void ) );


/*
 * Local booting procedures.
*/
void    init_mm         args( ( void ) );
void	load_area	args( ( FILE *fp ) );
void	load_bans	args( ( void ) );
void	load_helps	args( ( FILE *fp ) );
void	load_rooms	args( ( FILE *fp ) );
void	load_shops	args( ( FILE *fp ) );
void	load_specials	args( ( FILE *fp ) );
void	load_whoborder	args( ( void ) );

void	bad_reset	args( ( AREA_DATA *pArea, RESET_DATA *pReset,
				int c, char *txt ) );
void	fix_exits	args( ( void ) );
void	fix_all_overlands args( ( void ) );
void	validate_resets	args( ( void ) );


/*
 * Big mama top level function.
 */
void
boot_db( void )
{

    /*
     * Init some data space stuff.
     */
    {
	if ( ( string_space = calloc( 1, MAX_STRING ) ) == NULL )
	{
	    bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
	    abort_mud( "Can't alloc string space" );
	}
	top_string	= string_space;
	fBootDb		= TRUE;
    }

    /*
     * Init random number generator.
     */
    {
        init_mm( );
    }

    /*
     * Set time and weather.
     */
    {
	long lhour, lday, lmonth;

	lhour		= (current_time - 650336715)
			/ (PULSE_TICK / PULSE_PER_SECOND);
	time_info.hour	= lhour  % HOURS_PER_DAY;
	lday		= lhour  / HOURS_PER_DAY;
	time_info.day	= lday   % DAYS_PER_MONTH;
	lmonth		= lday   / DAYS_PER_MONTH;
	time_info.month	= lmonth % MONTHS_PER_YEAR;
	time_info.year	= lmonth / MONTHS_PER_YEAR;

	     if ( time_info.hour <  5 ) weather_info.sunlight = SUN_DARK;
	else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
	else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
	else if ( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
	else                            weather_info.sunlight = SUN_DARK;

	weather_info.change	= 0;
	weather_info.mmhg	= 960;
	if ( time_info.month >= 7 && time_info.month <=12 )
	    weather_info.mmhg += number_range( 1, 50 );
	else
	    weather_info.mmhg += number_range( 1, 80 );

	     if ( weather_info.mmhg <=  980 ) weather_info.sky = SKY_LIGHTNING;
	else if ( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
	else if ( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
	else                                  weather_info.sky = SKY_CLOUDLESS;

	/* We'll want to randomize this a bit */
	weather_info.wind_dir = DIR_EAST;
	weather_info.wind_speed = WIND_MODERATE;

	switch( time_info.month )
	{
	    case 0:
	    case 8:
		hour_sunrise = HOUR_SUNRISE + 2;
		hour_sunset  = HOUR_SUNSET  - 2;
		break;
	    case 1:
		hour_sunrise = HOUR_SUNRISE + 1;
		hour_sunset  = HOUR_SUNSET  - 1;
		break;
	    case 2:
		hour_sunrise = HOUR_SUNRISE;
		hour_sunset  = HOUR_SUNSET;
		break;
	    case 3:
		hour_sunrise = HOUR_SUNRISE - 1;
		hour_sunset  = HOUR_SUNSET  + 1;
		break;
	    case 4:
		hour_sunrise = HOUR_SUNRISE - 2;
		hour_sunset  = HOUR_SUNSET  + 2;
		break;
	    case 5:
		hour_sunrise = HOUR_SUNRISE - 1;
		hour_sunset  = HOUR_SUNSET  + 1;
		break;
	    case 6:
		hour_sunrise = HOUR_SUNRISE;
		hour_sunset  = HOUR_SUNSET;
		break;
	    case 7:
		hour_sunrise = HOUR_SUNRISE + 1;
		hour_sunset  = HOUR_SUNSET  - 1;
		break;
	}
    }

    /*
     * See how many skills/spells we have, and
     * assign gsn's for skills which have them.
     */
    {
	top_skill = 0;
	while ( !IS_NULLSTR( skill_table[top_skill].name ) )
	{
	    if ( skill_table[top_skill].pgsn != NULL )
		*skill_table[top_skill].pgsn = top_skill;

	    if ( IS_NULLSTR( skill_table[top_skill].noun_damage ) )
	        skill_table[top_skill].noun_damage = &str_empty[0];
            else
                skill_table[top_skill].noun_damage = str_dup( skill_table[top_skill].noun_damage );

	    if ( IS_NULLSTR( skill_table[top_skill].msg_off ) )
	        skill_table[top_skill].msg_off = &str_empty[0];
            else
                skill_table[top_skill].msg_off = str_dup( skill_table[top_skill].msg_off );

	    if ( IS_NULLSTR( skill_table[top_skill].msg_obj ) )
	        skill_table[top_skill].msg_obj = &str_empty[0];
            else
                skill_table[top_skill].msg_obj = str_dup( skill_table[top_skill].msg_obj );

	    if ( IS_NULLSTR( skill_table[top_skill].msg_room ) )
	        skill_table[top_skill].msg_room = &str_empty[0];
            else
                skill_table[top_skill].msg_room = str_dup( skill_table[top_skill].msg_room );

	    top_skill++;
	}
    }
 /* reboot counter */
    reboot_counter = 1440;  /* 12 hours */
    /*
     *  Assign race values
     */
    {
	int	iClass;

	for ( top_race = 0; race_table[top_race].name; top_race++ )
	{
	    if ( race_table[top_race].race_value != NULL )
		*race_table[top_race].race_value = top_race;
            if ( race_table[top_race].pc_race )
                for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
                    race_table[top_race].class_mult[iClass] = 100;
	}
	load_races( );
    }

    /*
     * Assign class values
     */
    {
	int iClass;

	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	    if ( class_table[iClass].class_value != NULL )
		*class_table[iClass].class_value = iClass;
    }

    /*
     * Load system configuration info
     */
    load_sysconfig( );

    /*
     * Load clan data and commands
     */
    load_clans( );

    load_commands( );

    /*
     * Read in all the area files.
     */
    {
	FILE *fpList;
	char *file;

	file = ( port == GAME_PORT ) ? AREA_LIST : AREA_LIST_BP;
	if ( ( fpList = fopen( file, "r" ) ) == NULL )
	{
	    perror( file );
	    abort_mud( "Area list not found." );
	}

	for ( ; ; )
	{
	    strcpy( strArea, fread_word( fpList ) );
	    if ( strArea[0] == '$' )
		break;

	    if ( strArea[0] == '-' )
	    {
		fpArea = stdin;
	    }
	    else
	    {
		if ( ( fpArea = fopen( strArea, "r" ) ) == NULL )
		{
		    perror( strArea );
		    abort_mud( "Can't open area file." );
		}
	    }

	    area_version = 0;

	    for ( ; ; )
	    {
		char *word;

		if ( fread_letter( fpArea ) != '#' )
		{
		    bug( "Boot_db: # not found.", 0 );
		    abort_mud( "'#' not found" );
		}

		word = fread_word( fpArea );

		     if ( word[0] == '$'               )                 break;
		else if ( !str_cmp( word, "AREA"     ) ) load_area    (fpArea);
		else if ( !str_cmp( word, "AREADATA" ) ) load_areadata(fpArea);
		else if ( !str_cmp( word, "HELPS"    ) ) load_helps   (fpArea);
		else if ( !str_cmp( word, "MOBILES"  ) ) load_mobiles (fpArea);
	  	else if ( !str_cmp( word, "OBJECTS"  ) ) load_objects (fpArea);
		else if ( !str_cmp( word, "RESETS"   ) ) load_resets  (fpArea);
		else if ( !str_cmp( word, "ROOMS"    ) ) load_rooms   (fpArea);
		else if ( !str_cmp( word, "SECTOR"   ) ) load_sectdesc(fpArea);
		else if ( !str_cmp( word, "SHOPS"    ) ) load_shops   (fpArea);
		else if ( !str_cmp( word, "SPECIALS" ) ) load_specials(fpArea);
		else if ( !str_cmp( word, "TRAVELS"  ) ) load_travels (fpArea);
		else if ( !str_cmp( word, "VROOMS"   ) ) load_vrooms  (fpArea);
		else
		{
		    bugf( "Boot_db: bad section name %s in %s.", word, strArea );
		    abort_mud( "Bad section name" );
		}
	    }

	    if ( fpArea != stdin )
		fclose( fpArea );
	    fpArea = NULL;
	}
	fclose( fpList );
    }

    /*
     * Fix up exits.
     * Declare db booting over.
     * Reset all areas once.
     */
    fBootDb	= FALSE;
    fix_exits( );
    area_renumber( );
    validate_resets( );
    fix_all_overlands( );
    load_travel_state( );
    area_update( );

    /*
     * Load up the songs, notes and ban files.
     */
    load_bans( );
    load_badnames( );
    load_boards( ) ;
    load_dreams( );
    load_most_players( );
    load_projects( );
    load_socials( );
    load_skills( );
    load_groups( );
    load_sql_data( );
    load_userlist( );
    load_greet_data( );		/* Must come after load_userlist() */
    load_vehicles( );
    load_whoborder( );

    /*
     * Check all rooms for boot progs
     */
    {
	AREA_DATA *		pArea;
	ROOM_INDEX_DATA *	pRoom;

	for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
	{
	    for ( pRoom = pArea->room_list; pRoom != NULL; pRoom = pRoom->next_in_area )
	    {
		rprog_boot_trigger( pRoom );
	    }
	}
    }


    last_nAllocPerm = nAllocPerm;
    last_sAllocPerm = sAllocPerm;

    return;
}


/*
 * Snarf an 'area' header line.
 */
void
load_area( FILE *fp )
{
    AREA_DATA *pArea;

    pArea		= alloc_perm( sizeof(*pArea) );
    pArea->reset_first	= NULL;
    pArea->reset_last	= NULL;
    pArea->file_name	= fread_string( fp );
    pArea->name		= fread_string( fp );
    pArea->credits	= fread_string( fp );
    pArea->builders	= &str_empty[0];
    pArea->resetmsg	= &str_empty[0];
    pArea->area_flags	= AREA_NONE;
    pArea->min_vnum	= fread_number( fp );
    pArea->max_vnum	= fread_number( fp );
    pArea->low_level	= 1;
    pArea->high_level	= LEVEL_HERO;
    pArea->security	= 0;
    pArea->recall	= ROOM_VNUM_TEMPLE;
    pArea->age		= 15;
    pArea->nplayer	= 0;
    pArea->color	= AT_AREA;
    pArea->exitsize	= SIZE_MEDIUM;
    pArea->world	= WORLD_NONE;
    pArea->empty	= FALSE;

    area_version	= 0;

    area_sort( pArea );

    top_area++;
    return;
}



/*
 * Snarf a help section.
 */
void
load_helps( FILE *fp )
{
    HELP_DATA *	pHelp;
    char *	word;

    for ( ; ; )
    {
	pHelp		= alloc_perm( sizeof( *pHelp ) );
	pHelp->area	= area_last;
	word		= fread_word( fp );
	if ( is_number( word ) )
	    pHelp->level = atoi( word );
	else
	    pHelp->level = level_lookup( word );	

	word		= fread_word( fp );
	if ( is_number( word ) )
	    pHelp->ed_level = atoi( word );
	else
	    pHelp->ed_level = level_lookup( word );	

	pHelp->keyword	= fread_string( fp );
	if ( pHelp->keyword[0] == '$' )
	    break;
	pHelp->text	= fread_string( fp );

	if ( !str_cmp( pHelp->keyword, "greeting" ) )
	    strcpy( help_greeting, pHelp->text );

	if ( help_first == NULL )
	    help_first = pHelp;
	if ( help_last  != NULL )
	    help_last->next = pHelp;

	help_last	= pHelp;
	pHelp->next	= NULL;
	top_help++;
    }

    return;
}


void
validate_resets( void )
{
    AREA_DATA *		pArea;
    EXIT_DATA *		pExit;
    RESET_DATA *	pReset;
    ROOM_INDEX_DATA *	pRoom;
    ROOM_INDEX_DATA *	rRoom;
    OBJ_INDEX_DATA *	pObj;
    int			count;
    char		letter;
    int			rvnum;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	count = 0;
	for ( rvnum = pArea->min_vnum; rvnum <= pArea->max_vnum; rvnum++ )
	if ( ( rRoom = get_room_index( rvnum ) ) != NULL )
	for ( pReset = rRoom->reset_first; pReset; pReset = pReset->next )
	{
	    ++count;
	    letter = pReset->command;
	    switch( letter )
	    {
	    default:
		bad_reset( pArea, pReset, count, "bad command" );

	    case 'M':
		if ( !get_mob_index( pReset->arg1 ) || !get_room_index( pReset->arg3 ) )
		   bad_reset( pArea, pReset, count, "bad vnum" );
		break;

	    case 'O':
		if ( ( pObj = get_obj_index( pReset->arg1 ) ) == NULL )
		    bad_reset( pArea, pReset, count, "bad obj vnum" );
		pObj->reset_num++;
		if ( get_room_index( pReset->arg3 ) == NULL )
		    bad_reset( pArea, pReset, count, "bad room vnum" );
		break;

	    case 'P':
		if ( ( pObj = get_obj_index( pReset->arg1 ) ) == NULL )
		    bad_reset( pArea, pReset, count, "Bad first obj vnum" );
		pObj->reset_num++;
		if ( ( get_obj_index( pReset->arg3 ) ) == NULL )
		    bad_reset( pArea, pReset, count, "Bad second obj" );
		break;

	    case 'E':
	    case 'G':
		if ( ( pObj = get_obj_index( pReset->arg1 ) ) == NULL )
		    bad_reset( pArea, pReset, count, "Bad vnum" );
		pObj->reset_num++;
		break;

	    case 'D':
		if ( ( pRoom = get_room_index( pReset->arg1 ) ) == NULL )
		    bad_reset( pArea, pReset, count, "Bad room vnum" );
		if (	pReset->arg2 < 0
		     || pReset->arg2 >= MAX_DIR
		     || ( pExit = pRoom->exit[pReset->arg2] ) == NULL
		     || !IS_SET( pExit->exit_info, EX_ISDOOR ) )
		{
		    bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
		    bad_reset( pArea, pReset, count, "Bad door" );
		    break;
		}

		switch ( pReset->arg3 )	/* OLC 1.1b */
		{
		    default:
			bug( "Load_resets: 'D': bad 'locks': %d." , pReset->arg3);
			bad_reset( pArea, pReset, count, "bad 'locks'" );
		    case 0:
			break;
		    case 1:
			SET_BIT( pExit->rs_flags, EX_CLOSED );
			break;
		    case 2:
			SET_BIT( pExit->rs_flags, EX_CLOSED | EX_LOCKED );
			break;
		}
		break;

	    case 'R':
		if ( ( pRoom = get_room_index( pReset->arg1 ) ) == NULL )
		{
		    bug( "Load_resets: 'R': bad room vnum %d.", pReset->arg1 );
		    bad_reset( pArea, pReset, count, "Bad room vnum" );
		}
		if ( pReset->arg2 < 0 || pReset->arg2 > MAX_DIR )
		{
		    bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
		    bad_reset( pArea, pReset, count, "Bad exit" );
		}
		break;
	    }
	}
    }

}


/*
 * Log a bad reset and exit
 */
void bad_reset( AREA_DATA *pArea, RESET_DATA *pReset, int count, char *text )
{
    bugf( "Area %s: reset #%d (type %c) bad reset",
	  pArea->file_name, count, pReset->command );
    abort_mud( text );
}


/*
 * Snarf a room section.
 */
void
load_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *	pRoomIndex;
    ROOM_INDEX_DATA *	pRoomLast;
    EXTRA_DESCR_DATA *	ed_last;
    char		tLetter;

    if ( area_last == NULL )
    {
	bug( "Load_rooms: no #AREA seen yet.", 0 );
	abort_mud( "Load_rooms: no #AREA" );
    }

    pRoomLast = NULL;

    for ( ; ; )
    {
	int vnum;
	char letter;
	int door;
	int iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_rooms: # not found.", 0 );
	    abort_mud( "Load_rooms: # not found" );
	}

	ed_last = NULL;
	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;
	if ( vnum > top_vnum_room )
	    top_vnum_room = vnum;

	fBootDb = FALSE;
	if ( get_room_index( vnum ) != NULL )
	{
	    bug( "Load_rooms: vnum %d duplicated.", vnum );
	    abort_mud( "Load_rooms: duplicate vnum" );
	}
	fBootDb = TRUE;

	pRoomIndex			= alloc_perm( sizeof(*pRoomIndex) );
	pRoomIndex->owner		= str_dup("");
	pRoomIndex->people		= NULL;
	pRoomIndex->contents		= NULL;
	pRoomIndex->extra_descr		= NULL;
	pRoomIndex->reset_first		= NULL;
	pRoomIndex->reset_last		= NULL;
	pRoomIndex->area		= area_last;
	pRoomIndex->vnum		= vnum;
	pRoomIndex->name		= fread_string( fp );
	pRoomIndex->description		= fread_string( fp );
	pRoomIndex->morning_desc	= fread_string( fp );
	pRoomIndex->evening_desc	= fread_string( fp );
	pRoomIndex->night_desc		= fread_string( fp );
	pRoomIndex->sound		= fread_string( fp );
	pRoomIndex->smell		= fread_string( fp );
	/* Area number */		  fread_number( fp );
	pRoomIndex->room_flags		= fread_flag( fp );
	pRoomIndex->affect_flags	= fread_flag( fp );
	pRoomIndex->sector_type		= fread_number( fp );
	pRoomIndex->light		= 0;
	for ( door = 0; door < MAX_DIR; door++ )
	    pRoomIndex->exit[door] = NULL;

	/* defaults */
	pRoomIndex->heal_rate		= 100;
	pRoomIndex->mana_rate		= 100;
	pRoomIndex->vehicle_type	= VEHICLE_NONE;
	pRoomIndex->short_descr		= &str_empty[0];

	for ( ; ; )
	{
	    letter = fread_letter( fp );

	    if ( letter == 'S' )
		break;

	    if ( letter == 'H') /* healing room */
		pRoomIndex->heal_rate = fread_number(fp);

	    else if ( letter == 'M') /* mana room */
		pRoomIndex->mana_rate = fread_number(fp);

	   else if ( letter == 'C') /* clan */
	   {
		if (pRoomIndex->clan)
	  	{
		    bug("Load_rooms: duplicate clan fields.",0);
		    exit(1);
		}
		pRoomIndex->clan = clan_lookup(fread_string(fp));
	    }

	    else if ( letter == 'D' )
	    {
		EXIT_DATA *	pexit;
		char *		word;
		int		locks;

		word = fread_word( fp );
		if ( is_number( word ) )
		{
		    door = atoi( word );
		    if ( door < 0 || door >= MAX_DIR )
		    {
		        bug( "Load_rooms: vnum %d has bad door number.", vnum );
		        abort_mud( "Load_rooms: bad door #" );
                    }
                    door = dir_remap[door];
                }
                else
                {
                    door = door_lookup( word );
                }
		if ( door < 0 || door >= MAX_DIR )
		{
		    bug( "Load_rooms: vnum %d has bad door number.", vnum );
		    abort_mud( "Load_rooms: bad door #" );
		}

		pexit			= alloc_perm( sizeof(*pexit) );
		pexit->size		= size_lookup( fread_word( fp ) );
		pexit->sound_closed	= fread_string( fp );
		pexit->sound_open	= fread_string( fp );
		pexit->description	= fread_string( fp );
		pexit->keyword		= fread_string( fp );
		pexit->exit_info	= 0;
		pexit->to_room		= NULL;
		locks			= fread_number( fp );
		pexit->rs_flags		= locks;
		pexit->exit_info	= locks;
		pexit->key		= fread_number( fp );
		pexit->vnum		= fread_number( fp );

		switch ( locks )
		{
		case 1: pexit->exit_info = EX_ISDOOR;                break;
		case 2: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF; break;
		case 3: pexit->exit_info = EX_ISDOOR | EX_NOPASS;    break;
		case 4: pexit->exit_info = EX_ISDOOR|EX_NOPASS|EX_PICKPROOF;
			break;
		}

		tLetter = fread_letter( fp );
		ungetc( tLetter, fp );
		if ( tLetter == '>' )
		    pexit->progtypes = load_oreprogs( fp, &pexit->eprogs, PTYPE_EXIT );
		pRoomIndex->exit[door]	= pexit;
		pRoomIndex->old_exit[door] = pexit;
		top_exit++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof(*ed) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= NULL;
		if ( ed_last == NULL )
		    pRoomIndex->extra_descr	= ed;
		else
		    ed_last->next = ed;
		ed_last = ed;
		top_ed++;
	    }

	    else if (letter == 'O')
	    {
		if (pRoomIndex->owner[0] != '\0')
		{
		    bug("Load_rooms: duplicate owner.",0);
		    exit(1);
		}

		pRoomIndex->owner = fread_string(fp);
	    }

	    else if ( letter == 'V' )
	    {
		int   type;

		type = flag_value( vehicle_types, fread_word( fp ) );
		if ( type == NO_FLAG )
		{
		    bug( "Load_rooms: bad vehicle type.", 0 );
		    type = VEHICLE_NONE;
		}
		pRoomIndex->vehicle_type = type;
		pRoomIndex->short_descr = fread_string( fp );
	    }

	    else
	    {
		bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
		exit( 1 );
	    }
	}

	letter = fread_letter( fp );
	ungetc( letter, fp );
	if ( letter == '>' )
	    pRoomIndex->progtypes = load_oreprogs( fp, &pRoomIndex->rprogs, PTYPE_ROOM );

	iHash			= vnum % MAX_KEY_HASH;
	pRoomIndex->next	= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoomIndex;
	top_room++;

	if ( pRoomLast == NULL )
	{
	    pRoomIndex->area->room_list = pRoomIndex;
	    pRoomLast = pRoomIndex;
	}
	else if ( pRoomIndex->vnum > pRoomLast->vnum )
	{
	    pRoomLast->next_in_area = pRoomIndex;
	    pRoomLast = pRoomIndex;
	}
    }

    return;
}



/*
 * Snarf a shop section.
 */
void
load_shops( FILE *fp )
{
    SHOP_DATA *pShop;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int iTrade;

	pShop			= alloc_perm( sizeof(*pShop) );
	pShop->keeper		= fread_number( fp );
	if ( pShop->keeper == 0 )
	    break;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    pShop->buy_type[iTrade]	= fread_number( fp );
	pShop->profit_buy	= fread_number( fp );
	pShop->profit_sell	= fread_number( fp );
	pShop->open_hour	= fread_number( fp );
	pShop->close_hour	= fread_number( fp );
				  fread_to_eol( fp );
	pMobIndex		= get_mob_index( pShop->keeper );
	pMobIndex->pShop	= pShop;

	if ( shop_first == NULL )
	    shop_first = pShop;
	if ( shop_last  != NULL )
	    shop_last->next = pShop;

	shop_last	= pShop;
	pShop->next	= NULL;
	top_shop++;
    }

    return;
}


/*
 * Snarf spec proc declarations.
 */
void
load_specials( FILE *fp )
{
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	char letter;

	switch ( letter = fread_letter( fp ) )
	{
	default:
	    bug( "Load_specials: letter '%c' not *MS.", letter );
	    exit( 1 );

	case 'S':
	    return;

	case '*':
	    break;

	case 'M':
	    pMobIndex		= get_mob_index	( fread_number ( fp ) );
	    pMobIndex->spec_fun	= spec_lookup	( fread_word   ( fp ) );
	    if ( pMobIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
	    break;
	}

	fread_to_eol( fp );
    }
}


/*
 * Load the who border info, if it exists.
 */
void
load_whoborder( void )
{
    FILE *	fp;
    char *	p;
    char	buf[MAX_INPUT_LENGTH];

    if ( ( fp = fopen( SYSTEM_DIR WHOBORDER_FILE, "r" ) ) != NULL )
    {
        if ( fgets( buf,  MAX_INPUT_LENGTH, fp ) )
	    strcpy( whoborder_imm, buf );
        if ( fgets( whoborder_mort, MAX_INPUT_LENGTH, fp ) )
	    strcpy( whoborder_mort, buf );
        fclose( fp );
        if ( ( p = strchr( whoborder_imm, '\n' ) ) != NULL )
            *p = '\0';
        if ( ( p = strchr( whoborder_mort, '\n' ) ) != NULL )
            *p = '\0';
    }

    return;
}


/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void
fix_exits( void )
{
    ROOM_INDEX_DATA *	pRoomIndex;
    EXIT_DATA *		pexit;
    int			iHash;
    int			door;
    int			iSize;
    AREA_DATA *		pArea;
    TRAVEL_DATA *	pTravel;
    EXIT_DATA *		pexit_rev;
    ROOM_INDEX_DATA *	to_room;
#if defined( DEBUG )
    extern const sh_int rev_dir [];
    char buf[MAX_STRING_LENGTH];
#endif

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    bool fexit;

	    fexit = FALSE;
	    for ( door = 0; door < MAX_DIR; door++ )
	    {
		if ( ( pexit = pRoomIndex->exit[door] ) != NULL )
		{
		    if ( pexit->vnum <= 0 
		    || get_room_index( pexit->vnum ) == NULL)
			pexit->to_room = NULL;
		    else
		    {
		   	fexit = TRUE; 
			pexit->to_room = get_room_index( pexit->vnum );
		    }
		    pexit->from_room = pRoomIndex;
		}
	    }
	    if ( !fexit )
		SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );
	}
    }

/* Define this to log room exits that link back to a different room */
#if defined( DEBUG )
    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    for ( door = 0; door < MAX_DIR; door++ )
	    {
		if ( ( pexit     = pRoomIndex->exit[door]       ) != NULL
		&&   ( to_room   = pexit->to_room            ) != NULL
		&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
		&&   pexit_rev->to_room != pRoomIndex 
		&&   (pRoomIndex->vnum < 1200 || pRoomIndex->vnum > 1299))
		{
		    sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
			pRoomIndex->vnum, door,
			to_room->vnum,    rev_dir[door],
			(pexit_rev->to_room == NULL)
			    ? 0 : pexit_rev->to_room->vnum );
		    bug( buf, 0 );
		}
	    }
	}
    }
#endif

    /* Make sure exit sizes are the same from both sides. */
    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    for ( door = 0; door < MAX_DIR; door++ )
	    {
		if ( ( pexit = pRoomIndex->exit[door] ) != NULL
		&&   ( to_room = pexit->to_room ) != NULL
		&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
		&&   pexit_rev->to_room == pRoomIndex )
		{
		    iSize = UMIN( pexit->size, pexit_rev->size );
		    pexit->size = iSize;
		    pexit_rev->size = iSize;
		}
	    }
	}
    }

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
	if ( pArea->travel_vnum == 0 || ( pTravel = pArea->travel_first ) == NULL )
	    continue;

	if ( ( pArea->travel_room = get_room_index( pArea->travel_vnum ) ) == NULL )
	{
	    bug( "Invalid traveling room vnum: %d", pArea->travel_vnum );
	    continue;
	}

	for ( ; pTravel != NULL; pTravel = pTravel->next )
	{
	    if ( ( pTravel->room = get_room_index( pTravel->room_vnum ) ) == NULL )
	    {
		bug( "Init travel: no room vnum %d.", pTravel->room_vnum );
	    }
	}
	pArea->travel_curr = pArea->travel_first;
    }

    return;
}


void
fix_all_overlands( void )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
	if ( pArea->overland != NULL )
	    fix_sectors( pArea );
}


void
fix_sectors( AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *	pRoom;
    int			index;
    int			lvnum;
    int			uvnum;

    if ( pArea == NULL || pArea->overland == NULL )
	return;

    lvnum = pArea->min_vnum;
    uvnum = pArea->min_vnum + pArea->maxx * pArea->maxy;

    for ( index = 0; index < MAX_KEY_HASH; index++ )
    {
	for ( pRoom = room_index_hash[index]; pRoom != NULL; pRoom = pRoom->next )
	{
	    if ( pRoom->vnum >= lvnum && pRoom->vnum < uvnum )
	    {
		pArea->overland->map[pRoom->vnum - pArea->min_vnum].sector_type = pRoom->sector_type;
	    }
	}
    }
}


/*
 * Repopulate areas periodically.
 */
void
area_update( void )
{
    AREA_DATA *		pArea;
    DESCRIPTOR_DATA *	d;
    CHAR_DATA *		rch;
    char		buf[MAX_STRING_LENGTH];

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {

	if ( ++pArea->age < 3 )
	    continue;

	if ( pArea->nplayer > 0
	&&   pArea->age == 15 - 1
	&&   !IS_SET( pArea->area_flags, AREA_QUIET ) )
	{
	    for ( d = descriptor_list; d; d = d->next )
	    {
		if ( d->connected == CON_PLAYING
		&& ( rch = d->character ) != NULL
		&& rch->in_room != NULL
		&& rch->in_room->area == pArea
		&& IS_AWAKE( rch )
		&& d->pString == NULL
		&& !IS_SET( rch->in_room->room_flags, ROOM_NO_RESETMSG ) )
		{
		    if ( pArea->resetmsg && *pArea->resetmsg != '\0' )
		    {
			send_to_char( pArea->resetmsg, rch );
		    }
		    else
		    {
			send_to_char( "You hear faint rustling sounds.", rch );
		    }
		    send_to_char( "\n\r", rch );
		}
	    }
	}

	/*
	 * Check age and reset.
	 * Note: Mud School resets every 3 minutes (not 15).
	 */
	if ( (!pArea->empty && (pArea->nplayer == 0 || pArea->age >= 15))
	||    pArea->age >= 31)
	{
	    reset_area( pArea );
	    sprintf(buf,"%s has just been reset.",pArea->name);
	    wiznet(buf,NULL,NULL,WIZ_RESETS,0,0);

	    pArea->age = number_range( 0, 3 );
	    if ( IS_SET( pArea->area_flags, AREA_MUDSCHOOL ) )
		pArea->age = 15 - 3;
	    else if (pArea->nplayer == 0) 
		pArea->empty = TRUE;
	}
    }

    return;
}



/*
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoom;
/*  int  vnum; */

/*
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pRoom = get_room_index(vnum) ) )
	    reset_room(pRoom);
    }
*/

    for ( pRoom = pArea->room_list; pRoom != NULL; pRoom = pRoom->next_in_area )
        reset_room( pRoom );

    return;
}


/*
 * Reset one room.
 */
void reset_room( ROOM_INDEX_DATA *pRoom )
{
    RESET_DATA *pReset;
    CHAR_DATA *	pMob;
    OBJ_DATA *	pObj;
    CHAR_DATA *	LastMob = NULL;
    OBJ_DATA *	LastObj = NULL;
    CLIST_DATA *mob_list = NULL;
    CLIST_DATA *pList;
    int iExit;
    int level = 0;
    bool last;

    if ( !pRoom )
        return;

    pMob        = NULL;
    last        = FALSE;

    for ( iExit = 0;  iExit < MAX_DIR;  iExit++ )
    {
        EXIT_DATA *pExit;
        if ( ( pExit = pRoom->exit[iExit] ) )
        {
            if ( IS_SET( pExit->rs_flags, EX_NORESET ) )
                continue;
            pExit->exit_info = pExit->rs_flags;
            if ( ( pExit->to_room != NULL )
              && ( ( pExit = pExit->to_room->exit[rev_dir[iExit]] ) ) )
            {
                /* nail the other side */
                pExit->exit_info = pExit->rs_flags;
            }
        }
    }

    for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
    {
        MOB_INDEX_DATA  *pMobIndex;
        OBJ_INDEX_DATA  *pObjIndex;
        OBJ_INDEX_DATA  *pObjToIndex;
        ROOM_INDEX_DATA *pRoomIndex;
        CHAR_DATA *mob;
	int count;

        switch ( pReset->command )
        {
        default:
                buildbug( "Reset_room: bad command %c in room %d.", pReset->command, pRoom->vnum );
                break;

        case 'M':
            if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
            {
                buildbug( "Reset_room: 'M': bad vnum %d in room %d.", pReset->arg1, pRoom->vnum );
                continue;
            }

	    if ( !(pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
                buildbug( "Reset_room: 'M': bad room vnum %d.", pReset->arg3 );
                continue;
            }

/* max world */
            if ( pMobIndex->count >= pReset->arg2 && pReset->arg2 > 0 )
            {
                last = FALSE;
                break;
            }

/* max room */
	    count = 0;
	    for (mob = pRoomIndex->people; mob != NULL; mob = mob->next_in_room)
		if (mob->pIndexData == pMobIndex)
		{
		    count++;
		    if (count >= pReset->arg4)
		    {
		    	last = FALSE;
		    	break;
		    }
		}

	    if (count >= pReset->arg4)
		break;

            pMob = create_mobile( pMobIndex );

            /*
             * Some more hard coding.
             */
            if ( room_is_dark( pRoom ) )
		xSET_BIT(pMob->affected_by, AFF_INFRARED);
            /*
             * Pet shop mobiles get ACT_PET set.
             */
            {
                ROOM_INDEX_DATA *pRoomIndexPrev;

                pRoomIndexPrev = get_room_index( pRoom->vnum - 1 );
		if ( pRoomIndexPrev
		&&   IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
		{
		    if ( IS_SET( pMobIndex->act, ACT_MOUNT ) )
			SET_BIT( pMob->act, ACT_MOUNT );
		    else
			SET_BIT( pMob->act, ACT_PET );
		}
            }

            char_to_room( pMob, pRoom );

            LastMob = pMob;
            level  = URANGE( 0, pMob->level - 2, LEVEL_HERO - 1 ); /* -1 ROM */
            last = TRUE;
            pList = new_clist_data( );
            pList->next = mob_list;
            pList->lch = pMob;
            mob_list = pList;
            break;

        case 'O':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                buildbug( "Reset_room: 'O': bad vnum %d in room %d.", pReset->arg1, pRoom->vnum );
                continue;
            }

            if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
            {
                buildbug( "Reset_room: 'O': bad vnum %d in room %d.", pReset->arg3, pRoom->vnum );
                continue;
            }

            if ( ( pRoom->area->nplayer > 0 && !IS_SET( pRoom->area->area_flags, AREA_MUDSCHOOL ) )
              || count_obj_list( pObjIndex, pRoomIndex->contents ) > 0 )
                break;

            pObj = create_object( pObjIndex,              /* UMIN - ROM OLC */
				  UMIN(number_fuzzy( level ), LEVEL_HERO -1) );
            pObj->cost = 0;
            obj_to_room( pObj, pRoom );
            break;

        case 'P':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                buildbug( "Reset_room: 'P': bad vnum %d in room %d.", pReset->arg1, pRoom->vnum );
                continue;
            }

            if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
            {
                buildbug( "Reset_room: 'P': bad vnum %d in room %d.", pReset->arg3, pRoom->vnum );
                continue;
            }

            if ( ( pRoom->area->nplayer > 0 && !IS_SET( pRoom->area->area_flags, AREA_MUDSCHOOL ) )
              || ( ( LastObj = get_obj_room( pObjToIndex, pRoom ) ) == NULL )
              || count_obj_list( pObjIndex, LastObj->contains ) >= pReset->arg2
              || pObjIndex->count >= 99 )
                break;
				                /* lastObj->level  -  ROM */
            pObj = create_object( pObjIndex, number_fuzzy( LastObj->level ) );
            obj_to_obj( pObj, LastObj );
            break;

        case 'G':
        case 'E':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                buildbug( "Reset_room: 'E' or 'G': bad vnum %d in room %d.", pReset->arg1, pRoom->vnum );
                continue;
            }

            if ( !last )
                break;

            if ( !LastMob )
            {
                buildbug( "Reset_room: 'E' or 'G': null mob for vnum %d in room %d.",
                    pReset->arg1, pRoom->vnum );
                last = FALSE;
                break;
            }

            if ( LastMob->pIndexData->pShop )   /* Shop-keeper? */
            {
                int olevel;

                switch ( pObjIndex->item_type )
                {
                default:		olevel = 0;                      break;
                case ITEM_PILL:		olevel = number_range(  0, 10 ); break;
                case ITEM_POTION:	olevel = number_range(  0, 10 ); break;
		case ITEM_HERB:		olevel = number_range(  0, 10 ); break;
                case ITEM_SCROLL:       olevel = number_range(  5, 15 ); break;
                case ITEM_WAND:         olevel = number_range( 10, 20 ); break;
                case ITEM_STAFF:        olevel = number_range( 15, 25 ); break;
                case ITEM_ARMOR:        olevel = number_range(  5, 15 ); break;
		case ITEM_WEAPON:       olevel = number_range(  5, 15 ); break;
		case ITEM_TREASURE:     olevel = number_range( 10, 20 ); break;

                  break;
                }

                pObj = create_object( pObjIndex, olevel );
		SET_BIT( pObj->extra_flags, ITEM_INVENTORY );  /* ROM OLC */
        

            }
	    else   /* ROM OLC else version */
	    {
		int limit;
		if (pReset->arg2 > 50 )  /* old format */
		    limit = 6;
		else if ( pReset->arg2 == -1 )  /* no limit */
		    limit = 999;
		else
		    limit = pReset->arg2;

		if ( pObjIndex->count < limit )
		    pObj = create_object( pObjIndex, 
			   UMIN( number_fuzzy( level ), LEVEL_HERO - 1 ) );
		else
		    break;
	    }

            obj_to_char( pObj, LastMob );
            if ( pReset->command == 'E' )
                equip_char( LastMob, pObj, pReset->arg3 );
            last = TRUE;
            break;

        case 'D':
            break;

        case 'R':
            if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
            {
                buildbug( "Reset_room: 'R': bad vnum %d in room %d.", pReset->arg1, pRoom->vnum );
                continue;
            }

            {
                EXIT_DATA *pExit;
                int d0;
                int d1;

                for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
                {
                    d1                   = number_range( d0, pReset->arg2-1 );
                    pExit                = pRoomIndex->exit[d0];
                    pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
                    pRoomIndex->exit[d1] = pExit;
                }
            }
            break;
        }
    }

    while ( mob_list )
    {
	pList = mob_list;
	mob_list = pList->next;
	mprog_repop_trigger( pList->lch );
	free_clist_data( pList );
    }

    return;
}


/*
 * Create an instance of a mobile.
 */
CHAR_DATA *
create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *	mob;
    int		i;
    AFFECT_DATA af;
    CLAN_DATA *	pClan;

    mobile_count++;

    if ( pMobIndex == NULL )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }

    mob = new_char();

    mob->pIndexData	= pMobIndex;

    mob->id		= get_mob_id();
    mob->spec_fun	= pMobIndex->spec_fun;
    mob->prompt		= NULL;
    mob->deleted	= FALSE;
    mob->mpscriptpos	= 0;

    mob->money.gold	= 0;
    mob->money.silver	= 0;
    mob->money.copper	= 0;
    if ( !IS_SET( pMobIndex->act, ACT_MOUNT ) && !IS_SET( pMobIndex->act, ACT_PET ) )
	mob->money.fract = number_range( pMobIndex->wealth/2, 3 * pMobIndex->wealth/2 );
    else
	mob->money.fract = 0;
    normalize( &mob->money );

    /* read from prototype */
    mob->group		= pMobIndex->group;
    mob->act 		= pMobIndex->act;
    mob->act2		= 0;
    mob->comm		= 0;
    mob->deaf		= 0;
    mob->logon		= current_time;
    mob->affected_by	= pMobIndex->affected_by;
    mob->shielded_by	= pMobIndex->shielded_by;
    mob->alignment	= pMobIndex->alignment;
    mob->ethos	= pMobIndex->ethos;
    mob->level		= pMobIndex->level;
    mob->hitroll	= pMobIndex->hitroll;
    mob->damroll	= pMobIndex->damage[DICE_BONUS];
    mob->max_hit	= dice( pMobIndex->hit[DICE_NUMBER],
                                   pMobIndex->hit[DICE_TYPE] )
                              + pMobIndex->hit[DICE_BONUS];
    mob->max_hit	= UMAX( 1, mob->max_hit );
    mob->hit		= mob->max_hit;
    mob->max_mana	= dice(pMobIndex->mana[DICE_NUMBER],
                                   pMobIndex->mana[DICE_TYPE])
                              + pMobIndex->mana[DICE_BONUS];
    mob->mana		= mob->max_mana;
    mob->damage[DICE_NUMBER]= pMobIndex->damage[DICE_NUMBER];
    mob->damage[DICE_TYPE] = pMobIndex->damage[DICE_TYPE];
    mob->dam_type	= pMobIndex->dam_type;
    if (mob->dam_type == 0)
    {
    	switch(number_range(1,3))
	{
	    case (1): mob->dam_type = 3;        break;  /* slash */
	    case (2): mob->dam_type = 7;        break;  /* pound */
	    case (3): mob->dam_type = 11;       break;  /* pierce */
	}
    }

    if ( IS_SET( pMobIndex->area->area_flags, AREA_CLAN ) )
    {
        if ( !IS_NULLSTR( pMobIndex->clan ) )
        {
            if ( ( pClan = get_clan( pMobIndex->clan ) ) == NULL )
                bugf( "Create_mobile: mob #%d -- bad clan \"%s\".", pMobIndex->vnum, pMobIndex->clan );
            else
                mob->clan = pClan;
        }
    }

	for (i = 0; i < 4; i++)
	    mob->armor[i]	= pMobIndex->ac[i]; 
	mob->off_flags		= pMobIndex->off_flags;
	mob->imm_flags		= pMobIndex->imm_flags;
	mob->res_flags		= pMobIndex->res_flags;
	mob->vuln_flags		= pMobIndex->vuln_flags;
	mob->start_pos		= pMobIndex->start_pos;
	mob->default_pos	= pMobIndex->default_pos;
	mob->sex		= pMobIndex->sex;
        if ( mob->sex == SEX_EITHER ) /* random sex */
            mob->sex = number_range( SEX_MALE, SEX_FEMALE );
	mob->race		= pMobIndex->race;
	mob->form		= pMobIndex->form;
	mob->parts		= pMobIndex->parts;
	mob->size		= pMobIndex->size;
	mob->material		= str_dup(pMobIndex->material);

	set_mob_strings( mob );

	/* computed on the spot */

	for ( i = 0; i < MAX_STATS; i ++ )
	    mob->perm_stat[i] = UMIN( 25, 11 + mob->level/4 );

	if ( IS_SET( pMobIndex->class, MCLASS_FIGHTER ) )
	{
	    mob->perm_stat[STAT_STR] += 3;
	    mob->perm_stat[STAT_INT] -= 1;
	    mob->perm_stat[STAT_CON] += 2;
	}

	if ( IS_SET( pMobIndex->class, MCLASS_ROGUE ) )
	{
	    mob->perm_stat[STAT_DEX] += 3;
	    mob->perm_stat[STAT_INT] += 1;
	    mob->perm_stat[STAT_WIS] -= 1;
	}

	if ( IS_SET( pMobIndex->class, MCLASS_CLERIC ) )
	{
	    mob->perm_stat[STAT_WIS] += 3;
	    mob->perm_stat[STAT_DEX] -= 1;
	    mob->perm_stat[STAT_STR] += 1;
	}

	if ( IS_SET( pMobIndex->class, MCLASS_MAGE ) )
	{
	    mob->perm_stat[STAT_INT] += 3;
	    mob->perm_stat[STAT_STR] -= 1;
	    mob->perm_stat[STAT_DEX] += 1;
	}

	if ( xIS_SET( mob->off_flags, OFF_FAST ) )
	    mob->perm_stat[STAT_DEX] += 2;

	mob->perm_stat[STAT_STR] += mob->size - SIZE_MEDIUM;
	mob->perm_stat[STAT_CON] += (mob->size - SIZE_MEDIUM) / 2;

	/* let's get some spell action */
	if ( IS_AFFECTED( mob, AFF_DANCING ) )
	{
	    af.where	 = TO_AFFECTS;
	    af.type      = gsn_dancing_lights;
	    af.level     = mob->level;
	    af.duration  = -1;
	    af.location  = APPLY_HITROLL;
	    af.modifier  = -5;
	    af.bitvector = AFF_DANCING;
	    affect_to_char( mob, &af );
	}

	if ( IS_AFFECTED( mob, AFF_HASTE ) )
	{
	    af.where	 = TO_AFFECTS;
	    af.type      = skill_lookup("haste");
    	    af.level     = mob->level;
      	    af.duration  = -1;
    	    af.location  = APPLY_DEX;
    	    af.modifier  = 1 + (mob->level >= 18) + (mob->level >= 25) + 
			   (mob->level >= 32);
    	    af.bitvector = AFF_HASTE;
    	    affect_to_char( mob, &af );
	}

	if ( IS_AFFECTED( mob, AFF_PEACE ) )
	{
	    af.where	 = TO_AFFECTS;
	    af.type      = gsn_aura_peace;
	    af.level     = mob->level;
	    af.duration  = -1;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_PEACE;
	    affect_to_char( mob, &af );
	}

	if ( IS_SHIELDED( mob, SHLD_PROTECT_EVIL ) )
	{
	    af.where	 = TO_SHIELDS;
	    af.type	 = skill_lookup( "protection evil" );
	    af.level	 = mob->level;
	    af.duration	 = -1;
	    af.location	 = APPLY_SAVES;
	    af.modifier	 = -1;
	    af.bitvector = SHLD_PROTECT_EVIL;
	    affect_to_char( mob, &af );
	}

        if ( IS_SHIELDED( mob, SHLD_PROTECT_GOOD ) )
        {
	    af.where	 = TO_SHIELDS;
            af.type      = skill_lookup( "protection good" );
            af.level     = mob->level;
            af.duration  = -1;
            af.location  = APPLY_SAVES;
            af.modifier  = -1;
            af.bitvector = SHLD_PROTECT_GOOD;
            affect_to_char( mob, &af );
        }

	if ( IS_SHIELDED( mob, SHLD_SANCTUARY ) )
	{
	    af.where	 = TO_SHIELDS;
	    af.type      = skill_lookup( "sanctuary" );
	    af.level     = mob->level;
	    af.duration  = -1;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = SHLD_SANCTUARY;
	    affect_to_char( mob, &af );
	}
if ( IS_SHIELDED( mob, SHLD_BLACK_MANTLE ) )
	{
	    af.where	 = TO_SHIELDS;
	    af.type      = skill_lookup( "black mantle" );
	    af.level     = mob->level;
	    af.duration  = -1;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = SHLD_BLACK_MANTLE;
	    affect_to_char( mob, &af );
	}

    mob->position = mob->start_pos;


    /* link the mob to the world list */
    mob->next		= char_list;
    char_list		= mob;
    pMobIndex->count++;
    return mob;
}

/* duplicate a mobile exactly -- except inventory */
void
clone_mobile( CHAR_DATA *parent, CHAR_DATA *clone )
{
    int i;
    AFFECT_DATA *paf;

    if ( parent == NULL || clone == NULL || !IS_NPC( parent ) )
	return;

    /* start fixing values */ 
    clone->name 	= str_dup( parent->name );
    clone->version	= parent->version;
    clone->short_descr	= str_dup( parent->short_descr );
    clone->long_descr	= str_dup( parent->long_descr );
    clone->description	= str_dup( parent->description );
    clone->group	= parent->group;
    clone->sex		= parent->sex;
    clone->class	= parent->class;
    clone->race		= parent->race;
    clone->clan		= parent->clan;
    clone->level	= parent->level;
    clone->trust	= 0;
    clone->timer	= parent->timer;
    clone->wait		= parent->wait;
    clone->hit		= parent->hit;
    clone->max_hit	= parent->max_hit;
    clone->mana		= parent->mana;
    clone->max_mana	= parent->max_mana;
    clone->move		= parent->move;
    clone->max_move	= parent->max_move;
    clone->money	= parent->money;
    clone->exp		= parent->exp;
    clone->act		= parent->act;
    clone->comm		= parent->comm;
    clone->imm_flags	= parent->imm_flags;
    clone->res_flags	= parent->res_flags;
    clone->vuln_flags	= parent->vuln_flags;
    clone->invis_level	= parent->invis_level;
    clone->affected_by	= parent->affected_by;
    clone->shielded_by	= parent->shielded_by;
    clone->position	= parent->position;
    clone->practice	= parent->practice;
    clone->train	= parent->train;
    clone->saving_throw	= parent->saving_throw;
    clone->alignment	= parent->alignment;
    clone->ethos	= parent->ethos;
    clone->hitroll	= parent->hitroll;
    clone->damroll	= parent->damroll;
    clone->wimpy	= parent->wimpy;
    clone->form		= parent->form;
    clone->parts	= parent->parts;
    clone->size		= parent->size;
    clone->material	= str_dup( parent->material );
    clone->off_flags	= parent->off_flags;
    clone->dam_type	= parent->dam_type;
    clone->start_pos	= parent->start_pos;
    clone->default_pos	= parent->default_pos;
    clone->spec_fun	= parent->spec_fun;

    for (i = 0; i < 4; i++)
    	clone->armor[i]	= parent->armor[i];

    for (i = 0; i < MAX_STATS; i++)
    {
	clone->perm_stat[i]	= parent->perm_stat[i];
	clone->mod_stat[i]	= parent->mod_stat[i];
    }

    for (i = 0; i < 3; i++)
	clone->damage[i]	= parent->damage[i];

    /* now add the affects */
    for ( paf = parent->affected; paf != NULL; paf = paf->next )
        affect_to_char( clone, paf );

}




/*
 * Create an instance of an object.
 */
OBJ_DATA *
create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    if ( pObjIndex == NULL )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	exit( 1 );
    }

    obj = new_obj( );
    object_count++;

    obj->pIndexData	= pObjIndex;
    obj->in_room	= NULL;
    obj->enchanted	= FALSE;

    obj->level		= pObjIndex->level;
    obj->wear_loc	= -1;

    obj->name		= str_dup( pObjIndex->name );
    obj->short_descr	= str_dup( pObjIndex->short_descr );
    obj->description	= str_dup( pObjIndex->description );
    obj->material	= pObjIndex->material;
    obj->item_type	= pObjIndex->item_type;
    obj->extra_flags	= pObjIndex->extra_flags;
    obj->extra_flags2	= pObjIndex->extra_flags2;
    obj->wear_flags	= pObjIndex->wear_flags;
    obj->race_flags	= pObjIndex->race_flags;
    obj->class_flags	= pObjIndex->class_flags;
    obj->value[0]	= pObjIndex->value[0];
    obj->value[1]	= pObjIndex->value[1];
    obj->value[2]	= pObjIndex->value[2];
    obj->value[3]	= pObjIndex->value[3];
    obj->value[4]	= pObjIndex->value[4];
    obj->value[5]	= pObjIndex->value[5];
    obj->weight		= pObjIndex->weight;
    obj->ac_type	= pObjIndex->ac_type;
    obj->ac_vnum	= pObjIndex->ac_vnum;
    obj->ac_charge[0]	= pObjIndex->ac_charge[1];
    obj->ac_charge[1]	= pObjIndex->ac_charge[1];
    obj->join		= pObjIndex->join;
    obj->sep_one	= pObjIndex->sep_one;
    obj->sep_two	= pObjIndex->sep_two;

    obj->cost	= pObjIndex->cost;
    obj->qcost	= pObjIndex->qcost;

    if ( obj->item_type != ITEM_CONTAINER )
    {
	obj->timer	= pObjIndex->duration;
    }

    /*
     * Mess with object properties.
     */
    switch ( obj->item_type )
    {
    default:
	bug( "Read_object: vnum %d bad type.", pObjIndex->vnum );
	break;

    case ITEM_LIGHT:
	if (obj->value[2] == 999)
		obj->value[2] = -1;
	break;

    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_BOOK:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_FOOD:
    case ITEM_BOAT:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
    case ITEM_MAP:
    case ITEM_CLOTHING:
    case ITEM_PORTAL:
    case ITEM_SHEATH:
	break;

    case ITEM_BERRY:
    case ITEM_QUESTITEM:
    case ITEM_TREASURE:
    case ITEM_WARP_STONE:
    case ITEM_GEM:
    case ITEM_JEWELRY:
    case ITEM_PAPER:
    case ITEM_INK:
    case ITEM_PERMKEY:
	break;

    case ITEM_NOTEBOARD:
	REMOVE_BIT( obj->wear_flags, ITEM_TAKE );
	break;

    case ITEM_SCROLL:
	break;

    case ITEM_LENS:
    case ITEM_WAND:
    case ITEM_STAFF:
	break;

    case ITEM_WEAPON:
    case ITEM_QUIVER:
    case ITEM_ARROW:
	break;

    case ITEM_ARMOR:
	break;

    case ITEM_POTION:
    case ITEM_PILL:
    case ITEM_HERB:
	break;

    case ITEM_MONEY:
	break;
	case ITEM_TOKEN:
        //if (!pObjIndex->new_format )
          //  obj->value[0]       = number_fuzzy( number_fuzzy( obj->value[0]) );
        break;
    }

    for (paf = pObjIndex->affected; paf != NULL; paf = paf->next) 
	if ( paf->location == APPLY_SPELL_AFFECT )
	    affect_to_obj(obj,paf);

    obj->next		= object_list;
    object_list		= obj;
    pObjIndex->count++;

    return obj;
}

/* duplicate an object exactly -- except contents */
void clone_object(OBJ_DATA *parent, OBJ_DATA *clone)
{
    int i;
    AFFECT_DATA *paf;
    EXTRA_DESCR_DATA *ed,*ed_new;

    if (parent == NULL || clone == NULL)
	return;

    /* start fixing the object */
    clone->name 	= str_dup(parent->name);
    clone->short_descr 	= str_dup(parent->short_descr);
    clone->description	= str_dup(parent->description);
    clone->item_type	= parent->item_type;
    clone->extra_flags	= parent->extra_flags;
    clone->extra_flags2	= parent->extra_flags2;
    clone->wear_flags	= parent->wear_flags;
    clone->race_flags	= parent->race_flags;
    clone->class_flags	= parent->class_flags;
    clone->weight	= parent->weight;
    clone->cost		= parent->cost;
    clone->qcost		= parent->qcost;
    clone->level	= parent->level;
    clone->condition	= parent->condition;
    clone->material	= parent->material;
    clone->timer	= parent->timer;

    for (i = 0;  i < 5; i ++)
	clone->value[i]	= parent->value[i];

    /* affects */
    clone->enchanted	= parent->enchanted;

    for (paf = parent->affected; paf != NULL; paf = paf->next) 
	affect_to_obj(clone,paf);

    /* extended desc */
    for (ed = parent->extra_descr; ed != NULL; ed = ed->next)
    {
        ed_new                  = new_extra_descr();
        ed_new->keyword    	= str_dup( ed->keyword);
        ed_new->description     = str_dup( ed->description );
        ed_new->next           	= clone->extra_descr;
        clone->extra_descr  	= ed_new;
    }

}



/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;
    int i;

    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->prompt                  = &str_empty[0];
    ch->logon			= current_time;
    ch->lines			= PAGELEN;
    for (i = 0; i < 4; i++)
    	ch->armor[i]		= 100;
    ch->position		= POS_STANDING;
    ch->hit			= 20;
    ch->max_hit			= 20;
    ch->mana			= 100;
    ch->max_mana		= 100;
    ch->move			= 100;
    ch->max_move		= 100;
    ch->on			= NULL;
    for (i = 0; i < MAX_STATS; i ++)
    {
	ch->perm_stat[i] = 13; 
	ch->mod_stat[i] = 0;
    }
    return;
}


/*
 * Returns a pointer to the dream with the given vnum, or NULL if not found.
 */
DREAM_DATA *
get_dream_index( int vnum )
{
    DREAM_DATA *	pDream;

    if ( vnum <= 0 )
        return NULL;

    for ( pDream = dream_list; pDream != NULL; pDream = pDream->next )
        if ( pDream->vnum == vnum )
            return pDream;

    return NULL;
}


/*
 * Get an extra description from a list.
 */
char *
get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    for ( ; ed != NULL; ed = ed->next )
    {
	if ( is_name( (char *) name, ed->keyword ) )
	    return ed->description;
    }
    return NULL;
}


/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *
get_mob_index( int vnum )
{
    MOB_INDEX_DATA *pMobIndex;

    for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
	  pMobIndex != NULL;
	  pMobIndex  = pMobIndex->next )
    {
	if ( pMobIndex->vnum == vnum )
	    return pMobIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_mob_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *
get_obj_index( int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;

    for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
	  pObjIndex != NULL;
	  pObjIndex  = pObjIndex->next )
    {
	if ( pObjIndex->vnum == vnum )
	    return pObjIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_obj_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}


/*
 * Translates room virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *
get_room_index( int vnum )
{
    ROOM_INDEX_DATA *	pRoomIndex;

    /* Search for a static room. */
    for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
	  pRoomIndex != NULL;
	  pRoomIndex  = pRoomIndex->next )
    {
	if ( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}


/*
 * Translates room virtual number to its room index struct.
 * Hash table lookup.
 * If room not found check/create virtual room.
 */
ROOM_INDEX_DATA *
get_vroom_index( int vnum )
{
    ROOM_INDEX_DATA *	pRoomIndex;

    /* Search for a static room. */
    for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
	  pRoomIndex != NULL;
	  pRoomIndex  = pRoomIndex->next )
    {
	if ( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    }


    /* No static room, search for (and possibly create) a virtual room. */
    if ( ( pRoomIndex = get_virtual_room( vnum ) ) != NULL )
	return pRoomIndex;

    if ( fBootDb )
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}


/*
 * Read a letter from a file.
 */
char
fread_letter( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    return c;
}


/*
 * Read a number from a file.
 */
int
fread_number( FILE *fp )
{
    int number;
    bool sign;
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
	c = getc( fp );
    }
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }

    if ( !isdigit(c) )
    {
	bug( "Fread_number: bad format.", 0 );
	exit( 1 );
    }

    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }

    if ( sign )
	number = 0 - number;

    if ( c == '|' )
	number += fread_number( fp );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}


bitvector
fread_flag( FILE *fp )
{
    int number;
    char c;
    bool negative = FALSE;

    do
    {
	c = getc(fp);
    }
    while ( isspace(c));

    if (c == '-')
    {
	negative = TRUE;
	c = getc(fp);
    }

    number = 0;

    if (!isdigit(c))
    {
	while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))
	{
	    number += flag_convert(c);
	    c = getc(fp);
	}
    }

    while (isdigit(c))
    {
	number = number * 10 + c - '0';
	c = getc(fp);
    }

    if (c == '|')
	number += fread_flag(fp);

    else if  ( c != ' ')
	ungetc(c,fp);

    if (negative)
	return -1 * number;

    return number;
}


/*
 * Read an extended bitvector from a file.			-Thoric
 */
EXT_BV
fread_xbits( FILE *fp )
{
    EXT_BV ret;
    int c, x = 0;
    int num = 0;

    memset( &ret, '\0', sizeof( ret ) );
    for ( ;; )
    {
	num = fread_flag( fp );
	if ( x < XBI )
	    ret.bits[x] = num;
	++x;
	if ( ( c = getc( fp ) ) != '&' )
	{
	    ungetc( c, fp );
	    break;
	}
    }

    return ret;
}


long
flag_convert( char letter )
{
    long bitsum = 0;
    char i;

    if ('A' <= letter && letter <= 'Z' ) 
    {
	bitsum = 1;
	for ( i = letter; i > 'A'; i-- )
	    bitsum *= 2;
    }
    else if ( 'a' <= letter && letter <= 'z' )
    {
	bitsum = 67108864; /* 2^26 */
	for ( i = letter; i > 'a'; i-- )
	    bitsum *= 2;
    }

    return bitsum;
}


/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 *   this function takes 40% to 50% of boot-up time.
 */
char *
fread_string( FILE *fp )
{
    char *plast;
    char c;

    plast = top_string + sizeof(char *);
    if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
    {
	bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
	exit( 1 );
    }

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    if ( ( *plast++ = c ) == '~' )
	return &str_empty[0];

    for ( ;; )
    {
        /*
         * Back off the char type lookup,
         *   it was too dirty for portability.
         *   -- Furey
         */

	switch ( *plast = getc(fp) )
	{
        default:
            plast++;
            break;

        case EOF:
	/* temp fix */
            bug( "Fread_string: EOF", 0 );
	    return NULL;
            /* exit( 1 ); */
            break;

        case '\n':
            plast++;
            *plast++ = '\r';
            break;

        case '\r':
            break;

        case '~':
            plast++;
	    {
		union
		{
		    char *	pc;
		    char	rgc[sizeof(char *)];
		} u1;
		int ic;
		int iHash;
		char *pHash;
		char *pHashPrev;
		char *pString;

		plast[-1] = '\0';
		iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
		for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
		{
		    for ( ic = 0; ic < sizeof(char *); ic++ )
			u1.rgc[ic] = pHash[ic];
		    pHashPrev = u1.pc;
		    pHash    += sizeof(char *);

		    if ( top_string[sizeof(char *)] == pHash[0]
		    &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
		    {
			dup_count++;
			dup_len += strlen( pHash );
			return pHash;
		    }
		}

		if ( fBootDb )
		{
		    pString		= top_string;
		    top_string		= plast;
		    u1.pc		= string_hash[iHash];
		    for ( ic = 0; ic < sizeof(char *); ic++ )
			pString[ic] = u1.rgc[ic];
		    string_hash[iHash]	= pString;

		    nAllocString += 1;
		    sAllocString += top_string - pString;
		    return pString + sizeof(char *);
		}
		else
		{
		    return str_dup( top_string + sizeof(char *) );
		}
	    }
	}
    }
}


char *
fread_string_eol( FILE *fp )
{
    static bool char_special[256-EOF];
    char *plast;
    char c;

    if ( char_special[EOF-EOF] != TRUE )
    {
        char_special[EOF -  EOF] = TRUE;
        char_special['\n' - EOF] = TRUE;
        char_special['\r' - EOF] = TRUE;
    }

    plast = top_string + sizeof(char *);
    if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
    {
        bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
        exit( 1 );
    }

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
        c = getc( fp );
    }
    while ( isspace(c) );

    if ( ( *plast++ = c ) == '\n')
        return &str_empty[0];

    for ( ;; )
    {
        if ( !char_special[ ( *plast++ = getc( fp ) ) - EOF ] )
            continue;

        switch ( plast[-1] )
        {
        default:
            break;

        case EOF:
            bug( "Fread_string_eol  EOF", 0 );
            exit( 1 );
            break;

        case '\n':  case '\r':
            {
                union
                {
                    char *      pc;
                    char        rgc[sizeof(char *)];
                } u1;
                int ic;
                int iHash;
                char *pHash;
                char *pHashPrev;
                char *pString;

                plast[-1] = '\0';
                iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
                for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
                {
                    for ( ic = 0; ic < sizeof(char *); ic++ )
                        u1.rgc[ic] = pHash[ic];
                    pHashPrev = u1.pc;
                    pHash    += sizeof(char *);

                    if ( top_string[sizeof(char *)] == pHash[0]
                    &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
                        return pHash;
                }

                if ( fBootDb )
                {
                    pString             = top_string;
                    top_string          = plast;
                    u1.pc               = string_hash[iHash];
                    for ( ic = 0; ic < sizeof(char *); ic++ )
                        pString[ic] = u1.rgc[ic];
                    string_hash[iHash]  = pString;

                    nAllocString += 1;
                    sAllocString += top_string - pString;
                    return pString + sizeof(char *);
                }
                else
                {
                    return str_dup( top_string + sizeof(char *) );
                }
            }
        }
    }
}


/*
 * Read to end of line (for comments).
 */
void
fread_to_eol( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( c != '\n' && c != '\r' );

    do
    {
	c = getc( fp );
    }
    while ( c == '\n' || c == '\r' );

    ungetc( c, fp );
    return;
}


/*
 * Read one word (into static buffer).
 */
char *
fread_word( FILE *fp )
{
    static char word[MAX_INPUT_LENGTH];
    char *pword;
    char cEnd;

    do
    {
	cEnd = getc( fp );
    }
    while ( isspace( cEnd ) );

    if ( cEnd == '\'' || cEnd == '"' )
    {
	pword   = word;
    }
    else
    {
	word[0] = cEnd;
	pword   = word+1;
	cEnd    = ' ';
    }

    for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
    {
	*pword = getc( fp );
	if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }

    bug( "Fread_word: word too long.", 0 );
    exit( 1 );
    return NULL;
}


/*
 * Read string into user supplied buffer.
 * Modified version of Furey's fread_string
 */
char *
fread_buf( FILE *fp, char *buf, int max )
{
    char *ptr;
    int	c;

    ptr = buf;
    --max;

    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    if ( ( *ptr++ = c ) == '~' )
    {   
	*buf = '\0';
	return buf;
    }

    while ( --max != 0 )
    {
	switch ( *ptr = getc( fp ) )
	{
	    default:
	        ptr++;
		break;

	    case EOF:
		bug( "Fread_buf: EOF", 0 );
		exit( 1 );
		break;

	    case '\n':
		ptr++;
		*ptr++ = '\r';
		break;

	    case '\r':
		break;

	    case '~':
		*ptr = '\0';
		return buf;
	}
    }

    bugf( "Fread_buf: string too long." );
    do
	c = getc( fp );
    while ( c != '~' && c != EOF );
    return buf;
}


/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( unsigned sMem )
{
    void *pMem;
    int *magic;
    int iList;

    sMem += sizeof(*magic);

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
        if ( sMem <= rgSizeList[iList] )
            break;
    }

    if ( iList == MAX_MEM_LIST )
    {
        bug( "Alloc_mem: size %d too large.", sMem );
        exit( 1 );
    }

    if ( rgFreeList[iList] == NULL )
    {
        pMem              = alloc_perm( rgSizeList[iList] );
    }
    else
    {
        pMem              = rgFreeList[iList];
        rgFreeList[iList] = * ((void **) rgFreeList[iList]);
    }

    magic = (int *) pMem;
    *magic = MAGIC_NUM;
    pMem += sizeof(*magic);

    return pMem;
}



/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, unsigned sMem )
{
    int iList;
    int *magic;

    pMem -= sizeof(*magic);
    magic = (int *) pMem;

    if (*magic != MAGIC_NUM)
    {
        bug("Attempt to recyle invalid memory of size %d.",sMem);
        bug((char*) pMem + sizeof(*magic),0);
abort( );
        return;
    }

    *magic = 0;
    sMem += sizeof(*magic);

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
        if ( sMem <= rgSizeList[iList] )
            break;
    }

    if ( iList == MAX_MEM_LIST )
    {
        bug( "Free_mem: size %d too large.", sMem );
        exit( 1 );
    }

    * ((void **) pMem) = rgFreeList[iList];
    rgFreeList[iList]  = pMem;

    return;
}


/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( unsigned sMem )
{
    static char *pMemPerm;
    static int iMemPerm;
    void *pMem;

    while ( sMem % sizeof(long) != 0 )
	sMem++;
    if ( sMem > MAX_PERM_BLOCK )
    {
	bug( "Alloc_perm: %d too large.", sMem );
	abort( );
    }

    if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if ( ( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) == NULL )
	{
	    perror( "Alloc_perm" );
	    exit( 1 );
	}
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;
    return pMem;
}


/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *
str_dup( const char *str )
{
    char *str_new;
    int   len;

    if ( str[0] == '\0' )
	return &str_empty[0];

    if ( str >= string_space && str < top_string )
    {
        dup_count++;
        dup_len += strlen( str );
	return (char *) str;
    }

    len = strlen( str );
    str_new = alloc_mem( len + 1 );
    strcpy( str_new, str );
    top_str_dup++;
    top_str_len += len;
    return str_new;
}



/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void
free_string( char *pstr )
{
    int len;

    if ( pstr == NULL
    ||   pstr == &str_empty[0] )
        return;

    if ( pstr >= string_space && pstr < top_string )
    {
	dup_count--;
	dup_len -= strlen( pstr );
	return;
    }

    len = strlen( pstr );
    free_mem( pstr, len + 1 );
    top_str_dup--;
    top_str_len -= len;
    return;
}


/*
 * Disabled functionality of this routine.
 * This is an old piece of debugging code that would basically report
 * ALL allocations of memory on the wiznet channel.
 * Its usefulness was limited by the fact that occasionally somebody's
 * output buffer would have to be expanded, causing another wiznet message,
 * causing another memory allocation, etc, etc, until the stack was
 * full with the result being a mud crash.
 *
 * I intend to rewrite this as a safer function, possibly reporting
 * via log_string() or some such.  Thus I made it just return rather than
 * removing it entirely (including the calls to it).
 */
void
check_mem_change( void )
{
    char buf[MAX_INPUT_LENGTH];

    return;
    if ( nAllocPerm == last_nAllocPerm && sAllocPerm == last_sAllocPerm )
	return;

    sprintf( buf, "Perms: %d(%+d) blocks of %d(%+d) bytes.",
	     nAllocPerm, nAllocPerm - last_nAllocPerm,
	     sAllocPerm, sAllocPerm - last_sAllocPerm );
    wiznet( buf, NULL, NULL, WIZ_MEMORY, 0, 0 );
    last_nAllocPerm = nAllocPerm;
    last_sAllocPerm = sAllocPerm;
    return;
}


/*
 * Check for an MSSP request.
 * Called from nanny().
 * MSSP protocol from http://tintin.sourceforge.net/mssp
 */
bool
check_mssp( DESCRIPTOR_DATA *d, char *argument )
{
    char                buf[MAX_INPUT_LENGTH];
    struct timeval      start;
    struct timeval      stop;
    DESCRIPTOR_DATA *   dl;
    MOB_INDEX_DATA *    pMob;
    OBJ_INDEX_DATA *    pObj;
    ROOM_INDEX_DATA *   pRoom;
    EXIT_DATA *         pExit;
    MPROG_DATA *        mprog;
    OREPROG_DATA *      oreprog;
    char *              p;
    double              elapsed;
    int                 count;
    int                 exit;
    int                 i;
    int                 lcount;         /* Count of mob/obj/etc prog lines */
    int                 pcount;         /* Player count, prog count */
    int                 rcount;         /* Room count */

    if ( str_cmp( argument, "MSSP-REQUEST" ) )
        return FALSE;

    gettimeofday( &start, NULL );

    write_to_buffer( d, "\r\nMSSP-REPLY-START\r\n", 0 );

    /* Required stuff -- mudname, # players, uptime */
    write_to_buffer( d, "NAME\t" MUD_NAME "\r\n", 0 );
    pcount = 0;
    for ( dl = descriptor_list; dl != NULL; dl = dl->next )
        if ( dl->connected == CON_PLAYING )
            pcount++;
    sprintf( buf, "PLAYERS\t%d\r\n", pcount );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "UPTIME\t%ld\r\n", startup_time );
    write_to_buffer( d, buf, 0 );

    /* Generic */
    write_to_buffer( d, "CREATED\t2005\r\n", 0 );
    write_to_buffer( d, "LANGUAGE\tEnglish\r\n", 0 );
    write_to_buffer( d, "MINIMUM AGE\t0\r\n", 0 );
#if defined(MUD_WEBSITE)
    write_to_buffer( d, "WEBSITE\t" MUD_WEBSITE "\r\n", 0 );
#endif

    /* Categorization */
    write_to_buffer( d, "FAMILY\tDiku\r\n", 0 );
    write_to_buffer( d, "GENRE\tFantasy\r\n", 0 );
    write_to_buffer( d, "GAMEPLAY\tRoleplaying\r\n", 0 );
    write_to_buffer( d, "STATUS\tAlpha\r\n", 0 );
    write_to_buffer( d, "SUBGENRE\tMedieval Fantasy\r\n", 0 );

    /* World */
    sprintf( buf, "AREAS\t%d\r\n", top_area );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "HELPFILES\t%d\r\n", top_help );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "MOBILES\t%d\r\n", top_mob_index );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "OBJECTS\t%d\r\n", top_obj_index );
    write_to_buffer( d, buf, 0 );
    rcount = top_room + map_mem / sizeof( VIRTUAL_ROOM_DATA );
    sprintf( buf, "ROOMS\t%d\r\n", rcount );
    write_to_buffer( d, buf, 0 );

    sprintf( buf, "CLASSES\t%d\r\n", MAX_CLASS );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "LEVELS\t%d\r\n", MAX_MORTAL );
    write_to_buffer( d, buf, 0 );

    count = 0;
    for ( i = 0; !IS_NULLSTR( race_table[i].name ); i++ )
        if ( race_table[i].pc_race )
            count++;
    sprintf( buf, "RACES\t%d\r\n", count );
    write_to_buffer( d, buf, 0 );
    count = 0;
    for ( i = 1; !IS_NULLSTR( skill_table[i].name ); i++ )
        count++;
    sprintf( buf, "SKILLS\t%d\r\n", count );
    write_to_buffer( d, buf, 0 );

    /* Protocols */
    write_to_buffer( d, "ANSI\t1\r\n", 0 );
    write_to_buffer( d, "MCCP\t0\r\n", 0 );
    write_to_buffer( d, "MCP\t0\r\n", 0 );
    write_to_buffer( d, "MSP\t0\r\n", 0 );
    write_to_buffer( d, "MXP\t0\r\n", 0 );
    write_to_buffer( d, "PUEBLO\t0\r\n", 0 );
    write_to_buffer( d, "VT100\t0\r\n", 0 );
    write_to_buffer( d, "XTERM 256 COLORS\t0\r\n", 0 );

    /* Commercial */
    write_to_buffer( d, "PAY TO PLAY\t0\r\n", 0 );
    write_to_buffer( d, "PAY FOR PERKS\t0\r\n", 0 );

    /* Hiring */
    sprintf( buf, "HIRING BUILDERS\t%d\r\n", hiring[HIRE_BUILDERS] );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "HIRING CODERS\t%d\r\n", hiring[HIRE_CODERS] );
    write_to_buffer( d, buf, 0 );

    /* MudBytes extended variables */
    sprintf( buf, "DBSIZE\t%d\r\n", rcount + top_exit + object_count + mobile_count + pcount );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "EXITS\t%d\r\n", top_exit );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "EXTRA DESCRIPTIONS\t%d\r\n", top_ed );
    write_to_buffer( d, buf, 0 );

    lcount = 0;
    pcount = 0;
    for ( i = 0; i < MAX_KEY_HASH; i++ )
    {
        /* count mobprogs and lines */
        for ( pMob = mob_index_hash[i]; pMob != NULL; pMob = pMob->next )
        {
            for ( mprog = pMob->mobprogs; mprog != NULL; mprog = mprog->next )
            {
                pcount++;
                for ( p = mprog->comlist; *p != '\0'; p++ )
                    if ( *p == '\n' )
                        lcount++;
            }
        }

        /* count obj progs and lines */
        for ( pObj = obj_index_hash[i]; pObj != NULL; pObj = pObj->next )
        {
            for ( oreprog = pObj->oprogs; oreprog != NULL; oreprog = oreprog->next )
            {
                pcount++;
                for ( p = oreprog->comlist; *p != '\0'; p++ )
                    if ( *p == '\n' )
                        lcount++;
            }
        }

        for ( pRoom = room_index_hash[i]; pRoom != NULL; pRoom = pRoom->next )
        {
            for ( oreprog = pRoom->rprogs; oreprog != NULL; oreprog = oreprog->next )
            {
                pcount++;
                for ( p = oreprog->comlist; *p != '\0'; p++ )
                    if ( *p == '\n' )
                        lcount++;
            }
            for ( exit = 0; exit < MAX_DIR; exit++ )
            {
                if ( ( pExit = pRoom->exit[exit] ) != NULL )
                {
                    for ( oreprog = pExit->eprogs; oreprog != NULL; oreprog = oreprog->next )
                    {
                        pcount++;
                        for ( p = oreprog->comlist; *p != '\0'; p++ )
                            if ( *p == '\n' )
                                lcount++;
                    }
                }
            }
        }

    }
    sprintf( buf, "MUDPROGS\t%d\r\n", lcount );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "MUDTRIGS\t%d\n", pcount );
    write_to_buffer( d, buf, 0 );
    sprintf( buf, "RESETS\t%d\r\n", top_reset );
    write_to_buffer( d, buf, 0 );

    write_to_buffer( d, "MSSP-REPLY-END\r\n", 0 );
    gettimeofday( &stop, NULL );
    if ( stop.tv_sec > start.tv_sec
    || ( stop.tv_sec == start.tv_sec && stop.tv_usec > start.tv_usec ) )
    {
	elapsed = ( ( stop.tv_sec - start.tv_sec ) * 1000000.0 +
	            ( stop.tv_usec - start.tv_usec ) ) / 1000000.0;
    }
    else
        elapsed = 0.0;
    sprintf( buf, "MSSP request from %s (%s) took %f seconds", d->host, d->ip, elapsed );
    close_socket( d );
    log_string( buf );
    wiznet( buf, NULL, NULL, WIZ_SITES, 0, IMPLEMENTOR );
    return TRUE;
}


#define ADD_MEM( s, v )		do {					\
				item = new_strlist( );			\
				sprintf( buf2, "%s:", (s) );		\
				item->len = sprintf( buf, "%-12s %5d", buf2, v );	\
				item->str = str_dup( buf );		\
				item->next = NULL;			\
				if ( strlist == NULL )			\
				strlist = item;				\
				else					\
				strlast->next = item;			\
				strlast = item;				\
				} while ( 0 )
#define LINK_MEM( s )		do {					\
				item = new_strlist( );			\
				item->str = str_dup( s );		\
				item->len = strlen( s );		\
				item->next = NULL;			\
				if ( strlist == NULL )			\
				strlist = item;				\
				else					\
				strlast->next = item;			\
				strlast = item;				\
				} while ( 0 )


void
do_memory( CHAR_DATA *ch, char *argument )
{
    char		buf[MAX_INPUT_LENGTH];
    char		buf2[SHORT_STRING_LENGTH];
    int			len_prev;
    BUFFER *		pBuf;
    STRLIST_DATA *	strlast;
    STRLIST_DATA *	strlist;
    STRLIST_DATA *	item;

    strlist = NULL;
    strlast = NULL;	/* Not necessary.
			GCC 4.x issues a warning without it though.
			Bleh. */

    ADD_MEM( "Affects", top_affect );
    ADD_MEM( "Aliases", top_alias );
    ADD_MEM( "Areas", top_area );
    ADD_MEM( "Avatar data", top_avatar );
    ADD_MEM( "Bans", top_ban );
    ADD_MEM( "Banks", top_bank );
    ADD_MEM( "Boards", top_board );
    ADD_MEM( "Board Info", top_board_info );
    ADD_MEM( "Buffers", top_buffer );
    ADD_MEM( "Chars", top_char );
    ADD_MEM( "Clans", top_clan );
    ADD_MEM( "Clists", top_clist );
    sprintf( buf, "Events:      %5d (in use: %d)", top_event, top_event_used );
    LINK_MEM( buf );
    ADD_MEM( "Descriptors", top_desc );
    ADD_MEM( "Dreams", top_dream );
    ADD_MEM( "Dream segs", top_dream_seg );
    ADD_MEM( "ExDes", top_ed );
    ADD_MEM( "Exits", top_exit );
    sprintf( buf, "Greet table%7d bytes", greet_size * greet_size );
    LINK_MEM( buf );
    ADD_MEM( "Helps", top_help );
    ADD_MEM( "Ignore data", top_ignore );
    ADD_MEM( "Milists", top_milist );
    sprintf( buf, "Mobiles:     %5d (in use: %d)", top_mob_index, mobile_count );
    LINK_MEM( buf );
    ADD_MEM( "Mob Progs", top_mprog );
    ADD_MEM( "Mprog acts", top_mpact );
    ADD_MEM( "Notes", top_note );
    sprintf( buf, "Objects:     %5d (in use: %d)", top_obj_index, object_count );
    LINK_MEM( buf );
    ADD_MEM( "Oilists", top_oilist );
    ADD_MEM( "O/R/E Progs", top_oreprog );
    sprintf( buf, "Overlands:   %5d using %d bytes", top_overland, map_mem );
    LINK_MEM( buf );
    ADD_MEM( "Pc data", top_pcdata );
    ADD_MEM( "Projects", top_project );
    ADD_MEM( "Resets", top_reset );
    sprintf( buf, "Rooms:       %5d (%d virtual)", top_room, top_vroom );
    LINK_MEM( buf );
    ADD_MEM( "Shops", top_shop );
    ADD_MEM( "Socials", top_social );
    ADD_MEM( "Text lists", top_textlist );
    ADD_MEM( "Travelers", top_travel );
    ADD_MEM( "Userlist", top_userlist );
    sprintf( buf, "DB strings: %6d strings of %7d bytes (max %d)",
	     nAllocString, sAllocString, MAX_STRING );
    LINK_MEM( buf );
    sprintf( buf, "Duplicates: %6d strings of %7d bytes.", dup_count, dup_len );
    LINK_MEM( buf );
    sprintf( buf, "Strings:    %6d strings of %7d bytes", top_str_dup, top_str_len );
    LINK_MEM( buf );
    sprintf( buf, "Perms:      %6d blocks  of %7d bytes", nAllocPerm, sAllocPerm );
    LINK_MEM( buf );

    len_prev = 0;
    pBuf = new_buf( );
    while ( strlist != NULL )
    {
	item = strlist;
	strlist = item->next;

	if ( len_prev != 0 )
	{
	    if ( len_prev > 39 || item->len > 39)
	    {
		add_buf( pBuf, "\n\r" );
		len_prev = 0;
	    }
	    else
	    {
		buf_printf( pBuf, "%*s", 40 - len_prev, "" );
		len_prev = 40;
	    }
	}

	add_buf( pBuf, item->str );
	len_prev += item->len;
	free_strlist( item );
    }

    add_buf( pBuf, "\n\r" );

    page_to_char( buf_string( pBuf ), ch );
    free_buf( pBuf );

    return;
}


void
do_dump( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *	pObj;
    MOB_INDEX_DATA *	pMob;
    ROOM_INDEX_DATA *	pRoom;
    MONEY		money;
    char		arg[MAX_INPUT_LENGTH];
    char		buf[MAX_INPUT_LENGTH];
    char		buf1[MAX_INPUT_LENGTH];
    char		short_d[MAX_INPUT_LENGTH];
    char *		p;
    FILE *		fp;
    int			iClass;
    int			start_vnum;
    int			vnum;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax: dump all|armor|cast|gold|group|mobiles|rooms|skills|teachers|values|weapons\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        do_dump( ch, "armor" );
        do_dump( ch, "cast" );
        do_dump( ch, "gold" );
        do_dump( ch, "group" );
        do_dump( ch, "mobiles" );
        do_dump( ch, "rooms" );
        do_dump( ch, "skills" );
        do_dump( ch, "teachers" );
        do_dump( ch, "values" );
        do_dump( ch, "weapons" );
        return;
    }

    /* Start with first non_limbo object/mob */
    start_vnum = area_first->max_vnum + 1;

    if ( !str_prefix( arg, "armor" ) )
    {
        if ( ( fp = fopen( "armor.txt", "w" ) ) == NULL )
        {
            perror( "dump: armor.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }
        for ( vnum = start_vnum; vnum < top_vnum_obj; vnum++ )
        {
            if ( ( pObj = get_obj_index( vnum ) ) == NULL )
                continue;
            if ( pObj->item_type != ITEM_ARMOR )
                continue;
            strip_color( short_d, pObj->short_descr );
            fprintf( fp, "%d, %d, \"%s\", %s\n",
                     vnum, pObj->level, short_d,
                     flag_string( wear_flags, pObj->wear_flags ) );
        }
        fclose( fp );
        send_to_char( "armor.txt written.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "cast" ) )
    {
        if ( ( fp = fopen( "objcast.txt", "w" ) ) == NULL )
        {
            perror( "dump: objcast.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }

        fprintf( fp, "Vnum, ObjLevel, Short, Type, Cost, SpellLvl, Spell1, Spell2, Spell3\n" );
        for ( vnum = start_vnum; vnum <= top_vnum_obj; vnum++ )
        {
            const char *	spell1;
            const char *	spell2;
            const char *	spell3;

            if ( ( pObj = get_obj_index( vnum ) ) == NULL )
                continue;
            switch ( pObj->item_type )
            {
                default: continue;
                case ITEM_PILL:
                case ITEM_POTION:
                case ITEM_SCROLL:
                    spell1 = pObj->value[1] > 0 ? skill_table[pObj->value[1]].name : "(none)";
                    spell2 = pObj->value[2] > 0 ? skill_table[pObj->value[2]].name : "(none)";
                    spell3 = pObj->value[3] > 0 ? skill_table[pObj->value[3]].name : "(none)";
                    break;
                case ITEM_WAND:
                case ITEM_STAFF:
                case ITEM_LENS:
                    spell1 = pObj->value[3] > 0 ? skill_table[pObj->value[3]].name : "(none)";
                    spell2 = "N/A";
                    spell3 = "N/A";
                    break;
                case ITEM_HERB:
                    spell1 = pObj->value[1] > 0 ? skill_table[pObj->value[1]].name : "(none)";
                    spell2 = pObj->value[2] > 0 ? skill_table[pObj->value[2]].name : "(none)";
                    spell3 = pObj->value[3] > 0 ? skill_table[pObj->value[3]].name : "(none)";
                    break;
            }
            strip_color( short_d, pObj->short_descr );
            fprintf( fp, "%d, %d, \"%s\", \"%s\", %d, %d, \"%s\", \"%s\", \"%s\"\n", 
                     vnum, pObj->level, short_d, item_name( pObj->item_type ),
                     pObj->cost, pObj->value[0], spell1, spell2, spell3 );
        }

        fclose( fp );
        send_to_char( "objcast.txt written.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "gold" ) )
    {
        if ( ( fp = fopen( "objgold.txt", "w" ) ) == NULL )
        {
            perror( "dump: objgold.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }
        fprintf( fp, "Vnum, Level, Short, Type, CostInCopper, Cost\n" );
        for ( vnum = start_vnum; vnum < top_vnum_obj; vnum++ )
        {
            if ( ( pObj = get_obj_index( vnum ) ) == NULL )
                continue;

            memset( &money, 0, sizeof( money ) );
            money.fract = pObj->cost;
            normalize( &money );

            strip_color( short_d, pObj->short_descr );
            fprintf( fp, "%d, %d, \"%s\", %s, %d, \"%s\"\n",
                     vnum,
                     pObj->level,
                     short_d,
                     flag_string( item_types, pObj->item_type ),
                     pObj->cost,
                     money_string( &money, TRUE, TRUE )
                     );
        }
        fclose( fp );
        send_to_char( "objgold.txt written.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "groups" ) )
    {
        if ( ( fp = fopen( "groups.txt", "w" ) ) == NULL )
        {
            perror( "dump: groups.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }

        fprintf( fp, "\"Group Name\"" );
        for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
            fprintf( fp, ", \"%s\"", class_table[iClass].who_name );
        fprintf( fp, ", \"Spell/Group\"\n" );

        for ( vnum = 0; vnum < MAX_GROUP; vnum++ )
        {
            int i;

            if ( IS_NULLSTR( group_table[vnum].name ) )
                break;
            fprintf( fp, "\"%s\"", group_table[vnum].name );
            for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
                fprintf( fp, ", %d", group_table[vnum].rating[iClass] );
            for ( i = 0; i < MAX_IN_GROUP; i++ )
                if ( !IS_NULLSTR( group_table[vnum].spells[i] ) )
                    fprintf( fp, ", \"%s\"", group_table[vnum].spells[i] );
            fprintf( fp, "\n" );
        }

        fclose( fp );
        send_to_char( "groups.txt written.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "mobiles" ) || !str_cmp( arg, "mobs" ) )
    {
        if ( ( fp= fopen( "mobiles.txt", "w" ) ) == NULL )
        {
            perror( "dump: mobiles.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }
        for ( vnum = start_vnum; vnum <= top_vnum_mob; vnum++ )
        {
            if ( ( pMob = get_mob_index( vnum ) ) == NULL )
                continue;
            money.gold = 0;
            money.silver = 0;
            money.copper = 0;
            money.fract = pMob->wealth;
            normalize( &money );
            strip_color( short_d, pMob->short_descr );
            fprintf( fp, "%d, %d, \"%s\", \"%s\", \"%s\"\n",
                vnum, pMob->level,
                short_d,
                money_string( &money, FALSE, TRUE ),
                flag_string( act_flags, pMob->act )
                );
        }
        fclose( fp );
        send_to_char( "mobiles.txt written.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "rooms" ) )
    {
        if ( ( fp= fopen( "rooms.txt", "w" ) ) == NULL )
        {
            perror( "dump: rooms.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }
        for ( vnum = start_vnum; vnum <= top_vnum_room; vnum++ )
        {
            if ( ( pRoom = get_room_index( vnum ) ) == NULL )
                continue;
            strip_color( short_d, pRoom->name );
            p = short_d;
            while ( *p != '\0' )
            {
                if ( *p == '"' )
                    *p = '\'';
                p++;
            }
            strcpy( buf, flag_string( sector_types, pRoom->sector_type ) );
            fprintf( fp, "%d, \"%s\", \"%s\", \"%s\"\n", vnum, short_d, buf,
                     flag_string( room_flags, pRoom->room_flags ) );
        }
        fclose( fp );
        send_to_char( "rooms.txt written.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "skills" ) || !str_prefix( arg, "spells" ) )
    {
        if ( ( fp= fopen( "skills.txt", "w" ) ) == NULL )
        {
            perror( "dump: skills.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }

        fprintf( fp, "\"Sn\", \"Name\", \"Type\", \"Mana\"" );
        for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
            fprintf( fp, ", \"%s\"", class_table[iClass].who_name );
        fprintf( fp, "\n" );

        for ( vnum = 1; !IS_NULLSTR( skill_table[vnum].name ); vnum++ )
        {
            fprintf( fp, "%d, \"%s\", \"%s\", %d",
                     vnum, skill_table[vnum].name,
                     skill_table[vnum].spell_fun == spell_null ? "Skill" : "Spell",
                     skill_table[vnum].min_mana );
            for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
                fprintf( fp, ", %d", skill_table[vnum].skill_level[iClass] );
            fprintf( fp, "\n" );
        }

        fclose( fp );
        send_to_char( "skills.txt written.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "teachers" ) )
    {
        if ( ( fp= fopen( "teachers.txt", "w" ) ) == NULL )
        {
            perror( "dump: teachers.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }
        for ( vnum = start_vnum; vnum <= top_vnum_mob; vnum++ )
        {
            if ( ( pMob = get_mob_index( vnum ) ) == NULL )
                continue;
            if ( !IS_SET( pMob->act, ACT_GAIN )
            &&	 !IS_SET( pMob->act, ACT_TRAIN )
            &&	 !IS_SET( pMob->act, ACT_PRACTICE ) )
                continue;
            strip_color( short_d, pMob->short_descr );

            strcpy( buf, " none?!?" );
            p = buf;
            if ( IS_SET( pMob->act, ACT_GAIN ) )
                p = stpcpy( p, " gain" );
            if ( IS_SET( pMob->act, ACT_PRACTICE ) )
                p = stpcpy( p, " practice" );
            if ( IS_SET( pMob->act, ACT_TRAIN ) )
                p = stpcpy( p, " train" );

            strcpy( buf1, " none" );
            p = buf1;
            for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
            {
                if ( xIS_SET( pMob->train, iClass ) )
                {
                    *p++ = ' ';
                    p = stpcpy( p, class_table[iClass].name );
                }
            }
            fprintf( fp, "%d, %d, \"%s\", \"%s\", \"%s\"\n",
                vnum, pMob->level,
                short_d, buf + 1,
                buf1 + 1
                );
        }
        fclose( fp );
        send_to_char( "teachers.txt written.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "values" ) )
    {
        if ( ( fp = fopen( "objvalue.txt", "w" ) ) == NULL )
        {
            perror( "dump: objvalue.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }
        for ( vnum = start_vnum; vnum < top_vnum_obj; vnum++ )
        {
            if ( ( pObj = get_obj_index( vnum ) ) == NULL )
                continue;

            strip_color( short_d, pObj->short_descr );
            fprintf( fp, "%d, %d, \"%s\", \"%s\", %d, %d, %d, %d, %d, %d\n",
                     vnum,
                     pObj->level,
                     short_d,
                     flag_string( item_types, pObj->item_type ),
                     pObj->value[0],
                     pObj->value[1],
                     pObj->value[2],
                     pObj->value[3],
                     pObj->value[4],
                     pObj->value[5]
                     );
        }
        fclose( fp );
        send_to_char( "objvalue.txt written.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "weapons" ) )
    {
        if ( ( fp = fopen( "weapons.txt", "w" ) ) == NULL )
        {
            perror( "dump: armor.txt" );
            send_to_char( "Unable to open file.\n\r", ch );
            return;
        }
        for ( vnum = start_vnum; vnum < top_vnum_obj; vnum++ )
        {
            if ( ( pObj = get_obj_index( vnum ) ) == NULL )
                continue;
            if ( pObj->item_type != ITEM_WEAPON )
                continue;
            strip_color( short_d, pObj->short_descr );
            fprintf( fp, "%d, %d, \"%s\", %s\n",
                     vnum, pObj->level, short_d,
                     flag_string( weapon_types, pObj->value[0] ) );
        }
        fclose( fp );
        send_to_char( "weapons.txt written.\n\r", ch );
        return;
    }

    do_dump( ch, "" );

}


/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    switch ( number_bits( 2 ) )
    {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}


/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
    int power;
    int number;

    if (from == 0 && to == 0)
	return 0;

    if ( ( to = to - from + 1 ) <= 1 )
	return from;

    for ( power = 2; power < to; power <<= 1 )
	;

    while ( ( number = number_mm() & (power -1 ) ) >= to )
	;

    return from + number;
}


/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    int percent;

    while ( (percent = number_mm() & (128-1) ) > 99 )
	;

    return 1 + percent;
}


/*
 * Generate a random door.
 */
int number_door( void )
{
    int door;

    while ( ( door = number_mm() & (16-1) ) >= MAX_DIR )
	;

    return door;
}

int number_bits( int width )
{
    return number_mm( ) & ( ( 1 << width ) - 1 );
}


/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */

/* I noticed streaking with this random number generator, so I switched
   back to the system srandom call.  If this doesn't work for you, 
   define OLD_RAND to use the old system -- Alander */

#if defined (OLD_RAND)
static  int     rgiState[2+55];
#endif

void init_mm( )
{
#if defined (OLD_RAND)
    int *piState;
    int iState;

    piState     = &rgiState[2];

    piState[-2] = 55 - 55;
    piState[-1] = 55 - 24;

    piState[0]  = ((int) current_time) & ((1 << 30) - 1);
    piState[1]  = 1;
    for ( iState = 2; iState < 55; iState++ )
    {
        piState[iState] = (piState[iState-1] + piState[iState-2])
                        & ((1 << 30) - 1);
    }
#else
    srandom(time(NULL)^getpid());
#endif
    return;
}


long
number_mm( void )
{
#if defined (OLD_RAND)
    int *piState;
    int iState1;
    int iState2;
    int iRand;

    piState             = &rgiState[2];
    iState1             = piState[-2];
    iState2             = piState[-1];
    iRand               = (piState[iState1] + piState[iState2])
                        & ((1 << 30) - 1);
    piState[iState1]    = iRand;
    if ( ++iState1 == 55 )
        iState1 = 0;
    if ( ++iState2 == 55 )
        iState2 = 0;
    piState[-2]         = iState1;
    piState[-1]         = iState2;
    return iRand >> 6;
#else
    return random() >> 6;
#endif
}


/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int idice;
    int sum;

    switch ( size )
    {
    case 0: return 0;
    case 1: return number;
    }

    for ( idice = 0, sum = 0; idice < number; idice++ )
	sum += number_range( 1, size );

    return sum;
}


/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_99 )
{
    return value_00 + level * (value_99 - value_00) / ( LEVEL_HERO - 1 );
}


/*
 * Set the HP/dam/mana values on a mob of a random level.
 */
void
set_mob_stats( CHAR_DATA *pMob, int difficulty )
{
    int efflevel;
    int	maxtable;

    /* find upper level in table */
    for ( maxtable = 1; hitdice_table[maxtable].level; maxtable++ )
	;

    /* adjust effective level for difficulty factor */
    switch( difficulty )
    {
	case DICE_EASY:
	    efflevel = 0 - ( pMob->level + 20 ) / 20;
	    break;
	default:
	case DICE_MEDIUM:
	    efflevel = 0;
	    break;
	case DICE_HARD:
	    efflevel = ( pMob->level + 20 ) / 20;
	    break;
    }

    /* adjust effective level for mob size */
    switch( pMob->size )
    {
	case SIZE_TINY:
	    efflevel -= 2 * ( ( 33 + pMob->level ) / 33 );
	    break;
	case SIZE_SMALL:
	    efflevel -= ( ( 33 + pMob->level ) / 33 );
	    break;
	default:
	case SIZE_MEDIUM:
	    break;
	case SIZE_LARGE:
	    efflevel += ( ( 33 + pMob->level ) / 33 );
	    break;
	case SIZE_HUGE:
	    efflevel += 2 * ( ( 33 + pMob->level ) / 33 );
	    break;
	case SIZE_GIANT:
	    efflevel += 3 * ( ( 33 + pMob->level ) / 33 );
	    break;
    }

    efflevel = URANGE( 0, pMob->level + efflevel, maxtable - 1 );

    pMob->max_hit	= dice( hitdice_table[efflevel].hit[DICE_NUMBER],
				hitdice_table[efflevel].hit[DICE_TYPE] ) +
				hitdice_table[efflevel].hit[DICE_BONUS];
    pMob->hit			= pMob->max_hit;
    pMob->hitroll		= hitdice_table[efflevel].hitroll;

    pMob->damage[DICE_NUMBER]	= hitdice_table[efflevel].dam[DICE_NUMBER];
    pMob->damage[DICE_TYPE]	= hitdice_table[efflevel].dam[DICE_TYPE];
    pMob->damroll		= hitdice_table[efflevel].dam[DICE_BONUS];

    pMob->max_mana	= dice(	hitdice_table[efflevel].mana[DICE_NUMBER],
				hitdice_table[efflevel].mana[DICE_TYPE] ) +
				hitdice_table[efflevel].mana[DICE_BONUS];
    pMob->mana			= pMob->max_mana;

    pMob->armor[AC_PIERCE]	= number_fuzzy( 10 ) +
				  hitdice_table[efflevel].ac - 10;
    pMob->armor[AC_BASH]	= number_fuzzy( 10 ) +
				  hitdice_table[efflevel].ac - 10;
    pMob->armor[AC_SLASH]	= number_fuzzy( 10 ) +
				  hitdice_table[efflevel].ac - 10;
    pMob->armor[AC_EXOTIC]	= number_fuzzy( 10 ) +
				  hitdice_table[efflevel].ac - 10;

    return;
}


/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
	if ( *str == '~' )
	    *str = '-';
    }

    return;
}


/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
	bug( "Str_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( bstr == NULL )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
    {
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    }

    return FALSE;
}


/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr is a prefix of arg and arg is a prefix of bstr.
 * Note this is the opposite sense of str_prefix() and str_cmp();
 * i.e., returns TRUE on match.
 */
bool
str_match( const char *arg, const char *astr, const char *bstr )
{
    if ( IS_NULLSTR( arg )
    ||	 IS_NULLSTR( astr )
    ||	 IS_NULLSTR( bstr ) )
	return FALSE;

    if ( str_prefix( astr, arg ) )
	return FALSE;

    return !str_prefix( arg, bstr );
}


/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
	bug( "Strn_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( bstr == NULL )
    {
	bug( "Strn_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr; astr++, bstr++ )
    {
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    }

    return FALSE;
}


/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;
    int ichar;
    char c0;

    if ( ( c0 = LOWER(astr[0]) ) == '\0' )
	return FALSE;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);

    for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
    {
	if ( c0 == LOWER(bstr[ichar]) && !str_prefix( astr, bstr + ichar ) )
	    return FALSE;
    }

    return TRUE;
}


/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
	return FALSE;
    else
	return TRUE;
}


/*
 * Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = UPPER(strcap[0]);
    return strcap;
}


/*
 * Returns TRUE or FALSE if a letter is a vowel			-Thoric
 */
bool isavowel( char letter )
{
    char c;

    c = LOWER( letter );
    if ( c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' )
	return TRUE;
    else
	return FALSE;
}


/*
 * Shove either "a " or "an " onto the beginning of a string	-Thoric
 * --unless it already starts with "a", "an", or "the"		-Mac/Nibios
 */
const char *
aoran( const char *str )
{
    static char temp[MAX_STRING_LENGTH];
    char	buf[MAX_STRING_LENGTH];

    if ( !str )
    {
	bug( "Aoran(): NULL str", 0 );
	return "";
    }

    one_argument( str, buf );
    if (   !str_cmp( buf, "a" )
	|| !str_cmp( buf, "an" )
	|| !str_cmp( buf, "the" ) )
    {
	return str;
    }

    if ( isavowel(str[0] )
    || ( strlen(str) > 1 && LOWER(str[0]) == 'y' && !isavowel(str[1])) )
	strcpy( temp, "an " );
    else
	strcpy( temp, "a " );
    strcat( temp, str );
    return temp;
}


const char *
aoran_skip( const char *str )
{
    char	buf[MAX_STRING_LENGTH];
    int		offset;

    if ( !str )
    {
	bug( "Aoran(): NULL str", 0 );
	return "";
    }

    one_argument( str, buf );
	 if ( !str_cmp( buf, "a" ) )	offset = 2;
    else if ( !str_cmp( buf, "an" ) )	offset = 3;
    else if ( !str_cmp( buf, "the" ) )	offset = 4;
    else				offset = 0;

    return str + offset;
}


/*
 * Append a string to a file.
 */
void
append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;

    if ( IS_NPC(ch) || str[0] == '\0' )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
	perror( file );
	send_to_char( "Could not open the file!\n\r", ch );
    }
    else
    {
	fprintf( fp, "[%5d] %s: %s\n",
	    ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Reports a bug.
 */
void
bug( const char *str, int param )
{
    char buf[MAX_STRING_LENGTH];

    if ( fpArea != NULL )
    {
	int iLine;
	int iChar;

	if ( fpArea == stdin )
	{
	    iLine = 0;
	}
	else
	{
	    iChar = ftell( fpArea );
	    fseek( fpArea, 0, 0 );
	    for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
	    {
		while ( getc( fpArea ) != '\n' )
		    ;
	    }
	    fseek( fpArea, iChar, 0 );
	}

	sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
	log_string( buf );
/* RT removed because we don't want bugs shutting the mud 
	if ( ( fp = fopen( "shutdown.txt", "a" ) ) != NULL )
	{
	    fprintf( fp, "[*****] %s\n", buf );
	    fclose( fp );
	}
*/
    }

    strcpy( buf, "[*****] BUG: " );
    sprintf( buf + strlen(buf), str, param );
    log_string( buf );
    wiznet( buf, NULL, NULL, WIZ_BUG, 0, 0 );
/* RT removed due to bug-file spamming 
    fclose( fpReserve );
    if ( ( fp = fopen( BUG_FILE, "a" ) ) != NULL )
    {
	fprintf( fp, "%s\n", buf );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
*/

    return;
}


/*
 * Formatted bug output
 */
void
bugf( const char * fmt, ... )
{
    char buf[MAX_STRING_LENGTH];
    va_list args;

    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    va_end( args );

    bug( buf, 0 );
}

void logf2 (const char * fmt, ...)                 
{            
        char buf [2*MSL];     
        va_list args;                
        va_start (args, fmt);     
        vsprintf (buf, fmt, args);     
        va_end (args);     

        log_string (buf);           
} 

/*
 * Report a building bug
 */
void
buildbug( const char *fmt, ... )
{
    va_list args;
    char buf[MAX_STRING_LENGTH];
    char *p;

    p = stpcpy( buf, "[*****] BUILD: " );
    va_start( args, fmt );
    vsnprintf( p, sizeof( buf ) - ( p - buf ), fmt, args );
    va_end( args );

    log_string( buf );
    wiznet( buf, NULL, NULL, WIZ_BUILDBUG, 0, 0 );
}


/*
 * Writes a string to the log.
 */
void
log_string( const char *str )
{
    char *strtime;

    strtime                    = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    fprintf( stderr, "%s :: %s\n", strtime, str );
    return;
}


/*
 * Writes a formatted string to the log.
 */
void
log_printf( const char * fmt, ... )
{
    char buf	[2*MAX_STRING_LENGTH];
    va_list	args;

    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    va_end( args );

    log_string( buf );
}


/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain( void )
{
    return;
}

/*
 * Trying this to see if the coder is full of shit.
 * and they probably are. Or at least, they are now
 * but maybe they were right back like a decade or two
 * ago. lol. We shall see.
 */
 void do_arealinks(CHAR_DATA *ch, char *argument)
{
    FILE *fp;
    BUFFER *buffer;
    AREA_DATA *parea;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *from_room;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int vnum = 0;
    int iHash, door;
    bool found = FALSE;

    /* To provide a convenient way to translate door numbers to words */
    static char * const dir_name[] = {"north","east","south","west","up","down"};

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    /* First, the 'all' option */
    if (!str_cmp(arg1,"all"))
    {
	/*
	 * If a filename was provided, try to open it for writing
	 * If that fails, just spit output to the screen.
	 */
	if (arg2[0] != '\0')
	{
	    fclose(fpReserve);
	    if( (fp = fopen(arg2, "w")) == NULL)
	    {
		send_to_char("Error opening file, printing to screen.\n\r",ch);
		fclose(fp);
		fpReserve = fopen(NULL_FILE, "r");
		fp = NULL;
	    }
	}
	else
	    fp = NULL;

	/* Open a buffer if it's to be output to the screen */
	if (!fp)
	    buffer = new_buf();

	/* Loop through all the areas */
	for (parea = area_first; parea != NULL; parea = parea->next)
	{
	    /* First things, add area name  and vnums to the buffer */
	    sprintf(buf, "*** %s (%d to %d) ***\n\r",
			 parea->name, parea->min_vnum, parea->max_vnum);
	    fp ? fprintf(fp, buf) : add_buf(buffer, buf);

	    /* Now let's start looping through all the rooms. */
	    found = FALSE;
	    for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	    {
		for( from_room = room_index_hash[iHash];
		     from_room != NULL;
		     from_room = from_room->next )
		{
		    /*
		     * If the room isn't in the current area,
		     * then skip it, not interested.
		     */
		    if ( from_room->vnum < parea->min_vnum
		    ||   from_room->vnum > parea->max_vnum )
			continue;

		    /* Aha, room is in the area, lets check all directions */
		    for (door = 0; door < 5; door++)
		    {
			/* Does an exit exist in this direction? */
			if( (pexit = from_room->exit[door]) != NULL )
			{
			    to_room = pexit->to_room;

			    /*
			     * If the exit links to a different area
			     * then add it to the buffer/file
			     */
			    if( to_room != NULL
			    &&  (to_room->vnum < parea->min_vnum
			    ||   to_room->vnum > parea->max_vnum) )
			    {
				found = TRUE;
				sprintf(buf, "    (%d) links %s to %s (%d)\n\r",
				    from_room->vnum, dir_name[door],
				    to_room->area->name, to_room->vnum);

				/* Add to either buffer or file */
				if(fp == NULL)
				    add_buf(buffer, buf);
				else
				    fprintf(fp, buf);
			    }
			}
		    }
		}
	    }

	    /* Informative message for areas with no external links */
	    if (!found)
		add_buf(buffer, "    No links to other areas found.\n\r");
	}

	/* Send the buffer to the player */
	if (!fp)
	{
	    page_to_char(buf_string(buffer), ch);
	    free_buf(buffer);
	}
	/* Or just clean up file stuff */
	else
	{
	    fclose(fp);
	    fpReserve = fopen(NULL_FILE, "r");
	}

	return;
    }

    /* No argument, let's grab the char's current area */
    if(arg1[0] == '\0')
    {
	parea = ch->in_room ? ch->in_room->area : NULL;

	/* In case something wierd is going on, bail */
	if (parea == NULL)
	{
	    send_to_char("You aren't in an area right now, funky.\n\r",ch);
	    return;
	}
    }
    /* Room vnum provided, so lets go find the area it belongs to */
    else if(is_number(arg1))
    {
	vnum = atoi(arg1);

	/* Hah! No funny vnums! I saw you trying to break it... */
	if (vnum <= 0 || vnum > 65536)
	{
	    send_to_char("The vnum must be between 1 and 65536.\n\r",ch);
	    return;
	}

	/* Search the areas for the appropriate vnum range */
	for (parea = area_first; parea != NULL; parea = parea->next)
	{
	    if(vnum >= parea->min_vnum && vnum <= parea->max_vnum)
		break;
	}

	/* Whoops, vnum not contained in any area */
	if (parea == NULL)
	{
	    send_to_char("There is no area containing that vnum.\n\r",ch);
	    return;
	}
    }
    /* Non-number argument, must be trying for an area name */
    else
    {
	/* Loop the areas, compare the name to argument */
	for(parea = area_first; parea != NULL; parea = parea->next)
	{
	    if(!str_prefix(arg1, parea->name))
		break;
	}

	/* Sorry chum, you picked a goofy name */
	if (parea == NULL)
	{
	    send_to_char("There is no such area.\n\r",ch);
	    return;
	}
    }

    /* Just like in all, trying to fix up the file if provided */
    if (arg2[0] != '\0')
    {
	fclose(fpReserve);
	if( (fp = fopen(arg2, "w")) == NULL)
	{
	    send_to_char("Error opening file, printing to screen.\n\r",ch);
	    fclose(fp);
	    fpReserve = fopen(NULL_FILE, "r");
	    fp = NULL;
	}
    }
    else
	fp = NULL;

    /* And we loop the rooms */
    for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
    {
	for( from_room = room_index_hash[iHash];
	     from_room != NULL;
	     from_room = from_room->next )
	{
	    /* Gotta make sure the room belongs to the desired area */
	    if ( from_room->vnum < parea->min_vnum
	    ||   from_room->vnum > parea->max_vnum )
		continue;

	    /* Room's good, let's check all the directions for exits */
	    for (door = 0; door < 5; door++)
	    {
		if( (pexit = from_room->exit[door]) != NULL )
		{
		    to_room = pexit->to_room;

		    /* Found an exit, does it lead to a different area? */
		    if( to_room != NULL
		    &&  (to_room->vnum < parea->min_vnum
		    ||   to_room->vnum > parea->max_vnum) )
		    {
			found = TRUE;
			sprintf(buf, "%s (%d) links %s to %s (%d)\n\r",
				    parea->name, from_room->vnum, dir_name[door],
				    to_room->area->name, to_room->vnum);

			/* File or buffer output? */
			if(fp == NULL)
			    send_to_char(buf, ch);
			else
			    fprintf(fp, buf);
		    }
		}
	    }
	}
    }

    /* Informative message telling you it's not externally linked */
    if(!found)
    {
	send_to_char("No links to other areas found.\n\r",ch);
	/* Let's just delete the file if no links found */
	if (fp)
	    unlink(arg2);
	return;
    }

    /* Close up and clean up file stuff */
    if(fp)
    {
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
    }

}

