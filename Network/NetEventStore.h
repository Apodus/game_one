#pragma once

namespace net
{
	class EventStore
	{
	public:
		EventStore()
		{}

		void AddEvent();

		struct Event
		{
			uint32_t timeMs;

		};

	private:


	};
}
