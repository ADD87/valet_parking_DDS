/**************************************************************
* @file prk_vin_bus.h
* @copyright GREENSTONE TECHNOLOGY CO.,LTD. 2020-2025
* All rights reserved
**************************************************************/

#ifndef PRK_VIN_BUS_fb6b73c9bc8457b0f02e247b29800f92_H
#define PRK_VIN_BUS_fb6b73c9bc8457b0f02e247b29800f92_H

#include <stdint.h>
#include <vector>
#include <array>
#include <map>
#include <string>
#include "magnadds/MagnaDDS.h"



typedef bool Boolean;
typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef uint32_t UInt32;
typedef uint64_t UInt64;
typedef int8_t SInt8;
typedef short SInt16;
typedef int32_t SInt32;
typedef float Float;

/**
* @class EIdcServiceResp_st
* @brief A class as the datatype for data exchange.
* @note
*/

class EIdcServiceResp_st
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(std::array<UInt16,3>);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	EIdcServiceResp_st();
	~EIdcServiceResp_st() = default;
	EIdcServiceResp_st(EIdcServiceResp_st const &x) = default;
	EIdcServiceResp_st(EIdcServiceResp_st &&x) = default;
	EIdcServiceResp_st& operator=(EIdcServiceResp_st const &x) = default;
	EIdcServiceResp_st& operator=(EIdcServiceResp_st &&x) = default;

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
	void set_key_val(EIdcServiceResp_st const* const _data) noexcept;



	void control_response(std::array<UInt16,3> const &_control_response);
	void control_response(std::array<UInt16,3> &&_control_response);
	std::array<UInt16,3> const& control_response() const;
	std::array<UInt16,3>& control_response();





private:
	std::array<UInt16,3> m_control_response;

};


/**
* @class CCPJFCU
* @brief A class as the datatype for data exchange.
* @note
*/

class CCPJFCU
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(SInt16) + sizeof(SInt16) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCPJFCU();
	~CCPJFCU() = default;
	CCPJFCU(CCPJFCU const &x) = default;
	CCPJFCU(CCPJFCU &&x) = default;
	CCPJFCU& operator=(CCPJFCU const &x) = default;
	CCPJFCU& operator=(CCPJFCU &&x) = default;

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
	void set_key_val(CCPJFCU const* const _data) noexcept;



	void m_Ob5Weekseconds(SInt16 const _m_Ob5Weekseconds);
	SInt16 m_Ob5Weekseconds() const;
	SInt16& m_Ob5Weekseconds();

	void padding(SInt16 const _padding);
	SInt16 padding() const;
	SInt16& padding();

	void m_Longitude(Float const _m_Longitude);
	Float m_Longitude() const;
	Float& m_Longitude();

	void m_Latitude(Float const _m_Latitude);
	Float m_Latitude() const;
	Float& m_Latitude();

	void m_Wgs84Hgt(Float const _m_Wgs84Hgt);
	Float m_Wgs84Hgt() const;
	Float& m_Wgs84Hgt();

	void m_MagYaw(Float const _m_MagYaw);
	Float m_MagYaw() const;
	Float& m_MagYaw();

	void m_VoteNavHorPosType(UInt32 const _m_VoteNavHorPosType);
	UInt32 m_VoteNavHorPosType() const;
	UInt32& m_VoteNavHorPosType();

	void m_NavpDevErr(UInt32 const _m_NavpDevErr);
	UInt32 m_NavpDevErr() const;
	UInt32& m_NavpDevErr();

	void m_NavsDevErr(UInt32 const _m_NavsDevErr);
	UInt32 m_NavsDevErr() const;
	UInt32& m_NavsDevErr();

	void m_AhrsDevErr(UInt32 const _m_AhrsDevErr);
	UInt32 m_AhrsDevErr() const;
	UInt32& m_AhrsDevErr();

	void m_Ob2Weekseconds(Float const _m_Ob2Weekseconds);
	Float m_Ob2Weekseconds() const;
	Float& m_Ob2Weekseconds();

	void m_Quaternion_q0(Float const _m_Quaternion_q0);
	Float m_Quaternion_q0() const;
	Float& m_Quaternion_q0();

	void m_Quaternion_q1(Float const _m_Quaternion_q1);
	Float m_Quaternion_q1() const;
	Float& m_Quaternion_q1();

	void m_Quaternion_q2(Float const _m_Quaternion_q2);
	Float m_Quaternion_q2() const;
	Float& m_Quaternion_q2();

	void m_Quaternion_q3(Float const _m_Quaternion_q3);
	Float m_Quaternion_q3() const;
	Float& m_Quaternion_q3();





private:
	SInt16 m_m_Ob5Weekseconds;
	SInt16 m_padding;
	Float m_m_Longitude;
	Float m_m_Latitude;
	Float m_m_Wgs84Hgt;
	Float m_m_MagYaw;
	UInt32 m_m_VoteNavHorPosType;
	UInt32 m_m_NavpDevErr;
	UInt32 m_m_NavsDevErr;
	UInt32 m_m_AhrsDevErr;
	Float m_m_Ob2Weekseconds;
	Float m_m_Quaternion_q0;
	Float m_m_Quaternion_q1;
	Float m_m_Quaternion_q2;
	Float m_m_Quaternion_q3;

};


/**
* @class CCPJIPB
* @brief A class as the datatype for data exchange.
* @note
*/

class CCPJIPB
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Float) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt16) + sizeof(UInt16) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Float) + sizeof(Float) + sizeof(UInt32) + sizeof(UInt16) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt16) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Float) + sizeof(Float) + sizeof(Float);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCPJIPB();
	~CCPJIPB() = default;
	CCPJIPB(CCPJIPB const &x) = default;
	CCPJIPB(CCPJIPB &&x) = default;
	CCPJIPB& operator=(CCPJIPB const &x) = default;
	CCPJIPB& operator=(CCPJIPB &&x) = default;

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
	void set_key_val(CCPJIPB const* const _data) noexcept;



	void m_IPB_VehVertAccel(Float const _m_IPB_VehVertAccel);
	Float m_IPB_VehVertAccel() const;
	Float& m_IPB_VehVertAccel();

	void m_IPB_XPU_ParkFailureSts(UInt32 const _m_IPB_XPU_ParkFailureSts);
	UInt32 m_IPB_XPU_ParkFailureSts() const;
	UInt32& m_IPB_XPU_ParkFailureSts();

	void m_IPB_VehVertAccelVD(Boolean const _m_IPB_VehVertAccelVD);
	Boolean m_IPB_VehVertAccelVD() const;
	Boolean& m_IPB_VehVertAccelVD();

	void m_IPB_XPU_ParkDrvBrkOverrideFlg(Boolean const _m_IPB_XPU_ParkDrvBrkOverrideFlg);
	Boolean m_IPB_XPU_ParkDrvBrkOverrideFlg() const;
	Boolean& m_IPB_XPU_ParkDrvBrkOverrideFlg();

	void m_IPB_MLWheelSpdEdgesSum(UInt16 const _m_IPB_MLWheelSpdEdgesSum);
	UInt16 m_IPB_MLWheelSpdEdgesSum() const;
	UInt16& m_IPB_MLWheelSpdEdgesSum();

	void m_IPB_MRWheelSpdEdgesSum(UInt16 const _m_IPB_MRWheelSpdEdgesSum);
	UInt16 m_IPB_MRWheelSpdEdgesSum() const;
	UInt16& m_IPB_MRWheelSpdEdgesSum();

	void m_IPB_MLWheelSpdVD_E2E(Boolean const _m_IPB_MLWheelSpdVD_E2E);
	Boolean m_IPB_MLWheelSpdVD_E2E() const;
	Boolean& m_IPB_MLWheelSpdVD_E2E();

	void m_IPB_MRWheelSpdVD_E2E(Boolean const _m_IPB_MRWheelSpdVD_E2E);
	Boolean m_IPB_MRWheelSpdVD_E2E() const;
	Boolean& m_IPB_MRWheelSpdVD_E2E();

	void m_IPB_MRWheelSpd_E2E(Float const _m_IPB_MRWheelSpd_E2E);
	Float m_IPB_MRWheelSpd_E2E() const;
	Float& m_IPB_MRWheelSpd_E2E();

	void m_IPB_MLWheelSpd_E2E(Float const _m_IPB_MLWheelSpd_E2E);
	Float m_IPB_MLWheelSpd_E2E() const;
	Float& m_IPB_MLWheelSpd_E2E();

	void m_IPB_StandStillSt(UInt32 const _m_IPB_StandStillSt);
	UInt32 m_IPB_StandStillSt() const;
	UInt32& m_IPB_StandStillSt();

	void m_IPB_CDD_Current_BrkTq(UInt16 const _m_IPB_CDD_Current_BrkTq);
	UInt16 m_IPB_CDD_Current_BrkTq() const;
	UInt16& m_IPB_CDD_Current_BrkTq();

	void m_IPB_ESPFault(Boolean const _m_IPB_ESPFault);
	Boolean m_IPB_ESPFault() const;
	Boolean& m_IPB_ESPFault();

	void m_IPB_CDD_Current_BrkTqVD(Boolean const _m_IPB_CDD_Current_BrkTqVD);
	Boolean m_IPB_CDD_Current_BrkTqVD() const;
	Boolean& m_IPB_CDD_Current_BrkTqVD();

	void m_IPB_BrkPedalStVD_E2E(Boolean const _m_IPB_BrkPedalStVD_E2E);
	Boolean m_IPB_BrkPedalStVD_E2E() const;
	Boolean& m_IPB_BrkPedalStVD_E2E();

	void padding(UInt8 const _padding);
	UInt8 padding() const;
	UInt8& padding();

	void padding1(UInt16 const _padding1);
	UInt16 padding1() const;
	UInt16& padding1();

	void m_IPB_APAActiveState_E2E(UInt32 const _m_IPB_APAActiveState_E2E);
	UInt32 m_IPB_APAActiveState_E2E() const;
	UInt32& m_IPB_APAActiveState_E2E();

	void m_IPB_MLWheelRotatedDirection(UInt32 const _m_IPB_MLWheelRotatedDirection);
	UInt32 m_IPB_MLWheelRotatedDirection() const;
	UInt32& m_IPB_MLWheelRotatedDirection();

	void m_IPB_MRWheelRotatedDirection(UInt32 const _m_IPB_MRWheelRotatedDirection);
	UInt32 m_IPB_MRWheelRotatedDirection() const;
	UInt32& m_IPB_MRWheelRotatedDirection();

	void m_IPB_PitchRate(Float const _m_IPB_PitchRate);
	Float m_IPB_PitchRate() const;
	Float& m_IPB_PitchRate();

	void m_IPB_RollRate(Float const _m_IPB_RollRate);
	Float m_IPB_RollRate() const;
	Float& m_IPB_RollRate();

	void m_IPB_YAWRate_E2E(Float const _m_IPB_YAWRate_E2E);
	Float m_IPB_YAWRate_E2E() const;
	Float& m_IPB_YAWRate_E2E();





private:
	Float m_m_IPB_VehVertAccel;
	UInt32 m_m_IPB_XPU_ParkFailureSts;
	Boolean m_m_IPB_VehVertAccelVD;
	Boolean m_m_IPB_XPU_ParkDrvBrkOverrideFlg;
	UInt16 m_m_IPB_MLWheelSpdEdgesSum;
	UInt16 m_m_IPB_MRWheelSpdEdgesSum;
	Boolean m_m_IPB_MLWheelSpdVD_E2E;
	Boolean m_m_IPB_MRWheelSpdVD_E2E;
	Float m_m_IPB_MRWheelSpd_E2E;
	Float m_m_IPB_MLWheelSpd_E2E;
	UInt32 m_m_IPB_StandStillSt;
	UInt16 m_m_IPB_CDD_Current_BrkTq;
	Boolean m_m_IPB_ESPFault;
	Boolean m_m_IPB_CDD_Current_BrkTqVD;
	Boolean m_m_IPB_BrkPedalStVD_E2E;
	UInt8 m_padding;
	UInt16 m_padding1;
	UInt32 m_m_IPB_APAActiveState_E2E;
	UInt32 m_m_IPB_MLWheelRotatedDirection;
	UInt32 m_m_IPB_MRWheelRotatedDirection;
	Float m_m_IPB_PitchRate;
	Float m_m_IPB_RollRate;
	Float m_m_IPB_YAWRate_E2E;

};


/**
* @class CCPJLA1
* @brief A class as the datatype for data exchange.
* @note
*/

class CCPJLA1
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt16) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCPJLA1();
	~CCPJLA1() = default;
	CCPJLA1(CCPJLA1 const &x) = default;
	CCPJLA1(CCPJLA1 &&x) = default;
	CCPJLA1& operator=(CCPJLA1 const &x) = default;
	CCPJLA1& operator=(CCPJLA1 &&x) = default;

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
	void set_key_val(CCPJLA1 const* const _data) noexcept;



	void m_LA1_Distance_mm(UInt16 const _m_LA1_Distance_mm);
	UInt16 m_LA1_Distance_mm() const;
	UInt16& m_LA1_Distance_mm();

	void m_LA1_Unstable(Boolean const _m_LA1_Unstable);
	Boolean m_LA1_Unstable() const;
	Boolean& m_LA1_Unstable();

	void padding(UInt8 const _padding);
	UInt8 padding() const;
	UInt8& padding();

	void m_LA1_ErrorState(UInt32 const _m_LA1_ErrorState);
	UInt32 m_LA1_ErrorState() const;
	UInt32& m_LA1_ErrorState();





private:
	UInt16 m_m_LA1_Distance_mm;
	Boolean m_m_LA1_Unstable;
	UInt8 m_padding;
	UInt32 m_m_LA1_ErrorState;

};


/**
* @class CCPJLA2
* @brief A class as the datatype for data exchange.
* @note
*/

class CCPJLA2
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt16) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCPJLA2();
	~CCPJLA2() = default;
	CCPJLA2(CCPJLA2 const &x) = default;
	CCPJLA2(CCPJLA2 &&x) = default;
	CCPJLA2& operator=(CCPJLA2 const &x) = default;
	CCPJLA2& operator=(CCPJLA2 &&x) = default;

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
	void set_key_val(CCPJLA2 const* const _data) noexcept;



	void m_LA2_Distance_mm(UInt16 const _m_LA2_Distance_mm);
	UInt16 m_LA2_Distance_mm() const;
	UInt16& m_LA2_Distance_mm();

	void m_LA2_Unstable(Boolean const _m_LA2_Unstable);
	Boolean m_LA2_Unstable() const;
	Boolean& m_LA2_Unstable();

	void padding(UInt8 const _padding);
	UInt8 padding() const;
	UInt8& padding();

	void m_LA2_ErrorState(UInt32 const _m_LA2_ErrorState);
	UInt32 m_LA2_ErrorState() const;
	UInt32& m_LA2_ErrorState();





private:
	UInt16 m_m_LA2_Distance_mm;
	Boolean m_m_LA2_Unstable;
	UInt8 m_padding;
	UInt32 m_m_LA2_ErrorState;

};


/**
* @class CCPJRDCU
* @brief A class as the datatype for data exchange.
* @note
*/

class CCPJRDCU
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt16) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt32) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCPJRDCU();
	~CCPJRDCU() = default;
	CCPJRDCU(CCPJRDCU const &x) = default;
	CCPJRDCU(CCPJRDCU &&x) = default;
	CCPJRDCU& operator=(CCPJRDCU const &x) = default;
	CCPJRDCU& operator=(CCPJRDCU &&x) = default;

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
	void set_key_val(CCPJRDCU const* const _data) noexcept;



	void m_RDCU_BonnetAjarSt(UInt32 const _m_RDCU_BonnetAjarSt);
	UInt32 m_RDCU_BonnetAjarSt() const;
	UInt32& m_RDCU_BonnetAjarSt();

	void m_RDCU_APBActSt(UInt32 const _m_RDCU_APBActSt);
	UInt32 m_RDCU_APBActSt() const;
	UInt32& m_RDCU_APBActSt();

	void m_RDCU_RLSBrightnessFW(UInt16 const _m_RDCU_RLSBrightnessFW);
	UInt16 m_RDCU_RLSBrightnessFW() const;
	UInt16& m_RDCU_RLSBrightnessFW();

	void m_RDCU_FliiLampOutputSt(Boolean const _m_RDCU_FliiLampOutputSt);
	Boolean m_RDCU_FliiLampOutputSt() const;
	Boolean& m_RDCU_FliiLampOutputSt();

	void m_RDCU_ChrgPortLockSt(Boolean const _m_RDCU_ChrgPortLockSt);
	Boolean m_RDCU_ChrgPortLockSt() const;
	Boolean& m_RDCU_ChrgPortLockSt();

	void m_RDCU_PsngrDoorLockOutput(UInt32 const _m_RDCU_PsngrDoorLockOutput);
	UInt32 m_RDCU_PsngrDoorLockOutput() const;
	UInt32& m_RDCU_PsngrDoorLockOutput();

	void m_RDCU_RRDoorLockOutput(UInt32 const _m_RDCU_RRDoorLockOutput);
	UInt32 m_RDCU_RRDoorLockOutput() const;
	UInt32& m_RDCU_RRDoorLockOutput();





private:
	UInt32 m_m_RDCU_BonnetAjarSt;
	UInt32 m_m_RDCU_APBActSt;
	UInt16 m_m_RDCU_RLSBrightnessFW;
	Boolean m_m_RDCU_FliiLampOutputSt;
	Boolean m_m_RDCU_ChrgPortLockSt;
	UInt32 m_m_RDCU_PsngrDoorLockOutput;
	UInt32 m_m_RDCU_RRDoorLockOutput;

};


/**
* @class CCPJRWS
* @brief A class as the datatype for data exchange.
* @note
*/

class CCPJRWS
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(Float);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCPJRWS();
	~CCPJRWS() = default;
	CCPJRWS(CCPJRWS const &x) = default;
	CCPJRWS(CCPJRWS &&x) = default;
	CCPJRWS& operator=(CCPJRWS const &x) = default;
	CCPJRWS& operator=(CCPJRWS &&x) = default;

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
	void set_key_val(CCPJRWS const* const _data) noexcept;



	void m_RWS_SysWarning(UInt32 const _m_RWS_SysWarning);
	UInt32 m_RWS_SysWarning() const;
	UInt32& m_RWS_SysWarning();

	void m_RWS_RLWheelangle(Float const _m_RWS_RLWheelangle);
	Float m_RWS_RLWheelangle() const;
	Float& m_RWS_RLWheelangle();





private:
	UInt32 m_m_RWS_SysWarning;
	Float m_m_RWS_RLWheelangle;

};


/**
* @class CCPJSRS
* @brief A class as the datatype for data exchange.
* @note
*/

class CCPJSRS
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Boolean);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCPJSRS();
	~CCPJSRS() = default;
	CCPJSRS(CCPJSRS const &x) = default;
	CCPJSRS(CCPJSRS &&x) = default;
	CCPJSRS& operator=(CCPJSRS const &x) = default;
	CCPJSRS& operator=(CCPJSRS &&x) = default;

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
	void set_key_val(CCPJSRS const* const _data) noexcept;



	void m_SRS_CrashOutputSt(Boolean const _m_SRS_CrashOutputSt);
	Boolean m_SRS_CrashOutputSt() const;
	Boolean& m_SRS_CrashOutputSt();





private:
	Boolean m_m_SRS_CrashOutputSt;

};


/**
* @class CCPJTBOX
* @brief A class as the datatype for data exchange.
* @note
*/

class CCPJTBOX
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(SInt32) + sizeof(SInt32) + sizeof(Float) + sizeof(Float) + sizeof(UInt16) + sizeof(SInt16) + sizeof(UInt32) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt16);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCPJTBOX();
	~CCPJTBOX() = default;
	CCPJTBOX(CCPJTBOX const &x) = default;
	CCPJTBOX(CCPJTBOX &&x) = default;
	CCPJTBOX& operator=(CCPJTBOX const &x) = default;
	CCPJTBOX& operator=(CCPJTBOX &&x) = default;

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
	void set_key_val(CCPJTBOX const* const _data) noexcept;



	void m_TBOX_GPSLatitude(SInt32 const _m_TBOX_GPSLatitude);
	SInt32 m_TBOX_GPSLatitude() const;
	SInt32& m_TBOX_GPSLatitude();

	void m_TBOX_GPSLongitude(SInt32 const _m_TBOX_GPSLongitude);
	SInt32 m_TBOX_GPSLongitude() const;
	SInt32& m_TBOX_GPSLongitude();

	void m_TBOX_GPSBearing(Float const _m_TBOX_GPSBearing);
	Float m_TBOX_GPSBearing() const;
	Float& m_TBOX_GPSBearing();

	void m_TBOX_GPSAltitude(Float const _m_TBOX_GPSAltitude);
	Float m_TBOX_GPSAltitude() const;
	Float& m_TBOX_GPSAltitude();

	void m_TBOX_GPSAccuracy(UInt16 const _m_TBOX_GPSAccuracy);
	UInt16 m_TBOX_GPSAccuracy() const;
	UInt16& m_TBOX_GPSAccuracy();

	void m_TBOX_GPSSpeed(SInt16 const _m_TBOX_GPSSpeed);
	SInt16 m_TBOX_GPSSpeed() const;
	SInt16& m_TBOX_GPSSpeed();

	void m_TBOX_GPSTime(UInt32 const _m_TBOX_GPSTime);
	UInt32 m_TBOX_GPSTime() const;
	UInt32& m_TBOX_GPSTime();

	void m_TBOX_FixFlag(Boolean const _m_TBOX_FixFlag);
	Boolean m_TBOX_FixFlag() const;
	Boolean& m_TBOX_FixFlag();

	void padding1(UInt8 const _padding1);
	UInt8 padding1() const;
	UInt8& padding1();

	void padding(UInt16 const _padding);
	UInt16 padding() const;
	UInt16& padding();





private:
	SInt32 m_m_TBOX_GPSLatitude;
	SInt32 m_m_TBOX_GPSLongitude;
	Float m_m_TBOX_GPSBearing;
	Float m_m_TBOX_GPSAltitude;
	UInt16 m_m_TBOX_GPSAccuracy;
	SInt16 m_m_TBOX_GPSSpeed;
	UInt32 m_m_TBOX_GPSTime;
	Boolean m_m_TBOX_FixFlag;
	UInt8 m_padding1;
	UInt16 m_padding;

};


/**
* @class CCPJXPU
* @brief A class as the datatype for data exchange.
* @note
*/

class CCPJXPU
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt16);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCPJXPU();
	~CCPJXPU() = default;
	CCPJXPU(CCPJXPU const &x) = default;
	CCPJXPU(CCPJXPU &&x) = default;
	CCPJXPU& operator=(CCPJXPU const &x) = default;
	CCPJXPU& operator=(CCPJXPU &&x) = default;

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
	void set_key_val(CCPJXPU const* const _data) noexcept;



	void m_XPU_ACCSt(UInt32 const _m_XPU_ACCSt);
	UInt32 m_XPU_ACCSt() const;
	UInt32& m_XPU_ACCSt();

	void m_XPU_IPB_ABAReq(Boolean const _m_XPU_IPB_ABAReq);
	Boolean m_XPU_IPB_ABAReq() const;
	Boolean& m_XPU_IPB_ABAReq();

	void padding1(UInt8 const _padding1);
	UInt8 padding1() const;
	UInt8& padding1();

	void padding(UInt16 const _padding);
	UInt16 padding() const;
	UInt16& padding();





private:
	UInt32 m_m_XPU_ACCSt;
	Boolean m_m_XPU_IPB_ABAReq;
	UInt8 m_padding1;
	UInt16 m_padding;

};


/**
* @class CCpjEps
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjEps
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Float) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCpjEps();
	~CCpjEps() = default;
	CCpjEps(CCpjEps const &x) = default;
	CCpjEps(CCpjEps &&x) = default;
	CCpjEps& operator=(CCpjEps const &x) = default;
	CCpjEps& operator=(CCpjEps &&x) = default;

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
	void set_key_val(CCpjEps const* const _data) noexcept;



	void m_STAT_EPSAPAErr(Boolean const _m_STAT_EPSAPAErr);
	Boolean m_STAT_EPSAPAErr() const;
	Boolean& m_STAT_EPSAPAErr();

	void m_EPS_SASFault_E2E(Boolean const _m_EPS_SASFault_E2E);
	Boolean m_EPS_SASFault_E2E() const;
	Boolean& m_EPS_SASFault_E2E();

	void m_EPS_SASCalibratedSt_E2E(Boolean const _m_EPS_SASCalibratedSt_E2E);
	Boolean m_EPS_SASCalibratedSt_E2E() const;
	Boolean& m_EPS_SASCalibratedSt_E2E();

	void padding(UInt8 const _padding);
	UInt8 padding() const;
	UInt8& padding();

	void m_STAT_EPSCtrlAvailable(UInt32 const _m_STAT_EPSCtrlAvailable);
	UInt32 m_STAT_EPSCtrlAvailable() const;
	UInt32& m_STAT_EPSCtrlAvailable();

	void m_STAT_EPSAPAAbort(UInt32 const _m_STAT_EPSAPAAbort);
	UInt32 m_STAT_EPSAPAAbort() const;
	UInt32& m_STAT_EPSAPAAbort();

	void m_EPSTorqueFeedBack_f32(Float const _m_EPSTorqueFeedBack_f32);
	Float m_EPSTorqueFeedBack_f32() const;
	Float& m_EPSTorqueFeedBack_f32();

	void m_EPS_Systemstate(UInt32 const _m_EPS_Systemstate);
	UInt32 m_EPS_Systemstate() const;
	UInt32& m_EPS_Systemstate();

	void m_EPS_AngleCtrlMode(UInt32 const _m_EPS_AngleCtrlMode);
	UInt32 m_EPS_AngleCtrlMode() const;
	UInt32& m_EPS_AngleCtrlMode();

	void m_EPS_AngleCtrlSt(UInt32 const _m_EPS_AngleCtrlSt);
	UInt32 m_EPS_AngleCtrlSt() const;
	UInt32& m_EPS_AngleCtrlSt();

	void m_EPS_EndAngleCtrlReason(UInt32 const _m_EPS_EndAngleCtrlReason);
	UInt32 m_EPS_EndAngleCtrlReason() const;
	UInt32& m_EPS_EndAngleCtrlReason();





private:
	Boolean m_m_STAT_EPSAPAErr;
	Boolean m_m_EPS_SASFault_E2E;
	Boolean m_m_EPS_SASCalibratedSt_E2E;
	UInt8 m_padding;
	UInt32 m_m_STAT_EPSCtrlAvailable;
	UInt32 m_m_STAT_EPSAPAAbort;
	Float m_m_EPSTorqueFeedBack_f32;
	UInt32 m_m_EPS_Systemstate;
	UInt32 m_m_EPS_AngleCtrlMode;
	UInt32 m_m_EPS_AngleCtrlSt;
	UInt32 m_m_EPS_EndAngleCtrlReason;

};


/**
* @class CCpjFctaWarning
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjFctaWarning
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCpjFctaWarning();
	~CCpjFctaWarning() = default;
	CCpjFctaWarning(CCpjFctaWarning const &x) = default;
	CCpjFctaWarning(CCpjFctaWarning &&x) = default;
	CCpjFctaWarning& operator=(CCpjFctaWarning const &x) = default;
	CCpjFctaWarning& operator=(CCpjFctaWarning &&x) = default;

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
	void set_key_val(CCpjFctaWarning const* const _data) noexcept;



	void m_fctaWarningLH(UInt32 const _m_fctaWarningLH);
	UInt32 m_fctaWarningLH() const;
	UInt32& m_fctaWarningLH();

	void m_fctaWarningRH(UInt32 const _m_fctaWarningRH);
	UInt32 m_fctaWarningRH() const;
	UInt32& m_fctaWarningRH();





private:
	UInt32 m_m_fctaWarningLH;
	UInt32 m_m_fctaWarningRH;

};


/**
* @class CCpjFusaInfo
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjFusaInfo
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCpjFusaInfo();
	~CCpjFusaInfo() = default;
	CCpjFusaInfo(CCpjFusaInfo const &x) = default;
	CCpjFusaInfo(CCpjFusaInfo &&x) = default;
	CCpjFusaInfo& operator=(CCpjFusaInfo const &x) = default;
	CCpjFusaInfo& operator=(CCpjFusaInfo &&x) = default;

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
	void set_key_val(CCpjFusaInfo const* const _data) noexcept;



	void m_IdleSafetyFail_b(Boolean const _m_IdleSafetyFail_b);
	Boolean m_IdleSafetyFail_b() const;
	Boolean& m_IdleSafetyFail_b();

	void m_SupParkSafetyFail_b(Boolean const _m_SupParkSafetyFail_b);
	Boolean m_SupParkSafetyFail_b() const;
	Boolean& m_SupParkSafetyFail_b();

	void m_PilParkSafetyFail_b(Boolean const _m_PilParkSafetyFail_b);
	Boolean m_PilParkSafetyFail_b() const;
	Boolean& m_PilParkSafetyFail_b();





private:
	Boolean m_m_IdleSafetyFail_b;
	Boolean m_m_SupParkSafetyFail_b;
	Boolean m_m_PilParkSafetyFail_b;

};


/**
* @class CCpjIHU
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjIHU
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt16) + sizeof(UInt16) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(SInt16) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(Boolean);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCpjIHU();
	~CCpjIHU() = default;
	CCpjIHU(CCpjIHU const &x) = default;
	CCpjIHU(CCpjIHU &&x) = default;
	CCpjIHU& operator=(CCpjIHU const &x) = default;
	CCpjIHU& operator=(CCpjIHU &&x) = default;

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
	void set_key_val(CCpjIHU const* const _data) noexcept;



	void REQ_AVMTouchPanelX_1(UInt16 const _REQ_AVMTouchPanelX_1);
	UInt16 REQ_AVMTouchPanelX_1() const;
	UInt16& REQ_AVMTouchPanelX_1();

	void REQ_AVMTouchPanelY_1(UInt16 const _REQ_AVMTouchPanelY_1);
	UInt16 REQ_AVMTouchPanelY_1() const;
	UInt16& REQ_AVMTouchPanelY_1();

	void IPC_LanguageMode(UInt32 const _IPC_LanguageMode);
	UInt32 IPC_LanguageMode() const;
	UInt32& IPC_LanguageMode();

	void AVM_ExitRequest(Boolean const _AVM_ExitRequest);
	Boolean AVM_ExitRequest() const;
	Boolean& AVM_ExitRequest();

	void APA_ExitRequest(Boolean const _APA_ExitRequest);
	Boolean APA_ExitRequest() const;
	Boolean& APA_ExitRequest();

	void AVMCalibrationReq(Boolean const _AVMCalibrationReq);
	Boolean AVMCalibrationReq() const;
	Boolean& AVMCalibrationReq();

	void AVMSoftKeyRequest(Boolean const _AVMSoftKeyRequest);
	Boolean AVMSoftKeyRequest() const;
	Boolean& AVMSoftKeyRequest();

	void MEB_Setting(UInt32 const _MEB_Setting);
	UInt32 MEB_Setting() const;
	UInt32& MEB_Setting();

	void PDCmute_Setting(UInt32 const _PDCmute_Setting);
	UInt32 PDCmute_Setting() const;
	UInt32& PDCmute_Setting();

	void AVM_Setting(UInt32 const _AVM_Setting);
	UInt32 AVM_Setting() const;
	UInt32& AVM_Setting();

	void DOW_Setting(UInt32 const _DOW_Setting);
	UInt32 DOW_Setting() const;
	UInt32& DOW_Setting();

	void BSD_Setting(UInt32 const _BSD_Setting);
	UInt32 BSD_Setting() const;
	UInt32& BSD_Setting();

	void RCW_Setting(UInt32 const _RCW_Setting);
	UInt32 RCW_Setting() const;
	UInt32& RCW_Setting();

	void RCTA_Setting(UInt32 const _RCTA_Setting);
	UInt32 RCTA_Setting() const;
	UInt32& RCTA_Setting();

	void FCTA_Setting(UInt32 const _FCTA_Setting);
	UInt32 FCTA_Setting() const;
	UInt32& FCTA_Setting();

	void HMI_AVM_TouchEventType(UInt32 const _HMI_AVM_TouchEventType);
	UInt32 HMI_AVM_TouchEventType() const;
	UInt32& HMI_AVM_TouchEventType();

	void FAPA_Setting(UInt32 const _FAPA_Setting);
	UInt32 FAPA_Setting() const;
	UInt32& FAPA_Setting();

	void MOD_Setting(UInt32 const _MOD_Setting);
	UInt32 MOD_Setting() const;
	UInt32& MOD_Setting();

	void HPA_Voice_FuctionSwitch_display(Boolean const _HPA_Voice_FuctionSwitch_display);
	Boolean HPA_Voice_FuctionSwitch_display() const;
	Boolean& HPA_Voice_FuctionSwitch_display();

	void HPA_Voice_Confirmed(Boolean const _HPA_Voice_Confirmed);
	Boolean HPA_Voice_Confirmed() const;
	Boolean& HPA_Voice_Confirmed();

	void APASoftKeyRequest(Boolean const _APASoftKeyRequest);
	Boolean APASoftKeyRequest() const;
	Boolean& APASoftKeyRequest();

	void NOASoftKeyRequest(Boolean const _NOASoftKeyRequest);
	Boolean NOASoftKeyRequest() const;
	Boolean& NOASoftKeyRequest();

	void IHU_CurrentTheme(UInt32 const _IHU_CurrentTheme);
	UInt32 IHU_CurrentTheme() const;
	UInt32& IHU_CurrentTheme();

	void IHU_CurrentThemeMode(UInt32 const _IHU_CurrentThemeMode);
	UInt32 IHU_CurrentThemeMode() const;
	UInt32& IHU_CurrentThemeMode();

	void SET_BSV_Enable(UInt32 const _SET_BSV_Enable);
	UInt32 SET_BSV_Enable() const;
	UInt32& SET_BSV_Enable();

	void Left_Mirror_Status(UInt32 const _Left_Mirror_Status);
	UInt32 Left_Mirror_Status() const;
	UInt32& Left_Mirror_Status();

	void Right_Mirror_Status(UInt32 const _Right_Mirror_Status);
	UInt32 Right_Mirror_Status() const;
	UInt32& Right_Mirror_Status();

	void Door_Lock_Status(UInt32 const _Door_Lock_Status);
	UInt32 Door_Lock_Status() const;
	UInt32& Door_Lock_Status();

	void CDCU_XPU_DisplayMode(UInt32 const _CDCU_XPU_DisplayMode);
	UInt32 CDCU_XPU_DisplayMode() const;
	UInt32& CDCU_XPU_DisplayMode();

	void CDCU_XPU_TranspChassisWorkSt(UInt32 const _CDCU_XPU_TranspChassisWorkSt);
	UInt32 CDCU_XPU_TranspChassisWorkSt() const;
	UInt32& CDCU_XPU_TranspChassisWorkSt();

	void CDCU_XPU_TranBody(UInt32 const _CDCU_XPU_TranBody);
	UInt32 CDCU_XPU_TranBody() const;
	UInt32& CDCU_XPU_TranBody();

	void AVMHardKeyRequest(Boolean const _AVMHardKeyRequest);
	Boolean AVMHardKeyRequest() const;
	Boolean& AVMHardKeyRequest();

	void BMS_ThermPrpgtonRequest(Boolean const _BMS_ThermPrpgtonRequest);
	Boolean BMS_ThermPrpgtonRequest() const;
	Boolean& BMS_ThermPrpgtonRequest();

	void CDCU_3603Dangle(SInt16 const _CDCU_3603Dangle);
	SInt16 CDCU_3603Dangle() const;
	SInt16& CDCU_3603Dangle();

	void CDCU_XPU_NRABtn(UInt32 const _CDCU_XPU_NRABtn);
	UInt32 CDCU_XPU_NRABtn() const;
	UInt32& CDCU_XPU_NRABtn();

	void EMU3ChrgDchaConnSts(UInt32 const _EMU3ChrgDchaConnSts);
	UInt32 EMU3ChrgDchaConnSts() const;
	UInt32& EMU3ChrgDchaConnSts();

	void CDCU_WashCarSt(Boolean const _CDCU_WashCarSt);
	Boolean CDCU_WashCarSt() const;
	Boolean& CDCU_WashCarSt();

	void CDCU_ESP_EPBReqVD(Boolean const _CDCU_ESP_EPBReqVD);
	Boolean CDCU_ESP_EPBReqVD() const;
	Boolean& CDCU_ESP_EPBReqVD();

	void FrntRadarSwt(Boolean const _FrntRadarSwt);
	Boolean FrntRadarSwt() const;
	Boolean& FrntRadarSwt();

	void RRMDisplayAVMOff(Boolean const _RRMDisplayAVMOff);
	Boolean RRMDisplayAVMOff() const;
	Boolean& RRMDisplayAVMOff();

	void CarMdlSetColor(UInt8 const _CarMdlSetColor);
	UInt8 CarMdlSetColor() const;
	UInt8& CarMdlSetColor();

	void Reset_Default(UInt8 const _Reset_Default);
	UInt8 Reset_Default() const;
	UInt8& Reset_Default();

	void OffRoadState(UInt8 const _OffRoadState);
	UInt8 OffRoadState() const;
	UInt8& OffRoadState();

	void padding(UInt8 const _padding);
	UInt8 padding() const;
	UInt8& padding();

	void APASoftKeyRequestNoReverse(Boolean const _APASoftKeyRequestNoReverse);
	Boolean APASoftKeyRequestNoReverse() const;
	Boolean& APASoftKeyRequestNoReverse();





private:
	UInt16 m_REQ_AVMTouchPanelX_1;
	UInt16 m_REQ_AVMTouchPanelY_1;
	UInt32 m_IPC_LanguageMode;
	Boolean m_AVM_ExitRequest;
	Boolean m_APA_ExitRequest;
	Boolean m_AVMCalibrationReq;
	Boolean m_AVMSoftKeyRequest;
	UInt32 m_MEB_Setting;
	UInt32 m_PDCmute_Setting;
	UInt32 m_AVM_Setting;
	UInt32 m_DOW_Setting;
	UInt32 m_BSD_Setting;
	UInt32 m_RCW_Setting;
	UInt32 m_RCTA_Setting;
	UInt32 m_FCTA_Setting;
	UInt32 m_HMI_AVM_TouchEventType;
	UInt32 m_FAPA_Setting;
	UInt32 m_MOD_Setting;
	Boolean m_HPA_Voice_FuctionSwitch_display;
	Boolean m_HPA_Voice_Confirmed;
	Boolean m_APASoftKeyRequest;
	Boolean m_NOASoftKeyRequest;
	UInt32 m_IHU_CurrentTheme;
	UInt32 m_IHU_CurrentThemeMode;
	UInt32 m_SET_BSV_Enable;
	UInt32 m_Left_Mirror_Status;
	UInt32 m_Right_Mirror_Status;
	UInt32 m_Door_Lock_Status;
	UInt32 m_CDCU_XPU_DisplayMode;
	UInt32 m_CDCU_XPU_TranspChassisWorkSt;
	UInt32 m_CDCU_XPU_TranBody;
	Boolean m_AVMHardKeyRequest;
	Boolean m_BMS_ThermPrpgtonRequest;
	SInt16 m_CDCU_3603Dangle;
	UInt32 m_CDCU_XPU_NRABtn;
	UInt32 m_EMU3ChrgDchaConnSts;
	Boolean m_CDCU_WashCarSt;
	Boolean m_CDCU_ESP_EPBReqVD;
	Boolean m_FrntRadarSwt;
	Boolean m_RRMDisplayAVMOff;
	UInt8 m_CarMdlSetColor;
	UInt8 m_Reset_Default;
	UInt8 m_OffRoadState;
	UInt8 m_padding;
	Boolean m_APASoftKeyRequestNoReverse;

};


/**
* @class CCpjLDCU
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjLDCU
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(SInt16) + sizeof(SInt16) + sizeof(SInt16) + sizeof(SInt16) + sizeof(Float) + sizeof(UInt16) + sizeof(UInt16) + sizeof(UInt16) + sizeof(SInt16) + sizeof(Float) + sizeof(Float) + sizeof(SInt16) + sizeof(SInt16) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(SInt16) + sizeof(SInt16) + sizeof(SInt16) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt16) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCpjLDCU();
	~CCpjLDCU() = default;
	CCpjLDCU(CCpjLDCU const &x) = default;
	CCpjLDCU(CCpjLDCU &&x) = default;
	CCpjLDCU& operator=(CCpjLDCU const &x) = default;
	CCpjLDCU& operator=(CCpjLDCU &&x) = default;

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
	void set_key_val(CCpjLDCU const* const _data) noexcept;



	void m_LDCU_XPU_ChargerSt(UInt32 const _m_LDCU_XPU_ChargerSt);
	UInt32 m_LDCU_XPU_ChargerSt() const;
	UInt32& m_LDCU_XPU_ChargerSt();

	void m_LDCU_XPU_AvailableSt(Boolean const _m_LDCU_XPU_AvailableSt);
	Boolean m_LDCU_XPU_AvailableSt() const;
	Boolean& m_LDCU_XPU_AvailableSt();

	void m_LDCU_XPU_StopAutoDrive(Boolean const _m_LDCU_XPU_StopAutoDrive);
	Boolean m_LDCU_XPU_StopAutoDrive() const;
	Boolean& m_LDCU_XPU_StopAutoDrive();

	void m_LDCU_OverRideSt(Boolean const _m_LDCU_OverRideSt);
	Boolean m_LDCU_OverRideSt() const;
	Boolean& m_LDCU_OverRideSt();

	void m_LDCU_FMotorSpdDirection(Boolean const _m_LDCU_FMotorSpdDirection);
	Boolean m_LDCU_FMotorSpdDirection() const;
	Boolean& m_LDCU_FMotorSpdDirection();

	void m_LDCU_RodSignals(UInt32 const _m_LDCU_RodSignals);
	UInt32 m_LDCU_RodSignals() const;
	UInt32& m_LDCU_RodSignals();

	void m_LDCU_DriveModeFeedback(UInt32 const _m_LDCU_DriveModeFeedback);
	UInt32 m_LDCU_DriveModeFeedback() const;
	UInt32& m_LDCU_DriveModeFeedback();

	void m_LDCU_XPU_AvailStFeedback(UInt32 const _m_LDCU_XPU_AvailStFeedback);
	UInt32 m_LDCU_XPU_AvailStFeedback() const;
	UInt32& m_LDCU_XPU_AvailStFeedback();

	void m_LDCU_APBWarningLamp(UInt32 const _m_LDCU_APBWarningLamp);
	UInt32 m_LDCU_APBWarningLamp() const;
	UInt32& m_LDCU_APBWarningLamp();

	void m_LDCU_APBSysSt(UInt32 const _m_LDCU_APBSysSt);
	UInt32 m_LDCU_APBSysSt() const;
	UInt32& m_LDCU_APBSysSt();

	void m_LDCUOffroadDrvMod(UInt32 const _m_LDCUOffroadDrvMod);
	UInt32 m_LDCUOffroadDrvMod() const;
	UInt32& m_LDCUOffroadDrvMod();

	void m_LDCU_RWheelDriverTorqReq(SInt16 const _m_LDCU_RWheelDriverTorqReq);
	SInt16 m_LDCU_RWheelDriverTorqReq() const;
	SInt16& m_LDCU_RWheelDriverTorqReq();

	void m_LDCU_FWheelPeakTorqMin(SInt16 const _m_LDCU_FWheelPeakTorqMin);
	SInt16 m_LDCU_FWheelPeakTorqMin() const;
	SInt16& m_LDCU_FWheelPeakTorqMin();

	void m_LDCU_FWheelPeakTorqMax(SInt16 const _m_LDCU_FWheelPeakTorqMax);
	SInt16 m_LDCU_FWheelPeakTorqMax() const;
	SInt16& m_LDCU_FWheelPeakTorqMax();

	void m_LDCU_FWheelCurTorq(SInt16 const _m_LDCU_FWheelCurTorq);
	SInt16 m_LDCU_FWheelCurTorq() const;
	SInt16& m_LDCU_FWheelCurTorq();

	void m_LDCUHIPUDesTorq(Float const _m_LDCUHIPUDesTorq);
	Float m_LDCUHIPUDesTorq() const;
	Float& m_LDCUHIPUDesTorq();

	void m_LDCU_FMotorSpd(UInt16 const _m_LDCU_FMotorSpd);
	UInt16 m_LDCU_FMotorSpd() const;
	UInt16& m_LDCU_FMotorSpd();

	void m_LDCU_MMotorSpd(UInt16 const _m_LDCU_MMotorSpd);
	UInt16 m_LDCU_MMotorSpd() const;
	UInt16& m_LDCU_MMotorSpd();

	void m_LDCU_RMotorSpd(UInt16 const _m_LDCU_RMotorSpd);
	UInt16 m_LDCU_RMotorSpd() const;
	UInt16& m_LDCU_RMotorSpd();

	void m_LDCU_MWheelPeakTorqMin(SInt16 const _m_LDCU_MWheelPeakTorqMin);
	SInt16 m_LDCU_MWheelPeakTorqMin() const;
	SInt16& m_LDCU_MWheelPeakTorqMin();

	void m_LDCUIPUFDesTorq(Float const _m_LDCUIPUFDesTorq);
	Float m_LDCUIPUFDesTorq() const;
	Float& m_LDCUIPUFDesTorq();

	void m_LDCUIPURDesTorq(Float const _m_LDCUIPURDesTorq);
	Float m_LDCUIPURDesTorq() const;
	Float& m_LDCUIPURDesTorq();

	void m_LDCU_MWheelPeakTorqMax(SInt16 const _m_LDCU_MWheelPeakTorqMax);
	SInt16 m_LDCU_MWheelPeakTorqMax() const;
	SInt16& m_LDCU_MWheelPeakTorqMax();

	void m_LDCU_MWheelCurTorq(SInt16 const _m_LDCU_MWheelCurTorq);
	SInt16 m_LDCU_MWheelCurTorq() const;
	SInt16& m_LDCU_MWheelCurTorq();

	void m_LDCU_BodySysSt1(UInt32 const _m_LDCU_BodySysSt1);
	UInt32 m_LDCU_BodySysSt1() const;
	UInt32& m_LDCU_BodySysSt1();

	void m_LDCU_MMotorSpdDirection(Boolean const _m_LDCU_MMotorSpdDirection);
	Boolean m_LDCU_MMotorSpdDirection() const;
	Boolean& m_LDCU_MMotorSpdDirection();

	void m_LDCU_RMotorSpdDirection(Boolean const _m_LDCU_RMotorSpdDirection);
	Boolean m_LDCU_RMotorSpdDirection() const;
	Boolean& m_LDCU_RMotorSpdDirection();

	void m_LDCU_RWheelPeakTorqMin(SInt16 const _m_LDCU_RWheelPeakTorqMin);
	SInt16 m_LDCU_RWheelPeakTorqMin() const;
	SInt16& m_LDCU_RWheelPeakTorqMin();

	void m_LDCU_RWheelPeakTorqMax(SInt16 const _m_LDCU_RWheelPeakTorqMax);
	SInt16 m_LDCU_RWheelPeakTorqMax() const;
	SInt16& m_LDCU_RWheelPeakTorqMax();

	void m_LDCU_RWheelCurTorq(SInt16 const _m_LDCU_RWheelCurTorq);
	SInt16 m_LDCU_RWheelCurTorq() const;
	SInt16& m_LDCU_RWheelCurTorq();

	void m_LDCU_DriverDoorLockSt(Boolean const _m_LDCU_DriverDoorLockSt);
	Boolean m_LDCU_DriverDoorLockSt() const;
	Boolean& m_LDCU_DriverDoorLockSt();

	void m_LDCU_AccPedalSigVD(Boolean const _m_LDCU_AccPedalSigVD);
	Boolean m_LDCU_AccPedalSigVD() const;
	Boolean& m_LDCU_AccPedalSigVD();

	void padding(UInt16 const _padding);
	UInt16 padding() const;
	UInt16& padding();

	void m_LDCU_EVSysReadySt(UInt32 const _m_LDCU_EVSysReadySt);
	UInt32 m_LDCU_EVSysReadySt() const;
	UInt32& m_LDCU_EVSysReadySt();

	void m_LDCU_TgatePosSts(UInt32 const _m_LDCU_TgatePosSts);
	UInt32 m_LDCU_TgatePosSts() const;
	UInt32& m_LDCU_TgatePosSts();

	void m_LDCU_TgateMvtSts(UInt32 const _m_LDCU_TgateMvtSts);
	UInt32 m_LDCU_TgateMvtSts() const;
	UInt32& m_LDCU_TgateMvtSts();

	void m_LDCU_CapPosSts(UInt32 const _m_LDCU_CapPosSts);
	UInt32 m_LDCU_CapPosSts() const;
	UInt32& m_LDCU_CapPosSts();

	void m_LDCU_CapMvtSts(UInt32 const _m_LDCU_CapMvtSts);
	UInt32 m_LDCU_CapMvtSts() const;
	UInt32& m_LDCU_CapMvtSts();

	void m_LDCU_TPMSPrWarnFL(UInt32 const _m_LDCU_TPMSPrWarnFL);
	UInt32 m_LDCU_TPMSPrWarnFL() const;
	UInt32& m_LDCU_TPMSPrWarnFL();

	void m_LDCU_TPMSPrWarnFR(UInt32 const _m_LDCU_TPMSPrWarnFR);
	UInt32 m_LDCU_TPMSPrWarnFR() const;
	UInt32& m_LDCU_TPMSPrWarnFR();

	void m_LDCU_TPMSPrWarnRL(UInt32 const _m_LDCU_TPMSPrWarnRL);
	UInt32 m_LDCU_TPMSPrWarnRL() const;
	UInt32& m_LDCU_TPMSPrWarnRL();

	void m_LDCU_TPMSPrWarnRR(UInt32 const _m_LDCU_TPMSPrWarnRR);
	UInt32 m_LDCU_TPMSPrWarnRR() const;
	UInt32& m_LDCU_TPMSPrWarnRR();

	void m_LDCU_TPMSPrWarn3rdL(UInt32 const _m_LDCU_TPMSPrWarn3rdL);
	UInt32 m_LDCU_TPMSPrWarn3rdL() const;
	UInt32& m_LDCU_TPMSPrWarn3rdL();

	void m_LDCU_TPMSPrWarn3rdR(UInt32 const _m_LDCU_TPMSPrWarn3rdR);
	UInt32 m_LDCU_TPMSPrWarn3rdR() const;
	UInt32& m_LDCU_TPMSPrWarn3rdR();

	void m_LDCU_XPU_InteractSt(UInt32 const _m_LDCU_XPU_InteractSt);
	UInt32 m_LDCU_XPU_InteractSt() const;
	UInt32& m_LDCU_XPU_InteractSt();





private:
	UInt32 m_m_LDCU_XPU_ChargerSt;
	Boolean m_m_LDCU_XPU_AvailableSt;
	Boolean m_m_LDCU_XPU_StopAutoDrive;
	Boolean m_m_LDCU_OverRideSt;
	Boolean m_m_LDCU_FMotorSpdDirection;
	UInt32 m_m_LDCU_RodSignals;
	UInt32 m_m_LDCU_DriveModeFeedback;
	UInt32 m_m_LDCU_XPU_AvailStFeedback;
	UInt32 m_m_LDCU_APBWarningLamp;
	UInt32 m_m_LDCU_APBSysSt;
	UInt32 m_m_LDCUOffroadDrvMod;
	SInt16 m_m_LDCU_RWheelDriverTorqReq;
	SInt16 m_m_LDCU_FWheelPeakTorqMin;
	SInt16 m_m_LDCU_FWheelPeakTorqMax;
	SInt16 m_m_LDCU_FWheelCurTorq;
	Float m_m_LDCUHIPUDesTorq;
	UInt16 m_m_LDCU_FMotorSpd;
	UInt16 m_m_LDCU_MMotorSpd;
	UInt16 m_m_LDCU_RMotorSpd;
	SInt16 m_m_LDCU_MWheelPeakTorqMin;
	Float m_m_LDCUIPUFDesTorq;
	Float m_m_LDCUIPURDesTorq;
	SInt16 m_m_LDCU_MWheelPeakTorqMax;
	SInt16 m_m_LDCU_MWheelCurTorq;
	UInt32 m_m_LDCU_BodySysSt1;
	Boolean m_m_LDCU_MMotorSpdDirection;
	Boolean m_m_LDCU_RMotorSpdDirection;
	SInt16 m_m_LDCU_RWheelPeakTorqMin;
	SInt16 m_m_LDCU_RWheelPeakTorqMax;
	SInt16 m_m_LDCU_RWheelCurTorq;
	Boolean m_m_LDCU_DriverDoorLockSt;
	Boolean m_m_LDCU_AccPedalSigVD;
	UInt16 m_padding;
	UInt32 m_m_LDCU_EVSysReadySt;
	UInt32 m_m_LDCU_TgatePosSts;
	UInt32 m_m_LDCU_TgateMvtSts;
	UInt32 m_m_LDCU_CapPosSts;
	UInt32 m_m_LDCU_CapMvtSts;
	UInt32 m_m_LDCU_TPMSPrWarnFL;
	UInt32 m_m_LDCU_TPMSPrWarnFR;
	UInt32 m_m_LDCU_TPMSPrWarnRL;
	UInt32 m_m_LDCU_TPMSPrWarnRR;
	UInt32 m_m_LDCU_TPMSPrWarn3rdL;
	UInt32 m_m_LDCU_TPMSPrWarn3rdR;
	UInt32 m_m_LDCU_XPU_InteractSt;

};


/**
* @class CCpjLongCtrl
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjLongCtrl
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Float);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCpjLongCtrl();
	~CCpjLongCtrl() = default;
	CCpjLongCtrl(CCpjLongCtrl const &x) = default;
	CCpjLongCtrl(CCpjLongCtrl &&x) = default;
	CCpjLongCtrl& operator=(CCpjLongCtrl const &x) = default;
	CCpjLongCtrl& operator=(CCpjLongCtrl &&x) = default;

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
	void set_key_val(CCpjLongCtrl const* const _data) noexcept;



	void m_ESC_LC_FailureSts(UInt32 const _m_ESC_LC_FailureSts);
	UInt32 m_ESC_LC_FailureSts() const;
	UInt32& m_ESC_LC_FailureSts();

	void m_ESC_LC_Status(UInt32 const _m_ESC_LC_Status);
	UInt32 m_ESC_LC_Status() const;
	UInt32& m_ESC_LC_Status();

	void m_ESC_LC_Availability(UInt32 const _m_ESC_LC_Availability);
	UInt32 m_ESC_LC_Availability() const;
	UInt32& m_ESC_LC_Availability();

	void m_Active_Vehicle_Hold(Boolean const _m_Active_Vehicle_Hold);
	Boolean m_Active_Vehicle_Hold() const;
	Boolean& m_Active_Vehicle_Hold();

	void m_ESC_Switch_b(Boolean const _m_ESC_Switch_b);
	Boolean m_ESC_Switch_b() const;
	Boolean& m_ESC_Switch_b();

	void m_VehInAutoHold_b(Boolean const _m_VehInAutoHold_b);
	Boolean m_VehInAutoHold_b() const;
	Boolean& m_VehInAutoHold_b();

	void m_MotorHomeConnect_b(Boolean const _m_MotorHomeConnect_b);
	Boolean m_MotorHomeConnect_b() const;
	Boolean& m_MotorHomeConnect_b();

	void m_GasPedalPos_f32(Float const _m_GasPedalPos_f32);
	Float m_GasPedalPos_f32() const;
	Float& m_GasPedalPos_f32();





private:
	UInt32 m_m_ESC_LC_FailureSts;
	UInt32 m_m_ESC_LC_Status;
	UInt32 m_m_ESC_LC_Availability;
	Boolean m_m_Active_Vehicle_Hold;
	Boolean m_m_ESC_Switch_b;
	Boolean m_m_VehInAutoHold_b;
	Boolean m_m_MotorHomeConnect_b;
	Float m_m_GasPedalPos_f32;

};


/**
* @class CCpjRctaWarning
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjRctaWarning
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCpjRctaWarning();
	~CCpjRctaWarning() = default;
	CCpjRctaWarning(CCpjRctaWarning const &x) = default;
	CCpjRctaWarning(CCpjRctaWarning &&x) = default;
	CCpjRctaWarning& operator=(CCpjRctaWarning const &x) = default;
	CCpjRctaWarning& operator=(CCpjRctaWarning &&x) = default;

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
	void set_key_val(CCpjRctaWarning const* const _data) noexcept;



	void m_rctaWarningLH(UInt32 const _m_rctaWarningLH);
	UInt32 m_rctaWarningLH() const;
	UInt32& m_rctaWarningLH();

	void m_rctaWarningRH(UInt32 const _m_rctaWarningRH);
	UInt32 m_rctaWarningRH() const;
	UInt32& m_rctaWarningRH();





private:
	UInt32 m_m_rctaWarningLH;
	UInt32 m_m_rctaWarningRH;

};


/**
* @class CCpjTDCU
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjTDCU
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(Float) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(SInt8) + sizeof(SInt8) + sizeof(SInt8) + sizeof(SInt8) + sizeof(SInt8) + sizeof(SInt8) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt8);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCpjTDCU();
	~CCpjTDCU() = default;
	CCpjTDCU(CCpjTDCU const &x) = default;
	CCpjTDCU(CCpjTDCU &&x) = default;
	CCpjTDCU& operator=(CCpjTDCU const &x) = default;
	CCpjTDCU& operator=(CCpjTDCU &&x) = default;

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
	void set_key_val(CCpjTDCU const* const _data) noexcept;



	void m_TDCU_XPU_FunctionMode(UInt32 const _m_TDCU_XPU_FunctionMode);
	UInt32 m_TDCU_XPU_FunctionMode() const;
	UInt32& m_TDCU_XPU_FunctionMode();

	void m_TDCU_XPU_SteerAngleReqVal(Float const _m_TDCU_XPU_SteerAngleReqVal);
	Float m_TDCU_XPU_SteerAngleReqVal() const;
	Float& m_TDCU_XPU_SteerAngleReqVal();

	void m_TDCU_XPU_AngCtrlReq(UInt32 const _m_TDCU_XPU_AngCtrlReq);
	UInt32 m_TDCU_XPU_AngCtrlReq() const;
	UInt32& m_TDCU_XPU_AngCtrlReq();

	void m_TDCU_XPU_EPBReq(UInt32 const _m_TDCU_XPU_EPBReq);
	UInt32 m_TDCU_XPU_EPBReq() const;
	UInt32& m_TDCU_XPU_EPBReq();

	void m_TDCU_XPU_SteerAngleReqValVD(Boolean const _m_TDCU_XPU_SteerAngleReqValVD);
	Boolean m_TDCU_XPU_SteerAngleReqValVD() const;
	Boolean& m_TDCU_XPU_SteerAngleReqValVD();

	void m_TDCU_XPU_EPBReqVD(Boolean const _m_TDCU_XPU_EPBReqVD);
	Boolean m_TDCU_XPU_EPBReqVD() const;
	Boolean& m_TDCU_XPU_EPBReqVD();

	void m_TdcuAsHgtFLmm(SInt8 const _m_TdcuAsHgtFLmm);
	SInt8 m_TdcuAsHgtFLmm() const;
	SInt8& m_TdcuAsHgtFLmm();

	void m_TdcuAsHgtFRmm(SInt8 const _m_TdcuAsHgtFRmm);
	SInt8 m_TdcuAsHgtFRmm() const;
	SInt8& m_TdcuAsHgtFRmm();

	void m_TdcuAsHgtMLmm(SInt8 const _m_TdcuAsHgtMLmm);
	SInt8 m_TdcuAsHgtMLmm() const;
	SInt8& m_TdcuAsHgtMLmm();

	void m_TdcuAsHgtMRmm(SInt8 const _m_TdcuAsHgtMRmm);
	SInt8 m_TdcuAsHgtMRmm() const;
	SInt8& m_TdcuAsHgtMRmm();

	void m_TdcuAsHgtRLmm(SInt8 const _m_TdcuAsHgtRLmm);
	SInt8 m_TdcuAsHgtRLmm() const;
	SInt8& m_TdcuAsHgtRLmm();

	void m_TdcuAsHgtRRmm(SInt8 const _m_TdcuAsHgtRRmm);
	SInt8 m_TdcuAsHgtRRmm() const;
	SInt8& m_TdcuAsHgtRRmm();

	void m_TdcuSepAndComSt(UInt32 const _m_TdcuSepAndComSt);
	UInt32 m_TdcuSepAndComSt() const;
	UInt32& m_TdcuSepAndComSt();

	void m_TdcuSNCParkReq(UInt32 const _m_TdcuSNCParkReq);
	UInt32 m_TdcuSNCParkReq() const;
	UInt32& m_TdcuSNCParkReq();

	void m_W_VoteNavHorPosType(UInt32 const _m_W_VoteNavHorPosType);
	UInt32 m_W_VoteNavHorPosType() const;
	UInt32& m_W_VoteNavHorPosType();

	void m_TDCU_YAWRate(Float const _m_TDCU_YAWRate);
	Float m_TDCU_YAWRate() const;
	Float& m_TDCU_YAWRate();

	void m_TdcuDistanceX_L(Float const _m_TdcuDistanceX_L);
	Float m_TdcuDistanceX_L() const;
	Float& m_TdcuDistanceX_L();

	void m_TdcuDistanceX_R(Float const _m_TdcuDistanceX_R);
	Float m_TdcuDistanceX_R() const;
	Float& m_TdcuDistanceX_R();

	void m_TdcuLADistanceX(Float const _m_TdcuLADistanceX);
	Float m_TdcuLADistanceX() const;
	Float& m_TdcuLADistanceX();

	void m_TdcuDistanceX_L_VD(UInt32 const _m_TdcuDistanceX_L_VD);
	UInt32 m_TdcuDistanceX_L_VD() const;
	UInt32& m_TdcuDistanceX_L_VD();

	void m_TdcuDistanceX_R_VD(UInt32 const _m_TdcuDistanceX_R_VD);
	UInt32 m_TdcuDistanceX_R_VD() const;
	UInt32& m_TdcuDistanceX_R_VD();

	void m_TDCU_SepComRKEEnable(Boolean const _m_TDCU_SepComRKEEnable);
	Boolean m_TDCU_SepComRKEEnable() const;
	Boolean& m_TDCU_SepComRKEEnable();

	void m_TdcuSNCParkPauseReq(Boolean const _m_TdcuSNCParkPauseReq);
	Boolean m_TdcuSNCParkPauseReq() const;
	Boolean& m_TdcuSNCParkPauseReq();

	void m_TdcuLADistanceX_VD(Boolean const _m_TdcuLADistanceX_VD);
	Boolean m_TdcuLADistanceX_VD() const;
	Boolean& m_TdcuLADistanceX_VD();

	void padding(UInt8 const _padding);
	UInt8 padding() const;
	UInt8& padding();





private:
	UInt32 m_m_TDCU_XPU_FunctionMode;
	Float m_m_TDCU_XPU_SteerAngleReqVal;
	UInt32 m_m_TDCU_XPU_AngCtrlReq;
	UInt32 m_m_TDCU_XPU_EPBReq;
	Boolean m_m_TDCU_XPU_SteerAngleReqValVD;
	Boolean m_m_TDCU_XPU_EPBReqVD;
	SInt8 m_m_TdcuAsHgtFLmm;
	SInt8 m_m_TdcuAsHgtFRmm;
	SInt8 m_m_TdcuAsHgtMLmm;
	SInt8 m_m_TdcuAsHgtMRmm;
	SInt8 m_m_TdcuAsHgtRLmm;
	SInt8 m_m_TdcuAsHgtRRmm;
	UInt32 m_m_TdcuSepAndComSt;
	UInt32 m_m_TdcuSNCParkReq;
	UInt32 m_m_W_VoteNavHorPosType;
	Float m_m_TDCU_YAWRate;
	Float m_m_TdcuDistanceX_L;
	Float m_m_TdcuDistanceX_R;
	Float m_m_TdcuLADistanceX;
	UInt32 m_m_TdcuDistanceX_L_VD;
	UInt32 m_m_TdcuDistanceX_R_VD;
	Boolean m_m_TDCU_SepComRKEEnable;
	Boolean m_m_TdcuSNCParkPauseReq;
	Boolean m_m_TdcuLADistanceX_VD;
	UInt8 m_padding;

};


/**
* @class CCpjVehInfo
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjVehInfo
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt32) + sizeof(Float) + sizeof(UInt32) + sizeof(UInt8) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Float) + sizeof(UInt16);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CCpjVehInfo();
	~CCpjVehInfo() = default;
	CCpjVehInfo(CCpjVehInfo const &x) = default;
	CCpjVehInfo(CCpjVehInfo &&x) = default;
	CCpjVehInfo& operator=(CCpjVehInfo const &x) = default;
	CCpjVehInfo& operator=(CCpjVehInfo &&x) = default;

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
	void set_key_val(CCpjVehInfo const* const _data) noexcept;



	void m_AccModeIsActive(Boolean const _m_AccModeIsActive);
	Boolean m_AccModeIsActive() const;
	Boolean& m_AccModeIsActive();

	void m_VehIsCrashed(Boolean const _m_VehIsCrashed);
	Boolean m_VehIsCrashed() const;
	Boolean& m_VehIsCrashed();

	void m_AEBDecCtrlIsReq(Boolean const _m_AEBDecCtrlIsReq);
	Boolean m_AEBDecCtrlIsReq() const;
	Boolean& m_AEBDecCtrlIsReq();

	void m_EBDDecCtrlIsReq(Boolean const _m_EBDDecCtrlIsReq);
	Boolean m_EBDDecCtrlIsReq() const;
	Boolean& m_EBDDecCtrlIsReq();

	void m_AssociatedSysFctActive_b(Boolean const _m_AssociatedSysFctActive_b);
	Boolean m_AssociatedSysFctActive_b() const;
	Boolean& m_AssociatedSysFctActive_b();

	void m_ActSafeFctActive_b(Boolean const _m_ActSafeFctActive_b);
	Boolean m_ActSafeFctActive_b() const;
	Boolean& m_ActSafeFctActive_b();

	void m_RCTBDecCtrlReq(Boolean const _m_RCTBDecCtrlReq);
	Boolean m_RCTBDecCtrlReq() const;
	Boolean& m_RCTBDecCtrlReq();

	void m_CddHoldActive(UInt8 const _m_CddHoldActive);
	UInt8 m_CddHoldActive() const;
	UInt8& m_CddHoldActive();

	void m_Stat_RCTB(UInt32 const _m_Stat_RCTB);
	UInt32 m_Stat_RCTB() const;
	UInt32& m_Stat_RCTB();

	void m_RCTBTgtDecel(Float const _m_RCTBTgtDecel);
	Float m_RCTBTgtDecel() const;
	Float& m_RCTBTgtDecel();

	void m_ParkingFaultCluster(UInt32 const _m_ParkingFaultCluster);
	UInt32 m_ParkingFaultCluster() const;
	UInt32& m_ParkingFaultCluster();

	void m_NoBrakeForce(UInt8 const _m_NoBrakeForce);
	UInt8 m_NoBrakeForce() const;
	UInt8& m_NoBrakeForce();

	void m_VehIsPowerLow(Boolean const _m_VehIsPowerLow);
	Boolean m_VehIsPowerLow() const;
	Boolean& m_VehIsPowerLow();

	void m_ChargeConnected(Boolean const _m_ChargeConnected);
	Boolean m_ChargeConnected() const;
	Boolean& m_ChargeConnected();

	void m_CameraIsBlocked(Boolean const _m_CameraIsBlocked);
	Boolean m_CameraIsBlocked() const;
	Boolean& m_CameraIsBlocked();

	void m_VehPowerValue(Float const _m_VehPowerValue);
	Float m_VehPowerValue() const;
	Float& m_VehPowerValue();

	void m_VehRemDrvRange(UInt16 const _m_VehRemDrvRange);
	UInt16 m_VehRemDrvRange() const;
	UInt16& m_VehRemDrvRange();





private:
	Boolean m_m_AccModeIsActive;
	Boolean m_m_VehIsCrashed;
	Boolean m_m_AEBDecCtrlIsReq;
	Boolean m_m_EBDDecCtrlIsReq;
	Boolean m_m_AssociatedSysFctActive_b;
	Boolean m_m_ActSafeFctActive_b;
	Boolean m_m_RCTBDecCtrlReq;
	UInt8 m_m_CddHoldActive;
	UInt32 m_m_Stat_RCTB;
	Float m_m_RCTBTgtDecel;
	UInt32 m_m_ParkingFaultCluster;
	UInt8 m_m_NoBrakeForce;
	Boolean m_m_VehIsPowerLow;
	Boolean m_m_ChargeConnected;
	Boolean m_m_CameraIsBlocked;
	Float m_m_VehPowerValue;
	UInt16 m_m_VehRemDrvRange;

};


/**
* @class CCpjTel
* @brief A class as the datatype for data exchange.
* @note
*/

class CCpjTel
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(SInt8) + sizeof(Boolean) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt8) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(EIdcServiceResp_st) + sizeof(UInt16) + sizeof(Boolean) + sizeof(Boolean);
	static constexpr bool IS_DATA_PADDING = true || (EIdcServiceResp_st::DATA_SIZE != sizeof(EIdcServiceResp_st)) || (EIdcServiceResp_st::IS_DATA_PADDING);
	static constexpr bool IS_ID_DEFINED = false;

	CCpjTel();
	~CCpjTel() = default;
	CCpjTel(CCpjTel const &x) = default;
	CCpjTel(CCpjTel &&x) = default;
	CCpjTel& operator=(CCpjTel const &x) = default;
	CCpjTel& operator=(CCpjTel &&x) = default;

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
	void set_key_val(CCpjTel const* const _data) noexcept;



	void m_TEL_Phone_ConnectSt(UInt32 const _m_TEL_Phone_ConnectSt);
	UInt32 m_TEL_Phone_ConnectSt() const;
	UInt32& m_TEL_Phone_ConnectSt();

	void m_TEL_RPAKeyPstSt(UInt32 const _m_TEL_RPAKeyPstSt);
	UInt32 m_TEL_RPAKeyPstSt() const;
	UInt32& m_TEL_RPAKeyPstSt();

	void m_TEL_RPADeviceSt(UInt32 const _m_TEL_RPADeviceSt);
	UInt32 m_TEL_RPADeviceSt() const;
	UInt32& m_TEL_RPADeviceSt();

	void m_TEL_RPACheckReq(Boolean const _m_TEL_RPACheckReq);
	Boolean m_TEL_RPACheckReq() const;
	Boolean& m_TEL_RPACheckReq();

	void m_TEL_RPABtnStPressed(Boolean const _m_TEL_RPABtnStPressed);
	Boolean m_TEL_RPABtnStPressed() const;
	Boolean& m_TEL_RPABtnStPressed();

	void m_TEL_PhoneBattSOC(SInt8 const _m_TEL_PhoneBattSOC);
	SInt8 m_TEL_PhoneBattSOC() const;
	SInt8& m_TEL_PhoneBattSOC();

	void m_TEL_RPACancelBtnStPressed(Boolean const _m_TEL_RPACancelBtnStPressed);
	Boolean m_TEL_RPACancelBtnStPressed() const;
	Boolean& m_TEL_RPACancelBtnStPressed();

	void m_TEL_RPAOutDirSel(UInt32 const _m_TEL_RPAOutDirSel);
	UInt32 m_TEL_RPAOutDirSel() const;
	UInt32& m_TEL_RPAOutDirSel();

	void m_TEL_RPADeviceFailReason(UInt32 const _m_TEL_RPADeviceFailReason);
	UInt32 m_TEL_RPADeviceFailReason() const;
	UInt32& m_TEL_RPADeviceFailReason();

	void m_TEL_DrvGestureStOk(UInt8 const _m_TEL_DrvGestureStOk);
	UInt8 m_TEL_DrvGestureStOk() const;
	UInt8& m_TEL_DrvGestureStOk();

	void m_TEL_DrvGestureCheckOk(Boolean const _m_TEL_DrvGestureCheckOk);
	Boolean m_TEL_DrvGestureCheckOk() const;
	Boolean& m_TEL_DrvGestureCheckOk();

	void m_TEL_RPAUndoBtnStPressed(Boolean const _m_TEL_RPAUndoBtnStPressed);
	Boolean m_TEL_RPAUndoBtnStPressed() const;
	Boolean& m_TEL_RPAUndoBtnStPressed();

	void m_TEL_BTCountSt_u8(UInt8 const _m_TEL_BTCountSt_u8);
	UInt8 m_TEL_BTCountSt_u8() const;
	UInt8& m_TEL_BTCountSt_u8();

	void m_TEL_IDC_AppParkingStResp(UInt32 const _m_TEL_IDC_AppParkingStResp);
	UInt32 m_TEL_IDC_AppParkingStResp() const;
	UInt32& m_TEL_IDC_AppParkingStResp();

	void m_TEL_APA_AuthSt(UInt32 const _m_TEL_APA_AuthSt);
	UInt32 m_TEL_APA_AuthSt() const;
	UInt32& m_TEL_APA_AuthSt();

	void m_TEL_RPA_AuthSt(UInt32 const _m_TEL_RPA_AuthSt);
	UInt32 m_TEL_RPA_AuthSt() const;
	UInt32& m_TEL_RPA_AuthSt();

	void m_TEL_HPA_AuthSt(UInt32 const _m_TEL_HPA_AuthSt);
	UInt32 m_TEL_HPA_AuthSt() const;
	UInt32& m_TEL_HPA_AuthSt();

	void m_TEL_RPAFunction_en(UInt32 const _m_TEL_RPAFunction_en);
	UInt32 m_TEL_RPAFunction_en() const;
	UInt32& m_TEL_RPAFunction_en();

	void m_TEL_RPADirSel_en(UInt32 const _m_TEL_RPADirSel_en);
	UInt32 m_TEL_RPADirSel_en() const;
	UInt32& m_TEL_RPADirSel_en();

	void m_TEL_RPAServiceResp_st(EIdcServiceResp_st const &_m_TEL_RPAServiceResp_st);
	void m_TEL_RPAServiceResp_st(EIdcServiceResp_st &&_m_TEL_RPAServiceResp_st);
	EIdcServiceResp_st const& m_TEL_RPAServiceResp_st() const;
	EIdcServiceResp_st& m_TEL_RPAServiceResp_st();

	void padding(UInt16 const _padding);
	UInt16 padding() const;
	UInt16& padding();

	void m_TEL_RPAPowerOffReq(Boolean const _m_TEL_RPAPowerOffReq);
	Boolean m_TEL_RPAPowerOffReq() const;
	Boolean& m_TEL_RPAPowerOffReq();

	void m_TEL_RPAobscancelCfm(Boolean const _m_TEL_RPAobscancelCfm);
	Boolean m_TEL_RPAobscancelCfm() const;
	Boolean& m_TEL_RPAobscancelCfm();





private:
	UInt32 m_m_TEL_Phone_ConnectSt;
	UInt32 m_m_TEL_RPAKeyPstSt;
	UInt32 m_m_TEL_RPADeviceSt;
	Boolean m_m_TEL_RPACheckReq;
	Boolean m_m_TEL_RPABtnStPressed;
	SInt8 m_m_TEL_PhoneBattSOC;
	Boolean m_m_TEL_RPACancelBtnStPressed;
	UInt32 m_m_TEL_RPAOutDirSel;
	UInt32 m_m_TEL_RPADeviceFailReason;
	UInt8 m_m_TEL_DrvGestureStOk;
	Boolean m_m_TEL_DrvGestureCheckOk;
	Boolean m_m_TEL_RPAUndoBtnStPressed;
	UInt8 m_m_TEL_BTCountSt_u8;
	UInt32 m_m_TEL_IDC_AppParkingStResp;
	UInt32 m_m_TEL_APA_AuthSt;
	UInt32 m_m_TEL_RPA_AuthSt;
	UInt32 m_m_TEL_HPA_AuthSt;
	UInt32 m_m_TEL_RPAFunction_en;
	UInt32 m_m_TEL_RPADirSel_en;
	EIdcServiceResp_st m_m_TEL_RPAServiceResp_st;
	UInt16 m_padding;
	Boolean m_m_TEL_RPAPowerOffReq;
	Boolean m_m_TEL_RPAobscancelCfm;

};


/**
* @class CPfBrakes
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfBrakes
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt16) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Float);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfBrakes();
	~CPfBrakes() = default;
	CPfBrakes(CPfBrakes const &x) = default;
	CPfBrakes(CPfBrakes &&x) = default;
	CPfBrakes& operator=(CPfBrakes const &x) = default;
	CPfBrakes& operator=(CPfBrakes &&x) = default;

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
	void set_key_val(CPfBrakes const* const _data) noexcept;



	void m_espOpMode(UInt32 const _m_espOpMode);
	UInt32 m_espOpMode() const;
	UInt32& m_espOpMode();

	void m_absActive(Boolean const _m_absActive);
	Boolean m_absActive() const;
	Boolean& m_absActive();

	void m_aebActive(Boolean const _m_aebActive);
	Boolean m_aebActive() const;
	Boolean& m_aebActive();

	void padding(UInt16 const _padding);
	UInt16 padding() const;
	UInt16& padding();

	void m_prkgBrkState(UInt32 const _m_prkgBrkState);
	UInt32 m_prkgBrkState() const;
	UInt32& m_prkgBrkState();

	void m_brkPedlState(UInt32 const _m_brkPedlState);
	UInt32 m_brkPedlState() const;
	UInt32& m_brkPedlState();

	void m_masterCylinderPress(Float const _m_masterCylinderPress);
	Float m_masterCylinderPress() const;
	Float& m_masterCylinderPress();





private:
	UInt32 m_m_espOpMode;
	Boolean m_m_absActive;
	Boolean m_m_aebActive;
	UInt16 m_padding;
	UInt32 m_m_prkgBrkState;
	UInt32 m_m_brkPedlState;
	Float m_m_masterCylinderPress;

};


/**
* @class CPfCamera
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfCamera
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfCamera();
	~CPfCamera() = default;
	CPfCamera(CPfCamera const &x) = default;
	CPfCamera(CPfCamera &&x) = default;
	CPfCamera& operator=(CPfCamera const &x) = default;
	CPfCamera& operator=(CPfCamera &&x) = default;

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
	void set_key_val(CPfCamera const* const _data) noexcept;



	void m_statusProtectionFlapFront(UInt32 const _m_statusProtectionFlapFront);
	UInt32 m_statusProtectionFlapFront() const;
	UInt32& m_statusProtectionFlapFront();

	void m_statusProtectionFlapRear(UInt32 const _m_statusProtectionFlapRear);
	UInt32 m_statusProtectionFlapRear() const;
	UInt32& m_statusProtectionFlapRear();

	void m_statusRearCamPosition(UInt32 const _m_statusRearCamPosition);
	UInt32 m_statusRearCamPosition() const;
	UInt32& m_statusRearCamPosition();

	void m_statusFrontCamPosition(UInt32 const _m_statusFrontCamPosition);
	UInt32 m_statusFrontCamPosition() const;
	UInt32& m_statusFrontCamPosition();





private:
	UInt32 m_m_statusProtectionFlapFront;
	UInt32 m_m_statusProtectionFlapRear;
	UInt32 m_m_statusRearCamPosition;
	UInt32 m_m_statusFrontCamPosition;

};


/**
* @class CPfDoorAndMirror
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfDoorAndMirror
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfDoorAndMirror();
	~CPfDoorAndMirror() = default;
	CPfDoorAndMirror(CPfDoorAndMirror const &x) = default;
	CPfDoorAndMirror(CPfDoorAndMirror &&x) = default;
	CPfDoorAndMirror& operator=(CPfDoorAndMirror const &x) = default;
	CPfDoorAndMirror& operator=(CPfDoorAndMirror &&x) = default;

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
	void set_key_val(CPfDoorAndMirror const* const _data) noexcept;



	void m_stateDoorSwitchFrontLeft(UInt32 const _m_stateDoorSwitchFrontLeft);
	UInt32 m_stateDoorSwitchFrontLeft() const;
	UInt32& m_stateDoorSwitchFrontLeft();

	void m_stateDoorSwitchFrontRight(UInt32 const _m_stateDoorSwitchFrontRight);
	UInt32 m_stateDoorSwitchFrontRight() const;
	UInt32& m_stateDoorSwitchFrontRight();

	void m_stateDoorSwitchRearLeft(UInt32 const _m_stateDoorSwitchRearLeft);
	UInt32 m_stateDoorSwitchRearLeft() const;
	UInt32& m_stateDoorSwitchRearLeft();

	void m_stateDoorSwitchRearRight(UInt32 const _m_stateDoorSwitchRearRight);
	UInt32 m_stateDoorSwitchRearRight() const;
	UInt32& m_stateDoorSwitchRearRight();

	void m_stateExteriorMirrorLeft(UInt32 const _m_stateExteriorMirrorLeft);
	UInt32 m_stateExteriorMirrorLeft() const;
	UInt32& m_stateExteriorMirrorLeft();

	void m_stateExteriorMirrorRight(UInt32 const _m_stateExteriorMirrorRight);
	UInt32 m_stateExteriorMirrorRight() const;
	UInt32& m_stateExteriorMirrorRight();

	void m_mirrFldState(UInt32 const _m_mirrFldState);
	UInt32 m_mirrFldState() const;
	UInt32& m_mirrFldState();

	void m_statusContactBootLid(UInt32 const _m_statusContactBootLid);
	UInt32 m_statusContactBootLid() const;
	UInt32& m_statusContactBootLid();

	void m_statusContactFrontLid(UInt32 const _m_statusContactFrontLid);
	UInt32 m_statusContactFrontLid() const;
	UInt32& m_statusContactFrontLid();

	void m_SrfOperateSts(UInt32 const _m_SrfOperateSts);
	UInt32 m_SrfOperateSts() const;
	UInt32& m_SrfOperateSts();





private:
	UInt32 m_m_stateDoorSwitchFrontLeft;
	UInt32 m_m_stateDoorSwitchFrontRight;
	UInt32 m_m_stateDoorSwitchRearLeft;
	UInt32 m_m_stateDoorSwitchRearRight;
	UInt32 m_m_stateExteriorMirrorLeft;
	UInt32 m_m_stateExteriorMirrorRight;
	UInt32 m_m_mirrFldState;
	UInt32 m_m_statusContactBootLid;
	UInt32 m_m_statusContactFrontLid;
	UInt32 m_m_SrfOperateSts;

};


/**
* @class CPfEnvironment
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfEnvironment
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt64);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfEnvironment();
	~CPfEnvironment() = default;
	CPfEnvironment(CPfEnvironment const &x) = default;
	CPfEnvironment(CPfEnvironment &&x) = default;
	CPfEnvironment& operator=(CPfEnvironment const &x) = default;
	CPfEnvironment& operator=(CPfEnvironment &&x) = default;

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
	void set_key_val(CPfEnvironment const* const _data) noexcept;



	void m_dynamometerMode(UInt32 const _m_dynamometerMode);
	UInt32 m_dynamometerMode() const;
	UInt32& m_dynamometerMode();

	void m_absoluteTimeUnix(UInt32 const _m_absoluteTimeUnix);
	UInt32 m_absoluteTimeUnix() const;
	UInt32& m_absoluteTimeUnix();

	void m_steeringWheelAngleSequenceCounter(UInt64 const _m_steeringWheelAngleSequenceCounter);
	UInt64 m_steeringWheelAngleSequenceCounter() const;
	UInt64& m_steeringWheelAngleSequenceCounter();





private:
	UInt32 m_m_dynamometerMode;
	UInt32 m_m_absoluteTimeUnix;
	UInt64 m_m_steeringWheelAngleSequenceCounter;

};


/**
* @class CPfGear
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfGear
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt16);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfGear();
	~CPfGear() = default;
	CPfGear(CPfGear const &x) = default;
	CPfGear(CPfGear &&x) = default;
	CPfGear& operator=(CPfGear const &x) = default;
	CPfGear& operator=(CPfGear &&x) = default;

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
	void set_key_val(CPfGear const* const _data) noexcept;



	void m_gearBoxType(UInt32 const _m_gearBoxType);
	UInt32 m_gearBoxType() const;
	UInt32& m_gearBoxType();

	void m_gearStatus(UInt32 const _m_gearStatus);
	UInt32 m_gearStatus() const;
	UInt32& m_gearStatus();

	void m_TargetGearStatus(UInt32 const _m_TargetGearStatus);
	UInt32 m_TargetGearStatus() const;
	UInt32& m_TargetGearStatus();

	void m_APAGearIntervention(Boolean const _m_APAGearIntervention);
	Boolean m_APAGearIntervention() const;
	Boolean& m_APAGearIntervention();

	void padding(UInt8 const _padding);
	UInt8 padding() const;
	UInt8& padding();

	void padding1(UInt16 const _padding1);
	UInt16 padding1() const;
	UInt16& padding1();





private:
	UInt32 m_m_gearBoxType;
	UInt32 m_m_gearStatus;
	UInt32 m_m_TargetGearStatus;
	Boolean m_m_APAGearIntervention;
	UInt8 m_padding;
	UInt16 m_padding1;

};


/**
* @class CPfGnssDateTime
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfGnssDateTime
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Float) + sizeof(UInt16) + sizeof(UInt16) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt16);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfGnssDateTime();
	~CPfGnssDateTime() = default;
	CPfGnssDateTime(CPfGnssDateTime const &x) = default;
	CPfGnssDateTime(CPfGnssDateTime &&x) = default;
	CPfGnssDateTime& operator=(CPfGnssDateTime const &x) = default;
	CPfGnssDateTime& operator=(CPfGnssDateTime &&x) = default;

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
	void set_key_val(CPfGnssDateTime const* const _data) noexcept;



	void m_gnssTmStmp(Float const _m_gnssTmStmp);
	Float m_gnssTmStmp() const;
	Float& m_gnssTmStmp();

	void m_gnssDateTimeMilliSecond(UInt16 const _m_gnssDateTimeMilliSecond);
	UInt16 m_gnssDateTimeMilliSecond() const;
	UInt16& m_gnssDateTimeMilliSecond();

	void m_gnssDateTimeYear(UInt16 const _m_gnssDateTimeYear);
	UInt16 m_gnssDateTimeYear() const;
	UInt16& m_gnssDateTimeYear();

	void m_gnssDateTimeSecond(UInt8 const _m_gnssDateTimeSecond);
	UInt8 m_gnssDateTimeSecond() const;
	UInt8& m_gnssDateTimeSecond();

	void m_gnssDateTimeMinute(UInt8 const _m_gnssDateTimeMinute);
	UInt8 m_gnssDateTimeMinute() const;
	UInt8& m_gnssDateTimeMinute();

	void m_gnssDateTimeMonth(UInt8 const _m_gnssDateTimeMonth);
	UInt8 m_gnssDateTimeMonth() const;
	UInt8& m_gnssDateTimeMonth();

	void m_gnssDateTimeDay(UInt8 const _m_gnssDateTimeDay);
	UInt8 m_gnssDateTimeDay() const;
	UInt8& m_gnssDateTimeDay();

	void m_gnssDateTimeHour(UInt8 const _m_gnssDateTimeHour);
	UInt8 m_gnssDateTimeHour() const;
	UInt8& m_gnssDateTimeHour();

	void m_gnssMasterCount1(UInt8 const _m_gnssMasterCount1);
	UInt8 m_gnssMasterCount1() const;
	UInt8& m_gnssMasterCount1();

	void m_gnssMasterCount2(UInt8 const _m_gnssMasterCount2);
	UInt8 m_gnssMasterCount2() const;
	UInt8& m_gnssMasterCount2();

	void m_gnssMasterCount3(UInt8 const _m_gnssMasterCount3);
	UInt8 m_gnssMasterCount3() const;
	UInt8& m_gnssMasterCount3();

	void m_gnssMasterCount4(UInt8 const _m_gnssMasterCount4);
	UInt8 m_gnssMasterCount4() const;
	UInt8& m_gnssMasterCount4();

	void padding1(UInt8 const _padding1);
	UInt8 padding1() const;
	UInt8& padding1();

	void padding(UInt16 const _padding);
	UInt16 padding() const;
	UInt16& padding();





private:
	Float m_m_gnssTmStmp;
	UInt16 m_m_gnssDateTimeMilliSecond;
	UInt16 m_m_gnssDateTimeYear;
	UInt8 m_m_gnssDateTimeSecond;
	UInt8 m_m_gnssDateTimeMinute;
	UInt8 m_m_gnssDateTimeMonth;
	UInt8 m_m_gnssDateTimeDay;
	UInt8 m_m_gnssDateTimeHour;
	UInt8 m_m_gnssMasterCount1;
	UInt8 m_m_gnssMasterCount2;
	UInt8 m_m_gnssMasterCount3;
	UInt8 m_m_gnssMasterCount4;
	UInt8 m_padding1;
	UInt16 m_padding;

};


/**
* @class CPfGnssLocalization
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfGnssLocalization
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(SInt32) + sizeof(UInt8) + sizeof(UInt8) + sizeof(UInt16) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfGnssLocalization();
	~CPfGnssLocalization() = default;
	CPfGnssLocalization(CPfGnssLocalization const &x) = default;
	CPfGnssLocalization(CPfGnssLocalization &&x) = default;
	CPfGnssLocalization& operator=(CPfGnssLocalization const &x) = default;
	CPfGnssLocalization& operator=(CPfGnssLocalization &&x) = default;

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
	void set_key_val(CPfGnssLocalization const* const _data) noexcept;



	void m_gnssAltitude(Float const _m_gnssAltitude);
	Float m_gnssAltitude() const;
	Float& m_gnssAltitude();

	void m_gnssHeading(Float const _m_gnssHeading);
	Float m_gnssHeading() const;
	Float& m_gnssHeading();

	void m_gnssLongitude(Float const _m_gnssLongitude);
	Float m_gnssLongitude() const;
	Float& m_gnssLongitude();

	void m_gnssLatitude(Float const _m_gnssLatitude);
	Float m_gnssLatitude() const;
	Float& m_gnssLatitude();

	void m_gnssLongitudeDeadReck(Float const _m_gnssLongitudeDeadReck);
	Float m_gnssLongitudeDeadReck() const;
	Float& m_gnssLongitudeDeadReck();

	void m_gnssLatitudeDeadReck(Float const _m_gnssLatitudeDeadReck);
	Float m_gnssLatitudeDeadReck() const;
	Float& m_gnssLatitudeDeadReck();

	void m_gnssVelOvrGrnd(Float const _m_gnssVelOvrGrnd);
	Float m_gnssVelOvrGrnd() const;
	Float& m_gnssVelOvrGrnd();

	void m_gnssSampleRate(SInt32 const _m_gnssSampleRate);
	SInt32 m_gnssSampleRate() const;
	SInt32& m_gnssSampleRate();

	void m_gnssVsblSat(UInt8 const _m_gnssVsblSat);
	UInt8 m_gnssVsblSat() const;
	UInt8& m_gnssVsblSat();

	void m_gnssTrackedSat(UInt8 const _m_gnssTrackedSat);
	UInt8 m_gnssTrackedSat() const;
	UInt8& m_gnssTrackedSat();

	void padding(UInt16 const _padding);
	UInt16 padding() const;
	UInt16& padding();

	void m_gnssVdop(Float const _m_gnssVdop);
	Float m_gnssVdop() const;
	Float& m_gnssVdop();

	void m_gnssHdop(Float const _m_gnssHdop);
	Float m_gnssHdop() const;
	Float& m_gnssHdop();

	void m_gnssPdop(Float const _m_gnssPdop);
	Float m_gnssPdop() const;
	Float& m_gnssPdop();

	void m_gnssPosFix(UInt32 const _m_gnssPosFix);
	UInt32 m_gnssPosFix() const;
	UInt32& m_gnssPosFix();





private:
	Float m_m_gnssAltitude;
	Float m_m_gnssHeading;
	Float m_m_gnssLongitude;
	Float m_m_gnssLatitude;
	Float m_m_gnssLongitudeDeadReck;
	Float m_m_gnssLatitudeDeadReck;
	Float m_m_gnssVelOvrGrnd;
	SInt32 m_m_gnssSampleRate;
	UInt8 m_m_gnssVsblSat;
	UInt8 m_m_gnssTrackedSat;
	UInt16 m_padding;
	Float m_m_gnssVdop;
	Float m_m_gnssHdop;
	Float m_m_gnssPdop;
	UInt32 m_m_gnssPosFix;

};


/**
* @class CPfHil
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfHil
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt8);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfHil();
	~CPfHil() = default;
	CPfHil(CPfHil const &x) = default;
	CPfHil(CPfHil &&x) = default;
	CPfHil& operator=(CPfHil const &x) = default;
	CPfHil& operator=(CPfHil &&x) = default;

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
	void set_key_val(CPfHil const* const _data) noexcept;



	void m_hilMode(UInt8 const _m_hilMode);
	UInt8 m_hilMode() const;
	UInt8& m_hilMode();





private:
	UInt8 m_m_hilMode;

};


/**
* @class CPfOdometry
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfOdometry
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(UInt64) + sizeof(UInt64) + sizeof(UInt64) + sizeof(UInt64) + sizeof(UInt16) + sizeof(UInt16) + sizeof(UInt16) + sizeof(UInt16) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfOdometry();
	~CPfOdometry() = default;
	CPfOdometry(CPfOdometry const &x) = default;
	CPfOdometry(CPfOdometry &&x) = default;
	CPfOdometry& operator=(CPfOdometry const &x) = default;
	CPfOdometry& operator=(CPfOdometry &&x) = default;

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
	void set_key_val(CPfOdometry const* const _data) noexcept;



	void m_vehicleVelocity(Float const _m_vehicleVelocity);
	Float m_vehicleVelocity() const;
	Float& m_vehicleVelocity();

	void m_vehicleVelocityDisplay(Float const _m_vehicleVelocityDisplay);
	Float m_vehicleVelocityDisplay() const;
	Float& m_vehicleVelocityDisplay();

	void m_brktrq(Float const _m_brktrq);
	Float m_brktrq() const;
	Float& m_brktrq();

	void m_longitudinalAcceleration(Float const _m_longitudinalAcceleration);
	Float m_longitudinalAcceleration() const;
	Float& m_longitudinalAcceleration();

	void m_lateralAcceleration(Float const _m_lateralAcceleration);
	Float m_lateralAcceleration() const;
	Float& m_lateralAcceleration();

	void m_yawRate(Float const _m_yawRate);
	Float m_yawRate() const;
	Float& m_yawRate();

	void m_wheelImpCtrFLTimestamp(UInt64 const _m_wheelImpCtrFLTimestamp);
	UInt64 m_wheelImpCtrFLTimestamp() const;
	UInt64& m_wheelImpCtrFLTimestamp();

	void m_wheelImpCtrFRTimestamp(UInt64 const _m_wheelImpCtrFRTimestamp);
	UInt64 m_wheelImpCtrFRTimestamp() const;
	UInt64& m_wheelImpCtrFRTimestamp();

	void m_wheelImpCtrRLTimestamp(UInt64 const _m_wheelImpCtrRLTimestamp);
	UInt64 m_wheelImpCtrRLTimestamp() const;
	UInt64& m_wheelImpCtrRLTimestamp();

	void m_wheelImpCtrRRTimestamp(UInt64 const _m_wheelImpCtrRRTimestamp);
	UInt64 m_wheelImpCtrRRTimestamp() const;
	UInt64& m_wheelImpCtrRRTimestamp();

	void m_wheelImpCtrFL(UInt16 const _m_wheelImpCtrFL);
	UInt16 m_wheelImpCtrFL() const;
	UInt16& m_wheelImpCtrFL();

	void m_wheelImpCtrFR(UInt16 const _m_wheelImpCtrFR);
	UInt16 m_wheelImpCtrFR() const;
	UInt16& m_wheelImpCtrFR();

	void m_wheelImpCtrRL(UInt16 const _m_wheelImpCtrRL);
	UInt16 m_wheelImpCtrRL() const;
	UInt16& m_wheelImpCtrRL();

	void m_wheelImpCtrRR(UInt16 const _m_wheelImpCtrRR);
	UInt16 m_wheelImpCtrRR() const;
	UInt16& m_wheelImpCtrRR();

	void m_vehicleDrvDir(UInt32 const _m_vehicleDrvDir);
	UInt32 m_vehicleDrvDir() const;
	UInt32& m_vehicleDrvDir();

	void m_qualifierVehicleVelocity(UInt32 const _m_qualifierVehicleVelocity);
	UInt32 m_qualifierVehicleVelocity() const;
	UInt32& m_qualifierVehicleVelocity();

	void m_wheelDrvDirFL(UInt32 const _m_wheelDrvDirFL);
	UInt32 m_wheelDrvDirFL() const;
	UInt32& m_wheelDrvDirFL();

	void m_wheelDrvDirFR(UInt32 const _m_wheelDrvDirFR);
	UInt32 m_wheelDrvDirFR() const;
	UInt32& m_wheelDrvDirFR();

	void m_wheelDrvDirRL(UInt32 const _m_wheelDrvDirRL);
	UInt32 m_wheelDrvDirRL() const;
	UInt32& m_wheelDrvDirRL();

	void m_wheelDrvDirRR(UInt32 const _m_wheelDrvDirRR);
	UInt32 m_wheelDrvDirRR() const;
	UInt32& m_wheelDrvDirRR();

	void m_wheelRotationFLQualifier(UInt32 const _m_wheelRotationFLQualifier);
	UInt32 m_wheelRotationFLQualifier() const;
	UInt32& m_wheelRotationFLQualifier();

	void m_wheelRotationFRQualifier(UInt32 const _m_wheelRotationFRQualifier);
	UInt32 m_wheelRotationFRQualifier() const;
	UInt32& m_wheelRotationFRQualifier();

	void m_wheelRotationRLQualifier(UInt32 const _m_wheelRotationRLQualifier);
	UInt32 m_wheelRotationRLQualifier() const;
	UInt32& m_wheelRotationRLQualifier();

	void m_wheelRotationRRQualifier(UInt32 const _m_wheelRotationRRQualifier);
	UInt32 m_wheelRotationRRQualifier() const;
	UInt32& m_wheelRotationRRQualifier();

	void m_lateralAccelerationQualifier(UInt32 const _m_lateralAccelerationQualifier);
	UInt32 m_lateralAccelerationQualifier() const;
	UInt32& m_lateralAccelerationQualifier();

	void m_longitudinalAccelerationQualifier(UInt32 const _m_longitudinalAccelerationQualifier);
	UInt32 m_longitudinalAccelerationQualifier() const;
	UInt32& m_longitudinalAccelerationQualifier();

	void m_qualifierYawVelocityVehicle(UInt32 const _m_qualifierYawVelocityVehicle);
	UInt32 m_qualifierYawVelocityVehicle() const;
	UInt32& m_qualifierYawVelocityVehicle();

	void m_PboxImuAcc_X(Float const _m_PboxImuAcc_X);
	Float m_PboxImuAcc_X() const;
	Float& m_PboxImuAcc_X();

	void m_PboxImuAcc_Y(Float const _m_PboxImuAcc_Y);
	Float m_PboxImuAcc_Y() const;
	Float& m_PboxImuAcc_Y();

	void m_PboxImuAcc_Z(Float const _m_PboxImuAcc_Z);
	Float m_PboxImuAcc_Z() const;
	Float& m_PboxImuAcc_Z();

	void m_PboxImuGyro_X(Float const _m_PboxImuGyro_X);
	Float m_PboxImuGyro_X() const;
	Float& m_PboxImuGyro_X();

	void m_PboxImuGyro_Y(Float const _m_PboxImuGyro_Y);
	Float m_PboxImuGyro_Y() const;
	Float& m_PboxImuGyro_Y();

	void m_PboxImuGyro_Z(Float const _m_PboxImuGyro_Z);
	Float m_PboxImuGyro_Z() const;
	Float& m_PboxImuGyro_Z();

	void m_PitchInstallAngle_f32(Float const _m_PitchInstallAngle_f32);
	Float m_PitchInstallAngle_f32() const;
	Float& m_PitchInstallAngle_f32();

	void m_reserve1_f32(Float const _m_reserve1_f32);
	Float m_reserve1_f32() const;
	Float& m_reserve1_f32();

	void m_reserve2_f32(Float const _m_reserve2_f32);
	Float m_reserve2_f32() const;
	Float& m_reserve2_f32();

	void m_reserve3_f32(Float const _m_reserve3_f32);
	Float m_reserve3_f32() const;
	Float& m_reserve3_f32();





private:
	Float m_m_vehicleVelocity;
	Float m_m_vehicleVelocityDisplay;
	Float m_m_brktrq;
	Float m_m_longitudinalAcceleration;
	Float m_m_lateralAcceleration;
	Float m_m_yawRate;
	UInt64 m_m_wheelImpCtrFLTimestamp;
	UInt64 m_m_wheelImpCtrFRTimestamp;
	UInt64 m_m_wheelImpCtrRLTimestamp;
	UInt64 m_m_wheelImpCtrRRTimestamp;
	UInt16 m_m_wheelImpCtrFL;
	UInt16 m_m_wheelImpCtrFR;
	UInt16 m_m_wheelImpCtrRL;
	UInt16 m_m_wheelImpCtrRR;
	UInt32 m_m_vehicleDrvDir;
	UInt32 m_m_qualifierVehicleVelocity;
	UInt32 m_m_wheelDrvDirFL;
	UInt32 m_m_wheelDrvDirFR;
	UInt32 m_m_wheelDrvDirRL;
	UInt32 m_m_wheelDrvDirRR;
	UInt32 m_m_wheelRotationFLQualifier;
	UInt32 m_m_wheelRotationFRQualifier;
	UInt32 m_m_wheelRotationRLQualifier;
	UInt32 m_m_wheelRotationRRQualifier;
	UInt32 m_m_lateralAccelerationQualifier;
	UInt32 m_m_longitudinalAccelerationQualifier;
	UInt32 m_m_qualifierYawVelocityVehicle;
	Float m_m_PboxImuAcc_X;
	Float m_m_PboxImuAcc_Y;
	Float m_m_PboxImuAcc_Z;
	Float m_m_PboxImuGyro_X;
	Float m_m_PboxImuGyro_Y;
	Float m_m_PboxImuGyro_Z;
	Float m_m_PitchInstallAngle_f32;
	Float m_m_reserve1_f32;
	Float m_m_reserve2_f32;
	Float m_m_reserve3_f32;

};


/**
* @class CPfPmaFixedPoint
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfPmaFixedPoint
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Float) + sizeof(UInt16) + sizeof(UInt16) + sizeof(UInt16) + sizeof(UInt16) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(UInt16) + sizeof(SInt8) + sizeof(UInt8) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfPmaFixedPoint();
	~CPfPmaFixedPoint() = default;
	CPfPmaFixedPoint(CPfPmaFixedPoint const &x) = default;
	CPfPmaFixedPoint(CPfPmaFixedPoint &&x) = default;
	CPfPmaFixedPoint& operator=(CPfPmaFixedPoint const &x) = default;
	CPfPmaFixedPoint& operator=(CPfPmaFixedPoint &&x) = default;

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
	void set_key_val(CPfPmaFixedPoint const* const _data) noexcept;



	void m_steeringWheelAngle(Float const _m_steeringWheelAngle);
	Float m_steeringWheelAngle() const;
	Float& m_steeringWheelAngle();

	void m_vehSpeed(UInt16 const _m_vehSpeed);
	UInt16 m_vehSpeed() const;
	UInt16& m_vehSpeed();

	void m_wheelRotationFL(UInt16 const _m_wheelRotationFL);
	UInt16 m_wheelRotationFL() const;
	UInt16& m_wheelRotationFL();

	void m_wheelRotationFR(UInt16 const _m_wheelRotationFR);
	UInt16 m_wheelRotationFR() const;
	UInt16& m_wheelRotationFR();

	void m_wheelRotationRL(UInt16 const _m_wheelRotationRL);
	UInt16 m_wheelRotationRL() const;
	UInt16& m_wheelRotationRL();

	void m_wheelSpeedFL(Float const _m_wheelSpeedFL);
	Float m_wheelSpeedFL() const;
	Float& m_wheelSpeedFL();

	void m_wheelSpeedFR(Float const _m_wheelSpeedFR);
	Float m_wheelSpeedFR() const;
	Float& m_wheelSpeedFR();

	void m_wheelSpeedRL(Float const _m_wheelSpeedRL);
	Float m_wheelSpeedRL() const;
	Float& m_wheelSpeedRL();

	void m_wheelSpeedRR(Float const _m_wheelSpeedRR);
	Float m_wheelSpeedRR() const;
	Float& m_wheelSpeedRR();

	void m_wheelRotationRR(UInt16 const _m_wheelRotationRR);
	UInt16 m_wheelRotationRR() const;
	UInt16& m_wheelRotationRR();

	void m_outsideTemp(SInt8 const _m_outsideTemp);
	SInt8 m_outsideTemp() const;
	SInt8& m_outsideTemp();

	void padding(UInt8 const _padding);
	UInt8 padding() const;
	UInt8& padding();

	void m_vehicleStopState(UInt32 const _m_vehicleStopState);
	UInt32 m_vehicleStopState() const;
	UInt32& m_vehicleStopState();





private:
	Float m_m_steeringWheelAngle;
	UInt16 m_m_vehSpeed;
	UInt16 m_m_wheelRotationFL;
	UInt16 m_m_wheelRotationFR;
	UInt16 m_m_wheelRotationRL;
	Float m_m_wheelSpeedFL;
	Float m_m_wheelSpeedFR;
	Float m_m_wheelSpeedRL;
	Float m_m_wheelSpeedRR;
	UInt16 m_m_wheelRotationRR;
	SInt8 m_m_outsideTemp;
	UInt8 m_padding;
	UInt32 m_m_vehicleStopState;

};


/**
* @class CPfSteering
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfSteering
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt8) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(UInt32) + sizeof(UInt64) + sizeof(UInt32) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfSteering();
	~CPfSteering() = default;
	CPfSteering(CPfSteering const &x) = default;
	CPfSteering(CPfSteering &&x) = default;
	CPfSteering& operator=(CPfSteering const &x) = default;
	CPfSteering& operator=(CPfSteering &&x) = default;

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
	void set_key_val(CPfSteering const* const _data) noexcept;



	void m_frontWheelAngle(Float const _m_frontWheelAngle);
	Float m_frontWheelAngle() const;
	Float& m_frontWheelAngle();

	void m_frontWheelAngleOffset(Float const _m_frontWheelAngleOffset);
	Float m_frontWheelAngleOffset() const;
	Float& m_frontWheelAngleOffset();

	void m_steeringWheelAngle(Float const _m_steeringWheelAngle);
	Float m_steeringWheelAngle() const;
	Float& m_steeringWheelAngle();

	void m_steeringWheelAngleSpd(Float const _m_steeringWheelAngleSpd);
	Float m_steeringWheelAngleSpd() const;
	Float& m_steeringWheelAngleSpd();

	void m_rearAxleSteeringAngle(Float const _m_rearAxleSteeringAngle);
	Float m_rearAxleSteeringAngle() const;
	Float& m_rearAxleSteeringAngle();

	void m_frontWheelAngleValid(Boolean const _m_frontWheelAngleValid);
	Boolean m_frontWheelAngleValid() const;
	Boolean& m_frontWheelAngleValid();

	void m_frontWheelAngleOffsetValid(Boolean const _m_frontWheelAngleOffsetValid);
	Boolean m_frontWheelAngleOffsetValid() const;
	Boolean& m_frontWheelAngleOffsetValid();

	void m_rearAxleSteeringAngleValid(Boolean const _m_rearAxleSteeringAngleValid);
	Boolean m_rearAxleSteeringAngleValid() const;
	Boolean& m_rearAxleSteeringAngleValid();

	void padding(UInt8 const _padding);
	UInt8 padding() const;
	UInt8& padding();

	void m_qualifierSteeringAngle(UInt32 const _m_qualifierSteeringAngle);
	UInt32 m_qualifierSteeringAngle() const;
	UInt32& m_qualifierSteeringAngle();

	void m_frontWheelAngleStatus(UInt32 const _m_frontWheelAngleStatus);
	UInt32 m_frontWheelAngleStatus() const;
	UInt32& m_frontWheelAngleStatus();

	void m_steerWhlTrq(Float const _m_steerWhlTrq);
	Float m_steerWhlTrq() const;
	Float& m_steerWhlTrq();

	void m_steerWhlTrqOffset(Float const _m_steerWhlTrqOffset);
	Float m_steerWhlTrqOffset() const;
	Float& m_steerWhlTrqOffset();

	void m_steerWhlTrqHys(Float const _m_steerWhlTrqHys);
	Float m_steerWhlTrqHys() const;
	Float& m_steerWhlTrqHys();

	void m_steerWhlRimTrqHys(Float const _m_steerWhlRimTrqHys);
	Float m_steerWhlRimTrqHys() const;
	Float& m_steerWhlRimTrqHys();

	void m_epsStatus(UInt32 const _m_epsStatus);
	UInt32 m_epsStatus() const;
	UInt32& m_epsStatus();

	void m_frontWheelAngleTimestamp(UInt64 const _m_frontWheelAngleTimestamp);
	UInt64 m_frontWheelAngleTimestamp() const;
	UInt64& m_frontWheelAngleTimestamp();

	void m_epsStatusRas(UInt32 const _m_epsStatusRas);
	UInt32 m_epsStatusRas() const;
	UInt32& m_epsStatusRas();

	void m_rearWheelAngleStatus(UInt32 const _m_rearWheelAngleStatus);
	UInt32 m_rearWheelAngleStatus() const;
	UInt32& m_rearWheelAngleStatus();





private:
	Float m_m_frontWheelAngle;
	Float m_m_frontWheelAngleOffset;
	Float m_m_steeringWheelAngle;
	Float m_m_steeringWheelAngleSpd;
	Float m_m_rearAxleSteeringAngle;
	Boolean m_m_frontWheelAngleValid;
	Boolean m_m_frontWheelAngleOffsetValid;
	Boolean m_m_rearAxleSteeringAngleValid;
	UInt8 m_padding;
	UInt32 m_m_qualifierSteeringAngle;
	UInt32 m_m_frontWheelAngleStatus;
	Float m_m_steerWhlTrq;
	Float m_m_steerWhlTrqOffset;
	Float m_m_steerWhlTrqHys;
	Float m_m_steerWhlRimTrqHys;
	UInt32 m_m_epsStatus;
	UInt64 m_m_frontWheelAngleTimestamp;
	UInt32 m_m_epsStatusRas;
	UInt32 m_m_rearWheelAngleStatus;

};


/**
* @class CPfTirePress
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfTirePress
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfTirePress();
	~CPfTirePress() = default;
	CPfTirePress(CPfTirePress const &x) = default;
	CPfTirePress(CPfTirePress &&x) = default;
	CPfTirePress& operator=(CPfTirePress const &x) = default;
	CPfTirePress& operator=(CPfTirePress &&x) = default;

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
	void set_key_val(CPfTirePress const* const _data) noexcept;



	void m_tirePressFL(Float const _m_tirePressFL);
	Float m_tirePressFL() const;
	Float& m_tirePressFL();

	void m_tirePressFR(Float const _m_tirePressFR);
	Float m_tirePressFR() const;
	Float& m_tirePressFR();

	void m_tirePressRL(Float const _m_tirePressRL);
	Float m_tirePressRL() const;
	Float& m_tirePressRL();

	void m_tirePressRR(Float const _m_tirePressRR);
	Float m_tirePressRR() const;
	Float& m_tirePressRR();

	void m_tirePressQualifier(UInt32 const _m_tirePressQualifier);
	UInt32 m_tirePressQualifier() const;
	UInt32& m_tirePressQualifier();





private:
	Float m_m_tirePressFL;
	Float m_m_tirePressFR;
	Float m_m_tirePressRL;
	Float m_m_tirePressRR;
	UInt32 m_m_tirePressQualifier;

};


/**
* @class CPfTrailer
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfTrailer
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Boolean) + sizeof(Boolean);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfTrailer();
	~CPfTrailer() = default;
	CPfTrailer(CPfTrailer const &x) = default;
	CPfTrailer(CPfTrailer &&x) = default;
	CPfTrailer& operator=(CPfTrailer const &x) = default;
	CPfTrailer& operator=(CPfTrailer &&x) = default;

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
	void set_key_val(CPfTrailer const* const _data) noexcept;



	void m_trailerState(Boolean const _m_trailerState);
	Boolean m_trailerState() const;
	Boolean& m_trailerState();

	void m_trailerHitchPresent(Boolean const _m_trailerHitchPresent);
	Boolean m_trailerHitchPresent() const;
	Boolean& m_trailerHitchPresent();





private:
	Boolean m_m_trailerState;
	Boolean m_m_trailerHitchPresent;

};


/**
* @class CPfVehicleInfo
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfVehicleInfo
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt8) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt32) + sizeof(Float) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt16) + sizeof(UInt32) + sizeof(UInt32) + sizeof(UInt32) + sizeof(Float) + sizeof(Boolean) + sizeof(Boolean) + sizeof(UInt8) + sizeof(std::array<UInt8,8>) + sizeof(Boolean) + sizeof(Boolean);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfVehicleInfo();
	~CPfVehicleInfo() = default;
	CPfVehicleInfo(CPfVehicleInfo const &x) = default;
	CPfVehicleInfo(CPfVehicleInfo &&x) = default;
	CPfVehicleInfo& operator=(CPfVehicleInfo const &x) = default;
	CPfVehicleInfo& operator=(CPfVehicleInfo &&x) = default;

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
	void set_key_val(CPfVehicleInfo const* const _data) noexcept;



	void m_ignSwState(UInt32 const _m_ignSwState);
	UInt32 m_ignSwState() const;
	UInt32& m_ignSwState();

	void m_indicatorStatus(UInt32 const _m_indicatorStatus);
	UInt32 m_indicatorStatus() const;
	UInt32& m_indicatorStatus();

	void m_turnSWStatus(UInt32 const _m_turnSWStatus);
	UInt32 m_turnSWStatus() const;
	UInt32& m_turnSWStatus();

	void m_uBatt(UInt8 const _m_uBatt);
	UInt8 m_uBatt() const;
	UInt8& m_uBatt();

	void m_KL15_status(Boolean const _m_KL15_status);
	Boolean m_KL15_status() const;
	Boolean& m_KL15_status();

	void m_lowBeamLeft(Boolean const _m_lowBeamLeft);
	Boolean m_lowBeamLeft() const;
	Boolean& m_lowBeamLeft();

	void m_lowBeamRight(Boolean const _m_lowBeamRight);
	Boolean m_lowBeamRight() const;
	Boolean& m_lowBeamRight();

	void m_highBeamLeft(Boolean const _m_highBeamLeft);
	Boolean m_highBeamLeft() const;
	Boolean& m_highBeamLeft();

	void m_highBeamRight(Boolean const _m_highBeamRight);
	Boolean m_highBeamRight() const;
	Boolean& m_highBeamRight();

	void m_fogLampFront(Boolean const _m_fogLampFront);
	Boolean m_fogLampFront() const;
	Boolean& m_fogLampFront();

	void m_fogLampRear(Boolean const _m_fogLampRear);
	Boolean m_fogLampRear() const;
	Boolean& m_fogLampRear();

	void m_brkLightAct(Boolean const _m_brkLightAct);
	Boolean m_brkLightAct() const;
	Boolean& m_brkLightAct();

	void m_DriverSeatBeltNotBuckled(Boolean const _m_DriverSeatBeltNotBuckled);
	Boolean m_DriverSeatBeltNotBuckled() const;
	Boolean& m_DriverSeatBeltNotBuckled();

	void m_externalFuncAct(Boolean const _m_externalFuncAct);
	Boolean m_externalFuncAct() const;
	Boolean& m_externalFuncAct();

	void m_chargeState(Boolean const _m_chargeState);
	Boolean m_chargeState() const;
	Boolean& m_chargeState();

	void m_engineStart(UInt32 const _m_engineStart);
	UInt32 m_engineStart() const;
	UInt32& m_engineStart();

	void m_accPedalPosition(Float const _m_accPedalPosition);
	Float m_accPedalPosition() const;
	Float& m_accPedalPosition();

	void m_engineIsRunning(Boolean const _m_engineIsRunning);
	Boolean m_engineIsRunning() const;
	Boolean& m_engineIsRunning();

	void m_accPedalIntervened(Boolean const _m_accPedalIntervened);
	Boolean m_accPedalIntervened() const;
	Boolean& m_accPedalIntervened();

	void padding(UInt16 const _padding);
	UInt16 padding() const;
	UInt16& padding();

	void m_VehDriveMode(UInt32 const _m_VehDriveMode);
	UInt32 m_VehDriveMode() const;
	UInt32& m_VehDriveMode();

	void m_bgLightState(UInt32 const _m_bgLightState);
	UInt32 m_bgLightState() const;
	UInt32& m_bgLightState();

	void m_dayNightModeState(UInt32 const _m_dayNightModeState);
	UInt32 m_dayNightModeState() const;
	UInt32& m_dayNightModeState();

	void m_motorTorqFeedback_nm(Float const _m_motorTorqFeedback_nm);
	Float m_motorTorqFeedback_nm() const;
	Float& m_motorTorqFeedback_nm();

	void m_engineIsReady(Boolean const _m_engineIsReady);
	Boolean m_engineIsReady() const;
	Boolean& m_engineIsReady();

	void m_CarModeValid(Boolean const _m_CarModeValid);
	Boolean m_CarModeValid() const;
	Boolean& m_CarModeValid();

	void m_CarMode(UInt8 const _m_CarMode);
	UInt8 m_CarMode() const;
	UInt8& m_CarMode();

	void m_LicenseNum(std::array<UInt8,8> const &_m_LicenseNum);
	void m_LicenseNum(std::array<UInt8,8> &&_m_LicenseNum);
	std::array<UInt8,8> const& m_LicenseNum() const;
	std::array<UInt8,8>& m_LicenseNum();

	void APASoftKeyRequest(Boolean const _APASoftKeyRequest);
	Boolean APASoftKeyRequest() const;
	Boolean& APASoftKeyRequest();

	void APASoftKeyRequestNoReverse(Boolean const _APASoftKeyRequestNoReverse);
	Boolean APASoftKeyRequestNoReverse() const;
	Boolean& APASoftKeyRequestNoReverse();





private:
	UInt32 m_m_ignSwState;
	UInt32 m_m_indicatorStatus;
	UInt32 m_m_turnSWStatus;
	UInt8 m_m_uBatt;
	Boolean m_m_KL15_status;
	Boolean m_m_lowBeamLeft;
	Boolean m_m_lowBeamRight;
	Boolean m_m_highBeamLeft;
	Boolean m_m_highBeamRight;
	Boolean m_m_fogLampFront;
	Boolean m_m_fogLampRear;
	Boolean m_m_brkLightAct;
	Boolean m_m_DriverSeatBeltNotBuckled;
	Boolean m_m_externalFuncAct;
	Boolean m_m_chargeState;
	UInt32 m_m_engineStart;
	Float m_m_accPedalPosition;
	Boolean m_m_engineIsRunning;
	Boolean m_m_accPedalIntervened;
	UInt16 m_padding;
	UInt32 m_m_VehDriveMode;
	UInt32 m_m_bgLightState;
	UInt32 m_m_dayNightModeState;
	Float m_m_motorTorqFeedback_nm;
	Boolean m_m_engineIsReady;
	Boolean m_m_CarModeValid;
	UInt8 m_m_CarMode;
	std::array<UInt8,8> m_m_LicenseNum;
	Boolean m_APASoftKeyRequest;
	Boolean m_APASoftKeyRequestNoReverse;

};


/**
* @class CPfVehicleLevel
* @brief A class as the datatype for data exchange.
* @note
*/

class CPfVehicleLevel
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(Float) + sizeof(UInt32);
	static constexpr bool IS_DATA_PADDING = true;
	static constexpr bool IS_ID_DEFINED = false;

	CPfVehicleLevel();
	~CPfVehicleLevel() = default;
	CPfVehicleLevel(CPfVehicleLevel const &x) = default;
	CPfVehicleLevel(CPfVehicleLevel &&x) = default;
	CPfVehicleLevel& operator=(CPfVehicleLevel const &x) = default;
	CPfVehicleLevel& operator=(CPfVehicleLevel &&x) = default;

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
	void set_key_val(CPfVehicleLevel const* const _data) noexcept;



	void m_relativeVehicleLevel(Float const _m_relativeVehicleLevel);
	Float m_relativeVehicleLevel() const;
	Float& m_relativeVehicleLevel();

	void m_vehicleLevelFL(Float const _m_vehicleLevelFL);
	Float m_vehicleLevelFL() const;
	Float& m_vehicleLevelFL();

	void m_vehicleLevelFR(Float const _m_vehicleLevelFR);
	Float m_vehicleLevelFR() const;
	Float& m_vehicleLevelFR();

	void m_vehicleLevelRL(Float const _m_vehicleLevelRL);
	Float m_vehicleLevelRL() const;
	Float& m_vehicleLevelRL();

	void m_vehicleLevelRR(Float const _m_vehicleLevelRR);
	Float m_vehicleLevelRR() const;
	Float& m_vehicleLevelRR();

	void m_qualifierVehicleLevel(UInt32 const _m_qualifierVehicleLevel);
	UInt32 m_qualifierVehicleLevel() const;
	UInt32& m_qualifierVehicleLevel();





private:
	Float m_m_relativeVehicleLevel;
	Float m_m_vehicleLevelFL;
	Float m_m_vehicleLevelFR;
	Float m_m_vehicleLevelRL;
	Float m_m_vehicleLevelRR;
	UInt32 m_m_qualifierVehicleLevel;

};


/**
* @class ValInOutputCpj
* @brief A class as the datatype for data exchange.
* @note
*/

class ValInOutputCpj
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(CCpjIHU) + sizeof(CCpjTel) + sizeof(CCpjEps) + sizeof(CCpjLongCtrl) + sizeof(CCpjRctaWarning) + sizeof(CCpjFctaWarning) + sizeof(CCpjVehInfo) + sizeof(CCpjFusaInfo) + sizeof(CCpjTDCU) + sizeof(CCpjLDCU) + sizeof(CCPJFCU) + sizeof(CCPJIPB) + sizeof(CCPJLA1) + sizeof(CCPJLA2) + sizeof(CCPJRDCU) + sizeof(CCPJRWS) + sizeof(CCPJSRS) + sizeof(CCPJTBOX) + sizeof(CCPJXPU) + sizeof(CPfGnssLocalization) + sizeof(CPfGnssDateTime) + sizeof(UInt64);
	static constexpr bool IS_DATA_PADDING = true || (CCpjIHU::DATA_SIZE != sizeof(CCpjIHU)) || (CCpjIHU::IS_DATA_PADDING) || (CCpjTel::DATA_SIZE != sizeof(CCpjTel)) || (CCpjTel::IS_DATA_PADDING) || (CCpjEps::DATA_SIZE != sizeof(CCpjEps)) || (CCpjEps::IS_DATA_PADDING) || (CCpjLongCtrl::DATA_SIZE != sizeof(CCpjLongCtrl)) || (CCpjLongCtrl::IS_DATA_PADDING) || (CCpjRctaWarning::DATA_SIZE != sizeof(CCpjRctaWarning)) || (CCpjRctaWarning::IS_DATA_PADDING) || (CCpjFctaWarning::DATA_SIZE != sizeof(CCpjFctaWarning)) || (CCpjFctaWarning::IS_DATA_PADDING) || (CCpjVehInfo::DATA_SIZE != sizeof(CCpjVehInfo)) || (CCpjVehInfo::IS_DATA_PADDING) || (CCpjFusaInfo::DATA_SIZE != sizeof(CCpjFusaInfo)) || (CCpjFusaInfo::IS_DATA_PADDING) || (CCpjTDCU::DATA_SIZE != sizeof(CCpjTDCU)) || (CCpjTDCU::IS_DATA_PADDING) || (CCpjLDCU::DATA_SIZE != sizeof(CCpjLDCU)) || (CCpjLDCU::IS_DATA_PADDING) || (CCPJFCU::DATA_SIZE != sizeof(CCPJFCU)) || (CCPJFCU::IS_DATA_PADDING) || (CCPJIPB::DATA_SIZE != sizeof(CCPJIPB)) || (CCPJIPB::IS_DATA_PADDING) || (CCPJLA1::DATA_SIZE != sizeof(CCPJLA1)) || (CCPJLA1::IS_DATA_PADDING) || (CCPJLA2::DATA_SIZE != sizeof(CCPJLA2)) || (CCPJLA2::IS_DATA_PADDING) || (CCPJRDCU::DATA_SIZE != sizeof(CCPJRDCU)) || (CCPJRDCU::IS_DATA_PADDING) || (CCPJRWS::DATA_SIZE != sizeof(CCPJRWS)) || (CCPJRWS::IS_DATA_PADDING) || (CCPJSRS::DATA_SIZE != sizeof(CCPJSRS)) || (CCPJSRS::IS_DATA_PADDING) || (CCPJTBOX::DATA_SIZE != sizeof(CCPJTBOX)) || (CCPJTBOX::IS_DATA_PADDING) || (CCPJXPU::DATA_SIZE != sizeof(CCPJXPU)) || (CCPJXPU::IS_DATA_PADDING) || (CPfGnssLocalization::DATA_SIZE != sizeof(CPfGnssLocalization)) || (CPfGnssLocalization::IS_DATA_PADDING) || (CPfGnssDateTime::DATA_SIZE != sizeof(CPfGnssDateTime)) || (CPfGnssDateTime::IS_DATA_PADDING);
	static constexpr bool IS_ID_DEFINED = false;

	ValInOutputCpj();
	~ValInOutputCpj() = default;
	ValInOutputCpj(ValInOutputCpj const &x) = default;
	ValInOutputCpj(ValInOutputCpj &&x) = default;
	ValInOutputCpj& operator=(ValInOutputCpj const &x) = default;
	ValInOutputCpj& operator=(ValInOutputCpj &&x) = default;

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
	void set_key_val(ValInOutputCpj const* const _data) noexcept;



	void m_IHU(CCpjIHU const &_m_IHU);
	void m_IHU(CCpjIHU &&_m_IHU);
	CCpjIHU const& m_IHU() const;
	CCpjIHU& m_IHU();

	void m_TEL(CCpjTel const &_m_TEL);
	void m_TEL(CCpjTel &&_m_TEL);
	CCpjTel const& m_TEL() const;
	CCpjTel& m_TEL();

	void m_Eps(CCpjEps const &_m_Eps);
	void m_Eps(CCpjEps &&_m_Eps);
	CCpjEps const& m_Eps() const;
	CCpjEps& m_Eps();

	void m_LongCtrl(CCpjLongCtrl const &_m_LongCtrl);
	void m_LongCtrl(CCpjLongCtrl &&_m_LongCtrl);
	CCpjLongCtrl const& m_LongCtrl() const;
	CCpjLongCtrl& m_LongCtrl();

	void m_rctaWarning(CCpjRctaWarning const &_m_rctaWarning);
	void m_rctaWarning(CCpjRctaWarning &&_m_rctaWarning);
	CCpjRctaWarning const& m_rctaWarning() const;
	CCpjRctaWarning& m_rctaWarning();

	void m_fctaWarning(CCpjFctaWarning const &_m_fctaWarning);
	void m_fctaWarning(CCpjFctaWarning &&_m_fctaWarning);
	CCpjFctaWarning const& m_fctaWarning() const;
	CCpjFctaWarning& m_fctaWarning();

	void m_vehInfo(CCpjVehInfo const &_m_vehInfo);
	void m_vehInfo(CCpjVehInfo &&_m_vehInfo);
	CCpjVehInfo const& m_vehInfo() const;
	CCpjVehInfo& m_vehInfo();

	void m_FusaInfo(CCpjFusaInfo const &_m_FusaInfo);
	void m_FusaInfo(CCpjFusaInfo &&_m_FusaInfo);
	CCpjFusaInfo const& m_FusaInfo() const;
	CCpjFusaInfo& m_FusaInfo();

	void m_TDCU(CCpjTDCU const &_m_TDCU);
	void m_TDCU(CCpjTDCU &&_m_TDCU);
	CCpjTDCU const& m_TDCU() const;
	CCpjTDCU& m_TDCU();

	void m_LDCU(CCpjLDCU const &_m_LDCU);
	void m_LDCU(CCpjLDCU &&_m_LDCU);
	CCpjLDCU const& m_LDCU() const;
	CCpjLDCU& m_LDCU();

	void m_FCU(CCPJFCU const &_m_FCU);
	void m_FCU(CCPJFCU &&_m_FCU);
	CCPJFCU const& m_FCU() const;
	CCPJFCU& m_FCU();

	void m_IPB(CCPJIPB const &_m_IPB);
	void m_IPB(CCPJIPB &&_m_IPB);
	CCPJIPB const& m_IPB() const;
	CCPJIPB& m_IPB();

	void m_LA1(CCPJLA1 const &_m_LA1);
	void m_LA1(CCPJLA1 &&_m_LA1);
	CCPJLA1 const& m_LA1() const;
	CCPJLA1& m_LA1();

	void m_LA2(CCPJLA2 const &_m_LA2);
	void m_LA2(CCPJLA2 &&_m_LA2);
	CCPJLA2 const& m_LA2() const;
	CCPJLA2& m_LA2();

	void m_RDCU(CCPJRDCU const &_m_RDCU);
	void m_RDCU(CCPJRDCU &&_m_RDCU);
	CCPJRDCU const& m_RDCU() const;
	CCPJRDCU& m_RDCU();

	void m_RWS(CCPJRWS const &_m_RWS);
	void m_RWS(CCPJRWS &&_m_RWS);
	CCPJRWS const& m_RWS() const;
	CCPJRWS& m_RWS();

	void m_SRS(CCPJSRS const &_m_SRS);
	void m_SRS(CCPJSRS &&_m_SRS);
	CCPJSRS const& m_SRS() const;
	CCPJSRS& m_SRS();

	void m_TBOX(CCPJTBOX const &_m_TBOX);
	void m_TBOX(CCPJTBOX &&_m_TBOX);
	CCPJTBOX const& m_TBOX() const;
	CCPJTBOX& m_TBOX();

	void m_XPU(CCPJXPU const &_m_XPU);
	void m_XPU(CCPJXPU &&_m_XPU);
	CCPJXPU const& m_XPU() const;
	CCPJXPU& m_XPU();

	void m_pfGnssLocalization(CPfGnssLocalization const &_m_pfGnssLocalization);
	void m_pfGnssLocalization(CPfGnssLocalization &&_m_pfGnssLocalization);
	CPfGnssLocalization const& m_pfGnssLocalization() const;
	CPfGnssLocalization& m_pfGnssLocalization();

	void m_pfGnssDateTime(CPfGnssDateTime const &_m_pfGnssDateTime);
	void m_pfGnssDateTime(CPfGnssDateTime &&_m_pfGnssDateTime);
	CPfGnssDateTime const& m_pfGnssDateTime() const;
	CPfGnssDateTime& m_pfGnssDateTime();

	void m_senderCallTimestamp(UInt64 const _m_senderCallTimestamp);
	UInt64 m_senderCallTimestamp() const;
	UInt64& m_senderCallTimestamp();





private:
	CCpjIHU m_m_IHU;
	CCpjTel m_m_TEL;
	CCpjEps m_m_Eps;
	CCpjLongCtrl m_m_LongCtrl;
	CCpjRctaWarning m_m_rctaWarning;
	CCpjFctaWarning m_m_fctaWarning;
	CCpjVehInfo m_m_vehInfo;
	CCpjFusaInfo m_m_FusaInfo;
	CCpjTDCU m_m_TDCU;
	CCpjLDCU m_m_LDCU;
	CCPJFCU m_m_FCU;
	CCPJIPB m_m_IPB;
	CCPJLA1 m_m_LA1;
	CCPJLA2 m_m_LA2;
	CCPJRDCU m_m_RDCU;
	CCPJRWS m_m_RWS;
	CCPJSRS m_m_SRS;
	CCPJTBOX m_m_TBOX;
	CCPJXPU m_m_XPU;
	CPfGnssLocalization m_m_pfGnssLocalization;
	CPfGnssDateTime m_m_pfGnssDateTime;
	UInt64 m_m_senderCallTimestamp;

};


/**
* @class ValInOutputPf
* @brief A class as the datatype for data exchange.
* @note
*/

class ValInOutputPf
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(CPfGear) + sizeof(CPfCamera) + sizeof(CPfDoorAndMirror) + sizeof(CPfOdometry) + sizeof(CPfPmaFixedPoint) + sizeof(CPfSteering) + sizeof(CPfTrailer) + sizeof(CPfVehicleLevel) + sizeof(CPfVehicleInfo) + sizeof(CPfTirePress) + sizeof(CPfBrakes) + sizeof(CPfEnvironment) + sizeof(CPfHil) + sizeof(UInt64) + sizeof(UInt64);
	static constexpr bool IS_DATA_PADDING = true || (CPfGear::DATA_SIZE != sizeof(CPfGear)) || (CPfGear::IS_DATA_PADDING) || (CPfCamera::DATA_SIZE != sizeof(CPfCamera)) || (CPfCamera::IS_DATA_PADDING) || (CPfDoorAndMirror::DATA_SIZE != sizeof(CPfDoorAndMirror)) || (CPfDoorAndMirror::IS_DATA_PADDING) || (CPfOdometry::DATA_SIZE != sizeof(CPfOdometry)) || (CPfOdometry::IS_DATA_PADDING) || (CPfPmaFixedPoint::DATA_SIZE != sizeof(CPfPmaFixedPoint)) || (CPfPmaFixedPoint::IS_DATA_PADDING) || (CPfSteering::DATA_SIZE != sizeof(CPfSteering)) || (CPfSteering::IS_DATA_PADDING) || (CPfTrailer::DATA_SIZE != sizeof(CPfTrailer)) || (CPfTrailer::IS_DATA_PADDING) || (CPfVehicleLevel::DATA_SIZE != sizeof(CPfVehicleLevel)) || (CPfVehicleLevel::IS_DATA_PADDING) || (CPfVehicleInfo::DATA_SIZE != sizeof(CPfVehicleInfo)) || (CPfVehicleInfo::IS_DATA_PADDING) || (CPfTirePress::DATA_SIZE != sizeof(CPfTirePress)) || (CPfTirePress::IS_DATA_PADDING) || (CPfBrakes::DATA_SIZE != sizeof(CPfBrakes)) || (CPfBrakes::IS_DATA_PADDING) || (CPfEnvironment::DATA_SIZE != sizeof(CPfEnvironment)) || (CPfEnvironment::IS_DATA_PADDING) || (CPfHil::DATA_SIZE != sizeof(CPfHil)) || (CPfHil::IS_DATA_PADDING);
	static constexpr bool IS_ID_DEFINED = false;

	ValInOutputPf();
	~ValInOutputPf() = default;
	ValInOutputPf(ValInOutputPf const &x) = default;
	ValInOutputPf(ValInOutputPf &&x) = default;
	ValInOutputPf& operator=(ValInOutputPf const &x) = default;
	ValInOutputPf& operator=(ValInOutputPf &&x) = default;

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
	void set_key_val(ValInOutputPf const* const _data) noexcept;



	void m_pfGear(CPfGear const &_m_pfGear);
	void m_pfGear(CPfGear &&_m_pfGear);
	CPfGear const& m_pfGear() const;
	CPfGear& m_pfGear();

	void m_pfCamera(CPfCamera const &_m_pfCamera);
	void m_pfCamera(CPfCamera &&_m_pfCamera);
	CPfCamera const& m_pfCamera() const;
	CPfCamera& m_pfCamera();

	void m_pfDoorAndMirror(CPfDoorAndMirror const &_m_pfDoorAndMirror);
	void m_pfDoorAndMirror(CPfDoorAndMirror &&_m_pfDoorAndMirror);
	CPfDoorAndMirror const& m_pfDoorAndMirror() const;
	CPfDoorAndMirror& m_pfDoorAndMirror();

	void m_pfOdometry(CPfOdometry const &_m_pfOdometry);
	void m_pfOdometry(CPfOdometry &&_m_pfOdometry);
	CPfOdometry const& m_pfOdometry() const;
	CPfOdometry& m_pfOdometry();

	void m_pfPmaFixedPoint(CPfPmaFixedPoint const &_m_pfPmaFixedPoint);
	void m_pfPmaFixedPoint(CPfPmaFixedPoint &&_m_pfPmaFixedPoint);
	CPfPmaFixedPoint const& m_pfPmaFixedPoint() const;
	CPfPmaFixedPoint& m_pfPmaFixedPoint();

	void m_pfSteering(CPfSteering const &_m_pfSteering);
	void m_pfSteering(CPfSteering &&_m_pfSteering);
	CPfSteering const& m_pfSteering() const;
	CPfSteering& m_pfSteering();

	void m_pfTrailer(CPfTrailer const &_m_pfTrailer);
	void m_pfTrailer(CPfTrailer &&_m_pfTrailer);
	CPfTrailer const& m_pfTrailer() const;
	CPfTrailer& m_pfTrailer();

	void m_pfVehicleLevel(CPfVehicleLevel const &_m_pfVehicleLevel);
	void m_pfVehicleLevel(CPfVehicleLevel &&_m_pfVehicleLevel);
	CPfVehicleLevel const& m_pfVehicleLevel() const;
	CPfVehicleLevel& m_pfVehicleLevel();

	void m_pfVehicleInfo(CPfVehicleInfo const &_m_pfVehicleInfo);
	void m_pfVehicleInfo(CPfVehicleInfo &&_m_pfVehicleInfo);
	CPfVehicleInfo const& m_pfVehicleInfo() const;
	CPfVehicleInfo& m_pfVehicleInfo();

	void m_pfTirePress(CPfTirePress const &_m_pfTirePress);
	void m_pfTirePress(CPfTirePress &&_m_pfTirePress);
	CPfTirePress const& m_pfTirePress() const;
	CPfTirePress& m_pfTirePress();

	void m_pfBrakes(CPfBrakes const &_m_pfBrakes);
	void m_pfBrakes(CPfBrakes &&_m_pfBrakes);
	CPfBrakes const& m_pfBrakes() const;
	CPfBrakes& m_pfBrakes();

	void m_pfEnvironment(CPfEnvironment const &_m_pfEnvironment);
	void m_pfEnvironment(CPfEnvironment &&_m_pfEnvironment);
	CPfEnvironment const& m_pfEnvironment() const;
	CPfEnvironment& m_pfEnvironment();

	void m_pfHil(CPfHil const &_m_pfHil);
	void m_pfHil(CPfHil &&_m_pfHil);
	CPfHil const& m_pfHil() const;
	CPfHil& m_pfHil();

	void m_senderCallTimestamp(UInt64 const _m_senderCallTimestamp);
	UInt64 m_senderCallTimestamp() const;
	UInt64& m_senderCallTimestamp();

	void m_timeStampVehicleUtcUs(UInt64 const _m_timeStampVehicleUtcUs);
	UInt64 m_timeStampVehicleUtcUs() const;
	UInt64& m_timeStampVehicleUtcUs();





private:
	CPfGear m_m_pfGear;
	CPfCamera m_m_pfCamera;
	CPfDoorAndMirror m_m_pfDoorAndMirror;
	CPfOdometry m_m_pfOdometry;
	CPfPmaFixedPoint m_m_pfPmaFixedPoint;
	CPfSteering m_m_pfSteering;
	CPfTrailer m_m_pfTrailer;
	CPfVehicleLevel m_m_pfVehicleLevel;
	CPfVehicleInfo m_m_pfVehicleInfo;
	CPfTirePress m_m_pfTirePress;
	CPfBrakes m_m_pfBrakes;
	CPfEnvironment m_m_pfEnvironment;
	CPfHil m_m_pfHil;
	UInt64 m_m_senderCallTimestamp;
	UInt64 m_m_timeStampVehicleUtcUs;

};

typedef ValInOutputPf PrkVinBusValInOutputPf;
typedef ValInOutputCpj PrkVinBusValInOutputCpj;

/**
* @class PrkVinBus
* @brief A class as the datatype for data exchange.
* @note
*/

class PrkVinBus
{
public:
	static constexpr bool IS_KEY_DEFINED = false;
	static constexpr uint32_t DATA_SIZE = sizeof(PrkVinBusValInOutputPf) + sizeof(PrkVinBusValInOutputCpj);
	static constexpr bool IS_DATA_PADDING = true || (PrkVinBusValInOutputPf::DATA_SIZE != sizeof(PrkVinBusValInOutputPf)) || (PrkVinBusValInOutputPf::IS_DATA_PADDING) || (PrkVinBusValInOutputCpj::DATA_SIZE != sizeof(PrkVinBusValInOutputCpj)) || (PrkVinBusValInOutputCpj::IS_DATA_PADDING);
	static constexpr bool IS_ID_DEFINED = false;

	PrkVinBus();
	~PrkVinBus() = default;
	PrkVinBus(PrkVinBus const &x) = default;
	PrkVinBus(PrkVinBus &&x) = default;
	PrkVinBus& operator=(PrkVinBus const &x) = default;
	PrkVinBus& operator=(PrkVinBus &&x) = default;

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
	void set_key_val(PrkVinBus const* const _data) noexcept;



	void ValInOutputPf(PrkVinBusValInOutputPf const &_ValInOutputPf);
	void ValInOutputPf(PrkVinBusValInOutputPf &&_ValInOutputPf);
	PrkVinBusValInOutputPf const& ValInOutputPf() const;
	PrkVinBusValInOutputPf& ValInOutputPf();

	void ValIOutPutCpj(PrkVinBusValInOutputCpj const &_ValIOutPutCpj);
	void ValIOutPutCpj(PrkVinBusValInOutputCpj &&_ValIOutPutCpj);
	PrkVinBusValInOutputCpj const& ValIOutPutCpj() const;
	PrkVinBusValInOutputCpj& ValIOutPutCpj();





private:
	PrkVinBusValInOutputPf m_ValInOutputPf;
	PrkVinBusValInOutputCpj m_ValIOutPutCpj;

};


#endif	// PRK_VIN_BUS_fb6b73c9bc8457b0f02e247b29800f92_H

