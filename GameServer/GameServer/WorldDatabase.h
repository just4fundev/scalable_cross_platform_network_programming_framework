#pragma once

namespace GameServer
{
	enum WorldSDBStmts : size_t
	{
		SDB_SelectMaps,
		SDB_SelectMapPortals,
		SDB_SelectMapNPC,
		SDB_SelectMapCollectables,
		SDB_SelectMapAreas,
		WorldSDBStmtsSize
	};
}