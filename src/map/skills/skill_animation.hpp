// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder
#ifndef SKILL_ANIMATION_HPP  
#define SKILL_ANIMATION_HPP  
  
#include <common/cbasetypes.hpp>  
#include <common/timer.hpp>  
#include <vector>  
  
struct s_animation_data {  
    int skill_id;  
    int start;  
    int interval;  
    int motion_speed;  
    int motion_count;  
    bool spin;  
};  
  
struct s_environment_data {  
    int skill_id;  
    int target_id;  
    int8 dir;
	bool hit_success;	
};  
  
// Function declarations  
s_animation_data skill_animation_get_info(int skill_id);  
void skill_animation_clear(struct block_list* bl);  
void do_init_skill_animation();  
void do_final_skill_animation();  
int skill_calc_dir_counter_clockwise(int dir);  
TIMER_FUNC(skill_animation_timer);  
bool skill_parse_skill_animation(char* split[], size_t columns, size_t current);  
void skill_clear_animation(struct block_list* bl);  
void clif_skill_damage_animation_hook(struct block_list* src, struct block_list* dst, t_tick tick, int32 sdelay, int32 ddelay, int64 sdamage, uint16 skill_id);
// External declarations  
extern std::vector<s_animation_data> skill_animation_db;  
  
#endif /* SKILL_ANIMATION_HPP */
