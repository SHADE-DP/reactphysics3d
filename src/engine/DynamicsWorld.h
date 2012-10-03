/********************************************************************************
* ReactPhysics3D physics library, http://code.google.com/p/reactphysics3d/      *
* Copyright (c) 2010-2012 Daniel Chappuis                                       *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

#ifndef DYNAMICS_WORLD_H
#define DYNAMICS_WORLD_H

// Libraries
#include "CollisionWorld.h"
#include "../collision/CollisionDetection.h"
#include "ConstraintSolver.h"
#include "../body/RigidBody.h"
#include "Timer.h"
#include "../configuration.h"

// Namespace ReactPhysics3D
namespace reactphysics3d {

/*  -------------------------------------------------------------------
    Class DynamicsWorld :
        This class represents a dynamics world. This class inherits from
        the CollisionWorld class. In a dynamics world, bodies can collide
        and their movements are simulated using the laws of physics.
    -------------------------------------------------------------------
*/
class DynamicsWorld : public CollisionWorld {

    protected :
        Timer timer;                                            // Timer of the physics engine
        ConstraintSolver constraintSolver;                      // Constraint solver
        bool isDeactivationActive;                              // True if the deactivation (sleeping) of inactive bodies is enabled
        std::set<RigidBody*> rigidBodies;                       // All the rigid bodies of the physics world
        std::vector<Constraint*> constraints;                   // List that contains all the current constraints
        Vector3 gravity;                                        // Gravity vector of the world
        bool isGravityOn;                                       // True if the gravity force is on
        MemoryPool<OverlappingPair> memoryPoolOverlappingPairs; // Memory pool for the overlapping pairs
        MemoryPool<RigidBody> memoryPoolRigidBodies;            // Memory pool for rigid bodies memory allocation
        MemoryPool<Contact> memoryPoolContacts;                 // Memory pool for the contacts

        void updateAllBodiesMotion();                                                                                                            // Compute the motion of all bodies and update their positions and orientations
        void updatePositionAndOrientationOfBody(RigidBody* body, const Vector3& newLinVelocity, const Vector3& newAngVelocity,
                                                const Vector3& linearVelocityErrorCorrection, const Vector3& angularVelocityErrorCorrection);    // Update the position and orientation of a body
        void setInterpolationFactorToAllBodies();                                                                                                // Compute and set the interpolation factor to all bodies
        void applyGravity();                                                                                                                     // Apply the gravity force to all bodies
        void resetBodiesMovementVariable();                                             // Reset the boolean movement variable of each body
        virtual void updateOverlappingPair(const BroadPhasePair* pair);                         // Update the overlapping pair
        virtual void notifyAddedOverlappingPair(const BroadPhasePair* addedPair);       // Notify the world about a new broad-phase overlapping pair
        virtual void notifyRemovedOverlappingPair(const BroadPhasePair* removedPair);   // Notify the world about a removed broad-phase overlapping pair
        virtual void notifyNewContact(const BroadPhasePair* pair,
                                      const ContactInfo* contactInfo);                  // Notify the world about a new narrow-phase contact

public :
        DynamicsWorld(const Vector3& gravity, decimal timeStep);    // Constructor
        virtual ~DynamicsWorld();                                   // Destructor

        Timer& getTimer() {return timer;}
        void start();                                                   // Start the physics simulation
        void stop();                                                    // Stop the physics simulation
        void update();                                                  // Update the physics simulation
        void setNbLCPIterations(uint nbIterations);                     // Set the number of iterations of the LCP solver
        void setIsErrorCorrectionActive(bool isErrorCorrectionActive);  // Set the isErrorCorrectionActive value
        RigidBody* createRigidBody(const Transform& transform, decimal mass,
                                   const Matrix3x3& inertiaTensorLocal, CollisionShape* collisionShape);  // Create a rigid body into the physics world
        void destroyRigidBody(RigidBody* rigidBody);                                    // Destroy a rigid body
        Vector3 getGravity() const;                                                     // Return the gravity vector of the world
        bool getIsGravityOn() const;                                                    // Return if the gravity is on
        void setIsGratityOn(bool isGravityOn);                                          // Set the isGravityOn attribute
        void addConstraint(Constraint* constraint);                                     // Add a constraint
        void removeConstraint(Constraint* constraint);                                  // Remove a constraint
        void removeAllContactConstraints();                                             // Remove all collision contacts constraints
        void removeAllConstraints();                                                    // Remove all constraints and delete them (free their memory)
        std::vector<Constraint*>::iterator getConstraintsBeginIterator();               // Return a start iterator on the constraint list
        std::vector<Constraint*>::iterator getConstraintsEndIterator();                 // Return a end iterator on the constraint list
        std::set<RigidBody*>::iterator getRigidBodiesBeginIterator();                   // Return an iterator to the beginning of the rigid bodies of the physics world
        std::set<RigidBody*>::iterator getRigidBodiesEndIterator();                     // Return an iterator to the end of the rigid bodies of the physics world
};

// --- Inline functions --- //

// Start the physics simulation
inline void DynamicsWorld::start() {
    timer.start();
}

inline void DynamicsWorld::stop() {
    std::cout << "Stop Simulation" << std::endl;
    timer.stop();
}                

// Set the number of iterations of the LCP solver
inline void DynamicsWorld::setNbLCPIterations(uint nbIterations) {
    constraintSolver.setNbLCPIterations(nbIterations);
}   

// Set the isErrorCorrectionActive value
inline void DynamicsWorld::setIsErrorCorrectionActive(bool isErrorCorrectionActive) {
    constraintSolver.setIsErrorCorrectionActive(isErrorCorrectionActive);
}

// Reset the boolean movement variable of each body
inline void DynamicsWorld::resetBodiesMovementVariable() {

    // For each rigid body
    for (std::set<RigidBody*>::iterator it = getRigidBodiesBeginIterator(); it != getRigidBodiesEndIterator(); it++) {

        // Set the hasMoved variable to false
        (*it)->setHasMoved(false);
    }
}

// Update the overlapping pair
inline void DynamicsWorld::updateOverlappingPair(const BroadPhasePair* pair) {

    // Get the pair of body index
    std::pair<bodyindex, bodyindex> indexPair = pair->getBodiesIndexPair();

    // Get the corresponding overlapping pair
    OverlappingPair* overlappingPair = overlappingPairs[indexPair];

    // Update the contact cache of the overlapping pair
    overlappingPair->update();
}


// Add a constraint into the physics world
inline void DynamicsWorld::addConstraint(Constraint* constraint) {
    assert(constraint != 0);
    constraints.push_back(constraint);
}

// Remove a constraint and free its memory
inline void DynamicsWorld::removeConstraint(Constraint* constraint) {
    std::vector<Constraint*>::iterator it;

    assert(constraint);
    it = std::find(constraints.begin(), constraints.end(), constraint);
    assert(*it == constraint);
    delete *it;
    constraints.erase(it);
}

// Return the gravity vector of the world
inline Vector3 DynamicsWorld::getGravity() const {
    return gravity;
}

// Return if the gravity is on
inline bool DynamicsWorld::getIsGravityOn() const {
    return isGravityOn;
}

// Set the isGravityOn attribute
inline void DynamicsWorld::setIsGratityOn(bool isGravityOn) {
    this->isGravityOn = isGravityOn;
}

// Return an iterator to the beginning of the bodies of the physics world
inline std::set<RigidBody*>::iterator DynamicsWorld::getRigidBodiesBeginIterator() {
    return rigidBodies.begin();
}

// Return an iterator to the end of the bodies of the physics world
inline std::set<RigidBody*>::iterator DynamicsWorld::getRigidBodiesEndIterator() {
    return rigidBodies.end();
}

// Return a start iterator on the constraint list
inline std::vector<Constraint*>::iterator DynamicsWorld::getConstraintsBeginIterator() {
    return constraints.begin();
}

// Return a end iterator on the constraint list
inline std::vector<Constraint*>::iterator DynamicsWorld::getConstraintsEndIterator() {
    return constraints.end();
}

}

#endif