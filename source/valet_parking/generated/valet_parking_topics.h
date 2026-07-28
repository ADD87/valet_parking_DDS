/**************************************************************
* @file valet_parking_topics.h
* @copyright GREENSTONE TECHNOLOGY CO.,LTD. 2020-2025
* All rights reserved
**************************************************************/

#ifndef VALET_PARKING_TOPICS_35e2de1fdefcdc5c0930a3f5d111d289_H
#define VALET_PARKING_TOPICS_35e2de1fdefcdc5c0930a3f5d111d289_H

#include <stdint.h>
#include <vector>
#include <array>
#include <map>
#include <string>
#include "magnadds/MagnaDDS.h"




/**
* @enum class PsPointPosition
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class PsPointPosition : uint32_t
{
	PS_POSITION_TOP_LEFT,
	PS_POSITION_TOP_RIGHT,
	PS_POSITION_BOTTOM_LEFT,
	PS_POSITION_BOTTOM_RIGHT,
	PS_POSITION_STOP_LEFT,
	PS_POSITION_STOP_RIGHT
};

/**
* @enum class PsPointQuality
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class PsPointQuality : uint32_t
{
	PS_QUALITY_LOW,
	PS_QUALITY_HIGH
};

/**
* @enum class ParkingType
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class ParkingType : uint32_t
{
	PARKING_TYPE_VERTICAL,
	PARKING_TYPE_LATERAL,
	PARKING_TYPE_OBLIQUE,
	PARKING_TYPE_NONE
};

/**
* @enum class ParkingSpaceSize
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class ParkingSpaceSize : uint32_t
{
	PARKING_SPACE_SIZE_NORMAL,
	PARKING_SPACE_SIZE_SMALL,
	PARKING_SPACE_SIZE_NARROW
};

/**
* @enum class ParkingStatus
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class ParkingStatus : uint32_t
{
	PARKING_STATUS_FREE,
	PARKING_STATUS_OCCUPIED,
	PARKING_STATUS_UNKOWN
};

/**
* @enum class ParkingSensorType
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class ParkingSensorType : uint32_t
{
	PARKING_SENSOR_CAMERA,
	PARKING_SENSOR_MAP,
	PARKING_SENSOR_CAM_MAP_FUSION,
	PARKING_SENSOR_USS
};

/**
* @enum class GearPosition
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class GearPosition : uint32_t
{
	GEAR_NEUTRAL,
	GEAR_DRIVE,
	GEAR_REVERSE,
	GEAR_PARKING,
	GEAR_LOW,
	GEAR_INVALID,
	GEAR_NONE
};

/**
* @enum class ObstacleType
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class ObstacleType : uint32_t
{
	OBSTACLE_TYPE_UNKNOWN,
	OBSTACLE_TYPE_UNKNOWN_MOVABLE,
	OBSTACLE_TYPE_UNKNOWN_UNMOVABLE,
	OBSTACLE_TYPE_PEDESTRIAN,
	OBSTACLE_TYPE_BICYCLE,
	OBSTACLE_TYPE_VEHICLE
};

/**
* @enum class PlanningTrajectoryType
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class PlanningTrajectoryType : uint32_t
{
	TRAJECTORY_TYPE_UNKNOWN,
	TRAJECTORY_TYPE_NORMAL,
	TRAJECTORY_TYPE_PATH_FALLBACK,
	TRAJECTORY_TYPE_SPEED_FALLBACK,
	TRAJECTORY_TYPE_PATH_REUSED,
	TRAJECTORY_TYPE_SHORT_PATH,
	TRAJECTORY_TYPE_PATH_LANE_KEEP,
	TRAJECTORY_TYPE_PATH_LANE_CHANGE,
	TRAJECTORY_TYPE_PATH_PULL_OVER
};

/**
* @enum class ParkingCommandMode
* @brief A enum class as the datatype for data exchange.
* @note
*/

enum class ParkingCommandMode : uint32_t
{
	PARKING_COMMAND_NONE,
	PARKING_COMMAND_PARKING_IN,
	PARKING_COMMAND_PARKING_OUT_LEFT,
	PARKING_COMMAND_PARKING_OUT_RIGHT,
	PARKING_COMMAND_PARKING_OUT_FRONT,
	PARKING_COMMAND_PARKING_OUT_BACK,
	PARKING_COMMAND_DIRECT_FORWARD,
	PARKING_COMMAND_DIRECT_BACKWARD,
	PARKING_COMMAND_PAUSE,
	PARKING_COMMAND_BRAKE,
	PARKING_COMMAND_FINISH
};

/**
* @class Header
* @brief A class as the datatype for data exchange.
* @note
*/

class Header
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = 0U;
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	Header();
	~Header() = default;
	Header(Header const &x) = default;
	Header(Header &&x) = default;
	Header& operator=(Header const &x) = default;
	Header& operator=(Header &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(Header const* const _data) noexcept;



	void seq(uint64_t const _seq);
	uint64_t seq() const;
	uint64_t& seq();

	void frame_id(std::string const &_frame_id);
	void frame_id(std::string &&_frame_id);
	std::string const& frame_id() const;
	std::string& frame_id();

	void publish_stamp_ms(uint64_t const _publish_stamp_ms);
	uint64_t publish_stamp_ms() const;
	uint64_t& publish_stamp_ms();

	void data_stamp_ms(uint64_t const _data_stamp_ms);
	uint64_t data_stamp_ms() const;
	uint64_t& data_stamp_ms();





private:
	uint64_t m_seq;
	std::string m_frame_id;
	uint64_t m_publish_stamp_ms;
	uint64_t m_data_stamp_ms;

};


/**
* @class Point3D
* @brief A class as the datatype for data exchange.
* @note
*/

class Point3D
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(double) + sizeof(double) + sizeof(double);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	Point3D();
	~Point3D() = default;
	Point3D(Point3D const &x) = default;
	Point3D(Point3D &&x) = default;
	Point3D& operator=(Point3D const &x) = default;
	Point3D& operator=(Point3D &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(Point3D const* const _data) noexcept;



	void x(double const _x);
	double x() const;
	double& x();

	void y(double const _y);
	double y() const;
	double& y();

	void z(double const _z);
	double z() const;
	double& z();





private:
	double m_x;
	double m_y;
	double m_z;

};


/**
* @class PsPoint
* @brief A class as the datatype for data exchange.
* @note
*/

class PsPoint
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Point3D) + sizeof(PsPointPosition) + sizeof(PsPointQuality);
	static constexpr bool IS_DATA_PADDING = true || (Point3D::DATA_SIZE != sizeof(Point3D)) || (Point3D::IS_DATA_PADDING);
	static constexpr bool IS_ID_DEFINED = false;

	PsPoint();
	~PsPoint() = default;
	PsPoint(PsPoint const &x) = default;
	PsPoint(PsPoint &&x) = default;
	PsPoint& operator=(PsPoint const &x) = default;
	PsPoint& operator=(PsPoint &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(PsPoint const* const _data) noexcept;



	void point(Point3D const &_point);
	void point(Point3D &&_point);
	Point3D const& point() const;
	Point3D& point();

	void position(PsPointPosition const _position);
	PsPointPosition position() const;
	PsPointPosition& position();

	void quality(PsPointQuality const _quality);
	PsPointQuality quality() const;
	PsPointQuality& quality();





private:
	Point3D m_point;
	PsPointPosition m_position;
	PsPointQuality m_quality;

};


/**
* @class ParkingPathPoint
* @brief A class as the datatype for data exchange.
* @note
*/

class ParkingPathPoint
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(uint32_t);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	ParkingPathPoint();
	~ParkingPathPoint() = default;
	ParkingPathPoint(ParkingPathPoint const &x) = default;
	ParkingPathPoint(ParkingPathPoint &&x) = default;
	ParkingPathPoint& operator=(ParkingPathPoint const &x) = default;
	ParkingPathPoint& operator=(ParkingPathPoint &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(ParkingPathPoint const* const _data) noexcept;



	void x(double const _x);
	double x() const;
	double& x();

	void y(double const _y);
	double y() const;
	double& y();

	void z(double const _z);
	double z() const;
	double& z();

	void yaw(double const _yaw);
	double yaw() const;
	double& yaw();

	void accumulate_s(double const _accumulate_s);
	double accumulate_s() const;
	double& accumulate_s();

	void gear(uint32_t const _gear);
	uint32_t gear() const;
	uint32_t& gear();





private:
	double m_x;
	double m_y;
	double m_z;
	double m_yaw;
	double m_accumulate_s;
	uint32_t m_gear;

};


/**
* @class LocalizationEstimate
* @brief A class as the datatype for data exchange.
* @note
*/

class LocalizationEstimate
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Header) + sizeof(bool) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double);
	static constexpr bool IS_DATA_PADDING = true || (Header::DATA_SIZE != sizeof(Header)) || (Header::IS_DATA_PADDING);
	static constexpr bool IS_ID_DEFINED = false;

	LocalizationEstimate();
	~LocalizationEstimate() = default;
	LocalizationEstimate(LocalizationEstimate const &x) = default;
	LocalizationEstimate(LocalizationEstimate &&x) = default;
	LocalizationEstimate& operator=(LocalizationEstimate const &x) = default;
	LocalizationEstimate& operator=(LocalizationEstimate &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(LocalizationEstimate const* const _data) noexcept;



	void header(Header const &_header);
	void header(Header &&_header);
	Header const& header() const;
	Header& header();

	void is_valid(bool const _is_valid);
	bool is_valid() const;
	bool& is_valid();

	void x(double const _x);
	double x() const;
	double& x();

	void y(double const _y);
	double y() const;
	double& y();

	void z(double const _z);
	double z() const;
	double& z();

	void heading(double const _heading);
	double heading() const;
	double& heading();





private:
	Header m_header;
	bool m_is_valid;
	double m_x;
	double m_y;
	double m_z;
	double m_heading;

};


/**
* @class ChassisState
* @brief A class as the datatype for data exchange.
* @note
*/

class ChassisState
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Header) + sizeof(bool) + sizeof(double) + sizeof(double) + sizeof(GearPosition);
	static constexpr bool IS_DATA_PADDING = true || (Header::DATA_SIZE != sizeof(Header)) || (Header::IS_DATA_PADDING);
	static constexpr bool IS_ID_DEFINED = false;

	ChassisState();
	~ChassisState() = default;
	ChassisState(ChassisState const &x) = default;
	ChassisState(ChassisState &&x) = default;
	ChassisState& operator=(ChassisState const &x) = default;
	ChassisState& operator=(ChassisState &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(ChassisState const* const _data) noexcept;



	void header(Header const &_header);
	void header(Header &&_header);
	Header const& header() const;
	Header& header();

	void is_valid(bool const _is_valid);
	bool is_valid() const;
	bool& is_valid();

	void speed_mps(double const _speed_mps);
	double speed_mps() const;
	double& speed_mps();

	void acceleration_mps2(double const _acceleration_mps2);
	double acceleration_mps2() const;
	double& acceleration_mps2();

	void gear(GearPosition const _gear);
	GearPosition gear() const;
	GearPosition& gear();





private:
	Header m_header;
	bool m_is_valid;
	double m_speed_mps;
	double m_acceleration_mps2;
	GearPosition m_gear;

};


/**
* @class Obstacle
* @brief A class as the datatype for data exchange.
* @note
*/

class Obstacle
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(uint32_t) + sizeof(ObstacleType) + sizeof(bool) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	Obstacle();
	~Obstacle() = default;
	Obstacle(Obstacle const &x) = default;
	Obstacle(Obstacle &&x) = default;
	Obstacle& operator=(Obstacle const &x) = default;
	Obstacle& operator=(Obstacle &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(Obstacle const* const _data) noexcept;



	void id(uint32_t const _id);
	uint32_t id() const;
	uint32_t& id();

	void type(ObstacleType const _type);
	ObstacleType type() const;
	ObstacleType& type();

	void is_dynamic(bool const _is_dynamic);
	bool is_dynamic() const;
	bool& is_dynamic();

	void center_x(double const _center_x);
	double center_x() const;
	double& center_x();

	void center_y(double const _center_y);
	double center_y() const;
	double& center_y();

	void heading(double const _heading);
	double heading() const;
	double& heading();

	void length(double const _length);
	double length() const;
	double& length();

	void width(double const _width);
	double width() const;
	double& width();

	void velocity_x(double const _velocity_x);
	double velocity_x() const;
	double& velocity_x();

	void velocity_y(double const _velocity_y);
	double velocity_y() const;
	double& velocity_y();





private:
	uint32_t m_id;
	ObstacleType m_type;
	bool m_is_dynamic;
	double m_center_x;
	double m_center_y;
	double m_heading;
	double m_length;
	double m_width;
	double m_velocity_x;
	double m_velocity_y;

};


/**
* @class ObstacleArray
* @brief A class as the datatype for data exchange.
* @note
*/

class ObstacleArray
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = 0U;
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	ObstacleArray();
	~ObstacleArray() = default;
	ObstacleArray(ObstacleArray const &x) = default;
	ObstacleArray(ObstacleArray &&x) = default;
	ObstacleArray& operator=(ObstacleArray const &x) = default;
	ObstacleArray& operator=(ObstacleArray &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(ObstacleArray const* const _data) noexcept;



	void header(Header const &_header);
	void header(Header &&_header);
	Header const& header() const;
	Header& header();

	void is_valid(bool const _is_valid);
	bool is_valid() const;
	bool& is_valid();

	void obstacles(std::vector<Obstacle> const &_obstacles);
	void obstacles(std::vector<Obstacle> &&_obstacles);
	std::vector<Obstacle> const& obstacles() const;
	std::vector<Obstacle>& obstacles();





private:
	Header m_header;
	bool m_is_valid;
	std::vector<Obstacle> m_obstacles;

};


/**
* @class ParkingCommand
* @brief A class as the datatype for data exchange.
* @note
*/

class ParkingCommand
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = 0U;
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	ParkingCommand();
	~ParkingCommand() = default;
	ParkingCommand(ParkingCommand const &x) = default;
	ParkingCommand(ParkingCommand &&x) = default;
	ParkingCommand& operator=(ParkingCommand const &x) = default;
	ParkingCommand& operator=(ParkingCommand &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(ParkingCommand const* const _data) noexcept;



	void header(Header const &_header);
	void header(Header &&_header);
	Header const& header() const;
	Header& header();

	void is_valid(bool const _is_valid);
	bool is_valid() const;
	bool& is_valid();

	void mode(ParkingCommandMode const _mode);
	ParkingCommandMode mode() const;
	ParkingCommandMode& mode();

	void parking_seq(uint32_t const _parking_seq);
	uint32_t parking_seq() const;
	uint32_t& parking_seq();

	void direct_distance_m(double const _direct_distance_m);
	double direct_distance_m() const;
	double& direct_distance_m();

	void direct_speed_mps(double const _direct_speed_mps);
	double direct_speed_mps() const;
	double& direct_speed_mps();

	void reset_history(bool const _reset_history);
	bool reset_history() const;
	bool& reset_history();

	void reason(std::string const &_reason);
	void reason(std::string &&_reason);
	std::string const& reason() const;
	std::string& reason();





private:
	Header m_header;
	bool m_is_valid;
	ParkingCommandMode m_mode;
	uint32_t m_parking_seq;
	double m_direct_distance_m;
	double m_direct_speed_mps;
	bool m_reset_history;
	std::string m_reason;

};


/**
* @class ParkingLot
* @brief A class as the datatype for data exchange.
* @note
*/

class ParkingLot
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = 0U;
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	ParkingLot();
	~ParkingLot() = default;
	ParkingLot(ParkingLot const &x) = default;
	ParkingLot(ParkingLot &&x) = default;
	ParkingLot& operator=(ParkingLot const &x) = default;
	ParkingLot& operator=(ParkingLot &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(ParkingLot const* const _data) noexcept;



	void parking_seq(uint32_t const _parking_seq);
	uint32_t parking_seq() const;
	uint32_t& parking_seq();

	void type(ParkingType const _type);
	ParkingType type() const;
	ParkingType& type();

	void status(ParkingStatus const _status);
	ParkingStatus status() const;
	ParkingStatus& status();

	void sensor_type(ParkingSensorType const _sensor_type);
	ParkingSensorType sensor_type() const;
	ParkingSensorType& sensor_type();

	void is_private_ps(bool const _is_private_ps);
	bool is_private_ps() const;
	bool& is_private_ps();

	void pts_vrf(std::vector<PsPoint> const &_pts_vrf);
	void pts_vrf(std::vector<PsPoint> &&_pts_vrf);
	std::vector<PsPoint> const& pts_vrf() const;
	std::vector<PsPoint>& pts_vrf();

	void time_creation(double const _time_creation);
	double time_creation() const;
	double& time_creation();

	void pts_enu(std::vector<PsPoint> const &_pts_enu);
	void pts_enu(std::vector<PsPoint> &&_pts_enu);
	std::vector<PsPoint> const& pts_enu() const;
	std::vector<PsPoint>& pts_enu();

	void hmi_angle(double const _hmi_angle);
	double hmi_angle() const;
	double& hmi_angle();

	void hmi_depth(double const _hmi_depth);
	double hmi_depth() const;
	double& hmi_depth();

	void hmi_width(double const _hmi_width);
	double hmi_width() const;
	double& hmi_width();

	void hmi_direction(double const _hmi_direction);
	double hmi_direction() const;
	double& hmi_direction();

	void hmi_type(double const _hmi_type);
	double hmi_type() const;
	double& hmi_type();

	void hmi_status(double const _hmi_status);
	double hmi_status() const;
	double& hmi_status();

	void park_size(ParkingSpaceSize const _park_size);
	ParkingSpaceSize park_size() const;
	ParkingSpaceSize& park_size();





private:
	uint32_t m_parking_seq;
	ParkingType m_type;
	ParkingStatus m_status;
	ParkingSensorType m_sensor_type;
	bool m_is_private_ps;
	std::vector<PsPoint> m_pts_vrf;
	double m_time_creation;
	std::vector<PsPoint> m_pts_enu;
	double m_hmi_angle;
	double m_hmi_depth;
	double m_hmi_width;
	double m_hmi_direction;
	double m_hmi_type;
	double m_hmi_status;
	ParkingSpaceSize m_park_size;

};


/**
* @class SelectedSlot
* @brief A class as the datatype for data exchange.
* @note
*/

class SelectedSlot
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = 0U;
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	SelectedSlot();
	~SelectedSlot() = default;
	SelectedSlot(SelectedSlot const &x) = default;
	SelectedSlot(SelectedSlot &&x) = default;
	SelectedSlot& operator=(SelectedSlot const &x) = default;
	SelectedSlot& operator=(SelectedSlot &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(SelectedSlot const* const _data) noexcept;



	void header(Header const &_header);
	void header(Header &&_header);
	Header const& header() const;
	Header& header();

	void count(uint32_t const _count);
	uint32_t count() const;
	uint32_t& count();

	void loc_seq(uint32_t const _loc_seq);
	uint32_t loc_seq() const;
	uint32_t& loc_seq();

	void opt_parking_seq(uint32_t const _opt_parking_seq);
	uint32_t opt_parking_seq() const;
	uint32_t& opt_parking_seq();

	void parking_lots(std::vector<ParkingLot> const &_parking_lots);
	void parking_lots(std::vector<ParkingLot> &&_parking_lots);
	std::vector<ParkingLot> const& parking_lots() const;
	std::vector<ParkingLot>& parking_lots();

	void is_valid(bool const _is_valid);
	bool is_valid() const;
	bool& is_valid();

	void path_point_size(uint32_t const _path_point_size);
	uint32_t path_point_size() const;
	uint32_t& path_point_size();

	void traced_path(std::vector<ParkingPathPoint> const &_traced_path);
	void traced_path(std::vector<ParkingPathPoint> &&_traced_path);
	std::vector<ParkingPathPoint> const& traced_path() const;
	std::vector<ParkingPathPoint>& traced_path();

	void hpp_cruising_to_parking(bool const _hpp_cruising_to_parking);
	bool hpp_cruising_to_parking() const;
	bool& hpp_cruising_to_parking();

	void enable_trigger(bool const _enable_trigger);
	bool enable_trigger() const;
	bool& enable_trigger();





private:
	Header m_header;
	uint32_t m_count;
	uint32_t m_loc_seq;
	uint32_t m_opt_parking_seq;
	std::vector<ParkingLot> m_parking_lots;
	bool m_is_valid;
	uint32_t m_path_point_size;
	std::vector<ParkingPathPoint> m_traced_path;
	bool m_hpp_cruising_to_parking;
	bool m_enable_trigger;

};


/**
* @class PathPoint
* @brief A class as the datatype for data exchange.
* @note
*/

class PathPoint
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = 0U;
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	PathPoint();
	~PathPoint() = default;
	PathPoint(PathPoint const &x) = default;
	PathPoint(PathPoint &&x) = default;
	PathPoint& operator=(PathPoint const &x) = default;
	PathPoint& operator=(PathPoint &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(PathPoint const* const _data) noexcept;



	void x(double const _x);
	double x() const;
	double& x();

	void y(double const _y);
	double y() const;
	double& y();

	void z(double const _z);
	double z() const;
	double& z();

	void theta(double const _theta);
	double theta() const;
	double& theta();

	void kappa(double const _kappa);
	double kappa() const;
	double& kappa();

	void s(double const _s);
	double s() const;
	double& s();

	void l(double const _l);
	double l() const;
	double& l();

	void dkappa(double const _dkappa);
	double dkappa() const;
	double& dkappa();

	void ddkappa(double const _ddkappa);
	double ddkappa() const;
	double& ddkappa();

	void lane_id(std::string const &_lane_id);
	void lane_id(std::string &&_lane_id);
	std::string const& lane_id() const;
	std::string& lane_id();

	void x_derivative(double const _x_derivative);
	double x_derivative() const;
	double& x_derivative();

	void y_derivative(double const _y_derivative);
	double y_derivative() const;
	double& y_derivative();





private:
	double m_x;
	double m_y;
	double m_z;
	double m_theta;
	double m_kappa;
	double m_s;
	double m_l;
	double m_dkappa;
	double m_ddkappa;
	std::string m_lane_id;
	double m_x_derivative;
	double m_y_derivative;

};


/**
* @class GaussianInfo
* @brief A class as the datatype for data exchange.
* @note
*/

class GaussianInfo
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	GaussianInfo();
	~GaussianInfo() = default;
	GaussianInfo(GaussianInfo const &x) = default;
	GaussianInfo(GaussianInfo &&x) = default;
	GaussianInfo& operator=(GaussianInfo const &x) = default;
	GaussianInfo& operator=(GaussianInfo &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(GaussianInfo const* const _data) noexcept;



	void sigma_x(double const _sigma_x);
	double sigma_x() const;
	double& sigma_x();

	void sigma_y(double const _sigma_y);
	double sigma_y() const;
	double& sigma_y();

	void correlation(double const _correlation);
	double correlation() const;
	double& correlation();

	void area_probability(double const _area_probability);
	double area_probability() const;
	double& area_probability();

	void ellipse_a(double const _ellipse_a);
	double ellipse_a() const;
	double& ellipse_a();

	void ellipse_b(double const _ellipse_b);
	double ellipse_b() const;
	double& ellipse_b();

	void theta_a(double const _theta_a);
	double theta_a() const;
	double& theta_a();





private:
	double m_sigma_x;
	double m_sigma_y;
	double m_correlation;
	double m_area_probability;
	double m_ellipse_a;
	double m_ellipse_b;
	double m_theta_a;

};


/**
* @class TrajectoryPoint
* @brief A class as the datatype for data exchange.
* @note
*/

class TrajectoryPoint
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(PathPoint) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(GaussianInfo);
	static constexpr bool IS_DATA_PADDING = true || (PathPoint::DATA_SIZE != sizeof(PathPoint)) || (PathPoint::IS_DATA_PADDING) || (GaussianInfo::DATA_SIZE != sizeof(GaussianInfo)) || (GaussianInfo::IS_DATA_PADDING);
	static constexpr bool IS_ID_DEFINED = false;

	TrajectoryPoint();
	~TrajectoryPoint() = default;
	TrajectoryPoint(TrajectoryPoint const &x) = default;
	TrajectoryPoint(TrajectoryPoint &&x) = default;
	TrajectoryPoint& operator=(TrajectoryPoint const &x) = default;
	TrajectoryPoint& operator=(TrajectoryPoint &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(TrajectoryPoint const* const _data) noexcept;



	void path_point(PathPoint const &_path_point);
	void path_point(PathPoint &&_path_point);
	PathPoint const& path_point() const;
	PathPoint& path_point();

	void v(double const _v);
	double v() const;
	double& v();

	void a(double const _a);
	double a() const;
	double& a();

	void relative_time(double const _relative_time);
	double relative_time() const;
	double& relative_time();

	void da(double const _da);
	double da() const;
	double& da();

	void steer(double const _steer);
	double steer() const;
	double& steer();

	void gaussian_info(GaussianInfo const &_gaussian_info);
	void gaussian_info(GaussianInfo &&_gaussian_info);
	GaussianInfo const& gaussian_info() const;
	GaussianInfo& gaussian_info();





private:
	PathPoint m_path_point;
	double m_v;
	double m_a;
	double m_relative_time;
	double m_da;
	double m_steer;
	GaussianInfo m_gaussian_info;

};


/**
* @class EStop
* @brief A class as the datatype for data exchange.
* @note
*/

class EStop
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = 0U;
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	EStop();
	~EStop() = default;
	EStop(EStop const &x) = default;
	EStop(EStop &&x) = default;
	EStop& operator=(EStop const &x) = default;
	EStop& operator=(EStop &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(EStop const* const _data) noexcept;



	void is_estop(bool const _is_estop);
	bool is_estop() const;
	bool& is_estop();

	void reason(std::string const &_reason);
	void reason(std::string &&_reason);
	std::string const& reason() const;
	std::string& reason();





private:
	bool m_is_estop;
	std::string m_reason;

};


/**
* @class PlanningTrajectory
* @brief A class as the datatype for data exchange.
* @note
*/

class PlanningTrajectory
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = 0U;
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	PlanningTrajectory();
	~PlanningTrajectory() = default;
	PlanningTrajectory(PlanningTrajectory const &x) = default;
	PlanningTrajectory(PlanningTrajectory &&x) = default;
	PlanningTrajectory& operator=(PlanningTrajectory const &x) = default;
	PlanningTrajectory& operator=(PlanningTrajectory &&x) = default;

	magna::dds::DdsCdr& serialize(magna::dds::DdsCdr &cdr) const;
	static uint32_t serialize(void *const data, char *const payload_buf, uint32_t const payload_len);

	magna::dds::DdsCdr& deserialize(magna::dds::DdsCdr &cdr);
	static bool deserialize(char *const payload_buf, uint32_t const payload_len, void *const data);

	static bool is_key_defined();
	void serialize_key(magna::dds::DdsCdr &cdr) const;
	void serialize_key(char **buf,unsigned int *len);
	bool is_key_serialize_by_cdr();
	static bool is_plain_types();
	uint32_t max_align_size(uint32_t const _cur_al) const;
	void set_key_val(PlanningTrajectory const* const _data) noexcept;



	void header(Header const &_header);
	void header(Header &&_header);
	Header const& header() const;
	Header& header();

	void total_path_length(double const _total_path_length);
	double total_path_length() const;
	double& total_path_length();

	void total_path_time(double const _total_path_time);
	double total_path_time() const;
	double& total_path_time();

	void trajectory_point(std::vector<TrajectoryPoint> const &_trajectory_point);
	void trajectory_point(std::vector<TrajectoryPoint> &&_trajectory_point);
	std::vector<TrajectoryPoint> const& trajectory_point() const;
	std::vector<TrajectoryPoint>& trajectory_point();

	void is_replan(bool const _is_replan);
	bool is_replan() const;
	bool& is_replan();

	void replan_type(uint32_t const _replan_type);
	uint32_t replan_type() const;
	uint32_t& replan_type();

	void replan_reason(std::string const &_replan_reason);
	void replan_reason(std::string &&_replan_reason);
	std::string const& replan_reason() const;
	std::string& replan_reason();

	void longitudinal_diff(double const _longitudinal_diff);
	double longitudinal_diff() const;
	double& longitudinal_diff();

	void lateral_diff(double const _lateral_diff);
	double lateral_diff() const;
	double& lateral_diff();

	void gear(GearPosition const _gear);
	GearPosition gear() const;
	GearPosition& gear();

	void estop(EStop const &_estop);
	void estop(EStop &&_estop);
	EStop const& estop() const;
	EStop& estop();

	void trajectory_type(PlanningTrajectoryType const _trajectory_type);
	PlanningTrajectoryType trajectory_type() const;
	PlanningTrajectoryType& trajectory_type();





private:
	Header m_header;
	double m_total_path_length;
	double m_total_path_time;
	std::vector<TrajectoryPoint> m_trajectory_point;
	bool m_is_replan;
	uint32_t m_replan_type;
	std::string m_replan_reason;
	double m_longitudinal_diff;
	double m_lateral_diff;
	GearPosition m_gear;
	EStop m_estop;
	PlanningTrajectoryType m_trajectory_type;

};


#endif	// VALET_PARKING_TOPICS_35e2de1fdefcdc5c0930a3f5d111d289_H

