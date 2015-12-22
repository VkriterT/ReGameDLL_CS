#include "precompiled.h"

/*
* Globals initialization
*/
#ifndef HOOK_GAMEDLL

NavDirType Opposite[ NUM_DIRECTIONS ] = { SOUTH, WEST, NORTH, EAST };

CNavNode *CNavNode::m_list = NULL;
unsigned int CNavNode::m_listLength = 0;

#else // HOOK_GAMEDLL

NavDirType Opposite[ NUM_DIRECTIONS ];

CNavNode *IMPL_CLASS(CNavNode, m_list);
unsigned int IMPL_CLASS(CNavNode, m_listLength);

#endif // HOOK_GAMEDLL

//NOXREF Extent NodeMapExtent;

/* <4f79dc> ../game_shared/bot/nav_node.cpp:23 */
CNavNode::CNavNode(const Vector *pos, const Vector *normal, CNavNode *parent)
{
	m_pos = *pos;
	m_normal = *normal;

	static unsigned int nextID = 1;
	m_id = nextID++;

	for (int i = 0; i < NUM_DIRECTIONS; ++i)
		m_to[i] = NULL;

	m_visited = 0;
	m_parent = parent;

	m_next = IMPL(m_list);
	IMPL(m_list) = this;
	IMPL(m_listLength)++;

	m_isCovered = FALSE;
	m_area = NULL;

	m_attributeFlags = 0;
}

// Create a connection FROM this node TO the given node, in the given direction

/* <4f7a31> ../game_shared/bot/nav_node.cpp:54 */
void CNavNode::ConnectTo(CNavNode *node, NavDirType dir)
{
	m_to[ dir ] = node;
}

// Return node at given position
// TODO: Need a hash table to make this lookup fast

/* <4f7a75> ../game_shared/bot/nav_node.cpp:64 */
const CNavNode *CNavNode::GetNode(const Vector *pos)
{
	const float tolerance = 0.45f * GenerationStepSize;

	for (const CNavNode *node = IMPL(m_list); node != NULL; node = node->m_next)
	{
		float dx = ABS(node->m_pos.x - pos->x);
		float dy = ABS(node->m_pos.y - pos->y);
		float dz = ABS(node->m_pos.z - pos->z);

		if (dx < tolerance && dy < tolerance && dz < tolerance)
			return node;
	}

	return NULL;
}

// Return true if this node is bidirectionally linked to 
// another node in the given direction

/* <4f7af2> ../game_shared/bot/nav_node.cpp:86 */
BOOL CNavNode::IsBiLinked(NavDirType dir) const
{
	if (m_to[ dir ] &&  m_to[ dir ]->m_to[ Opposite[dir] ] == this)
		return true;

	return false;
}

// Return true if this node is the NW corner of a quad of nodes
// that are all bidirectionally linked

/* <4f7b1c> ../game_shared/bot/nav_node.cpp:100 */
BOOL CNavNode::IsClosedCell(void) const
{
	if (IsBiLinked( SOUTH ) && IsBiLinked( EAST ) && m_to[ EAST ]->IsBiLinked( SOUTH ) && m_to[ SOUTH ]->IsBiLinked( EAST )
		&& m_to[ EAST ]->m_to[ SOUTH ] == m_to[ SOUTH ]->m_to[ EAST ])
		return true;

	return false;
}
