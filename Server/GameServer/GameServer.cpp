#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
//#include "ClientPacketHandler.h"
#include "Room.h"
#include <tchar.h>
#include "Job.h"
#include "GameMap.h"
#include <queue>
#include "GameMap.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}


int main()
{
	ServerPacketHandler::Init();

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[=]() { return make_shared<GameSession>(); }, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
				//cout << "Thread Launch : " <<  << endl;
			});
	}


	//shared_ptr<GameMap> Map = make_shared<GameMap>();
	//FNode& temp = Map->GetNode(3, 3);
	//cout << temp._x << " " << temp._y << endl;


	// Main Thread
	// DoWorkerJob(service);

	/*while (true)
	{
		Protocol::S_CHAT pkt;
		pkt.set_msg("HelloWorld");
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		GSessionManager.Broadcast(sendBuffer);
		this_thread::sleep_for(1s);
	}*/

	/*priority_queue<PQNode> nodes;

	set<PQNode> nodeset;

	PQNode a;
	a.F = 3;

	PQNode b;
	b.F = 5;

	nodes.push(a);
	nodes.push(b);

	nodeset.insert(a);
	nodeset.insert(b);


	PQNode c;
	c.F = 3;

	if (nodeset.find(c) != nodeset.end())
	{
		cout << c.F << endl;
	}*/
	

	GRoom->BeginPlay();
	GRoom->DoAsync(&Room::UpdateTick);

	while (true)
	{
		this_thread::sleep_for(1s);
	}

	// Template ID
	// DB ID
	// Game ID

	uint64 gameid; 

	GThreadManager->Join();
}