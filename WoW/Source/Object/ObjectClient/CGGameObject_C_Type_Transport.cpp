#include "CGGameObject_C_Type_Transport.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/TransportAnimationRec.h"
#include "WoW/Source/DB/Rec/TransportRotationRec.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/GameObject.h"
#include <cmath>
#include "WoW/Source/WorldClient/MapEntity.h"

// STATIC: 0x00ADA96C
static float s_progressBias = 0.5f;

// VERIFY: 0bbb79ee manual
// OFFSET: 0x0070C8C0
static int32_t FindTransportAnimationIndex(int32_t transportId) {
    int32_t high = g_transportAnimationDB.GetNumRecords();
    int32_t low = 0;

    if (high <= 0) {
        return -1;
    }

    do {
        int32_t mid = (high + low) / 2;
        const TransportAnimationRec* first = (0 >= g_transportAnimationDB.GetNumRecords()) ? nullptr : &g_transportAnimationDB.m_records[0];

        if (first[mid].m_transportID >= transportId) {
            high = mid;
        } else {
            low = mid + 1;
        }
    } while (low < high);

    const TransportAnimationRec* first = (0 >= g_transportAnimationDB.GetNumRecords()) ? nullptr : &g_transportAnimationDB.m_records[0];

    if (low < g_transportAnimationDB.GetNumRecords() && first[low].m_transportID == transportId) {
        return low;
    }

    return -1;
}

// VERIFY: 8c4056b2 manual
// OFFSET: 0x0070C930
static int32_t FindTransportRotationIndex(int32_t transportId) {
    int32_t high = g_transportRotationDB.GetNumRecords();
    int32_t low = 0;

    if (high <= 0) {
        return -1;
    }

    do {
        int32_t mid = (high + low) / 2;
        const TransportRotationRec* first = (0 >= g_transportRotationDB.GetNumRecords()) ? nullptr : &g_transportRotationDB.m_records[0];

        if (first[mid].m_gameObjectsID >= transportId) {
            high = mid;
        } else {
            low = mid + 1;
        }
    } while (low < high);

    const TransportRotationRec* first = (0 >= g_transportRotationDB.GetNumRecords()) ? nullptr : &g_transportRotationDB.m_records[0];

    if (low < g_transportRotationDB.GetNumRecords() && first[low].m_gameObjectsID == transportId) {
        return low;
    }

    return -1;
}

// VERIFY: e9e508da manual
// OFFSET: 0x00713820
CGGameObject_C_Type_Transport::CGGameObject_C_Type_Transport(CGGameObject_C* owner) : CGGameObject_C_Type_TransportBase(owner), m_posKeys(nullptr), m_numPosKeys(0), m_currentFrame(0), m_rotKeys(nullptr), m_rotationCount(0), m_currentRotation(0), m_lastSequence(ANIM_COUNT), m_stateChangeSequenceTime(0) {
    int32_t index = FindTransportAnimationIndex(owner->m_obj->m_entryID);

    if (index != -1) {
        this->m_posKeys = reinterpret_cast<TransportFrame*>((index < 0 || index >= g_transportAnimationDB.GetNumRecords()) ? nullptr : &g_transportAnimationDB.m_records[index]);
        STORM_ASSERT(m_posKeys->m_TimeIndex == 0);
        this->m_numPosKeys = 1;

        for (int32_t i = index + 1; i < g_transportAnimationDB.GetNumRecords(); i++) {
            const TransportAnimationRec* rec = (i < 0 || i >= g_transportAnimationDB.GetNumRecords()) ? nullptr : &g_transportAnimationDB.m_records[i];

            if (rec->m_transportID != owner->m_obj->m_entryID) {
                break;
            }

            this->m_numPosKeys++;
        }
    }

    index = FindTransportRotationIndex(owner->m_obj->m_entryID);

    if (index != -1) {
        this->m_rotKeys = (index < 0 || index >= g_transportRotationDB.GetNumRecords()) ? nullptr : &g_transportRotationDB.m_records[index];
        STORM_ASSERT(m_rotKeys->m_TimeIndex == 0);
        this->m_rotationCount = 1;

        for (int32_t i = index + 1; i < g_transportRotationDB.GetNumRecords(); i++) {
            const TransportRotationRec* rec = (i < 0 || i >= g_transportRotationDB.GetNumRecords()) ? nullptr : &g_transportRotationDB.m_records[i];

            if (rec->m_gameObjectsID != owner->m_obj->m_entryID) {
                break;
            }

            this->m_rotationCount++;
        }
    }

    this->InitializeMovement();
}

// VERIFY: 9cb18f7c auto
// OFFSET: 0x00710640
uint32_t CGGameObject_C_Type_Transport::GetSequenceTimeFromRealTime(int32_t state, uint32_t time) const {
    uint32_t count = this->m_numPosKeys;

    if (!count) {
        return time;
    }

    uint32_t period = this->m_owner->m_gameObject->m_level;

    if (!period) {
        return time % this->m_posKeys[count - 1].m_TimeIndex;
    }

    uint32_t elapsed = time - this->m_stateChangeTime;
    int32_t currentState = this->m_currentState;
    uint32_t result;

    if (state == currentState) {
        result = this->m_stateChangeSequenceTime + elapsed;
    } else if (elapsed < this->m_stateChangeSequenceTime) {
        result = this->m_stateChangeSequenceTime - elapsed;
    } else {
        result = 0;
    }

    if (currentState) {
        result += period;

        if (result >= this->m_posKeys[count - 1].m_TimeIndex) {
            return 0;
        }
    } else if (result > period) {
        return period;
    }

    return result;
}

// VERIFY: 4acdedb5 auto
// OFFSET: 0x00713990
uint32_t CGGameObject_C_Type_Transport::GetSequenceTimeFromRealTime(uint32_t time) const {
    return this->GetSequenceTimeFromRealTime(this->m_owner->m_gameObject->m_state, time);
}

// VERIFY: 15b1af82 manual
// OFFSET: 0x00712010
void CGGameObject_C_Type_Transport::Reenable() {
    CGGameObject_C_Type_TransportBase::Reenable();
    this->InitializeMovement();
}

// VERIFY: 9abb4c2a auto
// OFFSET: 0x0070B5C0
void CGGameObject_C_Type_Transport::PostInitWithStats() {
    this->CGGameObject_C_Type_TransportBase::PostInitWithStats();

    if (this->m_objectId) {
        World::ObjectSetSequenceFinishedCallback(this->m_objectId, &CGGameObject_C_Type_WorldObj::SequenceFinishedHandler, 0, 0);
        World::ObjectUpdate(this->m_objectId, this->m_owner->GetMatrix());
    }
}

// VERIFY: 1347bf69 auto
// OFFSET: 0x0070B630
void CGGameObject_C_Type_Transport::PostReenable() {
    this->m_lastSequence = ANIM_COUNT;
    this->CGGameObject_C_Type_TransportBase::PostReenable();

    if (this->m_objectId) {
        World::ObjectSetSequenceFinishedCallback(this->m_objectId, &CGGameObject_C_Type_WorldObj::SequenceFinishedHandler, 0, 0);
    }
}

// VERIFY: 8835b6bb manual
// OFFSET: 0x00711F20
void CGGameObject_C_Type_Transport::InitializeMovement() {
    CGGameObject_C* owner = this->m_owner;
    this->m_currentState = owner->m_gameObject->m_state;
    uint32_t time = owner->m_transportTimeOffset + OsGetAsyncTimeMs();
    this->InitializeStateChangeTime(time);
    C3Vector offset(0.0f, 0.0f, 0.0f);
    C4Quaternion rotation;
    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.z = 0.0f;
    rotation.w = 1.0f;
    this->m_pathTime = time;
    this->Interpolate(time, &offset, &rotation);
    C3Vector position = this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);
    this->m_position.x = position.x + offset.x;
    this->m_position.y = position.y + offset.y;
    this->m_position.z = position.z + offset.z;
    CPassenger& passenger = this->m_owner->m_passenger;
    reinterpret_cast<C4QuaternionCompressed&>(passenger.m_facing) = rotation;
    passenger.m_passengerFlags |= CPassenger::PASSENGER_QUATERNION_FACING;
    C44Matrix& matrix = this->m_owner->m_matrix;
    matrix = this->m_owner->GetRotation();
    matrix.d0 = this->m_position.x;
    matrix.d1 = this->m_position.y;
    matrix.d2 = this->m_position.z;
}

// VERIFY: 89bf86e6 auto
// OFFSET: 0x00710570
uint32_t CGGameObject_C_Type_Transport::GetStateDuration(int32_t state) {
    uint32_t count = this->m_numPosKeys;

    if (!count) {
        return 0;
    }

    uint32_t period = this->m_owner->m_gameObject->m_level;

    if (!period) {
        return this->m_posKeys[count - 1].m_TimeIndex;
    }

    if (state) {
        return this->m_posKeys[count - 1].m_TimeIndex - period;
    }

    return period;
}

// VERIFY: 5df80093 manual
// OFFSET: 0x007105D0
void CGGameObject_C_Type_Transport::InitializeStateChangeTime(uint32_t time) {
    CGGameObjectData* gameObject = this->m_owner->m_gameObject;

    if (gameObject->m_level && this->m_numPosKeys) {
        float elapsed = this->GetStateDuration(gameObject->m_state) * (gameObject->m_pathProgress * 0.000015259022f);
        this->m_stateChangeTime = time - static_cast<uint32_t>(elapsed);
    }
}

// INLINED
inline bool CGGameObject_C_Type_Transport::Is2State() const {
    return this->m_owner->m_gameObject->m_level != 0;
}

// VERIFY: 823a6baf manual
// OFFSET: 0x0070DA40
uint32_t CGGameObject_C_Type_Transport::ResetStateChange(uint32_t time) {
    STORM_ASSERT(Is2State() && m_numPosKeys);

    CGGameObjectData* gameObject = this->m_owner->m_gameObject;
    int32_t state = gameObject->m_state;
    this->m_currentState = state;
    this->m_stateChangeSequenceTime = 0;
    uint32_t period = gameObject->m_level;

    if (state) {
        this->m_stateChangeTime = time + period - this->m_posKeys[this->m_numPosKeys - 1].m_TimeIndex;
        return 0;
    }

    this->m_stateChangeTime = time - period;
    return period;
}

// VERIFY: 0fdff1cd manual
// OFFSET: 0x0070DAA0
C3Vector CGGameObject_C_Type_Transport::GetMovement(uint32_t time) {
    if (this->m_numPosKeys <= 1) {
        return C3Vector(0.0f, 0.0f, 0.0f);
    }

    uint32_t next = this->m_numPosKeys != this->m_currentFrame + 1 ? this->m_currentFrame + 1 : 0;
    const TransportFrame* current;
    const TransportFrame* following;

    while (true) {
        following = &this->m_posKeys[next];
        current = &this->m_posKeys[this->m_currentFrame];

        if (time >= current->m_TimeIndex && time < following->m_TimeIndex) {
            break;
        }

        this->m_currentFrame = next;
        next = this->m_numPosKeys != next + 1 ? next + 1 : 0;
    }

    float t = static_cast<float>(time - current->m_TimeIndex) / static_cast<float>(following->m_TimeIndex - current->m_TimeIndex);

    if (this->m_objectId) {
        uint32_t sequence = current->m_sequenceId;

        if (sequence != this->m_lastSequence) {
            CGGameObject_C* owner = this->m_owner;
            this->m_lastSequence = sequence;
            World::ObjectSetSequence(this->m_objectId, sequence, 0, 0);
            owner->UpdateObjectEffectAnimationStates(sequence);
        }
    }

    C33Matrix rotation(this->m_owner->m_gameObject->m_parentRotation);
    float inverse = 1.0f - t;
    float x = t * following->m_pos.x + current->m_pos.x * inverse;
    float y = following->m_pos.y * t + current->m_pos.y * inverse;
    float z = inverse * current->m_pos.z + following->m_pos.z * t;
    C3Vector result;
    result.x = rotation.c0 * z + rotation.b0 * y + x * rotation.a0;
    result.y = rotation.c1 * z + rotation.b1 * y + rotation.a1 * x;
    result.z = z * rotation.c2 + y * rotation.b2 + x * rotation.a2;
    return result;
}

// VERIFY: 06adb4af manual
// OFFSET: 0x0070DC10
C4Quaternion CGGameObject_C_Type_Transport::GetRotation(uint32_t time) {
    uint32_t count = this->m_rotationCount;

    if (count <= 1) {
        return this->m_owner->GetRotation();
    }

    uint32_t next = this->m_currentRotation + 1 != count ? this->m_currentRotation + 1 : 0;
    const TransportRotationRec* current = &this->m_rotKeys[this->m_currentRotation];
    const TransportRotationRec* following = &this->m_rotKeys[next];
    uint32_t endTime = following->m_TimeIndex;

    if (!endTime && this->m_posKeys) {
        endTime = this->m_posKeys[this->m_numPosKeys - 1].m_TimeIndex;
    }

    while (!(time >= static_cast<uint32_t>(current->m_TimeIndex) && time < endTime)) {
        this->m_currentRotation = next;
        next = this->m_rotationCount != next + 1 ? next + 1 : 0;
        current = &this->m_rotKeys[this->m_currentRotation];
        following = &this->m_rotKeys[next];
        endTime = following->m_TimeIndex;

        if (!endTime && this->m_posKeys) {
            endTime = this->m_posKeys[this->m_numPosKeys - 1].m_TimeIndex;
        }
    }

    float t = static_cast<float>(time - current->m_TimeIndex) / static_cast<float>(following->m_TimeIndex - current->m_TimeIndex);
    C4Quaternion rotation = C4Quaternion::Slerp(t, reinterpret_cast<const C4Quaternion&>(current->m_rot), reinterpret_cast<const C4Quaternion&>(following->m_rot));
    return rotation * this->m_owner->m_gameObject->m_parentRotation;
}

// VERIFY: 007eb5fe auto
// OFFSET: 0x007106D0
void CGGameObject_C_Type_Transport::Interpolate(uint32_t time, C3Vector* position, C4Quaternion* rotation) {
    STORM_ASSERT(position);
    STORM_ASSERT(rotation);

    int32_t state = this->m_owner->m_gameObject->m_state;
    uint32_t sequenceTime = this->GetSequenceTimeFromRealTime(state, time);

    if (this->m_stateChangeSequenceTime) {
        int32_t current = this->m_currentState;

        if (current != state || current) {
            if (!sequenceTime) {
                sequenceTime = this->ResetStateChange(time);
            }
        } else if (sequenceTime == this->m_owner->m_gameObject->m_level) {
            sequenceTime = this->ResetStateChange(time);
        }
    }

    *position = this->GetMovement(sequenceTime);
    *rotation = this->GetRotation(sequenceTime);
}

// VERIFY: 6f1084da manual
// OFFSET: 0x0070C650
C44Matrix& CGGameObject_C_Type_Transport::UpdateOwnerMatrix() {
    C44Matrix& matrix = this->m_owner->m_matrix;
    matrix = this->m_owner->GetRotation();
    matrix.d0 = this->m_position.x;
    matrix.d1 = this->m_position.y;
    matrix.d2 = this->m_position.z;
    return matrix;
}

// VERIFY: 181b2f3a manual
// OFFSET: 0x007139E0
void CGGameObject_C_Type_Transport::UpdateMovement(uint32_t time, uint32_t elapsed) {
    if (!this->m_worldObjectReady && this->m_objectId && World::ObjectIsLoaded(this->m_objectId)) {
        this->m_worldObjectReady = 1;

        if (!this->m_numPosKeys && !this->m_rotationCount) {
            World::ObjectUpdate(this->m_objectId, this->m_owner->GetMatrix());
            return;
        }
    }

    uint32_t step = elapsed;

    if (elapsed > 250) {
        CGGameObject_C* owner = this->m_owner;
        uint32_t pathTime = owner->m_transportTimeOffset + time - 250;
        this->m_pathTime = pathTime;

        if (owner->m_gameObject->m_level && static_cast<int32_t>(this->m_stateChangeTime - pathTime) >= 0) {
            this->m_pathTime = this->m_stateChangeTime;
        }

        C3Vector offset(0.0f, 0.0f, 0.0f);
        C4Quaternion rotation;
        rotation.x = 0.0f;
        rotation.y = 0.0f;
        rotation.z = 0.0f;
        rotation.w = 1.0f;
        this->Interpolate(this->m_pathTime, &offset, &rotation);
        CPassenger& passenger = this->m_owner->m_passenger;
        reinterpret_cast<C4QuaternionCompressed&>(passenger.m_facing) = rotation;
        passenger.m_passengerFlags |= CPassenger::PASSENGER_QUATERNION_FACING;
        C3Vector position = this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);
        step = 250;
        this->m_position.x = position.x + offset.x;
        this->m_position.y = position.y + offset.y;
        this->m_position.z = position.z + offset.z;
    }

    this->m_pathTime = this->m_owner->m_transportTimeOffset + time;
    this->m_transportUpdateTime = this->GetSequenceTimeFromRealTime(this->m_currentState, this->m_pathTime);
    C3Vector offset(0.0f, 0.0f, 0.0f);
    C4Quaternion rotation;
    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.z = 0.0f;
    rotation.w = 1.0f;
    this->Interpolate(this->m_pathTime, &offset, &rotation);
    C3Vector base = this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);
    C3Vector position(base.x + offset.x, base.y + offset.y, base.z + offset.z);
    C3Vector direction(0.0f, 0.0f, 0.0f);
    float speed = this->UpdateCurrentVelocity(step, position, &direction);

    if (this->m_passengers.Head()) {
        if (fabs(speed) >= 0.00000095367432f) {
            if (this->m_objectId && !World::ObjectIsWMO(this->m_objectId)) {
                C44Matrix& matrix = this->UpdateOwnerMatrix();

                for (CPassenger* passenger = this->m_passengers.Head(); passenger; passenger = this->m_passengers.Next(passenger)) {
                    if (passenger->m_passengerFlags & CPassenger::PASSENGER_MOVEMENT) {
                        static_cast<CMovement_C*>(passenger)->PassengerMove(time, step, speed, direction, matrix);
                    }
                }
            }

            this->m_position = position;
        }

        CPassenger& passenger = this->m_owner->m_passenger;
        reinterpret_cast<C4QuaternionCompressed&>(passenger.m_facing) = rotation;
        passenger.m_passengerFlags |= CPassenger::PASSENGER_QUATERNION_FACING;
        C44Matrix& matrix = this->m_owner->m_matrix;
        matrix = this->m_owner->GetRotation();
        matrix.d0 = this->m_position.x;
        matrix.d1 = this->m_position.y;
        matrix.d2 = this->m_position.z;
        int32_t moved = time;
        this->MovePassengers(&moved);
    } else {
        this->m_position = position;
        CPassenger& passenger = this->m_owner->m_passenger;
        reinterpret_cast<C4QuaternionCompressed&>(passenger.m_facing) = rotation;
        passenger.m_passengerFlags |= CPassenger::PASSENGER_QUATERNION_FACING;
        C44Matrix& matrix = this->m_owner->m_matrix;
        matrix = this->m_owner->GetRotation();
        matrix.d0 = this->m_position.x;
        matrix.d1 = this->m_position.y;
        matrix.d2 = this->m_position.z;

        if (this->m_objectId) {
            World::ObjectUpdate(this->m_objectId, matrix);
        }
    }
}

// VERIFY: 728231f0 auto
// OFFSET: 0x00710780
float CGGameObject_C_Type_Transport::GetStateProgress(uint32_t time, int32_t state) {
    uint32_t sequenceTime = this->GetSequenceTimeFromRealTime(state, time);
    int32_t current = this->m_currentState;
    uint32_t duration = this->GetStateDuration(current);

    if (current) {
        if (!sequenceTime) {
            sequenceTime = this->m_posKeys[this->m_numPosKeys - 1].m_TimeIndex;
        }

        sequenceTime -= this->m_owner->m_gameObject->m_level;
    }

    float elapsed = state == current ? static_cast<float>(sequenceTime) : static_cast<float>(duration - sequenceTime);
    return elapsed / static_cast<float>(duration);
}

// VERIFY: 476dc037 manual
// OFFSET: 0x00710820
void CGGameObject_C_Type_Transport::UpdateState(int32_t oldState, int32_t newState) {
    CGGameObject_C* owner = this->m_owner;

    if (!owner->m_gameObject->m_level || !this->m_numPosKeys || this->m_currentState == newState) {
        return;
    }

    uint32_t time = owner->m_transportTimeOffset + OsGetAsyncTimeMs();
    float progress = this->GetStateProgress(time, oldState);

    if (progress >= 0.95f) {
        this->m_stateChangeTime = time;
        this->m_currentState = newState;
        this->m_stateChangeSequenceTime = 0;
    } else {
        float elapsed = progress * this->GetStateDuration(oldState);
        this->m_stateChangeTime = time;
        this->m_stateChangeSequenceTime = CMath::fint_(elapsed - s_progressBias);
    }
}
