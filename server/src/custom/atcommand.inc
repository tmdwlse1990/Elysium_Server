// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Atcommands
 * Place the body of custom atcommands in this file.
 * Format:
 *	ACMD_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

//ACMD_FUNC(newcommand)
//{
//	clif_displaymessage(fd, "It works!");
//	clif_specialeffect(&sd->bl, EF_HEARTCASTING, AREA);
//	return 0;
//}
/*==========================================
* @afk
* you can use this even on autocombat state
*------------------------------------------*/
ACMD_FUNC(afk) {
 
	nullpo_retr(-1, sd);

	if (pc_isdead(sd)) {
		clif_displaymessage(fd, "You cannot enter afk mode when dead.");
		return -1;
	}

	sd->state.autotrade = 1;
	clif_authfail_fd(fd, 15);
	return 0;
}

/*==========================================  
 * @mercenaryon
 * Toggle automatic mercenary support skill usage  
 *------------------------------------------*/  
ACMD_FUNC(mercenaryon)    
{    
    nullpo_retr(-1, sd);    
        
    if (!sd->md) {    
        clif_displaymessage(fd, "You don't have a mercenary.");    
        return -1;    
    }    
        
    // Toggle the auto support flag  
    sd->md->auto_support = !sd->md->auto_support;  
        
    if (sd->md->auto_support) {    
        clif_displaymessage(fd, "Mercenary auto-skills enabled (support + aggressive).");    
        sd->md->support_timer = add_timer(gettick() + 1000, mercenary_support_timer, sd->id, 0);    
    } else {    
        clif_displaymessage(fd, "Mercenary auto-skills disabled.");    
        if (sd->md->support_timer != INVALID_TIMER) {    
            delete_timer(sd->md->support_timer, mercenary_support_timer);    
            sd->md->support_timer = INVALID_TIMER;    
        }    
    }    
        
    return 0;    
}

/*==========================================
* @reloadcollectiondb
*------------------------------------------*/
ACMD_FUNC(reloadcollectiondb){
	nullpo_retr(-1, sd);

	do_reload_collection();
	clif_displaymessage(fd, "Collection Database has been reloaded.");

	return 0;
}
