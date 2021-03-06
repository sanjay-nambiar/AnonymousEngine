#include "Scope.h"

namespace AnonymousEngine
{
	RTTI_DEFINITIONS(Scope)

	Scope::Scope() : mParent(nullptr), mParentDatumIndex(0)
	{
	}

	Scope::Scope(const Scope& rhs) : Scope()
	{
		Copy(rhs);
	}

	Scope& Scope::operator=(const Scope& rhs)
	{
		if (this != &rhs)
		{
			Clear();
			Copy(rhs);
		}
		return (*this);
	}

	Scope::Scope(Scope&& rhs) noexcept :
		Scope()
	{
		Move(rhs);
	}

	Scope& Scope::operator=(Scope&& rhs) noexcept
	{
		if (this != &rhs)
		{
			Clear();
			Move(rhs);
		}
		return (*this);
	}

	Datum* Scope::Find(const std::string& name)
	{
		auto it = mDatumMap.Find(name);
		if (it != mDatumMap.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	const Datum* Scope::Find(const std::string& name) const
	{
		return const_cast<Scope*>(this)->Find(name);
	}

	Datum* Scope::Search(const std::string& name, Scope** foundScope)
	{
		Scope* scopeToSearch = this;
		while (scopeToSearch != nullptr)
		{
			Datum* datum = scopeToSearch->Find(name);
			if (datum != nullptr)
			{
				if (foundScope != nullptr)
				{
					*foundScope = scopeToSearch;
				}
				return datum;
			}
			scopeToSearch = scopeToSearch->mParent;
		}
		return nullptr;
	}

	const Datum* Scope::Search(const std::string& name, Scope** foundScope) const
	{
		return const_cast<Scope*>(this)->Search(name, foundScope);
	}

	Datum& Scope::Append(const std::string& name)
	{
		bool hasInserted;
		auto it = mDatumMap.Insert(std::make_pair(name, Datum()), hasInserted);
		if (hasInserted)
		{
			mOrderVector.PushBack(&(*it));
		}
		return it->second;
	}

	Scope& Scope::AppendScope(const std::string& name)
	{
		Datum& datum = Append(name);
		datum.SetType(Datum::DatumType::Scope);
		Scope* scope = new Scope();
		scope->mParent = this;
		scope->mParentKey = name;
		scope->mParentDatumIndex = datum.Size();
		datum.PushBack(*scope);
		return *scope;
	}

	void Scope::Adopt(Scope& scope, const std::string& name)
	{
		if (&scope == this)
		{
			throw std::invalid_argument("Cannot adopt yourself.");
		}
		Datum& datum = Append(name);
		datum.SetType(Datum::DatumType::Scope);
		scope.Orphan();
		scope.mParent = this;
		scope.mParentKey = name;
		scope.mParentDatumIndex = datum.Size();
		datum.PushBack(scope);
	}

	Scope* Scope::GetParent() const
	{
		return mParent;
	}

	std::string Scope::GetParentKey() const
	{
		return mParentKey;
	}

	Datum& Scope::operator[](const std::string& name)
	{
		return Append(name);
	}

	const Datum& Scope::operator[](const std::string& name) const
	{
		return mDatumMap[name];
	}

	Datum& Scope::operator[](const std::uint32_t index)
	{
		return mOrderVector[index]->second;
	}

	const Datum& Scope::operator[](const std::uint32_t index) const
	{
		return mOrderVector[index]->second;
	}

	bool Scope::operator==(const Scope& rhs) const
	{
		return (mDatumMap == rhs.mDatumMap);
	}

	bool Scope::operator!=(const Scope& rhs) const
	{
		return !(operator==(rhs));
	}

	Scope::~Scope()
	{
		Clear();
	}

	bool Scope::Equals(const RTTI* rhs) const
	{
		if (this == rhs)
		{
			return true;
		}

		if (rhs == nullptr)
		{
			return false;
		}

		const Scope* scope = rhs->As<Scope>();
		if (scope != nullptr)
		{
			return operator==(*scope);
		}
		return false;
	}

	std::string Scope::ToString() const
	{
		std::string output = "{";
		std::uint32_t attributeIndex = 0;
		for (const auto& entry : mOrderVector)
		{
			if (entry->first == "this" || entry->second.Size() == 0)
			{
				continue;
			}

			std::uint32_t entrySize = entry->second.Size();
			if (attributeIndex > 0)
			{
				output.append(", ");
			}
			output.append("\"").append(entry->first).append("\"").append((entrySize > 1) ? ": [" : ": ");
			for (std::uint32_t i = 0; i < entry->second.Size(); ++i)
			{
				if (entry->second.Type() != Datum::DatumType::Scope)
				{
					output.append((i == 0) ? "\"" : ", \"");
				}
				else if (i > 0)
				{
					output.append(", ");
				}
				output.append(entry->second.ToString(i));
				if (entry->second.Type() != Datum::DatumType::Scope)
				{
					output.append("\"");
				}
			}
			output.append((entrySize > 1) ? "]" : "");
			++attributeIndex;
		}
		output.append("}");
		return output;
	}

	void Scope::FromString(const std::string&)
	{
		throw std::runtime_error("Unsupported operation");
	}

	void Scope::Clear()
	{
		Orphan();
		for (const auto& pairPtr : mOrderVector)
		{
			Datum& datum = pairPtr->second;
			if (datum.Type() == Datum::DatumType::Scope)
			{
				std::uint32_t size = datum.Size();
				for (std::uint32_t index = 0; index < size; ++index)
				{
					Scope& childScope = datum.Get<Scope>();
					delete &childScope;
				}
			}
		}
		mOrderVector.Clear();
		mDatumMap.Clear();
	}

	void Scope::Copy(const Scope& rhs)
	{
		for (const auto pairPtr : rhs.mOrderVector)
		{
			const std::string& key = pairPtr->first;
			const Datum& rhsDatum = pairPtr->second;
			if (rhsDatum.Type() == Datum::DatumType::Scope)
			{
				Datum& datumToAppend = Append(key);
				for (std::uint32_t index = 0; index < rhsDatum.Size(); ++index)
				{
					Scope* scope = new Scope(rhsDatum.Get<Scope>(index));
					scope->mParent = this;
					scope->mParentKey = key;
					scope->mParentDatumIndex = index;
					datumToAppend.PushBack(*scope);
				}
			}
			else
			{
				Append(key) = rhsDatum;
			}
		}
		mParent = nullptr;
		mParentKey.clear();
		mParentDatumIndex = 0;
	}

	void Scope::Move(Scope& rhs)
	{
		mDatumMap = std::move(rhs.mDatumMap);
		mOrderVector = std::move(rhs.mOrderVector);
		mParent = rhs.mParent;
		mParentKey = rhs.mParentKey;
		mParentDatumIndex = rhs.mParentDatumIndex;
		if (mParent != nullptr)
		{
			(*mParent)[mParentKey].Set(*this, mParentDatumIndex);
		}

		for (auto pairPtr : mOrderVector)
		{
			Datum& datum = pairPtr->second;
			if (datum.Type() == Datum::DatumType::Scope)
			{
				for(std::uint32_t index = 0; index < datum.Size(); ++index)
				{
					datum.Get<Scope>(index).mParent = this;
				}
			}
		}

		rhs.mParent = nullptr;
		rhs.mParentKey.clear();
		rhs.mParentDatumIndex = 0;
	}

	void Scope::Orphan()
	{
		if (mParent != nullptr)
		{
			(*mParent)[mParentKey].Remove(*this);
			mParent = nullptr;
			mParentKey.clear();
			mParentDatumIndex = 0;
		}
	}
}
