#include "WorldParserHelper.h"
#include "WorldSharedData.h"
#include "World.h"
#include "Sector.h"
#include "Entity.h"
#include "Action.h"
#include <cassert>

#define ValidateRequiredAttributes(attributes) assert(attributes.ContainsKey(NAME))
#define ValidateSharedDataNotNull(sharedData) assert(sharedData.mAttributed != nullptr)
#define ValidateSharedDataScopeType(sharedData, Type) assert(sharedData.mAttributed->Is(Type::TypeIdClass()))
#define ValidateFactoryInputAttributes(attributes) assert(attributes.ContainsKey(CLASS))
#define ValidateParentIsList(sharedData, listTypeString) assert(sharedData.mElementStack[sharedData.mElementStack.Size()-1] == listTypeString)

namespace AnonymousEngine
{
	namespace Parsers
	{
		using namespace AnonymousEngine::Containers;

		RTTI_DEFINITIONS(WorldParserHelper)

		const HashMap<std::string, WorldParserHelper::StartHandlerFunction> WorldParserHelper::StartElementHandlers = {
			{"integer", HandleIntegerStart},
			{"float", HandleFloatStart},
			{"string", HandleStringStart},
			{"vector", HandleVectorStart},
			{"matrix", HandleMatrixStart},
			{"world", HandleWorldStart},
			{"sectors", HandleListStart},
			{"sector", HandleSectorStart},
			{"entities", HandleListStart},
			{"entity", HandleEntityStart},
			{"actions", HandleListStart},
			{"action", HandleActionStart},
			{"file", HandleFileStart}
		};

		const HashMap<std::string, WorldParserHelper::EndHandlerFunction> WorldParserHelper::EndElementHandlers = {
			{"integer", HandlePrimitivesEnd},
			{"float", HandlePrimitivesEnd},
			{"string", HandlePrimitivesEnd},
			{"vector", HandlePrimitivesEnd},
			{"matrix", HandleMatrixEnd},
			{"world", HandleAttributedEnd},
			{"sectors", HandleListEnd},
			{"sector", HandleAttributedEnd},
			{"entities", HandleListEnd},
			{"entity", HandleAttributedEnd},
			{"actions", HandleListEnd},
			{"action", HandleAttributedEnd},
			{"file", HandleFileEnd}
		};

		const std::string WorldParserHelper::NAME = "name";
		const std::string WorldParserHelper::CLASS = "class";
		const std::string WorldParserHelper::VALUE = "value";
		const std::string WorldParserHelper::VECTOR_X = "x";
		const std::string WorldParserHelper::VECTOR_Y = "y";
		const std::string WorldParserHelper::VECTOR_Z = "z";
		const std::string WorldParserHelper::VECTOR_W = "w";

		IXmlParserHelper* WorldParserHelper::Create()
		{
			WorldParserHelper* helper = new WorldParserHelper();
			return helper;
		}

		bool WorldParserHelper::StartElementHandler(SharedData& sharedData, const std::string& name, const AttributeMap& attributes)
		{
			if (!sharedData.Is(WorldSharedData::TypeIdClass()) || !StartElementHandlers.ContainsKey(name))
			{
				return false;
			}
			WorldSharedData& data = *(sharedData.As<WorldSharedData>());
			StartElementHandlers[name](data, attributes);
			data.mElementStack.PushBack(name);
			return true;
		}

		bool WorldParserHelper::EndElementHandler(SharedData& sharedData, const std::string& name)
		{
			if (!sharedData.Is(WorldSharedData::TypeIdClass()) || !StartElementHandlers.ContainsKey(name))
			{
				return false;
			}
			WorldSharedData& data = *(sharedData.As<WorldSharedData>());
			EndElementHandlers[name](data);
			data.mElementStack.PopBack();
			return true;
		}

		void WorldParserHelper::HandleIntegerStart(WorldSharedData& sharedData, const AttributeMap& attributes)
		{
			ValidateSharedDataNotNull(sharedData);
			ValidateRequiredAttributes(attributes);
			Datum& datum = sharedData.mAttributed->Append(attributes[NAME]);
			if (attributes.ContainsKey(VALUE))
			{
				datum.PushBack(std::stoi(attributes[VALUE]));
			}
		}

		void WorldParserHelper::HandleFloatStart(WorldSharedData& sharedData, const AttributeMap& attributes)
		{
			ValidateSharedDataNotNull(sharedData);
			ValidateRequiredAttributes(attributes);
			Datum& datum = sharedData.mAttributed->Append(attributes[NAME]);
			if (attributes.ContainsKey(VALUE))
			{
				datum.PushBack(std::stof(attributes[VALUE]));
			}
		}

		void WorldParserHelper::HandleStringStart(WorldSharedData& sharedData, const AttributeMap& attributes)
		{
			ValidateSharedDataNotNull(sharedData);
			ValidateRequiredAttributes(attributes);
			Datum& datum = sharedData.mAttributed->Append(attributes[NAME]);
			if (attributes.ContainsKey(VALUE))
			{
				datum.PushBack(attributes[VALUE]);
			}
		}

		void WorldParserHelper::HandleVectorStart(WorldSharedData& sharedData, const AttributeMap& attributes)
		{
			ValidateSharedDataNotNull(sharedData);
			ValidateRequiredAttributes(attributes);
			if (attributes.ContainsKey(VECTOR_X) && attributes.ContainsKey(VECTOR_Y) && attributes.ContainsKey(VECTOR_Z) && attributes.ContainsKey(VECTOR_W))
			{
				Datum datum;
				datum.SetType(Datum::DatumType::Vector);
				std::string toParse = attributes[VECTOR_X];
				toParse.append(",");
				toParse.append(attributes[VECTOR_Y]).append(",");
				toParse.append(attributes[VECTOR_Z]).append(",");
				toParse.append(attributes[VECTOR_W]);
				datum.Resize(1U);
				datum.SetFromString(toParse);

				if (sharedData.mMatrixName.empty())
				{
					sharedData.mAttributed->Append(attributes[NAME]).PushBack(datum.Get<glm::vec4>());
				}
				else
				{
					sharedData.mMatrixVectors.PushBack(datum);
				}
			}
		}

		void WorldParserHelper::HandleMatrixStart(WorldSharedData& sharedData, const AttributeMap& attributes)
		{
			ValidateSharedDataNotNull(sharedData);
			ValidateRequiredAttributes(attributes);
			sharedData.mMatrixName = attributes[NAME];
		}

		void WorldParserHelper::HandleWorldStart(WorldSharedData& sharedData, const AttributeMap& attributes)
		{
			assert(sharedData.mAttributed == nullptr);
			ValidateRequiredAttributes(attributes);
			sharedData.mAttributed = new World(attributes[NAME]);
		}

		void WorldParserHelper::HandleSectorStart(WorldSharedData& sharedData, const AttributeMap& attributes)
		{
			ValidateSharedDataNotNull(sharedData);
			ValidateRequiredAttributes(attributes);
			ValidateSharedDataScopeType(sharedData, World);
			ValidateParentIsList(sharedData, "sectors");
			Sector* sector = &(static_cast<World*>(sharedData.mAttributed)->CreateSector(attributes[NAME]));
			sharedData.mAttributed = sector;
		}

		void WorldParserHelper::HandleEntityStart(WorldSharedData& sharedData, const AttributeMap& attributes)
		{
			ValidateSharedDataNotNull(sharedData);
			ValidateRequiredAttributes(attributes);
			ValidateSharedDataScopeType(sharedData, Sector);
			ValidateFactoryInputAttributes(attributes);
			ValidateParentIsList(sharedData, "entities");
			Entity* entity = &(static_cast<Sector*>(sharedData.mAttributed)->CreateEntity(attributes[NAME], attributes[CLASS]));
			sharedData.mAttributed = entity;
		}

		void WorldParserHelper::HandleActionStart(WorldSharedData& sharedData, const AttributeMap& attributes)
		{
			ValidateSharedDataNotNull(sharedData);
			ValidateRequiredAttributes(attributes);
			ValidateSharedDataScopeType(sharedData, Entity);
			ValidateFactoryInputAttributes(attributes);
			ValidateParentIsList(sharedData, "actions");
			Action* action = &(static_cast<Entity*>(sharedData.mAttributed)->CreateAction(attributes[NAME], attributes[CLASS]));
			sharedData.mAttributed = action;
		}

		void WorldParserHelper::HandleListStart(WorldSharedData&, const AttributeMap&)
		{
			// do nothing
		}

		void WorldParserHelper::HandleFileStart(WorldSharedData&, const AttributeMap&)
		{
			
		}

		void WorldParserHelper::HandlePrimitivesEnd(WorldSharedData&)
		{
			// do nothing
		}

		void WorldParserHelper::HandleMatrixEnd(WorldSharedData& sharedData)
		{
			sharedData.mAttributed->Append(sharedData.mMatrixName).PushBack(glm::mat4(sharedData.mMatrixVectors[0].Get<glm::vec4>(),
				sharedData.mMatrixVectors[1].Get<glm::vec4>(), sharedData.mMatrixVectors[2].Get<glm::vec4>(), sharedData.mMatrixVectors[3].Get<glm::vec4>()));
			sharedData.mMatrixName.clear();
		}

		void WorldParserHelper::HandleAttributedEnd(WorldSharedData& sharedData)
		{
			if (sharedData.Depth() > 1)
			{
				sharedData.mAttributed = static_cast<Attributed*>(sharedData.mAttributed->GetParent());
			}
		}

		void WorldParserHelper::HandleListEnd(WorldSharedData&)
		{
			// do nothing
		}

		void WorldParserHelper::HandleFileEnd(WorldSharedData&)
		{
			
		}
	}
}
