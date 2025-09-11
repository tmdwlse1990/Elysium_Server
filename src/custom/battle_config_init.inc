// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom battle config initializations
 * Place the definition of custom battle configuration initializations in this file.
 *
 * Do not forget to add a comma at the end of the line, if you want to add another config
 *
 * Format:
 *	 { "name", &battle_config.<variable name>, <default value>, <minimum value>, <maximum value> },
 **/

{ "autocombat_autoheal",		&battle_config.autocombat_autoheal,			1,		0,		1,				},
{ "autocombat_autopotion",		&battle_config.autocombat_autopotion,		1,		0,		1,				},
{ "autocombat_sittorest",		&battle_config.autocombat_sittorest,		1,		0,		1,				},
{ "autocombat_attackskill",		&battle_config.autocombat_attackskill,		1,		0,		1,				},
{ "autocombat_buffskill",		&battle_config.autocombat_buffskill,		1,		0,		1,				},
{ "autocombat_buffitems",		&battle_config.autocombat_buffitems,		1,		0,		1,				},
{ "autocombat_teleport",		&battle_config.autocombat_teleport,			1,		0,		1,				},
{ "autocombat_teleport_mvp",	&battle_config.autocombat_teleport_mvp,		1,		0,		1,				},
{ "autocombat_pickup",			&battle_config.autocombat_pickup,			1,		0,		1,				},
{ "autocombat_prefixname",		&battle_config.autocombat_prefixname,		1,		0,		1,				},
{ "autocombat_hateffect",		&battle_config.autocombat_hateffect,		1,		0,		1,				},
{ "autocombat_pdetection",		&battle_config.autocombat_pdetection,		15,     1,      30,      		},
{ "autocombat_mselection",		&battle_config.autocombat_mselection,		1,		0,		1,				},
{ "autocombat_mdetection",		&battle_config.autocombat_mdetection,		15,     1,      30,      		},
{ "autocombat_pickup_delay",	&battle_config.autocombat_pickup_delay,		500,   100,     INT_MAX,      	},
{ "autocombat_move_min",		&battle_config.autocombat_move_min,			7,      1,      20,      		},
{ "autocombat_move_max",		&battle_config.autocombat_move_max,			7,      1,      20,      		},
{ "autocombat_movetype",		&battle_config.autocombat_movetype,			0,		0,		2,				},
{ "autocombat_askill_delay",	&battle_config.autocombat_askill_delay,		500,	0,		INT_MAX,		},
{ "autocombat_bskill_delay",	&battle_config.autocombat_bskill_delay,		500,	0,		INT_MAX,		},
{ "autocombat_drop_ratio",		&battle_config.autocombat_drop_ratio,		100,	0,		INT_MAX,		},
{ "autocombat_exp_ratio",		&battle_config.autocombat_exp_ratio,		100,	0,		INT_MAX,		},
{ "autocombat_allow_town",		&battle_config.autocombat_allow_town,		1,		0,		1,				},
{ "autocombat_allow_pvp",		&battle_config.autocombat_allow_pvp,		1,		0,		1,				},
{ "autocombat_allow_gvg",		&battle_config.autocombat_allow_gvg,		1,		0,		1,				},
{ "autocombat_allow_bg",		&battle_config.autocombat_allow_bg,			1,		0,		1,				},
{ "autocombat_duration_type",	&battle_config.autocombat_duration_type,	0,		0,		1,				},
{ "autocombat_timer",			&battle_config.autocombat_timer,			250,	25,		INT_MAX,		},
{ "autocombat_iplimit",			&battle_config.autocombat_iplimit,			0,		0,		INT_MAX,		},
{ "autocombat_gepardlimit",		&battle_config.autocombat_gepardlimit,		0,		0,		INT_MAX,		},