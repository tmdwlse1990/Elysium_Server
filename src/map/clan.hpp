// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL  
// For more information, see LICENCE in the main folder  
  
#ifndef CLAN_HPP  
#define CLAN_HPP  
  
#include <string>    
#include <vector>    
#include <memory>  
#include <ctime>  
  
#include <common/database.hpp>  
#include <common/mmo.hpp>  
  
#include "script.hpp"  
  
class map_session_data;  
  
struct clan_member {  
    map_session_data* sd;  
    int32 char_id;  
    int32 online;  
    time_t last_login;  
};  
  
struct clan_alliance {  
	int32 opposition;  
	int32 clan_id;  
	char name[NAME_LENGTH];  
};  
    
struct clan {    
	int32 id;
	char name[NAME_LENGTH];
	char master[NAME_LENGTH];
	char map[MAP_NAME_LENGTH_EXT];
	int16 max_member, connect_member;
	map_session_data *members[MAX_CLAN];
	struct clan_alliance alliance[MAX_CLANALLIANCE];
	uint16 instance_id;
    int32 kick_time;  
    int32 check_time;    
    int32 buff_icon;	  
    char constant[NAME_LENGTH];  // For clan constants like "SWORDCLAN"    
    struct script_code* script;
	  
    // Additional fields for YAML support    
    std::vector<std::string> allies;    
    std::vector<std::string> antagonists;  
  
    ~clan() {    
        if (this->script) {    
            script_free_code(this->script);    
            this->script = nullptr;    
        }    
    } 	  
};  
  
class ClanDatabase : public TypesafeYamlDatabase<int32, clan> {    
public:    
    ClanDatabase() : TypesafeYamlDatabase("CLAN_DB", 1) {}    
        
    const std::string getDefaultLocation() override;    
    uint64 parseBodyNode(const ryml::NodeRef& node) override;    
};    
    
extern ClanDatabase clan_db_yaml;  
  
// Core clan functions  
void do_init_clan();  
void do_final_clan();  
struct clan* clan_search( int32 id );  
struct clan* clan_searchname( const char* name );
void clan_resolve_alliance_ids();
  
// Enhanced member management functions
void clan_member_joined( map_session_data& sd );
void clan_member_left( map_session_data& sd );
bool clan_member_join( map_session_data& sd, int32 clan_id, uint32 account_id, uint32 char_id );
bool clan_member_leave( map_session_data& sd, int32 clan_id, uint32 account_id, uint32 char_id );      
  
// Communication functions  
void clan_send_message( map_session_data& sd, const char *mes, size_t len );
void clan_recv_message( int32 clan_id, uint32 account_id, const char *mes, size_t len );  

// Alliance Count
map_session_data* clan_getavailablesd( struct clan& clan );
int32 clan_get_alliance_count( struct clan& clan, int32 flag );
  
// Buff system  
void clan_buff_start(map_session_data* sd, struct clan* c);  
void clan_buff_end(map_session_data* sd, struct clan* c);
  
#endif /* CLAN_HPP */