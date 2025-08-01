#include "collection.hpp"

#include <stdlib.h>

#include "../common/malloc.hpp"
#include "../common/nullpo.hpp"
#include "../common/showmsg.hpp"
#include "../common/strlib.hpp"
#include "../common/utils.hpp"

#include "pc.hpp"
#include "storage.hpp"

const std::string CollectionDatabase::getDefaultLocation()
{
	return std::string(db_path) + "/collection_db.yml";
}

uint64 CollectionDatabase::parseBodyNode(const ryml::NodeRef& node)  
{  
    uint16 stor_id;  
  
    if (!this->asUInt16(node, "StorageId", stor_id))  
        return 0;  
  
    std::shared_ptr<s_collection_stor> collection = this->find(stor_id);  
    bool exists = collection != nullptr;  
  
    if (!exists)  
    {  
        collection = std::make_shared<s_collection_stor>();  
        collection->stor_id = stor_id;  
  
        if (this->nodeExists(node, "Bound"))  
        {  
            bool bound;  
            if (!this->asBool(node, "Bound", bound))  
                return 0;  
            collection->bound = bound;  
        }  
        else  
        {  
            collection->bound = true;  
        }  
    }  
  
    // Parse storage-level properties  
    if (this->nodeExists(node, "Withdraw"))  
    {  
        bool withdraw;  
        if (!this->asBool(node, "Withdraw", withdraw))  
            return 0;  
        collection->withdraw = withdraw;  
    }  
    else  
    {  
        if (!exists)  
            collection->withdraw = true;  
    }  
  
    if (this->nodeExists(node, "CollectionFee"))  
    {  
        int32 collection_fee;  
        if (!this->asInt32(node, "CollectionFee", collection_fee))  
            return 0;  
        collection->collection_fee = collection_fee;  
    }  
    else  
    {  
        if (!exists)  
            collection->collection_fee = 0;  
    }  
  
    if (this->nodeExists(node, "WithdrawFee"))  
    {  
        int32 withdraw_fee;  
        if (!this->asInt32(node, "WithdrawFee", withdraw_fee))  
            return 0;  
        collection->withdraw_fee = withdraw_fee;  
    }  
    else  
    {  
        if (!exists)  
            collection->withdraw_fee = 0;  
    }  
  
    // Parse combos with individual properties  
    if (this->nodeExists(node, "Combos"))  
    {  
        for (const ryml::NodeRef& comboIt : node["Combos"])  
        {  
            static const std::string nodeName = "Combo";  
              
            if (!this->nodesExist(comboIt, { nodeName }))  
                return 0;  
              
            const ryml::NodeRef& comboNode = comboIt["Combo"];  
              
            if (!comboNode.is_seq())  
            {  
                this->invalidWarning(comboNode, "%s should be a sequence.\n", nodeName.c_str());  
                return 0;  
            }  
              
            std::vector<t_itemid> combo_items = {};  
              
            // Parse items directly from the sequence  
            for (const auto itemIt : comboNode)  
            {  
                std::string item_name;  
                c4::from_chars(itemIt.val(), &item_name);  
                  
                std::shared_ptr<item_data> item = item_db.search_aegisname(item_name.c_str());  
                  
                if (item == nullptr)  
                {  
                    this->invalidWarning(itemIt, "Invalid item %s, skipping.\n", item_name.c_str());  
                    continue;  
                }  
                  
                combo_items.push_back(item->nameid);  
            }  
              
            if (combo_items.empty())  
            {  
                this->invalidWarning(comboNode, "Empty combo, skipping.\n");  
                continue;  
            }  
              
            if (combo_items.size() < 2)  
            {  
                this->invalidWarning(comboNode, "Not enough items to make a combo (need at least 2). Skipping.\n");  
                continue;  
            }  
              
            // Store the combo  
            std::sort(combo_items.begin(), combo_items.end());  
              
            std::shared_ptr<s_collection_combo> combo_entry = std::make_shared<s_collection_combo>();  
            combo_entry->items = combo_items;  

			if (this->nodeExists(comboIt, "Name")) {    
				std::string combo_name;    
				if (!this->asString(comboIt, "Name", combo_name))    
					return 0;    
				combo_entry->name = combo_name;    
			} else {    
				combo_entry->name = "Unnamed Combo";  // Default fallback    
			}

            // Parse combo-level properties  
            if (this->nodeExists(comboIt, "Amount"))  
            {  
                uint16 amount;  
                if (!this->asUInt16(comboIt, "Amount", amount))  
                    return 0;  
                combo_entry->amount = amount;  
            }  
            else  
            {  
                combo_entry->amount = 1;  
            }  
              
            if (this->nodeExists(comboIt, "Refine"))  
            {  
                uint16 refine;  
                if (!this->asUInt16(comboIt, "Refine", refine))  
                    return 0;  
                if (refine > MAX_REFINE)  
                {  
                    this->invalidWarning(comboIt["Refine"], "Refine level %hu is invalid, capping to MAX_REFINE.\n", refine);  
                    refine = MAX_REFINE;  
                }  
                combo_entry->refine = (char)refine;  
            }  
            else  
            {  
                combo_entry->refine = 0;  
            }  
              
            if (this->nodeExists(comboIt, "Withdraw"))  
            {  
                bool withdraw;  
                if (!this->asBool(comboIt, "Withdraw", withdraw))  
                    return 0;  
                combo_entry->withdraw = withdraw;  
            }  
            else  
            {  
                combo_entry->withdraw = true;  
            }  
              
            if (this->nodeExists(comboIt, "CollectionFee"))  
            {  
                int32 collection_fee;  
                if (!this->asInt32(comboIt, "CollectionFee", collection_fee))  
                    return 0;  
                combo_entry->collection_fee = collection_fee;  
            }  
            else  
            {  
                combo_entry->collection_fee = 0;  
            }  
              
            if (this->nodeExists(comboIt, "Script"))  
            {  
                std::string script;  
                if (!this->asString(comboIt, "Script", script))  
                    return 0;  
                  
                combo_entry->script = parse_script(script.c_str(), this->getCurrentFile().c_str(), this->getLineNumber(comboIt["Script"]), SCRIPT_IGNORE_EXTERNAL_BRACKETS);  
            }  
              
            collection->combos.push_back(combo_entry);

        }  
    }

	collection->active_combos.resize(collection->combos.size(), false);	
  
    // Parse individual items (separate from combos)  
    if (this->nodeExists(node, "Items"))  
    {  
        for (const ryml::NodeRef& it : node["Items"])  
        {  
            std::string item_name;  
  
            if (!this->asString(it, "Item", item_name))  
            {  
                this->invalidWarning(it["Item"], "Invalid item name format.\n");  
                return 0;  
            }  
  
            std::shared_ptr<item_data> item = item_db.search_aegisname(item_name.c_str());  
  
            if (item == nullptr)  
            {  
                this->invalidWarning(it["Item"], "collection item %s does not exist, skipping.\n", item_name.c_str());  
                continue;  
            }  
  
            std::shared_ptr<s_collection_item> entry = nullptr;  
            bool new_entry = true;  
  
            for (std::shared_ptr<s_collection_item> ditem : collection->items)  
            {  
                if (ditem->nameid == item->nameid)  
                {  
                    entry = ditem;  
                    new_entry = false;  
                    break;  
                }  
            }  
  
            if (new_entry)  
            {  
                entry = std::make_shared<s_collection_item>();  
                entry->nameid = item->nameid;  
            }  
  
            // Parse individual item properties (same as before)  
            if (this->nodeExists(it, "Amount"))  
            {  
                uint16 amount;  
                if (!this->asUInt16(it, "Amount", amount))  
                    return 0;  
  
                if (amount > MAX_AMOUNT)  
                {  
                    this->invalidWarning(it["Amount"], "Amount exceeds MAX_AMOUNT. Capping...\n");  
                    amount = MAX_AMOUNT;  
                }  
  
                entry->amount = amount;  
            }  
            else  
            {  
                if (new_entry)  
                    entry->amount = 1;  
            }  
  
            if (this->nodeExists(it, "Refine"))  
            {  
                if (item->flag.no_refine)  
                {  
                    this->invalidWarning(it["Refine"], "Item %s is not refineable.\n", item->name.c_str());  
                    return 0;  
                }  
  
                uint16 refine;  
                if (!this->asUInt16(it, "Refine", refine))  
                    return 0;  
  
                if (refine > MAX_REFINE)  
                {  
                    this->invalidWarning(it["Refine"], "Refine level %hu is invalid, skipping.\n", refine);  
                    return 0;  
                }  
  
                entry->refine = (char)refine;  
            }  
            else  
            {  
                if (new_entry)  
                    entry->refine = 0;  
            }  
  
            if (this->nodeExists(it, "Withdraw"))  
            {  
                bool withdraw;  
                if (!this->asBool(it, "Withdraw", withdraw))  
                    return 0;  
                entry->withdraw = withdraw;  
            }  
            else  
            {  
                if (new_entry)  
                    entry->withdraw = false;  
            }  
  
            if (this->nodeExists(it, "CollectionFee"))  
            {  
                int32 collection_fee;  
                if (!this->asInt32(it, "CollectionFee", collection_fee))  
                    return 0;  
                entry->collection_fee = collection_fee;  
            }  
            else  
            {  
                if (new_entry)  
                    entry->collection_fee = 0;  
            }  
  
            if (this->nodeExists(it, "Script"))  
            {  
                std::string script;  
                if (!this->asString(it, "Script", script))  
                    return 0;  
  
                if (!new_entry && entry->script)  
                {  
                    script_free_code(entry->script);  
                    entry->script = nullptr;  
                }  
  
                entry->script = parse_script(script.c_str(), this->getCurrentFile().c_str(), this->getLineNumber(it["Script"]), SCRIPT_IGNORE_EXTERNAL_BRACKETS);  
            }  
            else  
            {  
                if (new_entry)  
                    entry->script = nullptr;  
            }  
  
            if (new_entry)  
                collection->items.push_back(entry);  
        }  
    }  
  
	if (!exists) {  
		if (collection->items.empty() && collection->combos.empty()) {  
			return 1;
		}  
		this->put(stor_id, collection);  
	}
	return 1;	
}

std::shared_ptr<s_collection_item> CollectionDatabase::findItemInStor(uint16 stor_id, t_itemid nameid)  
{  
    std::shared_ptr<s_collection_stor> collection = this->find(stor_id);  
    if (collection == nullptr)  
        return nullptr;  
  
    // First check individual items  
    for (std::shared_ptr<s_collection_item> entry : collection->items)  
    {  
        if (entry->nameid == nameid)  
            return entry;  
    }  
      
    // Then check combo items  
    for (std::shared_ptr<s_collection_combo> combo : collection->combos)  
    {  
        for (t_itemid combo_item : combo->items)  
        {  
            if (combo_item == nameid)  
            {  
                // Create a properly initialized collection item for combo items  
                std::shared_ptr<s_collection_item> temp_entry = std::make_shared<s_collection_item>();  
                temp_entry->nameid = nameid;  
                temp_entry->amount = combo->amount;  
                temp_entry->refine = combo->refine;  
                temp_entry->withdraw = combo->withdraw;  
                temp_entry->collection_fee = combo->collection_fee;  
                temp_entry->withdraw_fee = collection->withdraw_fee; // Use storage-wide withdraw fee  
                temp_entry->script = nullptr; // Don't share script pointers to avoid double-free  
                return temp_entry;  
            }  
        }  
    }  
      
    return nullptr;  
}

CollectionDatabase collection_db;

void collection_counter(map_session_data* sd, int type, int val1, int val2)
{
	if (!sd)
		return;

	if (!sd->collection.calc)
		return;

	std::vector<s_collection_bonus>& bonus = sd->collection.bonus;
	for (auto& it : bonus)
	{
		if (it.type == type)
		{
			if (val2 && it.val1 == val1)
			{
				it.val2 = it.val2 + val2;
				return;
			}
			else {
				it.val1 = it.val1 + val1;
				return;
			}
		}
	}

	struct s_collection_bonus entry = {};
	entry.type = type;

	if (val2)
	{
		entry.val1 = val1;
		entry.val2 += val2;
	}
	else
		entry.val1 += val1;

	bonus.push_back(entry);

	return;
}

void collection_save(map_session_data* sd, bool calc)  
{  
	struct s_storage* stor = &sd->premiumStorage;  
  
	std::shared_ptr<s_collection_stor> collection = collection_db.find(stor->stor_id);  
	if (collection == nullptr)  
		return;  
  
	sd->collection.items[stor->stor_id].clear();  
  
	// Process individual collection items first  
	for (int i = 0; i < stor->max_amount; i++)  
	{  
		struct item* it = &stor->u.items_storage[i];  
  
		if (it->nameid == 0)  
			continue;  
  
		std::shared_ptr<s_collection_item> entry = collection_db.findItemInStor(stor->stor_id, it->nameid);  
  
		if (entry != nullptr)  
		{  
			struct s_collection_items new_entry = {};  
  
			new_entry.nameid = it->nameid;  
			new_entry.amount = it->amount;  
			new_entry.refine = it->refine;  
  
			sd->collection.items[stor->stor_id].push_back(new_entry);  
		}  
	}  
	  
	// Process combo items to ensure they're tracked for display purposes  
	for (const auto& combo : collection->combos) {    
		for (t_itemid combo_item : combo->items) {    
			// Check if this combo item exists in storage    
			for (int i = 0; i < stor->max_amount; i++) {    
				struct item* it = &stor->u.items_storage[i];    
				if (it->nameid == combo_item && it->nameid != 0) {    
					// Check if already tracked (from individual items or previous combo items)  
					auto existing = std::find_if(sd->collection.items[stor->stor_id].begin(),     
						sd->collection.items[stor->stor_id].end(),    
						[combo_item](const s_collection_items& item) {     
							return item.nameid == combo_item;     
						});    
					    
					if (existing == sd->collection.items[stor->stor_id].end()) {    
						// Only add if not already tracked  
						struct s_collection_items new_entry = {};    
						new_entry.nameid = it->nameid;    
						new_entry.amount = it->amount;    
						new_entry.refine = it->refine;    
						sd->collection.items[stor->stor_id].push_back(new_entry);    
					} else {  
						// Update existing entry with current storage data  
						existing->amount = it->amount;  
						existing->refine = it->refine;  
					}  
					break; // Found the item, no need to continue this inner loop  
				}    
			}    
		}    
	}	  
  
	if (calc)  
		status_calc_pc(sd, SCO_NONE);  
}

void collection_load_combo_states(map_session_data* sd) {  
    if (SQL_ERROR == Sql_Query(mmysql_handle,  
        "SELECT `stor_id`, `combo_index`, `is_active` FROM `collection_combos` "  
        "WHERE `account_id` = '%d' AND `char_id` = '%d'",  
        sd->status.account_id, sd->status.char_id)) {  
        Sql_ShowDebug(mmysql_handle);  
        return;  
    }  
  
    while (SQL_SUCCESS == Sql_NextRow(mmysql_handle)) {  
        char* data;  
        int stor_id, combo_index, is_active;  
          
        Sql_GetData(mmysql_handle, 0, &data, NULL); stor_id = atoi(data);  
        Sql_GetData(mmysql_handle, 1, &data, NULL); combo_index = atoi(data);  
        Sql_GetData(mmysql_handle, 2, &data, NULL); is_active = atoi(data);  
  
        std::shared_ptr<s_collection_stor> collection = collection_db.find(stor_id);  
        if (collection != nullptr && combo_index < collection->active_combos.size()) {  
            collection->active_combos[combo_index] = (is_active == 1);  
        }  
    }  
    Sql_FreeResult(mmysql_handle);  
}

void do_init_collection(void)
{
	collection_db.load();
}

void do_final_collection(void)
{
	collection_db.clear();
}

void do_reload_collection(void)
{
	collection_db.clear();
	collection_db.load();

	struct s_mapiterator* iter = mapit_geteachpc();
	map_session_data* sd;

	for (sd = (map_session_data*)mapit_first(iter); mapit_exists(iter); sd = (map_session_data*)mapit_next(iter))
		status_calc_pc(sd, SCO_FORCE);

	mapit_free(iter);
}
