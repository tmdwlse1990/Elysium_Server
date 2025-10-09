// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Script Commands
 * Place the body of custom script commands in this file.
 * Format:
 *	BUILDIN_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

//BUILDIN_FUNC(example)
//{
//	ShowInfo("buildin_example: It works!\n");
//	script_pushint(st,1);
//	return 0;
//}

/*==========================================
* Returns infos on a skill.
* ac_getskillinfostring(<id>,<skill id>)
* ac_getskillinfostring(<id>,"<skill name>")
* 0 = Description
*------------------------------------------*/
BUILDIN_FUNC(ac_getskillinfostring)
{
	int skill_id, id;
	TBL_PC* sd;

	if (!script_rid2sd(sd))
		return SCRIPT_CMD_FAILURE;// no player attached, report source

	id = script_getnum(st, 2);
	skill_id = (script_isstring(st, 3) ? skill_name2id(script_getstr(st, 3)) : script_getnum(st, 3));

	switch (id) {
	case 0:	// skill->desc
		std::shared_ptr<s_skill_db> skill = skill_db.find(skill_id);
		if (!skill)
			return SCRIPT_CMD_SUCCESS;
		script_pushstrcopy(st, skill->desc);
		break;
	}

	return SCRIPT_CMD_SUCCESS;
}

/*==========================================
* Start Auto-Buff from rental items
*------------------------------------------*/
BUILDIN_FUNC(autocombat_fromitem) {
	map_session_data* sd;
	t_tick max_duration = 86400;

	if (!script_rid2sd(sd))
		return SCRIPT_CMD_FAILURE;

	t_itemid item_id = script_getnum(st, 2);

	return handleAutocombat_fromitem(sd, item_id, max_duration);
}

/*==========================================
* Returns info of player on autocombat
* ac_getautocombatstring(<id>{,<index>})
* 0 = Auto-heal all desc - Index = skill id
* 1 = HP / SP Potions
* 2 = Ressurrection
* 3 = Auto-Buff skill desc
* 4 = Auto-Sit rest
* 5 = Auto-Buff items desc
* 6 = teleport desc
* 7 = General autocombat_potions state (disable / enable)
* 8 = Return to savepoint when dead (disable / enable)
* 9 = Token of siegfried use for auto res (disable / enable)
* 10 = Auto accept party request
* 11 = Priorize Loot / Fight - 0 Fight - 1 Loot
* 12 = Monster selection
* 13 = Item pickup selection
*------------------------------------------*/
BUILDIN_FUNC(ac_getautocombatstring)
{
	int index = 0, extra_index = 0, id;
	TBL_PC* sd;
	std::ostringstream os_buf;
	os_buf.str("");
	struct party_data* p = nullptr;

	if (!script_rid2sd(sd))
		return SCRIPT_CMD_FAILURE;// no player attached, report source

	id = script_getnum(st, 2);
	index = script_getnum(st, 3);
	extra_index = script_getnum(st, 4);

	switch (id) {
		case 0: // Auto-heal desc
			handle_autocombat_heal(os_buf, index, extra_index, st, sd);
			break;

		case 1: // HP / SP Potions - // potion_menu_list
			handle_autocombat_potions(os_buf, index, st, sd);
			break;

		case 2: // Auto-combat skill desc
			handle_autocombat_attack(os_buf, index, extra_index, st, sd);
			break;

		case 3:// Auto-Buff skill desc
			handle_autocombat_buff(os_buf, index, extra_index, st, sd);
			break;

		case 4: // Auto-Sit to rest
			handle_autocombat_sitrest(os_buf, index, st, sd);
			break;

		case 5:	// Buff items - Used for show the buffitems_menu
			handle_autocombat_items(os_buf, index, st, sd);
			break;

		case 6: // Auto-Tele 
			handle_autocombat_teleport(os_buf, index, st, sd);
			break;

		case 7: // Auto-Potion
			handle_potions(os_buf, sd);
			break;

		case 8: // Auto-Return to Savepoint
			handle_return_to_savepoint(os_buf, sd);
			break;

		case 9: // Auto-Use of Token
			handle_token_of_siegfried(os_buf, sd);
			break;

		case 10: // Auto-Accept of Party
			handle_party_request(os_buf, sd);
			break;

		case 11: // Auto-Loot
			handle_priorize_loot_fight(os_buf, sd);
			break;

		case 12: // Monster desc
			handle_autocombat_monsterselection(os_buf, index, extra_index, st, sd);
			break;

		case 13: // Item pick up
			handle_autocombat_itempickup(os_buf, index, extra_index, st, sd);
			break;
	}

	script_pushstrcopy(st, os_buf.str().c_str());
	return SCRIPT_CMD_SUCCESS;
}

/*==========================================
* Returns info of player on autocombat
* ac_getautocombatint(<id>{,<index>})
* 0 = Auto-heal all desc - Index = skill id
* 1 = HP / SP Potions
* 2 = Active Auto-combat skills
* 3 = Active Auto-Buffs skills
* 4 = Active potions
* 5 = Buff items
* 6 = Return to save point on death
* 7 = Token of Siegfried
* 8 = Auto accept party request
* 9 = Pick up item config
* 10 = Prio item config
* 11 = Stop melee
* 12 = Teleport use fly wing
* 13 = Teleport use skill
* 14 = Sit regen hp
* 15 = Sit regen sp
* 16 = Attack aggressve monster
* 17 = Item pick up selecton size
* 18 = Action on end
*------------------------------------------*/
BUILDIN_FUNC(ac_getautocombatint)
{
	TBL_PC* sd;
	if (!script_rid2sd(sd)) return SCRIPT_CMD_SUCCESS; // No player attached, report source

	int id = script_getnum(st, 2);
	int num = 0; // Variable utilisee pour stocker le resultat
	std::shared_ptr<item_data> item_data;

	const uint32 char_id = sd->status.char_id;

	switch (id) {
		case 0: // Auto-heal
		case 2: // Active Auto-combat skills
		case 3: // Active Auto-Buffs skills
		case 4: // Active potions
		case 5: // Buff items
		case 6: // Return to save point on death
		case 7: // Token of Siegfried
		case 8: // Auto accept party request
		case 9: // Pick up item config
		case 10: // Prio item config
		case 11: // Stop melee
		case 12: // Teleport use fly wing
		case 13: // Teleport use skill
		case 14: // Sit regen hp
		case 15: // Sit regen sp
		case 16: // Attack aggressve monster
		case 17: // Item pick up selecton size
		case 18: // Action on end
			num = handleGetautocombatint(sd, id);
			break;
		case 1: // HP / SP Potions available in inventory
			for (int i = 0; i < MAX_INVENTORY; ++i) {
				item_data = item_db.find(sd->inventory.u.items_inventory[i].nameid);
				if (!item_data) break;
				if (item_data->type == IT_HEALING) ++num;
			}
			break;
		default:
			num = 0;
			break;
	}

	script_pushint(st, num);
	return SCRIPT_CMD_SUCCESS;
}

/*==========================================
* Save info of player on autocombat
* ac_setautocombat(<str>)
* Parse a string with ; as separator
* id;
* id = 0 - autoheal (is_active;skill_id;skill_lv;min_hp)
* id = 1 - autopotion (is_active;item_id;min_hp;min_sp)
* id = 2 - attack skill (is_active;skill_id;skill_lv)
* id = 3 - buff skills (is_active;skill_id;skill_lv)
* id = 4 - teleport flywing
* id = 5 - autocombatitems(is_active; item_id; delay)
* id = 6 - teleport skill
* id = 7 - Melee Attack
* id = 8 - Active autocombat potions
* id = 9 - Return to save point on death
* id = 10 - reset config
* id = 11 - Token of siegfried
* id = 12 - Auto accept parrty request
* id = 13 - Priorize loot / fight
* id = 14 - Sit rest hp
* id = 15 - Sit rest sp
* id = 16 - Teleport min hp
* id = 17 - Teleport delay no mob meet
* id = 18 - Ignore aggressive monster not in the list
* id = 19 - Monster selection list
* id = 20 - Item Pickup configuration
* id = 21 - Item Pickup selection
* id = 22 - Action on end
* id = 23 - Max monster surround
*------------------------------------------*/
BUILDIN_FUNC(ac_setautocombat)
{
	TBL_PC* sd;
	const char delim = ';';
	std::vector<std::string> result;
	std::string item, str;
	int id = -1;

	if (!script_rid2sd(sd))
		return SCRIPT_CMD_SUCCESS; // No player attached

	str = script_getstr(st, 2);
	std::stringstream ss(str);

	while (std::getline(ss, item, delim)) {
		result.push_back(item);
	}

	if (result.empty())
		return SCRIPT_CMD_FAILURE;

	// Extract the ID to process
	id = std::stoi(result[0]);

	switch (id) {
		case 0:
			handleAutoHeal(result, sd);
			break;
		case 1:
			handleAutoPotion(result, sd);
			break;
		case 2:
			handleAutoCombatSkills(result, sd);
			break;
		case 3:
			handleAutoBuffSkills(result, sd);
			break;
		case 4:
			handleTeleportFlywing(result, sd);
			break;
		case 5:
			handleAutoCombatItems(result, sd);
			break;
		case 6:
			handleTeleportSkill(result, sd);
			break;
		case 7:
			handleMeleeAttack(result, sd);
			break;
		case 8:
			handleAutoCombatPotionState(result, sd);
			break;
		case 9:
			handleReturnToSavepoint(result, sd);
			break;
		case 10:
			handleResetAutoCombatConfig(sd);
			break;
		case 11:
			handleTokenOfSiegfried(result, sd);
			break;
		case 12:
			handleAcceptPartyRequest(result, sd);
			break;
		case 13:
			handlePriorizeLootFight(result, sd);
			break;
		case 14:
			handleSitRestHp(result, sd);
			break;
		case 15:
			handleSitRestSp(result, sd);
			break;
		case 16:
			handleTeleportMinHp(result, sd);
			break;
		case 17:
			handleTeleportNoMobMeet(result, sd);
			break;
		case 18:
			handleIgnoreAggressiveMonster(result, sd);
			break;
		case 19:
			handleMonsterSelection(result, sd);
			break;
		case 20:
			handleItemPickupConfiguration(result, sd);
			break;
		case 21:
			handleItemPickupSelection(result, sd);
			break;
		case 22:
			handleActionOnEnd(result, sd);
			break;
		case 23:
			handleMonsterSurround(result, sd);
			break;
	}

	return SCRIPT_CMD_SUCCESS;
}

/*==========================================
* ac_getstate
*------------------------------------------*/
BUILDIN_FUNC(ac_getstate)
{
	TBL_PC* sd;

	if (!script_rid2sd(sd))
		return SCRIPT_CMD_SUCCESS;// no player attached, report source

	script_pushint(st, sd->state.autocombat);

	return SCRIPT_CMD_SUCCESS;
}

/*==========================================
* getcollectionstore
*------------------------------------------*/
BUILDIN_FUNC(getcollectionstore)  
{  
    int count = 0;  
  
    for (const auto& pair : collection_db)  
    {  
        std::shared_ptr<s_collection_stor> stor = pair.second;  
  
        // Include storages that have either items OR combos  
        if (stor->items.empty() && stor->combos.empty())  
            continue;  
  
        const char* storage_name = storage_getName((uint8)stor->stor_id);    
        if (storage_name != nullptr) {    
            setd_sub_num(st, NULL, ".@stor_id", count, (uint8)stor->stor_id, NULL);    
            setd_sub_str(st, NULL, ".@stor_name$", count++, storage_name, NULL);    
        }  
    }  
  
    script_pushint(st, count);  
    return SCRIPT_CMD_SUCCESS;  
}

/*==========================================
* getcollectionlist
*------------------------------------------*/
BUILDIN_FUNC(getcollectionlist)
{
	map_session_data* sd;

	if (script_hasdata(st, 3) && script_isstring(st, 3)) // Character Name
	{
		if (!script_nick2sd(3, sd))
			return SCRIPT_CMD_FAILURE;
	}
	else // Account ID
	{
		if (!script_accid2sd(3, sd))
			return SCRIPT_CMD_FAILURE;
	}

	int j = 0, k = 0;
	int stor_id = script_getnum(st, 2);
	std::shared_ptr<s_collection_stor> stor = collection_db.find(stor_id);
	if (stor != nullptr)
	{
		const auto& list = sd->collection.items[stor->stor_id];
		for (std::shared_ptr<s_collection_item> entry : stor->items)
		{
			t_itemid nameid = entry->nameid;

			auto idx = std::find_if(list.begin(), list.end(),
				[&nameid](const s_collection_items& s) { return (s.nameid == nameid); });

			if (idx != list.end())
			{
				setd_sub_num(st, NULL, ".@amount2", j, idx->amount, NULL);
				setd_sub_num(st, NULL, ".@refine2", j, idx->refine, NULL);
				if (idx->amount >= entry->amount && idx->refine >= entry->refine)
				{
					setd_sub_num(st, NULL, ".@flag", j, 1, NULL);
					setd_sub_num(st, NULL, ".@count", 0, ++k, NULL);
				}
			}

			setd_sub_num(st, NULL, ".@amount", j, entry->amount, NULL);
			setd_sub_num(st, NULL, ".@refine", j, entry->refine, NULL);
			setd_sub_num(st, NULL, ".@nameid", j++, entry->nameid, NULL);
		}
	}

	script_pushint(st, j);

	return SCRIPT_CMD_SUCCESS;
}

/*==========================================
* getcollectionbonus
*------------------------------------------*/
BUILDIN_FUNC(getcollectionbonus)
{
	map_session_data* sd;

	if (script_hasdata(st, 2) && script_isstring(st, 2))
	{
		if (!script_nick2sd(2, sd))
			return SCRIPT_CMD_FAILURE;
	}
	else
	{
		if (!script_accid2sd(2, sd))
			return SCRIPT_CMD_FAILURE;
	}

	int j = 0;
	const auto& list = sd->collection.bonus;
	for (const auto& it : list) {
		setd_sub_num(st, NULL, ".@type", j, it.type, NULL);
		setd_sub_num(st, NULL, ".@val1", j, it.val1, NULL);
		setd_sub_num(st, NULL, ".@val2", j, it.val2, NULL);
		j++;
	}

	script_pushint(st, j);
	return SCRIPT_CMD_SUCCESS;
}  

/*==========================================  
* checkcollectioncombo  
*------------------------------------------*/    
BUILDIN_FUNC(checkcollectioncombo)    
{    
    map_session_data* sd;    
    if (!script_rid2sd(sd))    
        return SCRIPT_CMD_FAILURE;    
        
    int stor_id = script_getnum(st, 2);    
        
    std::shared_ptr<s_collection_stor> collection = collection_db.find(stor_id);    
    if (collection == nullptr) {    
        script_pushint(st, 0);    
        return SCRIPT_CMD_SUCCESS;    
    }    
        
    const auto& player_items = sd->collection.items[stor_id];    
    int active_combo_count = 0;    
        
    // Check each combo set using indexed loop  
    for (size_t combo_idx = 0; combo_idx < collection->combos.size(); combo_idx++) {  
        auto& combo = collection->combos[combo_idx];  
        bool combo_complete = true;    
            
        // Check if ALL items in this combo are present with requirements    
        for (t_itemid combo_item : combo->items) {    
            auto it = std::find_if(player_items.begin(), player_items.end(),    
                [&](const s_collection_items& item) { return item.nameid == combo_item; });    
                    
            if (it == player_items.end() ||     
                it->amount < combo->amount ||     
                it->refine < combo->refine) {    
                combo_complete = false;    
                break;    
            }    
        }    
            
        if (combo_complete) {    
            active_combo_count++;    
        }    
    }    
        
    script_pushint(st, active_combo_count);    
    return SCRIPT_CMD_SUCCESS;    
}

/*==========================================
* getcollectioncombos
*------------------------------------------*/ 
BUILDIN_FUNC(getcollectioncombos)      
{      
    map_session_data* sd;      
    if (!script_rid2sd(sd))      
        return SCRIPT_CMD_FAILURE;      
          
    int stor_id = script_getnum(st, 2);      
          
    std::shared_ptr<s_collection_stor> collection = collection_db.find(stor_id);      
    if (collection == nullptr) {      
        script_pushint(st, 0);      
        return SCRIPT_CMD_SUCCESS;      
    }      
          
    int32 combo_count = 0;  // Initialize to 0, not combo->items.size()    
      
    // Store combo items using hash map for O(1) lookup    
    static std::unordered_map<std::string, t_itemid> combo_item_map;    
          
    // Iterate through combos and populate script arrays      
    for (const auto& combo : collection->combos) {      
        setd_sub_num(st, NULL, ".@combo_item_count", combo_count, static_cast<int>(combo->items.size()), NULL);      
        setd_sub_num(st, NULL, ".@combo_amounts", combo_count, combo->amount, NULL);  
        setd_sub_str(st, NULL, ".@combo_names$", combo_count, combo->name.c_str(), NULL);  
          
        // Store combo items with hash map optimization     
        for (size_t i = 0; i < combo->items.size(); i++) {      
            std::string combo_key = std::to_string(combo_count) + "_" + std::to_string(i);    
            combo_item_map[combo_key] = combo->items[i];    
              
            setd_sub_num(st, NULL, ".@combo_items", static_cast<int32>(combo_count * 10 + i), combo->items[i], NULL);      
        }      
              
        combo_count++;      
    }      
          
    script_pushint(st, combo_count);      
    return SCRIPT_CMD_SUCCESS;      
}

/*==========================================
* getcombocollectionlist
*------------------------------------------*/
BUILDIN_FUNC(getcombocollectionlist)      
{      
	map_session_data* sd;      
      
	if (script_hasdata(st, 3) && script_isstring(st, 3)) // Character Name      
	{      
		if (!script_nick2sd(3, sd))      
			return SCRIPT_CMD_FAILURE;      
	}      
	else // Account ID      
	{      
		if (!script_accid2sd(3, sd))      
			return SCRIPT_CMD_FAILURE;      
	}      
      
	int j = 0, k = 0;      
	int stor_id = script_getnum(st, 2);      
	std::shared_ptr<s_collection_stor> stor = collection_db.find(stor_id);      
	if (stor != nullptr)      
	{      
		const auto& list = sd->collection.items[stor->stor_id];      
		      
		// Process individual items (existing logic)      
		for (std::shared_ptr<s_collection_item> entry : stor->items)      
		{      
			t_itemid nameid = entry->nameid;      
      
			auto idx = std::find_if(list.begin(), list.end(),      
				[&nameid](const s_collection_items& s) { return (s.nameid == nameid); });      
      
			if (idx != list.end())      
			{      
				setd_sub_num(st, NULL, ".@amount2", j, idx->amount, NULL);      
				setd_sub_num(st, NULL, ".@refine2", j, idx->refine, NULL);      
				if (idx->amount >= entry->amount && idx->refine >= entry->refine)      
				{      
					setd_sub_num(st, NULL, ".@flag", j, 1, NULL);      
					setd_sub_num(st, NULL, ".@count", 0, ++k, NULL);      
				}      
			}      
      
			setd_sub_num(st, NULL, ".@amount", j, entry->amount, NULL);      
			setd_sub_num(st, NULL, ".@refine", j, entry->refine, NULL);      
			setd_sub_num(st, NULL, ".@nameid", j++, entry->nameid, NULL);      
		}      
		      
		// Process combos with individual item tracking (KEEP ONLY THIS LOOP)  
		for (size_t combo_idx = 0; combo_idx < stor->combos.size(); combo_idx++)      
		{      
			auto& combo = stor->combos[combo_idx];      
			bool combo_complete = true;      
				  
			// Check if all combo items are present with requirements      
			for (t_itemid combo_item : combo->items) {      
				auto it = std::find_if(list.begin(), list.end(),      
					[&](const s_collection_items& item) { return item.nameid == combo_item; });      
						  
				if (it == list.end() ||       
					it->amount < combo->amount ||       
					it->refine < combo->refine) {      
					combo_complete = false;      
					break;      
				}      
			}      
				  
			// Set combo status - cast size_t to int32    
			bool is_active = (combo_idx < stor->active_combos.size() && stor->active_combos[combo_idx]);      
			setd_sub_num(st, NULL, ".@combo_complete", static_cast<int32>(combo_idx), combo_complete ? 1 : 0, NULL);      
			setd_sub_num(st, NULL, ".@combo_active", static_cast<int32>(combo_idx), is_active ? 1 : 0, NULL);      
			setd_sub_num(st, NULL, ".@combo_amount", static_cast<int32>(combo_idx), combo->amount, NULL);      
			setd_sub_num(st, NULL, ".@combo_refine", static_cast<int32>(combo_idx), combo->refine, NULL);      
				  
			// Store combo item count and items with current/required amounts    
			setd_sub_num(st, NULL, ".@combo_item_count", static_cast<int32>(combo_idx), static_cast<int32>(combo->items.size()), NULL);      
			for (size_t i = 0; i < combo->items.size(); i++) {      
				int32 index = static_cast<int32>(combo_idx) * 10 + static_cast<int32>(i); // Max 10 items per combo      
				t_itemid combo_item = combo->items[i];    
				    
				// Store item ID    
				setd_sub_num(st, NULL, ".@combo_items", index, combo_item, NULL);      
				    
				// Find current amount player has of this combo item    
				int current_amount = 0;    
				auto it = std::find_if(list.begin(), list.end(),    
					[&](const s_collection_items& item) { return item.nameid == combo_item; });    
				    
				if (it != list.end()) {    
					current_amount = it->amount;    
				}    
				    
				// Store both current and required amounts for this combo item    
				setd_sub_num(st, NULL, ".@combo_current_amounts", index, current_amount, NULL);    
				setd_sub_num(st, NULL, ".@combo_required_amounts", index, combo->amount, NULL);    
				setd_sub_num(st, NULL, ".@combo_required_refine", index, combo->refine, NULL);    
			}      
		}
		      
		// Set combo count      
		setd_sub_num(st, NULL, ".@combo_count", 0, static_cast<int>(stor->combos.size()), NULL);      
	}      
      
	script_pushint(st, j);      
      
	return SCRIPT_CMD_SUCCESS;      
}

/*==========================================  
* checkspecificcombo  
*------------------------------------------*/  
BUILDIN_FUNC(checkspecificcombo)      
{      
    map_session_data* sd;      
    if (!script_rid2sd(sd))      
        return SCRIPT_CMD_FAILURE;      
          
    int stor_id = script_getnum(st, 2);      
    int combo_index = script_getnum(st, 3);      
          
    std::shared_ptr<s_collection_stor> collection = collection_db.find(stor_id);      
    if (collection == nullptr || combo_index < 0 || combo_index >= collection->combos.size()) {      
        script_pushint(st, 0);      
        return SCRIPT_CMD_SUCCESS;      
    }      
  
    // Enhanced bounds checking for active_combos vector  
    if (combo_index >= collection->active_combos.size()) {  
        ShowError("checkspecificcombo: combo_index %d exceeds active_combos size %zu for stor_id %d\\n",   
                  combo_index, collection->active_combos.size(), stor_id);  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
        
    // Check if combo is manually activated    
    if (!collection->active_combos[combo_index]) {    
        script_pushint(st, 0);    
        return SCRIPT_CMD_SUCCESS;    
    }    
          
    // Combo is active, return success  
    script_pushint(st, 1);      
    return SCRIPT_CMD_SUCCESS;      
} 

/*==========================================
* checkallcombos
*------------------------------------------*/
BUILDIN_FUNC(checkallcombos)      
{      
    map_session_data* sd;      
    if (!script_rid2sd(sd))      
        return SCRIPT_CMD_FAILURE;      
          
    int stor_id = script_getnum(st, 2);      
    std::shared_ptr<s_collection_stor> collection = collection_db.find(stor_id);      
      
    if (collection == nullptr) {      
        script_pushint(st, 0);      
        return SCRIPT_CMD_SUCCESS;      
    }      
      
    const auto& player_items = sd->collection.items[stor_id];  
      
    // Return all combo statuses in arrays  
    for (size_t i = 0; i < collection->combos.size(); i++) {  
        bool is_active = (i < collection->active_combos.size() && collection->active_combos[i]);  
        setd_sub_num(st, NULL, ".@combo_active", static_cast<int32>(i), is_active ? 1 : 0, NULL);
    }  
      
    script_pushint(st, collection->combos.size());      
    return SCRIPT_CMD_SUCCESS;      
}

/*==========================================  
* activatespecificcombo  
*------------------------------------------*/  
BUILDIN_FUNC(activatespecificcombo)    
{    
    map_session_data* sd;    
    if (!script_rid2sd(sd))    
        return SCRIPT_CMD_FAILURE;    
        
    int stor_id = script_getnum(st, 2);    
    int combo_index = script_getnum(st, 3);    
        
    std::shared_ptr<s_collection_stor> collection = collection_db.find(stor_id);    
    if (collection == nullptr || combo_index < 0 || combo_index >= collection->combos.size()) {    
        script_pushint(st, 0);    
        return SCRIPT_CMD_SUCCESS;    
    }    
  
    // Enhanced bounds checking for active_combos vector  
    if (combo_index >= collection->active_combos.size()) {  
        ShowError("activatespecificcombo: combo_index %d exceeds active_combos size %zu for stor_id %d\\n",   
                  combo_index, collection->active_combos.size(), stor_id);  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
        
    // Use consolidated validation logic instead of inline validation  
    if (!collection_validate_combo(sd, stor_id, combo_index, false)) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
  
    // Begin transaction for database safety  
    if (SQL_ERROR == Sql_Query(mmysql_handle, "START TRANSACTION")) {  
        Sql_ShowDebug(mmysql_handle);  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
  
    // Update combo state in memory first  
    collection->active_combos[combo_index] = true;  
  
    // Update database with transaction safety  
    if (SQL_ERROR == Sql_Query(mmysql_handle,     
        "INSERT INTO `collection_combos` (`account_id`, `char_id`, `stor_id`, `combo_index`, `is_active`) "    
        "VALUES ('%d', '%d', '%d', '%d', '1') "    
        "ON DUPLICATE KEY UPDATE `is_active` = '1'",     
        sd->status.account_id, sd->status.char_id, stor_id, combo_index)) {    
          
        Sql_ShowDebug(mmysql_handle);  
        // Rollback on error  
        Sql_Query(mmysql_handle, "ROLLBACK");  
        collection->active_combos[combo_index] = false; // Revert memory state  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
  
    // Commit transaction  
    if (SQL_ERROR == Sql_Query(mmysql_handle, "COMMIT")) {  
        Sql_ShowDebug(mmysql_handle);  
        // Even if commit fails, the operation succeeded, so don't revert  
    }  
  
    auto& combo = collection->combos[combo_index];  
  
    // Apply the specific combo's script    
    if (combo->script) {    
        run_script(combo->script, 0, sd->id, 0);    
    }    
        
    // Trigger status recalculation    
    status_calc_pc(sd, SCO_NONE);    
        
    script_pushint(st, 1);    
    return SCRIPT_CMD_SUCCESS;    
} 

/*==========================================  
* deactivatespecificcombo  
*------------------------------------------*/  
BUILDIN_FUNC(deactivatespecificcombo)      
{      
    map_session_data* sd;      
    if (!script_rid2sd(sd))      
        return SCRIPT_CMD_FAILURE;      
          
    int stor_id = script_getnum(st, 2);      
    int combo_index = script_getnum(st, 3);      
          
    std::shared_ptr<s_collection_stor> collection = collection_db.find(stor_id);      
    if (collection == nullptr || combo_index < 0 || combo_index >= collection->combos.size()) {      
        script_pushint(st, 0);      
        return SCRIPT_CMD_SUCCESS;      
    }      
  
    // Enhanced bounds checking for active_combos vector  
    if (combo_index >= collection->active_combos.size()) {  
        ShowError("deactivatespecificcombo: combo_index %d exceeds active_combos size %zu for stor_id %d\\n",   
                  combo_index, collection->active_combos.size(), stor_id);  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
  
    // Check if combo is already inactive  
    if (!collection->active_combos[combo_index]) {  
        script_pushint(st, 1); // Already inactive, consider it success  
        return SCRIPT_CMD_SUCCESS;  
    }  
  
    // Begin transaction for database safety  
    if (SQL_ERROR == Sql_Query(mmysql_handle, "START TRANSACTION")) {  
        Sql_ShowDebug(mmysql_handle);  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
  
    // Set the combo as inactive in memory first  
    collection->active_combos[combo_index] = false;  
  
    // Update database with transaction safety  
    if (SQL_ERROR == Sql_Query(mmysql_handle,     
        "UPDATE `collection_combos` SET `is_active` = '0' "    
        "WHERE `account_id` = '%d' AND `char_id` = '%d' AND `stor_id` = '%d' AND `combo_index` = '%d'",     
        sd->status.account_id, sd->status.char_id, stor_id, combo_index)) {    
          
        Sql_ShowDebug(mmysql_handle);  
        // Rollback on error  
        Sql_Query(mmysql_handle, "ROLLBACK");  
        collection->active_combos[combo_index] = true; // Revert memory state  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }   
  
    // Commit transaction  
    if (SQL_ERROR == Sql_Query(mmysql_handle, "COMMIT")) {  
        Sql_ShowDebug(mmysql_handle);  
        // Even if commit fails, the operation succeeded, so don't revert  
    }  
  
    // Recalculate status to remove combo bonuses    
    status_calc_pc(sd, SCO_NONE);      
          
    script_pushint(st, 1);      
    return SCRIPT_CMD_SUCCESS;      
}

BUILDIN_FUNC( runeui ){
#if PACKETVER_MAIN_NUM >= 20200916 || PACKETVER_RE_NUM >= 20200724
	map_session_data* sd;

	if( !script_charid2sd( 2, sd ) ){
		return SCRIPT_CMD_FAILURE;
	}

	clif_rune_ui_open( sd );

	return SCRIPT_CMD_SUCCESS;
#else
	ShowError( "buildin_runeui: This command requires PACKETVER 2020-07-24 or newer.\n" );
	return SCRIPT_CMD_FAILURE;
#endif
}

BUILDIN_FUNC( getupgrade_rune ){
#if PACKETVER_MAIN_NUM >= 20200916 || PACKETVER_RE_NUM >= 20200724
	map_session_data* sd;
	if( !script_rid2sd(sd) )
		return SCRIPT_CMD_FAILURE;

	if(!sd->runeactivated_data.empty()) {
		script_pushint(st, sd->runeactivated_data[0].upgrade);
	} else {
		script_pushint(st, 0);
	}
	return SCRIPT_CMD_SUCCESS;
#else
	ShowError( "buildin_getupgrade_rune: This command requires PACKETVER 2020-07-24 or newer.\n" );
	return SCRIPT_CMD_FAILURE;
#endif
}

/*==========================================
 * Removes all items from a barter shop
 * barter_clear <npc name>;
 *------------------------------------------*/
BUILDIN_FUNC(barter_clear)
{
	const char* npcname = script_getstr(st, 2);
	std::shared_ptr<s_npc_barter> barter = barter_db.find(npcname);
	
	if (barter != nullptr) {
		barter->items.clear();
	}

	return SCRIPT_CMD_SUCCESS;
}

/*==========================================
* Add an item to the barter shop
* Not extended:
* barter_add <npc name>,<item id>,<stock>,<req item id>,<req amount>;
*
* Extended:
* barter_add_ex <npc name>,<item id>,<stock>,<zeny>{,<req item id>,<req amount>,<req refine>}*;
*------------------------------------------*/
BUILDIN_FUNC(barter_add)
{
	const char* npcname = script_getstr(st, 2);
	std::shared_ptr<s_npc_barter> barter = barter_db.find(npcname);
	struct npc_data* nd = npc_name2id(npcname);
	const char* command = script_getfuncname(st);

	if (barter == nullptr) {
		barter = std::make_shared<s_npc_barter>();
		barter->name = npcname;
		barter_db.put(npcname, barter);
	}

	if (nd == nullptr) {
		nd = npc_create_dummy_barter_npc(npcname);
	}

	uint16 index = static_cast<uint16>(barter->items.size());

	std::shared_ptr<s_npc_barter_item> item = std::make_shared<s_npc_barter_item>();
	item->index = index;
	barter->items[index] = item;
	
	if (strcmpi(command, "barter_add_ex") == 0) {
		nd->u.barter.extended = true;
	
		if (script_isstring(st, 3)) {
			const char* name = script_getstr(st, 3);
	
			std::shared_ptr<item_data> id = item_db.searchname(name);
	
			if (id == nullptr) {
				ShowError("buildin_barter_add: Nonexistant item %s\n", name);
				return SCRIPT_CMD_FAILURE;
			}
	
			item->nameid = id->nameid;
		}
		else {
			item->nameid = script_getnum(st, 3);
		}
	
		item->stock = script_getnum(st, 4);
		item->stockLimited = false;
		item->price = script_getnum(st, 5);
	
		int offset = 6;
	
		while (script_hasdata(st, offset) && script_hasdata(st, offset + 1) && script_hasdata(st, offset + 2)) {
			std::shared_ptr<s_npc_barter_requirement> requirement = std::make_shared<s_npc_barter_requirement>();
	
			requirement->index = (uint16)item->requirements.size();
	
			if (script_isstring(st, offset)) {
				const char* name = script_getstr(st, offset);
	
				std::shared_ptr<item_data> id = item_db.searchname(name);
	
				if (id == nullptr) {
					ShowError("buildin_barter_add: Nonexistant item %s\n", name);
					return SCRIPT_CMD_FAILURE;
				}
	
				requirement->nameid = id->nameid;
			}
			else {
				requirement->nameid = script_getnum(st, offset);
			}
	
			requirement->amount = script_getnum(st, offset + 1);
			requirement->refine = script_getnum(st, offset + 2);
			item->requirements[requirement->index] = requirement;
			offset += 3;
		}
	}
	else {
		nd->u.barter.extended = false;
	
		if (script_isstring(st, 3)) {
			const char* name = script_getstr(st, 3);
		
			std::shared_ptr<item_data> id = item_db.searchname(name);
		
			if (id == nullptr) {
				ShowError("buildin_barter_add: Nonexistant item %s\n", name);
				return SCRIPT_CMD_FAILURE;
			}
		
			item->nameid = id->nameid;
		}
		else {
			item->nameid = script_getnum(st, 3);
		}
		
		item->stock = script_getnum(st, 4);
		item->stockLimited = false;
	
		std::shared_ptr<s_npc_barter_requirement> requirement = std::make_shared<s_npc_barter_requirement>();
		
		requirement->index = 0;
		
		if (script_isstring(st, 5)) {
			const char* name = script_getstr(st, 5);
		
			std::shared_ptr<item_data> id = item_db.searchname(name);
		
			if (id == nullptr) {
				ShowError("buildin_barter_add: Nonexistant item %s\n", name);
				return SCRIPT_CMD_FAILURE;
			}
		
			requirement->nameid = id->nameid;
		}
		else {
			requirement->nameid = script_getnum(st, 5);
		}
		
		requirement->amount = script_getnum(st, 6);
		item->requirements[0] = requirement;
	}

	return SCRIPT_CMD_SUCCESS;
}

/*==========================================    
* Get base items for a reform entry    
* getreformbaseitems("reform_item_name")    
* Returns array of base item names (alphabetically sorted)  
*------------------------------------------*/    
static std::unordered_map<std::string, std::vector<std::string>> reform_base_items_cache;  
static std::unordered_map<std::string, int> reform_cache_counts; 
BUILDIN_FUNC(getreformbaseitems)    
{    
    const char* reform_item = script_getstr(st, 2);    
    map_session_data* sd;    
        
    if (!script_rid2sd(sd))    
        return SCRIPT_CMD_FAILURE;    
        
    // Check cache first    
    auto cache_it = reform_base_items_cache.find(reform_item);    
    if (cache_it != reform_base_items_cache.end()) {    
        // Populate script arrays from cache    
        int count = 0;    
        for (const auto& item_name : cache_it->second) {    
            setd_sub_str(st, nullptr, ".@base_items$", count, item_name.c_str(), nullptr);    
            count++;    
        }    
        script_pushint(st, count);    
        return SCRIPT_CMD_SUCCESS;    
    }    
        
    // Cache miss - perform database lookup and cache result    
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);    
    if (!reform_data) {    
        script_pushint(st, 0);    
        return SCRIPT_CMD_SUCCESS;    
    }    
        
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);    
    if (!reform_entry) {    
        script_pushint(st, 0);    
        return SCRIPT_CMD_SUCCESS;    
    }    
        
    // Collect and sort items (as before)    
    std::vector<std::string> item_names;    
    for (const auto& base_pair : reform_entry->base_items) {    
        std::shared_ptr<item_data> item_data = item_db.find(base_pair.first);    
        if (item_data) {    
            item_names.push_back(item_data->name);    
        }    
    }    
    std::sort(item_names.begin(), item_names.end());    
        
    // Cache the results    
    reform_base_items_cache[reform_item] = item_names;    
    reform_cache_counts[reform_item] = static_cast<int>(item_names.size());
        
    // Populate script arrays    
    int count = 0;    
    for (const auto& item_name : item_names) {    
        setd_sub_str(st, nullptr, ".@base_items$", count, item_name.c_str(), nullptr);    
        count++;    
    }    
        
    script_pushint(st, count);    
    return SCRIPT_CMD_SUCCESS;    
}  
  
BUILDIN_FUNC(clearreformcache)    
{    
    // Now clears the shared global cache  
    reform_base_items_cache.clear();    
    reform_cache_counts.clear();  
    script_pushint(st, 1);    
    return SCRIPT_CMD_SUCCESS;    
}


/*==========================================  
* Get minimum refine requirement  
* getreformminrefine("reform_item_name", "base_item_name")  
* Returns minimum refine level  
*------------------------------------------*/  
BUILDIN_FUNC(getreformminrefine)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        // Use correct field name: minimumRefine  
        script_pushint(st, base_it->second->minimumRefine);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    script_pushint(st, 0);  
    return SCRIPT_CMD_SUCCESS;  
}

/*==========================================  
* Get maximum refine requirement for a base item reform  
* getreformmaxrefine("reform_item_name", "base_item_name")  
* Returns maximum refine level  
*------------------------------------------*/  
BUILDIN_FUNC(getreformmaxrefine)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        // Use correct field name: maximumRefine  
        script_pushint(st, base_it->second->maximumRefine);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    script_pushint(st, 0);  
    return SCRIPT_CMD_SUCCESS;  
}

/*==========================================  
* Get required random options for a base item reform  
* getreformrequiredrandomoptions("reform_item_name", "base_item_name")  
* Returns number of required random options  
*------------------------------------------*/  
BUILDIN_FUNC(getreformrequiredrandomoptions)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        // Use correct field name: requiredRandomOptions  
        script_pushint(st, base_it->second->requiredRandomOptions);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    script_pushint(st, 0);  
    return SCRIPT_CMD_SUCCESS;  
}  
  
/*==========================================  
* Get cards allowed setting for a base item reform  
* getreformcardsallowed("reform_item_name", "base_item_name")  
* Returns 1 if cards are allowed, 0 if not  
*------------------------------------------*/  
BUILDIN_FUNC(getreformcardsallowed)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushint(st, 1);  // Default: cards allowed  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushint(st, 1);  // Default: cards allowed  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushint(st, 1);  // Default: cards allowed  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        // Use correct field name: cardsAllowed  
        script_pushint(st, base_it->second->cardsAllowed ? 1 : 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    script_pushint(st, 1);  // Default: cards allowed  
    return SCRIPT_CMD_SUCCESS;  
}

/*==========================================  
* Get materials for a specific base item in reform  
* getreformmaterials("reform_item_name", "base_item_name")  
* Returns array of material info: "item_name:amount"  
*------------------------------------------*/  
BUILDIN_FUNC(getreformmaterials)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    int count = 0;  
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        // Materials are stored as std::unordered_map<t_itemid, uint16>  
        for (const auto& material_pair : base_it->second->materials) {  
            std::shared_ptr<item_data> mat_data = item_db.find(material_pair.first);  
            if (mat_data) {  
                char material_info[256];  
                snprintf(material_info, sizeof(material_info), "%s:%d",   
                        mat_data->name.c_str(), material_pair.second);  
                setd_sub_str(st, nullptr, ".@materials$", count, material_info, nullptr);  
                count++;  
            }  
        }  
    }  
      
    script_pushint(st, count);  
    return SCRIPT_CMD_SUCCESS;  
}  
  
/*==========================================  
* Get result item for a base item reform  
* getreformresult("reform_item_name", "base_item_name")  
* Returns result item name  
*------------------------------------------*/  
BUILDIN_FUNC(getreformresult)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushstrcopy(st, "");  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushstrcopy(st, "");  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushstrcopy(st, "");  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        // Use correct field name: resultItemId  
        std::shared_ptr<item_data> result_data = item_db.find(base_it->second->resultItemId);  
        if (result_data) {  
            script_pushstrcopy(st, result_data->name.c_str());  
            return SCRIPT_CMD_SUCCESS;  
        }  
    }  
      
    script_pushstrcopy(st, "");  
    return SCRIPT_CMD_SUCCESS;  
}  
  
/*==========================================  
* Get refine change for a base item reform  
* getreformrefinechange("reform_item_name", "base_item_name")  
* Returns refine change value  
*------------------------------------------*/  
BUILDIN_FUNC(getreformrefinechange)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        // Use correct field name: refineChange  
        script_pushint(st, base_it->second->refineChange);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    script_pushint(st, 0);  
    return SCRIPT_CMD_SUCCESS;  
}  

/*==========================================  
* Get random option group for a base item reform  
* getreformrandomoptiongroup("reform_item_name", "base_item_name")  
* Returns random option group name or empty string  
*------------------------------------------*/  
BUILDIN_FUNC(getreformrandomoptiongroup)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushstrcopy(st, "");  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushstrcopy(st, "");  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushstrcopy(st, "");  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        // Access randomOptionGroup field  
        if (base_it->second->randomOptionGroup) {  
            // Assuming the random option group has a name field  
            script_pushstrcopy(st, base_it->second->randomOptionGroup->name.c_str());  
        } else {  
            script_pushstrcopy(st, "");  
        }  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    script_pushstrcopy(st, "");  
    return SCRIPT_CMD_SUCCESS;  
}

/*==========================================  
* Get clear slots setting for a base item reform  
* getreformclearslots("reform_item_name", "base_item_name")  
* Returns 1 if slots will be cleared, 0 if not  
*------------------------------------------*/  
BUILDIN_FUNC(getreformclearslots)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        script_pushint(st, base_it->second->clearSlots ? 1 : 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    script_pushint(st, 0);  
    return SCRIPT_CMD_SUCCESS;  
}  
  
/*==========================================  
* Get remove enchantgrade setting for a base item reform  
* getreformremoveenchantgrade("reform_item_name", "base_item_name")  
* Returns 1 if enchantgrade will be removed, 0 if not  
*------------------------------------------*/  
BUILDIN_FUNC(getreformremoveenchantgrade)  
{  
    const char* reform_item = script_getstr(st, 2);  
    const char* base_item = script_getstr(st, 3);  
    map_session_data* sd;  
      
    if (!script_rid2sd(sd))  
        return SCRIPT_CMD_FAILURE;  
      
    // Convert reform item name to ID  
    std::shared_ptr<item_data> reform_data = item_db.searchname(reform_item);  
    if (!reform_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the reform entry using item ID as key  
    std::shared_ptr<s_item_reform> reform_entry = item_reform_db.find(reform_data->nameid);  
    if (!reform_entry) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Find the specific base item  
    std::shared_ptr<item_data> base_data = item_db.searchname(base_item);  
    if (!base_data) {  
        script_pushint(st, 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    auto base_it = reform_entry->base_items.find(base_data->nameid);  
    if (base_it != reform_entry->base_items.end()) {  
        script_pushint(st, base_it->second->removeEnchantgrade ? 1 : 0);  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    script_pushint(st, 0);  
    return SCRIPT_CMD_SUCCESS;  
}

BUILDIN_FUNC(open_stall){
	map_session_data* sd;
	uint16 skill_id, skill_lv;

	if( !script_rid2sd( sd ) ){
		return SCRIPT_CMD_FAILURE;
	}

	if (script_isstring(st, 2)) {
		const char *name = script_getstr(st, 2);

		if (!(skill_id = skill_name2id(name))) {
			ShowError("buildin_unitskilluseid: Invalid skill name %s passed to item bonus. Skipping.\n", name);
			return SCRIPT_CMD_FAILURE;
		}
	}
	skill_lv = script_getnum(st,3);

	if( sd->itemid == 0 ){
		ShowError( "open_stall: Called outside of an item script without item id.\n" );
		return SCRIPT_CMD_FAILURE;
	}

	if( sd->inventory.u.items_inventory[sd->itemindex].expire_time == 0 ){
		ShowError( "open_stall: Called from item %u, which is not a consumed delayed.\n", sd->itemid );
		return SCRIPT_CMD_FAILURE;
	}

	if( sd->state.stall_ui_open != 0 ){
		ShowError( "open_stall: Stall window was already open. Player %s (AID: %u, CID: %u) with item id %u.\n", sd->status.name, sd->status.account_id, sd->status.char_id, sd->itemid );
		return SCRIPT_CMD_FAILURE;
	}

	sd->stall_expire_time = sd->inventory.u.items_inventory[sd->itemindex].expire_time;
	sd->stallvending_level = skill_lv;

	// todo check if already set

	short type = (skill_id == ALL_ASSISTANT_BUYING) ? 1 : 0;
	if (stall_isStallOpen(sd->status.char_id, type)) {
		clif_skill_fail(*sd, skill_id, USESKILL_FAIL_EXIST_STORE_ASSISTANT, 0, skill_id);
		return SCRIPT_CMD_SUCCESS;
	}

	unit_skilluse_id(sd, sd->id, skill_id, skill_lv);

	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(getskillname)  
{  
    uint16 skill_id;  
      
    // Get input skill_id  
    if (!script_hasdata(st, 2)) {  
        script_pushconststr(st, "null");  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Support both numeric ID and skill name string  
    skill_id = (script_isstring(st, 2) ? skill_name2id(script_getstr(st, 2)) : script_getnum(st, 2));  
      
    // Validate skill exists  
    std::shared_ptr<s_skill_db> skill = skill_db.find(skill_id);  
    if (!skill) {  
        ShowError("buildin_getskillname: Unknown skill_id %d supplied.\\n", skill_id);  
        script_pushconststr(st, "null");  
        return SCRIPT_CMD_SUCCESS;  
    }  
      
    // Push the skill description (display name)  
    script_pushstrcopy(st, skill->desc);  
      
    return SCRIPT_CMD_SUCCESS;  
}
