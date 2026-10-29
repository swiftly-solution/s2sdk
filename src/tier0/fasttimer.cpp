#include "public/tier0/fasttimer.h"

CCycleCount::CCycleCount()
{
	Init( (uint64)0 );
}

CCycleCount::CCycleCount( uint64 cycles )
{
	Init( cycles );
}

void CCycleCount::Init()
{
	Init( (uint64)0 );
}

void CCycleCount::Init( uint64 cycles )
{
	m_Int64 = cycles;
}

void CCycleCount::Sample()
{
#ifdef COMPILER_MSVC64
	unsigned __int64* pSample = (unsigned __int64*)&m_Int64;
	*pSample = __rdtsc(); 
	//		Msg( "Sample = %I64x", pSample ); 
#elif defined( __GNUC__ )
	unsigned long* pSample = (unsigned long *)&m_Int64;
	__asm__ __volatile__ (  
		"rdtsc\n\t"
		"movl %%eax,  (%0)\n\t"
		"movl %%edx, 4(%0)\n\t"
		: /* no output regs */
	: "D" (pSample)
		: "%eax", "%edx" );
#elif defined( _WIN32 )
	unsigned long* pSample = (unsigned long *)&m_Int64;
	__asm
	{
		// force the cpu to synchronize the instruction queue
		// NJS: CPUID can really impact performance in tight loops.
		//cpuid
		//cpuid
		//cpuid
		mov		ecx, pSample
		rdtsc
		mov		[ecx], eax
		mov		[ecx+4], edx
	}
#elif defined( POSIX )
	unsigned long* pSample = (unsigned long *)&m_Int64;
    __asm__ __volatile__ (  
		"rdtsc\n\t"
		"movl %%eax,  (%0)\n\t"
		"movl %%edx, 4(%0)\n\t"
		: /* no output regs */
		: "D" (pSample)
		: "%eax", "%edx" );
#endif
}

CCycleCount& CCycleCount::operator+=( CCycleCount const &other )
{
	m_Int64 += other.m_Int64;
	return *this;
}

void CCycleCount::Add( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest )
{
	dest.m_Int64 = rSrc1.m_Int64 + rSrc2.m_Int64;
}

void CCycleCount::Sub( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest )
{
	dest.m_Int64 = rSrc1.m_Int64 - rSrc2.m_Int64;
}

uint64 CCycleCount::GetTimestamp()
{
	CCycleCount c;
	c.Sample();
	return c.GetLongCycles();
}

bool CCycleCount::IsLessThan(CCycleCount const &other) const
{
	return m_Int64 < other.m_Int64;
}

unsigned long CCycleCount::GetCycles() const
{
	return (unsigned long)m_Int64;
}

uint64 CCycleCount::GetLongCycles() const
{
	return m_Int64;
}

double CCycleCount::GetMillisecondsF() const
{
	return (double)m_Int64 * 1000.0 / (double)Plat_CPUTickFrequency();
}

void CFastTimer::Start()
{
	m_Duration.Sample();
#ifdef DEBUG_FASTTIMER
	m_bRunning = true;
#endif
}

void CFastTimer::End()
{
	CCycleCount cnt;
	cnt.Sample();
	m_Duration.m_Int64 = cnt.m_Int64 - m_Duration.m_Int64;

#ifdef DEBUG_FASTTIMER
	m_bRunning = false;
#endif
}

CCycleCount CFastTimer::GetDurationInProgress() const
{
	CCycleCount cnt;
	cnt.Sample();
	CCycleCount result;
	result.m_Int64 = cnt.m_Int64 - m_Duration.m_Int64;
	
	return result;
}

CCycleCount const& CFastTimer::GetDuration() const
{
#ifdef DEBUG_FASTTIMER
	assert( !m_bRunning );
#endif
	return m_Duration;
}
