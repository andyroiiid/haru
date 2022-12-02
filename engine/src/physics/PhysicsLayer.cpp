//
// Created by Andrew Huang on 12/2/2022.
//

#include "haru/physics/PhysicsLayer.h"

#include <vector>
#include <PxPhysicsAPI.h>

template<class Func>
static std::vector<physx::PxShape *> ForEachActorShape(physx::PxRigidActor *actor, Func &&func) {
    physx::PxU32 numShapes = actor->getNbShapes();
    std::vector<physx::PxShape *> shapes(numShapes);
    actor->getShapes(shapes.data(), shapes.size());
    for (physx::PxShape *shape: shapes) {
        func(shape);
    }
    return shapes;
}

//void PhysicsSetSimulationLayer(physx::PxRigidActor *actor, PhysicsLayer layer) {
//    const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(layer);
//    ForEachActorShape(actor, [&filterData](physx::PxShape *shape) {
//        shape->setSimulationFilterData(filterData);
//    });
//}

void PhysicsSetQueryLayer(physx::PxRigidActor *actor, PhysicsLayer layer) {
    const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(layer);
    ForEachActorShape(actor, [&filterData](physx::PxShape *shape) {
        shape->setQueryFilterData(filterData);
    });
}
