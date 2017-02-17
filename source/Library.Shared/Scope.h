#pragma once

#include "Datum.h"
#include "HashMap.h"
#include "Vector.h"

namespace AnonymousEngine
{
	class Scope
	{
	public:
		explicit Scope();

		Scope(const Scope& rhs);

		Scope& operator=(const Scope& rhs);

		Datum* Find(const std::string& name);

		Datum* Search(const std::string& name, Scope** foundScope = nullptr);

		Datum& Append(const std::string& name);

		Scope& AppendScope(const std::string& name);

		void Adopt(Scope& scope, const std::string& name);

		const Scope* GetParent() const;

		Datum* operator[](const std::string& name);

		Datum* operator[](const std::uint32_t index);

		bool operator==(const Scope& rhs) const;

		bool operator!=(const Scope& rhs) const;

		virtual ~Scope();

	private:

		HashMap<std::string, Datum> mData;
		Vector<std::pair<std::string, Datum>*> mOrderVector;

		Scope* mParent;

		void Copy(const Scope& rhs);
		void Clear();
		void Orphan(Scope& scope);
	};
}
