#ifndef TITLE_HPP  
#define TITLE_HPP  
  
#include <vector>  
#include <common/database.hpp>  
#include <common/db.hpp>  
#include <common/malloc.hpp>  
#include <common/mmo.hpp>  
  
#include "script.hpp"  
#include "status.hpp"  
  
struct s_title_bonus_db {  
    uint16 id;  
    std::string name;  
    int32 icon;  
    struct script_code* script;  
      
    ~s_title_bonus_db() {  
        if (this->script) {  
            script_free_code(this->script);  
            this->script = nullptr;  
        }  
    }  
};  
  
class TitleDatabase : public TypesafeYamlDatabase<uint16, s_title_bonus_db> {  
public:  
    TitleDatabase() : TypesafeYamlDatabase("TITLE_DB", 1) {}
      
    const std::string getDefaultLocation() override;  
    uint64 parseBodyNode(const ryml::NodeRef& node) override;  
};  
  
extern TitleDatabase title_db;  
  
// Core title functions  
void do_init_title();  
void do_final_title();  
  
#endif