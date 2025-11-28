

/**
 * @file parser.cpp
 * @brief Parsing implementation for Scheme syntax tree to expression tree conversion
 *
 * This file implements the parsing logic that converts syntax trees into
 * expression trees that can be evaluated.
 * primitive operations, and function applications.
 */

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "value.hpp"
#include "expr.hpp"
#include <map>
#include <string>
#include <iostream>

#define mp make_pair
using std::string;
using std::vector;
using std::pair;

extern std::map<std::string, ExprType> primitives;
extern std::map<std::string, ExprType> reserved_words;

/**
 * @brief Default parse method (should be overridden by subclasses)
 */
Expr Syntax::parse(Assoc &env) {
    throw RuntimeError("Unimplemented parse method");
}

Expr Number::parse(Assoc &env) {
    return Expr(new Fixnum(n));
}

Expr RationalSyntax::parse(Assoc &env) {
    //TODO: complete the rational parser
    return Expr(new RationalNum(numerator,denominator));
}

Expr SymbolSyntax::parse(Assoc &env) {
    return Expr(new Var(s));
}

Expr StringSyntax::parse(Assoc &env) {
    return Expr(new StringExpr(s));//创建符号语法结果的变量表达式，包装成智能指针
}

Expr TrueSyntax::parse(Assoc &env) {
    return Expr(new True());
}

Expr FalseSyntax::parse(Assoc &env) {
    return Expr(new False());
}

Expr List::parse(Assoc &env) {
    //检查是否为空列表
    if (stxs.empty()) {
        return Expr(new Quote(Syntax(new List())));
    }
    //TODO: check if the first element is a symbol
    //If not, use Apply function to package to a closure;
    //If so, find whether it's a variable or a keyword;
    //提取变量名
    SymbolSyntax *id = dynamic_cast<SymbolSyntax*>(stxs[0].get());//获取第一个单词作为命令名
    if (id == nullptr) {//如果没有命令词，从头开始参数解析
        //TODO: TO COMPLETE THE LOGIC
        vector<Expr> parameters;
        for (size_t i = 1; i < stxs.size(); i++) {
            parameters.push_back(stxs[i]->parse(env));
        }
        return Expr(new Apply(stxs[0]->parse(env), parameters));
    }

    //如果有命令词
    string op = id->s;

    //根据命令类型创建对应的命令对象
    if (find(op, env).get() != nullptr) {
    	//TODO: TO COMPLETE THE PARAMETER PARSER LOGIC
    	vector<Expr> parameters;
    	//跳过第一个（命令词）开始参数解析
    	for (size_t i = 1; i < stxs.size(); i++) {
    		parameters.push_back(stxs[i]->parse(env));
    	}

        return Expr(new Apply(stxs[0].get()->parse(env), parameters));
    }
    //primitives基本操作（内置函数）
    if (primitives.count(op) != 0) {
        // TODO: TO COMPLETE THE PARAMETER PARSER LOGIC
    	vector<Expr> parameters;
    	//跳过第一个（命令词）开始参数解析
    	for (size_t i = 1; i < stxs.size(); i++) {
    		parameters.push_back(stxs[i]->parse(env));
    	}

         ExprType op_type = primitives[op];
         if (op_type == E_PLUS) {
             if (parameters.size() == 0) {
                 return Expr(new PlusVar(parameters));
             }
         	 else if (parameters.size() == 1) {
                 return parameters[0];
             }
         	 else if (parameters.size() == 2) {
                 return Expr(new Plus(parameters[0], parameters[1]));
             }
         	 else {
                 return Expr(new PlusVar(parameters));
             }

        } else if (op_type == E_MINUS) {
            //TODO: TO COMPLETE THE LOGIC
        	if (parameters.size() == 0) {
        		throw RuntimeError("Undefined variable");
        	}
        	else if (parameters.size() == 1) {
        		return Expr(new MinusVar(parameters));//-x
        	}
            else if (parameters.size() == 2) {
                return Expr(new Minus(parameters[0], parameters[1]));
            }
        	else {
                return Expr(new MinusVar(parameters));
            }
        } else if (op_type == E_MUL) {
            //TODO: TO COMPLETE THE LOGIC
        	if (parameters.size() == 0) {
        		return Expr(new MultVar(parameters));
        	}
        	else if (parameters.size() == 1) {
        		return parameters[0];
        	}
            else if (parameters.size() == 2) {
                return Expr(new Mult(parameters[0], parameters[1]));
            }
        	else {
                return Expr(new MultVar(parameters));
            }
        }  else if (op_type == E_DIV) {
            //TODO: TO COMPLETE THE LOGIC
        	if (parameters.size() == 0) {
        		throw RuntimeError("Undefined variable");
        	}
            else if (parameters.size() == 2) {
                return Expr(new Div(parameters[0], parameters[1]));
            }
        	else if (parameters.size() == 1) {
        		return Expr(new DivVar(parameters));
        	}
        	else {
                return Expr(new DivVar(parameters));
            }
        } else if (op_type == E_MODULO) {
            if (parameters.size() != 2) {
                throw RuntimeError("Wrong number of arguments for modulo");
            }
            return Expr(new Modulo(parameters[0], parameters[1]));
        } else if (op_type == E_LIST) {
            return Expr(new ListFunc(parameters));
        }else if (op_type == E_LISTQ) {
        	if (parameters.size() != 1) {
        		throw RuntimeError("Wrong number of arguments for list?");
        	}
        	return Expr(new IsList(parameters[0]));
        }
    	else if (op_type == E_LT) {//<比较
            //TODO: TO COMPLETE THE LOGIC
        	if (parameters.size() < 2) {
        		throw RuntimeError("Wrong number of arguments for `<");
        	}
        	else if (parameters.size() == 2) {
                return Expr(new Less(parameters[0], parameters[1]));
            }
        	else {
        		return Expr(new LessVar(parameters));
        	}

        } else if (op_type == E_LE) {
            //TODO: TO COMPLETE THE LOGIC
        	if (parameters.size() < 2) {
        		throw RuntimeError("Wrong number of arguments for `<=");
        	}
            else if (parameters.size() == 2) {
                return Expr(new LessEq(parameters[0], parameters[1]));
            }
        	else {
        		return Expr(new LessEqVar(parameters));
        	}

        } else if (op_type == E_EQ) {
            //TODO: TO COMPLETE THE LOGIC
        	if (parameters.size() < 2) {
        		throw RuntimeError("Wrong number of arguments for `=");
        	}
            else if (parameters.size() == 2) {
                return Expr(new Equal(parameters[0], parameters[1]));
            }
        	else {
        		return Expr(new EqualVar(parameters));
        	}

        } else if (op_type == E_GE) {
            //TODO: TO COMPLETE THE LOGIC
        	if (parameters.size() < 2) {
        		throw RuntimeError("Wrong number of arguments for `>=");
        	}
            else if (parameters.size() == 2) {
                return Expr(new GreaterEq(parameters[0], parameters[1]));
            }
        	else {
        		return Expr(new GreaterEqVar(parameters));
        	}

        } else if (op_type == E_GT) {
            //TODO: TO COMPLETE THE LOGIC
        	if (parameters.size() < 2) {
        		throw RuntimeError("Wrong number of arguments for `>");
        	}
            else if (parameters.size() == 2) {
                return Expr(new Greater(parameters[0], parameters[1]));
            }
        	else {
        		return Expr(new GreaterVar(parameters));
        	}

        } else if (op_type == E_AND) {
            return Expr(new AndVar(parameters));
        } else if (op_type == E_OR) {
            return Expr(new OrVar(parameters));
        }//TODO: TO COMPLETE THE LOGIC
        else if (op_type == E_CONS) {
            if (parameters.size() == 2) {
                return Expr(new Cons(parameters[0], parameters[1]));
            }
            else {
                throw RuntimeError("Wrong number of arguments for Cons");
            }
        }
        else if (op_type == E_CAR) {
            if (parameters.size() == 1) {
                return Expr(new Car(parameters[0]));
            }
            else {
                throw RuntimeError("Wrong number of arguments for Car");
            }
        }
        else if (op_type == E_CDR) {
            if (parameters.size() == 1) {
                return Expr(new Cdr(parameters[0]));
            }
            else {
                throw RuntimeError("Wrong number of arguments for Cdr");
            }
        }
        else if (op_type == E_NOT) {
            if (parameters.size() == 1) {
                return Expr(new Not(parameters[0]));
            }
            else {
                throw RuntimeError("Wrong number of arguments for Not");
            }
        }else if (op_type == E_EXIT) {
        	if (parameters.size() != 0) {
        		throw RuntimeError("Wrong number of arguments for exit");
        	}
        	return Expr(new Exit());
        }

    }

    //语法关键字
    if (reserved_words.count(op) != 0) {//保留字字典：if,define,lambda....
    	switch (reserved_words[op]) {
    		//TODO: TO COMPLETE THE reserve_words PARSER LOGIC

    		case E_QUOTE:
    			if (stxs.size()==2) {
    				return Expr(new Quote(stxs[1]));
    			}
    			else {
    				throw RuntimeError("Wrong number of arguments for Quote");
    			}
    			break;

    			//???????
    		case E_BEGIN:{
    			vector<Expr> parameters;
    			//跳过第一个（命令词）开始参数解析
    			for (size_t i = 1; i < stxs.size(); i++) {
    				parameters.push_back(stxs[i]->parse(env));
    			}
    			return Expr(new Begin(parameters));
    	}
    		case E_IF: {
    		vector<Expr> parameters;
    		//跳过第一个（命令词）开始参数解析
    		for (size_t i = 1; i < stxs.size(); i++) {
    			parameters.push_back(stxs[i]->parse(env));
    		}
    		if (parameters.size() == 3) {
    			return Expr(new If(parameters[0], parameters[1], parameters[2]));
    		}
    		else {
    			throw RuntimeError("Wrong number of arguments for If");
    		}
    	}


    			//??????????????
    		case E_LAMBDA:
    			if (stxs.size() >= 3) {
    				vector<string> paramName;
    				Assoc New_env = env;//创建新环境

    				if (auto paramList = dynamic_cast<List*>(stxs[1].get())) {//参数列表容器
    					for (auto &paramSyntax : paramList->stxs ) {//遍历参数列表中的每个元素（包括类型和实际内容）
    						if (auto paramSym = dynamic_cast<SymbolSyntax*>(paramSyntax.get())) {//如果实际内容是字符串类型
    							paramName.push_back(paramSym->s);//获取纯字符串类型的参数
    							New_env = extend(paramSym->s, NullV(), New_env);
    						}else {
    							throw RuntimeError("Wrong syntax");
    						}
    					}
    				}else {
    					throw RuntimeError("Wrong syntax");
    				}

    				vector<Expr> body_exprs;
    				for (size_t i = 2; i < stxs.size(); i++) {
    					body_exprs.push_back(stxs[i]->parse(New_env));//改正：使用新环境解析
    				}
    				if (body_exprs.empty()) {
    					throw RuntimeError("Wrong syntax");
    				}


    				Expr body = (body_exprs.size()==1) ? body_exprs[0] : Expr(new Begin(body_exprs));

    				return Expr(new Lambda(paramName,body));

    			}
    			else {
    				throw RuntimeError("Wrong syntax");
    			}

    			case E_DEFINE: {
    			vector<Expr> parameters;
    			//跳过第一个（命令词）开始参数解析
    			for (size_t i = 2; i < stxs.size(); i++) {
    				parameters.push_back(stxs[i]->parse(env));
    			}
    			if (stxs.size() >= 3) {
    				//变量定义
    				if (auto var_syntax = dynamic_cast<SymbolSyntax*>(stxs[1].get())) {
    					string var_name = var_syntax->s;

    					//修正：添加关键字检查
    					if (reserved_words.count(var_name) != 0 || primitives.count(var_name) != 0) {
    						throw RuntimeError("Cannot redefine reserved word: " + var_name);
    					}

    					if (parameters.empty()) {
    						throw RuntimeError("Wrong value");
    					}
    					Expr value_expr = (parameters.size()==1) ? parameters[0] : Expr(new Begin(parameters));

    					return Expr(new Define(var_name, value_expr));
    				}
    				//函数定义
    				else if (auto func_list = dynamic_cast<List*>(stxs[1].get())) {
    					if (!func_list->stxs.empty()) {
    						if (auto func_name_syntax = dynamic_cast<SymbolSyntax*>(func_list->stxs[0].get())) {
    							string func_name = func_name_syntax->s;

    							vector<string> func_params;
    							for (size_t i = 1;i < func_list->stxs.size(); i++) {
    								if (auto param_syntax = dynamic_cast<SymbolSyntax*>(func_list->stxs[i].get())) {
    									func_params.push_back(param_syntax->s);
    								}
    								else {
    									throw RuntimeError("Wrong");
    								}
    							}
    							vector<Expr> body_exprs;
    							for (size_t i = 2; i < stxs.size(); i++) {
    								body_exprs.push_back(stxs[i]->parse(env));
    							}
    							if (body_exprs.empty()) {
    								throw RuntimeError("Wrong");
    							}
    							Expr body = (body_exprs.size()==1) ? body_exprs[0] : Expr(new Begin(body_exprs));
    							Expr lambda_expr = Expr(new Lambda(func_params,body));

    							return Expr(new Define(func_name,lambda_expr));
    						}

    					}
    				}
    			}
    		}



    	    case E_COND: {
    	        if (stxs.size() < 2) {
    	            throw RuntimeError("Wrong syntax");
    	        }

    	        std::vector<std::vector<Expr>> clauses;
    	        //遍历所有主句
    	        for (size_t i = 1; i < stxs.size(); i++) {
    	            auto clause_syntax = stxs[i];

    	            if (auto clause_list = dynamic_cast<List*>(clause_syntax.get())) {
    	                if (clause_list->stxs.empty()) {
    	                    throw RuntimeError("Wrong syntax");
    	                }
    	                std::vector<Expr> clause_exprs;
    	                for (auto &expr_syntax : clause_list->stxs) {
    	                    clause_exprs.push_back(expr_syntax->parse(env));
    	                }
    	                clauses.push_back(clause_exprs);
    	            }
    	        }
    	        return Expr(new Cond(clauses));
    	    }

    	    case E_SET: {
    	        if (stxs.size()==3) {
    	            auto var_syntax = dynamic_cast<SymbolSyntax*>(stxs[1].get());
    	            if (var_syntax) {
    	                string var_name = var_syntax->s;
    	                Expr value_expr = stxs[2]->parse(env);
    	                return Expr(new Set(var_name,value_expr));
    	            }
    	        }
    	    }
    			break;

    	    case E_LET: {
    	        if (stxs.size() < 3) {
	    	        throw RuntimeError("Wrong syntax");
    	        }

    	    	//所有绑定对<变量名称，值表达式>
    	    	std::vector<std::pair<string,Expr>> binds;
    	    	//获取绑定列表部分
    	    	auto bind_list = dynamic_cast<List*>(stxs[1].get());
    	    	if (!bind_list) {
    	    		throw RuntimeError("Wrong syntax");
    	    	}
    	    	//遍历每一个绑定对
    	    	for (auto &bind_syntax : bind_list->stxs) {
    	    		auto bind = dynamic_cast<List*>(bind_syntax.get());
    	    		if (!bind) {
    	    			throw RuntimeError("Wrong syntax");
    	    		}
    	    		if (bind->stxs.size() != 2) {
    	    			throw RuntimeError("Wrong syntax");
    	    		}
    	    		//解析变量名（绑定对第一个元素）
    	    		auto var_sym = dynamic_cast<SymbolSyntax*>(bind->stxs[0].get());
    	    		if (!var_sym) {
    	    			throw RuntimeError("Wrong syntax");
    	    		}
    	    		std::string var_name = var_sym->s;
    	    		//解析值表达式
    	    		Expr value_expr = bind->stxs[1]->parse(env);
    	    		//把这个绑定对加入到总的绑定对里
    	    		binds.push_back(std::make_pair(var_name,value_expr));
    	    	}


    	    	//创建新环境??????
    	    	//Let表达式创建的局部变量应该在let体内部可见
    	    	Assoc local_env = env;
    	    	for (auto &bind : binds) {
    	    		local_env = extend(bind.first,NullV(),local_env);
    	    	}

    	    	std::vector<Expr> body_exprs;
    	    	for (size_t i = 2; i < stxs.size(); i++) {
    	    		//递归处理body 中可能包含任意复杂的表达式
    	    		// struct Syntax {
    	    		// 	std::shared_ptr<SyntaxBase> ptr;
    	    		// 	 重载了 -> 操作符
    	    		// 	SyntaxBase* operator->() const {
    	    		// 		return ptr.get();
    	    		// 	}
    	    		// 	 获取原始指针
    	    		// 	SyntaxBase* get() const {
    	    		// 		return ptr.get();
    	    		// 	}
    	    		// };
    	    		//通过共享指针获取原始指针，然后调用 parse 方法
    	    		body_exprs.push_back(stxs[i]->parse(local_env));
    	    	}

    	    	// if (body_exprs.size() == 1) {
    	    	// 	Expr body = body_exprs[0];
    	    	// }else {
    	    	// 	Expr body = Expr(new Begin(body_exprs));
    	    	// }
    	    	Expr body = (body_exprs.size()==1) ? body_exprs[0] :  Expr(new Begin(body_exprs));
    	    	return Expr(new Let(binds,body));
    	    }

    		case E_LETREC: {
    	    	if (stxs.size() < 3) {
    	    		throw RuntimeError("Wrong syntax");
    	    	}
    	    	std::vector<std::pair<string,Expr>> all_binds;
    	    	auto bind_list = dynamic_cast<List*>(stxs[1].get());
    	    	if (!bind_list) {
    	    		throw RuntimeError("Wrong syntax");
    	    	}
    	    	//修改：建立新环境
    	    	Assoc temp_env = env;
    	    	for (auto &bind_syntax : bind_list->stxs) {
    	    		auto bind = dynamic_cast<List*>(bind_syntax.get());
    	    		if (!bind) {
    	    			throw RuntimeError("Wrong syntax");
    	    		}
    	    		if (bind->stxs.size() != 2) {
    	    			throw RuntimeError("Wrong syntax");
    	    		}
    	    		auto var_sym = dynamic_cast<SymbolSyntax*>(bind->stxs[0].get());
    	    		if (!var_sym) {
    	    			throw RuntimeError("Wrong syntax");
    	    		}
    	    		std::string var_name = var_sym->s;
    	    		temp_env = extend(var_name,NullV(),temp_env);
    	    	}
    	    	//修改：在临时环境中解析所有绑定表达式
    	    	for (auto &bind_syntax : bind_list->stxs) {
    	    		auto bind = dynamic_cast<List*>(bind_syntax.get());
    	    		auto var_sym = dynamic_cast<SymbolSyntax*>(bind->stxs[0].get());
    	    		std::string var_name = var_sym->s;
    	    		all_binds.push_back(std::make_pair(var_name,bind->stxs[1]->parse(temp_env)));
    	    	}

    	    	std::vector<Expr> body_exprs;
    	    	for (size_t i = 2; i < stxs.size(); i++) {
    	    		body_exprs.push_back(stxs[i]->parse(temp_env));//使用temp_env
    	    	}
    	    	Expr body = (body_exprs.size() == 1) ? body_exprs[0] : Expr(new Begin(body_exprs));
    	    	return Expr(new Letrec(all_binds,body));

    	    }



    	    default:
            	throw RuntimeError("Unknown reserved word: " + op);
    	}
    }

	vector<Expr> parameters;
	//跳过第一个（命令词）开始参数解析
	for (size_t i = 1; i < stxs.size(); i++) {
		parameters.push_back(stxs[i]->parse(env));
	}
    // 默认情况：当作函数调用
    Expr func = Expr(new Var(op));
    return Expr(new Apply(func, parameters));
    //default: use Apply to be an expression
    //TODO: TO COMPLETE THE PARSER LOGIC
}

