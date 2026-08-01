/**************************************************************
* @file prk_vin_busTopicDataType.cpp
* @copyright GREENSTONE TECHNOLOGY CO.,LTD. 2020-2025
* All rights reserved
**************************************************************/

#include "prk_vin_busTopicDataType.h"

EIdcServiceResp_stTopicDataType::EIdcServiceResp_stTopicDataType() : TopicDataType()
{
	set_name("EIdcServiceResp_st");
}
EIdcServiceResp_stTopicDataType::~EIdcServiceResp_stTopicDataType()
{

}
bool EIdcServiceResp_stTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	EIdcServiceResp_st* pData = static_cast<EIdcServiceResp_st*>(data);
	pData->serialize(cdr);
	return true;
}
bool EIdcServiceResp_stTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	EIdcServiceResp_st* pData = static_cast<EIdcServiceResp_st*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t EIdcServiceResp_stTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(EIdcServiceResp_st::DATA_SIZE + 8U);
}
void* EIdcServiceResp_stTopicDataType::create_data()
{
	return new EIdcServiceResp_st;
}
void EIdcServiceResp_stTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	EIdcServiceResp_st* pData = static_cast<EIdcServiceResp_st*>(data);
	delete pData;
}
bool EIdcServiceResp_stTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!EIdcServiceResp_st::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool EIdcServiceResp_stTopicDataType::is_key_defined()
{
	return EIdcServiceResp_st::is_key_defined();
}
CCPJFCUTopicDataType::CCPJFCUTopicDataType() : TopicDataType()
{
	set_name("CCPJFCU");
}
CCPJFCUTopicDataType::~CCPJFCUTopicDataType()
{

}
bool CCPJFCUTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJFCU* pData = static_cast<CCPJFCU*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCPJFCUTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJFCU* pData = static_cast<CCPJFCU*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCPJFCUTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCPJFCU::DATA_SIZE + 8U);
}
void* CCPJFCUTopicDataType::create_data()
{
	return new CCPJFCU;
}
void CCPJFCUTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCPJFCU* pData = static_cast<CCPJFCU*>(data);
	delete pData;
}
bool CCPJFCUTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCPJFCU::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCPJFCUTopicDataType::is_key_defined()
{
	return CCPJFCU::is_key_defined();
}
CCPJIPBTopicDataType::CCPJIPBTopicDataType() : TopicDataType()
{
	set_name("CCPJIPB");
}
CCPJIPBTopicDataType::~CCPJIPBTopicDataType()
{

}
bool CCPJIPBTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJIPB* pData = static_cast<CCPJIPB*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCPJIPBTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJIPB* pData = static_cast<CCPJIPB*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCPJIPBTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCPJIPB::DATA_SIZE + 8U);
}
void* CCPJIPBTopicDataType::create_data()
{
	return new CCPJIPB;
}
void CCPJIPBTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCPJIPB* pData = static_cast<CCPJIPB*>(data);
	delete pData;
}
bool CCPJIPBTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCPJIPB::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCPJIPBTopicDataType::is_key_defined()
{
	return CCPJIPB::is_key_defined();
}
CCPJLA1TopicDataType::CCPJLA1TopicDataType() : TopicDataType()
{
	set_name("CCPJLA1");
}
CCPJLA1TopicDataType::~CCPJLA1TopicDataType()
{

}
bool CCPJLA1TopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJLA1* pData = static_cast<CCPJLA1*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCPJLA1TopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJLA1* pData = static_cast<CCPJLA1*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCPJLA1TopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCPJLA1::DATA_SIZE + 8U);
}
void* CCPJLA1TopicDataType::create_data()
{
	return new CCPJLA1;
}
void CCPJLA1TopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCPJLA1* pData = static_cast<CCPJLA1*>(data);
	delete pData;
}
bool CCPJLA1TopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCPJLA1::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCPJLA1TopicDataType::is_key_defined()
{
	return CCPJLA1::is_key_defined();
}
CCPJLA2TopicDataType::CCPJLA2TopicDataType() : TopicDataType()
{
	set_name("CCPJLA2");
}
CCPJLA2TopicDataType::~CCPJLA2TopicDataType()
{

}
bool CCPJLA2TopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJLA2* pData = static_cast<CCPJLA2*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCPJLA2TopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJLA2* pData = static_cast<CCPJLA2*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCPJLA2TopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCPJLA2::DATA_SIZE + 8U);
}
void* CCPJLA2TopicDataType::create_data()
{
	return new CCPJLA2;
}
void CCPJLA2TopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCPJLA2* pData = static_cast<CCPJLA2*>(data);
	delete pData;
}
bool CCPJLA2TopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCPJLA2::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCPJLA2TopicDataType::is_key_defined()
{
	return CCPJLA2::is_key_defined();
}
CCPJRDCUTopicDataType::CCPJRDCUTopicDataType() : TopicDataType()
{
	set_name("CCPJRDCU");
}
CCPJRDCUTopicDataType::~CCPJRDCUTopicDataType()
{

}
bool CCPJRDCUTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJRDCU* pData = static_cast<CCPJRDCU*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCPJRDCUTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJRDCU* pData = static_cast<CCPJRDCU*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCPJRDCUTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCPJRDCU::DATA_SIZE + 8U);
}
void* CCPJRDCUTopicDataType::create_data()
{
	return new CCPJRDCU;
}
void CCPJRDCUTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCPJRDCU* pData = static_cast<CCPJRDCU*>(data);
	delete pData;
}
bool CCPJRDCUTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCPJRDCU::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCPJRDCUTopicDataType::is_key_defined()
{
	return CCPJRDCU::is_key_defined();
}
CCPJRWSTopicDataType::CCPJRWSTopicDataType() : TopicDataType()
{
	set_name("CCPJRWS");
}
CCPJRWSTopicDataType::~CCPJRWSTopicDataType()
{

}
bool CCPJRWSTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJRWS* pData = static_cast<CCPJRWS*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCPJRWSTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJRWS* pData = static_cast<CCPJRWS*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCPJRWSTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCPJRWS::DATA_SIZE + 8U);
}
void* CCPJRWSTopicDataType::create_data()
{
	return new CCPJRWS;
}
void CCPJRWSTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCPJRWS* pData = static_cast<CCPJRWS*>(data);
	delete pData;
}
bool CCPJRWSTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCPJRWS::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCPJRWSTopicDataType::is_key_defined()
{
	return CCPJRWS::is_key_defined();
}
CCPJSRSTopicDataType::CCPJSRSTopicDataType() : TopicDataType()
{
	set_name("CCPJSRS");
}
CCPJSRSTopicDataType::~CCPJSRSTopicDataType()
{

}
bool CCPJSRSTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJSRS* pData = static_cast<CCPJSRS*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCPJSRSTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJSRS* pData = static_cast<CCPJSRS*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCPJSRSTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCPJSRS::DATA_SIZE + 8U);
}
void* CCPJSRSTopicDataType::create_data()
{
	return new CCPJSRS;
}
void CCPJSRSTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCPJSRS* pData = static_cast<CCPJSRS*>(data);
	delete pData;
}
bool CCPJSRSTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCPJSRS::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCPJSRSTopicDataType::is_key_defined()
{
	return CCPJSRS::is_key_defined();
}
CCPJTBOXTopicDataType::CCPJTBOXTopicDataType() : TopicDataType()
{
	set_name("CCPJTBOX");
}
CCPJTBOXTopicDataType::~CCPJTBOXTopicDataType()
{

}
bool CCPJTBOXTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJTBOX* pData = static_cast<CCPJTBOX*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCPJTBOXTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJTBOX* pData = static_cast<CCPJTBOX*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCPJTBOXTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCPJTBOX::DATA_SIZE + 8U);
}
void* CCPJTBOXTopicDataType::create_data()
{
	return new CCPJTBOX;
}
void CCPJTBOXTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCPJTBOX* pData = static_cast<CCPJTBOX*>(data);
	delete pData;
}
bool CCPJTBOXTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCPJTBOX::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCPJTBOXTopicDataType::is_key_defined()
{
	return CCPJTBOX::is_key_defined();
}
CCPJXPUTopicDataType::CCPJXPUTopicDataType() : TopicDataType()
{
	set_name("CCPJXPU");
}
CCPJXPUTopicDataType::~CCPJXPUTopicDataType()
{

}
bool CCPJXPUTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJXPU* pData = static_cast<CCPJXPU*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCPJXPUTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCPJXPU* pData = static_cast<CCPJXPU*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCPJXPUTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCPJXPU::DATA_SIZE + 8U);
}
void* CCPJXPUTopicDataType::create_data()
{
	return new CCPJXPU;
}
void CCPJXPUTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCPJXPU* pData = static_cast<CCPJXPU*>(data);
	delete pData;
}
bool CCPJXPUTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCPJXPU::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCPJXPUTopicDataType::is_key_defined()
{
	return CCPJXPU::is_key_defined();
}
CCpjEpsTopicDataType::CCpjEpsTopicDataType() : TopicDataType()
{
	set_name("CCpjEps");
}
CCpjEpsTopicDataType::~CCpjEpsTopicDataType()
{

}
bool CCpjEpsTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjEps* pData = static_cast<CCpjEps*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjEpsTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjEps* pData = static_cast<CCpjEps*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjEpsTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjEps::DATA_SIZE + 8U);
}
void* CCpjEpsTopicDataType::create_data()
{
	return new CCpjEps;
}
void CCpjEpsTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjEps* pData = static_cast<CCpjEps*>(data);
	delete pData;
}
bool CCpjEpsTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjEps::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjEpsTopicDataType::is_key_defined()
{
	return CCpjEps::is_key_defined();
}
CCpjFctaWarningTopicDataType::CCpjFctaWarningTopicDataType() : TopicDataType()
{
	set_name("CCpjFctaWarning");
}
CCpjFctaWarningTopicDataType::~CCpjFctaWarningTopicDataType()
{

}
bool CCpjFctaWarningTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjFctaWarning* pData = static_cast<CCpjFctaWarning*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjFctaWarningTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjFctaWarning* pData = static_cast<CCpjFctaWarning*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjFctaWarningTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjFctaWarning::DATA_SIZE + 8U);
}
void* CCpjFctaWarningTopicDataType::create_data()
{
	return new CCpjFctaWarning;
}
void CCpjFctaWarningTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjFctaWarning* pData = static_cast<CCpjFctaWarning*>(data);
	delete pData;
}
bool CCpjFctaWarningTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjFctaWarning::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjFctaWarningTopicDataType::is_key_defined()
{
	return CCpjFctaWarning::is_key_defined();
}
CCpjFusaInfoTopicDataType::CCpjFusaInfoTopicDataType() : TopicDataType()
{
	set_name("CCpjFusaInfo");
}
CCpjFusaInfoTopicDataType::~CCpjFusaInfoTopicDataType()
{

}
bool CCpjFusaInfoTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjFusaInfo* pData = static_cast<CCpjFusaInfo*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjFusaInfoTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjFusaInfo* pData = static_cast<CCpjFusaInfo*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjFusaInfoTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjFusaInfo::DATA_SIZE + 8U);
}
void* CCpjFusaInfoTopicDataType::create_data()
{
	return new CCpjFusaInfo;
}
void CCpjFusaInfoTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjFusaInfo* pData = static_cast<CCpjFusaInfo*>(data);
	delete pData;
}
bool CCpjFusaInfoTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjFusaInfo::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjFusaInfoTopicDataType::is_key_defined()
{
	return CCpjFusaInfo::is_key_defined();
}
CCpjIHUTopicDataType::CCpjIHUTopicDataType() : TopicDataType()
{
	set_name("CCpjIHU");
}
CCpjIHUTopicDataType::~CCpjIHUTopicDataType()
{

}
bool CCpjIHUTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjIHU* pData = static_cast<CCpjIHU*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjIHUTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjIHU* pData = static_cast<CCpjIHU*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjIHUTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjIHU::DATA_SIZE + 8U);
}
void* CCpjIHUTopicDataType::create_data()
{
	return new CCpjIHU;
}
void CCpjIHUTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjIHU* pData = static_cast<CCpjIHU*>(data);
	delete pData;
}
bool CCpjIHUTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjIHU::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjIHUTopicDataType::is_key_defined()
{
	return CCpjIHU::is_key_defined();
}
CCpjLDCUTopicDataType::CCpjLDCUTopicDataType() : TopicDataType()
{
	set_name("CCpjLDCU");
}
CCpjLDCUTopicDataType::~CCpjLDCUTopicDataType()
{

}
bool CCpjLDCUTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjLDCU* pData = static_cast<CCpjLDCU*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjLDCUTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjLDCU* pData = static_cast<CCpjLDCU*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjLDCUTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjLDCU::DATA_SIZE + 8U);
}
void* CCpjLDCUTopicDataType::create_data()
{
	return new CCpjLDCU;
}
void CCpjLDCUTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjLDCU* pData = static_cast<CCpjLDCU*>(data);
	delete pData;
}
bool CCpjLDCUTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjLDCU::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjLDCUTopicDataType::is_key_defined()
{
	return CCpjLDCU::is_key_defined();
}
CCpjLongCtrlTopicDataType::CCpjLongCtrlTopicDataType() : TopicDataType()
{
	set_name("CCpjLongCtrl");
}
CCpjLongCtrlTopicDataType::~CCpjLongCtrlTopicDataType()
{

}
bool CCpjLongCtrlTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjLongCtrl* pData = static_cast<CCpjLongCtrl*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjLongCtrlTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjLongCtrl* pData = static_cast<CCpjLongCtrl*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjLongCtrlTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjLongCtrl::DATA_SIZE + 8U);
}
void* CCpjLongCtrlTopicDataType::create_data()
{
	return new CCpjLongCtrl;
}
void CCpjLongCtrlTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjLongCtrl* pData = static_cast<CCpjLongCtrl*>(data);
	delete pData;
}
bool CCpjLongCtrlTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjLongCtrl::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjLongCtrlTopicDataType::is_key_defined()
{
	return CCpjLongCtrl::is_key_defined();
}
CCpjRctaWarningTopicDataType::CCpjRctaWarningTopicDataType() : TopicDataType()
{
	set_name("CCpjRctaWarning");
}
CCpjRctaWarningTopicDataType::~CCpjRctaWarningTopicDataType()
{

}
bool CCpjRctaWarningTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjRctaWarning* pData = static_cast<CCpjRctaWarning*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjRctaWarningTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjRctaWarning* pData = static_cast<CCpjRctaWarning*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjRctaWarningTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjRctaWarning::DATA_SIZE + 8U);
}
void* CCpjRctaWarningTopicDataType::create_data()
{
	return new CCpjRctaWarning;
}
void CCpjRctaWarningTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjRctaWarning* pData = static_cast<CCpjRctaWarning*>(data);
	delete pData;
}
bool CCpjRctaWarningTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjRctaWarning::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjRctaWarningTopicDataType::is_key_defined()
{
	return CCpjRctaWarning::is_key_defined();
}
CCpjTDCUTopicDataType::CCpjTDCUTopicDataType() : TopicDataType()
{
	set_name("CCpjTDCU");
}
CCpjTDCUTopicDataType::~CCpjTDCUTopicDataType()
{

}
bool CCpjTDCUTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjTDCU* pData = static_cast<CCpjTDCU*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjTDCUTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjTDCU* pData = static_cast<CCpjTDCU*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjTDCUTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjTDCU::DATA_SIZE + 8U);
}
void* CCpjTDCUTopicDataType::create_data()
{
	return new CCpjTDCU;
}
void CCpjTDCUTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjTDCU* pData = static_cast<CCpjTDCU*>(data);
	delete pData;
}
bool CCpjTDCUTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjTDCU::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjTDCUTopicDataType::is_key_defined()
{
	return CCpjTDCU::is_key_defined();
}
CCpjVehInfoTopicDataType::CCpjVehInfoTopicDataType() : TopicDataType()
{
	set_name("CCpjVehInfo");
}
CCpjVehInfoTopicDataType::~CCpjVehInfoTopicDataType()
{

}
bool CCpjVehInfoTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjVehInfo* pData = static_cast<CCpjVehInfo*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjVehInfoTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjVehInfo* pData = static_cast<CCpjVehInfo*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjVehInfoTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjVehInfo::DATA_SIZE + 8U);
}
void* CCpjVehInfoTopicDataType::create_data()
{
	return new CCpjVehInfo;
}
void CCpjVehInfoTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjVehInfo* pData = static_cast<CCpjVehInfo*>(data);
	delete pData;
}
bool CCpjVehInfoTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjVehInfo::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjVehInfoTopicDataType::is_key_defined()
{
	return CCpjVehInfo::is_key_defined();
}
CCpjTelTopicDataType::CCpjTelTopicDataType() : TopicDataType()
{
	set_name("CCpjTel");
}
CCpjTelTopicDataType::~CCpjTelTopicDataType()
{

}
bool CCpjTelTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjTel* pData = static_cast<CCpjTel*>(data);
	pData->serialize(cdr);
	return true;
}
bool CCpjTelTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CCpjTel* pData = static_cast<CCpjTel*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CCpjTelTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CCpjTel::DATA_SIZE + 8U);
}
void* CCpjTelTopicDataType::create_data()
{
	return new CCpjTel;
}
void CCpjTelTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CCpjTel* pData = static_cast<CCpjTel*>(data);
	delete pData;
}
bool CCpjTelTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CCpjTel::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CCpjTelTopicDataType::is_key_defined()
{
	return CCpjTel::is_key_defined();
}
CPfBrakesTopicDataType::CPfBrakesTopicDataType() : TopicDataType()
{
	set_name("CPfBrakes");
}
CPfBrakesTopicDataType::~CPfBrakesTopicDataType()
{

}
bool CPfBrakesTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfBrakes* pData = static_cast<CPfBrakes*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfBrakesTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfBrakes* pData = static_cast<CPfBrakes*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfBrakesTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfBrakes::DATA_SIZE + 8U);
}
void* CPfBrakesTopicDataType::create_data()
{
	return new CPfBrakes;
}
void CPfBrakesTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfBrakes* pData = static_cast<CPfBrakes*>(data);
	delete pData;
}
bool CPfBrakesTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfBrakes::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfBrakesTopicDataType::is_key_defined()
{
	return CPfBrakes::is_key_defined();
}
CPfCameraTopicDataType::CPfCameraTopicDataType() : TopicDataType()
{
	set_name("CPfCamera");
}
CPfCameraTopicDataType::~CPfCameraTopicDataType()
{

}
bool CPfCameraTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfCamera* pData = static_cast<CPfCamera*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfCameraTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfCamera* pData = static_cast<CPfCamera*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfCameraTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfCamera::DATA_SIZE + 8U);
}
void* CPfCameraTopicDataType::create_data()
{
	return new CPfCamera;
}
void CPfCameraTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfCamera* pData = static_cast<CPfCamera*>(data);
	delete pData;
}
bool CPfCameraTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfCamera::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfCameraTopicDataType::is_key_defined()
{
	return CPfCamera::is_key_defined();
}
CPfDoorAndMirrorTopicDataType::CPfDoorAndMirrorTopicDataType() : TopicDataType()
{
	set_name("CPfDoorAndMirror");
}
CPfDoorAndMirrorTopicDataType::~CPfDoorAndMirrorTopicDataType()
{

}
bool CPfDoorAndMirrorTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfDoorAndMirror* pData = static_cast<CPfDoorAndMirror*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfDoorAndMirrorTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfDoorAndMirror* pData = static_cast<CPfDoorAndMirror*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfDoorAndMirrorTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfDoorAndMirror::DATA_SIZE + 8U);
}
void* CPfDoorAndMirrorTopicDataType::create_data()
{
	return new CPfDoorAndMirror;
}
void CPfDoorAndMirrorTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfDoorAndMirror* pData = static_cast<CPfDoorAndMirror*>(data);
	delete pData;
}
bool CPfDoorAndMirrorTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfDoorAndMirror::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfDoorAndMirrorTopicDataType::is_key_defined()
{
	return CPfDoorAndMirror::is_key_defined();
}
CPfEnvironmentTopicDataType::CPfEnvironmentTopicDataType() : TopicDataType()
{
	set_name("CPfEnvironment");
}
CPfEnvironmentTopicDataType::~CPfEnvironmentTopicDataType()
{

}
bool CPfEnvironmentTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfEnvironment* pData = static_cast<CPfEnvironment*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfEnvironmentTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfEnvironment* pData = static_cast<CPfEnvironment*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfEnvironmentTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfEnvironment::DATA_SIZE + 8U);
}
void* CPfEnvironmentTopicDataType::create_data()
{
	return new CPfEnvironment;
}
void CPfEnvironmentTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfEnvironment* pData = static_cast<CPfEnvironment*>(data);
	delete pData;
}
bool CPfEnvironmentTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfEnvironment::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfEnvironmentTopicDataType::is_key_defined()
{
	return CPfEnvironment::is_key_defined();
}
CPfGearTopicDataType::CPfGearTopicDataType() : TopicDataType()
{
	set_name("CPfGear");
}
CPfGearTopicDataType::~CPfGearTopicDataType()
{

}
bool CPfGearTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfGear* pData = static_cast<CPfGear*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfGearTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfGear* pData = static_cast<CPfGear*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfGearTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfGear::DATA_SIZE + 8U);
}
void* CPfGearTopicDataType::create_data()
{
	return new CPfGear;
}
void CPfGearTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfGear* pData = static_cast<CPfGear*>(data);
	delete pData;
}
bool CPfGearTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfGear::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfGearTopicDataType::is_key_defined()
{
	return CPfGear::is_key_defined();
}
CPfGnssDateTimeTopicDataType::CPfGnssDateTimeTopicDataType() : TopicDataType()
{
	set_name("CPfGnssDateTime");
}
CPfGnssDateTimeTopicDataType::~CPfGnssDateTimeTopicDataType()
{

}
bool CPfGnssDateTimeTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfGnssDateTime* pData = static_cast<CPfGnssDateTime*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfGnssDateTimeTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfGnssDateTime* pData = static_cast<CPfGnssDateTime*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfGnssDateTimeTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfGnssDateTime::DATA_SIZE + 8U);
}
void* CPfGnssDateTimeTopicDataType::create_data()
{
	return new CPfGnssDateTime;
}
void CPfGnssDateTimeTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfGnssDateTime* pData = static_cast<CPfGnssDateTime*>(data);
	delete pData;
}
bool CPfGnssDateTimeTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfGnssDateTime::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfGnssDateTimeTopicDataType::is_key_defined()
{
	return CPfGnssDateTime::is_key_defined();
}
CPfGnssLocalizationTopicDataType::CPfGnssLocalizationTopicDataType() : TopicDataType()
{
	set_name("CPfGnssLocalization");
}
CPfGnssLocalizationTopicDataType::~CPfGnssLocalizationTopicDataType()
{

}
bool CPfGnssLocalizationTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfGnssLocalization* pData = static_cast<CPfGnssLocalization*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfGnssLocalizationTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfGnssLocalization* pData = static_cast<CPfGnssLocalization*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfGnssLocalizationTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfGnssLocalization::DATA_SIZE + 8U);
}
void* CPfGnssLocalizationTopicDataType::create_data()
{
	return new CPfGnssLocalization;
}
void CPfGnssLocalizationTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfGnssLocalization* pData = static_cast<CPfGnssLocalization*>(data);
	delete pData;
}
bool CPfGnssLocalizationTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfGnssLocalization::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfGnssLocalizationTopicDataType::is_key_defined()
{
	return CPfGnssLocalization::is_key_defined();
}
CPfHilTopicDataType::CPfHilTopicDataType() : TopicDataType()
{
	set_name("CPfHil");
}
CPfHilTopicDataType::~CPfHilTopicDataType()
{

}
bool CPfHilTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfHil* pData = static_cast<CPfHil*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfHilTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfHil* pData = static_cast<CPfHil*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfHilTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfHil::DATA_SIZE + 8U);
}
void* CPfHilTopicDataType::create_data()
{
	return new CPfHil;
}
void CPfHilTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfHil* pData = static_cast<CPfHil*>(data);
	delete pData;
}
bool CPfHilTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfHil::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfHilTopicDataType::is_key_defined()
{
	return CPfHil::is_key_defined();
}
CPfOdometryTopicDataType::CPfOdometryTopicDataType() : TopicDataType()
{
	set_name("CPfOdometry");
}
CPfOdometryTopicDataType::~CPfOdometryTopicDataType()
{

}
bool CPfOdometryTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfOdometry* pData = static_cast<CPfOdometry*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfOdometryTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfOdometry* pData = static_cast<CPfOdometry*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfOdometryTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfOdometry::DATA_SIZE + 8U);
}
void* CPfOdometryTopicDataType::create_data()
{
	return new CPfOdometry;
}
void CPfOdometryTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfOdometry* pData = static_cast<CPfOdometry*>(data);
	delete pData;
}
bool CPfOdometryTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfOdometry::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfOdometryTopicDataType::is_key_defined()
{
	return CPfOdometry::is_key_defined();
}
CPfPmaFixedPointTopicDataType::CPfPmaFixedPointTopicDataType() : TopicDataType()
{
	set_name("CPfPmaFixedPoint");
}
CPfPmaFixedPointTopicDataType::~CPfPmaFixedPointTopicDataType()
{

}
bool CPfPmaFixedPointTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfPmaFixedPoint* pData = static_cast<CPfPmaFixedPoint*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfPmaFixedPointTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfPmaFixedPoint* pData = static_cast<CPfPmaFixedPoint*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfPmaFixedPointTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfPmaFixedPoint::DATA_SIZE + 8U);
}
void* CPfPmaFixedPointTopicDataType::create_data()
{
	return new CPfPmaFixedPoint;
}
void CPfPmaFixedPointTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfPmaFixedPoint* pData = static_cast<CPfPmaFixedPoint*>(data);
	delete pData;
}
bool CPfPmaFixedPointTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfPmaFixedPoint::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfPmaFixedPointTopicDataType::is_key_defined()
{
	return CPfPmaFixedPoint::is_key_defined();
}
CPfSteeringTopicDataType::CPfSteeringTopicDataType() : TopicDataType()
{
	set_name("CPfSteering");
}
CPfSteeringTopicDataType::~CPfSteeringTopicDataType()
{

}
bool CPfSteeringTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfSteering* pData = static_cast<CPfSteering*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfSteeringTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfSteering* pData = static_cast<CPfSteering*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfSteeringTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfSteering::DATA_SIZE + 8U);
}
void* CPfSteeringTopicDataType::create_data()
{
	return new CPfSteering;
}
void CPfSteeringTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfSteering* pData = static_cast<CPfSteering*>(data);
	delete pData;
}
bool CPfSteeringTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfSteering::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfSteeringTopicDataType::is_key_defined()
{
	return CPfSteering::is_key_defined();
}
CPfTirePressTopicDataType::CPfTirePressTopicDataType() : TopicDataType()
{
	set_name("CPfTirePress");
}
CPfTirePressTopicDataType::~CPfTirePressTopicDataType()
{

}
bool CPfTirePressTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfTirePress* pData = static_cast<CPfTirePress*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfTirePressTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfTirePress* pData = static_cast<CPfTirePress*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfTirePressTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfTirePress::DATA_SIZE + 8U);
}
void* CPfTirePressTopicDataType::create_data()
{
	return new CPfTirePress;
}
void CPfTirePressTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfTirePress* pData = static_cast<CPfTirePress*>(data);
	delete pData;
}
bool CPfTirePressTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfTirePress::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfTirePressTopicDataType::is_key_defined()
{
	return CPfTirePress::is_key_defined();
}
CPfTrailerTopicDataType::CPfTrailerTopicDataType() : TopicDataType()
{
	set_name("CPfTrailer");
}
CPfTrailerTopicDataType::~CPfTrailerTopicDataType()
{

}
bool CPfTrailerTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfTrailer* pData = static_cast<CPfTrailer*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfTrailerTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfTrailer* pData = static_cast<CPfTrailer*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfTrailerTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfTrailer::DATA_SIZE + 8U);
}
void* CPfTrailerTopicDataType::create_data()
{
	return new CPfTrailer;
}
void CPfTrailerTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfTrailer* pData = static_cast<CPfTrailer*>(data);
	delete pData;
}
bool CPfTrailerTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfTrailer::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfTrailerTopicDataType::is_key_defined()
{
	return CPfTrailer::is_key_defined();
}
CPfVehicleInfoTopicDataType::CPfVehicleInfoTopicDataType() : TopicDataType()
{
	set_name("CPfVehicleInfo");
}
CPfVehicleInfoTopicDataType::~CPfVehicleInfoTopicDataType()
{

}
bool CPfVehicleInfoTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfVehicleInfo* pData = static_cast<CPfVehicleInfo*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfVehicleInfoTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfVehicleInfo* pData = static_cast<CPfVehicleInfo*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfVehicleInfoTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfVehicleInfo::DATA_SIZE + 8U);
}
void* CPfVehicleInfoTopicDataType::create_data()
{
	return new CPfVehicleInfo;
}
void CPfVehicleInfoTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfVehicleInfo* pData = static_cast<CPfVehicleInfo*>(data);
	delete pData;
}
bool CPfVehicleInfoTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfVehicleInfo::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfVehicleInfoTopicDataType::is_key_defined()
{
	return CPfVehicleInfo::is_key_defined();
}
CPfVehicleLevelTopicDataType::CPfVehicleLevelTopicDataType() : TopicDataType()
{
	set_name("CPfVehicleLevel");
}
CPfVehicleLevelTopicDataType::~CPfVehicleLevelTopicDataType()
{

}
bool CPfVehicleLevelTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfVehicleLevel* pData = static_cast<CPfVehicleLevel*>(data);
	pData->serialize(cdr);
	return true;
}
bool CPfVehicleLevelTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	CPfVehicleLevel* pData = static_cast<CPfVehicleLevel*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t CPfVehicleLevelTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(CPfVehicleLevel::DATA_SIZE + 8U);
}
void* CPfVehicleLevelTopicDataType::create_data()
{
	return new CPfVehicleLevel;
}
void CPfVehicleLevelTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	CPfVehicleLevel* pData = static_cast<CPfVehicleLevel*>(data);
	delete pData;
}
bool CPfVehicleLevelTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!CPfVehicleLevel::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool CPfVehicleLevelTopicDataType::is_key_defined()
{
	return CPfVehicleLevel::is_key_defined();
}
ValInOutputCpjTopicDataType::ValInOutputCpjTopicDataType() : TopicDataType()
{
	set_name("ValInOutputCpj");
}
ValInOutputCpjTopicDataType::~ValInOutputCpjTopicDataType()
{

}
bool ValInOutputCpjTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	ValInOutputCpj* pData = static_cast<ValInOutputCpj*>(data);
	pData->serialize(cdr);
	return true;
}
bool ValInOutputCpjTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	ValInOutputCpj* pData = static_cast<ValInOutputCpj*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t ValInOutputCpjTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(ValInOutputCpj::DATA_SIZE + 8U);
}
void* ValInOutputCpjTopicDataType::create_data()
{
	return new ValInOutputCpj;
}
void ValInOutputCpjTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	ValInOutputCpj* pData = static_cast<ValInOutputCpj*>(data);
	delete pData;
}
bool ValInOutputCpjTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!ValInOutputCpj::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool ValInOutputCpjTopicDataType::is_key_defined()
{
	return ValInOutputCpj::is_key_defined();
}
ValInOutputPfTopicDataType::ValInOutputPfTopicDataType() : TopicDataType()
{
	set_name("ValInOutputPf");
}
ValInOutputPfTopicDataType::~ValInOutputPfTopicDataType()
{

}
bool ValInOutputPfTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	ValInOutputPf* pData = static_cast<ValInOutputPf*>(data);
	pData->serialize(cdr);
	return true;
}
bool ValInOutputPfTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	ValInOutputPf* pData = static_cast<ValInOutputPf*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t ValInOutputPfTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(ValInOutputPf::DATA_SIZE + 8U);
}
void* ValInOutputPfTopicDataType::create_data()
{
	return new ValInOutputPf;
}
void ValInOutputPfTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	ValInOutputPf* pData = static_cast<ValInOutputPf*>(data);
	delete pData;
}
bool ValInOutputPfTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!ValInOutputPf::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool ValInOutputPfTopicDataType::is_key_defined()
{
	return ValInOutputPf::is_key_defined();
}
PrkVinBusTopicDataType::PrkVinBusTopicDataType() : TopicDataType()
{
	set_name("PrkVinBus");
}
PrkVinBusTopicDataType::~PrkVinBusTopicDataType()
{

}
bool PrkVinBusTopicDataType::serialize(magna::dds::DdsCdr& cdr, void *data, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/)
{
	if (data == nullptr)
	{
		return false;
	}
	PrkVinBus* pData = static_cast<PrkVinBus*>(data);
	pData->serialize(cdr);
	return true;
}
bool PrkVinBusTopicDataType::deserialize(magna::dds::DdsCdr& cdr, std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, void *data)
{
	if (data == nullptr)
	{
		return false;
	}
	PrkVinBus* pData = static_cast<PrkVinBus*>(data);
	pData->deserialize(cdr);
	return true;
}
uint32_t PrkVinBusTopicDataType::get_serialized_size_bound()
{
	return static_cast<uint32_t>(PrkVinBus::DATA_SIZE + 8U);
}
void* PrkVinBusTopicDataType::create_data()
{
	return new PrkVinBus;
}
void PrkVinBusTopicDataType::delete_data(void *data)
{
	if (data == nullptr)
	{
		return;
	}
	PrkVinBus* pData = static_cast<PrkVinBus*>(data);
	delete pData;
}
bool PrkVinBusTopicDataType::get_key(std::shared_ptr<magna::dds::SerializedPayload_t> /*data_value*/, InstanceHandle_t* /*ihandle*/) noexcept
{
	if (!PrkVinBus::is_key_defined())
	{
		return false;
	}
	/* magnadds adapter mode: key extraction from serialized payload is not generated yet. */
	return false;
}
bool PrkVinBusTopicDataType::is_key_defined()
{
	return PrkVinBus::is_key_defined();
}
