#include "game/actor/stageactor.h"
#include "game/actor/actormgr.h"
#include "game/stage/eventmgr.h"

#include "game/graphics/model.h"
#include "game/graphics/drawmgr.h"

class ActorSpawner : public StageActor
{
public:
    ActorSpawner(const ActorBuildInfo* buildInfo);
    static Actor* build(const ActorBuildInfo* buildInfo);

    u32 onCreate() override;
    u32 onExecute() override;

    Actor* child;
    u16 spawnProfileID;
    bool spawned;
};

const Profile actorSpawnerProfile(&ActorSpawner::build, ProfileId::ActorSpawner, "ActorSpawner", nullptr, 0);


ActorSpawner::ActorSpawner(const ActorBuildInfo* buildInfo) : StageActor(buildInfo) { }

Actor* ActorSpawner::build(const ActorBuildInfo* buildInfo)
{
    return new ActorSpawner(buildInfo);
}

u32 ActorSpawner::onCreate() {
    this->spawned = false;

    if (!eventID1) // nybbles 3-4
        return 2;

    this->child = nullptr;

    u16 inputID = linkID | ((movementID & 0xF) << 8);
    if (movementID & 0x10)
        spawnProfileID = Profile::spriteToProfileList[inputID];
    else
        spawnProfileID = inputID;

    spawned = false;

    return this->onExecute();
}

u32 ActorSpawner::onExecute() {
    if (EventMgr::instance->isActive(eventID1 - 1)) {

        if (!this->spawned) {
            ActorBuildInfo buildInfo = { 0 };

            buildInfo.settings1 = this->settings1;
            buildInfo.settings2 = this->settings2;
            buildInfo.profile = Profile::get(spawnProfileID);
            buildInfo.position = position;
            buildInfo.eventID1 = (this->eventID2 >> 4) & 0xF;
            buildInfo.eventID2 = this->eventID2 & 0xF;
            buildInfo.parentID = this->id;
            this->child = ActorMgr::instance->create(&buildInfo, 0);

            spawned = true;
        }
    }
    else
        spawned = false;

    return 1;
}
