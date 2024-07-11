 
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

MARS - Multi Adventurer Roleplaying System

MARS is a ROM-based MUD codebase, with lots of changes. Many of them are to bring
the game in line with the d20 SRD (3.5e) as released under the Open Gaming License
with a world that is ready to be created and populated by the DMs. The skills and 
spells will all be based on feats and spells from the SRD, as are the races and 
classes. The majority of the game is already in line with the SRD, but there are
still some things that need to be added/changed so it is a work in progress. 

However, I felt that the best way to actually *see* that progress would be to open
MARS up to the community as a community project. I've been working on it since 2015
mostly by myself (Though parts of the code such as mounts, vehicles and overland are
from a previous project and were not written by me), starting with ROM2.4b6 as a base
and then adding/subtracting/changing code as I went. Very little of the MUD is from
snippets (as many of them would need to be modified to work with MARS anyway) though
on occassion I would see a snippet, like the overall idea, but implement it in my own
way.

At any rate, MARS will take some work on your end to compile on your system, as it has
a few requirements that most MUDs do not (SQL, ImageMagick, to name a couple) but I do
happen to know of a MUD hosting service where it will compile pretty easily;

https://vineyard.haus 

They;re my host of choice, and where I run my personal copy of MARS.

TO COMPILE & RUN
===========
cd to the src directory
type 'make'
when the MUD is finished compiling, type "nophup ./startup &"
The MUD will be running on port 3033. If you need to run it on a different port then you
would type "nohup ./startup <port number> &" replacing <port number> with the port number
you prefer to use. (do not type the <> symbols.)
Telnet or use your MUD client of choice to connect to your host on the chosen port (3033 by default)
and type "yes' or "no" for color, depending on whether or not your client supports it. Then create a
new character. Once you are finished the character creation, type "relevel ChangeMe" to 
be immediately advanced to maximum level and have your security and trust set accordingly. Afterwards
you can use the command "wizify" with your character name and max security (to make sure that your
security is set to the highest level) e.g. 'wizify mycharname 9'. This command will set your stats
to the maximum, then 'set skill mycharname all 100' to give yourself all skills/spells at 100%.
I would recommend using the "cmdedit" command to disable the relevel command, or editing do_relevl
in act_wiz.c to change the password to something stronger than 'ChangeMe' because until you either
disable to command or disable it, anyone who logs in will be able to use the relevel command to 
advance themselves to maximum level if they've read this README file. I would also consider setting
newlock on once your staff have all created characters until you are ready to open to the public.

You may notice that you are only level 30, where most ROM based MUDs are 60-200 levels. The reason
for this is due to the nature of MARS; like in D&D, MARS has an increasing amount of experience
points needed for each level. e.g. to reach level 2, you need to gain 1,000 exp. To reach level 3
you would need an additional 2,000 exp. To reach level 4, an additional 3,000, to reach level 5, 
and additional 4,000, and so on. So to reach level 5 you would need to gain a total of 10,000 exp
where in a standard ROM MUD, that much exp would put you close to level 10.

There are many such notable changes, such as no level requirements to use items. Non magic user classes
have no spells, etc. The idea is to bring it as close to D&D as possible.
