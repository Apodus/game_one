#pragma once

#include "NetTypes.h"

#include <vector>
#include <memory>

namespace net
{
	class Photon;
	class Engine;
	class DataAdapter;

	class Session
	{
		friend class Engine;
	public:
		Session(Engine& engine);
		virtual ~Session();

		void AddAdapter(uint8_t eventId, uint8_t keyId, net::DataAdapter& adapter);

		void AddMember(uint64_t id);

		void RemoveMember(uint64_t id);

		uint64_t GetMemberId(MemberIndex i) const;

		virtual void OnMemberJoined(MemberIndex i) = 0;

		virtual void OnMemberLeft(MemberIndex i) = 0;

		struct Member
		{
			uint64_t id;
		};

	private:
		void PreTick();

		void PostTick();

		std::vector<Member> myMembers;
		std::unique_ptr<net::Photon> myPhoton;
		Engine& myEngine;

	};
}