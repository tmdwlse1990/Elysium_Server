// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom battle config structure
 * Place the definition of custom battle configuration structure members in this file.
 *
 * Make sure that the name you want is not already taken by looking into battle.hpp
 * Do not forget to add initilization logic to battle_config_init.inc
 *
 * Format:
 *	 <datatype> name;
 **/

// autocombat
int32 autocombat_autoheal;
int32 autocombat_autopotion;
int32 autocombat_sittorest;
int32 autocombat_attackskill;
int32 autocombat_buffskill;
int32 autocombat_buffitems;
int32 autocombat_teleport;
int32 autocombat_teleport_mvp;
int32 autocombat_pickup;
int32 autocombat_prefixname;
int32 autocombat_hateffect;
int32 autocombat_pdetection;
int32 autocombat_mselection;
int32 autocombat_mdetection;
int32 autocombat_pickup_delay;
int32 autocombat_move_min;
int32 autocombat_move_max;
int32 autocombat_movetype;
int32 autocombat_askill_delay;
int32 autocombat_bskill_delay;
int32 autocombat_drop_ratio;
int32 autocombat_exp_ratio;
int32 autocombat_allow_town;
int32 autocombat_allow_pvp;
int32 autocombat_allow_gvg;
int32 autocombat_allow_bg;
int32 autocombat_duration_type;
int32 autocombat_timer;
int32 autocombat_ap_per_tick;
int32 autocombat_iplimit;
int32 autocombat_gepardlimit;
// The stat of renewal MVPs (ID 2083-31999)  
int32 mvp_str_rate;  
int32 mvp_agi_rate;  
int32 mvp_vit_rate;  
int32 mvp_int_rate;  
int32 mvp_dex_rate;  
int32 mvp_luk_rate;  
int32 mvp_atk_rate;  
int32 mvp_matk_rate;  
int32 mvp_def_rate;  
int32 mvp_mdef_rate;  
int32 mvp_hit_rate;  
int32 mvp_flee_rate;  
int32 mvp_crit_rate;  
int32 mvp_aspd_rate;
// The stat of renewal normal monsters (ID 2083-31999) 
int32 monster_str_rate;  
int32 monster_agi_rate;  
int32 monster_vit_rate;  
int32 monster_int_rate;  
int32 monster_dex_rate;  
int32 monster_luk_rate;  
int32 monster_atk_rate;  
int32 monster_matk_rate;  
int32 monster_def_rate;  
int32 monster_mdef_rate;  
int32 monster_hit_rate;  
int32 monster_flee_rate;  
int32 monster_crit_rate;  
int32 monster_aspd_rate;
// The stat of renewal MVPs (ID 2083-31999)  
int32 mvp_hp_rate2;  
int32 mvp_str_rate2;  
int32 mvp_agi_rate2;  
int32 mvp_vit_rate2;  
int32 mvp_int_rate2;  
int32 mvp_dex_rate2;  
int32 mvp_luk_rate2;  
int32 mvp_atk_rate2;  
int32 mvp_matk_rate2;  
int32 mvp_def_rate2;  
int32 mvp_mdef_rate2;  
int32 mvp_hit_rate2;  
int32 mvp_flee_rate2;  
int32 mvp_crit_rate2;  
int32 mvp_aspd_rate2;
// The stat of renewal normal monsters (ID 2083-31999)  
int32 monster_hp_rate2;  
int32 monster_str_rate2;  
int32 monster_agi_rate2;  
int32 monster_vit_rate2;  
int32 monster_int_rate2;  
int32 monster_dex_rate2;  
int32 monster_luk_rate2;  
int32 monster_atk_rate2;  
int32 monster_matk_rate2;  
int32 monster_def_rate2;  
int32 monster_mdef_rate2;  
int32 monster_hit_rate2;  
int32 monster_flee_rate2;  
int32 monster_crit_rate2;  
int32 monster_aspd_rate2;
// Soul Link
int32 sl_rogue_strip_bypass;
int32 sl_rogue_strip_item_id;
int32 sl_parry_1h_weapons;
// Skill Enhance
int32 closeconfine_blocks_bodyrelocation;
int32 spiderweb_blocks_bodyrelocation;
int32 anklesnare_blocks_bodyrelocation;
int32 preserve_toggle_mode;
int32 cp_remove_strip;
