/**************************************************************
* @file valet_parking_topics.cpp
* @copyright GREENSTONE TECHNOLOGY CO.,LTD. 2020-2025
* All rights reserved
**************************************************************/

#include "valet_parking_topics.h"
//#include <iostream>

Header::Header()
{
	m_seq = 0;
	m_publish_stamp_ms = 0;
	m_data_stamp_ms = 0;

}

magna::dds::DdsCdr& Header::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_seq);
	cdr.serialize(m_frame_id);
	cdr.serialize(m_publish_stamp_ms);
	cdr.serialize(m_data_stamp_ms);

	return cdr;
}
uint32_t Header::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& Header::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_seq);
	cdr.deserialize(m_frame_id);
	cdr.deserialize(m_publish_stamp_ms);
	cdr.deserialize(m_data_stamp_ms);

	return cdr;
}
bool Header::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool Header::is_key_defined()
{
	return false;

}
void Header::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void Header::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool Header::is_key_serialize_by_cdr()
{
	return false;

}
bool Header::is_plain_types()
{
	return false;
}
uint32_t Header::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void Header::set_key_val(Header const* const _data) noexcept
{

}
void Header::seq(uint64_t const _seq)
{
	m_seq = _seq;
}
uint64_t Header::seq() const
{
	return m_seq;
}
uint64_t& Header::seq()
{
	return m_seq;
}

void Header::frame_id(std::string const &_frame_id)
{
	m_frame_id = _frame_id;
}
void Header::frame_id(std::string &&_frame_id)
{
	m_frame_id = std::move(_frame_id);
}
std::string const& Header::frame_id() const
{
	return m_frame_id;
}
std::string& Header::frame_id()
{
	return m_frame_id;
}

void Header::publish_stamp_ms(uint64_t const _publish_stamp_ms)
{
	m_publish_stamp_ms = _publish_stamp_ms;
}
uint64_t Header::publish_stamp_ms() const
{
	return m_publish_stamp_ms;
}
uint64_t& Header::publish_stamp_ms()
{
	return m_publish_stamp_ms;
}

void Header::data_stamp_ms(uint64_t const _data_stamp_ms)
{
	m_data_stamp_ms = _data_stamp_ms;
}
uint64_t Header::data_stamp_ms() const
{
	return m_data_stamp_ms;
}
uint64_t& Header::data_stamp_ms()
{
	return m_data_stamp_ms;
}

Point3D::Point3D()
{
	m_x = 0;
	m_y = 0;
	m_z = 0;

}

magna::dds::DdsCdr& Point3D::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_x);
	cdr.serialize(m_y);
	cdr.serialize(m_z);

	return cdr;
}
uint32_t Point3D::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& Point3D::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_x);
	cdr.deserialize(m_y);
	cdr.deserialize(m_z);

	return cdr;
}
bool Point3D::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool Point3D::is_key_defined()
{
	return false;

}
void Point3D::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void Point3D::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool Point3D::is_key_serialize_by_cdr()
{
	return false;

}
bool Point3D::is_plain_types()
{
	return true;
}
uint32_t Point3D::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void Point3D::set_key_val(Point3D const* const _data) noexcept
{

}
void Point3D::x(double const _x)
{
	m_x = _x;
}
double Point3D::x() const
{
	return m_x;
}
double& Point3D::x()
{
	return m_x;
}

void Point3D::y(double const _y)
{
	m_y = _y;
}
double Point3D::y() const
{
	return m_y;
}
double& Point3D::y()
{
	return m_y;
}

void Point3D::z(double const _z)
{
	m_z = _z;
}
double Point3D::z() const
{
	return m_z;
}
double& Point3D::z()
{
	return m_z;
}

PsPoint::PsPoint()
{

}

magna::dds::DdsCdr& PsPoint::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_point);
	cdr.serialize(m_position);
	cdr.serialize(m_quality);

	return cdr;
}
uint32_t PsPoint::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& PsPoint::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_point);
	cdr.deserialize(m_position);
	cdr.deserialize(m_quality);

	return cdr;
}
bool PsPoint::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool PsPoint::is_key_defined()
{
	return false;

}
void PsPoint::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void PsPoint::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool PsPoint::is_key_serialize_by_cdr()
{
	return false;

}
bool PsPoint::is_plain_types()
{
	bool b0 = Point3D::is_plain_types();

	return b0;
}
uint32_t PsPoint::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void PsPoint::set_key_val(PsPoint const* const _data) noexcept
{

}
void PsPoint::point(Point3D const &_point)
{
	m_point = _point;
}
void PsPoint::point(Point3D &&_point)
{
	m_point = std::move(_point);
}
Point3D const& PsPoint::point() const
{
	return m_point;
}
Point3D& PsPoint::point()
{
	return m_point;
}

void PsPoint::position(PsPointPosition const _position)
{
	m_position = _position;
}
PsPointPosition PsPoint::position() const
{
	return m_position;
}
PsPointPosition& PsPoint::position()
{
	return m_position;
}

void PsPoint::quality(PsPointQuality const _quality)
{
	m_quality = _quality;
}
PsPointQuality PsPoint::quality() const
{
	return m_quality;
}
PsPointQuality& PsPoint::quality()
{
	return m_quality;
}

ParkingPathPoint::ParkingPathPoint()
{
	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_yaw = 0;
	m_accumulate_s = 0;
	m_gear = 0;

}

magna::dds::DdsCdr& ParkingPathPoint::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_x);
	cdr.serialize(m_y);
	cdr.serialize(m_z);
	cdr.serialize(m_yaw);
	cdr.serialize(m_accumulate_s);
	cdr.serialize(m_gear);

	return cdr;
}
uint32_t ParkingPathPoint::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& ParkingPathPoint::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_x);
	cdr.deserialize(m_y);
	cdr.deserialize(m_z);
	cdr.deserialize(m_yaw);
	cdr.deserialize(m_accumulate_s);
	cdr.deserialize(m_gear);

	return cdr;
}
bool ParkingPathPoint::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool ParkingPathPoint::is_key_defined()
{
	return false;

}
void ParkingPathPoint::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void ParkingPathPoint::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool ParkingPathPoint::is_key_serialize_by_cdr()
{
	return false;

}
bool ParkingPathPoint::is_plain_types()
{
	return true;
}
uint32_t ParkingPathPoint::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void ParkingPathPoint::set_key_val(ParkingPathPoint const* const _data) noexcept
{

}
void ParkingPathPoint::x(double const _x)
{
	m_x = _x;
}
double ParkingPathPoint::x() const
{
	return m_x;
}
double& ParkingPathPoint::x()
{
	return m_x;
}

void ParkingPathPoint::y(double const _y)
{
	m_y = _y;
}
double ParkingPathPoint::y() const
{
	return m_y;
}
double& ParkingPathPoint::y()
{
	return m_y;
}

void ParkingPathPoint::z(double const _z)
{
	m_z = _z;
}
double ParkingPathPoint::z() const
{
	return m_z;
}
double& ParkingPathPoint::z()
{
	return m_z;
}

void ParkingPathPoint::yaw(double const _yaw)
{
	m_yaw = _yaw;
}
double ParkingPathPoint::yaw() const
{
	return m_yaw;
}
double& ParkingPathPoint::yaw()
{
	return m_yaw;
}

void ParkingPathPoint::accumulate_s(double const _accumulate_s)
{
	m_accumulate_s = _accumulate_s;
}
double ParkingPathPoint::accumulate_s() const
{
	return m_accumulate_s;
}
double& ParkingPathPoint::accumulate_s()
{
	return m_accumulate_s;
}

void ParkingPathPoint::gear(uint32_t const _gear)
{
	m_gear = _gear;
}
uint32_t ParkingPathPoint::gear() const
{
	return m_gear;
}
uint32_t& ParkingPathPoint::gear()
{
	return m_gear;
}

LocalizationEstimate::LocalizationEstimate()
{
	m_is_valid = 0;
	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_heading = 0;

}

magna::dds::DdsCdr& LocalizationEstimate::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_header);
	cdr.serialize(m_is_valid);
	cdr.serialize(m_x);
	cdr.serialize(m_y);
	cdr.serialize(m_z);
	cdr.serialize(m_heading);

	return cdr;
}
uint32_t LocalizationEstimate::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& LocalizationEstimate::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_header);
	cdr.deserialize(m_is_valid);
	cdr.deserialize(m_x);
	cdr.deserialize(m_y);
	cdr.deserialize(m_z);
	cdr.deserialize(m_heading);

	return cdr;
}
bool LocalizationEstimate::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool LocalizationEstimate::is_key_defined()
{
	return false;

}
void LocalizationEstimate::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void LocalizationEstimate::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool LocalizationEstimate::is_key_serialize_by_cdr()
{
	return false;

}
bool LocalizationEstimate::is_plain_types()
{
	bool b0 = Header::is_plain_types();

	return b0;
}
uint32_t LocalizationEstimate::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void LocalizationEstimate::set_key_val(LocalizationEstimate const* const _data) noexcept
{

}
void LocalizationEstimate::header(Header const &_header)
{
	m_header = _header;
}
void LocalizationEstimate::header(Header &&_header)
{
	m_header = std::move(_header);
}
Header const& LocalizationEstimate::header() const
{
	return m_header;
}
Header& LocalizationEstimate::header()
{
	return m_header;
}

void LocalizationEstimate::is_valid(bool const _is_valid)
{
	m_is_valid = _is_valid;
}
bool LocalizationEstimate::is_valid() const
{
	return m_is_valid;
}
bool& LocalizationEstimate::is_valid()
{
	return m_is_valid;
}

void LocalizationEstimate::x(double const _x)
{
	m_x = _x;
}
double LocalizationEstimate::x() const
{
	return m_x;
}
double& LocalizationEstimate::x()
{
	return m_x;
}

void LocalizationEstimate::y(double const _y)
{
	m_y = _y;
}
double LocalizationEstimate::y() const
{
	return m_y;
}
double& LocalizationEstimate::y()
{
	return m_y;
}

void LocalizationEstimate::z(double const _z)
{
	m_z = _z;
}
double LocalizationEstimate::z() const
{
	return m_z;
}
double& LocalizationEstimate::z()
{
	return m_z;
}

void LocalizationEstimate::heading(double const _heading)
{
	m_heading = _heading;
}
double LocalizationEstimate::heading() const
{
	return m_heading;
}
double& LocalizationEstimate::heading()
{
	return m_heading;
}

ChassisState::ChassisState()
{
	m_is_valid = 0;
	m_speed_mps = 0;
	m_acceleration_mps2 = 0;

}

magna::dds::DdsCdr& ChassisState::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_header);
	cdr.serialize(m_is_valid);
	cdr.serialize(m_speed_mps);
	cdr.serialize(m_acceleration_mps2);
	cdr.serialize(m_gear);

	return cdr;
}
uint32_t ChassisState::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& ChassisState::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_header);
	cdr.deserialize(m_is_valid);
	cdr.deserialize(m_speed_mps);
	cdr.deserialize(m_acceleration_mps2);
	cdr.deserialize(m_gear);

	return cdr;
}
bool ChassisState::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool ChassisState::is_key_defined()
{
	return false;

}
void ChassisState::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void ChassisState::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool ChassisState::is_key_serialize_by_cdr()
{
	return false;

}
bool ChassisState::is_plain_types()
{
	bool b0 = Header::is_plain_types();

	return b0;
}
uint32_t ChassisState::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void ChassisState::set_key_val(ChassisState const* const _data) noexcept
{

}
void ChassisState::header(Header const &_header)
{
	m_header = _header;
}
void ChassisState::header(Header &&_header)
{
	m_header = std::move(_header);
}
Header const& ChassisState::header() const
{
	return m_header;
}
Header& ChassisState::header()
{
	return m_header;
}

void ChassisState::is_valid(bool const _is_valid)
{
	m_is_valid = _is_valid;
}
bool ChassisState::is_valid() const
{
	return m_is_valid;
}
bool& ChassisState::is_valid()
{
	return m_is_valid;
}

void ChassisState::speed_mps(double const _speed_mps)
{
	m_speed_mps = _speed_mps;
}
double ChassisState::speed_mps() const
{
	return m_speed_mps;
}
double& ChassisState::speed_mps()
{
	return m_speed_mps;
}

void ChassisState::acceleration_mps2(double const _acceleration_mps2)
{
	m_acceleration_mps2 = _acceleration_mps2;
}
double ChassisState::acceleration_mps2() const
{
	return m_acceleration_mps2;
}
double& ChassisState::acceleration_mps2()
{
	return m_acceleration_mps2;
}

void ChassisState::gear(GearPosition const _gear)
{
	m_gear = _gear;
}
GearPosition ChassisState::gear() const
{
	return m_gear;
}
GearPosition& ChassisState::gear()
{
	return m_gear;
}

Obstacle::Obstacle()
{
	m_id = 0;
	m_is_dynamic = 0;
	m_center_x = 0;
	m_center_y = 0;
	m_heading = 0;
	m_length = 0;
	m_width = 0;
	m_velocity_x = 0;
	m_velocity_y = 0;

}

magna::dds::DdsCdr& Obstacle::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_id);
	cdr.serialize(m_type);
	cdr.serialize(m_is_dynamic);
	cdr.serialize(m_center_x);
	cdr.serialize(m_center_y);
	cdr.serialize(m_heading);
	cdr.serialize(m_length);
	cdr.serialize(m_width);
	cdr.serialize(m_velocity_x);
	cdr.serialize(m_velocity_y);

	return cdr;
}
uint32_t Obstacle::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& Obstacle::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_id);
	cdr.deserialize(m_type);
	cdr.deserialize(m_is_dynamic);
	cdr.deserialize(m_center_x);
	cdr.deserialize(m_center_y);
	cdr.deserialize(m_heading);
	cdr.deserialize(m_length);
	cdr.deserialize(m_width);
	cdr.deserialize(m_velocity_x);
	cdr.deserialize(m_velocity_y);

	return cdr;
}
bool Obstacle::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool Obstacle::is_key_defined()
{
	return false;

}
void Obstacle::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void Obstacle::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool Obstacle::is_key_serialize_by_cdr()
{
	return false;

}
bool Obstacle::is_plain_types()
{
	return true;
}
uint32_t Obstacle::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void Obstacle::set_key_val(Obstacle const* const _data) noexcept
{

}
void Obstacle::id(uint32_t const _id)
{
	m_id = _id;
}
uint32_t Obstacle::id() const
{
	return m_id;
}
uint32_t& Obstacle::id()
{
	return m_id;
}

void Obstacle::type(ObstacleType const _type)
{
	m_type = _type;
}
ObstacleType Obstacle::type() const
{
	return m_type;
}
ObstacleType& Obstacle::type()
{
	return m_type;
}

void Obstacle::is_dynamic(bool const _is_dynamic)
{
	m_is_dynamic = _is_dynamic;
}
bool Obstacle::is_dynamic() const
{
	return m_is_dynamic;
}
bool& Obstacle::is_dynamic()
{
	return m_is_dynamic;
}

void Obstacle::center_x(double const _center_x)
{
	m_center_x = _center_x;
}
double Obstacle::center_x() const
{
	return m_center_x;
}
double& Obstacle::center_x()
{
	return m_center_x;
}

void Obstacle::center_y(double const _center_y)
{
	m_center_y = _center_y;
}
double Obstacle::center_y() const
{
	return m_center_y;
}
double& Obstacle::center_y()
{
	return m_center_y;
}

void Obstacle::heading(double const _heading)
{
	m_heading = _heading;
}
double Obstacle::heading() const
{
	return m_heading;
}
double& Obstacle::heading()
{
	return m_heading;
}

void Obstacle::length(double const _length)
{
	m_length = _length;
}
double Obstacle::length() const
{
	return m_length;
}
double& Obstacle::length()
{
	return m_length;
}

void Obstacle::width(double const _width)
{
	m_width = _width;
}
double Obstacle::width() const
{
	return m_width;
}
double& Obstacle::width()
{
	return m_width;
}

void Obstacle::velocity_x(double const _velocity_x)
{
	m_velocity_x = _velocity_x;
}
double Obstacle::velocity_x() const
{
	return m_velocity_x;
}
double& Obstacle::velocity_x()
{
	return m_velocity_x;
}

void Obstacle::velocity_y(double const _velocity_y)
{
	m_velocity_y = _velocity_y;
}
double Obstacle::velocity_y() const
{
	return m_velocity_y;
}
double& Obstacle::velocity_y()
{
	return m_velocity_y;
}

ObstacleArray::ObstacleArray()
{
	m_is_valid = 0;

}

magna::dds::DdsCdr& ObstacleArray::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_header);
	cdr.serialize(m_is_valid);
	cdr.serialize(m_obstacles);

	return cdr;
}
uint32_t ObstacleArray::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& ObstacleArray::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_header);
	cdr.deserialize(m_is_valid);
	cdr.deserialize(m_obstacles);

	return cdr;
}
bool ObstacleArray::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool ObstacleArray::is_key_defined()
{
	return false;

}
void ObstacleArray::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void ObstacleArray::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool ObstacleArray::is_key_serialize_by_cdr()
{
	return false;

}
bool ObstacleArray::is_plain_types()
{
	return false;
}
uint32_t ObstacleArray::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void ObstacleArray::set_key_val(ObstacleArray const* const _data) noexcept
{

}
void ObstacleArray::header(Header const &_header)
{
	m_header = _header;
}
void ObstacleArray::header(Header &&_header)
{
	m_header = std::move(_header);
}
Header const& ObstacleArray::header() const
{
	return m_header;
}
Header& ObstacleArray::header()
{
	return m_header;
}

void ObstacleArray::is_valid(bool const _is_valid)
{
	m_is_valid = _is_valid;
}
bool ObstacleArray::is_valid() const
{
	return m_is_valid;
}
bool& ObstacleArray::is_valid()
{
	return m_is_valid;
}

void ObstacleArray::obstacles(std::vector<Obstacle> const &_obstacles)
{
	m_obstacles = _obstacles;
}
void ObstacleArray::obstacles(std::vector<Obstacle> &&_obstacles)
{
	m_obstacles = std::move(_obstacles);
}
std::vector<Obstacle> const& ObstacleArray::obstacles() const
{
	return m_obstacles;
}
std::vector<Obstacle>& ObstacleArray::obstacles()
{
	return m_obstacles;
}

ParkingLot::ParkingLot()
{
	m_parking_seq = 0;
	m_is_private_ps = 0;
	m_time_creation = 0;
	m_hmi_angle = 0;
	m_hmi_depth = 0;
	m_hmi_width = 0;
	m_hmi_direction = 0;
	m_hmi_type = 0;
	m_hmi_status = 0;

}

magna::dds::DdsCdr& ParkingLot::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_parking_seq);
	cdr.serialize(m_type);
	cdr.serialize(m_status);
	cdr.serialize(m_sensor_type);
	cdr.serialize(m_is_private_ps);
	cdr.serialize(m_pts_vrf);
	cdr.serialize(m_time_creation);
	cdr.serialize(m_pts_enu);
	cdr.serialize(m_hmi_angle);
	cdr.serialize(m_hmi_depth);
	cdr.serialize(m_hmi_width);
	cdr.serialize(m_hmi_direction);
	cdr.serialize(m_hmi_type);
	cdr.serialize(m_hmi_status);
	cdr.serialize(m_park_size);

	return cdr;
}
uint32_t ParkingLot::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& ParkingLot::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_parking_seq);
	cdr.deserialize(m_type);
	cdr.deserialize(m_status);
	cdr.deserialize(m_sensor_type);
	cdr.deserialize(m_is_private_ps);
	cdr.deserialize(m_pts_vrf);
	cdr.deserialize(m_time_creation);
	cdr.deserialize(m_pts_enu);
	cdr.deserialize(m_hmi_angle);
	cdr.deserialize(m_hmi_depth);
	cdr.deserialize(m_hmi_width);
	cdr.deserialize(m_hmi_direction);
	cdr.deserialize(m_hmi_type);
	cdr.deserialize(m_hmi_status);
	cdr.deserialize(m_park_size);

	return cdr;
}
bool ParkingLot::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool ParkingLot::is_key_defined()
{
	return false;

}
void ParkingLot::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void ParkingLot::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool ParkingLot::is_key_serialize_by_cdr()
{
	return false;

}
bool ParkingLot::is_plain_types()
{
	return false;
}
uint32_t ParkingLot::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void ParkingLot::set_key_val(ParkingLot const* const _data) noexcept
{

}
void ParkingLot::parking_seq(uint32_t const _parking_seq)
{
	m_parking_seq = _parking_seq;
}
uint32_t ParkingLot::parking_seq() const
{
	return m_parking_seq;
}
uint32_t& ParkingLot::parking_seq()
{
	return m_parking_seq;
}

void ParkingLot::type(ParkingType const _type)
{
	m_type = _type;
}
ParkingType ParkingLot::type() const
{
	return m_type;
}
ParkingType& ParkingLot::type()
{
	return m_type;
}

void ParkingLot::status(ParkingStatus const _status)
{
	m_status = _status;
}
ParkingStatus ParkingLot::status() const
{
	return m_status;
}
ParkingStatus& ParkingLot::status()
{
	return m_status;
}

void ParkingLot::sensor_type(ParkingSensorType const _sensor_type)
{
	m_sensor_type = _sensor_type;
}
ParkingSensorType ParkingLot::sensor_type() const
{
	return m_sensor_type;
}
ParkingSensorType& ParkingLot::sensor_type()
{
	return m_sensor_type;
}

void ParkingLot::is_private_ps(bool const _is_private_ps)
{
	m_is_private_ps = _is_private_ps;
}
bool ParkingLot::is_private_ps() const
{
	return m_is_private_ps;
}
bool& ParkingLot::is_private_ps()
{
	return m_is_private_ps;
}

void ParkingLot::pts_vrf(std::vector<PsPoint> const &_pts_vrf)
{
	m_pts_vrf = _pts_vrf;
}
void ParkingLot::pts_vrf(std::vector<PsPoint> &&_pts_vrf)
{
	m_pts_vrf = std::move(_pts_vrf);
}
std::vector<PsPoint> const& ParkingLot::pts_vrf() const
{
	return m_pts_vrf;
}
std::vector<PsPoint>& ParkingLot::pts_vrf()
{
	return m_pts_vrf;
}

void ParkingLot::time_creation(double const _time_creation)
{
	m_time_creation = _time_creation;
}
double ParkingLot::time_creation() const
{
	return m_time_creation;
}
double& ParkingLot::time_creation()
{
	return m_time_creation;
}

void ParkingLot::pts_enu(std::vector<PsPoint> const &_pts_enu)
{
	m_pts_enu = _pts_enu;
}
void ParkingLot::pts_enu(std::vector<PsPoint> &&_pts_enu)
{
	m_pts_enu = std::move(_pts_enu);
}
std::vector<PsPoint> const& ParkingLot::pts_enu() const
{
	return m_pts_enu;
}
std::vector<PsPoint>& ParkingLot::pts_enu()
{
	return m_pts_enu;
}

void ParkingLot::hmi_angle(double const _hmi_angle)
{
	m_hmi_angle = _hmi_angle;
}
double ParkingLot::hmi_angle() const
{
	return m_hmi_angle;
}
double& ParkingLot::hmi_angle()
{
	return m_hmi_angle;
}

void ParkingLot::hmi_depth(double const _hmi_depth)
{
	m_hmi_depth = _hmi_depth;
}
double ParkingLot::hmi_depth() const
{
	return m_hmi_depth;
}
double& ParkingLot::hmi_depth()
{
	return m_hmi_depth;
}

void ParkingLot::hmi_width(double const _hmi_width)
{
	m_hmi_width = _hmi_width;
}
double ParkingLot::hmi_width() const
{
	return m_hmi_width;
}
double& ParkingLot::hmi_width()
{
	return m_hmi_width;
}

void ParkingLot::hmi_direction(double const _hmi_direction)
{
	m_hmi_direction = _hmi_direction;
}
double ParkingLot::hmi_direction() const
{
	return m_hmi_direction;
}
double& ParkingLot::hmi_direction()
{
	return m_hmi_direction;
}

void ParkingLot::hmi_type(double const _hmi_type)
{
	m_hmi_type = _hmi_type;
}
double ParkingLot::hmi_type() const
{
	return m_hmi_type;
}
double& ParkingLot::hmi_type()
{
	return m_hmi_type;
}

void ParkingLot::hmi_status(double const _hmi_status)
{
	m_hmi_status = _hmi_status;
}
double ParkingLot::hmi_status() const
{
	return m_hmi_status;
}
double& ParkingLot::hmi_status()
{
	return m_hmi_status;
}

void ParkingLot::park_size(ParkingSpaceSize const _park_size)
{
	m_park_size = _park_size;
}
ParkingSpaceSize ParkingLot::park_size() const
{
	return m_park_size;
}
ParkingSpaceSize& ParkingLot::park_size()
{
	return m_park_size;
}

SelectedSlot::SelectedSlot()
{
	m_count = 0;
	m_loc_seq = 0;
	m_opt_parking_seq = 0;
	m_is_valid = 0;
	m_path_point_size = 0;
	m_hpp_cruising_to_parking = 0;
	m_enable_trigger = 0;

}

magna::dds::DdsCdr& SelectedSlot::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_header);
	cdr.serialize(m_count);
	cdr.serialize(m_loc_seq);
	cdr.serialize(m_opt_parking_seq);
	cdr.serialize(m_parking_lots);
	cdr.serialize(m_is_valid);
	cdr.serialize(m_path_point_size);
	cdr.serialize(m_traced_path);
	cdr.serialize(m_hpp_cruising_to_parking);
	cdr.serialize(m_enable_trigger);

	return cdr;
}
uint32_t SelectedSlot::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& SelectedSlot::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_header);
	cdr.deserialize(m_count);
	cdr.deserialize(m_loc_seq);
	cdr.deserialize(m_opt_parking_seq);
	cdr.deserialize(m_parking_lots);
	cdr.deserialize(m_is_valid);
	cdr.deserialize(m_path_point_size);
	cdr.deserialize(m_traced_path);
	cdr.deserialize(m_hpp_cruising_to_parking);
	cdr.deserialize(m_enable_trigger);

	return cdr;
}
bool SelectedSlot::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool SelectedSlot::is_key_defined()
{
	return false;

}
void SelectedSlot::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void SelectedSlot::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool SelectedSlot::is_key_serialize_by_cdr()
{
	return false;

}
bool SelectedSlot::is_plain_types()
{
	return false;
}
uint32_t SelectedSlot::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void SelectedSlot::set_key_val(SelectedSlot const* const _data) noexcept
{

}
void SelectedSlot::header(Header const &_header)
{
	m_header = _header;
}
void SelectedSlot::header(Header &&_header)
{
	m_header = std::move(_header);
}
Header const& SelectedSlot::header() const
{
	return m_header;
}
Header& SelectedSlot::header()
{
	return m_header;
}

void SelectedSlot::count(uint32_t const _count)
{
	m_count = _count;
}
uint32_t SelectedSlot::count() const
{
	return m_count;
}
uint32_t& SelectedSlot::count()
{
	return m_count;
}

void SelectedSlot::loc_seq(uint32_t const _loc_seq)
{
	m_loc_seq = _loc_seq;
}
uint32_t SelectedSlot::loc_seq() const
{
	return m_loc_seq;
}
uint32_t& SelectedSlot::loc_seq()
{
	return m_loc_seq;
}

void SelectedSlot::opt_parking_seq(uint32_t const _opt_parking_seq)
{
	m_opt_parking_seq = _opt_parking_seq;
}
uint32_t SelectedSlot::opt_parking_seq() const
{
	return m_opt_parking_seq;
}
uint32_t& SelectedSlot::opt_parking_seq()
{
	return m_opt_parking_seq;
}

void SelectedSlot::parking_lots(std::vector<ParkingLot> const &_parking_lots)
{
	m_parking_lots = _parking_lots;
}
void SelectedSlot::parking_lots(std::vector<ParkingLot> &&_parking_lots)
{
	m_parking_lots = std::move(_parking_lots);
}
std::vector<ParkingLot> const& SelectedSlot::parking_lots() const
{
	return m_parking_lots;
}
std::vector<ParkingLot>& SelectedSlot::parking_lots()
{
	return m_parking_lots;
}

void SelectedSlot::is_valid(bool const _is_valid)
{
	m_is_valid = _is_valid;
}
bool SelectedSlot::is_valid() const
{
	return m_is_valid;
}
bool& SelectedSlot::is_valid()
{
	return m_is_valid;
}

void SelectedSlot::path_point_size(uint32_t const _path_point_size)
{
	m_path_point_size = _path_point_size;
}
uint32_t SelectedSlot::path_point_size() const
{
	return m_path_point_size;
}
uint32_t& SelectedSlot::path_point_size()
{
	return m_path_point_size;
}

void SelectedSlot::traced_path(std::vector<ParkingPathPoint> const &_traced_path)
{
	m_traced_path = _traced_path;
}
void SelectedSlot::traced_path(std::vector<ParkingPathPoint> &&_traced_path)
{
	m_traced_path = std::move(_traced_path);
}
std::vector<ParkingPathPoint> const& SelectedSlot::traced_path() const
{
	return m_traced_path;
}
std::vector<ParkingPathPoint>& SelectedSlot::traced_path()
{
	return m_traced_path;
}

void SelectedSlot::hpp_cruising_to_parking(bool const _hpp_cruising_to_parking)
{
	m_hpp_cruising_to_parking = _hpp_cruising_to_parking;
}
bool SelectedSlot::hpp_cruising_to_parking() const
{
	return m_hpp_cruising_to_parking;
}
bool& SelectedSlot::hpp_cruising_to_parking()
{
	return m_hpp_cruising_to_parking;
}

void SelectedSlot::enable_trigger(bool const _enable_trigger)
{
	m_enable_trigger = _enable_trigger;
}
bool SelectedSlot::enable_trigger() const
{
	return m_enable_trigger;
}
bool& SelectedSlot::enable_trigger()
{
	return m_enable_trigger;
}

PathPoint::PathPoint()
{
	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_theta = 0;
	m_kappa = 0;
	m_s = 0;
	m_l = 0;
	m_dkappa = 0;
	m_ddkappa = 0;
	m_x_derivative = 0;
	m_y_derivative = 0;

}

magna::dds::DdsCdr& PathPoint::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_x);
	cdr.serialize(m_y);
	cdr.serialize(m_z);
	cdr.serialize(m_theta);
	cdr.serialize(m_kappa);
	cdr.serialize(m_s);
	cdr.serialize(m_l);
	cdr.serialize(m_dkappa);
	cdr.serialize(m_ddkappa);
	cdr.serialize(m_lane_id);
	cdr.serialize(m_x_derivative);
	cdr.serialize(m_y_derivative);

	return cdr;
}
uint32_t PathPoint::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& PathPoint::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_x);
	cdr.deserialize(m_y);
	cdr.deserialize(m_z);
	cdr.deserialize(m_theta);
	cdr.deserialize(m_kappa);
	cdr.deserialize(m_s);
	cdr.deserialize(m_l);
	cdr.deserialize(m_dkappa);
	cdr.deserialize(m_ddkappa);
	cdr.deserialize(m_lane_id);
	cdr.deserialize(m_x_derivative);
	cdr.deserialize(m_y_derivative);

	return cdr;
}
bool PathPoint::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool PathPoint::is_key_defined()
{
	return false;

}
void PathPoint::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void PathPoint::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool PathPoint::is_key_serialize_by_cdr()
{
	return false;

}
bool PathPoint::is_plain_types()
{
	return false;
}
uint32_t PathPoint::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void PathPoint::set_key_val(PathPoint const* const _data) noexcept
{

}
void PathPoint::x(double const _x)
{
	m_x = _x;
}
double PathPoint::x() const
{
	return m_x;
}
double& PathPoint::x()
{
	return m_x;
}

void PathPoint::y(double const _y)
{
	m_y = _y;
}
double PathPoint::y() const
{
	return m_y;
}
double& PathPoint::y()
{
	return m_y;
}

void PathPoint::z(double const _z)
{
	m_z = _z;
}
double PathPoint::z() const
{
	return m_z;
}
double& PathPoint::z()
{
	return m_z;
}

void PathPoint::theta(double const _theta)
{
	m_theta = _theta;
}
double PathPoint::theta() const
{
	return m_theta;
}
double& PathPoint::theta()
{
	return m_theta;
}

void PathPoint::kappa(double const _kappa)
{
	m_kappa = _kappa;
}
double PathPoint::kappa() const
{
	return m_kappa;
}
double& PathPoint::kappa()
{
	return m_kappa;
}

void PathPoint::s(double const _s)
{
	m_s = _s;
}
double PathPoint::s() const
{
	return m_s;
}
double& PathPoint::s()
{
	return m_s;
}

void PathPoint::l(double const _l)
{
	m_l = _l;
}
double PathPoint::l() const
{
	return m_l;
}
double& PathPoint::l()
{
	return m_l;
}

void PathPoint::dkappa(double const _dkappa)
{
	m_dkappa = _dkappa;
}
double PathPoint::dkappa() const
{
	return m_dkappa;
}
double& PathPoint::dkappa()
{
	return m_dkappa;
}

void PathPoint::ddkappa(double const _ddkappa)
{
	m_ddkappa = _ddkappa;
}
double PathPoint::ddkappa() const
{
	return m_ddkappa;
}
double& PathPoint::ddkappa()
{
	return m_ddkappa;
}

void PathPoint::lane_id(std::string const &_lane_id)
{
	m_lane_id = _lane_id;
}
void PathPoint::lane_id(std::string &&_lane_id)
{
	m_lane_id = std::move(_lane_id);
}
std::string const& PathPoint::lane_id() const
{
	return m_lane_id;
}
std::string& PathPoint::lane_id()
{
	return m_lane_id;
}

void PathPoint::x_derivative(double const _x_derivative)
{
	m_x_derivative = _x_derivative;
}
double PathPoint::x_derivative() const
{
	return m_x_derivative;
}
double& PathPoint::x_derivative()
{
	return m_x_derivative;
}

void PathPoint::y_derivative(double const _y_derivative)
{
	m_y_derivative = _y_derivative;
}
double PathPoint::y_derivative() const
{
	return m_y_derivative;
}
double& PathPoint::y_derivative()
{
	return m_y_derivative;
}

GaussianInfo::GaussianInfo()
{
	m_sigma_x = 0;
	m_sigma_y = 0;
	m_correlation = 0;
	m_area_probability = 0;
	m_ellipse_a = 0;
	m_ellipse_b = 0;
	m_theta_a = 0;

}

magna::dds::DdsCdr& GaussianInfo::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_sigma_x);
	cdr.serialize(m_sigma_y);
	cdr.serialize(m_correlation);
	cdr.serialize(m_area_probability);
	cdr.serialize(m_ellipse_a);
	cdr.serialize(m_ellipse_b);
	cdr.serialize(m_theta_a);

	return cdr;
}
uint32_t GaussianInfo::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& GaussianInfo::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_sigma_x);
	cdr.deserialize(m_sigma_y);
	cdr.deserialize(m_correlation);
	cdr.deserialize(m_area_probability);
	cdr.deserialize(m_ellipse_a);
	cdr.deserialize(m_ellipse_b);
	cdr.deserialize(m_theta_a);

	return cdr;
}
bool GaussianInfo::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool GaussianInfo::is_key_defined()
{
	return false;

}
void GaussianInfo::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void GaussianInfo::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool GaussianInfo::is_key_serialize_by_cdr()
{
	return false;

}
bool GaussianInfo::is_plain_types()
{
	return true;
}
uint32_t GaussianInfo::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void GaussianInfo::set_key_val(GaussianInfo const* const _data) noexcept
{

}
void GaussianInfo::sigma_x(double const _sigma_x)
{
	m_sigma_x = _sigma_x;
}
double GaussianInfo::sigma_x() const
{
	return m_sigma_x;
}
double& GaussianInfo::sigma_x()
{
	return m_sigma_x;
}

void GaussianInfo::sigma_y(double const _sigma_y)
{
	m_sigma_y = _sigma_y;
}
double GaussianInfo::sigma_y() const
{
	return m_sigma_y;
}
double& GaussianInfo::sigma_y()
{
	return m_sigma_y;
}

void GaussianInfo::correlation(double const _correlation)
{
	m_correlation = _correlation;
}
double GaussianInfo::correlation() const
{
	return m_correlation;
}
double& GaussianInfo::correlation()
{
	return m_correlation;
}

void GaussianInfo::area_probability(double const _area_probability)
{
	m_area_probability = _area_probability;
}
double GaussianInfo::area_probability() const
{
	return m_area_probability;
}
double& GaussianInfo::area_probability()
{
	return m_area_probability;
}

void GaussianInfo::ellipse_a(double const _ellipse_a)
{
	m_ellipse_a = _ellipse_a;
}
double GaussianInfo::ellipse_a() const
{
	return m_ellipse_a;
}
double& GaussianInfo::ellipse_a()
{
	return m_ellipse_a;
}

void GaussianInfo::ellipse_b(double const _ellipse_b)
{
	m_ellipse_b = _ellipse_b;
}
double GaussianInfo::ellipse_b() const
{
	return m_ellipse_b;
}
double& GaussianInfo::ellipse_b()
{
	return m_ellipse_b;
}

void GaussianInfo::theta_a(double const _theta_a)
{
	m_theta_a = _theta_a;
}
double GaussianInfo::theta_a() const
{
	return m_theta_a;
}
double& GaussianInfo::theta_a()
{
	return m_theta_a;
}

TrajectoryPoint::TrajectoryPoint()
{
	m_v = 0;
	m_a = 0;
	m_relative_time = 0;
	m_da = 0;
	m_steer = 0;

}

magna::dds::DdsCdr& TrajectoryPoint::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_path_point);
	cdr.serialize(m_v);
	cdr.serialize(m_a);
	cdr.serialize(m_relative_time);
	cdr.serialize(m_da);
	cdr.serialize(m_steer);
	cdr.serialize(m_gaussian_info);

	return cdr;
}
uint32_t TrajectoryPoint::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& TrajectoryPoint::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_path_point);
	cdr.deserialize(m_v);
	cdr.deserialize(m_a);
	cdr.deserialize(m_relative_time);
	cdr.deserialize(m_da);
	cdr.deserialize(m_steer);
	cdr.deserialize(m_gaussian_info);

	return cdr;
}
bool TrajectoryPoint::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool TrajectoryPoint::is_key_defined()
{
	return false;

}
void TrajectoryPoint::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void TrajectoryPoint::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool TrajectoryPoint::is_key_serialize_by_cdr()
{
	return false;

}
bool TrajectoryPoint::is_plain_types()
{
	bool b0 = PathPoint::is_plain_types();
	bool b1 = GaussianInfo::is_plain_types();

	return b0 && b1;
}
uint32_t TrajectoryPoint::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void TrajectoryPoint::set_key_val(TrajectoryPoint const* const _data) noexcept
{

}
void TrajectoryPoint::path_point(PathPoint const &_path_point)
{
	m_path_point = _path_point;
}
void TrajectoryPoint::path_point(PathPoint &&_path_point)
{
	m_path_point = std::move(_path_point);
}
PathPoint const& TrajectoryPoint::path_point() const
{
	return m_path_point;
}
PathPoint& TrajectoryPoint::path_point()
{
	return m_path_point;
}

void TrajectoryPoint::v(double const _v)
{
	m_v = _v;
}
double TrajectoryPoint::v() const
{
	return m_v;
}
double& TrajectoryPoint::v()
{
	return m_v;
}

void TrajectoryPoint::a(double const _a)
{
	m_a = _a;
}
double TrajectoryPoint::a() const
{
	return m_a;
}
double& TrajectoryPoint::a()
{
	return m_a;
}

void TrajectoryPoint::relative_time(double const _relative_time)
{
	m_relative_time = _relative_time;
}
double TrajectoryPoint::relative_time() const
{
	return m_relative_time;
}
double& TrajectoryPoint::relative_time()
{
	return m_relative_time;
}

void TrajectoryPoint::da(double const _da)
{
	m_da = _da;
}
double TrajectoryPoint::da() const
{
	return m_da;
}
double& TrajectoryPoint::da()
{
	return m_da;
}

void TrajectoryPoint::steer(double const _steer)
{
	m_steer = _steer;
}
double TrajectoryPoint::steer() const
{
	return m_steer;
}
double& TrajectoryPoint::steer()
{
	return m_steer;
}

void TrajectoryPoint::gaussian_info(GaussianInfo const &_gaussian_info)
{
	m_gaussian_info = _gaussian_info;
}
void TrajectoryPoint::gaussian_info(GaussianInfo &&_gaussian_info)
{
	m_gaussian_info = std::move(_gaussian_info);
}
GaussianInfo const& TrajectoryPoint::gaussian_info() const
{
	return m_gaussian_info;
}
GaussianInfo& TrajectoryPoint::gaussian_info()
{
	return m_gaussian_info;
}

EStop::EStop()
{
	m_is_estop = 0;

}

magna::dds::DdsCdr& EStop::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_is_estop);
	cdr.serialize(m_reason);

	return cdr;
}
uint32_t EStop::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& EStop::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_is_estop);
	cdr.deserialize(m_reason);

	return cdr;
}
bool EStop::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool EStop::is_key_defined()
{
	return false;

}
void EStop::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void EStop::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool EStop::is_key_serialize_by_cdr()
{
	return false;

}
bool EStop::is_plain_types()
{
	return false;
}
uint32_t EStop::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void EStop::set_key_val(EStop const* const _data) noexcept
{

}
void EStop::is_estop(bool const _is_estop)
{
	m_is_estop = _is_estop;
}
bool EStop::is_estop() const
{
	return m_is_estop;
}
bool& EStop::is_estop()
{
	return m_is_estop;
}

void EStop::reason(std::string const &_reason)
{
	m_reason = _reason;
}
void EStop::reason(std::string &&_reason)
{
	m_reason = std::move(_reason);
}
std::string const& EStop::reason() const
{
	return m_reason;
}
std::string& EStop::reason()
{
	return m_reason;
}

PlanningTrajectory::PlanningTrajectory()
{
	m_total_path_length = 0;
	m_total_path_time = 0;
	m_is_replan = 0;
	m_replan_type = 0;
	m_longitudinal_diff = 0;
	m_lateral_diff = 0;

}

magna::dds::DdsCdr& PlanningTrajectory::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_header);
	cdr.serialize(m_total_path_length);
	cdr.serialize(m_total_path_time);
	cdr.serialize(m_trajectory_point);
	cdr.serialize(m_is_replan);
	cdr.serialize(m_replan_type);
	cdr.serialize(m_replan_reason);
	cdr.serialize(m_longitudinal_diff);
	cdr.serialize(m_lateral_diff);
	cdr.serialize(m_gear);
	cdr.serialize(m_estop);
	cdr.serialize(m_trajectory_type);

	return cdr;
}
uint32_t PlanningTrajectory::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& PlanningTrajectory::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_header);
	cdr.deserialize(m_total_path_length);
	cdr.deserialize(m_total_path_time);
	cdr.deserialize(m_trajectory_point);
	cdr.deserialize(m_is_replan);
	cdr.deserialize(m_replan_type);
	cdr.deserialize(m_replan_reason);
	cdr.deserialize(m_longitudinal_diff);
	cdr.deserialize(m_lateral_diff);
	cdr.deserialize(m_gear);
	cdr.deserialize(m_estop);
	cdr.deserialize(m_trajectory_type);

	return cdr;
}
bool PlanningTrajectory::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool PlanningTrajectory::is_key_defined()
{
	return false;

}
void PlanningTrajectory::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void PlanningTrajectory::serialize_key(char **buf,unsigned int *len)
{
	if (buf != nullptr)
	{
		*buf = nullptr;
	}
	if (len != nullptr)
	{
		*len = 0U;
	}

}
bool PlanningTrajectory::is_key_serialize_by_cdr()
{
	return false;

}
bool PlanningTrajectory::is_plain_types()
{
	return false;
}
uint32_t PlanningTrajectory::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void PlanningTrajectory::set_key_val(PlanningTrajectory const* const _data) noexcept
{

}
void PlanningTrajectory::header(Header const &_header)
{
	m_header = _header;
}
void PlanningTrajectory::header(Header &&_header)
{
	m_header = std::move(_header);
}
Header const& PlanningTrajectory::header() const
{
	return m_header;
}
Header& PlanningTrajectory::header()
{
	return m_header;
}

void PlanningTrajectory::total_path_length(double const _total_path_length)
{
	m_total_path_length = _total_path_length;
}
double PlanningTrajectory::total_path_length() const
{
	return m_total_path_length;
}
double& PlanningTrajectory::total_path_length()
{
	return m_total_path_length;
}

void PlanningTrajectory::total_path_time(double const _total_path_time)
{
	m_total_path_time = _total_path_time;
}
double PlanningTrajectory::total_path_time() const
{
	return m_total_path_time;
}
double& PlanningTrajectory::total_path_time()
{
	return m_total_path_time;
}

void PlanningTrajectory::trajectory_point(std::vector<TrajectoryPoint> const &_trajectory_point)
{
	m_trajectory_point = _trajectory_point;
}
void PlanningTrajectory::trajectory_point(std::vector<TrajectoryPoint> &&_trajectory_point)
{
	m_trajectory_point = std::move(_trajectory_point);
}
std::vector<TrajectoryPoint> const& PlanningTrajectory::trajectory_point() const
{
	return m_trajectory_point;
}
std::vector<TrajectoryPoint>& PlanningTrajectory::trajectory_point()
{
	return m_trajectory_point;
}

void PlanningTrajectory::is_replan(bool const _is_replan)
{
	m_is_replan = _is_replan;
}
bool PlanningTrajectory::is_replan() const
{
	return m_is_replan;
}
bool& PlanningTrajectory::is_replan()
{
	return m_is_replan;
}

void PlanningTrajectory::replan_type(uint32_t const _replan_type)
{
	m_replan_type = _replan_type;
}
uint32_t PlanningTrajectory::replan_type() const
{
	return m_replan_type;
}
uint32_t& PlanningTrajectory::replan_type()
{
	return m_replan_type;
}

void PlanningTrajectory::replan_reason(std::string const &_replan_reason)
{
	m_replan_reason = _replan_reason;
}
void PlanningTrajectory::replan_reason(std::string &&_replan_reason)
{
	m_replan_reason = std::move(_replan_reason);
}
std::string const& PlanningTrajectory::replan_reason() const
{
	return m_replan_reason;
}
std::string& PlanningTrajectory::replan_reason()
{
	return m_replan_reason;
}

void PlanningTrajectory::longitudinal_diff(double const _longitudinal_diff)
{
	m_longitudinal_diff = _longitudinal_diff;
}
double PlanningTrajectory::longitudinal_diff() const
{
	return m_longitudinal_diff;
}
double& PlanningTrajectory::longitudinal_diff()
{
	return m_longitudinal_diff;
}

void PlanningTrajectory::lateral_diff(double const _lateral_diff)
{
	m_lateral_diff = _lateral_diff;
}
double PlanningTrajectory::lateral_diff() const
{
	return m_lateral_diff;
}
double& PlanningTrajectory::lateral_diff()
{
	return m_lateral_diff;
}

void PlanningTrajectory::gear(GearPosition const _gear)
{
	m_gear = _gear;
}
GearPosition PlanningTrajectory::gear() const
{
	return m_gear;
}
GearPosition& PlanningTrajectory::gear()
{
	return m_gear;
}

void PlanningTrajectory::estop(EStop const &_estop)
{
	m_estop = _estop;
}
void PlanningTrajectory::estop(EStop &&_estop)
{
	m_estop = std::move(_estop);
}
EStop const& PlanningTrajectory::estop() const
{
	return m_estop;
}
EStop& PlanningTrajectory::estop()
{
	return m_estop;
}

void PlanningTrajectory::trajectory_type(PlanningTrajectoryType const _trajectory_type)
{
	m_trajectory_type = _trajectory_type;
}
PlanningTrajectoryType PlanningTrajectory::trajectory_type() const
{
	return m_trajectory_type;
}
PlanningTrajectoryType& PlanningTrajectory::trajectory_type()
{
	return m_trajectory_type;
}

