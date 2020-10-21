#ifndef _HAVOK_C_HKPHYSICS_H_
#define _HAVOK_C_HKPHYSICS_H_ 1

#include "hkcommon.h"

#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct hkpOpaqueWorld *hkpWorldRef;
	typedef struct hkpOpaqueCollisionDispatcher *hkpCollisionDispatcherRef;
	typedef struct hkpOpaqueShape *hkpShapeRef;
	typedef struct hkpOpaqueEntity *hkpEntityRef;

	HAVOK_C_API extern int8_t hkpWorldCinfo_SIMULATION_TYPE_INVALID;
	HAVOK_C_API extern int8_t hkpWorldCinfo_SIMULATION_TYPE_DISCRETE;
	HAVOK_C_API extern int8_t hkpWorldCinfo_SIMULATION_TYPE_CONTINUOUS;
	HAVOK_C_API extern int8_t hkpWorldCinfo_SIMULATION_TYPE_MULTITHREADED;

	HAVOK_C_API extern int8_t hkpWorldCinfo_BROADPHASE_BORDER_ASSERT;
	HAVOK_C_API extern int8_t hkpWorldCinfo_BROADPHASE_BORDER_FIX_ENTITY;
	HAVOK_C_API extern int8_t hkpWorldCinfo_BROADPHASE_BORDER_REMOVE_ENTITY;
	HAVOK_C_API extern int8_t hkpWorldCinfo_BROADPHASE_BORDER_DO_NOTHING;

	HAVOK_C_API extern int8_t hkpEntityActivation_HK_ENTITY_ACTIVATION_DO_NOT_ACTIVATE;
	HAVOK_C_API extern int8_t hkpEntityActivation_HK_ENTITY_ACTIVATION_DO_ACTIVATE;

	HAVOK_C_API extern float hkpConvexShape_hkConvexShapeDefaultRadius;

	HAVOK_C_API extern int8_t hkpMotion_MOTION_INVALID;
	HAVOK_C_API extern int8_t hkpMotion_MOTION_DYNAMIC;
	HAVOK_C_API extern int8_t hkpMotion_MOTION_SPHERE_INERTIA;
	HAVOK_C_API extern int8_t hkpMotion_MOTION_BOX_INERTIA;
	HAVOK_C_API extern int8_t hkpMotion_MOTION_KEYFRAMED;
	HAVOK_C_API extern int8_t hkpMotion_MOTION_FIXED;
	HAVOK_C_API extern int8_t hkpMotion_MOTION_THIN_BOX_INERTIA;
	HAVOK_C_API extern int8_t hkpMotion_MOTION_CHARACTER;
	HAVOK_C_API extern int8_t hkpMotion_MOTION_MAX_ID;

	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_INVALID;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_FIXED;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_KEYFRAMED;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_DEBRIS;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_DEBRIS_SIMPLE_TOI;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_MOVING;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_CRITICAL;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_BULLET;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_USER;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_CHARACTER;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING;
	HAVOK_C_API extern int8_t hkpCollidableQualityType_HK_COLLIDABLE_QUALITY_MAX;

	HAVOK_C_API extern int8_t hkpStepResult_HK_STEP_RESULT_SUCCESS;
	HAVOK_C_API extern int8_t hkpStepResult_HK_STEP_RESULT_MEMORY_FAILURE_BEFORE_INTEGRATION;
	HAVOK_C_API extern int8_t hkpStepResult_HK_STEP_RESULT_MEMORY_FAILURE_DURING_COLLIDE;
	HAVOK_C_API extern int8_t hkpStepResult_HK_STEP_RESULT_MEMORY_FAILURE_DURING_TOI_SOLVE;

	HAVOK_C_API void HAVOK_C_CALL hkpWorld_registerWithJobQueue(hkJobQueueRef jobQueue);

	HAVOK_C_API hkpWorldRef HAVOK_C_CALL hkpWorld_new(int8_t simulationType, int8_t broadPhaseBorderBehaviour);

	HAVOK_C_API void HAVOK_C_CALL hkpWorld_markForWrite(hkpWorldRef physicsWorld);

	HAVOK_C_API void HAVOK_C_CALL hkpWorld_unmarkForWrite(hkpWorldRef physicsWorld);

	HAVOK_C_API hkpCollisionDispatcherRef HAVOK_C_CALL hkpWorld_getCollisionDispatcher(hkpWorldRef physicsWorld);

	HAVOK_C_API hkpEntityRef HAVOK_C_CALL hkpWorld_addEntity(hkpWorldRef physicsWorld, hkpEntityRef entity, int8_t initialActivationState);

	HAVOK_C_API int8_t HAVOK_C_CALL hkpWorld_initMtStep(hkpWorldRef physicsWorld, hkJobQueueRef jobQueue, float physicsDeltaTime);

	HAVOK_C_API int8_t HAVOK_C_CALL hkpWorld_finishMtStep(hkpWorldRef physicsWorld);

	HAVOK_C_API void HAVOK_C_CALL hkpWorld_removeReference(hkpWorldRef physicsWorld);

	HAVOK_C_API void HAVOK_C_CALL hkpAgentRegisterUtil_registerAllAgents(hkpCollisionDispatcherRef dis);

	HAVOK_C_API hkpShapeRef HAVOK_C_CALL hkpBoxShape_new(float halfExtents_X, float halfExtents_Y, float halfExtents_Z, float radius);

	HAVOK_C_API hkpShapeRef HAVOK_C_CALL hkpSphereShape_new(float radius);

	HAVOK_C_API void HAVOK_C_CALL hkpShape_removeReference(hkpShapeRef shape);

	HAVOK_C_API hkpEntityRef HAVOK_C_CALL hkpRigidBody_new(
		hkpShapeRef shape,
		float position_X, float position_Y, float position_Z,
		float mass,
		int8_t motionType,
		int8_t qualityType);

	HAVOK_C_API void HAVOK_C_CALL hkpRigidBody_getposition(hkpEntityRef entity, float *position_X, float *position_Y, float *position_Z);
	
	HAVOK_C_API void HAVOK_C_CALL hkpEntity_removeReference(hkpEntityRef entity);

	HAVOK_C_API void HAVOK_C_CALL hkpPhysicsContext_registerAllPhysicsProcesses();

	HAVOK_C_API hkProcessContextRef HAVOK_C_CALL hkpPhysicsContext_new();

	HAVOK_C_API void HAVOK_C_CALL hkpPhysicsContext_addWorld(hkProcessContextRef context, hkpWorldRef physicsWorld);

	HAVOK_C_API void HAVOK_C_CALL hkpPhysicsContext_removeReference(hkProcessContextRef context);
#ifdef __cplusplus
}
#endif

#endif