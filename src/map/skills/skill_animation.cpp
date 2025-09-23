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
    dir += 2;  
    if (dir >= DIR_MAX)  
        dir = dir - DIR_MAX;  
    return dir;  
}  

struct s_animation_data skill_list[] = {    
    { AS_SONICBLOW, -1, 180, 150, 8, true },    
    { CG_ARROWVULCAN, -1, 200, 100, 9, false },  
    { GC_CROSSIMPACT, -1, 180, 150, 8, true }  
};
  
s_animation_data skill_animation_get_info(int skill_id) {  
    for (int i = 0; i < ARRAYLENGTH(skill_list); i++) {  
        if (skill_list[i].skill_id == skill_id) {  
            return skill_list[i];  
        }  
    }  
    return {}; // Return empty struct if not found  
}  
  
TIMER_FUNC(skill_animation_timer) {    
    struct block_list* bl = map_id2bl(id);    
    if (!bl || bl->type != BL_PC) {    
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
            clif_send_animation_dir(bl, env->target_id, env->dir);    
            env->dir = skill_calc_dir_counter_clockwise(env->dir);    
        }    
    
        sd->skill_animation.step++;    
        sd->skill_animation.tid = add_timer(tick + animation.interval, skill_animation_timer, bl->id, (intptr_t)env);  
    } else {    
        skill_animation_clear(bl);    
        aFree(env);    
    }    
    
    return 0;    
}  
  
void skill_animation_clear(struct block_list* bl) {  
    if (!bl || bl->type != BL_PC)  
        return;  
  
    map_session_data* sd = BL_CAST(BL_PC, bl);  
    if (sd->skill_animation.tid != INVALID_TIMER) {  
        delete_timer(sd->skill_animation.tid, skill_animation_timer);  
        sd->skill_animation.tid = INVALID_TIMER;  
        sd->skill_animation.step = 0;  
    }  
}  

