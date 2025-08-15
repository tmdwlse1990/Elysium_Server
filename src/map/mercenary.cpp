// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "mercenary.hpp"

#include <cmath>
#include <cstdlib>
#include <map>

#include <common/cbasetypes.hpp>
#include <common/malloc.hpp>
#include <common/mmo.hpp>
#include <common/nullpo.hpp>
#include <common/random.hpp>
#include <common/showmsg.hpp>
#include <common/strlib.hpp>
#include <common/timer.hpp>
#include <common/utilities.hpp>
#include <common/utils.hpp>

#include "clif.hpp"
#include "intif.hpp"
#include "itemdb.hpp"
#include "log.hpp"
#include "npc.hpp"
#include "party.hpp"
#include "pc.hpp"
#include "trade.hpp"

using namespace rathena;

MercenaryDatabase mercenary_db;

/**
* Get View Data of Mercenary by Class ID
* @param class_ The Class ID
* @return View Data of Mercenary
**/
struct view_data *mercenary_get_viewdata( uint16 class_ ){
	std::shared_ptr<s_mercenary_db> db = mercenary_db.find(class_);

	if( db ){
		return &db->vd;
	}else{
		return nullptr;
	}
}

/**
* Create a new Mercenary for Player
* @param sd The Player
* @param class_ Mercenary Class
* @param lifetime Contract duration
* @return false if failed, true otherwise
**/
bool mercenary_create(map_session_data *sd, uint16 class_, uint32 lifetime) {
	nullpo_retr(false,sd);

	std::shared_ptr<s_mercenary_db> db = mercenary_db.find(class_);

	if (db == nullptr) {
		ShowError("mercenary_create: Unknown mercenary class %d.\n", class_);
		return false;
	}

	s_mercenary merc = {};

	merc.char_id = sd->status.char_id;
	merc.class_ = class_;
	merc.hp = db->status.max_hp;
	merc.sp = db->status.max_sp;
	merc.life_time = lifetime;

	// Request Char Server to create this mercenary
	intif_mercenary_create(&merc);

	return true;
}

/**
* Get current Mercenary lifetime
* @param md The Mercenary
* @return The Lifetime
**/
t_tick mercenary_get_lifetime(s_mercenary_data *md) {
	if( md == nullptr || md->contract_timer == INVALID_TIMER )
		return 0;

	const struct TimerData *td = get_timer(md->contract_timer);
	return (td != nullptr) ? DIFF_TICK(td->tick, gettick()) : 0;
}

/**
* Get Guild type of Mercenary
* @param md Mercenary
* @return enum e_MercGuildType
**/
e_MercGuildType mercenary_get_guild(s_mercenary_data *md){
	if( md == nullptr || md->db == nullptr )
		return NONE_MERC_GUILD;

	uint16 class_ = md->db->class_;

	if( class_ >= MERID_MER_ARCHER01 && class_ <= MERID_MER_ARCHER10 )
		return ARCH_MERC_GUILD;
	if( class_ >= MERID_MER_LANCER01 && class_ <= MERID_MER_LANCER10 )
		return SPEAR_MERC_GUILD;
	if( class_ >= MERID_MER_SWORDMAN01 && class_ <= MERID_MER_SWORDMAN10 )
		return SWORD_MERC_GUILD;

	return NONE_MERC_GUILD;
}

/**
* Get Faith value of Mercenary
* @param md Mercenary
* @return the Faith value
**/
int32 mercenary_get_faith(s_mercenary_data *md) {
	map_session_data *sd;

	if( md == nullptr || md->db == nullptr || (sd = md->master) == nullptr )
		return 0;

	e_MercGuildType guild = mercenary_get_guild(md);

	switch( guild ){
		case ARCH_MERC_GUILD:
			return sd->status.arch_faith;
		case SPEAR_MERC_GUILD:
			return sd->status.spear_faith;
		case SWORD_MERC_GUILD:
			return sd->status.sword_faith;
		case NONE_MERC_GUILD:
		default:
			return 0;
	}
}

/**
* Set faith value of Mercenary
* @param md The Mercenary
* @param value Faith Value
**/
void mercenary_set_faith(s_mercenary_data *md, int32 value) {
	map_session_data *sd;

	if( md == nullptr || md->db == nullptr || (sd = md->master) == nullptr )
		return;

	e_MercGuildType guild = mercenary_get_guild(md);
	int32 *faith = nullptr;

	switch( guild ){
		case ARCH_MERC_GUILD:
			faith = &sd->status.arch_faith;
			break;
		case SPEAR_MERC_GUILD:
			faith = &sd->status.spear_faith;
			break;
		case SWORD_MERC_GUILD:
			faith = &sd->status.sword_faith;
			break;
		case NONE_MERC_GUILD:
			return;
	}

	*faith += value;
	*faith = cap_value(*faith, 0, SHRT_MAX);
	clif_mercenary_updatestatus(sd, SP_MERCFAITH);
}

/**
* Get Mercenary's calls
* @param md Mercenary
* @return Number of calls
**/
int32 mercenary_get_calls(s_mercenary_data *md) {
	map_session_data *sd;

	if( md == nullptr || md->db == nullptr || (sd = md->master) == nullptr )
		return 0;

	e_MercGuildType guild = mercenary_get_guild(md);

	switch( guild ){
		case ARCH_MERC_GUILD:
			return sd->status.arch_calls;
		case SPEAR_MERC_GUILD:
			return sd->status.spear_calls;
		case SWORD_MERC_GUILD:
			return sd->status.sword_calls;
		case NONE_MERC_GUILD:
		default:
			return 0;
	}
}

/**
* Set Mercenary's calls
* @param md Mercenary
* @param value
**/
void mercenary_set_calls(s_mercenary_data *md, int32 value) {
	map_session_data *sd;

	if( md == nullptr || md->db == nullptr || (sd = md->master) == nullptr )
		return;

	e_MercGuildType guild = mercenary_get_guild(md);
	int32 *calls = nullptr;

	switch( guild ){
		case ARCH_MERC_GUILD:
			calls = &sd->status.arch_calls;
			break;
		case SPEAR_MERC_GUILD:
			calls = &sd->status.spear_calls;
			break;
		case SWORD_MERC_GUILD:
			calls = &sd->status.sword_calls;
			break;
		case NONE_MERC_GUILD:
			return;
	}

	*calls += value;
	*calls = cap_value(*calls, 0, INT_MAX);
}

/**
* Save Mercenary data
* @param md Mercenary
**/
void mercenary_save(s_mercenary_data *md) {
	md->mercenary.hp = md->battle_status.hp;
	md->mercenary.sp = md->battle_status.sp;
	md->mercenary.life_time = mercenary_get_lifetime(md);

	// Clear skill cooldown array.
	for (uint16 i = 0; i < MAX_SKILLCOOLDOWN; i++)
		md->mercenary.scd[i] = {};
	
	// Store current cooldown entries.
	uint16 count = 0;
	t_tick tick = gettick();

	for (const auto &entry : md->scd) {
		const TimerData *timer = get_timer(entry.second);

		if (timer == nullptr || timer->func != skill_blockmerc_end || DIFF_TICK(timer->tick, tick) < 0)
			continue;

		md->mercenary.scd[count] = { entry.first, DIFF_TICK(timer->tick, tick) };

		count++;
	}

	intif_mercenary_save(&md->mercenary);
}

/**
* Ends contract of Mercenary
**/
static TIMER_FUNC(merc_contract_end){
	map_session_data *sd;
	s_mercenary_data *md;

	if( (sd = map_id2sd(id)) == nullptr )
		return 1;
	if( (md = sd->md) == nullptr )
		return 1;

	if( md->contract_timer != tid )
	{
		ShowError("merc_contract_end %d != %d.\n", md->contract_timer, tid);
		return 0;
	}

	md->contract_timer = INVALID_TIMER;
	mercenary_delete(md, 0); // Mercenary soldier's duty hour is over.

	return 0;
}

/**
* Delete Mercenary
* @param md Mercenary
* @param reply
**/
int32 mercenary_delete(s_mercenary_data *md, int32 reply) {
	map_session_data *sd = md->master;
	md->mercenary.life_time = 0;

	mercenary_contract_stop(md);

	if( !sd )
		return unit_free(md, CLR_OUTSIGHT);

	if( md->devotion_flag )
	{
		md->devotion_flag = 0;
		status_change_end(sd, SC_DEVOTION);
	}

	switch( reply )
	{
		case 0:
			// +1 Loyalty on Contract ends.
			mercenary_set_faith(md, 1);
			clif_msg( *sd, MSI_MER_FINISH );
			break; 
		case 1:
			// -1 Loyalty on Mercenary killed
			mercenary_set_faith(md, -1);
			clif_msg( *sd, MSI_MER_DIE );
			break; 
	}

	return unit_remove_map(md, CLR_OUTSIGHT);
}

/**
* Stop contract of Mercenary
* @param md Mercenary
**/
void mercenary_contract_stop(s_mercenary_data *md) {
	nullpo_retv(md);
	if( md->contract_timer != INVALID_TIMER )
		delete_timer(md->contract_timer, merc_contract_end);
	md->contract_timer = INVALID_TIMER;
}

/**
* Init contract of Mercenary
* @param md Mercenary
**/
void merc_contract_init(s_mercenary_data *md) {
	if( md->contract_timer == INVALID_TIMER )
		md->contract_timer = add_timer(gettick() + md->mercenary.life_time, merc_contract_end, md->master->id, 0);

	md->regen.state.block = 0;
}

/**
 * Received mercenary data from char-serv
 * @param merc : mercenary datas
 * @param flag : if inter-serv request was sucessfull
 * @return false:failure, true:sucess
 */
bool mercenary_recv_data(s_mercenary *merc, bool flag)
{
	map_session_data *sd;
	t_tick tick = gettick();

	if( (sd = map_charid2sd(merc->char_id)) == nullptr )
		return false;

	std::shared_ptr<s_mercenary_db> db = mercenary_db.find(merc->class_);

	if( !flag || !db ){ // Not created - loaded - DB info
		sd->status.mer_id = 0;
		return false;
	}

	s_mercenary_data *md;

	if( !sd->md ) {
		sd->md = md = (s_mercenary_data*)aCalloc(1,sizeof(s_mercenary_data));
		new (sd->md) s_mercenary_data();

		md->type = BL_MER;
		md->id = npc_get_new_npc_id();
		md->devotion_flag = 0;

		md->master = sd;
		md->db = db;
		memcpy(&md->mercenary, merc, sizeof(s_mercenary));
		status_set_viewdata(md, md->mercenary.class_);
		unit_dataset(md);
		md->ud.dir = sd->ud.dir;

		md->m = sd->m;
		md->x = sd->x;
		md->y = sd->y;
		unit_calc_pos(md, sd->x, sd->y, sd->ud.dir);
		md->x = md->ud.to_x;
		md->y = md->ud.to_y;

		// Ticks need to be initialized before adding bl to map_addiddb
		md->regen.tick.hp = tick;
		md->regen.tick.sp = tick;

		map_addiddb(md);
		status_calc_mercenary(md, SCO_FIRST);
		md->contract_timer = INVALID_TIMER;
		md->masterteleport_timer = INVALID_TIMER;
		md->auto_support = false;
		md->support_timer = INVALID_TIMER;
		md->last_support_time = 0;
		merc_contract_init(md);
	} else {
		memcpy(&sd->md->mercenary, merc, sizeof(s_mercenary));
		md = sd->md;
	}

	if( sd->status.mer_id == 0 )
		mercenary_set_calls(md, 1);
	sd->status.mer_id = merc->mercenary_id;

	if( md && md->prev == nullptr && sd->prev != nullptr ) {
		if(map_addblock(md))
			return false;
		clif_spawn(md);
		clif_mercenary_info(sd);
		clif_mercenary_skillblock(sd);
	}

	// Apply any active skill cooldowns.
	for (uint16 i = 0; i < ARRAYLENGTH(md->mercenary.scd); i++) {
		skill_blockmerc_start(*md, md->mercenary.scd[i].skill_id, md->mercenary.scd[i].tick);
	}

	return true;
}

/**
* Heals Mercenary
* @param md Mercenary
* @param hp HP amount
* @param sp SP amount
**/
void mercenary_heal(s_mercenary_data *md, int32 hp, int32 sp) {
	if (md->master == nullptr)
		return;
	if( hp )
		clif_mercenary_updatestatus(md->master, SP_HP);
	if( sp )
		clif_mercenary_updatestatus(md->master, SP_SP);
}

/**
 * Delete Mercenary
 * @param md: Mercenary
 * @return false for status_damage
 */
bool mercenary_dead(s_mercenary_data *md) {
	mercenary_delete(md, 1);
	return false;
}

/**
* Gives bonus to Mercenary
* @param md Mercenary
**/
void mercenary_killbonus(s_mercenary_data *md) {
	std::vector<sc_type> scs = { SC_MERC_FLEEUP, SC_MERC_ATKUP, SC_MERC_HPUP, SC_MERC_SPUP, SC_MERC_HITUP };

	sc_start(md,md, util::vector_random(scs), 100, rnd_value(1, 5), 300000); //Bonus lasts for 5 minutes
}

/**
* Mercenary does kill
* @param md Mercenary
**/
void mercenary_kills(s_mercenary_data *md){
	if(md->mercenary.kill_count <= (INT_MAX-1)) //safe cap to INT_MAX
		md->mercenary.kill_count++;

	if( (md->mercenary.kill_count % 50) == 0 )
	{
		mercenary_set_faith(md, 1);
		mercenary_killbonus(md);
	}

	if( md->master )
		clif_mercenary_updatestatus(md->master, SP_MERCKILLS);
}

/**
* Check if Mercenary has the skill
* @param md Mercenary
* @param skill_id The skill
* @return Skill Level or 0 if Mercenary doesn't have the skill
**/
uint16 mercenary_checkskill(s_mercenary_data *md, uint16 skill_id) {
	if (!md || !md->db)
		return 0;

		// Validate skill ID range for mercenary skills  
	if (!SKILL_CHK_MERC(skill_id))  
		return 0;

	auto skill_level = util::umap_find(md->db->skill, skill_id);
	return skill_level ? *skill_level : 0;
}

/*==========================================  
 * Mercenary support timer function  
 * Checks if master needs support buffs and casts them  
 *------------------------------------------*/  
TIMER_FUNC(mercenary_support_timer)      
{      
    map_session_data *sd = map_id2sd(id);        
            
    if (!sd || !sd->md || !sd->md->auto_support) {        
        return 0;      
    }        
            
    struct s_mercenary_data *md = sd->md;        
            
    if (status_isdead(*md) || md->battle_status.sp < 10) {        
        md->support_timer = add_timer(gettick() + 100, mercenary_support_timer, sd->id, 0);        
        return 0;        
    }        
      
    // Check if mercenary is stuck or idle for too long  
    static int last_x = 0, last_y = 0;  
    static int idle_count = 0;  
      
    if (md->x == last_x && md->y == last_y && !md->ud.walktimer) {  
        idle_count++;  
        if (idle_count > 10) { // 5 seconds of being idle  
            // Force movement towards master  
            unit_walktobl(md, (struct block_list*)md->master, 2, 1);  
            idle_count = 0;  
        }  
    } else {  
        idle_count = 0;  
        last_x = md->x;  
        last_y = md->y;  
    }  
            
    // Use enhanced AI function  
    if (md->master) {    
        mercenary_check_auto_skills(md, (struct block_list*)md->master);        
    }    
            
    md->support_timer = add_timer(gettick() + 100, mercenary_support_timer, sd->id, 0);        
    return 0;      
}

/*==========================================    
 * Check and cast support/aggressive skills on master/enemies  
 *------------------------------------------*/    
void mercenary_check_auto_skills(struct s_mercenary_data *md, struct block_list *master)  
{  
    status_change *sc = status_get_sc(master);    
    if (!sc) return;    
      
    // Check distance to master - if too far, move closer first  
    int master_dist = distance_bl(md, master);  
    if (master_dist > MAX_MER_DISTANCE - 3) {  
        unit_walktoxy(md, master->x, master->y, 0);  
        return;  
    }  
        
    // First priority: Support skills for master    
    for (const auto& skill_pair : md->db->skill) {    
        uint16 skill_id = skill_pair.first;    
        uint16 skill_level = skill_pair.second;    
            
        if (skill_level <= 0) continue;    
            
        auto cooldown_it = md->scd.find(skill_id);    
        if (cooldown_it != md->scd.end() && cooldown_it->second > gettick())    
            continue;    
                
        if (mercenary_is_support_skill(skill_id)) {    
            sc_type status_type = skill_get_sc(skill_id);    
            if (status_type != SC_NONE && !sc->getSCE(status_type)) {    
                unit_skilluse_id(md, master->id, skill_id, skill_level);    
                return;    
            }    
        }    
    }    
        
    // Second priority: Combat behavior  
    struct block_list *target = nullptr;    
        
    // Check current target validity with extended range  
    if (md->target_id) {    
        target = map_id2bl(md->target_id);    
        if (!target || status_isdead(*target) ||     
            !check_distance_bl(md, target, md->db->range3 + 3)) { // Extended range  
            md->target_id = 0;    
            target = nullptr;    
        }    
    }    
        
    // Find new target if needed  
    if (!target) {    
        target = mercenary_find_nearby_enemy(md, master);    
        if (target) {    
            md->target_id = target->id;    
        }    
    }    
        
    // Combat actions  
    if (target) {    
        int target_dist = distance_bl(md, target);  
        int attack_range = md->db->status.rhw.range;  
          
        // Move closer if target is out of attack range  
        if (target_dist > attack_range) {  
            unit_walktobl(md, target, attack_range, 1);  
            return;  
        }  
          
        // Use offensive skills  
        for (const auto& skill_pair : md->db->skill) {    
            uint16 skill_id = skill_pair.first;    
            uint16 skill_level = skill_pair.second;    
                
            if (skill_level <= 0) continue;    
                
            auto cooldown_it = md->scd.find(skill_id);    
            if (cooldown_it != md->scd.end() && cooldown_it->second > gettick())    
                continue;    
                    
            if (mercenary_is_offensive_skill(skill_id)) {    
                unit_skilluse_id(md, target->id, skill_id, skill_level);    
                return;    
            }    
        }    
            
        // Normal attack  
        unit_attack(md, target->id, 0);    
    } else {  
        // No target found - follow master if too far  
        if (master_dist > 3) {  
            unit_walktobl(md, master, 2, 1);  
        }  
    }  
}

/*==========================================  
 * Check if skill is a support/buff skill  
 *------------------------------------------*/  
bool mercenary_is_support_skill(uint16 skill_id)  
{  
    switch(skill_id) {  
        case MER_BLESSING:  
        case MER_INCAGI:  
        case MER_QUICKEN:  
        case MER_BENEDICTION:  
        case MER_REGAIN:  
        case ML_AUTOGUARD:  
        case ML_DEFENDER:  
        case ML_DEVOTION:  
        case MER_MAGNIFICAT:
            return true;  
        default:  
            return false;  
    }  
}  
  
/*==========================================  
 * Check if skill is an offensive skill  
 *------------------------------------------*/  
bool mercenary_is_offensive_skill(uint16 skill_id)  
{  
    switch(skill_id) {  
        case MS_BASH:  
        case MS_BOWLINGBASH:  
        case MS_MAGNUM:  
        case MER_CRASH:  
        case ML_BRANDISH:  
        case MER_PROVOKE:  
        case MS_BERSERK:  
        case MER_AUTOBERSERK:
            return true;  
        default:  
            return false;  
    }  
}  
  
/*==========================================  
 * Find nearby enemy for aggressive skills  
 *------------------------------------------*/  
 /*==========================================
  * Helper function for enhanced enemy finding
  *------------------------------------------*/
static int mercenary_find_enemy_sub(struct block_list* bl, va_list ap)
{
	struct s_mercenary_data* md = va_arg(ap, struct s_mercenary_data*);
	struct block_list** target = va_arg(ap, struct block_list**);
	struct block_list* master = va_arg(ap, struct block_list*);
	int* min_dist = va_arg(ap, int*);
	int search_range = va_arg(ap, int);

	if (bl->type == BL_MOB && !status_isdead(*bl)) {
		struct mob_data* mob = BL_CAST(BL_MOB, bl);

		// Prioritize mobs that are targeting the master  
		if (mob && mob->target_id == master->id) {
			*target = bl;
			*min_dist = 0; // Highest priority  
			return 1; // Stop searching, found priority target  
		}

		int dist = distance_bl(md, bl);
		if (dist < *min_dist&& dist <= search_range) {
			*target = bl;
			*min_dist = dist;
		}
	}
	return 0;
}

/*==========================================
 * Enhanced enemy finding with movement and positioning
 *------------------------------------------*/
struct block_list* mercenary_find_nearby_enemy(struct s_mercenary_data* md, struct block_list* master)
{
	struct block_list* target = nullptr;
	int min_distance = 999;
	int search_range = md->db->range3 > 0 ? md->db->range3 : 12; // Use ChaseRange or default  

	// Find enemies that are attacking the master or nearby  
	map_foreachinrange(mercenary_find_enemy_sub, md, search_range, BL_MOB, md, &target, master, &min_distance, search_range);

	return target;
}

/*==========================================  
 * Called when master takes damage - mercenary retaliates  
 *------------------------------------------*/  
void mercenary_damage_support(map_session_data *sd, struct block_list *src)    
{    
    if (!sd || !sd->md || !sd->md->auto_support)      
        return;      
              
    struct s_mercenary_data *md = sd->md;      
          
    // Check if source is valid target      
    if (!src || src->type != BL_MOB || status_isdead(*src))      
        return;      
      
    // Interrupt current action to respond immediately  
    unit_stop_attack(md);  
    unit_stop_walking(md, 1);  
              
    // Set the attacker as priority target      
    md->target_id = src->id;      
      
    // Move to attack range if needed  
    int dist = distance_bl(md, src);  
    int attack_range = md->db->status.rhw.range;  
      
    if (dist > attack_range) {  
        unit_walktobl(md, src, attack_range, 1);  
    }  
          
    // Try to use offensive skills first    
    for (const auto& skill_pair : md->db->skill) {    
        uint16 skill_id = skill_pair.first;    
        uint16 skill_level = skill_pair.second;    
            
        if (skill_level <= 0) continue;    
            
        // Check if skill is on cooldown    
        auto cooldown_it = md->scd.find(skill_id);    
        if (cooldown_it != md->scd.end() && cooldown_it->second > gettick())    
            continue;    
                
        // Use offensive skills for immediate retaliation    
        if (mercenary_is_offensive_skill(skill_id)) {    
            unit_skilluse_id(md, src->id, skill_id, skill_level);    
            return;  
        }    
    }    
        
    // Fall back to normal attack  
    unit_attack(md, src->id, 0);    
}

const std::string MercenaryDatabase::getDefaultLocation() {
	return std::string(db_path) + "/mercenary_db.yml";
}

/*
 * Reads and parses an entry from the mercenary_db.
 * @param node: YAML node containing the entry.
 * @return count of successfully parsed rows
 */
uint64 MercenaryDatabase::parseBodyNode(const ryml::NodeRef& node) {
	uint32 id;

	if (!this->asUInt32(node, "Id", id))
		return 0;

	std::shared_ptr<s_mercenary_db> mercenary = this->find(id);
	bool exists = mercenary != nullptr;

	if (!exists) {
		if (!this->nodesExist(node, { "AegisName", "Name" }))
			return 0;

		mercenary = std::make_shared<s_mercenary_db>();
		mercenary->class_ = id;
	}

	if (this->nodeExists(node, "AegisName")) {
		std::string name;

		if (!this->asString(node, "AegisName", name))
			return 0;

		if (name.size() > NAME_LENGTH) {
			this->invalidWarning(node["AegisName"], "AegisName \"%s\" exceeds maximum of %d characters, capping...\n", name.c_str(), NAME_LENGTH - 1);
		}

		name.resize(NAME_LENGTH);
		mercenary->sprite = name;
	}

	if (this->nodeExists(node, "Name")) {
		std::string name;

		if (!this->asString(node, "Name", name))
			return 0;

		if (name.size() > NAME_LENGTH) {
			this->invalidWarning(node["Name"], "Name \"%s\" exceeds maximum of %d characters, capping...\n", name.c_str(), NAME_LENGTH - 1);
		}

		name.resize(NAME_LENGTH);
		mercenary->name = name;
	}

	if (this->nodeExists(node, "Level")) {
		uint16 level;

		if (!this->asUInt16(node, "Level", level))
			return 0;

		if (level > MAX_LEVEL) {
			this->invalidWarning(node["Level"], "Level %d exceeds MAX_LEVEL, capping to %d.\n", level, MAX_LEVEL);
			level = MAX_LEVEL;
		}

		mercenary->lv = level;
	} else {
		if (!exists)
			mercenary->lv = 1;
	}

	if (this->nodeExists(node, "Hp")) {
		uint32 hp;

		if (!this->asUInt32(node, "Hp", hp))
			return 0;

		mercenary->status.max_hp = hp;
	} else {
		if (!exists)
			mercenary->status.max_hp = 1;
	}
	
	if (this->nodeExists(node, "Sp")) {
		uint32 sp;

		if (!this->asUInt32(node, "Sp", sp))
			return 0;

		mercenary->status.max_sp = sp;
	} else {
		if (!exists)
			mercenary->status.max_sp = 1;
	}

	if (this->nodeExists(node, "Attack")) {
		uint16 atk;

		if (!this->asUInt16(node, "Attack", atk))
			return 0;

		mercenary->status.rhw.atk = atk;
	} else {
		if (!exists)
			mercenary->status.rhw.atk = 0;
	}
	
	if (this->nodeExists(node, "Attack2")) {
		uint16 atk;

		if (!this->asUInt16(node, "Attack2", atk))
			return 0;

		mercenary->status.rhw.atk2 = mercenary->status.rhw.atk + atk;
	} else {
		if (!exists)
			mercenary->status.rhw.atk2 = mercenary->status.rhw.atk;
	}

	if (this->nodeExists(node, "Defense")) {
		int32 def;

		if (!this->asInt32(node, "Defense", def))
			return 0;

		if (def < DEFTYPE_MIN || def > DEFTYPE_MAX) {
			this->invalidWarning(node["Defense"], "Invalid defense %d, capping...\n", def);
			def = cap_value(def, DEFTYPE_MIN, DEFTYPE_MAX);
		}

		mercenary->status.def = static_cast<defType>(def);
	} else {
		if (!exists)
			mercenary->status.def = 0;
	}
	
	if (this->nodeExists(node, "MagicDefense")) {
		int32 def;

		if (!this->asInt32(node, "MagicDefense", def))
			return 0;

		if (def < DEFTYPE_MIN || def > DEFTYPE_MAX) {
			this->invalidWarning(node["MagicDefense"], "Invalid magic defense %d, capping...\n", def);
			def = cap_value(def, DEFTYPE_MIN, DEFTYPE_MAX);
		}

		mercenary->status.mdef = static_cast<defType>(def);
	} else {
		if (!exists)
			mercenary->status.mdef = 0;
	}

	if (this->nodeExists(node, "Str")) {
		uint16 stat;

		if (!this->asUInt16(node, "Str", stat))
			return 0;

		mercenary->status.str = stat;
	} else {
		if (!exists)
			mercenary->status.str = 1;
	}

	if (this->nodeExists(node, "Agi")) {
		uint16 stat;

		if (!this->asUInt16(node, "Agi", stat))
			return 0;

		mercenary->status.agi = stat;
	} else {
		if (!exists)
			mercenary->status.agi = 1;
	}

	if (this->nodeExists(node, "Vit")) {
		uint16 stat;

		if (!this->asUInt16(node, "Vit", stat))
			return 0;

		mercenary->status.vit = stat;
	} else {
		if (!exists)
			mercenary->status.vit = 1;
	}

	if (this->nodeExists(node, "Int")) {
		uint16 stat;

		if (!this->asUInt16(node, "Int", stat))
			return 0;

		mercenary->status.int_ = stat;
	} else {
		if (!exists)
			mercenary->status.int_ = 1;
	}

	if (this->nodeExists(node, "Dex")) {
		uint16 stat;

		if (!this->asUInt16(node, "Dex", stat))
			return 0;

		mercenary->status.dex = stat;
	} else {
		if (!exists)
			mercenary->status.dex = 1;
	}

	if (this->nodeExists(node, "Luk")) {
		uint16 stat;

		if (!this->asUInt16(node, "Luk", stat))
			return 0;

		mercenary->status.luk = stat;
	} else {
		if (!exists)
			mercenary->status.luk = 1;
	}

	if (this->nodeExists(node, "AttackRange")) {
		uint16 range;

		if (!this->asUInt16(node, "AttackRange", range))
			return 0;

		mercenary->status.rhw.range = range;
	} else {
		if (!exists)
			mercenary->status.rhw.range = 0;
	}
	
	if (this->nodeExists(node, "SkillRange")) {
		uint16 range;

		if (!this->asUInt16(node, "SkillRange", range))
			return 0;

		mercenary->range2 = range;
	} else {
		if (!exists)
			mercenary->range2 = 0;
	}
	
	if (this->nodeExists(node, "ChaseRange")) {
		uint16 range;

		if (!this->asUInt16(node, "ChaseRange", range))
			return 0;

		mercenary->range3 = range;
	} else {
		if (!exists)
			mercenary->range3 = 0;
	}

	if (this->nodeExists(node, "Size")) {
		std::string size;

		if (!this->asString(node, "Size", size))
			return 0;

		std::string size_constant = "Size_" + size;
		int64 constant;

		if (!script_get_constant(size_constant.c_str(), &constant)) {
			this->invalidWarning(node["Size"], "Unknown mercenary size %s, defaulting to Small.\n", size.c_str());
			constant = SZ_SMALL;
		}

		if (constant < SZ_SMALL || constant > SZ_BIG) {
			this->invalidWarning(node["Size"], "Invalid mercenary size %s, defaulting to Small.\n", size.c_str());
			constant = SZ_SMALL;
		}

		mercenary->status.size = static_cast<e_size>(constant);
	} else {
		if (!exists)
			mercenary->status.size = SZ_SMALL;
	}
	
	if (this->nodeExists(node, "Race")) {
		std::string race;

		if (!this->asString(node, "Race", race))
			return 0;

		std::string race_constant = "RC_" + race;
		int64 constant;

		if (!script_get_constant(race_constant.c_str(), &constant)) {
			this->invalidWarning(node["Race"], "Unknown mercenary race %s, defaulting to Formless.\n", race.c_str());
			constant = RC_FORMLESS;
		}

		if (!CHK_RACE(constant)) {
			this->invalidWarning(node["Race"], "Invalid mercenary race %s, defaulting to Formless.\n", race.c_str());
			constant = RC_FORMLESS;
		}

		mercenary->status.race = static_cast<e_race>(constant);
	} else {
		if (!exists)
			mercenary->status.race = RC_FORMLESS;
	}

	if (this->nodeExists(node, "Element")) {
		std::string ele;

		if (!this->asString(node, "Element", ele))
			return 0;

		std::string ele_constant = "ELE_" + ele;
		int64 constant;

		if (!script_get_constant(ele_constant.c_str(), &constant)) {
			this->invalidWarning(node["Element"], "Unknown mercenary element %s, defaulting to Neutral.\n", ele.c_str());
			constant = ELE_NEUTRAL;
		}

		if (!CHK_ELEMENT(constant)) {
			this->invalidWarning(node["Element"], "Invalid mercenary element %s, defaulting to Neutral.\n", ele.c_str());
			constant = ELE_NEUTRAL;
		}

		mercenary->status.def_ele = static_cast<e_element>(constant);
	} else {
		if (!exists)
			mercenary->status.def_ele = ELE_NEUTRAL;
	}

	if (this->nodeExists(node, "ElementLevel")) {
		uint16 level;

		if (!this->asUInt16(node, "ElementLevel", level))
			return 0;

		if (!CHK_ELEMENT_LEVEL(level)) {
			this->invalidWarning(node["ElementLevel"], "Invalid mercenary element level %hu, defaulting to 1.\n", level);
			level = 1;
		}

		mercenary->status.ele_lv = static_cast<uint8>(level);
	} else {
		if (!exists)
			mercenary->status.ele_lv = 1;
	}

	if (this->nodeExists(node, "WalkSpeed")) {
		uint16 speed;

		if (!this->asUInt16(node, "WalkSpeed", speed))
			return 0;

		if (speed < MIN_WALK_SPEED || speed > MAX_WALK_SPEED) {
			this->invalidWarning(node["WalkSpeed"], "Invalid mercenary walk speed %hu, capping...\n", speed);
			speed = cap_value(speed, MIN_WALK_SPEED, MAX_WALK_SPEED);
		}

		mercenary->status.speed = speed;
	} else {
		if (!exists)
			mercenary->status.speed = DEFAULT_WALK_SPEED;
	}

	if (this->nodeExists(node, "AttackDelay")) {
		uint16 speed;

		if (!this->asUInt16(node, "AttackDelay", speed))
			return 0;

		mercenary->status.adelay = cap_value(speed, MAX_ASPD_NOPC, MIN_ASPD);
	} else {
		if (!exists)
			mercenary->status.adelay = 4000;
	}
	
	if (this->nodeExists(node, "AttackMotion")) {
		uint16 speed;

		if (!this->asUInt16(node, "AttackMotion", speed))
			return 0;

		// amotion is only capped to MAX_ASPD_NOPC when receiving buffs/debuffs
		mercenary->status.amotion = cap_value(speed, 1, MIN_ASPD/AMOTION_DIVIDER_NOPC);
	} else {
		if (!exists)
			mercenary->status.amotion = 2000;
	}

	if (this->nodeExists(node, "DamageMotion")) {
		uint16 speed;

		if (!this->asUInt16(node, "DamageMotion", speed))
			return 0;

		mercenary->status.dmotion = speed;
	} else {
		if (!exists)
			mercenary->status.dmotion = 0;
	}

	mercenary->status.aspd_rate = 1000;

	if (this->nodeExists(node, "Skills")) {
		const ryml::NodeRef& skillsNode = node["Skills"];

		for (const ryml::NodeRef& skill : skillsNode) {
			std::string skill_name;

			if (!this->asString(skill, "Name", skill_name))
				return 0;

			uint16 skill_id = skill_name2id(skill_name.c_str());

			if (skill_id == 0) {
				this->invalidWarning(skill["Name"], "Invalid skill %s, skipping.\n", skill_name.c_str());
				return 0;
			}

			if (!SKILL_CHK_MERC(skill_id)) {
				this->invalidWarning(skill["Name"], "Skill %s (%u) is out of the mercenary skill range [%u-%u], skipping.\n", skill_name.c_str(), skill_id, MC_SKILLBASE, MC_SKILLBASE + MAX_MERCSKILL - 1);
				return 0;
			}

			uint16 level;

			if (!this->asUInt16(skill, "MaxLevel", level))
				return 0;

			if (level == 0) {
				if (mercenary->skill.erase(skill_id) == 0)
					this->invalidWarning(skill["Name"], "Failed to remove %s, the skill doesn't exist for mercenary %hu.\n", skill_name.c_str(), id);
				continue;
			}

			mercenary->skill[skill_id] = level;
		}
	}

	if (!exists)
		this->put(id, mercenary);

	return true;
}

/**
* Init Mercenary datas
**/
void do_init_mercenary(void){
	mercenary_db.load();

	add_timer_func_list(merc_contract_end, "merc_contract_end");
	add_timer_func_list(mercenary_support_timer, "mercenary_support_timer");
}

/**
* Do Final Mercenary datas
**/
void do_final_mercenary(void){
	mercenary_db.clear();
}
