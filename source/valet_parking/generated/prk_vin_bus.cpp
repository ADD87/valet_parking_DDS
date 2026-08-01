/**************************************************************
* @file prk_vin_bus.cpp
* @copyright GREENSTONE TECHNOLOGY CO.,LTD. 2020-2025
* All rights reserved
**************************************************************/

#include "prk_vin_bus.h"
//#include <iostream>

EIdcServiceResp_st::EIdcServiceResp_st()
{

}

magna::dds::DdsCdr& EIdcServiceResp_st::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_control_response);

	return cdr;
}
uint32_t EIdcServiceResp_st::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& EIdcServiceResp_st::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_control_response);

	return cdr;
}
bool EIdcServiceResp_st::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool EIdcServiceResp_st::is_key_defined()
{
	return false;

}
void EIdcServiceResp_st::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void EIdcServiceResp_st::serialize_key(char **buf,unsigned int *len)
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
bool EIdcServiceResp_st::is_key_serialize_by_cdr()
{
	return false;

}
bool EIdcServiceResp_st::is_plain_types()
{
	return true;
}
uint32_t EIdcServiceResp_st::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void EIdcServiceResp_st::set_key_val(EIdcServiceResp_st const* const _data) noexcept
{

}
void EIdcServiceResp_st::control_response(std::array<UInt16,3> const &_control_response)
{
	m_control_response = _control_response;
}
void EIdcServiceResp_st::control_response(std::array<UInt16,3> &&_control_response)
{
	m_control_response = std::move(_control_response);
}
std::array<UInt16,3> const& EIdcServiceResp_st::control_response() const
{
	return m_control_response;
}
std::array<UInt16,3>& EIdcServiceResp_st::control_response()
{
	return m_control_response;
}

CCPJFCU::CCPJFCU()
{

}

magna::dds::DdsCdr& CCPJFCU::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_Ob5Weekseconds);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_Longitude);
	cdr.serialize(m_m_Latitude);
	cdr.serialize(m_m_Wgs84Hgt);
	cdr.serialize(m_m_MagYaw);
	cdr.serialize(m_m_VoteNavHorPosType);
	cdr.serialize(m_m_NavpDevErr);
	cdr.serialize(m_m_NavsDevErr);
	cdr.serialize(m_m_AhrsDevErr);
	cdr.serialize(m_m_Ob2Weekseconds);
	cdr.serialize(m_m_Quaternion_q0);
	cdr.serialize(m_m_Quaternion_q1);
	cdr.serialize(m_m_Quaternion_q2);
	cdr.serialize(m_m_Quaternion_q3);

	return cdr;
}
uint32_t CCPJFCU::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCPJFCU::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_Ob5Weekseconds);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_Longitude);
	cdr.deserialize(m_m_Latitude);
	cdr.deserialize(m_m_Wgs84Hgt);
	cdr.deserialize(m_m_MagYaw);
	cdr.deserialize(m_m_VoteNavHorPosType);
	cdr.deserialize(m_m_NavpDevErr);
	cdr.deserialize(m_m_NavsDevErr);
	cdr.deserialize(m_m_AhrsDevErr);
	cdr.deserialize(m_m_Ob2Weekseconds);
	cdr.deserialize(m_m_Quaternion_q0);
	cdr.deserialize(m_m_Quaternion_q1);
	cdr.deserialize(m_m_Quaternion_q2);
	cdr.deserialize(m_m_Quaternion_q3);

	return cdr;
}
bool CCPJFCU::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCPJFCU::is_key_defined()
{
	return false;

}
void CCPJFCU::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCPJFCU::serialize_key(char **buf,unsigned int *len)
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
bool CCPJFCU::is_key_serialize_by_cdr()
{
	return false;

}
bool CCPJFCU::is_plain_types()
{
	return true;
}
uint32_t CCPJFCU::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCPJFCU::set_key_val(CCPJFCU const* const _data) noexcept
{

}
void CCPJFCU::m_Ob5Weekseconds(SInt16 const _m_Ob5Weekseconds)
{
	m_m_Ob5Weekseconds = _m_Ob5Weekseconds;
}
SInt16 CCPJFCU::m_Ob5Weekseconds() const
{
	return m_m_Ob5Weekseconds;
}
SInt16& CCPJFCU::m_Ob5Weekseconds()
{
	return m_m_Ob5Weekseconds;
}

void CCPJFCU::padding(SInt16 const _padding)
{
	m_padding = _padding;
}
SInt16 CCPJFCU::padding() const
{
	return m_padding;
}
SInt16& CCPJFCU::padding()
{
	return m_padding;
}

void CCPJFCU::m_Longitude(Float const _m_Longitude)
{
	m_m_Longitude = _m_Longitude;
}
Float CCPJFCU::m_Longitude() const
{
	return m_m_Longitude;
}
Float& CCPJFCU::m_Longitude()
{
	return m_m_Longitude;
}

void CCPJFCU::m_Latitude(Float const _m_Latitude)
{
	m_m_Latitude = _m_Latitude;
}
Float CCPJFCU::m_Latitude() const
{
	return m_m_Latitude;
}
Float& CCPJFCU::m_Latitude()
{
	return m_m_Latitude;
}

void CCPJFCU::m_Wgs84Hgt(Float const _m_Wgs84Hgt)
{
	m_m_Wgs84Hgt = _m_Wgs84Hgt;
}
Float CCPJFCU::m_Wgs84Hgt() const
{
	return m_m_Wgs84Hgt;
}
Float& CCPJFCU::m_Wgs84Hgt()
{
	return m_m_Wgs84Hgt;
}

void CCPJFCU::m_MagYaw(Float const _m_MagYaw)
{
	m_m_MagYaw = _m_MagYaw;
}
Float CCPJFCU::m_MagYaw() const
{
	return m_m_MagYaw;
}
Float& CCPJFCU::m_MagYaw()
{
	return m_m_MagYaw;
}

void CCPJFCU::m_VoteNavHorPosType(UInt32 const _m_VoteNavHorPosType)
{
	m_m_VoteNavHorPosType = _m_VoteNavHorPosType;
}
UInt32 CCPJFCU::m_VoteNavHorPosType() const
{
	return m_m_VoteNavHorPosType;
}
UInt32& CCPJFCU::m_VoteNavHorPosType()
{
	return m_m_VoteNavHorPosType;
}

void CCPJFCU::m_NavpDevErr(UInt32 const _m_NavpDevErr)
{
	m_m_NavpDevErr = _m_NavpDevErr;
}
UInt32 CCPJFCU::m_NavpDevErr() const
{
	return m_m_NavpDevErr;
}
UInt32& CCPJFCU::m_NavpDevErr()
{
	return m_m_NavpDevErr;
}

void CCPJFCU::m_NavsDevErr(UInt32 const _m_NavsDevErr)
{
	m_m_NavsDevErr = _m_NavsDevErr;
}
UInt32 CCPJFCU::m_NavsDevErr() const
{
	return m_m_NavsDevErr;
}
UInt32& CCPJFCU::m_NavsDevErr()
{
	return m_m_NavsDevErr;
}

void CCPJFCU::m_AhrsDevErr(UInt32 const _m_AhrsDevErr)
{
	m_m_AhrsDevErr = _m_AhrsDevErr;
}
UInt32 CCPJFCU::m_AhrsDevErr() const
{
	return m_m_AhrsDevErr;
}
UInt32& CCPJFCU::m_AhrsDevErr()
{
	return m_m_AhrsDevErr;
}

void CCPJFCU::m_Ob2Weekseconds(Float const _m_Ob2Weekseconds)
{
	m_m_Ob2Weekseconds = _m_Ob2Weekseconds;
}
Float CCPJFCU::m_Ob2Weekseconds() const
{
	return m_m_Ob2Weekseconds;
}
Float& CCPJFCU::m_Ob2Weekseconds()
{
	return m_m_Ob2Weekseconds;
}

void CCPJFCU::m_Quaternion_q0(Float const _m_Quaternion_q0)
{
	m_m_Quaternion_q0 = _m_Quaternion_q0;
}
Float CCPJFCU::m_Quaternion_q0() const
{
	return m_m_Quaternion_q0;
}
Float& CCPJFCU::m_Quaternion_q0()
{
	return m_m_Quaternion_q0;
}

void CCPJFCU::m_Quaternion_q1(Float const _m_Quaternion_q1)
{
	m_m_Quaternion_q1 = _m_Quaternion_q1;
}
Float CCPJFCU::m_Quaternion_q1() const
{
	return m_m_Quaternion_q1;
}
Float& CCPJFCU::m_Quaternion_q1()
{
	return m_m_Quaternion_q1;
}

void CCPJFCU::m_Quaternion_q2(Float const _m_Quaternion_q2)
{
	m_m_Quaternion_q2 = _m_Quaternion_q2;
}
Float CCPJFCU::m_Quaternion_q2() const
{
	return m_m_Quaternion_q2;
}
Float& CCPJFCU::m_Quaternion_q2()
{
	return m_m_Quaternion_q2;
}

void CCPJFCU::m_Quaternion_q3(Float const _m_Quaternion_q3)
{
	m_m_Quaternion_q3 = _m_Quaternion_q3;
}
Float CCPJFCU::m_Quaternion_q3() const
{
	return m_m_Quaternion_q3;
}
Float& CCPJFCU::m_Quaternion_q3()
{
	return m_m_Quaternion_q3;
}

CCPJIPB::CCPJIPB()
{

}

magna::dds::DdsCdr& CCPJIPB::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_IPB_VehVertAccel);
	cdr.serialize(m_m_IPB_XPU_ParkFailureSts);
	cdr.serialize(m_m_IPB_VehVertAccelVD);
	cdr.serialize(m_m_IPB_XPU_ParkDrvBrkOverrideFlg);
	cdr.serialize(m_m_IPB_MLWheelSpdEdgesSum);
	cdr.serialize(m_m_IPB_MRWheelSpdEdgesSum);
	cdr.serialize(m_m_IPB_MLWheelSpdVD_E2E);
	cdr.serialize(m_m_IPB_MRWheelSpdVD_E2E);
	cdr.serialize(m_m_IPB_MRWheelSpd_E2E);
	cdr.serialize(m_m_IPB_MLWheelSpd_E2E);
	cdr.serialize(m_m_IPB_StandStillSt);
	cdr.serialize(m_m_IPB_CDD_Current_BrkTq);
	cdr.serialize(m_m_IPB_ESPFault);
	cdr.serialize(m_m_IPB_CDD_Current_BrkTqVD);
	cdr.serialize(m_m_IPB_BrkPedalStVD_E2E);
	cdr.serialize(m_padding);
	cdr.serialize(m_padding1);
	cdr.serialize(m_m_IPB_APAActiveState_E2E);
	cdr.serialize(m_m_IPB_MLWheelRotatedDirection);
	cdr.serialize(m_m_IPB_MRWheelRotatedDirection);
	cdr.serialize(m_m_IPB_PitchRate);
	cdr.serialize(m_m_IPB_RollRate);
	cdr.serialize(m_m_IPB_YAWRate_E2E);

	return cdr;
}
uint32_t CCPJIPB::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCPJIPB::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_IPB_VehVertAccel);
	cdr.deserialize(m_m_IPB_XPU_ParkFailureSts);
	cdr.deserialize(m_m_IPB_VehVertAccelVD);
	cdr.deserialize(m_m_IPB_XPU_ParkDrvBrkOverrideFlg);
	cdr.deserialize(m_m_IPB_MLWheelSpdEdgesSum);
	cdr.deserialize(m_m_IPB_MRWheelSpdEdgesSum);
	cdr.deserialize(m_m_IPB_MLWheelSpdVD_E2E);
	cdr.deserialize(m_m_IPB_MRWheelSpdVD_E2E);
	cdr.deserialize(m_m_IPB_MRWheelSpd_E2E);
	cdr.deserialize(m_m_IPB_MLWheelSpd_E2E);
	cdr.deserialize(m_m_IPB_StandStillSt);
	cdr.deserialize(m_m_IPB_CDD_Current_BrkTq);
	cdr.deserialize(m_m_IPB_ESPFault);
	cdr.deserialize(m_m_IPB_CDD_Current_BrkTqVD);
	cdr.deserialize(m_m_IPB_BrkPedalStVD_E2E);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_padding1);
	cdr.deserialize(m_m_IPB_APAActiveState_E2E);
	cdr.deserialize(m_m_IPB_MLWheelRotatedDirection);
	cdr.deserialize(m_m_IPB_MRWheelRotatedDirection);
	cdr.deserialize(m_m_IPB_PitchRate);
	cdr.deserialize(m_m_IPB_RollRate);
	cdr.deserialize(m_m_IPB_YAWRate_E2E);

	return cdr;
}
bool CCPJIPB::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCPJIPB::is_key_defined()
{
	return false;

}
void CCPJIPB::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCPJIPB::serialize_key(char **buf,unsigned int *len)
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
bool CCPJIPB::is_key_serialize_by_cdr()
{
	return false;

}
bool CCPJIPB::is_plain_types()
{
	return true;
}
uint32_t CCPJIPB::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCPJIPB::set_key_val(CCPJIPB const* const _data) noexcept
{

}
void CCPJIPB::m_IPB_VehVertAccel(Float const _m_IPB_VehVertAccel)
{
	m_m_IPB_VehVertAccel = _m_IPB_VehVertAccel;
}
Float CCPJIPB::m_IPB_VehVertAccel() const
{
	return m_m_IPB_VehVertAccel;
}
Float& CCPJIPB::m_IPB_VehVertAccel()
{
	return m_m_IPB_VehVertAccel;
}

void CCPJIPB::m_IPB_XPU_ParkFailureSts(UInt32 const _m_IPB_XPU_ParkFailureSts)
{
	m_m_IPB_XPU_ParkFailureSts = _m_IPB_XPU_ParkFailureSts;
}
UInt32 CCPJIPB::m_IPB_XPU_ParkFailureSts() const
{
	return m_m_IPB_XPU_ParkFailureSts;
}
UInt32& CCPJIPB::m_IPB_XPU_ParkFailureSts()
{
	return m_m_IPB_XPU_ParkFailureSts;
}

void CCPJIPB::m_IPB_VehVertAccelVD(Boolean const _m_IPB_VehVertAccelVD)
{
	m_m_IPB_VehVertAccelVD = _m_IPB_VehVertAccelVD;
}
Boolean CCPJIPB::m_IPB_VehVertAccelVD() const
{
	return m_m_IPB_VehVertAccelVD;
}
Boolean& CCPJIPB::m_IPB_VehVertAccelVD()
{
	return m_m_IPB_VehVertAccelVD;
}

void CCPJIPB::m_IPB_XPU_ParkDrvBrkOverrideFlg(Boolean const _m_IPB_XPU_ParkDrvBrkOverrideFlg)
{
	m_m_IPB_XPU_ParkDrvBrkOverrideFlg = _m_IPB_XPU_ParkDrvBrkOverrideFlg;
}
Boolean CCPJIPB::m_IPB_XPU_ParkDrvBrkOverrideFlg() const
{
	return m_m_IPB_XPU_ParkDrvBrkOverrideFlg;
}
Boolean& CCPJIPB::m_IPB_XPU_ParkDrvBrkOverrideFlg()
{
	return m_m_IPB_XPU_ParkDrvBrkOverrideFlg;
}

void CCPJIPB::m_IPB_MLWheelSpdEdgesSum(UInt16 const _m_IPB_MLWheelSpdEdgesSum)
{
	m_m_IPB_MLWheelSpdEdgesSum = _m_IPB_MLWheelSpdEdgesSum;
}
UInt16 CCPJIPB::m_IPB_MLWheelSpdEdgesSum() const
{
	return m_m_IPB_MLWheelSpdEdgesSum;
}
UInt16& CCPJIPB::m_IPB_MLWheelSpdEdgesSum()
{
	return m_m_IPB_MLWheelSpdEdgesSum;
}

void CCPJIPB::m_IPB_MRWheelSpdEdgesSum(UInt16 const _m_IPB_MRWheelSpdEdgesSum)
{
	m_m_IPB_MRWheelSpdEdgesSum = _m_IPB_MRWheelSpdEdgesSum;
}
UInt16 CCPJIPB::m_IPB_MRWheelSpdEdgesSum() const
{
	return m_m_IPB_MRWheelSpdEdgesSum;
}
UInt16& CCPJIPB::m_IPB_MRWheelSpdEdgesSum()
{
	return m_m_IPB_MRWheelSpdEdgesSum;
}

void CCPJIPB::m_IPB_MLWheelSpdVD_E2E(Boolean const _m_IPB_MLWheelSpdVD_E2E)
{
	m_m_IPB_MLWheelSpdVD_E2E = _m_IPB_MLWheelSpdVD_E2E;
}
Boolean CCPJIPB::m_IPB_MLWheelSpdVD_E2E() const
{
	return m_m_IPB_MLWheelSpdVD_E2E;
}
Boolean& CCPJIPB::m_IPB_MLWheelSpdVD_E2E()
{
	return m_m_IPB_MLWheelSpdVD_E2E;
}

void CCPJIPB::m_IPB_MRWheelSpdVD_E2E(Boolean const _m_IPB_MRWheelSpdVD_E2E)
{
	m_m_IPB_MRWheelSpdVD_E2E = _m_IPB_MRWheelSpdVD_E2E;
}
Boolean CCPJIPB::m_IPB_MRWheelSpdVD_E2E() const
{
	return m_m_IPB_MRWheelSpdVD_E2E;
}
Boolean& CCPJIPB::m_IPB_MRWheelSpdVD_E2E()
{
	return m_m_IPB_MRWheelSpdVD_E2E;
}

void CCPJIPB::m_IPB_MRWheelSpd_E2E(Float const _m_IPB_MRWheelSpd_E2E)
{
	m_m_IPB_MRWheelSpd_E2E = _m_IPB_MRWheelSpd_E2E;
}
Float CCPJIPB::m_IPB_MRWheelSpd_E2E() const
{
	return m_m_IPB_MRWheelSpd_E2E;
}
Float& CCPJIPB::m_IPB_MRWheelSpd_E2E()
{
	return m_m_IPB_MRWheelSpd_E2E;
}

void CCPJIPB::m_IPB_MLWheelSpd_E2E(Float const _m_IPB_MLWheelSpd_E2E)
{
	m_m_IPB_MLWheelSpd_E2E = _m_IPB_MLWheelSpd_E2E;
}
Float CCPJIPB::m_IPB_MLWheelSpd_E2E() const
{
	return m_m_IPB_MLWheelSpd_E2E;
}
Float& CCPJIPB::m_IPB_MLWheelSpd_E2E()
{
	return m_m_IPB_MLWheelSpd_E2E;
}

void CCPJIPB::m_IPB_StandStillSt(UInt32 const _m_IPB_StandStillSt)
{
	m_m_IPB_StandStillSt = _m_IPB_StandStillSt;
}
UInt32 CCPJIPB::m_IPB_StandStillSt() const
{
	return m_m_IPB_StandStillSt;
}
UInt32& CCPJIPB::m_IPB_StandStillSt()
{
	return m_m_IPB_StandStillSt;
}

void CCPJIPB::m_IPB_CDD_Current_BrkTq(UInt16 const _m_IPB_CDD_Current_BrkTq)
{
	m_m_IPB_CDD_Current_BrkTq = _m_IPB_CDD_Current_BrkTq;
}
UInt16 CCPJIPB::m_IPB_CDD_Current_BrkTq() const
{
	return m_m_IPB_CDD_Current_BrkTq;
}
UInt16& CCPJIPB::m_IPB_CDD_Current_BrkTq()
{
	return m_m_IPB_CDD_Current_BrkTq;
}

void CCPJIPB::m_IPB_ESPFault(Boolean const _m_IPB_ESPFault)
{
	m_m_IPB_ESPFault = _m_IPB_ESPFault;
}
Boolean CCPJIPB::m_IPB_ESPFault() const
{
	return m_m_IPB_ESPFault;
}
Boolean& CCPJIPB::m_IPB_ESPFault()
{
	return m_m_IPB_ESPFault;
}

void CCPJIPB::m_IPB_CDD_Current_BrkTqVD(Boolean const _m_IPB_CDD_Current_BrkTqVD)
{
	m_m_IPB_CDD_Current_BrkTqVD = _m_IPB_CDD_Current_BrkTqVD;
}
Boolean CCPJIPB::m_IPB_CDD_Current_BrkTqVD() const
{
	return m_m_IPB_CDD_Current_BrkTqVD;
}
Boolean& CCPJIPB::m_IPB_CDD_Current_BrkTqVD()
{
	return m_m_IPB_CDD_Current_BrkTqVD;
}

void CCPJIPB::m_IPB_BrkPedalStVD_E2E(Boolean const _m_IPB_BrkPedalStVD_E2E)
{
	m_m_IPB_BrkPedalStVD_E2E = _m_IPB_BrkPedalStVD_E2E;
}
Boolean CCPJIPB::m_IPB_BrkPedalStVD_E2E() const
{
	return m_m_IPB_BrkPedalStVD_E2E;
}
Boolean& CCPJIPB::m_IPB_BrkPedalStVD_E2E()
{
	return m_m_IPB_BrkPedalStVD_E2E;
}

void CCPJIPB::padding(UInt8 const _padding)
{
	m_padding = _padding;
}
UInt8 CCPJIPB::padding() const
{
	return m_padding;
}
UInt8& CCPJIPB::padding()
{
	return m_padding;
}

void CCPJIPB::padding1(UInt16 const _padding1)
{
	m_padding1 = _padding1;
}
UInt16 CCPJIPB::padding1() const
{
	return m_padding1;
}
UInt16& CCPJIPB::padding1()
{
	return m_padding1;
}

void CCPJIPB::m_IPB_APAActiveState_E2E(UInt32 const _m_IPB_APAActiveState_E2E)
{
	m_m_IPB_APAActiveState_E2E = _m_IPB_APAActiveState_E2E;
}
UInt32 CCPJIPB::m_IPB_APAActiveState_E2E() const
{
	return m_m_IPB_APAActiveState_E2E;
}
UInt32& CCPJIPB::m_IPB_APAActiveState_E2E()
{
	return m_m_IPB_APAActiveState_E2E;
}

void CCPJIPB::m_IPB_MLWheelRotatedDirection(UInt32 const _m_IPB_MLWheelRotatedDirection)
{
	m_m_IPB_MLWheelRotatedDirection = _m_IPB_MLWheelRotatedDirection;
}
UInt32 CCPJIPB::m_IPB_MLWheelRotatedDirection() const
{
	return m_m_IPB_MLWheelRotatedDirection;
}
UInt32& CCPJIPB::m_IPB_MLWheelRotatedDirection()
{
	return m_m_IPB_MLWheelRotatedDirection;
}

void CCPJIPB::m_IPB_MRWheelRotatedDirection(UInt32 const _m_IPB_MRWheelRotatedDirection)
{
	m_m_IPB_MRWheelRotatedDirection = _m_IPB_MRWheelRotatedDirection;
}
UInt32 CCPJIPB::m_IPB_MRWheelRotatedDirection() const
{
	return m_m_IPB_MRWheelRotatedDirection;
}
UInt32& CCPJIPB::m_IPB_MRWheelRotatedDirection()
{
	return m_m_IPB_MRWheelRotatedDirection;
}

void CCPJIPB::m_IPB_PitchRate(Float const _m_IPB_PitchRate)
{
	m_m_IPB_PitchRate = _m_IPB_PitchRate;
}
Float CCPJIPB::m_IPB_PitchRate() const
{
	return m_m_IPB_PitchRate;
}
Float& CCPJIPB::m_IPB_PitchRate()
{
	return m_m_IPB_PitchRate;
}

void CCPJIPB::m_IPB_RollRate(Float const _m_IPB_RollRate)
{
	m_m_IPB_RollRate = _m_IPB_RollRate;
}
Float CCPJIPB::m_IPB_RollRate() const
{
	return m_m_IPB_RollRate;
}
Float& CCPJIPB::m_IPB_RollRate()
{
	return m_m_IPB_RollRate;
}

void CCPJIPB::m_IPB_YAWRate_E2E(Float const _m_IPB_YAWRate_E2E)
{
	m_m_IPB_YAWRate_E2E = _m_IPB_YAWRate_E2E;
}
Float CCPJIPB::m_IPB_YAWRate_E2E() const
{
	return m_m_IPB_YAWRate_E2E;
}
Float& CCPJIPB::m_IPB_YAWRate_E2E()
{
	return m_m_IPB_YAWRate_E2E;
}

CCPJLA1::CCPJLA1()
{

}

magna::dds::DdsCdr& CCPJLA1::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_LA1_Distance_mm);
	cdr.serialize(m_m_LA1_Unstable);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_LA1_ErrorState);

	return cdr;
}
uint32_t CCPJLA1::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCPJLA1::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_LA1_Distance_mm);
	cdr.deserialize(m_m_LA1_Unstable);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_LA1_ErrorState);

	return cdr;
}
bool CCPJLA1::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCPJLA1::is_key_defined()
{
	return false;

}
void CCPJLA1::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCPJLA1::serialize_key(char **buf,unsigned int *len)
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
bool CCPJLA1::is_key_serialize_by_cdr()
{
	return false;

}
bool CCPJLA1::is_plain_types()
{
	return true;
}
uint32_t CCPJLA1::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCPJLA1::set_key_val(CCPJLA1 const* const _data) noexcept
{

}
void CCPJLA1::m_LA1_Distance_mm(UInt16 const _m_LA1_Distance_mm)
{
	m_m_LA1_Distance_mm = _m_LA1_Distance_mm;
}
UInt16 CCPJLA1::m_LA1_Distance_mm() const
{
	return m_m_LA1_Distance_mm;
}
UInt16& CCPJLA1::m_LA1_Distance_mm()
{
	return m_m_LA1_Distance_mm;
}

void CCPJLA1::m_LA1_Unstable(Boolean const _m_LA1_Unstable)
{
	m_m_LA1_Unstable = _m_LA1_Unstable;
}
Boolean CCPJLA1::m_LA1_Unstable() const
{
	return m_m_LA1_Unstable;
}
Boolean& CCPJLA1::m_LA1_Unstable()
{
	return m_m_LA1_Unstable;
}

void CCPJLA1::padding(UInt8 const _padding)
{
	m_padding = _padding;
}
UInt8 CCPJLA1::padding() const
{
	return m_padding;
}
UInt8& CCPJLA1::padding()
{
	return m_padding;
}

void CCPJLA1::m_LA1_ErrorState(UInt32 const _m_LA1_ErrorState)
{
	m_m_LA1_ErrorState = _m_LA1_ErrorState;
}
UInt32 CCPJLA1::m_LA1_ErrorState() const
{
	return m_m_LA1_ErrorState;
}
UInt32& CCPJLA1::m_LA1_ErrorState()
{
	return m_m_LA1_ErrorState;
}

CCPJLA2::CCPJLA2()
{

}

magna::dds::DdsCdr& CCPJLA2::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_LA2_Distance_mm);
	cdr.serialize(m_m_LA2_Unstable);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_LA2_ErrorState);

	return cdr;
}
uint32_t CCPJLA2::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCPJLA2::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_LA2_Distance_mm);
	cdr.deserialize(m_m_LA2_Unstable);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_LA2_ErrorState);

	return cdr;
}
bool CCPJLA2::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCPJLA2::is_key_defined()
{
	return false;

}
void CCPJLA2::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCPJLA2::serialize_key(char **buf,unsigned int *len)
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
bool CCPJLA2::is_key_serialize_by_cdr()
{
	return false;

}
bool CCPJLA2::is_plain_types()
{
	return true;
}
uint32_t CCPJLA2::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCPJLA2::set_key_val(CCPJLA2 const* const _data) noexcept
{

}
void CCPJLA2::m_LA2_Distance_mm(UInt16 const _m_LA2_Distance_mm)
{
	m_m_LA2_Distance_mm = _m_LA2_Distance_mm;
}
UInt16 CCPJLA2::m_LA2_Distance_mm() const
{
	return m_m_LA2_Distance_mm;
}
UInt16& CCPJLA2::m_LA2_Distance_mm()
{
	return m_m_LA2_Distance_mm;
}

void CCPJLA2::m_LA2_Unstable(Boolean const _m_LA2_Unstable)
{
	m_m_LA2_Unstable = _m_LA2_Unstable;
}
Boolean CCPJLA2::m_LA2_Unstable() const
{
	return m_m_LA2_Unstable;
}
Boolean& CCPJLA2::m_LA2_Unstable()
{
	return m_m_LA2_Unstable;
}

void CCPJLA2::padding(UInt8 const _padding)
{
	m_padding = _padding;
}
UInt8 CCPJLA2::padding() const
{
	return m_padding;
}
UInt8& CCPJLA2::padding()
{
	return m_padding;
}

void CCPJLA2::m_LA2_ErrorState(UInt32 const _m_LA2_ErrorState)
{
	m_m_LA2_ErrorState = _m_LA2_ErrorState;
}
UInt32 CCPJLA2::m_LA2_ErrorState() const
{
	return m_m_LA2_ErrorState;
}
UInt32& CCPJLA2::m_LA2_ErrorState()
{
	return m_m_LA2_ErrorState;
}

CCPJRDCU::CCPJRDCU()
{

}

magna::dds::DdsCdr& CCPJRDCU::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_RDCU_BonnetAjarSt);
	cdr.serialize(m_m_RDCU_APBActSt);
	cdr.serialize(m_m_RDCU_RLSBrightnessFW);
	cdr.serialize(m_m_RDCU_FliiLampOutputSt);
	cdr.serialize(m_m_RDCU_ChrgPortLockSt);
	cdr.serialize(m_m_RDCU_PsngrDoorLockOutput);
	cdr.serialize(m_m_RDCU_RRDoorLockOutput);

	return cdr;
}
uint32_t CCPJRDCU::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCPJRDCU::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_RDCU_BonnetAjarSt);
	cdr.deserialize(m_m_RDCU_APBActSt);
	cdr.deserialize(m_m_RDCU_RLSBrightnessFW);
	cdr.deserialize(m_m_RDCU_FliiLampOutputSt);
	cdr.deserialize(m_m_RDCU_ChrgPortLockSt);
	cdr.deserialize(m_m_RDCU_PsngrDoorLockOutput);
	cdr.deserialize(m_m_RDCU_RRDoorLockOutput);

	return cdr;
}
bool CCPJRDCU::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCPJRDCU::is_key_defined()
{
	return false;

}
void CCPJRDCU::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCPJRDCU::serialize_key(char **buf,unsigned int *len)
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
bool CCPJRDCU::is_key_serialize_by_cdr()
{
	return false;

}
bool CCPJRDCU::is_plain_types()
{
	return true;
}
uint32_t CCPJRDCU::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCPJRDCU::set_key_val(CCPJRDCU const* const _data) noexcept
{

}
void CCPJRDCU::m_RDCU_BonnetAjarSt(UInt32 const _m_RDCU_BonnetAjarSt)
{
	m_m_RDCU_BonnetAjarSt = _m_RDCU_BonnetAjarSt;
}
UInt32 CCPJRDCU::m_RDCU_BonnetAjarSt() const
{
	return m_m_RDCU_BonnetAjarSt;
}
UInt32& CCPJRDCU::m_RDCU_BonnetAjarSt()
{
	return m_m_RDCU_BonnetAjarSt;
}

void CCPJRDCU::m_RDCU_APBActSt(UInt32 const _m_RDCU_APBActSt)
{
	m_m_RDCU_APBActSt = _m_RDCU_APBActSt;
}
UInt32 CCPJRDCU::m_RDCU_APBActSt() const
{
	return m_m_RDCU_APBActSt;
}
UInt32& CCPJRDCU::m_RDCU_APBActSt()
{
	return m_m_RDCU_APBActSt;
}

void CCPJRDCU::m_RDCU_RLSBrightnessFW(UInt16 const _m_RDCU_RLSBrightnessFW)
{
	m_m_RDCU_RLSBrightnessFW = _m_RDCU_RLSBrightnessFW;
}
UInt16 CCPJRDCU::m_RDCU_RLSBrightnessFW() const
{
	return m_m_RDCU_RLSBrightnessFW;
}
UInt16& CCPJRDCU::m_RDCU_RLSBrightnessFW()
{
	return m_m_RDCU_RLSBrightnessFW;
}

void CCPJRDCU::m_RDCU_FliiLampOutputSt(Boolean const _m_RDCU_FliiLampOutputSt)
{
	m_m_RDCU_FliiLampOutputSt = _m_RDCU_FliiLampOutputSt;
}
Boolean CCPJRDCU::m_RDCU_FliiLampOutputSt() const
{
	return m_m_RDCU_FliiLampOutputSt;
}
Boolean& CCPJRDCU::m_RDCU_FliiLampOutputSt()
{
	return m_m_RDCU_FliiLampOutputSt;
}

void CCPJRDCU::m_RDCU_ChrgPortLockSt(Boolean const _m_RDCU_ChrgPortLockSt)
{
	m_m_RDCU_ChrgPortLockSt = _m_RDCU_ChrgPortLockSt;
}
Boolean CCPJRDCU::m_RDCU_ChrgPortLockSt() const
{
	return m_m_RDCU_ChrgPortLockSt;
}
Boolean& CCPJRDCU::m_RDCU_ChrgPortLockSt()
{
	return m_m_RDCU_ChrgPortLockSt;
}

void CCPJRDCU::m_RDCU_PsngrDoorLockOutput(UInt32 const _m_RDCU_PsngrDoorLockOutput)
{
	m_m_RDCU_PsngrDoorLockOutput = _m_RDCU_PsngrDoorLockOutput;
}
UInt32 CCPJRDCU::m_RDCU_PsngrDoorLockOutput() const
{
	return m_m_RDCU_PsngrDoorLockOutput;
}
UInt32& CCPJRDCU::m_RDCU_PsngrDoorLockOutput()
{
	return m_m_RDCU_PsngrDoorLockOutput;
}

void CCPJRDCU::m_RDCU_RRDoorLockOutput(UInt32 const _m_RDCU_RRDoorLockOutput)
{
	m_m_RDCU_RRDoorLockOutput = _m_RDCU_RRDoorLockOutput;
}
UInt32 CCPJRDCU::m_RDCU_RRDoorLockOutput() const
{
	return m_m_RDCU_RRDoorLockOutput;
}
UInt32& CCPJRDCU::m_RDCU_RRDoorLockOutput()
{
	return m_m_RDCU_RRDoorLockOutput;
}

CCPJRWS::CCPJRWS()
{

}

magna::dds::DdsCdr& CCPJRWS::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_RWS_SysWarning);
	cdr.serialize(m_m_RWS_RLWheelangle);

	return cdr;
}
uint32_t CCPJRWS::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCPJRWS::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_RWS_SysWarning);
	cdr.deserialize(m_m_RWS_RLWheelangle);

	return cdr;
}
bool CCPJRWS::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCPJRWS::is_key_defined()
{
	return false;

}
void CCPJRWS::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCPJRWS::serialize_key(char **buf,unsigned int *len)
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
bool CCPJRWS::is_key_serialize_by_cdr()
{
	return false;

}
bool CCPJRWS::is_plain_types()
{
	return true;
}
uint32_t CCPJRWS::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCPJRWS::set_key_val(CCPJRWS const* const _data) noexcept
{

}
void CCPJRWS::m_RWS_SysWarning(UInt32 const _m_RWS_SysWarning)
{
	m_m_RWS_SysWarning = _m_RWS_SysWarning;
}
UInt32 CCPJRWS::m_RWS_SysWarning() const
{
	return m_m_RWS_SysWarning;
}
UInt32& CCPJRWS::m_RWS_SysWarning()
{
	return m_m_RWS_SysWarning;
}

void CCPJRWS::m_RWS_RLWheelangle(Float const _m_RWS_RLWheelangle)
{
	m_m_RWS_RLWheelangle = _m_RWS_RLWheelangle;
}
Float CCPJRWS::m_RWS_RLWheelangle() const
{
	return m_m_RWS_RLWheelangle;
}
Float& CCPJRWS::m_RWS_RLWheelangle()
{
	return m_m_RWS_RLWheelangle;
}

CCPJSRS::CCPJSRS()
{

}

magna::dds::DdsCdr& CCPJSRS::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_SRS_CrashOutputSt);

	return cdr;
}
uint32_t CCPJSRS::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCPJSRS::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_SRS_CrashOutputSt);

	return cdr;
}
bool CCPJSRS::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCPJSRS::is_key_defined()
{
	return false;

}
void CCPJSRS::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCPJSRS::serialize_key(char **buf,unsigned int *len)
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
bool CCPJSRS::is_key_serialize_by_cdr()
{
	return false;

}
bool CCPJSRS::is_plain_types()
{
	return true;
}
uint32_t CCPJSRS::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCPJSRS::set_key_val(CCPJSRS const* const _data) noexcept
{

}
void CCPJSRS::m_SRS_CrashOutputSt(Boolean const _m_SRS_CrashOutputSt)
{
	m_m_SRS_CrashOutputSt = _m_SRS_CrashOutputSt;
}
Boolean CCPJSRS::m_SRS_CrashOutputSt() const
{
	return m_m_SRS_CrashOutputSt;
}
Boolean& CCPJSRS::m_SRS_CrashOutputSt()
{
	return m_m_SRS_CrashOutputSt;
}

CCPJTBOX::CCPJTBOX()
{

}

magna::dds::DdsCdr& CCPJTBOX::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_TBOX_GPSLatitude);
	cdr.serialize(m_m_TBOX_GPSLongitude);
	cdr.serialize(m_m_TBOX_GPSBearing);
	cdr.serialize(m_m_TBOX_GPSAltitude);
	cdr.serialize(m_m_TBOX_GPSAccuracy);
	cdr.serialize(m_m_TBOX_GPSSpeed);
	cdr.serialize(m_m_TBOX_GPSTime);
	cdr.serialize(m_m_TBOX_FixFlag);
	cdr.serialize(m_padding1);
	cdr.serialize(m_padding);

	return cdr;
}
uint32_t CCPJTBOX::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCPJTBOX::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_TBOX_GPSLatitude);
	cdr.deserialize(m_m_TBOX_GPSLongitude);
	cdr.deserialize(m_m_TBOX_GPSBearing);
	cdr.deserialize(m_m_TBOX_GPSAltitude);
	cdr.deserialize(m_m_TBOX_GPSAccuracy);
	cdr.deserialize(m_m_TBOX_GPSSpeed);
	cdr.deserialize(m_m_TBOX_GPSTime);
	cdr.deserialize(m_m_TBOX_FixFlag);
	cdr.deserialize(m_padding1);
	cdr.deserialize(m_padding);

	return cdr;
}
bool CCPJTBOX::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCPJTBOX::is_key_defined()
{
	return false;

}
void CCPJTBOX::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCPJTBOX::serialize_key(char **buf,unsigned int *len)
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
bool CCPJTBOX::is_key_serialize_by_cdr()
{
	return false;

}
bool CCPJTBOX::is_plain_types()
{
	return true;
}
uint32_t CCPJTBOX::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCPJTBOX::set_key_val(CCPJTBOX const* const _data) noexcept
{

}
void CCPJTBOX::m_TBOX_GPSLatitude(SInt32 const _m_TBOX_GPSLatitude)
{
	m_m_TBOX_GPSLatitude = _m_TBOX_GPSLatitude;
}
SInt32 CCPJTBOX::m_TBOX_GPSLatitude() const
{
	return m_m_TBOX_GPSLatitude;
}
SInt32& CCPJTBOX::m_TBOX_GPSLatitude()
{
	return m_m_TBOX_GPSLatitude;
}

void CCPJTBOX::m_TBOX_GPSLongitude(SInt32 const _m_TBOX_GPSLongitude)
{
	m_m_TBOX_GPSLongitude = _m_TBOX_GPSLongitude;
}
SInt32 CCPJTBOX::m_TBOX_GPSLongitude() const
{
	return m_m_TBOX_GPSLongitude;
}
SInt32& CCPJTBOX::m_TBOX_GPSLongitude()
{
	return m_m_TBOX_GPSLongitude;
}

void CCPJTBOX::m_TBOX_GPSBearing(Float const _m_TBOX_GPSBearing)
{
	m_m_TBOX_GPSBearing = _m_TBOX_GPSBearing;
}
Float CCPJTBOX::m_TBOX_GPSBearing() const
{
	return m_m_TBOX_GPSBearing;
}
Float& CCPJTBOX::m_TBOX_GPSBearing()
{
	return m_m_TBOX_GPSBearing;
}

void CCPJTBOX::m_TBOX_GPSAltitude(Float const _m_TBOX_GPSAltitude)
{
	m_m_TBOX_GPSAltitude = _m_TBOX_GPSAltitude;
}
Float CCPJTBOX::m_TBOX_GPSAltitude() const
{
	return m_m_TBOX_GPSAltitude;
}
Float& CCPJTBOX::m_TBOX_GPSAltitude()
{
	return m_m_TBOX_GPSAltitude;
}

void CCPJTBOX::m_TBOX_GPSAccuracy(UInt16 const _m_TBOX_GPSAccuracy)
{
	m_m_TBOX_GPSAccuracy = _m_TBOX_GPSAccuracy;
}
UInt16 CCPJTBOX::m_TBOX_GPSAccuracy() const
{
	return m_m_TBOX_GPSAccuracy;
}
UInt16& CCPJTBOX::m_TBOX_GPSAccuracy()
{
	return m_m_TBOX_GPSAccuracy;
}

void CCPJTBOX::m_TBOX_GPSSpeed(SInt16 const _m_TBOX_GPSSpeed)
{
	m_m_TBOX_GPSSpeed = _m_TBOX_GPSSpeed;
}
SInt16 CCPJTBOX::m_TBOX_GPSSpeed() const
{
	return m_m_TBOX_GPSSpeed;
}
SInt16& CCPJTBOX::m_TBOX_GPSSpeed()
{
	return m_m_TBOX_GPSSpeed;
}

void CCPJTBOX::m_TBOX_GPSTime(UInt32 const _m_TBOX_GPSTime)
{
	m_m_TBOX_GPSTime = _m_TBOX_GPSTime;
}
UInt32 CCPJTBOX::m_TBOX_GPSTime() const
{
	return m_m_TBOX_GPSTime;
}
UInt32& CCPJTBOX::m_TBOX_GPSTime()
{
	return m_m_TBOX_GPSTime;
}

void CCPJTBOX::m_TBOX_FixFlag(Boolean const _m_TBOX_FixFlag)
{
	m_m_TBOX_FixFlag = _m_TBOX_FixFlag;
}
Boolean CCPJTBOX::m_TBOX_FixFlag() const
{
	return m_m_TBOX_FixFlag;
}
Boolean& CCPJTBOX::m_TBOX_FixFlag()
{
	return m_m_TBOX_FixFlag;
}

void CCPJTBOX::padding1(UInt8 const _padding1)
{
	m_padding1 = _padding1;
}
UInt8 CCPJTBOX::padding1() const
{
	return m_padding1;
}
UInt8& CCPJTBOX::padding1()
{
	return m_padding1;
}

void CCPJTBOX::padding(UInt16 const _padding)
{
	m_padding = _padding;
}
UInt16 CCPJTBOX::padding() const
{
	return m_padding;
}
UInt16& CCPJTBOX::padding()
{
	return m_padding;
}

CCPJXPU::CCPJXPU()
{

}

magna::dds::DdsCdr& CCPJXPU::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_XPU_ACCSt);
	cdr.serialize(m_m_XPU_IPB_ABAReq);
	cdr.serialize(m_padding1);
	cdr.serialize(m_padding);

	return cdr;
}
uint32_t CCPJXPU::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCPJXPU::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_XPU_ACCSt);
	cdr.deserialize(m_m_XPU_IPB_ABAReq);
	cdr.deserialize(m_padding1);
	cdr.deserialize(m_padding);

	return cdr;
}
bool CCPJXPU::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCPJXPU::is_key_defined()
{
	return false;

}
void CCPJXPU::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCPJXPU::serialize_key(char **buf,unsigned int *len)
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
bool CCPJXPU::is_key_serialize_by_cdr()
{
	return false;

}
bool CCPJXPU::is_plain_types()
{
	return true;
}
uint32_t CCPJXPU::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCPJXPU::set_key_val(CCPJXPU const* const _data) noexcept
{

}
void CCPJXPU::m_XPU_ACCSt(UInt32 const _m_XPU_ACCSt)
{
	m_m_XPU_ACCSt = _m_XPU_ACCSt;
}
UInt32 CCPJXPU::m_XPU_ACCSt() const
{
	return m_m_XPU_ACCSt;
}
UInt32& CCPJXPU::m_XPU_ACCSt()
{
	return m_m_XPU_ACCSt;
}

void CCPJXPU::m_XPU_IPB_ABAReq(Boolean const _m_XPU_IPB_ABAReq)
{
	m_m_XPU_IPB_ABAReq = _m_XPU_IPB_ABAReq;
}
Boolean CCPJXPU::m_XPU_IPB_ABAReq() const
{
	return m_m_XPU_IPB_ABAReq;
}
Boolean& CCPJXPU::m_XPU_IPB_ABAReq()
{
	return m_m_XPU_IPB_ABAReq;
}

void CCPJXPU::padding1(UInt8 const _padding1)
{
	m_padding1 = _padding1;
}
UInt8 CCPJXPU::padding1() const
{
	return m_padding1;
}
UInt8& CCPJXPU::padding1()
{
	return m_padding1;
}

void CCPJXPU::padding(UInt16 const _padding)
{
	m_padding = _padding;
}
UInt16 CCPJXPU::padding() const
{
	return m_padding;
}
UInt16& CCPJXPU::padding()
{
	return m_padding;
}

CCpjEps::CCpjEps()
{

}

magna::dds::DdsCdr& CCpjEps::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_STAT_EPSAPAErr);
	cdr.serialize(m_m_EPS_SASFault_E2E);
	cdr.serialize(m_m_EPS_SASCalibratedSt_E2E);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_STAT_EPSCtrlAvailable);
	cdr.serialize(m_m_STAT_EPSAPAAbort);
	cdr.serialize(m_m_EPSTorqueFeedBack_f32);
	cdr.serialize(m_m_EPS_Systemstate);
	cdr.serialize(m_m_EPS_AngleCtrlMode);
	cdr.serialize(m_m_EPS_AngleCtrlSt);
	cdr.serialize(m_m_EPS_EndAngleCtrlReason);

	return cdr;
}
uint32_t CCpjEps::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjEps::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_STAT_EPSAPAErr);
	cdr.deserialize(m_m_EPS_SASFault_E2E);
	cdr.deserialize(m_m_EPS_SASCalibratedSt_E2E);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_STAT_EPSCtrlAvailable);
	cdr.deserialize(m_m_STAT_EPSAPAAbort);
	cdr.deserialize(m_m_EPSTorqueFeedBack_f32);
	cdr.deserialize(m_m_EPS_Systemstate);
	cdr.deserialize(m_m_EPS_AngleCtrlMode);
	cdr.deserialize(m_m_EPS_AngleCtrlSt);
	cdr.deserialize(m_m_EPS_EndAngleCtrlReason);

	return cdr;
}
bool CCpjEps::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjEps::is_key_defined()
{
	return false;

}
void CCpjEps::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjEps::serialize_key(char **buf,unsigned int *len)
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
bool CCpjEps::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjEps::is_plain_types()
{
	return true;
}
uint32_t CCpjEps::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjEps::set_key_val(CCpjEps const* const _data) noexcept
{

}
void CCpjEps::m_STAT_EPSAPAErr(Boolean const _m_STAT_EPSAPAErr)
{
	m_m_STAT_EPSAPAErr = _m_STAT_EPSAPAErr;
}
Boolean CCpjEps::m_STAT_EPSAPAErr() const
{
	return m_m_STAT_EPSAPAErr;
}
Boolean& CCpjEps::m_STAT_EPSAPAErr()
{
	return m_m_STAT_EPSAPAErr;
}

void CCpjEps::m_EPS_SASFault_E2E(Boolean const _m_EPS_SASFault_E2E)
{
	m_m_EPS_SASFault_E2E = _m_EPS_SASFault_E2E;
}
Boolean CCpjEps::m_EPS_SASFault_E2E() const
{
	return m_m_EPS_SASFault_E2E;
}
Boolean& CCpjEps::m_EPS_SASFault_E2E()
{
	return m_m_EPS_SASFault_E2E;
}

void CCpjEps::m_EPS_SASCalibratedSt_E2E(Boolean const _m_EPS_SASCalibratedSt_E2E)
{
	m_m_EPS_SASCalibratedSt_E2E = _m_EPS_SASCalibratedSt_E2E;
}
Boolean CCpjEps::m_EPS_SASCalibratedSt_E2E() const
{
	return m_m_EPS_SASCalibratedSt_E2E;
}
Boolean& CCpjEps::m_EPS_SASCalibratedSt_E2E()
{
	return m_m_EPS_SASCalibratedSt_E2E;
}

void CCpjEps::padding(UInt8 const _padding)
{
	m_padding = _padding;
}
UInt8 CCpjEps::padding() const
{
	return m_padding;
}
UInt8& CCpjEps::padding()
{
	return m_padding;
}

void CCpjEps::m_STAT_EPSCtrlAvailable(UInt32 const _m_STAT_EPSCtrlAvailable)
{
	m_m_STAT_EPSCtrlAvailable = _m_STAT_EPSCtrlAvailable;
}
UInt32 CCpjEps::m_STAT_EPSCtrlAvailable() const
{
	return m_m_STAT_EPSCtrlAvailable;
}
UInt32& CCpjEps::m_STAT_EPSCtrlAvailable()
{
	return m_m_STAT_EPSCtrlAvailable;
}

void CCpjEps::m_STAT_EPSAPAAbort(UInt32 const _m_STAT_EPSAPAAbort)
{
	m_m_STAT_EPSAPAAbort = _m_STAT_EPSAPAAbort;
}
UInt32 CCpjEps::m_STAT_EPSAPAAbort() const
{
	return m_m_STAT_EPSAPAAbort;
}
UInt32& CCpjEps::m_STAT_EPSAPAAbort()
{
	return m_m_STAT_EPSAPAAbort;
}

void CCpjEps::m_EPSTorqueFeedBack_f32(Float const _m_EPSTorqueFeedBack_f32)
{
	m_m_EPSTorqueFeedBack_f32 = _m_EPSTorqueFeedBack_f32;
}
Float CCpjEps::m_EPSTorqueFeedBack_f32() const
{
	return m_m_EPSTorqueFeedBack_f32;
}
Float& CCpjEps::m_EPSTorqueFeedBack_f32()
{
	return m_m_EPSTorqueFeedBack_f32;
}

void CCpjEps::m_EPS_Systemstate(UInt32 const _m_EPS_Systemstate)
{
	m_m_EPS_Systemstate = _m_EPS_Systemstate;
}
UInt32 CCpjEps::m_EPS_Systemstate() const
{
	return m_m_EPS_Systemstate;
}
UInt32& CCpjEps::m_EPS_Systemstate()
{
	return m_m_EPS_Systemstate;
}

void CCpjEps::m_EPS_AngleCtrlMode(UInt32 const _m_EPS_AngleCtrlMode)
{
	m_m_EPS_AngleCtrlMode = _m_EPS_AngleCtrlMode;
}
UInt32 CCpjEps::m_EPS_AngleCtrlMode() const
{
	return m_m_EPS_AngleCtrlMode;
}
UInt32& CCpjEps::m_EPS_AngleCtrlMode()
{
	return m_m_EPS_AngleCtrlMode;
}

void CCpjEps::m_EPS_AngleCtrlSt(UInt32 const _m_EPS_AngleCtrlSt)
{
	m_m_EPS_AngleCtrlSt = _m_EPS_AngleCtrlSt;
}
UInt32 CCpjEps::m_EPS_AngleCtrlSt() const
{
	return m_m_EPS_AngleCtrlSt;
}
UInt32& CCpjEps::m_EPS_AngleCtrlSt()
{
	return m_m_EPS_AngleCtrlSt;
}

void CCpjEps::m_EPS_EndAngleCtrlReason(UInt32 const _m_EPS_EndAngleCtrlReason)
{
	m_m_EPS_EndAngleCtrlReason = _m_EPS_EndAngleCtrlReason;
}
UInt32 CCpjEps::m_EPS_EndAngleCtrlReason() const
{
	return m_m_EPS_EndAngleCtrlReason;
}
UInt32& CCpjEps::m_EPS_EndAngleCtrlReason()
{
	return m_m_EPS_EndAngleCtrlReason;
}

CCpjFctaWarning::CCpjFctaWarning()
{

}

magna::dds::DdsCdr& CCpjFctaWarning::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_fctaWarningLH);
	cdr.serialize(m_m_fctaWarningRH);

	return cdr;
}
uint32_t CCpjFctaWarning::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjFctaWarning::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_fctaWarningLH);
	cdr.deserialize(m_m_fctaWarningRH);

	return cdr;
}
bool CCpjFctaWarning::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjFctaWarning::is_key_defined()
{
	return false;

}
void CCpjFctaWarning::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjFctaWarning::serialize_key(char **buf,unsigned int *len)
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
bool CCpjFctaWarning::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjFctaWarning::is_plain_types()
{
	return true;
}
uint32_t CCpjFctaWarning::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjFctaWarning::set_key_val(CCpjFctaWarning const* const _data) noexcept
{

}
void CCpjFctaWarning::m_fctaWarningLH(UInt32 const _m_fctaWarningLH)
{
	m_m_fctaWarningLH = _m_fctaWarningLH;
}
UInt32 CCpjFctaWarning::m_fctaWarningLH() const
{
	return m_m_fctaWarningLH;
}
UInt32& CCpjFctaWarning::m_fctaWarningLH()
{
	return m_m_fctaWarningLH;
}

void CCpjFctaWarning::m_fctaWarningRH(UInt32 const _m_fctaWarningRH)
{
	m_m_fctaWarningRH = _m_fctaWarningRH;
}
UInt32 CCpjFctaWarning::m_fctaWarningRH() const
{
	return m_m_fctaWarningRH;
}
UInt32& CCpjFctaWarning::m_fctaWarningRH()
{
	return m_m_fctaWarningRH;
}

CCpjFusaInfo::CCpjFusaInfo()
{

}

magna::dds::DdsCdr& CCpjFusaInfo::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_IdleSafetyFail_b);
	cdr.serialize(m_m_SupParkSafetyFail_b);
	cdr.serialize(m_m_PilParkSafetyFail_b);

	return cdr;
}
uint32_t CCpjFusaInfo::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjFusaInfo::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_IdleSafetyFail_b);
	cdr.deserialize(m_m_SupParkSafetyFail_b);
	cdr.deserialize(m_m_PilParkSafetyFail_b);

	return cdr;
}
bool CCpjFusaInfo::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjFusaInfo::is_key_defined()
{
	return false;

}
void CCpjFusaInfo::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjFusaInfo::serialize_key(char **buf,unsigned int *len)
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
bool CCpjFusaInfo::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjFusaInfo::is_plain_types()
{
	return true;
}
uint32_t CCpjFusaInfo::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjFusaInfo::set_key_val(CCpjFusaInfo const* const _data) noexcept
{

}
void CCpjFusaInfo::m_IdleSafetyFail_b(Boolean const _m_IdleSafetyFail_b)
{
	m_m_IdleSafetyFail_b = _m_IdleSafetyFail_b;
}
Boolean CCpjFusaInfo::m_IdleSafetyFail_b() const
{
	return m_m_IdleSafetyFail_b;
}
Boolean& CCpjFusaInfo::m_IdleSafetyFail_b()
{
	return m_m_IdleSafetyFail_b;
}

void CCpjFusaInfo::m_SupParkSafetyFail_b(Boolean const _m_SupParkSafetyFail_b)
{
	m_m_SupParkSafetyFail_b = _m_SupParkSafetyFail_b;
}
Boolean CCpjFusaInfo::m_SupParkSafetyFail_b() const
{
	return m_m_SupParkSafetyFail_b;
}
Boolean& CCpjFusaInfo::m_SupParkSafetyFail_b()
{
	return m_m_SupParkSafetyFail_b;
}

void CCpjFusaInfo::m_PilParkSafetyFail_b(Boolean const _m_PilParkSafetyFail_b)
{
	m_m_PilParkSafetyFail_b = _m_PilParkSafetyFail_b;
}
Boolean CCpjFusaInfo::m_PilParkSafetyFail_b() const
{
	return m_m_PilParkSafetyFail_b;
}
Boolean& CCpjFusaInfo::m_PilParkSafetyFail_b()
{
	return m_m_PilParkSafetyFail_b;
}

CCpjIHU::CCpjIHU()
{

}

magna::dds::DdsCdr& CCpjIHU::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_REQ_AVMTouchPanelX_1);
	cdr.serialize(m_REQ_AVMTouchPanelY_1);
	cdr.serialize(m_IPC_LanguageMode);
	cdr.serialize(m_AVM_ExitRequest);
	cdr.serialize(m_APA_ExitRequest);
	cdr.serialize(m_AVMCalibrationReq);
	cdr.serialize(m_AVMSoftKeyRequest);
	cdr.serialize(m_MEB_Setting);
	cdr.serialize(m_PDCmute_Setting);
	cdr.serialize(m_AVM_Setting);
	cdr.serialize(m_DOW_Setting);
	cdr.serialize(m_BSD_Setting);
	cdr.serialize(m_RCW_Setting);
	cdr.serialize(m_RCTA_Setting);
	cdr.serialize(m_FCTA_Setting);
	cdr.serialize(m_HMI_AVM_TouchEventType);
	cdr.serialize(m_FAPA_Setting);
	cdr.serialize(m_MOD_Setting);
	cdr.serialize(m_HPA_Voice_FuctionSwitch_display);
	cdr.serialize(m_HPA_Voice_Confirmed);
	cdr.serialize(m_APASoftKeyRequest);
	cdr.serialize(m_NOASoftKeyRequest);
	cdr.serialize(m_IHU_CurrentTheme);
	cdr.serialize(m_IHU_CurrentThemeMode);
	cdr.serialize(m_SET_BSV_Enable);
	cdr.serialize(m_Left_Mirror_Status);
	cdr.serialize(m_Right_Mirror_Status);
	cdr.serialize(m_Door_Lock_Status);
	cdr.serialize(m_CDCU_XPU_DisplayMode);
	cdr.serialize(m_CDCU_XPU_TranspChassisWorkSt);
	cdr.serialize(m_CDCU_XPU_TranBody);
	cdr.serialize(m_AVMHardKeyRequest);
	cdr.serialize(m_BMS_ThermPrpgtonRequest);
	cdr.serialize(m_CDCU_3603Dangle);
	cdr.serialize(m_CDCU_XPU_NRABtn);
	cdr.serialize(m_EMU3ChrgDchaConnSts);
	cdr.serialize(m_CDCU_WashCarSt);
	cdr.serialize(m_CDCU_ESP_EPBReqVD);
	cdr.serialize(m_FrntRadarSwt);
	cdr.serialize(m_RRMDisplayAVMOff);
	cdr.serialize(m_CarMdlSetColor);
	cdr.serialize(m_Reset_Default);
	cdr.serialize(m_OffRoadState);
	cdr.serialize(m_padding);
	cdr.serialize(m_APASoftKeyRequestNoReverse);

	return cdr;
}
uint32_t CCpjIHU::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjIHU::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_REQ_AVMTouchPanelX_1);
	cdr.deserialize(m_REQ_AVMTouchPanelY_1);
	cdr.deserialize(m_IPC_LanguageMode);
	cdr.deserialize(m_AVM_ExitRequest);
	cdr.deserialize(m_APA_ExitRequest);
	cdr.deserialize(m_AVMCalibrationReq);
	cdr.deserialize(m_AVMSoftKeyRequest);
	cdr.deserialize(m_MEB_Setting);
	cdr.deserialize(m_PDCmute_Setting);
	cdr.deserialize(m_AVM_Setting);
	cdr.deserialize(m_DOW_Setting);
	cdr.deserialize(m_BSD_Setting);
	cdr.deserialize(m_RCW_Setting);
	cdr.deserialize(m_RCTA_Setting);
	cdr.deserialize(m_FCTA_Setting);
	cdr.deserialize(m_HMI_AVM_TouchEventType);
	cdr.deserialize(m_FAPA_Setting);
	cdr.deserialize(m_MOD_Setting);
	cdr.deserialize(m_HPA_Voice_FuctionSwitch_display);
	cdr.deserialize(m_HPA_Voice_Confirmed);
	cdr.deserialize(m_APASoftKeyRequest);
	cdr.deserialize(m_NOASoftKeyRequest);
	cdr.deserialize(m_IHU_CurrentTheme);
	cdr.deserialize(m_IHU_CurrentThemeMode);
	cdr.deserialize(m_SET_BSV_Enable);
	cdr.deserialize(m_Left_Mirror_Status);
	cdr.deserialize(m_Right_Mirror_Status);
	cdr.deserialize(m_Door_Lock_Status);
	cdr.deserialize(m_CDCU_XPU_DisplayMode);
	cdr.deserialize(m_CDCU_XPU_TranspChassisWorkSt);
	cdr.deserialize(m_CDCU_XPU_TranBody);
	cdr.deserialize(m_AVMHardKeyRequest);
	cdr.deserialize(m_BMS_ThermPrpgtonRequest);
	cdr.deserialize(m_CDCU_3603Dangle);
	cdr.deserialize(m_CDCU_XPU_NRABtn);
	cdr.deserialize(m_EMU3ChrgDchaConnSts);
	cdr.deserialize(m_CDCU_WashCarSt);
	cdr.deserialize(m_CDCU_ESP_EPBReqVD);
	cdr.deserialize(m_FrntRadarSwt);
	cdr.deserialize(m_RRMDisplayAVMOff);
	cdr.deserialize(m_CarMdlSetColor);
	cdr.deserialize(m_Reset_Default);
	cdr.deserialize(m_OffRoadState);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_APASoftKeyRequestNoReverse);

	return cdr;
}
bool CCpjIHU::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjIHU::is_key_defined()
{
	return false;

}
void CCpjIHU::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjIHU::serialize_key(char **buf,unsigned int *len)
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
bool CCpjIHU::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjIHU::is_plain_types()
{
	return true;
}
uint32_t CCpjIHU::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjIHU::set_key_val(CCpjIHU const* const _data) noexcept
{

}
void CCpjIHU::REQ_AVMTouchPanelX_1(UInt16 const _REQ_AVMTouchPanelX_1)
{
	m_REQ_AVMTouchPanelX_1 = _REQ_AVMTouchPanelX_1;
}
UInt16 CCpjIHU::REQ_AVMTouchPanelX_1() const
{
	return m_REQ_AVMTouchPanelX_1;
}
UInt16& CCpjIHU::REQ_AVMTouchPanelX_1()
{
	return m_REQ_AVMTouchPanelX_1;
}

void CCpjIHU::REQ_AVMTouchPanelY_1(UInt16 const _REQ_AVMTouchPanelY_1)
{
	m_REQ_AVMTouchPanelY_1 = _REQ_AVMTouchPanelY_1;
}
UInt16 CCpjIHU::REQ_AVMTouchPanelY_1() const
{
	return m_REQ_AVMTouchPanelY_1;
}
UInt16& CCpjIHU::REQ_AVMTouchPanelY_1()
{
	return m_REQ_AVMTouchPanelY_1;
}

void CCpjIHU::IPC_LanguageMode(UInt32 const _IPC_LanguageMode)
{
	m_IPC_LanguageMode = _IPC_LanguageMode;
}
UInt32 CCpjIHU::IPC_LanguageMode() const
{
	return m_IPC_LanguageMode;
}
UInt32& CCpjIHU::IPC_LanguageMode()
{
	return m_IPC_LanguageMode;
}

void CCpjIHU::AVM_ExitRequest(Boolean const _AVM_ExitRequest)
{
	m_AVM_ExitRequest = _AVM_ExitRequest;
}
Boolean CCpjIHU::AVM_ExitRequest() const
{
	return m_AVM_ExitRequest;
}
Boolean& CCpjIHU::AVM_ExitRequest()
{
	return m_AVM_ExitRequest;
}

void CCpjIHU::APA_ExitRequest(Boolean const _APA_ExitRequest)
{
	m_APA_ExitRequest = _APA_ExitRequest;
}
Boolean CCpjIHU::APA_ExitRequest() const
{
	return m_APA_ExitRequest;
}
Boolean& CCpjIHU::APA_ExitRequest()
{
	return m_APA_ExitRequest;
}

void CCpjIHU::AVMCalibrationReq(Boolean const _AVMCalibrationReq)
{
	m_AVMCalibrationReq = _AVMCalibrationReq;
}
Boolean CCpjIHU::AVMCalibrationReq() const
{
	return m_AVMCalibrationReq;
}
Boolean& CCpjIHU::AVMCalibrationReq()
{
	return m_AVMCalibrationReq;
}

void CCpjIHU::AVMSoftKeyRequest(Boolean const _AVMSoftKeyRequest)
{
	m_AVMSoftKeyRequest = _AVMSoftKeyRequest;
}
Boolean CCpjIHU::AVMSoftKeyRequest() const
{
	return m_AVMSoftKeyRequest;
}
Boolean& CCpjIHU::AVMSoftKeyRequest()
{
	return m_AVMSoftKeyRequest;
}

void CCpjIHU::MEB_Setting(UInt32 const _MEB_Setting)
{
	m_MEB_Setting = _MEB_Setting;
}
UInt32 CCpjIHU::MEB_Setting() const
{
	return m_MEB_Setting;
}
UInt32& CCpjIHU::MEB_Setting()
{
	return m_MEB_Setting;
}

void CCpjIHU::PDCmute_Setting(UInt32 const _PDCmute_Setting)
{
	m_PDCmute_Setting = _PDCmute_Setting;
}
UInt32 CCpjIHU::PDCmute_Setting() const
{
	return m_PDCmute_Setting;
}
UInt32& CCpjIHU::PDCmute_Setting()
{
	return m_PDCmute_Setting;
}

void CCpjIHU::AVM_Setting(UInt32 const _AVM_Setting)
{
	m_AVM_Setting = _AVM_Setting;
}
UInt32 CCpjIHU::AVM_Setting() const
{
	return m_AVM_Setting;
}
UInt32& CCpjIHU::AVM_Setting()
{
	return m_AVM_Setting;
}

void CCpjIHU::DOW_Setting(UInt32 const _DOW_Setting)
{
	m_DOW_Setting = _DOW_Setting;
}
UInt32 CCpjIHU::DOW_Setting() const
{
	return m_DOW_Setting;
}
UInt32& CCpjIHU::DOW_Setting()
{
	return m_DOW_Setting;
}

void CCpjIHU::BSD_Setting(UInt32 const _BSD_Setting)
{
	m_BSD_Setting = _BSD_Setting;
}
UInt32 CCpjIHU::BSD_Setting() const
{
	return m_BSD_Setting;
}
UInt32& CCpjIHU::BSD_Setting()
{
	return m_BSD_Setting;
}

void CCpjIHU::RCW_Setting(UInt32 const _RCW_Setting)
{
	m_RCW_Setting = _RCW_Setting;
}
UInt32 CCpjIHU::RCW_Setting() const
{
	return m_RCW_Setting;
}
UInt32& CCpjIHU::RCW_Setting()
{
	return m_RCW_Setting;
}

void CCpjIHU::RCTA_Setting(UInt32 const _RCTA_Setting)
{
	m_RCTA_Setting = _RCTA_Setting;
}
UInt32 CCpjIHU::RCTA_Setting() const
{
	return m_RCTA_Setting;
}
UInt32& CCpjIHU::RCTA_Setting()
{
	return m_RCTA_Setting;
}

void CCpjIHU::FCTA_Setting(UInt32 const _FCTA_Setting)
{
	m_FCTA_Setting = _FCTA_Setting;
}
UInt32 CCpjIHU::FCTA_Setting() const
{
	return m_FCTA_Setting;
}
UInt32& CCpjIHU::FCTA_Setting()
{
	return m_FCTA_Setting;
}

void CCpjIHU::HMI_AVM_TouchEventType(UInt32 const _HMI_AVM_TouchEventType)
{
	m_HMI_AVM_TouchEventType = _HMI_AVM_TouchEventType;
}
UInt32 CCpjIHU::HMI_AVM_TouchEventType() const
{
	return m_HMI_AVM_TouchEventType;
}
UInt32& CCpjIHU::HMI_AVM_TouchEventType()
{
	return m_HMI_AVM_TouchEventType;
}

void CCpjIHU::FAPA_Setting(UInt32 const _FAPA_Setting)
{
	m_FAPA_Setting = _FAPA_Setting;
}
UInt32 CCpjIHU::FAPA_Setting() const
{
	return m_FAPA_Setting;
}
UInt32& CCpjIHU::FAPA_Setting()
{
	return m_FAPA_Setting;
}

void CCpjIHU::MOD_Setting(UInt32 const _MOD_Setting)
{
	m_MOD_Setting = _MOD_Setting;
}
UInt32 CCpjIHU::MOD_Setting() const
{
	return m_MOD_Setting;
}
UInt32& CCpjIHU::MOD_Setting()
{
	return m_MOD_Setting;
}

void CCpjIHU::HPA_Voice_FuctionSwitch_display(Boolean const _HPA_Voice_FuctionSwitch_display)
{
	m_HPA_Voice_FuctionSwitch_display = _HPA_Voice_FuctionSwitch_display;
}
Boolean CCpjIHU::HPA_Voice_FuctionSwitch_display() const
{
	return m_HPA_Voice_FuctionSwitch_display;
}
Boolean& CCpjIHU::HPA_Voice_FuctionSwitch_display()
{
	return m_HPA_Voice_FuctionSwitch_display;
}

void CCpjIHU::HPA_Voice_Confirmed(Boolean const _HPA_Voice_Confirmed)
{
	m_HPA_Voice_Confirmed = _HPA_Voice_Confirmed;
}
Boolean CCpjIHU::HPA_Voice_Confirmed() const
{
	return m_HPA_Voice_Confirmed;
}
Boolean& CCpjIHU::HPA_Voice_Confirmed()
{
	return m_HPA_Voice_Confirmed;
}

void CCpjIHU::APASoftKeyRequest(Boolean const _APASoftKeyRequest)
{
	m_APASoftKeyRequest = _APASoftKeyRequest;
}
Boolean CCpjIHU::APASoftKeyRequest() const
{
	return m_APASoftKeyRequest;
}
Boolean& CCpjIHU::APASoftKeyRequest()
{
	return m_APASoftKeyRequest;
}

void CCpjIHU::NOASoftKeyRequest(Boolean const _NOASoftKeyRequest)
{
	m_NOASoftKeyRequest = _NOASoftKeyRequest;
}
Boolean CCpjIHU::NOASoftKeyRequest() const
{
	return m_NOASoftKeyRequest;
}
Boolean& CCpjIHU::NOASoftKeyRequest()
{
	return m_NOASoftKeyRequest;
}

void CCpjIHU::IHU_CurrentTheme(UInt32 const _IHU_CurrentTheme)
{
	m_IHU_CurrentTheme = _IHU_CurrentTheme;
}
UInt32 CCpjIHU::IHU_CurrentTheme() const
{
	return m_IHU_CurrentTheme;
}
UInt32& CCpjIHU::IHU_CurrentTheme()
{
	return m_IHU_CurrentTheme;
}

void CCpjIHU::IHU_CurrentThemeMode(UInt32 const _IHU_CurrentThemeMode)
{
	m_IHU_CurrentThemeMode = _IHU_CurrentThemeMode;
}
UInt32 CCpjIHU::IHU_CurrentThemeMode() const
{
	return m_IHU_CurrentThemeMode;
}
UInt32& CCpjIHU::IHU_CurrentThemeMode()
{
	return m_IHU_CurrentThemeMode;
}

void CCpjIHU::SET_BSV_Enable(UInt32 const _SET_BSV_Enable)
{
	m_SET_BSV_Enable = _SET_BSV_Enable;
}
UInt32 CCpjIHU::SET_BSV_Enable() const
{
	return m_SET_BSV_Enable;
}
UInt32& CCpjIHU::SET_BSV_Enable()
{
	return m_SET_BSV_Enable;
}

void CCpjIHU::Left_Mirror_Status(UInt32 const _Left_Mirror_Status)
{
	m_Left_Mirror_Status = _Left_Mirror_Status;
}
UInt32 CCpjIHU::Left_Mirror_Status() const
{
	return m_Left_Mirror_Status;
}
UInt32& CCpjIHU::Left_Mirror_Status()
{
	return m_Left_Mirror_Status;
}

void CCpjIHU::Right_Mirror_Status(UInt32 const _Right_Mirror_Status)
{
	m_Right_Mirror_Status = _Right_Mirror_Status;
}
UInt32 CCpjIHU::Right_Mirror_Status() const
{
	return m_Right_Mirror_Status;
}
UInt32& CCpjIHU::Right_Mirror_Status()
{
	return m_Right_Mirror_Status;
}

void CCpjIHU::Door_Lock_Status(UInt32 const _Door_Lock_Status)
{
	m_Door_Lock_Status = _Door_Lock_Status;
}
UInt32 CCpjIHU::Door_Lock_Status() const
{
	return m_Door_Lock_Status;
}
UInt32& CCpjIHU::Door_Lock_Status()
{
	return m_Door_Lock_Status;
}

void CCpjIHU::CDCU_XPU_DisplayMode(UInt32 const _CDCU_XPU_DisplayMode)
{
	m_CDCU_XPU_DisplayMode = _CDCU_XPU_DisplayMode;
}
UInt32 CCpjIHU::CDCU_XPU_DisplayMode() const
{
	return m_CDCU_XPU_DisplayMode;
}
UInt32& CCpjIHU::CDCU_XPU_DisplayMode()
{
	return m_CDCU_XPU_DisplayMode;
}

void CCpjIHU::CDCU_XPU_TranspChassisWorkSt(UInt32 const _CDCU_XPU_TranspChassisWorkSt)
{
	m_CDCU_XPU_TranspChassisWorkSt = _CDCU_XPU_TranspChassisWorkSt;
}
UInt32 CCpjIHU::CDCU_XPU_TranspChassisWorkSt() const
{
	return m_CDCU_XPU_TranspChassisWorkSt;
}
UInt32& CCpjIHU::CDCU_XPU_TranspChassisWorkSt()
{
	return m_CDCU_XPU_TranspChassisWorkSt;
}

void CCpjIHU::CDCU_XPU_TranBody(UInt32 const _CDCU_XPU_TranBody)
{
	m_CDCU_XPU_TranBody = _CDCU_XPU_TranBody;
}
UInt32 CCpjIHU::CDCU_XPU_TranBody() const
{
	return m_CDCU_XPU_TranBody;
}
UInt32& CCpjIHU::CDCU_XPU_TranBody()
{
	return m_CDCU_XPU_TranBody;
}

void CCpjIHU::AVMHardKeyRequest(Boolean const _AVMHardKeyRequest)
{
	m_AVMHardKeyRequest = _AVMHardKeyRequest;
}
Boolean CCpjIHU::AVMHardKeyRequest() const
{
	return m_AVMHardKeyRequest;
}
Boolean& CCpjIHU::AVMHardKeyRequest()
{
	return m_AVMHardKeyRequest;
}

void CCpjIHU::BMS_ThermPrpgtonRequest(Boolean const _BMS_ThermPrpgtonRequest)
{
	m_BMS_ThermPrpgtonRequest = _BMS_ThermPrpgtonRequest;
}
Boolean CCpjIHU::BMS_ThermPrpgtonRequest() const
{
	return m_BMS_ThermPrpgtonRequest;
}
Boolean& CCpjIHU::BMS_ThermPrpgtonRequest()
{
	return m_BMS_ThermPrpgtonRequest;
}

void CCpjIHU::CDCU_3603Dangle(SInt16 const _CDCU_3603Dangle)
{
	m_CDCU_3603Dangle = _CDCU_3603Dangle;
}
SInt16 CCpjIHU::CDCU_3603Dangle() const
{
	return m_CDCU_3603Dangle;
}
SInt16& CCpjIHU::CDCU_3603Dangle()
{
	return m_CDCU_3603Dangle;
}

void CCpjIHU::CDCU_XPU_NRABtn(UInt32 const _CDCU_XPU_NRABtn)
{
	m_CDCU_XPU_NRABtn = _CDCU_XPU_NRABtn;
}
UInt32 CCpjIHU::CDCU_XPU_NRABtn() const
{
	return m_CDCU_XPU_NRABtn;
}
UInt32& CCpjIHU::CDCU_XPU_NRABtn()
{
	return m_CDCU_XPU_NRABtn;
}

void CCpjIHU::EMU3ChrgDchaConnSts(UInt32 const _EMU3ChrgDchaConnSts)
{
	m_EMU3ChrgDchaConnSts = _EMU3ChrgDchaConnSts;
}
UInt32 CCpjIHU::EMU3ChrgDchaConnSts() const
{
	return m_EMU3ChrgDchaConnSts;
}
UInt32& CCpjIHU::EMU3ChrgDchaConnSts()
{
	return m_EMU3ChrgDchaConnSts;
}

void CCpjIHU::CDCU_WashCarSt(Boolean const _CDCU_WashCarSt)
{
	m_CDCU_WashCarSt = _CDCU_WashCarSt;
}
Boolean CCpjIHU::CDCU_WashCarSt() const
{
	return m_CDCU_WashCarSt;
}
Boolean& CCpjIHU::CDCU_WashCarSt()
{
	return m_CDCU_WashCarSt;
}

void CCpjIHU::CDCU_ESP_EPBReqVD(Boolean const _CDCU_ESP_EPBReqVD)
{
	m_CDCU_ESP_EPBReqVD = _CDCU_ESP_EPBReqVD;
}
Boolean CCpjIHU::CDCU_ESP_EPBReqVD() const
{
	return m_CDCU_ESP_EPBReqVD;
}
Boolean& CCpjIHU::CDCU_ESP_EPBReqVD()
{
	return m_CDCU_ESP_EPBReqVD;
}

void CCpjIHU::FrntRadarSwt(Boolean const _FrntRadarSwt)
{
	m_FrntRadarSwt = _FrntRadarSwt;
}
Boolean CCpjIHU::FrntRadarSwt() const
{
	return m_FrntRadarSwt;
}
Boolean& CCpjIHU::FrntRadarSwt()
{
	return m_FrntRadarSwt;
}

void CCpjIHU::RRMDisplayAVMOff(Boolean const _RRMDisplayAVMOff)
{
	m_RRMDisplayAVMOff = _RRMDisplayAVMOff;
}
Boolean CCpjIHU::RRMDisplayAVMOff() const
{
	return m_RRMDisplayAVMOff;
}
Boolean& CCpjIHU::RRMDisplayAVMOff()
{
	return m_RRMDisplayAVMOff;
}

void CCpjIHU::CarMdlSetColor(UInt8 const _CarMdlSetColor)
{
	m_CarMdlSetColor = _CarMdlSetColor;
}
UInt8 CCpjIHU::CarMdlSetColor() const
{
	return m_CarMdlSetColor;
}
UInt8& CCpjIHU::CarMdlSetColor()
{
	return m_CarMdlSetColor;
}

void CCpjIHU::Reset_Default(UInt8 const _Reset_Default)
{
	m_Reset_Default = _Reset_Default;
}
UInt8 CCpjIHU::Reset_Default() const
{
	return m_Reset_Default;
}
UInt8& CCpjIHU::Reset_Default()
{
	return m_Reset_Default;
}

void CCpjIHU::OffRoadState(UInt8 const _OffRoadState)
{
	m_OffRoadState = _OffRoadState;
}
UInt8 CCpjIHU::OffRoadState() const
{
	return m_OffRoadState;
}
UInt8& CCpjIHU::OffRoadState()
{
	return m_OffRoadState;
}

void CCpjIHU::padding(UInt8 const _padding)
{
	m_padding = _padding;
}
UInt8 CCpjIHU::padding() const
{
	return m_padding;
}
UInt8& CCpjIHU::padding()
{
	return m_padding;
}

void CCpjIHU::APASoftKeyRequestNoReverse(Boolean const _APASoftKeyRequestNoReverse)
{
	m_APASoftKeyRequestNoReverse = _APASoftKeyRequestNoReverse;
}
Boolean CCpjIHU::APASoftKeyRequestNoReverse() const
{
	return m_APASoftKeyRequestNoReverse;
}
Boolean& CCpjIHU::APASoftKeyRequestNoReverse()
{
	return m_APASoftKeyRequestNoReverse;
}

CCpjLDCU::CCpjLDCU()
{

}

magna::dds::DdsCdr& CCpjLDCU::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_LDCU_XPU_ChargerSt);
	cdr.serialize(m_m_LDCU_XPU_AvailableSt);
	cdr.serialize(m_m_LDCU_XPU_StopAutoDrive);
	cdr.serialize(m_m_LDCU_OverRideSt);
	cdr.serialize(m_m_LDCU_FMotorSpdDirection);
	cdr.serialize(m_m_LDCU_RodSignals);
	cdr.serialize(m_m_LDCU_DriveModeFeedback);
	cdr.serialize(m_m_LDCU_XPU_AvailStFeedback);
	cdr.serialize(m_m_LDCU_APBWarningLamp);
	cdr.serialize(m_m_LDCU_APBSysSt);
	cdr.serialize(m_m_LDCUOffroadDrvMod);
	cdr.serialize(m_m_LDCU_RWheelDriverTorqReq);
	cdr.serialize(m_m_LDCU_FWheelPeakTorqMin);
	cdr.serialize(m_m_LDCU_FWheelPeakTorqMax);
	cdr.serialize(m_m_LDCU_FWheelCurTorq);
	cdr.serialize(m_m_LDCUHIPUDesTorq);
	cdr.serialize(m_m_LDCU_FMotorSpd);
	cdr.serialize(m_m_LDCU_MMotorSpd);
	cdr.serialize(m_m_LDCU_RMotorSpd);
	cdr.serialize(m_m_LDCU_MWheelPeakTorqMin);
	cdr.serialize(m_m_LDCUIPUFDesTorq);
	cdr.serialize(m_m_LDCUIPURDesTorq);
	cdr.serialize(m_m_LDCU_MWheelPeakTorqMax);
	cdr.serialize(m_m_LDCU_MWheelCurTorq);
	cdr.serialize(m_m_LDCU_BodySysSt1);
	cdr.serialize(m_m_LDCU_MMotorSpdDirection);
	cdr.serialize(m_m_LDCU_RMotorSpdDirection);
	cdr.serialize(m_m_LDCU_RWheelPeakTorqMin);
	cdr.serialize(m_m_LDCU_RWheelPeakTorqMax);
	cdr.serialize(m_m_LDCU_RWheelCurTorq);
	cdr.serialize(m_m_LDCU_DriverDoorLockSt);
	cdr.serialize(m_m_LDCU_AccPedalSigVD);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_LDCU_EVSysReadySt);
	cdr.serialize(m_m_LDCU_TgatePosSts);
	cdr.serialize(m_m_LDCU_TgateMvtSts);
	cdr.serialize(m_m_LDCU_CapPosSts);
	cdr.serialize(m_m_LDCU_CapMvtSts);
	cdr.serialize(m_m_LDCU_TPMSPrWarnFL);
	cdr.serialize(m_m_LDCU_TPMSPrWarnFR);
	cdr.serialize(m_m_LDCU_TPMSPrWarnRL);
	cdr.serialize(m_m_LDCU_TPMSPrWarnRR);
	cdr.serialize(m_m_LDCU_TPMSPrWarn3rdL);
	cdr.serialize(m_m_LDCU_TPMSPrWarn3rdR);
	cdr.serialize(m_m_LDCU_XPU_InteractSt);

	return cdr;
}
uint32_t CCpjLDCU::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjLDCU::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_LDCU_XPU_ChargerSt);
	cdr.deserialize(m_m_LDCU_XPU_AvailableSt);
	cdr.deserialize(m_m_LDCU_XPU_StopAutoDrive);
	cdr.deserialize(m_m_LDCU_OverRideSt);
	cdr.deserialize(m_m_LDCU_FMotorSpdDirection);
	cdr.deserialize(m_m_LDCU_RodSignals);
	cdr.deserialize(m_m_LDCU_DriveModeFeedback);
	cdr.deserialize(m_m_LDCU_XPU_AvailStFeedback);
	cdr.deserialize(m_m_LDCU_APBWarningLamp);
	cdr.deserialize(m_m_LDCU_APBSysSt);
	cdr.deserialize(m_m_LDCUOffroadDrvMod);
	cdr.deserialize(m_m_LDCU_RWheelDriverTorqReq);
	cdr.deserialize(m_m_LDCU_FWheelPeakTorqMin);
	cdr.deserialize(m_m_LDCU_FWheelPeakTorqMax);
	cdr.deserialize(m_m_LDCU_FWheelCurTorq);
	cdr.deserialize(m_m_LDCUHIPUDesTorq);
	cdr.deserialize(m_m_LDCU_FMotorSpd);
	cdr.deserialize(m_m_LDCU_MMotorSpd);
	cdr.deserialize(m_m_LDCU_RMotorSpd);
	cdr.deserialize(m_m_LDCU_MWheelPeakTorqMin);
	cdr.deserialize(m_m_LDCUIPUFDesTorq);
	cdr.deserialize(m_m_LDCUIPURDesTorq);
	cdr.deserialize(m_m_LDCU_MWheelPeakTorqMax);
	cdr.deserialize(m_m_LDCU_MWheelCurTorq);
	cdr.deserialize(m_m_LDCU_BodySysSt1);
	cdr.deserialize(m_m_LDCU_MMotorSpdDirection);
	cdr.deserialize(m_m_LDCU_RMotorSpdDirection);
	cdr.deserialize(m_m_LDCU_RWheelPeakTorqMin);
	cdr.deserialize(m_m_LDCU_RWheelPeakTorqMax);
	cdr.deserialize(m_m_LDCU_RWheelCurTorq);
	cdr.deserialize(m_m_LDCU_DriverDoorLockSt);
	cdr.deserialize(m_m_LDCU_AccPedalSigVD);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_LDCU_EVSysReadySt);
	cdr.deserialize(m_m_LDCU_TgatePosSts);
	cdr.deserialize(m_m_LDCU_TgateMvtSts);
	cdr.deserialize(m_m_LDCU_CapPosSts);
	cdr.deserialize(m_m_LDCU_CapMvtSts);
	cdr.deserialize(m_m_LDCU_TPMSPrWarnFL);
	cdr.deserialize(m_m_LDCU_TPMSPrWarnFR);
	cdr.deserialize(m_m_LDCU_TPMSPrWarnRL);
	cdr.deserialize(m_m_LDCU_TPMSPrWarnRR);
	cdr.deserialize(m_m_LDCU_TPMSPrWarn3rdL);
	cdr.deserialize(m_m_LDCU_TPMSPrWarn3rdR);
	cdr.deserialize(m_m_LDCU_XPU_InteractSt);

	return cdr;
}
bool CCpjLDCU::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjLDCU::is_key_defined()
{
	return false;

}
void CCpjLDCU::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjLDCU::serialize_key(char **buf,unsigned int *len)
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
bool CCpjLDCU::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjLDCU::is_plain_types()
{
	return true;
}
uint32_t CCpjLDCU::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjLDCU::set_key_val(CCpjLDCU const* const _data) noexcept
{

}
void CCpjLDCU::m_LDCU_XPU_ChargerSt(UInt32 const _m_LDCU_XPU_ChargerSt)
{
	m_m_LDCU_XPU_ChargerSt = _m_LDCU_XPU_ChargerSt;
}
UInt32 CCpjLDCU::m_LDCU_XPU_ChargerSt() const
{
	return m_m_LDCU_XPU_ChargerSt;
}
UInt32& CCpjLDCU::m_LDCU_XPU_ChargerSt()
{
	return m_m_LDCU_XPU_ChargerSt;
}

void CCpjLDCU::m_LDCU_XPU_AvailableSt(Boolean const _m_LDCU_XPU_AvailableSt)
{
	m_m_LDCU_XPU_AvailableSt = _m_LDCU_XPU_AvailableSt;
}
Boolean CCpjLDCU::m_LDCU_XPU_AvailableSt() const
{
	return m_m_LDCU_XPU_AvailableSt;
}
Boolean& CCpjLDCU::m_LDCU_XPU_AvailableSt()
{
	return m_m_LDCU_XPU_AvailableSt;
}

void CCpjLDCU::m_LDCU_XPU_StopAutoDrive(Boolean const _m_LDCU_XPU_StopAutoDrive)
{
	m_m_LDCU_XPU_StopAutoDrive = _m_LDCU_XPU_StopAutoDrive;
}
Boolean CCpjLDCU::m_LDCU_XPU_StopAutoDrive() const
{
	return m_m_LDCU_XPU_StopAutoDrive;
}
Boolean& CCpjLDCU::m_LDCU_XPU_StopAutoDrive()
{
	return m_m_LDCU_XPU_StopAutoDrive;
}

void CCpjLDCU::m_LDCU_OverRideSt(Boolean const _m_LDCU_OverRideSt)
{
	m_m_LDCU_OverRideSt = _m_LDCU_OverRideSt;
}
Boolean CCpjLDCU::m_LDCU_OverRideSt() const
{
	return m_m_LDCU_OverRideSt;
}
Boolean& CCpjLDCU::m_LDCU_OverRideSt()
{
	return m_m_LDCU_OverRideSt;
}

void CCpjLDCU::m_LDCU_FMotorSpdDirection(Boolean const _m_LDCU_FMotorSpdDirection)
{
	m_m_LDCU_FMotorSpdDirection = _m_LDCU_FMotorSpdDirection;
}
Boolean CCpjLDCU::m_LDCU_FMotorSpdDirection() const
{
	return m_m_LDCU_FMotorSpdDirection;
}
Boolean& CCpjLDCU::m_LDCU_FMotorSpdDirection()
{
	return m_m_LDCU_FMotorSpdDirection;
}

void CCpjLDCU::m_LDCU_RodSignals(UInt32 const _m_LDCU_RodSignals)
{
	m_m_LDCU_RodSignals = _m_LDCU_RodSignals;
}
UInt32 CCpjLDCU::m_LDCU_RodSignals() const
{
	return m_m_LDCU_RodSignals;
}
UInt32& CCpjLDCU::m_LDCU_RodSignals()
{
	return m_m_LDCU_RodSignals;
}

void CCpjLDCU::m_LDCU_DriveModeFeedback(UInt32 const _m_LDCU_DriveModeFeedback)
{
	m_m_LDCU_DriveModeFeedback = _m_LDCU_DriveModeFeedback;
}
UInt32 CCpjLDCU::m_LDCU_DriveModeFeedback() const
{
	return m_m_LDCU_DriveModeFeedback;
}
UInt32& CCpjLDCU::m_LDCU_DriveModeFeedback()
{
	return m_m_LDCU_DriveModeFeedback;
}

void CCpjLDCU::m_LDCU_XPU_AvailStFeedback(UInt32 const _m_LDCU_XPU_AvailStFeedback)
{
	m_m_LDCU_XPU_AvailStFeedback = _m_LDCU_XPU_AvailStFeedback;
}
UInt32 CCpjLDCU::m_LDCU_XPU_AvailStFeedback() const
{
	return m_m_LDCU_XPU_AvailStFeedback;
}
UInt32& CCpjLDCU::m_LDCU_XPU_AvailStFeedback()
{
	return m_m_LDCU_XPU_AvailStFeedback;
}

void CCpjLDCU::m_LDCU_APBWarningLamp(UInt32 const _m_LDCU_APBWarningLamp)
{
	m_m_LDCU_APBWarningLamp = _m_LDCU_APBWarningLamp;
}
UInt32 CCpjLDCU::m_LDCU_APBWarningLamp() const
{
	return m_m_LDCU_APBWarningLamp;
}
UInt32& CCpjLDCU::m_LDCU_APBWarningLamp()
{
	return m_m_LDCU_APBWarningLamp;
}

void CCpjLDCU::m_LDCU_APBSysSt(UInt32 const _m_LDCU_APBSysSt)
{
	m_m_LDCU_APBSysSt = _m_LDCU_APBSysSt;
}
UInt32 CCpjLDCU::m_LDCU_APBSysSt() const
{
	return m_m_LDCU_APBSysSt;
}
UInt32& CCpjLDCU::m_LDCU_APBSysSt()
{
	return m_m_LDCU_APBSysSt;
}

void CCpjLDCU::m_LDCUOffroadDrvMod(UInt32 const _m_LDCUOffroadDrvMod)
{
	m_m_LDCUOffroadDrvMod = _m_LDCUOffroadDrvMod;
}
UInt32 CCpjLDCU::m_LDCUOffroadDrvMod() const
{
	return m_m_LDCUOffroadDrvMod;
}
UInt32& CCpjLDCU::m_LDCUOffroadDrvMod()
{
	return m_m_LDCUOffroadDrvMod;
}

void CCpjLDCU::m_LDCU_RWheelDriverTorqReq(SInt16 const _m_LDCU_RWheelDriverTorqReq)
{
	m_m_LDCU_RWheelDriverTorqReq = _m_LDCU_RWheelDriverTorqReq;
}
SInt16 CCpjLDCU::m_LDCU_RWheelDriverTorqReq() const
{
	return m_m_LDCU_RWheelDriverTorqReq;
}
SInt16& CCpjLDCU::m_LDCU_RWheelDriverTorqReq()
{
	return m_m_LDCU_RWheelDriverTorqReq;
}

void CCpjLDCU::m_LDCU_FWheelPeakTorqMin(SInt16 const _m_LDCU_FWheelPeakTorqMin)
{
	m_m_LDCU_FWheelPeakTorqMin = _m_LDCU_FWheelPeakTorqMin;
}
SInt16 CCpjLDCU::m_LDCU_FWheelPeakTorqMin() const
{
	return m_m_LDCU_FWheelPeakTorqMin;
}
SInt16& CCpjLDCU::m_LDCU_FWheelPeakTorqMin()
{
	return m_m_LDCU_FWheelPeakTorqMin;
}

void CCpjLDCU::m_LDCU_FWheelPeakTorqMax(SInt16 const _m_LDCU_FWheelPeakTorqMax)
{
	m_m_LDCU_FWheelPeakTorqMax = _m_LDCU_FWheelPeakTorqMax;
}
SInt16 CCpjLDCU::m_LDCU_FWheelPeakTorqMax() const
{
	return m_m_LDCU_FWheelPeakTorqMax;
}
SInt16& CCpjLDCU::m_LDCU_FWheelPeakTorqMax()
{
	return m_m_LDCU_FWheelPeakTorqMax;
}

void CCpjLDCU::m_LDCU_FWheelCurTorq(SInt16 const _m_LDCU_FWheelCurTorq)
{
	m_m_LDCU_FWheelCurTorq = _m_LDCU_FWheelCurTorq;
}
SInt16 CCpjLDCU::m_LDCU_FWheelCurTorq() const
{
	return m_m_LDCU_FWheelCurTorq;
}
SInt16& CCpjLDCU::m_LDCU_FWheelCurTorq()
{
	return m_m_LDCU_FWheelCurTorq;
}

void CCpjLDCU::m_LDCUHIPUDesTorq(Float const _m_LDCUHIPUDesTorq)
{
	m_m_LDCUHIPUDesTorq = _m_LDCUHIPUDesTorq;
}
Float CCpjLDCU::m_LDCUHIPUDesTorq() const
{
	return m_m_LDCUHIPUDesTorq;
}
Float& CCpjLDCU::m_LDCUHIPUDesTorq()
{
	return m_m_LDCUHIPUDesTorq;
}

void CCpjLDCU::m_LDCU_FMotorSpd(UInt16 const _m_LDCU_FMotorSpd)
{
	m_m_LDCU_FMotorSpd = _m_LDCU_FMotorSpd;
}
UInt16 CCpjLDCU::m_LDCU_FMotorSpd() const
{
	return m_m_LDCU_FMotorSpd;
}
UInt16& CCpjLDCU::m_LDCU_FMotorSpd()
{
	return m_m_LDCU_FMotorSpd;
}

void CCpjLDCU::m_LDCU_MMotorSpd(UInt16 const _m_LDCU_MMotorSpd)
{
	m_m_LDCU_MMotorSpd = _m_LDCU_MMotorSpd;
}
UInt16 CCpjLDCU::m_LDCU_MMotorSpd() const
{
	return m_m_LDCU_MMotorSpd;
}
UInt16& CCpjLDCU::m_LDCU_MMotorSpd()
{
	return m_m_LDCU_MMotorSpd;
}

void CCpjLDCU::m_LDCU_RMotorSpd(UInt16 const _m_LDCU_RMotorSpd)
{
	m_m_LDCU_RMotorSpd = _m_LDCU_RMotorSpd;
}
UInt16 CCpjLDCU::m_LDCU_RMotorSpd() const
{
	return m_m_LDCU_RMotorSpd;
}
UInt16& CCpjLDCU::m_LDCU_RMotorSpd()
{
	return m_m_LDCU_RMotorSpd;
}

void CCpjLDCU::m_LDCU_MWheelPeakTorqMin(SInt16 const _m_LDCU_MWheelPeakTorqMin)
{
	m_m_LDCU_MWheelPeakTorqMin = _m_LDCU_MWheelPeakTorqMin;
}
SInt16 CCpjLDCU::m_LDCU_MWheelPeakTorqMin() const
{
	return m_m_LDCU_MWheelPeakTorqMin;
}
SInt16& CCpjLDCU::m_LDCU_MWheelPeakTorqMin()
{
	return m_m_LDCU_MWheelPeakTorqMin;
}

void CCpjLDCU::m_LDCUIPUFDesTorq(Float const _m_LDCUIPUFDesTorq)
{
	m_m_LDCUIPUFDesTorq = _m_LDCUIPUFDesTorq;
}
Float CCpjLDCU::m_LDCUIPUFDesTorq() const
{
	return m_m_LDCUIPUFDesTorq;
}
Float& CCpjLDCU::m_LDCUIPUFDesTorq()
{
	return m_m_LDCUIPUFDesTorq;
}

void CCpjLDCU::m_LDCUIPURDesTorq(Float const _m_LDCUIPURDesTorq)
{
	m_m_LDCUIPURDesTorq = _m_LDCUIPURDesTorq;
}
Float CCpjLDCU::m_LDCUIPURDesTorq() const
{
	return m_m_LDCUIPURDesTorq;
}
Float& CCpjLDCU::m_LDCUIPURDesTorq()
{
	return m_m_LDCUIPURDesTorq;
}

void CCpjLDCU::m_LDCU_MWheelPeakTorqMax(SInt16 const _m_LDCU_MWheelPeakTorqMax)
{
	m_m_LDCU_MWheelPeakTorqMax = _m_LDCU_MWheelPeakTorqMax;
}
SInt16 CCpjLDCU::m_LDCU_MWheelPeakTorqMax() const
{
	return m_m_LDCU_MWheelPeakTorqMax;
}
SInt16& CCpjLDCU::m_LDCU_MWheelPeakTorqMax()
{
	return m_m_LDCU_MWheelPeakTorqMax;
}

void CCpjLDCU::m_LDCU_MWheelCurTorq(SInt16 const _m_LDCU_MWheelCurTorq)
{
	m_m_LDCU_MWheelCurTorq = _m_LDCU_MWheelCurTorq;
}
SInt16 CCpjLDCU::m_LDCU_MWheelCurTorq() const
{
	return m_m_LDCU_MWheelCurTorq;
}
SInt16& CCpjLDCU::m_LDCU_MWheelCurTorq()
{
	return m_m_LDCU_MWheelCurTorq;
}

void CCpjLDCU::m_LDCU_BodySysSt1(UInt32 const _m_LDCU_BodySysSt1)
{
	m_m_LDCU_BodySysSt1 = _m_LDCU_BodySysSt1;
}
UInt32 CCpjLDCU::m_LDCU_BodySysSt1() const
{
	return m_m_LDCU_BodySysSt1;
}
UInt32& CCpjLDCU::m_LDCU_BodySysSt1()
{
	return m_m_LDCU_BodySysSt1;
}

void CCpjLDCU::m_LDCU_MMotorSpdDirection(Boolean const _m_LDCU_MMotorSpdDirection)
{
	m_m_LDCU_MMotorSpdDirection = _m_LDCU_MMotorSpdDirection;
}
Boolean CCpjLDCU::m_LDCU_MMotorSpdDirection() const
{
	return m_m_LDCU_MMotorSpdDirection;
}
Boolean& CCpjLDCU::m_LDCU_MMotorSpdDirection()
{
	return m_m_LDCU_MMotorSpdDirection;
}

void CCpjLDCU::m_LDCU_RMotorSpdDirection(Boolean const _m_LDCU_RMotorSpdDirection)
{
	m_m_LDCU_RMotorSpdDirection = _m_LDCU_RMotorSpdDirection;
}
Boolean CCpjLDCU::m_LDCU_RMotorSpdDirection() const
{
	return m_m_LDCU_RMotorSpdDirection;
}
Boolean& CCpjLDCU::m_LDCU_RMotorSpdDirection()
{
	return m_m_LDCU_RMotorSpdDirection;
}

void CCpjLDCU::m_LDCU_RWheelPeakTorqMin(SInt16 const _m_LDCU_RWheelPeakTorqMin)
{
	m_m_LDCU_RWheelPeakTorqMin = _m_LDCU_RWheelPeakTorqMin;
}
SInt16 CCpjLDCU::m_LDCU_RWheelPeakTorqMin() const
{
	return m_m_LDCU_RWheelPeakTorqMin;
}
SInt16& CCpjLDCU::m_LDCU_RWheelPeakTorqMin()
{
	return m_m_LDCU_RWheelPeakTorqMin;
}

void CCpjLDCU::m_LDCU_RWheelPeakTorqMax(SInt16 const _m_LDCU_RWheelPeakTorqMax)
{
	m_m_LDCU_RWheelPeakTorqMax = _m_LDCU_RWheelPeakTorqMax;
}
SInt16 CCpjLDCU::m_LDCU_RWheelPeakTorqMax() const
{
	return m_m_LDCU_RWheelPeakTorqMax;
}
SInt16& CCpjLDCU::m_LDCU_RWheelPeakTorqMax()
{
	return m_m_LDCU_RWheelPeakTorqMax;
}

void CCpjLDCU::m_LDCU_RWheelCurTorq(SInt16 const _m_LDCU_RWheelCurTorq)
{
	m_m_LDCU_RWheelCurTorq = _m_LDCU_RWheelCurTorq;
}
SInt16 CCpjLDCU::m_LDCU_RWheelCurTorq() const
{
	return m_m_LDCU_RWheelCurTorq;
}
SInt16& CCpjLDCU::m_LDCU_RWheelCurTorq()
{
	return m_m_LDCU_RWheelCurTorq;
}

void CCpjLDCU::m_LDCU_DriverDoorLockSt(Boolean const _m_LDCU_DriverDoorLockSt)
{
	m_m_LDCU_DriverDoorLockSt = _m_LDCU_DriverDoorLockSt;
}
Boolean CCpjLDCU::m_LDCU_DriverDoorLockSt() const
{
	return m_m_LDCU_DriverDoorLockSt;
}
Boolean& CCpjLDCU::m_LDCU_DriverDoorLockSt()
{
	return m_m_LDCU_DriverDoorLockSt;
}

void CCpjLDCU::m_LDCU_AccPedalSigVD(Boolean const _m_LDCU_AccPedalSigVD)
{
	m_m_LDCU_AccPedalSigVD = _m_LDCU_AccPedalSigVD;
}
Boolean CCpjLDCU::m_LDCU_AccPedalSigVD() const
{
	return m_m_LDCU_AccPedalSigVD;
}
Boolean& CCpjLDCU::m_LDCU_AccPedalSigVD()
{
	return m_m_LDCU_AccPedalSigVD;
}

void CCpjLDCU::padding(UInt16 const _padding)
{
	m_padding = _padding;
}
UInt16 CCpjLDCU::padding() const
{
	return m_padding;
}
UInt16& CCpjLDCU::padding()
{
	return m_padding;
}

void CCpjLDCU::m_LDCU_EVSysReadySt(UInt32 const _m_LDCU_EVSysReadySt)
{
	m_m_LDCU_EVSysReadySt = _m_LDCU_EVSysReadySt;
}
UInt32 CCpjLDCU::m_LDCU_EVSysReadySt() const
{
	return m_m_LDCU_EVSysReadySt;
}
UInt32& CCpjLDCU::m_LDCU_EVSysReadySt()
{
	return m_m_LDCU_EVSysReadySt;
}

void CCpjLDCU::m_LDCU_TgatePosSts(UInt32 const _m_LDCU_TgatePosSts)
{
	m_m_LDCU_TgatePosSts = _m_LDCU_TgatePosSts;
}
UInt32 CCpjLDCU::m_LDCU_TgatePosSts() const
{
	return m_m_LDCU_TgatePosSts;
}
UInt32& CCpjLDCU::m_LDCU_TgatePosSts()
{
	return m_m_LDCU_TgatePosSts;
}

void CCpjLDCU::m_LDCU_TgateMvtSts(UInt32 const _m_LDCU_TgateMvtSts)
{
	m_m_LDCU_TgateMvtSts = _m_LDCU_TgateMvtSts;
}
UInt32 CCpjLDCU::m_LDCU_TgateMvtSts() const
{
	return m_m_LDCU_TgateMvtSts;
}
UInt32& CCpjLDCU::m_LDCU_TgateMvtSts()
{
	return m_m_LDCU_TgateMvtSts;
}

void CCpjLDCU::m_LDCU_CapPosSts(UInt32 const _m_LDCU_CapPosSts)
{
	m_m_LDCU_CapPosSts = _m_LDCU_CapPosSts;
}
UInt32 CCpjLDCU::m_LDCU_CapPosSts() const
{
	return m_m_LDCU_CapPosSts;
}
UInt32& CCpjLDCU::m_LDCU_CapPosSts()
{
	return m_m_LDCU_CapPosSts;
}

void CCpjLDCU::m_LDCU_CapMvtSts(UInt32 const _m_LDCU_CapMvtSts)
{
	m_m_LDCU_CapMvtSts = _m_LDCU_CapMvtSts;
}
UInt32 CCpjLDCU::m_LDCU_CapMvtSts() const
{
	return m_m_LDCU_CapMvtSts;
}
UInt32& CCpjLDCU::m_LDCU_CapMvtSts()
{
	return m_m_LDCU_CapMvtSts;
}

void CCpjLDCU::m_LDCU_TPMSPrWarnFL(UInt32 const _m_LDCU_TPMSPrWarnFL)
{
	m_m_LDCU_TPMSPrWarnFL = _m_LDCU_TPMSPrWarnFL;
}
UInt32 CCpjLDCU::m_LDCU_TPMSPrWarnFL() const
{
	return m_m_LDCU_TPMSPrWarnFL;
}
UInt32& CCpjLDCU::m_LDCU_TPMSPrWarnFL()
{
	return m_m_LDCU_TPMSPrWarnFL;
}

void CCpjLDCU::m_LDCU_TPMSPrWarnFR(UInt32 const _m_LDCU_TPMSPrWarnFR)
{
	m_m_LDCU_TPMSPrWarnFR = _m_LDCU_TPMSPrWarnFR;
}
UInt32 CCpjLDCU::m_LDCU_TPMSPrWarnFR() const
{
	return m_m_LDCU_TPMSPrWarnFR;
}
UInt32& CCpjLDCU::m_LDCU_TPMSPrWarnFR()
{
	return m_m_LDCU_TPMSPrWarnFR;
}

void CCpjLDCU::m_LDCU_TPMSPrWarnRL(UInt32 const _m_LDCU_TPMSPrWarnRL)
{
	m_m_LDCU_TPMSPrWarnRL = _m_LDCU_TPMSPrWarnRL;
}
UInt32 CCpjLDCU::m_LDCU_TPMSPrWarnRL() const
{
	return m_m_LDCU_TPMSPrWarnRL;
}
UInt32& CCpjLDCU::m_LDCU_TPMSPrWarnRL()
{
	return m_m_LDCU_TPMSPrWarnRL;
}

void CCpjLDCU::m_LDCU_TPMSPrWarnRR(UInt32 const _m_LDCU_TPMSPrWarnRR)
{
	m_m_LDCU_TPMSPrWarnRR = _m_LDCU_TPMSPrWarnRR;
}
UInt32 CCpjLDCU::m_LDCU_TPMSPrWarnRR() const
{
	return m_m_LDCU_TPMSPrWarnRR;
}
UInt32& CCpjLDCU::m_LDCU_TPMSPrWarnRR()
{
	return m_m_LDCU_TPMSPrWarnRR;
}

void CCpjLDCU::m_LDCU_TPMSPrWarn3rdL(UInt32 const _m_LDCU_TPMSPrWarn3rdL)
{
	m_m_LDCU_TPMSPrWarn3rdL = _m_LDCU_TPMSPrWarn3rdL;
}
UInt32 CCpjLDCU::m_LDCU_TPMSPrWarn3rdL() const
{
	return m_m_LDCU_TPMSPrWarn3rdL;
}
UInt32& CCpjLDCU::m_LDCU_TPMSPrWarn3rdL()
{
	return m_m_LDCU_TPMSPrWarn3rdL;
}

void CCpjLDCU::m_LDCU_TPMSPrWarn3rdR(UInt32 const _m_LDCU_TPMSPrWarn3rdR)
{
	m_m_LDCU_TPMSPrWarn3rdR = _m_LDCU_TPMSPrWarn3rdR;
}
UInt32 CCpjLDCU::m_LDCU_TPMSPrWarn3rdR() const
{
	return m_m_LDCU_TPMSPrWarn3rdR;
}
UInt32& CCpjLDCU::m_LDCU_TPMSPrWarn3rdR()
{
	return m_m_LDCU_TPMSPrWarn3rdR;
}

void CCpjLDCU::m_LDCU_XPU_InteractSt(UInt32 const _m_LDCU_XPU_InteractSt)
{
	m_m_LDCU_XPU_InteractSt = _m_LDCU_XPU_InteractSt;
}
UInt32 CCpjLDCU::m_LDCU_XPU_InteractSt() const
{
	return m_m_LDCU_XPU_InteractSt;
}
UInt32& CCpjLDCU::m_LDCU_XPU_InteractSt()
{
	return m_m_LDCU_XPU_InteractSt;
}

CCpjLongCtrl::CCpjLongCtrl()
{

}

magna::dds::DdsCdr& CCpjLongCtrl::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_ESC_LC_FailureSts);
	cdr.serialize(m_m_ESC_LC_Status);
	cdr.serialize(m_m_ESC_LC_Availability);
	cdr.serialize(m_m_Active_Vehicle_Hold);
	cdr.serialize(m_m_ESC_Switch_b);
	cdr.serialize(m_m_VehInAutoHold_b);
	cdr.serialize(m_m_MotorHomeConnect_b);
	cdr.serialize(m_m_GasPedalPos_f32);

	return cdr;
}
uint32_t CCpjLongCtrl::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjLongCtrl::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_ESC_LC_FailureSts);
	cdr.deserialize(m_m_ESC_LC_Status);
	cdr.deserialize(m_m_ESC_LC_Availability);
	cdr.deserialize(m_m_Active_Vehicle_Hold);
	cdr.deserialize(m_m_ESC_Switch_b);
	cdr.deserialize(m_m_VehInAutoHold_b);
	cdr.deserialize(m_m_MotorHomeConnect_b);
	cdr.deserialize(m_m_GasPedalPos_f32);

	return cdr;
}
bool CCpjLongCtrl::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjLongCtrl::is_key_defined()
{
	return false;

}
void CCpjLongCtrl::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjLongCtrl::serialize_key(char **buf,unsigned int *len)
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
bool CCpjLongCtrl::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjLongCtrl::is_plain_types()
{
	return true;
}
uint32_t CCpjLongCtrl::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjLongCtrl::set_key_val(CCpjLongCtrl const* const _data) noexcept
{

}
void CCpjLongCtrl::m_ESC_LC_FailureSts(UInt32 const _m_ESC_LC_FailureSts)
{
	m_m_ESC_LC_FailureSts = _m_ESC_LC_FailureSts;
}
UInt32 CCpjLongCtrl::m_ESC_LC_FailureSts() const
{
	return m_m_ESC_LC_FailureSts;
}
UInt32& CCpjLongCtrl::m_ESC_LC_FailureSts()
{
	return m_m_ESC_LC_FailureSts;
}

void CCpjLongCtrl::m_ESC_LC_Status(UInt32 const _m_ESC_LC_Status)
{
	m_m_ESC_LC_Status = _m_ESC_LC_Status;
}
UInt32 CCpjLongCtrl::m_ESC_LC_Status() const
{
	return m_m_ESC_LC_Status;
}
UInt32& CCpjLongCtrl::m_ESC_LC_Status()
{
	return m_m_ESC_LC_Status;
}

void CCpjLongCtrl::m_ESC_LC_Availability(UInt32 const _m_ESC_LC_Availability)
{
	m_m_ESC_LC_Availability = _m_ESC_LC_Availability;
}
UInt32 CCpjLongCtrl::m_ESC_LC_Availability() const
{
	return m_m_ESC_LC_Availability;
}
UInt32& CCpjLongCtrl::m_ESC_LC_Availability()
{
	return m_m_ESC_LC_Availability;
}

void CCpjLongCtrl::m_Active_Vehicle_Hold(Boolean const _m_Active_Vehicle_Hold)
{
	m_m_Active_Vehicle_Hold = _m_Active_Vehicle_Hold;
}
Boolean CCpjLongCtrl::m_Active_Vehicle_Hold() const
{
	return m_m_Active_Vehicle_Hold;
}
Boolean& CCpjLongCtrl::m_Active_Vehicle_Hold()
{
	return m_m_Active_Vehicle_Hold;
}

void CCpjLongCtrl::m_ESC_Switch_b(Boolean const _m_ESC_Switch_b)
{
	m_m_ESC_Switch_b = _m_ESC_Switch_b;
}
Boolean CCpjLongCtrl::m_ESC_Switch_b() const
{
	return m_m_ESC_Switch_b;
}
Boolean& CCpjLongCtrl::m_ESC_Switch_b()
{
	return m_m_ESC_Switch_b;
}

void CCpjLongCtrl::m_VehInAutoHold_b(Boolean const _m_VehInAutoHold_b)
{
	m_m_VehInAutoHold_b = _m_VehInAutoHold_b;
}
Boolean CCpjLongCtrl::m_VehInAutoHold_b() const
{
	return m_m_VehInAutoHold_b;
}
Boolean& CCpjLongCtrl::m_VehInAutoHold_b()
{
	return m_m_VehInAutoHold_b;
}

void CCpjLongCtrl::m_MotorHomeConnect_b(Boolean const _m_MotorHomeConnect_b)
{
	m_m_MotorHomeConnect_b = _m_MotorHomeConnect_b;
}
Boolean CCpjLongCtrl::m_MotorHomeConnect_b() const
{
	return m_m_MotorHomeConnect_b;
}
Boolean& CCpjLongCtrl::m_MotorHomeConnect_b()
{
	return m_m_MotorHomeConnect_b;
}

void CCpjLongCtrl::m_GasPedalPos_f32(Float const _m_GasPedalPos_f32)
{
	m_m_GasPedalPos_f32 = _m_GasPedalPos_f32;
}
Float CCpjLongCtrl::m_GasPedalPos_f32() const
{
	return m_m_GasPedalPos_f32;
}
Float& CCpjLongCtrl::m_GasPedalPos_f32()
{
	return m_m_GasPedalPos_f32;
}

CCpjRctaWarning::CCpjRctaWarning()
{

}

magna::dds::DdsCdr& CCpjRctaWarning::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_rctaWarningLH);
	cdr.serialize(m_m_rctaWarningRH);

	return cdr;
}
uint32_t CCpjRctaWarning::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjRctaWarning::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_rctaWarningLH);
	cdr.deserialize(m_m_rctaWarningRH);

	return cdr;
}
bool CCpjRctaWarning::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjRctaWarning::is_key_defined()
{
	return false;

}
void CCpjRctaWarning::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjRctaWarning::serialize_key(char **buf,unsigned int *len)
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
bool CCpjRctaWarning::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjRctaWarning::is_plain_types()
{
	return true;
}
uint32_t CCpjRctaWarning::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjRctaWarning::set_key_val(CCpjRctaWarning const* const _data) noexcept
{

}
void CCpjRctaWarning::m_rctaWarningLH(UInt32 const _m_rctaWarningLH)
{
	m_m_rctaWarningLH = _m_rctaWarningLH;
}
UInt32 CCpjRctaWarning::m_rctaWarningLH() const
{
	return m_m_rctaWarningLH;
}
UInt32& CCpjRctaWarning::m_rctaWarningLH()
{
	return m_m_rctaWarningLH;
}

void CCpjRctaWarning::m_rctaWarningRH(UInt32 const _m_rctaWarningRH)
{
	m_m_rctaWarningRH = _m_rctaWarningRH;
}
UInt32 CCpjRctaWarning::m_rctaWarningRH() const
{
	return m_m_rctaWarningRH;
}
UInt32& CCpjRctaWarning::m_rctaWarningRH()
{
	return m_m_rctaWarningRH;
}

CCpjTDCU::CCpjTDCU()
{

}

magna::dds::DdsCdr& CCpjTDCU::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_TDCU_XPU_FunctionMode);
	cdr.serialize(m_m_TDCU_XPU_SteerAngleReqVal);
	cdr.serialize(m_m_TDCU_XPU_AngCtrlReq);
	cdr.serialize(m_m_TDCU_XPU_EPBReq);
	cdr.serialize(m_m_TDCU_XPU_SteerAngleReqValVD);
	cdr.serialize(m_m_TDCU_XPU_EPBReqVD);
	cdr.serialize(m_m_TdcuAsHgtFLmm);
	cdr.serialize(m_m_TdcuAsHgtFRmm);
	cdr.serialize(m_m_TdcuAsHgtMLmm);
	cdr.serialize(m_m_TdcuAsHgtMRmm);
	cdr.serialize(m_m_TdcuAsHgtRLmm);
	cdr.serialize(m_m_TdcuAsHgtRRmm);
	cdr.serialize(m_m_TdcuSepAndComSt);
	cdr.serialize(m_m_TdcuSNCParkReq);
	cdr.serialize(m_m_W_VoteNavHorPosType);
	cdr.serialize(m_m_TDCU_YAWRate);
	cdr.serialize(m_m_TdcuDistanceX_L);
	cdr.serialize(m_m_TdcuDistanceX_R);
	cdr.serialize(m_m_TdcuLADistanceX);
	cdr.serialize(m_m_TdcuDistanceX_L_VD);
	cdr.serialize(m_m_TdcuDistanceX_R_VD);
	cdr.serialize(m_m_TDCU_SepComRKEEnable);
	cdr.serialize(m_m_TdcuSNCParkPauseReq);
	cdr.serialize(m_m_TdcuLADistanceX_VD);
	cdr.serialize(m_padding);

	return cdr;
}
uint32_t CCpjTDCU::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjTDCU::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_TDCU_XPU_FunctionMode);
	cdr.deserialize(m_m_TDCU_XPU_SteerAngleReqVal);
	cdr.deserialize(m_m_TDCU_XPU_AngCtrlReq);
	cdr.deserialize(m_m_TDCU_XPU_EPBReq);
	cdr.deserialize(m_m_TDCU_XPU_SteerAngleReqValVD);
	cdr.deserialize(m_m_TDCU_XPU_EPBReqVD);
	cdr.deserialize(m_m_TdcuAsHgtFLmm);
	cdr.deserialize(m_m_TdcuAsHgtFRmm);
	cdr.deserialize(m_m_TdcuAsHgtMLmm);
	cdr.deserialize(m_m_TdcuAsHgtMRmm);
	cdr.deserialize(m_m_TdcuAsHgtRLmm);
	cdr.deserialize(m_m_TdcuAsHgtRRmm);
	cdr.deserialize(m_m_TdcuSepAndComSt);
	cdr.deserialize(m_m_TdcuSNCParkReq);
	cdr.deserialize(m_m_W_VoteNavHorPosType);
	cdr.deserialize(m_m_TDCU_YAWRate);
	cdr.deserialize(m_m_TdcuDistanceX_L);
	cdr.deserialize(m_m_TdcuDistanceX_R);
	cdr.deserialize(m_m_TdcuLADistanceX);
	cdr.deserialize(m_m_TdcuDistanceX_L_VD);
	cdr.deserialize(m_m_TdcuDistanceX_R_VD);
	cdr.deserialize(m_m_TDCU_SepComRKEEnable);
	cdr.deserialize(m_m_TdcuSNCParkPauseReq);
	cdr.deserialize(m_m_TdcuLADistanceX_VD);
	cdr.deserialize(m_padding);

	return cdr;
}
bool CCpjTDCU::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjTDCU::is_key_defined()
{
	return false;

}
void CCpjTDCU::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjTDCU::serialize_key(char **buf,unsigned int *len)
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
bool CCpjTDCU::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjTDCU::is_plain_types()
{
	return true;
}
uint32_t CCpjTDCU::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjTDCU::set_key_val(CCpjTDCU const* const _data) noexcept
{

}
void CCpjTDCU::m_TDCU_XPU_FunctionMode(UInt32 const _m_TDCU_XPU_FunctionMode)
{
	m_m_TDCU_XPU_FunctionMode = _m_TDCU_XPU_FunctionMode;
}
UInt32 CCpjTDCU::m_TDCU_XPU_FunctionMode() const
{
	return m_m_TDCU_XPU_FunctionMode;
}
UInt32& CCpjTDCU::m_TDCU_XPU_FunctionMode()
{
	return m_m_TDCU_XPU_FunctionMode;
}

void CCpjTDCU::m_TDCU_XPU_SteerAngleReqVal(Float const _m_TDCU_XPU_SteerAngleReqVal)
{
	m_m_TDCU_XPU_SteerAngleReqVal = _m_TDCU_XPU_SteerAngleReqVal;
}
Float CCpjTDCU::m_TDCU_XPU_SteerAngleReqVal() const
{
	return m_m_TDCU_XPU_SteerAngleReqVal;
}
Float& CCpjTDCU::m_TDCU_XPU_SteerAngleReqVal()
{
	return m_m_TDCU_XPU_SteerAngleReqVal;
}

void CCpjTDCU::m_TDCU_XPU_AngCtrlReq(UInt32 const _m_TDCU_XPU_AngCtrlReq)
{
	m_m_TDCU_XPU_AngCtrlReq = _m_TDCU_XPU_AngCtrlReq;
}
UInt32 CCpjTDCU::m_TDCU_XPU_AngCtrlReq() const
{
	return m_m_TDCU_XPU_AngCtrlReq;
}
UInt32& CCpjTDCU::m_TDCU_XPU_AngCtrlReq()
{
	return m_m_TDCU_XPU_AngCtrlReq;
}

void CCpjTDCU::m_TDCU_XPU_EPBReq(UInt32 const _m_TDCU_XPU_EPBReq)
{
	m_m_TDCU_XPU_EPBReq = _m_TDCU_XPU_EPBReq;
}
UInt32 CCpjTDCU::m_TDCU_XPU_EPBReq() const
{
	return m_m_TDCU_XPU_EPBReq;
}
UInt32& CCpjTDCU::m_TDCU_XPU_EPBReq()
{
	return m_m_TDCU_XPU_EPBReq;
}

void CCpjTDCU::m_TDCU_XPU_SteerAngleReqValVD(Boolean const _m_TDCU_XPU_SteerAngleReqValVD)
{
	m_m_TDCU_XPU_SteerAngleReqValVD = _m_TDCU_XPU_SteerAngleReqValVD;
}
Boolean CCpjTDCU::m_TDCU_XPU_SteerAngleReqValVD() const
{
	return m_m_TDCU_XPU_SteerAngleReqValVD;
}
Boolean& CCpjTDCU::m_TDCU_XPU_SteerAngleReqValVD()
{
	return m_m_TDCU_XPU_SteerAngleReqValVD;
}

void CCpjTDCU::m_TDCU_XPU_EPBReqVD(Boolean const _m_TDCU_XPU_EPBReqVD)
{
	m_m_TDCU_XPU_EPBReqVD = _m_TDCU_XPU_EPBReqVD;
}
Boolean CCpjTDCU::m_TDCU_XPU_EPBReqVD() const
{
	return m_m_TDCU_XPU_EPBReqVD;
}
Boolean& CCpjTDCU::m_TDCU_XPU_EPBReqVD()
{
	return m_m_TDCU_XPU_EPBReqVD;
}

void CCpjTDCU::m_TdcuAsHgtFLmm(SInt8 const _m_TdcuAsHgtFLmm)
{
	m_m_TdcuAsHgtFLmm = _m_TdcuAsHgtFLmm;
}
SInt8 CCpjTDCU::m_TdcuAsHgtFLmm() const
{
	return m_m_TdcuAsHgtFLmm;
}
SInt8& CCpjTDCU::m_TdcuAsHgtFLmm()
{
	return m_m_TdcuAsHgtFLmm;
}

void CCpjTDCU::m_TdcuAsHgtFRmm(SInt8 const _m_TdcuAsHgtFRmm)
{
	m_m_TdcuAsHgtFRmm = _m_TdcuAsHgtFRmm;
}
SInt8 CCpjTDCU::m_TdcuAsHgtFRmm() const
{
	return m_m_TdcuAsHgtFRmm;
}
SInt8& CCpjTDCU::m_TdcuAsHgtFRmm()
{
	return m_m_TdcuAsHgtFRmm;
}

void CCpjTDCU::m_TdcuAsHgtMLmm(SInt8 const _m_TdcuAsHgtMLmm)
{
	m_m_TdcuAsHgtMLmm = _m_TdcuAsHgtMLmm;
}
SInt8 CCpjTDCU::m_TdcuAsHgtMLmm() const
{
	return m_m_TdcuAsHgtMLmm;
}
SInt8& CCpjTDCU::m_TdcuAsHgtMLmm()
{
	return m_m_TdcuAsHgtMLmm;
}

void CCpjTDCU::m_TdcuAsHgtMRmm(SInt8 const _m_TdcuAsHgtMRmm)
{
	m_m_TdcuAsHgtMRmm = _m_TdcuAsHgtMRmm;
}
SInt8 CCpjTDCU::m_TdcuAsHgtMRmm() const
{
	return m_m_TdcuAsHgtMRmm;
}
SInt8& CCpjTDCU::m_TdcuAsHgtMRmm()
{
	return m_m_TdcuAsHgtMRmm;
}

void CCpjTDCU::m_TdcuAsHgtRLmm(SInt8 const _m_TdcuAsHgtRLmm)
{
	m_m_TdcuAsHgtRLmm = _m_TdcuAsHgtRLmm;
}
SInt8 CCpjTDCU::m_TdcuAsHgtRLmm() const
{
	return m_m_TdcuAsHgtRLmm;
}
SInt8& CCpjTDCU::m_TdcuAsHgtRLmm()
{
	return m_m_TdcuAsHgtRLmm;
}

void CCpjTDCU::m_TdcuAsHgtRRmm(SInt8 const _m_TdcuAsHgtRRmm)
{
	m_m_TdcuAsHgtRRmm = _m_TdcuAsHgtRRmm;
}
SInt8 CCpjTDCU::m_TdcuAsHgtRRmm() const
{
	return m_m_TdcuAsHgtRRmm;
}
SInt8& CCpjTDCU::m_TdcuAsHgtRRmm()
{
	return m_m_TdcuAsHgtRRmm;
}

void CCpjTDCU::m_TdcuSepAndComSt(UInt32 const _m_TdcuSepAndComSt)
{
	m_m_TdcuSepAndComSt = _m_TdcuSepAndComSt;
}
UInt32 CCpjTDCU::m_TdcuSepAndComSt() const
{
	return m_m_TdcuSepAndComSt;
}
UInt32& CCpjTDCU::m_TdcuSepAndComSt()
{
	return m_m_TdcuSepAndComSt;
}

void CCpjTDCU::m_TdcuSNCParkReq(UInt32 const _m_TdcuSNCParkReq)
{
	m_m_TdcuSNCParkReq = _m_TdcuSNCParkReq;
}
UInt32 CCpjTDCU::m_TdcuSNCParkReq() const
{
	return m_m_TdcuSNCParkReq;
}
UInt32& CCpjTDCU::m_TdcuSNCParkReq()
{
	return m_m_TdcuSNCParkReq;
}

void CCpjTDCU::m_W_VoteNavHorPosType(UInt32 const _m_W_VoteNavHorPosType)
{
	m_m_W_VoteNavHorPosType = _m_W_VoteNavHorPosType;
}
UInt32 CCpjTDCU::m_W_VoteNavHorPosType() const
{
	return m_m_W_VoteNavHorPosType;
}
UInt32& CCpjTDCU::m_W_VoteNavHorPosType()
{
	return m_m_W_VoteNavHorPosType;
}

void CCpjTDCU::m_TDCU_YAWRate(Float const _m_TDCU_YAWRate)
{
	m_m_TDCU_YAWRate = _m_TDCU_YAWRate;
}
Float CCpjTDCU::m_TDCU_YAWRate() const
{
	return m_m_TDCU_YAWRate;
}
Float& CCpjTDCU::m_TDCU_YAWRate()
{
	return m_m_TDCU_YAWRate;
}

void CCpjTDCU::m_TdcuDistanceX_L(Float const _m_TdcuDistanceX_L)
{
	m_m_TdcuDistanceX_L = _m_TdcuDistanceX_L;
}
Float CCpjTDCU::m_TdcuDistanceX_L() const
{
	return m_m_TdcuDistanceX_L;
}
Float& CCpjTDCU::m_TdcuDistanceX_L()
{
	return m_m_TdcuDistanceX_L;
}

void CCpjTDCU::m_TdcuDistanceX_R(Float const _m_TdcuDistanceX_R)
{
	m_m_TdcuDistanceX_R = _m_TdcuDistanceX_R;
}
Float CCpjTDCU::m_TdcuDistanceX_R() const
{
	return m_m_TdcuDistanceX_R;
}
Float& CCpjTDCU::m_TdcuDistanceX_R()
{
	return m_m_TdcuDistanceX_R;
}

void CCpjTDCU::m_TdcuLADistanceX(Float const _m_TdcuLADistanceX)
{
	m_m_TdcuLADistanceX = _m_TdcuLADistanceX;
}
Float CCpjTDCU::m_TdcuLADistanceX() const
{
	return m_m_TdcuLADistanceX;
}
Float& CCpjTDCU::m_TdcuLADistanceX()
{
	return m_m_TdcuLADistanceX;
}

void CCpjTDCU::m_TdcuDistanceX_L_VD(UInt32 const _m_TdcuDistanceX_L_VD)
{
	m_m_TdcuDistanceX_L_VD = _m_TdcuDistanceX_L_VD;
}
UInt32 CCpjTDCU::m_TdcuDistanceX_L_VD() const
{
	return m_m_TdcuDistanceX_L_VD;
}
UInt32& CCpjTDCU::m_TdcuDistanceX_L_VD()
{
	return m_m_TdcuDistanceX_L_VD;
}

void CCpjTDCU::m_TdcuDistanceX_R_VD(UInt32 const _m_TdcuDistanceX_R_VD)
{
	m_m_TdcuDistanceX_R_VD = _m_TdcuDistanceX_R_VD;
}
UInt32 CCpjTDCU::m_TdcuDistanceX_R_VD() const
{
	return m_m_TdcuDistanceX_R_VD;
}
UInt32& CCpjTDCU::m_TdcuDistanceX_R_VD()
{
	return m_m_TdcuDistanceX_R_VD;
}

void CCpjTDCU::m_TDCU_SepComRKEEnable(Boolean const _m_TDCU_SepComRKEEnable)
{
	m_m_TDCU_SepComRKEEnable = _m_TDCU_SepComRKEEnable;
}
Boolean CCpjTDCU::m_TDCU_SepComRKEEnable() const
{
	return m_m_TDCU_SepComRKEEnable;
}
Boolean& CCpjTDCU::m_TDCU_SepComRKEEnable()
{
	return m_m_TDCU_SepComRKEEnable;
}

void CCpjTDCU::m_TdcuSNCParkPauseReq(Boolean const _m_TdcuSNCParkPauseReq)
{
	m_m_TdcuSNCParkPauseReq = _m_TdcuSNCParkPauseReq;
}
Boolean CCpjTDCU::m_TdcuSNCParkPauseReq() const
{
	return m_m_TdcuSNCParkPauseReq;
}
Boolean& CCpjTDCU::m_TdcuSNCParkPauseReq()
{
	return m_m_TdcuSNCParkPauseReq;
}

void CCpjTDCU::m_TdcuLADistanceX_VD(Boolean const _m_TdcuLADistanceX_VD)
{
	m_m_TdcuLADistanceX_VD = _m_TdcuLADistanceX_VD;
}
Boolean CCpjTDCU::m_TdcuLADistanceX_VD() const
{
	return m_m_TdcuLADistanceX_VD;
}
Boolean& CCpjTDCU::m_TdcuLADistanceX_VD()
{
	return m_m_TdcuLADistanceX_VD;
}

void CCpjTDCU::padding(UInt8 const _padding)
{
	m_padding = _padding;
}
UInt8 CCpjTDCU::padding() const
{
	return m_padding;
}
UInt8& CCpjTDCU::padding()
{
	return m_padding;
}

CCpjVehInfo::CCpjVehInfo()
{

}

magna::dds::DdsCdr& CCpjVehInfo::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_AccModeIsActive);
	cdr.serialize(m_m_VehIsCrashed);
	cdr.serialize(m_m_AEBDecCtrlIsReq);
	cdr.serialize(m_m_EBDDecCtrlIsReq);
	cdr.serialize(m_m_AssociatedSysFctActive_b);
	cdr.serialize(m_m_ActSafeFctActive_b);
	cdr.serialize(m_m_RCTBDecCtrlReq);
	cdr.serialize(m_m_CddHoldActive);
	cdr.serialize(m_m_Stat_RCTB);
	cdr.serialize(m_m_RCTBTgtDecel);
	cdr.serialize(m_m_ParkingFaultCluster);
	cdr.serialize(m_m_NoBrakeForce);
	cdr.serialize(m_m_VehIsPowerLow);
	cdr.serialize(m_m_ChargeConnected);
	cdr.serialize(m_m_CameraIsBlocked);
	cdr.serialize(m_m_VehPowerValue);
	cdr.serialize(m_m_VehRemDrvRange);

	return cdr;
}
uint32_t CCpjVehInfo::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjVehInfo::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_AccModeIsActive);
	cdr.deserialize(m_m_VehIsCrashed);
	cdr.deserialize(m_m_AEBDecCtrlIsReq);
	cdr.deserialize(m_m_EBDDecCtrlIsReq);
	cdr.deserialize(m_m_AssociatedSysFctActive_b);
	cdr.deserialize(m_m_ActSafeFctActive_b);
	cdr.deserialize(m_m_RCTBDecCtrlReq);
	cdr.deserialize(m_m_CddHoldActive);
	cdr.deserialize(m_m_Stat_RCTB);
	cdr.deserialize(m_m_RCTBTgtDecel);
	cdr.deserialize(m_m_ParkingFaultCluster);
	cdr.deserialize(m_m_NoBrakeForce);
	cdr.deserialize(m_m_VehIsPowerLow);
	cdr.deserialize(m_m_ChargeConnected);
	cdr.deserialize(m_m_CameraIsBlocked);
	cdr.deserialize(m_m_VehPowerValue);
	cdr.deserialize(m_m_VehRemDrvRange);

	return cdr;
}
bool CCpjVehInfo::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjVehInfo::is_key_defined()
{
	return false;

}
void CCpjVehInfo::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjVehInfo::serialize_key(char **buf,unsigned int *len)
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
bool CCpjVehInfo::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjVehInfo::is_plain_types()
{
	return true;
}
uint32_t CCpjVehInfo::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjVehInfo::set_key_val(CCpjVehInfo const* const _data) noexcept
{

}
void CCpjVehInfo::m_AccModeIsActive(Boolean const _m_AccModeIsActive)
{
	m_m_AccModeIsActive = _m_AccModeIsActive;
}
Boolean CCpjVehInfo::m_AccModeIsActive() const
{
	return m_m_AccModeIsActive;
}
Boolean& CCpjVehInfo::m_AccModeIsActive()
{
	return m_m_AccModeIsActive;
}

void CCpjVehInfo::m_VehIsCrashed(Boolean const _m_VehIsCrashed)
{
	m_m_VehIsCrashed = _m_VehIsCrashed;
}
Boolean CCpjVehInfo::m_VehIsCrashed() const
{
	return m_m_VehIsCrashed;
}
Boolean& CCpjVehInfo::m_VehIsCrashed()
{
	return m_m_VehIsCrashed;
}

void CCpjVehInfo::m_AEBDecCtrlIsReq(Boolean const _m_AEBDecCtrlIsReq)
{
	m_m_AEBDecCtrlIsReq = _m_AEBDecCtrlIsReq;
}
Boolean CCpjVehInfo::m_AEBDecCtrlIsReq() const
{
	return m_m_AEBDecCtrlIsReq;
}
Boolean& CCpjVehInfo::m_AEBDecCtrlIsReq()
{
	return m_m_AEBDecCtrlIsReq;
}

void CCpjVehInfo::m_EBDDecCtrlIsReq(Boolean const _m_EBDDecCtrlIsReq)
{
	m_m_EBDDecCtrlIsReq = _m_EBDDecCtrlIsReq;
}
Boolean CCpjVehInfo::m_EBDDecCtrlIsReq() const
{
	return m_m_EBDDecCtrlIsReq;
}
Boolean& CCpjVehInfo::m_EBDDecCtrlIsReq()
{
	return m_m_EBDDecCtrlIsReq;
}

void CCpjVehInfo::m_AssociatedSysFctActive_b(Boolean const _m_AssociatedSysFctActive_b)
{
	m_m_AssociatedSysFctActive_b = _m_AssociatedSysFctActive_b;
}
Boolean CCpjVehInfo::m_AssociatedSysFctActive_b() const
{
	return m_m_AssociatedSysFctActive_b;
}
Boolean& CCpjVehInfo::m_AssociatedSysFctActive_b()
{
	return m_m_AssociatedSysFctActive_b;
}

void CCpjVehInfo::m_ActSafeFctActive_b(Boolean const _m_ActSafeFctActive_b)
{
	m_m_ActSafeFctActive_b = _m_ActSafeFctActive_b;
}
Boolean CCpjVehInfo::m_ActSafeFctActive_b() const
{
	return m_m_ActSafeFctActive_b;
}
Boolean& CCpjVehInfo::m_ActSafeFctActive_b()
{
	return m_m_ActSafeFctActive_b;
}

void CCpjVehInfo::m_RCTBDecCtrlReq(Boolean const _m_RCTBDecCtrlReq)
{
	m_m_RCTBDecCtrlReq = _m_RCTBDecCtrlReq;
}
Boolean CCpjVehInfo::m_RCTBDecCtrlReq() const
{
	return m_m_RCTBDecCtrlReq;
}
Boolean& CCpjVehInfo::m_RCTBDecCtrlReq()
{
	return m_m_RCTBDecCtrlReq;
}

void CCpjVehInfo::m_CddHoldActive(UInt8 const _m_CddHoldActive)
{
	m_m_CddHoldActive = _m_CddHoldActive;
}
UInt8 CCpjVehInfo::m_CddHoldActive() const
{
	return m_m_CddHoldActive;
}
UInt8& CCpjVehInfo::m_CddHoldActive()
{
	return m_m_CddHoldActive;
}

void CCpjVehInfo::m_Stat_RCTB(UInt32 const _m_Stat_RCTB)
{
	m_m_Stat_RCTB = _m_Stat_RCTB;
}
UInt32 CCpjVehInfo::m_Stat_RCTB() const
{
	return m_m_Stat_RCTB;
}
UInt32& CCpjVehInfo::m_Stat_RCTB()
{
	return m_m_Stat_RCTB;
}

void CCpjVehInfo::m_RCTBTgtDecel(Float const _m_RCTBTgtDecel)
{
	m_m_RCTBTgtDecel = _m_RCTBTgtDecel;
}
Float CCpjVehInfo::m_RCTBTgtDecel() const
{
	return m_m_RCTBTgtDecel;
}
Float& CCpjVehInfo::m_RCTBTgtDecel()
{
	return m_m_RCTBTgtDecel;
}

void CCpjVehInfo::m_ParkingFaultCluster(UInt32 const _m_ParkingFaultCluster)
{
	m_m_ParkingFaultCluster = _m_ParkingFaultCluster;
}
UInt32 CCpjVehInfo::m_ParkingFaultCluster() const
{
	return m_m_ParkingFaultCluster;
}
UInt32& CCpjVehInfo::m_ParkingFaultCluster()
{
	return m_m_ParkingFaultCluster;
}

void CCpjVehInfo::m_NoBrakeForce(UInt8 const _m_NoBrakeForce)
{
	m_m_NoBrakeForce = _m_NoBrakeForce;
}
UInt8 CCpjVehInfo::m_NoBrakeForce() const
{
	return m_m_NoBrakeForce;
}
UInt8& CCpjVehInfo::m_NoBrakeForce()
{
	return m_m_NoBrakeForce;
}

void CCpjVehInfo::m_VehIsPowerLow(Boolean const _m_VehIsPowerLow)
{
	m_m_VehIsPowerLow = _m_VehIsPowerLow;
}
Boolean CCpjVehInfo::m_VehIsPowerLow() const
{
	return m_m_VehIsPowerLow;
}
Boolean& CCpjVehInfo::m_VehIsPowerLow()
{
	return m_m_VehIsPowerLow;
}

void CCpjVehInfo::m_ChargeConnected(Boolean const _m_ChargeConnected)
{
	m_m_ChargeConnected = _m_ChargeConnected;
}
Boolean CCpjVehInfo::m_ChargeConnected() const
{
	return m_m_ChargeConnected;
}
Boolean& CCpjVehInfo::m_ChargeConnected()
{
	return m_m_ChargeConnected;
}

void CCpjVehInfo::m_CameraIsBlocked(Boolean const _m_CameraIsBlocked)
{
	m_m_CameraIsBlocked = _m_CameraIsBlocked;
}
Boolean CCpjVehInfo::m_CameraIsBlocked() const
{
	return m_m_CameraIsBlocked;
}
Boolean& CCpjVehInfo::m_CameraIsBlocked()
{
	return m_m_CameraIsBlocked;
}

void CCpjVehInfo::m_VehPowerValue(Float const _m_VehPowerValue)
{
	m_m_VehPowerValue = _m_VehPowerValue;
}
Float CCpjVehInfo::m_VehPowerValue() const
{
	return m_m_VehPowerValue;
}
Float& CCpjVehInfo::m_VehPowerValue()
{
	return m_m_VehPowerValue;
}

void CCpjVehInfo::m_VehRemDrvRange(UInt16 const _m_VehRemDrvRange)
{
	m_m_VehRemDrvRange = _m_VehRemDrvRange;
}
UInt16 CCpjVehInfo::m_VehRemDrvRange() const
{
	return m_m_VehRemDrvRange;
}
UInt16& CCpjVehInfo::m_VehRemDrvRange()
{
	return m_m_VehRemDrvRange;
}

CCpjTel::CCpjTel()
{

}

magna::dds::DdsCdr& CCpjTel::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_TEL_Phone_ConnectSt);
	cdr.serialize(m_m_TEL_RPAKeyPstSt);
	cdr.serialize(m_m_TEL_RPADeviceSt);
	cdr.serialize(m_m_TEL_RPACheckReq);
	cdr.serialize(m_m_TEL_RPABtnStPressed);
	cdr.serialize(m_m_TEL_PhoneBattSOC);
	cdr.serialize(m_m_TEL_RPACancelBtnStPressed);
	cdr.serialize(m_m_TEL_RPAOutDirSel);
	cdr.serialize(m_m_TEL_RPADeviceFailReason);
	cdr.serialize(m_m_TEL_DrvGestureStOk);
	cdr.serialize(m_m_TEL_DrvGestureCheckOk);
	cdr.serialize(m_m_TEL_RPAUndoBtnStPressed);
	cdr.serialize(m_m_TEL_BTCountSt_u8);
	cdr.serialize(m_m_TEL_IDC_AppParkingStResp);
	cdr.serialize(m_m_TEL_APA_AuthSt);
	cdr.serialize(m_m_TEL_RPA_AuthSt);
	cdr.serialize(m_m_TEL_HPA_AuthSt);
	cdr.serialize(m_m_TEL_RPAFunction_en);
	cdr.serialize(m_m_TEL_RPADirSel_en);
	cdr.serialize(m_m_TEL_RPAServiceResp_st);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_TEL_RPAPowerOffReq);
	cdr.serialize(m_m_TEL_RPAobscancelCfm);

	return cdr;
}
uint32_t CCpjTel::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CCpjTel::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_TEL_Phone_ConnectSt);
	cdr.deserialize(m_m_TEL_RPAKeyPstSt);
	cdr.deserialize(m_m_TEL_RPADeviceSt);
	cdr.deserialize(m_m_TEL_RPACheckReq);
	cdr.deserialize(m_m_TEL_RPABtnStPressed);
	cdr.deserialize(m_m_TEL_PhoneBattSOC);
	cdr.deserialize(m_m_TEL_RPACancelBtnStPressed);
	cdr.deserialize(m_m_TEL_RPAOutDirSel);
	cdr.deserialize(m_m_TEL_RPADeviceFailReason);
	cdr.deserialize(m_m_TEL_DrvGestureStOk);
	cdr.deserialize(m_m_TEL_DrvGestureCheckOk);
	cdr.deserialize(m_m_TEL_RPAUndoBtnStPressed);
	cdr.deserialize(m_m_TEL_BTCountSt_u8);
	cdr.deserialize(m_m_TEL_IDC_AppParkingStResp);
	cdr.deserialize(m_m_TEL_APA_AuthSt);
	cdr.deserialize(m_m_TEL_RPA_AuthSt);
	cdr.deserialize(m_m_TEL_HPA_AuthSt);
	cdr.deserialize(m_m_TEL_RPAFunction_en);
	cdr.deserialize(m_m_TEL_RPADirSel_en);
	cdr.deserialize(m_m_TEL_RPAServiceResp_st);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_TEL_RPAPowerOffReq);
	cdr.deserialize(m_m_TEL_RPAobscancelCfm);

	return cdr;
}
bool CCpjTel::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CCpjTel::is_key_defined()
{
	return false;

}
void CCpjTel::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CCpjTel::serialize_key(char **buf,unsigned int *len)
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
bool CCpjTel::is_key_serialize_by_cdr()
{
	return false;

}
bool CCpjTel::is_plain_types()
{
	bool b0 = EIdcServiceResp_st::is_plain_types();

	return b0;
}
uint32_t CCpjTel::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CCpjTel::set_key_val(CCpjTel const* const _data) noexcept
{

}
void CCpjTel::m_TEL_Phone_ConnectSt(UInt32 const _m_TEL_Phone_ConnectSt)
{
	m_m_TEL_Phone_ConnectSt = _m_TEL_Phone_ConnectSt;
}
UInt32 CCpjTel::m_TEL_Phone_ConnectSt() const
{
	return m_m_TEL_Phone_ConnectSt;
}
UInt32& CCpjTel::m_TEL_Phone_ConnectSt()
{
	return m_m_TEL_Phone_ConnectSt;
}

void CCpjTel::m_TEL_RPAKeyPstSt(UInt32 const _m_TEL_RPAKeyPstSt)
{
	m_m_TEL_RPAKeyPstSt = _m_TEL_RPAKeyPstSt;
}
UInt32 CCpjTel::m_TEL_RPAKeyPstSt() const
{
	return m_m_TEL_RPAKeyPstSt;
}
UInt32& CCpjTel::m_TEL_RPAKeyPstSt()
{
	return m_m_TEL_RPAKeyPstSt;
}

void CCpjTel::m_TEL_RPADeviceSt(UInt32 const _m_TEL_RPADeviceSt)
{
	m_m_TEL_RPADeviceSt = _m_TEL_RPADeviceSt;
}
UInt32 CCpjTel::m_TEL_RPADeviceSt() const
{
	return m_m_TEL_RPADeviceSt;
}
UInt32& CCpjTel::m_TEL_RPADeviceSt()
{
	return m_m_TEL_RPADeviceSt;
}

void CCpjTel::m_TEL_RPACheckReq(Boolean const _m_TEL_RPACheckReq)
{
	m_m_TEL_RPACheckReq = _m_TEL_RPACheckReq;
}
Boolean CCpjTel::m_TEL_RPACheckReq() const
{
	return m_m_TEL_RPACheckReq;
}
Boolean& CCpjTel::m_TEL_RPACheckReq()
{
	return m_m_TEL_RPACheckReq;
}

void CCpjTel::m_TEL_RPABtnStPressed(Boolean const _m_TEL_RPABtnStPressed)
{
	m_m_TEL_RPABtnStPressed = _m_TEL_RPABtnStPressed;
}
Boolean CCpjTel::m_TEL_RPABtnStPressed() const
{
	return m_m_TEL_RPABtnStPressed;
}
Boolean& CCpjTel::m_TEL_RPABtnStPressed()
{
	return m_m_TEL_RPABtnStPressed;
}

void CCpjTel::m_TEL_PhoneBattSOC(SInt8 const _m_TEL_PhoneBattSOC)
{
	m_m_TEL_PhoneBattSOC = _m_TEL_PhoneBattSOC;
}
SInt8 CCpjTel::m_TEL_PhoneBattSOC() const
{
	return m_m_TEL_PhoneBattSOC;
}
SInt8& CCpjTel::m_TEL_PhoneBattSOC()
{
	return m_m_TEL_PhoneBattSOC;
}

void CCpjTel::m_TEL_RPACancelBtnStPressed(Boolean const _m_TEL_RPACancelBtnStPressed)
{
	m_m_TEL_RPACancelBtnStPressed = _m_TEL_RPACancelBtnStPressed;
}
Boolean CCpjTel::m_TEL_RPACancelBtnStPressed() const
{
	return m_m_TEL_RPACancelBtnStPressed;
}
Boolean& CCpjTel::m_TEL_RPACancelBtnStPressed()
{
	return m_m_TEL_RPACancelBtnStPressed;
}

void CCpjTel::m_TEL_RPAOutDirSel(UInt32 const _m_TEL_RPAOutDirSel)
{
	m_m_TEL_RPAOutDirSel = _m_TEL_RPAOutDirSel;
}
UInt32 CCpjTel::m_TEL_RPAOutDirSel() const
{
	return m_m_TEL_RPAOutDirSel;
}
UInt32& CCpjTel::m_TEL_RPAOutDirSel()
{
	return m_m_TEL_RPAOutDirSel;
}

void CCpjTel::m_TEL_RPADeviceFailReason(UInt32 const _m_TEL_RPADeviceFailReason)
{
	m_m_TEL_RPADeviceFailReason = _m_TEL_RPADeviceFailReason;
}
UInt32 CCpjTel::m_TEL_RPADeviceFailReason() const
{
	return m_m_TEL_RPADeviceFailReason;
}
UInt32& CCpjTel::m_TEL_RPADeviceFailReason()
{
	return m_m_TEL_RPADeviceFailReason;
}

void CCpjTel::m_TEL_DrvGestureStOk(UInt8 const _m_TEL_DrvGestureStOk)
{
	m_m_TEL_DrvGestureStOk = _m_TEL_DrvGestureStOk;
}
UInt8 CCpjTel::m_TEL_DrvGestureStOk() const
{
	return m_m_TEL_DrvGestureStOk;
}
UInt8& CCpjTel::m_TEL_DrvGestureStOk()
{
	return m_m_TEL_DrvGestureStOk;
}

void CCpjTel::m_TEL_DrvGestureCheckOk(Boolean const _m_TEL_DrvGestureCheckOk)
{
	m_m_TEL_DrvGestureCheckOk = _m_TEL_DrvGestureCheckOk;
}
Boolean CCpjTel::m_TEL_DrvGestureCheckOk() const
{
	return m_m_TEL_DrvGestureCheckOk;
}
Boolean& CCpjTel::m_TEL_DrvGestureCheckOk()
{
	return m_m_TEL_DrvGestureCheckOk;
}

void CCpjTel::m_TEL_RPAUndoBtnStPressed(Boolean const _m_TEL_RPAUndoBtnStPressed)
{
	m_m_TEL_RPAUndoBtnStPressed = _m_TEL_RPAUndoBtnStPressed;
}
Boolean CCpjTel::m_TEL_RPAUndoBtnStPressed() const
{
	return m_m_TEL_RPAUndoBtnStPressed;
}
Boolean& CCpjTel::m_TEL_RPAUndoBtnStPressed()
{
	return m_m_TEL_RPAUndoBtnStPressed;
}

void CCpjTel::m_TEL_BTCountSt_u8(UInt8 const _m_TEL_BTCountSt_u8)
{
	m_m_TEL_BTCountSt_u8 = _m_TEL_BTCountSt_u8;
}
UInt8 CCpjTel::m_TEL_BTCountSt_u8() const
{
	return m_m_TEL_BTCountSt_u8;
}
UInt8& CCpjTel::m_TEL_BTCountSt_u8()
{
	return m_m_TEL_BTCountSt_u8;
}

void CCpjTel::m_TEL_IDC_AppParkingStResp(UInt32 const _m_TEL_IDC_AppParkingStResp)
{
	m_m_TEL_IDC_AppParkingStResp = _m_TEL_IDC_AppParkingStResp;
}
UInt32 CCpjTel::m_TEL_IDC_AppParkingStResp() const
{
	return m_m_TEL_IDC_AppParkingStResp;
}
UInt32& CCpjTel::m_TEL_IDC_AppParkingStResp()
{
	return m_m_TEL_IDC_AppParkingStResp;
}

void CCpjTel::m_TEL_APA_AuthSt(UInt32 const _m_TEL_APA_AuthSt)
{
	m_m_TEL_APA_AuthSt = _m_TEL_APA_AuthSt;
}
UInt32 CCpjTel::m_TEL_APA_AuthSt() const
{
	return m_m_TEL_APA_AuthSt;
}
UInt32& CCpjTel::m_TEL_APA_AuthSt()
{
	return m_m_TEL_APA_AuthSt;
}

void CCpjTel::m_TEL_RPA_AuthSt(UInt32 const _m_TEL_RPA_AuthSt)
{
	m_m_TEL_RPA_AuthSt = _m_TEL_RPA_AuthSt;
}
UInt32 CCpjTel::m_TEL_RPA_AuthSt() const
{
	return m_m_TEL_RPA_AuthSt;
}
UInt32& CCpjTel::m_TEL_RPA_AuthSt()
{
	return m_m_TEL_RPA_AuthSt;
}

void CCpjTel::m_TEL_HPA_AuthSt(UInt32 const _m_TEL_HPA_AuthSt)
{
	m_m_TEL_HPA_AuthSt = _m_TEL_HPA_AuthSt;
}
UInt32 CCpjTel::m_TEL_HPA_AuthSt() const
{
	return m_m_TEL_HPA_AuthSt;
}
UInt32& CCpjTel::m_TEL_HPA_AuthSt()
{
	return m_m_TEL_HPA_AuthSt;
}

void CCpjTel::m_TEL_RPAFunction_en(UInt32 const _m_TEL_RPAFunction_en)
{
	m_m_TEL_RPAFunction_en = _m_TEL_RPAFunction_en;
}
UInt32 CCpjTel::m_TEL_RPAFunction_en() const
{
	return m_m_TEL_RPAFunction_en;
}
UInt32& CCpjTel::m_TEL_RPAFunction_en()
{
	return m_m_TEL_RPAFunction_en;
}

void CCpjTel::m_TEL_RPADirSel_en(UInt32 const _m_TEL_RPADirSel_en)
{
	m_m_TEL_RPADirSel_en = _m_TEL_RPADirSel_en;
}
UInt32 CCpjTel::m_TEL_RPADirSel_en() const
{
	return m_m_TEL_RPADirSel_en;
}
UInt32& CCpjTel::m_TEL_RPADirSel_en()
{
	return m_m_TEL_RPADirSel_en;
}

void CCpjTel::m_TEL_RPAServiceResp_st(EIdcServiceResp_st const &_m_TEL_RPAServiceResp_st)
{
	m_m_TEL_RPAServiceResp_st = _m_TEL_RPAServiceResp_st;
}
void CCpjTel::m_TEL_RPAServiceResp_st(EIdcServiceResp_st &&_m_TEL_RPAServiceResp_st)
{
	m_m_TEL_RPAServiceResp_st = std::move(_m_TEL_RPAServiceResp_st);
}
EIdcServiceResp_st const& CCpjTel::m_TEL_RPAServiceResp_st() const
{
	return m_m_TEL_RPAServiceResp_st;
}
EIdcServiceResp_st& CCpjTel::m_TEL_RPAServiceResp_st()
{
	return m_m_TEL_RPAServiceResp_st;
}

void CCpjTel::padding(UInt16 const _padding)
{
	m_padding = _padding;
}
UInt16 CCpjTel::padding() const
{
	return m_padding;
}
UInt16& CCpjTel::padding()
{
	return m_padding;
}

void CCpjTel::m_TEL_RPAPowerOffReq(Boolean const _m_TEL_RPAPowerOffReq)
{
	m_m_TEL_RPAPowerOffReq = _m_TEL_RPAPowerOffReq;
}
Boolean CCpjTel::m_TEL_RPAPowerOffReq() const
{
	return m_m_TEL_RPAPowerOffReq;
}
Boolean& CCpjTel::m_TEL_RPAPowerOffReq()
{
	return m_m_TEL_RPAPowerOffReq;
}

void CCpjTel::m_TEL_RPAobscancelCfm(Boolean const _m_TEL_RPAobscancelCfm)
{
	m_m_TEL_RPAobscancelCfm = _m_TEL_RPAobscancelCfm;
}
Boolean CCpjTel::m_TEL_RPAobscancelCfm() const
{
	return m_m_TEL_RPAobscancelCfm;
}
Boolean& CCpjTel::m_TEL_RPAobscancelCfm()
{
	return m_m_TEL_RPAobscancelCfm;
}

CPfBrakes::CPfBrakes()
{

}

magna::dds::DdsCdr& CPfBrakes::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_espOpMode);
	cdr.serialize(m_m_absActive);
	cdr.serialize(m_m_aebActive);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_prkgBrkState);
	cdr.serialize(m_m_brkPedlState);
	cdr.serialize(m_m_masterCylinderPress);

	return cdr;
}
uint32_t CPfBrakes::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfBrakes::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_espOpMode);
	cdr.deserialize(m_m_absActive);
	cdr.deserialize(m_m_aebActive);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_prkgBrkState);
	cdr.deserialize(m_m_brkPedlState);
	cdr.deserialize(m_m_masterCylinderPress);

	return cdr;
}
bool CPfBrakes::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfBrakes::is_key_defined()
{
	return false;

}
void CPfBrakes::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfBrakes::serialize_key(char **buf,unsigned int *len)
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
bool CPfBrakes::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfBrakes::is_plain_types()
{
	return true;
}
uint32_t CPfBrakes::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfBrakes::set_key_val(CPfBrakes const* const _data) noexcept
{

}
void CPfBrakes::m_espOpMode(UInt32 const _m_espOpMode)
{
	m_m_espOpMode = _m_espOpMode;
}
UInt32 CPfBrakes::m_espOpMode() const
{
	return m_m_espOpMode;
}
UInt32& CPfBrakes::m_espOpMode()
{
	return m_m_espOpMode;
}

void CPfBrakes::m_absActive(Boolean const _m_absActive)
{
	m_m_absActive = _m_absActive;
}
Boolean CPfBrakes::m_absActive() const
{
	return m_m_absActive;
}
Boolean& CPfBrakes::m_absActive()
{
	return m_m_absActive;
}

void CPfBrakes::m_aebActive(Boolean const _m_aebActive)
{
	m_m_aebActive = _m_aebActive;
}
Boolean CPfBrakes::m_aebActive() const
{
	return m_m_aebActive;
}
Boolean& CPfBrakes::m_aebActive()
{
	return m_m_aebActive;
}

void CPfBrakes::padding(UInt16 const _padding)
{
	m_padding = _padding;
}
UInt16 CPfBrakes::padding() const
{
	return m_padding;
}
UInt16& CPfBrakes::padding()
{
	return m_padding;
}

void CPfBrakes::m_prkgBrkState(UInt32 const _m_prkgBrkState)
{
	m_m_prkgBrkState = _m_prkgBrkState;
}
UInt32 CPfBrakes::m_prkgBrkState() const
{
	return m_m_prkgBrkState;
}
UInt32& CPfBrakes::m_prkgBrkState()
{
	return m_m_prkgBrkState;
}

void CPfBrakes::m_brkPedlState(UInt32 const _m_brkPedlState)
{
	m_m_brkPedlState = _m_brkPedlState;
}
UInt32 CPfBrakes::m_brkPedlState() const
{
	return m_m_brkPedlState;
}
UInt32& CPfBrakes::m_brkPedlState()
{
	return m_m_brkPedlState;
}

void CPfBrakes::m_masterCylinderPress(Float const _m_masterCylinderPress)
{
	m_m_masterCylinderPress = _m_masterCylinderPress;
}
Float CPfBrakes::m_masterCylinderPress() const
{
	return m_m_masterCylinderPress;
}
Float& CPfBrakes::m_masterCylinderPress()
{
	return m_m_masterCylinderPress;
}

CPfCamera::CPfCamera()
{

}

magna::dds::DdsCdr& CPfCamera::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_statusProtectionFlapFront);
	cdr.serialize(m_m_statusProtectionFlapRear);
	cdr.serialize(m_m_statusRearCamPosition);
	cdr.serialize(m_m_statusFrontCamPosition);

	return cdr;
}
uint32_t CPfCamera::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfCamera::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_statusProtectionFlapFront);
	cdr.deserialize(m_m_statusProtectionFlapRear);
	cdr.deserialize(m_m_statusRearCamPosition);
	cdr.deserialize(m_m_statusFrontCamPosition);

	return cdr;
}
bool CPfCamera::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfCamera::is_key_defined()
{
	return false;

}
void CPfCamera::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfCamera::serialize_key(char **buf,unsigned int *len)
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
bool CPfCamera::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfCamera::is_plain_types()
{
	return true;
}
uint32_t CPfCamera::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfCamera::set_key_val(CPfCamera const* const _data) noexcept
{

}
void CPfCamera::m_statusProtectionFlapFront(UInt32 const _m_statusProtectionFlapFront)
{
	m_m_statusProtectionFlapFront = _m_statusProtectionFlapFront;
}
UInt32 CPfCamera::m_statusProtectionFlapFront() const
{
	return m_m_statusProtectionFlapFront;
}
UInt32& CPfCamera::m_statusProtectionFlapFront()
{
	return m_m_statusProtectionFlapFront;
}

void CPfCamera::m_statusProtectionFlapRear(UInt32 const _m_statusProtectionFlapRear)
{
	m_m_statusProtectionFlapRear = _m_statusProtectionFlapRear;
}
UInt32 CPfCamera::m_statusProtectionFlapRear() const
{
	return m_m_statusProtectionFlapRear;
}
UInt32& CPfCamera::m_statusProtectionFlapRear()
{
	return m_m_statusProtectionFlapRear;
}

void CPfCamera::m_statusRearCamPosition(UInt32 const _m_statusRearCamPosition)
{
	m_m_statusRearCamPosition = _m_statusRearCamPosition;
}
UInt32 CPfCamera::m_statusRearCamPosition() const
{
	return m_m_statusRearCamPosition;
}
UInt32& CPfCamera::m_statusRearCamPosition()
{
	return m_m_statusRearCamPosition;
}

void CPfCamera::m_statusFrontCamPosition(UInt32 const _m_statusFrontCamPosition)
{
	m_m_statusFrontCamPosition = _m_statusFrontCamPosition;
}
UInt32 CPfCamera::m_statusFrontCamPosition() const
{
	return m_m_statusFrontCamPosition;
}
UInt32& CPfCamera::m_statusFrontCamPosition()
{
	return m_m_statusFrontCamPosition;
}

CPfDoorAndMirror::CPfDoorAndMirror()
{

}

magna::dds::DdsCdr& CPfDoorAndMirror::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_stateDoorSwitchFrontLeft);
	cdr.serialize(m_m_stateDoorSwitchFrontRight);
	cdr.serialize(m_m_stateDoorSwitchRearLeft);
	cdr.serialize(m_m_stateDoorSwitchRearRight);
	cdr.serialize(m_m_stateExteriorMirrorLeft);
	cdr.serialize(m_m_stateExteriorMirrorRight);
	cdr.serialize(m_m_mirrFldState);
	cdr.serialize(m_m_statusContactBootLid);
	cdr.serialize(m_m_statusContactFrontLid);
	cdr.serialize(m_m_SrfOperateSts);

	return cdr;
}
uint32_t CPfDoorAndMirror::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfDoorAndMirror::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_stateDoorSwitchFrontLeft);
	cdr.deserialize(m_m_stateDoorSwitchFrontRight);
	cdr.deserialize(m_m_stateDoorSwitchRearLeft);
	cdr.deserialize(m_m_stateDoorSwitchRearRight);
	cdr.deserialize(m_m_stateExteriorMirrorLeft);
	cdr.deserialize(m_m_stateExteriorMirrorRight);
	cdr.deserialize(m_m_mirrFldState);
	cdr.deserialize(m_m_statusContactBootLid);
	cdr.deserialize(m_m_statusContactFrontLid);
	cdr.deserialize(m_m_SrfOperateSts);

	return cdr;
}
bool CPfDoorAndMirror::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfDoorAndMirror::is_key_defined()
{
	return false;

}
void CPfDoorAndMirror::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfDoorAndMirror::serialize_key(char **buf,unsigned int *len)
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
bool CPfDoorAndMirror::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfDoorAndMirror::is_plain_types()
{
	return true;
}
uint32_t CPfDoorAndMirror::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfDoorAndMirror::set_key_val(CPfDoorAndMirror const* const _data) noexcept
{

}
void CPfDoorAndMirror::m_stateDoorSwitchFrontLeft(UInt32 const _m_stateDoorSwitchFrontLeft)
{
	m_m_stateDoorSwitchFrontLeft = _m_stateDoorSwitchFrontLeft;
}
UInt32 CPfDoorAndMirror::m_stateDoorSwitchFrontLeft() const
{
	return m_m_stateDoorSwitchFrontLeft;
}
UInt32& CPfDoorAndMirror::m_stateDoorSwitchFrontLeft()
{
	return m_m_stateDoorSwitchFrontLeft;
}

void CPfDoorAndMirror::m_stateDoorSwitchFrontRight(UInt32 const _m_stateDoorSwitchFrontRight)
{
	m_m_stateDoorSwitchFrontRight = _m_stateDoorSwitchFrontRight;
}
UInt32 CPfDoorAndMirror::m_stateDoorSwitchFrontRight() const
{
	return m_m_stateDoorSwitchFrontRight;
}
UInt32& CPfDoorAndMirror::m_stateDoorSwitchFrontRight()
{
	return m_m_stateDoorSwitchFrontRight;
}

void CPfDoorAndMirror::m_stateDoorSwitchRearLeft(UInt32 const _m_stateDoorSwitchRearLeft)
{
	m_m_stateDoorSwitchRearLeft = _m_stateDoorSwitchRearLeft;
}
UInt32 CPfDoorAndMirror::m_stateDoorSwitchRearLeft() const
{
	return m_m_stateDoorSwitchRearLeft;
}
UInt32& CPfDoorAndMirror::m_stateDoorSwitchRearLeft()
{
	return m_m_stateDoorSwitchRearLeft;
}

void CPfDoorAndMirror::m_stateDoorSwitchRearRight(UInt32 const _m_stateDoorSwitchRearRight)
{
	m_m_stateDoorSwitchRearRight = _m_stateDoorSwitchRearRight;
}
UInt32 CPfDoorAndMirror::m_stateDoorSwitchRearRight() const
{
	return m_m_stateDoorSwitchRearRight;
}
UInt32& CPfDoorAndMirror::m_stateDoorSwitchRearRight()
{
	return m_m_stateDoorSwitchRearRight;
}

void CPfDoorAndMirror::m_stateExteriorMirrorLeft(UInt32 const _m_stateExteriorMirrorLeft)
{
	m_m_stateExteriorMirrorLeft = _m_stateExteriorMirrorLeft;
}
UInt32 CPfDoorAndMirror::m_stateExteriorMirrorLeft() const
{
	return m_m_stateExteriorMirrorLeft;
}
UInt32& CPfDoorAndMirror::m_stateExteriorMirrorLeft()
{
	return m_m_stateExteriorMirrorLeft;
}

void CPfDoorAndMirror::m_stateExteriorMirrorRight(UInt32 const _m_stateExteriorMirrorRight)
{
	m_m_stateExteriorMirrorRight = _m_stateExteriorMirrorRight;
}
UInt32 CPfDoorAndMirror::m_stateExteriorMirrorRight() const
{
	return m_m_stateExteriorMirrorRight;
}
UInt32& CPfDoorAndMirror::m_stateExteriorMirrorRight()
{
	return m_m_stateExteriorMirrorRight;
}

void CPfDoorAndMirror::m_mirrFldState(UInt32 const _m_mirrFldState)
{
	m_m_mirrFldState = _m_mirrFldState;
}
UInt32 CPfDoorAndMirror::m_mirrFldState() const
{
	return m_m_mirrFldState;
}
UInt32& CPfDoorAndMirror::m_mirrFldState()
{
	return m_m_mirrFldState;
}

void CPfDoorAndMirror::m_statusContactBootLid(UInt32 const _m_statusContactBootLid)
{
	m_m_statusContactBootLid = _m_statusContactBootLid;
}
UInt32 CPfDoorAndMirror::m_statusContactBootLid() const
{
	return m_m_statusContactBootLid;
}
UInt32& CPfDoorAndMirror::m_statusContactBootLid()
{
	return m_m_statusContactBootLid;
}

void CPfDoorAndMirror::m_statusContactFrontLid(UInt32 const _m_statusContactFrontLid)
{
	m_m_statusContactFrontLid = _m_statusContactFrontLid;
}
UInt32 CPfDoorAndMirror::m_statusContactFrontLid() const
{
	return m_m_statusContactFrontLid;
}
UInt32& CPfDoorAndMirror::m_statusContactFrontLid()
{
	return m_m_statusContactFrontLid;
}

void CPfDoorAndMirror::m_SrfOperateSts(UInt32 const _m_SrfOperateSts)
{
	m_m_SrfOperateSts = _m_SrfOperateSts;
}
UInt32 CPfDoorAndMirror::m_SrfOperateSts() const
{
	return m_m_SrfOperateSts;
}
UInt32& CPfDoorAndMirror::m_SrfOperateSts()
{
	return m_m_SrfOperateSts;
}

CPfEnvironment::CPfEnvironment()
{

}

magna::dds::DdsCdr& CPfEnvironment::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_dynamometerMode);
	cdr.serialize(m_m_absoluteTimeUnix);
	cdr.serialize(m_m_steeringWheelAngleSequenceCounter);

	return cdr;
}
uint32_t CPfEnvironment::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfEnvironment::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_dynamometerMode);
	cdr.deserialize(m_m_absoluteTimeUnix);
	cdr.deserialize(m_m_steeringWheelAngleSequenceCounter);

	return cdr;
}
bool CPfEnvironment::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfEnvironment::is_key_defined()
{
	return false;

}
void CPfEnvironment::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfEnvironment::serialize_key(char **buf,unsigned int *len)
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
bool CPfEnvironment::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfEnvironment::is_plain_types()
{
	return true;
}
uint32_t CPfEnvironment::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfEnvironment::set_key_val(CPfEnvironment const* const _data) noexcept
{

}
void CPfEnvironment::m_dynamometerMode(UInt32 const _m_dynamometerMode)
{
	m_m_dynamometerMode = _m_dynamometerMode;
}
UInt32 CPfEnvironment::m_dynamometerMode() const
{
	return m_m_dynamometerMode;
}
UInt32& CPfEnvironment::m_dynamometerMode()
{
	return m_m_dynamometerMode;
}

void CPfEnvironment::m_absoluteTimeUnix(UInt32 const _m_absoluteTimeUnix)
{
	m_m_absoluteTimeUnix = _m_absoluteTimeUnix;
}
UInt32 CPfEnvironment::m_absoluteTimeUnix() const
{
	return m_m_absoluteTimeUnix;
}
UInt32& CPfEnvironment::m_absoluteTimeUnix()
{
	return m_m_absoluteTimeUnix;
}

void CPfEnvironment::m_steeringWheelAngleSequenceCounter(UInt64 const _m_steeringWheelAngleSequenceCounter)
{
	m_m_steeringWheelAngleSequenceCounter = _m_steeringWheelAngleSequenceCounter;
}
UInt64 CPfEnvironment::m_steeringWheelAngleSequenceCounter() const
{
	return m_m_steeringWheelAngleSequenceCounter;
}
UInt64& CPfEnvironment::m_steeringWheelAngleSequenceCounter()
{
	return m_m_steeringWheelAngleSequenceCounter;
}

CPfGear::CPfGear()
{

}

magna::dds::DdsCdr& CPfGear::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_gearBoxType);
	cdr.serialize(m_m_gearStatus);
	cdr.serialize(m_m_TargetGearStatus);
	cdr.serialize(m_m_APAGearIntervention);
	cdr.serialize(m_padding);
	cdr.serialize(m_padding1);

	return cdr;
}
uint32_t CPfGear::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfGear::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_gearBoxType);
	cdr.deserialize(m_m_gearStatus);
	cdr.deserialize(m_m_TargetGearStatus);
	cdr.deserialize(m_m_APAGearIntervention);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_padding1);

	return cdr;
}
bool CPfGear::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfGear::is_key_defined()
{
	return false;

}
void CPfGear::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfGear::serialize_key(char **buf,unsigned int *len)
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
bool CPfGear::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfGear::is_plain_types()
{
	return true;
}
uint32_t CPfGear::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfGear::set_key_val(CPfGear const* const _data) noexcept
{

}
void CPfGear::m_gearBoxType(UInt32 const _m_gearBoxType)
{
	m_m_gearBoxType = _m_gearBoxType;
}
UInt32 CPfGear::m_gearBoxType() const
{
	return m_m_gearBoxType;
}
UInt32& CPfGear::m_gearBoxType()
{
	return m_m_gearBoxType;
}

void CPfGear::m_gearStatus(UInt32 const _m_gearStatus)
{
	m_m_gearStatus = _m_gearStatus;
}
UInt32 CPfGear::m_gearStatus() const
{
	return m_m_gearStatus;
}
UInt32& CPfGear::m_gearStatus()
{
	return m_m_gearStatus;
}

void CPfGear::m_TargetGearStatus(UInt32 const _m_TargetGearStatus)
{
	m_m_TargetGearStatus = _m_TargetGearStatus;
}
UInt32 CPfGear::m_TargetGearStatus() const
{
	return m_m_TargetGearStatus;
}
UInt32& CPfGear::m_TargetGearStatus()
{
	return m_m_TargetGearStatus;
}

void CPfGear::m_APAGearIntervention(Boolean const _m_APAGearIntervention)
{
	m_m_APAGearIntervention = _m_APAGearIntervention;
}
Boolean CPfGear::m_APAGearIntervention() const
{
	return m_m_APAGearIntervention;
}
Boolean& CPfGear::m_APAGearIntervention()
{
	return m_m_APAGearIntervention;
}

void CPfGear::padding(UInt8 const _padding)
{
	m_padding = _padding;
}
UInt8 CPfGear::padding() const
{
	return m_padding;
}
UInt8& CPfGear::padding()
{
	return m_padding;
}

void CPfGear::padding1(UInt16 const _padding1)
{
	m_padding1 = _padding1;
}
UInt16 CPfGear::padding1() const
{
	return m_padding1;
}
UInt16& CPfGear::padding1()
{
	return m_padding1;
}

CPfGnssDateTime::CPfGnssDateTime()
{

}

magna::dds::DdsCdr& CPfGnssDateTime::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_gnssTmStmp);
	cdr.serialize(m_m_gnssDateTimeMilliSecond);
	cdr.serialize(m_m_gnssDateTimeYear);
	cdr.serialize(m_m_gnssDateTimeSecond);
	cdr.serialize(m_m_gnssDateTimeMinute);
	cdr.serialize(m_m_gnssDateTimeMonth);
	cdr.serialize(m_m_gnssDateTimeDay);
	cdr.serialize(m_m_gnssDateTimeHour);
	cdr.serialize(m_m_gnssMasterCount1);
	cdr.serialize(m_m_gnssMasterCount2);
	cdr.serialize(m_m_gnssMasterCount3);
	cdr.serialize(m_m_gnssMasterCount4);
	cdr.serialize(m_padding1);
	cdr.serialize(m_padding);

	return cdr;
}
uint32_t CPfGnssDateTime::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfGnssDateTime::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_gnssTmStmp);
	cdr.deserialize(m_m_gnssDateTimeMilliSecond);
	cdr.deserialize(m_m_gnssDateTimeYear);
	cdr.deserialize(m_m_gnssDateTimeSecond);
	cdr.deserialize(m_m_gnssDateTimeMinute);
	cdr.deserialize(m_m_gnssDateTimeMonth);
	cdr.deserialize(m_m_gnssDateTimeDay);
	cdr.deserialize(m_m_gnssDateTimeHour);
	cdr.deserialize(m_m_gnssMasterCount1);
	cdr.deserialize(m_m_gnssMasterCount2);
	cdr.deserialize(m_m_gnssMasterCount3);
	cdr.deserialize(m_m_gnssMasterCount4);
	cdr.deserialize(m_padding1);
	cdr.deserialize(m_padding);

	return cdr;
}
bool CPfGnssDateTime::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfGnssDateTime::is_key_defined()
{
	return false;

}
void CPfGnssDateTime::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfGnssDateTime::serialize_key(char **buf,unsigned int *len)
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
bool CPfGnssDateTime::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfGnssDateTime::is_plain_types()
{
	return true;
}
uint32_t CPfGnssDateTime::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfGnssDateTime::set_key_val(CPfGnssDateTime const* const _data) noexcept
{

}
void CPfGnssDateTime::m_gnssTmStmp(Float const _m_gnssTmStmp)
{
	m_m_gnssTmStmp = _m_gnssTmStmp;
}
Float CPfGnssDateTime::m_gnssTmStmp() const
{
	return m_m_gnssTmStmp;
}
Float& CPfGnssDateTime::m_gnssTmStmp()
{
	return m_m_gnssTmStmp;
}

void CPfGnssDateTime::m_gnssDateTimeMilliSecond(UInt16 const _m_gnssDateTimeMilliSecond)
{
	m_m_gnssDateTimeMilliSecond = _m_gnssDateTimeMilliSecond;
}
UInt16 CPfGnssDateTime::m_gnssDateTimeMilliSecond() const
{
	return m_m_gnssDateTimeMilliSecond;
}
UInt16& CPfGnssDateTime::m_gnssDateTimeMilliSecond()
{
	return m_m_gnssDateTimeMilliSecond;
}

void CPfGnssDateTime::m_gnssDateTimeYear(UInt16 const _m_gnssDateTimeYear)
{
	m_m_gnssDateTimeYear = _m_gnssDateTimeYear;
}
UInt16 CPfGnssDateTime::m_gnssDateTimeYear() const
{
	return m_m_gnssDateTimeYear;
}
UInt16& CPfGnssDateTime::m_gnssDateTimeYear()
{
	return m_m_gnssDateTimeYear;
}

void CPfGnssDateTime::m_gnssDateTimeSecond(UInt8 const _m_gnssDateTimeSecond)
{
	m_m_gnssDateTimeSecond = _m_gnssDateTimeSecond;
}
UInt8 CPfGnssDateTime::m_gnssDateTimeSecond() const
{
	return m_m_gnssDateTimeSecond;
}
UInt8& CPfGnssDateTime::m_gnssDateTimeSecond()
{
	return m_m_gnssDateTimeSecond;
}

void CPfGnssDateTime::m_gnssDateTimeMinute(UInt8 const _m_gnssDateTimeMinute)
{
	m_m_gnssDateTimeMinute = _m_gnssDateTimeMinute;
}
UInt8 CPfGnssDateTime::m_gnssDateTimeMinute() const
{
	return m_m_gnssDateTimeMinute;
}
UInt8& CPfGnssDateTime::m_gnssDateTimeMinute()
{
	return m_m_gnssDateTimeMinute;
}

void CPfGnssDateTime::m_gnssDateTimeMonth(UInt8 const _m_gnssDateTimeMonth)
{
	m_m_gnssDateTimeMonth = _m_gnssDateTimeMonth;
}
UInt8 CPfGnssDateTime::m_gnssDateTimeMonth() const
{
	return m_m_gnssDateTimeMonth;
}
UInt8& CPfGnssDateTime::m_gnssDateTimeMonth()
{
	return m_m_gnssDateTimeMonth;
}

void CPfGnssDateTime::m_gnssDateTimeDay(UInt8 const _m_gnssDateTimeDay)
{
	m_m_gnssDateTimeDay = _m_gnssDateTimeDay;
}
UInt8 CPfGnssDateTime::m_gnssDateTimeDay() const
{
	return m_m_gnssDateTimeDay;
}
UInt8& CPfGnssDateTime::m_gnssDateTimeDay()
{
	return m_m_gnssDateTimeDay;
}

void CPfGnssDateTime::m_gnssDateTimeHour(UInt8 const _m_gnssDateTimeHour)
{
	m_m_gnssDateTimeHour = _m_gnssDateTimeHour;
}
UInt8 CPfGnssDateTime::m_gnssDateTimeHour() const
{
	return m_m_gnssDateTimeHour;
}
UInt8& CPfGnssDateTime::m_gnssDateTimeHour()
{
	return m_m_gnssDateTimeHour;
}

void CPfGnssDateTime::m_gnssMasterCount1(UInt8 const _m_gnssMasterCount1)
{
	m_m_gnssMasterCount1 = _m_gnssMasterCount1;
}
UInt8 CPfGnssDateTime::m_gnssMasterCount1() const
{
	return m_m_gnssMasterCount1;
}
UInt8& CPfGnssDateTime::m_gnssMasterCount1()
{
	return m_m_gnssMasterCount1;
}

void CPfGnssDateTime::m_gnssMasterCount2(UInt8 const _m_gnssMasterCount2)
{
	m_m_gnssMasterCount2 = _m_gnssMasterCount2;
}
UInt8 CPfGnssDateTime::m_gnssMasterCount2() const
{
	return m_m_gnssMasterCount2;
}
UInt8& CPfGnssDateTime::m_gnssMasterCount2()
{
	return m_m_gnssMasterCount2;
}

void CPfGnssDateTime::m_gnssMasterCount3(UInt8 const _m_gnssMasterCount3)
{
	m_m_gnssMasterCount3 = _m_gnssMasterCount3;
}
UInt8 CPfGnssDateTime::m_gnssMasterCount3() const
{
	return m_m_gnssMasterCount3;
}
UInt8& CPfGnssDateTime::m_gnssMasterCount3()
{
	return m_m_gnssMasterCount3;
}

void CPfGnssDateTime::m_gnssMasterCount4(UInt8 const _m_gnssMasterCount4)
{
	m_m_gnssMasterCount4 = _m_gnssMasterCount4;
}
UInt8 CPfGnssDateTime::m_gnssMasterCount4() const
{
	return m_m_gnssMasterCount4;
}
UInt8& CPfGnssDateTime::m_gnssMasterCount4()
{
	return m_m_gnssMasterCount4;
}

void CPfGnssDateTime::padding1(UInt8 const _padding1)
{
	m_padding1 = _padding1;
}
UInt8 CPfGnssDateTime::padding1() const
{
	return m_padding1;
}
UInt8& CPfGnssDateTime::padding1()
{
	return m_padding1;
}

void CPfGnssDateTime::padding(UInt16 const _padding)
{
	m_padding = _padding;
}
UInt16 CPfGnssDateTime::padding() const
{
	return m_padding;
}
UInt16& CPfGnssDateTime::padding()
{
	return m_padding;
}

CPfGnssLocalization::CPfGnssLocalization()
{

}

magna::dds::DdsCdr& CPfGnssLocalization::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_gnssAltitude);
	cdr.serialize(m_m_gnssHeading);
	cdr.serialize(m_m_gnssLongitude);
	cdr.serialize(m_m_gnssLatitude);
	cdr.serialize(m_m_gnssLongitudeDeadReck);
	cdr.serialize(m_m_gnssLatitudeDeadReck);
	cdr.serialize(m_m_gnssVelOvrGrnd);
	cdr.serialize(m_m_gnssSampleRate);
	cdr.serialize(m_m_gnssVsblSat);
	cdr.serialize(m_m_gnssTrackedSat);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_gnssVdop);
	cdr.serialize(m_m_gnssHdop);
	cdr.serialize(m_m_gnssPdop);
	cdr.serialize(m_m_gnssPosFix);

	return cdr;
}
uint32_t CPfGnssLocalization::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfGnssLocalization::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_gnssAltitude);
	cdr.deserialize(m_m_gnssHeading);
	cdr.deserialize(m_m_gnssLongitude);
	cdr.deserialize(m_m_gnssLatitude);
	cdr.deserialize(m_m_gnssLongitudeDeadReck);
	cdr.deserialize(m_m_gnssLatitudeDeadReck);
	cdr.deserialize(m_m_gnssVelOvrGrnd);
	cdr.deserialize(m_m_gnssSampleRate);
	cdr.deserialize(m_m_gnssVsblSat);
	cdr.deserialize(m_m_gnssTrackedSat);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_gnssVdop);
	cdr.deserialize(m_m_gnssHdop);
	cdr.deserialize(m_m_gnssPdop);
	cdr.deserialize(m_m_gnssPosFix);

	return cdr;
}
bool CPfGnssLocalization::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfGnssLocalization::is_key_defined()
{
	return false;

}
void CPfGnssLocalization::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfGnssLocalization::serialize_key(char **buf,unsigned int *len)
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
bool CPfGnssLocalization::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfGnssLocalization::is_plain_types()
{
	return true;
}
uint32_t CPfGnssLocalization::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfGnssLocalization::set_key_val(CPfGnssLocalization const* const _data) noexcept
{

}
void CPfGnssLocalization::m_gnssAltitude(Float const _m_gnssAltitude)
{
	m_m_gnssAltitude = _m_gnssAltitude;
}
Float CPfGnssLocalization::m_gnssAltitude() const
{
	return m_m_gnssAltitude;
}
Float& CPfGnssLocalization::m_gnssAltitude()
{
	return m_m_gnssAltitude;
}

void CPfGnssLocalization::m_gnssHeading(Float const _m_gnssHeading)
{
	m_m_gnssHeading = _m_gnssHeading;
}
Float CPfGnssLocalization::m_gnssHeading() const
{
	return m_m_gnssHeading;
}
Float& CPfGnssLocalization::m_gnssHeading()
{
	return m_m_gnssHeading;
}

void CPfGnssLocalization::m_gnssLongitude(Float const _m_gnssLongitude)
{
	m_m_gnssLongitude = _m_gnssLongitude;
}
Float CPfGnssLocalization::m_gnssLongitude() const
{
	return m_m_gnssLongitude;
}
Float& CPfGnssLocalization::m_gnssLongitude()
{
	return m_m_gnssLongitude;
}

void CPfGnssLocalization::m_gnssLatitude(Float const _m_gnssLatitude)
{
	m_m_gnssLatitude = _m_gnssLatitude;
}
Float CPfGnssLocalization::m_gnssLatitude() const
{
	return m_m_gnssLatitude;
}
Float& CPfGnssLocalization::m_gnssLatitude()
{
	return m_m_gnssLatitude;
}

void CPfGnssLocalization::m_gnssLongitudeDeadReck(Float const _m_gnssLongitudeDeadReck)
{
	m_m_gnssLongitudeDeadReck = _m_gnssLongitudeDeadReck;
}
Float CPfGnssLocalization::m_gnssLongitudeDeadReck() const
{
	return m_m_gnssLongitudeDeadReck;
}
Float& CPfGnssLocalization::m_gnssLongitudeDeadReck()
{
	return m_m_gnssLongitudeDeadReck;
}

void CPfGnssLocalization::m_gnssLatitudeDeadReck(Float const _m_gnssLatitudeDeadReck)
{
	m_m_gnssLatitudeDeadReck = _m_gnssLatitudeDeadReck;
}
Float CPfGnssLocalization::m_gnssLatitudeDeadReck() const
{
	return m_m_gnssLatitudeDeadReck;
}
Float& CPfGnssLocalization::m_gnssLatitudeDeadReck()
{
	return m_m_gnssLatitudeDeadReck;
}

void CPfGnssLocalization::m_gnssVelOvrGrnd(Float const _m_gnssVelOvrGrnd)
{
	m_m_gnssVelOvrGrnd = _m_gnssVelOvrGrnd;
}
Float CPfGnssLocalization::m_gnssVelOvrGrnd() const
{
	return m_m_gnssVelOvrGrnd;
}
Float& CPfGnssLocalization::m_gnssVelOvrGrnd()
{
	return m_m_gnssVelOvrGrnd;
}

void CPfGnssLocalization::m_gnssSampleRate(SInt32 const _m_gnssSampleRate)
{
	m_m_gnssSampleRate = _m_gnssSampleRate;
}
SInt32 CPfGnssLocalization::m_gnssSampleRate() const
{
	return m_m_gnssSampleRate;
}
SInt32& CPfGnssLocalization::m_gnssSampleRate()
{
	return m_m_gnssSampleRate;
}

void CPfGnssLocalization::m_gnssVsblSat(UInt8 const _m_gnssVsblSat)
{
	m_m_gnssVsblSat = _m_gnssVsblSat;
}
UInt8 CPfGnssLocalization::m_gnssVsblSat() const
{
	return m_m_gnssVsblSat;
}
UInt8& CPfGnssLocalization::m_gnssVsblSat()
{
	return m_m_gnssVsblSat;
}

void CPfGnssLocalization::m_gnssTrackedSat(UInt8 const _m_gnssTrackedSat)
{
	m_m_gnssTrackedSat = _m_gnssTrackedSat;
}
UInt8 CPfGnssLocalization::m_gnssTrackedSat() const
{
	return m_m_gnssTrackedSat;
}
UInt8& CPfGnssLocalization::m_gnssTrackedSat()
{
	return m_m_gnssTrackedSat;
}

void CPfGnssLocalization::padding(UInt16 const _padding)
{
	m_padding = _padding;
}
UInt16 CPfGnssLocalization::padding() const
{
	return m_padding;
}
UInt16& CPfGnssLocalization::padding()
{
	return m_padding;
}

void CPfGnssLocalization::m_gnssVdop(Float const _m_gnssVdop)
{
	m_m_gnssVdop = _m_gnssVdop;
}
Float CPfGnssLocalization::m_gnssVdop() const
{
	return m_m_gnssVdop;
}
Float& CPfGnssLocalization::m_gnssVdop()
{
	return m_m_gnssVdop;
}

void CPfGnssLocalization::m_gnssHdop(Float const _m_gnssHdop)
{
	m_m_gnssHdop = _m_gnssHdop;
}
Float CPfGnssLocalization::m_gnssHdop() const
{
	return m_m_gnssHdop;
}
Float& CPfGnssLocalization::m_gnssHdop()
{
	return m_m_gnssHdop;
}

void CPfGnssLocalization::m_gnssPdop(Float const _m_gnssPdop)
{
	m_m_gnssPdop = _m_gnssPdop;
}
Float CPfGnssLocalization::m_gnssPdop() const
{
	return m_m_gnssPdop;
}
Float& CPfGnssLocalization::m_gnssPdop()
{
	return m_m_gnssPdop;
}

void CPfGnssLocalization::m_gnssPosFix(UInt32 const _m_gnssPosFix)
{
	m_m_gnssPosFix = _m_gnssPosFix;
}
UInt32 CPfGnssLocalization::m_gnssPosFix() const
{
	return m_m_gnssPosFix;
}
UInt32& CPfGnssLocalization::m_gnssPosFix()
{
	return m_m_gnssPosFix;
}

CPfHil::CPfHil()
{

}

magna::dds::DdsCdr& CPfHil::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_hilMode);

	return cdr;
}
uint32_t CPfHil::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfHil::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_hilMode);

	return cdr;
}
bool CPfHil::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfHil::is_key_defined()
{
	return false;

}
void CPfHil::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfHil::serialize_key(char **buf,unsigned int *len)
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
bool CPfHil::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfHil::is_plain_types()
{
	return true;
}
uint32_t CPfHil::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfHil::set_key_val(CPfHil const* const _data) noexcept
{

}
void CPfHil::m_hilMode(UInt8 const _m_hilMode)
{
	m_m_hilMode = _m_hilMode;
}
UInt8 CPfHil::m_hilMode() const
{
	return m_m_hilMode;
}
UInt8& CPfHil::m_hilMode()
{
	return m_m_hilMode;
}

CPfOdometry::CPfOdometry()
{

}

magna::dds::DdsCdr& CPfOdometry::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_vehicleVelocity);
	cdr.serialize(m_m_vehicleVelocityDisplay);
	cdr.serialize(m_m_brktrq);
	cdr.serialize(m_m_longitudinalAcceleration);
	cdr.serialize(m_m_lateralAcceleration);
	cdr.serialize(m_m_yawRate);
	cdr.serialize(m_m_wheelImpCtrFLTimestamp);
	cdr.serialize(m_m_wheelImpCtrFRTimestamp);
	cdr.serialize(m_m_wheelImpCtrRLTimestamp);
	cdr.serialize(m_m_wheelImpCtrRRTimestamp);
	cdr.serialize(m_m_wheelImpCtrFL);
	cdr.serialize(m_m_wheelImpCtrFR);
	cdr.serialize(m_m_wheelImpCtrRL);
	cdr.serialize(m_m_wheelImpCtrRR);
	cdr.serialize(m_m_vehicleDrvDir);
	cdr.serialize(m_m_qualifierVehicleVelocity);
	cdr.serialize(m_m_wheelDrvDirFL);
	cdr.serialize(m_m_wheelDrvDirFR);
	cdr.serialize(m_m_wheelDrvDirRL);
	cdr.serialize(m_m_wheelDrvDirRR);
	cdr.serialize(m_m_wheelRotationFLQualifier);
	cdr.serialize(m_m_wheelRotationFRQualifier);
	cdr.serialize(m_m_wheelRotationRLQualifier);
	cdr.serialize(m_m_wheelRotationRRQualifier);
	cdr.serialize(m_m_lateralAccelerationQualifier);
	cdr.serialize(m_m_longitudinalAccelerationQualifier);
	cdr.serialize(m_m_qualifierYawVelocityVehicle);
	cdr.serialize(m_m_PboxImuAcc_X);
	cdr.serialize(m_m_PboxImuAcc_Y);
	cdr.serialize(m_m_PboxImuAcc_Z);
	cdr.serialize(m_m_PboxImuGyro_X);
	cdr.serialize(m_m_PboxImuGyro_Y);
	cdr.serialize(m_m_PboxImuGyro_Z);
	cdr.serialize(m_m_PitchInstallAngle_f32);
	cdr.serialize(m_m_reserve1_f32);
	cdr.serialize(m_m_reserve2_f32);
	cdr.serialize(m_m_reserve3_f32);

	return cdr;
}
uint32_t CPfOdometry::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfOdometry::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_vehicleVelocity);
	cdr.deserialize(m_m_vehicleVelocityDisplay);
	cdr.deserialize(m_m_brktrq);
	cdr.deserialize(m_m_longitudinalAcceleration);
	cdr.deserialize(m_m_lateralAcceleration);
	cdr.deserialize(m_m_yawRate);
	cdr.deserialize(m_m_wheelImpCtrFLTimestamp);
	cdr.deserialize(m_m_wheelImpCtrFRTimestamp);
	cdr.deserialize(m_m_wheelImpCtrRLTimestamp);
	cdr.deserialize(m_m_wheelImpCtrRRTimestamp);
	cdr.deserialize(m_m_wheelImpCtrFL);
	cdr.deserialize(m_m_wheelImpCtrFR);
	cdr.deserialize(m_m_wheelImpCtrRL);
	cdr.deserialize(m_m_wheelImpCtrRR);
	cdr.deserialize(m_m_vehicleDrvDir);
	cdr.deserialize(m_m_qualifierVehicleVelocity);
	cdr.deserialize(m_m_wheelDrvDirFL);
	cdr.deserialize(m_m_wheelDrvDirFR);
	cdr.deserialize(m_m_wheelDrvDirRL);
	cdr.deserialize(m_m_wheelDrvDirRR);
	cdr.deserialize(m_m_wheelRotationFLQualifier);
	cdr.deserialize(m_m_wheelRotationFRQualifier);
	cdr.deserialize(m_m_wheelRotationRLQualifier);
	cdr.deserialize(m_m_wheelRotationRRQualifier);
	cdr.deserialize(m_m_lateralAccelerationQualifier);
	cdr.deserialize(m_m_longitudinalAccelerationQualifier);
	cdr.deserialize(m_m_qualifierYawVelocityVehicle);
	cdr.deserialize(m_m_PboxImuAcc_X);
	cdr.deserialize(m_m_PboxImuAcc_Y);
	cdr.deserialize(m_m_PboxImuAcc_Z);
	cdr.deserialize(m_m_PboxImuGyro_X);
	cdr.deserialize(m_m_PboxImuGyro_Y);
	cdr.deserialize(m_m_PboxImuGyro_Z);
	cdr.deserialize(m_m_PitchInstallAngle_f32);
	cdr.deserialize(m_m_reserve1_f32);
	cdr.deserialize(m_m_reserve2_f32);
	cdr.deserialize(m_m_reserve3_f32);

	return cdr;
}
bool CPfOdometry::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfOdometry::is_key_defined()
{
	return false;

}
void CPfOdometry::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfOdometry::serialize_key(char **buf,unsigned int *len)
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
bool CPfOdometry::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfOdometry::is_plain_types()
{
	return true;
}
uint32_t CPfOdometry::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfOdometry::set_key_val(CPfOdometry const* const _data) noexcept
{

}
void CPfOdometry::m_vehicleVelocity(Float const _m_vehicleVelocity)
{
	m_m_vehicleVelocity = _m_vehicleVelocity;
}
Float CPfOdometry::m_vehicleVelocity() const
{
	return m_m_vehicleVelocity;
}
Float& CPfOdometry::m_vehicleVelocity()
{
	return m_m_vehicleVelocity;
}

void CPfOdometry::m_vehicleVelocityDisplay(Float const _m_vehicleVelocityDisplay)
{
	m_m_vehicleVelocityDisplay = _m_vehicleVelocityDisplay;
}
Float CPfOdometry::m_vehicleVelocityDisplay() const
{
	return m_m_vehicleVelocityDisplay;
}
Float& CPfOdometry::m_vehicleVelocityDisplay()
{
	return m_m_vehicleVelocityDisplay;
}

void CPfOdometry::m_brktrq(Float const _m_brktrq)
{
	m_m_brktrq = _m_brktrq;
}
Float CPfOdometry::m_brktrq() const
{
	return m_m_brktrq;
}
Float& CPfOdometry::m_brktrq()
{
	return m_m_brktrq;
}

void CPfOdometry::m_longitudinalAcceleration(Float const _m_longitudinalAcceleration)
{
	m_m_longitudinalAcceleration = _m_longitudinalAcceleration;
}
Float CPfOdometry::m_longitudinalAcceleration() const
{
	return m_m_longitudinalAcceleration;
}
Float& CPfOdometry::m_longitudinalAcceleration()
{
	return m_m_longitudinalAcceleration;
}

void CPfOdometry::m_lateralAcceleration(Float const _m_lateralAcceleration)
{
	m_m_lateralAcceleration = _m_lateralAcceleration;
}
Float CPfOdometry::m_lateralAcceleration() const
{
	return m_m_lateralAcceleration;
}
Float& CPfOdometry::m_lateralAcceleration()
{
	return m_m_lateralAcceleration;
}

void CPfOdometry::m_yawRate(Float const _m_yawRate)
{
	m_m_yawRate = _m_yawRate;
}
Float CPfOdometry::m_yawRate() const
{
	return m_m_yawRate;
}
Float& CPfOdometry::m_yawRate()
{
	return m_m_yawRate;
}

void CPfOdometry::m_wheelImpCtrFLTimestamp(UInt64 const _m_wheelImpCtrFLTimestamp)
{
	m_m_wheelImpCtrFLTimestamp = _m_wheelImpCtrFLTimestamp;
}
UInt64 CPfOdometry::m_wheelImpCtrFLTimestamp() const
{
	return m_m_wheelImpCtrFLTimestamp;
}
UInt64& CPfOdometry::m_wheelImpCtrFLTimestamp()
{
	return m_m_wheelImpCtrFLTimestamp;
}

void CPfOdometry::m_wheelImpCtrFRTimestamp(UInt64 const _m_wheelImpCtrFRTimestamp)
{
	m_m_wheelImpCtrFRTimestamp = _m_wheelImpCtrFRTimestamp;
}
UInt64 CPfOdometry::m_wheelImpCtrFRTimestamp() const
{
	return m_m_wheelImpCtrFRTimestamp;
}
UInt64& CPfOdometry::m_wheelImpCtrFRTimestamp()
{
	return m_m_wheelImpCtrFRTimestamp;
}

void CPfOdometry::m_wheelImpCtrRLTimestamp(UInt64 const _m_wheelImpCtrRLTimestamp)
{
	m_m_wheelImpCtrRLTimestamp = _m_wheelImpCtrRLTimestamp;
}
UInt64 CPfOdometry::m_wheelImpCtrRLTimestamp() const
{
	return m_m_wheelImpCtrRLTimestamp;
}
UInt64& CPfOdometry::m_wheelImpCtrRLTimestamp()
{
	return m_m_wheelImpCtrRLTimestamp;
}

void CPfOdometry::m_wheelImpCtrRRTimestamp(UInt64 const _m_wheelImpCtrRRTimestamp)
{
	m_m_wheelImpCtrRRTimestamp = _m_wheelImpCtrRRTimestamp;
}
UInt64 CPfOdometry::m_wheelImpCtrRRTimestamp() const
{
	return m_m_wheelImpCtrRRTimestamp;
}
UInt64& CPfOdometry::m_wheelImpCtrRRTimestamp()
{
	return m_m_wheelImpCtrRRTimestamp;
}

void CPfOdometry::m_wheelImpCtrFL(UInt16 const _m_wheelImpCtrFL)
{
	m_m_wheelImpCtrFL = _m_wheelImpCtrFL;
}
UInt16 CPfOdometry::m_wheelImpCtrFL() const
{
	return m_m_wheelImpCtrFL;
}
UInt16& CPfOdometry::m_wheelImpCtrFL()
{
	return m_m_wheelImpCtrFL;
}

void CPfOdometry::m_wheelImpCtrFR(UInt16 const _m_wheelImpCtrFR)
{
	m_m_wheelImpCtrFR = _m_wheelImpCtrFR;
}
UInt16 CPfOdometry::m_wheelImpCtrFR() const
{
	return m_m_wheelImpCtrFR;
}
UInt16& CPfOdometry::m_wheelImpCtrFR()
{
	return m_m_wheelImpCtrFR;
}

void CPfOdometry::m_wheelImpCtrRL(UInt16 const _m_wheelImpCtrRL)
{
	m_m_wheelImpCtrRL = _m_wheelImpCtrRL;
}
UInt16 CPfOdometry::m_wheelImpCtrRL() const
{
	return m_m_wheelImpCtrRL;
}
UInt16& CPfOdometry::m_wheelImpCtrRL()
{
	return m_m_wheelImpCtrRL;
}

void CPfOdometry::m_wheelImpCtrRR(UInt16 const _m_wheelImpCtrRR)
{
	m_m_wheelImpCtrRR = _m_wheelImpCtrRR;
}
UInt16 CPfOdometry::m_wheelImpCtrRR() const
{
	return m_m_wheelImpCtrRR;
}
UInt16& CPfOdometry::m_wheelImpCtrRR()
{
	return m_m_wheelImpCtrRR;
}

void CPfOdometry::m_vehicleDrvDir(UInt32 const _m_vehicleDrvDir)
{
	m_m_vehicleDrvDir = _m_vehicleDrvDir;
}
UInt32 CPfOdometry::m_vehicleDrvDir() const
{
	return m_m_vehicleDrvDir;
}
UInt32& CPfOdometry::m_vehicleDrvDir()
{
	return m_m_vehicleDrvDir;
}

void CPfOdometry::m_qualifierVehicleVelocity(UInt32 const _m_qualifierVehicleVelocity)
{
	m_m_qualifierVehicleVelocity = _m_qualifierVehicleVelocity;
}
UInt32 CPfOdometry::m_qualifierVehicleVelocity() const
{
	return m_m_qualifierVehicleVelocity;
}
UInt32& CPfOdometry::m_qualifierVehicleVelocity()
{
	return m_m_qualifierVehicleVelocity;
}

void CPfOdometry::m_wheelDrvDirFL(UInt32 const _m_wheelDrvDirFL)
{
	m_m_wheelDrvDirFL = _m_wheelDrvDirFL;
}
UInt32 CPfOdometry::m_wheelDrvDirFL() const
{
	return m_m_wheelDrvDirFL;
}
UInt32& CPfOdometry::m_wheelDrvDirFL()
{
	return m_m_wheelDrvDirFL;
}

void CPfOdometry::m_wheelDrvDirFR(UInt32 const _m_wheelDrvDirFR)
{
	m_m_wheelDrvDirFR = _m_wheelDrvDirFR;
}
UInt32 CPfOdometry::m_wheelDrvDirFR() const
{
	return m_m_wheelDrvDirFR;
}
UInt32& CPfOdometry::m_wheelDrvDirFR()
{
	return m_m_wheelDrvDirFR;
}

void CPfOdometry::m_wheelDrvDirRL(UInt32 const _m_wheelDrvDirRL)
{
	m_m_wheelDrvDirRL = _m_wheelDrvDirRL;
}
UInt32 CPfOdometry::m_wheelDrvDirRL() const
{
	return m_m_wheelDrvDirRL;
}
UInt32& CPfOdometry::m_wheelDrvDirRL()
{
	return m_m_wheelDrvDirRL;
}

void CPfOdometry::m_wheelDrvDirRR(UInt32 const _m_wheelDrvDirRR)
{
	m_m_wheelDrvDirRR = _m_wheelDrvDirRR;
}
UInt32 CPfOdometry::m_wheelDrvDirRR() const
{
	return m_m_wheelDrvDirRR;
}
UInt32& CPfOdometry::m_wheelDrvDirRR()
{
	return m_m_wheelDrvDirRR;
}

void CPfOdometry::m_wheelRotationFLQualifier(UInt32 const _m_wheelRotationFLQualifier)
{
	m_m_wheelRotationFLQualifier = _m_wheelRotationFLQualifier;
}
UInt32 CPfOdometry::m_wheelRotationFLQualifier() const
{
	return m_m_wheelRotationFLQualifier;
}
UInt32& CPfOdometry::m_wheelRotationFLQualifier()
{
	return m_m_wheelRotationFLQualifier;
}

void CPfOdometry::m_wheelRotationFRQualifier(UInt32 const _m_wheelRotationFRQualifier)
{
	m_m_wheelRotationFRQualifier = _m_wheelRotationFRQualifier;
}
UInt32 CPfOdometry::m_wheelRotationFRQualifier() const
{
	return m_m_wheelRotationFRQualifier;
}
UInt32& CPfOdometry::m_wheelRotationFRQualifier()
{
	return m_m_wheelRotationFRQualifier;
}

void CPfOdometry::m_wheelRotationRLQualifier(UInt32 const _m_wheelRotationRLQualifier)
{
	m_m_wheelRotationRLQualifier = _m_wheelRotationRLQualifier;
}
UInt32 CPfOdometry::m_wheelRotationRLQualifier() const
{
	return m_m_wheelRotationRLQualifier;
}
UInt32& CPfOdometry::m_wheelRotationRLQualifier()
{
	return m_m_wheelRotationRLQualifier;
}

void CPfOdometry::m_wheelRotationRRQualifier(UInt32 const _m_wheelRotationRRQualifier)
{
	m_m_wheelRotationRRQualifier = _m_wheelRotationRRQualifier;
}
UInt32 CPfOdometry::m_wheelRotationRRQualifier() const
{
	return m_m_wheelRotationRRQualifier;
}
UInt32& CPfOdometry::m_wheelRotationRRQualifier()
{
	return m_m_wheelRotationRRQualifier;
}

void CPfOdometry::m_lateralAccelerationQualifier(UInt32 const _m_lateralAccelerationQualifier)
{
	m_m_lateralAccelerationQualifier = _m_lateralAccelerationQualifier;
}
UInt32 CPfOdometry::m_lateralAccelerationQualifier() const
{
	return m_m_lateralAccelerationQualifier;
}
UInt32& CPfOdometry::m_lateralAccelerationQualifier()
{
	return m_m_lateralAccelerationQualifier;
}

void CPfOdometry::m_longitudinalAccelerationQualifier(UInt32 const _m_longitudinalAccelerationQualifier)
{
	m_m_longitudinalAccelerationQualifier = _m_longitudinalAccelerationQualifier;
}
UInt32 CPfOdometry::m_longitudinalAccelerationQualifier() const
{
	return m_m_longitudinalAccelerationQualifier;
}
UInt32& CPfOdometry::m_longitudinalAccelerationQualifier()
{
	return m_m_longitudinalAccelerationQualifier;
}

void CPfOdometry::m_qualifierYawVelocityVehicle(UInt32 const _m_qualifierYawVelocityVehicle)
{
	m_m_qualifierYawVelocityVehicle = _m_qualifierYawVelocityVehicle;
}
UInt32 CPfOdometry::m_qualifierYawVelocityVehicle() const
{
	return m_m_qualifierYawVelocityVehicle;
}
UInt32& CPfOdometry::m_qualifierYawVelocityVehicle()
{
	return m_m_qualifierYawVelocityVehicle;
}

void CPfOdometry::m_PboxImuAcc_X(Float const _m_PboxImuAcc_X)
{
	m_m_PboxImuAcc_X = _m_PboxImuAcc_X;
}
Float CPfOdometry::m_PboxImuAcc_X() const
{
	return m_m_PboxImuAcc_X;
}
Float& CPfOdometry::m_PboxImuAcc_X()
{
	return m_m_PboxImuAcc_X;
}

void CPfOdometry::m_PboxImuAcc_Y(Float const _m_PboxImuAcc_Y)
{
	m_m_PboxImuAcc_Y = _m_PboxImuAcc_Y;
}
Float CPfOdometry::m_PboxImuAcc_Y() const
{
	return m_m_PboxImuAcc_Y;
}
Float& CPfOdometry::m_PboxImuAcc_Y()
{
	return m_m_PboxImuAcc_Y;
}

void CPfOdometry::m_PboxImuAcc_Z(Float const _m_PboxImuAcc_Z)
{
	m_m_PboxImuAcc_Z = _m_PboxImuAcc_Z;
}
Float CPfOdometry::m_PboxImuAcc_Z() const
{
	return m_m_PboxImuAcc_Z;
}
Float& CPfOdometry::m_PboxImuAcc_Z()
{
	return m_m_PboxImuAcc_Z;
}

void CPfOdometry::m_PboxImuGyro_X(Float const _m_PboxImuGyro_X)
{
	m_m_PboxImuGyro_X = _m_PboxImuGyro_X;
}
Float CPfOdometry::m_PboxImuGyro_X() const
{
	return m_m_PboxImuGyro_X;
}
Float& CPfOdometry::m_PboxImuGyro_X()
{
	return m_m_PboxImuGyro_X;
}

void CPfOdometry::m_PboxImuGyro_Y(Float const _m_PboxImuGyro_Y)
{
	m_m_PboxImuGyro_Y = _m_PboxImuGyro_Y;
}
Float CPfOdometry::m_PboxImuGyro_Y() const
{
	return m_m_PboxImuGyro_Y;
}
Float& CPfOdometry::m_PboxImuGyro_Y()
{
	return m_m_PboxImuGyro_Y;
}

void CPfOdometry::m_PboxImuGyro_Z(Float const _m_PboxImuGyro_Z)
{
	m_m_PboxImuGyro_Z = _m_PboxImuGyro_Z;
}
Float CPfOdometry::m_PboxImuGyro_Z() const
{
	return m_m_PboxImuGyro_Z;
}
Float& CPfOdometry::m_PboxImuGyro_Z()
{
	return m_m_PboxImuGyro_Z;
}

void CPfOdometry::m_PitchInstallAngle_f32(Float const _m_PitchInstallAngle_f32)
{
	m_m_PitchInstallAngle_f32 = _m_PitchInstallAngle_f32;
}
Float CPfOdometry::m_PitchInstallAngle_f32() const
{
	return m_m_PitchInstallAngle_f32;
}
Float& CPfOdometry::m_PitchInstallAngle_f32()
{
	return m_m_PitchInstallAngle_f32;
}

void CPfOdometry::m_reserve1_f32(Float const _m_reserve1_f32)
{
	m_m_reserve1_f32 = _m_reserve1_f32;
}
Float CPfOdometry::m_reserve1_f32() const
{
	return m_m_reserve1_f32;
}
Float& CPfOdometry::m_reserve1_f32()
{
	return m_m_reserve1_f32;
}

void CPfOdometry::m_reserve2_f32(Float const _m_reserve2_f32)
{
	m_m_reserve2_f32 = _m_reserve2_f32;
}
Float CPfOdometry::m_reserve2_f32() const
{
	return m_m_reserve2_f32;
}
Float& CPfOdometry::m_reserve2_f32()
{
	return m_m_reserve2_f32;
}

void CPfOdometry::m_reserve3_f32(Float const _m_reserve3_f32)
{
	m_m_reserve3_f32 = _m_reserve3_f32;
}
Float CPfOdometry::m_reserve3_f32() const
{
	return m_m_reserve3_f32;
}
Float& CPfOdometry::m_reserve3_f32()
{
	return m_m_reserve3_f32;
}

CPfPmaFixedPoint::CPfPmaFixedPoint()
{

}

magna::dds::DdsCdr& CPfPmaFixedPoint::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_steeringWheelAngle);
	cdr.serialize(m_m_vehSpeed);
	cdr.serialize(m_m_wheelRotationFL);
	cdr.serialize(m_m_wheelRotationFR);
	cdr.serialize(m_m_wheelRotationRL);
	cdr.serialize(m_m_wheelSpeedFL);
	cdr.serialize(m_m_wheelSpeedFR);
	cdr.serialize(m_m_wheelSpeedRL);
	cdr.serialize(m_m_wheelSpeedRR);
	cdr.serialize(m_m_wheelRotationRR);
	cdr.serialize(m_m_outsideTemp);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_vehicleStopState);

	return cdr;
}
uint32_t CPfPmaFixedPoint::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfPmaFixedPoint::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_steeringWheelAngle);
	cdr.deserialize(m_m_vehSpeed);
	cdr.deserialize(m_m_wheelRotationFL);
	cdr.deserialize(m_m_wheelRotationFR);
	cdr.deserialize(m_m_wheelRotationRL);
	cdr.deserialize(m_m_wheelSpeedFL);
	cdr.deserialize(m_m_wheelSpeedFR);
	cdr.deserialize(m_m_wheelSpeedRL);
	cdr.deserialize(m_m_wheelSpeedRR);
	cdr.deserialize(m_m_wheelRotationRR);
	cdr.deserialize(m_m_outsideTemp);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_vehicleStopState);

	return cdr;
}
bool CPfPmaFixedPoint::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfPmaFixedPoint::is_key_defined()
{
	return false;

}
void CPfPmaFixedPoint::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfPmaFixedPoint::serialize_key(char **buf,unsigned int *len)
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
bool CPfPmaFixedPoint::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfPmaFixedPoint::is_plain_types()
{
	return true;
}
uint32_t CPfPmaFixedPoint::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfPmaFixedPoint::set_key_val(CPfPmaFixedPoint const* const _data) noexcept
{

}
void CPfPmaFixedPoint::m_steeringWheelAngle(Float const _m_steeringWheelAngle)
{
	m_m_steeringWheelAngle = _m_steeringWheelAngle;
}
Float CPfPmaFixedPoint::m_steeringWheelAngle() const
{
	return m_m_steeringWheelAngle;
}
Float& CPfPmaFixedPoint::m_steeringWheelAngle()
{
	return m_m_steeringWheelAngle;
}

void CPfPmaFixedPoint::m_vehSpeed(UInt16 const _m_vehSpeed)
{
	m_m_vehSpeed = _m_vehSpeed;
}
UInt16 CPfPmaFixedPoint::m_vehSpeed() const
{
	return m_m_vehSpeed;
}
UInt16& CPfPmaFixedPoint::m_vehSpeed()
{
	return m_m_vehSpeed;
}

void CPfPmaFixedPoint::m_wheelRotationFL(UInt16 const _m_wheelRotationFL)
{
	m_m_wheelRotationFL = _m_wheelRotationFL;
}
UInt16 CPfPmaFixedPoint::m_wheelRotationFL() const
{
	return m_m_wheelRotationFL;
}
UInt16& CPfPmaFixedPoint::m_wheelRotationFL()
{
	return m_m_wheelRotationFL;
}

void CPfPmaFixedPoint::m_wheelRotationFR(UInt16 const _m_wheelRotationFR)
{
	m_m_wheelRotationFR = _m_wheelRotationFR;
}
UInt16 CPfPmaFixedPoint::m_wheelRotationFR() const
{
	return m_m_wheelRotationFR;
}
UInt16& CPfPmaFixedPoint::m_wheelRotationFR()
{
	return m_m_wheelRotationFR;
}

void CPfPmaFixedPoint::m_wheelRotationRL(UInt16 const _m_wheelRotationRL)
{
	m_m_wheelRotationRL = _m_wheelRotationRL;
}
UInt16 CPfPmaFixedPoint::m_wheelRotationRL() const
{
	return m_m_wheelRotationRL;
}
UInt16& CPfPmaFixedPoint::m_wheelRotationRL()
{
	return m_m_wheelRotationRL;
}

void CPfPmaFixedPoint::m_wheelSpeedFL(Float const _m_wheelSpeedFL)
{
	m_m_wheelSpeedFL = _m_wheelSpeedFL;
}
Float CPfPmaFixedPoint::m_wheelSpeedFL() const
{
	return m_m_wheelSpeedFL;
}
Float& CPfPmaFixedPoint::m_wheelSpeedFL()
{
	return m_m_wheelSpeedFL;
}

void CPfPmaFixedPoint::m_wheelSpeedFR(Float const _m_wheelSpeedFR)
{
	m_m_wheelSpeedFR = _m_wheelSpeedFR;
}
Float CPfPmaFixedPoint::m_wheelSpeedFR() const
{
	return m_m_wheelSpeedFR;
}
Float& CPfPmaFixedPoint::m_wheelSpeedFR()
{
	return m_m_wheelSpeedFR;
}

void CPfPmaFixedPoint::m_wheelSpeedRL(Float const _m_wheelSpeedRL)
{
	m_m_wheelSpeedRL = _m_wheelSpeedRL;
}
Float CPfPmaFixedPoint::m_wheelSpeedRL() const
{
	return m_m_wheelSpeedRL;
}
Float& CPfPmaFixedPoint::m_wheelSpeedRL()
{
	return m_m_wheelSpeedRL;
}

void CPfPmaFixedPoint::m_wheelSpeedRR(Float const _m_wheelSpeedRR)
{
	m_m_wheelSpeedRR = _m_wheelSpeedRR;
}
Float CPfPmaFixedPoint::m_wheelSpeedRR() const
{
	return m_m_wheelSpeedRR;
}
Float& CPfPmaFixedPoint::m_wheelSpeedRR()
{
	return m_m_wheelSpeedRR;
}

void CPfPmaFixedPoint::m_wheelRotationRR(UInt16 const _m_wheelRotationRR)
{
	m_m_wheelRotationRR = _m_wheelRotationRR;
}
UInt16 CPfPmaFixedPoint::m_wheelRotationRR() const
{
	return m_m_wheelRotationRR;
}
UInt16& CPfPmaFixedPoint::m_wheelRotationRR()
{
	return m_m_wheelRotationRR;
}

void CPfPmaFixedPoint::m_outsideTemp(SInt8 const _m_outsideTemp)
{
	m_m_outsideTemp = _m_outsideTemp;
}
SInt8 CPfPmaFixedPoint::m_outsideTemp() const
{
	return m_m_outsideTemp;
}
SInt8& CPfPmaFixedPoint::m_outsideTemp()
{
	return m_m_outsideTemp;
}

void CPfPmaFixedPoint::padding(UInt8 const _padding)
{
	m_padding = _padding;
}
UInt8 CPfPmaFixedPoint::padding() const
{
	return m_padding;
}
UInt8& CPfPmaFixedPoint::padding()
{
	return m_padding;
}

void CPfPmaFixedPoint::m_vehicleStopState(UInt32 const _m_vehicleStopState)
{
	m_m_vehicleStopState = _m_vehicleStopState;
}
UInt32 CPfPmaFixedPoint::m_vehicleStopState() const
{
	return m_m_vehicleStopState;
}
UInt32& CPfPmaFixedPoint::m_vehicleStopState()
{
	return m_m_vehicleStopState;
}

CPfSteering::CPfSteering()
{

}

magna::dds::DdsCdr& CPfSteering::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_frontWheelAngle);
	cdr.serialize(m_m_frontWheelAngleOffset);
	cdr.serialize(m_m_steeringWheelAngle);
	cdr.serialize(m_m_steeringWheelAngleSpd);
	cdr.serialize(m_m_rearAxleSteeringAngle);
	cdr.serialize(m_m_frontWheelAngleValid);
	cdr.serialize(m_m_frontWheelAngleOffsetValid);
	cdr.serialize(m_m_rearAxleSteeringAngleValid);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_qualifierSteeringAngle);
	cdr.serialize(m_m_frontWheelAngleStatus);
	cdr.serialize(m_m_steerWhlTrq);
	cdr.serialize(m_m_steerWhlTrqOffset);
	cdr.serialize(m_m_steerWhlTrqHys);
	cdr.serialize(m_m_steerWhlRimTrqHys);
	cdr.serialize(m_m_epsStatus);
	cdr.serialize(m_m_frontWheelAngleTimestamp);
	cdr.serialize(m_m_epsStatusRas);
	cdr.serialize(m_m_rearWheelAngleStatus);

	return cdr;
}
uint32_t CPfSteering::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfSteering::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_frontWheelAngle);
	cdr.deserialize(m_m_frontWheelAngleOffset);
	cdr.deserialize(m_m_steeringWheelAngle);
	cdr.deserialize(m_m_steeringWheelAngleSpd);
	cdr.deserialize(m_m_rearAxleSteeringAngle);
	cdr.deserialize(m_m_frontWheelAngleValid);
	cdr.deserialize(m_m_frontWheelAngleOffsetValid);
	cdr.deserialize(m_m_rearAxleSteeringAngleValid);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_qualifierSteeringAngle);
	cdr.deserialize(m_m_frontWheelAngleStatus);
	cdr.deserialize(m_m_steerWhlTrq);
	cdr.deserialize(m_m_steerWhlTrqOffset);
	cdr.deserialize(m_m_steerWhlTrqHys);
	cdr.deserialize(m_m_steerWhlRimTrqHys);
	cdr.deserialize(m_m_epsStatus);
	cdr.deserialize(m_m_frontWheelAngleTimestamp);
	cdr.deserialize(m_m_epsStatusRas);
	cdr.deserialize(m_m_rearWheelAngleStatus);

	return cdr;
}
bool CPfSteering::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfSteering::is_key_defined()
{
	return false;

}
void CPfSteering::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfSteering::serialize_key(char **buf,unsigned int *len)
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
bool CPfSteering::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfSteering::is_plain_types()
{
	return true;
}
uint32_t CPfSteering::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfSteering::set_key_val(CPfSteering const* const _data) noexcept
{

}
void CPfSteering::m_frontWheelAngle(Float const _m_frontWheelAngle)
{
	m_m_frontWheelAngle = _m_frontWheelAngle;
}
Float CPfSteering::m_frontWheelAngle() const
{
	return m_m_frontWheelAngle;
}
Float& CPfSteering::m_frontWheelAngle()
{
	return m_m_frontWheelAngle;
}

void CPfSteering::m_frontWheelAngleOffset(Float const _m_frontWheelAngleOffset)
{
	m_m_frontWheelAngleOffset = _m_frontWheelAngleOffset;
}
Float CPfSteering::m_frontWheelAngleOffset() const
{
	return m_m_frontWheelAngleOffset;
}
Float& CPfSteering::m_frontWheelAngleOffset()
{
	return m_m_frontWheelAngleOffset;
}

void CPfSteering::m_steeringWheelAngle(Float const _m_steeringWheelAngle)
{
	m_m_steeringWheelAngle = _m_steeringWheelAngle;
}
Float CPfSteering::m_steeringWheelAngle() const
{
	return m_m_steeringWheelAngle;
}
Float& CPfSteering::m_steeringWheelAngle()
{
	return m_m_steeringWheelAngle;
}

void CPfSteering::m_steeringWheelAngleSpd(Float const _m_steeringWheelAngleSpd)
{
	m_m_steeringWheelAngleSpd = _m_steeringWheelAngleSpd;
}
Float CPfSteering::m_steeringWheelAngleSpd() const
{
	return m_m_steeringWheelAngleSpd;
}
Float& CPfSteering::m_steeringWheelAngleSpd()
{
	return m_m_steeringWheelAngleSpd;
}

void CPfSteering::m_rearAxleSteeringAngle(Float const _m_rearAxleSteeringAngle)
{
	m_m_rearAxleSteeringAngle = _m_rearAxleSteeringAngle;
}
Float CPfSteering::m_rearAxleSteeringAngle() const
{
	return m_m_rearAxleSteeringAngle;
}
Float& CPfSteering::m_rearAxleSteeringAngle()
{
	return m_m_rearAxleSteeringAngle;
}

void CPfSteering::m_frontWheelAngleValid(Boolean const _m_frontWheelAngleValid)
{
	m_m_frontWheelAngleValid = _m_frontWheelAngleValid;
}
Boolean CPfSteering::m_frontWheelAngleValid() const
{
	return m_m_frontWheelAngleValid;
}
Boolean& CPfSteering::m_frontWheelAngleValid()
{
	return m_m_frontWheelAngleValid;
}

void CPfSteering::m_frontWheelAngleOffsetValid(Boolean const _m_frontWheelAngleOffsetValid)
{
	m_m_frontWheelAngleOffsetValid = _m_frontWheelAngleOffsetValid;
}
Boolean CPfSteering::m_frontWheelAngleOffsetValid() const
{
	return m_m_frontWheelAngleOffsetValid;
}
Boolean& CPfSteering::m_frontWheelAngleOffsetValid()
{
	return m_m_frontWheelAngleOffsetValid;
}

void CPfSteering::m_rearAxleSteeringAngleValid(Boolean const _m_rearAxleSteeringAngleValid)
{
	m_m_rearAxleSteeringAngleValid = _m_rearAxleSteeringAngleValid;
}
Boolean CPfSteering::m_rearAxleSteeringAngleValid() const
{
	return m_m_rearAxleSteeringAngleValid;
}
Boolean& CPfSteering::m_rearAxleSteeringAngleValid()
{
	return m_m_rearAxleSteeringAngleValid;
}

void CPfSteering::padding(UInt8 const _padding)
{
	m_padding = _padding;
}
UInt8 CPfSteering::padding() const
{
	return m_padding;
}
UInt8& CPfSteering::padding()
{
	return m_padding;
}

void CPfSteering::m_qualifierSteeringAngle(UInt32 const _m_qualifierSteeringAngle)
{
	m_m_qualifierSteeringAngle = _m_qualifierSteeringAngle;
}
UInt32 CPfSteering::m_qualifierSteeringAngle() const
{
	return m_m_qualifierSteeringAngle;
}
UInt32& CPfSteering::m_qualifierSteeringAngle()
{
	return m_m_qualifierSteeringAngle;
}

void CPfSteering::m_frontWheelAngleStatus(UInt32 const _m_frontWheelAngleStatus)
{
	m_m_frontWheelAngleStatus = _m_frontWheelAngleStatus;
}
UInt32 CPfSteering::m_frontWheelAngleStatus() const
{
	return m_m_frontWheelAngleStatus;
}
UInt32& CPfSteering::m_frontWheelAngleStatus()
{
	return m_m_frontWheelAngleStatus;
}

void CPfSteering::m_steerWhlTrq(Float const _m_steerWhlTrq)
{
	m_m_steerWhlTrq = _m_steerWhlTrq;
}
Float CPfSteering::m_steerWhlTrq() const
{
	return m_m_steerWhlTrq;
}
Float& CPfSteering::m_steerWhlTrq()
{
	return m_m_steerWhlTrq;
}

void CPfSteering::m_steerWhlTrqOffset(Float const _m_steerWhlTrqOffset)
{
	m_m_steerWhlTrqOffset = _m_steerWhlTrqOffset;
}
Float CPfSteering::m_steerWhlTrqOffset() const
{
	return m_m_steerWhlTrqOffset;
}
Float& CPfSteering::m_steerWhlTrqOffset()
{
	return m_m_steerWhlTrqOffset;
}

void CPfSteering::m_steerWhlTrqHys(Float const _m_steerWhlTrqHys)
{
	m_m_steerWhlTrqHys = _m_steerWhlTrqHys;
}
Float CPfSteering::m_steerWhlTrqHys() const
{
	return m_m_steerWhlTrqHys;
}
Float& CPfSteering::m_steerWhlTrqHys()
{
	return m_m_steerWhlTrqHys;
}

void CPfSteering::m_steerWhlRimTrqHys(Float const _m_steerWhlRimTrqHys)
{
	m_m_steerWhlRimTrqHys = _m_steerWhlRimTrqHys;
}
Float CPfSteering::m_steerWhlRimTrqHys() const
{
	return m_m_steerWhlRimTrqHys;
}
Float& CPfSteering::m_steerWhlRimTrqHys()
{
	return m_m_steerWhlRimTrqHys;
}

void CPfSteering::m_epsStatus(UInt32 const _m_epsStatus)
{
	m_m_epsStatus = _m_epsStatus;
}
UInt32 CPfSteering::m_epsStatus() const
{
	return m_m_epsStatus;
}
UInt32& CPfSteering::m_epsStatus()
{
	return m_m_epsStatus;
}

void CPfSteering::m_frontWheelAngleTimestamp(UInt64 const _m_frontWheelAngleTimestamp)
{
	m_m_frontWheelAngleTimestamp = _m_frontWheelAngleTimestamp;
}
UInt64 CPfSteering::m_frontWheelAngleTimestamp() const
{
	return m_m_frontWheelAngleTimestamp;
}
UInt64& CPfSteering::m_frontWheelAngleTimestamp()
{
	return m_m_frontWheelAngleTimestamp;
}

void CPfSteering::m_epsStatusRas(UInt32 const _m_epsStatusRas)
{
	m_m_epsStatusRas = _m_epsStatusRas;
}
UInt32 CPfSteering::m_epsStatusRas() const
{
	return m_m_epsStatusRas;
}
UInt32& CPfSteering::m_epsStatusRas()
{
	return m_m_epsStatusRas;
}

void CPfSteering::m_rearWheelAngleStatus(UInt32 const _m_rearWheelAngleStatus)
{
	m_m_rearWheelAngleStatus = _m_rearWheelAngleStatus;
}
UInt32 CPfSteering::m_rearWheelAngleStatus() const
{
	return m_m_rearWheelAngleStatus;
}
UInt32& CPfSteering::m_rearWheelAngleStatus()
{
	return m_m_rearWheelAngleStatus;
}

CPfTirePress::CPfTirePress()
{

}

magna::dds::DdsCdr& CPfTirePress::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_tirePressFL);
	cdr.serialize(m_m_tirePressFR);
	cdr.serialize(m_m_tirePressRL);
	cdr.serialize(m_m_tirePressRR);
	cdr.serialize(m_m_tirePressQualifier);

	return cdr;
}
uint32_t CPfTirePress::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfTirePress::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_tirePressFL);
	cdr.deserialize(m_m_tirePressFR);
	cdr.deserialize(m_m_tirePressRL);
	cdr.deserialize(m_m_tirePressRR);
	cdr.deserialize(m_m_tirePressQualifier);

	return cdr;
}
bool CPfTirePress::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfTirePress::is_key_defined()
{
	return false;

}
void CPfTirePress::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfTirePress::serialize_key(char **buf,unsigned int *len)
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
bool CPfTirePress::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfTirePress::is_plain_types()
{
	return true;
}
uint32_t CPfTirePress::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfTirePress::set_key_val(CPfTirePress const* const _data) noexcept
{

}
void CPfTirePress::m_tirePressFL(Float const _m_tirePressFL)
{
	m_m_tirePressFL = _m_tirePressFL;
}
Float CPfTirePress::m_tirePressFL() const
{
	return m_m_tirePressFL;
}
Float& CPfTirePress::m_tirePressFL()
{
	return m_m_tirePressFL;
}

void CPfTirePress::m_tirePressFR(Float const _m_tirePressFR)
{
	m_m_tirePressFR = _m_tirePressFR;
}
Float CPfTirePress::m_tirePressFR() const
{
	return m_m_tirePressFR;
}
Float& CPfTirePress::m_tirePressFR()
{
	return m_m_tirePressFR;
}

void CPfTirePress::m_tirePressRL(Float const _m_tirePressRL)
{
	m_m_tirePressRL = _m_tirePressRL;
}
Float CPfTirePress::m_tirePressRL() const
{
	return m_m_tirePressRL;
}
Float& CPfTirePress::m_tirePressRL()
{
	return m_m_tirePressRL;
}

void CPfTirePress::m_tirePressRR(Float const _m_tirePressRR)
{
	m_m_tirePressRR = _m_tirePressRR;
}
Float CPfTirePress::m_tirePressRR() const
{
	return m_m_tirePressRR;
}
Float& CPfTirePress::m_tirePressRR()
{
	return m_m_tirePressRR;
}

void CPfTirePress::m_tirePressQualifier(UInt32 const _m_tirePressQualifier)
{
	m_m_tirePressQualifier = _m_tirePressQualifier;
}
UInt32 CPfTirePress::m_tirePressQualifier() const
{
	return m_m_tirePressQualifier;
}
UInt32& CPfTirePress::m_tirePressQualifier()
{
	return m_m_tirePressQualifier;
}

CPfTrailer::CPfTrailer()
{

}

magna::dds::DdsCdr& CPfTrailer::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_trailerState);
	cdr.serialize(m_m_trailerHitchPresent);

	return cdr;
}
uint32_t CPfTrailer::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfTrailer::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_trailerState);
	cdr.deserialize(m_m_trailerHitchPresent);

	return cdr;
}
bool CPfTrailer::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfTrailer::is_key_defined()
{
	return false;

}
void CPfTrailer::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfTrailer::serialize_key(char **buf,unsigned int *len)
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
bool CPfTrailer::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfTrailer::is_plain_types()
{
	return true;
}
uint32_t CPfTrailer::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfTrailer::set_key_val(CPfTrailer const* const _data) noexcept
{

}
void CPfTrailer::m_trailerState(Boolean const _m_trailerState)
{
	m_m_trailerState = _m_trailerState;
}
Boolean CPfTrailer::m_trailerState() const
{
	return m_m_trailerState;
}
Boolean& CPfTrailer::m_trailerState()
{
	return m_m_trailerState;
}

void CPfTrailer::m_trailerHitchPresent(Boolean const _m_trailerHitchPresent)
{
	m_m_trailerHitchPresent = _m_trailerHitchPresent;
}
Boolean CPfTrailer::m_trailerHitchPresent() const
{
	return m_m_trailerHitchPresent;
}
Boolean& CPfTrailer::m_trailerHitchPresent()
{
	return m_m_trailerHitchPresent;
}

CPfVehicleInfo::CPfVehicleInfo()
{

}

magna::dds::DdsCdr& CPfVehicleInfo::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_ignSwState);
	cdr.serialize(m_m_indicatorStatus);
	cdr.serialize(m_m_turnSWStatus);
	cdr.serialize(m_m_uBatt);
	cdr.serialize(m_m_KL15_status);
	cdr.serialize(m_m_lowBeamLeft);
	cdr.serialize(m_m_lowBeamRight);
	cdr.serialize(m_m_highBeamLeft);
	cdr.serialize(m_m_highBeamRight);
	cdr.serialize(m_m_fogLampFront);
	cdr.serialize(m_m_fogLampRear);
	cdr.serialize(m_m_brkLightAct);
	cdr.serialize(m_m_DriverSeatBeltNotBuckled);
	cdr.serialize(m_m_externalFuncAct);
	cdr.serialize(m_m_chargeState);
	cdr.serialize(m_m_engineStart);
	cdr.serialize(m_m_accPedalPosition);
	cdr.serialize(m_m_engineIsRunning);
	cdr.serialize(m_m_accPedalIntervened);
	cdr.serialize(m_padding);
	cdr.serialize(m_m_VehDriveMode);
	cdr.serialize(m_m_bgLightState);
	cdr.serialize(m_m_dayNightModeState);
	cdr.serialize(m_m_motorTorqFeedback_nm);
	cdr.serialize(m_m_engineIsReady);
	cdr.serialize(m_m_CarModeValid);
	cdr.serialize(m_m_CarMode);
	cdr.serialize(m_m_LicenseNum);
	cdr.serialize(m_APASoftKeyRequest);
	cdr.serialize(m_APASoftKeyRequestNoReverse);

	return cdr;
}
uint32_t CPfVehicleInfo::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfVehicleInfo::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_ignSwState);
	cdr.deserialize(m_m_indicatorStatus);
	cdr.deserialize(m_m_turnSWStatus);
	cdr.deserialize(m_m_uBatt);
	cdr.deserialize(m_m_KL15_status);
	cdr.deserialize(m_m_lowBeamLeft);
	cdr.deserialize(m_m_lowBeamRight);
	cdr.deserialize(m_m_highBeamLeft);
	cdr.deserialize(m_m_highBeamRight);
	cdr.deserialize(m_m_fogLampFront);
	cdr.deserialize(m_m_fogLampRear);
	cdr.deserialize(m_m_brkLightAct);
	cdr.deserialize(m_m_DriverSeatBeltNotBuckled);
	cdr.deserialize(m_m_externalFuncAct);
	cdr.deserialize(m_m_chargeState);
	cdr.deserialize(m_m_engineStart);
	cdr.deserialize(m_m_accPedalPosition);
	cdr.deserialize(m_m_engineIsRunning);
	cdr.deserialize(m_m_accPedalIntervened);
	cdr.deserialize(m_padding);
	cdr.deserialize(m_m_VehDriveMode);
	cdr.deserialize(m_m_bgLightState);
	cdr.deserialize(m_m_dayNightModeState);
	cdr.deserialize(m_m_motorTorqFeedback_nm);
	cdr.deserialize(m_m_engineIsReady);
	cdr.deserialize(m_m_CarModeValid);
	cdr.deserialize(m_m_CarMode);
	cdr.deserialize(m_m_LicenseNum);
	cdr.deserialize(m_APASoftKeyRequest);
	cdr.deserialize(m_APASoftKeyRequestNoReverse);

	return cdr;
}
bool CPfVehicleInfo::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfVehicleInfo::is_key_defined()
{
	return false;

}
void CPfVehicleInfo::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfVehicleInfo::serialize_key(char **buf,unsigned int *len)
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
bool CPfVehicleInfo::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfVehicleInfo::is_plain_types()
{
	return true;
}
uint32_t CPfVehicleInfo::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfVehicleInfo::set_key_val(CPfVehicleInfo const* const _data) noexcept
{

}
void CPfVehicleInfo::m_ignSwState(UInt32 const _m_ignSwState)
{
	m_m_ignSwState = _m_ignSwState;
}
UInt32 CPfVehicleInfo::m_ignSwState() const
{
	return m_m_ignSwState;
}
UInt32& CPfVehicleInfo::m_ignSwState()
{
	return m_m_ignSwState;
}

void CPfVehicleInfo::m_indicatorStatus(UInt32 const _m_indicatorStatus)
{
	m_m_indicatorStatus = _m_indicatorStatus;
}
UInt32 CPfVehicleInfo::m_indicatorStatus() const
{
	return m_m_indicatorStatus;
}
UInt32& CPfVehicleInfo::m_indicatorStatus()
{
	return m_m_indicatorStatus;
}

void CPfVehicleInfo::m_turnSWStatus(UInt32 const _m_turnSWStatus)
{
	m_m_turnSWStatus = _m_turnSWStatus;
}
UInt32 CPfVehicleInfo::m_turnSWStatus() const
{
	return m_m_turnSWStatus;
}
UInt32& CPfVehicleInfo::m_turnSWStatus()
{
	return m_m_turnSWStatus;
}

void CPfVehicleInfo::m_uBatt(UInt8 const _m_uBatt)
{
	m_m_uBatt = _m_uBatt;
}
UInt8 CPfVehicleInfo::m_uBatt() const
{
	return m_m_uBatt;
}
UInt8& CPfVehicleInfo::m_uBatt()
{
	return m_m_uBatt;
}

void CPfVehicleInfo::m_KL15_status(Boolean const _m_KL15_status)
{
	m_m_KL15_status = _m_KL15_status;
}
Boolean CPfVehicleInfo::m_KL15_status() const
{
	return m_m_KL15_status;
}
Boolean& CPfVehicleInfo::m_KL15_status()
{
	return m_m_KL15_status;
}

void CPfVehicleInfo::m_lowBeamLeft(Boolean const _m_lowBeamLeft)
{
	m_m_lowBeamLeft = _m_lowBeamLeft;
}
Boolean CPfVehicleInfo::m_lowBeamLeft() const
{
	return m_m_lowBeamLeft;
}
Boolean& CPfVehicleInfo::m_lowBeamLeft()
{
	return m_m_lowBeamLeft;
}

void CPfVehicleInfo::m_lowBeamRight(Boolean const _m_lowBeamRight)
{
	m_m_lowBeamRight = _m_lowBeamRight;
}
Boolean CPfVehicleInfo::m_lowBeamRight() const
{
	return m_m_lowBeamRight;
}
Boolean& CPfVehicleInfo::m_lowBeamRight()
{
	return m_m_lowBeamRight;
}

void CPfVehicleInfo::m_highBeamLeft(Boolean const _m_highBeamLeft)
{
	m_m_highBeamLeft = _m_highBeamLeft;
}
Boolean CPfVehicleInfo::m_highBeamLeft() const
{
	return m_m_highBeamLeft;
}
Boolean& CPfVehicleInfo::m_highBeamLeft()
{
	return m_m_highBeamLeft;
}

void CPfVehicleInfo::m_highBeamRight(Boolean const _m_highBeamRight)
{
	m_m_highBeamRight = _m_highBeamRight;
}
Boolean CPfVehicleInfo::m_highBeamRight() const
{
	return m_m_highBeamRight;
}
Boolean& CPfVehicleInfo::m_highBeamRight()
{
	return m_m_highBeamRight;
}

void CPfVehicleInfo::m_fogLampFront(Boolean const _m_fogLampFront)
{
	m_m_fogLampFront = _m_fogLampFront;
}
Boolean CPfVehicleInfo::m_fogLampFront() const
{
	return m_m_fogLampFront;
}
Boolean& CPfVehicleInfo::m_fogLampFront()
{
	return m_m_fogLampFront;
}

void CPfVehicleInfo::m_fogLampRear(Boolean const _m_fogLampRear)
{
	m_m_fogLampRear = _m_fogLampRear;
}
Boolean CPfVehicleInfo::m_fogLampRear() const
{
	return m_m_fogLampRear;
}
Boolean& CPfVehicleInfo::m_fogLampRear()
{
	return m_m_fogLampRear;
}

void CPfVehicleInfo::m_brkLightAct(Boolean const _m_brkLightAct)
{
	m_m_brkLightAct = _m_brkLightAct;
}
Boolean CPfVehicleInfo::m_brkLightAct() const
{
	return m_m_brkLightAct;
}
Boolean& CPfVehicleInfo::m_brkLightAct()
{
	return m_m_brkLightAct;
}

void CPfVehicleInfo::m_DriverSeatBeltNotBuckled(Boolean const _m_DriverSeatBeltNotBuckled)
{
	m_m_DriverSeatBeltNotBuckled = _m_DriverSeatBeltNotBuckled;
}
Boolean CPfVehicleInfo::m_DriverSeatBeltNotBuckled() const
{
	return m_m_DriverSeatBeltNotBuckled;
}
Boolean& CPfVehicleInfo::m_DriverSeatBeltNotBuckled()
{
	return m_m_DriverSeatBeltNotBuckled;
}

void CPfVehicleInfo::m_externalFuncAct(Boolean const _m_externalFuncAct)
{
	m_m_externalFuncAct = _m_externalFuncAct;
}
Boolean CPfVehicleInfo::m_externalFuncAct() const
{
	return m_m_externalFuncAct;
}
Boolean& CPfVehicleInfo::m_externalFuncAct()
{
	return m_m_externalFuncAct;
}

void CPfVehicleInfo::m_chargeState(Boolean const _m_chargeState)
{
	m_m_chargeState = _m_chargeState;
}
Boolean CPfVehicleInfo::m_chargeState() const
{
	return m_m_chargeState;
}
Boolean& CPfVehicleInfo::m_chargeState()
{
	return m_m_chargeState;
}

void CPfVehicleInfo::m_engineStart(UInt32 const _m_engineStart)
{
	m_m_engineStart = _m_engineStart;
}
UInt32 CPfVehicleInfo::m_engineStart() const
{
	return m_m_engineStart;
}
UInt32& CPfVehicleInfo::m_engineStart()
{
	return m_m_engineStart;
}

void CPfVehicleInfo::m_accPedalPosition(Float const _m_accPedalPosition)
{
	m_m_accPedalPosition = _m_accPedalPosition;
}
Float CPfVehicleInfo::m_accPedalPosition() const
{
	return m_m_accPedalPosition;
}
Float& CPfVehicleInfo::m_accPedalPosition()
{
	return m_m_accPedalPosition;
}

void CPfVehicleInfo::m_engineIsRunning(Boolean const _m_engineIsRunning)
{
	m_m_engineIsRunning = _m_engineIsRunning;
}
Boolean CPfVehicleInfo::m_engineIsRunning() const
{
	return m_m_engineIsRunning;
}
Boolean& CPfVehicleInfo::m_engineIsRunning()
{
	return m_m_engineIsRunning;
}

void CPfVehicleInfo::m_accPedalIntervened(Boolean const _m_accPedalIntervened)
{
	m_m_accPedalIntervened = _m_accPedalIntervened;
}
Boolean CPfVehicleInfo::m_accPedalIntervened() const
{
	return m_m_accPedalIntervened;
}
Boolean& CPfVehicleInfo::m_accPedalIntervened()
{
	return m_m_accPedalIntervened;
}

void CPfVehicleInfo::padding(UInt16 const _padding)
{
	m_padding = _padding;
}
UInt16 CPfVehicleInfo::padding() const
{
	return m_padding;
}
UInt16& CPfVehicleInfo::padding()
{
	return m_padding;
}

void CPfVehicleInfo::m_VehDriveMode(UInt32 const _m_VehDriveMode)
{
	m_m_VehDriveMode = _m_VehDriveMode;
}
UInt32 CPfVehicleInfo::m_VehDriveMode() const
{
	return m_m_VehDriveMode;
}
UInt32& CPfVehicleInfo::m_VehDriveMode()
{
	return m_m_VehDriveMode;
}

void CPfVehicleInfo::m_bgLightState(UInt32 const _m_bgLightState)
{
	m_m_bgLightState = _m_bgLightState;
}
UInt32 CPfVehicleInfo::m_bgLightState() const
{
	return m_m_bgLightState;
}
UInt32& CPfVehicleInfo::m_bgLightState()
{
	return m_m_bgLightState;
}

void CPfVehicleInfo::m_dayNightModeState(UInt32 const _m_dayNightModeState)
{
	m_m_dayNightModeState = _m_dayNightModeState;
}
UInt32 CPfVehicleInfo::m_dayNightModeState() const
{
	return m_m_dayNightModeState;
}
UInt32& CPfVehicleInfo::m_dayNightModeState()
{
	return m_m_dayNightModeState;
}

void CPfVehicleInfo::m_motorTorqFeedback_nm(Float const _m_motorTorqFeedback_nm)
{
	m_m_motorTorqFeedback_nm = _m_motorTorqFeedback_nm;
}
Float CPfVehicleInfo::m_motorTorqFeedback_nm() const
{
	return m_m_motorTorqFeedback_nm;
}
Float& CPfVehicleInfo::m_motorTorqFeedback_nm()
{
	return m_m_motorTorqFeedback_nm;
}

void CPfVehicleInfo::m_engineIsReady(Boolean const _m_engineIsReady)
{
	m_m_engineIsReady = _m_engineIsReady;
}
Boolean CPfVehicleInfo::m_engineIsReady() const
{
	return m_m_engineIsReady;
}
Boolean& CPfVehicleInfo::m_engineIsReady()
{
	return m_m_engineIsReady;
}

void CPfVehicleInfo::m_CarModeValid(Boolean const _m_CarModeValid)
{
	m_m_CarModeValid = _m_CarModeValid;
}
Boolean CPfVehicleInfo::m_CarModeValid() const
{
	return m_m_CarModeValid;
}
Boolean& CPfVehicleInfo::m_CarModeValid()
{
	return m_m_CarModeValid;
}

void CPfVehicleInfo::m_CarMode(UInt8 const _m_CarMode)
{
	m_m_CarMode = _m_CarMode;
}
UInt8 CPfVehicleInfo::m_CarMode() const
{
	return m_m_CarMode;
}
UInt8& CPfVehicleInfo::m_CarMode()
{
	return m_m_CarMode;
}

void CPfVehicleInfo::m_LicenseNum(std::array<UInt8,8> const &_m_LicenseNum)
{
	m_m_LicenseNum = _m_LicenseNum;
}
void CPfVehicleInfo::m_LicenseNum(std::array<UInt8,8> &&_m_LicenseNum)
{
	m_m_LicenseNum = std::move(_m_LicenseNum);
}
std::array<UInt8,8> const& CPfVehicleInfo::m_LicenseNum() const
{
	return m_m_LicenseNum;
}
std::array<UInt8,8>& CPfVehicleInfo::m_LicenseNum()
{
	return m_m_LicenseNum;
}

void CPfVehicleInfo::APASoftKeyRequest(Boolean const _APASoftKeyRequest)
{
	m_APASoftKeyRequest = _APASoftKeyRequest;
}
Boolean CPfVehicleInfo::APASoftKeyRequest() const
{
	return m_APASoftKeyRequest;
}
Boolean& CPfVehicleInfo::APASoftKeyRequest()
{
	return m_APASoftKeyRequest;
}

void CPfVehicleInfo::APASoftKeyRequestNoReverse(Boolean const _APASoftKeyRequestNoReverse)
{
	m_APASoftKeyRequestNoReverse = _APASoftKeyRequestNoReverse;
}
Boolean CPfVehicleInfo::APASoftKeyRequestNoReverse() const
{
	return m_APASoftKeyRequestNoReverse;
}
Boolean& CPfVehicleInfo::APASoftKeyRequestNoReverse()
{
	return m_APASoftKeyRequestNoReverse;
}

CPfVehicleLevel::CPfVehicleLevel()
{

}

magna::dds::DdsCdr& CPfVehicleLevel::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_relativeVehicleLevel);
	cdr.serialize(m_m_vehicleLevelFL);
	cdr.serialize(m_m_vehicleLevelFR);
	cdr.serialize(m_m_vehicleLevelRL);
	cdr.serialize(m_m_vehicleLevelRR);
	cdr.serialize(m_m_qualifierVehicleLevel);

	return cdr;
}
uint32_t CPfVehicleLevel::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& CPfVehicleLevel::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_relativeVehicleLevel);
	cdr.deserialize(m_m_vehicleLevelFL);
	cdr.deserialize(m_m_vehicleLevelFR);
	cdr.deserialize(m_m_vehicleLevelRL);
	cdr.deserialize(m_m_vehicleLevelRR);
	cdr.deserialize(m_m_qualifierVehicleLevel);

	return cdr;
}
bool CPfVehicleLevel::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool CPfVehicleLevel::is_key_defined()
{
	return false;

}
void CPfVehicleLevel::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void CPfVehicleLevel::serialize_key(char **buf,unsigned int *len)
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
bool CPfVehicleLevel::is_key_serialize_by_cdr()
{
	return false;

}
bool CPfVehicleLevel::is_plain_types()
{
	return true;
}
uint32_t CPfVehicleLevel::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void CPfVehicleLevel::set_key_val(CPfVehicleLevel const* const _data) noexcept
{

}
void CPfVehicleLevel::m_relativeVehicleLevel(Float const _m_relativeVehicleLevel)
{
	m_m_relativeVehicleLevel = _m_relativeVehicleLevel;
}
Float CPfVehicleLevel::m_relativeVehicleLevel() const
{
	return m_m_relativeVehicleLevel;
}
Float& CPfVehicleLevel::m_relativeVehicleLevel()
{
	return m_m_relativeVehicleLevel;
}

void CPfVehicleLevel::m_vehicleLevelFL(Float const _m_vehicleLevelFL)
{
	m_m_vehicleLevelFL = _m_vehicleLevelFL;
}
Float CPfVehicleLevel::m_vehicleLevelFL() const
{
	return m_m_vehicleLevelFL;
}
Float& CPfVehicleLevel::m_vehicleLevelFL()
{
	return m_m_vehicleLevelFL;
}

void CPfVehicleLevel::m_vehicleLevelFR(Float const _m_vehicleLevelFR)
{
	m_m_vehicleLevelFR = _m_vehicleLevelFR;
}
Float CPfVehicleLevel::m_vehicleLevelFR() const
{
	return m_m_vehicleLevelFR;
}
Float& CPfVehicleLevel::m_vehicleLevelFR()
{
	return m_m_vehicleLevelFR;
}

void CPfVehicleLevel::m_vehicleLevelRL(Float const _m_vehicleLevelRL)
{
	m_m_vehicleLevelRL = _m_vehicleLevelRL;
}
Float CPfVehicleLevel::m_vehicleLevelRL() const
{
	return m_m_vehicleLevelRL;
}
Float& CPfVehicleLevel::m_vehicleLevelRL()
{
	return m_m_vehicleLevelRL;
}

void CPfVehicleLevel::m_vehicleLevelRR(Float const _m_vehicleLevelRR)
{
	m_m_vehicleLevelRR = _m_vehicleLevelRR;
}
Float CPfVehicleLevel::m_vehicleLevelRR() const
{
	return m_m_vehicleLevelRR;
}
Float& CPfVehicleLevel::m_vehicleLevelRR()
{
	return m_m_vehicleLevelRR;
}

void CPfVehicleLevel::m_qualifierVehicleLevel(UInt32 const _m_qualifierVehicleLevel)
{
	m_m_qualifierVehicleLevel = _m_qualifierVehicleLevel;
}
UInt32 CPfVehicleLevel::m_qualifierVehicleLevel() const
{
	return m_m_qualifierVehicleLevel;
}
UInt32& CPfVehicleLevel::m_qualifierVehicleLevel()
{
	return m_m_qualifierVehicleLevel;
}

ValInOutputCpj::ValInOutputCpj()
{

}

magna::dds::DdsCdr& ValInOutputCpj::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_IHU);
	cdr.serialize(m_m_TEL);
	cdr.serialize(m_m_Eps);
	cdr.serialize(m_m_LongCtrl);
	cdr.serialize(m_m_rctaWarning);
	cdr.serialize(m_m_fctaWarning);
	cdr.serialize(m_m_vehInfo);
	cdr.serialize(m_m_FusaInfo);
	cdr.serialize(m_m_TDCU);
	cdr.serialize(m_m_LDCU);
	cdr.serialize(m_m_FCU);
	cdr.serialize(m_m_IPB);
	cdr.serialize(m_m_LA1);
	cdr.serialize(m_m_LA2);
	cdr.serialize(m_m_RDCU);
	cdr.serialize(m_m_RWS);
	cdr.serialize(m_m_SRS);
	cdr.serialize(m_m_TBOX);
	cdr.serialize(m_m_XPU);
	cdr.serialize(m_m_pfGnssLocalization);
	cdr.serialize(m_m_pfGnssDateTime);
	cdr.serialize(m_m_senderCallTimestamp);

	return cdr;
}
uint32_t ValInOutputCpj::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& ValInOutputCpj::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_IHU);
	cdr.deserialize(m_m_TEL);
	cdr.deserialize(m_m_Eps);
	cdr.deserialize(m_m_LongCtrl);
	cdr.deserialize(m_m_rctaWarning);
	cdr.deserialize(m_m_fctaWarning);
	cdr.deserialize(m_m_vehInfo);
	cdr.deserialize(m_m_FusaInfo);
	cdr.deserialize(m_m_TDCU);
	cdr.deserialize(m_m_LDCU);
	cdr.deserialize(m_m_FCU);
	cdr.deserialize(m_m_IPB);
	cdr.deserialize(m_m_LA1);
	cdr.deserialize(m_m_LA2);
	cdr.deserialize(m_m_RDCU);
	cdr.deserialize(m_m_RWS);
	cdr.deserialize(m_m_SRS);
	cdr.deserialize(m_m_TBOX);
	cdr.deserialize(m_m_XPU);
	cdr.deserialize(m_m_pfGnssLocalization);
	cdr.deserialize(m_m_pfGnssDateTime);
	cdr.deserialize(m_m_senderCallTimestamp);

	return cdr;
}
bool ValInOutputCpj::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool ValInOutputCpj::is_key_defined()
{
	return false;

}
void ValInOutputCpj::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void ValInOutputCpj::serialize_key(char **buf,unsigned int *len)
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
bool ValInOutputCpj::is_key_serialize_by_cdr()
{
	return false;

}
bool ValInOutputCpj::is_plain_types()
{
	bool b0 = CCpjIHU::is_plain_types();
	bool b1 = CCpjTel::is_plain_types();
	bool b2 = CCpjEps::is_plain_types();
	bool b3 = CCpjLongCtrl::is_plain_types();
	bool b4 = CCpjRctaWarning::is_plain_types();
	bool b5 = CCpjFctaWarning::is_plain_types();
	bool b6 = CCpjVehInfo::is_plain_types();
	bool b7 = CCpjFusaInfo::is_plain_types();
	bool b8 = CCpjTDCU::is_plain_types();
	bool b9 = CCpjLDCU::is_plain_types();
	bool b10 = CCPJFCU::is_plain_types();
	bool b11 = CCPJIPB::is_plain_types();
	bool b12 = CCPJLA1::is_plain_types();
	bool b13 = CCPJLA2::is_plain_types();
	bool b14 = CCPJRDCU::is_plain_types();
	bool b15 = CCPJRWS::is_plain_types();
	bool b16 = CCPJSRS::is_plain_types();
	bool b17 = CCPJTBOX::is_plain_types();
	bool b18 = CCPJXPU::is_plain_types();
	bool b19 = CPfGnssLocalization::is_plain_types();
	bool b20 = CPfGnssDateTime::is_plain_types();

	return b0 && b1 && b2 && b3 && b4 && b5 && b6 && b7 && b8 && b9 && b10 && b11 && b12 && b13 && b14 && b15 && b16 && b17 && b18 && b19 && b20;
}
uint32_t ValInOutputCpj::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void ValInOutputCpj::set_key_val(ValInOutputCpj const* const _data) noexcept
{

}
void ValInOutputCpj::m_IHU(CCpjIHU const &_m_IHU)
{
	m_m_IHU = _m_IHU;
}
void ValInOutputCpj::m_IHU(CCpjIHU &&_m_IHU)
{
	m_m_IHU = std::move(_m_IHU);
}
CCpjIHU const& ValInOutputCpj::m_IHU() const
{
	return m_m_IHU;
}
CCpjIHU& ValInOutputCpj::m_IHU()
{
	return m_m_IHU;
}

void ValInOutputCpj::m_TEL(CCpjTel const &_m_TEL)
{
	m_m_TEL = _m_TEL;
}
void ValInOutputCpj::m_TEL(CCpjTel &&_m_TEL)
{
	m_m_TEL = std::move(_m_TEL);
}
CCpjTel const& ValInOutputCpj::m_TEL() const
{
	return m_m_TEL;
}
CCpjTel& ValInOutputCpj::m_TEL()
{
	return m_m_TEL;
}

void ValInOutputCpj::m_Eps(CCpjEps const &_m_Eps)
{
	m_m_Eps = _m_Eps;
}
void ValInOutputCpj::m_Eps(CCpjEps &&_m_Eps)
{
	m_m_Eps = std::move(_m_Eps);
}
CCpjEps const& ValInOutputCpj::m_Eps() const
{
	return m_m_Eps;
}
CCpjEps& ValInOutputCpj::m_Eps()
{
	return m_m_Eps;
}

void ValInOutputCpj::m_LongCtrl(CCpjLongCtrl const &_m_LongCtrl)
{
	m_m_LongCtrl = _m_LongCtrl;
}
void ValInOutputCpj::m_LongCtrl(CCpjLongCtrl &&_m_LongCtrl)
{
	m_m_LongCtrl = std::move(_m_LongCtrl);
}
CCpjLongCtrl const& ValInOutputCpj::m_LongCtrl() const
{
	return m_m_LongCtrl;
}
CCpjLongCtrl& ValInOutputCpj::m_LongCtrl()
{
	return m_m_LongCtrl;
}

void ValInOutputCpj::m_rctaWarning(CCpjRctaWarning const &_m_rctaWarning)
{
	m_m_rctaWarning = _m_rctaWarning;
}
void ValInOutputCpj::m_rctaWarning(CCpjRctaWarning &&_m_rctaWarning)
{
	m_m_rctaWarning = std::move(_m_rctaWarning);
}
CCpjRctaWarning const& ValInOutputCpj::m_rctaWarning() const
{
	return m_m_rctaWarning;
}
CCpjRctaWarning& ValInOutputCpj::m_rctaWarning()
{
	return m_m_rctaWarning;
}

void ValInOutputCpj::m_fctaWarning(CCpjFctaWarning const &_m_fctaWarning)
{
	m_m_fctaWarning = _m_fctaWarning;
}
void ValInOutputCpj::m_fctaWarning(CCpjFctaWarning &&_m_fctaWarning)
{
	m_m_fctaWarning = std::move(_m_fctaWarning);
}
CCpjFctaWarning const& ValInOutputCpj::m_fctaWarning() const
{
	return m_m_fctaWarning;
}
CCpjFctaWarning& ValInOutputCpj::m_fctaWarning()
{
	return m_m_fctaWarning;
}

void ValInOutputCpj::m_vehInfo(CCpjVehInfo const &_m_vehInfo)
{
	m_m_vehInfo = _m_vehInfo;
}
void ValInOutputCpj::m_vehInfo(CCpjVehInfo &&_m_vehInfo)
{
	m_m_vehInfo = std::move(_m_vehInfo);
}
CCpjVehInfo const& ValInOutputCpj::m_vehInfo() const
{
	return m_m_vehInfo;
}
CCpjVehInfo& ValInOutputCpj::m_vehInfo()
{
	return m_m_vehInfo;
}

void ValInOutputCpj::m_FusaInfo(CCpjFusaInfo const &_m_FusaInfo)
{
	m_m_FusaInfo = _m_FusaInfo;
}
void ValInOutputCpj::m_FusaInfo(CCpjFusaInfo &&_m_FusaInfo)
{
	m_m_FusaInfo = std::move(_m_FusaInfo);
}
CCpjFusaInfo const& ValInOutputCpj::m_FusaInfo() const
{
	return m_m_FusaInfo;
}
CCpjFusaInfo& ValInOutputCpj::m_FusaInfo()
{
	return m_m_FusaInfo;
}

void ValInOutputCpj::m_TDCU(CCpjTDCU const &_m_TDCU)
{
	m_m_TDCU = _m_TDCU;
}
void ValInOutputCpj::m_TDCU(CCpjTDCU &&_m_TDCU)
{
	m_m_TDCU = std::move(_m_TDCU);
}
CCpjTDCU const& ValInOutputCpj::m_TDCU() const
{
	return m_m_TDCU;
}
CCpjTDCU& ValInOutputCpj::m_TDCU()
{
	return m_m_TDCU;
}

void ValInOutputCpj::m_LDCU(CCpjLDCU const &_m_LDCU)
{
	m_m_LDCU = _m_LDCU;
}
void ValInOutputCpj::m_LDCU(CCpjLDCU &&_m_LDCU)
{
	m_m_LDCU = std::move(_m_LDCU);
}
CCpjLDCU const& ValInOutputCpj::m_LDCU() const
{
	return m_m_LDCU;
}
CCpjLDCU& ValInOutputCpj::m_LDCU()
{
	return m_m_LDCU;
}

void ValInOutputCpj::m_FCU(CCPJFCU const &_m_FCU)
{
	m_m_FCU = _m_FCU;
}
void ValInOutputCpj::m_FCU(CCPJFCU &&_m_FCU)
{
	m_m_FCU = std::move(_m_FCU);
}
CCPJFCU const& ValInOutputCpj::m_FCU() const
{
	return m_m_FCU;
}
CCPJFCU& ValInOutputCpj::m_FCU()
{
	return m_m_FCU;
}

void ValInOutputCpj::m_IPB(CCPJIPB const &_m_IPB)
{
	m_m_IPB = _m_IPB;
}
void ValInOutputCpj::m_IPB(CCPJIPB &&_m_IPB)
{
	m_m_IPB = std::move(_m_IPB);
}
CCPJIPB const& ValInOutputCpj::m_IPB() const
{
	return m_m_IPB;
}
CCPJIPB& ValInOutputCpj::m_IPB()
{
	return m_m_IPB;
}

void ValInOutputCpj::m_LA1(CCPJLA1 const &_m_LA1)
{
	m_m_LA1 = _m_LA1;
}
void ValInOutputCpj::m_LA1(CCPJLA1 &&_m_LA1)
{
	m_m_LA1 = std::move(_m_LA1);
}
CCPJLA1 const& ValInOutputCpj::m_LA1() const
{
	return m_m_LA1;
}
CCPJLA1& ValInOutputCpj::m_LA1()
{
	return m_m_LA1;
}

void ValInOutputCpj::m_LA2(CCPJLA2 const &_m_LA2)
{
	m_m_LA2 = _m_LA2;
}
void ValInOutputCpj::m_LA2(CCPJLA2 &&_m_LA2)
{
	m_m_LA2 = std::move(_m_LA2);
}
CCPJLA2 const& ValInOutputCpj::m_LA2() const
{
	return m_m_LA2;
}
CCPJLA2& ValInOutputCpj::m_LA2()
{
	return m_m_LA2;
}

void ValInOutputCpj::m_RDCU(CCPJRDCU const &_m_RDCU)
{
	m_m_RDCU = _m_RDCU;
}
void ValInOutputCpj::m_RDCU(CCPJRDCU &&_m_RDCU)
{
	m_m_RDCU = std::move(_m_RDCU);
}
CCPJRDCU const& ValInOutputCpj::m_RDCU() const
{
	return m_m_RDCU;
}
CCPJRDCU& ValInOutputCpj::m_RDCU()
{
	return m_m_RDCU;
}

void ValInOutputCpj::m_RWS(CCPJRWS const &_m_RWS)
{
	m_m_RWS = _m_RWS;
}
void ValInOutputCpj::m_RWS(CCPJRWS &&_m_RWS)
{
	m_m_RWS = std::move(_m_RWS);
}
CCPJRWS const& ValInOutputCpj::m_RWS() const
{
	return m_m_RWS;
}
CCPJRWS& ValInOutputCpj::m_RWS()
{
	return m_m_RWS;
}

void ValInOutputCpj::m_SRS(CCPJSRS const &_m_SRS)
{
	m_m_SRS = _m_SRS;
}
void ValInOutputCpj::m_SRS(CCPJSRS &&_m_SRS)
{
	m_m_SRS = std::move(_m_SRS);
}
CCPJSRS const& ValInOutputCpj::m_SRS() const
{
	return m_m_SRS;
}
CCPJSRS& ValInOutputCpj::m_SRS()
{
	return m_m_SRS;
}

void ValInOutputCpj::m_TBOX(CCPJTBOX const &_m_TBOX)
{
	m_m_TBOX = _m_TBOX;
}
void ValInOutputCpj::m_TBOX(CCPJTBOX &&_m_TBOX)
{
	m_m_TBOX = std::move(_m_TBOX);
}
CCPJTBOX const& ValInOutputCpj::m_TBOX() const
{
	return m_m_TBOX;
}
CCPJTBOX& ValInOutputCpj::m_TBOX()
{
	return m_m_TBOX;
}

void ValInOutputCpj::m_XPU(CCPJXPU const &_m_XPU)
{
	m_m_XPU = _m_XPU;
}
void ValInOutputCpj::m_XPU(CCPJXPU &&_m_XPU)
{
	m_m_XPU = std::move(_m_XPU);
}
CCPJXPU const& ValInOutputCpj::m_XPU() const
{
	return m_m_XPU;
}
CCPJXPU& ValInOutputCpj::m_XPU()
{
	return m_m_XPU;
}

void ValInOutputCpj::m_pfGnssLocalization(CPfGnssLocalization const &_m_pfGnssLocalization)
{
	m_m_pfGnssLocalization = _m_pfGnssLocalization;
}
void ValInOutputCpj::m_pfGnssLocalization(CPfGnssLocalization &&_m_pfGnssLocalization)
{
	m_m_pfGnssLocalization = std::move(_m_pfGnssLocalization);
}
CPfGnssLocalization const& ValInOutputCpj::m_pfGnssLocalization() const
{
	return m_m_pfGnssLocalization;
}
CPfGnssLocalization& ValInOutputCpj::m_pfGnssLocalization()
{
	return m_m_pfGnssLocalization;
}

void ValInOutputCpj::m_pfGnssDateTime(CPfGnssDateTime const &_m_pfGnssDateTime)
{
	m_m_pfGnssDateTime = _m_pfGnssDateTime;
}
void ValInOutputCpj::m_pfGnssDateTime(CPfGnssDateTime &&_m_pfGnssDateTime)
{
	m_m_pfGnssDateTime = std::move(_m_pfGnssDateTime);
}
CPfGnssDateTime const& ValInOutputCpj::m_pfGnssDateTime() const
{
	return m_m_pfGnssDateTime;
}
CPfGnssDateTime& ValInOutputCpj::m_pfGnssDateTime()
{
	return m_m_pfGnssDateTime;
}

void ValInOutputCpj::m_senderCallTimestamp(UInt64 const _m_senderCallTimestamp)
{
	m_m_senderCallTimestamp = _m_senderCallTimestamp;
}
UInt64 ValInOutputCpj::m_senderCallTimestamp() const
{
	return m_m_senderCallTimestamp;
}
UInt64& ValInOutputCpj::m_senderCallTimestamp()
{
	return m_m_senderCallTimestamp;
}

ValInOutputPf::ValInOutputPf()
{

}

magna::dds::DdsCdr& ValInOutputPf::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_m_pfGear);
	cdr.serialize(m_m_pfCamera);
	cdr.serialize(m_m_pfDoorAndMirror);
	cdr.serialize(m_m_pfOdometry);
	cdr.serialize(m_m_pfPmaFixedPoint);
	cdr.serialize(m_m_pfSteering);
	cdr.serialize(m_m_pfTrailer);
	cdr.serialize(m_m_pfVehicleLevel);
	cdr.serialize(m_m_pfVehicleInfo);
	cdr.serialize(m_m_pfTirePress);
	cdr.serialize(m_m_pfBrakes);
	cdr.serialize(m_m_pfEnvironment);
	cdr.serialize(m_m_pfHil);
	cdr.serialize(m_m_senderCallTimestamp);
	cdr.serialize(m_m_timeStampVehicleUtcUs);

	return cdr;
}
uint32_t ValInOutputPf::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& ValInOutputPf::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_m_pfGear);
	cdr.deserialize(m_m_pfCamera);
	cdr.deserialize(m_m_pfDoorAndMirror);
	cdr.deserialize(m_m_pfOdometry);
	cdr.deserialize(m_m_pfPmaFixedPoint);
	cdr.deserialize(m_m_pfSteering);
	cdr.deserialize(m_m_pfTrailer);
	cdr.deserialize(m_m_pfVehicleLevel);
	cdr.deserialize(m_m_pfVehicleInfo);
	cdr.deserialize(m_m_pfTirePress);
	cdr.deserialize(m_m_pfBrakes);
	cdr.deserialize(m_m_pfEnvironment);
	cdr.deserialize(m_m_pfHil);
	cdr.deserialize(m_m_senderCallTimestamp);
	cdr.deserialize(m_m_timeStampVehicleUtcUs);

	return cdr;
}
bool ValInOutputPf::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool ValInOutputPf::is_key_defined()
{
	return false;

}
void ValInOutputPf::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void ValInOutputPf::serialize_key(char **buf,unsigned int *len)
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
bool ValInOutputPf::is_key_serialize_by_cdr()
{
	return false;

}
bool ValInOutputPf::is_plain_types()
{
	bool b0 = CPfGear::is_plain_types();
	bool b1 = CPfCamera::is_plain_types();
	bool b2 = CPfDoorAndMirror::is_plain_types();
	bool b3 = CPfOdometry::is_plain_types();
	bool b4 = CPfPmaFixedPoint::is_plain_types();
	bool b5 = CPfSteering::is_plain_types();
	bool b6 = CPfTrailer::is_plain_types();
	bool b7 = CPfVehicleLevel::is_plain_types();
	bool b8 = CPfVehicleInfo::is_plain_types();
	bool b9 = CPfTirePress::is_plain_types();
	bool b10 = CPfBrakes::is_plain_types();
	bool b11 = CPfEnvironment::is_plain_types();
	bool b12 = CPfHil::is_plain_types();

	return b0 && b1 && b2 && b3 && b4 && b5 && b6 && b7 && b8 && b9 && b10 && b11 && b12;
}
uint32_t ValInOutputPf::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void ValInOutputPf::set_key_val(ValInOutputPf const* const _data) noexcept
{

}
void ValInOutputPf::m_pfGear(CPfGear const &_m_pfGear)
{
	m_m_pfGear = _m_pfGear;
}
void ValInOutputPf::m_pfGear(CPfGear &&_m_pfGear)
{
	m_m_pfGear = std::move(_m_pfGear);
}
CPfGear const& ValInOutputPf::m_pfGear() const
{
	return m_m_pfGear;
}
CPfGear& ValInOutputPf::m_pfGear()
{
	return m_m_pfGear;
}

void ValInOutputPf::m_pfCamera(CPfCamera const &_m_pfCamera)
{
	m_m_pfCamera = _m_pfCamera;
}
void ValInOutputPf::m_pfCamera(CPfCamera &&_m_pfCamera)
{
	m_m_pfCamera = std::move(_m_pfCamera);
}
CPfCamera const& ValInOutputPf::m_pfCamera() const
{
	return m_m_pfCamera;
}
CPfCamera& ValInOutputPf::m_pfCamera()
{
	return m_m_pfCamera;
}

void ValInOutputPf::m_pfDoorAndMirror(CPfDoorAndMirror const &_m_pfDoorAndMirror)
{
	m_m_pfDoorAndMirror = _m_pfDoorAndMirror;
}
void ValInOutputPf::m_pfDoorAndMirror(CPfDoorAndMirror &&_m_pfDoorAndMirror)
{
	m_m_pfDoorAndMirror = std::move(_m_pfDoorAndMirror);
}
CPfDoorAndMirror const& ValInOutputPf::m_pfDoorAndMirror() const
{
	return m_m_pfDoorAndMirror;
}
CPfDoorAndMirror& ValInOutputPf::m_pfDoorAndMirror()
{
	return m_m_pfDoorAndMirror;
}

void ValInOutputPf::m_pfOdometry(CPfOdometry const &_m_pfOdometry)
{
	m_m_pfOdometry = _m_pfOdometry;
}
void ValInOutputPf::m_pfOdometry(CPfOdometry &&_m_pfOdometry)
{
	m_m_pfOdometry = std::move(_m_pfOdometry);
}
CPfOdometry const& ValInOutputPf::m_pfOdometry() const
{
	return m_m_pfOdometry;
}
CPfOdometry& ValInOutputPf::m_pfOdometry()
{
	return m_m_pfOdometry;
}

void ValInOutputPf::m_pfPmaFixedPoint(CPfPmaFixedPoint const &_m_pfPmaFixedPoint)
{
	m_m_pfPmaFixedPoint = _m_pfPmaFixedPoint;
}
void ValInOutputPf::m_pfPmaFixedPoint(CPfPmaFixedPoint &&_m_pfPmaFixedPoint)
{
	m_m_pfPmaFixedPoint = std::move(_m_pfPmaFixedPoint);
}
CPfPmaFixedPoint const& ValInOutputPf::m_pfPmaFixedPoint() const
{
	return m_m_pfPmaFixedPoint;
}
CPfPmaFixedPoint& ValInOutputPf::m_pfPmaFixedPoint()
{
	return m_m_pfPmaFixedPoint;
}

void ValInOutputPf::m_pfSteering(CPfSteering const &_m_pfSteering)
{
	m_m_pfSteering = _m_pfSteering;
}
void ValInOutputPf::m_pfSteering(CPfSteering &&_m_pfSteering)
{
	m_m_pfSteering = std::move(_m_pfSteering);
}
CPfSteering const& ValInOutputPf::m_pfSteering() const
{
	return m_m_pfSteering;
}
CPfSteering& ValInOutputPf::m_pfSteering()
{
	return m_m_pfSteering;
}

void ValInOutputPf::m_pfTrailer(CPfTrailer const &_m_pfTrailer)
{
	m_m_pfTrailer = _m_pfTrailer;
}
void ValInOutputPf::m_pfTrailer(CPfTrailer &&_m_pfTrailer)
{
	m_m_pfTrailer = std::move(_m_pfTrailer);
}
CPfTrailer const& ValInOutputPf::m_pfTrailer() const
{
	return m_m_pfTrailer;
}
CPfTrailer& ValInOutputPf::m_pfTrailer()
{
	return m_m_pfTrailer;
}

void ValInOutputPf::m_pfVehicleLevel(CPfVehicleLevel const &_m_pfVehicleLevel)
{
	m_m_pfVehicleLevel = _m_pfVehicleLevel;
}
void ValInOutputPf::m_pfVehicleLevel(CPfVehicleLevel &&_m_pfVehicleLevel)
{
	m_m_pfVehicleLevel = std::move(_m_pfVehicleLevel);
}
CPfVehicleLevel const& ValInOutputPf::m_pfVehicleLevel() const
{
	return m_m_pfVehicleLevel;
}
CPfVehicleLevel& ValInOutputPf::m_pfVehicleLevel()
{
	return m_m_pfVehicleLevel;
}

void ValInOutputPf::m_pfVehicleInfo(CPfVehicleInfo const &_m_pfVehicleInfo)
{
	m_m_pfVehicleInfo = _m_pfVehicleInfo;
}
void ValInOutputPf::m_pfVehicleInfo(CPfVehicleInfo &&_m_pfVehicleInfo)
{
	m_m_pfVehicleInfo = std::move(_m_pfVehicleInfo);
}
CPfVehicleInfo const& ValInOutputPf::m_pfVehicleInfo() const
{
	return m_m_pfVehicleInfo;
}
CPfVehicleInfo& ValInOutputPf::m_pfVehicleInfo()
{
	return m_m_pfVehicleInfo;
}

void ValInOutputPf::m_pfTirePress(CPfTirePress const &_m_pfTirePress)
{
	m_m_pfTirePress = _m_pfTirePress;
}
void ValInOutputPf::m_pfTirePress(CPfTirePress &&_m_pfTirePress)
{
	m_m_pfTirePress = std::move(_m_pfTirePress);
}
CPfTirePress const& ValInOutputPf::m_pfTirePress() const
{
	return m_m_pfTirePress;
}
CPfTirePress& ValInOutputPf::m_pfTirePress()
{
	return m_m_pfTirePress;
}

void ValInOutputPf::m_pfBrakes(CPfBrakes const &_m_pfBrakes)
{
	m_m_pfBrakes = _m_pfBrakes;
}
void ValInOutputPf::m_pfBrakes(CPfBrakes &&_m_pfBrakes)
{
	m_m_pfBrakes = std::move(_m_pfBrakes);
}
CPfBrakes const& ValInOutputPf::m_pfBrakes() const
{
	return m_m_pfBrakes;
}
CPfBrakes& ValInOutputPf::m_pfBrakes()
{
	return m_m_pfBrakes;
}

void ValInOutputPf::m_pfEnvironment(CPfEnvironment const &_m_pfEnvironment)
{
	m_m_pfEnvironment = _m_pfEnvironment;
}
void ValInOutputPf::m_pfEnvironment(CPfEnvironment &&_m_pfEnvironment)
{
	m_m_pfEnvironment = std::move(_m_pfEnvironment);
}
CPfEnvironment const& ValInOutputPf::m_pfEnvironment() const
{
	return m_m_pfEnvironment;
}
CPfEnvironment& ValInOutputPf::m_pfEnvironment()
{
	return m_m_pfEnvironment;
}

void ValInOutputPf::m_pfHil(CPfHil const &_m_pfHil)
{
	m_m_pfHil = _m_pfHil;
}
void ValInOutputPf::m_pfHil(CPfHil &&_m_pfHil)
{
	m_m_pfHil = std::move(_m_pfHil);
}
CPfHil const& ValInOutputPf::m_pfHil() const
{
	return m_m_pfHil;
}
CPfHil& ValInOutputPf::m_pfHil()
{
	return m_m_pfHil;
}

void ValInOutputPf::m_senderCallTimestamp(UInt64 const _m_senderCallTimestamp)
{
	m_m_senderCallTimestamp = _m_senderCallTimestamp;
}
UInt64 ValInOutputPf::m_senderCallTimestamp() const
{
	return m_m_senderCallTimestamp;
}
UInt64& ValInOutputPf::m_senderCallTimestamp()
{
	return m_m_senderCallTimestamp;
}

void ValInOutputPf::m_timeStampVehicleUtcUs(UInt64 const _m_timeStampVehicleUtcUs)
{
	m_m_timeStampVehicleUtcUs = _m_timeStampVehicleUtcUs;
}
UInt64 ValInOutputPf::m_timeStampVehicleUtcUs() const
{
	return m_m_timeStampVehicleUtcUs;
}
UInt64& ValInOutputPf::m_timeStampVehicleUtcUs()
{
	return m_m_timeStampVehicleUtcUs;
}

PrkVinBus::PrkVinBus()
{

}

magna::dds::DdsCdr& PrkVinBus::serialize(magna::dds::DdsCdr &cdr) const
{
	cdr.serialize(m_ValInOutputPf);
	cdr.serialize(m_ValIOutPutCpj);

	return cdr;
}
uint32_t PrkVinBus::serialize(void *const /*data*/, char *const /*payload_buf*/, uint32_t const /*payload_len*/)
{
	return 0U;
}

magna::dds::DdsCdr& PrkVinBus::deserialize(magna::dds::DdsCdr &cdr)
{
	cdr.deserialize(m_ValInOutputPf);
	cdr.deserialize(m_ValIOutPutCpj);

	return cdr;
}
bool PrkVinBus::deserialize(char *const /*payload_buf*/, uint32_t const /*payload_len*/, void *const /*data*/)
{
	return false;
}

bool PrkVinBus::is_key_defined()
{
	return false;

}
void PrkVinBus::serialize_key(magna::dds::DdsCdr &cdr) const
{

}
void PrkVinBus::serialize_key(char **buf,unsigned int *len)
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
bool PrkVinBus::is_key_serialize_by_cdr()
{
	return false;

}
bool PrkVinBus::is_plain_types()
{
	bool b0 = PrkVinBusValInOutputPf::is_plain_types();
	bool b1 = PrkVinBusValInOutputCpj::is_plain_types();

	return b0 && b1;
}
uint32_t PrkVinBus::max_align_size(uint32_t const _cur_al) const
{
	return _cur_al + DATA_SIZE;

}
void PrkVinBus::set_key_val(PrkVinBus const* const _data) noexcept
{

}
void PrkVinBus::ValInOutputPf(PrkVinBusValInOutputPf const &_ValInOutputPf)
{
	m_ValInOutputPf = _ValInOutputPf;
}
void PrkVinBus::ValInOutputPf(PrkVinBusValInOutputPf &&_ValInOutputPf)
{
	m_ValInOutputPf = std::move(_ValInOutputPf);
}
PrkVinBusValInOutputPf const& PrkVinBus::ValInOutputPf() const
{
	return m_ValInOutputPf;
}
PrkVinBusValInOutputPf& PrkVinBus::ValInOutputPf()
{
	return m_ValInOutputPf;
}

void PrkVinBus::ValIOutPutCpj(PrkVinBusValInOutputCpj const &_ValIOutPutCpj)
{
	m_ValIOutPutCpj = _ValIOutPutCpj;
}
void PrkVinBus::ValIOutPutCpj(PrkVinBusValInOutputCpj &&_ValIOutPutCpj)
{
	m_ValIOutPutCpj = std::move(_ValIOutPutCpj);
}
PrkVinBusValInOutputCpj const& PrkVinBus::ValIOutPutCpj() const
{
	return m_ValIOutPutCpj;
}
PrkVinBusValInOutputCpj& PrkVinBus::ValIOutPutCpj()
{
	return m_ValIOutPutCpj;
}

