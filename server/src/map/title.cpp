#include "title.hpp"

#include <stdlib.h>

#include <common/malloc.hpp>
#include <common/nullpo.hpp>
#include <common/showmsg.hpp>
#include <common/strlib.hpp>
#include <common/utils.hpp>

#include "pc.hpp"

const std::string TitleDatabase::getDefaultLocation() {  
    return std::string(db_path) + "/title_db.yml";  
}  
  
uint64 TitleDatabase::parseBodyNode(const ryml::NodeRef& node) {  
    uint16 id;  
      
    if (!this->asUInt16(node, "ID", id))  
        return 0;  
          
    std::shared_ptr<s_title_bonus_db> title_entry = this->find(id);  
    bool exists = title_entry != nullptr;  
      
    if (!exists) {  
        if (!this->nodesExist(node, { "Name" }))  
            return 0;  
              
        title_entry = std::make_shared<s_title_bonus_db>();  
        title_entry->id = id;  
    }  
      
    if (this->nodeExists(node, "Name")) {  
        std::string name;  
        if (!this->asString(node, "Name", name))  
            return 0;  
        title_entry->name = name;  
    }  

    if (this->nodeExists(node, "Icon")) {  
        std::string icon_name;  
          
        if (!this->asString(node, "Icon", icon_name))  
            return 0;  
          
        int64 constant;  
          
        if (!script_get_constant(icon_name.c_str(), &constant)) {  
            ShowError("EFST constant '%s' not found in script constants\n", icon_name.c_str());  
            this->invalidWarning(node["Icon"], "Icon %s is invalid, defaulting to EFST_BLANK.\n", icon_name.c_str());  
            constant = EFST_BLANK;  
        }  
  
        if (constant < EFST_BLANK || constant >= EFST_MAX) {  
            this->invalidWarning(node["Icon"], "Icon %s is out of bounds, defaulting to EFST_BLANK.\n", icon_name.c_str());  
            constant = EFST_BLANK;  
        }  
          
        title_entry->icon = static_cast<int32>(constant);  
    } else {  
        if (!exists)  
            title_entry->icon = EFST_BLANK;  
    }  
      
    if (this->nodeExists(node, "Script")) {  
        std::string script;  
        if (!this->asString(node, "Script", script))  
            return 0;  
              
        if (exists && title_entry->script) {  
            script_free_code(title_entry->script);  
            title_entry->script = nullptr;  
        }  
          
        title_entry->script = parse_script(script.c_str(), this->getCurrentFile().c_str(),   
                                         this->getLineNumber(node["Script"]), SCRIPT_IGNORE_EXTERNAL_BRACKETS);  
          
        if (title_entry->script == nullptr) {  
            ShowError("Failed to compile script for title %d\n", id);  
            return 0;  
        }  
    } else {  
        if (!exists)  
            title_entry->script = nullptr;  
    }  
      
    if (!exists)  
        this->put(id, title_entry);  
          
    return 1;  
}

TitleDatabase title_db;

void do_init_title() {  
    title_db.load();  
    ShowStatus("Loaded title database.\n");  
}  
  
void do_final_title() {  
    title_db.clear();  
}


