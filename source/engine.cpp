/*
 * engine.cpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

#include "move_generator.hpp"
#include "evaluator.hpp"
#include "io.hpp"

using namespace chess;

int main()
{
	try
	{
		std::cout << sizeof(node_t) << std::endl;

		const node_t node("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 1 1");
		std::cout << node << std::endl;

		const move_generator<all_tag> moves(node);
		std::cout << moves.size() << std::endl;
		for (const auto move : moves)
			std::cout << move << std::endl;

		std::cout << std::bitset<64>(node.attack<white_tag>()) << std::endl;
		std::cout << std::bitset<64>(node.attack<black_tag>()) << std::endl;

		std::cout << evaluator::evaluate(node) << std::endl;
	}
	catch(const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}
