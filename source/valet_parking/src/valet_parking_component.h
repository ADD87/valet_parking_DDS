#pragma once

#include "valet_parking_c_api.h"
#include "magnadds/MagnaDDS.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace valet_parking {

class ValetParkingComponent final {
 public:
  explicit ValetParkingComponent(const valet_parking_config_t& config);
  ~ValetParkingComponent();

  ValetParkingComponent(const ValetParkingComponent&) = delete;
  ValetParkingComponent& operator=(const ValetParkingComponent&) = delete;

  int Start();
  int Stop();

  const std::string& LastError() const;

 private:
    bool InitDds();
    void CleanupDds() noexcept;
    bool HandleOneSample();
    bool BuildTrajectoryPayloadFromInput(const std::string& input_payload,
                                         std::string* output_payload,
                                         std::string* status_reason) const;
    static std::string ReturnCodeToString(magna::dds::ReturnCode_t rc);
    void SetLastError(const std::string& message);
  void WorkerLoop();

 private:
  valet_parking_config_t config_{};
    std::string input_topic_name_;
    std::string output_topic_name_;
  std::atomic<bool> running_{false};
  std::mutex mutex_;
  std::condition_variable cv_;
  std::thread worker_;
  std::string last_error_;
    uint64_t handled_samples_{0U};

    std::unique_ptr<magna::dds::TopicDataType_raw> raw_topic_type_;
    magna::dds::DomainParticipantFactory* dds_factory_{nullptr};
    magna::dds::DomainParticipant* participant_{nullptr};
    magna::dds::Topic* input_topic_{nullptr};
    magna::dds::Topic* output_topic_{nullptr};
    magna::dds::Subscriber* subscriber_{nullptr};
    magna::dds::Publisher* publisher_{nullptr};
    magna::dds::DataReader* input_reader_{nullptr};
    magna::dds::DataWriter* output_writer_{nullptr};
};

}  // namespace valet_parking
