/*
    rogue_str.h - Define for change rogueWii language

    Copyright (C) 2019 Nebiun
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name(s) of the author(s) nor the names of other contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

#ifndef _rogue_str_h_
#define _rogue_str_h_

/* Strings */
#define RS_PRESS_KEY_TO_CONTINUE        "[Press 2 to continue]"
#define RS_PRESS_KEY_TO_CONTINUE2       "--Press 2 to continue--"
#define RS_PRESS_KEY_TO_EXIT       		"[Press HOME to exit]"

#define CHR_ESCAPE(x)		(((x) == RC_KEY_ABORT) || ((x) == RC_KEY_ESCAPE))
#define CHR_CTRL(x)	((x) & 0x1f)
#define CHR_UP(x)	((((x) >= 'a') && ((x) <= 'z')) ? ('A' + ((x) - 'a')) : (x))
/* Commands */
#define RC_KEY_NONE					'\0'
#define RC_KEY_CONTINUE				' '
#define RC_KEY_CONFIRM				' '
#define RC_KEY_ABORT				'Q'
#define RC_KEY_UP                               'k'
#define RC_KEY_DOWN                             'j'
#define RC_KEY_RIGHT				'l'
#define RC_KEY_LEFT                             'h'
#define RC_KEY_UPANDLEFT			'y'
#define RC_KEY_UPANDRIGHT			'u'
#define RC_KEY_DOWNANDLEFT			'b'
#define RC_KEY_DOWNANDRIGHT			'n'
#define RC_KEY_THROW				't'	// Throw an object
#define RC_KEY_FIGHT				'f'	// Fight until someone dies
#define RC_KEY_MOVE                             'm'     // Move without picking up
#define RC_KEY_ZAP                              'z'     // Zap with a staff or wand
#define RC_KEY_TRAP                             '^'     // Identify a trap
#define RC_KEY_SEARCH                           's'     // Search for traps and secret doors
#define RC_KEY_CLIMBDOWN                        '>'     // Climb down a staircase
#define RC_KEY_CLIMBUP                          '<'     // Climb up a staircase
#define RC_KEY_REST                             '.'     // Rest. Good for waiting and healing
#define RC_KEY_PICKUP				','	// Pick up something
#define RC_KEY_INVENTORY                        'i'     // Inventory
#define RC_KEY_SELECTIVEINVENTORY               'I'     // Selective inventory
#define RC_KEY_QUAFF                            'q'     // Quaff a potion
#define RC_KEY_READ                             'r'     // Read a scroll
#define RC_KEY_EAT                              'e'     // Eat food
#define RC_KEY_WIELD                            'w'     // Wield a weapon
#define RC_KEY_WEAR                             'W'     // Wear armor
#define RC_KEY_TAKEOFF                          'T'     // Take off armor
#define RC_KEY_PUTON                            'P'     // Put on a ring
#define RC_KEY_REMOVE                           'R'     // Remove a ring
#define RC_KEY_DROP                             'd'     // Drop an object
#define RC_KEY_RENAME                           'c'     // Rename an object
#define RC_KEY_DISPLAY                          'D'     // Print out discovered object
#define RC_KEY_OPTIONS                          'o'     // Examine and set options
#define RC_KEY_REFRESH				CHR_CTRL('R')	// Redraws the screen
#define RC_KEY_SHOWLASTMSG			CHR_CTRL('P')	// Print last message
#define RC_KEY_ESCAPE				'\033'			// Cancel a command, prefix, count
#define RC_KEY_TOSHELL                          '!'     // Escape to shell
#define RC_KEY_QUIT                             'Q'     // Leave the game
#define RC_KEY_SAVE                             'S'     // Save the game
#define RC_KEY_SHOWVERSION                      'v'     // Print program version
#define RC_KEY_SHOWWEAPON			')'	// Print current weapon
#define RC_KEY_SHOWARMOR                        ']'     // Print current armor
#define RC_KEY_SHOWRING                         '='     // Print current ring
#define RC_KEY_SHOWSTATUS                       '@'     // Reprint status line
#define RC_KEY_REPEAT				'a'	// Repeat last command
#define RC_KEY_HELP                             '?'     // Help
#define RC_KEY_WHATIS                           '/'     // What is this?

#ifdef MASTER
#define RC_KEY_WIZARD				'+'
#endif

#endif