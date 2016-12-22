
#pragma once

#include "util/logging.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <fstream>

template<typename T>
class Value {
public:
	T t;
	std::string name;
	std::string parent;
	std::vector<std::string> children;
	size_t priority;

	Value()
	{
		priority = 0;
	}
	Value(const T& value) : t(value), priority(0)
	{}
	Value(const Value& value)
	{
		t = value.t;
		name = value.name;
		parent = value.parent;
		children = value.children;
		priority = value.priority;
	}

	operator T&()
	{
		return t;
	}
	operator const T&() const
	{
		return t;
	}

	Value& operator = (const T& other)
	{
		t = other;
		return *this;
	}

	Value& operator += (const Value& v)
	{
		t += v.t;
		return *this;
	}

	Value& operator -= (const Value& v)
	{
		t -= v.t;
		return *this;
	}

	Value& operator *= (const Value& v)
	{
		t *= v.t;
		return *this;
	}

	Value& operator /= (const Value& v)
	{
		t /= v.t;
		return *this;
	}
};

namespace
{
	bool isSpace(char c)
	{
		return c == ' ' || c == '\t' || c == '\n';
	}

	bool isCastOperator(const std::string& cmd)
	{
		return cmd == "icast" || cmd == "fcast";
	}

	bool isArithmeticOperator(const std::string& cmd)
	{
		return cmd == "+" || cmd == "-" || cmd == "*" || cmd == "/";
	}

	bool isCompareOperator(const std::string& cmd)
	{
		return cmd == "<" || cmd == ">" || cmd == "==";
	}

	bool isAssignOperator(const std::string& cmd)
	{
		return cmd == "=";
	}

	bool isLogicalOperator(const std::string& cmd)
	{
		return cmd == "or" || cmd == "and" || cmd == "xor" || cmd == "not";
	}

	bool isOperator(const std::string& cmd)
	{
		return isArithmeticOperator(cmd) || isCompareOperator(cmd) || isAssignOperator(cmd) || isCastOperator(cmd) || isLogicalOperator(cmd);
	}

	bool isBracket(const std::string& cmd)
	{
		return cmd == "(" || cmd == ")" || cmd == "{" || cmd == "}" || cmd == "[" || cmd == "]";
	}
}

class Scripter {
	std::string peekWord(std::stringstream& sstream)
	{
		std::string result;
		if (!sstream.eof())
		{
			auto pos = sstream.tellg();
			sstream >> result;
			sstream.seekg(pos);
		}
		return result;
	}

	struct Struct {
		std::unordered_map<std::string, std::string> nameToType;
	};

public:
	class Interface {
	public:
		Interface(Scripter& scripter) : host(scripter)
		{}

		bool exists(const std::string varName)
		{
			return host.exists(varName);
		}
		bool getIntVal(const std::string& valueName, int& v)
		{
			auto it = host.ints.find(valueName);
			if (it == host.ints.end())
				return false;
			v = it->second;
			return true;
		}

		bool getFloatVal(const std::string& valueName, float& v)
		{
			auto it = host.floats.find(valueName);
			if (it == host.floats.end())
				return false;
			v = it->second;
			return true;
		}

		bool getStringVal(const std::string& valueName, std::string& v)
		{
			auto it = host.strings.find(valueName);
			if (it == host.strings.end())
				return false;
			v = host.prepareOutputString(it->second);
			return true;
		}

		void prepareForOutput(std::string& s)
		{
			s = host.prepareOutputString(s);
		}

		std::string readCodeBlock(std::stringstream& ss)
		{
			return host.readCodeBlock(ss);
		}

		void pushCommand(const std::string& cmd)
		{
			host.commandStack.push_back(cmd);
		}

		const std::string& operatorTop() const
		{
			return host.commandStack.back();
		}
		void operatorConsume()
		{
			host.commandStack.pop_back();
		}

		const std::string& variableNameTop() const
		{
			return host.operatorVariables.back().first;
		}
		void variableNameConsume()
		{
			host.operatorVariables.pop_back();
		}

		int tempIntTop() const
		{
			return host.tempInt.back();
		}
		void tempIntConsume()
		{
			host.tempInt.pop_back();
		}

		float tempFloatTop() const
		{
			return host.tempFloat.back();
		}
		void tempFloatConsume()
		{
			host.tempFloat.pop_back();
		}

		const std::string& tempStringTop() const
		{
			return host.tempString.back();
		}
		void tempStringConsume()
		{
			host.tempString.pop_back();
		}

		bool tempBoolTop() const
		{
			return host.tempBool.back();
		}
		void tempBoolConsume()
		{
			host.tempBool.pop_back();
		}

		void fillParameter(float& f)
		{
			host.fillParameterFloat(f);
		}
		void fillParameter(int& i)
		{
			host.fillParameterInt(i);
		}
		void fillParameter(std::string& s)
		{
			host.fillParameterString(s);
		}

		Scripter& host;
	};

	friend class Interface;

	class Extension {
	public:
		Extension(std::function<bool(Scripter::Interface&, const std::string&, std::stringstream&)> commandHandler, std::function<bool(Scripter::Interface&, std::stringstream&)> resultHandler)
		{
			handleCommand = commandHandler;
			handleResult = resultHandler;
		}

		std::function<bool(Scripter::Interface&, const std::string&, std::stringstream& ss)> handleCommand;
		std::function<bool(Scripter::Interface&, std::stringstream& ss)> handleResult;
	};
private:

	std::vector<Extension> extensions;
public:
	Scripter(const std::string& path = "")
	{
		scriptPath = path;
		commandStack.push_back("");
		currentPriority = 0;
		parenthesisBalance = 0;
		scopeStack.push_back(std::unordered_map<std::string, std::string>());
		functionParameterMapping.push_back(std::unordered_map<std::string, std::string>());
	}

	void addExtension(const Extension& extension)
	{
		extensions.push_back(extension);
	}

	void processFile(const std::string& fileName)
	{
		std::ifstream f(fileName);
		std::string cmdMsg;
		while (f.good() && !f.eof())
		{
			std::string line;
			getline(f, line);
			cmdMsg += line + "\n";
		}

		scriptFileStack.push_back(fileName);
		process(cmdMsg);
		scriptFileStack.pop_back();
	}

	std::string scriptFileTrace() const
	{
		std::string trace;
		for (const std::string& scriptFile : scriptFileStack)
		{
			trace += scriptFile + "\n";
		}
		return trace;
	}

	void process(std::string cmdMsg)
	{

		bool inStringLiteral = false;
		for (int i = 0; i < static_cast<int>(cmdMsg.size()); ++i)
		{
			std::string str;
			str += cmdMsg[i];

			if (str == "\"")
				inStringLiteral = !inStringLiteral;

			if (!inStringLiteral && (isOperator(str) || isBracket(str)))
			{
				if (i > 0 && !isSpace(cmdMsg[i - 1]))
				{
					cmdMsg.insert(i, " ");
					++i;
				}
				if (i < static_cast<int>(cmdMsg.length()) && !isSpace(cmdMsg[i + 1]))
				{
					cmdMsg.insert(i + 1, " ");
					++i;
				}
			}
		}

		std::stringstream ss(cmdMsg);
		std::string cmd;

		auto consumeGlobal = [this]() -> bool {
			if (!commandStack.empty() && commandStack.back() == "global")
			{
				commandStack.pop_back();
				return true;
			}
			return false;
		};

		while (ss >> cmd)
		{
			if (handleCommand(cmd, ss))
				continue;
			else if (commandStack.back() == "int")
			{
				commandStack.pop_back();
				bool global = consumeGlobal();
				if (ints.find(cmd) == ints.end())
				{
					if (global) scopeStack.front()[cmd] = "float";
					else scopeStack.back()[cmd] = "float";
					ints[cmd] = 0;
				}
			}
			else if (commandStack.back() == "float")
			{
				commandStack.pop_back();
				bool global = consumeGlobal();
				if (floats.find(cmd) == floats.end())
				{
					if (global) scopeStack.front()[cmd] = "float";
					else scopeStack.back()[cmd] = "float";
					floats[cmd] = 0;
				}
			}
			else if (commandStack.back() == "string")
			{
				commandStack.pop_back();
				bool global = consumeGlobal();
				if (strings.find(cmd) == strings.end())
				{
					if (global) scopeStack.front()[cmd] = "float";
					else scopeStack.back()[cmd] = "float";
					strings[cmd] = "";
				}
			}
			else if (structs.find(commandStack.back()) != structs.end())
			{
				instantiateStruct(commandStack.back(), cmd);
				commandStack.pop_back();
			}
			else
			{

				bool handled = false;

				// check all extensions if they want to do something with this.
				for (auto& extension : extensions)
				{
					if (extension.handleCommand(Scripter::Interface(*this), cmd, ss))
					{
						handled = true;
						break;
					}
				}

				auto func_it = functions.find(cmd);
				if (func_it != functions.end())
				{
					operatorVariables.emplace_back(std::make_pair(cmd, ++currentPriority));
					handled = true;
				}

				if (!handled)
				{
					// if nothing else, then it's a parameter name.
					ASSERT(exists(cmd) || (commandStack.back() == "function" || commandStack.back() == "call" || functionParameterMapping.back().find(cmd) != functionParameterMapping.back().end()), "What to do with '%s'? it is not a known variable or command.", cmd.c_str());
					if (functionParameterMapping.back().find(cmd) != functionParameterMapping.back().end())
					{
						size_t currentIndex = functionParameterMapping.size() - 2;
						std::string mappedCmd = functionParameterMapping.back()[cmd];
						while (functionParameterMapping[currentIndex].find(mappedCmd) != functionParameterMapping[currentIndex].end())
						{
							mappedCmd = functionParameterMapping[currentIndex][mappedCmd];
							--currentIndex;
						}
						if (exists(mappedCmd))
							operatorVariables.emplace_back(std::make_pair(functionParameterMapping.back()[cmd], ++currentPriority));
						else
							process(mappedCmd);
					}
					else
						operatorVariables.emplace_back(std::make_pair(cmd, ++currentPriority));
				}
			}
		}
	}

	int getInt(const std::string& name) const
	{
		auto it = ints.find(name);
		if (it == ints.end())
			return 0;
		return it->second;
	}

	float getFloat(const std::string& name) const
	{
		auto it = floats.find(name);
		if (it == floats.end())
			return 0;
		return it->second;
	}

	std::string getString(const std::string& name) const
	{
		auto it = strings.find(name);
		if (it == strings.end())
			return "";
		return it->second;
	}

private:
	template<typename T>
	bool has(const std::unordered_map<std::string, T>& m, const std::string& name) const
	{
		return m.find(name) != m.end();
	}

	std::string readCodeBlock(std::stringstream& ss)
	{
		std::string result;
		std::string tmp;

		ss >> tmp;
		ASSERT(tmp == "{", "Expected a code block '{ ... }'\nScript trace: %s", this->scriptFileTrace().c_str());

		result = tmp + " ";
		int balance = 1;

		while (ss >> tmp)
		{
			result += tmp + " ";
			if (tmp == "{") ++balance;
			if (tmp == "}") --balance;
			if (balance == 0) break;
		}
		return result;
	}

	std::string readOperationBlock(std::stringstream& ss)
	{
		std::string result;
		std::string tmp;

		ss >> tmp;
		ASSERT(tmp == "(", "Expected an operation block '( ... )'\nScript trace: %s", this->scriptFileTrace().c_str());

		result = tmp + " ";
		int balance = 1;

		while (ss >> tmp)
		{
			result += tmp + " ";
			if (tmp == "(") ++balance;
			if (tmp == ")") --balance;
			if (balance == 0) break;
		}
		return result;
	}

	bool exists(const std::string& name)
	{
		bool found = false;
		found |= ints.find(name) != ints.end();
		found |= floats.find(name) != floats.end();
		found |= strings.find(name) != strings.end();
		found |= structVarNameToTypeName.find(name) != structVarNameToTypeName.end();
		return found;
	}

	void fillParameterInt(int& p)
	{
		size_t operatorVarPrio = 0;
		size_t tempPrio = 0;

		if (!operatorVariables.empty())
			operatorVarPrio = operatorVariables.back().second;
		if (!tempInt.empty())
			tempPrio = tempInt.back().priority;

		if (tempPrio > operatorVarPrio)
		{
			p = tempInt.back();
			tempInt.pop_back();
		}
		else
		{
			std::string name = operatorVariables.back().first;
			operatorVariables.pop_back();
			p = ints[name];
		}
	}

	void fillParameterFloat(float& p)
	{
		size_t operatorVarPrio = 0;
		size_t tempPrio = 0;

		if (!operatorVariables.empty())
			operatorVarPrio = operatorVariables.back().second;
		if (!tempFloat.empty())
			tempPrio = tempFloat.back().priority;

		if (tempPrio > operatorVarPrio)
		{
			p = tempFloat.back();
			tempFloat.pop_back();
		}
		else
		{
			std::string name = operatorVariables.back().first;
			operatorVariables.pop_back();
			p = floats[name];
		}
	}

	void fillParameterString(std::string& p)
	{
		size_t operatorVarPrio = 0;
		size_t tempPrio = 0;

		if (!operatorVariables.empty())
			operatorVarPrio = operatorVariables.back().second;
		if (!tempString.empty())
			tempPrio = tempString.back().priority;

		if (tempPrio > operatorVarPrio)
		{
			p = tempString.back();
			tempString.pop_back();
		}
		else
		{
			std::string name = operatorVariables.back().first;
			operatorVariables.pop_back();
			p = strings[name];
		}
	}

	bool handleResultAssign()
	{
		if (commandStack.back() == "=")
		{
			commandStack.pop_back();
			std::string variableName = operatorVariables.back().first;
			operatorVariables.pop_back();
			if (has(ints, variableName))
			{
				ints[variableName] = tempInt.back();
				tempInt.pop_back();
			}
			else if (has(floats, variableName))
			{
				floats[variableName] = tempFloat.back();
				tempFloat.pop_back();
			}
			else if (has(strings, variableName))
			{
				strings[variableName] = tempString.back();
				tempString.pop_back();
			}
			else if (has(structVarNameToTypeName, variableName))
			{
				std::string target = operatorVariables.back().first;
				operatorVariables.pop_back();
				assignStruct(target, variableName);
			}
			return true;
		}
		return false;
	}

	std::string getCurrentOperandType() const
	{
		size_t operandImportance = 0;
		size_t temporaryImportance = 0;
		std::string temporaryType;
		std::string operandType;

		if (!operatorVariables.empty())
		{
			auto& varName = operatorVariables.back().first;
			operandImportance = operatorVariables.back().second;

			if (has(ints, varName))
				operandType = "int";
			if (has(floats, varName))
				operandType = "float";
			if (has(strings, varName))
				operandType = "string";
		}

		if (!tempInt.empty() && tempInt.back().priority > temporaryImportance)
		{
			temporaryImportance = tempInt.back().priority;
			temporaryType = "int";
		}
		if (!tempFloat.empty() && tempFloat.back().priority > temporaryImportance)
		{
			temporaryImportance = tempFloat.back().priority;
			temporaryType = "float";
		}
		if (!tempString.empty() && tempString.back().priority > temporaryImportance)
		{
			temporaryImportance = tempString.back().priority;
			temporaryType = "string";
		}

		if (temporaryImportance > operandImportance)
			return temporaryType;
		return operandType;

	}

	bool handleResult(std::stringstream& ss)
	{
		if (commandStack.back() == "while")
		{
			// do nothing, but consume the result handle request
			return true;
		}
		else if (isLogicalOperator(commandStack.back()))
		{
			std::string op = commandStack.back();
			commandStack.pop_back();

			if (op == "or")
			{
				bool a = tempBool.back(); tempBool.pop_back();
				bool b = tempBool.back(); tempBool.pop_back();
				tempBool.push_back(a || b);
			}
			else if (op == "and")
			{
				bool a = tempBool.back(); tempBool.pop_back();
				bool b = tempBool.back(); tempBool.pop_back();
				tempBool.push_back(a && b);
			}
			else if (op == "xor")
			{
				bool a = tempBool.back(); tempBool.pop_back();
				bool b = tempBool.back(); tempBool.pop_back();
				tempBool.push_back(a ^ b);
			}
			else if (op == "not")
			{
				bool a = tempBool.back(); tempBool.pop_back();
				tempBool.push_back(!a);
			}
			return true;
		}
		else if (commandStack.back() == "+")
		{
			commandStack.pop_back();
			std::string operandType = getCurrentOperandType();
			if (operandType == "int")
			{
				int a = 0;
				int b = 0;
				fillParameterInt(a);
				fillParameterInt(b);
				tempInt.push_back(a + b);
				tempInt.back().priority = ++currentPriority;
			}
			else if (operandType == "float")
			{
				float a = 0;
				float b = 0;
				fillParameterFloat(a);
				fillParameterFloat(b);
				tempFloat.push_back(a + b);
				tempFloat.back().priority = ++currentPriority;
			}
			else if (operandType == "string")
			{
				std::string a;
				std::string b;
				fillParameterString(a);
				fillParameterString(b);
				tempString.push_back(a + b);
				tempString.back().priority = ++currentPriority;
			}
			return true;
		}
		else if (commandStack.back() == "-")
		{
			commandStack.pop_back();
			std::string operandType = getCurrentOperandType();
			if (operandType == "int")
			{
				int a;
				int b;
				fillParameterInt(b);
				fillParameterInt(a);
				tempInt.push_back(a - b);
				tempInt.back().priority = ++currentPriority;
			}
			else if (operandType == "float")
			{
				float a;
				float b;
				fillParameterFloat(b);
				fillParameterFloat(a);
				tempFloat.push_back(a - b);
				tempFloat.back().priority = ++currentPriority;
			}
			else if (operandType == "string")
			{
				std::string a;
				std::string b;
				fillParameterString(b);
				fillParameterString(a);
				auto pos = a.find(b);
				if (pos != std::string::npos)
				{
					a.replace(pos, b.length(), "");
				}
				tempString.push_back(a);
				tempString.back().priority = ++currentPriority;
			}
			return true;
		}
		else if (commandStack.back() == "*")
		{
			commandStack.pop_back();
			std::string operandType = getCurrentOperandType();
			if (operandType == "int")
			{
				int a;
				int b;
				fillParameterInt(a);
				fillParameterInt(b);
				tempInt.push_back(a * b);
				tempInt.back().priority = ++currentPriority;
			}
			else if (operandType == "float")
			{
				float a;
				float b;
				fillParameterFloat(a);
				fillParameterFloat(b);
				tempFloat.push_back(a * b);
				tempFloat.back().priority = ++currentPriority;
			}
			else if (operandType == "string")
			{
				throw "up";
			}
			return true;
		}
		else if (commandStack.back() == "/")
		{
			commandStack.pop_back();
			std::string operandType = getCurrentOperandType();
			if (operandType == "int")
			{
				int a;
				int b;
				fillParameterInt(b);
				fillParameterInt(a);
				tempInt.push_back(a / b);
				tempInt.back().priority = ++currentPriority;
			}
			else if (operandType == "float")
			{
				float a;
				float b;
				fillParameterFloat(b);
				fillParameterFloat(a);
				tempFloat.push_back(a / b);
				tempFloat.back().priority = ++currentPriority;
			}
			else if (operandType == "string")
			{
				throw "up";
			}
			return true;
		}
		else if (commandStack.back() == ">")
		{
			commandStack.pop_back();
			std::string operandType = getCurrentOperandType();
			if (operandType == "int")
			{
				int a;
				int b;
				fillParameterInt(b);
				fillParameterInt(a);
				tempBool.push_back(a > b);
			}
			else if (operandType == "float")
			{
				float a;
				float b;
				fillParameterFloat(b);
				fillParameterFloat(a);
				tempBool.push_back(a > b);
			}
			else if (operandType == "string")
			{
				throw "up";
			}
			return true;
		}
		else if (commandStack.back() == "<")
		{
			commandStack.pop_back();
			std::string operandType = getCurrentOperandType();
			if (operandType == "int")
			{
				int a;
				int b;
				fillParameterInt(b);
				fillParameterInt(a);
				tempBool.push_back(a < b);
			}
			else if (operandType == "float")
			{
				float a;
				float b;
				fillParameterFloat(b);
				fillParameterFloat(a);
				tempBool.push_back(a < b);
			}
			else if (operandType == "string")
			{
				throw "up";
			}
			return true;
		}
		else if (commandStack.back() == "==")
		{
			commandStack.pop_back();
			std::string operandType = getCurrentOperandType();
			if (operandType == "int")
			{
				int a;
				int b;
				fillParameterInt(b);
				fillParameterInt(a);
				tempBool.push_back(a == b);
			}
			else if (operandType == "float")
			{
				float a;
				float b;
				fillParameterFloat(b);
				fillParameterFloat(a);
				tempBool.push_back(a == b);
			}
			else if (operandType == "string")
			{
				throw "up";
			}
			return true;
		}
		else if (commandStack.back() == "icast")
		{
			commandStack.pop_back();
			ASSERT(!tempFloat.empty(), "integer cast requires a temporary float. temporary float not found.");
			tempInt.push_back(static_cast<int>(tempFloat.back()));
			tempInt.back().priority = ++currentPriority;
			tempFloat.pop_back();
			return true;
		}
		else if (commandStack.back() == "fcast")
		{
			commandStack.pop_back();
			ASSERT(!tempInt.empty(), "float cast requires a temporary int. temporary int not found.");
			tempFloat.push_back(static_cast<float>(tempInt.back()));
			tempFloat.back().priority = ++currentPriority;
			tempInt.pop_back();
			return true;
		}
		else if (commandStack.back() == "run")
		{
			std::string scriptFile;
			fillParameterString(scriptFile);
			commandStack.pop_back();
			processFile(scriptPath + scriptFile);
			return true;
		}
		else if (commandStack.back() == "function")
		{
			commandStack.pop_back();
			std::string functionName = operatorVariables.back().first; operatorVariables.pop_back();

			// read parameter definitions
			std::string parameters = readOperationBlock(ss);

			// read function body
			std::string functionCode = readCodeBlock(ss);

			functions[functionName] = std::make_pair(parameters, functionCode);
			return true;
		}
		else if (commandStack.back() == "call")
		{

			// TODO: Remove the need for a call operation, just naming a known function should be enough to instantiate a function call
			// TODO: Compound parameters to work the same way as temporaries

			std::string functionName = operatorVariables.back().first; operatorVariables.pop_back();
			auto it = functions.find(functionName);
			ASSERT(it != functions.end(), "Unknown function called: '%s'", functionName.c_str());
			commandStack.pop_back();
			std::string parameterChunk = readOperationBlock(ss);

			// TODO (optimization): vector of parameter entities should be stored in functions as the first parameter, instead of the string variable from which the vector is built

			// parameter mapping
			struct ParameterEntity {
				std::string type;
				std::string name;
			};
			std::vector<ParameterEntity> parameterDefinitions;

			// construct definitions
			{
				std::istringstream ss(it->second.first);
				ss.ignore(1); // ignore the initiating (
											// auto parameterDefCopy = it->second.first.substr(1);

				std::vector<std::string> tokens;
				std::string token;
				while (std::getline(ss, token, ','))
				{
					tokens.emplace_back(std::move(token));
				}

				auto readParameterEntity = [&parameterDefinitions](const std::string& str)
				{
					std::stringstream param_ss(str);
					ParameterEntity entity;
					param_ss >> entity.type >> entity.name;
					if (!entity.type.empty() && !entity.name.empty())
						parameterDefinitions.emplace_back(std::move(entity));
				};

				for (auto&& t : tokens)
				{
					readParameterEntity(t);
				}
			}

			std::unordered_map<std::string, std::string> remap;

			{
				// break parameter chunk into separate parameters
				std::stringstream ss(parameterChunk);
				ss.ignore(1); // beginning (

				std::vector<std::string> tokens;
				std::string token;
				while (std::getline(ss, token, ','))
				{
					// TODO: Use some prettier trim method. And when you do, take the tailing ) symbol to account.
					std::stringstream token_ss(token);
					std::string tmpToken;
					token_ss >> tmpToken;
					tokens.emplace_back(std::move(tmpToken));
				}

				ASSERT(parameterDefinitions.size() >= tokens.size(), "Too many parameters given for %s%s given-> %s", functionName.c_str(), it->second.first.c_str(), parameterChunk.c_str());
				ASSERT(parameterDefinitions.size() <= tokens.size(), "Not enough parameters given for %s%s given-> %s", functionName.c_str(), it->second.first.c_str(), parameterChunk.c_str());

				for (size_t i = 0; i < tokens.size(); ++i)
				{
					auto& param = parameterDefinitions[i];
					auto& given = tokens[i];

					// TODO: Verify that type of given == param.type
					remap[param.name] = given;
				}
			}

			functionCallStack.push_back(functionName);
			functionParameterMapping.push_back(std::move(remap));
			process(it->second.second); // func body
			functionParameterMapping.pop_back();
			functionCallStack.pop_back();
			return true;
		}
		else if (commandStack.back() == "fetch")
		{
			commandStack.pop_back();
			std::string parameterName;
			fillParameterString(parameterName);
			ASSERT(exists(parameterName), "Tried to fetch '%s', but variable by that name does not exist", parameterName.c_str());
			operatorVariables.push_back(std::make_pair(parameterName, ++currentPriority));
			return true;
		}
		return false;
	}

	void handleResultAll(std::stringstream& ss)
	{
		if (!handleResult(ss))
		{
			for (auto& extension : extensions)
			{
				if (extension.handleResult(Scripter::Interface(*this), ss))
					break;
			}
		}
	}

	void instantiateStruct(const std::string& structName, const std::string& varNameOuter, bool pushToStack = true)
	{
		auto it = structs.find(structName);
		ASSERT(it != structs.end(), "Struct '%s' not found!", structName.c_str());

		if (pushToStack)
		{
			bool global = false;
			if (!commandStack.empty() && commandStack.back() == "global")
			{
				commandStack.pop_back();
				global = true;
			}

			if (global)
				scopeStack.front()[varNameOuter] = structName;
			else
				scopeStack.back()[varNameOuter] = structName;
		}

		structVarNameToTypeName[varNameOuter] = structName;

		Struct& s = it->second;
		for (auto& svar : s.nameToType)
		{
			std::string varName = varNameOuter + "." + svar.first;

			if (svar.second == "int")
			{
				if (!has(ints, varName))
					ints[varName] = 0;
			}
			else if (svar.second == "float")
			{
				if (!has(floats, varName))
					floats[varName] = 0;
			}
			else if (svar.second == "string")
			{
				if (!has(strings, varName))
					strings[varName] = "";
			}
			else
			{
				instantiateStruct(svar.second, varName, false);
			}
		}
	}

	void eraseStructVariable(const std::string& type, const std::string& name)
	{
		structVarNameToTypeName.erase(name);
		auto it = structs.find(type);
		for (auto& varTypePair : it->second.nameToType)
		{
			std::string nextName = name + "." + varTypePair.first;
			if (varTypePair.second == "int")
			{
				ints.erase(nextName);
			}
			else if (varTypePair.second == "float")
			{
				floats.erase(nextName);
			}
			else if (varTypePair.second == "string")
			{
				strings.erase(nextName);
			}
			else
			{
				eraseStructVariable(varTypePair.second, nextName);
			}
		}
	}

	template<typename T> void assign(std::unordered_map<std::string, T>& storage, const std::string& name, const std::string& value)
	{
		storage[name] = storage[value];
	}

	template<typename T> void assign(std::unordered_map<std::string, T>& storage, const std::string& name, const T& value)
	{
		storage[name] = value;
	}

	void assignStruct(const std::string& varName1, const std::string& varName2)
	{
		std::string type1 = structVarNameToTypeName[varName1];
		std::string type2 = structVarNameToTypeName[varName2];
		ASSERT(type1 == type2, "Can only assign to same type structs (%s = %s)", type1.c_str(), type2.c_str());

		for (auto& it : structs[type1].nameToType)
		{
			const std::string& variableName = it.first;
			const std::string& variableType = it.second;

			if (variableType == "int")
			{
				assign(ints, varName1 + "." + variableName, varName2 + "." + variableName);
			}
			else if (variableType == "float")
			{
				assign(floats, varName1 + "." + variableName, varName2 + "." + variableName);
			}
			else if (variableType == "string")
			{
				assign(strings, varName1 + "." + variableName, varName2 + "." + variableName);
			}
			else
			{
				assignStruct(varName1 + "." + variableName, varName2 + "." + variableName);
			}
		}
	}

	void clearCurrentScopeStack()
	{
		auto& currentScopeStack = scopeStack.back();
		for (auto& varTypePair : currentScopeStack)
		{
			if (varTypePair.second == "int")
			{
				ints.erase(varTypePair.first);
			}
			else if (varTypePair.second == "float")
			{
				floats.erase(varTypePair.first);
			}
			else if (varTypePair.second == "string")
			{
				strings.erase(varTypePair.first);
			}
			else
			{
				eraseStructVariable(varTypePair.second, varTypePair.first);
			}
		}
	}

	bool handleCommand(const std::string& cmd, std::stringstream& ss)
	{
		if (cmd.find_first_not_of("0123456789") == std::string::npos)
		{
			tempInt.push_back(std::stoi(cmd.c_str()));
			tempInt.back().priority = ++currentPriority;
			return true;
		}
		else if (cmd.find_first_not_of("-+.0123456789") == std::string::npos && cmd.find_first_of("0123456789") != std::string::npos)
		{
			tempFloat.push_back(std::stof(cmd.c_str()));
			tempFloat.back().priority = ++currentPriority;
			return true;
		}
		else if (cmd == "{")
		{
			commandStack.push_back(cmd);
			scopeStack.push_back(std::unordered_map<std::string, std::string>());
			return true;
		}
		else if (cmd == "}")
		{
			clearCurrentScopeStack();
			scopeStack.pop_back();

			// scopestack should never end up in an empty state, as initially the main layer exists.
			ASSERT(!scopeStack.empty(), "Unmatched closing bracket '}'");
			ASSERT(commandStack.back() == "{", "Unconsumed operations in scope: %s", commandStack.back().c_str());
			commandStack.pop_back();
			return true;
		}
		else if (cmd == "(")
		{
			++parenthesisBalance;
			commandStack.push_back(cmd);
			return true;
		}
		else if (cmd == ")")
		{
			--parenthesisBalance;
			if (commandStack.back() == "(")
			{
				commandStack.pop_back();

				if (!operatorVariables.empty())
				{
					std::string name = operatorVariables.back().first;
					size_t maxPriority = operatorVariables.back().second;
					if (has(ints, name))
					{
						if (tempInt.empty() || maxPriority > tempInt.back().priority)
						{
							operatorVariables.pop_back();
							tempInt.push_back(ints[name]);
							tempInt.back().priority = ++currentPriority;
						}
					}
					else if (has(floats, name))
					{
						if (tempFloat.empty() || maxPriority > tempFloat.back().priority)
						{
							operatorVariables.pop_back();
							tempFloat.push_back(floats[name]);
							tempFloat.back().priority = ++currentPriority;
						}
					}
					else if (has(strings, name))
					{
						if (tempString.empty() || maxPriority > tempString.back().priority)
						{
							operatorVariables.pop_back();
							tempString.push_back(strings[name]);
							tempString.back().priority = ++currentPriority;
						}
					}

					handleResultAssign();
				}
			}
			else
			{
				while (commandStack.back() != "(")
				{
					handleResultAll(ss);
				}
				commandStack.pop_back();
				handleResultAssign();
			}

			if (parenthesisBalance == 0 && functionCallStack.empty())
			{
				if (!tempBool.empty())
				{
					bool accept = false;
					if (!commandStack.empty())
					{
						if (commandStack.back() == "while" || commandStack.back() == "if")
						{
							accept = true;
						}
					}
					ASSERT(accept, "Unused logical boolean temporary value: '%s'\n\nScript file trace:\n%s", std::to_string(tempBool.back()).c_str(), scriptFileTrace().c_str());
				}
				ASSERT(tempInt.empty(), "Unused integer temporary value: '%d'\n\nScript file trace:\n%s", tempInt.back().t, scriptFileTrace().c_str());
				ASSERT(tempFloat.empty(), "Unused float temporary value: '%f'\n\nScript file trace:\n%s", tempFloat.back().t, scriptFileTrace().c_str());
				ASSERT(tempString.empty(), "Unused string temporary value: '%s'\n\nScript file trace:\n%s", tempString.back().t.c_str(), scriptFileTrace().c_str());
				ASSERT(operatorVariables.empty(), "Unused operator variable name: '%s'\n\nScript file trace:\n%s", operatorVariables.back().first.c_str(), scriptFileTrace().c_str());
			}

			return true;
		}
		else if (cmd == "if")
		{
			commandStack.push_back(cmd);
			std::string conditionBlock = readOperationBlock(ss);
			std::string codeBlockTrue = readCodeBlock(ss);
			std::string codeBlockFalse;
			std::string nextWord = peekWord(ss);

			bool containsElse = (nextWord == "else");
			if (containsElse)
			{
				ss >> nextWord; // remember to re-consume the word.
				codeBlockFalse = readCodeBlock(ss);
			}

			process(conditionBlock);
			ASSERT(!tempBool.empty(), "if statement's operation block (condition) did not return a boolean temporary: if%s", conditionBlock.c_str());
			bool conditionValue = tempBool.back();
			tempBool.pop_back();
			if (conditionValue)
			{
				process(codeBlockTrue);
			}
			else
			{
				process(codeBlockFalse);
			}
			commandStack.pop_back();
			return true;
		}
		else if (cmd == "while")
		{
			commandStack.push_back(cmd);

			// read condition.
			std::string word;
			std::string condition;
			int balance = 0;
			while (ss >> word)
			{
				condition += std::string(" ") + word;
				if (word == "(") ++balance;
				if (word == ")") --balance;
				if (balance == 0) break;
			}
			loopConditions.push_back(condition);

			// read body.
			std::string body;
			balance = 0;
			while (ss >> word)
			{
				body += std::string(" ") + word;
				if (word == "{") ++balance;
				if (word == "}") --balance;
				if (balance == 0) break;
			}
			loopBodies.push_back(body);

			while (true)
			{
				process(loopConditions.back());
				bool val = tempBool.back();
				tempBool.pop_back();
				if (val)
				{
					process(loopBodies.back());
				}
				else
				{
					loopBodies.pop_back();
					loopConditions.pop_back();
					break;
				}
			}

			commandStack.pop_back();
			return true;
		}
		else if (cmd == "int" || cmd == "float" || cmd == "string" || structs.find(cmd) != structs.end())
		{
			commandStack.push_back(cmd);
			return true;
		}
		else if (cmd == "fetch")
		{
			commandStack.push_back(cmd);
			return true;
		}
		else if (isOperator(cmd))
		{
			commandStack.push_back(cmd);
			return true;
		}
		else if (cmd[0] == '"')
		{
			std::string word = cmd.substr(1);
			std::stringstream stringValue;
			while (word.back() != '"')
			{
				stringValue << word << " ";
				ss >> word;
			}
			word.pop_back();
			stringValue << word;
			tempString.push_back(stringValue.str());
			tempString.back().priority = ++currentPriority;
			return true;
		}
		else if (cmd == "run")
		{
			commandStack.push_back(cmd);
			return true;
		}
		else if (cmd == "call" || cmd == "function")
		{
			commandStack.push_back(cmd);
			return true;
		}
		else if (cmd == "global")
		{
			commandStack.push_back(cmd);
			return true;
		}
		else if (cmd == "struct")
		{
			std::string structName;
			ss >> structName;
			auto structBlock = readCodeBlock(ss);
			std::stringstream struct_ss(structBlock);

			Struct userType;

			std::string structWord;
			while (struct_ss >> structWord)
			{
				if (structWord == "{" || structWord == "}")
					continue;
				std::string varName;
				struct_ss >> varName;

				userType.nameToType[varName] = structWord;
			}

			structs[structName] = userType;
			return true;
		}
		return false;
	}

	std::string prepareOutputString(std::string str) const
	{
		std::stringstream wordStream(str);
		std::string word;
		while (wordStream >> word)
		{

			std::vector<int> positions;
			for (int i = 0; i < static_cast<int>(word.length()); ++i)
			{
				if (word[i] == '$')
				{
					positions.push_back(i);
				}
			}

			while (positions.size() >= 2)
			{
				int end = positions.back(); positions.pop_back();
				int begin = positions.back(); positions.pop_back();

				std::string name = word.substr(begin + 1, end - begin - 1);
				std::string replaceString = word.substr(begin, end - begin + 1);

				if (has(ints, name))
				{
					auto pos = str.find(replaceString);
					ASSERT(pos != std::string::npos, "Unable to replace");
					str.replace(pos, end - begin + 1, std::to_string(ints.find(name)->second));
				}
				else if (has(floats, name))
				{
					auto pos = str.find(replaceString);
					ASSERT(pos != std::string::npos, "Unable to replace");
					str.replace(pos, end - begin + 1, std::to_string(floats.find(name)->second));
				}
				else if (has(strings, name))
				{
					auto pos = str.find(replaceString);
					ASSERT(pos != std::string::npos, "Unable to replace");
					str.replace(pos, end - begin + 1, strings.find(name)->second);
				}
			}
		}

		return str;
	}

	size_t currentPriority;
	int parenthesisBalance;
	std::string scriptPath;

	std::unordered_map<std::string, Value<int>> ints;
	std::unordered_map<std::string, Value<float>> floats;
	std::unordered_map<std::string, Value<std::string>> strings;

	std::unordered_map<std::string, std::pair<std::string, std::string>> functions;
	std::vector<std::unordered_map<std::string, std::string>> functionParameterMapping;

	std::unordered_map<std::string, Struct> structs;

	std::vector<Value<int>> tempInt;
	std::vector<Value<float>> tempFloat;
	std::vector<Value<std::string>> tempString;
	std::vector<bool> tempBool;
	std::vector<std::string> loopConditions;
	std::vector<std::string> loopBodies;

	std::vector<std::pair<std::string, size_t>> operatorVariables;
	std::vector<std::string> commandStack;

	std::unordered_map<std::string, std::string> structVarNameToTypeName;
	std::vector<std::unordered_map<std::string, std::string>> scopeStack;
	std::vector<std::string> scriptFileStack;
	std::vector<std::string> functionCallStack;
};