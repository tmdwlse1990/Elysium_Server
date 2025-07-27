// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "autocombat_script.hpp"
#include "battle.hpp"
#include "script.hpp"

#include <common/db.hpp>
#include <common/showmsg.hpp>

#include <cstdlib> // atoi, strtol, strtoll, exit
#include <sstream>
#include <string>
#include <vector>

bool find_ac_skill_heal(map_session_data* sd, int skill_id, s_autoheal& found_buff) {
	auto it = std::find_if(sd->ac.autoheal.begin(), sd->ac.autoheal.end(),
		[skill_id](const s_autoheal& v) { return v.skill_id == skill_id; });

	if (it != sd->ac.autoheal.end()) {
		found_buff = *it;
		return true;
	}
	return false;
}

bool find_ac_skill_buff(map_session_data* sd, int skill_id, s_autobuffskills& found_buff) {
	auto it = std::find_if(sd->ac.autobuffskills.begin(), sd->ac.autobuffskills.end(),
		[skill_id](const s_autobuffskills& v) { return v.skill_id == skill_id; });

	if (it != sd->ac.autobuffskills.end()) {
		found_buff = *it;
		return true;
	}
	return false;
}

bool find_ac_skill_attack(map_session_data* sd, int skill_id, s_autocombatskills& found_buff) {
	auto it = std::find_if(sd->ac.autocombatskills.begin(), sd->ac.autocombatskills.end(),
		[skill_id](const s_autocombatskills& v) { return v.skill_id == skill_id; });

	if (it != sd->ac.autocombatskills.end()) {
		found_buff = *it;
		return true;
	}
	return false;
}

bool find_ac_mob(map_session_data* sd, int mob_id) {
	auto it = std::find_if(sd->ac.mobs.id.begin(), sd->ac.mobs.id.end(),
		[mob_id](const int& v) { return v == mob_id; });

	if (it != sd->ac.mobs.id.end())
		return true;

	return false;
}

// Fonction pour recuperer les monstres de la map
std::map<int, std::string> get_unique_monsters_onmap(TBL_PC* sd, const char* monster_menu_state, script_state* st) {
	std::map<int, std::string> unique_mobs;
	if (!sd->ac.mobs.id.empty()) {
		for (const auto& itAutocombatmobs : sd->ac.mobs.id) {
			auto mob = mob_db.find(itAutocombatmobs);
			if (!mob) continue;

			std::ostringstream menu_temp;
			menu_temp << "^008000(ON)^000000 [" << mob->id << "] " << mob->name.c_str();

			unique_mobs[itAutocombatmobs] = menu_temp.str();

			set_reg_num(st, sd, reference_uid(add_str(monster_menu_state), itAutocombatmobs), monster_menu_state, 1, nullptr);
		}
	}

	// add other mobs
	unsigned short mapindex = sd->mapindex;
	s_mapiterator* it = mapit_geteachmob();
	bool mob_searched[MAX_MOB_DB2];
	memset(mob_searched, 0, MAX_MOB_DB2);

	while (true) {
		TBL_MOB* md = (TBL_MOB*)mapit_next(it);
		if (md == NULL)
			break;

		if (md->m != sd->m || md->status.hp <= 0)
			continue;

		if (mob_searched[md->mob_id] == true)
			continue; // Already found, skip it

		std::shared_ptr<s_mob_db> mob = mob_db.find(md->mob_id);
		if (!mob)
			continue;

		mob_searched[md->mob_id] = true;

		if (unique_mobs.count(md->mob_id) == 0) {
			std::ostringstream mob_temp;
			mob_temp << "^FF0000(OFF)^000000 [" << mob->id << "] " << mob->name.c_str();
			unique_mobs[mob->id] = mob_temp.str();

			set_reg_num(st, sd, reference_uid(add_str(monster_menu_state), mob->id), monster_menu_state, 0, nullptr);
		}
	}
	mapit_free(it);

	return unique_mobs;
}

// Fonction pour recuperer les competences uniques
std::map<int, std::string> get_unique_attack_skills(TBL_PC* sd, const char* skill_menu_state, script_state* st) {
	std::map<int, std::string> unique_skills;
	if (!sd->ac.autocombatskills.empty()) {
		for (const auto& itAutocombatskills : sd->ac.autocombatskills) {
			auto skill = skill_db.find(itAutocombatskills.skill_id);
			if (!skill) continue;

			std::ostringstream menu_temp;
			menu_temp << "^008000(ON)^000000 " << skill->desc << " - Lv " << itAutocombatskills.skill_lv;

			unique_skills[itAutocombatskills.skill_id] = menu_temp.str();

			set_reg_num(st, sd, reference_uid(add_str(skill_menu_state), itAutocombatskills.skill_id), skill_menu_state, 1, nullptr);
		}
	}

	// add other skills
	for (int i = 0; i < MAX_SKILL; ++i) {
		if (sd->status.skill[i].id > 0 && sd->status.skill[i].lv > 0
			//uncomment the following line to prevent a skill to be shown in the list
			//&& sd->status.skill[i].id != AL_RUWACH
			) {
			auto skill = skill_db.find(sd->status.skill[i].id);
			if (!skill) continue;

			e_cast_type type = skill_get_casttype(sd->status.skill[i].id);
			if ((skill->skill_type != BF_NONE
				&& skill->inf != INF_PASSIVE_SKILL
				&& !skill_get_nk(sd->status.skill[i].id, NK_NODAMAGE)
				&& (type != CAST_NODAMAGE || skill->inf & INF_SELF_SKILL)
				)
				// Add skills like the following lines to force the skill to show in the list
				|| skill->nameid == KN_BRANDISHSPEAR
				|| skill->nameid == CG_TAROTCARD
				) {

				if (unique_skills.count(skill->nameid) == 0) {
					std::ostringstream skill_temp;
					skill_temp << "^FF0000(OFF)^000000 " << skill->desc << " - Lv " << std::to_string(sd->status.skill[i].lv);
					unique_skills[skill->nameid] = skill_temp.str();

					set_reg_num(st, sd, reference_uid(add_str(skill_menu_state), skill->nameid), skill_menu_state, 0, nullptr);
				}
			}
		}
	}
	return unique_skills;
}

// Fonction pour recuperer les competences uniques
std::map<int, std::string> get_unique_buff_skills(TBL_PC* sd, const char* skill_menu_state, script_state* st) {
	std::map<int, std::string> unique_skills;
	if (!sd->ac.autobuffskills.empty()) {
		for (const auto& itAutobuffskills : sd->ac.autobuffskills) {
			auto skill = skill_db.find(itAutobuffskills.skill_id);
			if (!skill) continue;

			std::ostringstream menu_temp;
			menu_temp << "^008000(ON)^000000 " << skill->desc << " - Lv " << itAutobuffskills.skill_lv;

			unique_skills[itAutobuffskills.skill_id] = menu_temp.str();

			set_reg_num(st, sd, reference_uid(add_str(skill_menu_state), itAutobuffskills.skill_id), skill_menu_state, 1, nullptr);
		}
	}

	// add other skills
	for (int i = 0; i < MAX_SKILL; ++i) {
		if (sd->status.skill[i].id > 0 && sd->status.skill[i].lv > 0
			//Add skill like the following line to prevent a skill to be shown in the list
			&& sd->status.skill[i].id != AL_RUWACH
			) {
			auto skill = skill_db.find(sd->status.skill[i].id);
			if (!skill) continue;

			e_cast_type type = skill_get_casttype(sd->status.skill[i].id);
			if ((type == CAST_NODAMAGE
				&& skill->inf & (INF_SUPPORT_SKILL | INF_SELF_SKILL)
				&& skill_get_sc(skill->nameid) != SC_NONE)
				|| skill->nameid == MO_CALLSPIRITS
				|| skill->nameid == CH_SOULCOLLECT
				|| skill->nameid == SA_AUTOSPELL ) {

				if (unique_skills.count(skill->nameid) == 0) {
					std::ostringstream skill_temp;
					skill_temp << "^FF0000(OFF)^000000 " << skill->desc << " - Lv " << std::to_string(sd->status.skill[i].lv);
					unique_skills[skill->nameid] = skill_temp.str();

					set_reg_num(st, sd, reference_uid(add_str(skill_menu_state), skill->nameid), skill_menu_state, 0, nullptr);
				}
			}
		}
	}
	return unique_skills;
}

std::string format_skill_heal_status(const s_autoheal& sheal, std::shared_ptr<s_skill_db> skill) {
	std::ostringstream os;
	os << (sheal.is_active ? "^008000(ON)^000000 " : "^FF0000(OFF)^000000 ");
	os << "[" << skill->desc << "] - Lv " << sheal.skill_lv << " - HP % " << sheal.min_hp;
	return os.str();
}

std::string format_skill_buff_status(const s_autobuffskills& sbuff, std::shared_ptr<s_skill_db> skill) {
	std::ostringstream os;
	os << (sbuff.is_active ? "^008000(ON)^000000 " : "^FF0000(OFF)^000000 ");
	os << "[" << skill->desc << "]";
	os << (sbuff.is_active ? " - Lv " + std::to_string(sbuff.skill_lv) : "");
	return os.str();
}

std::string format_skill_attack_status(const s_autocombatskills& sattack, std::shared_ptr<s_skill_db> skill) {
	std::ostringstream os;
	os << (sattack.is_active ? "^008000(ON)^000000 " : "^FF0000(OFF)^000000 ");
	os << "[" << skill->desc << "]";
	os << (sattack.is_active ? " - Lv " + std::to_string(sattack.skill_lv) : "");
	return os.str();
}

std::string format_mob_status(const int& mob_id) {
	std::ostringstream os;
	std::shared_ptr<s_mob_db> mob = mob_db.find(mob_id);
	if(mob)
		os << "^008000(ON)^000000 [" << mob->id << "] " << mob->name.c_str();
	return os.str();
}

void format_skill_list(map_session_data* sd, std::ostringstream& menu, const std::map<int, std::string>& unique_skills, const char* skill_menu_ids, script_state* st) {
	bool first_done = false;
	int j = 0;

	for (const auto& uskill : unique_skills) {
		if (first_done)
			menu << ":";
		else
			first_done = true;

		menu << uskill.second;

		set_reg_num(st, sd, reference_uid(add_str(skill_menu_ids), ++j), skill_menu_ids, uskill.first, nullptr);
	}
}

void set_skill_state(script_state* st, map_session_data* sd, const char* state_name, int skill_id, bool is_active) {
	set_reg_num(st, sd, reference_uid(add_str(state_name), skill_id), state_name, is_active ? 1 : 0, nullptr);
}

void handle_autocombat_heal(std::ostringstream& os_buf, int index, int extra_index, script_state* st, map_session_data* sd) {

	std::shared_ptr<s_skill_db> skill;

	const char* player_heal_list = ".@player_heal_list$";
	const char* player_heal_ids = ".@player_heal_ids";
	const char* player_heal_state = ".@player_heal_state";
	int j = 0;

	std::ostringstream menu;

	if (index == 0) {
		skill = skill_db.find(extra_index);
		if (!skill)
			return;

		s_autoheal sheal;
		if (find_ac_skill_heal(sd, extra_index, sheal)) {
			os_buf << format_skill_heal_status(sheal, skill);

			set_reg_str(st, sd, reference_uid(add_str(player_heal_list), 0), player_heal_list, menu.str().c_str(), nullptr);
		}
	}
	else if (index > 0) {
		skill = skill_db.find(index);
		if (!skill)
			return;

		s_autoheal sheal;
		if (find_ac_skill_heal(sd, index, sheal)) {
			os_buf << format_skill_heal_status(sheal, skill);
			set_skill_state(st, sd, ".@skill_menu_state", index, true);
		}
		else {
			os_buf << "^FF0000(OFF)^000000 " << skill->desc << " - Max lv " << std::to_string(pc_checkskill(sd, index));
			set_skill_state(st, sd, ".@skill_menu_state", index, false);
		}
	}

	return;
}

void handle_autocombat_potions(std::ostringstream& os_buf, int index, script_state* st, map_session_data* sd) {
	const char* potion_menu_list = ".@potion_menu_list$";
	const char* potion_menu_ids = ".@potion_menu_ids";
	const char* potion_menu_state = ".@potion_menu_state";
	std::ostringstream menu;
	bool first_done = false;

	int j = 0;

	std::map<int, std::string> unique_items;

	// Helper function to build potion item string
	auto buildPotionItemString = [](const std::string& status, const std::string& item_name, short amount = 0, uint16 min_hp = 0, uint16 min_sp = 0) {
		std::ostringstream item_str;
		item_str << status << " " << item_name;
		if (min_hp > 0)
			item_str << " - HP < " << min_hp << "%";
		if (min_sp > 0)
			item_str << " - SP < " << min_sp << "%";
		if (amount > 0)
			item_str << " - x" << amount;
		return item_str.str();
		};

	// Process active autobuff potions
	for (const auto& itAutopotion : sd->ac.autopotion) {
		if (auto item_data = item_db.find(itAutopotion.item_id)) {
			unique_items[itAutopotion.item_id] = buildPotionItemString("^008000(ON)^000000", item_data->ename, 0, itAutopotion.min_hp, itAutopotion.min_sp);
			set_reg_num(st, sd, reference_uid(add_str(potion_menu_state), itAutopotion.item_id), potion_menu_state, 1, nullptr);
		}
	}

	// Process potions in inventory
	t_itemid inventory_potion_id[MAX_INVENTORY];
	int inventory_potion_amount[MAX_INVENTORY];
	char inventory_potion_name[MAX_INVENTORY][ITEM_NAME_LENGTH + 1];
	int amount = 0;

	ac_getusablepotions(sd, inventory_potion_id, inventory_potion_amount, inventory_potion_name, &amount);

	for (int i = 0; i < amount; ++i) {
		if (unique_items.count(inventory_potion_id[i]) > 0) {
			unique_items[inventory_potion_id[i]] += " - x" + std::to_string(inventory_potion_amount[i]); // append the amount
		}
		else {
			unique_items[inventory_potion_id[i]] = buildPotionItemString("^FF0000(OFF)^000000", inventory_potion_name[i], inventory_potion_amount[i]);
			set_reg_num(st, sd, reference_uid(add_str(potion_menu_state), inventory_potion_id[i]), potion_menu_state, 0, nullptr);
		}
	}

	for (const auto& uitem : unique_items) {
		if (first_done)
			menu << ":";
		else
			first_done = true;

		menu << uitem.second;
		set_reg_num(st, sd, reference_uid(add_str(potion_menu_ids), ++j), potion_menu_ids, uitem.first, nullptr);
	}

	set_reg_str(st, sd, reference_uid(add_str(potion_menu_list), 0), potion_menu_list, menu.str().c_str(), nullptr);

	return;
}



void handle_autocombat_itempickup(std::ostringstream& os_buf, int index, int extra_index, script_state* st, map_session_data* sd) {
	std::shared_ptr<item_data> item_data;

	switch(sd->ac.pickup_item_config){
		case 0:
			os_buf << "Pick up every item on the floor \n";
			break;
		case 1:
			os_buf << "Pick up only items on the list \n";
			break;
		case 2:
			os_buf << "Don't pick up any item \n";
			break;	
	}
	if(sd->ac.pickup_item_id.size() > 0){
		for (int i=0; i<sd->ac.pickup_item_id.size(); i++){
			if( ( item_data = item_db.find(sd->ac.pickup_item_id.at(i)) ) == NULL )
				break;

			os_buf << "^FF0000(" << item_data->nameid << ")^000000 [" << item_data->name.c_str() << "] \n";
		}
	} else
		os_buf << "\n No item to pick up in the list \n";

	return;
}

void handle_autocombat_monsterselection(std::ostringstream& os_buf, int index, int extra_index, script_state* st, map_session_data* sd) {
	std::shared_ptr<s_skill_db> skill;

	const char* mobs_menu_list = ".@mobs_menu_list$";
	const char* mobs_menu_ids = ".@mobs_menu_ids";
	const char* mobs_menu_state = ".@mobs_menu_state";

	if (index == 0) { // Monster menu
		std::ostringstream menu;
		std::map<int, std::string> unique_mobs = get_unique_monsters_onmap(sd, mobs_menu_state, st);

		format_skill_list(sd, menu, unique_mobs, mobs_menu_ids, st); // works for mobs also
		set_reg_str(st, sd, reference_uid(add_str(mobs_menu_list), 0), mobs_menu_list, menu.str().c_str(), nullptr);
	}
	else if (index > 0 && extra_index == 1) { // Player list menu
		const char* player_buff_list = ".@player_buff_list$";
		const char* player_buff_ids = ".@player_buff_ids";
		const char* player_buff_state = ".@player_buff_state";

		if (find_ac_mob(sd, index)) 
			os_buf << format_mob_status(index);
		else
			os_buf << "^FF0000(OFF)^000000 " << skill_db.find(index)->desc << "\n";

	}
	else {
		std::shared_ptr<s_mob_db> mob = mob_db.find(index);
		if (mob)
			os_buf << "^FF0000(OFF)^000000 [" << mob->id << "] " << mob->name.c_str();
	}

	return;
}

void handle_autocombat_attack(std::ostringstream& os_buf, int index, int extra_index, script_state* st, map_session_data* sd) {
	std::shared_ptr<s_skill_db> skill;

	const char* skill_menu_list = ".@skill_menu_list$";
	const char* skill_menu_ids = ".@skill_menu_ids";
	const char* skill_menu_state = ".@skill_menu_state";

	if (index == 0) { // Skill menu
		std::ostringstream menu;
		std::map<int, std::string> unique_skills = get_unique_attack_skills(sd, skill_menu_state, st);
		format_skill_list(sd, menu, unique_skills, skill_menu_ids, st);
		set_reg_str(st, sd, reference_uid(add_str(skill_menu_list), 0), skill_menu_list, menu.str().c_str(), nullptr);
	}
	else if (index > 0 && extra_index == 1) { // Player list menu
		const char* player_buff_list = ".@player_buff_list$";
		const char* player_buff_ids = ".@player_buff_ids";
		const char* player_buff_state = ".@player_buff_state";

		s_autocombatskills found_attack;
		if (find_ac_skill_attack(sd, index, found_attack)) {
			os_buf << format_skill_attack_status(found_attack, skill_db.find(index));

			//std::map<int, std::string> unique_players = get_unique_players(sd, found_attack, party_id, player_buff_state, st, os_buf);
			std::map<int, std::string> unique_players;
			std::ostringstream menu;
			format_skill_list(sd, menu, unique_players, player_buff_ids, st);

			set_reg_str(st, sd, reference_uid(add_str(player_buff_list), 0), player_buff_list, menu.str().c_str(), nullptr);
		}
		else {
			os_buf << "^FF0000(OFF)^000000 " << skill_db.find(index)->desc << "\n";
		}
	}
	else {
		os_buf << "^FF0000(" << skill_db.find(index)->nameid << ")^000000 " << skill_db.find(index)->desc;
	}

	return;
}

void handle_autocombat_buff(std::ostringstream& os_buf, int index, int extra_index, script_state* st, map_session_data* sd) {
	std::shared_ptr<s_skill_db> skill;

	const char* skill_menu_list = ".@skill_menu_list$";
	const char* skill_menu_ids = ".@skill_menu_ids";
	const char* skill_menu_state = ".@skill_menu_state";

	if (index == 0) { // Skill menu
		std::ostringstream menu;
		std::map<int, std::string> unique_skills = get_unique_buff_skills(sd, skill_menu_state, st);
		format_skill_list(sd, menu, unique_skills, skill_menu_ids, st);
		set_reg_str(st, sd, reference_uid(add_str(skill_menu_list), 0), skill_menu_list, menu.str().c_str(), nullptr);
	}
	else if (index > 0 && extra_index == 1) { // Player list menu
		const char* player_buff_list = ".@player_buff_list$";
		const char* player_buff_ids = ".@player_buff_ids";
		const char* player_buff_state = ".@player_buff_state";

		s_autobuffskills found_buff;
		if (find_ac_skill_buff(sd, index, found_buff)) {
			os_buf << format_skill_buff_status(found_buff, skill_db.find(index));

			//std::map<int, std::string> unique_players = get_unique_players(sd, found_buff, party_id, player_buff_state, st, os_buf);
			std::map<int, std::string> unique_players;
			std::ostringstream menu;
			format_skill_list(sd, menu, unique_players, player_buff_ids, st);

			set_reg_str(st, sd, reference_uid(add_str(player_buff_list), 0), player_buff_list, menu.str().c_str(), nullptr);
		}
		else {
			os_buf << "^FF0000(OFF)^000000 " << skill_db.find(index)->desc << "\n";
		}
	}
	else {
		os_buf << "^FF0000(" << skill_db.find(index)->nameid << ")^000000 " << skill_db.find(index)->desc;
	}

	return;
}

void handle_autocombat_sitrest(std::ostringstream& os_buf, int index, script_state* st, map_session_data* sd) {
	if (sd->ac.autositregen.is_active) {
		os_buf << "^FF0000(Enabled)^000000 \n";

		if (sd->ac.autositregen.min_hp > 0)
			os_buf << "HP - Sit if hp < " << sd->ac.autositregen.min_hp << "% - Stand if hp >= " << sd->ac.autositregen.max_hp << "% \n";

		if (sd->ac.autositregen.min_sp)
			os_buf << "SP - Sit if sp < " << sd->ac.autositregen.min_sp << "% - Stand if sp >= " << sd->ac.autositregen.max_sp << "% \n";
	}
	else
		os_buf << "^FF0000(Disabled)^000000 \n";

}

void handle_autocombat_teleport(std::ostringstream& os_buf, int index, script_state* st, map_session_data* sd) {
	if (sd->ac.teleport.min_hp > 0)
		os_buf << "^008000(ON)^000000 Minimum HP value for emergency teleport set to " << sd->ac.teleport.min_hp << "%\n";
	else
		os_buf << "^FF0000(OFF)^000000 Minimum HP value for emergency teleport \n";
	

	if (sd->ac.teleport.delay_nomobmeet > 0)
		os_buf << "^008000(ON)^000000 Teleport delay if no monster is encountered within that time to " << sd->ac.teleport.delay_nomobmeet/1000 << "s\n";
	else
		os_buf << "^FF0000(OFF)^000000 Teleport delay if no monster is encountered within that time \n";

	if (sd->ac.monster_surround > 0)
		os_buf << "^008000(ON)^000000 Teleport if more than " << sd->ac.monster_surround << " attack you \n";
}

void handle_autocombat_items(std::ostringstream& os_buf, int index, script_state* st, map_session_data* sd) {
	const char* buffitem_menu_state = ".@buffitem_menu_state";
	auto item_data = item_db.find(index);

	if (item_data && item_data->type == IT_USABLE) {
		auto itAutoattackitem = std::find_if(sd->ac.autobuffitems.begin(), sd->ac.autobuffitems.end(),
			[index](const s_autobuffitems& v) { return v.item_id == index; });

		bool is_autocombat_item = (itAutoattackitem != sd->ac.autobuffitems.end());
		os_buf << (is_autocombat_item ? "^008000(ON)^000000 " : "^FF0000(OFF)^000000 ")
			<< item_data->ename.c_str();
		set_reg_num(st, sd, reference_uid(add_str(buffitem_menu_state), index), buffitem_menu_state, is_autocombat_item, nullptr);
	}
}

void handle_potions(std::ostringstream& os_buf, map_session_data* sd) {
	os_buf << "[Potions] - " << (sd->ac.state_potions ? "Enabled" : "Disabled");
}

void handle_token_of_siegfried(std::ostringstream& os_buf, map_session_data* sd) {
	os_buf << "[Token of Siegfried] - " << (sd->ac.token_siegfried ? "Enabled" : "Disabled");
}

void handle_priorize_loot_fight(std::ostringstream& os_buf, map_session_data* sd) {
	os_buf << (sd->ac.prio_item_config ? "[Item Pickup] Priorized loot over fight" : "[Item Pickup] Priorized fight over loot"); // - 0 Fight - 1 Loot
}

void handle_party_request(std::ostringstream& os_buf, map_session_data* sd) {
	os_buf << "[Auto accept party request] - " << (sd->ac.accept_party_request ? "Enabled" : "Disabled");
}

void handle_return_to_savepoint(std::ostringstream& os_buf, map_session_data* sd) {
	os_buf << "[Return to save point on death] - " << (sd->ac.return_to_savepoint ? "Enabled" : "Disabled");
}

bool handleAutocombat_fromitem(map_session_data* sd, t_itemid item_id, t_tick max_duration) {
	t_tick duration_ = 0;
	std::shared_ptr<item_data> id;

	switch (battle_config.function_autocombat_duration_type) {
	case 0:
		uint32 item_expire_time;

		id = item_db.find(item_id);

		if (!id)
			return SCRIPT_CMD_FAILURE;

		item_expire_time = ac_getrental_search_inventory(sd, item_id);

		if (item_expire_time == 0) {
			ShowError("autobuff_fromitem: Item not found in the inventory or is not a consumed delay.\n", item_id);
			return SCRIPT_CMD_FAILURE;
		}

		duration_ = DIFF_TICK(item_expire_time, time(NULL));

		if (duration_ >= max_duration)
			duration_ = max_duration * 1000;
		else
			duration_ = duration_ * 1000;
		break;

	case 1:
		if (sd->ac.duration_ <= 0) {
			std::string msg = "Automessage - You don't have timer left on autocombat system!";
			ac_message(sd, "TimerOut", msg.data(), 300, nullptr);
		}
		else
			duration_ = sd->ac.duration_;
		break;
	}

	if(duration_ > 0)
		status_change_start(sd, sd, SC_AUTOCOMBAT, 10000, 0, 0, 0, 0, duration_, SCSTART_NOAVOID);

	return SCRIPT_CMD_SUCCESS;
}

void handleAutoHeal(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() == 5) {
		bool is_active = static_cast<bool>(std::stoi(result.at(1)));
		uint16 skill_id = static_cast<uint16>(std::stoi(result.at(2)));
		uint16 skill_lv = static_cast<uint16>(std::stoi(result.at(3)));
		uint16 min_hp = static_cast<uint16>(std::stoi(result.at(4)));
		t_tick last_use = 1;

		auto skill = skill_db.find(skill_id);
		if (!skill || (skill->nameid != AL_HEAL && skill->nameid != AB_HIGHNESSHEAL && skill->nameid != AB_CHEAL))
			return;

		if (!is_active) {
			sd->ac.autoheal.erase(
				std::remove_if(sd->ac.autoheal.begin(), sd->ac.autoheal.end(),
					[skill_id](const s_autoheal& v) {
						return v.skill_id == skill_id;
					}),
				sd->ac.autoheal.end());
			return;
		}

		auto itAutoheal = std::find_if(sd->ac.autoheal.begin(), sd->ac.autoheal.end(),
			[skill_id](const s_autoheal& v) { return v.skill_id == skill_id; });

		if (itAutoheal != sd->ac.autoheal.end()) {
			itAutoheal->is_active = is_active;
			itAutoheal->skill_lv = skill_lv;
			itAutoheal->min_hp = min_hp;
		}
		else {
			s_autoheal autoheal{ is_active, skill_id, skill_lv, min_hp, last_use };
			sd->ac.autoheal.push_back(autoheal);
		}
	}
}

void handleAutoPotion(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 5)
		return; // Invalid input

	bool is_active = static_cast<bool>(std::stoi(result.at(1)));
	t_itemid nameid = std::stoi(result.at(2));
	uint16 min_hp = static_cast<uint16>(std::stoi(result.at(3)));
	uint16 min_sp = static_cast<uint16>(std::stoi(result.at(4)));

	auto item_data = item_db.find(nameid);
	if (!item_data || item_data->type != IT_HEALING)
		return; // Item doesn't exist or isn't a healing item

	if (!is_active || (min_hp == 0 && min_sp == 0)) {
		sd->ac.autopotion.erase(
			std::remove_if(sd->ac.autopotion.begin(), sd->ac.autopotion.end(), [nameid](const s_autopotion& v) {
				return v.item_id == nameid;
				}),
			sd->ac.autopotion.end()
		);
	}
	else {
		auto itAutopotion = std::find_if(sd->ac.autopotion.begin(), sd->ac.autopotion.end(), [nameid](const s_autopotion& v) {
			return v.item_id == nameid;
			});

		if (itAutopotion != sd->ac.autopotion.end()) {
			itAutopotion->is_active = is_active;
			itAutopotion->min_hp = min_hp;
			itAutopotion->min_sp = min_sp;
		}
		else {
			s_autopotion autobuff_potions = { is_active, nameid, min_hp, min_sp };
			sd->ac.autopotion.push_back(autobuff_potions);
		}
	}
}

void handleAutoBuffSkills(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() == 4) {
		bool is_active = static_cast<bool>(std::stoi(result.at(1)));
		uint16 skill_id = static_cast<uint16>(std::stoi(result.at(2)));
		uint16 skill_lv = static_cast<uint16>(std::stoi(result.at(3)));

		auto skill = skill_db.find(skill_id);
		if (!skill)
			return; // Invalid skill

		if (skill_id != MO_CALLSPIRITS && skill_id != CH_SOULCOLLECT && skill_id != CH_SOULCOLLECT) { // Bypass if one of this skill
			if (!(skill->inf & (INF_SUPPORT_SKILL | INF_SELF_SKILL)))
				return; // not a support/self skill

			if (skill_get_sc(skill->nameid) == SC_NONE)
				return; // not status linked to the skill
		}

		if (!is_active) {
			sd->ac.autobuffskills.erase(
				std::remove_if(sd->ac.autobuffskills.begin(), sd->ac.autobuffskills.end(), [skill_id](const s_autobuffskills& v) {
					return v.skill_id == skill_id;
					}),
				sd->ac.autobuffskills.end()
			);
		}
		else {
			auto itBuffSkill = std::find_if(sd->ac.autobuffskills.begin(), sd->ac.autobuffskills.end(), [skill_id](const s_autobuffskills& v) {
				return v.skill_id == skill_id;
				});

			if (itBuffSkill != sd->ac.autobuffskills.end()) {
				itBuffSkill->is_active = is_active;
				itBuffSkill->skill_lv = skill_lv;
			}
			else {
				s_autobuffskills buffskills = { is_active, skill_id, skill_lv };
				sd->ac.autobuffskills.push_back(buffskills);
			}
		}
	}
}

void handleAutoCombatSkills(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() == 4) {
		bool is_active = static_cast<bool>(std::stoi(result.at(1)));
		uint16 skill_id = static_cast<uint16>(std::stoi(result.at(2)));
		uint16 skill_lv = static_cast<uint16>(std::stoi(result.at(3)));

		auto skill = skill_db.find(skill_id);
		if (!skill)
			return; // Invalid skill

		if (skill_id != KN_BRANDISHSPEAR && skill_id != CG_TAROTCARD) { // Bypass if one of this skill
			e_cast_type type = skill_get_casttype(skill_id);

			if (skill->skill_type == BF_NONE)
				return; // not a attack skill

			if (skill_get_nk(skill_id, NK_NODAMAGE))
				return; // not damage skill

			if (type == CAST_NODAMAGE && !(skill->inf & INF_SELF_SKILL))
				return; // not damage skill
		}

		if (!is_active) {
			sd->ac.autocombatskills.erase(
				std::remove_if(sd->ac.autocombatskills.begin(), sd->ac.autocombatskills.end(), [skill_id](const s_autocombatskills& v) {
					return v.skill_id == skill_id;
					}),
				sd->ac.autocombatskills.end()
			);
		}
		else {
			auto itAttackSkill = std::find_if(sd->ac.autocombatskills.begin(), sd->ac.autocombatskills.end(), [skill_id](const s_autocombatskills& v) {
				return v.skill_id == skill_id;
				});

			if (itAttackSkill != sd->ac.autocombatskills.end()) {
				itAttackSkill->is_active = is_active;
				itAttackSkill->skill_lv = skill_lv;
			}
			else {
				s_autocombatskills attackskills = { is_active, skill_id, skill_lv };
				sd->ac.autocombatskills.push_back(attackskills);
			}
		}
		ac_skill_range_calc(sd);
	}
}

void handleAutoCombatItems(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 4)
		return; // Invalid input

	bool is_active = static_cast<bool>(std::stoi(result.at(1)));
	t_itemid item_id = std::stoi(result.at(2));
	int item_status = std::stoi(result.at(3));

	if (item_status <= SC_NONE || item_status >= SC_MAX)
		return; // Invalid status

	auto item_data = item_db.find(item_id);
	if (!item_data || item_data->type != IT_USABLE)
		return; // Item doesn't exist or isn't usable

	if (!is_active) {
		sd->ac.autobuffitems.erase(
			std::remove_if(sd->ac.autobuffitems.begin(), sd->ac.autobuffitems.end(), [item_id](const s_autobuffitems& v) {
				return v.item_id == item_id;
				}),
			sd->ac.autobuffitems.end()
		);
	}
	else {
		auto itBuffItem = std::find_if(sd->ac.autobuffitems.begin(), sd->ac.autobuffitems.end(), [item_id](const s_autobuffitems& v) {
			return v.item_id == item_id;
			});

		if (itBuffItem != sd->ac.autobuffitems.end()) {
			itBuffItem->is_active = is_active;
			itBuffItem->status = item_status;
		}
		else {
			s_autobuffitems buffitems = { is_active, item_id, item_status };
			sd->ac.autobuffitems.push_back(buffitems);
		}
	}
}

void handleAutoCombatPotionState(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	bool state_autocombat_potions = std::stoi(result.at(1));
	sd->ac.state_potions = state_autocombat_potions;
}

void handleReturnToSavepoint(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	bool return_to_savepoint = std::stoi(result.at(1));
	sd->ac.return_to_savepoint = return_to_savepoint;
}

void handleResetAutoCombatConfig(map_session_data* sd) {
	sd->ac.pickup_item_id.clear();
	sd->ac.pickup_item_config = 0;
	sd->ac.prio_item_config = 0;
	sd->ac.mobs.id.clear();
	sd->ac.mobs.aggressive_behavior = 0;
	sd->ac.autobuffitems.clear();
	sd->ac.stopmelee = 0;
	sd->ac.autocombatskills.clear();
	sd->ac.autobuffskills.clear();
	sd->ac.autositregen.is_active = 0;
	sd->ac.autopotion.clear();
	sd->ac.autoheal.clear();
	sd->ac.state_potions = true;
	sd->ac.return_to_savepoint = false;
	sd->ac.token_siegfried = false;
	sd->ac.msg_list.clear();
}

void handleTokenOfSiegfried(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	bool autobuff_token_siegfried = std::stoi(result.at(1));
	sd->ac.token_siegfried = autobuff_token_siegfried;
}

void handleTeleportFlywing(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	bool tp_flywing = std::stoi(result.at(1));
	sd->ac.teleport.use_flywing = tp_flywing;
}

void handleTeleportSkill(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	bool tp_skill = std::stoi(result.at(1));
	sd->ac.teleport.use_teleport = tp_skill;
}

void handleMeleeAttack(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	int melee_attack = std::stoi(result.at(1));
	sd->ac.stopmelee = melee_attack;
}

void handleAcceptPartyRequest(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	bool autobuff_accept_party_request = std::stoi(result.at(1));
	sd->ac.accept_party_request = autobuff_accept_party_request;
}

void handleIgnoreAggressiveMonster(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	bool aggressive_behavior = std::stoi(result.at(1));
	sd->ac.mobs.aggressive_behavior = aggressive_behavior;
}

void handleItemPickupConfiguration(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	int pickup_item_config = std::stoi(result.at(1));
	sd->ac.pickup_item_config = pickup_item_config;
}

void handleActionOnEnd(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	int action_on_end = std::stoi(result.at(1));
	sd->ac.action_on_end = action_on_end;
}

void handleMonsterSurround(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	int monster_surround = std::stoi(result.at(1));
	sd->ac.monster_surround = monster_surround;
}

void handleItemPickupSelection(const std::vector<std::string>& result, map_session_data* sd) {
	if(result.size() == 3){ // id = 9 - item pickup (is_active;item_id)
		int i;
		int status = std::stoi(result.at(1));
		t_itemid nameid = std::stoi(result.at(2));
		bool item_found = false;
		std::shared_ptr<item_data> item_data;

		switch(status){
			case -1:
				sd->ac.pickup_item_id.clear();
				break;
			case 0:
				if(!sd->ac.pickup_item_id.empty()){
					for(i=0;i<sd->ac.pickup_item_id.size(); i++){
						if(sd->ac.pickup_item_id.at(i) == nameid){
							sd->ac.pickup_item_id.erase(sd->ac.pickup_item_id.begin()+i);
							break;
						}
					}
				}
				break;
			case 1:
				//check if item exist
				if( ( item_data = item_db.find(nameid) ) == NULL )
					return;

				if(!sd->ac.pickup_item_id.empty()){
					for(i=0;i<sd->ac.pickup_item_id.size(); i++){
						if(sd->ac.pickup_item_id.at(i) == nameid){
							item_found = true;
							break;
						}
					}
				}
				if(!item_found)
					sd->ac.pickup_item_id.push_back(nameid);

				break;
		}
	}
}

void handleMonsterSelection(const std::vector<std::string>& result, map_session_data* sd) {
    if (result.size() != 3)
        return; // Invalid input

    uint32_t mob_id = std::stoul(result.at(1));
    bool active = std::stoi(result.at(2));

    auto& mob_ids = sd->ac.mobs.id;
    auto it = std::find(mob_ids.begin(), mob_ids.end(), mob_id);

    if (active) {
        if (it == mob_ids.end()) {
            // Add the mob_id if not already present
            mob_ids.push_back(mob_id);
        }
    } else {
        if (it != mob_ids.end()) {
            // Remove the mob_id if it exists
            mob_ids.erase(it);
        }
    }
}

void handlePriorizeLootFight(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	bool autobuff_priorizelootfight = std::stoi(result.at(1));
	sd->ac.prio_item_config = autobuff_priorizelootfight;
}

void handleSitRestHp(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 3)
		return; // Invalid input

	int min_hp = std::stoi(result.at(1));
	int max_hp = std::stoi(result.at(2));

	if (min_hp == 0 && sd->ac.autositregen.min_sp == 0)
		sd->ac.autositregen.is_active = 0;
	else
		sd->ac.autositregen.is_active = 1;

	sd->ac.autositregen.min_hp = min_hp;
	sd->ac.autositregen.max_hp = max_hp;
}

void handleSitRestSp(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 3)
		return; // Invalid input

	int min_sp = std::stoi(result.at(1));
	int max_sp = std::stoi(result.at(2));

	if (min_sp == 0 && sd->ac.autositregen.min_hp == 0)
		sd->ac.autositregen.is_active = 0;
	else
		sd->ac.autositregen.is_active = 1;

	sd->ac.autositregen.min_sp = min_sp;
	sd->ac.autositregen.max_sp = max_sp;
}

void handleTeleportMinHp(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	int tp_min_hp = std::stoi(result.at(1));

	sd->ac.teleport.min_hp = tp_min_hp;
}

void handleTeleportNoMobMeet(const std::vector<std::string>& result, map_session_data* sd) {
	if (result.size() != 2)
		return; // Invalid input

	int tp_delay_nomobmeet = std::stoi(result.at(1));

	sd->ac.teleport.delay_nomobmeet = tp_delay_nomobmeet;
}

extern int handleGetautocombatint(map_session_data* sd, const int value) {
	int result = 0;

	switch (value) {
	case 0:
		result = static_cast<int>(sd->ac.autoheal.size());
		break;
	case 2:
		result = static_cast<int>(sd->ac.autocombatskills.size());
		break;
	case 3:
		result = static_cast<int>(sd->ac.autobuffskills.size());
		break;
	case 4:
		result = sd->ac.state_potions;
		break;
	case 5:
		result = static_cast<int>(sd->ac.autobuffitems.size());
		break;
	case 6:
		result = sd->ac.return_to_savepoint;
		break;
	case 7:
		result = sd->ac.token_siegfried;
		break;
	case 8:
		result = sd->ac.accept_party_request;
		break;
	case 9:
		result = sd->ac.pickup_item_config;
		break;
	case 10:
		result = sd->ac.prio_item_config;
		break;
	case 11:
		result = sd->ac.stopmelee;
		break;
	case 12:
		result = sd->ac.teleport.use_flywing;
		break;
	case 13:
		result = sd->ac.teleport.use_teleport;
		break;
	case 14:
		if (sd->ac.autositregen.is_active) {
			switch (sd->ac.autositregen.min_hp) {
			case 75: result = 1;
				break;
			case 50: result = 2;
				break;
			case 25: result = 3;
				break;
			default:
				result = 0;
				break;
			}
		} else
			result = sd->ac.autositregen.is_active;
		break;
	case 15:
		if (sd->ac.autositregen.is_active) {
			switch (sd->ac.autositregen.min_sp) {
				case 75 : result = 1;
					break;
				case 50 : result = 2;
					break;
				case 25 : result = 3;
					break;
				default:
					result = 0;
					break;
			}
		} else
			result = sd->ac.autositregen.is_active;
		break;
	case 16:
		result = sd->ac.mobs.aggressive_behavior;
		break;
	case 17:
		result = static_cast<int>(sd->ac.pickup_item_id.size());
		break;
	case 18:
		result = sd->ac.action_on_end;
		break;
	default:
		return result;
		break;
	}

	return result;
}
