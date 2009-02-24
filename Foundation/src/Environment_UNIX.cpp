//
// Environment_UNIX.cpp
//
// $Id: //poco/1.3/Foundation/src/Environment_UNIX.cpp#4 $
//
// Library: Foundation
// Package: Core
// Module:  Environment
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Environment_UNIX.h"
#include "Poco/Exception.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <cstring>

namespace Poco {


EnvironmentImpl::StringMap EnvironmentImpl::_map;
FastMutex EnvironmentImpl::_mutex;


std::string EnvironmentImpl::getImpl(const std::string& name)
{
	FastMutex::ScopedLock lock(_mutex);
	
	const char* val = getenv(name.c_str());
	if (val)
		return std::string(val);
	else
		throw NotFoundException(name);
}


bool EnvironmentImpl::hasImpl(const std::string& name)
{
	FastMutex::ScopedLock lock(_mutex);

	return getenv(name.c_str()) != 0;
}


void EnvironmentImpl::setImpl(const std::string& name, const std::string& value)
{
	FastMutex::ScopedLock lock(_mutex);
	
	std::string var = name;
	var.append("=");
	var.append(value);
	_map[name] = var;
	if (putenv((char*) _map[name].c_str()))
	{
		std::string msg = "cannot set environment variable: ";
		msg.append(name);
		throw SystemException(msg);
	}
}


std::string EnvironmentImpl::osNameImpl()
{
	struct utsname uts;
	uname(&uts);
	return uts.sysname;
}


std::string EnvironmentImpl::osVersionImpl()
{
	struct utsname uts;
	uname(&uts);
	return uts.release;
}


std::string EnvironmentImpl::osArchitectureImpl()
{
	struct utsname uts;
	uname(&uts);
	return uts.machine;
}


std::string EnvironmentImpl::nodeNameImpl()
{
	struct utsname uts;
	uname(&uts);
	return uts.nodename;
}


} // namespace Poco


//
// nodeIdImpl
//
#if defined(POCO_OS_FAMILY_BSD) || POCO_OS == POCO_OS_QNX
//
// BSD variants
//
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if_dl.h>


namespace Poco {


void EnvironmentImpl::nodeIdImpl(NodeId& id)
{
	struct ifaddrs* ifaphead;
	int rc = getifaddrs(&ifaphead);
	if (rc) throw SystemException("cannot get network adapter list");

	bool foundAdapter = false;
	for (struct ifaddrs* ifap = ifaphead; ifap; ifap = ifap->ifa_next) 
	{
		if (ifap->ifa_addr && ifap->ifa_addr->sa_family == AF_LINK) 
		{
			struct sockaddr_dl* sdl = reinterpret_cast<struct sockaddr_dl*>(ifap->ifa_addr);
			caddr_t ap = (caddr_t) (sdl->sdl_data + sdl->sdl_nlen);
			int alen = sdl->sdl_alen;
			if (ap && alen > 0) 
			{
				std::memcpy(&id, ap, sizeof(id));
				foundAdapter = true;
				break;
			}
		}
	}
	freeifaddrs(ifaphead);
	if (!foundAdapter) throw SystemException("cannot determine MAC address (no suitable network adapter found)");
}


} // namespace Poco


#elif defined(__CYGWIN__) || POCO_OS == POCO_OS_LINUX
//
// Linux, Cygwin
//
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>


namespace Poco {


void EnvironmentImpl::nodeIdImpl(NodeId& id)
{
	struct ifreq ifr;

	int s = socket(PF_INET, SOCK_DGRAM, 0);
	if (s == -1) throw SystemException("cannot open socket");

	strcpy(ifr.ifr_name, "eth0");
	int rc = ioctl(s, SIOCGIFHWADDR, &ifr);
	close(s);
	if (rc < 0) throw SystemException("cannot get MAC address");
	struct sockaddr* sa = reinterpret_cast<struct sockaddr*>(&ifr.ifr_addr);
	std::memcpy(&id, sa->sa_data, sizeof(id));
}


} // namespace Poco


#elif defined(POCO_OS_FAMILY_UNIX)
//
// General Unix
//
#include <sys/ioctl.h>
#if defined(sun) || defined(__sun) || defined(__sun__)
#define __EXTENSIONS__
#include <net/if_arp.h>
#include <sys/sockio.h>
#include <stropts.h>
#endif
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <unistd.h>

namespace Poco {


void EnvironmentImpl::nodeIdImpl(NodeId& id)
{
	char name[MAXHOSTNAMELEN];
	if (gethostname(name, sizeof(name)))
		throw SystemException("cannot get host name");

	struct hostent* pHost = gethostbyname(name);
	if (!pHost) throw SystemException("cannot get host IP address");

	int s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == -1) throw SystemException("cannot open socket");

	struct arpreq ar;
	std::memset(&ar, 0, sizeof(ar));
	struct sockaddr_in* pAddr = reinterpret_cast<struct sockaddr_in*>(&ar.arp_pa);
	pAddr->sin_family = AF_INET;
	std::memcpy(&pAddr->sin_addr, *pHost->h_addr_list, sizeof(struct in_addr));
	int rc = ioctl(s, SIOCGARP, &ar);
	close(s);
	if (rc < 0) throw SystemException("cannot get MAC address");
	std::memcpy(&id, ar.arp_ha.sa_data, sizeof(id));
}


} // namespace Poco


#endif
