// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Script Commands
 * Place the definition of custom script commands in this file.
 * Format:
 *	BUILDIN_DEF(command_name,"parameters"),
 **/

//BUILDIN_DEF(example,""),

// Autocombat System
BUILDIN_DEF(ac_getskillinfostring, "iv"),
BUILDIN_DEF(ac_setautocombat, "s"),
BUILDIN_DEF(autocombat_fromitem, "i"),
BUILDIN_DEF(ac_getautocombatstring, "i??"),
BUILDIN_DEF(ac_getautocombatint, "i?"),
BUILDIN_DEF(ac_getstate, ""),

// Collection System
BUILDIN_DEF(getcollectionstore, ""),
BUILDIN_DEF(getcollectionlist, "i?"),
BUILDIN_DEF(getcollectionbonus, "?"),  
BUILDIN_DEF(checkcollectioncombo, "i?"),
BUILDIN_DEF(getcollectioncombos, "i?"),
BUILDIN_DEF(getcombocollectionlist, "ii"),
BUILDIN_DEF(checkspecificcombo, "ii"),
BUILDIN_DEF(checkallcombos, "i"),
BUILDIN_DEF(activatespecificcombo, "ii"),  
BUILDIN_DEF(deactivatespecificcombo, "ii"),

// Rune System
BUILDIN_DEF(runeui, ""),
BUILDIN_DEF(getupgrade_rune, ""),