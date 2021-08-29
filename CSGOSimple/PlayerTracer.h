#include "./features/Misc.hpp"
#include <algorithm>
#include "helpers/math.hpp"

class trace_pos_info
{
public:
	trace_pos_info(Vector positions)
	{
		this->position = positions;
	}

	Vector position;
};