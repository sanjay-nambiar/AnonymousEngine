#include "Pch.h"
#include <cstdint>
#include <cstring>
#include "TestClassHelper.h"
#include "HashFunctions.h"
#include "Foo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestLibraryDesktop
{
	TEST_CLASS(HashFunctionsTest)
	{
	public:
		TEST_METHOD(TestHashFunctionWithInt)
		{
			std::uint32_t value1 = mHelper.GetRandomUInt32();
			std::uint32_t value2 = mHelper.GetRandomUInt32();
			Assert::AreEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&value1), sizeof(value1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&value1), sizeof(value1)));
			Assert::AreNotEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&value1), sizeof(value1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&value2), sizeof(value2)));
		}

		TEST_METHOD(TestHashFunctionWithIntPointer)
		{
			std::uint32_t value1 = mHelper.GetRandomUInt32();
			std::uint32_t value2 = value1;
			std::uint32_t value3 = mHelper.GetRandomUInt32();
			std::uint32_t* ptr1 = &value1;
			std::uint32_t* ptr2 = &value1;
			Assert::AreEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&ptr1), sizeof(ptr1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&ptr2), sizeof(ptr2)));
			ptr2 = &value2;
			Assert::AreNotEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&ptr1), sizeof(ptr1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&ptr2), sizeof(ptr2)));
			ptr2 = &value3;
			Assert::AreNotEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&ptr1), sizeof(ptr1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&ptr2), sizeof(ptr2)));
		}

		TEST_METHOD(TestHashFunctionWithCharPointer)
		{
			const char* string1 = "Hello";
			const char* string2 = "Hello";
			const char* string3 = "Hello, World!";
			Assert::AreEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string1), strlen(string1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string2), strlen(string2)));
			Assert::AreEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string1), strlen(string1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string1), strlen(string1)));
			Assert::AreNotEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string1), strlen(string1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string3), strlen(string3)));
		}

		TEST_METHOD(TestHashFunctionWithString)
		{
			std::string string1 = "Hello";
			std::string string2 = "Hello";
			std::string string3 = "Hello, World!";
			Assert::AreEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string1.c_str()), string1.size()),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string2.c_str()), string2.size()));
			Assert::AreEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string1.c_str()), string1.size()),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string1.c_str()), string1.size()));
			Assert::AreNotEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string1.c_str()), string1.size()),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(string3.c_str()), string3.size()));
		}

		TEST_METHOD(TestHashFunctionWithFoo)
		{
			Foo f1 = Foo(mHelper.GetRandomUInt32());
			Foo f2 = f1;
			Foo f3 = Foo(mHelper.GetRandomUInt32());
			Assert::AreEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&f1), sizeof(f1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&f1), sizeof(f1)));
			Assert::AreNotEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&f2), sizeof(f2)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&f1), sizeof(f1)));
			std::uint32_t value1 = f1.Data();
			std::uint32_t value2 = f2.Data();
			Assert::AreEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&value1), sizeof(f1)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&value2), sizeof(f2)));
			Assert::AreNotEqual(AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&f3), sizeof(f3)),
				AnonymousEngine::SuperFastHash(reinterpret_cast<const std::int8_t*>(&f1), sizeof(f1)));
		}

		TEST_CLASS_INITIALIZE(InitializeClass)
		{
			mHelper.BeginClass();
		}

		TEST_METHOD_INITIALIZE(Setup)
		{
			mHelper.Setup();
		}

		TEST_METHOD_CLEANUP(Teardown)
		{
			mHelper.Teardown();
		}

		TEST_CLASS_CLEANUP(CleanupClass)
		{
			mHelper.EndClass();
		}
	private:
		static TestClassHelper mHelper;
	};

	TestClassHelper HashFunctionsTest::mHelper;
}
