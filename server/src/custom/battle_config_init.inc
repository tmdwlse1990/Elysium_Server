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

{ "function.autocombat_autoheal",		&battle_config.function_autocombat_autoheal,		1,		0,		1,				},
{ "function.autocombat_autopotion",		&battle_config.function_autocombat_autopotion,		1,		0,		1,				},
{ "function.autocombat_sittorest",		&battle_config.function_autocombat_sittorest,		1,		0,		1,				},
{ "function.autocombat_attackskill",	&battle_config.function_autocombat_attackskill,		1,		0,		1,				},
{ "function.autocombat_buffskill",		&battle_config.function_autocombat_buffskill,		1,		0,		1,				},
{ "function.autocombat_buffitems",		&battle_config.function_autocombat_buffitems,		1,		0,		1,				},
{ "function.autocombat_teleport",		&battle_config.function_autocombat_teleport,		1,		0,		1,				},
{ "function.autocombat_teleport_mvp",	&battle_config.function_autocombat_teleport_mvp,	1,		0,		1,				},
{ "function.autocombat_pickup",			&battle_config.function_autocombat_pickup,			1,		0,		1,				},
{ "function.autocombat_prefixname",		&battle_config.function_autocombat_prefixname,		1,		0,		1,				},
{ "function.autocombat_hateffect",		&battle_config.function_autocombat_hateffect,		1,		0,		1,				},
{ "function.autocombat_pdetection",		&battle_config.function_autocombat_pdetection,		15,     1,      30,      		},
{ "function.autocombat_mselection",		&battle_config.function_autocombat_mselection,		1,		0,		1,				},
{ "function.autocombat_mdetection",		&battle_config.function_autocombat_mdetection,		15,     1,      30,      		},
{ "function.autocombat_pickup_delay",	&battle_config.function_autocombat_pickup_delay,	500,   100,     INT_MAX,      	},
{ "function.autocombat_move_min",		&battle_config.function_autocombat_move_min,		7,      1,      20,      		},
{ "function.autocombat_move_max",		&battle_config.function_autocombat_move_max,		7,      1,      20,      		},
{ "function.autocombat_movetype",		&battle_config.function_autocombat_movetype,		0,		0,		2,				},
{ "function.autocombat_askill_delay",	&battle_config.function_autocombat_askill_delay,	500,	0,		INT_MAX,		},
{ "function.autocombat_bskill_delay",	&battle_config.function_autocombat_bskill_delay,	500,	0,		INT_MAX,		},
{ "function.autocombat_drop_ratio",		&battle_config.function_autocombat_drop_ratio,		100,	0,		INT_MAX,		},
{ "function.autocombat_exp_ratio",		&battle_config.function_autocombat_exp_ratio,		100,	0,		INT_MAX,		},
{ "function.autocombat_allow_town",		&battle_config.function_autocombat_allow_town,		1,		0,		1,				},
{ "function.autocombat_allow_pvp",		&battle_config.function_autocombat_allow_pvp,		1,		0,		1,				},
{ "function.autocombat_allow_gvg",		&battle_config.function_autocombat_allow_gvg,		1,		0,		1,				},
{ "function.autocombat_allow_bg",		&battle_config.function_autocombat_allow_bg,		1,		0,		1,				},
{ "function.autocombat_duration_type",	&battle_config.function_autocombat_duration_type,	0,		0,		1,				},
{ "function.autocombat_timer",			&battle_config.function_autocombat_timer,			250,	25,		INT_MAX,		},
{ "function.autocombat_iplimit",		&battle_config.function_autocombat_iplimit,			0,		0,		INT_MAX,		},
{ "function.autocombat_gepardlimit",	&battle_config.function_autocombat_gepardlimit,		0,		0,		INT_MAX,		},