#ifndef COLLECTION_HPP  
#define COLLECTION_HPP  
  
#include <vector>  
  
#include <common/database.hpp>  
#include <common/db.hpp>  
#include <common/malloc.hpp>  
#include <common/mmo.hpp>  
  
#include "script.hpp"  
#include "status.hpp"  
  
struct s_collection_item {  
    t_itemid nameid;  
    uint16 amount;  
    char refine;  
    bool withdraw;  
    int32 collection_fee;  
    int32 withdraw_fee;  
    struct script_code* script;  
  
    ~s_collection_item() {  
        if (this->script) {  
            script_free_code(this->script);  
            this->script = nullptr;  
        }  
    }  
};  
  
struct s_collection_combo {
	std::string name;
    std::vector<t_itemid> items;  
    uint16 amount;  
    char refine;  
    bool withdraw;  
    int32 collection_fee;  
    int32 withdraw_fee;  
    struct script_code* script;
      
    ~s_collection_combo() {  
        if (this->script) {  
            script_free_code(this->script);  
            this->script = nullptr;  
        }  
    }  
};  
  
struct s_collection_stor {  
    uint16 stor_id;  
    bool withdraw;  
    bool bound;  
    int32 collection_fee;  
    int32 withdraw_fee;  
    std::vector<std::shared_ptr<s_collection_item>> items;  
    std::vector<std::shared_ptr<s_collection_combo>> combos;
	std::vector<bool> active_combos;	
};  
  
class CollectionDatabase : public TypesafeCachedYamlDatabase<uint16, s_collection_stor> {  
public:  
    CollectionDatabase() : TypesafeCachedYamlDatabase("COLLECTION_DB", 3, 1) {  
  
    }  
  
    const std::string getDefaultLocation() override;  
    uint64 parseBodyNode(const ryml::NodeRef& node) override;  
  
    // Additional  
    std::shared_ptr<s_collection_item> findItemInStor(uint16 stor_id, t_itemid nameid);  
};  
  
extern CollectionDatabase collection_db;
  
void collection_counter(map_session_data* sd, int type, int val1, int val2);  
void collection_save(map_session_data* sd, bool calc = true);  
void collection_load_combo_states(map_session_data* sd);
bool collection_validate_combo(map_session_data* sd, uint16 stor_id, size_t combo_index, bool check_active_state = true);
void do_init_collection(void);  
void do_final_collection(void);  
void do_reload_collection(void);  
  
#endif
