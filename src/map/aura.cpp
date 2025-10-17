// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "aura.hpp"

#include "battle.hpp"
#include "map.hpp"
#include "pc.hpp"

AuraDatabase aura_db;

std::unordered_map<uint16, enum e_aura_special> special_effects{
	{202, AURA_SPECIAL_HIDE_DISAPPEAR},
	{362, AURA_SPECIAL_HIDE_DISAPPEAR}
};

/*==========================================  
* Method: getDefaultLocation  
* Description: Get the specific path of the YAML data file  
* Access: public   
* Returns: const std::string  
* Author: Sola丶小克(CairoLee)  2020/09/26 15:30  
 * -----------------------------------------------------------*/
const std::string AuraDatabase::getDefaultLocation() {
	return std::string(db_path) + "/aura_db.yml";
}

/*==========================================
* Method: parseBodyNode
* Description: The main processing function for parsing the Body node
* Access: public
* Parameter: const ryml::NodeRef & node
* Returns: uint64
* Author: Sola丶小克(CairoLee) 2020/09/26 15:30
 * -----------------------------------------------------------*/
uint64 AuraDatabase::parseBodyNode(const ryml::NodeRef& node) {
	uint32 aura_id = 0;

	if (!this->asUInt32(node, "AuraID", aura_id)) {
		return 0;
	}

	if (aura_id <= 0) {
		return 0;
	}

	auto aura = this->find(aura_id);
	bool exists = aura != nullptr;

	if (!exists) {
		aura = std::make_shared<s_aura>();
		aura->aura_id = aura_id;
	}
	else {
		aura->effects.clear();
	}

	if (!this->nodeExists(node, "EffectList")) {
		return 0;
	}

	for (const ryml::NodeRef& subNode : node["EffectList"]) {
		uint16 effect_id = 0;
		if (!this->asUInt16(subNode, "EffectID", effect_id)) {
			return 0;
		}

		uint32 replay_interval = 0;
		if (this->nodeExists(subNode, "ReplayInterval")) {
			if (!this->asUInt32(subNode, "ReplayInterval", replay_interval)) {
				return 0;
			}
		}

		auto effect = std::make_shared<s_aura_effect>();
		effect->effect_id = effect_id;
		effect->replay_interval = replay_interval;

		aura->effects.push_back(effect);
	}

	if (!exists) {
		this->put(aura->aura_id, aura);
	}

	return 1;
}

/*==========================================
* Method: aura_search
* Description: Get the information recorded in aura_db.yml according to the aura number
* Access: public
* Parameter: uint32 aura_id
* Returns: std::shared_ptr<s_aura>
* Author: Sola丶小克(CairoLee) 2020/09/26 16:30
 *------------------------------------------*/
std::shared_ptr<s_aura> aura_search(uint32 aura_id) {
	return aura_db.find(aura_id);
}

/*==========================================
* Method: aura_special
* Description: Given an effect number, query whether it is a special effect and return its flag bit
* Access: public
* Parameter: uint16 effect_id
* Returns: enum e_aura_special special flag bit
* Author: Sola丶小克(CairoLee) 2020/10/08 10:59
 *------------------------------------------*/
enum e_aura_special aura_special(uint16 effect_id) {
	if (special_effects.find(effect_id) != special_effects.end()) {
		return special_effects[effect_id];
	}
	return AURA_SPECIAL_NOTHING;
}

/*==========================================
* Method: aura_effects_timer_sub
* Description: Processing function used to traverse nearby players and send them specified halo effects
* Access: public
* Parameter: struct block_list * bl traverses to nearby player units
* Parameter: va_list ap
* Returns: int
* Author: Sola丶小克(CairoLee) 2021/12/29 21:27
 *------------------------------------------*/
int aura_effects_timer_sub(struct block_list* bl, va_list ap) {
	map_session_data* sd = nullptr;
	struct block_list* effect_unit_bl = va_arg(ap, struct block_list*);
	unsigned int effect_id = va_arg(ap, unsigned int);

	if (!bl || !effect_unit_bl || bl->type != BL_PC) {
		return 0;
	}

	sd = BL_CAST(BL_PC, bl);

	if (aura_need_hiding(effect_unit_bl, bl)) {
		return 0;
	}

	clif_specialeffect_single(effect_unit_bl, effect_id, sd->fd);
	return 1;
}

/*==========================================
* Method: aura_effects_timer
* Description: Timer processing function for scheduled playback of non-persistent special effects
* Access: public
* Parameter: aura_effects_timer
* Returns:
* Author: Sola丶小克(CairoLee) 2021/04/24 14:56
 *------------------------------------------*/
TIMER_FUNC(aura_effects_timer) {
	struct block_list* bl = map_id2bl(id);
	if (!bl) {
		delete_timer(tid, aura_effects_timer);
		return 1;
	}

	struct s_unit_common_data* ucd = status_get_ucd(bl);
	if (!ucd) {
		delete_timer(tid, aura_effects_timer);
		return 1;
	}

	for (auto it : ucd->aura.effects) {
		if (it->replay_tid != tid) continue;
		map_foreachinallrange(aura_effects_timer_sub, bl, AREA_SIZE, BL_PC, bl, it->effect_id);
	}

	return 0;
}

/*==========================================  
* Method: aura_need_hiding  
* Description: Determine whether the aura needs to be hidden (or not displayed)  
* Access: public   
* Parameter: struct block_list * bl  
*			This parameter specifies which bl unit's aura needs to be checked for hiding  
* Parameter: struct block_list * observer_bl  
*			The observer's bl pointer (defaults to nullptr indicating no observer, no need to consider cloak effects)  
*			Typically, if the detected bl unit is an NPC,  
*			it may have been hidden/shown in a certain observer_bl's view (cloakonnpc/cloakoffnpc)  
*			Therefore, when determining whether a target bl unit can display an aura, including observer_bl in the judgment will incorporate the observer's perspective  
* Returns: bool  
* Author: Sola丶小克(CairoLee)  2021/12/29 22:46  
 * -----------------------------------------------------------*/
bool aura_need_hiding(struct block_list* bl, struct block_list* observer_bl) {
	if (!bl) return true;

	if (bl->type == BL_PET) return false;

	status_change* sc = status_get_sc(bl);
	if (!sc) return true;
	
	return status_ishiding(bl, observer_bl) || status_isinvisible(bl) || sc->getSCE(SC_CAMOUFLAGE);
}

/*==========================================
* Method: aura_effects_clear
* Description: Remove all currently active halo effects
* Access: public
* Parameter: struct s_unit_common_data * ucd
* Returns: void
* Author: Sola丶小克(CairoLee) 2021/04/23 22:04
 *------------------------------------------*/
void aura_effects_clear(struct block_list* bl) {
	if (!bl) return;

	struct s_unit_common_data* ucd = status_get_ucd(bl);
	if (!ucd) return;

	for (auto &it : ucd->aura.effects) {
		clif_specialeffect_remove(bl, it->effect_id, AREA, bl);
		if (it->replay_tid != INVALID_TIMER) {
			delete_timer(it->replay_tid, aura_effects_timer);
			it->replay_tid = INVALID_TIMER;
		}
	}
	ucd->aura.effects.clear();
}

/*==========================================
* Method: aura_effects_refill
* Description: Fill the special effects into the effective list based on the halo information in ucd
* Access: public
* Parameter: struct s_unit_common_data * ucd
* Returns: void
* Author: Sola丶小克(CairoLee) 2021/04/21 23:23
 *------------------------------------------*/
void aura_effects_refill(struct block_list* bl) {
	if (!bl) return;

	aura_effects_clear(bl);

	struct s_unit_common_data* ucd = status_get_ucd(bl);
	if (!ucd || !ucd->aura.id) return;

	std::shared_ptr<s_aura> aura = aura_search(ucd->aura.id);
	if (!aura) return;

	for (auto it : aura->effects) {
		auto effect = std::make_shared<s_aura_effect>();
		effect->effect_id = it->effect_id;
		effect->replay_interval = it->replay_interval;
		effect->replay_tid = INVALID_TIMER;
		
		if (effect->replay_interval) {
			effect->replay_tid = add_timer_interval(
				gettick() + 100, aura_effects_timer, bl->id, 0, effect->replay_interval
			);
		}
		
		ucd->aura.effects.push_back(effect);
	}
}

/*==========================================
* Method: aura_refresh_client
* Description: Refresh the client so that it can see the visual effects of the halo.
* Access: public
* Parameter: struct block_list * bl
* Returns: void
* Author: Sola丶小克(CairoLee) 2021/04/24 18:16
 *------------------------------------------*/
void aura_refresh_client(struct block_list* bl) {
	if (!bl) return;
	struct map_data* mapdata = map_getmapdata(bl->m);

	switch (bl->type)
	{
	case BL_PC: {
#if PACKETVER_MAIN_NUM >= 20181002 || PACKETVER_RE_NUM >= 20181002
		clif_send_auras(bl, AREA, true, AURA_SPECIAL_NOTHING);
#else
		TBL_PC* sd = map_id2sd(bl->id);
		if (sd) {
			pc_setpos(sd, sd->mapindex, sd->bl.x, sd->bl.y, CLR_OUTSIGHT);
		}
#endif
		break;
	}
	case BL_MOB:
	case BL_PET:
		// If it is a monster or pet, use clif_clearunit_area to directly send CLR_TRICKDEAD to clear the cache.
		// Instead of broadcasting the clif_outsight packet like the else branch,
		// Otherwise, when the unit's halo is replaced during movement, there will be an obvious effect of disappearing and reappearing.
		if (mapdata && mapdata->users) {
			clif_clearunit_area(*bl, CLR_TRICKDEAD);
			map_foreachinallrange(clif_insight, bl, AREA_SIZE, BL_PC, bl);
		}
		break;
	default:
		if (mapdata && mapdata->users) {
			map_foreachinallrange(clif_outsight, bl, AREA_SIZE, BL_PC, bl);
			map_foreachinallrange(clif_insight, bl, AREA_SIZE, BL_PC, bl);
		}
		break;
	}
}

/*==========================================
* Method: aura_make_effective
* Description: Sets the aura for the specified unit and allows it to be refreshed and take effect immediately.
* Access: public
* Parameter: struct block_list * bl
* Parameter: uint32 aura_id
* Parameter: bool pc_saved If it is a player unit, whether to save this aura to the database
* Returns: void
* Author: Sola丶小克(CairoLee) 2020/10/13 00:21
 *------------------------------------------*/
void aura_make_effective(struct block_list* bl, uint32 aura_id, bool pc_saved) {
	if (!bl) return;
	if (aura_id && !aura_search(aura_id)) return;

	map_freeblock_lock();

	struct s_unit_common_data* ucd = status_get_ucd(bl);
	if (ucd) {
		ucd->aura.id = aura_id;
		aura_effects_refill(bl);
	}

	switch (bl->type)
	{
	case BL_PC: {
		map_session_data* sd = BL_CAST(BL_PC, bl);
		if (sd && pc_saved) {
			pc_setglobalreg(sd, add_str(AURA_VARIABLE), aura_id);
		}
		break;
	}
	default:
		break;
	}

	aura_refresh_client(bl);

	map_freeblock_unlock();
}

/*==========================================
* Method: aura_reload
* Description: Reload aura_db.yml aura combination database
* Access: public
* Parameter: void
* Returns: void
* Author: Sola丶小克(CairoLee) 2020/09/26 16:41
 *------------------------------------------*/
void aura_reload(void) {
	aura_db.clear();
	aura_db.load();

	struct block_list* bl = nullptr;
	struct s_mapiterator* iter = mapit_geteachiddb();
	for (bl = (struct block_list*)mapit_first(iter); mapit_exists(iter); bl = (struct block_list*)mapit_next(iter)) {
		aura_effects_refill(bl);
		aura_refresh_client(bl);
	}
	mapit_free(iter);

}

/*==========================================
* Method: do_final_aura
* Description: Release the content saved by the instance object aura_db of AuraDatabase
* Access: public
* Parameter: void
* Returns: void
* Author: Sola丶小克(CairoLee) 2020/09/26 16:41
 *------------------------------------------*/
void do_final_aura(void) {
	aura_db.clear();
}

/*==========================================
* Method: do_init_aura
* Description: Initialize the aura mechanism, load data from the aura_db.yml aura combination database
* Access: public
* Parameter: void
* Returns: void
* Author: Sola丶小克(CairoLee) 2020/09/26 16:41
 *------------------------------------------*/
void do_init_aura(void) {
	add_timer_func_list(aura_effects_timer, "aura_effects_timer");

	aura_db.load();
}
