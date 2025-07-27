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
