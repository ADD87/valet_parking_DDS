#pragma once

#include "valet_parking_c_api.h"
#include "valet_parking_stage_parking_adapter.h"
#include "magnadds/MagnaDDS.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

class PlanningTrajectory;
class PlanningTrajectoryTopicDataType;
class ParkingCommand;
class ParkingCommandTopicDataType;
class SelectedSlot;
class SelectedSlotTopicDataType;
class LocalizationEstimateTopicDataType;
class ChassisStateTopicDataType;
class ObstacleArrayTopicDataType;
class PrkVinBusTopicDataType;

namespace valet_parking {

class ValetParkingComponent final {
 public:
  explicit ValetParkingComponent(const valet_parking_config_t& config);
  ~ValetParkingComponent();

  ValetParkingComponent(const ValetParkingComponent&) = delete;
  ValetParkingComponent& operator=(const ValetParkingComponent&) = delete;

  int Start();
  int Stop();
  int UpdateVehicleState(const valet_parking_vehicle_state_t& vehicle_state);
  int ClearVehicleState();
  int UpdateObstacles(const valet_parking_obstacle_t* obstacles,
                      uint32_t obstacle_count);
  int ClearObstacles();

  const std::string& LastError() const;

 private:
    bool InitDds();
    void CleanupDds() noexcept;
    bool DrainCommandSamples();
    bool HandleCommandSample();
    bool DrainAuxInputSamples();
    bool HandleLocalizationSample();
    bool HandleChassisSample();
    bool HandleObstacleSample();
    bool HandlePrkVinBusSample();
    void ApplyAuxVehicleInput();
    bool HandleOneSample();
    bool BuildTrajectoryFromInput(const SelectedSlot& input_sample,
                                  PlanningTrajectory* output_sample,
                                  std::string* status_reason);
    static std::string ReturnCodeToString(magna::dds::ReturnCode_t rc);
    void SetLastError(const std::string& message);
  void WorkerLoop();

 private:
  valet_parking_config_t config_{};
  ValetParkingStageParkingAdapter stage_parking_adapter_;
    std::string input_topic_name_;
    std::string output_topic_name_;
    std::string command_topic_name_;
    std::string localization_topic_name_;
    std::string chassis_topic_name_;
    std::string obstacle_topic_name_;
    std::string prk_vin_bus_topic_name_;
    bool command_topic_enabled_{false};
    bool aux_input_topics_enabled_{false};
    bool prk_vin_bus_topic_enabled_{false};
  std::atomic<bool> running_{false};
  std::mutex mutex_;
  std::condition_variable cv_;
  std::thread worker_;
    std::string last_error_;
    uint64_t handled_samples_{0U};
    uint64_t handled_command_samples_{0U};
    uint64_t handled_localization_samples_{0U};
    uint64_t handled_chassis_samples_{0U};
    uint64_t handled_obstacle_samples_{0U};
    uint64_t handled_prk_vin_samples_{0U};

    struct AuxVehicleInputCache {
      bool has_localization{false};
      bool has_chassis{false};
      valet_parking_vehicle_state_t state{};
    };
    AuxVehicleInputCache aux_vehicle_input_{};
    std::unique_ptr<ParkingCommand> latest_parking_command_;

    std::unique_ptr<SelectedSlotTopicDataType> selected_slot_topic_type_;
    std::unique_ptr<PlanningTrajectoryTopicDataType> planning_trajectory_topic_type_;
    std::unique_ptr<ParkingCommandTopicDataType> command_topic_type_;
    std::unique_ptr<LocalizationEstimateTopicDataType> localization_topic_type_;
    std::unique_ptr<ChassisStateTopicDataType> chassis_topic_type_;
    std::unique_ptr<ObstacleArrayTopicDataType> obstacle_topic_type_;
    std::unique_ptr<PrkVinBusTopicDataType> prk_vin_bus_topic_type_;
    magna::dds::DomainParticipantFactory* dds_factory_{nullptr};
    magna::dds::DomainParticipant* participant_{nullptr};
    magna::dds::Topic* input_topic_{nullptr};
    magna::dds::Topic* output_topic_{nullptr};
    magna::dds::Topic* command_topic_{nullptr};
    magna::dds::Topic* localization_topic_{nullptr};
    magna::dds::Topic* chassis_topic_{nullptr};
    magna::dds::Topic* obstacle_topic_{nullptr};
    magna::dds::Topic* prk_vin_bus_topic_{nullptr};
    magna::dds::Subscriber* subscriber_{nullptr};
    magna::dds::Publisher* publisher_{nullptr};
    magna::dds::DataReader* input_reader_{nullptr};
    magna::dds::DataReader* command_reader_{nullptr};
    magna::dds::DataReader* localization_reader_{nullptr};
    magna::dds::DataReader* chassis_reader_{nullptr};
    magna::dds::DataReader* obstacle_reader_{nullptr};
    magna::dds::DataReader* prk_vin_bus_reader_{nullptr};
    magna::dds::DataWriter* output_writer_{nullptr};
};

}  // namespace valet_parking
