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

#ifndef M_SERVICESLIST_LIST_H
#define M_SERVICESLIST_LIST_H

#define INITIAL_SIZE 50

#include "commonheaders.h"

#define SERVICE_NORMAL						0x0000
#define SERVICE_TRANSIENT					0x0001

#define SLF_DYNAMIC								0x8000

struct TService{
	char *name;
	char *module;
	const void *service;
	DWORD flags;
	DWORD hash;
	DWORD call_count;
	double total_call_time_ms;
	
	TService(const char *initName, const char *initModule, const void *initService, DWORD initFlags, DWORD initHash):
		call_count(0), total_call_time_ms(0.0)
	{
		name = _strdup(initName);
		module = _strdup(initModule);
		service = initService;
		flags = initFlags;
		hash = initHash;
	}
	
	~TService()
	{
		free(this->name);
		free(this->module);
	}
};

typedef TService *PService;

class CServicesList{
	protected:
		PService *_services;
		int _count;
		int _capacity;
		
		void Enlarge(int increaseAmount);
		void EnsureCapacity();		
	
	public:
		CServicesList(int initialSize = INITIAL_SIZE);
		~CServicesList();

		void Clear();
		
		int Add(const char *name, const char *module, void *service, DWORD flags);
		int Remove(int index);
		int Remove(const char *name);
		int Contains(const char *name) const;
		int Index(const char *name) const;
		
		const PService operator [](int index);
		
		int Count() const;
		int Capacity() const;
		
		void Sort();
};

extern CServicesList &lstServices; //list of services

#endif //M_SERVICESLIST_LIST_H