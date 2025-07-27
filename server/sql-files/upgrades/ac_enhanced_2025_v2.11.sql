CREATE TABLE `ac_common_config` (
  `char_id` int unsigned NOT NULL,
  `stopmelee` tinyint(1) NOT NULL DEFAULT '0',
  `pickup_item_config` int unsigned NOT NULL DEFAULT '0',
  `prio_item_config` int unsigned NOT NULL DEFAULT '0',
  `aggressive_behavior` tinyint(1) NOT NULL DEFAULT '0',
  `autositregen_conf` tinyint(1) NOT NULL DEFAULT '0',
  `autositregen_maxhp` smallint unsigned NOT NULL DEFAULT '0',
  `autositregen_minhp` smallint unsigned NOT NULL DEFAULT '0',
  `autositregen_maxsp` smallint unsigned NOT NULL DEFAULT '0',
  `autositregen_minsp` smallint unsigned NOT NULL DEFAULT '0',
  `tp_use_teleport` tinyint(1) NOT NULL DEFAULT '0',
  `tp_use_flywing` tinyint(1) NOT NULL DEFAULT '0',
  `tp_min_hp` smallint unsigned NOT NULL DEFAULT '0',
  `tp_delay_nomobmeet` int unsigned NOT NULL DEFAULT '0',
  `tp_mvp` smallint NOT NULL DEFAULT '0',
  `tp_miniboss` smallint NOT NULL DEFAULT '0',
  `accept_party_request` tinyint(1) NOT NULL DEFAULT '1',
  `token_siegfried` tinyint(1) NOT NULL DEFAULT '1',
  `return_to_savepoint` tinyint(1) NOT NULL DEFAULT '1',
  `map_mob_selection` int NOT NULL DEFAULT '0',
  `action_on_end` int NOT NULL DEFAULT '0',
  `monster_surround` int NOT NULL DEFAULT '0',
  UNIQUE KEY `char_id` (`char_id`)
);

CREATE TABLE `ac_items` (
  `char_id` int unsigned NOT NULL,
  `type` smallint unsigned NOT NULL,
  `item_id` int unsigned NOT NULL,
  `min_hp` smallint unsigned NOT NULL DEFAULT '0',
  `min_sp` smallint unsigned NOT NULL DEFAULT '0',
  `delay` int unsigned NOT NULL DEFAULT '0',
  `status` int unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `char_id` (`char_id`,`type`,`item_id`)
);

CREATE TABLE `ac_mobs` (
  `char_id` int unsigned NOT NULL,
  `mob_id` int unsigned NOT NULL,
  UNIQUE KEY `char_id` (`char_id`,`mob_id`)
);

CREATE TABLE `ac_skills` (
  `char_id` int unsigned NOT NULL,
  `type` smallint unsigned NOT NULL,
  `skill_id` smallint unsigned NOT NULL,
  `skill_lv` smallint unsigned NOT NULL DEFAULT '0',
  `min_hp` smallint unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `char_id` (`char_id`,`type`,`skill_id`)
);
