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


#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"


char *
print_flags( bitvector flag )
{
    int		count;
    int		pos;
    static char buf[52];


    pos = 0;
    for ( count = 0; count < 32;  count++ )
    {
        if ( IS_SET( flag, 1<<count ) )
        {
            if ( count < 26 )
                buf[pos] = 'A' + count;
            else
                buf[pos] = 'a' + ( count - 26 );
            pos++;
        }
    }

    if ( pos == 0 )
    {
        buf[pos] = '0';
        pos++;
    }

    buf[pos] = '\0';

    return buf;
}


/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];



/*
 * Local functions.
 */
void	fwrite_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fwrite_obj	args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
			    FILE *fp, int iNest ) );
void	fwrite_pet	args( ( CHAR_DATA *pet, FILE *fp, bool fMount ) );
void	fread_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fread_pet	args( ( CHAR_DATA *ch,  FILE *fp, bool fMount ) );
void	fread_obj	args( ( CHAR_DATA *ch,  FILE *fp ) );


/*
 * Save the donation pit. I fucking hope I don't screw the 
 * the pooch writing my very first function ever.
 * We'll see.
 */

 void
 save_donation_pit( CHAR_DATA *ch, char *vnum )
 {
 	FILE *fp;
 	char filename[256];
 	sh_int templvl;
 	OBJ_DATA *contents;

 	if ( !vnum )
 	{
 		bug( "save_donation_pit: Null vnum pointer!", 0);
 		return;
 	}

 	if ( !ch )
 	{
 		bug ("save_donation_pit: Null ch pointer!", 0);
 		return;
 	}

 		sprintf( filename, "%s%s.vault", VAULT_DIR, vnum );
 	if ( ( fp = fopen( filename, "w" ) ) == NULL )
 	{
 		bug( "save_donation_pit: fopen", 0 );
 		perror( filename );
 	}
 	else
 	{
 		templvl = ch->level;
 		ch->level = LEVEL_HERO;  /* make sure EQ doesn't get lost */
		contents = ch->in_room->contents;
        	if (contents)
          		fwrite_obj(ch, contents, fp, 0);
        	fprintf( fp, "#END\n" );
        	ch->level = templvl;
		fclose( fp );
		return;
	}
	return;
}
/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void
save_char_obj( CHAR_DATA *ch )
{
    char	strsave[MAX_INPUT_LENGTH];
    FILE *	fp;
    BANK_DATA *	bank;

    if ( IS_NPC(ch) )
	return;

    if ( ch->desc != NULL && ch->desc->original != NULL )
	ch = ch->desc->original;

    /* create god log */
    if (IS_IMMORTAL(ch) || ch->level >= LEVEL_IMMORTAL)
    {
	fclose(fpReserve);
	sprintf(strsave, "%s%s",GOD_DIR, capitalize(ch->name));
	if ((fp = fopen(strsave,"w")) == NULL)
	{
	    bug("Save_char_obj: fopen",0);
	    perror(strsave);
 	}

	fprintf(fp,"Lev %2d Trust %2d  %s%s\n",
	    ch->level, get_trust(ch), ch->name, ch->pcdata->title);
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
    }

    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
    {
	bug( "Save_char_obj: fopen", 0 );
	perror( strsave );
    }
    else
    {
	fwrite_char( ch, fp );
	if ( ch->carrying != NULL )
	    fwrite_obj( ch, ch->carrying, fp, 0 );

	/* save the pets */
	if ( ch->pet != NULL && ch->pet->in_room == ch->in_room )
	    fwrite_pet( ch->pet, fp, FALSE );

	if (ch->mount != NULL && ch->mount->in_room == ch->in_room )
	    fwrite_pet( ch->mount, fp, TRUE );

	for ( bank = ch->pcdata->bank; bank != NULL; bank = bank->next )
	    if ( bank->storage != NULL )
		fwrite_obj( ch, bank->storage, fp, 0 );
	fprintf( fp, "#END\n" );
    }
    fclose( fp );
    rename( TEMP_FILE, strsave );
    fpReserve = fopen( NULL_FILE, "r" );
    sql_save_char( ch );
    return;
}



/*
 * Write the char.
 */
void
fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    AFFECT_DATA *	paf;
    ALIAS_DATA *	alias;
    BANK_DATA *		bank;
    IGNORE_DATA *	pId;
    BOARD_INFO *	pInfo;
    int			i;
    int			sn;
    int			gn;
    int			trst;
    bitvector 		actflags;

    fprintf( fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER"	);

    fprintf( fp, "Name %s~\n",	ch->name		);
    fprintf( fp, "Id   %ld\n", ch->id			);
    fprintf( fp, "LogO %ld\n",	current_time		);
    fprintf( fp, "Vers %d\n",   7			);
    if (ch->short_descr[0] != '\0')
      	fprintf( fp, "ShD  %s~\n",	ch->short_descr	);
    if( ch->long_descr[0] != '\0')
	fprintf( fp, "LnD  %s~\n",	ch->long_descr	);
    if (ch->description[0] != '\0')
    	fprintf( fp, "Desc %s~\n",	fix_string( ch->description ) );
    if (ch->prompt != NULL || !str_cmp(ch->prompt,"<%hhp %mm %vmv> "))
        fprintf( fp, "Prom %s~\n",      ch->prompt  	);
		fprintf( fp, "Tpld %d\n", ch->pcdata->timesplayed);
    fprintf( fp, "Race %s~\n", race_table[ch->race].name );
    if ( ch->clan )
    {
    	fprintf( fp, "Clan %s\n", ch->clan->name );
    	if ( ch->clvl )
    	    fprintf( fp, "Clvl %d\n", ch->clvl );
    }
    fprintf( fp, "Sex  %d\n",	   ch->sex		);
    fprintf( fp, "Cla  '%s'\n",	   class_table[ch->class].name	);
    fprintf( fp, "Levl %s\n",	   level_name( IS_AVATAR( ch ) ? ch->pcdata->avatar->level : ch->level ) );

    trst = IS_AVATAR( ch ) ? ch->pcdata->avatar->trust : ch->trust;
    if ( trst != 0 )
	fprintf( fp, "Tru  %d\n",	trst	);
    fprintf( fp, "Plyd %d\n",
	ch->played + (int) (current_time - ch->logon)	);
    fprintf( fp, "Scro %d\n", 	ch->lines		);
    fprintf( fp, "Room %d\n",
        (  ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
	&& ch->was_in_room != 0 )
            ? ch->was_in_room
            : ch->in_room == NULL ? ROOM_VNUM_TEMPLE : ch->in_room->vnum );

    fprintf( fp, "Size %s\n", size_name( ch->size ) );
    if ( IS_AVATAR( ch ) )
        fprintf( fp, "HMV  %d %d %d %d %d %d\n",
            ch->pcdata->avatar->hit, ch->pcdata->avatar->max_hit, 
            ch->pcdata->avatar->mana, ch->pcdata->avatar->max_mana,
            ch->pcdata->avatar->move, ch->pcdata->avatar->max_move );
    else
        fprintf( fp, "HMV  %d %d %d %d %d %d\n",
	    ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );

    if ( ch->money.gold > 0 )
	fprintf( fp, "Gold %d\n",	ch->money.gold	 );
    if ( ch->money.silver  > 0 )
	fprintf( fp, "Silv %d\n",	ch->money.silver );
    if ( ch->money.copper > 0 )
	fprintf( fp, "Copper %d\n",	ch->money.copper );
    if ( ch->money.fract >0 )
	fprintf( fp, "Fract %d\n",	ch->money.fract  );

    fprintf( fp, "Exp  %d\n",	ch->exp			 );

    actflags = ch->act & ~PLR_SWITCHED;
    if ( IS_AVATAR( ch ) )
        actflags = ( actflags & ~PLR_HOLYLIGHT ) | ch->pcdata->avatar->holylight;
    if ( actflags != 0 )
	fprintf( fp, "Act %s\n",    print_flags( actflags ) );

    if ( ch->act2 != 0 )
	fprintf( fp, "Act2 %s\n",   print_flags( ch->act2 & ~( PLR_PLOADED | PLR_MARKING ) ) );
    if ( !xIS_EMPTY( ch->affected_by ) )
	fprintf( fp, "AfBy %s\n",   print_xbits( &ch->affected_by ) );
    if ( !xIS_EMPTY( ch->shielded_by ) )
	fprintf( fp, "ShBy %s\n",   print_xbits( &ch->shielded_by ) );
    if ( ch->deaf )
	fprintf( fp, "Deaf %s\n",   print_flags( ch->deaf ) );
    fprintf( fp, "Comm %s\n",       print_flags(ch->comm));
    if ( ch->info != 0 )
	fprintf( fp, "Info %s\n",   print_flags( ch->info ) );
    if (ch->wiznet)
    	fprintf( fp, "Wizn %s\n",   print_flags(ch->wiznet));
    if ( !IS_NPC( ch ) )
    {
	fprintf( fp, "LogC %d\n", ch->pcdata->logcount );
	fprintf( fp, "Security %d\n", ch->pcdata->security );
	if ( ch->pcdata->beacon != 0 )
	    fprintf( fp, "Beacon %d\n", ch->pcdata->beacon );
	fprintf( fp, "ClrMap  " );
	for ( i = 1; i < MAX_CLRMAP; i++ )
	    fprintf( fp, " %d", ch->pcdata->colormap[i] );
	fprintf( fp, "\n" ); 
	if ( ch->pcdata->greet_id != 0 )
	    fprintf( fp, "Gid %d\n", ch->pcdata->greet_id );
        if ( ch->pcdata->outcast != 0 )
            fprintf( fp, "Outcast %d\n", ch->pcdata->outcast );
        for ( pId = ch->pcdata->ignoring; pId != NULL; pId = pId->next )
            fprintf( fp, "Ignor %s %s\n", pId->name, print_flags( pId->flags ) );
    }
    if (ch->invis_level)
	fprintf( fp, "Invi %d\n", 	ch->invis_level	);
    if (ch->incog_level)
	fprintf(fp,"Inco %d\n",ch->incog_level);
    fprintf( fp, "Pos  %s\n",
	position_table[ch->position == POS_FIGHTING ? POS_STANDING : ch->position].short_name );
    if (ch->practice != 0)
    	fprintf( fp, "Prac %d\n",	ch->practice	);
    if (ch->train != 0)
	fprintf( fp, "Trai %d\n",	ch->train	);
    if (ch->saving_throw != 0)
	fprintf( fp, "Save  %d\n",	ch->saving_throw);
    fprintf( fp, "Ethos  %d\n",	ch->ethos		);
    fprintf( fp, "Alig  %d\n",	ch->alignment		);
    if (ch->hitroll != 0)
	fprintf( fp, "Hit   %d\n",	ch->hitroll	);
    if (ch->damroll != 0)
	fprintf( fp, "Dam   %d\n",	ch->damroll	);
    fprintf( fp, "ACs %d %d %d %d\n",	
	ch->armor[0],ch->armor[1],ch->armor[2],ch->armor[3]);
    if (ch->wimpy !=0 )
	fprintf( fp, "Wimp  %d\n",	ch->wimpy	);
    if ( ch->questpoints )
	fprintf( fp, "QPts %d\n", ch->questpoints );
    if ( ch->nextquest )
    {
	fprintf( fp, "QNext %ld\n", current_time + 60 * ch->nextquest );
	fprintf( fp, "QNone %d\n", ch->questnone );
    }
    else if ( ch->countdown )
	fprintf( fp, "QNext %ld\n", current_time + ch->countdown * 60 );
    fprintf( fp, "Attr %d %d %d %d %d\n",
	ch->perm_stat[STAT_STR],
	ch->perm_stat[STAT_INT],
	ch->perm_stat[STAT_WIS],
	ch->perm_stat[STAT_DEX],
	ch->perm_stat[STAT_CON] );

    fprintf (fp, "AMod %d %d %d %d %d\n",
	ch->mod_stat[STAT_STR],
	ch->mod_stat[STAT_INT],
	ch->mod_stat[STAT_WIS],
	ch->mod_stat[STAT_DEX],
	ch->mod_stat[STAT_CON] );

    if ( IS_NPC(ch) )
    {
	fprintf( fp, "Vnum %d\n",	ch->pIndexData->vnum	);
    }
    else
    {
	fprintf( fp, "Pass %s~\n",	ch->pcdata->pwd		);
	if ( ch->pcdata->sec_groups != 0 )
	    fprintf( fp, "SecGrp %s\n", print_flags( ch->pcdata->sec_groups ) );
	if ( ch->pcdata->last_medit != 0 && IS_IMMORTAL( ch ) )
	    fprintf( fp, "LastMob %d\n", ch->pcdata->last_medit );
	if ( ch->pcdata->last_oedit != 0 && IS_IMMORTAL( ch ) )
	    fprintf( fp, "LastObj %d\n", ch->pcdata->last_oedit );
	if ( ch->pcdata->bamfin[0] != '\0' && strlen_wo_col ( ch->pcdata->bamfin ) < 79 )
	    fprintf( fp, "Bin  %s~\n",	ch->pcdata->bamfin);
	if (ch->pcdata->bamfout[0] != '\0' && strlen_wo_col( ch->pcdata->bamfout ) < 79 )
		fprintf( fp, "Bout %s~\n",	ch->pcdata->bamfout);
	if ( *ch->pcdata->pose != '\0' )
	    fprintf( fp, "Pose %s~\n",		ch->pcdata->pose );
	fprintf( fp, "Titl %s~\n",	ch->pcdata->title	);
	if ( !IS_NULLSTR( ch->pcdata->lname ) )
	    fprintf( fp, "Lnm %s~\n", ch->pcdata->lname );
	if ( ch->pcdata->who_text && *ch->pcdata->who_text != '\0' )
	    fprintf( fp, "WhoTxt %s~\n",	ch->pcdata->who_text );
    	fprintf( fp, "HairC %s~\n", ch->pcdata->hair_color );
    	fprintf( fp, "HairA %s~\n", ch->pcdata->hair_adj );
    	fprintf( fp, "EyeC %s~\n", ch->pcdata->eye_color );
    	fprintf( fp, "EyeA %s~\n", ch->pcdata->eye_adj );
    	if ( IS_IMMORTAL( ch ) && !IS_NULLSTR( ch->pcdata->empower ) )
    	    fprintf( fp, "Empw %s~\n", ch->pcdata->empower );
    	if ( IS_IMMORTAL( ch ) && !IS_NULLSTR( ch->pcdata->detract ) )
    	    fprintf( fp, "Detr %s~\n", ch->pcdata->detract );
    	fprintf( fp, "Pnts %d\n",   	ch->pcdata->points      );
	fprintf( fp, "TSex %d\n",	ch->pcdata->true_sex	);
	fprintf( fp, "LLev %d\n",	ch->pcdata->last_level	);
	fprintf( fp, "HMVP %d %d %d\n", ch->pcdata->perm_hit, 
						   ch->pcdata->perm_mana,
						   ch->pcdata->perm_move);
	fprintf( fp, "Cnds  %d %d %d %d %d %d %d %d\n",
	    ch->pcdata->condition[0],
	    ch->pcdata->condition[1],
	    ch->pcdata->condition[2],
	    ch->pcdata->condition[3],
	    ch->pcdata->condition[4],
	    ch->pcdata->condition[5],
	    ch->pcdata->condition[6],
	    ch->pcdata->condition[7] );

        if ( IS_DEAD( ch ) )
        {
            fprintf( fp, "DeadS %s\n", flag_string( death_types, ch->pcdata->deathstate ) );
            fprintf( fp, "DeadR %d\n", ch->pcdata->deathroom );
        }

        if ( ch->pcdata->kingdom != 0 )
            fprintf( fp, "Kngdm %d\n", ch->pcdata->kingdom );

        if ( ch->pcdata->forcedream[1] != 0 )
            fprintf( fp, "Fdrm %d\n", ch->pcdata->forcedream[1] );
        if ( ch->pcdata->lastdream[0] != 0 )
            fprintf( fp, "Fdrm %d\n", ch->pcdata->forcedream[0] );
        if ( ch->pcdata->lastdream[1] != 0 )
            fprintf( fp, "Ldrm %d\n", ch->pcdata->lastdream[1] );
        if ( ch->pcdata->lastdream[0] != 0 )
            fprintf( fp, "Ldrm %d\n", ch->pcdata->lastdream[0] );
	for ( alias = ch->pcdata->alias_list; alias != NULL; alias = alias->next )
	    fprintf( fp, "Alias %s %s~\n", alias->old, alias->new );

	for ( bank = ch->pcdata->bank; bank != NULL; bank = bank->next )
	{
	    if ( bank->vnum != 0
	    && ( bank->acct.gold != 0
	    ||	 bank->acct.silver != 0
	    ||	 bank->acct.copper != 0
	    ||	 bank->acct.fract != 0
	    ||	 bank->storage != NULL ) )
	    {
		fprintf( fp, "Bank %d %d %d %d %d\n",
			 bank->vnum,
			 bank->acct.gold,
			 bank->acct.silver,
			 bank->acct.copper,
			 bank->acct.fract );
	    }
	}

	for ( pInfo = ch->pcdata->last_read; pInfo != NULL; pInfo = pInfo->next )
	{
	    if ( board_lookup( pInfo->name ) != NULL && pInfo->time != 0 )
		fprintf( fp, "Brd %s %ld\n", pInfo->name, pInfo->time );
	}

	for ( sn = 0; sn < top_skill; sn++ )
	{
	    if ( skill_table[sn].name != NULL && ch->pcdata->skill[sn].percent > 0 )
	    {
		fprintf( fp, "Skl %d %d %d %d'%s'\n",
		    ch->pcdata->skill[sn].percent,
		    ch->pcdata->skill[sn].usage,
		    ch->pcdata->skill[sn].learned_age,
		    ch->pcdata->skill[sn].used_age,
		    skill_table[sn].name );
	    }
	}

	for ( gn = 0; gn < MAX_GROUP; gn++ )
        {
            if ( group_table[gn].name != NULL && ch->pcdata->group_known[gn])
            {
                fprintf( fp, "Gr '%s'\n",group_table[gn].name);
            }
        }
    }

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
	if (paf->type < 0 || paf->type>= top_skill)
	    continue;

	fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
	    skill_table[paf->type].name,
	    paf->where,
	    paf->level,
	    paf->duration,
	    paf->modifier,
	    paf->location,
	    paf->bitvector
	    );
    }

    fprintf( fp, "End\n\n" );
    return;
}


/* write a pet */
void
fwrite_pet( CHAR_DATA *pet, FILE *fp, bool fMount )
{
    AFFECT_DATA *paf;

    if ( fMount )
	fprintf( fp, "#MOUNT\n" );
    else
	fprintf( fp, "#PET\n" );

    fprintf( fp, "Vnum %d\n", pet->pIndexData->vnum );

    fprintf(fp,"Name %s~\n", pet->name);
    fprintf(fp,"LogO %ld\n", current_time);
    if (pet->short_descr != pet->pIndexData->short_descr)
    	fprintf(fp,"ShD  %s~\n", pet->short_descr);
    if (pet->long_descr != pet->pIndexData->long_descr)
    	fprintf(fp,"LnD  %s~\n", pet->long_descr);
    if (pet->description != pet->pIndexData->description)
    	fprintf(fp,"Desc %s~\n", fix_string( pet->description ) );
    if (pet->race != pet->pIndexData->race)
    	fprintf(fp,"Race %s~\n", race_table[pet->race].name);
    if (pet->clan)
        fprintf( fp, "Clan %s~\n",pet->clan->name );
    fprintf(fp,"Sex  %d\n", pet->sex);
    if (pet->level != pet->pIndexData->level)
    	fprintf(fp,"Levl %d\n", pet->level);
    fprintf(fp, "HMV  %d %d %d %d %d %d\n",
    	pet->hit, pet->max_hit, pet->mana, pet->max_mana, pet->move, pet->max_move);

    if ( pet->money.gold > 0 )
	fprintf( fp, "Gold %d\n", pet->money.gold );
    if ( pet->money.silver > 0 )
	fprintf( fp, "Silv %d\n", pet->money.silver );
    if ( pet->money.copper > 0 )
	fprintf( fp, "Copper %d\n", pet->money.copper );
    if ( pet->money.fract > 0 )
	fprintf( fp, "Fract %d\n", pet->money.fract );

    if (pet->exp > 0)
    	fprintf(fp, "Exp  %d\n", pet->exp);
    if (pet->act != pet->pIndexData->act)
    	fprintf(fp, "Act  %s\n", print_flags(pet->act));
    if ( !xSAME_BITS( pet->affected_by, pet->pIndexData->affected_by ) )
    	fprintf( fp, "AfBy %s\n", print_xbits( &pet->affected_by ) );
    if ( !xIS_EMPTY( pet->shielded_by ) )
	fprintf( fp, "ShBy %s\n",   print_xbits( &pet->shielded_by ) );
    if ( pet->deaf )
	fprintf( fp, "Deaf %s\n", print_flags( pet->deaf ) );
    if (pet->comm != 0)
    	fprintf(fp, "Comm %s\n", print_flags(pet->comm));
    fprintf(fp,"Pos  %s\n",
        position_table[pet->position = POS_FIGHTING ? POS_STANDING : pet->position].short_name );
    if (pet->saving_throw != 0)
    	fprintf(fp, "Save %d\n", pet->saving_throw);
    if (pet->alignment != pet->pIndexData->alignment)
    	fprintf(fp, "Alig %d\n", pet->alignment);
    if (pet->hitroll != pet->pIndexData->hitroll)
    	fprintf(fp, "Hit  %d\n", pet->hitroll);
    if ( pet->size != pet->pIndexData->size )
	fprintf( fp, "Size %s\n", size_name( pet->size ) );
    if (pet->damroll != pet->pIndexData->damage[DICE_BONUS])
    	fprintf(fp, "Dam  %d\n", pet->damroll);
    fprintf(fp, "ACs  %d %d %d %d\n",
    	pet->armor[0],pet->armor[1],pet->armor[2],pet->armor[3]);
    fprintf(fp, "Attr %d %d %d %d %d\n",
    	pet->perm_stat[STAT_STR], pet->perm_stat[STAT_INT],
    	pet->perm_stat[STAT_WIS], pet->perm_stat[STAT_DEX],
    	pet->perm_stat[STAT_CON]);
    fprintf(fp, "AMod %d %d %d %d %d\n",
    	pet->mod_stat[STAT_STR], pet->mod_stat[STAT_INT],
    	pet->mod_stat[STAT_WIS], pet->mod_stat[STAT_DEX],
    	pet->mod_stat[STAT_CON]);

    for ( paf = pet->affected; paf != NULL; paf = paf->next )
    {
    	if (paf->type < 0 || paf->type >= top_skill)
    	    continue;

    	fprintf(fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
    	    skill_table[paf->type].name,
    	    paf->where, paf->level, paf->duration, paf->modifier,paf->location,
    	    paf->bitvector);
    }

    fprintf(fp,"End\n");
    return;
}


/*
 * Write an object and its contents.
 */
void
fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
    EXTRA_DESCR_DATA *	ed;
    AFFECT_DATA *	paf;
    int			i;

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->next_content != NULL )
	fwrite_obj( ch, obj->next_content, fp, iNest );

    /*
     * Castrate storage characters.
     */
    if ( (ch->level < obj->level - 2 && obj->item_type != ITEM_CONTAINER)
    ||   obj->item_type == ITEM_KEY
    ||   (obj->item_type == ITEM_MAP && !obj->value[0]))
	return;

    fprintf( fp, "#O\n" );
    fprintf( fp, "Vnum %d\n",   obj->pIndexData->vnum        );
    if (obj->enchanted)
	fprintf( fp,"Enchanted\n");
    fprintf( fp, "Nest %d\n",	iNest	  	     );

    if ( obj->stored_in != NULL )
	fprintf( fp, "Bank %d\n", obj->stored_in->vnum );

    /* these data are only used if they do not match the defaults */

    if ( obj->name != obj->pIndexData->name)
    	fprintf( fp, "Name %s~\n",	obj->name		     );
    if ( obj->short_descr != obj->pIndexData->short_descr)
        fprintf( fp, "ShD  %s~\n",	obj->short_descr	     );
    if ( obj->description != obj->pIndexData->description)
        fprintf( fp, "Desc %s~\n",	obj->description	     );
    if ( obj->extra_flags != obj->pIndexData->extra_flags)
        fprintf( fp, "ExtFl %s\n",	print_flags( obj->extra_flags ) );
		if ( obj->extra_flags2 != obj->pIndexData->extra_flags2)
        fprintf( fp, "ExtFl2 %s\n",	print_flags( obj->extra_flags2 ) );
    if ( obj->wear_flags != obj->pIndexData->wear_flags)
        fprintf( fp, "WearF %s\n",	print_flags( obj->wear_flags ) );
    if ( obj->item_type != obj->pIndexData->item_type)
        fprintf( fp, "Ityp %d\n",	obj->item_type		     );
    if ( obj->weight != obj->pIndexData->weight)
        fprintf( fp, "Wt   %d\n",	obj->weight		     );
    if ( obj->condition != obj->pIndexData->condition)
	fprintf( fp, "Cond %d\n",	obj->condition		     );

    /* variable data */

    fprintf( fp, "Wear %d\n",   obj->wear_loc                );
    if (obj->level != obj->pIndexData->level)
        fprintf( fp, "Lev  %d\n",	obj->level		     );
    if (obj->timer != 0)
        fprintf( fp, "Time %d\n",	obj->timer	     );
    fprintf( fp, "Cost %d\n",	obj->cost		     );
	fprintf( fp, "QCost %d\n",	obj->qcost		     );

    if (obj->value[0] != obj->pIndexData->value[0]
    ||  obj->value[1] != obj->pIndexData->value[1]
    ||  obj->value[2] != obj->pIndexData->value[2]
    ||  obj->value[3] != obj->pIndexData->value[3]
    ||  obj->value[4] != obj->pIndexData->value[4]
    ||  obj->value[5] != obj->pIndexData->value[5]) 
    	fprintf( fp, "Val  %d %d %d %d %d %d\n",
	    obj->value[0], obj->value[1], obj->value[2], obj->value[3],
	    obj->value[4], obj->value[5] );

    switch ( obj->item_type )
    {
    case ITEM_HERB:
    case ITEM_POTION:
    case ITEM_SCROLL:
    case ITEM_PILL:
	if ( obj->value[1] > 0 )
	{
	    fprintf( fp, "Spell 1 '%s'\n", 
		skill_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
	    fprintf( fp, "Spell 2 '%s'\n", 
		skill_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3 '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;

    case ITEM_LENS:
    case ITEM_STAFF:
    case ITEM_WAND:
	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3 '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;
    }

    if ( obj->ac_charge[0] != obj->pIndexData->ac_charge[0]
    ||   obj->ac_charge[1] != obj->pIndexData->ac_charge[1] )
    {
	fprintf( fp, "Ichrg  %d %d\n", obj->ac_charge[0], obj->ac_charge[1] );
    }

    for ( i = 1; !IS_NULLSTR( race_table[i].name ); i++ )
        if ( xIS_SET( obj->race_flags, i ) )
            fprintf( fp, "Anti '%s'\n", race_table[i].name );
    for ( i = 0; i < MAX_CLASS; i++ )
        if ( xIS_SET( obj->class_flags, i ) )
            fprintf( fp, "Anti '%s'\n", class_table[i].name );


    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if (paf->type < 0 || paf->type >= top_skill)
	    continue;
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
            skill_table[paf->type].name,
            paf->where,
            paf->level,
            paf->duration,
            paf->modifier,
            paf->location,
            paf->bitvector
            );
    }

    for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
    {
	fprintf( fp, "ExDe %s~ %s~\n",
	    ed->keyword, ed->description );
    }

    fprintf( fp, "End\n\n" );

    if ( obj->contains != NULL )
	fwrite_obj( ch, obj->contains, fp, iNest + 1 );

    return;
}


/*
 * Load a char and inventory into a new ch structure.
 */
bool
load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
    char strsave[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *ch;
    FILE *fp;
    bool found;
    int stat;
    int i;
    int retval;	/* Useless unused variablle here only to make GCC happy */

    ch = new_char( );
    ch->pcdata = new_pcdata( );

    d->character			= ch;
    ch->desc				= d;
    ch->name				= str_dup( name );
    ch->id				= get_pc_id();
    ch->race				= race_lookup("human");
    ch->act				= PLR_AUTOEXIT
					| PLR_AUTOCOINS
					| PLR_AUTOLOOT
					| PLR_AUTOSAC
					| PLR_AUTODAMAGEDEALT
					| PLR_BLINK
					| PLR_NOSUMMON;

    ch->act2				= PLR_FMETER
					| PLR_AUTOCHANNEL
					| PLR_AUTOTICK;
    ch->deaf				= 0;
    ch->deleted				= FALSE;
    ch->comm				= COMM_COMBINE 
					| COMM_PROMPT;
    ch->info				= INFO_ON
					| INFO_DEATHS
					| INFO_LEVELS
					| INFO_LOGINS;
    ch->prompt 				= str_dup("<%hhp %mm %vmv> ");
    ch->pcdata->confirm_flags		= 0;
    ch->pcdata->sec_groups		= 0;
    for ( stat =0; stat < MAX_STATS; stat++ )
	ch->perm_stat[stat]		= 13;
    for ( i = 0; i < MAX_CLRMAP; i++ )
	ch->pcdata->colormap[i] = i;
    ch->pcdata->condition[COND_THIRST]	= MAX_COND; 
    ch->pcdata->condition[COND_FULL]	= MAX_COND;
    ch->pcdata->condition[COND_HUNGER]	= MAX_COND;
    ch->pcdata->condition[COND_TIRED]	= MAX_COND;

    found = FALSE;
    fclose( fpReserve );

    /* decompress if .gz file exists */
    sprintf( strsave, "%s%s%s", PLAYER_DIR, capitalize(name),".gz");
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	fclose( fp );
	sprintf( buf, "gzip -dfq %s", strsave );
	retval = system( buf );
    }

    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if      ( !str_cmp( word, "PLAYER" ) ) fread_char ( ch, fp );
	    else if ( !str_cmp( word, "MOUNT"  ) ) fread_pet  ( ch, fp, TRUE );
	    else if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( ch, fp );
	    else if ( !str_cmp( word, "O"      ) ) fread_obj  ( ch, fp );
	    else if ( !str_cmp( word, "PET"    ) ) fread_pet  ( ch, fp, FALSE );
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	}
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );


    /* initialize race */
    if (found)
    {
	int i;

	if (ch->race == 0)
	    ch->race = race_lookup("human");

	if ( !race_table[ch->race].pc_race )
	{
	    bugf( "Load_char_obj: bad race for %s: %s", ch->name,
		   race_table[ch->race].name );
	    ch->race = race_lookup( "Human" );
	}

	if ( ch->version < 7 )
	    ch->size = race_table[ch->race].size;

	ch->dam_type = 17; /*punch */
	for (i = 0; i < MAX_RACE_SKILLS; i++)
	{
	    if (race_table[ch->race].skills[i] == NULL)
		break;
	    group_add(ch,race_table[ch->race].skills[i],FALSE);
	}
	xSET_BITS( ch->affected_by, race_table[ch->race].aff );
	ch->imm_flags	= ch->imm_flags | race_table[ch->race].imm;
	ch->res_flags	= ch->res_flags | race_table[ch->race].res;
	ch->vuln_flags	= ch->vuln_flags | race_table[ch->race].vuln;
	ch->form	= race_table[ch->race].form;
	ch->parts	= race_table[ch->race].parts;
    }

    /* RT initialize skills */

    if (found && ch->version < 2)  /* need to add the new skills */
    {
	group_add(ch,"rom basics",FALSE);
	group_add(ch,class_table[ch->class].base_group,FALSE);
	group_add(ch,class_table[ch->class].default_group,TRUE);
	ch->pcdata->skill[gsn_recall].percent = 50;
    }

    /* fix levels */
    if (found && ch->version < 3 && (ch->level > 35 || ch->trust > 35))
    {
	switch (ch->level)
	{
	    case(40) : ch->level = 60;	break;  /* imp -> imp */
	    case(39) : ch->level = 58; 	break;	/* god -> supreme */
	    case(38) : ch->level = 56;  break;	/* deity -> god */
	    case(37) : ch->level = 53;  break;	/* angel -> demigod */
	}

        switch (ch->trust)
        {
            case(40) : ch->trust = 60;  break;	/* imp -> imp */
            case(39) : ch->trust = 58;  break;	/* god -> supreme */
            case(38) : ch->trust = 56;  break;	/* deity -> god */
            case(37) : ch->trust = 53;  break;	/* angel -> demigod */
            case(36) : ch->trust = 51;  break;	/* hero -> hero */
        }
    }
    if ( found && ch->version < 6 )
	SET_BIT( ch->act, PLR_BLINK );

    if ( found && !IS_NPC( ch ) && IS_SET( ch->act2, PLR_KEEPALIVE ) )
        add_keepalive( ch );
    return found;
}


/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

/* provided to free strings */
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    free_string(field);			\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

void
fread_char( CHAR_DATA *ch, FILE *fp )
{
    char		buf[MAX_STRING_LENGTH];
    ALIAS_DATA *	alias_last;
    ALIAS_DATA *	alias_new;
    BANK_DATA *		bank;
    BANK_DATA *		bank_last = NULL;
    IGNORE_DATA *	ignore_last = NULL;
    IGNORE_DATA *	pId;
    BOARD_INFO *	pInfo;
    BOARD_INFO *	pInfo_last = NULL;
    char *		word;
    char *		word2;
    bool		fMatch;
    int			lastlogoff = current_time;
    int			percent;
    int			value;
    int			q;
    time_t		timestamp;

    sprintf( buf, "Loading %s.", ch->name );
    log_string( buf );

    alias_last = NULL;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Act",		ch->act,		fread_flag( fp ) );
	    KEY( "Act2",	ch->act2,		fread_flag( fp ) );
	    KEY( "AffectedBy",	ch->affected_by,	fread_xbits( fp ) );
	    KEY( "AfBy",	ch->affected_by,	fread_xbits( fp ) );
	    KEY( "Alignment",	ch->alignment,		fread_number( fp ) );
	    KEY( "Alig",	ch->alignment,		fread_number( fp ) );

	    if ( !str_cmp( word, "Alias" ) )
	    {
		alias_new = new_alias( );
		alias_new->old = str_dup( fread_word( fp ) );
		alias_new->new = fread_string( fp );
		alias_new->next = NULL;
		if ( alias_last == NULL )
		    ch->pcdata->alias_list = alias_new;
		else
		    alias_last->next = alias_new;
		alias_last = alias_new;
		fMatch = TRUE;
		break;
	    }

	    if (!str_cmp( word, "AC") || !str_cmp(word,"Armor"))
	    {
		fread_to_eol(fp);
		fMatch = TRUE;
		break;
	    }

	    if (!str_cmp(word,"ACs"))
	    {
		int i;

		for (i = 0; i < 4; i++)
		    ch->armor[i] = fread_number(fp);
		fMatch = TRUE;
		break;
	    }

	    if (!str_cmp(word, "AffD"))
	    {
		AFFECT_DATA *paf;
		int sn;

		paf = new_affect();

		sn = skill_lookup(fread_word(fp));
		if (sn < 0)
		    bug("Fread_char: unknown skill.",0);
		else
		    paf->type = sn;

		paf->level	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->next	= ch->affected;
		ch->affected	= paf;
		fMatch = TRUE;
		break;
	    }

            if (!str_cmp(word, "Affc"))
            {
                AFFECT_DATA *paf;
                int sn;

                paf = new_affect();

                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                    paf->type = sn;

                paf->where  = fread_number(fp);
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
                paf->next       = ch->affected;
                ch->affected    = paf;
                fMatch = TRUE;
                break;
            }

	    if ( !str_cmp( word, "AttrMod"  ) || !str_cmp(word,"AMod"))
	    {
		int stat;
		for (stat = 0; stat < MAX_STATS; stat ++)
		   ch->mod_stat[stat] = fread_number(fp);
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "AttrPerm" ) || !str_cmp(word,"Attr"))
	    {
		int stat;

		for (stat = 0; stat < MAX_STATS; stat++)
		    ch->perm_stat[stat] = fread_number(fp);
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'B':
	    KEY( "Bamfin",	ch->pcdata->bamfin,	fread_string( fp ) );
	    KEY( "Bamfout",	ch->pcdata->bamfout,	fread_string( fp ) );
	    KEY( "Beacon",	ch->pcdata->beacon,	fread_number( fp ) );
	    KEY( "Bin",		ch->pcdata->bamfin,	fread_string( fp ) );
	    KEY( "Bout",	ch->pcdata->bamfout,	fread_string( fp ) );
	    if ( !str_cmp( word, "Bank" ) )
	    {
		bank = new_bank( );
		bank->owner			=	ch;
		bank->vnum			=	fread_number( fp );
		bank->acct.gold			=	fread_number( fp );
		bank->acct.silver		=	fread_number( fp );
		bank->acct.copper		=	fread_number( fp );
		bank->acct.fract			=	fread_number( fp );
		if ( bank_last == NULL )
		    ch->pcdata->bank = bank;
		else
		    bank_last->next = bank;
		bank_last = bank;

		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Brd" ) )
	    {
		word = fread_word( fp );
		timestamp = fread_number( fp );
		if ( NULL != board_lookup( word ) )
		{
		    if ( ( pInfo = get_board_info( ch, word ) ) == NULL )
			{
			pInfo = new_board_info( );
			pInfo->name = str_dup( word );
			if ( pInfo_last == NULL )
			    ch->pcdata->last_read = pInfo;
			else
			    pInfo_last->next = pInfo;
			pInfo_last = pInfo;
		    }
		    pInfo->time = UMAX( pInfo->time, timestamp );
		}

		fMatch = TRUE;
		break;
	    }
            break;

	case 'C':
	    KEY( "Chit",	ch->money.fract,		fread_number( fp ) );
//	    KEY( "Class",	ch->class,		fread_number( fp ) );
//	    KEY( "Cla",		ch->class,		fread_number( fp ) );
	    KEY( "Clan",	ch->clan,	get_clan( fread_word(fp)));
	    KEY( "Clvl",	ch->clvl,		fread_number( fp ) );
	    KEY( "Copper",	ch->money.copper,	fread_number( fp ) );
	    if ( !str_cmp( word, "Cla" ) || !str_cmp( word, "Class" ) )
	    {
		char word2[SHORT_STRING_LENGTH];
		int  nClass;

		strcpy( word2, fread_word( fp ) );
		if ( is_number( word2 ) )
		    ch->class = atoi( word2 );
		else
		{
		    nClass = class_lookup( word2 );
		    if ( nClass == NO_CLASS )
		    {
			bugf( "Fread_char: bad class %s", word2 );
			nClass = class_mage;
		    }
		    ch->class = nClass;
		}
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "ClrMap" ) )
	    {
		int i;
		for ( i = 1; i < MAX_CLRMAP; i++ )
		    ch->pcdata->colormap[i] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Condition" ) || !str_cmp(word,"Cond"))
	    {
		ch->pcdata->condition[0] = fread_number( fp );
		ch->pcdata->condition[1] = fread_number( fp );
		ch->pcdata->condition[2] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
            if (!str_cmp(word,"Cnd"))
            {
                ch->pcdata->condition[0] = fread_number( fp );
                ch->pcdata->condition[1] = fread_number( fp );
                ch->pcdata->condition[2] = fread_number( fp );
		ch->pcdata->condition[3] = fread_number( fp );
                fMatch = TRUE;
                break;
            }
            if ( !str_cmp( word, "Cnds" ) )
            {
                ch->pcdata->condition[0] = fread_number( fp );
                ch->pcdata->condition[1] = fread_number( fp );
                ch->pcdata->condition[2] = fread_number( fp );
		ch->pcdata->condition[3] = fread_number( fp );
                ch->pcdata->condition[4] = fread_number( fp );
                ch->pcdata->condition[5] = fread_number( fp );
                ch->pcdata->condition[6] = fread_number( fp );
		ch->pcdata->condition[7] = fread_number( fp );
                fMatch = TRUE;
                break;
            }

	    KEY("Comm",		ch->comm,		fread_flag( fp ) ); 

	    break;

	case 'D':
	    KEY( "Damroll",	ch->damroll,		fread_number( fp ) );
	    KEY( "Dam",		ch->damroll,		fread_number( fp ) );
	    KEY( "Deaf",	ch->deaf,		fread_flag( fp ) );
	    KEY( "Description",	ch->description,	fread_string( fp ) );
	    KEY( "Desc",	ch->description,	fread_string( fp ) );
	    KEY( "Detr",	ch->pcdata->detract,	fread_string( fp ) );
	    KEY( "DeadR",	ch->pcdata->deathroom,	fread_number( fp ) );
	    if ( !str_cmp( word, "DeadS" ) )
	    {
        	word = fread_word( fp );
        	ch->pcdata->deathstate = flag_value( death_types, word );
        	if ( ch->pcdata->deathstate == NO_FLAG )
        	{
        	    bugf( "Fread_char: bad deathstate '%s'", word );
        	    ch->pcdata->deathstate = DEATH_NONE;
        	}
        	fMatch = TRUE;
		break;
	    }
	    break;

	case 'E':
	    KEY( "Empw",	ch->pcdata->empower,	fread_string( fp ) );
	    if ( !str_cmp( word, "End" ) )
	    {
    		/* adjust hp mana move up  -- here for speed's sake */
    		percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);

		percent = UMIN(percent,100);

    		if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON)
    		&&  !IS_AFFECTED(ch,AFF_PLAGUE))
    		{
        	    ch->hit	+= (ch->max_hit - ch->hit) * percent / 100;
        	    ch->mana    += (ch->max_mana - ch->mana) * percent / 100;
        	    ch->move    += (ch->max_move - ch->move)* percent / 100;
    		}
		return;
	    }
	    KEY( "Exp",		ch->exp,		fread_number( fp ) );
	    KEY( "Ethos",	ch->ethos,	fread_number( fp ) );
	    KEY( "EyeA",	ch->pcdata->eye_adj,	fread_string( fp ) );
	    KEY( "EyeC",	ch->pcdata->eye_color,	fread_string( fp ) );
	    break;

	case 'F':
	    KEY( "Fract",	ch->money.fract,	fread_number( fp ) );
	    if ( !str_cmp( word, "Fdrm" ) )
	    {
	        ch->pcdata->forcedream[1] = ch->pcdata->forcedream[0];
	        ch->pcdata->forcedream[0] = fread_number( fp );
	        fMatch = TRUE;
	        break;
	    }
	    break;

	case 'G':
	    if ( !str_cmp( word, "Gid" ) )
	    {
		ch->pcdata->greet_id = fread_number( fp );
		if ( ch->pcdata->greet_id >= greet_size )
		{
		    bugf( "%s greet id %d, greet_size %d",
		          ch->name, ch->pcdata->greet_id, greet_size );
		    ch->pcdata->greet_id = 0;
		}
		fMatch = TRUE;
		break;
	    }

	    KEY( "Gold",	ch->money.gold,		fread_number( fp ) );

            if ( !str_cmp( word, "Group" )  || !str_cmp(word,"Gr"))
            {
                int gn;
                char *temp;

                temp = fread_word( fp ) ;
                gn = group_lookup(temp);
                /* gn    = group_lookup( fread_word( fp ) ); */
                if ( gn < 0 )
                {
                    fprintf(stderr,"%s",temp);
                    bug( "Fread_char: unknown group. ", 0 );
                }
                else
		    gn_add(ch,gn);
                fMatch = TRUE;
            }
	    break;

	case 'H':
	    KEY( "HairA",	ch->pcdata->hair_adj,	fread_string( fp ) );
	    KEY( "HairC",	ch->pcdata->hair_color,	fread_string( fp ) );
	    KEY( "Hitroll",	ch->hitroll,		fread_number( fp ) );
	    KEY( "Hit",		ch->hitroll,		fread_number( fp ) );

	    if ( !str_cmp( word, "HpManaMove" ) || !str_cmp(word,"HMV"))
	    {
		ch->hit		= fread_number( fp );
		ch->max_hit	= fread_number( fp );
		ch->mana	= fread_number( fp );
		ch->max_mana	= fread_number( fp );
		ch->move	= fread_number( fp );
		ch->max_move	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }

            if ( !str_cmp( word, "HpManaMovePerm" ) || !str_cmp(word,"HMVP"))
            {
                ch->pcdata->perm_hit	= fread_number( fp );
                ch->pcdata->perm_mana   = fread_number( fp );
                ch->pcdata->perm_move   = fread_number( fp );
                fMatch = TRUE;
                break;
            }

	    break;

	case 'I':
	    KEY( "Id",		ch->id,			fread_number( fp ) );
	    if ( !str_cmp( word, "Ignor" ) )
	    {
	        pId = new_ignore_data( );
	        pId->name = str_dup( fread_word( fp ) );
	        pId->flags = fread_flag( fp );
	        if ( ignore_last == NULL )
	            ch->pcdata->ignoring = pId;
                else
                    ignore_last->next = pId;
                pId->prev = ignore_last;
                ignore_last = pId;
	        fMatch = TRUE;
	        break;
            }
	    KEY( "Inco",	ch->incog_level,	fread_number( fp ) );
	    KEY( "Info",	ch->info,		fread_flag( fp ) );
	    KEY( "InvisLevel",	ch->invis_level,	fread_number( fp ) );
	    KEY( "Invi",	ch->invis_level,	fread_number( fp ) );
	    break;

	case 'K':
	    KEY( "Kngdm",	ch->pcdata->kingdom,	fread_number( fp ) );
	    KEY( "Kop",		ch->money.copper,	fread_number( fp ) );
	    break;

	case 'L':
	    KEY( "LastLevel",	ch->pcdata->last_level, fread_number( fp ) );
	    KEY( "LastMob",	ch->pcdata->last_medit, fread_number( fp ) );
	    KEY( "LastObj",	ch->pcdata->last_oedit, fread_number( fp ) );
	    KEY( "LLev",	ch->pcdata->last_level, fread_number( fp ) );
	    KEY( "Lnm",		ch->pcdata->lname,	fread_string( fp ) );
	    KEY( "LogC",	ch->pcdata->logcount,	fread_number( fp ) );
	    KEY( "LogO",	lastlogoff,		fread_number( fp ) );
	    KEY( "LongDescr",	ch->long_descr,		fread_string( fp ) );
	    KEY( "LnD",		ch->long_descr,		fread_string( fp ) );
	    if ( !str_cmp( word, "Level" ) || !str_cmp( word, "Lev" )
	    ||	 !str_cmp( word, "Levl" ) )
	    {
		ch->level = level_lookup( fread_word( fp ) );
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Ldrm" ) )
	    {
	        ch->pcdata->lastdream[1] = ch->pcdata->lastdream[0];
	        ch->pcdata->lastdream[0] = fread_number( fp );
	        fMatch = TRUE;
	        break;
	    }
	    break;

	case 'N':
	    KEYS( "Name",	ch->name,		fread_string( fp ) );
	    if ( !str_cmp( word, "Not" ) )
	    {
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Note" ) )
	    {
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'O':
	    KEY( "Outcast",	ch->pcdata->outcast,	fread_number( fp ) );
	    KEY( "Dollar",	ch->money.gold,		fread_number( fp ) );
	    break;

	case 'P':
	    KEY( "Password",	ch->pcdata->pwd,	fread_string( fp ) );
	    KEY( "Pass",	ch->pcdata->pwd,	fread_string( fp ) );
	    KEY( "Played",	ch->played,		fread_number( fp ) );
	    KEY( "Plyd",	ch->played,		fread_number( fp ) );
	    KEY( "Points",	ch->pcdata->points,	fread_number( fp ) );
	    KEY( "Pnts",	ch->pcdata->points,	fread_number( fp ) );
//	    KEY( "Position",	ch->position,		fread_number( fp ) );
	    if ( !str_cmp( word, "Pos" ) )
	    {
	        fMatch = TRUE;
	        word2 = fread_word( fp );
	        if ( is_number( word2 ) )
	        {
	            value = atoi( word2 );
                }
                else
                {
                    value = position_lookup( word2 );
                    if ( value == NO_POSITION )
                    {
                        bugf( "Fread_char: bad position \"%s\" for %s", word2, ch->name );
                        value = POS_STANDING;
                    }
                }
                if ( value == POS_FIGHTING )
                    value = POS_STANDING;
                ch->position = value;
                break;
	    }
	    KEY( "Pose",	ch->pcdata->pose,	fread_string( fp ) );
	    KEY( "Practice",	ch->practice,		fread_number( fp ) );
	    KEY( "Prac",	ch->practice,		fread_number( fp ) );
            KEYS( "Prompt",	ch->prompt,		fread_string( fp ) );
 	    KEY( "Prom",	ch->prompt,		fread_string( fp ) );
	    break;

	case 'Q':
            KEY( "QPts",	ch->questpoints,	fread_number( fp ) );
	    if ( !str_cmp( word, "QNext" ) )
	    {
		fMatch = TRUE;
		q = fread_number( fp );
		if ( q < current_time )
		    ch->nextquest = 0;
		else
		    ch->nextquest = ( q - current_time + 59 ) / 60;
		break;
	    }
	    KEY( "QNone",	ch->questnone,		fread_number( fp ) );
	    break;

	case 'R':
	    KEY( "Race",        ch->race,	
				race_lookup( fread_buf( fp, buf, sizeof( buf ) ) ) );
	    KEY( "Room",	ch->room_vnum,		fread_number( fp ) );

	    break;

	case 'S':
	    KEY( "SavingThrow",	ch->saving_throw,	fread_number( fp ) );
	    KEY( "Save",	ch->saving_throw,	fread_number( fp ) );
	    KEY( "Scro",	ch->lines,		fread_number( fp ) );
	    KEY( "SecGrp",	ch->pcdata->sec_groups,	fread_flag  ( fp ) );
	    KEY( "Security",	ch->pcdata->security,	fread_number( fp ) );
	    KEY( "Sex",		ch->sex,		fread_number( fp ) );
	    KEY( "ShBy",	ch->shielded_by,	fread_xbits ( fp ) );
	    KEY( "ShortDescr",	ch->short_descr,	fread_string( fp ) );
	    KEY( "ShD",		ch->short_descr,	fread_string( fp ) );
	    KEY( "TenP",	ch->money.silver,	fread_number( fp ) );
	    KEY( "Silv",	ch->money.silver,	fread_number( fp ) );
	    KEY( "Size",	ch->size,		size_lookup( fread_word( fp ) ) );

	    if ( !str_cmp( word, "Skl" ) )
	    {
		int sn;
		int prct;
		int used;
		int f_use;
		int l_use;
		char *temp;

		prct  = fread_number( fp );
		used  = fread_number( fp );
		f_use = fread_number( fp );
		l_use = fread_number( fp );
		temp = fread_word( fp ) ;
		sn = skill_lookup( temp );
		if ( sn < 0 )
		{
		    bugf( "Fread_char: unknown skill '%s'. ", temp );
		}
		else
		{
		    ch->pcdata->skill[sn].percent	= prct;
		    ch->pcdata->skill[sn].usage		= used;
		    ch->pcdata->skill[sn].learned_age	= f_use;
		    ch->pcdata->skill[sn].used_age	= l_use;
		}
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Skill" ) || !str_cmp(word,"Sk"))
	    {
		int sn;
		int value;
		char *temp;

		value = fread_number( fp );
		temp = fread_word( fp ) ;
		sn = skill_lookup(temp);
		/* sn    = skill_lookup( fread_word( fp ) ); */
		if ( sn < 0 )
		{
		    fprintf(stderr,"%s",temp);
		    bug( "Fread_char: unknown skill. ", 0 );
		}
		else
		{
		    ch->pcdata->skill[sn].percent	= value;
		    ch->pcdata->skill[sn].usage		= 1;
		    ch->pcdata->skill[sn].learned_age	= 1;
		    ch->pcdata->skill[sn].used_age	= 2;
		}
		fMatch = TRUE;
	    }

	    break;

	case 'T':
            KEY( "TrueSex",     ch->pcdata->true_sex,  	fread_number( fp ) );
	    KEY( "TSex",	ch->pcdata->true_sex,   fread_number( fp ) );
	    KEY( "Trai",	ch->train,		fread_number( fp ) );
	    KEY( "Trust",	ch->trust,		fread_number( fp ) );
	    KEY( "Tru",		ch->trust,		fread_number( fp ) );
        KEY( "Tpld",	ch->pcdata->timesplayed, fread_number( fp ) );
	    if ( !str_cmp( word, "Title" )  || !str_cmp( word, "Titl"))
	    {
		ch->pcdata->title = fread_string( fp );
    		if (ch->pcdata->title[0] != '.' && ch->pcdata->title[0] != ',' 
		&&  ch->pcdata->title[0] != '!' && ch->pcdata->title[0] != '?')
		{
		    sprintf( buf, " %s", ch->pcdata->title );
		    free_string( ch->pcdata->title );
		    ch->pcdata->title = str_dup( buf );
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'V':
	    KEY( "Version",     ch->version,		fread_number ( fp ) );
	    KEY( "Vers",	ch->version,		fread_number ( fp ) );
	    if ( !str_cmp( word, "Vnum" ) )
	    {
		ch->pIndexData = get_mob_index( fread_number( fp ) );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "Wimpy",	ch->wimpy,		fread_number( fp ) );
	    KEY( "Wimp",	ch->wimpy,		fread_number( fp ) );
	    KEY( "Wizn",	ch->wiznet,		fread_flag( fp ) );
	    KEY( "WhoTxt",	ch->pcdata->who_text,	fread_string( fp ) );
	    break;
	}

	if ( !fMatch )
	{
	    bugf( "Fread_char: no match: %s", word );
	    fread_to_eol( fp );
	}
    }
}


/* load a pet or mount from the forgotten reaches */
void
fread_pet( CHAR_DATA *ch, FILE *fp, bool fMount )
{
    char *	word;
    char *	word2;
    CHAR_DATA *	pet;
    bool	fMatch;
    int lastlogoff = current_time;
    int		percent;
    int		value;

    /* first entry had BETTER be the vnum or we barf */
    word = feof(fp) ? "END" : fread_word(fp);
    if (!str_cmp(word,"Vnum"))
    {
    	int vnum;

    	vnum = fread_number(fp);
    	if (get_mob_index(vnum) == NULL)
	{
    	    bug("Fread_pet: bad vnum %d.",vnum);
	    pet = create_mobile(get_mob_index(MOB_VNUM_SLUG));
	}
    	else
    	    pet = create_mobile(get_mob_index(vnum));
    }
    else
    {
        bug("Fread_pet: no vnum in file.",0);
        pet = create_mobile(get_mob_index(MOB_VNUM_SLUG));
    }

    for ( ; ; )
    {
    	word 	= feof(fp) ? "END" : fread_word(fp);
    	fMatch = FALSE;

    	switch (UPPER(word[0]))
    	{
    	case '*':
    	    fMatch = TRUE;
    	    fread_to_eol(fp);
    	    break;

    	case 'A':
    	    KEY( "Act",		pet->act,		fread_flag(fp));
    	    KEY( "AfBy",	pet->affected_by,	fread_xbits(fp));
    	    KEY( "Alig",	pet->alignment,		fread_number(fp));

    	    if (!str_cmp(word,"ACs"))
    	    {
    	    	int i;

    	    	for (i = 0; i < 4; i++)
    	    	    pet->armor[i] = fread_number(fp);
    	    	fMatch = TRUE;
    	    	break;
    	    }

	    if ( !str_cmp( word, "Affc" ) || !str_cmp( word, "AffD" ) )
	    {
		AFFECT_DATA *paf;
		AFFECT_DATA *paf_prev;
		int sn;

		paf = new_affect( );

                sn = skill_lookup( fread_word( fp ) );
                if ( sn < 0 )
                    bug( "Fread_char: unknown skill.", 0 );
		else
		    paf->type = sn;

		paf->where	= fread_number( fp );
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );

		/* hack to avoid duping race affects */
		for ( paf_prev = pet->affected; paf_prev != NULL; paf_prev = paf_prev->next )
		{
		    if ( paf_prev->type      == paf->type
		    &&	 paf_prev->where     == paf->where
		    &&	 paf_prev->level     == paf->level
		    &&	 paf_prev->duration  == paf->duration
		    &&	 paf_prev->modifier  == paf->modifier
		    &&	 paf_prev->location  == paf->location
		    &&	 paf_prev->bitvector == paf->bitvector )
			break;
		}
		if ( paf_prev == NULL )
		{
		    paf->next       = pet->affected;
		    pet->affected   = paf;
		}
		else
		    free_affect( paf );
                fMatch          = TRUE;
                break;
            }

    	    if (!str_cmp(word,"AMod"))
    	    {
    	     	int stat;

    	     	for (stat = 0; stat < MAX_STATS; stat++)
    	     	    pet->mod_stat[stat] = fread_number(fp);
    	     	fMatch = TRUE;
    	     	break;
    	    }

    	    if (!str_cmp(word,"Attr"))
    	    {
    	         int stat;

    	         for (stat = 0; stat < MAX_STATS; stat++)
    	             pet->perm_stat[stat] = fread_number(fp);
    	         fMatch = TRUE;
    	         break;
    	    }
    	    break;

    	 case 'C':
	    KEY( "Chit",	pet->money.fract,	fread_number( fp ) );
            KEY( "Clan",       pet->clan,       get_clan(fread_word(fp)));
    	    KEY( "Comm",	pet->comm,		fread_flag(fp));
    	    KEY( "Copper",	pet->money.copper,	fread_number( fp ) );
    	    break;

    	 case 'D':
    	     KEY( "Dam",	pet->damroll,		fread_number(fp));
	     KEY( "Deaf",	pet->deaf,		fread_flag( fp ) );
    	     KEY( "Desc",	pet->description,	fread_string(fp));
    	     break;

    	 case 'E':
    	     if (!str_cmp(word,"End"))
	     {
		if ( fMount )
		{
		    ch->mount = pet;
		    pet->mount = ch;
		}
		else
		{
		    pet->leader = ch;
		    pet->master = ch;
		    ch->pet = pet;
		}
    		/* adjust hp mana move up  -- here for speed's sake */
    		percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);

    		if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON)
    		&&  !IS_AFFECTED(ch,AFF_PLAGUE))
    		{
		    percent = UMIN(percent,100);
    		    pet->hit	+= (pet->max_hit - pet->hit) * percent / 100;
        	    pet->mana   += (pet->max_mana - pet->mana) * percent / 100;
        	    pet->move   += (pet->max_move - pet->move)* percent / 100;
    		}
    	     	return;
	     }
    	     KEY( "Exp",	pet->exp,		fread_number(fp));
    	     break;

	case 'F':
	    KEY( "Fract",	pet->money.fract,	fread_number( fp ) );
	    break;

	case 'G':
	    KEY( "Gold",	pet->money.gold,	fread_number( fp ) );
	    break;    	     

    	 case 'H':
    	     KEY( "Hit",	pet->hitroll,		fread_number(fp));

    	     if (!str_cmp(word,"HMV"))
    	     {
    	     	pet->hit	= fread_number(fp);
    	     	pet->max_hit	= fread_number(fp);
    	     	pet->mana	= fread_number(fp);
    	     	pet->max_mana	= fread_number(fp);
    	     	pet->move	= fread_number(fp);
    	     	pet->max_move	= fread_number(fp);
    	     	fMatch = TRUE;
    	     	break;
    	     }
    	     break;

	case 'K':
	    KEY( "Kop",		pet->money.copper,	fread_number( fp ) );

     	case 'L':
    	     KEY( "Levl",	pet->level,		fread_number(fp));
    	     KEY( "LnD",	pet->long_descr,	fread_string(fp));
	     KEY( "LogO",	lastlogoff,		fread_number(fp));
    	     break;

    	case 'N':
    	     KEY( "Name",	pet->name,		fread_string(fp));
    	     break;

	case 'O':
	    KEY( "Dollar",	pet->money.gold,	fread_number( fp ) );
	    break;

    	case 'P':
	    if ( !str_cmp( word, "Pos" ) )
	    {
	        fMatch = TRUE;
	        word2 = fread_word( fp );
	        if ( is_number( word2 ) )
	        {
	            value = atoi( word2 );
                }
                else
                {
                    value = position_lookup( word2 );
                    if ( value == NO_POSITION )
                    {
                        bugf( "Fread_pet: bad position \"%s\" for %s", word2, pet->name );
                        value = POS_STANDING;
                    }
                }
                if ( value == POS_FIGHTING )
                    value = POS_STANDING;
                pet->position = value;
                break;
	    }
    	    break;

	case 'R':
    	    KEY( "Race",	pet->race, race_lookup(fread_string(fp)));
    	    break;

    	case 'S' :
    	    KEY( "Save",	pet->saving_throw,	fread_number( fp ) );
    	    KEY( "Sex",		pet->sex,		fread_number( fp ) );
    	    KEY( "ShBy",	pet->shielded_by,	fread_xbits ( fp ) );
    	    KEY( "ShD",		pet->short_descr,	fread_string( fp ) );
	    KEY( "TenP",	pet->money.silver,	fread_number( fp ) );
	    KEY( "Silv",	pet->money.silver,	fread_number( fp ) );
	    KEY( "Size",	pet->size,		size_lookup( fread_word( fp ) ) );
	    break;

    	if ( !fMatch )
    	{
    	    bugf( "Fread_pet: no match: %s", word );
    	    fread_to_eol(fp);
    	}

    	}
    }
}


void
fread_obj( CHAR_DATA *ch, FILE *fp )
{
    OBJ_DATA *	obj;
    char *	word;
    int		iNest;
    BANK_DATA *	bank;
    int		bank_vnum;
    bool	fMatch;
    bool	fNest;
    bool	fVnum;
    bool	first;
    bool	make_new;    /* update object */

    fVnum = FALSE;
    obj = NULL;
    first = TRUE;  /* used to counter fp offset */
    make_new = FALSE;
    bank_vnum = 0;

    word   = feof( fp ) ? "End" : fread_word( fp );
    if (!str_cmp(word,"Vnum" ))
    {
        int vnum;
	first = FALSE;  /* fp will be in right place */

        vnum = fread_number( fp );
        if (  get_obj_index( vnum )  == NULL )
	{
            bug( "Fread_obj: bad vnum %d.", vnum );
	}
        else
	{
	    obj = create_object( get_obj_index( vnum ), -1 );
	}
    }

    if (obj == NULL)  /* either not found or old style */
    {
    	obj = new_obj();
    	obj->name		= str_dup( "" );
    	obj->short_descr	= str_dup( "" );
    	obj->description	= str_dup( "" );
    }

    fNest		= FALSE;
    fVnum		= TRUE;
    iNest		= 0;

    xCLEAR_BITS( obj->race_flags );
    xCLEAR_BITS( obj->class_flags );

    for ( ; ; )
    {
	if (first)
	    first = FALSE;
	else
	    word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    if (!str_cmp(word,"AffD"))
	    {
		AFFECT_DATA *paf;
		int sn;

		paf = new_affect();

		sn = skill_lookup(fread_word(fp));
		if (sn < 0)
		    bug("Fread_obj: unknown skill.",0);
		else
		    paf->type = sn;

		paf->level	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->next	= obj->affected;
		obj->affected	= paf;
		fMatch		= TRUE;
		break;
	    }
            if (!str_cmp(word,"Affc"))
            {
                AFFECT_DATA *paf;
                int sn;

                paf = new_affect();

                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_obj: unknown skill.",0);
                else
                    paf->type = sn;

		paf->where	= fread_number( fp );
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
                paf->next       = obj->affected;
                obj->affected   = paf;
                fMatch          = TRUE;
                break;
            }
	    if ( !str_cmp( word, "Anti" ) )
	    {
		char *	word;
		int	bit;
		word = fread_word( fp );
		if ( ( bit = race_lookup( word ) ) != 0 )
		    xSET_BIT( obj->race_flags, bit );
                else if ( ( bit = class_lookup( word ) ) != NO_CLASS )
                    xSET_BIT( obj->class_flags, bit );
                else
                    bugf( "Fread_obj: bad race/class '%s'", word );
                fMatch = TRUE;
                break;
	    }

	    break;

	case 'B':
	    KEY( "Bank",	bank_vnum,		fread_number( fp ) );
	    break;

	case 'C':
	    KEY( "Cond",	obj->condition,		fread_number( fp ) );
	    KEY( "Cost",	obj->cost,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Description",	obj->description,	fread_string( fp ) );
	    KEY( "Desc",	obj->description,	fread_string( fp ) );
	    break;

	case 'E':

	    if ( !str_cmp( word, "Enchanted"))
	    {
		obj->enchanted = TRUE;
	 	fMatch 	= TRUE;
		break;
	    }

	    KEY( "ExtFl",	obj->extra_flags,	fread_flag( fp ) );
	    KEY( "ExtraFlags",	obj->extra_flags,	fread_number( fp ) );
	    KEY( "ExtF",	obj->extra_flags,	fread_number( fp ) );
		KEY( "ExtFl2",	obj->extra_flags2,	fread_flag( fp ) );
	    KEY( "ExtraFlags2",	obj->extra_flags2,	fread_number( fp ) );
	    KEY( "ExtF2",	obj->extra_flags2,	fread_number( fp ) );

	    if ( !str_cmp( word, "ExtraDescr" ) || !str_cmp(word,"ExDe"))
	    {
		EXTRA_DESCR_DATA *ed;

		ed = new_extra_descr();

		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= obj->extra_descr;
		obj->extra_descr	= ed;
		fMatch = TRUE;
	    }

	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !fNest || !fVnum || obj->pIndexData == NULL)
		{
		    bug( "Fread_obj: incomplete object.", 0 );
		    free_obj(obj);
		    return;
		}
		else
		{
		    if (make_new)
		    {
			int wear;

			wear = obj->wear_loc;
			extract_obj(obj);

			obj = create_object(obj->pIndexData,0);
			obj->wear_loc = wear;
		    }
		    if ( bank_vnum != 0 )
		    {
			if ( IS_NPC( ch ) )
			{
			    bugf( "NPC with stored item: %s.", ch->short_descr );
			    extract_obj( obj );
			    return;
			}
			for ( bank = ch->pcdata->bank; bank != NULL; bank = bank->next )
			    if ( bank->vnum == bank_vnum )
				break;
			if ( bank != NULL )
			{
			    obj_to_bank( obj, bank );
			    return;
			}
		    }
		    if ( iNest == 0 || rgObjNest[iNest] == NULL )
			obj_to_char( obj, ch );
		    else
			obj_to_obj( obj, rgObjNest[iNest-1] );
		    return;
		}
	    }
	    break;

	case 'I':
	    KEY( "ItemType",	obj->item_type,		fread_number( fp ) );
	    KEY( "Ityp",	obj->item_type,		fread_number( fp ) );
	    if ( !str_cmp( word, "Icharge" ) || !str_cmp( word, "Ichrg" ) )
	    {
		obj->ac_charge[0]		=	fread_number( fp );
		obj->ac_charge[1]		=	fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'L':
	    KEY( "Level",	obj->level,		fread_number( fp ) );
	    KEY( "Lev",		obj->level,		fread_number( fp ) );
	    break;

	case 'N':
	    KEY( "Name",	obj->name,		fread_string( fp ) );

	    if ( !str_cmp( word, "Nest" ) )
	    {
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
		    bug( "Fread_obj: bad nest %d.", iNest );
		}
		else
		{
		    rgObjNest[iNest] = obj;
		    fNest = TRUE;
		}
		fMatch = TRUE;
	    }
	    break;

   	case 'O':
	    break;
    
	case 'Q':
	KEY( "QCost",	obj->qcost,		fread_number( fp ) );
	break;

	case 'S':
	    KEY( "ShortDescr",	obj->short_descr,	fread_string( fp ) );
	    KEY( "ShD",		obj->short_descr,	fread_string( fp ) );

	    if ( !str_cmp( word, "Spell" ) )
	    {
		int iValue;
		int sn;

		iValue = fread_number( fp );
		sn     = skill_lookup( fread_word( fp ) );
		if ( iValue < 0 || iValue > 3 )
		{
		    bug( "Fread_obj: bad iValue %d.", iValue );
		}
		else if ( sn < 0 )
		{
		    bug( "Fread_obj: unknown skill.", 0 );
		}
		else
		{
		    obj->value[iValue] = sn;
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'T':
	    KEY( "Timer",	obj->timer,		fread_number( fp ) );
	    KEY( "Time",	obj->timer,		fread_number( fp ) );
	    break;

	case 'V':
	    if ( !str_cmp( word, "Values" ) || !str_cmp(word,"Vals"))
	    {
		obj->value[0]	= fread_number( fp );
		obj->value[1]	= fread_number( fp );
		obj->value[2]	= fread_number( fp );
		obj->value[3]	= fread_number( fp );
		if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
		   obj->value[0] = obj->pIndexData->value[0];
		fMatch		= TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Val" ) )
	    {
		char tLetter;

		obj->value[0] 		= fread_number( fp );
	 	obj->value[1]		= fread_number( fp );
	 	obj->value[2] 		= fread_number( fp );
		obj->value[3]		= fread_number( fp );
		obj->value[4]		= fread_number( fp );

		/* Hack to read value[5] if present */
		tLetter			= fread_letter( fp );
		ungetc( tLetter, fp );
		if ( isdigit( tLetter ) )
		    obj->value[5]	= fread_number( fp );

		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Vnum" ) )
	    {
		int vnum;

		vnum = fread_number( fp );
		if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
		    bug( "Fread_obj: bad vnum %d.", vnum );
		else
		    fVnum = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "WearF",	obj->wear_flags,	fread_flag  ( fp ) );
	    KEY( "WearFlags",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WeaF",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WearLoc",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Wear",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Weight",	obj->weight,		fread_number( fp ) );
	    KEY( "Wt",		obj->weight,		fread_number( fp ) );
	    break;

	}

	if ( !fMatch )
	{
	    bugf( "Fread_obj: no match: %s", word );
	    fread_to_eol( fp );
	}
    }
}


