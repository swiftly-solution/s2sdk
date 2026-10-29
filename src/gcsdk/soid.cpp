#include "public/gcsdk/soid.h"

namespace GCSDK {
bool SOID_t::operator==( const SOID_t &rhs ) const
{ return m_type == rhs.m_type && m_id == rhs.m_id; }
}

namespace GCSDK {
bool SOID_t::operator!=( const SOID_t &rhs ) const
{ return !( *this == rhs ); }
}

namespace GCSDK {
bool SOID_t::operator<( const SOID_t &rhs ) const
{ return m_type != rhs.m_type ? m_type < rhs.m_type : m_id < rhs.m_id; }
}
