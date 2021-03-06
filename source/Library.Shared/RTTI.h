#pragma once

#include <string>
#include <cstdint>

#define ANONYMOUS_UNREFERENCED(variable) variable;

namespace AnonymousEngine
{
	class RTTI
	{
	public:
		virtual ~RTTI() = default;
		
		virtual std::uint64_t TypeIdInstance() const = 0;
		
		virtual RTTI* QueryInterface(const std::uint64_t id) const
		{
			ANONYMOUS_UNREFERENCED(id);
			return nullptr;
		}

		virtual bool Is(std::uint64_t id) const
		{
			ANONYMOUS_UNREFERENCED(id);
			return false;
		}

		virtual bool Is(const std::string& name) const
		{
			ANONYMOUS_UNREFERENCED(name);
			return false;
		}

		template <typename T>
		T* As() const
		{
			if (Is(T::TypeIdClass()))
			{
				return reinterpret_cast<T*>(const_cast<RTTI*>(this));
			}

			return nullptr;
		}

		virtual bool Equals(const RTTI* rhs) const
		{
			return this == rhs;
		}

		virtual std::string ToString() const
		{
			throw std::runtime_error("Not implemented");
		}

		virtual void FromString(const std::string& str)
		{
			ANONYMOUS_UNREFERENCED(str);
			throw std::runtime_error("Not implemented");
		}
	};

#define RTTI_DECLARATIONS(Type, ParentType)																	 \
		public:                                                                                              \
			typedef ParentType Parent;                                                                       \
			static std::string TypeName() { return std::string(#Type); }                                     \
			static std::uint64_t TypeIdClass() { return sRunTimeTypeId; }                                    \
			virtual std::uint64_t TypeIdInstance() const override { return Type::TypeIdClass(); }            \
			virtual AnonymousEngine::RTTI* QueryInterface(const std::uint64_t id) const override             \
            {                                                                                                \
                if (id == sRunTimeTypeId)                                                                    \
					{ return (RTTI*)this; }                                                                  \
                else                                                                                         \
					{ return Parent::QueryInterface(id); }                                                   \
            }                                                                                                \
			virtual bool Is(std::uint64_t id) const override                                                 \
			{                                                                                                \
				if (id == sRunTimeTypeId)                                                                    \
					{ return true; }                                                                         \
				else                                                                                         \
					{ return Parent::Is(id); }                                                               \
			}                                                                                                \
			virtual bool Is(const std::string& name) const override                                          \
			{                                                                                                \
				if (name == TypeName())                                                                      \
					{ return true; }                                                                         \
				else                                                                                         \
					{ return Parent::Is(name); }                                                             \
			}                                                                                                \
			private:                                                                                         \
				static std::uint64_t sRunTimeTypeId;

#define RTTI_DEFINITIONS(Type) std::uint64_t Type::sRunTimeTypeId = reinterpret_cast<std::uint64_t>(&Type::sRunTimeTypeId);
}
