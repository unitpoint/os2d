/******************************************************************************
* Copyright (C) 2014 Evgeniy Golovin (evgeniy.golovin@unitpoint.ru)
*
* Please feel free to contact me at anytime, 
* my email is evgeniy.golovin@unitpoint.ru, skype: egolovin
*
* Latest source code: https://github.com/unitpoint/os2d
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifndef __OX_PHYSICS_H__
#define __OX_PHYSICS_H__

#include "ox-binder.h"
#include "MathLib.h"
#include <Box2D\Box2D.h>

// =====================================================================

DECLARE_SMART(PhysObject, spPhysObject);
class PhysObject: public oxygine::Object
{
public:
	OS_DECLARE_CLASSINFO(PhysObject);

	PhysObject();
	virtual ~PhysObject();

	virtual int getId();
	virtual void destroy();

protected:
};

// =====================================================================

DECLARE_SMART(PhysWorld, spPhysWorld);
DECLARE_SMART(PhysFixtureDef, spPhysFixtureDef);
DECLARE_SMART(PhysFixture, spPhysFixture);
DECLARE_SMART(PhysBodyDef, spPhysBodyDef);
DECLARE_SMART(PhysBody, spPhysBody);
DECLARE_SMART(PhysJointDef, spPhysJointDef);
DECLARE_SMART(PhysJoint, spPhysJoint);
DECLARE_SMART(PhysContactCache, spPhysContactCache);

// =====================================================================

#define DEF_TO_PHYS_SCALE	(1.0f / 10.0f)
// #define PHYS_REGISTER_OBJECT

enum EPhysPreSolveContactType
{
	PHYS_CONTACT_ENABLED,
	PHYS_CONTACT_STEP_ENABLED,
	PHYS_CONTACT_STEP_DISABLED,
	PHYS_CONTACT_DISABLED,
	// internal
	PHYS_CONTACT_USED_AND_DISABLED,
};

class PhysWorld: public PhysObject, public b2DestructionListener, public b2ContactListener, public b2ContactFilter
{
public:
	OS_DECLARE_CLASSINFO(PhysWorld);

	PhysWorld();
	~PhysWorld();

	int getId();
	void destroy();
	b2World * getCore();

	spPhysBody createBody(spPhysBodyDef def);
	void destroyBody(spPhysBody);

	spPhysJoint createJoint(spPhysJointDef def);
	void destroyJoint(spPhysJoint);

	float getPersistentDeltaTime() const;
	void setPersistentDeltaTime(float);

	void update(float timeStep, int velocityIterations, int positionIterations);

	// void ClearForces();
	// void QueryAABB(b2QueryCallback* callback, const b2AABB& aabb) const;
	// void RayCast(b2RayCastCallback* callback, const b2Vec2& point1, const b2Vec2& point2) const;
	// b2Body* GetBodyList();
	// b2Joint* GetJointList();
	// b2Contact* GetContactList();

	vec2 getGravity() const;
	void setGravity(const vec2& gravity);

	static float getToPhysScale();
	static void setToPhysScale(float);

	static float toPhysValue(float a);
	static float fromPhysValue(float a);
	static b2Vec2 toPhysVec(const vec2 &pos);
	static vec2 fromPhysVec(const b2Vec2 &pos);

	void pushBodyList(ObjectScript::OS*);
	void pushJointList(ObjectScript::OS*);

	spPhysFixture getFixture(b2Fixture * coreFixture);
	spPhysBody getBody(b2Body * coreBody);
	spPhysJoint getJoint(b2Joint * coreJoint);

protected:

	friend class PhysFixture;
	friend class PhysBody;
	friend class PhysJoint;
	friend class PhysContactCache;

	struct ContactCache
	{
		enum EType 
		{
			PRESOLVE,
			BEGIN,
			END,
		};

		b2WorldManifold worldManifold;
		spPhysFixture fixtures[2];
		spPhysBody bodies[2];
		int numPoints;
		b2Vec2 contactPoint;
		float contactSeparation;
		EType type;
		int id;

		ContactCache(){}
		ContactCache(EType _type){ type = _type; }
	};

	b2World * core;
	float accumTime;
	float persistentDeltaTime;
	bool updateInProgress;

	static float toPhysScale;

	std::vector<b2Body*> waitBodyListToDestroy;
	// std::vector<b2Joint*> waitJointListToDestroy;
#ifdef PHYS_REGISTER_OBJECT
	std::vector<spPhysBody> bodyList;
	std::vector<spPhysJoint> jointList;
#endif

	std::map<int, EPhysPreSolveContactType> preSolveContactTypeMap;
	std::vector<ContactCache> contactCacheList;
	spPhysContactCache physContactCache;

	void initContactCache(ContactCache& contactCache, b2Contact*);

#ifdef PHYS_REGISTER_OBJECT
	void registerBody(spPhysBody body);
	void unregisterBody(spPhysBody body);

	void registerJoint(spPhysJoint joint);
	void unregisterJoint(spPhysJoint joint);
#endif

	void destroyAllBodies();
	void destroyAllJoints();

	void dispatchContacts();
	EPhysPreSolveContactType testPreSolveContact(ContactCache&);

	/// Called when two fixtures begin to touch.
	void BeginContact(b2Contact* contact); // override b2ContactListener

	/// Called when two fixtures cease to touch.
	void EndContact(b2Contact* contact); // override b2ContactListener

	/// This is called after a contact is updated. This allows you to inspect a
	/// contact before it goes to the solver. If you are careful, you can modify the
	/// contact manifold (e.g. disable contact).
	/// A copy of the old manifold is provided so that you can detect changes.
	/// Note: this is called only for awake bodies.
	/// Note: this is called even when the number of contact points is zero.
	/// Note: this is not called for sensors.
	/// Note: if you set the number of contact points to zero, you will not
	/// get an EndContact callback. However, you may get a BeginContact callback
	/// the next step.
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

	/// Called when any joint is about to be destroyed due
	/// to the destruction of one of its attached bodies.
	void SayGoodbye(b2Joint* joint); // override b2DestructionListener

	/// Called when any fixture is about to be destroyed due
	/// to the destruction of its parent body.
	void SayGoodbye(b2Fixture* fixture); // override b2DestructionListener

	/// Return true if contact calculations should be performed between these two shapes.
	/// @warning for performance reasons this is only called when the AABBs begin to overlap.
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
};

// =====================================================================

class PhysContactCache: public PhysObject
{
public:
	OS_DECLARE_CLASSINFO(PhysContactCache);

	PhysWorld::ContactCache * contactCache;

	PhysContactCache();
	~PhysContactCache();

	spPhysFixture getFixture(int i);
	spPhysBody getBody(int i);

	int getId();
	vec2 getNormal();
	int getNumPoints();
	vec2 getPoint();
	vec2 getPointA();
	vec2 getPointB();
	float getSeparation();
	float getSeparationA();
	float getSeparationB();
};

// =====================================================================

class PhysFixtureDef: public PhysObject
{
public:
	OS_DECLARE_CLASSINFO(PhysFixtureDef);

	b2CircleShape circleShape;
	b2EdgeShape edgeShape;
	b2PolygonShape polygonShape;
	b2ChainShape chainShape;

	b2FixtureDef def;

	PhysFixtureDef();
	~PhysFixtureDef();

	b2Shape::Type getType() const { return def.shape->m_type; }
	void setType(b2Shape::Type value);

	float getFriction() const { return def.friction; }
	void setFriction(float value){ def.friction = value; }

	float getRestitution() const { return def.restitution; }
	void setRestitution(float value){ def.restitution = value; }

	float getDensity() const { return def.density; }
	void setDensity(float value){ def.density = value; }

	bool getIsSensor() const { return def.isSensor; }
	void setIsSensor(bool value){ def.isSensor = value; }

	uint16 getCategoryBits() const { return def.filter.categoryBits; }
	void setCategoryBits(uint16 value){ def.filter.categoryBits = value; }

	uint16 getMaskBits() const { return def.filter.maskBits; }
	void setMaskBits(uint16 value){ def.filter.maskBits = value; }

	uint16 getGroupIndex() const { return def.filter.groupIndex; }
	void setGroupIndex(uint16 value){ def.filter.groupIndex = value; }

	float getCircleRadius() const;
	void setCircleRadius(float value);

	vec2 getCirclePos() const;
	void setCirclePos(const vec2& value);

	void setPolygonPointsInPhysScale(const std::vector<b2Vec2>& points);
	void setPolygonPoints(const std::vector<vec2>& points);
	void setPolygonAsBox(const vec2& halfSize, const vec2& center, float angle);
	void setPolygonAsBox(const vec2& halfSize);
	void setPolygonAsBounds(const vec2& a, const vec2& b, float angle);

	void setEdgePoints(const vec2& p1, const vec2& p2);

	void setChainPointsInPhysScale(const std::vector<b2Vec2>& points);
	void setChainLoopPointsInPhysScale(const std::vector<b2Vec2>& points);
	void setChainPrevPoint(const vec2& p);
	void setChainNextPoint(const vec2& p);

	static void readPointArrayInPhysScale(std::vector<b2Vec2>& points, ObjectScript::OS * os, int offs);
};

// =====================================================================

class PhysFixture: public PhysObject
{
public:
	OS_DECLARE_CLASSINFO(PhysFixture);

	PhysFixture(PhysWorld * world, b2Fixture * fixture);
	~PhysFixture();

	int getId();
	void destroy();
	b2Fixture * getCore();

	b2Shape::Type getType() const;
	// void setType(b2Shape::Type value);

	float getFriction() const;
	void setFriction(float value);

	float getRestitution() const;
	void setRestitution(float value);

	float getDensity() const;
	void setDensity(float value);

	bool getIsSensor() const;
	void setIsSensor(bool value);

	uint16 getCategoryBits() const;
	void setCategoryBits(uint16 value);

	uint16 getMaskBits() const;
	void setMaskBits(uint16 value);

	uint16 getGroupIndex() const;
	void setGroupIndex(uint16 value);

	void refilter();

	spPhysBody getBody() const;
	spPhysFixture getNext() const;

	// bool TestPoint(const b2Vec2& p) const;
	// bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input, int32 childIndex) const;
	// void GetMassData(b2MassData* massData) const;
	// const b2AABB& GetAABB(int32 childIndex) const;

protected:

	friend class PhysWorld;
	friend class PhysBody;

	PhysWorld * world;
	b2Fixture * core;

	void unlink();
};

// =====================================================================

class PhysBodyDef: public PhysObject
{
public:
	OS_DECLARE_CLASSINFO(PhysBodyDef);

	b2BodyDef def;

	PhysBodyDef();
	~PhysBodyDef();

	b2BodyType getType() const { return def.type; }
	void setType(b2BodyType value){ def.type = value; }

	vec2 getPos() const { return PhysWorld::fromPhysVec(def.position); }
	void setPos(const vec2& value){ def.position = PhysWorld::toPhysVec(value); }

	float getAngle() const { return def.angle * MathLib::RAD2DEG; }
	void setAngle(float value){	def.angle = value * MathLib::DEG2RAD; }

	vec2 getLinearVelocity() const { return PhysWorld::fromPhysVec(def.linearVelocity); }
	void setLinearVelocity(const vec2& value){ def.linearVelocity = PhysWorld::toPhysVec(value); }

	float getAngularVelocity() const { return def.angularVelocity * MathLib::RAD2DEG; }
	void setAngularVelocity(float value){ def.angularVelocity = value * MathLib::DEG2RAD; }

	float getLinearDamping() const { return def.linearDamping; }
	void setLinearDamping(float value){ def.linearDamping = value; }

	float getAngularDamping() const { return def.angularDamping; }
	void setAngularDamping(float value){ def.angularDamping = value; }

	bool getIsSleepingAllowed() const { return def.allowSleep; }
	void setIsSleepingAllowed(bool value){ def.allowSleep = value; }

	bool getIsAwake() const { return def.awake; }
	void setIsAwake(bool value){ def.awake = value; }

	bool getIsFixedRotation() const { return def.fixedRotation; }
	void setIsFixedRotation(bool value){ def.fixedRotation = value; }

	bool getIsBullet() const { return def.bullet; }
	void setIsBullet(bool value){ def.bullet = value; }

	bool getIsActive() const { return def.active; }
	void setIsActive(bool value){ def.active = value; }

	float getGravityScale() const { return def.gravityScale; }
	void setGravityScale(float value){ def.gravityScale = value; }
};

class PhysBody: public PhysObject
{
public:
	OS_DECLARE_CLASSINFO(PhysBody);

	PhysBody(PhysWorld * world, b2Body * body);
	~PhysBody();

	int getId();
	void destroy();
	b2Body * getCore();

	spPhysFixture createFixture(spPhysFixtureDef def);
	void destroyFixture(spPhysFixture);

	void pushFixtureList(ObjectScript::OS * os);

	void setTransform(const vec2& pos, float angle);

	vec2 getPos() const;
	void setPos(const vec2&);

	float getAngle() const;
	void setAngle(float);

	vec2 getWorldCenter() const;
	vec2 getLocalCenter() const;

	vec2 getLinearVelocity() const;
	void setLinearVelocity(const vec2&);

	float getAngularVelocity() const;
	void setAngularVelocity(float);

	void applyForce(const vec2& force, const vec2& point, bool wake);
	void applyForceToCenter(const vec2& force, bool wake);
	void applyTorque(float torque, bool wake);
	void applyLinearImpulse(const vec2& impulse, const vec2& point, bool wake);
	void applyAngularImpulse(float impulse, bool wake);

	float getMass() const;
	float getInertia() const;

	// void GetMassData(b2MassData* data) const;
	// void SetMassData(const b2MassData* data);
	void resetMassData();

	vec2 getWorldPoint(const vec2& localPoint) const;
	vec2 getWorldVector(const vec2& localVector) const;

	vec2 getLocalPoint(const vec2& worldPoint) const;
	vec2 getLocalVector(const vec2& worldVector) const;

	vec2 getLinearVelocityFromWorldPoint(const vec2& worldPoint) const;
	vec2 getLinearVelocityFromLocalPoint(const vec2& localPoint) const;

	float getLinearDamping() const;
	void setLinearDamping(float linearDamping);

	float getAngularDamping() const;
	void setAngularDamping(float angularDamping);

	float getGravityScale() const;
	void setGravityScale(float scale);

	b2BodyType getType() const;
	void setType(b2BodyType type);

	bool getIsBullet() const;
	void setIsBullet(bool flag);

	bool getIsSleepingAllowed() const;
	void setIsSleepingAllowed(bool flag);

	bool getIsAwake() const;
	void setIsAwake(bool flag);

	bool getIsActive() const;
	void setIsActive(bool flag);

	bool getIsFixedRotation() const;
	void setIsFixedRotation(bool flag);

	// b2JointEdge* GetJointList();
	// b2ContactEdge* GetContactList();
	// b2Body* GetNext();
	// b2World* GetWorld();

protected:

	friend class PhysWorld;
	friend class PhysFixture;

	PhysWorld * world;
	b2Body * core;

#ifdef PHYS_REGISTER_OBJECT
	std::vector<spPhysFixture> fixtureList;
#endif
	// std::vector<b2Fixture*> waitFixtureListToDestroy;

	void unlink();

#ifdef PHYS_REGISTER_OBJECT
	void registerFixture(spPhysFixture);
	void unregisterFixture(spPhysFixture);
#endif
};

// =====================================================================

class PhysJointDef: public PhysObject
{
public:
	OS_DECLARE_CLASSINFO(PhysJointDef);

	b2JointDef * pdef;
	spPhysBody bodies[2];

	PhysJointDef();
	~PhysJointDef();

	b2JointType getType() const { OX_ASSERT(pdef); return pdef->type; }
	void setType(b2JointType value){ OX_ASSERT(pdef); pdef->type = value; }

	spPhysBody getBodyA() const;
	void setBodyA(spPhysBody);

	spPhysBody getBodyB() const;
	void setBodyB(spPhysBody);

	bool getCollideConnected() const { OX_ASSERT(pdef); return pdef->collideConnected; }
	void setCollideConnected(bool value){ OX_ASSERT(pdef); pdef->collideConnected = value; }
};

class PhysJoint: public PhysObject
{
public:
	OS_DECLARE_CLASSINFO(PhysBody);

	PhysJoint(PhysWorld * world, b2Joint * joint);
	~PhysJoint();

	int getId();
	void destroy();
	b2Joint * getCore();

protected:

	friend class PhysWorld;

	PhysWorld * world;
	b2Joint * core;

	void unlink();
};

// =====================================================================
// =====================================================================
// =====================================================================

namespace ObjectScript {

OS_DECL_CTYPE_NAME(vec2, "vec2");
template <> struct CtypeValue<vec2>: public CtypeValuePoint<vec2> {};

OS_DECL_CTYPE_NAME(b2Vec2, "vec2");
template <> struct CtypeValue<b2Vec2>: public CtypeValuePointOf<b2Vec2, float32> {};

// =====================================================================

OS_DECL_CTYPE_NAME(b2AABB, "PhysAABB");

template <>
struct CtypeValue<b2AABB>
{
	typedef b2AABB type;

	static bool isValid(const type&){ return true; }

	static type def(ObjectScript::OS * os){ type aabb; aabb.lowerBound = aabb.upperBound = b2Vec2(0, 0); return aabb; }
	static type getArg(ObjectScript::OS * os, int offs)
	{
		if(os->isObject(offs)){
			type aabb;
			aabb.lowerBound = (os->getProperty(offs, "a"), PhysWorld::toPhysVec(CtypeValue<vec2>::getArg(os, -1))); os->pop();
			aabb.upperBound = (os->getProperty(offs, "b"), PhysWorld::toPhysVec(CtypeValue<vec2>::getArg(os, -1))); os->pop();
			return aabb;
		}
		os->setException("PhysAABB required");
		return type();
	}

	static void push(ObjectScript::OS * os, const type& aabb)
	{
		os->getGlobal("PhysAABB");
		OX_ASSERT(os->isObject());
		os->pushGlobals();
		pushCtypeValue(os, PhysWorld::fromPhysVec(aabb.lowerBound));
		pushCtypeValue(os, PhysWorld::fromPhysVec(aabb.upperBound));
		os->callFT(2, 1);
		os->handleException();
	}
};

// =====================================================================

OS_DECL_CTYPE_ENUM(b2BodyType);
OS_DECL_CTYPE_ENUM(b2JointType);
OS_DECL_CTYPE_ENUM(b2Shape::Type);

// =====================================================================

OS_DECL_OX_CLASS(PhysObject);
OS_DECL_OX_CLASS(PhysWorld);
OS_DECL_OX_CLASS(PhysFixtureDef);
OS_DECL_OX_CLASS(PhysFixture);
OS_DECL_OX_CLASS(PhysBodyDef);
OS_DECL_OX_CLASS(PhysBody);
OS_DECL_OX_CLASS(PhysJointDef);
OS_DECL_OX_CLASS(PhysJoint);
OS_DECL_OX_CLASS(PhysContactCache);

// #define DEF_GETTER(name, type, prop)

} // namespace ObjectScript

#endif // __OX_PHYSICS_H__
