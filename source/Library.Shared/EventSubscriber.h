#pragma once

namespace AnonymousEngine
{
	namespace Core
	{
		class EventSubscriber
		{
		public:
			EventSubscriber() = default;
			virtual ~EventSubscriber() = default;
			virtual void Notify(class EventPublisher& publisher) = 0;
		};
	}
}
