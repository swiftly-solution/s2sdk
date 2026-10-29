#ifndef NETMESSAGES_H
#define NETMESSAGES_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/keyvalues.h"
#include "public/tier0/utlbuffer.h"

template <typename T>
class CBaseCmdKeyValues : public T
{
public:
	CBaseCmdKeyValues( KeyValues *pKeyValues = NULL ) : m_pKeyValues( NULL ) { SetKeyValues( pKeyValues ); }
	CBaseCmdKeyValues( const CBaseCmdKeyValues &other );
	virtual ~CBaseCmdKeyValues() { delete m_pKeyValues; }

	// Copies the KeyValues only, not the message's other fields; a source not parsed yet empties the data
	CBaseCmdKeyValues &operator=( const CBaseCmdKeyValues &other );

	// Deletes the current KeyValues, so don't pass it back in, and takes ownership of pKeyValues
	void SetKeyValues( KeyValues *pKeyValues );

	bool HasKeyValues() const { return this->has_data() && this->data().size() > 0; }

	// Parsed from the data field on the first call only, owned by the message
	KeyValues *GetKeyValues() const;

protected:
	mutable KeyValues *m_pKeyValues;
};

template <typename T>
inline CBaseCmdKeyValues<T>::CBaseCmdKeyValues( const CBaseCmdKeyValues &other ) : T( other ), m_pKeyValues( NULL )
{
	if ( other.m_pKeyValues )
		SetKeyValues( other.m_pKeyValues->MakeCopy() );
}

template <typename T>
inline CBaseCmdKeyValues<T> &CBaseCmdKeyValues<T>::operator=( const CBaseCmdKeyValues &other )
{
	SetKeyValues( other.m_pKeyValues ? other.m_pKeyValues->MakeCopy() : NULL );
	return *this;
}

template <typename T>
inline void CBaseCmdKeyValues<T>::SetKeyValues( KeyValues *pKeyValues )
{
	delete m_pKeyValues;
	m_pKeyValues = pKeyValues;

	if ( m_pKeyValues )
	{
		CUtlBuffer buf;
		m_pKeyValues->WriteAsBinary( buf );
		this->set_data( buf.Base(), buf.TellPut() );
	}
	else
	{
		this->set_data( "" );
	}
}

template <typename T>
inline KeyValues *CBaseCmdKeyValues<T>::GetKeyValues() const
{
	if ( !m_pKeyValues )
	{
		CUtlBuffer buf;
		buf.Put( this->data().data(), (int)this->data().size() );
		m_pKeyValues = new KeyValues( "Base_CmdKeyValues", KeyValues::KV_DISPOSABLE_STRING_TABLE_STATE() );
		m_pKeyValues->ReadAsBinary( buf );
	}

	return m_pKeyValues;
}

#endif // NETMESSAGES_H
