#ifndef QUERYGRAPH_HPP
#define QUERYGRAPH_HPP

#include <algorithm>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cassert>
#include "Database.hpp"
#include "operator/HashJoin.hpp"

struct OptimizerResult {
   double cost=-1.0;
   std::unique_ptr<Operator> root;
};

namespace std {
    template<> struct hash<std::pair<size_t, size_t>> {
        size_t operator() (const std::pair<size_t, size_t>& key) const noexcept {
            size_t h1 = hash<size_t>{}(key.first);
            size_t h2 = hash<size_t>{}(key.second);
            return h1^(h2 << 1);
        }
    };
}

struct QueryGraph {
   // get the cardinality of a relation after selection pushdown according to the formulae we learned in the exercises
   double getCardinality(const std::string& alias) {
       if (bindingMaps.count(alias)) {
           double ret = 1;
           size_t Iam = bindingMaps[alias];
           for (auto edge_idx = head_loop[Iam]; edge_idx.has_value(); edge_idx = edges[edge_idx.value()].next) {
               ret *= edges[edge_idx.value()].selectivity();
           }
           ret *= tableScans[Iam]->getTable().getCardinality();
           return ret;
       }
       return -1.0;
   }

   //get the selectivity of a join between two relations according to the formulae we learned in the exercises
   double getSelectivity(const std::string& alias1, const std::string& alias2) {
       if (bindingMaps.count(alias1) && bindingMaps.count(alias2)) {
           double ret = 1;
           size_t from = bindingMaps[alias1];
           size_t to = bindingMaps[alias2];
           for (auto edge_idx = head[from]; edge_idx.has_value(); edge_idx = edges[edge_idx.value()].next) {
               if (edges[edge_idx.value()].to == to) {
                   ret *= edges[edge_idx.value()].selectivity();
               }
           }
           return ret;
       }
       return 1.0;
   }

    struct union_find {
        std::vector<size_t> prt;
        explicit union_find (size_t size) {
            for (size_t i = 0; i < size; ++i) {
                prt.emplace_back(i);
            }
        }
        size_t get_prt(size_t x) {
            if (prt[x] == x) {
                return x;
            }
            return prt[x] = get_prt(prt[x]);
        }
    };

   OptimizerResult runGOO() {
       struct entry {
           double cost;
           std::vector<size_t> edge_idxes;
       };
       std::vector<entry> records;

       union_find uf(head.size());

       std::vector<double> cardinality;

       for (size_t i = 0; i < head.size(); ++i) {
           cardinality.emplace_back(getCardinality(bindingMaps_reverse[i]));
       }

       size_t cnt = head.size();
       while (--cnt) {
           std::unordered_map<std::pair<size_t, size_t>, entry> queue;

           for (size_t i = 0; i < head.size(); ++i) {
               for (auto edge_idx = head[i]; edge_idx.has_value(); edge_idx = edges[edge_idx.value()].next) {
                   auto edge = edges[edge_idx.value()];

                   if (uf.get_prt(i) == uf.get_prt(edge.to)) {
                       continue;
                   }
                   if (queue.count(std::pair{uf.prt[i], uf.prt[edge.to]})) {
                       queue[std::pair{uf.prt[i], uf.prt[edge.to]}].edge_idxes.emplace_back(edge_idx.value());
                       queue[std::pair{uf.prt[i], uf.prt[edge.to]}].cost *= edge.selectivity();
                   }
                   else {
                       double cost = edge.selectivity() * cardinality[uf.prt[i]] * cardinality[uf.prt[edge.to]];
                       queue.emplace(std::pair{uf.prt[i], uf.prt[edge.to]}, entry{cost, std::vector<size_t>{edge_idx.value()}});
                   }
               }
           }
           if (queue.empty()) {
               break;
           }
           entry best{queue.begin()->second};
           for (auto candidate = ++queue.begin(); candidate != queue.end(); ++candidate) {
               if (candidate->second.cost < best.cost) {
                   best = candidate->second;
               }
           }
           records.emplace_back(best);
           cardinality[uf.get_prt(edges[best.edge_idxes[0]].from)] = best.cost;
           uf.prt[uf.get_prt(edges[best.edge_idxes[0]].to)] = uf.prt[uf.get_prt(edges[best.edge_idxes[0]].from)];
       }

       std::vector<Tablescan*>scans;
       for (size_t i = 0; i < head.size(); ++i) {
           scans.emplace_back(tableScans[i].get());
       }

       std::vector<std::unique_ptr<Operator>> trees;

       for (size_t i = 0; i < head.size(); ++i) {
           trees.emplace_back(std::move(tableScans[i]));
           auto* scan = scans[i];
           for (auto edge_idx = head_loop[i]; edge_idx.has_value(); edge_idx = edges[edge_idx.value()].next) {
               auto edge = edges[edge_idx.value()];
               trees.back() = std::make_unique<Selection>(std::move(trees.back()), scan->getOutput(edge.fromAttribute->getName()), edge.constReg);
           }
       }

       union_find buildTheTree(head.size());

       union_find trace(head.size());

       double total = 0;

       for (const auto& record: records) {
           auto from = edges[record.edge_idxes[0]].from;
           auto to = edges[record.edge_idxes[0]].to;
           std::cout << "R" << trace.prt.size() - head.size() << " = ";
           if (trace.get_prt(from) == from) {
               std::cout << bindingMaps_reverse[from] << " join ";
           }
           else {
               std::cout << "R" << trace.prt[from] - head.size() << " join ";
           }

           if (trace.get_prt(to) == to) {
               std::cout << bindingMaps_reverse[to];
           }
           else {
               std::cout << "R" << trace.prt[to] - head.size();
           }
           std::cout << " " << record.cost << std::endl;
           trace.prt[trace.get_prt(from)] = trace.prt.size();
           trace.prt[trace.get_prt(to)] = trace.prt.size();
           auto tmp = trace.prt.size();
           trace.prt.emplace_back(tmp);

           total += record.cost;

           std::unique_ptr<Operator> left, right;
           left = std::move(trees[buildTheTree.get_prt(from)]);
           right = std::move(trees[buildTheTree.get_prt(to)]);

           buildTheTree.prt[buildTheTree.get_prt(to)] = buildTheTree.prt[from];

           trees[buildTheTree.prt[from]] =
                   std::make_unique<HashJoin>(std::move(left), std::move(right),
                                              scans[from]->getOutput(edges[record.edge_idxes[0]].fromAttribute->getName()),
                                              scans[to]->getOutput(edges[record.edge_idxes[0]].toAttribute->getName()));
           for (size_t i = 1; i < record.edge_idxes.size(); ++i) {
               auto local_from = edges[record.edge_idxes[i]].from;
               auto local_to = edges[record.edge_idxes[i]].to;

               trees[buildTheTree.prt[local_from]] =
                       std::make_unique<Selection>(std::move(trees[buildTheTree.prt[local_from]]),
                                                   scans[local_from]->getOutput(edges[record.edge_idxes[i]].fromAttribute->getName()),
                                                   scans[local_to]->getOutput(edges[record.edge_idxes[i]].toAttribute->getName())
                                                   );
           }
       }

       std::unique_ptr<Operator> root = std::move(trees[buildTheTree.get_prt(0)]);
       if (!projectionRegs.empty()) {
           root = std::make_unique<Projection>(std::move(root), projectionRegs);
       }
       return {total,std::move(root)};
   }

   static inline bool single(size_t x) {  // x contains a single "1"?
       x &= x - 1;
       return !x;
   }

   static inline size_t find_pos(size_t x) {  // find the id (starting from 0) of the right-most bit of x
       size_t checker = 1;
       size_t id = 0;
       while (!(checker & x)) {
           checker <<= 1;
           ++id;
       }
       return id;
   }

   static inline size_t enumerate(size_t x, size_t start) {  // enumerate the next subset of x, starting from "start"
       ++start;
       while (start < x && x != (x | start)) {
           ++start;
       }
       return start;
   }

   static inline bool belongsTo(size_t pos, size_t X) {  // (1 << pos) belongs to X?
       return ((1 << pos) | X) == X;
   }

   static inline bool edgeTaken(size_t edge_idx, const std::vector<uint64_t>& edge_taken) {  // edge_idx already taken according to edge_taken?
       return edge_taken[edge_idx / 64] & (1ll << (edge_idx % 64));
   }

    struct record {
        double card;
        double cost;  // = -1 --> no valid join for this record
        size_t from;  // left sub-set of this record
        std::vector<size_t> edge_idxes;  // edge indexes (a.k.a. join predicates) for this join
        std::vector<uint64_t> edge_taken;  // bitmaps of edges already taken up to this record
    };

   std::unique_ptr<Operator>&& ConstructTheTree(const std::vector<record>& records,
                                                std::vector<std::unique_ptr<Operator>>& trees,
                                                const std::vector<Tablescan*>& scans,
                                                const size_t state) {
       if (single(state)) {
           return std::move(trees[find_pos(state)]);
       }

       auto left = records[state].from;
       auto right = state - left;

       std::unique_ptr<Operator> leftTree = ConstructTheTree(records, trees, scans, left);
       std::unique_ptr<Operator> rightTree = ConstructTheTree(records, trees, scans, right);

       auto edge = edges[records[state].edge_idxes[0]];
       auto leftPredicate = scans[edge.from]->getOutput(edge.fromAttribute->getName());
       auto rightPredicate = scans[edge.to]->getOutput(edge.toAttribute->getName());

       trees.emplace_back(std::make_unique<HashJoin>(std::move(leftTree), std::move(rightTree),
                                          leftPredicate, rightPredicate));
       for (size_t i = 1; i < records[state].edge_idxes.size(); ++i) {
           auto local_edge = edges[records[state].edge_idxes[i]];
           auto local_leftPredicate = scans[edge.from]->getOutput(edge.fromAttribute->getName());
           auto local_rightPredicate = scans[edge.to]->getOutput(edge.toAttribute->getName());
           trees.emplace_back(std::make_unique<Selection>(std::move(trees.back()), local_leftPredicate, local_rightPredicate));
       }

       return std::move(trees.back());
   }

   OptimizerResult runDP() { // DPsub, No Cross Product

       std::vector<record> records((1 << head.size()));

       size_t edge_taken_size = edges.size() / 64 + (edges.size() % 64 > 0);
       // how many 64-bit bitmaps are required for the "edge_taken" field of struct "record"

       std::cout << std::setw(20) << "index";
       std::cout << std::setw(20) << "left";
       std::cout << std::setw(20) << "right";
       std::cout << std::setw(20) << "cardinality";
       std::cout << std::setw(20) << "cost" << std::endl;

       for (size_t i = 1; i < (1 << head.size()); ++i) {
           records[i].edge_idxes.clear();
           records[i].card = 0;
           records[i].cost = -1;
           records[i].from = 0;
           for (size_t j = 0; j < edge_taken_size; ++j) {
               records[i].edge_taken.emplace_back(0);
           }

           if (single(i)) {
               size_t pos = find_pos(i);
               records[i].card = getCardinality(bindingMaps_reverse[pos]);
               records[i].cost = 0;
           }
           else {
               for (size_t left = enumerate(i, 0); left < i; left = enumerate(i, left)) {
                   auto right = i - left;
                   if (right < left) { // assume join cost symmetric
                       break;
                   }

                   if (records[left].cost == -1 || records[right].cost == -1) {
                       continue;
                   }

                   double selectivity = 1;
                   std::vector<size_t> edge_idxes;

                   for (size_t edge_idx = 0; edge_idx < edges.size(); ++edge_idx) {
                       if (belongsTo(edges[edge_idx].from, left) &&
                       belongsTo(edges[edge_idx].to, right) &&
                       !edgeTaken(edge_idx, records[left].edge_taken) &&
                       !edgeTaken(edge_idx, records[right].edge_taken)) {
                           edge_idxes.emplace_back(edge_idx);
                           selectivity *= edges[edge_idx].selectivity();
                       }
                   }
                   if (edge_idxes.empty()) {  // no cross product
                       continue;
                   }

                   double newCost = records[left].cost + records[right].cost +
                                    records[left].card * records[right].card * selectivity;
                   if (records[i].cost == -1 || records[i].cost > newCost) {
                       records[i].card = records[left].card * records[right].card * selectivity;
                       records[i].cost = newCost;
                       records[i].from = left;
                       records[i].edge_idxes = std::move(edge_idxes);
                       for (size_t j = 0; j < edge_taken_size; ++j) {
                           records[i].edge_taken[j] = records[left].edge_taken[j] | records[right].edge_taken[j];
                       }
                       for (const auto& edge_idx: records[i].edge_idxes) {
                           records[i].edge_taken[edge_idx / 64] |= 1ll << (edge_idx % 64);
                       }
                   }
               }
           }
           std::cout << std::setw(20) << std::bitset<10>(i);
           if (records[i].cost != -1) {
               std::cout << std::setw(20) << std::bitset<10>(records[i].from);
               std::cout << std::setw(20) << std::bitset<10>(i - records[i].from);
               std::cout << std::setw(20) << records[i].card;
               std::cout << std::setw(20) << records[i].cost << std::endl;
           }
           else {
               std::cout << std::setw(20) << "N/A";
               std::cout << std::setw(20) << "N/A";
               std::cout << std::setw(20) << "N/A";
               std::cout << std::setw(20) << "N/A" << std::endl;
           }
       }

       std::vector<Tablescan*>scans;  // ordered pointers to all tableScans, required since unique_ptr<Tablescan> are moved around
       for (size_t i = 0; i < head.size(); ++i) {
           scans.emplace_back(tableScans[i].get());
       }

       std::vector<std::unique_ptr<Operator>> trees;  // half-way join trees

       for (size_t i = 0; i < head.size(); ++i) {
           trees.emplace_back(std::move(tableScans[i]));  // starting with all tableScans

           auto* scan = scans[i];
           for (auto edge_idx = head_loop[i]; edge_idx.has_value(); edge_idx = edges[edge_idx.value()].next) {  // add the selection predicates if any
               auto edge = edges[edge_idx.value()];
               trees.back() = std::make_unique<Selection>(std::move(trees.back()), scan->getOutput(edge.fromAttribute->getName()), edge.constReg);
           }
       }

       std::unique_ptr<Operator> root = ConstructTheTree(records, trees, scans, (1 << head.size()) - 1);

       if (!projectionRegs.empty()) {  // add the projection predicates if any
           root = std::make_unique<Projection>(std::move(root), projectionRegs);
       }

       return {records.back().cost,std::move(root)};
   }

   std::vector<std::unique_ptr<Tablescan>> tableScans;
   std::unordered_map<std::string, size_t> bindingMaps;  // table names to table IDs
   std::unordered_map<size_t, std::string> bindingMaps_reverse;  // table IDs to table names
   std::vector<const Register*> projectionRegs;
   std::vector<std::unique_ptr<Register>> constRegs;

   struct Edge {
       size_t from;
       size_t to;
       std::optional<size_t> next;
       Table* fromTable;
       const Attribute* fromAttribute;
       Table* toTable;
       const Attribute* toAttribute;
       Register* constReg;
       [[nodiscard]] double selectivity() const {
           if (from == to) {
               if (fromAttribute->getKey()) {
                   return 1.0/fromTable->getCardinality();
               }
               else {
                   return 1.0/fromAttribute->getUniqueValues();
               }
           }
           else {
               if (fromAttribute->getKey() && toAttribute->getKey()) {
                   return 1.0 / std::max(fromTable->getCardinality(), toTable->getCardinality());
               }
               else if (fromAttribute->getKey() && !toAttribute->getKey()) {
                   return 1.0 / fromTable->getCardinality();
               }
               else if (!fromAttribute->getKey() && toAttribute->getKey()) {
                   return 1.0 / toTable->getCardinality();
               }
               else {
                   return 1.0 / std::max(fromAttribute->getUniqueValues(), toAttribute->getUniqueValues());
               }
           }
       }
   };
   std::vector<Edge> edges;
   std::vector<std::optional<size_t>> head;  // head[i] --> all edges starting from table i, excluding self-loops
   std::vector<std::optional<size_t>> head_loop;  // head_loop[i] --> all self-loops starting from table i
};

QueryGraph sql_to_graph(Database& db, const std::string& sql) {
    QueryGraph ret;
    auto result = parse_and_analyse_sql_statement(db, sql);

    size_t cnt = 0;
    for (const auto& tableName: result.relations) {
        ret.tableScans.emplace_back(std::make_unique<Tablescan>(db.getTable(tableName.first)));
        ret.bindingMaps.emplace(tableName.second, cnt);
        ret.bindingMaps_reverse.emplace(cnt++, tableName.second);
        ret.head.emplace_back(std::nullopt);
        ret.head_loop.emplace_back(std::nullopt);
    }

    for (size_t i = 0; i < result.projections.size(); ++i) {
        ret.projectionRegs.emplace_back(ret.tableScans[result.link_projections[i]]->getOutput(result.projections[i]));
    }

    for (size_t i = 0; i < result.joinConditions.size(); ++i) {
        auto x = result.link_joinConditions[i].first;
        auto* xTable = &ret.tableScans[result.link_joinConditions[i].first]->getTable();
        auto* xAttribute = &xTable->getAttribute(xTable->findAttribute(result.joinConditions[i].first.second));
        auto y = result.link_joinConditions[i].second;
        auto* yTable = &ret.tableScans[result.link_joinConditions[i].second]->getTable();
        auto* yAttribute = &yTable->getAttribute(yTable->findAttribute(result.joinConditions[i].second.second));
        assert(xTable != nullptr);
        assert(yTable != nullptr);
        assert(xAttribute != nullptr);
        assert(yAttribute != nullptr);

        ret.edges.emplace_back(QueryGraph::Edge{x, y, ret.head[x], xTable, xAttribute, yTable, yAttribute, nullptr});
        ret.head[x] = ret.edges.size() - 1;

        ret.edges.emplace_back(QueryGraph::Edge{y, x, ret.head[y], yTable, yAttribute, xTable, xAttribute, nullptr});
        ret.head[y] = ret.edges.size() - 1;
    }

    for (size_t i = 0; i < result.selections.size(); ++i) {
        auto x = result.link_selections[i];
        auto* xTable = &ret.tableScans[result.link_selections[i]]->getTable();
        auto* xAttribute = &xTable->getAttribute(xTable->findAttribute(result.selections[i].first.second));

        ret.constRegs.emplace_back(std::make_unique<Register>());
        auto attr_idx = xTable->findAttribute(result.selections[i].first.second);
        auto attr_type = xTable->getAttribute(attr_idx).getType();
        switch (attr_type) {
            case Attribute::Type::Int:
                ret.constRegs.back()->setInt(std::stoi(result.selections[i].second));
                break;
            case Attribute::Type::Double:
                ret.constRegs.back()->setDouble(std::stod(result.selections[i].second));
                break;
            case Attribute::Type::String:
                ret.constRegs.back()->setString(result.selections[i].second);
                break;
            default:
                if (result.selections[i].second == "false" ||
                    result.selections[i].second == "False" ||
                    result.selections[i].second == "FALSE" ||
                    result.selections[i].second == "0") {
                    ret.constRegs.back()->setBool(false);
                }
                else {
                    ret.constRegs.back()->setBool(true);
                }
        }
        assert(xTable != nullptr);
        assert(xAttribute != nullptr);

        ret.edges.emplace_back(QueryGraph::Edge{x, x, ret.head_loop[x], xTable, xAttribute,
                                                nullptr, nullptr,
                                                ret.constRegs.back().get()});
        ret.head_loop[x] = ret.edges.size() - 1;
    }

    return ret;
}

#endif
