/*
ServicesList plugin for Miranda IM

Copyright © 2006 Cristian Libotean

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "list.h"

CRITICAL_SECTION CRITICALSECTION_SORT;

static inline int BinarySearch(const void *key, const void *element)
{
	DWORD hash = (DWORD) key;
	PService service = *(PService *) element;
	
	if (hash < service->hash)
	{
		return -1;
	}
	
	return (hash == service->hash) ? 0 : 1;
}

static inline int QuickSort(const void *key, const void *element)
{
	PService first  = *(PService *) key;
	PService second =  *(PService *) element;
	
	if (first->hash < second->hash)
	{
		return -1;
	}
	
	return (first->hash == second->hash) ? 0 : 1;
}


CServicesList &lstServices = CServicesList();

CServicesList::CServicesList(int initialCapacity)
{
	_services = NULL;
	_count = 0;
	_capacity = 0;
	
	InitializeCriticalSection(&CRITICALSECTION_SORT);
	
	Enlarge(initialCapacity);
}

CServicesList::~CServicesList()
{
	Clear();
	free(_services);
	
	DeleteCriticalSection(&CRITICALSECTION_SORT);
}

void CServicesList::Clear()
{
	int i;
	for (i = 0; i < Count(); i++)
	{
		delete _services[i];
	}
	_count = 0;
}

int CServicesList::Count() const
{
	return _count;
}

int CServicesList::Capacity() const
{
	return _capacity;
}

void CServicesList::EnsureCapacity()
{
	if (_count >= _capacity)
		{
			Enlarge(_capacity / 2);
		}
}

void CServicesList::Enlarge(int increaseAmount)
{
	int newSize = _capacity + increaseAmount;
	_services = (PService *) realloc(_services, newSize * sizeof(PService));
	_capacity = newSize;
}

int CServicesList::Contains(const char *name) const
{
	int pos = Index(name);
	return (pos >= 0);
}

int CServicesList::Index(const char *name) const
{
	DWORD hash = NameHashFunction(name);

	PService *service = (PService *) bsearch((void *) hash, _services, Count(), sizeof(PService), BinarySearch);
	
	return (service != NULL) ? (service - _services) : -1;
}

int CServicesList::Add(const char *name, const char *module, void *service, DWORD flags)
{
	int exists = Contains(name);
	if (!exists)
	{
		EnsureCapacity();
		
		PService s = new TService(name, module, service, flags, NameHashFunction(name));
		_services[_count++] = s;
		Sort();
	}
	
	return exists;
}

int CServicesList::Remove(int index)
{
	if ((index < 0) && (index >= Count()))
	{
		return 1;
	}
	
	int i;
	PService tmp = _services[index];
	for (i = index; i < _count - 1; i++)
	{
		_services[i] = _services[i + 1];
	}
	_count--;
	delete tmp;

	
	return 0;
}

int CServicesList::Remove(const char *name)
{
	int index = Index(name);
	if (index >= 0)
	{
		return Remove(index);
	}
	
	return 1;
}

const PService CServicesList::operator [](int index)
{
	if ((index < 0) || (index >= Count()))
	{
		return NULL;
	}
	
	return _services[index];
}

void CServicesList::Sort()
{
	EnterCriticalSection(&CRITICALSECTION_SORT);
	qsort(_services, Count(), sizeof(PService), QuickSort);
	LeaveCriticalSection(&CRITICALSECTION_SORT);
}