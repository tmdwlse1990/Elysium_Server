// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// Copyright (c) Shakto Scripts - https://ronovelty.com/
// For more information, see LICENCE in the main folder


#ifndef AUTOCOMBAT_HPP
#define AUTOCOMBAT_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "path.hpp"
#include "status.hpp"

#include <common/mmo.hpp>
#include <common/database.hpp>
#include <common/db.hpp>

constexpr auto AC_WALK_CELL = 13;
constexpr auto AC_PREFIX_NAME = "[AUTO]";
const std::vector<int> AC_HATEFFECTS = { 97, 131, 123, 31, 3 };
extern std::vector<t_itemid> AC_ITEMIDS;

class map_session_data;
struct block_list;

// Translates dx,dy into walking direction
static enum directions walk_choices[3][3] =
{
	{DIR_NORTHWEST,DIR_NORTH,DIR_NORTHEAST},
	{DIR_WEST,DIR_CENTER,DIR_EAST},
	{DIR_SOUTHWEST,DIR_SOUTH,DIR_SOUTHEAST},
};

// Structure pour representer une cellule
struct Cell {
	int x, y;
	float cost;

	// Comparateur pour la file de priorite (min-heap)
	bool operator>(const Cell& other) const {
		return cost > other.cost;
	}
};

float heuristic(int x1, int y1, int x2, int y2);

// Functor de hachage pour std::tuple<int, int>
struct TupleHash {
	std::size_t operator()(const std::tuple<int, int>& key) const {
		auto [x, y] = key;
		return std::hash<int>()(x) ^ (std::hash<int>()(y) << 1);
	}
};

enum aa_current_order {
	AC_NOTHING = 0,
	AC_MOVE_TO_ITEM = 1,
	AC_MOVE_TO_MONSTER = 2
};

bool algorithm_path_finding(map_session_data* sd, int16_t m, int start_x, int start_y, int target_x, int target_y);
int ac_move_to_path(std::vector<std::tuple<int, int>>& path, map_session_data* sd);
void ac_move_path(map_session_data* sd);

struct s_autocombatskills {
	bool is_active;
	uint16 skill_id;
	uint16 skill_lv;
	t_tick last_use;
};

struct s_autobuffskills {
	bool is_active;
	uint16 skill_id;
	uint16 skill_lv;
	t_tick last_use;
};

struct s_autoheal {
	bool is_active;
	uint16 skill_id;
	uint16 skill_lv;
	uint16 min_hp;
	t_tick last_use;
};

struct s_autopotion {
	bool is_active;
	t_itemid item_id;
	uint16 min_hp;
	uint16 min_sp;
};

struct s_autositregen {
	bool is_active;
	uint16 max_hp;
	uint16 min_hp;
	uint16 max_sp;
	uint16 min_sp;
};

struct s_autobuffitems {
	bool is_active;
	t_itemid item_id;
	int status;
};

struct s_lastposition {
	int map; // Previous map on Map Change
	short x,y;
	short dx,dy;
};

struct s_targetposition {
	int map;
	short x, y;
	short dx, dy;
};

struct s_teleport {
	bool use_teleport;
	bool use_flywing;
	uint16 min_hp;
	unsigned int delay_nomobmeet;
	bool tp_mvp;
	bool tp_miniboss;
};

struct s_mobs {
	int map;
	std::vector<uint32> id;
	bool aggressive_behavior; //0 attack - 1 ignore
};

struct s_autocombat {
	time_t last_teleport;
	time_t last_move;
	time_t last_attack;
	time_t last_pick;
	t_tick skill_cd;
	t_tick last_hit;
	int attack_target_id;
	int target_id;
	int itempick_id;
	int stopmelee;
	bool accept_party_request;
	bool return_to_savepoint;
	bool token_siegfried;
	unsigned int pickup_item_config;
	unsigned int prio_item_config;
	struct s_teleport teleport;
	struct s_lastposition lastposition;
	struct s_targetposition targetposition;
	struct s_autositregen autositregen;
	struct s_mobs mobs;
	bool state_potions;
	std::vector<s_autoheal> autoheal;
	std::vector<s_autopotion> autopotion;
	std::vector<s_autobuffskills> autobuffskills;
	std::vector<s_autocombatskills> autocombatskills;
	std::vector<s_autobuffitems> autobuffitems;
	std::vector<t_itemid> pickup_item_id;
	std::vector<std::pair<std::string, t_tick>> msg_list;
	std::vector<std::tuple<int, int>> path;
	int path_index;
	int action_on_end;
	int monster_surround;
	t_tick duration_;
	unsigned int unique_id;
	uint32 client_addr; // remote client address
	int skill_range;
};

void ac_save(map_session_data* sd);
void ac_load(map_session_data* sd);
void ac_skill_range_calc(map_session_data* sd);
void ac_mob_ai_search_mvpcheck(struct block_list* bl, struct mob_data* md);
void ac_priority_on_hit(map_session_data* sd, struct block_list* src);
void ac_target_change(map_session_data* sd, int id);
void ac_reset_ondead(map_session_data* sd);
bool ac_canuseskill(map_session_data* sd, uint16 skill_id, uint16 skill_lv);
int buildin_autopick_sub(struct block_list* bl, va_list ap);
bool ac_check_item_pickup(map_session_data* sd, struct block_list* bl);
unsigned int ac_check_item_pickup_onfloor(map_session_data* sd);
bool ac_check_target(map_session_data* sd, unsigned int id);
int buildin_autocombat_sub(struct block_list* bl, va_list ap);
int buildin_autocombat_monsters_sub(struct block_list* bl, va_list ap);
unsigned int ac_check_target_alive(map_session_data* sd);
int ac_check_surround_monster(map_session_data* sd);
bool ac_teleport(map_session_data* sd);
int ac_ammochange(map_session_data* sd, struct mob_data* md, const unsigned short* ammoIds, const unsigned short* ammoElements, const unsigned short* ammoAtk, size_t ammoCount, int rqAmount, const unsigned short* ammoLevels);
void ac_arrowchange(map_session_data* sd, struct mob_data* md);
void ac_bulletchange(map_session_data* sd, mob_data* md);
void ac_kunaichange(map_session_data* sd, struct mob_data* md, int rqamount);
void ac_cannonballchange(map_session_data* sd, struct mob_data* md);
bool ac_elemstrong(const mob_data* md, int ele);
bool ac_elemallowed(struct mob_data* md, int ele);
int ac_status(map_session_data* sd);
bool ac_status_checkteleport_delay(map_session_data* sd, t_tick last_tick);
bool ac_status_check_reset(map_session_data* sd, t_tick last_tick);
bool ac_status_pickup(map_session_data* sd, t_tick last_tick);
bool ac_status_heal(map_session_data* sd, const status_data* status, t_tick last_tick);
bool ac_status_potion(map_session_data* sd, const status_data* status);
bool ac_status_rest(map_session_data* sd, const status_data* status, t_tick last_tick);
bool ac_status_buffs(map_session_data* sd, t_tick last_tick);
void ac_token_respawn(map_session_data* sd, int flag);
void handle_autospell(map_session_data* sd, int skill_lv);
bool ac_checkactivestatus(map_session_data* sd, sc_type type);
bool ac_status_buffitem(map_session_data* sd, t_tick last_tick);
bool ac_status_attack(map_session_data* sd, struct mob_data* md_target, t_tick last_tick);
bool ac_status_melee(map_session_data* sd, struct mob_data* md_target, t_tick last_tick, const status_data* status);
bool ac_move_short(map_session_data* sd, t_tick last_tick);
void ac_status_checkmapchange(map_session_data* sd);
bool ac_message(map_session_data* sd, std::string key, char* message, int delay, struct party_data* p);
void ac_getusablepotions(map_session_data* sd, t_itemid* inventory_potion_id, int* inventory_potion_amount, char inventory_potion_name[MAX_INVENTORY][ITEM_NAME_LENGTH + 1], int* amount);
uint32 ac_getrental_search_inventory(map_session_data* sd, t_itemid nameid);
bool ac_changestate_autocombat(map_session_data* sd, int flag);

int ac_get_random_coords(int16 m, int& x, int& y);
bool ac_party_request(map_session_data* sd);
void ac_moblist_reset_mapchange(map_session_data* sd);

#endif /* AUTOCOMBAT_HPP */