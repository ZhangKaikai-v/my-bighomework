#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H

#include "Python3BaseVisitor.h"
#include <string>
#include <iostream>
#include "Bigint.h"
#include <map>
#include <algorithm>
#include <vector>

std::map<std::string, antlrcpp::Any> my_map;
class EvalVisitor : public Python3BaseVisitor
{

    //判断any 的类型
    int Find_type(antlrcpp::Any val)
    {
        int flag = 0;
        if (val.is<std::string>())
            flag = 1;
        else if (val.is<Bigint>())
            flag = 2;
        else if (val.is<bool>())
            flag = 3;
        else if (val.is<double>())
            flag = 4;
        return flag;
    }

    //判断变量名是否在map中
    bool Find_map_key(std::map<std::string, antlrcpp::Any> my_map, std::string key)
    {
        std::map<std::string, antlrcpp::Any>::iterator iter = my_map.begin();
        iter = my_map.find(key);
        if (iter != my_map.end())
            return true;
        else
            return false;
    }

    antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override
    {
        if (ctx->simple_stmt() != nullptr)
        {
            return visitSimple_stmt(ctx->simple_stmt());
        }
        else
            return visitCompound_stmt(ctx->compound_stmt());
    }

    antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override
    {
        return visitSmall_stmt(ctx->small_stmt());
    }

    antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override
    {
        if (ctx->expr_stmt() != nullptr)
        {
            return visitExpr_stmt(ctx->expr_stmt());
        }
        else
        {
            return visitFlow_stmt(ctx->flow_stmt());
        }
    }
    // flag = 1 表示字符串，2 表示 bigint  3 表示 bool   4表示double  5 none(待补充)
    antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override
    {
        if (ctx->testlist().size() == 1)
            return visit(ctx->testlist()[0]);
        int sign_num = ctx->ASSIGN().size();
        if (sign_num >= 1)
        {
            antlrcpp::Any R_val = visit(ctx->testlist().back());
            // 如果被赋的值不是变量名
            bool judge = 0;
            if (R_val.is<std::string>())
            {
                std::string key = R_val.as<std::string>();
                judge = Find_map_key(my_map, key);
            }
            if (!judge)
            {
                int flag = Find_type(R_val);
                for (int i = 0; i < sign_num; i++)
                {
                    antlrcpp::Any name_tmp = visit(ctx->testlist()[i]->test()[0]);
                    std::string name_str = name_tmp.as<std::string>();
                    switch (flag)
                    {
                    case 1:
                    {
                        std::string tmp_Str = R_val.as<std::string>();
                        my_map[name_str] = tmp_Str;
                        break;
                    }
                    case 2:
                    {
                        Bigint tmp_Int = R_val.as<Bigint>();
                        my_map[name_str] = tmp_Int;
                        break;
                    }
                    case 3:
                    {
                        bool tmp_bool = R_val.as<bool>();
                        if (tmp_bool)
                        {
                            std::string true_str = "True";
                            my_map[name_str] = true_str;
                        }
                        else
                        {
                            std::string false_str = "False";
                            my_map[name_str] = false_str;
                        }
                        break;
                    }
                    case 4:
                    {
                        double tmp_double = R_val.as<double>();
                        my_map[name_str] = tmp_double;
                        break;
                    }
                    default:
                    {
                        std::cout << "error : Invalid type！" << std::endl;
                        break;
                    }
                    }
                }
                return R_val;
            }
            //如果赋值的是变量名
            else
            {
                std::string key = R_val.as<std::string>();
                antlrcpp::Any map_val = my_map[key];
                int flag = Find_type(map_val);
                for (int i = 0; i < sign_num; i++)
                {
                    antlrcpp::Any name_tmp = visit(ctx->testlist()[i]->test()[0]);
                    std::string name_str = name_tmp.as<std::string>();
                    switch (flag)
                    {
                    case 1:
                    {
                        std::string tmp_Str = map_val.as<std::string>();
                        my_map[name_str] = tmp_Str;
                        break;
                    }
                    case 2:
                    {
                        Bigint tmp_Int = map_val.as<Bigint>();
                        my_map[name_str] = tmp_Int;
                        break;
                    }
                    case 3:
                    {
                        bool tmp_bool = map_val.as<bool>();
                        if (tmp_bool)
                        {
                            std::string true_str = "True";
                            my_map[name_str] = true_str;
                        }
                        else
                        {
                            std::string false_str = "False";
                            my_map[name_str] = false_str;
                        }
                        break;
                    }
                    case 4:
                    {
                        double tmp_double = map_val.as<double>();
                        my_map[name_str] = tmp_double;
                        break;
                    }
                    default:
                    {
                        std::cout << "error : Invalid type！" << std::endl;
                        break;
                    }
                    }
                }
                return R_val;
            }
        }
        // += *=
        else
        {
            antlrcpp::Any key = visit(ctx->testlist()[0]);
            std::string map_key = key.as<std::string>();
            antlrcpp::Any left_val;
            if (Find_map_key(my_map, map_key))
            {
                left_val = my_map[map_key];
            }
            else
            {
                std::cerr << "Error : Undefined variables!\n";
                return 1;
            }
            antlrcpp::Any right_val = visit(ctx->testlist()[1]);
            if (right_val.is<std::string>())
            {
                if (Find_map_key(my_map, right_val.as<std::string>()))
                {
                    right_val = my_map[right_val.as<std::string>()];
                }
            }
            antlrcpp::Any any_sign_num = visit(ctx->augassign());
            double sign_num = any_sign_num.as<double>();
            switch ((int)sign_num)
            {
            case 1: // +=
            {
                if (left_val.is<Bigint>() && right_val.is<Bigint>())
                    my_map[map_key] = left_val.as<Bigint>() + right_val.as<Bigint>();

                else if (left_val.is<bool>() && right_val.is<Bigint>())
                    my_map[map_key] = Bigint(left_val.as<bool>()) + right_val.as<Bigint>();

                else if (left_val.is<Bigint>() && right_val.is<bool>())
                    my_map[map_key] = left_val.as<Bigint>() + Bigint(right_val.as<bool>());

                else if (left_val.is<bool>() && right_val.is<bool>())
                    my_map[map_key] = left_val.as<bool>() + right_val.as<bool>();

                else if (left_val.is<double>() && right_val.is<double>())
                    my_map[map_key] = left_val.as<double>() + right_val.as<double>();

                else if (left_val.is<double>() && right_val.is<bool>())
                    my_map[map_key] = left_val.as<double>() + right_val.as<bool>();

                else if (left_val.is<bool>() && right_val.is<double>())
                    my_map[map_key] = left_val.as<bool>() + right_val.as<double>();

                else if (left_val.is<double>() && right_val.is<Bigint>())
                    my_map[map_key] = left_val.as<double>() + double(right_val.as<Bigint>());

                else if (left_val.is<Bigint>() && right_val.is<double>())
                    my_map[map_key] = double(left_val.as<Bigint>()) + right_val.as<double>();

                else if (left_val.is<std::string>() && right_val.is<std::string>())
                    my_map[map_key] = left_val.as<std::string>() + right_val.as<std::string>();

                else
                {
                    std::cerr << "Error: Undefined operation!\n";
                    return 1;
                }
                break;
            }
            case 2: // -=
            {
                if (left_val.is<Bigint>() && right_val.is<Bigint>())
                    my_map[map_key] = left_val.as<Bigint>() - right_val.as<Bigint>();

                else if (left_val.is<bool>() && right_val.is<Bigint>())
                    my_map[map_key] = Bigint(left_val.as<bool>()) - right_val.as<Bigint>();

                else if (left_val.is<Bigint>() && right_val.is<bool>())
                    my_map[map_key] = left_val.as<Bigint>() - Bigint(right_val.as<bool>());

                else if (left_val.is<bool>() && right_val.is<bool>())
                    my_map[map_key] = left_val.as<bool>() - right_val.as<bool>();

                else if (left_val.is<double>() && right_val.is<double>())
                    my_map[map_key] = left_val.as<double>() - right_val.as<double>();

                else if (left_val.is<double>() && right_val.is<bool>())
                    my_map[map_key] = left_val.as<double>() - right_val.as<bool>();

                else if (left_val.is<bool>() && right_val.is<double>())
                    my_map[map_key] = left_val.as<bool>() - right_val.as<double>();

                else if (left_val.is<double>() && right_val.is<Bigint>())
                    my_map[map_key] = left_val.as<double>() - double(right_val.as<Bigint>());

                else if (left_val.is<Bigint>() && right_val.is<double>())
                    my_map[map_key] = double(left_val.as<Bigint>()) - right_val.as<double>();

                else
                {
                    std::cerr << "Error: Undefined operation!\n";
                    return 1;
                }
                break;
            }
            case 3: // *=
            {
                if (left_val.is<Bigint>() && right_val.is<Bigint>())
                    my_map[map_key] = left_val.as<Bigint>() * right_val.as<Bigint>();

                else if (left_val.is<bool>() && right_val.is<Bigint>())
                    my_map[map_key] = Bigint(left_val.as<bool>()) * right_val.as<Bigint>();

                else if (left_val.is<Bigint>() && right_val.is<bool>())
                    my_map[map_key] = left_val.as<Bigint>() * Bigint(right_val.as<bool>());

                else if (left_val.is<bool>() && right_val.is<bool>())
                    my_map[map_key] = left_val.as<bool>() * right_val.as<bool>();

                else if (left_val.is<double>() && right_val.is<double>())
                    my_map[map_key] = left_val.as<double>() * right_val.as<double>();

                else if (left_val.is<double>() && right_val.is<bool>())
                    my_map[map_key] = left_val.as<double>() * right_val.as<bool>();

                else if (left_val.is<bool>() && right_val.is<double>())
                    my_map[map_key] = left_val.as<bool>() * right_val.as<double>();

                else if (left_val.is<double>() && right_val.is<Bigint>())
                    my_map[map_key] = left_val.as<double>() * double(right_val.as<Bigint>());

                else if (left_val.is<Bigint>() && right_val.is<double>())
                    my_map[map_key] = double(left_val.as<Bigint>()) * right_val.as<double>();

                else if (left_val.is<Bigint>() && right_val.is<std::string>())
                {
                    if (left_val.as<Bigint>() == Bigint(0))
                    {
                        my_map[map_key] = "";
                    }
                    std::string str = right_val.as<std::string>();
                    double cnt = double(left_val.as<Bigint>());
                    for (double i = 1; i < cnt; i++)
                    {
                        str = str + right_val.as<std::string>();
                    }
                    my_map[map_key] = str;
                }
                else if (left_val.is<std::string>() && right_val.is<Bigint>())
                {
                    if (right_val.as<Bigint>() == Bigint(0))
                    {
                        my_map[map_key] = "";
                    }
                    std::string str = left_val.as<std::string>();
                    double cnt = double(right_val.as<Bigint>()); // 可能会出事
                    for (double i = 1; i < cnt; i++)
                    {
                        str = str + left_val.as<std::string>();
                    }
                    my_map[map_key] = str;
                }
                else if (left_val.is<bool>() && right_val.is<std::string>())
                {
                    if (left_val.as<bool>())
                        my_map[map_key] = right_val.as<std::string>();
                    else
                        my_map[map_key] = "";
                }
                else if (left_val.is<std::string>() && right_val.is<bool>())
                {
                    if (right_val.as<bool>())
                        my_map[map_key] = left_val.as<std::string>();
                    else
                        my_map[map_key] = "";
                }
                else
                {
                    std::cerr << "Error: Undefined operation!\n";
                    return 1;
                }
                break;
            }
            case 4: //    /=
            {
                if (left_val.is<Bigint>() && right_val.is<Bigint>())
                {
                    if (right_val.as<Bigint>() == (Bigint)0)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    else
                    {
                        antlrcpp::Any ans = (double)left_val.as<Bigint>() / (double)right_val.as<Bigint>();
                        my_map[map_key] = ans;
                    }
                }
                else if (left_val.is<bool>() && right_val.is<Bigint>())
                {
                    if (right_val.as<Bigint>() == Bigint(0))
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    my_map[map_key] = double(left_val.as<bool>()) / (double)right_val.as<Bigint>();
                }
                else if (left_val.is<Bigint>() && right_val.is<bool>())
                {
                    if (right_val.as<bool>() == false)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    my_map[map_key] = (double)left_val.as<Bigint>() / (double)(right_val.as<bool>());
                }
                else if (left_val.is<bool>() && right_val.is<bool>())
                {
                    if (right_val.as<bool>() == false)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    my_map[map_key] = (double)left_val.as<bool>() / (double)right_val.as<bool>();
                }
                else if (left_val.is<double>() && right_val.is<Bigint>())
                {
                    if (right_val.as<Bigint>() == (Bigint)0)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    else
                        my_map[map_key] = left_val.as<double>() / (double)right_val.as<Bigint>();
                }
                else if (left_val.is<Bigint>() && right_val.is<double>())
                {
                    if (right_val.as<double>() == 0)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    else
                        my_map[map_key] = (double)left_val.as<Bigint>() / right_val.as<double>();
                }
                else if (left_val.is<double>() && right_val.is<bool>())
                {
                    if (right_val.as<bool>() == false)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    else
                        my_map[map_key] = left_val.as<double>() / (double)right_val.as<bool>();
                }
                else if (left_val.is<bool>() && right_val.is<double>())
                {
                    if (right_val.as<double>() == 0)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    else
                        my_map[map_key] = (double)left_val.as<bool>() / right_val.as<double>();
                }
                else if (left_val.is<double>() && right_val.is<double>())
                {
                    if (right_val.as<double>() == 0)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    else
                        my_map[map_key] = left_val.as<double>() / right_val.as<double>();
                }
                else
                {
                    std::cerr << "Error: Undefined operation!\n";
                    return 1;
                }
                break;
            }
            case 5: //   //=
            {
                if (left_val.is<Bigint>() && right_val.is<Bigint>())
                {
                    if (right_val.as<Bigint>() == (Bigint)0)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    else
                        my_map[map_key] = left_val.as<Bigint>() / right_val.as<Bigint>();
                }

                else if (left_val.is<bool>() && right_val.is<Bigint>())
                {
                    if (right_val.as<Bigint>() == Bigint(0))
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    my_map[map_key] = Bigint(left_val.as<bool>()) / right_val.as<Bigint>();
                }
                else if (left_val.is<Bigint>() && right_val.is<bool>())
                {
                    if (right_val.as<bool>() == false)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    my_map[map_key] = left_val.as<Bigint>() / Bigint(right_val.as<bool>());
                }
                else if (left_val.is<bool>() && right_val.is<bool>())
                {
                    if (right_val.as<bool>() == false)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    my_map[map_key] = left_val.as<bool>() / right_val.as<bool>();
                }
                else if (left_val.is<double>() || right_val.is<double>())
                {
                    std::cerr << "Undefined behavior!\n";
                    return 1;
                }
                else
                {
                    std::cerr << "Error: Undefined operation!\n";
                    return 1;
                }
                break;
            }
            case 6:
            {
                if (left_val.is<Bigint>() && right_val.is<Bigint>())
                {
                    if (right_val.as<Bigint>() == (Bigint)0)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    else
                        my_map[map_key] = left_val.as<Bigint>() % right_val.as<Bigint>();
                }

                else if (left_val.is<bool>() && right_val.is<Bigint>())
                {
                    if (right_val.as<Bigint>() == Bigint(0))
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    my_map[map_key] = Bigint(left_val.as<bool>()) % right_val.as<Bigint>();
                }
                else if (left_val.is<Bigint>() && right_val.is<bool>())
                {
                    if (right_val.as<bool>() == false)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    my_map[map_key] = left_val.as<Bigint>() % Bigint(right_val.as<bool>());
                }
                else if (left_val.is<bool>() && right_val.is<bool>())
                {
                    if (right_val.as<bool>() == false)
                    {
                        std::cerr << "Error : The divisor cannot be zero!\n";
                        return 1;
                    }
                    my_map[map_key] = left_val.as<bool>() % right_val.as<bool>();
                }
                else if (left_val.is<double>() || right_val.is<double>())
                {
                    std::cerr << "Undefined behavior!\n";
                    return 1;
                }
                else
                {
                    std::cerr << "Error: Undefined operation!\n";
                    return 1;
                }
                break;
            }
            default:
            {
                std::cerr << "Error : Undefined operator\n";
                return 1;
            }
            }
            return left_val;
        }
    }

    antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override
    {
        if (ctx->ADD_ASSIGN() != nullptr)
        {
            return 1.0;
        }
        else if (ctx->SUB_ASSIGN() != nullptr)
        {
            return 2.0;
        }
        else if (ctx->MULT_ASSIGN() != nullptr)
        {
            return 3.0;
        }
        else if (ctx->DIV_ASSIGN() != nullptr)
        {
            return 4.0;
        }
        else if (ctx->IDIV_ASSIGN() != nullptr)
        {
            return 5.0;
        }
        else if (ctx->MULT_ASSIGN() != nullptr)
        {
            return 6.0;
        }
    }

    antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override
    {
        if (ctx -> break_stmt() != nullptr)
        {
            return 1.0;
        }
        else if (ctx -> continue_stmt() != nullptr)
        {
            return 2.0;
        }
        else return 3.0;
    }

    antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override
    {
        int flag = 0;
        int num = ctx->test().size();
        for (int i = 0; i < num; i++)
        {
            antlrcpp::Any judge = visit(ctx->test()[i]);
            if (judge.is<std::string>())
            {
                if (Find_map_key(my_map, judge.as<std::string>()))
                {
                    judge = my_map[judge.as<std::string>()];
                }
            }
            if (judge.is<bool>())
            {
                if (judge.as<bool>())
                    flag = 1;
            }
            else if (judge.is<std::string>())
            {
                if (!judge.as<std::string>().empty())
                    flag = 1;
            }
            else if (judge.is<Bigint>())
            {
                if (judge.as<Bigint>() != Bigint(0))
                    flag = 1;
            }
            else if (judge.is<double>())
            {
                if (judge.as<double>() != 0)
                    flag = 1;
            }
            else
            {
                std::cerr << "Error : Invalid judgment conditions !\n";
                return 1;
            }
            if (flag)
            {
                visitSuite(ctx->suite()[i]);
                break;
            }
        }
        if (ctx->ELSE() != nullptr && flag == 0)
        {
            visitSuite(ctx->suite()[num]);
        }
        return 1;
    }

    antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override
    {
        antlrcpp::Any judge = visit(ctx->test());
        antlrcpp::Any ret;
        int flag = 0;
        if (judge.is<std::string>())
        {
            if (Find_map_key(my_map, judge.as<std::string>()))
            {
                judge = my_map[judge.as<std::string>()];
            }
        }
        if (judge.is<bool>())
        {
            if (judge.as<bool>())
                flag = 1;
        }
        else if (judge.is<std::string>())
        {
            if (!judge.as<std::string>().empty())
                flag = 1;
        }
        else if (judge.is<Bigint>())
        {
            if (judge.as<Bigint>() != Bigint(0))
                flag = 1;
        }
        else if (judge.is<double>())
        {
            if (judge.as<double>() != 0)
                flag = 1;
        }
        else
        {
            std::cerr << "Error : Invalid judgment conditions !\n";
            return 1;
        }

        while(flag)
        {
            ret = visitSuite(ctx -> suite());
            judge = visit(ctx->test());
            flag = 0;
            if (ret.is<double>())
            {
                if (ret.as<double>() == 10.0)
                    break;
            }
            if (judge.is<std::string>())
            {
                if (Find_map_key(my_map, judge.as<std::string>()))
                {
                    judge = my_map[judge.as<std::string>()];
                }
            }
            if (judge.is<bool>())
            {
                if (judge.as<bool>())
                    flag = 1;
             }
            else if (judge.is<std::string>())
            {
                if (!judge.as<std::string>().empty())
                    flag = 1;
            }
            else if (judge.is<Bigint>())
            {
                if (judge.as<Bigint>() != Bigint(0))
                    flag = 1;
            }
            else if (judge.is<double>())
            {
                if (judge.as<double>() != 0)
                    flag = 1;
            }
            else
            {
                std::cerr << "Error : Invalid judgment conditions !\n";
                return 1;
            }
        }
        return 1;
    }

    antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override
    {
        antlrcpp::Any ret = nullptr;
        if (ctx -> simple_stmt() != nullptr)
        {
            ret = visit(ctx -> simple_stmt());
            return ret;
        }
        else
        {
            int num = ctx -> stmt().size();
            for (int i = 0; i < num; i++)
            {
                ret = visit(ctx -> stmt()[i]);
                if (ret.is<double>())
                {
                    if (ret.as<double>() == 1.0)
                    {
                        return 10.0;
                    }
                    else if (ret.as<double>() == 2.0)
                    { 
                        return 20.0;
                    }
                }
            }
            return ret;
        }
    }

    antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override
    {
        return visitOr_test(ctx->or_test());
    }

    antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override
    {
        if (ctx->and_test().size() == 1)
            return visitAnd_test(ctx->and_test()[0]);
        else
        {
            int x = ctx->and_test().size();
            for (int i = 0; i < x; i++)
            {
                antlrcpp::Any tmp_and = visit(ctx->and_test()[i]);
                if (tmp_and.is<std::string>())
                {
                    if (Find_map_key(my_map, tmp_and.as<std::string>()))
                    {
                        tmp_and = my_map[tmp_and.as<std::string>()];
                    }
                }
                if (tmp_and.is<Bigint>())
                {
                    if (tmp_and.as<Bigint>())
                        return true;
                    else
                        continue;
                }
                else if (tmp_and.is<double>())
                {
                    if (tmp_and.as<double>())
                        return true;
                    else
                        continue;
                }
                else if (tmp_and.is<bool>())
                {
                    if (tmp_and.as<bool>())
                        return true;
                    else
                        continue;
                }
                else if (tmp_and.is<std::string>())
                {
                    if (!tmp_and.as<std::string>().empty())
                        return true;
                    else
                        continue;
                }
                else
                {
                    std::cerr << "Errortype ! \n";
                    return 1;
                }
            }
            return false;
        }
    }

    antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override
    {
        if (ctx->not_test().size() == 1)
            return visitNot_test(ctx->not_test()[0]);
        else
        {
            int x = ctx->not_test().size();
            for (int i = 0; i < x; i++)
            {
                antlrcpp::Any tmp_not = visit(ctx->not_test()[i]);
                if (tmp_not.is<std::string>())
                {
                    if (Find_map_key(my_map, tmp_not.as<std::string>()))
                    {
                        tmp_not = my_map[tmp_not.as<std::string>()];
                    }
                }
                if (tmp_not.is<Bigint>())
                {
                    if (tmp_not.as<Bigint>())
                        continue;
                    else
                        return false;
                }
                else if (tmp_not.is<double>())
                {
                    if (tmp_not.as<double>())
                        continue;
                    else
                        return false;
                }
                else if (tmp_not.is<bool>())
                {
                    if (tmp_not.as<bool>())
                        continue;
                    else
                        return false;
                }
                else if (tmp_not.is<std::string>())
                {
                    if (!tmp_not.as<std::string>().empty())
                        continue;
                    else
                        return false;
                }
                else
                {
                    std::cerr << "Errortype ! \n";
                    return 1;
                }
            }
            return true;
        }
    }

    antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override
    {
        if (ctx->comparison() != nullptr)
            return visitComparison(ctx->comparison());
        else
        {
            antlrcpp::Any tmp_end_not = visit(ctx->not_test());
            if (tmp_end_not.is<std::string>())
            {
                if (Find_map_key(my_map, tmp_end_not.as<std::string>()))
                {
                    tmp_end_not = my_map[tmp_end_not.as<std::string>()];
                }
            }
            if (tmp_end_not.is<Bigint>())
            {
                if (tmp_end_not.as<Bigint>())
                    return false;
                else
                    return true;
            }
            else if (tmp_end_not.is<double>())
            {
                if (tmp_end_not.as<double>())
                    return false;
                else
                    return true;
            }
            else if (tmp_end_not.is<bool>())
            {
                if (tmp_end_not.as<bool>())
                    return false;
                else
                    return true;
            }
            else if (tmp_end_not.is<std::string>())
            {
                if (tmp_end_not.as<std::string>().empty())
                    return true;
                else
                    return false;
            }
            else
            {
                std::cerr << "Errortype ! \n";
                return 1;
            }
        }
    }

    antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx) override
    {
        if (ctx->comp_op().empty())
            return visitArith_expr(ctx->arith_expr()[0]);
        else
        {
            int x = ctx->comp_op().size();
            bool flag = 1;
            for (int i = 0; i < x; i++)
            {
                antlrcpp::Any comp_token_enum = visit(ctx->comp_op()[i]);
                double comp_token = comp_token_enum.as<double>();
                antlrcpp::Any left_val = visit(ctx->arith_expr()[i]);
                if (left_val.is<std::string>())
                {
                    if (Find_map_key(my_map, left_val.as<std::string>()))
                    {
                        left_val = my_map[left_val.as<std::string>()];
                    }
                }
                antlrcpp::Any right_val = visit(ctx->arith_expr()[i + 1]);
                if (right_val.is<std::string>())
                {
                    if (Find_map_key(my_map, right_val.as<std::string>()))
                    {
                        right_val = my_map[right_val.as<std::string>()];
                    }
                }
                switch ((int)comp_token)
                {
                case 1: // <
                {
                    if (left_val.is<Bigint>() && right_val.is<Bigint>())
                    {
                        if (left_val.as<Bigint>() >= right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<Bigint>())
                    {
                        if (Bigint(left_val.as<bool>()) >= right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<bool>())
                    {
                        if (left_val.as<Bigint>() >= Bigint(right_val.as<bool>()))
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<bool>())
                    {
                        if (left_val.as<bool>() >= right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<double>())
                    {
                        if (left_val.as<double>() >= right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<bool>())
                    {
                        if (left_val.as<double>() >= right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<double>())
                    {
                        if (left_val.as<bool>() >= right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<Bigint>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (left_val.as<double>() >= double(right_val.as<Bigint>()))
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<double>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (double(left_val.as<Bigint>()) >= right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<std::string>() && right_val.is<std::string>())
                    {
                        if (left_val.as<std::string>() >= right_val.as<std::string>())
                            flag = 0;
                    }
                    else
                    {
                        std::cerr << "TypeError: '<' not supported between these two instances !";
                        return 1;
                    }
                    break;
                }
                case 2: // >
                {
                    if (left_val.is<Bigint>() && right_val.is<Bigint>())
                    {
                        if (left_val.as<Bigint>() <= right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<Bigint>())
                    {
                        if (Bigint(left_val.as<bool>()) <= right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<bool>())
                    {
                        if (left_val.as<Bigint>() <= Bigint(right_val.as<bool>()))
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<bool>())
                    {
                        if (left_val.as<bool>() <= right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<double>())
                    {
                        if (left_val.as<double>() <= right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<bool>())
                    {
                        if (left_val.as<double>() <= right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<double>())
                    {
                        if (left_val.as<bool>() <= right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<Bigint>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (left_val.as<double>() <= double(right_val.as<Bigint>()))
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<double>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (double(left_val.as<Bigint>()) <= right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<std::string>() && right_val.is<std::string>())
                    {
                        if (left_val.as<std::string>() <= right_val.as<std::string>())
                            flag = 0;
                    }
                    else
                    {
                        std::cerr << "TypeError: '>' not supported between these two instances !";
                        return 1;
                    }
                    break;
                }
                case 3: // ==
                {
                    if (left_val.is<Bigint>() && right_val.is<Bigint>())
                    {
                        if (left_val.as<Bigint>() != right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<Bigint>())
                    {
                        if (Bigint(left_val.as<bool>()) != right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<bool>())
                    {
                        if (left_val.as<Bigint>() != Bigint(right_val.as<bool>()))
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<bool>())
                    {
                        if (left_val.as<bool>() != right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<double>())
                    {
                        if (left_val.as<double>() != right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<bool>())
                    {
                        if (left_val.as<double>() != right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<double>())
                    {
                        if (left_val.as<bool>() != right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<Bigint>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (left_val.as<double>() != double(right_val.as<Bigint>()))
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<double>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (double(left_val.as<Bigint>()) != right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<std::string>() && right_val.is<std::string>())
                    {
                        if (left_val.as<std::string>() != right_val.as<std::string>())
                            flag = 0;
                    }
                    else if (left_val.is<std::string>() && right_val.is<std::string>())
                    {
                        if (left_val.as<std::string>() != right_val.as<std::string>())
                            flag = 0;
                    }
                    else
                    {
                        std::cerr << "TypeError: '==' not supported between these two instances !";
                        return 1;
                    }
                    break;
                }
                case 4: // >=
                {
                    if (left_val.is<Bigint>() && right_val.is<Bigint>())
                    {
                        if (left_val.as<Bigint>() < right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<Bigint>())
                    {
                        if (Bigint(left_val.as<bool>()) < right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<bool>())
                    {
                        if (left_val.as<Bigint>() < Bigint(right_val.as<bool>()))
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<bool>())
                    {
                        if (left_val.as<bool>() < right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<double>())
                    {
                        if (left_val.as<double>() < right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<bool>())
                    {
                        if (left_val.as<double>() < right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<double>())
                    {
                        if (left_val.as<bool>() < right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<Bigint>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (left_val.as<double>() < double(right_val.as<Bigint>()))
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<double>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (double(left_val.as<Bigint>()) < right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<std::string>() && right_val.is<std::string>())
                    {
                        if (left_val.as<std::string>() < right_val.as<std::string>())
                            flag = 0;
                    }
                    else
                    {
                        std::cerr << "TypeError: '>=' not supported between these two instances !";
                        return 1;
                    }
                    break;
                }
                case 5: // <=
                {
                    if (left_val.is<Bigint>() && right_val.is<Bigint>())
                    {
                        if (left_val.as<Bigint>() > right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<Bigint>())
                    {
                        if (Bigint(left_val.as<bool>()) > right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<bool>())
                    {
                        if (left_val.as<Bigint>() > Bigint(right_val.as<bool>()))
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<bool>())
                    {
                        if (left_val.as<bool>() > right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<double>())
                    {
                        if (left_val.as<double>() > right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<bool>())
                    {
                        if (left_val.as<double>() > right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<double>())
                    {
                        if (left_val.as<bool>() > right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<Bigint>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (left_val.as<double>() > double(right_val.as<Bigint>()))
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<double>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (double(left_val.as<Bigint>()) > right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<std::string>() && right_val.is<std::string>())
                    {
                        if (left_val.as<std::string>() > right_val.as<std::string>())
                            flag = 0;
                    }
                    else
                    {
                        std::cerr << "TypeError: '<=' not supported between these two instances !";
                        return 1;
                    }
                    break;
                }
                case 6: // !=
                {
                    if (left_val.is<Bigint>() && right_val.is<Bigint>())
                    {
                        if (left_val.as<Bigint>() == right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<Bigint>())
                    {
                        if (Bigint(left_val.as<bool>()) == right_val.as<Bigint>())
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<bool>())
                    {
                        if (left_val.as<Bigint>() == Bigint(right_val.as<bool>()))
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<bool>())
                    {
                        if (left_val.as<bool>() == right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<double>())
                    {
                        if (left_val.as<double>() == right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<bool>())
                    {
                        if (left_val.as<double>() == right_val.as<bool>())
                            flag = 0;
                    }
                    else if (left_val.is<bool>() && right_val.is<double>())
                    {
                        if (left_val.as<bool>() == right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<double>() && right_val.is<Bigint>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (left_val.as<double>() == double(right_val.as<Bigint>()))
                            flag = 0;
                    }
                    else if (left_val.is<Bigint>() && right_val.is<double>())
                    {
                        // 可能会出问题   Bigint 转 double 精度丢失
                        if (double(left_val.as<Bigint>()) == right_val.as<double>())
                            flag = 0;
                    }
                    else if (left_val.is<std::string>() && right_val.is<std::string>())
                    {
                        if (left_val.as<std::string>() == right_val.as<std::string>())
                            flag = 0;
                    }
                    else
                    {
                        std::cerr << "TypeError: '!=' not supported between these two instances !";
                        return 1;
                    }
                    break;
                }
                default:
                {
                    std::cerr << "Error : Undefined comparison symbol ! \n";
                    return 1;
                }
                }
            }
            if (flag)
                return true;
            else
                return false;
        }
    }

    antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override
    {
        if (ctx->LESS_THAN() != nullptr)
        {
            return 1.0;
        }
        else if (ctx->GREATER_THAN() != nullptr)
        {
            return 2.0;
        }
        else if (ctx->EQUALS() != nullptr)
        {
            return 3.0;
        }
        else if (ctx->GT_EQ() != nullptr)
        {
            return 4.0;
        }
        else if (ctx->LT_EQ() != nullptr)
        {
            return 5.0;
        }
        else if (ctx->NOT_EQ_2() != nullptr)
        {
            return 6.0;
        }
    }

    antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override
    {
        if (ctx->term().size() == 1)
            return visitTerm(ctx->term()[0]);
        else
        {
            // ok
            int token_num = ctx->term().size() - 1;
            int add_rec = 0, minus_rec = 0;
            int add_size = ctx->ADD().size();
            int minus_size = ctx->MINUS().size();
            antlrcpp::Any ans = visit(ctx->term()[0]);
            //判断左值是否为变量名
            if (ans.is<std::string>())
            {
                if (Find_map_key(my_map, ans.as<std::string>()))
                {
                    ans = my_map[ans.as<std::string>()];
                }
            }
            int add_index = 1e9;
            int minus_index = 1e9;
            for (int i = 0; i < token_num; i++)
            {
                antlrcpp::Any tmp = visit(ctx->term()[i + 1]);
                //判断右值是否为变量名
                if (tmp.is<std::string>())
                {
                    if (Find_map_key(my_map, tmp.as<std::string>()))
                    {
                        tmp = my_map[tmp.as<std::string>()];
                    }
                }
                if (add_size > 0)
                {
                    add_index = ctx->ADD()[add_rec]->getSymbol()->getTokenIndex();
                }
                if (minus_size > 0)
                {
                    minus_index = ctx->MINUS()[minus_rec]->getSymbol()->getTokenIndex();
                }
                if (add_index < minus_index)
                {
                    // Bigint + Bigint
                    if (ans.is<Bigint>() && tmp.is<Bigint>())
                    {
                        ans = ans.as<Bigint>() + tmp.as<Bigint>();
                    }
                    //double + Bigint
                    else if (ans.is<double>() && tmp.is<Bigint>())
                    {
                        ans = ans.as<double>() + double(tmp.as<Bigint>());
                    }
                    //Bigint + double
                    else if (ans.is<Bigint>() && tmp.is<double>())
                    {
                        ans = double(ans.as<Bigint>()) + tmp.as<double>();
                    }
                    //double + double
                    else if (ans.is<double>() && tmp.is<double>())
                    {
                        ans = ans.as<double>() + tmp.as<double>();
                    }
                    //Bigint + bool
                    else if (ans.is<Bigint>() && tmp.is<bool>())
                    {
                        ans = ans.as<Bigint>() + Bigint(tmp.as<bool>());
                    }
                    //bool + bigint
                    else if (ans.is<bool>() && tmp.is<Bigint>())
                    {
                        ans = Bigint(ans.as<bool>()) + tmp.as<Bigint>();
                    }
                    //bool + bool
                    else if (ans.is<bool>() && tmp.is<bool>())
                    {
                        ans = ans.as<bool>() + tmp.as<bool>();
                    }
                    //bool + double
                    else if (ans.is<bool>() && tmp.is<double>())
                    {
                        ans = double(ans.as<bool>()) + tmp.as<double>();
                    }
                    //double + bool
                    else if (ans.is<double>() && tmp.is<bool>())
                    {
                        ans = ans.as<double>() + double(tmp.as<bool>());
                    }
                    //string + string
                    else if (ans.is<std::string>() && tmp.is<std::string>())
                    {
                        ans = ans.as<std::string>() + tmp.as<std::string>();
                    }
                    else
                    {
                        std::cerr << "Error operation type! \n ";
                        return 1;
                    }
                    add_rec += 1;
                }
                else
                {
                    //Bigint - Bigint
                    if (ans.is<Bigint>() && tmp.is<Bigint>())
                    {
                        ans = ans.as<Bigint>() + tmp.as<Bigint>();
                    }
                    //double - Bigint
                    else if (ans.is<double>() && tmp.is<Bigint>())
                    {
                        ans = ans.as<double>() - double(tmp.as<Bigint>());
                    }
                    //Bigint - double
                    else if (ans.is<Bigint>() && tmp.is<double>())
                    {
                        ans = double(ans.as<Bigint>()) - tmp.as<double>();
                    }
                    //double - double
                    else if (ans.is<double>() && tmp.is<double>())
                    {
                        ans = ans.as<double>() - tmp.as<double>();
                    }
                    //Bigint - bool
                    else if (ans.is<Bigint>() && tmp.is<bool>())
                    {
                        ans = ans.as<Bigint>() - Bigint(tmp.as<bool>());
                    }
                    //bool - Bigint
                    else if (ans.is<bool>() && tmp.is<Bigint>())
                    {
                        ans = Bigint((ans.as<bool>())) - tmp.as<Bigint>();
                    }
                    //bool - bool
                    else if (ans.is<bool>() && tmp.is<bool>())
                    {
                        ans = Bigint((ans.as<bool>())) - Bigint((tmp.as<bool>()));
                    }
                    //bool - double
                    else if (ans.is<bool>() && tmp.is<double>())
                    {
                        ans = double(ans.as<bool>()) - tmp.as<double>();
                    }
                    //double - bool
                    else if (ans.is<double>() && tmp.is<bool>())
                    {
                        ans = ans.as<double>() - double(tmp.as<bool>());
                    }
                    else
                    {
                        std::cerr << "Error operation type! \n ";
                        return 1;
                    }
                    minus_rec += 1;
                }
            }
            return ans;
        }
    }

    antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override
    {
        if (ctx->factor().size() == 1)
            return visitFactor(ctx->factor()[0]);
        else
        {
            // star *   div /   idiv // mod %
            int token_num = ctx->factor().size() - 1;
            int star_rec = 0, div_rec = 0, idiv_rec = 0, mod_rec = 0;
            int star_size = ctx->STAR().size();
            int div_size = ctx->DIV().size();
            int idiv_size = ctx->IDIV().size();
            int mod_size = ctx->MOD().size();
            int star_index = 1e9;
            int div_index = 1e9;
            int idiv_index = 1e9;
            int mod_index = 1e9;
            antlrcpp::Any ans = visit(ctx->factor()[0]);
            if (ans.is<std::string>())
            {
                if (Find_map_key(my_map, ans.as<std::string>()))
                    ans = my_map[ans.as<std::string>()];
            }
            for (int i = 1; i < token_num + 1; i++)
            {
                antlrcpp::Any tmp = visit(ctx->factor()[i]);

                if (tmp.is<std::string>())
                {
                    if (Find_map_key(my_map, tmp.as<std::string>()))
                    {
                        tmp = my_map[tmp.as<std::string>()];
                    }
                }
                if (star_size > 0)
                {
                    star_index = ctx->STAR()[star_rec]->getSymbol()->getTokenIndex();
                }
                if (div_size > 0)
                {
                    div_index = ctx->DIV()[div_rec]->getSymbol()->getTokenIndex();
                }
                if (idiv_size > 0)
                {
                    idiv_index = ctx->IDIV()[idiv_rec]->getSymbol()->getTokenIndex();
                }
                if (mod_size > 0)
                {
                    mod_index = ctx->MOD()[mod_rec]->getSymbol()->getTokenIndex();
                }

                int tmp_index = min(min(star_index, div_index), min(idiv_index, mod_index));

                if (tmp_index == star_index)
                {
                    if (ans.is<Bigint>() && tmp.is<Bigint>())
                    {
                        ans = ans.as<Bigint>() * tmp.as<Bigint>();
                    }
                    else if (ans.is<Bigint>() && tmp.is<double>())
                    {
                        ans = double(ans.as<Bigint>()) * tmp.as<double>();
                    }
                    else if ((ans.is<double>() && tmp.is<Bigint>()))
                    {
                        ans = ans.as<double>() * double(tmp.as<Bigint>());
                    }
                    else if (ans.is<Bigint>() && tmp.is<std::string>())
                    {
                        if (ans.as<Bigint>() == (Bigint)0)
                        {
                            ans = "";
                            continue;
                        }
                        std::string str = tmp.as<std::string>();
                        double cnt = double(ans.as<Bigint>());
                        for (double i = 1; i < cnt; i++)
                        {
                            str = str + tmp.as<std::string>();
                        }
                        ans = str;
                    }
                    else if (ans.is<std::string>() && tmp.is<Bigint>())
                    {
                        if (tmp.as<Bigint>() == Bigint(0))
                        {
                            ans = "";
                            continue;
                        }
                        std::string str = ans.as<std::string>();
                        double cnt = double(tmp.as<Bigint>()); //可能会出事
                        //  for (Bigint i(0); i < tmp.as<Bigint>(); i++)  //WHY？？
                        for (double i = 1; i < cnt; i++)
                        {
                            str = str + ans.as<std::string>();
                        }
                        ans = str;
                    }
                    else if (ans.is<bool>() && tmp.is<bool>())
                    {
                        ans = Bigint(ans.as<bool>()) * Bigint(tmp.as<bool>());
                    }
                    else if (ans.is<bool>() && tmp.is<Bigint>())
                    {
                        ans = Bigint(ans.as<bool>()) * tmp.as<Bigint>();
                    }
                    else if (ans.is<Bigint>() && tmp.is<bool>())
                    {
                        ans = ans.as<Bigint>() * Bigint(tmp.as<bool>());
                    }
                    else if (ans.is<bool>() && tmp.is<std::string>())
                    {
                        if (ans.as<bool>())
                            ans = tmp.as<std::string>();
                        else
                            ans = "";
                    }
                    else if (ans.is<std::string>() && tmp.is<bool>())
                    {
                        if (!tmp.as<bool>())
                            ans = "";
                    }
                    else
                    {
                        std::cerr << "Error operation type!\n ";
                        return 1;
                    }
                    star_rec += 1;
                }
                else if (div_index == tmp_index)
                {
                    if (ans.is<Bigint>() && tmp.is<Bigint>())
                    {
                        if (tmp.as<Bigint>() == Bigint(0))
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = double(ans.as<Bigint>()) / double(tmp.as<Bigint>());
                    }
                    else if (ans.is<bool>() && tmp.is<bool>())
                    {
                        if (tmp.as<bool>() == false)
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = double(ans.as<bool>()) / double(tmp.as<bool>());
                    }
                    else if (ans.is<bool>() && tmp.is<Bigint>())
                    {
                        if (tmp.as<Bigint>() == (Bigint)0)
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = (double)ans.as<bool>() / (double)tmp.as<Bigint>();
                    }
                    else if (ans.is<Bigint>() && tmp.is<bool>())
                    {
                        if (tmp.as<bool>() == false)
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = double(ans.as<Bigint>()) / double((tmp.as<bool>()));
                    }
                    else
                    {
                        std::cerr << "Error operation type!\n ";
                        return 1;
                    }
                    div_rec += 1;
                }
                else if (idiv_index == tmp_index)
                {
                    if (ans.is<Bigint>() && tmp.is<Bigint>())
                    {
                        if (tmp.as<Bigint>() == (Bigint)0)
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = ans.as<Bigint>() / tmp.as<Bigint>();
                    }
                    else if (ans.is<bool>() && tmp.is<bool>())
                    {
                        if (tmp.as<bool>() == false)
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = Bigint(ans.as<bool>()) / Bigint(tmp.as<bool>());
                    }
                    else if (ans.is<bool>() && tmp.is<Bigint>())
                    {
                        if (tmp.as<Bigint>() == (Bigint)0)
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = Bigint((ans.as<bool>())) / tmp.as<Bigint>();
                    }
                    else if (ans.is<Bigint>() && tmp.is<bool>())
                    {
                        if (tmp.as<bool>() == false)
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = ans.as<Bigint>() / Bigint(tmp.as<bool>());
                    }
                    else if (ans.is<double>() || tmp.is<double>())
                    {
                        std::cerr << "Undefined behavior\n ";
                        return 1;
                    }
                    else
                    {
                        std::cerr << "Error operation type!\n ";
                        return 1;
                    }
                    idiv_rec += 1;
                }
                else if (mod_index == tmp_index)
                {
                    if (ans.is<Bigint>() && tmp.is<Bigint>())
                    {
                        if (tmp.as<Bigint>() == Bigint(0))
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = ans.as<Bigint>() % tmp.as<Bigint>();
                    }
                    else if (ans.is<bool>() && tmp.is<bool>())
                    {
                        if (tmp.as<bool>() == false)
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = Bigint((ans.as<bool>())) % Bigint((tmp.as<bool>()));
                    }
                    else if (ans.is<bool>() && tmp.is<Bigint>())
                    {
                        if (tmp.as<Bigint>() == Bigint(0))
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = Bigint((ans.as<bool>())) % tmp.as<Bigint>();
                    }
                    else if (ans.is<Bigint>() && tmp.is<bool>())
                    {
                        if (tmp.as<bool>() == false)
                        {
                            std::cerr << "Error : The divisor cannot be zero!\n";
                            return 1;
                        }
                        ans = ans.as<Bigint>() % Bigint((tmp.as<bool>()));
                    }
                    else if (ans.is<double>() || tmp.is<double>())
                    {
                        std::cerr << "Undefined behavior\n ";
                        return 1;
                    }
                    else
                    {
                        std::cerr << "Error operation type!\n ";
                        return 1;
                    }
                    mod_rec += 1;
                }
            }
            return ans;
        }
    }

    //待补充
    antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override
    {
        if (ctx->factor() == nullptr)
            return visitAtom_expr(ctx->atom_expr());
        else
        {
            antlrcpp::Any ans = visit(ctx->factor());
            if (ctx->ADD() != nullptr)
            {
                if (ans.is<Bigint>())
                {
                    return ans.as<Bigint>();
                }
            }
            else
            {
                if (ans.is<Bigint>())
                {
                    return ans.as<Bigint>() * Bigint(-1);
                }
            }
        }
    }

    antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override
    {
        if (!ctx->trailer())
            return visitAtom(ctx->atom());
        std::string ret = ctx->atom()->NAME()->toString();
        if (ret == "print")
        {
            antlrcpp::Any tmp = visit(ctx->trailer()->arglist()->argument()[0]->test());
            if (tmp.is<std::string>())
            {
                std::string Key = tmp.as<std::string>();
                if (Find_map_key(my_map, Key)) // 从map 中查找 看是否为变量名
                {
                    antlrcpp::Any tmp_val = my_map[Key];
                    if (tmp_val.is<Bigint>())
                    {
                        Bigint ans = tmp_val.as<Bigint>();
                        std::cout << ans << std::endl;
                        return ans;
                    }
                    else if (tmp_val.is<std::string>())
                    {
                        std::string str = tmp_val.as<std::string>();
                        std::cout << str << std::endl;
                        return str;
                    }
                    else if (tmp_val.is<double>())
                    {
                        double ans = tmp_val.as<double>();
                        std::cout << ans << std::endl;
                        return ans;
                    }
                }
                std::cout << Key << std::endl;
                return Key;
            }
            else if (tmp.is<double>())
            {
                std::cout << tmp.as<double>() << std::endl;
            }
            else if (tmp.is<Bigint>())
            {
                std::cout << tmp.as<Bigint>() << std::endl;
            }
            else if (tmp.is<bool>())
            {
                bool x = tmp.as<bool>();
                if (x)
                    std::cout << "True" << std::endl;
                else
                    std::cout << "False" << std::endl;
            }
            return tmp;
        }
    }

    antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override
    {
        return visitChildren(ctx);
    }

    antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override
    {
        if (ctx->NAME() != nullptr)
        {
            std::string str = ctx->NAME()->toString();
            return str;
        }
        else if (ctx->STRING().size() != 0)
        {
            std::string tmpStr;
            for (int i = 0; i < ctx->STRING().size(); i++)
            {
                std::string str = ctx->STRING()[i]->toString();
                int len = str.size();
                str = str.substr(1, len - 2);
                tmpStr += str;
            }
            return tmpStr;
        }
        else if (ctx->NUMBER() != nullptr)
        {
            char c[5000];
            std::string s1 = ctx->NUMBER()->toString();
            strcpy(c, s1.c_str());
            if (s1.find(".") == std::string::npos)
            {
                Bigint x(s1);
                antlrcpp::Any tmp = x;
                return tmp;
            }
            else
            {
                double x = atof(c);
                return x;
            }
        }
        else if (ctx->TRUE() != nullptr)
        {
            bool flag = true;
            return flag;
        }
        else if (ctx->FALSE() != nullptr)
        {
            bool flag = false;
            return flag;
        }
        else if (ctx->NONE() != nullptr)
        {
            std::string str = "None";
            return str;
        }
        else if (ctx->test() != nullptr)
        {
            return visitTest(ctx->test());
        }
    }

    antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override
    {
        if (ctx->test().size() == 1)
        {
            return visitTest(ctx->test()[0]);
        }
    }

    antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override
    {
        return visitArgument(ctx->argument()[0]);
    }

    antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override
    {
        return visitTest(ctx->test());
    }

    //todo:override all methods of Python3BaseVisitor
};

#endif //PYTHON_INTERPRETER_EVALVISITOR_H