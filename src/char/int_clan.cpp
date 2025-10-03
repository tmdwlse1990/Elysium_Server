// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "int_clan.hpp"

#include <cstdlib>
#include <cstring> //memset
#include <memory>
#include <unordered_map>
#include <vector>

#include <common/cbasetypes.hpp>
#include <common/malloc.hpp>
#include <common/mmo.hpp>
#include <common/nullpo.hpp>
#include <common/showmsg.hpp>
#include <common/socket.hpp>
#include <common/strlib.hpp>

#include "char.hpp"
#include "char_mapif.hpp"
#include "inter.hpp"

#include "../map/clan.hpp"

// int32 clan_id -> struct clan*
static std::unordered_map<int32, std::shared_ptr<struct clan>> clan_db;

using namespace rathena;

static int32 mapif_parse_clan_message( int32 fd ){
	unsigned char buf[500];
	uint16 len;

	len = RFIFOW(fd,2);

	WBUFW(buf,0) = 0x38A1;
	memcpy( WBUFP(buf,2), RFIFOP(fd,2), len-2 );

	chmapif_sendallwos( fd, buf, len );

	return 0;
}

static void mapif_clan_refresh_onlinecount( int32 fd, std::shared_ptr<struct clan> clan ){
	unsigned char buf[8];

	WBUFW(buf,0) = 0x38A2;
	WBUFL(buf,2) = clan->id;
	WBUFW(buf,6) = clan->connect_member;

	chmapif_sendallwos( fd, buf, 8 );
}

static void mapif_parse_clan_member_left( int32 fd ){
	std::shared_ptr<struct clan> clan = util::umap_find( clan_db, static_cast<int32>( RFIFOL( fd, 2 ) ) );

	// Unknown clan
	if( clan == nullptr ){
		return;
	}

	if( clan->connect_member > 0 ){
		clan->connect_member--;

		mapif_clan_refresh_onlinecount( fd, clan );
	}
}

static void mapif_parse_clan_member_joined( int32 fd ){
	std::shared_ptr<struct clan> clan = util::umap_find( clan_db, static_cast<int32>( RFIFOL( fd, 2 ) ) );

	// Unknown clan
	if( clan == nullptr ){
		return;
	}

	clan->connect_member++;

	mapif_clan_refresh_onlinecount( fd, clan );
}

// Communication from the map server
// - Can analyzed only one by one packet
// Data packet length that you set to inter.cpp
//- Shouldn't do checking and packet length, RFIFOSKIP is done by the caller
// Must Return
//	1 : ok
//  0 : error
int32 inter_clan_parse_frommap( int32 fd ){
	switch(RFIFOW(fd,0)) {		
		case 0x30A1:
			mapif_parse_clan_message( fd );
			return 1;
		case 0x30A2:
			mapif_parse_clan_member_left( fd );
			return 1;
		case 0x30A3:
			mapif_parse_clan_member_joined( fd );
			return 1;
		default:
			return 0;
	}
}

int32 inter_clan_init(void){  
    // Character server no longer loads clan data from SQL  
    // Clan data is now handled by map server via YAML  
    ShowStatus("Clan inter-server messaging system initialized.\n");  
    return 0;  
}

void inter_clan_final(){
	clan_db.clear();
}
