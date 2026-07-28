/**************************************************************
* @file valet_parking_topicsTopicDataType.h
* @copyright GREENSTONE TECHNOLOGY CO.,LTD. 2020-2025
* All rights reserved
**************************************************************/

#ifndef VALET_PARKING_TOPICSTOPICDATATYPE_35e2de1fdefcdc5c0930a3f5d111d289_H
#define VALET_PARKING_TOPICSTOPICDATATYPE_35e2de1fdefcdc5c0930a3f5d111d289_H

#include "magnadds/MagnaDDS.h"

#include "valet_parking_topics.h"




/**
* @class HeaderTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class HeaderTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	HeaderTopicDataType();
	virtual ~HeaderTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class Point3DTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class Point3DTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	Point3DTopicDataType();
	virtual ~Point3DTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class PsPointTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class PsPointTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	PsPointTopicDataType();
	virtual ~PsPointTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class ParkingPathPointTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class ParkingPathPointTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	ParkingPathPointTopicDataType();
	virtual ~ParkingPathPointTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class LocalizationEstimateTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class LocalizationEstimateTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	LocalizationEstimateTopicDataType();
	virtual ~LocalizationEstimateTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class ChassisStateTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class ChassisStateTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	ChassisStateTopicDataType();
	virtual ~ChassisStateTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class ObstacleTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class ObstacleTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	ObstacleTopicDataType();
	virtual ~ObstacleTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class ObstacleArrayTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class ObstacleArrayTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	ObstacleArrayTopicDataType();
	virtual ~ObstacleArrayTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class ParkingCommandTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class ParkingCommandTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	ParkingCommandTopicDataType();
	virtual ~ParkingCommandTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class ParkingLotTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class ParkingLotTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	ParkingLotTopicDataType();
	virtual ~ParkingLotTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class SelectedSlotTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class SelectedSlotTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	SelectedSlotTopicDataType();
	virtual ~SelectedSlotTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class PathPointTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class PathPointTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	PathPointTopicDataType();
	virtual ~PathPointTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class GaussianInfoTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class GaussianInfoTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	GaussianInfoTopicDataType();
	virtual ~GaussianInfoTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class TrajectoryPointTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class TrajectoryPointTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	TrajectoryPointTopicDataType();
	virtual ~TrajectoryPointTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class EStopTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class EStopTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	EStopTopicDataType();
	virtual ~EStopTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class PlanningTrajectoryTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class PlanningTrajectoryTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	PlanningTrajectoryTopicDataType();
	virtual ~PlanningTrajectoryTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

#endif	// VALET_PARKING_TOPICSTOPICDATATYPE_35e2de1fdefcdc5c0930a3f5d111d289_H

