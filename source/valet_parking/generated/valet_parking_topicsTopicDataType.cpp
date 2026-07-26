/**************************************************************
* @file valet_parking_topicsTopicDataType.cpp
* @copyright GREENSTONE TECHNOLOGY CO.,LTD. 2020-2025
* All rights reserved
**************************************************************/

#include "valet_parking_topicsTopicDataType.h"

HeaderTopicDataType::HeaderTopicDataType() : TopicDataType()
{
	set_name("Header");
}
HeaderTopicDataType::~HeaderTopicDataType()
{

}
bool HeaderTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	Header* pData = static_cast<Header*>(data);
	pData->serialize(cdr);
	return true;
}
bool HeaderTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	Header* pData = static_cast<Header*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t HeaderTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(Header::DATA_SIZE + 8U);
}
void* HeaderTopicDataType::create_data()
{
	return new Header;
}
void HeaderTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	Header* pData = static_cast<Header*>(data);
	delete pData;
}
bool HeaderTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!Header::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool HeaderTopicDataType::is_key_defined()
{
	return Header::is_key_defined();
}
Point3DTopicDataType::Point3DTopicDataType() : TopicDataType()
{
	set_name("Point3D");
}
Point3DTopicDataType::~Point3DTopicDataType()
{

}
bool Point3DTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	Point3D* pData = static_cast<Point3D*>(data);
	pData->serialize(cdr);
	return true;
}
bool Point3DTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	Point3D* pData = static_cast<Point3D*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t Point3DTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(Point3D::DATA_SIZE + 8U);
}
void* Point3DTopicDataType::create_data()
{
	return new Point3D;
}
void Point3DTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	Point3D* pData = static_cast<Point3D*>(data);
	delete pData;
}
bool Point3DTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!Point3D::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool Point3DTopicDataType::is_key_defined()
{
	return Point3D::is_key_defined();
}
PsPointTopicDataType::PsPointTopicDataType() : TopicDataType()
{
	set_name("PsPoint");
}
PsPointTopicDataType::~PsPointTopicDataType()
{

}
bool PsPointTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	PsPoint* pData = static_cast<PsPoint*>(data);
	pData->serialize(cdr);
	return true;
}
bool PsPointTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	PsPoint* pData = static_cast<PsPoint*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t PsPointTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(PsPoint::DATA_SIZE + 8U);
}
void* PsPointTopicDataType::create_data()
{
	return new PsPoint;
}
void PsPointTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	PsPoint* pData = static_cast<PsPoint*>(data);
	delete pData;
}
bool PsPointTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!PsPoint::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool PsPointTopicDataType::is_key_defined()
{
	return PsPoint::is_key_defined();
}
ParkingPathPointTopicDataType::ParkingPathPointTopicDataType() : TopicDataType()
{
	set_name("ParkingPathPoint");
}
ParkingPathPointTopicDataType::~ParkingPathPointTopicDataType()
{

}
bool ParkingPathPointTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	ParkingPathPoint* pData = static_cast<ParkingPathPoint*>(data);
	pData->serialize(cdr);
	return true;
}
bool ParkingPathPointTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	ParkingPathPoint* pData = static_cast<ParkingPathPoint*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t ParkingPathPointTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(ParkingPathPoint::DATA_SIZE + 8U);
}
void* ParkingPathPointTopicDataType::create_data()
{
	return new ParkingPathPoint;
}
void ParkingPathPointTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	ParkingPathPoint* pData = static_cast<ParkingPathPoint*>(data);
	delete pData;
}
bool ParkingPathPointTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!ParkingPathPoint::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool ParkingPathPointTopicDataType::is_key_defined()
{
	return ParkingPathPoint::is_key_defined();
}
ParkingLotTopicDataType::ParkingLotTopicDataType() : TopicDataType()
{
	set_name("ParkingLot");
}
ParkingLotTopicDataType::~ParkingLotTopicDataType()
{

}
bool ParkingLotTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	ParkingLot* pData = static_cast<ParkingLot*>(data);
	pData->serialize(cdr);
	return true;
}
bool ParkingLotTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	ParkingLot* pData = static_cast<ParkingLot*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t ParkingLotTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(ParkingLot::DATA_SIZE + 8U);
}
void* ParkingLotTopicDataType::create_data()
{
	return new ParkingLot;
}
void ParkingLotTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	ParkingLot* pData = static_cast<ParkingLot*>(data);
	delete pData;
}
bool ParkingLotTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!ParkingLot::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool ParkingLotTopicDataType::is_key_defined()
{
	return ParkingLot::is_key_defined();
}
SelectedSlotTopicDataType::SelectedSlotTopicDataType() : TopicDataType()
{
	set_name("SelectedSlot");
}
SelectedSlotTopicDataType::~SelectedSlotTopicDataType()
{

}
bool SelectedSlotTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	SelectedSlot* pData = static_cast<SelectedSlot*>(data);
	pData->serialize(cdr);
	return true;
}
bool SelectedSlotTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	SelectedSlot* pData = static_cast<SelectedSlot*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t SelectedSlotTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(SelectedSlot::DATA_SIZE + 8U);
}
void* SelectedSlotTopicDataType::create_data()
{
	return new SelectedSlot;
}
void SelectedSlotTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	SelectedSlot* pData = static_cast<SelectedSlot*>(data);
	delete pData;
}
bool SelectedSlotTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!SelectedSlot::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool SelectedSlotTopicDataType::is_key_defined()
{
	return SelectedSlot::is_key_defined();
}
PathPointTopicDataType::PathPointTopicDataType() : TopicDataType()
{
	set_name("PathPoint");
}
PathPointTopicDataType::~PathPointTopicDataType()
{

}
bool PathPointTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	PathPoint* pData = static_cast<PathPoint*>(data);
	pData->serialize(cdr);
	return true;
}
bool PathPointTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	PathPoint* pData = static_cast<PathPoint*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t PathPointTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(PathPoint::DATA_SIZE + 8U);
}
void* PathPointTopicDataType::create_data()
{
	return new PathPoint;
}
void PathPointTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	PathPoint* pData = static_cast<PathPoint*>(data);
	delete pData;
}
bool PathPointTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!PathPoint::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool PathPointTopicDataType::is_key_defined()
{
	return PathPoint::is_key_defined();
}
GaussianInfoTopicDataType::GaussianInfoTopicDataType() : TopicDataType()
{
	set_name("GaussianInfo");
}
GaussianInfoTopicDataType::~GaussianInfoTopicDataType()
{

}
bool GaussianInfoTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	GaussianInfo* pData = static_cast<GaussianInfo*>(data);
	pData->serialize(cdr);
	return true;
}
bool GaussianInfoTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	GaussianInfo* pData = static_cast<GaussianInfo*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t GaussianInfoTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(GaussianInfo::DATA_SIZE + 8U);
}
void* GaussianInfoTopicDataType::create_data()
{
	return new GaussianInfo;
}
void GaussianInfoTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	GaussianInfo* pData = static_cast<GaussianInfo*>(data);
	delete pData;
}
bool GaussianInfoTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!GaussianInfo::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool GaussianInfoTopicDataType::is_key_defined()
{
	return GaussianInfo::is_key_defined();
}
TrajectoryPointTopicDataType::TrajectoryPointTopicDataType() : TopicDataType()
{
	set_name("TrajectoryPoint");
}
TrajectoryPointTopicDataType::~TrajectoryPointTopicDataType()
{

}
bool TrajectoryPointTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	TrajectoryPoint* pData = static_cast<TrajectoryPoint*>(data);
	pData->serialize(cdr);
	return true;
}
bool TrajectoryPointTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	TrajectoryPoint* pData = static_cast<TrajectoryPoint*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t TrajectoryPointTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(TrajectoryPoint::DATA_SIZE + 8U);
}
void* TrajectoryPointTopicDataType::create_data()
{
	return new TrajectoryPoint;
}
void TrajectoryPointTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	TrajectoryPoint* pData = static_cast<TrajectoryPoint*>(data);
	delete pData;
}
bool TrajectoryPointTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!TrajectoryPoint::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool TrajectoryPointTopicDataType::is_key_defined()
{
	return TrajectoryPoint::is_key_defined();
}
EStopTopicDataType::EStopTopicDataType() : TopicDataType()
{
	set_name("EStop");
}
EStopTopicDataType::~EStopTopicDataType()
{

}
bool EStopTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	EStop* pData = static_cast<EStop*>(data);
	pData->serialize(cdr);
	return true;
}
bool EStopTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	EStop* pData = static_cast<EStop*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t EStopTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(EStop::DATA_SIZE + 8U);
}
void* EStopTopicDataType::create_data()
{
	return new EStop;
}
void EStopTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	EStop* pData = static_cast<EStop*>(data);
	delete pData;
}
bool EStopTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!EStop::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool EStopTopicDataType::is_key_defined()
{
	return EStop::is_key_defined();
}
PlanningTrajectoryTopicDataType::PlanningTrajectoryTopicDataType() : TopicDataType()
{
	set_name("PlanningTrajectory");
}
PlanningTrajectoryTopicDataType::~PlanningTrajectoryTopicDataType()
{

}
bool PlanningTrajectoryTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	PlanningTrajectory* pData = static_cast<PlanningTrajectory*>(data);
	pData->serialize(cdr);
	return true;
}
bool PlanningTrajectoryTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	PlanningTrajectory* pData = static_cast<PlanningTrajectory*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t PlanningTrajectoryTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(PlanningTrajectory::DATA_SIZE + 8U);
}
void* PlanningTrajectoryTopicDataType::create_data()
{
	return new PlanningTrajectory;
}
void PlanningTrajectoryTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	PlanningTrajectory* pData = static_cast<PlanningTrajectory*>(data);
	delete pData;
}
bool PlanningTrajectoryTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!PlanningTrajectory::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool PlanningTrajectoryTopicDataType::is_key_defined()
{
	return PlanningTrajectory::is_key_defined();
}
