#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

VehicleSeatRec::VehicleSeatRec() {
}

VehicleSeatRec::~VehicleSeatRec() {
}

// VERIFY: 6f693d65 auto
// OFFSET: 0x008BAD50
const char* VehicleSeatRec::GetFilename() {
    return "DBFilesClient\\VehicleSeat.dbc";
}

// VERIFY: f1d4806f auto
// OFFSET: 0x008BAD60
bool VehicleSeatRec::Read(SFileImpl* file, const char* stringBuffer) {
    int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
    int32_t flagsRead = SFileReadFile(file, &this->m_flags, sizeof(this->m_flags), nullptr, nullptr, 0);
    int32_t attachmentIDRead = SFileReadFile(file, &this->m_attachmentID, sizeof(this->m_attachmentID), nullptr, nullptr, 0);
    int32_t attachmentRead0 = SFileReadFile(file, &this->m_attachmentOffset[0], sizeof(this->m_attachmentOffset[0]), nullptr, nullptr, 0);
    int32_t attachmentRead1 = SFileReadFile(file, &this->m_attachmentOffset[1], sizeof(this->m_attachmentOffset[1]), nullptr, nullptr, 0);
    int32_t attachmentRead2 = SFileReadFile(file, &this->m_attachmentOffset[2], sizeof(this->m_attachmentOffset[2]), nullptr, nullptr, 0);
    int32_t enterPreDelayRead = SFileReadFile(file, &this->m_enterPreDelay, sizeof(this->m_enterPreDelay), nullptr, nullptr, 0);
    int32_t enterSpeedRead = SFileReadFile(file, &this->m_enterSpeed, sizeof(this->m_enterSpeed), nullptr, nullptr, 0);
    int32_t enterGravityRead = SFileReadFile(file, &this->m_enterGravity, sizeof(this->m_enterGravity), nullptr, nullptr, 0);
    int32_t enterMinDurationRead = SFileReadFile(file, &this->m_enterMinDuration, sizeof(this->m_enterMinDuration), nullptr, nullptr, 0);
    int32_t enterMaxDurationRead = SFileReadFile(file, &this->m_enterMaxDuration, sizeof(this->m_enterMaxDuration), nullptr, nullptr, 0);
    int32_t enterMinArcHeightRead = SFileReadFile(file, &this->m_enterMinArcHeight, sizeof(this->m_enterMinArcHeight), nullptr, nullptr, 0);
    int32_t enterMaxArcHeightRead = SFileReadFile(file, &this->m_enterMaxArcHeight, sizeof(this->m_enterMaxArcHeight), nullptr, nullptr, 0);
    int32_t enterAnimStartRead = SFileReadFile(file, &this->m_enterAnimStart, sizeof(this->m_enterAnimStart), nullptr, nullptr, 0);
    int32_t enterAnimLoopRead = SFileReadFile(file, &this->m_enterAnimLoop, sizeof(this->m_enterAnimLoop), nullptr, nullptr, 0);
    int32_t rideAnimStartRead = SFileReadFile(file, &this->m_rideAnimStart, sizeof(this->m_rideAnimStart), nullptr, nullptr, 0);
    int32_t rideAnimLoopRead = SFileReadFile(file, &this->m_rideAnimLoop, sizeof(this->m_rideAnimLoop), nullptr, nullptr, 0);
    int32_t rideUpperAnimStartRead = SFileReadFile(file, &this->m_rideUpperAnimStart, sizeof(this->m_rideUpperAnimStart), nullptr, nullptr, 0);
    int32_t rideUpperAnimLoopRead = SFileReadFile(file, &this->m_rideUpperAnimLoop, sizeof(this->m_rideUpperAnimLoop), nullptr, nullptr, 0);
    int32_t exitPreDelayRead = SFileReadFile(file, &this->m_exitPreDelay, sizeof(this->m_exitPreDelay), nullptr, nullptr, 0);
    int32_t exitSpeedRead = SFileReadFile(file, &this->m_exitSpeed, sizeof(this->m_exitSpeed), nullptr, nullptr, 0);
    int32_t exitGravityRead = SFileReadFile(file, &this->m_exitGravity, sizeof(this->m_exitGravity), nullptr, nullptr, 0);
    int32_t exitMinDurationRead = SFileReadFile(file, &this->m_exitMinDuration, sizeof(this->m_exitMinDuration), nullptr, nullptr, 0);
    int32_t exitMaxDurationRead = SFileReadFile(file, &this->m_exitMaxDuration, sizeof(this->m_exitMaxDuration), nullptr, nullptr, 0);
    int32_t exitMinArcHeightRead = SFileReadFile(file, &this->m_exitMinArcHeight, sizeof(this->m_exitMinArcHeight), nullptr, nullptr, 0);
    int32_t exitMaxArcHeightRead = SFileReadFile(file, &this->m_exitMaxArcHeight, sizeof(this->m_exitMaxArcHeight), nullptr, nullptr, 0);
    int32_t exitAnimStartRead = SFileReadFile(file, &this->m_exitAnimStart, sizeof(this->m_exitAnimStart), nullptr, nullptr, 0);
    int32_t exitAnimLoopRead = SFileReadFile(file, &this->m_exitAnimLoop, sizeof(this->m_exitAnimLoop), nullptr, nullptr, 0);
    int32_t exitAnimEndRead = SFileReadFile(file, &this->m_exitAnimEnd, sizeof(this->m_exitAnimEnd), nullptr, nullptr, 0);
    int32_t passengerYawRead = SFileReadFile(file, &this->m_passengerYaw, sizeof(this->m_passengerYaw), nullptr, nullptr, 0);
    int32_t passengerPitchRead = SFileReadFile(file, &this->m_passengerPitch, sizeof(this->m_passengerPitch), nullptr, nullptr, 0);
    int32_t passengerRollRead = SFileReadFile(file, &this->m_passengerRoll, sizeof(this->m_passengerRoll), nullptr, nullptr, 0);
    int32_t passengerAttachmentIDRead = SFileReadFile(file, &this->m_passengerAttachmentID, sizeof(this->m_passengerAttachmentID), nullptr, nullptr, 0);
    int32_t vehicleEnterAnimRead = SFileReadFile(file, &this->m_vehicleEnterAnim, sizeof(this->m_vehicleEnterAnim), nullptr, nullptr, 0);
    int32_t vehicleExitAnimRead = SFileReadFile(file, &this->m_vehicleExitAnim, sizeof(this->m_vehicleExitAnim), nullptr, nullptr, 0);
    int32_t vehicleRideAnimLoopRead = SFileReadFile(file, &this->m_vehicleRideAnimLoop, sizeof(this->m_vehicleRideAnimLoop), nullptr, nullptr, 0);
    int32_t field33512213034Read = SFileReadFile(file, &this->m_field33512213034, sizeof(this->m_field33512213034), nullptr, nullptr, 0);
    int32_t vehicleExitAnimBoneRead = SFileReadFile(file, &this->m_vehicleExitAnimBone, sizeof(this->m_vehicleExitAnimBone), nullptr, nullptr, 0);
    int32_t vehicleEnterAnimBoneRead = SFileReadFile(file, &this->m_vehicleEnterAnimBone, sizeof(this->m_vehicleEnterAnimBone), nullptr, nullptr, 0);
    int32_t enterTransitionAnimTimeoutRead = SFileReadFile(file, &this->m_enterTransitionAnimTimeout, sizeof(this->m_enterTransitionAnimTimeout), nullptr, nullptr, 0);
    int32_t exitTransitionAnimTimeoutRead = SFileReadFile(file, &this->m_exitTransitionAnimTimeout, sizeof(this->m_exitTransitionAnimTimeout), nullptr, nullptr, 0);
    int32_t vehicleAbilityDisplayRead = SFileReadFile(file, &this->m_vehicleAbilityDisplay, sizeof(this->m_vehicleAbilityDisplay), nullptr, nullptr, 0);
    int32_t enterUisoundIDRead = SFileReadFile(file, &this->m_enterUisoundID, sizeof(this->m_enterUisoundID), nullptr, nullptr, 0);
    int32_t exitUisoundIDRead = SFileReadFile(file, &this->m_exitUisoundID, sizeof(this->m_exitUisoundID), nullptr, nullptr, 0);
    int32_t uiSkinRead = SFileReadFile(file, &this->m_uiSkin, sizeof(this->m_uiSkin), nullptr, nullptr, 0);
    int32_t flagsBRead = SFileReadFile(file, &this->m_flagsB, sizeof(this->m_flagsB), nullptr, nullptr, 0);
    int32_t cameraEnterDelayRead = SFileReadFile(file, &this->m_cameraEnterDelay, sizeof(this->m_cameraEnterDelay), nullptr, nullptr, 0);
    int32_t cameraEnterDurationRead = SFileReadFile(file, &this->m_cameraEnterDuration, sizeof(this->m_cameraEnterDuration), nullptr, nullptr, 0);
    int32_t cameraExitDelayRead = SFileReadFile(file, &this->m_cameraExitDelay, sizeof(this->m_cameraExitDelay), nullptr, nullptr, 0);
    int32_t cameraExitDurationRead = SFileReadFile(file, &this->m_cameraExitDuration, sizeof(this->m_cameraExitDuration), nullptr, nullptr, 0);
    int32_t field33512213048Read = SFileReadFile(file, &this->m_field33512213048, sizeof(this->m_field33512213048), nullptr, nullptr, 0);
    int32_t field33512213049Read = SFileReadFile(file, &this->m_field33512213049, sizeof(this->m_field33512213049), nullptr, nullptr, 0);
    int32_t field33512213050Read = SFileReadFile(file, &this->m_field33512213050, sizeof(this->m_field33512213050), nullptr, nullptr, 0);
    int32_t cameraPosChaseRateRead = SFileReadFile(file, &this->m_cameraPosChaseRate, sizeof(this->m_cameraPosChaseRate), nullptr, nullptr, 0);
    int32_t cameraFacingChaseRateRead = SFileReadFile(file, &this->m_cameraFacingChaseRate, sizeof(this->m_cameraFacingChaseRate), nullptr, nullptr, 0);
    int32_t cameraEnteringZoomRead = SFileReadFile(file, &this->m_cameraEnteringZoom, sizeof(this->m_cameraEnteringZoom), nullptr, nullptr, 0);
    int32_t cameraSeatZoomMinRead = SFileReadFile(file, &this->m_cameraSeatZoomMin, sizeof(this->m_cameraSeatZoomMin), nullptr, nullptr, 0);
    int32_t cameraSeatZoomMaxRead = SFileReadFile(file, &this->m_cameraSeatZoomMax, sizeof(this->m_cameraSeatZoomMax), nullptr, nullptr, 0);

    if (!idRead
        | !flagsRead
        | !attachmentIDRead
        | !attachmentRead0
        | !attachmentRead1
        | !attachmentRead2
        | !enterPreDelayRead
        | !enterSpeedRead
        | !enterGravityRead
        | !enterMinDurationRead
        | !enterMaxDurationRead
        | !enterMinArcHeightRead
        | !enterMaxArcHeightRead
        | !enterAnimStartRead
        | !enterAnimLoopRead
        | !rideAnimStartRead
        | !rideAnimLoopRead
        | !rideUpperAnimStartRead
        | !rideUpperAnimLoopRead
        | !exitPreDelayRead
        | !exitSpeedRead
        | !exitGravityRead
        | !exitMinDurationRead
        | !exitMaxDurationRead
        | !exitMinArcHeightRead
        | !exitMaxArcHeightRead
        | !exitAnimStartRead
        | !exitAnimLoopRead
        | !exitAnimEndRead
        | !passengerYawRead
        | !passengerPitchRead
        | !passengerRollRead
        | !passengerAttachmentIDRead
        | !vehicleEnterAnimRead
        | !vehicleExitAnimRead
        | !vehicleRideAnimLoopRead
        | !field33512213034Read
        | !vehicleExitAnimBoneRead
        | !vehicleEnterAnimBoneRead
        | !enterTransitionAnimTimeoutRead
        | !exitTransitionAnimTimeoutRead
        | !vehicleAbilityDisplayRead
        | !enterUisoundIDRead
        | !exitUisoundIDRead
        | !uiSkinRead
        | !flagsBRead
        | !cameraEnterDelayRead
        | !cameraEnterDurationRead
        | !cameraExitDelayRead
        | !cameraExitDurationRead
        | !field33512213048Read
        | !field33512213049Read
        | !field33512213050Read
        | !cameraPosChaseRateRead
        | !cameraFacingChaseRateRead
        | !cameraEnteringZoomRead
        | !cameraSeatZoomMinRead
        | !cameraSeatZoomMaxRead) {
        return false;
    }

    return true;
}
