
#include "test/testframework.hpp"
#include "script/Scripter.hpp"

TEST(Scripts, BasicOperations)
{
	Scripter scripter;
	scripter.process("int value");
	scripter.process("int test");
	scripter.process("value = (5 + 7)");
	scripter.process("test = (value * (5 + 7))");
	scripter.process("if(test < value){test = (0) value = (0)}");
	scripter.process("string name string lastname string firstname");
	scripter.process("name = (\"Anna-Liisa Kekkonen\")");
	scripter.process("lastname = (\"Kekkonen\")");
	scripter.process("firstname = (name - lastname)");
	scripter.process("int a a = (1+2+3+4+5)");
	EXPECT_EQ(5 + 7, scripter.getInt("value"));
	EXPECT_EQ(15, scripter.getInt("a"));
	EXPECT_EQ((5 + 7) * (5 + 7), scripter.getInt("test"));
	EXPECT_EQ(std::string("Anna-Liisa Kekkonen"), scripter.getString("name"));
	EXPECT_EQ(std::string("Kekkonen"), scripter.getString("lastname"));
	EXPECT_EQ(std::string("Anna-Liisa "), scripter.getString("firstname"));
}

TEST(Scripts, ExtensionsTest)
{
	Scripter scripter;
	bool reaction = false;
	auto commandFunc = [&reaction](Scripter::Interface& iscripter, const std::string& cmd, std::stringstream&) -> bool {
		if (cmd == "testCommand")
		{
			reaction = true;
			return true;
		}

		return false;
	};

	auto resultFunc = [](Scripter::Interface& iscripter, std::stringstream&) -> bool { return false; };

	Scripter::Extension extension(commandFunc, resultFunc);
	scripter.addExtension(extension);
	scripter.process("testCommand");
	EXPECT_TRUE(reaction);
}

TEST(Scripts, Loops)
{
	Scripter scripter;
	scripter.process("int a int b");
	scripter.process("a = (0) b = (10)");
	scripter.process("while(a < b){a = (a + 1)}");
	EXPECT_EQ(scripter.getInt("a"), scripter.getInt("b"));
}

TEST(Scripts, StringValueReplacement)
{
	Scripter scripter;
	scripter.process("int a string b");
	scripter.process("a = (10) b = (\"$a$/10\")");
	
	Scripter::Interface interf(scripter);
	std::string bakedString;
	interf.getStringVal("b", bakedString);
	EXPECT_EQ(std::string("10/10"), bakedString);
}

TEST(Scripts, LogicalOperators)
{
	Scripter scripter;
	scripter.process("int a int b int c");
	scripter.process("a = (10) b = (4) c = (0)");
	scripter.process("if((a > b) and (b < 5)) {c=(1)}");
	EXPECT_EQ(1, scripter.getInt("c"));
	scripter.process("if((a > b) xor (b < 5)) {c=(2)}");
	EXPECT_EQ(1, scripter.getInt("c"));
	scripter.process("if((a > b) or (b < 5)) {c=(3)}");
	EXPECT_EQ(3, scripter.getInt("c"));
	scripter.process("if(not(a > b)) {c=(4)}");
	EXPECT_EQ(3, scripter.getInt("c"));
	scripter.process("if(not(a < b)) {c=(5)}");
	EXPECT_EQ(5, scripter.getInt("c"));
}

TEST(Scripts, FunctionCall)
{
	Scripter scripter;
	scripter.process("(function foo)(int a, float b){(a+(icast(b)))} int res res = ((call foo)(1, 2.0))");
	EXPECT_EQ(3, scripter.getInt("res"));
}

TEST(Scripts, NestedFunctionCall)
{
	Scripter scripter;
	scripter.process("(function foo)(int a, float b) {(a+(icast(b)))} \
					  (function bar)(float c, int d) {(call foo)(d, c)} \
					  int res \
					  res = (call bar)(1.0, 2)"
					);
	EXPECT_EQ(3, scripter.getInt("res"));
}