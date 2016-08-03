/*
 * move.cpp
 *
 *  Created on: 21.05.2016
 *      Author: mike_gresens
 */

#include "move.hpp"

namespace chess {

std::ostream&
operator<<(std::ostream& stream, const move_t& move)
{
	stream << char('a' + file_of(move.from));
	stream << char('1' + rank_of(move.from));
	stream << char('a' + file_of(move.to));
	stream << char('1' + rank_of(move.to));
	return stream;
}

}
