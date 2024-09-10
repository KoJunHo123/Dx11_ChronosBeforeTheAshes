#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CModel;
class CShader;
END

// 그냥 장애물 전용
BEGIN(Client)
class CObstacle final : public CGameObject
{

};
END
