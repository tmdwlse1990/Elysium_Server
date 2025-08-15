--
-- Table structure for table `item_collection`
-- rename this table to be used, make sure the same with inter_server.yml Table value
--

CREATE TABLE IF NOT EXISTS `item_collection` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `account_id` int(11) unsigned NOT NULL default '0',
  `char_id` int(11) unsigned NOT NULL default '0',
  `nameid` int(10) unsigned NOT NULL default '0',
  `amount` smallint(11) unsigned NOT NULL default '0',
  `equip` int(11) unsigned NOT NULL default '0',
  `identify` smallint(6) unsigned NOT NULL default '0',
  `refine` tinyint(3) unsigned NOT NULL default '0',
  `attribute` tinyint(4) unsigned NOT NULL default '0',
  `card0` int(10) unsigned NOT NULL default '0',
  `card1` int(10) unsigned NOT NULL default '0',
  `card2` int(10) unsigned NOT NULL default '0',
  `card3` int(10) unsigned NOT NULL default '0',
  `option_id0` smallint(5) NOT NULL default '0',
  `option_val0` smallint(5) NOT NULL default '0',
  `option_parm0` tinyint(3) NOT NULL default '0',
  `option_id1` smallint(5) NOT NULL default '0',
  `option_val1` smallint(5) NOT NULL default '0',
  `option_parm1` tinyint(3) NOT NULL default '0',
  `option_id2` smallint(5) NOT NULL default '0',
  `option_val2` smallint(5) NOT NULL default '0',
  `option_parm2` tinyint(3) NOT NULL default '0',
  `option_id3` smallint(5) NOT NULL default '0',
  `option_val3` smallint(5) NOT NULL default '0',
  `option_parm3` tinyint(3) NOT NULL default '0',
  `option_id4` smallint(5) NOT NULL default '0',
  `option_val4` smallint(5) NOT NULL default '0',
  `option_parm4` tinyint(3) NOT NULL default '0',
  `expire_time` int(11) unsigned NOT NULL default '0',
  `bound` tinyint(3) unsigned NOT NULL default '0',
  `unique_id` bigint(20) unsigned NOT NULL default '0',
  `enchantgrade` tinyint unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `account_id` (`account_id`),
  KEY `char_id` (`char_id`)
) ENGINE=MyISAM;

--
-- Table structure for table `collection_combos`
--
CREATE TABLE IF NOT EXISTS `collection_combos` (    
  `account_id` int(11) NOT NULL,    
  `char_id` int(11) NOT NULL,    
  `stor_id` smallint(5) NOT NULL,    
  `combo_index` tinyint(3) NOT NULL,    
  `is_active` tinyint(1) NOT NULL DEFAULT 0,    
  PRIMARY KEY (`account_id`, `char_id`, `stor_id`, `combo_index`)    
) ENGINE=MyISAM;
