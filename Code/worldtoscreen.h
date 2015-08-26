#ifndef _WORLDTOSCREEN_H
#define _WORLDTOSCREEN_H

#include "gamespec.h"

/*
	TigerSnail CS:GO External Hack
	For Counter-Strike: Global Offensive

	Version: 0.1

	Developed by sk0r / Czybik
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Read the file 'readme.txt' for more details

	File: worldtoscreen.h: WorldToScreen interface
*/

//======================================================================
bool WorldToScreen(const w2smatrix_t& vMatrix, const Vector& vEntOrigin, Vector& vOut);
//======================================================================

#endif
