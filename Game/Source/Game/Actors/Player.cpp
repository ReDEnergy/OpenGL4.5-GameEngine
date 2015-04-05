#include "Player.h"

#include <include/utils.h>

#include <Component/Transform.h>

#include <Game/Input/PlayerInput.h>

#include <Manager/Manager.h>

// #include <Manager/HavokCore.h>
// #include <Component/Physics.h>

/*
// character rigid body
#include <Physics2012/Dynamics/Entity/hkpEntity.h>
#include <Physics2012/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>
#include <Physics2012/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBodyListener.h>
// state machine
#include <Physics2012/Utilities/CharacterControl/StateMachine/hkpDefaultCharacterStates.h>

// constrains
#include <Physics2012/Dynamics/Constraint/ConstraintKit/hkpConstraintConstructionKit.h>
#include <Physics/Constraint/Data/PointToPlane/hkpPointToPlaneConstraintData.h>

// Capsule shape
#include <Physics2012/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>

*/

Player::Player(const GameObject &gameObject)
	: GameObject(gameObject)
{
	moveSpeed = 30;
	wantToJump = false;

	// TODO: move it outside
	input = new PlayerInput(this);

	/*
	Manager::GetHavok()->MarkForWrite();

	//	Create a character rigid body object
	{
		// Construct a shape
		hkVector4 top(0, 0.60f, 0);
		hkVector4 bottom(0, -0.15f, 0);

		// Create a capsule to represent the character standing
		hkpCapsuleShape *shape = new hkpCapsuleShape(bottom, top, 0.375f);

		hkpCharacterRigidBodyCinfo info;
		info.m_mass = 50.0f;
		info.m_shape = shape;
		info.m_position = hkVector4f(-3.0f, 0, 0, 0);
		info.m_maxForce = 150.0f;
		info.m_up = hkVector4(0, 1, 0);
		// info.m_rotation = hkQuaternion(0, 1, 1, 0);
		info.m_maxSlope = 50.0f * HK_REAL_DEG_TO_RAD;
		info.m_supportDistance = 0.12f;
        info.m_hardSupportDistance = 0.2f;

		characterRigidBody = new hkpCharacterRigidBody(info);
		{
			hkpCharacterRigidBodyListener* listener = new hkpCharacterRigidBodyListener();
			characterRigidBody->setListener( listener );
			listener->removeReference();
		}
		Manager::GetHavok()->_pWorld->addEntity(characterRigidBody->getRigidBody());

		this->physics = new Physics(this);
		this->physics->body = characterRigidBody->getRigidBody();
	}


	// Create the Character state machine and context
	{
		hkpCharacterState* state;
		hkpCharacterStateManager* manager = new hkpCharacterStateManager();

		state = new hkpCharacterStateOnGround();
		manager->registerState( state,	HK_CHARACTER_ON_GROUND);
		state->removeReference();

		state = new hkpCharacterStateInAir();
		manager->registerState( state,	HK_CHARACTER_IN_AIR);
		state->removeReference();

		state = new hkpCharacterStateJumping();
		manager->registerState( state,	HK_CHARACTER_JUMPING);
		state->removeReference();

		state = new hkpCharacterStateClimbing();
		manager->registerState( state,	HK_CHARACTER_CLIMBING);
		state->removeReference();

		characterContext = new hkpCharacterContext( manager, HK_CHARACTER_ON_GROUND );
		manager->removeReference();

		// Set character type
		characterContext->setCharacterType(hkpCharacterContext::HK_CHARACTER_RIGIDBODY);
	}

	// Constrain to limit movement to XOY plane
	{
		hkpPointToPlaneConstraintData* plane = new hkpPointToPlaneConstraintData();
		hkVector4 zdir(0, 0, 1);

		// Create constraint
		hkTransform b1 = this->physics->body->getTransform();
		hkTransform b2;
		b2.setIdentity();
		hkVector4 center = this->physics->body->getCenterOfMassInWorld();
		plane->setInWorldSpace(b1, b2, center, zdir);

		//	Create and add the constraint
		{
			hkpConstraintInstance* constraint = new hkpConstraintInstance(this->physics->body, NULL, plane );
			Manager::GetHavok()->_pWorld->addConstraint(constraint);
			constraint->removeReference();
		}		

		plane->removeReference();  
	}

	// Restrict rotation to Y axis
	{
		//hkMatrix3 invInertiaMatrix;
		//this->physics->body->getInertiaInvLocal(invInertiaMatrix);
		//this->physics->body->setInertiaInvLocal(invInertiaMatrix);

	}

	Manager::GetHavok()->UnmarkForWrite();
	*/

}

Player::~Player() {
	SAFE_FREE(input);
}

void Player::Update(float deltaTime) {

	/*

	HK_TIMER_BEGIN( "set character state", HK_NULL );

	//	Get user input data

	hkpCharacterInput pInput;
	hkpCharacterOutput output;

	Manager::GetHavok()->MarkForWrite();

	pInput.m_up.set(0, 1, 0);
	pInput.m_forward.set(1, 0, 0);
	pInput.m_inputLR = 0;
	pInput.m_inputUD = deltaMove;
	pInput.m_atLadder = false;
	pInput.m_wantJump = wantToJump;
	pInput.m_characterGravity.set(0, -16.0f, 0);
	pInput.m_velocity = characterRigidBody->getRigidBody()->getLinearVelocity();
	pInput.m_position = characterRigidBody->getRigidBody()->getPosition();

	hkStepInfo stepInfo;
	stepInfo.m_deltaTime = deltaTime;
	stepInfo.m_invDeltaTime = 1.0f / deltaTime;
			
	pInput.m_stepInfo = stepInfo;

	characterRigidBody->checkSupport(stepInfo, pInput.m_surfaceInfo);

	characterContext->update(pInput, output);
	characterRigidBody->setLinearVelocity(output.m_velocity, deltaTime);

	Manager::GetHavok()->UnmarkForWrite();

	// --- Display state --- //
	{
		hkpCharacterStateType state = characterContext->getState();
		const char * stateStr;

		switch (state)
		{
			case HK_CHARACTER_ON_GROUND:
				stateStr = "On Ground";	break;
			case HK_CHARACTER_JUMPING:
				stateStr = "Jumping"; break;
			case HK_CHARACTER_IN_AIR:
				stateStr = "In Air"; break;
			case HK_CHARACTER_CLIMBING:
				stateStr = "Climbing"; break;
			default:
				stateStr = "Other";	break;
		}
		//printf("State : %s\n", stateStr);
	}

	float p[4];
	hkVector4 pos = characterRigidBody->getPosition();
	memcpy(p, &pos, 4 * sizeof(float));
	transform->position = glm::vec3(p[0], p[1], p[2]);
	transform->Update();

	ResetMotion();
	*/
};

void Player::ResetMotion() {
	wantToJump = false;
	deltaMove = 0;
}

void Player::Jump() {
	wantToJump = true;
}

void Player::MoveLeft(float deltaTime) {
	deltaMove = moveSpeed * deltaTime;
}

void Player::MoveRight(float deltaTime) {
	deltaMove = -moveSpeed * deltaTime;
}