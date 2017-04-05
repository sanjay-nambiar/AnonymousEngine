#include "Pch.h"
#include "CreateAction.h"
#include "ActionList.h"
#include "Entity.h"
#include "TestClassHelper.h"
#include "WorldParserHelper.h"
#include "XmlParseMaster.h"
#include "World.h"
#include "DestroyAction.h"
#include "GameClock.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestLibraryDesktop
{
	using namespace std::chrono;

	TEST_CLASS(WorldXmlParserTest)
	{
		typedef AnonymousEngine::Parsers::XmlParseMaster XmlParseMaster;
		typedef AnonymousEngine::Parsers::WorldParserHelper WorldParserHelper;
		typedef AnonymousEngine::Parsers::WorldSharedData WorldSharedData;
	public:
		TEST_METHOD(TestConstructor)
		{
			WorldSharedData data;
			XmlParseMaster parser(data);
			WorldParserHelper helper;
			parser.AddHelper(helper);
			parser.RemoveHelper(helper);
			Assert::IsTrue(&data == parser.GetSharedData());
		}

		TEST_METHOD(TestParseXmlFiles)
		{
			Containers::EntityFactory entityFactory;
			Containers::ActionListFactory actionFactory;
			Containers::CreateActionFactory createActionFactory;
			Containers::DestroyActionFactory destroyActionFactory;

			for (const auto& xmlFile : TestXmlFiles)
			{
				WorldSharedData data;
				XmlParseMaster parser(data);
				WorldParserHelper helper;
				parser.AddHelper(helper);
				parser.ParseFromFile(xmlFile);
				parser.RemoveHelper(helper);

				Containers::World* world = data.ExtractWorld();
				std::string output = world->ToString();
				Assert::AreEqual(TestWorldDataString, output);
				Assert::AreEqual(xmlFile, parser.GetFileName());
				delete world;
			}
		}

		TEST_METHOD(TestInitialize)
		{
			Containers::EntityFactory entityFactory;
			Containers::ActionListFactory actionFactory;
			Containers::CreateActionFactory createActionFactory;
			Containers::DestroyActionFactory destroyActionFactory;

			WorldSharedData data1;
			XmlParseMaster parser1(data1);
			WorldParserHelper helper1;
			parser1.AddHelper(helper1);
			parser1.ParseFromFile(TestXmlFiles[0]);
			Containers::World* world1 = data1.ExtractWorld();
			std::string output = world1->ToString();
			Assert::AreEqual(TestWorldDataString, output);

			WorldSharedData data2;
			parser1.SetSharedData(data2);
			parser1.ParseFromFile(TestXmlFiles[0]);
			Containers::World* world2 = data2.ExtractWorld();
			output = world2->ToString();
			Assert::AreEqual(TestWorldDataString, output);
			Assert::AreEqual(0U, data1.Depth());
			Assert::AreEqual(0U, data2.Depth());
			delete world1;
			delete world2;
		}

		TEST_METHOD(TestClone)
		{
			Containers::EntityFactory entityFactory;
			Containers::ActionListFactory actionFactory;
			Containers::CreateActionFactory createActionFactory;
			Containers::DestroyActionFactory destroyActionFactory;

			WorldSharedData data1;
			XmlParseMaster parser1(data1);
			WorldParserHelper helper1;
			parser1.AddHelper(helper1);
			parser1.ParseFromFile(TestXmlFiles[0]);
			Assert::AreEqual(0U, data1.Depth());

			WorldSharedData& data2 = *data1.Create()->As<WorldSharedData>();
			Assert::AreEqual(data2.Depth(), data1.Depth());
			Assert::AreEqual(0U, data2.Depth());
			delete &data2;

			WorldParserHelper* helper2 = helper1.Create()->As<WorldParserHelper>();
			delete helper2;

			XmlParseMaster* parser2 = parser1.Clone();
			Assert::ExpectException<std::runtime_error>([&parser2, &helper1]() { parser2->AddHelper(helper1); });
			Assert::ExpectException<std::runtime_error>([&parser2, &helper1] () { parser2->RemoveHelper(helper1); });
			WorldSharedData* parser2Data = parser2->GetSharedData()->As<WorldSharedData>();
			parser2Data;
			delete parser2;
		}

		TEST_METHOD(TestInvalidXmls)
		{
			Containers::EntityFactory entityFactory;
			Containers::CreateActionFactory actionFactory;

			WorldSharedData data;
			XmlParseMaster parser(data);
			WorldParserHelper helper;
			parser.AddHelper(helper);
			parser.Parse(InvalidXmls[0]);
			Parsers::SharedData baseData;
			parser.SetSharedData(baseData);
			Assert::ExpectException<std::runtime_error>([&parser] () { parser.Parse(TestXmlFiles[0]); });
		}

		TEST_METHOD(TestWorldUpdate)
		{
			Containers::EntityFactory entityFactory;
			Containers::ActionListFactory actionFactory;
			Containers::CreateActionFactory createActionFactory;
			Containers::DestroyActionFactory destroyActionFactory;

			WorldSharedData data;
			XmlParseMaster parser(data);
			WorldParserHelper helper;
			parser.AddHelper(helper);
			parser.ParseFromFile(TestXmlFiles[0]);
			Containers::World* world = data.ExtractWorld();
			std::string output = world->ToString();
			Assert::AreEqual(TestWorldDataString, output);

			Containers::WorldState state;
			GameClock clock;
			clock.StartTime();
			clock.UpdateGameTime(state.mGameTime);
			std::uint32_t runs = ((mHelper.GetRandomUInt32() % 10) + 5);
			for (std::uint32_t index = 0; index < runs; ++index)
			{
				clock.UpdateGameTime(state.mGameTime);
				Assert::IsTrue(clock.StartTime() <= clock.CurrentTime());
				Assert::IsTrue(clock.StartTime() <= clock.LastTime());
				Assert::IsTrue(clock.CurrentTime() >= clock.LastTime());
				Assert::IsTrue(state.mGameTime.CurrentTime() == clock.CurrentTime());
				Assert::IsTrue(state.mGameTime.ElapsedGameTime() >= milliseconds());
				Assert::IsTrue(state.mGameTime.TotalGameTime() == duration_cast<milliseconds>(clock.CurrentTime() - clock.StartTime()));
				world->Update(state);
			}

			delete world;
		}

		TEST_CLASS_INITIALIZE(InitializeClass)
		{
			mHelper.BeginClass();
		}

		TEST_METHOD_INITIALIZE(Setup)
		{
			//mHelper.Setup();
		}

		TEST_METHOD_CLEANUP(Teardown)
		{
			//mHelper.Teardown();
		}

		TEST_CLASS_CLEANUP(CleanupClass)
		{
			mHelper.EndClass();
		}

		static TestClassHelper mHelper;
		static const AnonymousEngine::Vector<std::string> TestXmlFiles;
		static const AnonymousEngine::Vector<std::string> InvalidXmls;
		static const std::string TestWorldDataString;
	};

	TestClassHelper WorldXmlParserTest::mHelper;

	const AnonymousEngine::Vector<std::string> WorldXmlParserTest::TestXmlFiles = {
		"TestData/world.xml"
	};

	const AnonymousEngine::Vector<std::string> WorldXmlParserTest::InvalidXmls = {
		"<foo></foo>"
	};

	const std::string WorldXmlParserTest::TestWorldDataString = "{\"Name\": \"Skyrim\", \"Sectors\": [{\"Name\": \"Whiterun\", \"Entities\": [{\"Name\": \"Bannered Mare\", \"Actions\": [{\"Name\": \"Init\", \"Capacity\": \"10\"}, {\"Name\": \"Upgrade\", \"Capacity\": \"5\"}], \"Owner\": \"Hulda\", \"Beds\": \"10\", \"Price\": \"20000.500000\", \"Location\": \"0.500000,10.200000,100.000000,1.000000\", \"Transform\": \"1.200000,10.200000,0.005000,1.000000,3.600000,102.000000,0.010000,1.000000,1000.000000,177.199997,101.000000,1.000000,11.000000,13.600000,100.000351,1.000000\"}, {\"Name\": \"Dragonsreach\", \"Owner\": \"Balgruuf the Greater\", \"Location\": \"50.400002,12.300000,10.000000,1.000000\"}], \"Init\": {\"Name\": \"Init\", \"MaxStudents\": \"100\"}, \"Jarl\": \"Balgruuf the Greater\"}, {\"Name\": \"Winterhold\", \"Entities\": {\"Name\": \"College of Winterhold\", \"Actions\": {\"Name\": \"Init\", \"MaxStudents\": \"100\"}, \"Arch-Mage\": \"Savos Aren\", \"Location\": \"77.040001,27.900000,20.000000,1.000000\"}, \"Jarl\": \"Korir\"}], \"Actions\": [{\"Name\": \"Init\", \"InstanceName\": \"TestAction\", \"ClassName\": \"ActionList\"}, {\"Name\": \"Destroy\", \"InstanceName\": \"TestAction\"}], \"Population\": \"100000\"}";
}
