#pragma once

// External libs
#include <noob/component/component.hpp>
#include <btBulletDynamicsCommon.h>

// Internal project-related files
#include "Body.hpp"
#include "Shape.hpp"

namespace noob
{
	class joint
	{
		public:
			void init(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true);

			void set_frames(const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true);


			void set_enabled(uint8_t, bool) noexcept(true);
			void set_stiffness(uint8_t, float) noexcept(true);
			void set_damping(uint8_t, float) noexcept(true);
			void set_limits(uint8_t, const noob::vec2f&) noexcept(true);
		
			bool is_enabled(uint8_t) const noexcept(true);
			// TODO: Patch Bullet? Maybe good reason?
			// float get_stiffness(uint8_t) const noexcept(true);
			// float get_damping(uint8_t) const noexcept(true);
			// noob::vec2f get_limits(uint8_t) const noexcept(true);
			
			noob::vec3f get_axis(uint8_t) const noexcept(true);
			float get_angle(uint8_t) const noexcept(true);

		protected:
			btGeneric6DofSpringConstraint* inner;
			// btGeneric6DofConstraint (btRigidBody &rbA, btRigidBody &rbB, const btTransform &frameInA, const btTransform &frameInB, bool useLinearReferenceFrameA)
			// btGeneric6DofConstraint (btRigidBody &rbB, const btTransform &frameInB, bool useLinearReferenceFrameB)
			// void enableSpring (int index, bool onOff)
			// void setStiffness (int index, btScalar stiffness)
			// void setDamping (int index, btScalar damping)
			// void setEquilibriumPoint ()
			// void setEquilibriumPoint (int index)
			// void setEquilibriumPoint (int index, btScalar val)
			// virtual void setAxis (const btVector3 &axis1, const btVector3 &axis2)
			// void calculateTransforms(const btTransform &transA, const btTransform &transB)
			// void calculateTransforms ()
			// const btTransform & getCalculatedTransformA () const
			// const btTransform& getCalculatedTransformB () const
			// const btTransform& getFrameOffsetA () const
			// const btTransform& getFrameOffsetB () const
			// btTransform& getFrameOffsetA ()
			// btTransform& getFrameOffsetB ()
			// virtual void buildJacobian ()
			// btVector3 getAxis (int axis_index) const
			// btScalar getAngle (int axis_index) const
			// btScalar getRelativePivotPosition(int axis_index) const // Get the relative position of the constraint pivot. 
			// void setFrames(const btTransform &frameA, const btTransform &frameB)
			// bool testAngularLimitMotor(int axis_index)
			// void setLinearLowerLimit(const btVector3 &linearLower)
			// void getLinearLowerLimit(btVector3 &linearLower)
			// void setLinearUpperLimit(const btVector3 &linearUpper)
			// void getLinearUpperLimit(btVector3 &linearUpper)
			// void setAngularLowerLimit(const btVector3 &angularLower)
			// void getAngularLowerLimit(btVector3 &angularLower)
			// void setAngularUpperLimit(const btVector3 &angularUpper)
			// void getAngularUpperLimit(btVector3 &angularUpper)
			// btRotationalLimitMotor* getRotationalLimitMotor(int index) // Retrieves the angular limit informacion.
			// btTranslationalLimitMotor * 	getTranslationalLimitMotor () // Retrieves the limit informacion. More...
			// void setLimit(int axis, btScalar lo, btScalar hi)
			// bool isLimited (int limitIndex)
			// virtual void calcAnchorPos (void)
			// bool getUseFrameOffset ()
			// void  setUseFrameOffset (bool frameOffsetOnOff)
			// virtual void setParam (int num, btScalar value, int axis=-1)	// Overrides the default global value of a parameter (such as ERP or CFM), optionally provide the axis (0..5). More...
			// virtual btScalar getParam (int num, int axis=-1) const // Returns the local value of parameter More...
			// void setAxis(const btVector3 &axis1, const btVector3 &axis2)
			// int getOverrideNumSolverIterations () const
			// void setOverrideNumSolverIterations (int overideNumIterations) // Overrides the number of constraint solver iterations used to solve this constraint -1 will use the default number of iterations, as specified in SolverInfo.m_numIterations.
			// btScalar getBreakingImpulseThreshold () const
			// void setBreakingImpulseThreshold (btScalar threshold)
			// bool isEnabled () const
			// void setEnabled (bool enabled)
			// const btRigidBody & getRigidBodyA () const
			// const btRigidBody & getRigidBodyB () const
			// btRigidBody & getRigidBodyA ()
			// btRigidBody & getRigidBodyB ()
			// int getUserConstraintType () const
			// void setUserConstraintType (int userConstraintType)
			// void setUserConstraintId (int uid)
			// int getUserConstraintId () const
			// void setUserConstraintPtr (void *ptr)
			// void * getUserConstraintPtr ()
			// void setJointFeedback (btJointFeedback *jointFeedback)
			// const btJointFeedback *	getJointFeedback () const
			// btJointFeedback * getJointFeedback ()
			// int getUid () const
			// bool needsFeedback () const
			// void  enableFeedback (bool needsFeedback) // RnableFeedback will allow to read the applied linear and angular impulse use getAppliedImpulse, getAppliedLinearImpulse and getAppliedAngularImpulse to read feedback information.
			// btScalar getAppliedImpulse () const
			// getAppliedImpulse is an estimated total applied impulse. More...

	};

	typedef noob::handle<noob::joint> joint_handle;
}
