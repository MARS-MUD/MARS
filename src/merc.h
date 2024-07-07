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
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_EVENT_FUN( fun )	bool EVENT_FUN( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_EVENT_FUN( fun )	EVENT_FUN fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif


/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	!defined(BERR)
#define BERR	255
#endif


#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int				sh_int;
typedef int				bool;
#define unix
#else
typedef int				sh_int;
#if !defined( __cplusplus )
typedef unsigned char			bool;
#endif
#endif

typedef	unsigned int			bitvector;


/*
 * Structure types.
 */
typedef struct	affect_data		AFFECT_DATA;
typedef struct	alias_data		ALIAS_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct	avatar_data		AVATAR_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	bank_data		BANK_DATA;
typedef struct	board_data		BOARD_DATA;
typedef struct	board_info		BOARD_INFO;
typedef struct 	buf_type	 	BUFFER;
typedef struct	char_data		CHAR_DATA;
typedef struct	clan_data		CLAN_DATA;
typedef struct	clist_data		CLIST_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	dream_data		DREAM_DATA;
typedef struct	dreamseg_data		DREAMSEG_DATA;
typedef struct	dreamlist_data		DREAMLIST_DATA;
typedef struct	event_data		EVENT_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extended_bitvector	EXT_BV;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct  gen_data		GEN_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	ignore_data		IGNORE_DATA;
typedef struct	kill_data		KILL_DATA;
typedef struct	mem_data		MEM_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	mob_index_list		MILIST_DATA;
typedef struct	mob_prog_act_list	MPROG_ACT_LIST;
typedef struct	mob_prog_data		MPROG_DATA;
typedef struct	money_type		MONEY;
typedef struct	note_data		NOTE_DATA;
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	obj_index_list		OILIST_DATA;
typedef struct	oreprog_data		OREPROG_DATA;
typedef struct	overland_data		OVERLAND_DATA;
typedef struct	pc_data			PC_DATA;
typedef struct	project_data		PROJECT_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	social_data		SOCIAL_DATA;
typedef struct	strlist_data		STRLIST_DATA;
typedef struct	text_data		TEXT_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	travel_data		TRAVEL_DATA;
typedef struct	userlist_data		USERLIST_DATA;
typedef struct	virtual_room_data	VIRTUAL_ROOM_DATA;
typedef struct	weather_data		WEATHER_DATA;



/*
 * Function types.
 */
typedef	void DO_FUN	args( ( CHAR_DATA *ch, char *argument ) );
typedef bool EVENT_FUN  args( ( EVENT_DATA *event ) );
typedef bool SPEC_FUN	args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN	args( ( int sn, int level, CHAR_DATA *ch, void *vo,
				int target ) );

/*
 * ANSI color definitions
 */
#define AT_NONE		   -1
#define AT_BLACK	    0
#define AT_BLOOD	    1
#define AT_DGREEN           2
#define AT_ORANGE	    3
#define AT_DBLUE	    4
#define AT_PURPLE	    5
#define AT_CYAN	  	    6
#define AT_GREY		    7
#define AT_DGREY	    8
#define AT_RED		    9
#define AT_GREEN	   10
#define AT_YELLOW	   11
#define AT_BLUE		   12
#define AT_PINK		   13
#define AT_LBLUE	   14
#define AT_WHITE	   15
#define AT_BLINK	   16

/* ^^ Ansi junk */

#define C_DEFAULT	(AT_GREY)

#define AT_ACTION	AT_WHITE
#define AT_AREA		AT_GREY
#define AT_CHANNEL	AT_LBLUE
#define AT_EMOTE	AT_WHITE
#define AT_GOSSIP	AT_CYAN
#define AT_IMM		AT_RED
#define AT_OSAY		AT_ORANGE
#define AT_SAY		AT_GREEN
#define AT_SOCIAL	AT_WHITE
#define AT_TELL		AT_YELLOW
#define AT_WIZNET	AT_CYAN
#define AT_MAGIC 	AT_YELLOW


/*
 * String and memory management parameters.
 */
#define MAX_EVENT_HASH		  128
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 (8192 + 2048)
#define MAX_INPUT_LENGTH	 1024
#define SHORT_STRING_LENGTH	  128
#define PAGELEN			   22
#define WHOTEXT_LEN		   13
#define TELLBUF_SIZE		   24
#define MSL   MAX_STRING_LENGTH
#define MIL    MAX_INPUT_LENGTH
#define SEND   send_to_char
#define printf_to_char ch_printf
/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MUD_NAME		  "MARS"
#define IMP_NAME		  "Om"
#define GAME_PORT		  3033
#define BUILD_PORT		  3033
#define MUD_WEBSITE               "https://wr3tch.com/MARS"

#define WEBWHO_OFFSET		  2	/* offset from port to webwho port */
#define TIMEZONE		  "EST5EDT"
/* #define MAX_SKILL		  0	This is now obsolete */
#define MAX_RACE_SKILLS		    5	/* Max bonus skills for a race */
#define MAX_PRAC_PERCENT	   75	/* max amount a player can be trained */
#define MAX_PREREQ		    3
#define MAX_GROUP		   75
#define MAX_IN_GROUP		   30
#define MAX_BRAWLS		   45
#define MAX_RECURSION		    8
#define MAX_CLASS		   5
/*#define MAX_PC_RACE		   11*/	/* Note this is actually max plus one */
#define MAX_CLAN_LEVELS		   10
#define MAX_DAMAGE_MESSAGE	   41
#define MAX_CLRMAP		   16
#define MAX_FALL		   10
#define MAX_LEVEL		   30
#define MAX_MLOAD		   1000	/* max # of mobs of a given vnum that can be loaded */
#define MAX_VNUM		   10240000
#define MAX_RECENT_LINES	   24
#define MAX_KINGDOM		   8
#define MAX_FLEE_ATTEMPTS	   12
#define MAX_OUTCASTS		   2	/* # of times a player may be outcast */
#define MAX_QUOTES 15 /* This must equal the # of quotes you have */
#define LEVEL_HERO		   (21)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 9)
#define LEVEL_NEWBIE		   2
#define stc send_to_char
#define MAX_MORTAL		   (LEVEL_IMMORTAL - 1)

#define GREET_INCREMENT		   50	/* Amount by which the greet table is expandec */
#define PULSE_PER_SECOND	    4
#define PULSE_PER_MINUTE	  (60 * PULSE_PER_SECOND)

#define PULSE_AREA		  (61 * PULSE_PER_SECOND )
#define PULSE_AUCTION		  ( 8 * PULSE_PER_SECOND )
#define PULSE_MOBILE		  ( 4 * PULSE_PER_SECOND )
#define PULSE_OVERLAND		  (10 * PULSE_PER_SECOND )
#define PULSE_TICK		  (60 * PULSE_PER_SECOND )
#define PULSE_TRAVEL		  (30 * PULSE_PER_SECOND )
#define PULSE_VEHICLE_SAVE	  (31 * PULSE_PER_SECOND )
#define PULSE_VIOLENCE		  ( 3 * PULSE_PER_SECOND )
#define PULSE_QUEST               (60 * PULSE_PER_SECOND )
#define PULSE_UNDERWATER          (20 * PULSE_PER_SECOND )
#define KEEPALIVE_TIMER           (5*60  * PULSE_PER_SECOND ) /* Five minutes */

/* Not really a pulse in update() */
#define PULSE_DREAM		  (10 * PULSE_PER_SECOND )

#define SERAPHIM		MAX_LEVEL
#define CHERUBIM		(MAX_LEVEL - 1)
#define THRONE			(MAX_LEVEL - 2)
#define DOMINION		(MAX_LEVEL - 3)
#define POWERS			(MAX_LEVEL - 4)
#define VIRTUES			(MAX_LEVEL - 5)
#define ARCHANGEL		(MAX_LEVEL - 6)
#define ANGEL			(MAX_LEVEL - 7)
#define AVATAR			(MAX_LEVEL - 8)
#define VISITOR			(MAX_LEVEL - 9)

/* The old ROM level definitions */
#define IMPLEMENTOR		MAX_LEVEL
#define	CREATOR			(MAX_LEVEL - 1)
#define SUPREME			(MAX_LEVEL - 2)
#define DEITY			(MAX_LEVEL - 3)
#define GOD			(MAX_LEVEL - 4)
#define IMMORTAL		(MAX_LEVEL - 5)
#define DEMI			(MAX_LEVEL - 6)
#if 0 /* Same as above */
#define ANGEL			(MAX_LEVEL - 7)
#define AVATAR			(MAX_LEVEL - 8)
#define VISITOR			(MAX_LEVEL - 9)
#endif
#define HERO			LEVEL_HERO

/* Admin level */
#define L_ADMIN			(SUPREME)

/*
 * Currency
 */
#define FRACT_PER_COPPER	10
#define COPPER_PER_SILVER	10
#define SILVER_PER_GOLD		10

#define FRACT_NOUN		"pieces"
#define FRACT_PLURAL		"copper pieces"
#define FRACT_INITIAL		'p'
#define COPPER_NOUN		"copper"
#define COPPER_PLURAL		"copper coins"
#define COPPER_INITIAL		'c'
#define SILVER_NOUN		"silver"
#define SILVER_PLURAL		"silver coins"
#define SILVER_INITIAL		's'
#define GOLD_NOUN		"gold"
#define GOLD_PLURAL		"gold coins"
#define GOLD_INITIAL		'g'

/* max value for gold without causing overflow when converting to fractional */
#define MAX_GOLD	((0x7FFFFFFF / (FRACT_PER_COPPER * COPPER_PER_SILVER * SILVER_PER_GOLD)) - 1 )


/*
 * This is the default level for skills.  Characters (presumably immortals)
 * of this level or higher will know all spells/skills.
 */
#define L_APP			(ANGEL)


/*
 * Calendar stuff
 */
#define MAX_HOUR		24
#define HOURS_PER_DAY		MAX_HOUR
#define HOUR_SUNRISE		(HOURS_PER_DAY / 4)
#define HOUR_DAY_BEGIN		((HOUR_SUNRISE) + 1)
#define HOUR_SUNSET		((HOURS_PER_DAY / 4) * 3 )
#define HOUR_NIGHT_BEGIN	((HOUR_SUNSET)+1)

#define DAYS_PER_WEEK		6
#define WEEKS_PER_MONTH		7
#define DAYS_PER_MONTH		(DAYS_PER_WEEK * WEEKS_PER_MONTH)
#define MONTHS_PER_YEAR		8
#define MONTHS_PER_SEASON	(MONTHS_PER_YEAR / 4)

#define SEASON_WINTER		 0
#define SEASON_SPRING		 1
#define SEASON_SUMMER		 2
#define SEASON_FALL		 3
#define SEASON_AUTUMN		 3
#define SEASON_NONE		-1

#define SEASON			(time_info.month / MONTHS_PER_SEASON)


/*
 * Return values for failed lookups
 */
#define NO_CLASS		-1
#define NO_COMMAND		-1
#define NO_FLAG			((unsigned)(-99)) /* Must not be used in flags or stats */
#define	NO_POSITION		-1
#define NO_RACE			0
#define NO_SKILL		-1
#define NO_VALUE		((unsigned)(-1))


/*
 * Hiring stuff, for MSSP
 * Offsets into hiring array.
 */
#define HIRE_BUILDERS           0
#define HIRE_CODERS             1


/*
 * Defines for extended bitvectors
 */
#ifndef INTBITS
  #define INTBITS	32
#endif
#define XBM		31	/* extended bitmask   ( INTBITS - 1 )	*/
#define RSV		5	/* right-shift value  ( sqrt(XBM+1) )	*/
#define XBI		4	/* integers in an extended bitvector	*/
#define MAX_BITS	XBI * INTBITS
/*
 * Structure for extended bitvectors -- Thoric
 */
struct extended_bitvector
{
    unsigned int		bits[XBI];
};


/*
 * Animal structure, for spell animal summoning
 */
struct animal_summon_type
{
    int		level;
    int		hit;
    int		move;
    int *	race;
    int		size;
    char *	name;
    char *	short_descr;
    char *	long_descr;
    char *	to_char;
    char *	to_room;
};


/*
 * Site ban structure.
 */

#define BAN_SUFFIX		A
#define BAN_PREFIX		B
#define BAN_NEWBIES		C
#define BAN_ALL			D
#define BAN_PERMIT		E
#define BAN_PERMANENT		F

struct	ban_data
{
    BAN_DATA *	next;
    bool	valid;
    sh_int	ban_flags;
    sh_int	level;
    char *	name;
};

/*
 * Money data
 */
struct money_type
{
    int		gold;		/* gold */
    int		silver;		/* silver */
    int		copper;		/* copper */
    int		fract;		/* fractional copper */
};

struct bank_data
{
    BANK_DATA *		next;
    CHAR_DATA *		owner;		/* owner of bank account */
    int			vnum;		/* vnum of bank */
    int			count;		/* count of items in storage */
    OBJ_DATA *		storage;	/* items in storage */
    MONEY		acct;		/* account info */
    bool		valid;
};


struct buf_type
{
    BUFFER *    next;
    sh_int      state;  /* error state of the buffer */
    sh_int      size;   /* size in k */
    int		length;	/* length of string */
    char *      string; /* buffer's string */
    bool        valid;
};


/*
 * Dreams
 */
struct dream_data
{
    DREAM_DATA *	next;
    int			vnum;
    int			type;
    int			sex;
    AFFECT_DATA *	affect;
    char *		author;
    char *		title;
    bitvector		dream_flags;
    EXT_BV		class_flags;
    EXT_BV		race_flags;
    DREAMSEG_DATA *	seg;
};

struct dreamseg_data
{
    DREAMSEG_DATA *	next;
    char *		text;
};

struct dreamlist_data
{
    DREAMLIST_DATA *	next;
    DREAM_DATA *	dream;
};


/*
 * Events
 */
struct event_data
{
    EVENT_DATA *	next_global;
    EVENT_DATA *	prev_global;
    EVENT_DATA *	next_local;
    EVENT_DATA *	prev_local;
    EVENT_DATA *	link;
    EVENT_FUN *		fun;
    char *		argument;
    int			value0;
    int			value1;
    int			value2;
    int			passes;
    int			type;
    int			ownertype;
    int			bucket;
    union
    {
	AREA_DATA *	area;
	CHAR_DATA *	ch;
	DESCRIPTOR_DATA *d;
	EXIT_DATA *	exit;
	OBJ_DATA *	obj;
	ROOM_INDEX_DATA *room;
    } owner;
};


/*
 * Flag type
 */
struct flag_type
{
    char *name;
    int bit;
    bool settable;
};


/*
 * Project data structure
 */
struct	project_data
{
    PROJECT_DATA *	next;
    int			vnum;
    int			priority;
    char *		name;
    char *		assigner;
    char *		assignee;
    time_t		date;
    time_t		deadline;
    time_t		completed;
    char *		status;
    char *		description;
    char *		progress;
    bool		fCompleted;
    bool		deleted;
};

/*
 * Project priorities
 */
#define PRI_CRITICAL		0
#define PRI_HIGH		1
#define PRI_MEDIUM		2
#define PRI_LOW			3


/*
 * Data used when recompiling
 */
struct rebuild_data
{
    int		pid;
    int		status;
    int		state;
    time_t	time;
};


/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

#define WIND_CALM		    0
#define WIND_ZEPHYR		    1
#define WIND_LIGHT		    2
#define WIND_MODERATE		    3
#define WIND_STRONG		    4
#define WIND_GALE		    5
#define WIND_HURRICANE		    6

struct	time_info_data
{
    int		hour;
    int		day;
    int		month;
    int		year;
};

struct	weather_data
{
    int		mmhg;
    int		change;
    int		sky;
    int		sunlight;
    int		wind_dir;
    int		wind_speed;
};


/*
 * Connected state for a socket.
 */
#define CON_PLAYING			 0
#define CON_GET_NAME			 1
#define CON_GET_OLD_PASSWORD		 2
#define CON_CONFIRM_NEW_NAME		 3
#define CON_GET_NEW_PASSWORD		 4
#define CON_CONFIRM_NEW_PASSWORD	 5
#define CON_GET_NEW_LNAME		 6
#define CON_GET_NEW_SEX			 7
#define CON_GET_CONTINENT		 8
#define CON_GET_NEW_RACE		10
#define CON_GET_NEW_CLASS		12
#define CON_GET_DESCRS			13
#define CON_GET_SHORT_DESCR		14
#define CON_GET_LONG_DESCR		15
#define CON_GET_ETHOS           16
#define CON_ACK_ALIGNMENT		17
#define CON_GET_ALIGNMENT		18
#define CON_DEFAULT_CHOICE		19
#define CON_GEN_GROUPS			20
#define CON_PICK_WEAPON			21
#define CON_READ_IMOTD			22
#define CON_READ_MOTD			23
#define CON_BREAK_CONNECT		24
#define CON_HOTBOOT_RECOVER		25
#define CON_GET_ANSI			26
#define CON_GET_HAIR_COLOR		27
#define CON_GET_HAIR_ADJ        28
#define CON_GET_EYE_ADJ         29
#define CON_GET_EYE_COLOR		30



/*
 * Death states, for a dead player character
 */
#define	DEATH_NONE	0
#define DEATH_ASK	1
#define DEATH_CONFIRM	2
#define DEATH_SEARCH	3
#define DEATH_FOUND	4
#define DEATH_RESURRECT	5


/*
 * Dream flags
 */
#define DREAM_FORCE	(A)

/*
 * Dream types
 */
#define DREAM_NORMAL	 0
#define DREAM_NIGHTMARE	 1


/*
 * OLC editor state
 */
#define ED_NONE		 0
#define ED_AREA		 1
#define ED_BOARD	 2
#define ED_CLAN		 3
#define ED_CMD		 4
#define ED_DREAM	 5
#define ED_EPROG	 6
#define ED_GROUP	 7
#define ED_HELP		 8
#define ED_MOBILE	 9
#define ED_MPROG	10
#define ED_OBJECT	11
#define ED_OPROG	12
#define ED_PROJECT	13
#define ED_RACE		14
#define ED_ROOM		15
#define ED_RPROG	16
#define ED_SECURITY	17
#define ED_SKILL	18
#define ED_SOCIAL	19
#define ED_TRAVEL	20
#define FORGE_OBJECT	21
#define RENAME_OBJECT	22


/*
 * Descriptor (channel) structure.
 */
struct	descriptor_data
{
    DESCRIPTOR_DATA *	next;
    DESCRIPTOR_DATA *	snoop_by;
    EVENT_DATA *	event_first;
    EVENT_DATA *	event_last;
    CHAR_DATA *		character;
    CHAR_DATA *		original;
    char *		ip;
    char *		host;
    sh_int		descriptor;
    sh_int		connected;
    bool		fcommand;
    int			idle;
    char		inbuf		[4 * MAX_INPUT_LENGTH];
    char		incomm		[MAX_INPUT_LENGTH];
    char		inlast		[MAX_INPUT_LENGTH];
    int			repeat;
    char *		outbuf;
    int			outsize;
    int			outtop;
    char *		showstr_head;
    char *		showstr_point;
    char **		pString;
    void *		pEdit;		/* what's being edited */
    void *		inEdit;		/* nested edit */
    int			editor;		/* which OLC editor is in use */
    int			editin;		/* nested editing */
    char		lastcolor;
    int			ride_dir;
    bool		riding;
    bool		valid;
    bool		ansi;
};



/*
 * Attribute bonus structures.
 */
struct	str_app_type
{
    sh_int	tohit;
    sh_int	todam;
    sh_int	carry;
    sh_int	wield;
};

struct	int_app_type
{
    sh_int	learn;
};

struct	wis_app_type
{
    sh_int	practice;
};

struct	dex_app_type
{
    sh_int	defensive;
};

struct	con_app_type
{
    sh_int	hitp;
    sh_int	shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3
#define TO_ALL		    4



/*
 * Help table types.
 */
struct	help_data
{
    HELP_DATA *	next;
    AREA_DATA *	area;
    int		level;
    int		ed_level;
    char *	keyword;
    char *	text;
};


/*
 * Ignore states.
 */
#define IGNORE_SET		A	/* Must always be set */
#define IGNORE_CHANNELS		B
#define IGNORE_TELLS		C

/*
 * Ignore data, for ignore command.
 */
struct ignore_data
{
    IGNORE_DATA *	next;
    IGNORE_DATA *	prev;
    bitvector		flags;
    char *		name;
    bool		valid;
};


/*
 * Last command buffer
 */
#define LCB_SIZE	8
struct last_cmd_buf
{
    time_t		b_time;
    DESCRIPTOR_DATA *	d;
    char		cmd[MAX_INPUT_LENGTH];
};


/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct	shop_data
{
    SHOP_DATA *	next;			/* Next shop in list		*/
    sh_int	keeper;			/* Vnum of shop keeper mob	*/
    sh_int	qkeeper;			/* Vnum of shop keeper mob	*/
    sh_int	buy_type [MAX_TRADE];	/* Item types shop will buy	*/
    sh_int	profit_buy;		/* Cost multiplier for buying	*/
    sh_int	profit_sell;		/* Cost multiplier for selling	*/
    sh_int	open_hour;		/* First opening hour		*/
    sh_int	close_hour;		/* First closing hour		*/
};


/*
 * Skill info, used to determine how well a player knows a skill.
 */
struct skill_data
{
    int		percent;	/* old "learned" value */
    int		usage;		/* # of times player has used skill */
    int		learned_age;	/* age at which player first acquired skill */
    int		used_age;	/* age at which player last used skill */
};


/*
 * Per-class stuff.
 */

#define MAX_GUILD 	2
#define MAX_STATS 	5
#define STAT_STR 	0
#define STAT_INT	1
#define STAT_WIS	2
#define STAT_DEX	3
#define STAT_CON	4

#define BASE_NONE	0
#define BASE_CLERIC	1
#define	BASE_MAGE	2
#define BASE_ROGUE	3
#define BASE_FIGHTER	4

#define TIER_NONE	0
#define TIER_ONE	1
#define TIER_TWO	2
#define TIER_THREE	3
#define TIER_FOUR	4

struct	class_type
{
    char *	name;			/* The full name of the class	*/
    char 	who_name	[4];	/* Three-letter name for 'who'	*/
    char *	plural;			/* pluralized form of name	*/
    sh_int	attr_prime;		/* Prime attribute		*/
    sh_int	weapon;			/* First weapon			*/
    sh_int	guild[MAX_GUILD];	/* Vnum of guild rooms		*/
    sh_int	skill_adept;		/* Maximum skill level		*/
    sh_int	thac0_00;		/* Thac0 for level  0		*/
    sh_int	thac0_99;		/* Thac0 for level 99		*/
    sh_int	hp_min;			/* Min hp gained on leveling	*/
    sh_int	hp_max;			/* Max hp gained on leveling	*/
    bitvector	flags;			/* Class gains mana on level	*/
    char *	base_group;		/* base skills gained		*/
    char *	default_group;		/* default skills gained	*/
    int *	class_value;		/* Index to class		*/
    int		tier;			/* Class tier			*/
    int		max_align;		/* Maximum alignment		*/
    int		min_align;		/* Minimum alignment		*/
    int     max_ethos;      /* Maximum ethos        */
    int     min_ethos;      /* Minimum ethos        */
    int		base_class;		/* Root class for this class	*/
};

/*
 * Userlist
 */
struct userlist_data
{
    USERLIST_DATA *		next;
    char *			name;
    char *			host;
    time_t			id;
    time_t			lastlogon;
    int				level;
    char *                      class;
    char *                      race;
    int				logcount;
    int				played;
    bool			valid;
};

struct weapon_type
{
    char *	name;
    int	vnum;
    sh_int	type;
    sh_int	*gsn;
};

struct wiznet_type
{
    char *	name;
    long 	flag;
    int		level;
};

struct attack_type
{
    char *	name;			/* name */
    char *	noun;			/* message */
    int   	damage;			/* damage class */
};

struct race_type
{
    char *	name;			/* call name of the race */
    char 	who_name[6];
    bool	pc_race;		/* can be chosen by pcs */
    bitvector	act;			/* act bits for the race */
    EXT_BV	aff;			/* aff bits for the race */
    EXT_BV	off;			/* off bits for the race */
    bitvector	imm;			/* imm bits for the race */
    bitvector	res;			/* res bits for the race */
    bitvector	vuln;			/* vuln bits for the race */
    bitvector	form;			/* default form flag for the race */
    bitvector	parts;			/* default parts for the race */
    int		max_align;		/* max alignment for PC's */
    int		min_align;		/* min alignment for PC's */
    int     max_ethos;      /* max ethos for PC's */
    int     min_ethos;      /* min ethos for PC's */
    int		size;			/* default size for the race */
    int *	race_value;		/* index number of race */
    int		weight_min;		/* minimum weight (roughly) */
    int		weight_max;		/* maximum weight (roughly) */
    sh_int	points;			/* cost in points of the race */
    sh_int	class_mult[MAX_CLASS];	/* exp multiplier for class, * 100 */
    char *	skills[MAX_RACE_SKILLS];/* bonus skills for the race */
    sh_int 	stats[MAX_STATS];	/* starting stats */
    sh_int	max_stats[MAX_STATS];	/* maximum stats */
    bool	kingdom_list[MAX_KINGDOM]; /* where they can be found */
};


/*
 *
 */
struct armor_val_type
{
    int		level;
    int		value;
};

/*
 * Hitdice table struct
 * Used by automob in OLC
 */
struct hitdice_type
{
    int		level;
    int		hit[3];
    int		dam[3];
    int		mana[3];
    int		ac;
    int		hitroll;
};

/*
 * Weapon default values
 * Used by autoset in OLC
 */
struct weapon_val_type
{
/*    int		level;	*/
    int		class;
    int		v1;
    int		v2;
    int		v5;
    int		v3;
};


struct spec_type
{
    char * 	name;			/* special function name */
    SPEC_FUN *	function;		/* the function */
};



/*
 * Data structure for boards
 */

struct board_data
{
    BOARD_DATA *	next;
    NOTE_DATA *		note_first;
    NOTE_DATA *		note_last;
    char *		name;
    char *		desc;
    int			read;		/* min level to read */
    int			write;		/* min level to write */
    int			expire;		/* default expiration */
    bool		valid;
};


/*
 * Data structure for board info
 * (last board read info)
 */

struct board_info
{
    BOARD_INFO *	next;
    char *		name;
    time_t		time;
    bool		valid;
};


/*
 * Data structure for notes.
 */

struct	note_data
{
    NOTE_DATA *		next;
    BOARD_DATA *	board;
    char *		sender;
    char *		date;
    char *		to_list;
    char *		subject;
    char *		text;
    time_t		date_stamp;
    int			on_board;
    bitvector		flags;
    bool		valid;
};

/*
 * Note flags
 */
#define NOTE_PROTECTED	(A)


/*
 * An affect.
 */
struct	affect_data
{
    AFFECT_DATA *	next;
    bool		valid;
    sh_int		where;
    sh_int		type;
    sh_int		level;
    sh_int		duration;
    sh_int		location;
    sh_int		modifier;
    int			bitvector;
};

/* where definitions */
#define TO_AFFECTS	0
#define TO_OBJECT	1
#define TO_IMMUNE	2
#define TO_RESIST	3
#define TO_VULN		4
#define TO_WEAPON	5
#define TO_SHIELDS	6


/*
 * A kill structure (indexed by level).
 */
struct	kill_data
{
    sh_int		number;
    sh_int		killed;
};



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_SUPERMOB	   3
#define MOB_VNUM_CORPSE 	   4
#define MOB_VNUM_LIFE		   5
#define MOB_VNUM_SLUG		   6
#define MOB_VNUM_ARMOR		   9
#define MOB_VNUM_HAWK		  10
#define MOB_VNUM_WOLF		  11
#define MOB_VNUM_FIRE_ELEMENTAL   12
#define MOB_VNUM_TIGER            13
#define MOB_VNUM_GREATER_WOLF     14
#define MOB_VNUM_WEAPON		   15
#define MOB_VNUM_KEY		   16
#define MOB_VNUM_WATER_ELEMENTAL   18
#define MOB_VNUM_AIR_ELEMENTAL	   19
#define MOB_VNUM_EARTH_ELEMENTAL   20
#define MOB_VNUM_FAMILIAR          21
#define MOB_VNUM_GGOLEM		  22
#define MOB_VNUM_GREYBLOB	  23
#define MOB_VNUM_ZOMBIE		   24
#define MOB_VNUM_MUMMY         25
#define MOB_VNUM_SKELETON      26
#define MOB_VNUM_CITYGUARD	 3060
#define MOB_VNUM_VAMPIRE	 3404

#define MOB_VNUM_PATROLMAN	   2106
#define GROUP_VNUM_TROLLS	   2100
#define GROUP_VNUM_OGRES	   2101




/* RT ASCII conversions -- used so we can have letters in this file */

#define A		  	1
#define B			2
#define C			4
#define D			8
#define E			16
#define F			32
#define G			64
#define H			128

#define I			256
#define J			512
#define K		        1024
#define L		 	2048
#define M			4096
#define N		 	8192
#define O			16384
#define P			32768

#define Q			65536
#define R			131072
#define S			262144
#define T			524288
#define U			1048576
#define V			2097152
#define W			4194304
#define X			8388608

#define Y			16777216
#define Z			33554432
#define aa			67108864 	/* doubled due to conflicts */
#define bb			134217728
#define cc			268435456
#define dd			536870912
#define ee			1073741824

#define BV00			( 1 << 00 )
#define BV01			( 1 << 01 )
#define BV02			( 1 << 02 )
#define BV03			( 1 << 03 )
#define BV04			( 1 << 04 )
#define BV05			( 1 << 05 )
#define BV06			( 1 << 06 )
#define BV07			( 1 << 07 )
#define BV08			( 1 << 08 )
#define BV09			( 1 << 09 )
#define BV10			( 1 << 10 )
#define BV11			( 1 << 11 )
#define BV12			( 1 << 12 )
#define BV13			( 1 << 13 )
#define BV14			( 1 << 14 )
#define BV15			( 1 << 15 )
#define BV16			( 1 << 16 )
#define BV17			( 1 << 17 )
#define BV18			( 1 << 18 )
#define BV19			( 1 << 19 )
#define BV20			( 1 << 20 )
#define BV21			( 1 << 21 )
#define BV22			( 1 << 22 )
#define BV23			( 1 << 23 )
#define BV24			( 1 << 24 )
#define BV25			( 1 << 25 )
#define BV26			( 1 << 26 )
#define BV27			( 1 << 27 )
#define BV28			( 1 << 28 )
#define BV29			( 1 << 29 )
#define BV30			( 1 << 30 )
#define BV31			( 1 << 31 )

/*
 * Structure types.
 * Used for cases where a struct might be either a char or an obj
 * cast to void *, such as act().
 */
#define STYPE_CHAR		 1
#define STYPE_OBJ		 2

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		(A)		/* Auto set for mobs	*/
#define ACT_SENTINEL	    	(B)		/* Stays in one room	*/
#define ACT_SCAVENGER	      	(C)		/* Picks up objects	*/
#define ACT_MOUNT		(E)		/* Mob is rideable	*/
#define ACT_AGGRESSIVE		(F)    		/* Attacks PC's		*/
#define ACT_STAY_AREA		(G)		/* Won't leave area	*/
#define ACT_WIMPY		(H)
#define ACT_PET			(I)		/* Auto set for pets	*/
#define ACT_TRAIN		(J)		/* Can train PC's	*/
#define ACT_PRACTICE		(K)		/* Can practice PC's	*/
#define ACT_ARTIFICER		(L)		/* Can rename objects	*/
#define	ACT_NOQUEST		(M)		/* Cannot be quest target */
#define ACT_UNDEAD		(O)
#define ACT_TETHERED		(T)		/* Mount is tethered */
#define ACT_NOALIGN		(U)
#define ACT_NOPURGE		(V)
#define ACT_OUTDOORS		(W)
#define ACT_INDOORS		(Y)
#define ACT_IS_HEALER		(aa)
#define ACT_GAIN		(bb)
#define ACT_UPDATE_ALWAYS	(cc)
#define ACT_IS_CHANGER		(dd)

/*
 * Class flags, in class_table[]
 */
#define CLASS_NOFLAGS		 0
#define CLASS_FMANA		(A)		/* Class gains mana on level */
#define CLASS_DISABLED		(B)		/* New chars can't choose class */
#define CLASS_MALEONLY		(C)
#define CLASS_FEMALEONLY	(D)

/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT		15
#define DAM_OTHER               16
#define DAM_HARM		17
#define DAM_CHARM		18
#define DAM_SOUND		19

/* OFF xbits for mobiles */
#define OFF_AREA_ATTACK         ( 0)
#define OFF_BACKSTAB            ( 1)
#define OFF_BASH                ( 2)
#define OFF_BERSERK             ( 3)
#define OFF_DISARM              ( 4)
#define OFF_DODGE               ( 5)
#define OFF_FADE                ( 6)
#define OFF_FAST                ( 7)
#define OFF_KICK                ( 8)
#define OFF_KICK_DIRT           ( 9)
#define OFF_PARRY               (10)
#define OFF_RESCUE              (11)
#define OFF_TAIL                (12)
#define OFF_TRIP                (13)
#define OFF_CRUSH		(14)
#define ASSIST_ALL       	(15)
#define ASSIST_ALIGN	        (16)
#define ASSIST_RACE    	     	(17)
#define ASSIST_PLAYERS      	(18)
#define ASSIST_GUARD        	(19)
#define ASSIST_VNUM		(20)

/* return values for check_imm */
#define IS_NORMAL		0
#define IS_IMMUNE		1
#define IS_RESISTANT		2
#define IS_VULNERABLE		3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT		(S)
#define IMM_SOUND		(T)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)

/* RES bits for mobs */
#define RES_SUMMON		(A)
#define RES_CHARM		(B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT		(S)
#define RES_SOUND		(T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)

/* VULN bits for mobs */
#define VULN_SUMMON		(A)
#define VULN_CHARM		(B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT		(S)
#define VULN_SOUND		(T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON		(Z)

/* body form */
#define FORM_EDIBLE             (A)
#define FORM_POISON             (B)
#define FORM_MAGICAL            (C)
#define FORM_INSTANT_DECAY      (D)
#define FORM_OTHER              (E)  /* defined by material bit */

/* actual form */
#define FORM_ANIMAL             (G)
#define FORM_SENTIENT           (H)
#define FORM_UNDEAD             (I)
#define FORM_CONSTRUCT          (J)
#define FORM_MIST               (K)
#define FORM_INTANGIBLE         (L)

#define FORM_BIPED              (M)
#define FORM_CENTAUR            (N)
#define FORM_INSECT             (O)
#define FORM_SPIDER             (P)
#define FORM_CRUSTACEAN         (Q)
#define FORM_WORM               (R)
#define FORM_BLOB		(S)

#define FORM_MAMMAL             (V)
#define FORM_BIRD               (W)
#define FORM_REPTILE            (X)
#define FORM_SNAKE              (Y)
#define FORM_DRAGON             (Z)
#define FORM_AMPHIBIAN          (aa)
#define FORM_FISH               (bb)
#define FORM_COLD_BLOOD		(cc)

/* body parts */
#define PART_HEAD               (A)
#define PART_ARMS               (B)
#define PART_LEGS               (C)
#define PART_HEART              (D)
#define PART_BRAINS             (E)
#define PART_GUTS               (F)
#define PART_HANDS              (G)
#define PART_FEET               (H)
#define PART_FINGERS            (I)
#define PART_EAR                (J)
#define PART_EYE		(K)
#define PART_LONG_TONGUE        (L)
#define PART_EYESTALKS          (M)
#define PART_TENTACLES          (N)
#define PART_FINS               (O)
#define PART_WINGS              (P)
#define PART_TAIL               (Q)
/* for combat */
#define PART_CLAWS              (U)
#define PART_FANGS              (V)
#define PART_HORNS              (W)
#define PART_SCALES             (X)
#define PART_TUSKS		(Y)

/* mob class */
#define MCLASS_MAGE		(A)
#define MCLASS_CLERIC		(B)
#define MCLASS_FIGHTER		(C)
#define MCLASS_ROGUE		(D)

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_NONE		 0
#define AFF_INVISIBLE		 1
#define AFF_DETECT_EVIL		 2
#define AFF_DETECT_INVIS	 3
#define AFF_DETECT_MAGIC	 4
#define AFF_DETECT_HIDDEN	 5
#define AFF_DETECT_GOOD		 6
#define AFF_SANCTUARY		 7
#define AFF_FAERIE_FIRE		 8
#define AFF_INFRARED		 9
#define AFF_CURSE		10
#define AFF_UNUSED_FLAG		11	/* unused */
#define AFF_POISON		12
#define AFF_PROTECT_EVIL	13
#define AFF_PROTECT_GOOD	14
#define AFF_SNEAK		15
#define AFF_HIDE		16
#define AFF_SLEEP		17
#define AFF_CHARM		18
#define AFF_FLYING		19
#define AFF_PASS_DOOR		20
#define AFF_HASTE		21
#define AFF_CALM		22
#define AFF_PLAGUE		23
#define AFF_WEAKEN		24
#define AFF_DARK_VISION		25
#define AFF_BERSERK		26
#define AFF_SWIM		27
#define AFF_REGENERATION        28
#define AFF_SLOW		29
#define AFF_DEATHGRIP           30
#define AFF_SEVERED             31
#define AFF_BLIND		32
//#define AFF_FORCE_SHIELD	33
//#define AFF_STATIC_SHIELD	34
//#define AFF_FLAME_SHIELD	35
#define AFF_PEACE		36
#define AFF_DANCING		37
#define AFF_CURSE_NATURE	38
#define AFF_WEARINESS		39
#define AFF_DREAM		40
#define AFF_DEAF                41
#define AFF_BLACK_MANTLE        42
/*
 * Bits for 'shielded_by'.
 * Used in #MOBILES
 */
#define SHLD_NONE		 0
#define SHLD_ACID		 1
#define SHLD_BLADE		 2
#define SHLD_FLAME		 3
#define SHLD_FORCE		 4
#define SHLD_GHOST		 5
#define SHLD_HOLY		 6
#define SHLD_ICE		 7
#define SHLD_LIGHT		 8
#define SHLD_PROTECT_ACID	 9
#define SHLD_PROTECT_AIR	10
#define SHLD_PROTECT_COLD	11
#define SHLD_PROTECT_EARTH	12
#define SHLD_PROTECT_ENERGY	13
#define SHLD_PROTECT_EVIL	14
#define SHLD_PROTECT_FIRE	15
#define SHLD_PROTECT_GOOD	16
#define SHLD_PROTECT_LIGHTNING	17
#define SHLD_PROTECT_WATER	18
#define SHLD_SANCTUARY		19
#define SHLD_SONIC		20
#define SHLD_STATIC		21
#define SHLD_UNHOLY		22
#define SHLD_BLADES		23
#define SHLD_BLACK_MANTLE       24

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL			0
#define SEX_MALE			1
#define SEX_FEMALE			2
#define SEX_EITHER			3

/* AC types */
#define AC_PIERCE			0
#define AC_BASH				1
#define AC_SLASH			2
#define AC_EXOTIC			3

/* dice */
#define DICE_NUMBER			0
#define DICE_TYPE			1
#define DICE_BONUS			2

/*
 * dice difficulty levels
 */
#define DICE_NOSET			0	/* don't use automob values */
#define DICE_EASY			1
#define DICE_MEDIUM			2
#define DICE_HARD			3

/* size */
#define SIZE_TINY			0
#define SIZE_SMALL			1
#define SIZE_MEDIUM			2
#define SIZE_LARGE			3
#define SIZE_HUGE			4
#define SIZE_GIANT			5
#define SIZE_TITANIC			6
#define MAX_SIZE			7


/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_SILVER_ONE	      1
#define OBJ_VNUM_GOLD_ONE	      2
#define OBJ_VNUM_GOLD_SOME	      3
#define OBJ_VNUM_SILVER_SOME	      4
#define OBJ_VNUM_COINS		      5
#define OBJ_VNUM_COPPER_ONE	      6
#define OBJ_VNUM_FRACT_ONE	      7
#define OBJ_VNUM_COPPER_SOME	      8
#define OBJ_VNUM_FRACT_SOME           9

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_GUTS		     16
#define OBJ_VNUM_BRAINS		     17

#define OBJ_VNUM_LEGS                18
#define OBJ_VNUM_BERRY		     19
#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_DISC		     23
#define OBJ_VNUM_HERBAL_TEA	     24
#define OBJ_VNUM_PORTAL		     25
#define OBJ_VNUM_NOTE		     26
#define OBJ_VNUM_ROSE		     27

#define OBJ_VNUM_BLOOD		     28
#define OBJ_VNUM_BLOOD_TRAIL	     29
/* Vnums 30-39 reserved for quest items. */
#define OBJ_VNUM_BEARCLAW	     40
#define OBJ_VNUM_WIZARDRY_1	     41
#define OBJ_VNUM_WIZARDRY_2	     42
#define OBJ_VNUM_DBOOT		     43
#define OBJ_VNUM_LSHIELD	     44

#define OBJ_VNUM_ARROW		     45
#define OBJ_VNUM_BLOODSTAIN	     46

/* More body parts */
#define OBJ_VNUM_TAIL		     47
#define OBJ_VNUM_WINGS		     48
#define OBJ_VNUM_DUMMY               49
/* #define OBJ_VNUM_PIT		   1800 */

#define OBJ_VNUM_RING                  601                                                                                   
#define OBJ_VNUM_BRACELET              602                                                                                   
#define OBJ_VNUM_GLOVES                603                                                                                   
#define OBJ_VNUM_HELMET                604                                                                                   
#define OBJ_VNUM_NECKLACE              605
#define OBJ_VNUM_BEGINING              606
#define OBJ_VNUM_SCHOOL_SATCHEL        607
#define OBJ_VNUM_SCHOOL_SANDLES        608
#define OBJ_VNUM_SCHOOL_WATER_SKIN     609
#define OBJ_VNUM_BREAD                 610
#define OBJ_VNUM_SCHOOL_LAMP       611
#define OBJ_VNUM_SCHOOL_MACE	   900
#define OBJ_VNUM_SCHOOL_DAGGER	   901
#define OBJ_VNUM_SCHOOL_SWORD	   902
#define OBJ_VNUM_SCHOOL_SPEAR	   903
#define OBJ_VNUM_SCHOOL_STAFF	   904
#define OBJ_VNUM_SCHOOL_AXE	       905
#define OBJ_VNUM_SCHOOL_FLAIL	   906
#define OBJ_VNUM_SCHOOL_WHIP	   907
#define OBJ_VNUM_SCHOOL_POLEARM    908
#define OBJ_VNUM_SCHOOL_HAMMER	   909

#define OBJ_VNUM_SCHOOL_VEST	   910
#define OBJ_VNUM_SCHOOL_SHIELD	   911
#define OBJ_VNUM_SCHOOL_BANNER     912
#define OBJ_VNUM_SCHOOL_BOOTS	   913
#define OBJ_VNUM_SCHOOL_SHIRT	   914
#define OBJ_VNUM_SCHOOL_PANTS	   915
#define OBJ_VNUM_MAP		       916

/* Skinning and butchering */
#define OBJ_VNUM_STEAK 50
#define OBJ_VNUM_SKIN  51




/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_CLOTHING		     11
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
/* #define ITEM_PROTECT		     27 */
#define ITEM_MAP		     28
#define ITEM_PORTAL		     29
#define ITEM_WARP_STONE		     30
/*  #define ITEM_ROOM_KEY	     31*/
#define ITEM_GEM		     32
#define ITEM_JEWELRY		     33
/* #define ITEM_JUKEBOX		     34 */
#define ITEM_LENS		     35
#define ITEM_QUESTITEM		     36
#define ITEM_HERB		     37
#define ITEM_BERRY		     38
#define ITEM_NOTEBOARD		     39
#define ITEM_PAPER		     40
#define ITEM_INK		     41
#define ITEM_PERMKEY		     42
#define ITEM_QUIVER		     43
#define ITEM_ARROW		     44
#define ITEM_BOOK		     45
#define ITEM_SHEATH		     46
#define ITEM_TOKEN           47


/*
 * Material types
 */
#define MAT_NONE	 0
#define MAT_AMBER	 1
#define	MAT_BLOODSTONE	 2
#define	MAT_BONE	 3
#define	MAT_BRASS	 4
#define MAT_BREAD	 5
#define	MAT_BRONZE	 6
#define MAT_CHARCOAL	 7
#define MAT_CHITIN	 8
#define MAT_CLAY	 9
#define	MAT_CLOTH	10
#define MAT_COAL	11
#define	MAT_COPPER	12
#define MAT_CRYSTAL	13
#define MAT_DIAMOND	14
#define MAT_DIRT	15
#define	MAT_DRAGONSCALE	16
#define	MAT_EBONY	17
#define MAT_EMERALD	18
#define	MAT_ENERGY	19
#define MAT_FIBER	20
#define MAT_FISHSCALE	21
#define MAT_FLAX	22
#define	MAT_FLESH	23
#define	MAT_FOOD	24
#define MAT_FUR		25
#define	MAT_GEMSTONE	26
#define	MAT_GLASS	27
#define	MAT_GOLD	28
#define	MAT_GRANITE	29
#define MAT_GUANO	30
#define MAT_HEMP	31
#define	MAT_IRON	32
#define MAT_JADE	33
#define	MAT_LEAD	34
#define	MAT_LEATHER	35
#define	MAT_MARBLE	36
#define MAT_MERCURY	37
#define	MAT_MITHRIL	38
#define	MAT_OBSIDIAN	39
#define MAT_OIL		40
#define MAT_PAPER	41
#define MAT_PARCHMENT	42
#define MAT_PEARL	43
#define	MAT_PLATINUM	44
#define	MAT_PORCELAIN	45
#define MAT_RUBY	46
#define MAT_RUST	47
#define MAT_SAND	48
#define	MAT_SILK	49
#define	MAT_SILVER	50
#define MAT_SINEW	51
#define MAT_SKIN	52
#define	MAT_STEEL	53
#define	MAT_STONE	54
#define MAT_STRAW	55
#define MAT_SULFUR	56
#define MAT_TIN		57
#define MAT_VEGETABLE	58
#define MAT_VELVET	59
#define	MAT_WATER	60
#define	MAT_WAX		61
#define	MAT_WOOD	62
#define	MAT_WOOL	63
#define MAT_ZINC	64
#define MAT_SAPPHIRE	65
#define MAT_FEATHER	66
#define MAT_SHELL	67
#define MAT_SALT	68
#define MAT_IVORY	69


/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		(A)
#define ITEM_HUM		(B)
#define ITEM_DARK		(C)
#define ITEM_HIDDEN		(D)
#define ITEM_EVIL		(E)
#define ITEM_INVIS		(F)
#define ITEM_MAGIC		(G)
#define ITEM_NODROP		(H)
#define ITEM_BLESS		(I)
#define ITEM_ANTI_GOOD		(J)
#define ITEM_ANTI_EVIL		(K)
#define ITEM_ANTI_NEUTRAL	(L)
#define ITEM_NOREMOVE		(M)
#define ITEM_INVENTORY		(N)
#define ITEM_NOPURGE		(O)
#define ITEM_ROT_DEATH		(P)
#define ITEM_VIS_DEATH		(Q)
#define ITEM_INVIS_DAY		(R)
#define ITEM_NONMETAL		(S)
#define ITEM_NOLOCATE		(T)
#define ITEM_MELT_DROP		(U)
#define ITEM_HAD_TIMER		(V)
#define ITEM_SELL_EXTRACT	(W)
#define ITEM_INVIS_NIGHT	(X)
#define ITEM_BURN_PROOF		(Y)
#define ITEM_NOUNCURSE		(Z)
#define ITEM_DEATH              (aa)
#define ITEM_WHISTLE		(bb)
#define ITEM_QUESTOBJ		(cc)
#define ITEM_NOLIST		(dd)
#define ITEM_NOIDENTIFY		(ee)
//#define ITEM_QUESTOBJ       (ff)

/*
 * Extra2 Flags
 * Used in #OBJECTS
 */
#define ITEM2_NONE	(A)
/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		(A)
#define ITEM_WEAR_FINGER	(B)
#define ITEM_WEAR_NECK		(C)
#define ITEM_WEAR_BODY		(D)
#define ITEM_WEAR_HEAD		(E)
#define ITEM_WEAR_LEGS		(F)
#define ITEM_WEAR_FEET		(G)
#define ITEM_WEAR_HANDS		(H)
#define ITEM_WEAR_ARMS		(I)
#define ITEM_WEAR_SHIELD	(J)
#define ITEM_WEAR_ABOUT		(K)
#define ITEM_WEAR_WAIST		(L)
#define ITEM_WEAR_WRIST		(M)
#define ITEM_WIELD		(N)
#define ITEM_HOLD		(O)
#define ITEM_NO_SAC		(P)
#define ITEM_WEAR_FLOAT		(Q)
#define ITEM_WEAR_CONTACT	(R)
#define ITEM_WEAR_FACE		(S)
#define ITEM_WEAR_EAR		(T)
#define ITEM_WEAR_CPATCH	(U)
#define ITEM_WEAR_RPATCH	(V)
#define ITEM_WEAR_SHOULDER	(W)
#define ITEM_WEAR_ANKLE		(X)
#define ITEM_WEAR_TAIL          (Y)
#define ITEM_WEAR_HORNS         (Z)
/* weapon class */
#define WEAPON_NONE		-1
#define WEAPON_EXOTIC		 0
#define WEAPON_SWORD		 1
#define WEAPON_DAGGER		 2
#define WEAPON_SPEAR		 3
#define WEAPON_MACE		 4
#define WEAPON_AXE		 5
#define WEAPON_FLAIL		 6
#define WEAPON_WHIP		 7
#define WEAPON_POLEARM		 8
#define WEAPON_HAMMER		 9
#define WEAPON_LONGSWORD	10
#define WEAPON_STAFF		11
#define WEAPON_STILETTO		12
#define WEAPON_CLUB		13
#define WEAPON_SCIMITAR		14
#define WEAPON_SHORTSWORD	15
#define WEAPON_BASTARDSWORD	16
#define WEAPON_TWOHANDSWORD	17
#define WEAPON_DIRK		18
#define WEAPON_LANCE		19
#define WEAPON_GREATMACE	20
#define WEAPON_GREATAXE		21
#define WEAPON_HORSEMACE	22
#define WEAPON_HORSEFLAIL	23
#define WEAPON_BOW		24
#define WEAPON_RAPIER		25
#define WEAPON_SABRE		26

#define WEAPON_BROADSWORD	28
#define WEAPON_CROSSBOW		29


/* weapon types */
#define WEAPON_FLAMING		(A)
#define WEAPON_FROST		(B)
#define WEAPON_VAMPIRIC		(C)
#define WEAPON_SHARP		(D)
#define WEAPON_VORPAL		(E)
#define WEAPON_TWO_HANDS	(F)
#define WEAPON_SHOCKING		(G)
#define WEAPON_POISON		(H)
#define WEAPON_TALKING		(I)
#define WEAPON_TALKING_MACE	(J)
#define WEAPON_GRIPPED		(K)
#define WEAPON_PESTILENCE	(L)
#define WEAPON_POWERLEECH	(M)

/* gate flags */
#define GATE_NORMAL_EXIT	(A)
#define GATE_NOCURSE		(B)
#define GATE_GOWITH		(C)
#define GATE_BUGGY		(D)
#define GATE_RANDOM		(E)
#define GATE_CLOSEABLE		(F)
#define GATE_PICKPROOF		(G)
#define GATE_CLOSED		(H)
#define GATE_LOCKED		(I)
#define GATE_WINDOW		(J)
#define GATE_OPAQUE		(K)

/* furniture flags */
#define STAND_AT		(A)
#define STAND_ON		(B)
#define STAND_IN		(C)
#define SIT_AT			(D)
#define SIT_ON			(E)
#define SIT_IN			(F)
#define REST_AT			(G)
#define REST_ON			(H)
#define REST_IN			(I)
#define SLEEP_AT		(J)
#define SLEEP_ON		(K)
#define SLEEP_IN		(L)
#define PUT_AT			(M)
#define PUT_ON			(N)
#define PUT_IN			(O)
#define PUT_INSIDE		(P)


/* Poison flags, for herbs */
#define POISONED_EATEN		(A)
#define POISONED_RUBBED		(B)
#define POISONED_BREWED		(C)


/*
 *  Invoke types
 */
#define INVOKE_NONE		0
#define INVOKE_MOB		1
#define INVOKE_OBJ		2
#define INVOKE_TRANS		3
#define INVOKE_MORPH		4
#define INVOKE_SPELL		5


/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_WEALTH		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVES		     20
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24
#define APPLY_SPELL_AFFECT	     25
#define APPLY_SPELL_LEVEL	     26
#define APPLY_SIZE		     27
#define APPLY_REGENERATION	     28

/*
 * Values for containers and sheaths (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		     (A)
#define CONT_PICKPROOF		     (B)
#define CONT_CLOSED		     (C)
#define CONT_LOCKED		     (D)
#define CONT_PUT_ON		     (E)
#define SHEATH_CONCEAL		     (F)


/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_DISPOSAL	      3
#define ROOM_VNUM_SPAM		      4
#define ROOM_VNUM_CHAT		      2
#define ROOM_VNUM_TEMPLE            900
#define ROOM_VNUM_MORGUE	    939
#define ROOM_VNUM_ALTAR		    940
#define ROOM_VNUM_SCHOOL	    601
#define ROOM_VNUM_NEWBIE_MORGUE	    939
#define ROOM_VNUM_RIFT		  13000
#define ROOM_VNUM_UNDERWATER       995

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		(A)
#define ROOM_SUNLIGHT		(B)
#define ROOM_NO_MOB		(C)
#define ROOM_INDOORS		(D)
#define ROOM_NO_MOUNT		(E)
#define ROOM_NO_MAGIC		(F)
#define ROOM_NO_IN		(G)
#define ROOM_NO_OUT		(H)
#define ROOM_NO_RESETMSG	(I)
#define ROOM_PRIVATE		(J)
#define ROOM_SAFE		(K)
#define ROOM_SOLITARY		(L)
#define ROOM_PET_SHOP		(M)
#define ROOM_NO_RECALL		(N)
#define ROOM_IMP_ONLY		(O)
#define ROOM_GODS_ONLY		(P)
#define ROOM_HEROES_ONLY	(Q)
#define ROOM_NEWBIES_ONLY	(R)
#define ROOM_LAW		(S)
#define ROOM_NOWHERE		(T)
#define ROOM_BANK		(U)
#define ROOM_VIRTUAL		(V)
#define ROOM_NO_FLOOR		(W)
#define ROOM_NO_FORMAT		(X)
#define ROOM_BRAWL		(Y)
#define ROOM_SILENT		(Z)
#define ROOM_UNFINISHED		(aa)
#define ROOM_NOSCAN		(bb)


/*
 * Room affect flags.
 * Used in #ROOMS.
 */
#define ROOM_AFF_NONE		 0
#define ROOM_AFF_BLIND		(B)
#define ROOM_AFF_CURSE		(C)
#define ROOM_AFF_PLAGUE		(D)
#define ROOM_AFF_POISON		(E)
#define ROOM_AFF_SLEEP		(F)
#define ROOM_AFF_SLOW		(G)
#define ROOM_AFF_WEAKEN		(H)


/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NONE		     -1
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5
#define DIR_NORTHWEST		      6
#define DIR_NORTHEAST		      7
#define DIR_SOUTHWEST		      8
#define DIR_SOUTHEAST		      9
#define MAX_DIR			     10


/*
 * Vehicle types.
 * Used in #ROOMS.
 */
#define VEHICLE_NONE		0
#define VEHICLE_LAND		1
#define VEHICLE_SEA		2
#define VEHICLE_AIR		3
#define VEHICLE_AMPHIB		4


/*
 * Kingdoms
 */
#define KINGDOM_NONE		0
#define KINGDOM_BLACKMERE		1
#define KINGDOM_EVERVALE		2
#define KINGDOM_WINTERGARDE		3
#define KINGDOM_DRUUNA		4
#define KINGDOM_AMBERFALL		5
#define KINGDOM_TINKERWICK		6
#define KINGDOM_GHOSTWALLOW	7
/* #define MAX_KINGDOM		8 */

/*
 * Event definitions
 */
#define EVENT_NONE		0

#define EVENT_OWNER_UNOWNED	0
#define EVENT_OWNER_NONE	1
#define EVENT_OWNER_AREA	2
#define EVENT_OWNER_CHAR	3
#define EVENT_OWNER_DESC	4
#define EVENT_OWNER_EXIT	5
#define EVENT_OWNER_GAME	6
#define EVENT_OWNER_OBJ		7
#define EVENT_OWNER_ROOM	8

#define EVENT_DUMMY		 1

/*
 * The values below may be duplicates for different event types.
 * However none should be equal to EVENT_DUMMY.
 */

/* Definitions for EVENT_OWNER_CHAR events */
#define EVENT_CHAR_MPDELAY	 2
#define EVENT_CHAR_HEAL		 3
#define EVENT_CHAR_NOPK		 4
#define EVENT_CHAR_DREAM	 5

/* Definitions for EVENT_OWNER_DESC events */
#define EVENT_DESC_IDLE		 2
#define EVENT_DESC_LOGIN	 3
#define EVENT_DESC_KEEPALIVE     4

/* Definitions for EVENT_OWNER_EXIT events */
#define EVENT_EXIT_MPDELAY	 2

/* Definitions for EVENT_OWNER_GAME events */
#define EVENT_GAME_SAVE_USERLIST 2
#define EVENT_GAME_SAVE_GREET	 3

/* Definitions for EVENT_OWNER_OBJ events */
#define EVENT_OBJ_MPDELAY	 2
#define EVENT_OBJ_FALL		 3

/* Definitions for EVENT_OWNER_ROOM events */
#define EVENT_ROOM_MPDELAY	 2
#define EVENT_ROOM_SAIL		 3


/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      (A)
#define EX_CLOSED		      (B)
#define EX_LOCKED		      (C)
#define EX_PICKPROOF		      (F)
#define EX_NOPASS		      (G)
#define EX_EASY			      (H)
#define EX_HARD			      (I)
#define EX_INFURIATING		      (J)
#define EX_NOCLOSE		      (K)
#define EX_NOLOCK		      (L)
#define EX_BASHED		      (M)
#define EX_BASHPROOF		      (N)
#define EX_HIDDEN		      (O)
#define EX_NO_MOB		      (P)
#define EX_SECRET		      (Q)
#define EX_NORESET		      (R)


/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_NONE		     -1
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNDERWATER		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_SWAMP		     11
#define SECT_ROAD		     12
#define SECT_OCEAN		     13

#define SECT_UNDERGROUND	     14
#define SECT_GRAVEYARD		     15
#define SECT_TREECITY		     16
#define SECT_ELFFOREST		     17
#define SECT_JUNGLE		     18
#define SECT_ARCTIC		     19
#define SECT_BEACH		     20
#define	SECT_BOILING		     21
#define SECT_LAKE		     22
#define SECT_RIVER		     23
#define SECT_WASTELAND		     24

#define SECT_MAX		     25


/*
 * Sector elevations.
 * Used for map in overland areas.
 */
#define ELEV_AIR			 0
#define ELEV_CITY			 4
#define ELEV_DESERT			 4
#define ELEV_ENTER			 0
#define ELEV_FIELD			 2
#define ELEV_FOREST			 6
#define ELEV_HILLS			 8
#define ELEV_INSIDE			 0
#define ELEV_MOUNTAIN			10
#define ELEV_ROAD			 2
#define ELEV_SWAMP			 2
#define ELEV_WATER_NOSWIM		 2
#define ELEV_WATER_SWIM			 2
#define ELEV_WATER_OCEAN		 2
#define ELEV_UNUSED			 0
#define ELEV_ARCTIC			 4
#define ELEV_WASTELAND			10
#define ELEV_NONE			-1


/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		-1
#define WEAR_LIGHT		 0
#define WEAR_FINGER_L		 1
#define WEAR_FINGER_R		 2
#define WEAR_NECK_1		 3
#define WEAR_NECK_2		 4
#define WEAR_BODY		 5
#define WEAR_HEAD		 6
#define WEAR_LEGS		 7
#define WEAR_FEET		 8
#define WEAR_HANDS		 9
#define WEAR_ARMS		10
#define WEAR_SHIELD		11
#define WEAR_ABOUT		12
#define WEAR_WAIST		13
#define WEAR_WRIST_L		14
#define WEAR_WRIST_R		15
#define WEAR_WIELD		16
#define WEAR_HOLD		17
#define WEAR_FLOAT		18
#define WEAR_DUAL		19
#define WEAR_EYES		20
#define WEAR_FACE		21
#define WEAR_EAR_L		22
#define WEAR_EAR_R		23
#define WEAR_CPATCH		24
#define WEAR_RPATCH		25
#define WEAR_SHOULDER		26
#define WEAR_ANKLE_L		27
#define WEAR_ANKLE_R		28
#define WEAR_TAIL		29
#define WEAR_HORNS              30
#define MAX_WEAR		31



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2
#define COND_HUNGER		      3
#define COND_TIRED		      4
#define COND_RFU5		      5
#define COND_RFU6		      6
#define COND_RFU7		      7

#define MAX_COND		     48	/* max for hunger, thirst etc */

/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_RESTING		      5
#define POS_SITTING		      6
#define POS_FIGHTING		      7
#define POS_STANDING		      8




/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		(A)		/* Don't EVER set.	*/

/* RT auto flags */
#define PLR_AUTOASSIST		(C)
#define PLR_AUTOEXIT		(D)
#define PLR_AUTOLOOT		(E)
#define PLR_AUTOSAC             (F)
#define PLR_AUTOCOINS		(G)
#define PLR_AUTOSPLIT		(H)

/* RT personal flags */
#define PLR_HOLYLIGHT		(N)
#define PLR_CANLOOT		(P)
#define PLR_NOSUMMON		(Q)
#define PLR_NOFOLLOW		(R)
#define PLR_AUTODAMAGEDEALT     (S)
#define PLR_BLINK		(T)

/* penalty flags */
#define PLR_PERMIT		(U)
#define PLR_LOG			(W)
#define PLR_DENY		(X)
#define PLR_FREEZE		(Y)
#define PLR_THIEF		(Z)
#define PLR_KILLER		(aa)
#define PLR_NO_EMOTE		(bb)
#define PLR_SWITCHED		(cc)
#define PLR_QUESTOR		(dd)
#define PLR_NOEXP               (ee)
/*
 * ACT2 bits for players
 */
#define CODER			(A)
#define HBUILDER		(B)
#define PLR_FMETER		(C)
#define PLR_SEE_COL_CODE	(D)
#define PLR_PEEK		(E)
#define PLR_AUTOCHANNEL		(F)
#define PLR_PLOADED		(G)
#define PLR_CODING		(H)
#define PLR_MARKING		(I)
#define PLR_RFLAGS		(J)
#define PLR_AUTOOPEN		(K)
#define PLR_AUTOMOVE		(L)
#define PLR_POSE		(M)
#define PLR_NOPUNCT		(N)
#define PLR_OUTCAST		(O)
#define PLR_NOCOPY		(P)
#define PLR_KEEPALIVE           (Q)
#define PLR_AUTOTICK              (R)
/*
 *  Channel bits
 */
#define CHANNEL_NONE		0
#define CHANNEL_AUCTION		A
#define CHANNEL_CLAN		B
#define CHANNEL_CODER		C

#define CHANNEL_GOSSIP		E

#define CHANNEL_HERO		G
#define CHANNEL_IMMTALK		H
#define CHANNEL_IMPLEMENTOR	I

#define CHANNEL_NEWBIE		K
#define CHANNEL_OOC		L
#define CHANNEL_QUESTION	M

#define CHANNEL_SHOUT		O
#define CHANNEL_ADMIN		P
#define CHANNEL_YELL		Q
#define CHANNEL_PRAY		R


/* RT comm flags -- may be used on both mobs and chars */
#define COMM_QUIET              (A)
#define COMM_DEAF            	(B)
//#define COMM_NOWIZ              (C)
//#define COMM_NOAUCTION          (D)
//#define COMM_NOGOSSIP           (E)
//#define COMM_NOQUESTION         (F)
//#define COMM_NOMUSIC            (G)
//#define COMM_NOCLAN		(H)
//#define COMM_NOQUOTE		(I)
//#define COMM_SHOUTSOFF		(J)

/* display flags */
#define COMM_DETAIL		(K)
#define COMM_COMPACT		(L)
#define COMM_BRIEF		(M)
#define COMM_PROMPT		(N)
#define COMM_COMBINE		(O)
#define COMM_TELNET_GA		(P)
#define COMM_SHOW_AFFECTS	(Q)
#define COMM_NOGRATS		(R)
#define COMM_VNUMS		(S)

/* penalties */
#define COMM_NOEMOTE		(T)
#define COMM_NOSHOUT		(U)
#define COMM_NOTELL		(V)
#define COMM_NOCHANNELS		(W)
#define COMM_SNOOP_PROOF	(Y)
#define COMM_AFK		(Z)

/* Confirmation flags */
#define CONF_DELETE		(A)


/* Info channel flags */
#define INFO_ON			(A)
#define INFO_DEATHS		(B)
#define INFO_LEVELS		(C)
#define INFO_LOGINS		(D)

/* WIZnet flags */
#define WIZ_ON			(A)
#define WIZ_TICKS		(B)
#define WIZ_LOGINS		(C)
#define WIZ_SITES		(D)
#define WIZ_LINKS		(E)
#define WIZ_DEATHS		(F)
#define WIZ_RESETS		(G)
#define WIZ_MOBDEATHS		(H)
#define WIZ_FLAGS		(I)
#define WIZ_PENALTIES		(J)
#define WIZ_SACCING		(K)
#define WIZ_LEVELS		(L)
#define WIZ_SECURE		(M)
#define WIZ_SWITCHES		(N)
#define WIZ_SNOOPS		(O)
#define WIZ_RESTORE		(P)
#define WIZ_LOAD		(Q)
#define WIZ_NEWBIE		(R)
#define WIZ_PREFIX		(S)
#define WIZ_SPAM		(T)
#define WIZ_BUILD		(U)
#define WIZ_BUG			(V)
#define WIZ_MEMORY		(W)
#define WIZ_AUCTION		(X)
#define WIZ_DAMAGE		(Y)
#define WIZ_BUILDBUG		(Z)
#define WIZ_CLANS		(aa)
#define WIZ_COMBAT		(bb)

/*
 * Marriage/spouse state
 */
#define SPOUSE_NONE		0
#define SPOUSE_PROPOSE		1
#define SPOUSE_BETROTHED	2
#define SPOUSE_MARRIED		3
#define SOOUSE_DIVORCED		4


/*
 * Clan data
 */
struct	clan_data
{
    CLAN_DATA *		next;
    char *		name;
    char *		who_name;
    char *		description;
    char *		leader;
    char *		ctitle[MAX_CLAN_LEVELS][2];
    int			induct_level;
    int			outcast_level;
    int			vnum;
    int			morgue;
    int			recall;
    int			members;
    int			pkills;
    int			plosses;
    int			hall;
    int			box;
    int			bank;
    int			withdraw;
    int			honor;
    time_t		id;
    bool		fHouse;
    bool		pkill;
};

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    AREA_DATA *		area;
    MPROG_DATA *	mobprogs;
    SHOP_DATA *		pShop;
    SPEC_FUN *		spec_fun;
    int		vnum;
    bitvector		progtypes;
    sh_int		group;
    sh_int		count;
    sh_int		killed;
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    char *		clan;
    bitvector		act;
    EXT_BV		affected_by;
    EXT_BV		shielded_by;
    sh_int		alignment;
    sh_int      ethos;
    sh_int		level;
    sh_int		hitroll;
    sh_int		hit[3];
    sh_int		mana[3];
    sh_int		damage[3];
    sh_int		ac[4];
    sh_int 		dam_type;
    EXT_BV		off_flags;
    bitvector		imm_flags;
    bitvector		res_flags;
    bitvector		vuln_flags;
    int			autoset;
    sh_int		start_pos;
    sh_int		default_pos;
    sh_int		sex;
    sh_int		race;
    bitvector		class;
    EXT_BV		train;
    long		wealth;
    bitvector		form;
    bitvector		parts;
    int			size;
    int			weight;
    char *		material;
};

/* memory settings */
#define MEM_CUSTOMER	A
#define MEM_SELLER	B
#define MEM_HOSTILE	C
#define MEM_AFRAID	D

/* memory for mobs */
struct mem_data
{
    MEM_DATA 	*next;
    bool	valid;
    int		id;
    int 	reaction;
    time_t 	when;
};


/*
 * Structure for a generic list of chars
 */
struct	clist_data
{
    CLIST_DATA *	next;
    CHAR_DATA *		lch;
    DESCRIPTOR_DATA *	d;
    int			number;
};


/*
 * Structure for a generic list of mob indices
 */
struct mob_index_list
{
    MILIST_DATA *	next;
    MOB_INDEX_DATA *	lch;
};


/*
 * Structure for a generic list of object indices
 */
struct obj_index_list
{
    OILIST_DATA *	next;
    OBJ_INDEX_DATA *	o;
};


/*
 * Structure for a generic list of strings
 */
struct strlist_data
{
    STRLIST_DATA *	next;
    char *		str;
    int			len;
};


/*
 * Structure for a generic list of text.
 * Same as strlist_data but without the lengths.
 */
struct text_data
{
    TEXT_DATA *		next;
    char *		text;
};


/*
 * One character (PC or NPC).
 */
struct	char_data
{
    int			stype;		/* MUST be first field in struct */
    CHAR_DATA *		next;
    CHAR_DATA *		next_in_room;
    CHAR_DATA *		master;
    CHAR_DATA *		leader;
    CHAR_DATA *		fighting;
    CHAR_DATA *		reply;
    CHAR_DATA *		pet;
    CHAR_DATA *		mount;
    CHAR_DATA *		qmem;		/* mob memory, 'q' */
    CHAR_DATA *		questgiver;
    MEM_DATA *		memory;
    EVENT_DATA *	event_first;
    EVENT_DATA *	event_last;
    SPEC_FUN *		spec_fun;
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    NOTE_DATA *		pnote;
    OBJ_DATA *		carrying;
    OBJ_DATA *		on;
    ROOM_INDEX_DATA *	in_room;
    int			was_in_room;
    int			room_vnum;
    int			from_dir;
    AREA_DATA *		zone;
    PC_DATA *		pcdata;
    GEN_DATA *		gen_data;
    MPROG_ACT_LIST *	mpact;
    CLAN_DATA *		clan;
    void *		smob_ptr;
    int			smob_type;
    char *		name;
    long		id;
    sh_int		version;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    char *		prompt;
    char *		prefix;
    int			mpscriptpos;
    int			mpactnum;
    sh_int		group;
    int			clvl;
    sh_int		sex;
    sh_int		class;
    sh_int		race;
    sh_int		level;
    sh_int		trust;
    int			played;
    int			lines;  /* for the pager */
    time_t		logon;
    sh_int		timer;
    sh_int		wait;
    sh_int		daze;
    sh_int		hit;
    sh_int		max_hit;
    sh_int		mana;
    sh_int		max_mana;
    sh_int		move;
    sh_int		max_move;
    int			regen_rate;
    MONEY		money;
    int			exp;
    int			summon_timer;
    int			questpoints;		/* Vassago */
    int			nextquest;		/* Vassago */
    int			countdown;		/* Vassago */
    int			questobj;		/* Vassago */
    int			questmob;		/* Vassago */
    bitvector		act;
    bitvector		act2;
    bitvector		deaf;
    bitvector		comm;   /* RT added to pad the vector */
    bitvector		info;	/* info channel */
    bitvector		wiznet; /* wiz stuff */
    bitvector		imm_flags;
    bitvector		res_flags;
    bitvector		vuln_flags;
    sh_int		invis_level;
    sh_int		incog_level;
    EXT_BV		affected_by;
    EXT_BV		shielded_by;
    sh_int		position;
    sh_int		practice;
    sh_int		train;
    sh_int		carry_weight;
    sh_int		carry_number;
    sh_int		saving_throw;
    sh_int		alignment;
    sh_int      ethos;
    sh_int		hitroll;
    sh_int		damroll;
    sh_int		armor[4];
    sh_int		wimpy;
     CHAR_DATA *         tracking;
    /* stats */
    sh_int		perm_stat[MAX_STATS];
    sh_int		mod_stat[MAX_STATS];
    /* parts stuff */
    bitvector		form;
    bitvector		parts;
    int			size;
    int			weight;
    char*		material;
    /* mobile stuff */
    EXT_BV		off_flags;
    sh_int		damage[3];
    sh_int		dam_type;
    sh_int		start_pos;
    sh_int		default_pos;
    int			fall_count;	/* for no_floor rooms */
    bool		deleted;
    bool		valid;
    bool		questnone;
    bool		riding;
};



/*
 * Data which only PC's have.
 */
struct	pc_data
{
    PC_DATA *		next;
    ALIAS_DATA *	alias_list;
    AVATAR_DATA *	avatar;
    BANK_DATA *		bank;
    BUFFER * 		buffer;
    IGNORE_DATA *	ignoring;
    BOARD_INFO *	last_read;
    char *		pwd;
    char *		bamfin;
    char *		bamfout;
    char *		lname;
    char *		title;
    char *		hair_color;
    char *      hair_adj;
    char *		eye_color;
    char *      eye_adj;
    //char *      ethos;
    char *		who_text;
    char *		empower;
    char *		detract;
    char *		current_board;
    char *		pose;
    char *		tell_buf[TELLBUF_SIZE];
    int			tell_buf_index;
    int			perm_hit;
    int			perm_mana;
    int			perm_move;
    int			true_sex;
    int			last_level;
    int			condition	[8];
    int                 learned;
    struct skill_data *	skill;
    //int *		learned;
    bool		group_known	[MAX_GROUP];
    int			logcount;
    int			pload_room;
    int			deathroom;
    int			deathstate;
    int			deathmob;
    int			greet_id;
    int			points;
    int			etimer;
    int			kingdom;
    int			security;
    int			last_medit;
    int			last_oedit;
    int			beacon;
    int			mark_sector;
    int			colormap[MAX_CLRMAP];
    int			lastdream[2];
    int			forcedream[2];
    int			outcast;
    bitvector		sec_groups;
    bitvector		confirm_flags;
    int			stats[12];
    bool		valid;
    int	timesplayed;

};


/* Data for generating characters -- only used during generation */
struct gen_data
{
    GEN_DATA	*next;
    bool	valid;
    bool *	skill_chosen;
    bool	group_chosen[MAX_GROUP];
    int		points_chosen;
};


/*
 * Alias data, for PC's
 */
struct alias_data
{
    ALIAS_DATA *	next;
    char *		old;	/* this text... */
    char *		new;	/* ...gets replaced with this text. */
};


/*
 * Avatar data, for imms
 */
struct avatar_data
{
    AVATAR_DATA *	next;
    int			level;
    int			trust;
    int			hit;
    int			max_hit;
    int			mana;
    int			max_mana;
    int			move;
    int			max_move;
    bitvector		holylight;
    bool		valid;
};


/*
 * Liquids.
 */
#define LIQ_WATER        0

struct	liq_type
{
    char *	liq_name;
    char *	liq_color;
    sh_int	liq_affect[5];
};



/*
 * Extra description data for a room or object.
 */
struct	extra_descr_data
{
    EXTRA_DESCR_DATA *next;	/* Next in list                     */
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
    bool valid;
};



/*
 * Prototype for an object.
 */
struct	obj_index_data
{
    OBJ_INDEX_DATA *	next;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    AREA_DATA *		area;
    OREPROG_DATA *	oprogs;
    bitvector		progtypes;
    TEXT_DATA *		page;
    bool                new_format;
    char *		name;
    char *		short_descr;
    char *		description;
    int			vnum;
    int			reset_num;
    EXT_BV		material;
    int			item_type;
    bitvector		extra_flags;
    bitvector		extra_flags2;
    bitvector		wear_flags;
    EXT_BV		race_flags;
    EXT_BV		class_flags;
    sh_int		level;
    sh_int 		condition;
    sh_int		count;
    sh_int		weight;
    int			cost;
    int         qcost;
    int			timer;
    int			duration;
    int			value[6];
    int			ac_type;
    int			ac_vnum;
    int			ac_charge[2];
    int			join;
    int			sep_one;
    int			sep_two;
};



/*
 * One object.
 */
struct	obj_data
{
    int			stype;	/* MUST be first field in the struct */
    OBJ_DATA *		next;
    OBJ_DATA *		next_content;
    OBJ_DATA *		contains;
    OBJ_DATA *		in_obj;
    OBJ_DATA *		on;
    CHAR_DATA *		carried_by;
    BANK_DATA *		stored_in;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
    EVENT_DATA *	event_first;
    EVENT_DATA *	event_last;
    char *	        owner;
    char *		name;
    char *		short_descr;
    char *		description;
    sh_int		item_type;
    bitvector		extra_flags;
    bitvector		extra_flags2;
    bitvector		wear_flags;
    EXT_BV		race_flags;
    EXT_BV		class_flags;
    sh_int		wear_loc;
    sh_int		weight;
    int			cost;
    int         qcost;
    sh_int		level;
    sh_int 		condition;
    EXT_BV		material;
    sh_int		timer;
    int			value	[6];
    int			join;
    int			sep_one;
    int			sep_two;
    int			ac_type;
    int			ac_vnum;
    int			ac_spell;
    int			ac_charge [ 2 ];
    int			fall_count;
    bool		deleted;
    bool		valid;
    bool		enchanted;
};



/*
 * Exit data.
 */
struct	exit_data
{
    union
    {
        ROOM_INDEX_DATA *       to_room;
        int                  vnum;
    } u1;
    ROOM_INDEX_DATA *   to_room;
    ROOM_INDEX_DATA *	from_room;
    EXIT_DATA *		next;
    EVENT_DATA *	event_first;
    EVENT_DATA *	event_last;
    OREPROG_DATA *	eprogs;
    bitvector		progtypes;
    CHAR_DATA *		qmem;
    int			vnum;
    bitvector		rs_flags;
    bitvector		exit_info;
    int			size;
    int			key;
    char *		keyword;
    char *		description;
    char *		sound_closed;
    char *		sound_open;
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct	reset_data
{
    RESET_DATA *	next;
    char		command;
    int		arg1;
    int		arg2;
    int		arg3;
    int		arg4;
};



/*
 * Area flags
 */
#define AREA_NONE		 0
#define AREA_CHANGED		(A)	/* Area has been modified */
#define AREA_PROTOTYPE		(B)	/* Prototype area (no mortals) */
#define AREA_QUIET		(C)	/* No reset msgs or weather reports */
#define AREA_NOQUEST		(D)	/* No autoquests in this area */
#define AREA_CLAN		(E)	/* Area is a clan hall */
#define AREA_MUDSCHOOL		(F)	/* Fast repop */
#define AREA_FREEZE		(G)	/* All mobs are sentinel */
#define AREA_NO_UNFINISHED	(H)	/* don't flag unfinished rooms */
#define AREA_QUEST		(I)	/* area is a quest area */
#define AREA_TRAVEDIT		(J)	/* builder may edit traveling rooms */
#define AREA_ARENA		(K)	/* Area is an arena. */
#define AREA_NOLIST		(L)	/* Do not include area in 'areas' or 'kingdoms' commands */
#define AREA_NOWRAP             (M)     /* Area overland map does not wrap */

/*
 * World values
 */
#define WORLD_NONE		0
#define WORLD_ALL		1

/*
 * Continent Values  (Ren)
 */
/* Special "landmasses" */
#define LAND_NONE		 0
#define LAND_OVERLAND		 1
/* Continental landmasses */
#define LAND_GRAVMERE		 2
#define LAND_TINKERTON		 3
#define LAND_DARK_SANDS		 4
#define LAND_BLACK_ISLE		 5
/* Island landmasses */

#define LAND_MAX		6


/*
 * Room desc equates
 */
#define DESC_DESC		0
#define DESC_MORN		1
#define DESC_EVEN		2
#define DESC_NIGHT		3


/*
 * Table of landmass names
 */
struct landmass_name_types
{
    int			land;
    const char *	name;
};

/*
 * Landmass table
 */
struct landmass_type
{
    int			landmass;
    int			lx;		/* x-ordinate of upper left corner  */
    int			uy;		/* y-ordinate of upper left corner  */
    int			rx;		/* x-ordinate of lower right corner */
    int			ly;		/* y-ordinate of lower right corner */
    bool		fIsland;	/* TRUE if land is island	    */
};


/*
 * Kingdom table
 */
struct kingdom_type
{
    int			num;
    int			color;
    int			recall;		/* vnum of recall point */
    int			box;		/* vnum of donation box */
    char *		name;
    char *		desc;
};


/*
 * Area definition.
 */
struct	area_data
{
    AREA_DATA *		next;
    RESET_DATA *	reset_first;
    RESET_DATA *	reset_last;
    ROOM_INDEX_DATA *	room_list;
    OVERLAND_DATA *	overland;
    EVENT_DATA *	event_first;
    EVENT_DATA *	event_last;
    char *		builders;
    char *		file_name;
    char *		name;
    char *		credits;
    char *		resetmsg;
    char *		norecall;
    sh_int		age;
    bitvector		area_flags;
    int			color;
    int			exitsize;
    time_t		created;
    time_t		last_changed;
    int			low_level;
    int			high_level;
    int 		min_vnum;
    int			max_vnum;
    int			maxx;
    int			maxy;
    int			nplayer;
    int			recall;
    int			security;
    int			vnum;
    int			kingdom;
    int			continent;
    int			world;
    int			travel_vnum;
    ROOM_INDEX_DATA *	travel_room;
    TRAVEL_DATA *	travel_first;
    TRAVEL_DATA *	travel_curr;
    int			travel_timer;
    EXIT_DATA *		link;
    EXIT_DATA *		rev_link;
    bool		empty;
};


/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    ROOM_INDEX_DATA *	prev;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	next_in_area;
    ROOM_INDEX_DATA *	next_in_room;
    ROOM_INDEX_DATA *   next_track;
    ROOM_INDEX_DATA *   track_came_from;
    CHAR_DATA *		people;
    OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[MAX_DIR];
    EXIT_DATA * 	old_exit[MAX_DIR];
    RESET_DATA *	reset_first;
    RESET_DATA *	reset_last;
    OREPROG_DATA *	rprogs;
    EVENT_DATA *	event_first;
    EVENT_DATA *	event_last;
    bitvector		progtypes;
    char *		name;
    char *		description;
    char *		morning_desc;
    char *		evening_desc;
    char *		night_desc;
    char *		sound;
    char *		smell;
    char *		owner;
    int			vnum;
    bitvector		room_flags;
    bitvector		affect_flags;
    int			vehicle_type;
    char *		short_descr;
    CHAR_DATA *		helmsman;
    int			light;
    int			sector_type;
    int			heal_rate;
    int			mana_rate;
    int			vtimer;			/* virtual room timer */
    int			clan;
};


/*
 * Sector info
 */
struct sector_data_types
{
    int			sector_type;
    char *		name;
    char *		desc;
    int			move_loss;
    int			room_color;
    int			color_index;
    int			red;
    int			green;
    int			blue;
    int			elevation;
    char		color_char;
    char		symbol;
    char *		html_color;
};

/*	Overland stuff		*/

struct overland_data
{
    OVERLAND_DATA *		next;
    VIRTUAL_ROOM_DATA *		map;
    AREA_DATA *			area;
    ROOM_INDEX_DATA *		room_first;
    ROOM_INDEX_DATA *		room_last;
    char *			sect_desc[SECT_MAX][4];
    int				x;
    int				y;
};

struct virtual_room_data
{
    unsigned char	flags;
    unsigned char	sector_type;
};


/*
 * Moving rooms stuff
 */
struct travel_data
{
    TRAVEL_DATA *	next;
    AREA_DATA *		area;
    int			room_vnum;	/* room vnum */
    int			exit_dir;	/* dir from travel to static */
    int			stop_timer;	/* length of time to stay linked */
    int			move_timer;	/* length of time to stay unlinked */
    ROOM_INDEX_DATA *	room;		/* room traveling room is linked to */
    char *		arrive_travel;	/* msg sent to moving room on arrival */
    char *		arrive_room;	/* msg sent to static room on arrival */
    char *		depart_travel;	/* msg sent to moving room on departure */
    char *		depart_room;	/* msg sent to static room on departure */
    bool		stopped;	/* stopped or moving */
};


struct phys_attr_type
{
    const char *	desc;		/* description of attribute */
    const char *	list;		/* list of races that may have the attr */
};


/****************************************************************/
/*		EXIT/MOB/OBJ/ROOM PROG INFO			*/
/****************************************************************/

#define ERROR_PROG		0

#define PTYPE_EXIT		1
#define PTYPE_MOB		2
#define PTYPE_OBJ		3
#define PTYPE_ROOM		4

/*
 * enter/exit/pass rules are the same as those for room traps.
 * note that look trap is only called if scry trap fails.
 */
#define EXIT_PROG_ERROR          0  /* error! */
#define EXIT_PROG_BASH		 A  /* someone bashes the exit (door) */
#define EXIT_PROG_CLOSE          B  /* someone closes the exit (door) */
#define EXIT_PROG_ENTER          C  /* someone enters through the exit */
#define EXIT_PROG_EXIT           D  /* someone leaves through the exit */
#define EXIT_PROG_KNOCK		 E  /* someone knocks on the door */
#define EXIT_PROG_LOCK           F  /* someone locks the exit (door) */
#define EXIT_PROG_LOOK           G  /* someone looks through exit */
#define EXIT_PROG_OPEN           H  /* someone opens the exit (door) */
#define EXIT_PROG_PASS           I  /* someone enters/leaves through exit */
#define EXIT_PROG_PICK           J  /* someone picks the exit (locked door) */
#define EXIT_PROG_SCRY           K  /* someone scrys through the exit */
#define EXIT_PROG_UNLOCK         L  /* someone unlocks the exit (door) */
#define EXIT_PROG_LISTEN	 M  /* someone listened at the exit */

/*
 * Mob prog types
 */
#define MP_ACT_PROG		A
#define MP_ALL_GREET_PROG	B
#define MP_BRIBE_PROG		C
#define MP_DEATH_PROG		D
#define MP_ENTRY_PROG		E
#define MP_FIGHT_PROG		F
#define MP_GIVE_PROG		G
#define MP_GREET_PROG		H
#define MP_HITPRCNT_PROG	I
#define MP_HOUR_PROG		J
#define MP_RAND_PROG		K
#define MP_SPEECH_PROG		L
#define MP_TELL_PROG		M
#define MP_TIME_PROG		N
#define MP_REPOP_PROG		O
#define MP_LIST_PROG		P
#define MP_BUY_PROG		Q
#define MP_SELL_PROG		R
#define MP_WHISPER_PROG		S


/*
 * The object triggers.. quite a few of em.. :)
 */
#define OBJ_PROG_ERROR           0  /* error! */
#define OBJ_PROG_BUY             A  /* obj is bought from store */
#define OBJ_PROG_CAST            B  /* spell is cast on obj - percent */
#define OBJ_PROG_CAST_SN         C  /* spell is cast on obj - by slot */
#define OBJ_PROG_CLOSE           D  /* obj is closed (containers) */
#define OBJ_PROG_DROP            E  /* obj is dropped */
#define OBJ_PROG_FILL            F  /* obj is filled (drink_cons) */
#define OBJ_PROG_GET             G  /* obj is picked up */
#define OBJ_PROG_GET_FROM        H  /* to allow secondary obj's in get */
#define OBJ_PROG_GIVE            I  /* give an obj away */
#define OBJ_PROG_INVOKE          J  /* obj is invoked */
#define OBJ_PROG_JOIN            K  /* obj is joined with another */
#define OBJ_PROG_LOCK            L  /* obj is locked (containers) */
#define OBJ_PROG_LOOK            M  /* obj is looked at/examined */
#define OBJ_PROG_LOOK_IN         N  /* obj is looked inside (containers) */
#define OBJ_PROG_OPEN            O  /* obj is opened (containers) */
#define OBJ_PROG_PICK            P  /* obj is picked (containers) */
#define OBJ_PROG_PUT             Q  /* obj is put into something */
#define OBJ_PROG_RANDOM          R  /* random trigger */
#define OBJ_PROG_RETRIEVE        S  /* obj is retrieved from storage */
#define OBJ_PROG_SELL            T  /* obj is sold to store */
#define OBJ_PROG_SEPARATE        U  /* obj is separated into two */
#define OBJ_PROG_STORE           V  /* obj is stored in storage boxes */
#define OBJ_PROG_THROW           W  /* obj is thrown */
#define OBJ_PROG_UNLOCK          X  /* obj is unlocked (containers) */
#define OBJ_PROG_USE             Y  /* obj is used (recited, zapped, ect) */
#define OBJ_PROG_WEAR            Z  /* obj is worn */
#define OBJ_PROG_READ		aa  /* obj is read */
#define OBJ_PROG_REMOVE		bb  /* object is removed */
#define OBJ_PROG_UNUSE		cc  /* object is no longer used (usually furniture ) */

/*
 * Note that entry/exit/pass are only called if the equivalent exit
 * trap for the exit the person went through failed.
 * Pass is only called if the respective enter or exit trap failed.
 */
#define ROOM_PROG_ERROR		 0  /* error! */
#define ROOM_PROG_CAST		 A  /* a spell was cast in room - percent */
#define ROOM_PROG_CAST_SN	 B  /* a spell was cast in room - by slot */
#define ROOM_PROG_DEATH		 C  /* someone dies in the room */
#define ROOM_PROG_ENTER		 D  /* someone enters the room */
#define ROOM_PROG_EXIT		 E  /* someone leaves the room */
#define ROOM_PROG_PASS		 F  /* someone enters or leaves */
#define ROOM_PROG_PULL		 G  /* someone pulls something */
#define ROOM_PROG_PUSH		 H  /* someone pushes something */
#define ROOM_PROG_RANDOM	 I  /* random trigger */
#define ROOM_PROG_REST		 J  /* someone rests in the room */
#define ROOM_PROG_SIT		 K  /* someone sits in the room */
#define ROOM_PROG_SLEEP		 L  /* someone sleeps in the room */
#define ROOM_PROG_TIME		 M  /* depends on the time of day */
#define ROOM_PROG_WAKE		 N  /* someone wakes up in the room */
#define ROOM_PROG_SPEECH	 O  /* Someone says something in the room */
#define ROOM_PROG_LOOK		 P  /* Someone looks at a room extra desc */
#define ROOM_PROG_LISTEN	 Q  /* Someone listened in the room */
#define ROOM_PROG_SMELL		 R  /* Someone smelled in the room */
#define ROOM_PROG_INTERP	 S  /* Someone typed a command in the room */
#define ROOM_PROG_BOOT		 T  /* Run one time, at boot time */
#define ROOM_PROG_STAND		 U  /* Someone stands up in the room */
#define ROOM_PROG_ARRIVE	 V  /* Traveling room arrives (links) */
#define ROOM_PROG_DEPART	 W  /* Traveling room departs (unlinks) */

/*
 * mobprog act_prog queue
 */
struct  mob_prog_act_list
{
    MPROG_ACT_LIST * next;
    char *           buf;
    CHAR_DATA *      ch;
    OBJ_DATA *       obj;
    void *           vo;
};

/*
 * Mob prog struct
 */
struct  mob_prog_data
{
    MPROG_DATA *	next;
    bitvector		type;
    char *		arglist;
    char *		comlist;
    bool		triggered;
};


struct oreprog_data
{
    OREPROG_DATA *	next;
    bitvector		type;
    char *		arglist;
    char *		comlist;
//  OBJ_INDEX_DATA *	on_obj;
//  ROOM_INDEX_DATA *	in_room;
//  EXIT_DATA *		on_exit;
    bool		triggered;
};

/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000


/*
 * Spell flags
 */
#define SPELL_NONE		    0
#define SPELL_NOMOB		   (A)
#define SPELL_NOPC		   (B)
#define SPELL_NOINVOKE		   (C)
#define SPELL_NOSCROLL		   (D)
#define SPELL_NOWAND		   (E)
#define SPELL_NOSTAFF		   (F)
#define SPELL_NOPOTION		   (G)
#define SPELL_NOPILL		   (H)
#define SPELL_NOLENS		   (I)
#define SPELL_NOHERB		   (J)

/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4
#define TAR_OBJ_CHAR_DEF	    5
#define TAR_OBJ_CHAR_OFF	    6

#define TARGET_CHAR		    0
#define TARGET_OBJ		    1
#define TARGET_ROOM		    2
#define TARGET_NONE		    3



/*
 * interp.h
  */
  /*Command Types*/    

#define COM_IMMO                0     
#define COM_MOVE                1    
#define COM_COMB                2    
#define COM_BLDR                3    
#define COM_INFO                4    
#define COM_MISC                5    
#define COM_OTHR                6    
#define COM_CFIG                7    
#define COM_COMM                8    
#define COM_OBJM                9    
#define COM_CLAN                10    
#define COM_LEAD                11    
#define COM_PBWR                12    
#define COM_QEST                13    


#define CIM     COM_IMMO    
#define CMV     COM_MOVE    
#define CFT     COM_COMB    
#define CBR     COM_BLDR    
#define CDT     COM_INFO    
#define CMC     COM_MISC    
#define COR     COM_OTHR    
#define CCO     COM_COMM    
#define CFG     COM_CFIG    
#define COJ     COM_OBJM    
#define CCN     COM_CLAN    
#define CLD     COM_LEAD    
#define CPB     COM_PBWR            
#define CQT     COM_QEST              

/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
    char *	name;			/* Name of skill		*/
    sh_int	skill_level[MAX_CLASS];	/* Level needed by class	*/
    sh_int	rating[MAX_CLASS];	/* How hard it is to learn	*/
    int		prereq[MAX_PREREQ];	/* Prerequisite skill(s)	*/
    SPELL_FUN *	spell_fun;		/* Spell pointer (for spells)	*/
    sh_int	target;			/* Legal targets		*/
    sh_int	minimum_position;	/* Position for caster / user	*/
    sh_int *	pgsn;			/* Pointer to associated gsn	*/
/*    sh_int	slot;		*/	/* Slot for #OBJECT loading	*/
    bitvector	spell_flags;		/* Flags for who/what can/cannot cast */
    sh_int	min_mana;		/* Minimum mana used		*/
    sh_int	beats;			/* Waiting time after use	*/
    int		forget;			/* How easy it is to forget	*/
    char *	noun_damage;		/* Damage message		*/
    char *	msg_off;		/* Wear off message		*/
    char *	msg_obj;		/* Wear off message for obects	*/
    char *	msg_room;		/* Wear off msg shown to room	*/
};

struct  group_type
{
    char *	name;
    int		rating[MAX_CLASS];
    char *	spells[MAX_IN_GROUP];
};



/*
 * These are skill_lookup return values for common skills and spells.
 */
extern	int	gsn_adrenaline;
extern  int     gsn_assassinate;
extern	int	gsn_atrophy;
extern	int	gsn_aura_peace;
extern	int	gsn_axe;
extern	int	gsn_backstab;
extern	int	gsn_bash;
extern	int	gsn_bastardsword;
extern	int	gsn_berserk;
extern	int	gsn_black_mantle;
extern	int	gsn_blade_barrier;
extern	int	gsn_blindness;
extern	int	gsn_bow;
extern	int	gsn_broadsword;
extern	int	gsn_charge;
extern	int	gsn_charm_person;
extern  int     gsn_circle;
extern	int	gsn_club;
extern	int	gsn_counter;
extern	int	gsn_crossbow;
extern	int	gsn_curse;
extern	int	gsn_dagger;
extern	int	gsn_dancing_lights;
extern  int     gsn_deafen;
extern	int	gsn_dirk;
extern	int	gsn_dirt;
extern	int	gsn_disarm;
extern	int	gsn_dodge;
extern	int	gsn_dream;
extern	int	gsn_dual;
extern	int	gsn_enhanced_damage;
extern	int	gsn_enlarge;
extern	int	gsn_envenom;
extern	int	gsn_fast_healing;
extern	int	gsn_fifth_attack;
extern	int	gsn_flail;
extern	int	gsn_fly;
extern	int	gsn_fourth_attack;
extern	int	gsn_glance;
extern	int	gsn_greataxe;
extern	int	gsn_greatmace;
extern	int	gsn_haggle;
extern	int	gsn_hammer;
extern	int	gsn_hand_to_hand;
extern	int	gsn_herbs;
extern	int	gsn_hide;
extern	int	gsn_horseflail;
extern	int	gsn_horsemace;
extern	int	gsn_invis;
extern	int	gsn_kick;
extern	int	gsn_lance;
extern	int	gsn_longsword;
extern	int	gsn_lore;
extern	int	gsn_mace;
extern	int	gsn_mass_invis;
extern	int	gsn_meditation;
extern	int	gsn_parry;
extern	int	gsn_peek;
extern	int	gsn_phase;
extern	int	gsn_pick_lock;
extern	int	gsn_plague;
extern	int	gsn_poison;
extern	int	gsn_polearm;
extern	int	gsn_rapier;
extern	int	gsn_recall;
extern	int	gsn_rescue;
extern	int	gsn_riding;
extern	int	gsn_sabre;
extern	int	gsn_sanctuary;
extern	int	gsn_scimitar;
extern	int	gsn_scrolls;
extern	int	gsn_second_attack;
extern	int	gsn_shield_block;
extern	int	gsn_shortsword;
extern	int	gsn_shrink;
extern	int	gsn_sleep;
extern	int	gsn_sneak;
extern	int	gsn_spear;
extern	int	gsn_spellcraft;
extern	int	gsn_staff;
extern	int	gsn_staves;
extern	int	gsn_steal;
extern	int	gsn_stiletto;
extern  int     gsn_swimming;
extern	int	gsn_sword;
extern	int	gsn_third_attack;
extern  int     gsn_track;
extern	int	gsn_trip;
extern	int	gsn_twohandsword;
extern  int     gsn_wail;
extern	int	gsn_wands;
extern  int     gsn_warmount;
extern	int	gsn_weariness;
extern	int	gsn_whip;
extern   int   gsn_sharpen;
extern   int   gsn_butcher;
extern   int   gsn_skin;
extern   int   gsn_whirlwind;
extern   int   gsn_blackjack;
extern int     gsn_acid_breath;
extern int     gsn_fire_breath;
extern int     gsn_frost_breath;
extern int     gsn_gas_breath;
extern int     gsn_lightning_breath;
extern int     gsn_breath;
extern int     gsn_engage;
extern int     gsn_purify;

/*
 * race values
 */
extern		int			race_bat;
extern		int			race_bear;
extern		int			race_cat;
extern		int			race_centaur;
extern		int			race_centipede;
extern		int			race_chimera;
extern		int			race_demon;
extern		int			race_dog;
extern		int			race_doll;
extern		int			race_dragon;
extern		int			race_dragonborn;
extern		int			race_drow;
extern		int			race_dwarf;
extern		int			race_elf;
extern		int			race_felis;
extern		int			race_fido;
extern		int			race_fish;
extern		int			race_fox;
extern		int			race_giant;
extern		int			race_gnome;
extern		int			race_goblin;
extern		int			race_halfelf;
extern		int			race_halfling;
extern		int			race_halforc;
extern		int			race_highelf;
extern		int			race_hobgoblin;
extern		int			race_horse;
extern		int			race_human;
extern		int			race_insect;
extern		int			race_kobold;
extern		int			race_lizard;
extern		int			race_minotaur;
extern		int			race_ogre;
extern		int			race_orc;
extern		int			race_pig;
extern		int			race_pixie;
extern		int			race_rabbit;
extern		int			race_satyr;
extern		int			race_school_monster;
extern		int			race_shadow;
extern		int			race_snake;
extern		int			race_songbird;
extern		int			race_sprite;
extern		int			race_tiefling;
extern		int			race_titan;
extern		int			race_troll;
extern		int			race_undead;
extern		int			race_unique;
extern		int			race_waterfowl;
extern		int			race_wolf;
extern		int			race_wyvern;
extern		int			race_zombie;

/*
 * and the classes
 */
extern		int			class_cleric;
extern		int			class_fighter;
extern		int			class_druid;
extern		int			class_mage;
extern		int			class_rogue;


/*
 * Macros for accessing virtually unlimited bitvectors.		-Thoric
 *
 * Note that these macros use the bit number rather than the bit value
 * itself -- which means that you can only access _one_ bit at a time
 *
 * This code uses an array of integers
 */

/*
 * The functions for these prototypes can be found in handler.c
 * They are up here because they are used by the macros below
 */
bool	ext_is_empty		args( ( EXT_BV *bits ) );
void	ext_clear_bits		args( ( EXT_BV *bits ) );
int	ext_has_bits		args( ( EXT_BV *var, EXT_BV *bits) );
bool	ext_same_bits		args( ( EXT_BV *var, EXT_BV *bits) );
void	ext_set_bits		args( ( EXT_BV *var, EXT_BV *bits) );
void	ext_remove_bits		args( ( EXT_BV *var, EXT_BV *bits) );
void	ext_toggle_bits		args( ( EXT_BV *var, EXT_BV *bits) );

/*
 * Here are the extended bitvector macros:
 */
#define xIS_SET(var, bit)	((var).bits[(bit) >> RSV] & 1 << ((bit) & XBM))
#define xSET_BIT(var, bit)	((var).bits[(bit) >> RSV] |= 1 << ((bit) & XBM))
#define xSET_BITS(var, bit)	(ext_set_bits(&(var), &(bit)))
#define xREMOVE_BIT(var, bit)	((var).bits[(bit) >> RSV] &= ~(1 << ((bit) & XBM)))
#define xREMOVE_BITS(var, bit)	(ext_remove_bits(&(var), &(bit)))
#define xTOGGLE_BIT(var, bit)	((var).bits[(bit) >> RSV] ^= 1 << ((bit) & XBM))
#define xTOGGLE_BITS(var, bit)	(ext_toggle_bits(&(var), &(bit)))
#define xCLEAR_BITS(var)	(ext_clear_bits(&(var)))
#define xIS_EMPTY(var)		(ext_is_empty(&(var)))
#define xHAS_BITS(var, bit)	(ext_has_bits(&(var), &(bit)))
#define xSAME_BITS(var, bit)	(ext_same_bits(&(var), &(bit)))

/*
 * Utility macros.
 */
#define IS_VALID(data)		((data) != NULL && (data)->valid)
#define VALIDATE(data)		((data)->valid = TRUE)
#define INVALIDATE(data)	((data)->valid = FALSE)
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) ^= (bit))

/*
 * Returns TRUE if room is in virtual vnum range for area even though
 * the room may actually be a real room.
 */
#define IS_VIRTUAL(room)	((room)->area->overland != NULL && \
				 (room)->vnum < (room)->area->min_vnum +\
				 (room)->area->maxx * (room)->area->maxy )

#define IS_WATER(sector)	((sector) == SECT_WATER_SWIM	||\
                                 (sector) == SECT_WATER_NOSWIM	||\
                                 (sector) == SECT_UNDERWATER	||\
                                 (sector) == SECT_LAKE		||\
                                 (sector) == SECT_RIVER		||\
                                 (sector) == SECT_OCEAN		  )

#define CAN_ACCESS(room)	(!IS_SET(room->room_flags, ROOM_PRIVATE)\
				&& !IS_SET(room->room_flags, ROOM_SOLITARY)\
				&& !IS_SET(room->room_flags, ROOM_NEWBIES_ONLY)\
				&& !IS_SET(room->room_flags, ROOM_GODS_ONLY)\
				&& !IS_SET(room->area->area_flags, AREA_PROTOTYPE )\
				&& !IS_SET(room->room_flags, ROOM_IMP_ONLY))

#define IS_NULLSTR(str)		( (str) == NULL || *(str) == '\0' )

/*
 * Character macros.
 */
#define IS_AFFECTED(ch, sn)	(xIS_SET((ch)->affected_by, (sn)))
#define IS_SHIELDED(ch, sn )	(xIS_SET((ch)->shielded_by, (sn)))
#define IS_AVATAR( ch )		((!IS_NPC(ch)) && (ch)->pcdata->avatar != NULL )
#define IS_CODER( ch )		( IS_SET( (ch)->act2, CODER ) )
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_TETHERED(ch)		(IS_NPC(ch) && IS_SET((ch)->act, ACT_TETHERED))
#define IS_TRUSTED(ch,level)	(get_trust((ch)) >= (level))
#define HAS_SHIELD(ch)		(!xIS_EMPTY((ch)->shielded_by))
#define GET_AGE(ch)		((int) (17 + ((ch)->played \
				    + current_time - (ch)->logon )/72000))

#define IS_DEAD(ch)		(!IS_NPC(ch) && !IS_IMMORTAL(ch) && (ch)->pcdata->deathstate != DEATH_NONE)

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))
#define IS_LAWFUL(ch)     (ch->ethos >= 350)
#define IS_CHAOTIC(ch)     (ch->ethos <= -350)
#define IS_TRUE_NEUTRAL(ch)      (!IS_NEUTRAL(ch) && !IS_LAWFUL(ch) && !IS_CHAOTIC(ch))
#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch,type)		((ch)->armor[type]			    \
		        + ( IS_AWAKE(ch)			    \
			? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ))
#define GET_HITROLL(ch)	\
		((ch)->hitroll+str_app[get_curr_stat(ch,STAT_STR)].tohit)
#define GET_DAMROLL(ch) \
		((ch)->damroll+str_app[get_curr_stat(ch,STAT_STR)].todam)

#define IS_OUTSIDE(ch)		((ch)->in_room->sector_type != SECT_UNDERGROUND	\
				    && !IS_SET((ch)->in_room->room_flags, \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, (npulse)))
#define get_carry_weight( ch )	((ch)->carry_weight + \
					(ch)->money.gold    * 8 + \
					(ch)->money.silver * 8 + \
					(ch)->money.copper   * 8 + \
					(ch)->money.fract )
#define IS_DRUNK(ch)	((ch)->pcdata->condition[COND_DRUNK] > 10 )

/* Returns the mob a player is mounted on. */
#define MOUNTED(ch)	((!IS_NPC(ch) && ch->mount && ch->riding )?ch->mount:NULL)
/* Returns the player riding a mob. */
#define RIDDEN(ch)	((IS_NPC(ch) && ch->mount && ch->riding )?ch->mount:NULL)

#define act(format,ch,arg1,arg2,type)\
	act_color(C_DEFAULT,(format),(ch),(arg1),(arg2),(type),POS_RESTING)
#define act_new(format,ch,arg1,arg2,type,pos)\
	act_color(C_DEFAULT,(format),(ch),(arg1),(arg2),(type),(pos))

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_OBJ2_STAT(obj, stat)	(IS_SET((obj)->extra_flags2, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)	((obj)->item_type == ITEM_CONTAINER ? \
	(obj)->value[4] : 100)
#define IS_WEARABLE( obj )	(    CAN_WEAR( obj, ITEM_WEAR_FINGER	) \
				  || CAN_WEAR( obj, ITEM_WEAR_NECK	) \
				  || CAN_WEAR( obj, ITEM_WEAR_BODY	) \
				  || CAN_WEAR( obj, ITEM_WEAR_HEAD	) \
				  || CAN_WEAR( obj, ITEM_WEAR_LEGS	) \
				  || CAN_WEAR( obj, ITEM_WEAR_FEET	) \
				  || CAN_WEAR( obj, ITEM_WEAR_HANDS	) \
				  || CAN_WEAR( obj, ITEM_WEAR_ARMS	) \
				  || CAN_WEAR( obj, ITEM_WEAR_SHIELD	) \
				  || CAN_WEAR( obj, ITEM_WEAR_ABOUT	) \
				  || CAN_WEAR( obj, ITEM_WEAR_WAIST	) \
				  || CAN_WEAR( obj, ITEM_WEAR_WRIST	) \
				  || CAN_WEAR( obj, ITEM_WIELD		) \
				  || CAN_WEAR( obj, ITEM_HOLD		) \
				  || CAN_WEAR( obj, ITEM_WEAR_FLOAT	) \
				  || CAN_WEAR( obj, ITEM_WEAR_CONTACT	) \
				  || CAN_WEAR( obj, ITEM_WEAR_FACE	) \
				  || CAN_WEAR( obj, ITEM_WEAR_EAR	) \
				  || CAN_WEAR( obj, ITEM_WEAR_CPATCH	) \
				  || CAN_WEAR( obj, ITEM_WEAR_RPATCH	) \
				  || CAN_WEAR( obj, ITEM_WEAR_SHOULDER	) \
				  || CAN_WEAR( obj, ITEM_WEAR_ANKLE	) \
				  || CAN_WEAR( obj, ITEM_WEAR_TAIL      ) \
				  || CAN_WEAR( obj, ITEM_WEAR_HORNS     ) )


/*
 * Money macros
 */
#define CASH_VALUE(amt)		(UMIN((amt).gold,MAX_GOLD)*FRACT_PER_COPPER*COPPER_PER_SILVER*SILVER_PER_GOLD \
				 + (amt).silver * FRACT_PER_COPPER*COPPER_PER_SILVER \
				 + (amt).copper * FRACT_PER_COPPER \
				 + (amt).fract )

/*
 * Description macros.
 */
/* old ROM macro
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: (ch)->name ) : "someone" )
*/
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: ( knows_char( looker, (ch) ) ?	\
				  (ch)->name : (ch)->short_descr ) )	\
					:  "someone" )

#define ROOMNAME(room)		( IS_NULLSTR( (room)->short_descr ) ?	\
				(room)->name : (room)->short_descr )

/* double-linked list handling macros -Thoric */
/* Updated by Scion 8/6/1999 */
#define LINK(link, first, last, next, prev)                     	\
do                                                              	\
{                                                               	\
   if ( !(first) )								\
   {                                           				\
      (first) = (link);				                       	\
      (last) = (link);							    	\
   }											\
   else                                                      	\
      (last)->next = (link);			                       	\
   (link)->next = NULL;			                         	\
   if (first == link)								\
      (link)->prev = NULL;							\
   else										\
      (link)->prev = (last);			                       	\
   (last) = (link);				                       	\
} while(0)

#define INSERT(link, insert, first, next, prev)                 \
do                                                              \
{                                                               \
   (link)->prev = (insert)->prev;			                \
   if ( !(insert)->prev )                                       \
      (first) = (link);                                         \
   else                                                         \
      (insert)->prev->next = (link);                            \
   (insert)->prev = (link);                                     \
   (link)->next = (insert);                                     \
} while(0)

#define UNLINK(link, first, last, next, prev)                   	\
do                                                              	\
{                                                               	\
	if ( !(link)->prev )							\
	{			                                    	\
         (first) = (link)->next;			                 	\
	   if ((first))							 	\
	      (first)->prev = NULL;						\
	} 										\
	else										\
	{                                                 		\
         (link)->prev->next = (link)->next;                 	\
	}										\
	if ( !(link)->next ) 							\
	{				                                    \
         (last) = (link)->prev;                 			\
	   if ((last))								\
	      (last)->next = NULL;						\
	} 										\
	else										\
	{                                                    		\
         (link)->next->prev = (link)->prev;                 	\
	}										\
} while(0)


/*
 *  Social data
 */
struct social_data
{
    SOCIAL_DATA *	next;
    char *		name;
    char *		cnoarg;
    char *		onoarg;
    char *		cfound;
    char *		ofound;
    char *		vfound;
    char *		nfound;
    char *		cself;
    char *		oself;
    int			ed_level;
    bool		deleted;
};


/*
 *  Quest logging
 */
#define QP_GAIN		0
#define QP_LOSS		1
#define QP_SPEND	2
#define QP_GRANT	3

/*
 * Structure for the table of quest items
 */
struct quest_type
{
    int		vnum;		/* vnum of the object */
    int		qcost;		/* cost, in quest points, to buy */
};



/*
 * Global constants.
 */

/* This is in comm.c, the rest are in const.c */
extern	const	char *	const		ansi_color[];

extern	const	struct	armor_val_type	armor_values	[];
extern	const	struct	attack_type	attack_table	[];
extern	const	struct	class_type	class_table	[MAX_CLASS];
extern	const		char		colorcode_list	[];
extern	const	struct	con_app_type	con_app		[26];
extern		char *	const		day_name	[];
extern	const	struct	dex_app_type	dex_app		[26];
extern		char *	const		dir_letter	[];
extern	const	char *	const		dir_desc	[];
extern		char *	const		dir_from	[];
extern		char *	const		dir_name	[];
extern		int	const		dir_next	[MAX_DIR][2];
extern	const	int			dir_remap	[];
extern  const   struct  phys_attr_type  eye_adj_table [];
extern	const	struct	phys_attr_type	eye_color_table	[];
extern		struct  group_type      group_table	[MAX_GROUP];
extern  const   struct  phys_attr_type  hair_adj_table[];
extern	const	struct	phys_attr_type	hair_color_table[];
extern	const	char *	const		he_she		[];
extern	const	char *	const		him_her		[];
extern	const	char *	const		his_her		[];
extern	const	struct	hitdice_type	hitdice_table	[];
extern	const	struct	wiznet_type	info_table	[];
extern	const	struct	int_app_type	int_app		[26];
extern	const	struct	kingdom_type	kingdom_table	[];
extern	const	struct	landmass_name_types     land_name_table[];
extern	const	struct	landmass_type	landmass_table	[];
extern	const	struct	liq_type	liq_table	[];
extern		char *	const		month_name	[];
extern		char *	const		ordinal		[];
extern	const	struct	quest_type	quest_table	[ ];
extern	const		int		rev_dir		[];
extern		struct  race_type	race_table	[];
extern		struct	sector_data_types sector_data	[];
extern		struct	skill_type	skill_table	[];
extern  const	struct	spec_type	spec_table	[];
extern	const	struct	str_app_type	str_app		[26];
extern	const	struct	weapon_val_type	weapon_values	[];
extern	const	struct	weapon_type	weapon_table	[];
extern		char *	const		week_name	[];
extern		int			wear_order	[];
extern	const	struct	wis_app_type	wis_app		[26];
extern	const	struct	wiznet_type	wiznet_table	[];
extern  char *const    tick_table[];

/*
 * Global variables, in global.c
 */
extern		AREA_DATA	  *	area_first;
extern		AREA_DATA	  *	area_last;

extern		CHAR_DATA	  *	auc_bid;
extern		int			auc_cost;
extern		int			auc_count;
extern		CHAR_DATA	  *	auc_held;
extern		OBJ_DATA	  *	auc_obj;

extern		char *			badname_list;
extern		BOARD_DATA	  *	board_first;
extern		BOARD_DATA	  *	board_last;
extern		char			boot_file	[];
extern		time_t			boot_time;
extern		char			bug_buf		[];

extern		CHAR_DATA	  *	char_list;
extern		bool			changed_boards;
extern		bool			changed_clan;
extern		bool			changed_command;
extern		bool			changed_dream;
extern		bool			changed_group;
extern		bool			changed_project;
extern		bool			changed_race;
extern		bool			changed_secgroup;
extern		bool			changed_skill;
extern		bool			changed_social;
extern		bool			changed_system;

extern		CLAN_DATA	  *	clan_first;
extern		int			control;
extern		time_t			current_time;

extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		DREAM_DATA *		dream_last;
extern		DREAM_DATA *		dream_list;

extern		bool			fLogAll;
extern		int			forcetick;
extern		FILE *			fpReserve;

extern		int			greet_size;
extern		bool *			greet_table;

extern		HELP_DATA	  *	help_first;
extern		char			help_greeting[];
extern		HELP_DATA	  *	help_last;
extern		int			hour_sunrise;
extern          int                     hiring          [];
extern		int			hour_sunset;
extern		KILL_DATA		kill_table	[];
extern		struct last_cmd_buf	lcb[];
extern		int			lci;
extern		char			log_buf		[];
extern		int			map_mem;
extern		bool			merc_down;
extern		bool			MOBtrigger;
extern		int			most_on;
extern		int			most_yesterday;
extern		bool			newlock;
extern		int			object_count;
extern		OBJ_DATA	  *	object_list;

extern		int			port;
extern		PROJECT_DATA *		project_last;
extern		PROJECT_DATA *		project_list;

extern		struct rebuild_data	rebuild;
extern		int			recursion_counter;

extern		SHOP_DATA	*	shop_first;
extern		SHOP_DATA	*	shop_last;
extern		SOCIAL_DATA	*	social_first;
extern		SOCIAL_DATA	*	social_last;
extern		char		*	sql_db;
extern		bool			sql_flag;
extern		char		*	sql_host;
extern		char		*	sql_passwd;
extern		int			sql_port;
extern		char		*	sql_socket;
extern		char		*	sql_user;
extern		time_t			startup_time;
extern		char			str_boot_time[];
extern		int			sys_pkill;
extern		bool			sys_webwho;

extern		TIME_INFO_DATA		time_info;
extern		int			today;
extern		int			top_alias;
extern		int			top_area;
extern		int			top_avatar;
extern		int			top_ban;
extern		int			top_bank;
extern		int			top_board;
extern		int			top_board_info;
extern		int			top_buffer;
extern		int			top_char;
extern		int			top_clan;
extern		int			top_clist;
extern		int			top_desc;
extern		int			top_dream;
extern		int			top_dream_seg;
extern		int			top_dreamlist;
extern		int			top_help;
extern		int			top_ignore;
extern		int			top_milist;
extern		int			top_mpact;
extern		int			top_mprog;
extern		int			top_note;
extern		int			top_oilist;
extern		int			top_oreprog;
extern		int			top_overland;
extern		int			top_pcdata;
extern		int			top_project;
extern		int			top_race;
extern		int			top_reset;
extern		int			top_room;
extern		int			top_shop;
extern		int			top_skill;
extern		int			top_textlist;
extern		int			top_travel;
extern		int			top_userlist;
extern		int			top_vnum_dream;
extern		int			top_vnum_mob;
extern		int			top_vnum_obj;
extern		int			top_vnum_room;
extern		int			top_vroom;

extern		USERLIST_DATA	*	user_first;
extern		USERLIST_DATA	*	user_last;

extern		bool			vehicle_moved;

extern		WEATHER_DATA		weather_info;
extern		char			whoborder_imm[];
extern		char			whoborder_mort[];
extern		int			whosock;	/* socket for web who */
extern		bool			wizlock;
extern      int         reboot_counter;

/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */

#define BOARDS_DIR	"../boards/"		/* notes/boards */
#define EXPORT_DIR	"../export/"		/* for exporting/renaming areas */
#define GOD_DIR         "../gods/"  		/* list of gods */
#define IMAGE_DIR	"/tmp/"			/* image output */
#define LOG_DIR		"../log/"		/* directory for log files */
#define OBJ_DIR		"o/"			/* Path to object files */
#define PLAYER_DIR      "../player/"        	/* Player files */
#define SYSTEM_DIR	"../system/"		/* System data files */
#define SOURCE_DIR	"../src"		/* Source code directory */
#define TEMP_FILE	"../player/romtmp"
#define TEMP_DIR	"../tmp/"
#define UPLOAD_DIR	"../uploads/"		/* Upload dir for desc imports */
#define WEB_DIR		"../html/"		/* Web page/HTML stuff */
#define VAULT_DIR       "../vault/"             /* Where donation pit contents are stored. */

#define AREA_LIST       "area.lst" 	/* List of areas*/
#define AREA_LIST_BP	"areaBP.lst"	/* List of areas if not game port */
#define BADNAME_FILE	"badnames.txt"	/* List of disallowed names for new chars */
#define BAN_FILE	"ban.txt"
#define BOARDS_FILE	"boards.txt"	/* List of available note boards */
#define BOOTTIME_FILE	"boottime.txt"	/* time the mud started up (coldboot) */
#define BUG_FILE        "bugs.txt"	/* For 'bug' and bug() */
#define CHANGES_FILE   "changes.txt"  /* The to-do file */
#define CLAN_FILE	"clans.txt"	/* Clan data file */
#define COMMAND_FILE	"commands.txt"	/* Commands file */
#define CONFIG_FILE	"config.dat"	/* System configuration */
#define DREAM_FILE	"dreams.txt"	/* Dreams */
#define EXE_TEMP	"temp.egate"	/* For rebuild command */
#define GREET_FILE	"greet.dat"	/* Table of who knows whom */
#define GSKILL_FILE	"gskills.txt"	/* Skill groups */
#define HOTBOOT_FILE	"hotboot.dat"	/* Hotboot data */
#define HOTBOOT_SAVE	"hotboot.save"	/* Hotboot data */
#define MAX_PLAYERS	"maxtoday.txt"	/* most players on today */
#define NOHELP_FILE	"nohelp.txt"	/* Unsuccessful help requests. */
#define NULL_FILE	"/dev/null"		/* To reserve one stream */
#define PROJECT_FILE	"projects.txt"	/* Projects */
#define QUEST_FILE	"quest.log"	/* Log of all quest events */
#define RACE_FILE	"races.txt"	/* data for race table */
#define REBUILD_LOG	"rebuild.log"	/* Rebuild (recompile) log */
#define RECENT_FILE	"recent.txt"	/* Recent connects */
#define SECGROUP_FILE	"security.txt"	/* Command group security file */
#define SHUTDOWN_FILE   "shutdown.txt"	/* For 'shutdown'*/
#define SKILL_FILE	"skills.txt"	/* Skill table */
#define SOCIAL_FILE	"socials.txt"
#define SQL_FILE	"sql.conf"	/* parameters for MySQL */
#define TODO_FILE	"todo.txt"	/* The to-do file */
#define TRAVEL_FILE	"travel.dat"	/* Traveling rooms state */
#define TYPO_FILE       "typos.txt"	/* For 'typo'*/
#define USER_FILE	"userlist.txt"	/* list of users */
#define VEHICLE_FILE	"vehicles.dat"	/* Locations of vehicles file */
#define WHOBORDER_FILE	"whoborder.txt"	/* The who border info */
#define VAULT_LIST      "vault.lst"     /* The donation pit contents */

DECLARE_EVENT_FUN( event_char_mpdelay	);
DECLARE_EVENT_FUN( event_char_nopk	);
DECLARE_EVENT_FUN( event_dream		);

DECLARE_EVENT_FUN( event_desc_idle	);
DECLARE_EVENT_FUN( event_desc_keepalive );
DECLARE_EVENT_FUN( event_desc_login	);

DECLARE_EVENT_FUN( event_exit_mpdelay	);

DECLARE_EVENT_FUN( event_obj_mpdelay	);
DECLARE_EVENT_FUN( event_obj_fall	);

DECLARE_EVENT_FUN( event_room_mpdelay	);
DECLARE_EVENT_FUN( event_room_sail	);

DECLARE_EVENT_FUN( event_save_userlist	);


/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(	spec_archer		);
DECLARE_SPEC_FUN(	spec_beholder		);
DECLARE_SPEC_FUN(	spec_breath_acid	);
DECLARE_SPEC_FUN(	spec_breath_any		);
DECLARE_SPEC_FUN(	spec_breath_fire	);
DECLARE_SPEC_FUN(	spec_breath_frost	);
DECLARE_SPEC_FUN(	spec_breath_gas		);
DECLARE_SPEC_FUN(	spec_breath_lightning	);
DECLARE_SPEC_FUN(	spec_cast_adept		);
DECLARE_SPEC_FUN(	spec_cast_cleric	);
DECLARE_SPEC_FUN(	spec_cast_judge		);
DECLARE_SPEC_FUN(	spec_cast_mage		);
DECLARE_SPEC_FUN(	spec_cast_undead	);
DECLARE_SPEC_FUN(	spec_chimera		);
DECLARE_SPEC_FUN(	spec_customs_weapons	);
DECLARE_SPEC_FUN(	spec_executioner	);
DECLARE_SPEC_FUN(	spec_fido		);
DECLARE_SPEC_FUN(	spec_guard		);
DECLARE_SPEC_FUN(	spec_janitor		);
DECLARE_SPEC_FUN(	spec_nasty		);
DECLARE_SPEC_FUN(	spec_ogre_member	);
DECLARE_SPEC_FUN(	spec_patrolman		);
DECLARE_SPEC_FUN(	spec_poison		);
DECLARE_SPEC_FUN(	spec_questmaster	);
DECLARE_SPEC_FUN(	spec_resurrect		);
DECLARE_SPEC_FUN(	spec_thief		);
DECLARE_SPEC_FUN(	spec_troll_member	);


/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define AD	AFFECT_DATA
#define BDD	BOARD_DATA
#define CD	CHAR_DATA
#define CL	CLAN_DATA
#define DRD	DREAM_DATA
#define ED	EXIT_DATA
#define EVD	EVENT_DATA
#define HD	HELP_DATA
#define MID	MOB_INDEX_DATA
#define OID	OBJ_INDEX_DATA
#define OD	OBJ_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN
#define UL	USERLIST_DATA

/* act_clan.c */
void	insert_clan	args( ( CLAN_DATA *pClan ) );
void	load_clans	args( ( void ) );
void	save_clans	args( ( void ) );
void	sort_clans	args( ( void ) );

/* act_comm.c */
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	auc_channel	args( ( const char *argument ) );
void  	check_sex	args( ( CHAR_DATA *ch) );
void	die_follower	args( ( CHAR_DATA *ch ) );
void	herald_announce	args( ( const char *s ) );
void	info		args( ( const char *str, CHAR_DATA *ich, void *vo,
				bitvector flag, bitvector flag_skip, int minlevel ) );
bool	is_same_group	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void 	nuke_pets	args( ( CHAR_DATA *ch ) );
const char *parse_social_string args( ( const char *s ) );
void	sayto		args( ( CHAR_DATA *ch, CHAR_DATA *vch, const char *fmt, ... ) __attribute__ ( ( format( printf, 3, 4 ) ) ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );

/* act_enter.c */
RID  *get_random_room   args ( (CHAR_DATA *ch) );

/* act_info.c */
bool	check_blind	args( ( CHAR_DATA *ch ) );
const char *level_range args( ( AREA_DATA *pArea ) );
void	set_title	args( ( CHAR_DATA *ch, char *title ) );
void	show_affects	args( ( CHAR_DATA *ch, CHAR_DATA *vch, BUFFER *buf ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
void	show_score	args( ( CHAR_DATA *ch, CHAR_DATA *vch ) );
void	web_who		args( ( int fd ) );
bool	who_comp	args( ( CHAR_DATA *ch1, CHAR_DATA *ch2 ) );

/* act_misc.c */

/* act_move.c */
void	check_life_xtal	args( ( CHAR_DATA *ch ) );
void	check_nofloor	args( ( CHAR_DATA *ch ) );
void	check_riding	args( ( DESCRIPTOR_DATA *d ) );
void	check_room_aff	args( ( CHAR_DATA *ch ) );
int	find_door	args( ( CHAR_DATA *ch, char *arg, bool fSecret ) );
void	move_char	args( ( CHAR_DATA *ch, int door, bool follow ) );
bool	move_vehicle	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *veh, int dir ) );
void	recall_char	args( ( CHAR_DATA *ch, char *argument, int room ) );
bool	set_dreaming	args( ( CHAR_DATA *ch ) );

/* act_obj.c */
bool	add_obj_fall_event	args( ( OBJ_DATA *obj ) );
bool	can_loot	args( ( CHAR_DATA *ch, OBJ_DATA *obj) );
bool	can_use_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fMsg ) );
void    get_obj         args( ( CHAR_DATA *ch, OBJ_DATA *obj,
                            OBJ_DATA *container ) );
const char * god_name	args( ( CHAR_DATA *ch ) );
int	handcount	args( ( CHAR_DATA *ch ) );
void	wear_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
void    save_donation_pit args( ( CHAR_DATA *ch, char *vnum ) );
/* act_skills.c */
const char * get_health_str args( ( int percent ) );

/* act_wiz.c */
ROOM_INDEX_DATA *	find_location	args( ( CHAR_DATA *ch, char *arg ) );
void	wiznet		args( (char *string, CHAR_DATA *ch, void *vo,
			       bitvector flag, bitvector flag_skip, int min_level ) );
void reboot_mars     args( ( void) );
/* act_wiz2.c */
int days_since_last_file_mod args((char *filename));

/* alias.c */
void 	substitute_alias args( (DESCRIPTOR_DATA *d, char *input) );

/* ban.c */
bool	check_ban	args( ( char *site, int type) );

/* boards.c */
BDD *	board_lookup	args( ( const char *name ) );
BDD *	find_board	args( ( CHAR_DATA *ch, const char *argument ) );
BOARD_INFO *
	get_board_info	args( ( CHAR_DATA *ch, const char *name ) );
char *	get_note_text	args( ( CHAR_DATA *ch, char *string ) );
BDD *	next_board	args( ( CHAR_DATA *ch, BOARD_DATA *pBoard ) );
void	show_board_list	args( ( CHAR_DATA *ch, bool fAll ) );

/* comm.c */
void	act		args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type ) );
void	act_new		args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type,
			    int min_pos) );
void	act_color	args( ( int AType, const char *format,
				CHAR_DATA *ch, const void *arg1,
				const void *arg2, int type, int min_pos) );
void    add_keepalive   args( ( CHAR_DATA *ch ) );
void	bust_a_prompt	args( ( CHAR_DATA *ch ) );
void	ch_printf	( CHAR_DATA *ch, char *fmt, ... ) __attribute__ ((format (printf, 2, 3 )));
void	close_socket	args( ( DESCRIPTOR_DATA *dclose ) );
void	count_players	args( ( void ) );
void	fix_sex		args( ( CHAR_DATA *ch ) );
int	get_char_color	args( ( CHAR_DATA *ch ) );
void	page_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void	queue_userlist_save args( ( void ) );
void	save_userlist	args( ( void ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void	send_to_room	args( ( int color, ROOM_INDEX_DATA *room,
				const char *txt, int min_position ) );
int	set_char_color	args( ( int AType, CHAR_DATA *ch ) );
void	show_string	args( ( struct descriptor_data *d, char *input) );
void	update_userlist	args( ( CHAR_DATA *ch, bool fLogon ) );
void	write_to_buffer	args( ( DESCRIPTOR_DATA *d, const char *txt,
			    int length ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length, bool fAnsi ) );

/* File open/close wrappers to handle fpreserve correctly in all cases.
 * Also does some basic error checking. -Chilalin
 */
FILE *  file_open       args( ( const char *path, const char *mode ) );
void    file_close      args( ( FILE *fp ) );
bool    file_exists     args( ( const char *path ) );

/* db.c */
void *	alloc_mem	args( ( unsigned sMem ) );
void *	alloc_perm	args( ( unsigned sMem ) );
const char * aoran	args( ( const char *str ) );
const char * aoran_skip	args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	area_update	args( ( void ) );
void	boot_db		args( ( void ) );
void	bug		args( ( const char *str, int param ) );
void	bugf		args( ( const char *fmt, ... ) __attribute__ (( format( printf, 1, 2 ) )) );
void	buildbug	args( ( const char *fmt, ... ) __attribute__ (( format( printf, 1, 2 ) )) );
char *	capitalize	args( ( const char *str ) );
bool    check_mssp      args( ( DESCRIPTOR_DATA *d, char *argument ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
void	clone_mobile	args( ( CHAR_DATA *parent, CHAR_DATA *clone) );
void	clone_object	args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
int	dice		args( ( int number, int size ) );
void	fix_sectors	args( ( AREA_DATA *pArea ) );
long	flag_convert	args( ( char letter) );
char *	fread_buf	args( ( FILE *fp, char *buf, int max ) );
bitvector fread_flag	args( ( FILE *fp ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp ) );
char *	fread_string	args( ( FILE *fp ) );
char *  fread_string_eol args(( FILE *fp ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp ) );
EXT_BV	fread_xbits	args( ( FILE *fp ) );
void	free_mem	args( ( void *pMem, unsigned sMem ) );
void	free_string	args( ( char *pstr ) );
DRD *	get_dream_index args( ( int vnum ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
RID *	get_vroom_index	args( ( int vnum ) );
bool	isavowel	args( ( char letter ) );
void	log_printf	args( ( const char *fmt, ... ) __attribute__ (( format( printf, 1, 2 ) )) );
void    logf2           args( ( const char * fmt, ... ) );
void	log_string	args( ( const char *str ) );
int	number_bits	args( ( int width ) );
int	number_door	args( ( void ) );
int	number_fuzzy	args( ( int number ) );
long	number_mm       args( ( void ) );
int	number_percent	args( ( void ) );
int	number_range	args( ( int from, int to ) );
int	interpolate	args( ( int level, int value_00, int value_99 ) );
char *	print_flags	args( ( bitvector flag ) );
void	reset_room	args( ( ROOM_INDEX_DATA *pRoom ) );
void	set_mob_stats	args( ( CHAR_DATA *pMob, int difficulty ) );
void	smash_tilde	args( ( char *str ) );
bool	str_cmp		args( ( const char *astr, const char *bstr ) );
char *	str_dup		args( ( const char *str ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_match	args( ( const char *arg, const char *astr, const char *bstr ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
void	tail_chain	args( ( void ) );

/* db2.c */
void	abort_mud	args( ( char *reason ) );
void	insert_dream	args( ( DREAM_DATA *pDream ) );

/* effect.c */
void	acid_effect	args( (void *vo, int level, int dam, int target) );
void	cold_effect	args( (void *vo, int level, int dam, int target) );
void	fire_effect	args( (void *vo, int level, int dam, int target) );
void	poison_effect	args( (void *vo, int level, int dam, int target) );
void	shock_effect	args( (void *vo, int level, int dam, int target) );

/* event.c */
void	add_event_area	args( ( EVENT_DATA *event, AREA_DATA *ch, int delay ) );
void	add_event_char	args( ( EVENT_DATA *event, CHAR_DATA *ch, int delay ) );
void	add_event_desc	args( ( EVENT_DATA *event, DESCRIPTOR_DATA *d, int delay ) );
void	add_event_exit	args( ( EVENT_DATA *event, EXIT_DATA *exit, int delay ) );
void	add_event_game	args( ( EVENT_DATA *event, int delay ) );
void	add_event_obj	args( ( EVENT_DATA *event, OBJ_DATA *obj, int delay ) );
void	add_event_room	args( ( EVENT_DATA *event, ROOM_INDEX_DATA *room, int delay ) );
void	dequeue_event	args( ( EVENT_DATA *event ) );
int	event_time_left	args( ( EVENT_DATA *event ) );
EVD *	get_event_area	args( ( AREA_DATA *area, int type ) );
EVD *	get_event_char	args( ( CHAR_DATA *ch, int type ) );
EVD *	get_event_desc	args( ( DESCRIPTOR_DATA *d, int type ) );
EVD *	get_event_exit	args( ( EXIT_DATA *exit, int type ) );
EVD *	get_event_game	args( ( int type ) );
EVD *	get_event_obj	args( ( OBJ_DATA *obj, int type ) );
EVD *	get_event_room	args( ( ROOM_INDEX_DATA *room, int type ) );
void	strip_event_area args(( AREA_DATA *area, int type ) );
void	strip_event_char args(( CHAR_DATA *ch, int type ) );
void	strip_event_desc args(( DESCRIPTOR_DATA *d, int type ) );
void	strip_event_exit args(( EXIT_DATA *exit, int type ) );
void	strip_event_game args(( int type ) );
void	strip_event_obj	args( ( OBJ_DATA *obj, int type ) );
void	strip_event_room args(( ROOM_INDEX_DATA *room, int type ) );
void	update_events	args( ( void ) );

/* fight.c */
void	death_cry	args( ( CHAR_DATA *victim, int dam ) );
bool 	is_safe		args( (CHAR_DATA *ch, CHAR_DATA *victim ) );
bool 	is_safe_spell	args( (CHAR_DATA *ch, CHAR_DATA *victim, bool area ) );
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
bool	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			        int dt, int class, bool show ) );
bool    damage_old      args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                                int dt, int class, bool show ) );
void    raw_kill        args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	stuff_corpse	args( ( CHAR_DATA *ch, OBJ_DATA *corpse ) );
void	check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim) );

/* flags.c */
int	flag_lookup	args( ( const char *name,
				const struct flag_type *flag_table) );
char *	flag_string	args( ( const struct flag_type *flag_table,
				         int bits ) );
extern int flag_value	args( ( const struct flag_type *flag_table,
				 const char *argument ) );
char *	xbit_string	args( ( const struct flag_type *flag_table, EXT_BV bits ) );

/* handler.c */
void	affect_enchant	args( ( OBJ_DATA *obj ) );
void	affect_modify	args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
void	affect_check	args( ( CHAR_DATA *ch, int where, int vector ) );
AD *	affect_find	args( ( AFFECT_DATA *paf, int sn ) );
void	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove_obj args(( OBJ_DATA *obj, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_to_obj	args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
int	apply_ac	args( ( OBJ_DATA *obj, int iWear, int type ) );
int	attack_lookup	args( ( const char *name) );
int	baseclass	args( ( CHAR_DATA *ch ) );
const char * baseclassname	args( ( int iClass ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_see_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
int 	check_immune	args( ( CHAR_DATA *ch, int dam_type ) );
bool	check_nightmare	args( ( CHAR_DATA *ch ) );
bool	char_died	args( ( CHAR_DATA *ch ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
int	class_lookup	args( ( const char *name) );
char *  comm_bit_name	args( ( int comm_flags ) );
int	count_lines	args( ( const char *str ) );
int	count_obj_levels args(( OBJ_DATA *obj ) );
int	count_obj_list	args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
int	count_pages	args( ( OBJ_INDEX_DATA *pObj ) );
int	count_users	args( ( OBJ_DATA *obj ) );
OD *	create_money	args( ( MONEY *amount ) );
char *	date_string	args( ( time_t date_val ) );
void 	deduct_cost	args( ( CHAR_DATA *ch, int cost ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
void	extract_obj	args( ( OBJ_DATA *obj ) );
AREA_DATA *find_area	args( ( CHAR_DATA *ch, char *argument ) );
DREAM_DATA *find_dream	args( ( int vnum ) );
char *	fix_string	args( ( const char *str ) );
int     get_age         args( ( CHAR_DATA *ch ) );
AREA_DATA *get_area	args( ( int vnum ) );
CD *	get_char_area	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
CL *	get_clan	args( ( const char *argument ) );
CL *	get_clan_index	args( ( int index ) );
int	get_curr_stat	args( ( CHAR_DATA *ch, int stat ) );
OD *	get_eq_char	args( ( CHAR_DATA *ch, int iWear ) );
HD *	get_help	args( ( char *key, bool fExact ) );
int	get_landmass	args( ( ROOM_INDEX_DATA *room ) );
int 	get_max_train	args( ( CHAR_DATA *ch, int stat ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
				OBJ_DATA *list ) );
OD *	get_obj_bank	args( ( CHAR_DATA *ch, BANK_DATA *bank, char *argument ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument,
				CHAR_DATA *viewer ) );
int	get_obj_count	args( ( OBJ_DATA *obj ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
int	get_obj_number	args( ( OBJ_DATA *obj ) );
OD *	get_obj_room	args( ( OBJ_INDEX_DATA *pObjIndex, ROOM_INDEX_DATA *pRoom ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
RID *	get_room_room	args( ( CHAR_DATA *ch, char *argument ) );
int	get_skill	args( ( CHAR_DATA *ch, int sn ) );
int	get_true_weight	args( ( OBJ_DATA *obj ) );
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_weapon_skill args(( CHAR_DATA *ch, int sn ) );
int	get_weapon_sn	args( ( CHAR_DATA *ch ) );
bool	has_colcode	args( ( const char *p ) );
bool	has_same_word	args( ( const char *alist, const char *blist ) );
int	info_lookup	args( ( const char *name ) );
void	insert_room	args( ( ROOM_INDEX_DATA *room ) );
bool	is_affected	args( ( CHAR_DATA *ch, int sn ) );
bool	is_clan		args( ( CHAR_DATA *ch ) );
bool	is_colcode	args( ( const char *s ) );
bool	is_donation_box	args( ( OBJ_DATA *obj ) );
bool	is_exact_name	args( ( char *str, const char *namelist ) );
bool	is_friend	args( ( CHAR_DATA *ch,CHAR_DATA *victim ) );
bool	is_ignoring	args( ( CHAR_DATA *ch, CHAR_DATA *vch, bitvector flags ) );
bool	is_metal	args( ( OBJ_DATA *obj ) );
bool	is_name		args( ( char *str, char *namelist ) );
bool	is_room_owner	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool	is_same_clan	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_same_landmass args(( ROOM_INDEX_DATA *src_room, ROOM_INDEX_DATA *dst_room ) );
bool	is_same_world	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_water	args( ( ROOM_INDEX_DATA * room ) );
bool	knows_char	args( ( CHAR_DATA *ch, CHAR_DATA *vch ) );
void deduct_move( CHAR_DATA *ch, int amount );
const char *landmass_name args( ( int index ) );
int	liq_lookup	args( ( const char *name) );
int 	material_lookup args( ( const char *name) );
int	money_add	args( ( MONEY *augend, MONEY *addend, bool fNormalize ) );
int	money_compare	args( ( MONEY *amt1, MONEY *amt2 ) );
int	money_muldiv	args( ( MONEY *amt, int multiplier, int divisor ) );
int	money_subtract	args( ( MONEY *minuend, MONEY *subtrahend, bool fNormal ) );
char *	money_string	args( ( MONEY *amount, bool fZero, bool fShort ) );
MONEY *	money_value	args( ( MONEY *money, const char *str ) );
bool	mount_success	args( ( CHAR_DATA *ch, CHAR_DATA *mount, bool canattack ) );
void	normalize	args( ( MONEY *money ) );
bool	obj_extracted	args( ( OBJ_DATA *pObj ) );
void	obj_from_bank	args( ( OBJ_DATA *obj ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
void	obj_from_room	args( ( OBJ_DATA *obj ) );
void	obj_to_bank	args( ( OBJ_DATA *obj, BANK_DATA *bank ) );
void	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
bool	purge_room	args( ( ROOM_INDEX_DATA *pRoom ) );
int	race_lookup	args( ( const char *name) );
void	reset_char	args( ( CHAR_DATA *ch )  );
void	room_from_room	args( ( ROOM_INDEX_DATA *room ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
void	room_to_room	args( (ROOM_INDEX_DATA *room, ROOM_INDEX_DATA *to_room ) );
char *	show_color_codes args( ( char *dst, const char *src ) );
char *	strip_color	args( ( char *dest, const char *src ) );
char *	strip_trailing_comma args( ( char *s ) );
int	strlen_wo_col	args( ( const char *s ) );
char *	str_lower	args( ( char *dst, const char *src ) );
char *	str_upper	args( ( char *dst, const char *src ) );
char *	strncpy_wocol	args( ( char *dest, const char *src, int n ) );
bool    can_gate(CHAR_DATA *ch, CHAR_DATA *victim);
void    transfer_char(CHAR_DATA *ch, CHAR_DATA *vch,
                      ROOM_INDEX_DATA *to_room,
                      const char *msg_out,
                      const char *msg_travel,
                      const char *msg_in);
const char *strtimediff	args( ( time_t start, time_t stop ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	update_greet_data args( ( void ) );
void	update_known	args( ( CHAR_DATA *ch, CHAR_DATA *vch, bool fSet ) );
UL   *	user_lookup	args( ( const char *name ) );
int	weapon_lookup	args( ( const char *name) );
int	weapon_type	args( ( const char *name) );
char *	weapon_name	args( ( int weapon_Type) );
const char * weight_string args( ( int weight ) );
bool	weight_value	args( ( const char *str, int *weight ) );
int	wiznet_lookup	args( ( const char *name ) );

char *	affect_bit_name	args( ( EXT_BV vector ) );
char *	affect_bitval_name args( ( int vector ) );
char *	affect_loc_name	args( ( int location ) );
char *	act_bit_name	args( ( bitvector act_flags ) );
char *	cont_bit_name	args( ( bitvector cont_flags) );
char *	extra_bit_name	args( ( bitvector extra_flags ) );
char *	extra2_bit_name	args( ( bitvector extra_flags2 ) );
char * 	form_bit_name	args( ( bitvector form_flags ) );
char *  imm_bit_name	args( ( bitvector imm_flags ) );
char *	off_bit_name	args( ( bitvector off_flags ) );
char *	part_bit_name	args( ( bitvector part_flags ) );
char *	shield_bit_name	args( ( EXT_BV vector ) );
char *	shield_bitval_name args( ( int vector ) );
char *	weapon_bit_name	args( ( bitvector weapon_flags ) );
char * 	wear_bit_name	args( ( bitvector wear_flags ) );
bool    can_pack        args( ( CHAR_DATA *ch ) );

/* interp.c */
char *	get_money_string args( ( char *argument, char *buffer ) );
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( const char *arg ) );
int	mult_argument	args( ( char *argument, char *arg) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( const char *argument, char *arg_first ) );

/* magic.c */
int	find_spell	args( ( CHAR_DATA *ch, const char *name) );
int 	mana_cost 	(CHAR_DATA *ch, int min_mana, int level);
int	skill_find	args( ( const char *name ) );
int	skill_lookup	args( ( const char *name ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim, int dam_type ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
				    CHAR_DATA *victim, OBJ_DATA *obj ) );
/* multi.c */
bool	can_use_skpell	args( ( CHAR_DATA *ch, int sn ) );
bool	is_class	args( ( CHAR_DATA *ch, int classnum ) );
int	prime_class	args( ( CHAR_DATA *ch ) );

/* nanny.c */
void	check_death_cmds args( ( DESCRIPTOR_DATA* d, char *argument ) );
bool	check_legal_name args( ( char *name ) );
bool	check_parse_name args( ( char *name ) );
bool	create_rift_mob	 args( ( CHAR_DATA *ch ) );
void	die_pc		 args( ( CHAR_DATA *ch ) );
OBJ_DATA *find_corpse	 args( ( CHAR_DATA *ch ) );
void	nanny  		 args( ( DESCRIPTOR_DATA *d, char *argument ) );

/* olc.c */
const	char *	olc_ed_name	args( ( CHAR_DATA *ch ) );
const	char *	olc_ed_vnum	args( ( CHAR_DATA *ch ) );
	bool	run_olc_editor	args( ( DESCRIPTOR_DATA *d, char *argument ) );

/* olc_area.c */
void	show_area_info		args( ( AREA_DATA *pArea, BUFFER *pBuf ) );
void	show_travel_info	args( ( AREA_DATA *pArea, BUFFER *pBuf ) );

/* olc_board.c */
void	show_board		args( ( CHAR_DATA *ch, BOARD_DATA *board,
					BUFFER *buf ) );
/* olc_dream.c */
void	show_dream_info		args( ( DREAM_DATA *pDream, BUFFER *pBuf ) );

/* olc_mob.c */
bool	delete_mobile		args( ( MOB_INDEX_DATA *pMob ) );
void	show_mob_info		args( ( CHAR_DATA *ch, MOB_INDEX_DATA *pMob, BUFFER *pBuf ) );

/* olc_obj.c */
bool	delete_object		args( ( OBJ_INDEX_DATA *pObjIndex ) );
void	show_obj_info		args( ( CHAR_DATA *ch, OBJ_INDEX_DATA *pMob, BUFFER *pBuf ) );

/* olc_proj.c */
void	show_project		args( ( CHAR_DATA *ch, PROJECT_DATA *pProj ) );

/* olc_race.c */
void	show_race_info		args( ( int iRace, BUFFER *buf ) );

/* olc_room.c */
void	add_reset		args( ( ROOM_INDEX_DATA *room,
                                        RESET_DATA *pReset, int index ) );
bool	delete_room		args( ( ROOM_INDEX_DATA *pRoom ) );
int	get_eprog_dir		args( ( ROOM_INDEX_DATA *pRoom, OREPROG_DATA *pProg ) );
void	show_room_info		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom,
					BUFFER *pBuf ) );
/* olc_save.c */
char *	print_xbits		args( ( EXT_BV *bits ) );
void	save_boards_file	args( ( void ) );
void	save_cmd_files		args( ( void ) );
void	save_greet_data		args( ( void ) );
void	save_projects		args( ( void ) );
void	save_race_file		args( ( void ) );
bool	save_skill_files	args( ( void ) );
void	save_socials		args( ( void ) );
void	save_sysconfig		args( ( void ) );
void	save_vehicles		args( ( void ) );

/* olc_skill.c */
void	show_skill_info		args( ( int sn, BUFFER * pBuf ) );

/* overland.c */
char *	format_map		args( ( char *map, char *desc, char areacolor ) );
bool	get_coordinates		args( ( ROOM_INDEX_DATA *room, int *x, int *y ) );
ED *	get_exit		args( ( ROOM_INDEX_DATA *room, int dir ) );
RID *	get_virtual_room	args( ( int vnum ) );
char *	make_minimap		args( ( AREA_DATA *pArea, int offset ) );

/* progs.c */
void	mprog_percent_check     args ( ( CHAR_DATA *mob, CHAR_DATA* actor,
					OBJ_DATA* object, void* vo,
					bitvector type ) );
void    mprog_wordlist_check    args ( ( char * arg, CHAR_DATA *mob,
                			CHAR_DATA* actor, OBJ_DATA* object,
					void* vo, bitvector type ) );
void	progbug			args ( ( CHAR_DATA *ch, const char *fmt, ... )__attribute__ ((format (printf, 2, 3 )) ) )
				  __attribute__ ((format (printf, 2, 3 )));

void    eprog_bash_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_close_trigger     args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_enter_trigger     args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_exit_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_knock_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
bool    eprog_listen_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_lock_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, OBJ_DATA *obj ) );
void    eprog_look_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_open_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_pass_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, bool fEnter ) );
void    eprog_pick_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_scry_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_unlock_trigger    args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, OBJ_DATA *obj ) );


void    mprog_act_trigger       args ( ( char* buf, CHAR_DATA* mob,
		                        CHAR_DATA* ch, OBJ_DATA* obj,
					void* vo ) );
void    mprog_bribe_trigger	args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
					 MONEY * amount ) );
void	mprog_buy_trigger	args ( ( CHAR_DATA *mob, CHAR_DATA *ch,
					 CHAR_DATA *pet, OBJ_DATA *obj ) );
void    mprog_death_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch, int dam ) );
void    mprog_entry_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_fight_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_give_trigger      args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
                		        OBJ_DATA* obj ) );
void    mprog_greet_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_hitprcnt_trigger  args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void	mprog_list_trigger	args ( ( CHAR_DATA *mob, CHAR_DATA *ch ) );
void    mprog_random_trigger    args ( ( CHAR_DATA* mob ) );
void	mprog_repop_trigger	args ( ( CHAR_DATA *mob ) );
void	mprog_sell_trigger	args ( ( CHAR_DATA *mob, CHAR_DATA *ch,
					 OBJ_DATA *obj ) );
void    mprog_speech_trigger    args ( ( char* txt, CHAR_DATA* mob ) );
void	mprog_tell_trigger	args( ( char *txt, CHAR_DATA *mob,
					CHAR_DATA *talker ) );
void	mprog_time_trigger	args ( ( void ) );
void	mprog_whisper_trigger	args( ( char *txt, CHAR_DATA *mob,
					CHAR_DATA *talker ) );



void    oprog_buy_trigger       args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 CHAR_DATA *vendor ) );
void    oprog_cast_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_cast_sn_trigger   args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 int sn, void *vo ) );
void    oprog_close_trigger     args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_drop_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_fill_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *spring ) );
void    oprog_get_trigger       args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_get_from_trigger  args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *secondary ) );
void    oprog_give_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 CHAR_DATA *victim ) );
void    oprog_invoke_trigger    args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 void *vo ) );
void	oprog_join_trigger	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
					OBJ_DATA *obj2 ) );
void    oprog_lock_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *key ) );
void    oprog_look_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_look_in_trigger   args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_open_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_pick_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_put_trigger       args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *secondary ) );
void    oprog_random_trigger    args ( ( OBJ_DATA *obj ) );
void	oprog_read_trigger	args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	oprog_remove_trigger	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_retrieve_trigger  args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_sell_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 CHAR_DATA *vendor ) );
void	oprog_separate_trigger args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_store_trigger     args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_throw_trigger     args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_unlock_trigger    args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *key ) );
void	oprog_unuse_trigger	args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
bool    oprog_use_trigger       args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 void *vo ) );
void    oprog_wear_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );

bool	rprog_arrive_trigger	args ( ( ROOM_INDEX_DATA *here, ROOM_INDEX_DATA *there ) );
void	rprog_boot_trigger	args ( ( ROOM_INDEX_DATA *room ) );
void    rprog_cast_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_cast_sn_trigger   args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, int sn,
					 void *vo ) );
void    rprog_death_trigger     args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
bool	rprog_depart_trigger	args ( ( ROOM_INDEX_DATA *here, ROOM_INDEX_DATA *there ) );
void    rprog_enter_trigger     args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_exit_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
bool	rprog_interp_trigger	args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, char *txt ) );
bool    rprog_listen_trigger    args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
bool	rprog_look_trigger	args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, char *arg ) );
void    rprog_pass_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
bool	rprog_pull_trigger	args ( ( ROOM_INDEX_DATA *room, CHAR_DATA *ch,
					 char *arg ) );
bool	rprog_push_trigger	args ( ( ROOM_INDEX_DATA *room, CHAR_DATA *ch,
					 char *arg ) );
void    rprog_random_trigger    args ( ( ROOM_INDEX_DATA *room ) );
bool    rprog_rest_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
bool    rprog_sit_trigger	args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
bool    rprog_sleep_trigger     args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
bool    rprog_smell_trigger     args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void	rprog_speech_trigger	args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, char *txt ) );
bool    rprog_stand_trigger	args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_time_trigger      args ( ( ROOM_INDEX_DATA *room, int hour ) );
void    rprog_wake_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, CHAR_DATA *rch ) );

/* quest.c */
void	log_qp			args( ( CHAR_DATA *ch, int value, int how ) );
void	quest_log		args( ( const char *fmt, ... ) __attribute__ (( format( printf, 1, 2 ) )) );
void	sayf			( CHAR_DATA *ch, const char *fmt, ... ) __attribute__ ((format (printf, 2, 3 )));

/* save.c */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );

/* skills.c */
bool 	parse_gen_groups args( ( CHAR_DATA *ch,char *argument ) );
void 	list_group_costs args( ( CHAR_DATA *ch ) );
void    list_group_known args( ( CHAR_DATA *ch ) );
int 	exp_per_level	args( ( CHAR_DATA *ch, int points ) );
void 	check_improve	args( ( CHAR_DATA *ch, int sn, bool success,
				    int multiplier ) );
int	get_prereq_ability args( ( CHAR_DATA *ch, int sn ) );
int 	group_find	args( (const char *name) );
int 	group_lookup	args( (const char *name) );
void	gn_add		args( ( CHAR_DATA *ch, int gn) );
void 	gn_remove	args( ( CHAR_DATA *ch, int gn) );
void 	group_add	args( ( CHAR_DATA *ch, const char *name, bool deduct) );
void	group_remove	args( ( CHAR_DATA *ch, const char *name) );

/* special.c */
SF *	spec_lookup	args( ( const char *name ) );
char *	spec_name	args( ( SPEC_FUN *function ) );

/* sql.c */
void	sql_add_user		args( ( CHAR_DATA *ch ) );
void	sql_remove_user		args( ( CHAR_DATA *ch ) );
void	sql_save_char		args( ( CHAR_DATA *ch ) );
void	sql_update_level	args( ( CHAR_DATA *ch ) );
void	sql_update_logout	args( ( CHAR_DATA *ch ) );
void	sql_update_name		args( ( const char *oldname, const char *newname ) );
void	sql_update_password	args( ( CHAR_DATA *ch ) );
void	sql_update_player_clan	args( ( CHAR_DATA *ch ) );

/* string.c */
char *	all_capitalize	args( ( const char *str ) );
char *  first_arg	args( ( char *argument, char *arg_first, bool fCase ) );
char *  format_string	args( ( char *oldstring /*, bool fSpace */ ) );
void    string_add	args( ( CHAR_DATA *ch, char *argument ) );
void    string_append	args( ( CHAR_DATA *ch, char **pString ) );
char *	string_change	args( ( CHAR_DATA *ch, char *src, char *arg ) );
char *  string_delline	args( ( CHAR_DATA *ch, char *arg1, char *arg2, char *old ) );
void	string_edit	args( ( CHAR_DATA *ch, char **pString ) );
char *	string_insline	args( ( CHAR_DATA *ch, char *argument, char *old ) );
char *	string_proper	args( ( char * argument ) );
char *	string_replace	args( ( char * orig, char * old, char * new, int offset ) );
char *	string_unpad	args( ( char * argument ) );

/* teleport.c */
RID *	room_by_name	args( ( char *target, int level, bool error) );

/* track.c */
//CD *	get_char_area	args( ( CHAR_DATA *ch, char *argument ) );

/* update.c */
void	advance_level	args( ( CHAR_DATA *ch, bool hide ) );
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	save_travel_state args( ( void ) );
void	update_handler	args( ( void ) );
#undef	UL
#undef	SF
#undef	RID
#undef	OD
#undef	OID
#undef	MID
#undef	HD
#undef	EVD
#undef	ED
#undef	DRD
#undef	CL
#undef	CD
#undef	BDD
#undef	AD
