#pragma once

#include "node.hpp"

namespace chess {

extern int TBlargest; // 5 if 5-piece tables, 6 if 6-piece tables were found.

typedef int Value; // TODO: score_t ?!

void init_tablebases(char *path);
int probe_wdl(const node_t& pos, int *success);
int probe_dtz(const node_t& pos, int *success);
int root_probe(const node_t& pos, Value& TBScore);
int root_probe_wdl(const node_t& pos, Value& TBScore);

int probe_wdl_table(const node_t& pos, int *success);

}
