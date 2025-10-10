// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder
#include "skill_animation.hpp"
#include "../skill.hpp"
#include "../clif.hpp"
#include "../map.hpp"
#include "../pc.hpp"
#include "../unit.hpp"
#include <common/showmsg.hpp>
#include <common/strlib.hpp>
#include <common/utilities.hpp>

using namespace rathena;
  
std::vector<s_animation_data> skill_animation_db;  
  
int skill_calc_dir_counter_clockwise(int dir) {  
    if (dir < 0 || dir >= DIR_MAX) { 
		// Invalid direction
        return -1; 
    }
    dir += 2;  
    if (dir >= DIR_MAX)  
        dir = dir - DIR_MAX;  
    return dir;  
}  

struct s_animation_data skill_list[] = {    
    { CG_ARROWVULCAN, -1, 200, 100, 9, false },
    { AS_SONICBLOW, -1, 180, 150, 8, true },      
    { GC_CROSSIMPACT, -1, 180, 150, 7, true },  
    { SHC_SAVAGE_IMPACT, -1, 180, 150, 3, true },  
    { HN_MEGA_SONIC_BLOW, -1, 180, 150, 8, true }  
};
  
s_animation_data skill_animation_get_info(int skill_id) {  
    for (int i = 0; i < ARRAYLENGTH(skill_list); i++) {  
        if (skill_list[i].skill_id == skill_id) {  
            // Validate animation data before returning  
            if (skill_list[i].motion_count <= 0 || skill_list[i].interval <= 0) {  
                ShowWarning("skill_animation_get_info: Invalid animation data for skill '" CL_WHITE "%d" CL_RESET "'.\n", skill_id);
				// Return empty struct for invalid data  				
                return {}; 
            }  
            return skill_list[i];  
        }  
    }
	// Return empty struct if not found  	
    return {};
}  
  
TIMER_FUNC(skill_animation_timer) {    
    struct block_list* bl = map_id2bl(id);    
    if (!bl) {    
        // Entity no longer exists, clean up environment data  
        struct s_environment_data* env = (struct s_environment_data*)data;  
        if (env) {  
            aFree(env);  
        }
        return 0;    
    }    
    
    struct s_environment_data* env = (struct s_environment_data*)data;    
    if (!env) {    
        return 0;    
    }    
    
    s_animation_data animation = skill_animation_get_info(env->skill_id);    
    if (animation.skill_id == 0) {    
        aFree(env);    
        return 0;    
    }    
    if (bl->type == BL_PC) {    
		map_session_data* sd = BL_CAST(BL_PC, bl);    
		
		if (sd->skill_animation.step < animation.motion_count) {    
			if (env->hit_success) {  
				// Show damage animation for hits  
				clif_send_animation_motion(bl, env->target_id, animation.motion_speed);  
			} else {  
				// Show miss animation - use type 11 (lucky dodge/miss)  
				clif_send_miss_motion(bl, env->target_id);  
			}  
			  
			if (animation.spin && env->dir != -1) {    
                int new_dir = skill_calc_dir_counter_clockwise(env->dir);  
                if (new_dir != -1) { // Only update if valid direction
					clif_send_animation_dir(bl, env->target_id, env->dir);    
					env->dir = skill_calc_dir_counter_clockwise(env->dir);    
				}
			}    
		
			sd->skill_animation.step++;    
			sd->skill_animation.tid = add_timer(tick + animation.interval, skill_animation_timer, bl->id, (intptr_t)env);  
		} else {    
			skill_animation_clear_internal(bl);
			aFree(env);    
		}    
    } else {  
        // For non-PC entities, just clean up after one iteration  
        // since we don't track animation state for them  
        if (env->hit_success) {  
            clif_send_animation_motion(bl, env->target_id, animation.motion_speed);  
        } else {  
            clif_send_miss_motion(bl, env->target_id);  
        }  
        aFree(env);  
    }
    return 0;    
}  

static void skill_animation_clear_internal(struct block_list* bl) {    
    if (!bl)    
        return;    
   
    if (bl->type == BL_PC) {    
        map_session_data* sd = BL_CAST(BL_PC, bl);    
        if (sd->skill_animation.tid != INVALID_TIMER) {    
            sd->skill_animation.tid = INVALID_TIMER;    
            sd->skill_animation.step = 0;    
        }  
    }  
}
  
void skill_animation_clear(struct block_list* bl) {  
    if (!bl)  
        return;  
    if (bl->type == BL_PC) {  
		map_session_data* sd = BL_CAST(BL_PC, bl);  
		if (sd->skill_animation.tid != INVALID_TIMER) {  
			delete_timer(sd->skill_animation.tid, skill_animation_timer);  
			sd->skill_animation.tid = INVALID_TIMER;  
			sd->skill_animation.step = 0;  
		}
	}	
}  

