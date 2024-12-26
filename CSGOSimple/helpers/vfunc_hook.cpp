#include "vfunc_hook.hpp"

vfunc_hook::vfunc_hook()
    : class_base(nullptr), vftbl_len(0), new_vftbl(nullptr), old_vftbl(nullptr)
{
}
vfunc_hook::vfunc_hook(void* base)
    : class_base(base), vftbl_len(0), new_vftbl(nullptr), old_vftbl(nullptr)
{
}
vfunc_hook::~vfunc_hook()
{
    unhook_all();

    delete[] new_vftbl;
}

bool vfunc_hook::setup(void* base /*= nullptr*/)
{
    if(base != nullptr)
        class_base = base;

    if(class_base == nullptr)
        return false;

    old_vftbl = *(std::uintptr_t**)class_base;
    vftbl_len = estimate_vftbl_length(old_vftbl) * sizeof(std::uintptr_t);

    if(vftbl_len == 0)
        return false;

    new_vftbl = new std::uintptr_t[vftbl_len + 1]();

	std::memcpy(&new_vftbl[1], old_vftbl, vftbl_len * sizeof(std::uintptr_t));

    try {
        auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
		new_vftbl[0] = old_vftbl[-1];
		*(std::uintptr_t**)class_base = &new_vftbl[1];
    } catch(...) {
        delete[] new_vftbl;
        return false;
    }

    return true;
}
std::size_t vfunc_hook::estimate_vftbl_length(std::uintptr_t* vftbl_start)
{
	MEMORY_BASIC_INFORMATION memInfo = { NULL };
	int m_nSize = -1;
	do {
		m_nSize++;
		VirtualQuery(reinterpret_cast<LPCVOID>(vftbl_start[m_nSize]), &memInfo, sizeof(memInfo));
	} while (memInfo.Protect == PAGE_EXECUTE_READ || memInfo.Protect == PAGE_EXECUTE_READWRITE);

	return m_nSize;
}


vmthook::vmthook()
{
	memset(this, NULL, sizeof(vmthook));
}

vmthook::vmthook(PDWORD* ppdwClassBase)
{
	initialize(ppdwClassBase);
}

vmthook::~vmthook()
{
	unhook();
}

bool vmthook::initialize(PDWORD* ppdwClassBase)
{
	m_ClassBase = ppdwClassBase;
	m_OldVT = *ppdwClassBase;
	m_VTSize = get_vt_count(*ppdwClassBase);

	m_NewVT = new DWORD[m_VTSize + 1]; //-V121
	memcpy(&m_NewVT[1], m_OldVT, sizeof(DWORD) * m_VTSize);
	m_NewVT[0] = (uintptr_t)m_OldVT[-1]; //-V201 //-V103

	DWORD old;
	VirtualProtect(ppdwClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
	*ppdwClassBase = &m_NewVT[1];
	VirtualProtect(ppdwClassBase, sizeof(DWORD), old, &old);

	return true;
}

bool vmthook::initialize(PDWORD** pppdwClassBase)
{
	return initialize(*pppdwClassBase);
}

void vmthook::clear_class_base()
{
	m_ClassBase = NULL;
}

void vmthook::unhook()
{
	if (m_ClassBase)
	{
		DWORD old;
		VirtualProtect(m_ClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
		*m_ClassBase = m_OldVT;
		VirtualProtect(m_ClassBase, sizeof(DWORD), old, &old);
	}
}

void vmthook::rehook()
{
	if (m_ClassBase)
	{
		DWORD old;
		VirtualProtect(m_ClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
		*m_ClassBase = &m_NewVT[1];
		VirtualProtect(m_ClassBase, sizeof(DWORD), old, &old);
	}
}

int vmthook::get_func_count() {

	return (int)m_VTSize;
}

PDWORD vmthook::get_old_vt()
{
	return m_OldVT;
}

DWORD vmthook::hook_function(DWORD dwNewFunc, unsigned int iIndex)
{
	if (m_NewVT && m_OldVT && iIndex <= m_VTSize)
	{
		m_NewVT[iIndex + 1] = dwNewFunc;
		return m_OldVT[iIndex];
	}

	return NULL;
}

DWORD vmthook::get_vt_count(PDWORD pdwVMT)
{
	DWORD dwIndex = NULL;

	for (dwIndex = NULL; pdwVMT[dwIndex]; dwIndex++)
		if (IS_INTRESOURCE((FARPROC)pdwVMT[dwIndex]))
			break;

	return dwIndex;
}