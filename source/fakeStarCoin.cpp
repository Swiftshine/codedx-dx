#include "game/actor/stageactor.h"

#include "game/actor/movementhandler.h"
#include "game/graphics/model.h"
#include "game/graphics/drawmgr.h"
#include "game/graphics/effect.h"
#include "game/sound/sound.h"
#include "game/stage/eventmgr.h"

class FakeStarCoin : public StageActor
{
public:
    FakeStarCoin(const ActorBuildInfo* buildInfo);
    static Actor* build(const ActorBuildInfo* buildInfo);

    u32 onCreate() override;
    u32 onExecute() override;
    u32 onDraw() override;

    void updateModel();
    void collect();

    static void collisionCallback(ActiveCollider* acSelf, ActiveCollider* acOther);

    ModelWrapper* model;
    MovementHandler movementHandler;
    u8 rotationDirection;

    static const ActiveCollider::Info collisionInfo;
};

const Profile fakeStarCoinProfile(&FakeStarCoin::build, ProfileId::FakeStarCoin, "FakeStarCoin", nullptr, 0);
PROFILE_RESOURCES(ProfileId::FakeStarCoin, "star_coin");

const ActiveCollider::Info FakeStarCoin::collisionInfo = { {0.0f, -3.0f}, {12.0f, 15.0f}, 0, 5, 0, 0x824F, 0x20208, 0, &FakeStarCoin::collisionCallback };


FakeStarCoin::FakeStarCoin(const ActorBuildInfo* buildInfo) : StageActor(buildInfo) { }

Actor* FakeStarCoin::build(const ActorBuildInfo* buildInfo)
{
    return new FakeStarCoin(buildInfo);
}


u32 FakeStarCoin::onCreate()
{
    rotationDirection = settings1 & 3;
    model = ModelWrapper::create("star_coin", (settings1 & 0x10) ? "star_coinB" : "star_coinA");

    aCollider.init(this, &FakeStarCoin::collisionInfo, 0);
    addActiveColliders();

    u32 movementMask = movementHandler.getMaskForMovementType(settings2 & 0xFF);
    movementHandler.link(position, movementMask, movementID);

    updateModel();

    return 1;
}

u32 FakeStarCoin::onExecute()
{
    static const u32 rotationSpeed = 0x3FD27D2;

    movementHandler.execute();
    position = movementHandler.position;

    switch (rotationDirection)
    {
        case 0: rotation.y -= rotationSpeed; break;
        case 1: rotation.y += rotationSpeed; break;
        case 2: rotation.x += rotationSpeed; break;
        case 3: rotation.x -= rotationSpeed; break;
    }

    updateModel();

    return 1;
}

u32 FakeStarCoin::onDraw()
{
    DrawMgr::instance->drawModel(model);
    return 1;
}


void FakeStarCoin::updateModel()
{
    model->updateAnimations();

    Mtx34 mtx;
    mtx.rotateAndTranslate(rotation, position);

    model->setMtx(mtx);
    model->updateModel();
}

void FakeStarCoin::collect()
{
    Vec3 effectPos = { position.x, position.y - 18.0f, 4500.0f };

    Effect::spawn(921, &effectPos, nullptr, nullptr);
    PlaySound("SE_OBJ_DDOOR_OPEN", position);

    if (eventID1)
        EventMgr::instance->set(eventID1-1, 0, true);

    isDeleted = true;
}


void FakeStarCoin::collisionCallback(ActiveCollider* acSelf, ActiveCollider* acOther)
{
    reinterpret_cast<FakeStarCoin*>(acSelf->owner)->collect();
}
