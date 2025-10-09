// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "clan.hpp"

#include <cstring> //memset

#include <common/cbasetypes.hpp>
#include <common/database.hpp>
#include <common/malloc.hpp>
#include <common/mmo.hpp>
#include <common/nullpo.hpp>
#include <common/showmsg.hpp>

#include "battle.hpp"
#include "chrif.hpp"
#include "clif.hpp"
#include "instance.hpp"
#include "intif.hpp"
#include "log.hpp"
#include "pc.hpp"
#include "script.hpp"
#include "status.hpp"

const std::string ClanDatabase::getDefaultLocation(){    
	return std::string(db_path) + "/clan_db.yml";    
}    

uint64 ClanDatabase::parseBodyNode(const ryml::NodeRef& node) {    
	int32 id;  
	  
	if (!this->asInt32(node, "Id", id))  
		return 0;  

	std::shared_ptr<clan> clan_entry = this->find(id);  
	bool exists = clan_entry != nullptr;  
	  
	if (!exists) {  
		if (!this->nodesExist(node, { "Name", "Leader" }))  
			return 0;  

		clan_entry = std::shared_ptr<clan>(new clan());  
		clan_entry->id = id;  
	}  

	// Add this missing parsing  
	std::string name;  
	if (this->asString(node, "Name", name)) {  
		safestrncpy(clan_entry->name, name.c_str(), NAME_LENGTH);  
	}  

	std::string leader;  
	if (this->asString(node, "Leader", leader)) {  
		safestrncpy(clan_entry->master, leader.c_str(), NAME_LENGTH);  
	}    

	std::string map;    
	if (this->asString(node, "Map", map)) {    
		safestrncpy(clan_entry->map, map.c_str(), MAP_NAME_LENGTH_EXT);    
	}    

	if (this->nodeExists(node, "MaxMembers")) {  
		int32 max_members_temp;  
		if (!this->asInt32(node, "MaxMembers", max_members_temp))  
			return 0;  
		clan_entry->max_member = static_cast<int16>(max_members_temp);  
	} else {  
		if (!exists)  
			clan_entry->max_member = 500;  
	}

	if (this->nodeExists(node, "KickTime")) {
		int32 kick_time;  
		if (this->asInt32(node, "KickTime", kick_time)) {  
			clan_entry->kick_time = kick_time;  
		}  
	} else {  
		if (!exists)  
			clan_entry->kick_time = 0; // 0 = disabled, no global fallback  
	}  

	if (this->nodeExists(node, "CheckTime")) {  
		int32 check_time;  
		if (this->asInt32(node, "CheckTime", check_time)) {  
			clan_entry->check_time = check_time;  
		}  
	} else {  
		if (!exists)  
			clan_entry->check_time = 3600; // Default 1 hour  
	}

	if (this->nodeExists(node, "Buff")) {    
		const ryml::NodeRef& buffNode = node["Buff"];    

		if (this->nodeExists(buffNode, "Icon")) {  
			std::string icon_name;  
		  
			if (!this->asString(buffNode, "Icon", icon_name))  
				return 0;  

			int64 constant;  

			if (!script_get_constant(icon_name.c_str(), &constant)) {  
				ShowError("EFST constant '" CL_WHITE "%s" CL_RESET "' not found in script constants\n", icon_name.c_str());  
				this->invalidWarning(buffNode["Icon"], "Icon %s is invalid, defaulting to EFST_BLANK.\n", icon_name.c_str());  
				constant = EFST_BLANK;  
			}  

			if (constant < EFST_BLANK || constant >= EFST_MAX) {  
				this->invalidWarning(buffNode["Icon"], "Icon %s is out of bounds, defaulting to EFST_BLANK.\n", icon_name.c_str());  
				constant = EFST_BLANK;  
			}  

			clan_entry->buff_icon = static_cast<int32>(constant);  
		} else {  
			if (!exists)
				clan_entry->buff_icon = EFST_BLANK;  
		}    

		if (this->nodeExists(buffNode, "Script")) {  
			std::string script;  

			if (!this->asString(buffNode, "Script", script))  
				return 0;  

			if (exists && clan_entry->script) {  
				script_free_code(clan_entry->script);  
				clan_entry->script = nullptr;  
			}

			clan_entry->script = parse_script(script.c_str(), this->getCurrentFile().c_str(), this->getLineNumber(buffNode["Script"]), SCRIPT_IGNORE_EXTERNAL_BRACKETS);  

			if (clan_entry->script == nullptr) {  
				ShowError("Failed to compile script for clan '" CL_WHITE "%d" CL_RESET "'.\n", id);  
				return 0;  
			}  
		} else {
			if (!exists)  
				clan_entry->script = nullptr;  
		}

	}

	memset(clan_entry->alliance, 0, sizeof(clan_entry->alliance));    
	int32 alliance_index = 0;  
	  
	if (this->nodeExists(node, "Allies")) {    
		for (const ryml::NodeRef& allyNode : node["Allies"]) {    
			if (alliance_index >= MAX_CLANALLIANCE) {    
				this->invalidWarning(allyNode, "Maximum alliances reached, skipping remaining.\n");    
				break;    
			}    
				
			std::string allyName;    
			allyNode >> allyName;    
				
			clan_entry->alliance[alliance_index].opposition = 0; // 0 = ally    
			clan_entry->alliance[alliance_index].clan_id = 0;  
			safestrncpy(clan_entry->alliance[alliance_index].name, allyName.c_str(), NAME_LENGTH);    
			alliance_index++;    
		}    
	}  
	  
	if (this->nodeExists(node, "Antagonists")) {      
		for (const ryml::NodeRef& antagonistNode : node["Antagonists"]) {      
			if (alliance_index >= MAX_CLANALLIANCE) {      
				this->invalidWarning(antagonistNode, "Maximum alliances reached, skipping remaining.\n");      
				break;      
			}      
				  
			std::string antagonistName;      
			antagonistNode >> antagonistName;      
				  
			clan_entry->alliance[alliance_index].opposition = 1; // 1 = antagonist      
			clan_entry->alliance[alliance_index].clan_id = 0;  
			safestrncpy(clan_entry->alliance[alliance_index].name, antagonistName.c_str(), NAME_LENGTH);      
			alliance_index++;      
		}      
	}    
		
if (!exists)  
	this->put(id, clan_entry);  
	  
return 1;    
};

ClanDatabase clan_db_yaml;

void do_init_clan() {  
    if (!clan_db_yaml.load()) {  
        ShowError("Failed to load clan database from YAML.\n");  
        return;  
    }  

    clan_resolve_alliance_ids();  
      
    ShowStatus("Loaded '" CL_WHITE "%zu" CL_RESET "' clans from YAML database.\n", clan_db_yaml.size());      
}

void do_final_clan(){
	
	clan_db_yaml.clear();
	
}

void clan_resolve_alliance_ids() {  
    for (const auto& pair : clan_db_yaml) {  
        std::shared_ptr<clan> clan_entry = pair.second;  
          
        for (int32 i = 0; i < MAX_CLANALLIANCE; i++) {  
            if (clan_entry->alliance[i].name[0] == '\0' || clan_entry->alliance[i].clan_id > 0) {  
                continue;  
            }  

            struct clan* target_clan = clan_searchname(clan_entry->alliance[i].name);  
            if (target_clan != nullptr) {  
                clan_entry->alliance[i].clan_id = target_clan->id;  
            } else {  
                ShowWarning("Clan alliance resolution: Could not find clan '%s' for clan %d:%s\n",   
                           clan_entry->alliance[i].name, clan_entry->id, clan_entry->name);  
                memset(&clan_entry->alliance[i], 0, sizeof(clan_entry->alliance[i]));  
            }  
        }  
    }  
}

struct clan* clan_search( int32 id ){  
    std::shared_ptr<clan> clan_entry = clan_db_yaml.find(id);  
    return clan_entry ? clan_entry.get() : nullptr;  
}  
  
struct clan* clan_searchname( const char* name ){  
    for (const auto& pair : clan_db_yaml) {  
        if (strncmpi(pair.second->name, name, NAME_LENGTH) == 0) {  
            return pair.second.get();  
        }  
    }  
    return nullptr;  
}

map_session_data* clan_getavailablesd( struct clan& clan ){
	int32 i;

	ARR_FIND( 0, clan.max_member, i, clan.members[i] != nullptr );

	return ( i < clan.max_member ) ? clan.members[i] : nullptr;
}

int32 clan_getMemberIndex( struct clan* clan, uint32 account_id ){
	int32 i;

	nullpo_retr(-1,clan);

	ARR_FIND( 0, clan->max_member, i, clan->members[i] != nullptr && clan->members[i]->status.account_id == account_id );

	if( i == clan->max_member ){
		return -1;
	}else{
		return i;
	}
}

int32 clan_getNextFreeMemberIndex( struct clan* clan ){
	int32 i;

	nullpo_retr(-1,clan);

	ARR_FIND( 0, clan->max_member, i, clan->members[i] == nullptr );

	if( i == clan->max_member ){
		return -1;
	}else{
		return i;
	}
}

void clan_member_joined( map_session_data& sd ){
	if( sd.clan != nullptr ){
		clif_clan_basicinfo( sd );
		clif_clan_onlinecount( *sd.clan );
		return;
	}

	struct clan* clan = clan_search( sd.status.clan_id );

	if( clan == nullptr ){
		return;
	}

	int32 index = clan_getNextFreeMemberIndex( clan );

	if( index >= 0 ){
		sd.clan = clan;
		clan->members[index] = &sd;
		clan->connect_member++;

		clif_clan_basicinfo(sd);

		intif_clan_member_joined(clan->id);
		clif_clan_onlinecount( *clan );
	}
}

void clan_member_left( map_session_data& sd ){
	struct clan* clan = sd.clan;

	if( clan == nullptr ){
		return;
	}

	int32 index = clan_getMemberIndex( clan, sd.status.account_id );

	if( index >= 0 ){
		clan->members[index] = nullptr;
		clan->connect_member--;

		intif_clan_member_left(clan->id);
		clif_clan_onlinecount( *clan );
	}
}

bool clan_member_join( map_session_data& sd, int32 clan_id, uint32 account_id, uint32 char_id ){
    nullpo_ret(&sd);  

    if (sd.status.guild_id > 0 || sd.guild != nullptr) {  
        ShowError("clan_member_join: Player already joined in a guild. char_id: '" CL_WHITE "%d" CL_RESET "'.\n", sd.status.char_id);  
        return false;  
    } else if (sd.status.clan_id > 0 || sd.clan != nullptr) {  
        ShowError("clan_member_join: Player already joined in a clan. char_id: '" CL_WHITE "%d" CL_RESET "'.\n", sd.status.char_id);  
        return false;  
    }  

    if (sd.status.account_id != account_id || sd.status.char_id != char_id) {  
        return false;  
    }  

    struct clan* c = clan_search(clan_id);  
    if (c == nullptr) {  
        ShowError("clan_member_join: Invalid Clan ID: '" CL_WHITE "%d" CL_RESET "'.\n", clan_id);  
        return false;  
    }  

    if (clan_getMemberIndex(c, sd.status.account_id) >= 0) {  
        ShowError("clan_member_join: Player already joined this clan. char_id: '" CL_WHITE "%d" CL_RESET "' clan_id: '" CL_WHITE "%d" CL_RESET "'.\n", sd.status.char_id, clan_id);  
        return false;  
    }  

    if (c->connect_member >= c->max_member) {  
        ShowError("clan_member_join: Clan '" CL_WHITE "%s" CL_RESET "' already reached its max capacity!.\n", c->name);  
        return false;  
    }  

    if (c->instance_id > 0 && battle_config.instance_block_invite) {  
        return false;  
    }  

    sd.status.clan_id = clan_id;  
    clan_member_joined(sd);
	
    clan_buff_start(&sd, c);

    status_calc_pc(&sd, SCO_FORCE);	

    chrif_save(&sd, 0);  

    return true;
}


bool clan_member_leave(map_session_data& sd, int32 clan_id, uint32 account_id, uint32 char_id) {  
    nullpo_ret(&sd);  
      
    struct clan* c = sd.clan;  
      
    if (c == nullptr) {  
        return false;  
    }  

    if (sd.status.account_id != account_id || sd.status.char_id != char_id || sd.status.clan_id != clan_id) {  
        return false;  
    }  

    if (c->instance_id > 0 && battle_config.instance_block_leave) {  
        return false;  
    }  

    clan_member_left(sd);

    sd.clan = nullptr;  
    sd.status.clan_id = 0;  

    clan_buff_end(&sd, c);  

    status_change_end(&sd, SC_CLAN_INFO, INVALID_TIMER);  

    status_calc_pc(&sd, SCO_FORCE);  

    chrif_save(&sd, 0);  

    clif_clan_leave(sd);  
      
    return true;  
}

void clan_recv_message( int32 clan_id, uint32 account_id, const char *mes, size_t len ){
	struct clan *clan = clan_search( clan_id );

	if( clan == nullptr ){
		return;
	}

	clif_clan_message( *clan, mes, len );
}

void clan_send_message( map_session_data& sd, const char *mes, size_t len ){  
	if( sd.clan == nullptr ){  
		return;  
	}  
  
	intif_clan_message( sd.status.clan_id, sd.status.account_id, mes, len );
	clan_recv_message( sd.status.clan_id, sd.status.account_id, mes, len );
	log_chat( LOG_CHAT_CLAN, sd.status.clan_id, sd.status.char_id, sd.status.account_id, mapindex_id2name( sd.mapindex ), sd.x, sd.y, nullptr, mes );  
}

int32 clan_get_alliance_count( struct clan& clan, int32 flag ){
	int32 count = 0;

	for( int32 i = 0; i < MAX_CLANALLIANCE; i++ ){
		if(	clan.alliance[i].clan_id > 0 && clan.alliance[i].opposition == flag ){
			count++;
		}
	}

	return count;
}

void clan_buff_start(map_session_data* sd, struct clan* c) {      
    nullpo_retv(sd);      
    nullpo_retv(c);      
      
    if (c->buff_icon <= 0) {      
        return;      
    }      
    
    sc_type sc_constant;      
    switch(c->buff_icon) {      
        case EFST_SWORDCLAN: sc_constant = SC_SWORDCLAN; break;      
        case EFST_ARCWANDCLAN: sc_constant = SC_ARCWANDCLAN; break;      
        case EFST_GOLDENMACECLAN: sc_constant = SC_GOLDENMACECLAN; break;      
        case EFST_CROSSBOWCLAN: sc_constant = SC_CROSSBOWCLAN; break;      
        case EFST_JUMPINGCLAN: sc_constant = SC_JUMPINGCLAN; break;      
        default:       
            ShowError("Unknown clan buff_icon: '" CL_WHITE "%d" CL_RESET "'.\n", c->buff_icon);  
            return;      
    }    

    if (!sd->sc.getSCE(sc_constant)) {  
		status_change_start(sd, sd, sc_constant, 10000, 0, c->id, 0, 0, INFINITE_TICK, SCSTART_NOAVOID);
        clif_status_change(sd, static_cast<int32>(c->buff_icon), 1, INFINITE_TICK, 0, c->id, 0);
    }  

    if (c->script) {      
        run_script(c->script, 0, sd->id, 0);      
    }      
}  
  
void clan_buff_end(map_session_data* sd, struct clan* c) {      
    nullpo_retv(sd);      
    nullpo_retv(c);      
      
    if (c->buff_icon <= 0) {      
        return;      
    }      

    sc_type sc_constant;    
    switch(c->buff_icon) {    
        case EFST_SWORDCLAN: sc_constant = SC_SWORDCLAN; break;    
        case EFST_ARCWANDCLAN: sc_constant = SC_ARCWANDCLAN; break;    
        case EFST_GOLDENMACECLAN: sc_constant = SC_GOLDENMACECLAN; break;    
        case EFST_CROSSBOWCLAN: sc_constant = SC_CROSSBOWCLAN; break;    
        case EFST_JUMPINGCLAN: sc_constant = SC_JUMPINGCLAN; break;    
        default:     
            return;
    }  

    status_change_end(sd, sc_constant);      
    clif_status_change(sd, static_cast<int32>(c->buff_icon), 0, 0, 0, 0, 0);
}
