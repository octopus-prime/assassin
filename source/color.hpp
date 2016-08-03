/*
 * color.hpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

#pragma once

#include <cstdint>

namespace chess {

typedef std::int16_t color_t;

enum color : color_t
{
	white = +1,
	black = -1
};

}
