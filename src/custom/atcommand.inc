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
* @statbonus
* shows stat bonus of players
* Louis T Steinhil  
*------------------------------------------*/  
void atcommand_statbonus_sub(int fd, class map_session_data *sd) {  
    char output[CHAT_SIZE_MAX];  
  
	sprintf(output, "STAT BONUSES: %s", sd->status.name);    
	clif_displaymessage(fd, output);    
	  
	clif_displaymessage(fd, "BASIC BONUSES");        
	  
	clif_displaymessage(fd, " * Base Stats");  
	bool base_stats_found = false;

	for (int param = PARAM_STR; param <= PARAM_LUK; param++) {    
		int total_bonus = sd->indexed_bonus.param_bonus[param] + sd->indexed_bonus.param_equip[param];    
			
		if (total_bonus != 0) {    
			std::string stat_name = get_stat_name(param);
				
			if (total_bonus < 0) {    
				sprintf(output, "   -> %s Bonus: %s", stat_name.c_str(), rathena::util::insert_comma(total_bonus).c_str());    
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
			} else {    
				sprintf(output, "   -> %s Bonus: +%s", stat_name.c_str(), rathena::util::insert_comma(total_bonus).c_str());    
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}    
			base_stats_found = true;    
		}    
	}
	
    if (!base_stats_found) {  
        clif_displaymessage(fd, "   -> No Base Stats bonuses found");  
    }
	
	clif_displaymessage(fd, " * Trait Stats");  
	bool trait_stats_found = false;
	
	for (int param = PARAM_POW; param <= PARAM_CRT; param++) {    
		int total_bonus = sd->indexed_bonus.param_bonus[param] + sd->indexed_bonus.param_equip[param];    
			
		if (total_bonus != 0) {    
			std::string trait_name = get_trait_name(param);    
				
			if (total_bonus < 0) {    
				sprintf(output, "   -> %s Bonus: %s", trait_name.c_str(), rathena::util::insert_comma(total_bonus).c_str());    
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
			} else {    
				sprintf(output, "   -> %s Bonus: +%s", trait_name.c_str(), rathena::util::insert_comma(total_bonus).c_str());    
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}    
			trait_stats_found = true;    
		}    
	}  
  
    if (!trait_stats_found) {  
        clif_displaymessage(fd, "   -> No Trait Stats bonuses found");  
    }

	clif_displaymessage(fd, " * HP/SP/AP");    
	bool hp_sp_ap_found = false;

	if (sd->bonus.hp != 0) {    
		if (sd->bonus.hp < 0) {    
			sprintf(output, "   -> MaxHP Bonus: %s", rathena::util::insert_comma(sd->bonus.hp).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> MaxHP Bonus: +%s", rathena::util::insert_comma(sd->bonus.hp).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		hp_sp_ap_found = true;    
	}    
	  
	if (sd->bonus.sp != 0) {    
		if (sd->bonus.sp < 0) {    
			sprintf(output, "   -> MaxSP Bonus: %s", rathena::util::insert_comma(sd->bonus.sp).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> MaxSP Bonus: +%s", rathena::util::insert_comma(sd->bonus.sp).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		hp_sp_ap_found = true;    
	}    
	  
	if (sd->bonus.ap != 0) {    
		if (sd->bonus.ap < 0) {    
			sprintf(output, "   -> MaxAP Bonus: %s", rathena::util::insert_comma(sd->bonus.ap).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> MaxAP Bonus: +%s", rathena::util::insert_comma(sd->bonus.ap).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		hp_sp_ap_found = true;    
	}    
	  
	if (sd->hprate != 100) {    
		int hp_rate_bonus = sd->hprate - 100;    
		if (hp_rate_bonus < 0) {    
			sprintf(output, "   -> MaxHP Rate: %s%%", rathena::util::insert_comma(hp_rate_bonus).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> MaxHP Rate: +%s%%", rathena::util::insert_comma(hp_rate_bonus).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		hp_sp_ap_found = true;    
	}    
	  
	if (sd->sprate != 100) {    
		int sp_rate_bonus = sd->sprate - 100;    
		if (sp_rate_bonus < 0) {    
			sprintf(output, "   -> MaxSP Rate: %s%%", rathena::util::insert_comma(sp_rate_bonus).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> MaxSP Rate: +%s%%", rathena::util::insert_comma(sp_rate_bonus).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		hp_sp_ap_found = true;    
	}    
	  
	if (sd->aprate != 100) {    
		int ap_rate_bonus = sd->aprate - 100;    
		if (ap_rate_bonus < 0) {    
			sprintf(output, "   -> MaxAP Rate: %s%%", rathena::util::insert_comma(ap_rate_bonus).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> MaxAP Rate: +%s%%", rathena::util::insert_comma(ap_rate_bonus).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		hp_sp_ap_found = true;    
	} 

    if (!hp_sp_ap_found) {  
        clif_displaymessage(fd, "   -> No HP/SP/AP bonuses found");  
    }

	clif_displaymessage(fd, " * Atk/Def");    
	bool atk_def_found = false;

	if (sd->bonus.eatk != 0) {    
		if (sd->bonus.eatk < 0) {    
			sprintf(output, "   -> Equipment ATK: %s", rathena::util::insert_comma(sd->bonus.eatk).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> Equipment ATK: +%s", rathena::util::insert_comma(sd->bonus.eatk).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		atk_def_found = true;    
	}    
	  
	if (sd->bonus.ematk != 0) {    
		if (sd->bonus.ematk < 0) {    
			sprintf(output, "   -> Equipment MATK: %s", rathena::util::insert_comma(sd->bonus.ematk).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> Equipment MATK: +%s", rathena::util::insert_comma(sd->bonus.ematk).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		atk_def_found = true;    
	}    
	  
	if (sd->bonus.ematk_hidden != 0) {    
		if (sd->bonus.ematk_hidden < 0) {    
			sprintf(output, "   -> Hidden MATK: %s", rathena::util::insert_comma(sd->bonus.ematk_hidden).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> Hidden MATK: +%s", rathena::util::insert_comma(sd->bonus.ematk_hidden).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		atk_def_found = true;    
	}    
	  
	if (sd->bonus.atk_rate != 0) {    
		if (sd->bonus.atk_rate < 0) {    
			sprintf(output, "   -> ATK Rate: %s%%", rathena::util::insert_comma(sd->bonus.atk_rate).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> ATK Rate: +%s%%", rathena::util::insert_comma(sd->bonus.atk_rate).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		atk_def_found = true;    
	}    

	if (sd->matk_rate != 100) {      
		int matk_rate_bonus = sd->matk_rate - 100;      
		if (matk_rate_bonus < 0) {      
			sprintf(output, "   -> MATK Rate: %s%%", rathena::util::insert_comma(matk_rate_bonus).c_str());      
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);      
		} else {      
			sprintf(output, "   -> MATK Rate: +%s%%", rathena::util::insert_comma(matk_rate_bonus).c_str());      
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
		}      
		atk_def_found = true;      
	}
	  
	if (sd->bonus.weapon_atk_rate != 0) {    
		if (sd->bonus.weapon_atk_rate < 0) {    
			sprintf(output, "   -> Weapon ATK Rate: %s%%", rathena::util::insert_comma(sd->bonus.weapon_atk_rate).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> Weapon ATK Rate: +%s%%", rathena::util::insert_comma(sd->bonus.weapon_atk_rate).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		atk_def_found = true;    
	}    
	  
	if (sd->bonus.weapon_matk_rate != 0) {    
		if (sd->bonus.weapon_matk_rate < 0) {    
			sprintf(output, "   -> Weapon MATK Rate: %s%%", rathena::util::insert_comma(sd->bonus.weapon_matk_rate).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> Weapon MATK Rate: +%s%%", rathena::util::insert_comma(sd->bonus.weapon_matk_rate).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}    
		atk_def_found = true;    
	}

    if (!atk_def_found) {  
        clif_displaymessage(fd, "   -> No Atk/Def bonuses found");  
    }

	clif_displaymessage(fd, " * Additional Stats");  
	bool additional_stats_found = false;
 
	if (sd->hit_rate != 100) {  
		int hit_rate_bonus = sd->hit_rate - 100;  
		if (hit_rate_bonus < 0) {  
			sprintf(output, "   -> Hit Rate: %s%%", rathena::util::insert_comma(hit_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Hit Rate: +%s%%", rathena::util::insert_comma(hit_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  

	if (sd->critical_rate != 100) {  
		int critical_rate_bonus = sd->critical_rate - 100;  
		if (critical_rate_bonus < 0) {  
			sprintf(output, "   -> Critical Rate: %s%%", rathena::util::insert_comma(critical_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Critical Rate: +%s%%", rathena::util::insert_comma(critical_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  
	  
	if (sd->bonus.critical_rangeatk != 0) {  
		int critical_long = sd->bonus.critical_rangeatk / 10;  
		if (critical_long < 0) {  
			sprintf(output, "   -> Critical Long Range: %s", rathena::util::insert_comma(critical_long).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Critical Long Range: +%s", rathena::util::insert_comma(critical_long).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  

	if (sd->flee_rate != 100) {  
		int flee_rate_bonus = sd->flee_rate - 100;  
		if (flee_rate_bonus < 0) {  
			sprintf(output, "   -> Flee Rate: %s%%", rathena::util::insert_comma(flee_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Flee Rate: +%s%%", rathena::util::insert_comma(flee_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  
	  
	if (sd->flee2_rate != 100) {  
		int flee2_rate_bonus = sd->flee2_rate - 100;  
		if (flee2_rate_bonus < 0) {  
			sprintf(output, "   -> Perfect Dodge Rate: %s%%", rathena::util::insert_comma(flee2_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Perfect Dodge Rate: +%s%%", rathena::util::insert_comma(flee2_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  

	if (sd->bonus.perfect_hit != 0) {  
		if (sd->bonus.perfect_hit < 0) {  
			sprintf(output, "   -> Perfect Hit Rate: %s%%", rathena::util::insert_comma(sd->bonus.perfect_hit).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Perfect Hit Rate: +%s%%", rathena::util::insert_comma(sd->bonus.perfect_hit).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  
	  
	if (sd->bonus.perfect_hit_add != 0) {  
		if (sd->bonus.perfect_hit_add < 0) {  
			sprintf(output, "   -> Perfect Hit Add Rate: %s%%", rathena::util::insert_comma(sd->bonus.perfect_hit_add).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Perfect Hit Add Rate: +%s%%", rathena::util::insert_comma(sd->bonus.perfect_hit_add).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  

	if (sd->bonus.speed_rate != 0) {  
		if (sd->bonus.speed_rate < 0) {  
			sprintf(output, "   -> Speed Rate: %s%%", rathena::util::insert_comma(sd->bonus.speed_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Speed Rate: +%s%%", rathena::util::insert_comma(sd->bonus.speed_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  
	  
	if (sd->bonus.speed_add_rate != 0) {  
		if (sd->bonus.speed_add_rate < 0) {  
			sprintf(output, "   -> Speed Add Rate: %s%%", rathena::util::insert_comma(sd->bonus.speed_add_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Speed Add Rate: +%s%%", rathena::util::insert_comma(sd->bonus.speed_add_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  

	if (sd->bonus.aspd_add != 0) {  
		if (sd->bonus.aspd_add < 0) {  
			sprintf(output, "   -> ASPD Add: %s", rathena::util::insert_comma(sd->bonus.aspd_add).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> ASPD Add: +%s", rathena::util::insert_comma(sd->bonus.aspd_add).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  

	if (sd->add_max_weight != 0) {  
		if (sd->add_max_weight < 0) {  
			sprintf(output, "   -> Max Weight: %s", rathena::util::insert_comma(sd->add_max_weight).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Max Weight: +%s", rathena::util::insert_comma(sd->add_max_weight).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  

	if (sd->patk_rate != 100) {  
		int patk_rate_bonus = sd->patk_rate - 100;  
		if (patk_rate_bonus < 0) {  
			sprintf(output, "   -> PAtk Rate: %s%%", rathena::util::insert_comma(patk_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> PAtk Rate: +%s%%", rathena::util::insert_comma(patk_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  
	  
	if (sd->smatk_rate != 100) {  
		int smatk_rate_bonus = sd->smatk_rate - 100;  
		if (smatk_rate_bonus < 0) {  
			sprintf(output, "   -> SMatk Rate: %s%%", rathena::util::insert_comma(smatk_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> SMatk Rate: +%s%%", rathena::util::insert_comma(smatk_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  
	  
	if (sd->res_rate != 100) {  
		int res_rate_bonus = sd->res_rate - 100;  
		if (res_rate_bonus < 0) {  
			sprintf(output, "   -> Res Rate: %s%%", rathena::util::insert_comma(res_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Res Rate: +%s%%", rathena::util::insert_comma(res_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  
	  
	if (sd->mres_rate != 100) {  
		int mres_rate_bonus = sd->mres_rate - 100;  
		if (mres_rate_bonus < 0) {  
			sprintf(output, "   -> MRes Rate: %s%%", rathena::util::insert_comma(mres_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> MRes Rate: +%s%%", rathena::util::insert_comma(mres_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  
	  
	if (sd->hplus_rate != 100) {  
		int hplus_rate_bonus = sd->hplus_rate - 100;  
		if (hplus_rate_bonus < 0) {  
			sprintf(output, "   -> HPlus Rate: %s%%", rathena::util::insert_comma(hplus_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> HPlus Rate: +%s%%", rathena::util::insert_comma(hplus_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}  
	  
	if (sd->crate_rate != 100) {  
		int crate_rate_bonus = sd->crate_rate - 100;  
		if (crate_rate_bonus < 0) {  
			sprintf(output, "   -> CRate Rate: %s%%", rathena::util::insert_comma(crate_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> CRate Rate: +%s%%", rathena::util::insert_comma(crate_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}

    if (!additional_stats_found) {  
        clif_displaymessage(fd, "   -> No Additional Stats bonuses found"); 
    }

	clif_displaymessage(fd, "EXTENDED BONUSES");  
	clif_displaymessage(fd, " * HP/SP Recovery");  
	bool hp_sp_found = false;  

	if (sd->hprecov_rate != 100) {  
		int hp_recov_bonus = sd->hprecov_rate - 100;  
		if (hp_recov_bonus < 0) {  
			sprintf(output, "   -> HP Recovery Rate: %s%%", rathena::util::insert_comma(hp_recov_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> HP Recovery Rate: +%s%%", rathena::util::insert_comma(hp_recov_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		hp_sp_found = true;
	}  
	  
	if (sd->sprecov_rate != 100) {  
		int sp_recov_bonus = sd->sprecov_rate - 100;  
		if (sp_recov_bonus < 0) {  
			sprintf(output, "   -> SP Recovery Rate: %s%%", rathena::util::insert_comma(sp_recov_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> SP Recovery Rate: +%s%%", rathena::util::insert_comma(sp_recov_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		hp_sp_found = true;
	}  
 
	if (sd->hp_regen.value != 0) {  
		if (sd->hp_regen.value < 0) {  
			sprintf(output, "   -> HP Regen: %s every %s ms",   
					rathena::util::insert_comma(sd->hp_regen.value).c_str(),  
					rathena::util::insert_comma(sd->hp_regen.rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> HP Regen: +%s every %s ms",   
					rathena::util::insert_comma(sd->hp_regen.value).c_str(),  
					rathena::util::insert_comma(sd->hp_regen.rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		hp_sp_found = true;
	}  
	  
	if (sd->sp_regen.value != 0) {  
		if (sd->sp_regen.value < 0) {  
			sprintf(output, "   -> SP Regen: %s every %s ms",   
					rathena::util::insert_comma(sd->sp_regen.value).c_str(),  
					rathena::util::insert_comma(sd->sp_regen.rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> SP Regen: +%s every %s ms",   
					rathena::util::insert_comma(sd->sp_regen.value).c_str(),  
					rathena::util::insert_comma(sd->sp_regen.rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		hp_sp_found = true;
	}  

	if (sd->hp_loss.value != 0) {  
		sprintf(output, "   -> HP Loss: %s every %s ms",   
				rathena::util::insert_comma(sd->hp_loss.value).c_str(),  
				rathena::util::insert_comma(sd->hp_loss.rate).c_str());  
		clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		hp_sp_found = true;
	}  
	  
	if (sd->sp_loss.value != 0) {  
		sprintf(output, "   -> SP Loss: %s every %s ms",   
				rathena::util::insert_comma(sd->sp_loss.value).c_str(),  
				rathena::util::insert_comma(sd->sp_loss.rate).c_str());  
		clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		hp_sp_found = true;
	}  

	if (sd->percent_hp_regen.value != 0) {  
		if (sd->percent_hp_regen.value < 0) {  
			sprintf(output, "   -> HP Percent Regen: %s%% every %s ms",   
					rathena::util::insert_comma(sd->percent_hp_regen.value).c_str(),  
					rathena::util::insert_comma(sd->percent_hp_regen.rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> HP Percent Regen: +%s%% every %s ms",   
					rathena::util::insert_comma(sd->percent_hp_regen.value).c_str(),  
					rathena::util::insert_comma(sd->percent_hp_regen.rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		hp_sp_found = true;
	}  
	  
	if (sd->percent_sp_regen.value != 0) {  
		if (sd->percent_sp_regen.value < 0) {  
			sprintf(output, "   -> SP Percent Regen: %s%% every %s ms",   
					rathena::util::insert_comma(sd->percent_sp_regen.value).c_str(),  
					rathena::util::insert_comma(sd->percent_sp_regen.rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> SP Percent Regen: +%s%% every %s ms",   
					rathena::util::insert_comma(sd->percent_sp_regen.value).c_str(),  
					rathena::util::insert_comma(sd->percent_sp_regen.rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		hp_sp_found = true;
	}  

	if (sd->regen.state.block != 0) {  
		if (sd->regen.state.block & 1) {  
			sprintf(output, "   -> HP Regeneration: Blocked");  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			hp_sp_found = true;
		}  
		if (sd->regen.state.block & 2) {  
			sprintf(output, "   -> SP Regeneration: Blocked");  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			hp_sp_found = true;
		}  
	}  
	  
	clif_displaymessage(fd, " * SP Consumption");  
	bool sp_consumption_found = false;  

	if (sd->dsprate != 100) {  
		int sp_consumption_bonus = sd->dsprate - 100;  
		if (sp_consumption_bonus < 0) {  
			sprintf(output, "   -> SP Consumption Rate: %s%%", rathena::util::insert_comma(sp_consumption_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		} else {  
			sprintf(output, "   -> SP Consumption Rate: +%s%%", rathena::util::insert_comma(sp_consumption_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		}  
		sp_consumption_found = true;
	}  

	for (const auto &bonus : sd->skillusesp) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s SP Cost: %s", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s SP Cost: +%s", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			}  
			sp_consumption_found = true;
		}  
	}  
	  
	for (const auto &bonus : sd->skillusesprate) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s SP Cost Rate: %s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s SP Cost Rate: +%s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			}  
			sp_consumption_found = true;
		}  
	}  
	  
	if (!hp_sp_found) {  
		clif_displaymessage(fd, "   -> No HP/SP recovery bonuses found");  
	}  
	  
	if (!sp_consumption_found) {  
		clif_displaymessage(fd, "   -> No SP consumption bonuses found");  
	}

	clif_displaymessage(fd, " * Extended Atk/Def");  
	bool extended_atkdef_found = false;  

	for (const auto &bonus : sd->skillatk) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Damage: %s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Damage: +%s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			extended_atkdef_found = true;  
		}  
	}  
  
	if (sd->bonus.skill_ratio != 0) {  
		if (sd->bonus.skill_ratio < 0) {  
			sprintf(output, "   -> Skill Ratio: %s", rathena::util::insert_comma(sd->bonus.skill_ratio).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Skill Ratio: +%s", rathena::util::insert_comma(sd->bonus.skill_ratio).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
  
	if (sd->bonus.short_attack_atk_rate != 0) {  
		if (sd->bonus.short_attack_atk_rate < 0) {  
			sprintf(output, "   -> Short Range Attack: %s%%", rathena::util::insert_comma(sd->bonus.short_attack_atk_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Short Range Attack: +%s%%", rathena::util::insert_comma(sd->bonus.short_attack_atk_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
	  
	if (sd->bonus.long_attack_atk_rate != 0) {  
		if (sd->bonus.long_attack_atk_rate < 0) {  
			sprintf(output, "   -> Long Range Attack: %s%%", rathena::util::insert_comma(sd->bonus.long_attack_atk_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Long Range Attack: +%s%%", rathena::util::insert_comma(sd->bonus.long_attack_atk_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
  
	if (sd->bonus.crit_atk_rate != 0) {  
		if (sd->bonus.crit_atk_rate < 0) {  
			sprintf(output, "   -> Critical Attack Rate: %s%%", rathena::util::insert_comma(sd->bonus.crit_atk_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Critical Attack Rate: +%s%%", rathena::util::insert_comma(sd->bonus.crit_atk_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
	  
	if (sd->bonus.crit_def_rate != 0) {  
		if (sd->bonus.crit_def_rate < 0) {  
			sprintf(output, "   -> Critical Defense Rate: %s%%", rathena::util::insert_comma(sd->bonus.crit_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Critical Defense Rate: +%s%%", rathena::util::insert_comma(sd->bonus.crit_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
	  
	if (sd->bonus.critical_def != 0) {  
		if (sd->bonus.critical_def < 0) {  
			sprintf(output, "   -> Critical Defense: %s", rathena::util::insert_comma(sd->bonus.critical_def).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Critical Defense: +%s", rathena::util::insert_comma(sd->bonus.critical_def).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
  
	for (int i = 0; i < 16; i++) {  
		if (sd->indexed_bonus.weapon_atk[i] != 0) {  
			if (sd->indexed_bonus.weapon_atk[i] < 0) {  
				sprintf(output, "   -> Weapon Type %d ATK: %s", i, rathena::util::insert_comma(sd->indexed_bonus.weapon_atk[i]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Weapon Type %d ATK: +%s", i, rathena::util::insert_comma(sd->indexed_bonus.weapon_atk[i]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			extended_atkdef_found = true;  
		}  
		  
		if (sd->indexed_bonus.weapon_damage_rate[i] != 0) {  
			if (sd->indexed_bonus.weapon_damage_rate[i] < 0) {  
				sprintf(output, "   -> Weapon Type %d Damage Rate: %s%%", i, rathena::util::insert_comma(sd->indexed_bonus.weapon_damage_rate[i]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Weapon Type %d Damage Rate: +%s%%", i, rathena::util::insert_comma(sd->indexed_bonus.weapon_damage_rate[i]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			extended_atkdef_found = true;  
		}  
	}  
  
	if (sd->bonus.near_attack_def_rate != 0) {  
		if (sd->bonus.near_attack_def_rate < 0) {  
			sprintf(output, "   -> Near Attack Defense: %s%%", rathena::util::insert_comma(sd->bonus.near_attack_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Near Attack Defense: +%s%%", rathena::util::insert_comma(sd->bonus.near_attack_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
	  
	if (sd->bonus.long_attack_def_rate != 0) {  
		if (sd->bonus.long_attack_def_rate < 0) {  
			sprintf(output, "   -> Long Attack Defense: %s%%", rathena::util::insert_comma(sd->bonus.long_attack_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Long Attack Defense: +%s%%", rathena::util::insert_comma(sd->bonus.long_attack_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
	  
	if (sd->bonus.magic_def_rate != 0) {  
		if (sd->bonus.magic_def_rate < 0) {  
			sprintf(output, "   -> Magic Attack Defense: %s%%", rathena::util::insert_comma(sd->bonus.magic_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Magic Attack Defense: +%s%%", rathena::util::insert_comma(sd->bonus.magic_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
	  
	if (sd->bonus.misc_def_rate != 0) {  
		if (sd->bonus.misc_def_rate < 0) {  
			sprintf(output, "   -> Misc Attack Defense: %s%%", rathena::util::insert_comma(sd->bonus.misc_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Misc Attack Defense: +%s%%", rathena::util::insert_comma(sd->bonus.misc_def_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		extended_atkdef_found = true;  
	}  
	  
	if (!extended_atkdef_found) {  
		clif_displaymessage(fd, "   -> No extended attack/defense bonuses found");  
	}

	clif_displaymessage(fd, " * Healing");  
	bool healing_found = false;  
  
	if (sd->bonus.add_heal_rate != 0) {  
		if (sd->bonus.add_heal_rate < 0) {  
			sprintf(output, "   -> Heal Power: %s%%", rathena::util::insert_comma(sd->bonus.add_heal_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Heal Power: +%s%%", rathena::util::insert_comma(sd->bonus.add_heal_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		healing_found = true;  
	}  
	  
	if (sd->bonus.add_heal2_rate != 0) {  
		if (sd->bonus.add_heal2_rate < 0) {  
			sprintf(output, "   -> Heal Power (Received): %s%%", rathena::util::insert_comma(sd->bonus.add_heal2_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Heal Power (Received): +%s%%", rathena::util::insert_comma(sd->bonus.add_heal2_rate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		healing_found = true;  
	}  
  
	for (const auto &bonus : sd->skillheal) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Heal: %s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Heal: +%s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			healing_found = true;  
		}  
	}  
  
	for (const auto &bonus : sd->skillheal2) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Heal (Received): %s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Heal (Received): +%s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			healing_found = true;  
		}  
	}  
  
	if (sd->bonus.itemhealrate2 != 0) {  
		if (sd->bonus.itemhealrate2 < 0) {  
			sprintf(output, "   -> Item HP Heal Rate: %s%%", rathena::util::insert_comma(sd->bonus.itemhealrate2).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Item HP Heal Rate: +%s%%", rathena::util::insert_comma(sd->bonus.itemhealrate2).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		healing_found = true;  
	}  
	  
	if (sd->bonus.itemsphealrate2 != 0) {  
		if (sd->bonus.itemsphealrate2 < 0) {  
			sprintf(output, "   -> Item SP Heal Rate: %s%%", rathena::util::insert_comma(sd->bonus.itemsphealrate2).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Item SP Heal Rate: +%s%%", rathena::util::insert_comma(sd->bonus.itemsphealrate2).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		healing_found = true;  
	}  
  
	for (const auto &bonus : sd->itemhealrate) {  
		if (bonus.val != 0) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> Item %d HP Heal: %s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Item %d HP Heal: +%s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			healing_found = true;  
		}  
	}  
  
	for (const auto &bonus : sd->itemsphealrate) {  
		if (bonus.val != 0) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> Item %d SP Heal: %s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Item %d SP Heal: +%s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			healing_found = true;  
		}  
	}  
  
	for (const auto &bonus : sd->itemgrouphealrate) {  
		if (bonus.val != 0) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> Item Group %d HP Heal: %s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Item Group %d HP Heal: +%s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			healing_found = true;  
		}  
	}  
  
	for (const auto &bonus : sd->itemgroupsphealrate) {  
		if (bonus.val != 0) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> Item Group %d SP Heal: %s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Item Group %d SP Heal: +%s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			healing_found = true;  
		}  
	}  
	  
	if (!healing_found) {  
		clif_displaymessage(fd, "   -> No healing bonuses found");  
	}

	clif_displaymessage(fd, " * Cast Time/Delay");  
	bool casting_found = false;  
	  
	if (sd->castrate != 100) {  
		int cast_rate_bonus = sd->castrate - 100;  
		if (cast_rate_bonus < 0) {  
			sprintf(output, "   -> Cast Rate: %s%%", rathena::util::insert_comma(cast_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Cast Rate: +%s%%", rathena::util::insert_comma(cast_rate_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		}  
		casting_found = true;  
	} 
	  
#ifdef RENEWAL_CAST  
	if (sd->bonus.fixcastrate != 0) {  
		if (sd->bonus.fixcastrate < 0) {  
			sprintf(output, "   -> Fixed Cast Rate: %s%%", rathena::util::insert_comma(sd->bonus.fixcastrate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Fixed Cast Rate: +%s%%", rathena::util::insert_comma(sd->bonus.fixcastrate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		}  
		casting_found = true;  
	}  
  
	if (sd->bonus.varcastrate != 0) {  
		if (sd->bonus.varcastrate < 0) {  
			sprintf(output, "   -> Variable Cast Rate: %s%%", rathena::util::insert_comma(sd->bonus.varcastrate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Variable Cast Rate: +%s%%", rathena::util::insert_comma(sd->bonus.varcastrate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		}  
		casting_found = true;  
	}  
  
	if (sd->bonus.add_fixcast != 0) {  
		if (sd->bonus.add_fixcast < 0) {  
			sprintf(output, "   -> Fixed Cast Time: %s ms", rathena::util::insert_comma(sd->bonus.add_fixcast).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Fixed Cast Time: +%s ms", rathena::util::insert_comma(sd->bonus.add_fixcast).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		}  
		casting_found = true;  
	}  
#endif  

	if (sd->bonus.add_varcast != 0) {  
		if (sd->bonus.add_varcast < 0) {  
			sprintf(output, "   -> Variable Cast Time: %s ms", rathena::util::insert_comma(sd->bonus.add_varcast).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Variable Cast Time: +%s ms", rathena::util::insert_comma(sd->bonus.add_varcast).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		}  
		casting_found = true;  
	}  

	for (const auto &bonus : sd->skillcastrate) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Cast Rate: %s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Cast Rate: +%s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			}  
			casting_found = true;  
		}  
	}  
	  
#ifdef RENEWAL_CAST
	for (const auto &bonus : sd->skillfixcastrate) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Fixed Cast Rate: %s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Fixed Cast Rate: +%s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			}  
			casting_found = true;  
		}  
	}  
  
	for (const auto &bonus : sd->skillfixcast) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Fixed Cast Time: %s ms", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Fixed Cast Time: +%s ms", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			}  
			casting_found = true;  
		}  
	}  
#endif  

	for (const auto &bonus : sd->skillvarcast) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Variable Cast Time: %s ms", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Variable Cast Time: +%s ms", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			}  
			casting_found = true;  
		}  
	}  
 
	if (sd->special_state.no_castcancel) {  
		sprintf(output, "   -> Cast Cancel Protection: Enabled (Non-GvG)");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		casting_found = true;  
	}  
	  
	if (sd->special_state.no_castcancel2) {  
		sprintf(output, "   -> Cast Cancel Protection: Enabled (All Areas)");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		casting_found = true;  
	}  
	  
	if (sd->bonus.delayrate != 0) {  
		if (sd->bonus.delayrate < 0) {  
			sprintf(output, "   -> Skill Delay Rate: %s%%", rathena::util::insert_comma(sd->bonus.delayrate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Skill Delay Rate: +%s%%", rathena::util::insert_comma(sd->bonus.delayrate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		}  
		casting_found = true;  
	}

	for (const auto &bonus : sd->skilldelay) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Delay: %s ms", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Delay: +%s ms", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			}  
			casting_found = true;  
		}  
	}  

	for (const auto &bonus : sd->skillcooldown) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Cooldown: %s ms", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Cooldown: +%s ms", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			}  
			casting_found = true;  
		}  
	}  
	  
	if (!casting_found) {  
		clif_displaymessage(fd, "   -> No cast time/delay bonuses found");  
	}

	clif_displaymessage(fd, "GROUP-SPECIFIC BONUSES");  
	clif_displaymessage(fd, " * Damage Modifiers");  
	bool damage_modifiers_found = false;   

	for (int ele = 0; ele < ELE_MAX; ele++) {  
		if (sd->right_weapon.addele[ele] != 0) {  
			std::string ele_name = get_element_name(ele);  
			if (sd->right_weapon.addele[ele] < 0) {  
				sprintf(output, "   -> %s Physical Damage: %s%%", ele_name.c_str(), rathena::util::insert_comma(sd->right_weapon.addele[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Physical Damage: +%s%%", ele_name.c_str(), rathena::util::insert_comma(sd->right_weapon.addele[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (sd->indexed_bonus.magic_addele_script[ele] != 0) {  
			std::string ele_name = get_element_name(ele);  
			if (sd->indexed_bonus.magic_addele_script[ele] < 0) {  
				sprintf(output, "   -> %s Magic Damage: %s%%", ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_addele_script[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Magic Damage: +%s%%", ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_addele_script[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (sd->indexed_bonus.subele_script[ele] != 0) {  
			std::string ele_name = get_element_name(ele);  
			if (sd->indexed_bonus.subele_script[ele] < 0) {  
				sprintf(output, "   -> %s Damage Reduction: %s%%", ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.subele_script[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Damage Reduction: +%s%%", ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.subele_script[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}  

	for (int ele = 0; ele < ELE_MAX; ele++) {  
		if (sd->indexed_bonus.subdefele[ele] != 0) {  
			std::string ele_name = get_element_name(ele);  
			if (sd->indexed_bonus.subdefele[ele] < 0) {  
				sprintf(output, "   -> Physical Damage Reduction from %s Defense: %s%%",   
						ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.subdefele[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Physical Damage Reduction from %s Defense: +%s%%",   
						ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.subdefele[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}

		if (sd->indexed_bonus.magic_subdefele[ele] != 0) {  
			std::string ele_name = get_element_name(ele);  
			if (sd->indexed_bonus.magic_subdefele[ele] < 0) {  
				sprintf(output, "   -> Magic Damage Reduction from %s Defense: %s%%",   
						ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_subdefele[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Magic Damage Reduction from %s Defense: +%s%%",   
						ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_subdefele[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}		
	}

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->right_weapon.addrace[race] != 0) {  
			std::string race_name = get_race_name(race);  
			if (sd->right_weapon.addrace[race] < 0) {  
				sprintf(output, "   -> %s Physical Damage: %s%%", race_name.c_str(), rathena::util::insert_comma(sd->right_weapon.addrace[race]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Physical Damage: +%s%%", race_name.c_str(), rathena::util::insert_comma(sd->right_weapon.addrace[race]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (sd->indexed_bonus.magic_addrace[race] != 0) {  
			std::string race_name = get_race_name(race);  
			if (sd->indexed_bonus.magic_addrace[race] < 0) {  
				sprintf(output, "   -> %s Magic Damage: %s%%", race_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_addrace[race]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Magic Damage: +%s%%", race_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_addrace[race]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (sd->indexed_bonus.subrace[race] != 0) {  
			std::string race_name = get_race_name(race);  
			if (sd->indexed_bonus.subrace[race] < 0) {  
				sprintf(output, "   -> %s Damage Reduction: %s%%", race_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.subrace[race]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Damage Reduction: +%s%%", race_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.subrace[race]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}  

	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {
		if (class_type == CLASS_ALL) continue;
		
		int physical_bonus = sd->right_weapon.addclass[class_type] + sd->right_weapon.addclass[CLASS_ALL];  
		int magic_bonus = sd->indexed_bonus.magic_addclass[class_type] + sd->indexed_bonus.magic_addclass[CLASS_ALL];  
		int reduction_bonus = sd->indexed_bonus.subclass[class_type] + sd->indexed_bonus.subclass[CLASS_ALL];

		if (physical_bonus != 0) {  
			std::string class_name = get_class_name(class_type);
			if (physical_bonus < 0) {
				sprintf(output, "   -> %s Physical Damage: %s%%", class_name.c_str(), rathena::util::insert_comma(physical_bonus).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Physical Damage: +%s%%", class_name.c_str(), rathena::util::insert_comma(physical_bonus).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (magic_bonus < 0) {
			std::string class_name = get_class_name(class_type);
			if (magic_bonus != 0) {
				sprintf(output, "   -> %s Magic Damage: %s%%", class_name.c_str(), rathena::util::insert_comma(magic_bonus).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Magic Damage: +%s%%", class_name.c_str(), rathena::util::insert_comma(magic_bonus).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (reduction_bonus != 0) {
			std::string class_name = get_class_name(class_type);
			if (reduction_bonus < 0) {
				sprintf(output, "   -> %s Damage Reduction: %s%%", class_name.c_str(), rathena::util::insert_comma(reduction_bonus).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Damage Reduction: +%s%%", class_name.c_str(), rathena::util::insert_comma(reduction_bonus).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}  

	for (int size = 0; size < SZ_MAX; size++) {  
		if (sd->right_weapon.addsize[size] != 0) {  
			std::string size_name = get_size_name(size);
			if (sd->right_weapon.addsize[size] < 0) {  
				sprintf(output, "   -> %s Physical Damage: %s%%", size_name.c_str(), rathena::util::insert_comma(sd->right_weapon.addsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Physical Damage: +%s%%", size_name.c_str(), rathena::util::insert_comma(sd->right_weapon.addsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (sd->indexed_bonus.magic_addsize[size] != 0) {  
			std::string size_name = get_size_name(size);
			if (sd->indexed_bonus.magic_addsize[size] < 0) {  
				sprintf(output, "   -> %s Magic Damage: %s%%", size_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_addsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Magic Damage: +%s%%", size_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_addsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (sd->indexed_bonus.subsize[size] != 0) {  
			std::string size_name = get_size_name(size);
			if (sd->indexed_bonus.subsize[size] < 0) {  
				sprintf(output, "   -> %s Damage Reduction: %s%%", size_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.subsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Damage Reduction: +%s%%", size_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.subsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (sd->indexed_bonus.weapon_subsize[size] != 0) {  
			std::string size_name = get_size_name(size);
			if (sd->indexed_bonus.weapon_subsize[size] < 0) {  
				sprintf(output, "   -> %s Weapon Damage Reduction: %s%%", size_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.weapon_subsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Weapon Damage Reduction: +%s%%", size_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.weapon_subsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}

		if (sd->indexed_bonus.magic_subsize[size] != 0) {  
			std::string size_name = get_size_name(size);
			if (sd->indexed_bonus.magic_subsize[size] < 0) {  
				sprintf(output, "   -> %s Magic Damage Reduction: %s%%", size_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_subsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Magic Damage Reduction: +%s%%", size_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_subsize[size]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}    

	for (const auto &bonus : sd->right_weapon.add_dmg) {  
		if (bonus.val != 0) {
			const char* monster_name = mob_get_jname(bonus.id);  
			if (!monster_name) monster_name = "Unknown Monster";
			
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Physical Damage: %s%%", monster_name, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Physical Damage: +%s%%", monster_name, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}  

	for (const auto &bonus : sd->add_mdmg) {  
		if (bonus.val != 0) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Magic Damage: %s%%", monster_name, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Magic Damage: +%s%%", monster_name, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}  

	for (const auto &bonus : sd->add_def) {  
		if (bonus.val != 0) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> Monster %d Physical Defense: %s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Monster %d Physical Defense: +%s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}  

	for (const auto &bonus : sd->add_mdef) {  
		if (bonus.val != 0) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> Monster %d Magic Defense: %s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Monster %d Magic Defense: +%s%%", bonus.id, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}  

	for (int race2 = 0; race2 < RC2_MAX; race2++) {  
		if (sd->right_weapon.addrace2[race2] != 0) {  
			if (sd->right_weapon.addrace2[race2] < 0) {  
				sprintf(output, "   -> Race2 %d Physical Damage: %s%%", race2, rathena::util::insert_comma(sd->right_weapon.addrace2[race2]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Race2 %d Physical Damage: +%s%%", race2, rathena::util::insert_comma(sd->right_weapon.addrace2[race2]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (sd->indexed_bonus.magic_addrace2[race2] != 0) {  
			if (sd->indexed_bonus.magic_addrace2[race2] < 0) {  
				sprintf(output, "   -> Race2 %d Magic Damage: %s%%", race2, rathena::util::insert_comma(sd->indexed_bonus.magic_addrace2[race2]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Race2 %d Magic Damage: +%s%%", race2, rathena::util::insert_comma(sd->indexed_bonus.magic_addrace2[race2]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  

		if (sd->indexed_bonus.subrace2[race2] != 0) {  
			if (sd->indexed_bonus.subrace2[race2] < 0) {  
				sprintf(output, "   -> Race2 %d Damage Reduction: %s%%", race2, rathena::util::insert_comma(sd->indexed_bonus.subrace2[race2]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Race2 %d Damage Reduction: +%s%%", race2, rathena::util::insert_comma(sd->indexed_bonus.subrace2[race2]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}  

	for (const auto &bonus : sd->subskill) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			if (bonus.val < 0) {  
				sprintf(output, "   -> %s Damage Reduction: %s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Damage Reduction: +%s%%", skill_desc, rathena::util::insert_comma(bonus.val).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			damage_modifiers_found = true;  
		}  
	}  

	if (sd->bonus.absorb_dmg_maxhp != 0) {  
		sprintf(output, "   -> Absorb Damage MaxHP: %s%%", rathena::util::insert_comma(sd->bonus.absorb_dmg_maxhp).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_modifiers_found = true;  
	}  
	  
	if (sd->bonus.absorb_dmg_maxhp2 != 0) {  
		sprintf(output, "   -> Absorb Damage MaxHP2: %s%%", rathena::util::insert_comma(sd->bonus.absorb_dmg_maxhp2).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_modifiers_found = true;  
	}  

	if (sd->special_state.no_sizefix) {  
		sprintf(output, "   -> Size Fix: Ignored");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_modifiers_found = true;  
	}  
	  
	if (!damage_modifiers_found) {  
		clif_displaymessage(fd, "   -> No damage modifier bonuses found");  
	}

	clif_displaymessage(fd, " * Additional Atk/Def");  
	bool additional_atkdef_found = false;  

	if (sd->battle_status.rhw.ele != ELE_NEUTRAL) {  
		std::string ele_name = get_element_name(sd->battle_status.rhw.ele);  
		sprintf(output, "   -> Attack Element: %s", ele_name.c_str());    
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		additional_atkdef_found = true;  
	}  

	if (sd->battle_status.def_ele != ELE_NEUTRAL) {  
		std::string ele_name = get_element_name(sd->battle_status.def_ele);  
		sprintf(output, "   -> Defense Element: %s", ele_name.c_str());    
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		additional_atkdef_found = true;  
	}  

	for (int ele = 0; ele < ELE_MAX; ele++) {  
		if (sd->indexed_bonus.magic_atk_ele[ele] != 0) {  
			std::string ele_name = get_element_name(ele);  
			if (sd->indexed_bonus.magic_atk_ele[ele] < 0) {  
				sprintf(output, "   -> %s Magic Attack: %s%%", ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_atk_ele[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> %s Magic Attack: +%s%%", ele_name.c_str(), rathena::util::insert_comma(sd->indexed_bonus.magic_atk_ele[ele]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			additional_atkdef_found = true;  
		}  
	}  

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->right_weapon.def_ratio_atk_race & (1 << race)) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Defense Ratio Attack vs %s: Enabled", race_name.c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			additional_atkdef_found = true;  
		}  
	}  

	for (int ele = 0; ele < ELE_MAX; ele++) {  
		if (sd->right_weapon.def_ratio_atk_ele & (1 << ele)) {  
			std::string ele_name = get_element_name(ele);  
			sprintf(output, "   -> Defense Ratio Attack vs %s: Enabled", ele_name.c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			additional_atkdef_found = true;  
		}  
	}  

	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {
		if (class_type == CLASS_ALL) continue;

		bool has_specific = sd->right_weapon.def_ratio_atk_class & (1 << class_type);  
		bool has_all = sd->right_weapon.def_ratio_atk_class & (1 << CLASS_ALL);

		if (has_specific || has_all) {  
			std::string class_name = get_class_name(class_type);
			sprintf(output, "   -> Defense Ratio Attack vs %s: Enabled", class_name.c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			additional_atkdef_found = true;  
		}  
	}  

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->def_set_race[race].rate != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Set DEF vs %s: %s%% chance, %s ms, DEF=%s",race_name.c_str(),rathena::util::insert_comma(sd->def_set_race[race].rate).c_str(),rathena::util::insert_comma(sd->def_set_race[race].tick).c_str(),rathena::util::insert_comma(sd->def_set_race[race].value).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			additional_atkdef_found = true;  
		}  
	}  

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->mdef_set_race[race].rate != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Set MDEF vs %s: %s%% chance, %s ms, MDEF=%s",race_name.c_str(),rathena::util::insert_comma(sd->mdef_set_race[race].rate).c_str(),rathena::util::insert_comma(sd->mdef_set_race[race].tick).c_str(),rathena::util::insert_comma(sd->mdef_set_race[race].value).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			additional_atkdef_found = true;  
		}  
	}  
	  
	if (!additional_atkdef_found) {  
		clif_displaymessage(fd, "   -> No additional attack/defense bonuses found");  
	}

	clif_displaymessage(fd, " * Ignore Def");  
	bool ignore_def_found = false;

	for (int ele = 0; ele < ELE_MAX; ele++) {  
		if (sd->right_weapon.ignore_def_ele & (1 << ele)) {  
			std::string ele_name = get_element_name(ele);  
			sprintf(output, "   -> Ignore DEF vs %s: Enabled", ele_name.c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->right_weapon.ignore_def_race & (1 << race)) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Ignore DEF vs %s: Enabled", race_name.c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {
		if (class_type == CLASS_ALL) continue;
		
		bool has_specific = sd->right_weapon.ignore_def_class & (1 << class_type);  
		bool has_all = sd->right_weapon.ignore_def_class & (1 << CLASS_ALL);

		if (has_specific || has_all) {  
			std::string class_name = get_class_name(class_type);
			sprintf(output, "   -> Ignore DEF vs %s: Enabled", class_name.c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int ele = 0; ele < ELE_MAX; ele++) {  
		if (sd->bonus.ignore_mdef_ele & (1 << ele)) {  
			std::string ele_name = get_element_name(ele);  
			sprintf(output, "   -> Ignore MDEF vs %s: Enabled", ele_name.c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->bonus.ignore_mdef_race & (1 << race)) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Ignore MDEF vs %s: Enabled", race_name.c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {
		if (class_type == CLASS_ALL) continue;
		
		bool has_specific = sd->bonus.ignore_mdef_class & (1 << class_type);  
		bool has_all = sd->bonus.ignore_mdef_class & (1 << CLASS_ALL);

		if (has_specific || has_all) {  
			std::string class_name = get_class_name(class_type);
			sprintf(output, "   -> Ignore MDEF vs %s: Enabled", class_name.c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->indexed_bonus.ignore_def_by_race[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Ignore DEF vs %s: %s%%", race_name.c_str(),   
					rathena::util::insert_comma(sd->indexed_bonus.ignore_def_by_race[race]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->indexed_bonus.ignore_mdef_by_race[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Ignore MDEF vs %s: %s%%", race_name.c_str(),   
					rathena::util::insert_comma(sd->indexed_bonus.ignore_mdef_by_race[race]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int race2 = 0; race2 < RC2_MAX; race2++) {  
		if (sd->indexed_bonus.ignore_mdef_by_race2[race2] != 0) {  
			sprintf(output, "   -> Ignore MDEF vs Race2 %d: %s%%", race2,   
					rathena::util::insert_comma(sd->indexed_bonus.ignore_mdef_by_race2[race2]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {
		if (class_type == CLASS_ALL) continue;
		
		int total_bonus = sd->indexed_bonus.ignore_def_by_class[class_type] + sd->indexed_bonus.ignore_def_by_class[CLASS_ALL];

		if (total_bonus != 0) {
			std::string class_name = get_class_name(class_type);
			sprintf(output, "   -> Ignore DEF vs %s: %s%%", class_name.c_str(),rathena::util::insert_comma(total_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  

	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {
		if (class_type == CLASS_ALL) continue; 
		
		int total_bonus = sd->indexed_bonus.ignore_mdef_by_class[class_type] + sd->indexed_bonus.ignore_mdef_by_class[CLASS_ALL];
		
		if (total_bonus != 0) {
			std::string class_name = get_class_name(class_type);
			sprintf(output, "   -> Ignore MDEF vs %s: %s%%", class_name.c_str(),rathena::util::insert_comma(total_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_def_found = true;  
		}  
	}  
	  
	if (!ignore_def_found) {  
		clif_displaymessage(fd, "   -> No ignore defense bonuses found");  
	}

	clif_displaymessage(fd, " * Ignore Res");  
	bool ignore_res_found = false;    
 
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->indexed_bonus.ignore_res_by_race[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Ignore Res vs %s: %s%%", race_name.c_str(),rathena::util::insert_comma(sd->indexed_bonus.ignore_res_by_race[race]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_res_found = true;  
		}  
	}  

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->indexed_bonus.ignore_mres_by_race[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Ignore MRes vs %s: %s%%", race_name.c_str(),rathena::util::insert_comma(sd->indexed_bonus.ignore_mres_by_race[race]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			ignore_res_found = true;  
		}  
	}  
	  
	if (!ignore_res_found) {  
		clif_displaymessage(fd, "   -> No ignore resistance bonuses found");  
	}  
	  
	clif_displaymessage(fd, " * Experience");  
	bool experience_found = false;  

	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->indexed_bonus.expaddrace[race] != 0) {  
			std::string race_name = get_race_name(race);  
			if (sd->indexed_bonus.expaddrace[race] < 0) {  
				sprintf(output, "   -> EXP vs %s: %s%%", race_name.c_str(),rathena::util::insert_comma(sd->indexed_bonus.expaddrace[race]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> EXP vs %s: +%s%%", race_name.c_str(),rathena::util::insert_comma(sd->indexed_bonus.expaddrace[race]).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			experience_found = true;  
		}  
	}  

	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {
		if (class_type == CLASS_ALL) continue;
		
		int total_bonus = sd->indexed_bonus.expaddclass[class_type] + sd->indexed_bonus.expaddclass[CLASS_ALL];

		if (total_bonus != 0) {
			std::string class_name = get_class_name(class_type);
			if (sd->indexed_bonus.expaddclass[class_type] < 0) {  
				sprintf(output, "   -> EXP vs %s: %s%%", class_name.c_str(),rathena::util::insert_comma(total_bonus).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> EXP vs %s: +%s%%", class_name.c_str(),rathena::util::insert_comma(total_bonus).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			experience_found = true;  
		}  
	}  
	  
	if (!experience_found) {  
		clif_displaymessage(fd, "   -> No experience bonuses found");  
	}
}
  
/*==========================================  
* @statbonus  
*------------------------------------------*/  
ACMD_FUNC(statbonus) {  
    nullpo_retr(-1, sd);  
    atcommand_statbonus_sub(fd, sd);  
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

/*==========================================
* @ws - Open the searchstore UI with found sold items
*------------------------------------------*/
ACMD_FUNC(whosell)
{
	char item_name[100];
	int item_id = 0, j, sat_num = 0;
	int count = 1, i;
	int s_type = 1; // search bitmask: 0-name,1-id, 2-card, 4-refine, 8-options
	int refine = 0,card_id = 0, option_id = 0, option_value = 0;
	bool flag = 0; // place dot on the minimap?
	class map_session_data* pl_sd;
	struct s_mapiterator* iter;
	unsigned int MinPrice = battle_config.vending_max_value, MaxPrice = 0;
	time_t querytime;
	std::map<t_itemid, std::shared_ptr<item_data>> item_array = {};
	bool search_item_name = false;

	nullpo_retr(-1, sd);

	if (!message || !*message) {
		clif_displaymessage(fd, "Use: @whosell (<+refine> )(<item_id>)(<[card_id]>)((<option_id> <option_value>)) or @whosell <name>");
		return -1;
	}

	time(&querytime);

	if( sd->searchstore.nextquerytime > querytime ) {
		clif_search_store_info_failed(*sd, SSI_FAILED_LIMIT_SEARCH_TIME); // SSI_FAILED_LIMIT_SEARCH_TIME
		return -1;
	}

	sd->searchstore.uses--;
	sd->searchstore.type = SEARCHTYPE_VENDING; // vending
	sd->searchstore.nextquerytime = querytime+battle_config.searchstore_querydelay;

	searchstore_clear(*sd); // clear previous result

	if( !sd->searchstore.open )
		searchstore_open(*sd, 1, SEARCHSTORE_EFFECT_REMOTE, sd->m);

	if (sscanf(message, "+%d %d[%d](%d %d)", &refine, &item_id, &card_id, &option_id, &option_value) == 5){
		s_type = 1+2+4+8;
	} else if (sscanf(message, "+%d %d(%d %d)", &refine, &item_id, &option_id, &option_value) == 4){
		s_type = 1+4+8;
	} else if (sscanf(message, "+%d [%d](%d %d)", &refine, &card_id, &option_id, &option_value) == 4){
		s_type = 2+4+8;
	} else if (sscanf(message, "%d[%d](%d %d)", &item_id, &card_id, &option_id, &option_value) == 4){
		s_type = 1+2+8;
	} else if (sscanf(message, "+%d %d[%d]", &refine, &item_id, &card_id) == 3){
		s_type = 1+2+4;
	} else if (sscanf(message, "[%d](%d %d)", &card_id, &option_id, &option_value) == 3){
		s_type = 2+8;
	} else if (sscanf(message, "+%d (%d %d)", &refine, &option_id, &option_value) == 3){
		s_type = 4+8;
	} else if (sscanf(message, "%d(%d %d)", &item_id, &option_id, &option_value) == 3){
		s_type = 1+8;
	} else if (sscanf(message, "+%d %d", &refine, &item_id) == 2){
		s_type = 1+4;
	} else if (sscanf(message, "+%d [%d]", &refine, &card_id) == 2){
		s_type = 2+4;
	} else if (sscanf(message, "%d[%d]", &item_id, &card_id) == 2){
		s_type = 1+2;
	} else if (sscanf(message, "(%d %d)", &option_id, &option_value) == 2){
		s_type = 8;
	} else if (sscanf(message, "[%d]", &card_id) == 1){
		s_type = 2;
	} else if (sscanf(message, "+%d", &refine) == 1){
		s_type = 4;
	} else if (sscanf(message, "%d", &item_id) == 1 && item_id == atoi(message)){
		s_type = 1;
	} else if (sscanf(message, "%99[^\n]", item_name) == 1){
		s_type = 1;
		count = itemdb_searchname_array(item_array, MAX_SEARCH, item_name);

		if( !count ){
			clif_displaymessage(fd, "No item found with this name");
			return -1;
		}

		search_item_name = true;
	} else {
		clif_displaymessage(fd, "Use: @whosell (<+refine> )(<item_id>)(<[card_id]>)((<option_id> <option_value>)) or @whosell <name>");
		return -1;
	}
   
	for(i = 0; i < count; i++){
		std::shared_ptr<item_data> item_data_ptr = item_db.find(item_id);

		if(search_item_name){
			auto it = std::next(item_array.begin(), i);
			std::shared_ptr<item_data> item_data_ptr = it->second;
			item_id = item_data_ptr->nameid;
		}

		//check card
		if (s_type & 2) {
			std::shared_ptr<item_data> card_data = item_db.find(card_id);
			if (card_data == nullptr || card_data->type != IT_CARD) {
				clif_displaymessage(fd, "Not found a card with that ID");
				return -1;
			}
		}
		//check item
		if (s_type & 1 && !search_item_name) {
			std::shared_ptr<item_data> item_data_ptr = item_db.find(item_id);
			if (item_data_ptr == nullptr) {
				clif_displaymessage(fd, "Not found an item with that ID");
				return -1;
			}
		}
		//check refine
		if(s_type & 4){
			if (refine < 0 || refine > MAX_REFINE) {  
				sprintf(atcmd_output, "Refine out of bounds: 0 - %d", MAX_REFINE);  
				clif_displaymessage(fd, atcmd_output);  
				return -1;  
			}
		}
		iter = mapit_getallusers();

		for( pl_sd = (TBL_PC*)mapit_first(iter); mapit_exists(iter); pl_sd = (TBL_PC*)mapit_next(iter) )
		{
			if( pl_sd->state.vending ) //check if player is vending
			{
				for (j = 0; j < pl_sd->vend_num; j++) 
				{
					std::shared_ptr<item_data> current_item = nullptr;

					if (!search_item_name) {
						current_item = item_db.find(pl_sd->buyingstore.items[j].nameid);
						if (current_item == nullptr)
							continue;
					}
					if(s_type & 1 && pl_sd->cart.u.items_cart[pl_sd->vending[j].index].nameid != item_id)
						continue;
					if (s_type & 2 && ((current_item->type != IT_ARMOR && current_item->type != IT_WEAPON) ||
						(pl_sd->cart.u.items_cart[pl_sd->vending[j].index].card[0] != card_id &&
							pl_sd->cart.u.items_cart[pl_sd->vending[j].index].card[1] != card_id &&
							pl_sd->cart.u.items_cart[pl_sd->vending[j].index].card[2] != card_id &&
							pl_sd->cart.u.items_cart[pl_sd->vending[j].index].card[3] != card_id)))
						continue;
					if (s_type & 4 && ((current_item->type != IT_ARMOR && current_item->type != IT_WEAPON) ||
						pl_sd->cart.u.items_cart[pl_sd->vending[j].index].refine != refine))
						continue;
					if (s_type & 8 && ((current_item->type != IT_ARMOR && current_item->type != IT_WEAPON) ||
						((pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[0].id != option_id && pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[0].value != option_value)
							&& (pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[1].id != option_id && pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[1].value != option_value)
							&& (pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[2].id != option_id && pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[2].value != option_value)
							&& (pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[3].id != option_id && pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[3].value != option_value)
							&& (pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[4].id != option_id && pl_sd->cart.u.items_cart[pl_sd->vending[j].index].option[4].value != option_value))))
						continue;

					// Check if the result set is full
					if( sd->searchstore.items.size() >= (unsigned int)battle_config.searchstore_maxresults ){
						return false;
					}

					std::shared_ptr<s_search_store_info_item> ssitem = std::make_shared<s_search_store_info_item>();

					ssitem->store_id = pl_sd->vender_id;
					ssitem->account_id = pl_sd->status.account_id;
					safestrncpy( ssitem->store_name, pl_sd->message, sizeof( ssitem->store_name ) );
					ssitem->nameid = pl_sd->cart.u.items_cart[pl_sd->vending[j].index].nameid;
					ssitem->amount = pl_sd->vending[j].amount;
					ssitem->price = pl_sd->vending[j].value;
					for( int k = 0; k < MAX_SLOTS; k++ )
						ssitem->card[k] = pl_sd->cart.u.items_cart[pl_sd->vending[j].index].card[k];
					ssitem->refine = pl_sd->cart.u.items_cart[pl_sd->vending[j].index].refine;
					ssitem->enchantgrade = pl_sd->cart.u.items_cart[pl_sd->vending[j].index].enchantgrade;

					sd->searchstore.items.push_back( ssitem );

					flag = 1;
				}

				if (flag && pl_sd->mapindex == sd->mapindex) {
					clif_viewpoint(*sd, 1, 1, pl_sd->x, pl_sd->y, ++sat_num, 0xFFFFFF);
					flag = 0;
				}
			}
		}
		
		mapit_free(iter);
	}

	if(!sd->searchstore.items.empty() ) {
		if (count == MAX_SEARCH) {
			sprintf(atcmd_output, msg_txt(sd,269), MAX_SEARCH); // Displaying first %d matches
			clif_displaymessage(fd, atcmd_output);
		}

	   //sort by price
		sort(sd->searchstore.items.begin(), sd->searchstore.items.end(), [](std::shared_ptr<s_search_store_info_item> ss1, std::shared_ptr<s_search_store_info_item> ss2) {
			return (ss1->price < ss2->price);
		});

		// present results
		clif_search_store_info_ack(*sd);

		// one page displayed
		sd->searchstore.pages++;
	}  else {
		// cleanup
		searchstore_clear(*sd);

		// update uses
		clif_search_store_info_ack(*sd);

		// notify of failure
		clif_search_store_info_failed(*sd, SSI_FAILED_NOTHING_SEARCH_ITEM); // SSI_FAILED_NOTHING_SEARCH_ITEM
	}

	return 0;
}


/*==========================================
* @wb - Open the searchstore UI with found buying items
*------------------------------------------*/
ACMD_FUNC(whobuy)
{
	char item_name[100];
	int item_id = 0, j, sat_num = 0;
	int count = 1, i;
	int s_type = 1; // search bitmask: 0-name,1-id, 2-card, 4-refine  
	int refine = 0, card_id = 0;
	bool flag = 0; // place dot on the minimap?  
	class map_session_data* pl_sd;
	struct s_mapiterator* iter;
	unsigned int MinPrice = battle_config.vending_max_value, MaxPrice = 0;
	time_t querytime;
	std::map<t_itemid, std::shared_ptr<item_data>> item_array = {};
	bool search_item_name = false;

	nullpo_retr(-1, sd);

	if (!message || !*message) {
		clif_displaymessage(fd, "Use: @whobuy (<+refine> )(<item_id>) or @whobuy <name>");
		return -1;
	}

	time(&querytime);

	if( sd->searchstore.nextquerytime > querytime ) {
		clif_search_store_info_failed(*sd, SSI_FAILED_LIMIT_SEARCH_TIME); // SSI_FAILED_LIMIT_SEARCH_TIME
		return -1;
	}

	sd->searchstore.uses--;
	sd->searchstore.type = SEARCHTYPE_BUYING_STORE; // buying store
	sd->searchstore.nextquerytime = querytime+battle_config.searchstore_querydelay;

	searchstore_clear(*sd); // clear previous result

	if( !sd->searchstore.open )
		searchstore_open(*sd, 1, SEARCHSTORE_EFFECT_REMOTE, sd->m);

	if (sscanf(message, "+%d %d", &refine, &item_id) == 2){
		s_type = 1+2;
	} else if (sscanf(message, "+%d", &refine) == 1){
		s_type = 2;
	} else if (sscanf(message, "%d", &item_id) == 1 && item_id == atoi(message)){
		s_type = 1;
	} else if (sscanf(message, "%99[^\n]", item_name) == 1){
		s_type = 1;
		count = itemdb_searchname_array(item_array, MAX_SEARCH, item_name);

		if( !count ){
			clif_displaymessage(fd, "No item found with this name");
			return -1;
		}

		search_item_name = true;
	} else {
		clif_displaymessage(fd, "Use: @whobuy (<+refine> )(<item_id>) or @whobuy <name>");
		return -1;
	}
    
	std::shared_ptr<item_data> item_data_ptr = item_db.find(item_id);

	//check item
	if (s_type & 1 && !search_item_name) {
		std::shared_ptr<item_data> item_data_ptr = item_db.find(item_id);
		if (item_data_ptr == nullptr) {
			clif_displaymessage(fd, "Not found an item with that ID");
			return -1;
		}
	}
	//check refine
	if(s_type & 2){
		if (refine<0 || refine>10){
			clif_displaymessage(fd, "Refine out of bounds: 0 - 10");
			return -1;
		}
	}
	iter = mapit_getallusers();

	for(i = 0; i < count; i++){
		if(search_item_name){
			auto it = std::next(item_array.begin(), i);
			std::shared_ptr<item_data> item_data_ptr = it->second;
			item_id = item_data_ptr->nameid;
		}

		for( pl_sd = (TBL_PC*)mapit_first(iter); mapit_exists(iter); pl_sd = (TBL_PC*)mapit_next(iter) )
		{
			if( pl_sd->state.buyingstore ) //check if player is buying
			{
				for (j = 0; j < pl_sd->buyingstore.slots; j++) 
				{
					if (!search_item_name) {
						std::shared_ptr<item_data> current_item = item_db.find(pl_sd->buyingstore.items[j].nameid);
						if (current_item == nullptr)
							continue;

						// Now use current_item for type checks  
						if (s_type & 2 && ((current_item->type != IT_ARMOR && current_item->type != IT_WEAPON) ||
							pl_sd->cart.u.items_cart[pl_sd->vending[j].index].refine != refine))
							continue;
					}

					if (s_type & 1 && pl_sd->buyingstore.items[j].nameid != item_id)
						continue;

					// Check if the result set is full
					if( sd->searchstore.items.size() >= (unsigned int)battle_config.searchstore_maxresults ){
						return false;
					}

					std::shared_ptr<s_search_store_info_item> ssitem = std::make_shared<s_search_store_info_item>();

					ssitem->store_id = pl_sd->buyer_id;
					ssitem->account_id = pl_sd->status.account_id;
					safestrncpy( ssitem->store_name, pl_sd->message, sizeof( ssitem->store_name ) );
					ssitem->nameid = pl_sd->buyingstore.items[j].nameid;
					ssitem->amount = pl_sd->buyingstore.items[j].amount;
					ssitem->price = pl_sd->buyingstore.items[j].price;
					for( int k = 0; k < MAX_SLOTS; k++ )
						ssitem->card[k] = 0;
					ssitem->refine = 0;
					ssitem->enchantgrade = 0;

					sd->searchstore.items.push_back( ssitem );

					flag = 1;
				}

				if (flag && pl_sd->mapindex == sd->mapindex) {
					clif_viewpoint(*sd, 1, 1, pl_sd->x, pl_sd->y, ++sat_num, 0xFFFFFF);
					flag = 0;
				}
			}
		}
	}
	mapit_free(iter);

	if(!sd->searchstore.items.empty() ) {
		if (count == MAX_SEARCH) {
			sprintf(atcmd_output, msg_txt(sd,269), MAX_SEARCH); // Displaying first %d matches
			clif_displaymessage(fd, atcmd_output);
		}

	   //sort by price
		sort(sd->searchstore.items.begin(), sd->searchstore.items.end(), [](std::shared_ptr<s_search_store_info_item> ss1, std::shared_ptr<s_search_store_info_item> ss2) {
			return (ss1->price > ss2->price);
		});

		// present results
		clif_search_store_info_ack(*sd);

		// one page displayed
		sd->searchstore.pages++;
	}  else {
		// cleanup
		searchstore_clear(*sd);

		// update uses
		clif_search_store_info_ack(*sd);

		// notify of failure
		clif_search_store_info_failed(*sd, SSI_FAILED_NOTHING_SEARCH_ITEM); // SSI_FAILED_NOTHING_SEARCH_ITEM
	}

	return 0;
}
