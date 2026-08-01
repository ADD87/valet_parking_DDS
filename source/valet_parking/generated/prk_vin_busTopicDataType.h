/**************************************************************
* @file prk_vin_busTopicDataType.h
* @copyright GREENSTONE TECHNOLOGY CO.,LTD. 2020-2025
* All rights reserved
**************************************************************/

#ifndef PRK_VIN_BUSTOPICDATATYPE_fb6b73c9bc8457b0f02e247b29800f92_H
#define PRK_VIN_BUSTOPICDATATYPE_fb6b73c9bc8457b0f02e247b29800f92_H

#include "magnadds/MagnaDDS.h"

#include "prk_vin_bus.h"




/**
* @class EIdcServiceResp_stTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class EIdcServiceResp_stTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	EIdcServiceResp_stTopicDataType();
	virtual ~EIdcServiceResp_stTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCPJFCUTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCPJFCUTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCPJFCUTopicDataType();
	virtual ~CCPJFCUTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCPJIPBTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCPJIPBTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCPJIPBTopicDataType();
	virtual ~CCPJIPBTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCPJLA1TopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCPJLA1TopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCPJLA1TopicDataType();
	virtual ~CCPJLA1TopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCPJLA2TopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCPJLA2TopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCPJLA2TopicDataType();
	virtual ~CCPJLA2TopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCPJRDCUTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCPJRDCUTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCPJRDCUTopicDataType();
	virtual ~CCPJRDCUTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCPJRWSTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCPJRWSTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCPJRWSTopicDataType();
	virtual ~CCPJRWSTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCPJSRSTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCPJSRSTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCPJSRSTopicDataType();
	virtual ~CCPJSRSTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCPJTBOXTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCPJTBOXTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCPJTBOXTopicDataType();
	virtual ~CCPJTBOXTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCPJXPUTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCPJXPUTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCPJXPUTopicDataType();
	virtual ~CCPJXPUTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjEpsTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjEpsTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjEpsTopicDataType();
	virtual ~CCpjEpsTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjFctaWarningTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjFctaWarningTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjFctaWarningTopicDataType();
	virtual ~CCpjFctaWarningTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjFusaInfoTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjFusaInfoTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjFusaInfoTopicDataType();
	virtual ~CCpjFusaInfoTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjIHUTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjIHUTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjIHUTopicDataType();
	virtual ~CCpjIHUTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjLDCUTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjLDCUTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjLDCUTopicDataType();
	virtual ~CCpjLDCUTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjLongCtrlTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjLongCtrlTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjLongCtrlTopicDataType();
	virtual ~CCpjLongCtrlTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjRctaWarningTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjRctaWarningTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjRctaWarningTopicDataType();
	virtual ~CCpjRctaWarningTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjTDCUTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjTDCUTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjTDCUTopicDataType();
	virtual ~CCpjTDCUTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjVehInfoTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjVehInfoTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjVehInfoTopicDataType();
	virtual ~CCpjVehInfoTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CCpjTelTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CCpjTelTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CCpjTelTopicDataType();
	virtual ~CCpjTelTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfBrakesTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfBrakesTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfBrakesTopicDataType();
	virtual ~CPfBrakesTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfCameraTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfCameraTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfCameraTopicDataType();
	virtual ~CPfCameraTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfDoorAndMirrorTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfDoorAndMirrorTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfDoorAndMirrorTopicDataType();
	virtual ~CPfDoorAndMirrorTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfEnvironmentTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfEnvironmentTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfEnvironmentTopicDataType();
	virtual ~CPfEnvironmentTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfGearTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfGearTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfGearTopicDataType();
	virtual ~CPfGearTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfGnssDateTimeTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfGnssDateTimeTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfGnssDateTimeTopicDataType();
	virtual ~CPfGnssDateTimeTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfGnssLocalizationTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfGnssLocalizationTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfGnssLocalizationTopicDataType();
	virtual ~CPfGnssLocalizationTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfHilTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfHilTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfHilTopicDataType();
	virtual ~CPfHilTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfOdometryTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfOdometryTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfOdometryTopicDataType();
	virtual ~CPfOdometryTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfPmaFixedPointTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfPmaFixedPointTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfPmaFixedPointTopicDataType();
	virtual ~CPfPmaFixedPointTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfSteeringTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfSteeringTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfSteeringTopicDataType();
	virtual ~CPfSteeringTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfTirePressTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfTirePressTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfTirePressTopicDataType();
	virtual ~CPfTirePressTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfTrailerTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfTrailerTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfTrailerTopicDataType();
	virtual ~CPfTrailerTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfVehicleInfoTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfVehicleInfoTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfVehicleInfoTopicDataType();
	virtual ~CPfVehicleInfoTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class CPfVehicleLevelTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class CPfVehicleLevelTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	CPfVehicleLevelTopicDataType();
	virtual ~CPfVehicleLevelTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class ValInOutputCpjTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class ValInOutputCpjTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	ValInOutputCpjTopicDataType();
	virtual ~ValInOutputCpjTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

/**
* @class ValInOutputPfTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class ValInOutputPfTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	ValInOutputPfTopicDataType();
	virtual ~ValInOutputPfTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};
typedef ValInOutputPfTopicDataType PrkVinBusValInOutputPfTopicDataType;
typedef ValInOutputCpjTopicDataType PrkVinBusValInOutputCpjTopicDataType;

/**
* @class PrkVinBusTopicDataType
* @brief A class used as the topic during data exchange.
* @note
*/

class PrkVinBusTopicDataType : public magna::dds::TopicDataType
{
public:
	using InstanceHandle_t = magna::dds::InstanceHandle_t;

	PrkVinBusTopicDataType();
	virtual ~PrkVinBusTopicDataType();

	bool serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> data_value) override;
	bool deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> data_value, void* data) override;
	uint32_t get_serialized_size_bound() override;
	void* create_data() override;
	void delete_data(void *data) override;
	bool get_key(std::shared_ptr<magna::dds::SerializedPayload_t> data_value, InstanceHandle_t* ihandle) noexcept override;
	bool is_key_defined() override;
};

#endif	// PRK_VIN_BUSTOPICDATATYPE_fb6b73c9bc8457b0f02e247b29800f92_H

