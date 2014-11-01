#include "ox-physics.h"

namespace ObjectScript {

// =====================================================================

static void registerPhysGlobal(OS * os)
{
	struct Lib {
	};

	OS::FuncDef funcs[] = {
		{}
	};
	OS::NumberDef nums[] = {
		{"PHYS_BODY_STATIC", b2_staticBody},
		{"PHYS_BODY_KINEMATIC", b2_kinematicBody},
		{"PHYS_BODY_DYNAMIC", b2_dynamicBody},

		{"PHYS_SHAPE_CIRCLE", b2Shape::e_circle},
		{"PHYS_SHAPE_EDGE", b2Shape::e_edge},
		{"PHYS_SHAPE_POLYGON", b2Shape::e_polygon},
		{"PHYS_SHAPE_CHAIN", b2Shape::e_chain},
		{}
	};
	os->pushGlobals();
	os->setFuncs(funcs);
	os->setNumbers(nums);
	os->pop();
}
static bool __registerPhysGlobal = addRegFunc(registerPhysGlobal);

// =====================================================================

static void registerPhysObject(OS * os)
{
	struct Lib
	{
	};
	OS::FuncDef funcs[] = {
		// def("__newinstance", &Lib::__newinstance),
		def("destroy", &PhysObject::destroy),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<PhysObject, oxygine::Object>(os, funcs, nums, false OS_DBG_FILEPOS);
}
static bool __registerPhysObject = addRegFunc(registerPhysObject);

// =====================================================================

static void registerPhysWorld(OS * os)
{
	struct Lib
	{
		static PhysWorld * __newinstance()
		{
			return new PhysWorld();
		}

		static int getToPhysScale(OS * os, int params, int, int, void*)
		{
			os->pushNumber(PhysWorld::getToPhysScale());
			return 1;
		}

		static int setToPhysScale(OS * os, int params, int, int, void*)
		{
			float scale = os->toFloat(-params+0);
			if(scale <= 0.0f){
				os->setException("PhysWorld::toPhysScale must be greater 0");
				return 0;
			}
			PhysWorld::setToPhysScale(scale);
			return 0;
		}
		
		static int getBodyList(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(PhysWorld*);
			self->pushBodyList(os);
			return 1;
		}
		
		static int getJointList(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(PhysWorld*);
			self->pushJointList(os);
			return 1;
		}

		static int queryAABB(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(PhysWorld*);
			if(params != 2){
				os->setException("2 arguments required");
				return 0;
			}
			if(!os->isFunction(-params+1)){
				os->setException("function required for third argument");
				return 0;
			}
			int funcId = os->getValueId(-params+1);

			b2AABB aabb = CtypeValue<b2AABB>::getArg(os, -params+0);
			if(os->isExceptionSet()){
				return 0;
			}
			
			class QueryCallback: public b2QueryCallback
			{
			public:

				OS * os;
				PhysWorld * physWorld;
				int funcId;

				QueryCallback(OS * _os, PhysWorld * _physWorld, int _funcId)
				{
					os = _os;
					physWorld = _physWorld;
					funcId = _funcId;
				}

				/// Called for each fixture found in the query AABB.
				/// @return false to terminate the query.
				virtual bool ReportFixture(b2Fixture* coreFixture)
				{
					spPhysBody body = dynamic_cast<PhysBody*>((PhysObject*)coreFixture->GetBody()->GetUserData());
					spPhysFixture fixture = dynamic_cast<PhysFixture*>((PhysObject*)coreFixture->GetUserData());
					if(body){
						os->pushValueById(funcId);
						OX_ASSERT(os->isFunction());
						pushCtypeValue(os, fixture);
						pushCtypeValue(os, body);
						os->callF(2, 1);
						bool result = os->getType() == OS_VALUE_TYPE_BOOL && !os->toBool() ? false : true;
						os->pop();
						os->handleException();
						return result;
					}
					return true;
				}
			};
			QueryCallback queryCallback(os, self, funcId);
			self->getCore()->QueryAABB(&queryCallback, aabb);
			return 0;
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		def("createBody", &PhysWorld::createBody),
		def("destroyBody", &PhysWorld::destroyBody),
		def("createJoint", &PhysWorld::createJoint),
		def("destroyJoint", &PhysWorld::destroyJoint),
		def("update", &PhysWorld::update),
		{"__get@toPhysScale", &Lib::getToPhysScale},
		{"__set@toPhysScale", &Lib::setToPhysScale},
		{"__get@bodyList", &Lib::getBodyList},
		{"__get@jointList", &Lib::getJointList},
		{"queryAABB", &Lib::queryAABB},
		DEF_PROP("gravity", PhysWorld, Gravity),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<PhysWorld, PhysObject>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerPhysWorld = addRegFunc(registerPhysWorld);

// =====================================================================

static void registerPhysFixtureDef(OS * os)
{
	struct Lib
	{
		static PhysFixtureDef * __newinstance()
		{
			return new PhysFixtureDef();
		}

		static int setPolygonPoints(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(PhysFixtureDef*);
			std::vector<b2Vec2> points;
			PhysFixtureDef::readPointArrayPhysScale(points, os, -params+0);
			self->setPolygonPointsPhysScale(points);
			return 0;
		}

		static int setPolygonAsBox(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(PhysFixtureDef*);
			if(params == 1){
				self->setPolygonAsBox(CtypeValue<vec2>::getArg(os, -params+0));
			}else if(params == 3){
				self->setPolygonAsBox(
					CtypeValue<vec2>::getArg(os, -params+0),
					CtypeValue<vec2>::getArg(os, -params+1),
					os->toFloat(-params+2));
			}else{
				os->setException("1 or 3 arguments required");
			}
			return 0;
		}

		static int setChainPoints(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(PhysFixtureDef*);
			std::vector<b2Vec2> points;
			PhysFixtureDef::readPointArrayPhysScale(points, os, -params+0);
			self->setChainPointsPhysScale(points);
			return 0;
		}

		static int setChainLoopPoints(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(PhysFixtureDef*);
			std::vector<b2Vec2> points;
			PhysFixtureDef::readPointArrayPhysScale(points, os, -params+0);
			self->setChainLoopPointsPhysScale(points);
			return 0;
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_PROP("type", PhysFixtureDef, Type),
		DEF_PROP("friction", PhysFixtureDef, Friction),
		DEF_PROP("restitution", PhysFixtureDef, Restitution),
		DEF_PROP("density", PhysFixtureDef, Density),
		DEF_PROP("isSensor", PhysFixtureDef, IsSensor),
		DEF_PROP("categoryBits", PhysFixtureDef, CategoryBits),
		DEF_PROP("maskBits", PhysFixtureDef, MaskBits),
		DEF_PROP("groupIndex", PhysFixtureDef, GroupIndex),
		DEF_PROP("circleRadius", PhysFixtureDef, CircleRadius),
		DEF_PROP("circlePos", PhysFixtureDef, CirclePos),
		{"setPolygonPoints", &Lib::setPolygonPoints},
		{"setPolygonAsBox", &Lib::setPolygonAsBox},
		def("setEdgePoints", &PhysFixtureDef::setEdgePoints),
		{"setChainPoints", &Lib::setChainPoints},
		{"setChainLoopPoints", &Lib::setChainLoopPoints},
		def("setChainPrevPoint", &PhysFixtureDef::setChainPrevPoint),
		def("setChainNextPoint", &PhysFixtureDef::setChainNextPoint),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<PhysFixtureDef, PhysObject>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerPhysFixtureDef = addRegFunc(registerPhysFixtureDef);

// =====================================================================

static void registerPhysFixture(OS * os)
{
	struct Lib
	{
		static int __newinstance(OS * os, int params, int, int, void*)
		{
			os->setException("use PhysBody.createFixture");
			return 0;
		}
	};
	OS::FuncDef funcs[] = {
		{"__newinstance", &Lib::__newinstance},
		DEF_GET("body", PhysFixture, Body),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<PhysFixture, PhysObject>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerPhysFixture = addRegFunc(registerPhysFixture);

// =====================================================================

static void registerPhysBodyDef(OS * os)
{
	struct Lib
	{
		static PhysBodyDef * __newinstance()
		{
			return new PhysBodyDef();
		}
	};
	OS::FuncDef funcs[] = {
		def("__newinstance", &Lib::__newinstance),
		DEF_PROP("type", PhysBodyDef, Type),
		DEF_PROP("pos", PhysBodyDef, Pos),
		DEF_PROP("angle", PhysBodyDef, Angle),
		DEF_PROP("linearVelocity", PhysBodyDef, LinearVelocity),
		DEF_PROP("angularVelocity", PhysBodyDef, AngularVelocity),
		DEF_PROP("linearDamping", PhysBodyDef, LinearDamping),
		DEF_PROP("angularDamping", PhysBodyDef, AngularDamping),
		DEF_PROP("isSleepingAllowed", PhysBodyDef, IsSleepingAllowed),
		DEF_PROP("isAwake", PhysBodyDef, IsAwake),
		DEF_PROP("isFixedRotation", PhysBodyDef, IsFixedRotation),
		DEF_PROP("isBullet", PhysBodyDef, IsBullet),
		DEF_PROP("isActive", PhysBodyDef, IsActive),
		DEF_PROP("gravityScale", PhysBodyDef, GravityScale),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<PhysBodyDef, PhysObject>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerPhysBodyDef = addRegFunc(registerPhysBodyDef);

// =====================================================================

static void registerPhysBody(OS * os)
{
	struct Lib
	{
		static int __newinstance(OS * os, int params, int, int, void*)
		{
			os->setException("use PhysWorld.createBody");
			return 0;
		}

		static int getFixtureList(OS * os, int params, int, int, void*)
		{
			OS_GET_SELF(PhysBody*);
			self->pushFixtureList(os);
			return 1;
		}
	};
	OS::FuncDef funcs[] = {
		{"__newinstance", &Lib::__newinstance},
		def("createFixture", &PhysBody::createFixture),
		def("destroyFixture", &PhysBody::destroyFixture),
		{"__get@fixtureList", &Lib::getFixtureList},
		def("setTransform", &PhysBody::setTransform),
		DEF_PROP("pos", PhysBody, Pos),
		DEF_PROP("angle", PhysBody, Angle),
		DEF_GET("worldCenter", PhysBody, WorldCenter),
		DEF_GET("localCenter", PhysBody, LocalCenter),
		DEF_PROP("linearVelocity", PhysBody, LinearVelocity),
		DEF_PROP("angularVelocity", PhysBody, AngularVelocity),
		def("applyForce", &PhysBody::applyForce),
		def("applyForceToCenter", &PhysBody::applyForceToCenter),
		def("applyLinearImpulse", &PhysBody::applyLinearImpulse),
		def("applyAngularImpulse", &PhysBody::applyAngularImpulse),
		DEF_GET("mass", PhysBody, Mass),
		DEF_GET("inertia", PhysBody, Inertia),
		def("resetMassData", &PhysBody::resetMassData),
		def("getWorldPoint", &PhysBody::getWorldPoint),
		def("getWorldVector", &PhysBody::getWorldVector),
		def("getLocalPoint", &PhysBody::getLocalPoint),
		def("getLocalVector", &PhysBody::getLocalVector),
		def("getLinearVelocityFromWorldPoint", &PhysBody::getLinearVelocityFromWorldPoint),
		def("getLinearVelocityFromLocalPoint", &PhysBody::getLinearVelocityFromLocalPoint),
		DEF_PROP("linearDamping", PhysBody, LinearDamping),
		DEF_PROP("angularDamping", PhysBody, AngularDamping),
		DEF_PROP("gravityScale", PhysBody, GravityScale),
		DEF_PROP("type", PhysBody, Type),
		DEF_PROP("isBullet", PhysBody, IsBullet),
		DEF_PROP("isSleepingAllowed", PhysBody, IsSleepingAllowed),
		DEF_PROP("isAwake", PhysBody, IsAwake),
		DEF_PROP("isActive", PhysBody, IsActive),
		DEF_PROP("isFixedRotation", PhysBody, IsFixedRotation),
		{}
	};
	OS::NumberDef nums[] = {
		{}
	};
	registerOXClass<PhysBody, PhysObject>(os, funcs, nums, true OS_DBG_FILEPOS);
}
static bool __registerPhysBody = addRegFunc(registerPhysBody);


} // namespace ObjectScript

// =====================================================================
// =====================================================================
// =====================================================================

PhysObject::PhysObject()
{
}

PhysObject::~PhysObject()
{
}

void PhysObject::destroy()
{
}

// =====================================================================
// =====================================================================
// =====================================================================

float PhysWorld::toPhysScale = DEF_TO_PHYS_SCALE;

float PhysWorld::getToPhysScale()
{
	return toPhysScale;
}

void PhysWorld::setToPhysScale(float value)
{
	toPhysScale = value;
}

float PhysWorld::toPhysValue(float a)
{
	return a * toPhysScale;
}

float PhysWorld::fromPhysValue(float a)
{
	return a / toPhysScale;
}

b2Vec2 PhysWorld::toPhysVec(const vec2 &pos)
{
	return b2Vec2(toPhysValue(pos.x), toPhysValue(pos.y));
}

vec2 PhysWorld::fromPhysVec(const b2Vec2 &pos)
{
	return vec2(fromPhysValue(pos.x), fromPhysValue(pos.y));
}

void PhysWorld::pushBodyList(ObjectScript::OS * os)
{
	os->newArray();
	std::vector<spPhysBody>::iterator it = bodyList.begin();
	for(; it != bodyList.end(); ++it){
		ObjectScript::pushCtypeValue(os, *it);
		os->addProperty(-2);
	}
}

void PhysWorld::pushJointList(ObjectScript::OS * os)
{
	os->newArray();
	std::vector<spPhysJoint>::iterator it = jointList.begin();
	for(; it != jointList.end(); ++it){
		ObjectScript::pushCtypeValue(os, *it);
		os->addProperty(-2);
	}
}

// =====================================================================

PhysWorld::PhysWorld()
{
	accumTime = 0.0f;
	persistentDeltaTime = 1.0f / 30.0f;
	updateInProgress = false;

	core = new b2World(b2Vec2(0, 0));
	core->SetDestructionListener(this);
	core->SetContactListener(this);
	core->SetContactFilter(this);
}

PhysWorld::~PhysWorld()
{
	destroy();
}

b2World * PhysWorld::getCore()
{
	return core;
}

void PhysWorld::destroy()
{
	if(core){
		destroyAllBodies();
		destroyAllJoints();
		delete core;
		core = NULL;
	}
}

void PhysWorld::registerBody(spPhysBody body)
{
	OX_ASSERT(std::find(bodyList.begin(), bodyList.end(), body) == bodyList.end());
	bodyList.push_back(body);
}

void PhysWorld::unregisterBody(spPhysBody body)
{
	std::vector<spPhysBody>::iterator it = std::find(bodyList.begin(), bodyList.end(), body); 
	OX_ASSERT(it != bodyList.end());
	bodyList.erase(it);
}

void PhysWorld::registerJoint(spPhysJoint joint)
{
	OX_ASSERT(std::find(jointList.begin(), jointList.end(), joint) == jointList.end());
	jointList.push_back(joint);
}

void PhysWorld::unregisterJoint(spPhysJoint joint)
{
	std::vector<spPhysJoint>::iterator it = std::find(jointList.begin(), jointList.end(), joint); 
	OX_ASSERT(it != jointList.end());
	jointList.erase(it);
}

void PhysWorld::destroyAllBodies()
{
	if(core){
		b2Body * body = core->GetBodyList(), * next;
		for(; body; body = next){
			next = body->GetNext();
			destroyBody(dynamic_cast<PhysBody*>((PhysObject*)body->GetUserData()));
		}
		waitBodyListToDestroy.clear();
	}
}

void PhysWorld::destroyAllJoints()
{
	if(core){
		b2Joint * joint = core->GetJointList(), * next;
		for(; joint; joint = next){
			next = joint->GetNext();
			destroyJoint(dynamic_cast<PhysJoint*>((PhysObject*)joint->GetUserData()));
		}
	}
}

spPhysBody PhysWorld::createBody(spPhysBodyDef def)
{
	if(!core || !def){
		return NULL;
	}
	b2Body * bodyCore = core->CreateBody(&def->def);
	return new PhysBody(this, bodyCore);
}

void PhysWorld::destroyBody(spPhysBody body)
{
	if(!core || !body){
		return;
	}
	if(!body->core){
		OX_ASSERT(std::find(bodyList.begin(), bodyList.end(), body) == bodyList.end());
		return;
	}
	OX_ASSERT(std::find(waitBodyListToDestroy.begin(), waitBodyListToDestroy.end(), body->core) == waitBodyListToDestroy.end());

	b2Body * coreBody = body->core;
	body->unlink();

	if(updateInProgress){
		b2Fixture * coreFixture = coreBody->GetFixtureList();
		while(coreFixture){
			coreBody->DestroyFixture(coreFixture);
			coreFixture = coreBody->GetFixtureList();
		}
		waitBodyListToDestroy.push_back(coreBody);
	}else{
		core->DestroyBody(coreBody);
	}
}

spPhysJoint PhysWorld::createJoint(spPhysJointDef def)
{
	if(!core || !def){
		return NULL;
	}
	b2Joint * jointCore = core->CreateJoint(def->pdef);
	return new PhysJoint(this, jointCore);
}

void PhysWorld::destroyJoint(spPhysJoint joint)
{
	if(!core || !joint){
		return;
	}
	if(!joint->core){
		OX_ASSERT(std::find(jointList.begin(), jointList.end(), joint) == jointList.end());
		return;
	}
	// OX_ASSERT(std::find(waitJointListToDestroy.begin(), waitJointListToDestroy.end(), joint->core) == waitJointListToDestroy.end());
	// waitJointListToDestroy.push_back(joint->core);
	b2Joint * coreJoint = joint->core;
	joint->unlink();
	core->DestroyJoint(coreJoint);
}

float PhysWorld::getPersistentDeltaTime() const
{
	return persistentDeltaTime;
}

void PhysWorld::setPersistentDeltaTime(float value)
{
	persistentDeltaTime = MathLib::clamp(value, 0.001f, 0.1f);
}

void PhysWorld::update(float timeStep, int velocityIterations, int positionIterations)
{
	if(core && !updateInProgress){
		updateInProgress = true;
		if(waitBodyListToDestroy.size() > 0){
			std::vector<b2Body*> waitBodyListToDestroy = this->waitBodyListToDestroy;
			this->waitBodyListToDestroy.clear();
			for(int i = 0; i < (int)waitBodyListToDestroy.size(); i++){
				b2Body * coreBody = waitBodyListToDestroy[i];
				core->DestroyBody(coreBody);
			}
		}
		accumTime += timeStep;
		while(accumTime >= persistentDeltaTime){
			core->Step(persistentDeltaTime, velocityIterations, positionIterations);
			accumTime -= persistentDeltaTime;
		}
		updateInProgress = false;
	}
}

vec2 PhysWorld::getGravity() const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetGravity());
	}
	return vec2(0, 0);
}

void PhysWorld::setGravity(const vec2& gravity)
{
	if(core){
		core->SetGravity(PhysWorld::toPhysVec(gravity));
	}
}

void PhysWorld::BeginContact(b2Contact* contact)
{
}

void PhysWorld::EndContact(b2Contact* contact)
{
}

void PhysWorld::SayGoodbye(b2Joint* coreJoint)
{
	if(!core){
		OX_ASSERT(false);
		return;
	}
	PhysJoint * joint = dynamic_cast<PhysJoint*>((PhysObject*)coreJoint->GetUserData());
	if(joint){
		joint->unlink();
	}
}

void PhysWorld::SayGoodbye(b2Fixture* coreFixture)
{
	if(!core){
		OX_ASSERT(false);
		return;
	}
	PhysFixture * fixture = dynamic_cast<PhysFixture*>((PhysObject*)coreFixture->GetUserData());
	if(fixture){
		fixture->unlink();
	}
}

static b2ContactFilter defaultFilter;

bool PhysWorld::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	if(!core){
		OX_ASSERT(false);
	}
	return defaultFilter.ShouldCollide(fixtureA, fixtureB);
}

// =====================================================================
// =====================================================================
// =====================================================================

PhysFixtureDef::PhysFixtureDef()
{
	def.shape = &circleShape;
}

PhysFixtureDef::~PhysFixtureDef()
{
}

void PhysFixtureDef::setType(b2Shape::Type value)
{
	switch(value){
	default:
		// TODO: throw exception?
		// no break

	case b2Shape::e_circle:
		def.shape = &circleShape;
		break;
	
	case b2Shape::e_edge:
		def.shape = &edgeShape;
		break;
	
	case b2Shape::e_polygon:
		def.shape = &polygonShape;
		break;
	
	case b2Shape::e_chain:
		def.shape = &chainShape;
		break;
	}
}

float PhysFixtureDef::getCircleRadius() const
{
	return PhysWorld::fromPhysValue(circleShape.m_radius);
}

void PhysFixtureDef::setCircleRadius(float value)
{
	circleShape.m_radius = PhysWorld::toPhysValue(value);
}

vec2 PhysFixtureDef::getCirclePos() const
{
	return PhysWorld::fromPhysVec(circleShape.m_p);
}

void PhysFixtureDef::setCirclePos(const vec2& value)
{
	circleShape.m_p = PhysWorld::toPhysVec(value);
}

void PhysFixtureDef::setPolygonPointsPhysScale(const std::vector<b2Vec2>& points)
{
	polygonShape.Set(&points[0], points.size());
}

void PhysFixtureDef::setPolygonAsBox(const vec2& halfSize, const vec2& center, float angle)
{
	polygonShape.SetAsBox(PhysWorld::toPhysValue(halfSize.x), PhysWorld::toPhysValue(halfSize.y), 
		PhysWorld::toPhysVec(center), angle);
}

void PhysFixtureDef::setPolygonAsBox(const vec2& halfSize)
{
	polygonShape.SetAsBox(PhysWorld::toPhysValue(halfSize.x), PhysWorld::toPhysValue(halfSize.y));
}

void PhysFixtureDef::setEdgePoints(const vec2& p1, const vec2& p2)
{
	edgeShape.Set(PhysWorld::toPhysVec(p1), PhysWorld::toPhysVec(p2));
}

void PhysFixtureDef::setChainPointsPhysScale(const std::vector<b2Vec2>& points)
{
	chainShape.CreateChain(&points[0], points.size());
}

void PhysFixtureDef::setChainLoopPointsPhysScale(const std::vector<b2Vec2>& points)
{
	chainShape.CreateLoop(&points[0], points.size());
}

void PhysFixtureDef::setChainPrevPoint(const vec2& p)
{
	chainShape.SetPrevVertex(PhysWorld::toPhysVec(p));
}

void PhysFixtureDef::setChainNextPoint(const vec2& p)
{
	chainShape.SetNextVertex(PhysWorld::toPhysVec(p));
}

void PhysFixtureDef::readPointArrayPhysScale(std::vector<b2Vec2>& points, ObjectScript::OS * os, int offs)
{
	points.clear();
	if(os->isArray(offs)){
		int count = os->getLen(offs);
		points.reserve(count);
		for(int i = 0; i < count && !os->isExceptionSet(); i++){
			os->pushStackValue(offs);
			os->pushNumber(i);
			os->getProperty();
			if(os->isObject()){
				b2Vec2 p = ObjectScript::CtypeValue<b2Vec2>::getArg(os, -1);
				p.x = PhysWorld::toPhysValue(p.x);
				p.y = PhysWorld::toPhysValue(p.y);
				points.push_back(p);
			}
			os->pop();
		}
		return;
	}
	os->setException("array of vec2 required");
}

// =====================================================================

PhysFixture::PhysFixture(PhysWorld * p_world, b2Fixture * fixture)
{
	world = p_world;
	core = fixture;
	core->SetUserData(this);

	spPhysBody body = dynamic_cast<PhysBody*>((PhysObject*)fixture->GetBody()->GetUserData());
	if(body){
		body->registerFixture(this);
	}else{
		OX_ASSERT(false);
	}
}

PhysFixture::~PhysFixture()
{
	if(world){
		unlink();
	}
}

void PhysFixture::unlink()
{
	OX_ASSERT(world && core && core->GetUserData() == this);
	spPhysBody body = dynamic_cast<PhysBody*>((PhysObject*)core->GetBody()->GetUserData());
	core->SetUserData(NULL);
	core = NULL;
	world = NULL;
	if(body){
		body->unregisterFixture(this);
	}else{
		// OX_ASSERT(false);
	}
}

void PhysFixture::destroy()
{
	if(world){
		OX_ASSERT(core && core->GetUserData() == this);
		spPhysBody body = dynamic_cast<PhysBody*>((PhysObject*)core->GetBody()->GetUserData());
		if(body){
			body->destroyFixture(this);
		}else{
			OX_ASSERT(false);
		}
	}
}

spPhysBody PhysFixture::getBody() const
{
	return dynamic_cast<PhysBody*>((PhysObject*)core->GetBody()->GetUserData());
}

// =====================================================================

PhysBodyDef::PhysBodyDef()
{
}

PhysBodyDef::~PhysBodyDef()
{
}

// =====================================================================

PhysBody::PhysBody(PhysWorld * p_world, b2Body * body)
{
	world = p_world;
	core = body;
	core->SetUserData(this);
	world->registerBody(this);
}

PhysBody::~PhysBody()
{
	if(world){
		unlink();
	}
}

void PhysBody::unlink()
{
	OX_ASSERT(world && core && core->GetUserData() == this);
	world->unregisterBody(this);
	core->SetUserData(NULL);
	core = NULL;
	world = NULL;
	fixtureList.clear();
}

b2Body * PhysBody::getCore()
{
	return core;
}

void PhysBody::destroy()
{
	if(world){
		world->destroyBody(this);
	}
}

void PhysBody::registerFixture(spPhysFixture fixture)
{
	OX_ASSERT(std::find(fixtureList.begin(), fixtureList.end(), fixture) == fixtureList.end());
	fixtureList.push_back(fixture);
}

void PhysBody::unregisterFixture(spPhysFixture fixture)
{
	std::vector<spPhysFixture>::iterator it = std::find(fixtureList.begin(), fixtureList.end(), fixture); 
	OX_ASSERT(it != fixtureList.end());
	fixtureList.erase(it);
}

spPhysFixture PhysBody::createFixture(spPhysFixtureDef def)
{
	if(!core || !def){
		return NULL;
	}
	b2Fixture * coreFixture = core->CreateFixture(&def->def);
	spPhysFixture fixture = new PhysFixture(world, coreFixture);
	// fixtureList.push_back(fixture);
	return fixture;
}

void PhysBody::destroyFixture(spPhysFixture fixture)
{
	if(!core || !fixture){
		return;
	}
	if(!fixture->core){
		OX_ASSERT(std::find(fixtureList.begin(), fixtureList.end(), fixture) == fixtureList.end());
		return;
	}
	b2Fixture * coreFixture = fixture->core;
	fixture->unlink();
	core->DestroyFixture(fixture->core);
}

void PhysBody::pushFixtureList(ObjectScript::OS * os)
{
	os->newArray();
	std::vector<spPhysFixture>::iterator it = fixtureList.begin();
	for(; it != fixtureList.end(); ++it){
		ObjectScript::pushCtypeValue(os, *it);
		os->addProperty(-2);
	}
}

void PhysBody::setTransform(const vec2& pos, float angle)
{
	if(core){
		core->SetTransform(PhysWorld::toPhysVec(pos), angle * MathLib::DEG2RAD);
	}
}

vec2 PhysBody::getPos() const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetPosition());
	}
	return vec2(0, 0);
}

void PhysBody::setPos(const vec2& pos)
{
	if(core){
		core->SetTransform(PhysWorld::toPhysVec(pos), core->GetAngle());
	}
}

float PhysBody::getAngle() const
{
	if(core){
		return core->GetAngle() * MathLib::RAD2DEG;
	}
	return 0;
}

void PhysBody::setAngle(float angle)
{
	if(core){
		core->SetTransform(core->GetPosition(), angle * MathLib::DEG2RAD);
	}
}

vec2 PhysBody::getWorldCenter() const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetWorldCenter());
	}
	return vec2(0, 0);
}
vec2 PhysBody::getLocalCenter() const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetLocalCenter());
	}
	return vec2(0, 0);
}

vec2 PhysBody::getLinearVelocity() const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetLinearVelocity());
	}
	return vec2(0, 0);
}
void PhysBody::setLinearVelocity(const vec2& value)
{
	if(core){
		core->SetLinearVelocity(PhysWorld::toPhysVec(value));
	}
}

float PhysBody::getAngularVelocity() const
{
	if(core){
		return core->GetAngularVelocity() * MathLib::RAD2DEG;
	}
	return 0;
}
void PhysBody::setAngularVelocity(float value)
{
	if(core){
		core->SetAngularVelocity(value * MathLib::DEG2RAD);
	}
}

void PhysBody::applyForce(const vec2& force, const vec2& point, bool wake)
{
	if(core){
		core->ApplyForce(PhysWorld::toPhysVec(force), PhysWorld::toPhysVec(point), wake);
	}
}
void PhysBody::applyForceToCenter(const vec2& force, bool wake)
{
	if(core){
		core->ApplyForceToCenter(PhysWorld::toPhysVec(force), wake);
	}
}
void PhysBody::applyTorque(float torque, bool wake)
{
	if(core){
		core->ApplyTorque(torque * MathLib::DEG2RAD, wake);
	}
}
void PhysBody::applyLinearImpulse(const vec2& impulse, const vec2& point, bool wake)
{
	if(core){
		core->ApplyLinearImpulse(PhysWorld::toPhysVec(impulse), PhysWorld::toPhysVec(point), wake);
	}
}
void PhysBody::applyAngularImpulse(float impulse, bool wake)
{
	if(core){
		core->ApplyAngularImpulse(impulse * MathLib::DEG2RAD, wake);
	}
}

float PhysBody::getMass() const
{
	if(core){
		return core->GetMass();
	}
	return 0;
}
float PhysBody::getInertia() const
{
	if(core){
		return core->GetInertia();
	}
	return 0;
}

// void GetMassData(b2MassData* data) const;
// void SetMassData(const b2MassData* data);
void PhysBody::resetMassData()
{
	if(core){
		core->ResetMassData();
	}
}

vec2 PhysBody::getWorldPoint(const vec2& localPoint) const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetWorldPoint(PhysWorld::toPhysVec(localPoint)));
	}
	return vec2(0, 0);
}
vec2 PhysBody::getWorldVector(const vec2& localVector) const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetWorldVector(PhysWorld::toPhysVec(localVector)));
	}
	return vec2(0, 0);
}

vec2 PhysBody::getLocalPoint(const vec2& worldPoint) const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetLocalPoint(PhysWorld::toPhysVec(worldPoint)));
	}
	return vec2(0, 0);
}
vec2 PhysBody::getLocalVector(const vec2& worldVector) const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetLocalVector(PhysWorld::toPhysVec(worldVector)));
	}
	return vec2(0, 0);
}

vec2 PhysBody::getLinearVelocityFromWorldPoint(const vec2& worldPoint) const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetLinearVelocityFromWorldPoint(PhysWorld::toPhysVec(worldPoint)));
	}
	return vec2(0, 0);
}
vec2 PhysBody::getLinearVelocityFromLocalPoint(const vec2& localPoint) const
{
	if(core){
		return PhysWorld::fromPhysVec(core->GetLinearVelocityFromLocalPoint(PhysWorld::toPhysVec(localPoint)));
	}
	return vec2(0, 0);
}

float PhysBody::getLinearDamping() const
{
	if(core){
		return core->GetLinearDamping();
	}
	return 0;
}
void PhysBody::setLinearDamping(float linearDamping)
{
	if(core){
		return core->SetLinearDamping(linearDamping);
	}
}

float PhysBody::getAngularDamping() const
{
	if(core){
		return core->GetAngularDamping();
	}
	return 0;
}
void PhysBody::setAngularDamping(float angularDamping)
{
	if(core){
		return core->SetAngularDamping(angularDamping);
	}
}

float PhysBody::getGravityScale() const
{
	if(core){
		return core->GetGravityScale();
	}
	return 0;
}
void PhysBody::setGravityScale(float scale)
{
	if(core){
		return core->SetGravityScale(scale);
	}
}

b2BodyType PhysBody::getType() const
{
	if(core){
		return core->GetType();
	}
	return b2_staticBody;
}
void PhysBody::setType(b2BodyType type)
{
	if(core){
		return core->SetType(type);
	}
}

bool PhysBody::getIsBullet() const
{
	if(core){
		return core->IsBullet();
	}
	return false;
}
void PhysBody::setIsBullet(bool flag)
{
	if(core){
		return core->SetBullet(flag);
	}
}

bool PhysBody::getIsSleepingAllowed() const
{
	if(core){
		return core->IsSleepingAllowed();
	}
	return false;
}
void PhysBody::setIsSleepingAllowed(bool flag)
{
	if(core){
		return core->SetSleepingAllowed(flag);
	}
}

bool PhysBody::getIsAwake() const
{
	if(core){
		return core->IsAwake();
	}
	return false;
}
void PhysBody::setIsAwake(bool flag)
{
	if(core){
		return core->SetAwake(flag);
	}
}

bool PhysBody::getIsActive() const
{
	if(core){
		return core->IsActive();
	}
	return false;
}
void PhysBody::setIsActive(bool flag)
{
	if(core){
		return core->SetActive(flag);
	}
}

bool PhysBody::getIsFixedRotation() const
{
	if(core){
		return core->IsFixedRotation();
	}
	return false;
}
void PhysBody::setIsFixedRotation(bool flag)
{
	if(core){
		return core->SetFixedRotation(flag);
	}
}

// =====================================================================
// =====================================================================
// =====================================================================

PhysJointDef::PhysJointDef()
{
	pdef = NULL;
}

PhysJointDef::~PhysJointDef()
{
}

spPhysBody PhysJointDef::getBodyA() const
{
	OX_ASSERT(pdef);
	return dynamic_cast<PhysBody*>((PhysObject*)pdef->bodyA->GetUserData());
}

void PhysJointDef::setBodyA(spPhysBody body)
{
	OX_ASSERT(pdef);
	pdef->bodyA = body->getCore();
}

spPhysBody PhysJointDef::getBodyB() const
{
	OX_ASSERT(pdef);
	return dynamic_cast<PhysBody*>((PhysObject*)pdef->bodyB->GetUserData());
}

void PhysJointDef::setBodyB(spPhysBody body)
{
	OX_ASSERT(pdef);
	pdef->bodyB = body->getCore();
}

// =====================================================================

PhysJoint::PhysJoint(PhysWorld * p_world, b2Joint * joint)
{
	world = p_world;
	core = joint;
	core->SetUserData(this);
	world->registerJoint(this);
}

PhysJoint::~PhysJoint()
{
	if(world){
		unlink();
	}
}

void PhysJoint::unlink()
{
	OX_ASSERT(world && core && core->GetUserData() == this);
	world->unregisterJoint(this);
	core->SetUserData(NULL);
	core = NULL;
	world = NULL;
}

void PhysJoint::destroy()
{
	if(world){
		world->destroyJoint(this);
	}
}

