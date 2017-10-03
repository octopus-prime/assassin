/*
 * engine.cpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

#include "searcher.hpp"
#include "tbprobe.h"
#include "io.hpp"
#include <chrono>
#include <iomanip>
#include <future>
#include <deque>
#include <ratio>

constexpr unsigned long long
operator "" _GB(const unsigned long long value)
{
	return value << 30;
}

constexpr unsigned long long
operator "" _MB(const unsigned long long value)
{
	return value << 20;
}

using namespace chess;
using namespace std::literals::chrono_literals;

const auto report_output = [](const std::uint_fast8_t iteration, const std::uint_fast8_t max_height, const score_t score, const auto duration, const std::size_t count, const std::string& pv)
{
	const auto dt = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(duration);
	std::cout << std::setw(2) << int(iteration) << '/' << std::setw(2) << int(max_height) << ", ";
	std::cout << std::setiosflags(std::ios_base::fixed | std::ios_base::showpos) << std::setw(7) << std::setprecision(2) << float(score / 100.0f) << std::resetiosflags(std::ios_base::showpos) << " p, ";
	std::cout << std::setiosflags(std::ios_base::fixed) << std::setw(6) << std::setprecision(2) << dt.count() << " s, ";
	std::cout << std::setw(10) << count << " N, ";
	std::cout << std::setw(8) << int(count / dt.count()) << " N/s, ";
//	std::copy(pv_begin, pv_end, std::ostream_iterator<std::string>(std::cout, " "));

	std::cout << pv << std::endl;
};

void test(transposition_table_t & t_table, const node_t& node, const unsigned depth)
{
	searcher search(t_table, report_output);
	search(node, depth);
	t_table.clear();
}

void test(transposition_table_t & t_table, const node_t& node, const std::chrono::seconds& time)
{
	searcher search(t_table, report_output);
	const std::future<void> work = std::async
	(
		std::launch::async,
		[&]() {search(node, 255);}
	);
	const std::future_status status = work.wait_for(time);
	if (status == std::future_status::timeout)
		search.stop();
	work.wait();
	t_table.clear();
}

typedef const std::initializer_list<std::pair<std::string, std::string>> test_suite;

clock::duration
run_test_suite(transposition_table_t & t_table, const test_suite& suite, const std::chrono::seconds& max_duration)
{
	std::deque<std::pair<bool, clock::duration>> collection;

	clock::duration sum(0);

	const auto run_test = [&collection, &sum, &t_table, &max_duration](const std::string& fen, const std::string& solution)
	{
		std::cout << "Solution = " << solution << std::endl;

		const report_t report = [&collection, &solution](const std::uint_fast8_t iteration, const std::uint_fast8_t max_height, const score_t score, const clock::duration duration, const std::size_t count, const std::string& pv)
		{
			report_output(iteration, max_height, score, duration, count, pv);
			collection.emplace_front(pv.find(solution) == 0, duration);
		};

		const node_t node(fen);
		clock::duration min = max_duration;

		try
		{
			searcher search(t_table, report);
			const std::future<void> work = std::async
			(
				std::launch::async,
				[&]() {search(node, 255);}
			);
			const std::future_status status = work.wait_for(max_duration);
			if (status == std::future_status::timeout)
				search.stop();
			work.wait();

			for (const auto& element : collection)
			{
				if (!element.first)
					break;

				min = element.second;
			}

			sum += min;
		}
		catch (const std::exception& exception)
		{
			std::cerr << "Error: " << exception.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << "Error: Unknown" << std::endl;
		}

		std::cout << "Result = " << std::chrono::duration_cast<std::chrono::seconds>(min).count() << std::endl;
		std::cout << "-------------------------------------------------------------------" << std::endl;

		t_table.clear();
		collection.clear();
	};

	for (const auto& test : suite)
		run_test(test.first, test.second);

	return sum;
}

void run_bt2630(transposition_table_t& t_table)
{
	static const test_suite bt2630
	{
		{"rq2r1k1/5pp1/p7/4bNP1/1p2P2P/5Q2/PP4K1/5R1R w - - 0 1",					"Nf5xg7"},
		{"6k1/2b2p1p/ppP3p1/4p3/PP1B4/5PP1/7P/7K w - - 0 1",						"Bd4xb6"},
		{"5r1k/p1q2pp1/1pb4p/n3R1NQ/7P/3B1P2/2P3P1/7K w - - 0 1", 					"Re5-e6"},
		{"5r1k/1P4pp/3P1p2/4p3/1P5P/3q2P1/Q2b2K1/B3R3 w - - 0 1", 					"Qa2-f7"},
		{"3B4/8/2B5/1K6/8/8/3p4/3k4 w - - 0 1", 									"Kb5-a6"},
		{"1k1r4/1pp4p/2n5/P6R/2R1p1r1/2P2p2/1PP2B1P/4K3 b - - 0 1",					"pe4-e3"},
		{"6k1/p3q2p/1nr3pB/8/3Q1P2/6P1/PP5P/3R2K1 b - - 0 1",						"rc6-d6"},
		{"2krr3/1p4pp/p1bRpp1n/2p5/P1B1PP2/8/1PP3PP/R1K3B1 w - - 0 1",				"Rd6xc6+"},
		{"r5k1/pp2p1bp/6p1/n1p1P3/2qP1NP1/2PQB3/P5PP/R4K2 b - - 0 1",				"pg6-g5"},
		{"2r3k1/1qr1b1p1/p2pPn2/nppPp3/8/1PP1B2P/P1BQ1P2/5KRR w - - 0 1",			"Rg1xg7+"},
		{"1br3k1/p4p2/2p1r3/3p1b2/3Bn1p1/1P2P1Pq/P3Q1BP/2R1NRK1 b - - 0 1",			"qh3xh2+"},
		{"8/pp3k2/2p1qp2/2P5/5P2/1R2p1rp/PP2R3/4K2Q b - - 0 1",						"qe6-e4"},
		{"2bq3k/2p4p/p2p4/7P/1nBPPQP1/r1p5/8/1K1R2R1 b - - 0 1",					"bc8-e6"},
		{"3r1rk1/1p3pnp/p3pBp1/1qPpP3/1P1P2R1/P2Q3R/6PP/6K1 w - - 0 1",				"Rh3xh7"},
		{"2b1q3/p7/1p1p2kb/nPpN3p/P1P1P2P/6P1/5R1K/5Q2 w - - 0 1",					"Pe4-e5"},
		{"2krr3/pppb1ppp/3b4/3q4/3P3n/2P2N1P/PP2B1P1/R1BQ1RK1 b - - 0 1",			"nh4xg2"},
		{"4r1k1/p1qr1p2/2pb1Bp1/1p5p/3P1n1R/3B1P2/PP3PK1/2Q4R w - - 0 1",			"Qc1xf4"},
		{"8/4p3/8/3P3p/P2pK3/6P1/7b/3k4 w - - 0 1",									"Pd5-d6"},
		{"3r2k1/pp4B1/6pp/PP1Np2n/2Pp1p2/3P2Pq/3QPPbP/R4RK1 b - - 0 1",				"pf4-f3"},
		{"r4rk1/5p2/1n4pQ/2p5/p5P1/P4N2/1qb1BP1P/R3R1K1 w - - 0 1",					"Ra1-a2"},
		{"k7/8/PP1b2P1/K2Pn2P/4R3/8/6np/8 w - - 0 1",								"Re4-e1"},
		{"rnb1k2r/pp2qppp/3p1n2/2pp2B1/1bP5/2N1P3/PP2NPPP/R2QKB1R w KQkq - 0 1",	"Pa2-a3"},
		{"8/7p/8/p4p2/5K2/Bpk3P1/4P2P/8 w - - 0 1",									"Pg3-g4"},
		{"R7/3p3p/8/3P2P1/3k4/1p5p/1P1NKP1P/7q w - - 0 1",							"Pg5-g6"},
		{"8/8/3k1p2/p2BnP2/4PN2/1P2K1p1/8/5b2 b - - 0 1",							"ne5-d3"},
		{"2r3k1/pbr1q2p/1p2pnp1/3p4/3P1P2/1P1BR3/PB1Q2PP/5RK1 w - - 0 1",			"Pf4-f5"},
		{"3r2k1/p2r2p1/1p1B2Pp/4PQ1P/2b1p3/P3P3/7K/8 w - - 0 1",					"Pe5-e6"},
		{"rnb1k1nr/p2p1ppp/3B4/1p1N1N1P/4P1P1/3P1Q2/PqP5/R4Kb1 w kq - 0 1",			"Pe4-e5"},
		{"r1b1kb1r/pp1n1ppp/2q5/2p3B1/Q1B5/2p2N2/PP3PPP/R3K2R w KQkq - 0 1",		"Ke1-c1"},
		{"2k5/2p3Rp/p1pb4/1p2p3/4P3/PN1P1P2/1P2KP1r/8 w - - 0 1",					"Pf3-f4"}
	};

	const auto sum = run_test_suite(t_table, bt2630, 90s);
	const auto elo = 2630 - std::chrono::duration_cast<std::chrono::seconds>(sum / 30).count();

//	BT = 2.630 - Gesamtzeit / 30 (Zeit in Sekunden)

	std::cout << "GZ = " << std::chrono::duration_cast<std::chrono::seconds>(sum).count() << std::endl;
	std::cout << "Elo = " << elo << std::endl;

//	for (const std::pair<std::string, std::string>& x : bt2630)
//	{
//		std::cout << "Solution = " << x.second << std::endl;
//		test(t_table, node_t(x.first));
//		std::cout << std::endl;
//	}
}

void
run_bs2830(transposition_table_t& t_table)
{
	static const test_suite bs2830
	{
		{"4r1k1/p1pb1ppp/Qbp1r3/8/1P6/2Pq1B2/R2P1PPP/2B2RK1 b - - 0 1",				"qd3xf3"},
		{"7r/2qpkp2/p3p3/6P1/1p2b2r/7P/PPP2QP1/R2N1RK1 b - - 0 1",					"pf7-f5"},
		{"r1bq1rk1/pp4bp/2np4/2p1p1p1/P1N1P3/1P1P1NP1/1BP1QPKP/1R3R2 b - - 0 1",	"bc8-h3+"},
		{"8/2kPR3/5q2/5N2/8/1p1P4/1p6/1K6 w - - 0 1",								"Nf5-d4"},
		{"2r1r3/p3bk1p/1pnqpppB/3n4/3P2Q1/PB3N2/1P3PPP/3RR1K1 w - - 0 1",			"Re1xe6"},
		{"8/2p5/7p/pP2k1pP/5pP1/8/1P2PPK1/8 w - - 0 1",								"Pf2-f3"},
		{"8/5p1p/1p2pPk1/p1p1P3/P1P1K2b/4B3/1P5P/8 w - - 0 1",						"Pb2-b4"},
		{"rn2r1k1/pp3ppp/8/1qNp4/3BnQb1/5N2/PPP2PPP/2KR3R b - - 0 1",				"bg4-h5"},
		{"r3kb1r/1p1b1p2/p1nppp2/7p/4PP2/qNN5/P1PQB1PP/R4R1K w qk - 0 1",			"Nc3-b1"},
		{"r3r1k1/pp1bp2p/1n2q1P1/6b1/1B2B3/5Q2/5PPP/1R3RK1 w - - 0 1",				"Bb4-d2"},
		{"r3k2r/pb3pp1/2p1qnnp/1pp1P3/Q1N4B/2PB1P2/P5PP/R4RK1 w qk - 0 1",			"Pe5xf6"},
		{"r1b1r1k1/ppp2ppp/2nb1q2/8/2B5/1P1Q1N2/P1PP1PPP/R1B2RK1 w - - 0 1",		"Bc1-b2"},
		{"rnb1kb1r/1p3ppp/p5q1/4p3/3N4/4BB2/PPPQ1P1P/R3K2R w qkQK - 0 1",			"Ke1-c1"},
		{"r1bqr1k1/pp1n1ppp/5b2/4N1B1/3p3P/8/PPPQ1PP1/2K1RB1R w - - 0 1",			"Ne5xf7"},
		{"2r2rk1/1bpR1p2/1pq1pQp1/p3P2p/P1PR3P/5N2/2P2PPK/8 w - - 0 1",				"Kh2-g3"},
		{"8/pR4pk/1b6/2p5/N1p5/8/PP1r2PP/6K1 b - - 0 1",							"rd2xb2"},
		{"r1b1qrk1/ppBnppb1/2n4p/1NN1P1p1/3p4/8/PPP1BPPP/R2Q1R1K w - - 0 1",		"Nc5-e6"},
		{"8/8/4b1p1/2Bp3p/5P1P/1pK1Pk2/8/8 b - - 0 1",								"pg6-g5"},
		{"r3k2r/pp1n1ppp/1qpnp3/3bN1PP/3P2Q1/2B1R3/PPP2P2/2KR1B2 w qk - 0 1",		"Bc3-e1"},
		{"r1bqk2r/pppp1Npp/8/2bnP3/8/6K1/PB4PP/RN1Q3R b qk - 0 1",					"ke8-g8"},
		{"r4r1k/pbnq1ppp/np3b2/3p1N2/5B2/2N3PB/PP3P1P/R2QR1K1 w - - 0 1",			"Nc3-e4"},
		{"r2qr2k/pbp3pp/1p2Bb2/2p5/2P2P2/3R2P1/PP2Q1NP/5RK1 b - - 0 1",				"qd8xd3"},
		{"5r2/1p4r1/3kp1b1/1Pp1p2p/2PpP3/q2B1PP1/3Q2K1/1R5R b - - 0 1",				"rf8xf3"},
		{"8/7p/8/7P/1p6/1p5P/1P2Q1pk/1K6 w - - 0 1",								"Kb1-a1"},
		{"r5k1/p4n1p/6p1/2qPp3/2p1P1Q1/8/1rB3PP/R4R1K b - - 0 1",					"ra8-f8"},
		{"1r4k1/1q2pN1p/3pPnp1/8/2pQ4/P5PP/5P2/3R2K1 b - - 0 1",					"qb7-d5"},
		{"2rq1rk1/pb3ppp/1p2pn2/4N3/1b1PPB2/4R1P1/P4PBP/R2Q2K1 w - - 0 1",			"Pd4-d5"}
	};

	const auto sum = run_test_suite(t_table, bs2830, 90s);
	const auto elo = 2830 - std::chrono::duration_cast<std::chrono::seconds>(sum / 90).count() - std::chrono::duration_cast<std::chrono::seconds>(sum / 1320).count() * std::chrono::duration_cast<std::chrono::seconds>(sum / 1320).count();

//	BS = 2.830 - Gesamtzeit / 1,5 - (Gesamtzeit x Gesamtzeit) / (22 * 22) (Zeit in Minuten)

	std::cout << "GZ = " << std::chrono::duration_cast<std::chrono::seconds>(sum).count() << std::endl;
	std::cout << "Elo = " << elo << std::endl;
}

void
run_gs2930(transposition_table_t& t_table)
{
	static const test_suite gs2930
	{
		{"r3k2r/pp1qn1pp/2p2p2/8/3P4/5N2/PP2QPPP/2R1R1K1 w kq - 0 1",			"Pd4-d5"},
		{"r1b1kb1r/1p4pp/p2ppn2/8/2qNP3/2N1B3/PPP3PP/R2Q1RK1 w kq - 0 1",		"Rf1xf6"},
		{"5k2/p1p4R/1pr5/3p1pP1/P2P1P2/2P2K2/8/8 w - - 0 1",					"Kf3-g3"},
		{"r1bk1n1r/pp1n1q1p/2p2p1R/3p4/3PpN2/2NB2Q1/PPP2PP1/2K1R3 w - - 0 1",	"Bd3xe4"},
		{"r1b1n2r/1p2q3/1Qp1npk1/4p1p1/P1B1P3/2P1BNP1/1P3PK1/R3R3 b - - 0 1",	"ne6-f4+"},
		{"1nr3k1/p4pbp/1n1Rp1p1/2p5/4bB2/P4NPB/1P3P1P/R5K1 w - - 0 1",			"Nf3-g5"},
		{"2rq1rk1/pb3p1p/1p2pbp1/3p3R/2PP4/PP5R/1B3PPP/3Q1BK1 b - - 0 1",		"pd5xc4"},
		{"r1b2rn1/1ppn3k/p4q1p/3P1p2/1p3Q2/1NNB4/P5PP/4RRK1 w - - 0 1",			"Re1-e6"},
		{"4k3/7p/1pr1np2/p1p1pNpP/P1P1K1P1/1P2P3/3R1P2/8 w - - 0 1",			"Rd2-d6"},
		{"2kr3r/1p1b1pp1/p1nRpn1p/q7/5P1B/2N5/PPP3PP/2K1QB1R w - - 0 1",		"Rd6-d1"},
		{"2r5/R6p/5p2/1k2p1p1/1b1pP1P1/3K1P2/4N2P/8 b - - 0 1",					"bb4-a5"},
		{"8/4kpbn/p1p3p1/Pp2p2p/1P2Pn2/N1P1BP2/5P1P/5BK1 w - - 0 1",			"Na3xb5"},
		{"2r2rk1/p1q1bpp1/1p6/n2R4/8/P4N2/1B2QPPP/5RK1 w - - 0 1",				"Bb2xg7"},
		{"r1b2rk1/p2nbqpp/p3p3/2ppPpB1/N2P1N1P/8/PPP2PP1/R2Q1RK1 w - - 0 1",	"Pc2-c4"}
	};

	const auto sum = run_test_suite(t_table, gs2930, 120s);
	const auto elo = 2930 - std::chrono::duration_cast<std::chrono::seconds>(sum / 90).count();

//	BS = 2.830 - Gesamtzeit / 1,5 - (Gesamtzeit x Gesamtzeit) / (22 * 22) (Zeit in Minuten)

	std::cout << "GZ = " << std::chrono::duration_cast<std::chrono::seconds>(sum).count() << std::endl;
	std::cout << "Elo = " << elo << std::endl;
}

#include "attacker.hpp"

int main()
{
	try
	{
		transposition_table_t t_table(16_GB);
//		transposition_table_t t_table(24_GB);
//		transposition_table_t t_table(256_MB); // 16GB
//		run_bt2630(t_table);
		run_bs2830(t_table);
//		run_gs2930(t_table);

//		const node_t nodex("r3r1k1/pp1bp2p/1n2q1P1/6b1/1B2B3/5Q2/5PPP/1R3RK1 w - - 0 1");//,				"Bb4-d2"},

//		const node_t node0("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 1 1");
//		const node_t node1("rq2r1k1/5pp1/p7/4bNP1/1p2P2P/5Q2/PP4K1/5R1R w - - 0 1"); // "Nf5xg7"
//		const node_t node2("6k1/2b2p1p/ppP3p1/4p3/PP1B4/5PP1/7P/7K w - - 0 1"); // Bd4xb6
//		const node_t node3("4r1k1/p1pb1ppp/Qbp1r3/8/1P6/2Pq1B2/R2P1PPP/2B2RK1 b - - 0 1");	// "qd3xf3"
//		const node_t node4("7r/2qpkp2/p3p3/6P1/1p2b2r/7P/PPP2QP1/R2N1RK1 b - - 0 1"); // "pf7-f5"
//		const node_t node4a("7r/2qp1k2/p3pP2/8/1p2b2r/7P/PPP2QP1/R2N1RK1 w - - 1 3");
//		const node_t node5("r3kb1r/1p1b1p2/p1nppp2/7p/4PP2/qNN5/P1PQB1PP/R4R1K w qk - 0 1"); // "Nc3-b1"
//		const node_t node6("2b1kb2/8/8/8/8/8/8/4K3 w - - 0 1"); // "..."
//		const node_t node7("4r3/p1p1pPp1/P1P1P1P1/5K2/3p2P1/7p/3P1ppr/3R1nkq w - - 0 1"); // fxe8=N
//		const node_t node8("7K/2p1p1p1/2PpP1Pk/6pP/Pp4P1/8/1P1P4/8 w - - 0 1"); // a5
//		const node_t node9("n1QBq1k1/5p1p/5KP1/p7/8/8/8/8 w - - 0 1"); // Bc7
//		const node_t nodea("r1bq1rk1/pp4bp/2np4/2p1p1p1/P1N1P3/1P1P1NP1/1BP1QPKP/1R3R2 b - - 0 1");//	"bc8-h3+"
//		const node_t nodeb("8/2kPR3/5q2/5N2/8/1p1P4/1p6/1K6 w - - 0 1");//	"Nf5-d4"
//		const node_t nodec("k7/7R/8/8/8/8/8/1R5K w - - 0 1");//	patt
//		const node_t noded("r3k2r/pp1n1ppp/1qpnp3/3bN1PP/3P2Q1/2B1R3/PPP2P2/2KR1B2 w kq - 0 1");
//		const node_t nodee("q2n2k1/1b3rpp/p4n2/3P2B1/8/2P5/4QPPP/3RR1K1 w - - 0 28");
//		test(t_table, nodex, 120s);//, 14);
//		test(t_table, nodee, 120s);//, 14);
//		test(t_table, node1);//, 12);
//		test(t_table, node2);//, 20);
//		test(t_table, node3);//, 14);
//		test(t_table, node4, 120s);//, 14);
//		test(t_table, node4a, 120s);//, 14);
//		test(t_table, noded, 120s);//, 14);
//		test(node5, 14);
//		test(node6, 50);
//		test(node7, 30);
//		test(node8, 30);
//		test(node9, 30);
//		test(nodea, 30);
//		test(t_table, nodeb);//, 30);
//		test(environment, nodec, 30);

//		init_tablebases("/home/mike/Downloads/syzygy");
//		std::cout << TBlargest << std::endl;
//
//		int success = 1;
//		int ret = probe_wdl_table(nodec, &success);
//
//		std::cout << success << std::endl;
//		std::cout << ret << std::endl;

	}
	catch(const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}
