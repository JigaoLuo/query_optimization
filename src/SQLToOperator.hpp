#include "Database.hpp"
#include "operator/Operator.hpp"
#include "operator/Tablescan.hpp"
#include "operator/Selection.hpp"
#include "operator/CrossProduct.hpp"
#include "operator/Projection.hpp"
#include "Attribute.hpp"
#include <string>
#include <memory>
#include <algorithm>

struct PlanContainer {
    std::unique_ptr<Operator> root;
    std::vector<std::unique_ptr<Register>> constRegs;

    //extend here as needed
};

PlanContainer sql_to_plan(Database& db, const std::string& sql) {
    auto result = parse_and_analyse_sql_statement(db, sql);

    std::vector<size_t> index_selections;

    for (size_t i = 0; i < result.link_selections.size(); ++i) {
        index_selections.emplace_back(i);
    }
    std::sort(index_selections.begin(), index_selections.end(), [&](size_t x, size_t y){
        return result.link_selections[x] < result.link_selections[y];
    });

    std::vector<std::unique_ptr<Tablescan>> tableScans;
    for (const auto& tableName: result.relations) {
        tableScans.emplace_back(std::make_unique<Tablescan>(db.getTable(tableName.first)));
    }

    std::vector<std::pair<const Register*, const Register*>> joinRegs;
    for (size_t i = 0; i < result.joinConditions.size(); ++i) {
        joinRegs.emplace_back(std::pair{tableScans[result.link_joinConditions[i].first]->getOutput(result.joinConditions[i].first.second),
                                        tableScans[result.link_joinConditions[i].second]->getOutput(result.joinConditions[i].second.second)});
    }

    std::vector<const Register*> projectionRegs;
    for (size_t i = 0; i < result.projections.size(); ++i) {
        projectionRegs.emplace_back(tableScans[result.link_projections[i]]->getOutput(result.projections[i]));
    }

    PlanContainer ret;

    std::unique_ptr<Operator> root = std::move(tableScans[0]);
    size_t idx_ptr = 0;
    Operator* scan = root.get();
    while (idx_ptr < index_selections.size() && result.link_selections[index_selections[idx_ptr]] == 0) {
        const Register* attr = dynamic_cast<Tablescan*>(scan)->getOutput(result.selections[index_selections[idx_ptr]].first.second);
        ret.constRegs.emplace_back(std::make_unique<Register>());
        auto attr_idx = dynamic_cast<Tablescan*>(scan)->getTable().findAttribute(result.selections[index_selections[idx_ptr]].first.second);
        auto attr_type = dynamic_cast<Tablescan*>(scan)->getTable().getAttribute(attr_idx).getType();
        switch (attr_type) {
            case Attribute::Type::Int:
                ret.constRegs.back()->setInt(std::stoi(result.selections[index_selections[idx_ptr]].second));
                break;
            case Attribute::Type::Double:
                ret.constRegs.back()->setDouble(std::stod(result.selections[index_selections[idx_ptr]].second));
                break;
            case Attribute::Type::String:
                ret.constRegs.back()->setString(result.selections[index_selections[idx_ptr]].second);
                break;
            default:
                if (result.selections[index_selections[idx_ptr]].second == "false" ||
                result.selections[index_selections[idx_ptr]].second == "False" ||
                result.selections[index_selections[idx_ptr]].second == "FALSE" ||
                result.selections[index_selections[idx_ptr]].second == "0") {
                    ret.constRegs.back()->setBool(false);
                }
                else {
                    ret.constRegs.back()->setBool(true);
                }
        }

        root = std::make_unique<Selection>(std::move(root), attr, ret.constRegs.back().get());
        ++idx_ptr;
    }

    for (size_t i = 1; i < result.relations.size(); ++i) {
        std::unique_ptr<Operator> right = std::move(tableScans[i]);
        scan = right.get();
        while (idx_ptr < index_selections.size() && result.link_selections[index_selections[idx_ptr]] == i) {
            const Register* attr = dynamic_cast<Tablescan*>(scan)->getOutput(result.selections[index_selections[idx_ptr]].first.second);
            ret.constRegs.emplace_back(std::make_unique<Register>());
            auto attr_idx = dynamic_cast<Tablescan*>(scan)->getTable().findAttribute(result.selections[index_selections[idx_ptr]].first.second);
            auto attr_type = dynamic_cast<Tablescan*>(scan)->getTable().getAttribute(attr_idx).getType();
            switch (attr_type) {
                case Attribute::Type::Int:
                    ret.constRegs.back()->setInt(std::stoi(result.selections[index_selections[idx_ptr]].second));
                    break;
                case Attribute::Type::Double:
                    ret.constRegs.back()->setDouble(std::stod(result.selections[index_selections[idx_ptr]].second));
                    break;
                case Attribute::Type::String:
                    ret.constRegs.back()->setString(result.selections[index_selections[idx_ptr]].second);
                    break;
                default:
                    if (result.selections[index_selections[idx_ptr]].second == "false" ||
                        result.selections[index_selections[idx_ptr]].second == "False" ||
                        result.selections[index_selections[idx_ptr]].second == "FALSE" ||
                        result.selections[index_selections[idx_ptr]].second == "0") {
                        ret.constRegs.back()->setBool(false);
                    }
                    else {
                        ret.constRegs.back()->setBool(true);
                    }
            }

            right = std::make_unique<Selection>(std::move(right), attr, ret.constRegs.back().get());
            ++idx_ptr;
        }

        root = std::make_unique<CrossProduct>(std::move(root), std::move(right));
    }

    for (auto joinReg: joinRegs) {
        root = std::make_unique<Selection>(std::move(root), joinReg.first, joinReg.second);
    }

    if (!result.projections.empty()) {
        root = std::make_unique<Projection>(std::move(root), projectionRegs);
    }

    ret.root = std::move(root);
    return ret;
}
