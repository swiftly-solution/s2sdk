//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared object based on a protobuf message
//
//=============================================================================

#ifndef PROTOBUFSHAREDOBJECT_H
#define PROTOBUFSHAREDOBJECT_H

#ifdef _WIN32
#pragma once
#endif

#include "sharedobject.h"

namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

namespace GCSDK
{

//----------------------------------------------------------------------------
// Purpose: Base class for CProtoBufSharedObject. This is where all the actual
//			code lives.
//----------------------------------------------------------------------------
class CProtoBufSharedObjectBase : public CSharedObject
{
public:
	typedef CSharedObject BaseClass;

	virtual bool BParseFromMessage( SOID_t owner, const CUtlBuffer &buffer ) override;
	virtual bool BUpdateFromNetwork( const CSharedObject &objUpdate ) override;
	virtual bool BIsKeyLess( const CSharedObject &soRHS ) const override;
	virtual void Copy( const CSharedObject &soRHS ) override;
	virtual void Dump() const override;
	virtual int FindKeyIndex( const CUtlVector<CSharedObject *> &vecObjects, const CUtlVector<int> &vecIndices ) const override;
	virtual bool BAddToMessage( std::string *pBuffer, bool bUnk ) const override;
	virtual bool BAddDestroyToMessage( std::string *pBuffer ) const override;

protected:
	virtual ::google::protobuf::Message *GetPObject() = 0;
	const ::google::protobuf::Message *GetPObject() const;
};

//----------------------------------------------------------------------------
// Purpose: Template for making a shared object that uses a specific protobuf
//			message class for its wire protocol and in-memory representation.
//----------------------------------------------------------------------------
template<typename Message_t, int nTypeID>
class CProtoBufSharedObject : public CProtoBufSharedObjectBase
{
public:
	virtual int GetTypeID() const override { return nTypeID; }

	Message_t &Obj() { return m_msgObject; }
	const Message_t &Obj() const { return m_msgObject; }

	typedef Message_t SchObjectType_t;
	const static int k_nTypeID = nTypeID;

protected:
	virtual ::google::protobuf::Message *GetPObject() override { return &m_msgObject; }

private:
	Message_t m_msgObject;
};

} // namespace GCSDK

#endif // PROTOBUFSHAREDOBJECT_H
