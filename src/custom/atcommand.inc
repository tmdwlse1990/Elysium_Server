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
* @basicbonuses    
* shows basic stat bonuses of players    
*------------------------------------------*/      
void atcommand_basicbonuses_sub(int fd, class map_session_data *sd) {
	status_data *status = status_get_status_data(*sd);	
    char output[CHAT_SIZE_MAX];  

	clif_displaymessage(fd, "=============================");
    clif_displaymessage(fd, "BASIC BONUSES");  
    clif_displaymessage(fd, "=============================");  
      
	// Base Stats (bStr, bAgi, bVit, bInt, bDex & bLuk) 
    clif_displaymessage(fd, " * Base Stats");    
    bool base_stats_found = false;  
  
	for (int param = PARAM_STR; param <= PARAM_LUK; param++) { 
		// Calculate job bonus for this parameter  
		int job_bonus = 0;  
		std::shared_ptr<s_job_info> job_info = job_db.find(pc_mapid2jobid(sd->class_, sd->status.sex));  
		if (job_info != nullptr && sd->status.job_level > 0) {  
			const auto& bonus = job_info->job_bonus[sd->status.job_level - 1];  
			job_bonus = bonus[param];  
		}
		// Include job bonus in total calculation  
		int total_bonus = sd->indexed_bonus.param_bonus[param] + sd->indexed_bonus.param_equip[param] + job_bonus;

		if (total_bonus != 0) {        
			std::string stat_name = get_stat_name(param);  

			int base_stat_value = 0;  
			switch(param) {  
				case PARAM_STR: base_stat_value = sd->status.str; break;  
				case PARAM_AGI: base_stat_value = sd->status.agi; break;  
				case PARAM_VIT: base_stat_value = sd->status.vit; break;  
				case PARAM_INT: base_stat_value = sd->status.int_; break;  
				case PARAM_DEX: base_stat_value = sd->status.dex; break;  
				case PARAM_LUK: base_stat_value = sd->status.luk; break;  
			}  
					
			if (total_bonus < 0) {        
				sprintf(output, "   -> %s: %s ~ %s", stat_name.c_str(),   
						rathena::util::insert_comma(base_stat_value).c_str(),   
						rathena::util::insert_comma(total_bonus).c_str());        
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);        
			} else {        
				sprintf(output, "   -> %s: %s ~ +%s", stat_name.c_str(),   
						rathena::util::insert_comma(base_stat_value).c_str(),   
						rathena::util::insert_comma(total_bonus).c_str());        
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);      
			}        
			base_stats_found = true;        
		}        
	}  
      
    if (!base_stats_found) {    
        clif_displaymessage(fd, "   -> No Base Stats bonuses found");    
    }  
      
    // Trait Stats  (bPow, bSta, bWis, bSpl, bCon & bCrt)
    clif_displaymessage(fd, " * Trait Stats");    
    bool trait_stats_found = false;  
      
    for (int param = PARAM_POW; param <= PARAM_CRT; param++) {      
		// Calculate job bonus for this parameter  
		int job_bonus = 0;  
		std::shared_ptr<s_job_info> job_info = job_db.find(pc_mapid2jobid(sd->class_, sd->status.sex));  
		if (job_info != nullptr && sd->status.job_level > 0) {  
			const auto& bonus = job_info->job_bonus[sd->status.job_level - 1];  
			job_bonus = bonus[param];  
		}
		// Include job bonus in total calculation
        int total_bonus = sd->indexed_bonus.param_bonus[param] + sd->indexed_bonus.param_equip[param] + job_bonus;      

        int base_trait_value = 0;  
        switch(param) {  
            case PARAM_POW: base_trait_value = sd->status.pow; break;  
            case PARAM_STA: base_trait_value = sd->status.sta; break;  
            case PARAM_WIS: base_trait_value = sd->status.wis; break;  
            case PARAM_SPL: base_trait_value = sd->status.spl; break;  
            case PARAM_CON: base_trait_value = sd->status.con; break;  
            case PARAM_CRT: base_trait_value = sd->status.crt; break;  
        }

        if (total_bonus != 0) {      
            std::string trait_name = get_trait_name(param);      
                  
            if (total_bonus < 0) {      
                sprintf(output, "   -> %s Bonus: %s", trait_name.c_str(), rathena::util::insert_comma(base_trait_value).c_str());      
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
  
    // HP/SP/AP Section  
    clif_displaymessage(fd, " * HP/SP/AP");      
    bool hp_sp_ap_found = false;  

	// Flat bMaxHP, bMaxSP & bMaxAP
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

	// % bMaxHPrate, bMaxSPrate & bMaxAPrate
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
  
    // Atk/Def Section (Fixed with missing bonuses)  
    clif_displaymessage(fd, " * Atk/Def");      
    bool atk_def_found = false;    
  
	// Physical Attack (ATK)
	if (pc_leftside_atk(sd) != 0 || pc_rightside_atk(sd) != 0) {  
		sprintf(output, "   -> Physical Damage (ATK): %s ~ %s",   
				rathena::util::insert_comma(pc_leftside_atk(sd)).c_str(),  
				rathena::util::insert_comma(pc_rightside_atk(sd)).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		atk_def_found = true;  
	}  
	  
	// Magical Attack (MATK)
	if (pc_leftside_matk(sd) != 0 || pc_rightside_matk(sd) != 0) {  
		sprintf(output, "   -> Magical Damage (MATK): %s ~ %s",   
				rathena::util::insert_comma(pc_leftside_matk(sd)).c_str(),  
				rathena::util::insert_comma(pc_rightside_matk(sd)).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		atk_def_found = true;  
	}  
	  
	// Physical Defense (DEF)  
	if (pc_leftside_def(sd) != 0 || pc_rightside_def(sd) != 0) {  
		sprintf(output, "   -> Physical Defense (DEF): %s ~ %s",   
				rathena::util::insert_comma(pc_leftside_def(sd)).c_str(),  
				rathena::util::insert_comma(pc_rightside_def(sd)).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		atk_def_found = true;  
	}  
	  
	// Magical Defense (MDEF)  
	if (pc_leftside_mdef(sd) != 0 || pc_rightside_mdef(sd) != 0) {  
		sprintf(output, "   -> Magical Defense (MDEF): %s ~ %s",   
				rathena::util::insert_comma(pc_leftside_mdef(sd)).c_str(),  
				rathena::util::insert_comma(pc_rightside_mdef(sd)).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		atk_def_found = true;  
	}  

	// bAtkRate
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

	// bMatkRate 
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

	if (!atk_def_found) {    
        clif_displaymessage(fd, "   -> No Atk/Def bonuses found");    
    }  
  
    // Additional Stats Section  
    clif_displaymessage(fd, " * Additional Stats");    
    bool additional_stats_found = false;  

	// bHit (stats + equipment)
	if (status->hit != 0) {  
		if (status->hit < 0) {  
			sprintf(output, "   -> Hit: %s", rathena::util::insert_comma(status->hit).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Hit: +%s", rathena::util::insert_comma(status->hit).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}

	// bonus bHitRate
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

	// bCritical - Flat Critical bonus (stats + equipment)
	if (status->cri / 10 != 0) {  
		if (status->cri / 10 < 0) {  
			sprintf(output, "   -> Critical: %s", rathena::util::insert_comma(status->cri / 10).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Critical: +%s", rathena::util::insert_comma(status->cri / 10).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}

	// bCriticalRate
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

	// bCriticalLong
    if (sd->bonus.critical_rangeatk != 0) {    
        int critical_ranged_bonus = sd->bonus.critical_rangeatk / 10;    
        if (critical_ranged_bonus < 0) {    
            sprintf(output, "   -> Critical Long Range: %s", rathena::util::insert_comma(critical_ranged_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
        } else {    
            sprintf(output, "   -> Critical Long Range: +%s", rathena::util::insert_comma(critical_ranged_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }    
        additional_stats_found = true;    
    }    

	// bFlee - Flat Flee bonus (stats + equipment)   
	if (status->flee != 0) {  
		if (status->flee < 0) {  
			sprintf(output, "   -> Flee: %s", rathena::util::insert_comma(status->flee).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> Flee: +%s", rathena::util::insert_comma(status->flee).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}

	// bFleeRate
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

	// bFlee2 
	if (status->flee2 / 10 > 0) {  
		if (status->flee2 / 10 != 0) {  
			if (status->flee2 / 10 < 0) {  
				sprintf(output, "   -> Perfect Dodge: %s", rathena::util::insert_comma(status->flee2 / 10).c_str());  
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
			} else {  
				sprintf(output, "   -> Perfect Dodge: +%s", rathena::util::insert_comma(status->flee2 / 10).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			}  
			additional_stats_found = true;  
		}  
	}

	// bFlee2Rate
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

	// bPerfectHitRate
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

	// bPerfectHitAddRate
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

	// bSpeedRate
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

	// bSpeedAddRate
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

	// bAspd - Equipment ASPD bonus only  
	if (sd->bonus.aspd_add != 0) {  
		// Convert from internal format (aspd_add is stored as negative values for bonuses)  
		int aspd_bonus = -sd->bonus.aspd_add / 10;  
		  
		if (aspd_bonus < 0) {  
			sprintf(output, "   -> ASPD: %s", rathena::util::insert_comma(aspd_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {  
			sprintf(output, "   -> ASPD: +%s", rathena::util::insert_comma(aspd_bonus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		}  
		additional_stats_found = true;  
	}
    
	// bAspdRate - ASPD Rate bonus  
#ifndef RENEWAL_ASPD  
	if (sd->battle_status.aspd_rate != 1000) {    
		int aspd_rate_bonus = (1000 - sd->battle_status.aspd_rate) / 10;
		if (aspd_rate_bonus < 0) {    
			sprintf(output, "   -> ASPD Rate: %s%%", rathena::util::insert_comma(aspd_rate_bonus).c_str());    
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
			sprintf(output, "   -> ASPD Rate: +%s%%", rathena::util::insert_comma(aspd_rate_bonus).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
		}    
		additional_stats_found = true;    
	}  
#else  
	// For Renewal, use aspd_rate2 which starts at 0  
	if (sd->battle_status.aspd_rate2 != 0) {    
		sprintf(output, "   -> ASPD Rate: %s%s%%",   
				sd->battle_status.aspd_rate2 > 0 ? "+" : "",  
				rathena::util::insert_comma(sd->battle_status.aspd_rate2).c_str());    
		clif_messagecolor(sd, sd->battle_status.aspd_rate2 > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);    
		additional_stats_found = true;    
	}  
#endif

	// bAtkRange - Attack Range bonus  
	// Note: This shows total range including weapon base + bonuses  
	if (sd->battle_status.rhw.range > 0) {  
		sprintf(output, "   -> Total Attack Range: %s",   
				rathena::util::insert_comma(sd->battle_status.rhw.range).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		additional_stats_found = true;  
	}  
	  
	// For bonus-only display, you'd need to compare against base weapon range  
	// which requires accessing the weapon's base range from inventory data  
	if (sd->inventory_data[sd->equip_index[EQI_HAND_R]]) {  
		int base_range = sd->inventory_data[sd->equip_index[EQI_HAND_R]]->range;  
		int range_bonus = sd->battle_status.rhw.range - base_range;  
		if (range_bonus != 0) {  
			sprintf(output, "   -> Attack Range Bonus: %s%s",   
					range_bonus > 0 ? "+" : "",  
					rathena::util::insert_comma(range_bonus).c_str());  
			clif_messagecolor(sd, range_bonus > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			additional_stats_found = true;  
		}  
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

	// bPAtk - Flat PAtk bonus (from SP_PATK in pc_bonus)  
	if (status->patk != 0) {  
		if (status->patk < 0) {  
			sprintf(output, "   -> Final Physical Damage (P.ATK): %s", rathena::util::insert_comma(status->patk).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {    
            sprintf(output, "   -> Final Physical Damage (P.ATK): +%s", rathena::util::insert_comma(status->patk).c_str());  
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        } 
		additional_stats_found = true;  		
	}

	// bPAtkRate
    if (sd->patk_rate != 100) {    
        int patk_rate_bonus = sd->patk_rate - 100;    
        if (patk_rate_bonus < 0) {    
            sprintf(output, "   -> P.ATK Rate: %s%%", rathena::util::insert_comma(patk_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
        } else {    
            sprintf(output, "   -> P.ATK Rate: +%s%%", rathena::util::insert_comma(patk_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }    
        additional_stats_found = true;    
    }    

	// bSMatk - Flat SMatk bonus (from SP_SMATK in pc_bonus)  
	if (status->smatk != 0) {  
		if (status->smatk < 0) {  
			sprintf(output, "   -> Final Magical Damage (S.MATK): %s", rathena::util::insert_comma(status->smatk).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
            sprintf(output, "   -> Final Magical Damage (S.MATK): +%s", rathena::util::insert_comma(status->smatk).c_str());
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }  
		additional_stats_found = true;		
	}

	// bSMatkRate
    if (sd->smatk_rate != 100) {    
        int smatk_rate_bonus = sd->smatk_rate - 100;    
        if (smatk_rate_bonus < 0) {    
            sprintf(output, "   -> S.MATK Rate: %s%%", rathena::util::insert_comma(smatk_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
        } else {    
            sprintf(output, "   -> S.MATK Rate: +%s%%", rathena::util::insert_comma(smatk_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }    
        additional_stats_found = true;    
    }    

	// bRes 
	if (status->res != 0) {  
		if (status->res < 0) {  
			sprintf(output, "   -> Physical Resistance (RES): %s", rathena::util::insert_comma(status->res).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
            sprintf(output, "   -> Physical Resistance (RES): +%s", rathena::util::insert_comma(status->res).c_str());  
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }  
		additional_stats_found = true;		
	}

	// bResRate
    if (sd->res_rate != 100) {    
        int res_rate_bonus = sd->res_rate - 100;    
        if (res_rate_bonus < 0) {    
            sprintf(output, "   -> RES Rate: %s%%", rathena::util::insert_comma(res_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
        } else {    
            sprintf(output, "   -> RES Rate: +%s%%", rathena::util::insert_comma(res_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }    
        additional_stats_found = true;    
    }    

	// bMRes
	if (status->mres != 0) {  
		if (status->mres < 0) {  
			sprintf(output, "   -> Magical Resistance (MRES): %s", rathena::util::insert_comma(status->mres).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
		} else {    
            sprintf(output, "   -> Magical Resistance (MRES): +%s", rathena::util::insert_comma(status->mres).c_str());  
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }  
		additional_stats_found = true;		
	}

	// bMResRate
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

	// bHPlus 
	if (status->hplus != 0) {  
		if (status->hplus < 0) {  
			sprintf(output, "   -> Heal Plus (H.Plus): %s", rathena::util::insert_comma(status->hplus).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {    
            sprintf(output, "   -> Heal Plus (H.Plus): +%s", rathena::util::insert_comma(status->hplus).c_str());  
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }    
        additional_stats_found = true;  
	}

	// bHPlusRate
    if (sd->hplus_rate != 100) {    
        int hplus_rate_bonus = sd->hplus_rate - 100;    
        if (hplus_rate_bonus < 0) {    
            sprintf(output, "   -> H.Plus Rate: %s%%", rathena::util::insert_comma(hplus_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
        } else {    
            sprintf(output, "   -> H.Plus Rate: +%s%%", rathena::util::insert_comma(hplus_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }    
        additional_stats_found = true;    
    }    

	// bCRate 
	if (status->crate != 0) {  
		if (status->crate < 0) {  
			sprintf(output, "   -> Critical Damage (C.Rate): %s", rathena::util::insert_comma(status->crate).c_str());  
			clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		} else {    
            sprintf(output, "   -> Critical Damage (C.Rate): +%s", rathena::util::insert_comma(status->crate).c_str());  
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }    
        additional_stats_found = true;  
	}

	// bCRateRate
    if (sd->crate_rate != 100) {    
        int crate_rate_bonus = sd->crate_rate - 100;    
        if (crate_rate_bonus < 0) {    
            sprintf(output, "   -> Critical Damage Rate: %s%%", rathena::util::insert_comma(crate_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
        } else {    
            sprintf(output, "   -> Critical Damage Rate: +%s%%", rathena::util::insert_comma(crate_rate_bonus).c_str());    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
        }    
        additional_stats_found = true;    
    }  
  
    if (!additional_stats_found) {    
        clif_displaymessage(fd, "   -> No Additional Stats bonuses found");   
    }    
}
  
ACMD_FUNC(basicbonuses) {    
    nullpo_retr(-1, sd);    
    atcommand_basicbonuses_sub(fd, sd);    
    return 0;    
}

/*==========================================    
* @extendedbonuses  
* shows extended stat bonuses of players  
*------------------------------------------*/    
void atcommand_extendedbonuses_sub(int fd, class map_session_data *sd) {    
    char output[CHAT_SIZE_MAX];    

	clif_displaymessage(fd, "=============================");
    clif_displaymessage(fd, "EXTENDED BONUSES");  
    clif_displaymessage(fd, "=============================");

	// bHPrecovRate
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

	// bSPrecovRate
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
 
	// bHPRegenRate
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

	// bSPRegenRate
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

	// bHPLossRate
	if (sd->hp_loss.value != 0) {  
		sprintf(output, "   -> HP Loss: %s every %s ms",   
				rathena::util::insert_comma(sd->hp_loss.value).c_str(),  
				rathena::util::insert_comma(sd->hp_loss.rate).c_str());  
		clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		hp_sp_found = true;
	}  

	// bSPLossRate
	if (sd->sp_loss.value != 0) {  
		sprintf(output, "   -> SP Loss: %s every %s ms",   
				rathena::util::insert_comma(sd->sp_loss.value).c_str(),  
				rathena::util::insert_comma(sd->sp_loss.rate).c_str());  
		clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);  
		hp_sp_found = true;
	}  

	// bRegenPercentHP
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

	// bRegenPercentSP
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

	// bNoRegen
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

	// bUseSPrate
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

	// bSkillUseSP
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

	// bSkillUseSPrate
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

	// bSkillAtk
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

	// bSkillRatio
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

	// bShortAtkRate
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

	// bLongAtkRate
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

	// bCritAtkRate
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

	// bCritDefRate
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

	// bCriticalDef
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
		// bWeaponAtk
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

		// bWeaponDamageRate
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

	// bNearAtkDef
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

	// bLongAtkDef
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

	// bMagicAtkDef
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

	// bMiscAtkDef
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

	// bNoWeaponDamage - Physical damage reduction  
	if (sd->special_state.no_weapon_damage != 0) {  
		sprintf(output, "   -> Physical Damage Reduction: %s%%",   
				rathena::util::insert_comma(sd->special_state.no_weapon_damage).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		extended_atkdef_found = true;  
	}  
	  
	// bNoMagicDamage - Magic damage reduction    
	if (sd->special_state.no_magic_damage != 0) {  
		sprintf(output, "   -> Magic Damage Reduction: %s%%",   
				rathena::util::insert_comma(sd->special_state.no_magic_damage).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		extended_atkdef_found = true;  
	}  
	  
	// bNoMiscDamage - Misc damage reduction  
	if (sd->special_state.no_misc_damage != 0) {  
		sprintf(output, "   -> Misc Damage Reduction: %s%%",   
				rathena::util::insert_comma(sd->special_state.no_misc_damage).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		extended_atkdef_found = true;  
	}

	if (!extended_atkdef_found) {  
		clif_displaymessage(fd, "   -> No extended attack/defense bonuses found");  
	}

	clif_displaymessage(fd, " * Healing");  
	bool healing_found = false;  

	// bHealPower
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

	// bHealPower2
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

	// bSkillHeal
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

	// bSkillHeal2
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

	// bAddItemHealRate
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

	// bAddItemSPHealRate
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

	// bAddItemSPHealRate
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

	// bAddItemSPHealRate
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

	// bAddItemGroupHealRate
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

	// bAddItemGroupSPHealRate
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

	// bCastrate
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
	// bFixedCastrate
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

	// bVariableCastrate
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

	// bFixedCast
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

	// bVariableCast
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

	// bSkillVariableCast
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
	// bSkillFixedCast
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

	// bFixedCast
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

	// bSkillVariableCast
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

	// bNoCastCancel
	if (sd->special_state.no_castcancel) {  
		sprintf(output, "   -> Cast Cancel Protection: Enabled (Non-GvG)");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		casting_found = true;  
	}  

	// bNoCastCancel2
	if (sd->special_state.no_castcancel2) {  
		sprintf(output, "   -> Cast Cancel Protection: Enabled (All Areas)");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		casting_found = true;  
	}  

	// bDelayrate
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

	// bSkillDelay
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

	// bSkillCooldown
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
}  
  
ACMD_FUNC(extendedbonuses) {    
    nullpo_retr(-1, sd);    
    atcommand_extendedbonuses_sub(fd, sd);    
    return 0;    
}

/*==========================================    
* @gspecificbonuses  
* shows group-specific bonuses of players  
*------------------------------------------*/    
void atcommand_gspecificbonuses_sub(int fd, class map_session_data *sd) {    
    char output[CHAT_SIZE_MAX];    

	clif_displaymessage(fd, "=============================");
	clif_displaymessage(fd, "GROUP-SPECIFIC BONUSES");
	clif_displaymessage(fd, "=============================");        
	  
	// Element-based bonuses (bAddEle, bMagicAddEle, bSubEle)  
	clif_displaymessage(fd, " * Damage Modifiers");    
	bool damage_modifiers_found = false;
	
	for (int ele = 0; ele < ELE_MAX; ele++) {    
		if (sd->right_weapon.addele[ele] != 0) {    
			std::string ele_name = get_element_name(ele);    
			sprintf(output, "   -> Physical damage against %s element: %s%s%%",     
					ele_name.c_str(), sd->right_weapon.addele[ele] > 0 ? "+" : "",     
					rathena::util::insert_comma(sd->right_weapon.addele[ele]).c_str());    
			clif_messagecolor(sd, sd->right_weapon.addele[ele] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);    
			damage_modifiers_found = true;    
		}    
			
		if (sd->indexed_bonus.magic_addele_script[ele] != 0) {    
			std::string ele_name = get_element_name(ele);    
			sprintf(output, "   -> Magical damage against %s element: %s%s%%",     
					ele_name.c_str(), sd->indexed_bonus.magic_addele_script[ele] > 0 ? "+" : "",     
					rathena::util::insert_comma(sd->indexed_bonus.magic_addele_script[ele]).c_str());    
			clif_messagecolor(sd, sd->indexed_bonus.magic_addele_script[ele] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);    
			damage_modifiers_found = true;    
		}    
			
		if (sd->indexed_bonus.subele_script[ele] != 0) {    
			std::string ele_name = get_element_name(ele);    
			sprintf(output, "   -> Damage reduction against attack %s element: %s%s%%",     
					ele_name.c_str(), sd->indexed_bonus.subele_script[ele] > 0 ? "+" : "",     
					rathena::util::insert_comma(sd->indexed_bonus.subele_script[ele]).c_str());    
			clif_messagecolor(sd, sd->indexed_bonus.subele_script[ele] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);    
			damage_modifiers_found = true;    
		}    
	}  
	  
	// bSubDefEle and bMagicSubDefEle  
	for (int ele = 0; ele < ELE_MAX; ele++) {  
		if (sd->indexed_bonus.subdefele[ele] != 0) {  
			std::string ele_name = get_element_name(ele);  
			sprintf(output, "   -> Physical damage reduction from enemy with %s defense element: %s%s%%",   
					ele_name.c_str(), sd->indexed_bonus.subdefele[ele] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.subdefele[ele]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.subdefele[ele] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.magic_subdefele[ele] != 0) {  
			std::string ele_name = get_element_name(ele);  
			sprintf(output, "   -> Magic damage reduction from enemy with %s defense element: %s%s%%",   
					ele_name.c_str(), sd->indexed_bonus.magic_subdefele[ele] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.magic_subdefele[ele]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.magic_subdefele[ele] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}  
	  
	// Race-based bonuses (bAddRace, bMagicAddRace, bSubRace)  
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->right_weapon.addrace[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Physical damage against %s race: %s%s%%",   
					race_name.c_str(), sd->right_weapon.addrace[race] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->right_weapon.addrace[race]).c_str());  
			clif_messagecolor(sd, sd->right_weapon.addrace[race] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.magic_addrace[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Magical damage against %s race: %s%s%%",   
					race_name.c_str(), sd->indexed_bonus.magic_addrace[race] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.magic_addrace[race]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.magic_addrace[race] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.subrace[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Damage reduction against %s race: %s%s%%",   
					race_name.c_str(), sd->indexed_bonus.subrace[race] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.subrace[race]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.subrace[race] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}  
	  
	// Class-based bonuses with CLASS_ALL merging (bAddClass, bMagicAddClass, bSubClass)  
	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {  
		if (sd->right_weapon.addclass[class_type] != 0) {  
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> Physical damage against %s class: %s%s%%",   
					class_name.c_str(),   
					sd->right_weapon.addclass[class_type] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->right_weapon.addclass[class_type]).c_str());  
			clif_messagecolor(sd, sd->right_weapon.addclass[class_type] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.magic_addclass[class_type] != 0) {  
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> Magical damage against %s class: %s%s%%",   
					class_name.c_str(),   
					sd->indexed_bonus.magic_addclass[class_type] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.magic_addclass[class_type]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.magic_addclass[class_type] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.subclass[class_type] != 0) {  
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> Damage reduction against %s class: %s%s%%",   
					class_name.c_str(),   
					sd->indexed_bonus.subclass[class_type] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.subclass[class_type]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.subclass[class_type] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}  
	  
	// Size-based bonuses (bAddSize, bMagicAddSize, bSubSize, bWeaponSubSize, bMagicSubSize)  
	for (int size = 0; size < SZ_MAX; size++) {  
		if (sd->right_weapon.addsize[size] != 0) {  
			std::string size_name = get_size_name(size);  
			sprintf(output, "   -> Physical damage against %s size: %s%s%%",   
					size_name.c_str(), sd->right_weapon.addsize[size] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->right_weapon.addsize[size]).c_str());  
			clif_messagecolor(sd, sd->right_weapon.addsize[size] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.magic_addsize[size] != 0) {  
			std::string size_name = get_size_name(size);  
			sprintf(output, "   -> Magical damage against %s size: %s%s%%",   
					size_name.c_str(), sd->indexed_bonus.magic_addsize[size] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.magic_addsize[size]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.magic_addsize[size] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.subsize[size] != 0) {  
			std::string size_name = get_size_name(size);  
			sprintf(output, "   -> Damage reduction against %s size: %s%s%%",   
					size_name.c_str(), sd->indexed_bonus.subsize[size] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.subsize[size]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.subsize[size] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.weapon_subsize[size] != 0) {  
			std::string size_name = get_size_name(size);  
			sprintf(output, "   -> Physical damage reduction against %s size: %s%s%%",   
					size_name.c_str(), sd->indexed_bonus.weapon_subsize[size] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.weapon_subsize[size]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.weapon_subsize[size] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.magic_subsize[size] != 0) {  
			std::string size_name = get_size_name(size);  
			sprintf(output, "   -> Magic damage reduction against %s size: %s%s%%",   
					size_name.c_str(), sd->indexed_bonus.magic_subsize[size] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.magic_subsize[size]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.magic_subsize[size] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}  
	  
	// bNoSizeFix  
	if (sd->special_state.no_sizefix) {  
		sprintf(output, "   -> Size modifier: Ignored");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_modifiers_found = true;  
	}  
	  
	// Monster-specific bonuses (bAddDamageClass, bAddMagicDamageClass)  
	for (const auto &bonus : sd->right_weapon.add_dmg) {  
		if (bonus.val != 0) {  
			std::shared_ptr<s_mob_db> mob = mob_db.find(bonus.id);  
			const char* monster_name = (mob != nullptr) ? mob->jname.c_str() : "Unknown Monster";  
			  
			sprintf(output, "   -> Physical damage against %s: %s%s%%",   
					monster_name, bonus.val > 0 ? "+" : "",   
					rathena::util::insert_comma(bonus.val).c_str());  
			clif_messagecolor(sd, bonus.val > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}  
	  
	for (const auto &bonus : sd->add_mdmg) {  
		if (bonus.val != 0) {  
			std::shared_ptr<s_mob_db> mob = mob_db.find(bonus.id);  
			const char* monster_name = (mob != nullptr) ? mob->jname.c_str() : "Unknown Monster";  
			  
			sprintf(output, "   -> Magical damage against %s: %s%s%%",   
					monster_name, bonus.val > 0 ? "+" : "",   
					rathena::util::insert_comma(bonus.val).c_str());  
			clif_messagecolor(sd, bonus.val > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}

	// Defense bonuses (bAddDefMonster, bAddMDefMonster)  
	for (const auto &bonus : sd->add_def) {  
		if (bonus.val != 0) {  
			std::shared_ptr<s_mob_db> mob = mob_db.find(bonus.id);  
			const char* monster_name = (mob != nullptr) ? mob->jname.c_str() : "Unknown Monster";  
			  
			sprintf(output, "   -> Physical damage reduction against %s: %s%s%%",   
					monster_name, bonus.val > 0 ? "+" : "",   
					rathena::util::insert_comma(bonus.val).c_str());  
			clif_messagecolor(sd, bonus.val > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}  
	  
	for (const auto &bonus : sd->add_mdef) {  
		if (bonus.val != 0) {  
			std::shared_ptr<s_mob_db> mob = mob_db.find(bonus.id);  
			const char* monster_name = (mob != nullptr) ? mob->jname.c_str() : "Unknown Monster";  
			  
			sprintf(output, "   -> Magical damage reduction against %s: %s%s%%",   
					monster_name, bonus.val > 0 ? "+" : "",   
					rathena::util::insert_comma(bonus.val).c_str());  
			clif_messagecolor(sd, bonus.val > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}  
	  
	// Race2 bonuses (bAddRace2, bSubRace2, bMagicAddRace2)  
	for (int race2 = 0; race2 < RC2_MAX; race2++) {  
		if (sd->right_weapon.addrace2[race2] != 0) {
			std::string race2_name = get_race2_name(race2);
			sprintf(output, "   -> Damage against %s race: %s%s%%",   
					race2_name.c_str(),   
					sd->right_weapon.addrace2[race2] > 0 ? "+" : "",  
					rathena::util::insert_comma(sd->right_weapon.addrace2[race2]).c_str());  
			clif_messagecolor(sd, sd->right_weapon.addrace2[race2] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
		  
		if (sd->indexed_bonus.subrace2[race2] != 0) {    
			std::string race2_name = get_race2_name(race2);  
			sprintf(output, "   -> Damage reduction against %s race: %s%s%%",     
					race2_name.c_str(), sd->indexed_bonus.subrace2[race2] > 0 ? "+" : "",     
					rathena::util::insert_comma(sd->indexed_bonus.subrace2[race2]).c_str());    
			clif_messagecolor(sd, sd->indexed_bonus.subrace2[race2] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);    
			damage_modifiers_found = true;    
		}    
			
		if (sd->indexed_bonus.magic_addrace2[race2] != 0) {    
			std::string race2_name = get_race2_name(race2);  
			sprintf(output, "   -> Magic damage against %s race: %s%s%%",     
					race2_name.c_str(), sd->indexed_bonus.magic_addrace2[race2] > 0 ? "+" : "",     
					rathena::util::insert_comma(sd->indexed_bonus.magic_addrace2[race2]).c_str());    
			clif_messagecolor(sd, sd->indexed_bonus.magic_addrace2[race2] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);    
			damage_modifiers_found = true;    
		}
  
	}  
	  
	// Skill damage reduction (bSubSkill)  
	for (const auto &bonus : sd->subskill) {  
		const char* skill_desc = skill_get_desc(bonus.id);  
		if (skill_desc) {  
			sprintf(output, "   -> Damage reduction from %s: %s%s%%",   
					skill_desc, bonus.val > 0 ? "+" : "",   
					rathena::util::insert_comma(bonus.val).c_str());  
			clif_messagecolor(sd, bonus.val > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}  
	  
	// Absorb damage bonuses (bAbsorbDmgMaxHP, bAbsorbDmgMaxHP2)  
	if (sd->bonus.absorb_dmg_maxhp != 0) {  
		sprintf(output, "   -> Absorb damage MaxHP (Legacy): %s%%",   
				rathena::util::insert_comma(sd->bonus.absorb_dmg_maxhp).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_modifiers_found = true;  
	}  
	  
	if (sd->bonus.absorb_dmg_maxhp2 != 0) {  
		sprintf(output, "   -> Absorb damage MaxHP2 (Official): %s%%",   
				rathena::util::insert_comma(sd->bonus.absorb_dmg_maxhp2).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_modifiers_found = true;  
	}  

	// bCriticalAddRace - Critical bonus vs Race  
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->indexed_bonus.critaddrace[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Critical vs %s race: %s%s",  
					race_name.c_str(),  
					sd->indexed_bonus.critaddrace[race] > 0 ? "+" : "",  
					rathena::util::insert_comma(sd->indexed_bonus.critaddrace[race] / 10).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.critaddrace[race] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			damage_modifiers_found = true;  
		}  
	}

	if (!damage_modifiers_found) {  
		clif_displaymessage(fd, "   -> No damage modifier bonuses found");  
	}

	clif_displaymessage(fd, " * Atk/Def");    
	bool atkdef_found = false;  
	  
	// bAtkEle - Attack Element  
	if (sd->battle_status.rhw.ele != ELE_NEUTRAL) {    
		std::string ele_name = get_element_name(sd->battle_status.rhw.ele);    
		sprintf(output, "   -> Attack Element: %s", ele_name.c_str());      
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
		atkdef_found = true;    
	}    
	  
	// bDefEle - Defense Element  
	if (sd->battle_status.def_ele != ELE_NEUTRAL) {    
		std::string ele_name = get_element_name(sd->battle_status.def_ele);    
		sprintf(output, "   -> Defense Element: %s", ele_name.c_str());      
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
		atkdef_found = true;    
	}    
	  
	// bMagicAtkEle - Magic Attack Element bonuses  
	for (int ele = 0; ele < ELE_MAX; ele++) {    
		if (sd->indexed_bonus.magic_atk_ele[ele] != 0) {    
			std::string ele_name = get_element_name(ele);    
			sprintf(output, "   -> %s element magic damage: %s%s%%",   
					ele_name.c_str(),   
					sd->indexed_bonus.magic_atk_ele[ele] > 0 ? "+" : "",   
					rathena::util::insert_comma(sd->indexed_bonus.magic_atk_ele[ele]).c_str());    
			clif_messagecolor(sd, sd->indexed_bonus.magic_atk_ele[ele] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);    
			atkdef_found = true;    
		}    
	}    
	  
	// bDefRatioAtkRace - Defense Ratio Attack vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->right_weapon.def_ratio_atk_race & (1 << race)) {    
			std::string race_name = get_race_name(race);    
			sprintf(output, "   -> Defense ratio attack vs %s race: Enabled", race_name.c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			atkdef_found = true;    
		}    
	}    
	  
	// bDefRatioAtkEle - Defense Ratio Attack vs Element  
	for (int ele = 0; ele < ELE_MAX; ele++) {    
		if (sd->right_weapon.def_ratio_atk_ele & (1 << ele)) {    
			std::string ele_name = get_element_name(ele);    
			sprintf(output, "   -> Defense ratio attack vs %s element: Enabled", ele_name.c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			atkdef_found = true;    
		}    
	}    
	  
	// bDefRatioAtkClass - Defense Ratio Attack vs Class  
	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {  
		if (sd->right_weapon.def_ratio_atk_class & (1 << class_type)) {    
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> Defense ratio attack vs %s class: Enabled", class_name.c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			atkdef_found = true;    
		}    
	}    
	  
	// bSetDefRace - Set DEF vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->def_set_race[race].rate != 0) {    
			std::string race_name = get_race_name(race);    
			sprintf(output, "   -> Set DEF vs %s race: %s%% chance, %s ms, DEF=%s",  
					race_name.c_str(),  
					rathena::util::insert_comma(sd->def_set_race[race].rate).c_str(),  
					rathena::util::insert_comma(sd->def_set_race[race].tick).c_str(),  
					rathena::util::insert_comma(sd->def_set_race[race].value).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			atkdef_found = true;    
		}    
	}    
	  
	// bSetMDefRace - Set MDEF vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->mdef_set_race[race].rate != 0) {    
			std::string race_name = get_race_name(race);    
			sprintf(output, "   -> Set MDEF vs %s race: %s%% chance, %s ms, MDEF=%s",  
					race_name.c_str(),  
					rathena::util::insert_comma(sd->mdef_set_race[race].rate).c_str(),  
					rathena::util::insert_comma(sd->mdef_set_race[race].tick).c_str(),  
					rathena::util::insert_comma(sd->mdef_set_race[race].value).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			atkdef_found = true;    
		}    
	}    
	  
	if (!atkdef_found) {    
		clif_displaymessage(fd, "   -> No Atk/Def bonuses found");    
	}

	clif_displaymessage(fd, " * Ignore Def");    
	bool ignore_def_found = false;  
	  
	// bIgnoreDefEle - Ignore DEF vs Element  
	for (int ele = 0; ele < ELE_MAX; ele++) {    
		if (sd->right_weapon.ignore_def_ele & (1 << ele)) {    
			std::string ele_name = get_element_name(ele);    
			sprintf(output, "   -> Ignore DEF vs %s element: Enabled", ele_name.c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_def_found = true;    
		}    
	}    
	  
	// bIgnoreDefRace - Ignore DEF vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->right_weapon.ignore_def_race & (1 << race)) {    
			std::string race_name = get_race_name(race);    
			sprintf(output, "   -> Ignore DEF vs %s race: Enabled", race_name.c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_def_found = true;    
		}    
	}    
	  
	// bIgnoreDefClass - Ignore DEF vs Class  
	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {  
		if (sd->right_weapon.ignore_def_class & (1 << class_type)) {    
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> Ignore DEF vs %s class: Enabled", class_name.c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_def_found = true;    
		}    
	}    
	  
	// bIgnoreMDefRace - Ignore MDEF vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->bonus.ignore_mdef_race & (1 << race)) {    
			std::string race_name = get_race_name(race);    
			sprintf(output, "   -> Ignore MDEF vs %s race: Enabled", race_name.c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_def_found = true;    
		}    
	}    
	  
	// bIgnoreMDefEle - Ignore MDEF vs Element  
	for (int ele = 0; ele < ELE_MAX; ele++) {    
		if (sd->bonus.ignore_mdef_ele & (1 << ele)) {    
			std::string ele_name = get_element_name(ele);    
			sprintf(output, "   -> Ignore MDEF vs %s element: Enabled", ele_name.c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_def_found = true;    
		}    
	}    
	  
	// bIgnoreDefRaceRate - Ignore DEF Rate vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->indexed_bonus.ignore_def_by_race[race] != 0) {    
			std::string race_name = get_race_name(race);    
			sprintf(output, "   -> Ignore DEF vs %s race: %s%%", race_name.c_str(),     
					rathena::util::insert_comma(sd->indexed_bonus.ignore_def_by_race[race]).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_def_found = true;    
		}    
	}    
	  
	// bIgnoreMdefRaceRate - Ignore MDEF Rate vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->indexed_bonus.ignore_mdef_by_race[race] != 0) {    
			std::string race_name = get_race_name(race);    
			sprintf(output, "   -> Ignore MDEF vs %s race: %s%%", race_name.c_str(),     
					rathena::util::insert_comma(sd->indexed_bonus.ignore_mdef_by_race[race]).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_def_found = true;    
		}    
	}    
	  
	// bIgnoreMdefRace2Rate - Ignore MDEF Rate vs Race2    
	for (int race2 = 0; race2 < RC2_MAX; race2++) {      
		if (sd->indexed_bonus.ignore_mdef_by_race2[race2] != 0) {  
			std::string race2_name = get_race2_name(race2);  
			sprintf(output, "   -> Ignore MDEF vs %s race: %s%%", race2_name.c_str(),       
					rathena::util::insert_comma(sd->indexed_bonus.ignore_mdef_by_race2[race2]).c_str());      
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);      
			ignore_def_found = true;      
		}      
	}    
	  
	// bIgnoreDefClassRate - Ignore DEF Rate vs Class  
	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {  
		if (sd->indexed_bonus.ignore_def_by_class[class_type] != 0) {  
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> Ignore DEF vs %s class: %s%%", class_name.c_str(),  
					rathena::util::insert_comma(sd->indexed_bonus.ignore_def_by_class[class_type]).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_def_found = true;    
		}    
	}    
	  
	// bIgnoreMdefClassRate - Ignore MDEF Rate vs Class  
	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {  
		if (sd->indexed_bonus.ignore_mdef_by_class[class_type] != 0) {  
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> Ignore MDEF vs %s class: %s%%", class_name.c_str(),  
					rathena::util::insert_comma(sd->indexed_bonus.ignore_mdef_by_class[class_type]).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_def_found = true;    
		}    
	}    
	  
	if (!ignore_def_found) {    
		clif_displaymessage(fd, "   -> No ignore defense bonuses found");    
	}  
	  
	clif_displaymessage(fd, " * Ignore Res");    
	bool ignore_res_found = false;      
	  
	// bIgnoreResRaceRate - Ignore Res Rate vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->indexed_bonus.ignore_res_by_race[race] != 0) {    
			std::string race_name = get_race_name(race);    
			sprintf(output, "   -> Ignore Res vs %s race: %s%%", race_name.c_str(),  
					rathena::util::insert_comma(sd->indexed_bonus.ignore_res_by_race[race]).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_res_found = true;    
		}    
	}    
	  
	// bIgnoreMResRaceRate - Ignore MRes Rate vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->indexed_bonus.ignore_mres_by_race[race] != 0) {    
			std::string race_name = get_race_name(race);    
			sprintf(output, "   -> Ignore MRes vs %s race: %s%%", race_name.c_str(),  
					rathena::util::insert_comma(sd->indexed_bonus.ignore_mres_by_race[race]).c_str());    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			ignore_res_found = true;    
		}    
	}    
	  
	if (!ignore_res_found) {    
		clif_displaymessage(fd, "   -> No ignore resistance bonuses found");    
	}    
	  
	clif_displaymessage(fd, " * Experience");    
	bool experience_found = false;    
	  
	// bExpAddRace - Experience vs Race  
	for (int race = 0; race < RC_MAX; race++) {    
		if (sd->indexed_bonus.expaddrace[race] != 0) {    
			std::string race_name = get_race_name(race);    
			if (sd->indexed_bonus.expaddrace[race] < 0) {    
				sprintf(output, "   -> Experience vs %s race: %s%%", race_name.c_str(),  
						rathena::util::insert_comma(sd->indexed_bonus.expaddrace[race]).c_str());    
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
			} else {    
				sprintf(output, "   -> Experience vs %s race: +%s%%", race_name.c_str(),  
						rathena::util::insert_comma(sd->indexed_bonus.expaddrace[race]).c_str());    
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			}    
			experience_found = true;    
		}    
	}    
	  
	// bExpAddClass - Experience vs Class  
	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {  
		if (sd->indexed_bonus.expaddclass[class_type] != 0) {  
			std::string class_name = get_class_name(class_type);  
			if (sd->indexed_bonus.expaddclass[class_type] < 0) {    
				sprintf(output, "   -> Experience vs %s class: %s%%", class_name.c_str(),  
						rathena::util::insert_comma(sd->indexed_bonus.expaddclass[class_type]).c_str());    
				clif_messagecolor(sd, color_table[COLOR_RED], output, false, SELF);    
			} else {    
				sprintf(output, "   -> Experience vs %s class: +%s%%", class_name.c_str(),  
						rathena::util::insert_comma(sd->indexed_bonus.expaddclass[class_type]).c_str());    
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			}    
			experience_found = true;    
		}    
	}    
	  
	if (!experience_found) {    
		clif_displaymessage(fd, "   -> No experience bonuses found");    
	}	
}  
  
ACMD_FUNC(gspecificbonuses) {    
    nullpo_retr(-1, sd);    
    atcommand_gspecificbonuses_sub(fd, sd);    
    return 0;    
}

/*==========================================      
* @srelatedbonuses    
* shows status related bonuses of players    
*------------------------------------------*/      
void atcommand_srelatedbonuses_sub(int fd, class map_session_data *sd) {      
    char output[CHAT_SIZE_MAX];  

	clif_displaymessage(fd, "=============================");
    clif_displaymessage(fd, "STATUS-RELATED BONUSES");      
	clif_displaymessage(fd, "=============================");    
        
    // bAddEff - Status effects on attack    
	bool status_found = false;
	
    for (const auto &effect : sd->addeff) {    
        if (effect.rate != 0) {    
            std::string status_name = get_status_name(effect.sc);    
            sprintf(output, "   -> Chance to cause %s on attack: %.2f%%",     
                    status_name.c_str(),     
                    effect.rate / 100.0);    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
            status_found = true;    
        }    
    }    
        
    // bAddEffWhenHit - Status effects when hit    
    for (const auto &effect : sd->addeff_atked) {    
        if (effect.rate != 0) {    
            std::string status_name = get_status_name(effect.sc);    
            sprintf(output, "   -> Chance to cause %s when hit: %.2f%%",     
                    status_name.c_str(),     
                    effect.rate / 100.0);    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
            status_found = true;    
        }    
    }    
        
    // bAddEffOnSkill - Status effects on skill use    
    for (const auto &effect : sd->addeff_onskill) {    
        if (effect.rate != 0) {    
            std::string status_name = get_status_name(effect.sc);    
            const char* skill_desc = skill_get_desc(effect.skill_id);    
            if (skill_desc) {    
                sprintf(output, "   -> Chance to cause %s when using %s: %.2f%%",     
                        status_name.c_str(), skill_desc,    
                        effect.rate / 100.0);    
                clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
                status_found = true;    
            }    
        }    
    }    
        
    // bResEff - Status effect resistance    
    for (const auto &resist : sd->reseff) {    
        if (resist.val != 0) {    
            std::string status_name = get_status_name(resist.id);    
            sprintf(output, "   -> %s resistance: %s%.2f%%",     
                    status_name.c_str(),    
                    resist.val > 0 ? "+" : "",    
                    resist.val / 1.0);  // Assuming resist.val is already in percentage format  
            clif_messagecolor(sd, resist.val > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);    
            status_found = true;    
        }    
    }    
        
    // bComaClass - Coma chance vs Class    
    for (int class_type = 0; class_type < CLASS_MAX; class_type++) {    
        if (sd->indexed_bonus.coma_class[class_type] != 0) {    
            std::string class_name = get_class_name(class_type);    
            sprintf(output, "   -> Coma chance vs %s class: %.2f%%",     
                    class_name.c_str(),    
                    sd->indexed_bonus.coma_class[class_type] / 100.0);    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
            status_found = true;    
        }    
    }    
        
    // bComaRace - Coma chance vs Race    
    for (int race = 0; race < RC_MAX; race++) {    
        if (sd->indexed_bonus.coma_race[race] != 0) {    
            std::string race_name = get_race_name(race);    
            sprintf(output, "   -> Coma chance vs %s race: %.2f%%",     
                    race_name.c_str(),    
                    sd->indexed_bonus.coma_race[race] / 100.0);    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
            status_found = true;    
        }    
    }    
        
    // bWeaponComaEle - Weapon Coma vs Element    
    for (int ele = 0; ele < ELE_MAX; ele++) {    
        if (sd->indexed_bonus.weapon_coma_ele[ele] != 0) {    
            std::string ele_name = get_element_name(ele);    
            sprintf(output, "   -> Weapon coma chance vs %s element: %.2f%%",     
                    ele_name.c_str(),    
                    sd->indexed_bonus.weapon_coma_ele[ele] / 100.0);    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
            status_found = true;    
        }    
    }    
        
    // bWeaponComaRace - Weapon Coma vs Race    
    for (int race = 0; race < RC_MAX; race++) {    
        if (sd->indexed_bonus.weapon_coma_race[race] != 0) {    
            std::string race_name = get_race_name(race);    
            sprintf(output, "   -> Weapon coma chance vs %s race: %.2f%%",     
                    race_name.c_str(),    
                    sd->indexed_bonus.weapon_coma_race[race] / 100.0);    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
            status_found = true;    
        }    
    }    
        
    // bWeaponComaClass - Weapon Coma vs Class    
    for (int class_type = 0; class_type < CLASS_MAX; class_type++) {    
        if (sd->indexed_bonus.weapon_coma_class[class_type] != 0) {    
            std::string class_name = get_class_name(class_type);    
            sprintf(output, "   -> Weapon coma chance vs %s class: %.2f%%",     
                    class_name.c_str(),    
                    sd->indexed_bonus.weapon_coma_class[class_type] / 100.0);    
            clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
            status_found = true;    
        }    
    }    
        
    if (!status_found) {    
        clif_displaymessage(fd, "   -> No status-related bonuses found");    
    }  
}

ACMD_FUNC(srelatedbonuses) {    
    nullpo_retr(-1, sd);    
    atcommand_srelatedbonuses_sub(fd, sd);    
    return 0;    
}

/*==========================================  
* @autospellbonuses
* shows stat bonus of players
* Louis T Steinhil  
*------------------------------------------*/  
void atcommand_autospellbonuses_sub(int fd, class map_session_data *sd) {  
    char output[CHAT_SIZE_MAX];

	clif_displaymessage(fd, "=============================");
	clif_displaymessage(fd, "AUTOSPELL BONUSES");
	clif_displaymessage(fd, "=============================");      
	  
	// bAutoSpell - AutoSpell on attack
	bool autospell_found = false;	

	for (const auto &spell : sd->autospell) {  
		if (spell.rate != 0) {  
			const char* skill_desc = skill_get_desc(spell.id);  
			if (skill_desc) {  
				std::string flags_desc = "";  
				if (spell.flag & AUTOSPELL_FORCE_SELF) {  
					flags_desc += " (cast on self)";  
				} else if (spell.flag & AUTOSPELL_FORCE_TARGET) {  
					flags_desc += " (cast on target)";  
				}  
				if (spell.flag & AUTOSPELL_FORCE_RANDOM_LEVEL) {  
					flags_desc += " (random level 1-" + std::to_string(spell.lv) + ")";  
				}  
				  
				sprintf(output, "   -> AutoSpell on attack: %s Lv.%d (%s%%)%s",   
						skill_desc, spell.lv,  
						rathena::util::insert_comma(spell.rate / 10).c_str(),  
						flags_desc.c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
				autospell_found = true;  
			}  
		}  
	}  
	  
	// bAutoSpellWhenHit - AutoSpell when hit  
	for (const auto &spell : sd->autospell2) {  
		if (spell.rate != 0) {  
			const char* skill_desc = skill_get_desc(spell.id);  
			if (skill_desc) {  
				std::string flags_desc = "";  
				if (spell.flag & AUTOSPELL_FORCE_SELF) {  
					flags_desc += " (cast on self)";  
				} else if (spell.flag & AUTOSPELL_FORCE_TARGET) {  
					flags_desc += " (cast on target)";  
				}  
				if (spell.flag & AUTOSPELL_FORCE_RANDOM_LEVEL) {  
					flags_desc += " (random level 1-" + std::to_string(spell.lv) + ")";  
				}  
				  
				sprintf(output, "   -> AutoSpell when hit: %s Lv.%d (%s%%)%s",   
						skill_desc, spell.lv,  
						rathena::util::insert_comma(spell.rate / 10).c_str(),  
						flags_desc.c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
				autospell_found = true;  
			}  
		}  
	}  
	  
	// bAutoSpellOnSkill - AutoSpell on skill use  
	for (const auto &spell : sd->autospell3) {  
		if (spell.rate != 0) {  
			const char* trigger_skill_desc = skill_get_desc(spell.trigger_skill);  
			const char* cast_skill_desc = skill_get_desc(spell.id);  
			if (trigger_skill_desc && cast_skill_desc) {  
				std::string flags_desc = "";  
				if (spell.flag & AUTOSPELL_FORCE_TARGET) {  
					flags_desc += " (cast on self)";  // Note: For OnSkill, FORCE_TARGET means cast on self  
				} else {  
					flags_desc += " (cast on target)";  
				}  
				if (spell.flag & AUTOSPELL_FORCE_RANDOM_LEVEL) {  
					flags_desc += " (random level 1-" + std::to_string(spell.lv) + ")";  
				}  
				  
				sprintf(output, "   -> AutoSpell on %s: %s Lv.%d (%s%%)%s",   
						trigger_skill_desc, cast_skill_desc, spell.lv,  
						rathena::util::insert_comma(spell.rate / 10).c_str(),  
						flags_desc.c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
				autospell_found = true;  
			}  
		}  
	}  
	  
	if (!autospell_found) {  
		clif_displaymessage(fd, "   -> No AutoSpell bonuses found");  
	}
}

ACMD_FUNC(autospellbonuses) {    
    nullpo_retr(-1, sd);    
    atcommand_autospellbonuses_sub(fd, sd);    
    return 0;    
}

/*==========================================      
* @miscbonuses    
* shows misc bonuses of players    
*------------------------------------------*/      
void atcommand_miscbonuses_sub(int fd, class map_session_data *sd) {      
    char output[CHAT_SIZE_MAX];

	clif_displaymessage(fd, "=============================");
	clif_displaymessage(fd, "MISC BONUSES");      
	clif_displaymessage(fd, "=============================");
	
	// HP/SP Drain Section  
	clif_displaymessage(fd, " * HP/SP Drain");  
	bool misc_found = false;	
	  
	// bHPDrainValue - Flat HP drain on normal attack (stored per class)  
	if (sd->right_weapon.hp_drain_class[CLASS_NORMAL] != 0 || sd->right_weapon.hp_drain_class[CLASS_BOSS] != 0) {  
		int normal_drain = sd->right_weapon.hp_drain_class[CLASS_NORMAL];  
		int boss_drain = sd->right_weapon.hp_drain_class[CLASS_BOSS];  
		  
		if (normal_drain == boss_drain && normal_drain != 0) {  
			sprintf(output, "   -> HP Drain Value: +%s HP per attack",   
					rathena::util::insert_comma(normal_drain).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			misc_found = true;  
		} else {  
			if (normal_drain != 0) {  
				sprintf(output, "   -> HP Drain Value (Normal): +%s HP per attack",   
						rathena::util::insert_comma(normal_drain).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
				misc_found = true;  
			}  
			if (boss_drain != 0) {  
				sprintf(output, "   -> HP Drain Value (Boss): +%s HP per attack",   
						rathena::util::insert_comma(boss_drain).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
				misc_found = true;  
			}  
		}  
	}  
	  
	// bSPDrainValue - Flat SP drain on normal attack (stored per class)  
	if (sd->right_weapon.sp_drain_class[CLASS_NORMAL] != 0 || sd->right_weapon.sp_drain_class[CLASS_BOSS] != 0) {  
		int normal_drain = sd->right_weapon.sp_drain_class[CLASS_NORMAL];  
		int boss_drain = sd->right_weapon.sp_drain_class[CLASS_BOSS];  
		  
		if (normal_drain == boss_drain && normal_drain != 0) {  
			sprintf(output, "   -> SP Drain Value: +%s SP per attack",   
					rathena::util::insert_comma(normal_drain).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			misc_found = true;  
		} else {  
			if (normal_drain != 0) {  
				sprintf(output, "   -> SP Drain Value (Normal): +%s SP per attack",   
						rathena::util::insert_comma(normal_drain).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
				misc_found = true;  
			}  
			if (boss_drain != 0) {  
				sprintf(output, "   -> SP Drain Value (Boss): +%s SP per attack",   
						rathena::util::insert_comma(boss_drain).c_str());  
				clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
				misc_found = true;  
			}  
		}  
	}  
	  
	// bHPDrainValueRace - HP drain vs specific races  
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->right_weapon.hp_drain_race[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> HP Drain vs %s race: +%s HP per attack",   
					race_name.c_str(),  
					rathena::util::insert_comma(sd->right_weapon.hp_drain_race[race]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			misc_found = true;  
		}  
	}  
	  
	// bSPDrainValueRace - SP drain vs specific races  
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->right_weapon.sp_drain_race[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> SP Drain vs %s race: +%s SP per attack",   
					race_name.c_str(),  
					rathena::util::insert_comma(sd->right_weapon.sp_drain_race[race]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			misc_found = true;  
		}  
	}  
	  
	// bHpDrainValueClass - HP drain vs specific classes (note: different from CLASS_NORMAL/BOSS above)  
	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {  
		if (class_type == CLASS_NORMAL || class_type == CLASS_BOSS) continue; // Skip already handled  
		if (sd->right_weapon.hp_drain_class[class_type] != 0) {  
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> HP Drain vs %s class: +%s HP per attack",   
					class_name.c_str(),  
					rathena::util::insert_comma(sd->right_weapon.hp_drain_class[class_type]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			misc_found = true;  
		}  
	}  
	  
	// bSpDrainValueClass - SP drain vs specific classes  
	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {  
		if (class_type == CLASS_NORMAL || class_type == CLASS_BOSS) continue; // Skip already handled  
		if (sd->right_weapon.sp_drain_class[class_type] != 0) {  
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> SP Drain vs %s class: +%s SP per attack",   
					class_name.c_str(),  
					rathena::util::insert_comma(sd->right_weapon.sp_drain_class[class_type]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			misc_found = true;  
		}  
	}  
	  
	// bHPDrainRate - Percentage-based HP drain  
	if (sd->right_weapon.hp_drain_rate.rate != 0) {  
		sprintf(output, "   -> HP Drain Rate: %.1f%% chance to drain %s%% of damage as HP",   
				sd->right_weapon.hp_drain_rate.rate / 10.0,  
				rathena::util::insert_comma(sd->right_weapon.hp_drain_rate.per).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_found = true;  
	}  
	  
	// bSPDrainRate - Percentage-based SP drain  
	if (sd->right_weapon.sp_drain_rate.rate != 0) {  
		sprintf(output, "   -> SP Drain Rate: %.1f%% chance to drain %s%% of damage as SP",   
				sd->right_weapon.sp_drain_rate.rate / 10.0,  
				rathena::util::insert_comma(sd->right_weapon.sp_drain_rate.per).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_found = true;  
	}  

	if (!misc_found) {  
		clif_displaymessage(fd, "   -> No HP/SP Drain bonuses found");  
	}

	clif_displaymessage(fd, " * HP/SP Vanish");  
	bool vanish_found = false;  
	  
	// bHPVanishRate - HP vanish on normal attack  
	for (const auto &vanish : sd->hp_vanish) {  
		if (vanish.rate != 0) {  
			sprintf(output, "   -> HP Vanish chance: %.2f%% (reduces %.2f%% HP)",  
					vanish.rate / 10.0, vanish.per / 1.0);  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			vanish_found = true;  
		}  
	}  
	  
	// bSPVanishRate - SP vanish on normal attack    
	for (const auto &vanish : sd->sp_vanish) {  
		if (vanish.rate != 0) {  
			sprintf(output, "   -> SP Vanish chance: %.2f%% (reduces %.2f%% SP)",  
					vanish.rate / 10.0, vanish.per / 1.0);  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			vanish_found = true;  
		}  
	}  
	  
	// bHPVanishRaceRate - HP vanish vs specific races  
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->hp_vanish_race[race].rate != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> HP Vanish vs %s race: %.2f%% chance (reduces %.2f%% HP)",  
					race_name.c_str(), sd->hp_vanish_race[race].rate / 10.0,   
					sd->hp_vanish_race[race].per / 1.0);  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			vanish_found = true;  
		}  
	}  
	  
	// bSPVanishRaceRate - SP vanish vs specific races  
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->sp_vanish_race[race].rate != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> SP Vanish vs %s race: %.2f%% chance (reduces %.2f%% SP)",  
					race_name.c_str(), sd->sp_vanish_race[race].rate / 10.0,  
					sd->sp_vanish_race[race].per / 1.0);  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			vanish_found = true;  
		}  
	}  
	  
	// bStateNoRecoverRace - No recovery state vs races  
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->norecover_state_race[race].rate != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> No Recovery vs %s race: %.2f%% chance for %d ms",  
					race_name.c_str(), sd->norecover_state_race[race].rate / 100.0,  
					sd->norecover_state_race[race].tick);  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			vanish_found = true;  
		}  
	}  
	  
	if (!vanish_found) {  
		clif_displaymessage(fd, "   -> No HP/SP vanish bonuses found");  
	}

	clif_displaymessage(fd, " * HP/SP Gain");  
	bool gain_found = false;  
	  
	// bHPGainValue - HP gain on melee kill  
	if (sd->bonus.hp_gain_value != 0) {  
		sprintf(output, "   -> HP gain on melee kill: +%s",   
				rathena::util::insert_comma(sd->bonus.hp_gain_value).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		gain_found = true;  
	}  
	  
	// bSPGainValue - SP gain on melee kill  
	if (sd->bonus.sp_gain_value != 0) {  
		sprintf(output, "   -> SP gain on melee kill: +%s",   
				rathena::util::insert_comma(sd->bonus.sp_gain_value).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		gain_found = true;  
	}  
	  
	// bSPGainRace - SP gain on melee kill by race  
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->indexed_bonus.sp_gain_race[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> SP gain on melee kill vs %s race: +%s",   
					race_name.c_str(),  
					rathena::util::insert_comma(sd->indexed_bonus.sp_gain_race[race]).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			gain_found = true;  
		}  
	}  
	  
	// bLongHPGainValue - HP gain on ranged kill  
	if (sd->bonus.long_hp_gain_value != 0) {  
		sprintf(output, "   -> HP gain on ranged kill: +%s",   
				rathena::util::insert_comma(sd->bonus.long_hp_gain_value).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		gain_found = true;  
	}  
	  
	// bLongSPGainValue - SP gain on ranged kill  
	if (sd->bonus.long_sp_gain_value != 0) {  
		sprintf(output, "   -> SP gain on ranged kill: +%s",   
				rathena::util::insert_comma(sd->bonus.long_sp_gain_value).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		gain_found = true;  
	}  
	  
	// bMagicHPGainValue - HP gain on magic kill  
	if (sd->bonus.magic_hp_gain_value != 0) {  
		sprintf(output, "   -> HP gain on magic kill: +%s",   
				rathena::util::insert_comma(sd->bonus.magic_hp_gain_value).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		gain_found = true;  
	}  
	  
	// bMagicSPGainValue - SP gain on magic kill  
	if (sd->bonus.magic_sp_gain_value != 0) {  
		sprintf(output, "   -> SP gain on magic kill: +%s",   
				rathena::util::insert_comma(sd->bonus.magic_sp_gain_value).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		gain_found = true;  
	}  
	  
	if (!gain_found) {  
		clif_displaymessage(fd, "   -> No HP/SP gain bonuses found");  
	}

	clif_displaymessage(fd, " * Damage Return");  
	bool damage_return_found = false;  
	  
	// bShortWeaponDamageReturn - Reflect melee damage  
	if (sd->bonus.short_weapon_damage_return != 0) {  
		sprintf(output, "   -> Melee Damage Reflection: %s%%",   
				rathena::util::insert_comma(sd->bonus.short_weapon_damage_return).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_return_found = true;  
	}  
	  
	// bLongWeaponDamageReturn - Reflect ranged damage  
	if (sd->bonus.long_weapon_damage_return != 0) {  
		sprintf(output, "   -> Ranged Damage Reflection: %s%%",   
				rathena::util::insert_comma(sd->bonus.long_weapon_damage_return).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_return_found = true;  
	}  
	  
	// bMagicDamageReturn - Reflect magic damage  
	if (sd->bonus.magic_damage_return != 0) {  
		sprintf(output, "   -> Magic Damage Reflection: %s%%",   
				rathena::util::insert_comma(sd->bonus.magic_damage_return).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_return_found = true;  
	}  
	  
	// bReduceDamageReturn - Reduce reflected damage  
	if (sd->bonus.reduce_damage_return != 0) {  
		sprintf(output, "   -> Reflected Damage Reduction: %s%%",   
				rathena::util::insert_comma(sd->bonus.reduce_damage_return).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		damage_return_found = true;  
	}  
	  
	if (!damage_return_found) {  
		clif_displaymessage(fd, "   -> No damage return bonuses found");  
	}

	clif_displaymessage(fd, " * Strip/Break Equipment");  
	bool strip_break_found = false;  
	  
	// Unstripable Equipment bonuses  
	if (sd->bonus.unstripable_equip & EQP_WEAPON) {  
		sprintf(output, "   -> Weapon: Cannot be stripped");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	if (sd->bonus.unstripable_equip & EQP_ARMOR) {  
		sprintf(output, "   -> Armor: Cannot be stripped");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	if (sd->bonus.unstripable_equip & EQP_HELM) {  
		sprintf(output, "   -> Helm: Cannot be stripped");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	if (sd->bonus.unstripable_equip & EQP_SHIELD) {  
		sprintf(output, "   -> Shield: Cannot be stripped");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	// Unbreakable Equipment bonuses  
	if (sd->bonus.unbreakable_equip & EQP_WEAPON) {  
		sprintf(output, "   -> Weapon: Cannot be broken");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	if (sd->bonus.unbreakable_equip & EQP_ARMOR) {  
		sprintf(output, "   -> Armor: Cannot be broken");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	if (sd->bonus.unbreakable_equip & EQP_HELM) {  
		sprintf(output, "   -> Helm: Cannot be broken");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	if (sd->bonus.unbreakable_equip & EQP_SHIELD) {  
		sprintf(output, "   -> Shield: Cannot be broken");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	if (sd->bonus.unbreakable_equip & EQP_GARMENT) {  
		sprintf(output, "   -> Garment: Cannot be broken");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	if (sd->bonus.unbreakable_equip & EQP_SHOES) {  
		sprintf(output, "   -> Shoes: Cannot be broken");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	// bUnbreakable - General break resistance  
	if (sd->bonus.unbreakable != 0) {  
		sprintf(output, "   -> Equipment break resistance: %s%%",   
				rathena::util::insert_comma(sd->bonus.unbreakable).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	// bBreakWeaponRate - Break enemy weapon chance  
	if (sd->bonus.break_weapon_rate != 0) {  
		sprintf(output, "   -> Break enemy weapon chance: %.2f%%",   
				sd->bonus.break_weapon_rate / 100.0);  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	// bBreakArmorRate - Break enemy armor chance  
	if (sd->bonus.break_armor_rate != 0) {  
		sprintf(output, "   -> Break enemy armor chance: %.2f%%",   
				sd->bonus.break_armor_rate / 100.0);  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		strip_break_found = true;  
	}  
	  
	if (!strip_break_found) {  
		clif_displaymessage(fd, "   -> No strip/break equipment bonuses found");  
	}

	clif_displaymessage(fd, " * Monster Drops");  
	bool drops_found = false;  
	  
	// bDropAddRace - Drop rate bonus vs Race  
	for (int race = 0; race < RC_MAX; race++) {  
		if (sd->indexed_bonus.dropaddrace[race] != 0) {  
			std::string race_name = get_race_name(race);  
			sprintf(output, "   -> Drop rate vs %s race: %s%s%%",   
					race_name.c_str(), sd->indexed_bonus.dropaddrace[race] > 0 ? "+" : "",  
					rathena::util::insert_comma(sd->indexed_bonus.dropaddrace[race]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.dropaddrace[race] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			drops_found = true;  
		}  
	}  
	  
	// bDropAddClass - Drop rate bonus vs Class  
	for (int class_type = 0; class_type < CLASS_MAX; class_type++) {  
		if (sd->indexed_bonus.dropaddclass[class_type] != 0) {  
			std::string class_name = get_class_name(class_type);  
			sprintf(output, "   -> Drop rate vs %s class: %s%s%%",   
					class_name.c_str(), sd->indexed_bonus.dropaddclass[class_type] > 0 ? "+" : "",  
					rathena::util::insert_comma(sd->indexed_bonus.dropaddclass[class_type]).c_str());  
			clif_messagecolor(sd, sd->indexed_bonus.dropaddclass[class_type] > 0 ? color_table[COLOR_WHITE] : color_table[COLOR_RED], output, false, SELF);  
			drops_found = true;  
		}  
	}  
	  
	// bAddMonsterDropItem, bAddMonsterIdDropItem, bAddClassDropItem, bAddMonsterDropItemGroup, bAddClassDropItemGroup  
	for (const auto &drop : sd->add_drop) {    
		if (drop.rate != 0) {    
			std::string drop_desc;    
			const char* item_name = nullptr;  
				
			// Determine drop type and target    
			if (drop.nameid > 0) {    
				std::shared_ptr<item_data> item = item_db.find(drop.nameid);  
				if (item != nullptr) {    
					item_name = item->ename.c_str();    
				} else {    
					item_name = "Unknown Item";    
				}  
				drop_desc = std::string("Item ") + item_name;    
			} else if (drop.group > 0) {    
				drop_desc = "Item Group " + std::to_string(drop.group);    
			} else {    
				continue; // Skip invalid entries    
			}    
				
			// Determine target condition    
			std::string target_desc;    
			if (drop.race < RC_NONE_) {    
				// Monster ID (negative race value)    
				std::shared_ptr<s_mob_db> mob = mob_db.find(-drop.race);    
				const char* monster_name = (mob != nullptr) ? mob->jname.c_str() : "Unknown Monster";    
				target_desc = " when killing " + std::string(monster_name);    
			} else if (drop.race != RC_NONE_ && drop.race != RC_ALL) {    
				// Specific race    
				std::string race_name = get_race_name(drop.race);    
				target_desc = " when killing " + race_name + " race monsters";    
			} else if (drop.class_ != CLASS_NONE && drop.class_ != CLASS_ALL) {    
				// Specific class    
				std::string class_name = get_class_name(drop.class_);    
				target_desc = " when killing " + class_name + " class monsters";    
			} else {    
				// All monsters    
				target_desc = " when killing any monster";    
			}    
				
			// Display the drop bonus    
			if (drop.rate < 0) {    
				sprintf(output, "   -> %s drop chance%s: Formula-based (rate * mob_level/10)",     
						drop_desc.c_str(), target_desc.c_str());    
			} else {    
				sprintf(output, "   -> %s drop chance%s: %.2f%%",     
						drop_desc.c_str(), target_desc.c_str(), drop.rate / 100.0);    
			}    
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);    
			drops_found = true;    
		}    
	}  
	  
	// bGetZenyNum - Zeny gain (highest value only)  
	if (sd->bonus.get_zeny_rate != 0 && sd->bonus.get_zeny_num != 0) {  
		if (sd->bonus.get_zeny_num > 0) {  
			sprintf(output, "   -> Zeny gain on kill: 1~%s zeny at %.2f%% chance (highest only)",   
					rathena::util::insert_comma(sd->bonus.get_zeny_num).c_str(),  
					sd->bonus.get_zeny_rate / 100.0);  
		} else {  
			sprintf(output, "   -> Zeny gain on kill: Formula-based (mob_level * %s) at %.2f%% chance (highest only)",   
					rathena::util::insert_comma(-sd->bonus.get_zeny_num).c_str(),  
					sd->bonus.get_zeny_rate / 100.0);  
		}  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		drops_found = true;  
	}  
	  
	if (!drops_found) {  
		clif_displaymessage(fd, "   -> No monster drop bonuses found");  
	}

	clif_displaymessage(fd, " * Misc Effects");  
	bool misc_effects_found = false;  
	  
	// bDoubleRate - Double Attack probability (highest value applied)  
	if (sd->bonus.double_rate != 0) {  
		sprintf(output, "   -> Double Attack Rate: %s%% (highest value applied)",   
				rathena::util::insert_comma(sd->bonus.double_rate).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bDoubleAddRate - Double Attack probability additive  
	if (sd->bonus.double_add_rate != 0) {  
		sprintf(output, "   -> Double Attack Add Rate: +%s%%",   
				rathena::util::insert_comma(sd->bonus.double_add_rate).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bSplashRange - Splash attack radius (highest value applied)  
	if (sd->bonus.splash_range != 0) {  
		sprintf(output, "   -> Splash Range: %s (highest value applied)",   
				rathena::util::insert_comma(sd->bonus.splash_range).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bSplashAddRange - Splash attack radius additive  
	if (sd->bonus.splash_add_range != 0) {  
		sprintf(output, "   -> Splash Add Range: +%s",   
				rathena::util::insert_comma(sd->bonus.splash_add_range).c_str());  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bAddSkillBlow - Skill knockback  
	for (const auto &blow : sd->skillblown) {  
		if (blow.val != 0) {  
			const char* skill_desc = skill_get_desc(blow.id);  
			if (!skill_desc) skill_desc = "Unknown Skill";  
			  
			sprintf(output, "   -> Knockback on %s: %s cells",   
					skill_desc, rathena::util::insert_comma(blow.val).c_str());  
			clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
			misc_effects_found = true;  
		}  
	}  
	  
	// bNoKnockback - Immunity to knockback  
	if (sd->special_state.no_knockback) {  
		sprintf(output, "   -> No Knockback: Enabled");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bNoGemStone - No gemstone requirement  
	if (sd->special_state.no_gemstone) {  
		sprintf(output, "   -> No Gemstone: Enabled");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bIntravision - See hidden players/mobs  
	if (sd->special_state.intravision) {  
		sprintf(output, "   -> Intravision: Enabled");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bPerfectHide - Undetectable when hidden  
	if (sd->special_state.perfect_hiding) {  
		sprintf(output, "   -> Perfect Hide: Enabled");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bRestartFullRecover - Full heal on revival  
	if (sd->special_state.restart_full_recover) {  
		sprintf(output, "   -> Restart Full Recover: Enabled");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bClassChange - Monster class change chance  
	if (sd->bonus.classchange != 0) {  
		sprintf(output, "   -> Class Change Chance: %.2f%%",   
				sd->bonus.classchange / 100.0);  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bAddStealRate - Steal success rate increase  
	if (sd->bonus.add_steal_rate != 0) {  
		sprintf(output, "   -> Steal Rate Increase: %.2f%%",   
				sd->bonus.add_steal_rate / 100.0);  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bNoMadoFuel - No mado fuel requirement  
	if (sd->special_state.no_mado_fuel) {  
		sprintf(output, "   -> No Mado Fuel: Enabled");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	// bNoWalkDelay - Infinite endure  
	if (sd->special_state.no_walk_delay) {  
		sprintf(output, "   -> No Walk Delay (Infinite Endure): Enabled");  
		clif_messagecolor(sd, color_table[COLOR_WHITE], output, false, SELF);  
		misc_effects_found = true;  
	}  
	  
	if (!misc_effects_found) {  
		clif_displaymessage(fd, "   -> No misc effects bonuses found");  
	}

}
ACMD_FUNC(miscbonuses) {    
    nullpo_retr(-1, sd);    
    atcommand_miscbonuses_sub(fd, sd);    
    return 0;    
}
  
/*==========================================  
* @statbonus  
*------------------------------------------*/  
ACMD_FUNC(statbonus) {  
    nullpo_retr(-1, sd);  
	atcommand_basicbonuses_sub(fd, sd);
	atcommand_extendedbonuses_sub(fd, sd);
	atcommand_gspecificbonuses_sub(fd, sd);
	atcommand_srelatedbonuses_sub(fd, sd);
    atcommand_autospellbonuses_sub(fd, sd);
	atcommand_miscbonuses_sub(fd, sd);
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
 * Show Monster DB Info   v 2.0
 * originally by [Lupus]
 * edited by [Louis T Steinhil]
 *------------------------------------------*/
ACMD_FUNC(mobinfo2)
{
	unsigned char msize[SZ_ALL][7] = { "Small", "Medium", "Large" };
	unsigned char mrace[RC_ALL][11] = { "Formless", "Undead", "Beast", "Plant", "Insect", "Fish", "Demon", "Demi-Human", "Angel", "Dragon", "Player" };
	unsigned char melement[ELE_ALL][8] = { "Neutral", "Water", "Earth", "Fire", "Wind", "Poison", "Holy", "Dark", "Ghost", "Undead" };
	char atcmd_output2[CHAT_SIZE_MAX];
	uint16 mob_ids[MAX_SEARCH], count;
	uint16 i;

	memset(atcmd_output, '\0', sizeof(atcmd_output));
	memset(atcmd_output2, '\0', sizeof(atcmd_output2));

	if (!message || !*message) {
		clif_displaymessage(fd, msg_txt(sd,1239)); // Please enter a monster name/ID (usage: @mobinfo <monster_name_or_monster_ID>).
		return -1;
	}

	// If monster identifier/name argument is a name
	i = util::strtoint32def(message);
	if (i != 0 && (i = mobdb_checkid(i)))
	{
		mob_ids[0] = i;
		count = 1;
	} else
		count = mobdb_searchname_array(message, mob_ids, MAX_SEARCH);

	if (!count) {
		clif_displaymessage(fd, msg_txt(sd,40)); // Invalid monster ID or name.
		return -1;
	}

	if (count >= MAX_SEARCH) {
		sprintf(atcmd_output, msg_txt(sd,269), MAX_SEARCH); // Displaying first %d matches
		clif_displaymessage(fd, atcmd_output);
		count = MAX_SEARCH;
	}
	for (uint16 k = 0; k < count; k++) {
		std::shared_ptr<s_mob_db> mob = mob_db.find(mob_ids[k]);

		if (mob == nullptr)
			continue;

		t_exp base_exp = mob->base_exp;
		t_exp job_exp = mob->job_exp;

		if (pc_isvip(sd)) { // Display EXP rate increase for VIP
			base_exp += (base_exp * battle_config.vip_base_exp_increase) / 100;
			job_exp += (job_exp * battle_config.vip_job_exp_increase) / 100;
		}
#ifdef RENEWAL_EXP
		if( battle_config.atcommand_mobinfo_type ) {
			int32 penalty = pc_level_penalty_mod( sd, PENALTY_EXP, mob );

			base_exp = base_exp * penalty / 100;
			job_exp = job_exp * penalty / 100;
		}
#endif
		// stats
		if( mob->get_bosstype() == BOSSTYPE_MVP )
			sprintf(atcmd_output, "MVP Monster: %s | %s | %s (%d)", mob->name.c_str(), mob->jname.c_str(), mob->sprite.c_str(), mob->id);
		else
			sprintf(atcmd_output, "Monster: %s | %s | %s (%d)", mob->name.c_str(), mob->jname.c_str(), mob->sprite.c_str(), mob->id);
		clif_displaymessage(fd, atcmd_output);

		std::string formatted_lv = rathena::util::insert_comma(mob->lv);
		std::string formatted_hp = rathena::util::insert_comma(mob->status.max_hp);
		std::string formatted_base_exp = rathena::util::insert_comma(static_cast<int32>(base_exp));
		std::string formatted_job_exp = rathena::util::insert_comma(static_cast<int32>(job_exp));
		std::string formatted_hit = rathena::util::insert_comma(MOB_HIT(mob));
		std::string formatted_flee = rathena::util::insert_comma(MOB_FLEE(mob));

		sprintf(atcmd_output, "Lv: %s  HP: %s  Base EXP: %s  Job EXP: %s  HIT: %s  FLEE: %s", formatted_lv.c_str(), formatted_hp.c_str(), formatted_base_exp.c_str(), formatted_job_exp.c_str(), formatted_hit.c_str(), formatted_flee.c_str());    
		clif_displaymessage(fd, atcmd_output);

		std::string formatted_def = rathena::util::insert_comma(mob->status.def);
		std::string formatted_mdef = rathena::util::insert_comma(mob->status.mdef);
		std::string formatted_str = rathena::util::insert_comma(mob->status.str);
		std::string formatted_agi = rathena::util::insert_comma(mob->status.agi);
		std::string formatted_vit = rathena::util::insert_comma(mob->status.vit);
		std::string formatted_int = rathena::util::insert_comma(mob->status.int_);
		std::string formatted_dex = rathena::util::insert_comma(mob->status.dex);
		std::string formatted_luk = rathena::util::insert_comma(mob->status.luk);

		sprintf(atcmd_output, "DEF: %s  MDEF: %s  STR: %s  AGI: %s  VIT: %s  INT: %s  DEX: %s  LUK: %s", formatted_def.c_str(), formatted_mdef.c_str(), formatted_str.c_str(), formatted_agi.c_str(), formatted_vit.c_str(), formatted_int.c_str(), formatted_dex.c_str(), formatted_luk.c_str());
		clif_displaymessage(fd, atcmd_output);

		std::string formatted_atk1 = rathena::util::insert_comma(mob->status.batk + mob->status.rhw.atk);
		std::string formatted_atk2 = rathena::util::insert_comma(mob->status.batk + mob->status.rhw.atk2);
		std::string formatted_range1 = rathena::util::insert_comma(mob->status.rhw.range);
		std::string formatted_range2 = rathena::util::insert_comma(mob->range2);
		std::string formatted_range3 = rathena::util::insert_comma(mob->range3);
		std::string formatted_ele_lv = rathena::util::insert_comma(mob->status.ele_lv);

		sprintf(atcmd_output, "ATK: %s~%s  Range: %s~%s~%s  Size: %s  Race: %s  Element: %s (Lv: %s)", formatted_atk1.c_str(), formatted_atk2.c_str(), formatted_range1.c_str(), formatted_range2.c_str(), formatted_range3.c_str(), msize[mob->status.size], mrace[mob->status.race], melement[mob->status.def_ele], formatted_ele_lv.c_str());
		clif_displaymessage(fd, atcmd_output);

#ifdef RENEWAL    
		std::string formatted_res = rathena::util::insert_comma(mob->status.res);
		std::string formatted_mres = rathena::util::insert_comma(mob->status.mres);

		sprintf(atcmd_output, "RES: %s  MRES: %s", formatted_res.c_str(), formatted_mres.c_str());      
		clif_displaymessage(fd, atcmd_output);
#endif
		// drops
		clif_displaymessage(fd, msg_txt(sd,1245)); //  Drops:
		if( mob->dropitem.empty() ){
			clif_displaymessage(fd, msg_txt(sd,1246)); // This monster has no drops.
		}else{
			int32 drop_modifier = 100;
#ifdef RENEWAL_DROP
			if( battle_config.atcommand_mobinfo_type ){
				drop_modifier = pc_level_penalty_mod( sd, PENALTY_DROP, mob );
			}
#endif

			for( const std::shared_ptr<s_mob_drop>& entry : mob->dropitem ){
				if (entry->nameid == 0 || entry->rate < 1)
					continue;

				std::shared_ptr<item_data> id = item_db.find(entry->nameid);

				if (id == nullptr)
					continue;

				int32 droprate = mob_getdroprate( sd, mob, entry->rate, drop_modifier );
				sprintf(atcmd_output, " - %s %02.02f%%", item_db.create_item_link(id).c_str(), (float)droprate / 100);
				clif_displaymessage(fd, atcmd_output);
			}
		}

		// mvp
		if( mob->get_bosstype() == BOSSTYPE_MVP ){

			std::string formatted_mexp = rathena::util::insert_comma(static_cast<int32>(mob->mexp));  

			sprintf(atcmd_output, "MVP Bonus EXP: %s", formatted_mexp.c_str());
			clif_displaymessage(fd, atcmd_output);
			clif_displaymessage(fd, msg_txt(sd,1248)); //  MVP drops:

			if( mob->mvpitem.empty() ){
				clif_displaymessage(fd, msg_txt(sd,1249)); // This monster has no MVP drops.
			}else{
				float mvpremain = 100.0; //Remaining drop chance for official mvp drop mode

				for( const std::shared_ptr<s_mob_drop>& entry : mob->mvpitem ){
					if (entry->nameid == 0)
						continue;

					std::shared_ptr<item_data> id = item_db.find(entry->nameid);

					if (id == nullptr)
						continue;

					//Because if there are 3 MVP drops at 50%, the first has a chance of 50%, the second 25% and the third 12.5%
					float mvppercent = (float)entry->rate * mvpremain / 10000.0f;
					if(battle_config.item_drop_mvp_mode == 0) {
						mvpremain -= mvppercent;
					}
					if (mvppercent > 0) {
						sprintf(atcmd_output, " - %s %02.02f%%", item_db.create_item_link(id).c_str(), mvppercent);
						clif_displaymessage(fd, atcmd_output);
					}
				}
			}
		}
	}
	return 0;
}

/*==========================================
 * Show who drops the item.   v 2.0
 * edited by [Louis T Steinhil]
 *------------------------------------------*/
ACMD_FUNC(whodrops2)
{
	if (!message || !*message) {
		clif_displaymessage(fd, msg_txt(sd,1284)); // Please enter item name/ID (usage: @whodrops <item name/ID>).
		return -1;
	}

	std::map<t_itemid, std::shared_ptr<item_data>> item_array = {};
	uint16 count = 1;
	t_itemid itemid = strtoul(message, nullptr, 10);

	if (itemid == 0) // Entered a string
		count = itemdb_searchname_array(item_array, MAX_SEARCH, message);
	else {
		if ((item_array[0] = item_db.find(itemid)) == nullptr)
			count = 0;
	}

	if (!count) {
		clif_displaymessage(fd, msg_txt(sd,19));	// Invalid item ID or name.
		return -1;
	}

	if (count == MAX_SEARCH) {
		sprintf(atcmd_output, msg_txt(sd,269), MAX_SEARCH); // Displaying first %d matches
		clif_displaymessage(fd, atcmd_output);
	}
	for (const auto &result : item_array) {
		std::shared_ptr<item_data> id = result.second;

		sprintf(atcmd_output, msg_txt(sd,1285), item_db.create_item_link( id ).c_str(), id->nameid); // Item: '%s' (ID:%u)
		clif_displaymessage(fd, atcmd_output);

		if (id->mob[0].chance == 0) {
			strcpy(atcmd_output, msg_txt(sd,1286)); //  - Item is not dropped by mobs.
			clif_displaymessage(fd, atcmd_output);
		} else {
			sprintf(atcmd_output, msg_txt(sd,1287), MAX_SEARCH); //  - Common mobs with highest drop chance (only max %d are listed):
			clif_displaymessage(fd, atcmd_output);

			for (uint16 j=0; j < MAX_SEARCH && id->mob[j].chance > 0; j++) {  
				std::shared_ptr<s_mob_db> mob = mob_db.find(id->mob[j].id);  
				if(!mob) continue;  
		  
				int32 drop_modifier = 100;  
#ifdef RENEWAL_DROP  
				if( battle_config.atcommand_mobinfo_type ){  
					drop_modifier = pc_level_penalty_mod( sd, PENALTY_DROP, mob );  
				}  
#endif

				for( const std::shared_ptr<s_mob_drop>& entry : mob->dropitem ){  
					if (entry->nameid != id->nameid)  
						continue;  
		  
					if (entry->rate < 1)  
						continue;  
		  
					int32 droprate = mob_getdroprate( sd, mob, entry->rate, drop_modifier );  
		  
					if (pc_isvip(sd))
						droprate += (droprate * battle_config.vip_drop_increase) / 100;  
		  
					sprintf(atcmd_output, "- %s (%d): %02.02f%%", mob->jname.c_str(), id->mob[j].id, (float)droprate / 100);  
					clif_displaymessage(fd, atcmd_output);  
					break;
				}  
			}
		}
		sprintf(atcmd_output, "Map drops:");
		clif_displaymessage(fd, atcmd_output);

		std::map<std::pair<uint16, double>, std::vector<std::string>> map_drop_groups;
		bool found_map_drops = false;

		for (const auto& map_entry : map_drop_db) {
			std::shared_ptr<s_map_drops> mapdrops = map_entry.second;
			std::string map_name = map_mapid2mapname(map_entry.first);
			std::string display_name = map_name;

			if (map_name.find("@") != std::string::npos) {
				for (const auto& instance_entry : instance_db) {
					std::shared_ptr<s_instance_db> instance = instance_entry.second;
					if (strcmp(map_mapid2mapname(instance->enter.map), map_name.c_str()) == 0) {
						display_name = instance->name;
						break;
					}
					for (const auto& additional_map : instance->maplist) {
						if (strcmp(map_mapid2mapname(additional_map), map_name.c_str()) == 0) {
							display_name = instance->name;
							break;
						}
					}
					if (display_name != map_name) break;
				}
			}

			for (const auto& global_drop : mapdrops->globals) {
				if (global_drop.second->nameid == id->nameid) {
					double rate_percent = (global_drop.second->rate * 100.0) / 100000.0;
					std::pair<uint16, double> key = std::make_pair(0, rate_percent);
					map_drop_groups[key].push_back(display_name);
					found_map_drops = true;
				}
			}

			for (const auto& specific_entry : mapdrops->specific) {
				uint16 mob_id = specific_entry.first;
				for (const auto& drop : specific_entry.second) {
					if (drop.second->nameid == id->nameid) {
						double rate_percent = (drop.second->rate * 100.0) / 100000.0;
						std::pair<uint16, double> key = std::make_pair(mob_id, rate_percent);
						map_drop_groups[key].push_back(display_name);
						found_map_drops = true;
					}
				}
			}
		}

		if (!found_map_drops) {
			sprintf(atcmd_output, " - Item is not dropped by map-specific drops.");
			clif_displaymessage(fd, atcmd_output);
		} else {
			for (const auto& group : map_drop_groups) {
				uint16 mob_id = group.first.first;
				double rate = group.first.second;
				const std::vector<std::string>& maps = group.second;

				std::string map_list = "";
				for (size_t i = 0; i < maps.size(); ++i) {
					if (i > 0) map_list += ", ";
					map_list += maps[i];
				}

				if (mob_id == 0) {  
					sprintf(atcmd_output, "- All monsters: %.2f%% - (%s)", rate, map_list.c_str());  
					clif_displaymessage(fd, atcmd_output);  
				} else {  
					std::shared_ptr<s_mob_db> mob = mob_db.find(mob_id);  
					if (mob) {  
						sprintf(atcmd_output, "- %s (%d): %.2f%% - (%s)", mob->jname.c_str(), mob_id, rate, map_list.c_str());  
						clif_displaymessage(fd, atcmd_output);  
					}  
				}
			}  
		}
	}
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

ACMD_FUNC(reloadrunedb)
{
	nullpo_retr(-1, sd);

	rune_db_reload();
	clif_displaymessage(fd, "Rune system has been reloaded");

	return 0;
}
