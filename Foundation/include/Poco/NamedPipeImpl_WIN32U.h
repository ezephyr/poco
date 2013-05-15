//
// NamedPipeImpl_WIN32U.h
//
// $Id: //poco/1.4/Foundation/include/Poco/NamedPipeImpl_WIN32U.h#2 $
//
// Library: Foundation
// Package: Processes
// Module:  NamedPipeImpl
//
// Definition of the NamedPipeImpl class for WIN32.
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


#ifndef Foundation_NamedPipeImpl_WIN32U_INCLUDED
#define Foundation_NamedPipeImpl_WIN32U_INCLUDED


#include "Poco/Foundation.h"
#include "Poco/RefCountedObject.h"
#include "Poco/UnWindows.h"
#include "Poco/Path.h"


namespace Poco {


class Foundation_API NamedPipeImpl: public RefCountedObject
	/// A dummy implementation of NamedPipeImpl for platforms
	/// that do not support pipes.
{
public:
	typedef HANDLE Handle;

	NamedPipeImpl(const Path& path);
	NamedPipeImpl(const Path& path, bool clienttag);
	~NamedPipeImpl();
	int writeBytes(const void* buffer, int length);
	int readBytes(void* buffer, int length);
	Handle readHandle() const;
	Handle writeHandle() const;
	void closeRead();
	void closeWrite();
	
private:
	Path   _path;
	bool   _client;
	HANDLE _readHandle;
	HANDLE _writeHandle;

};


} // namespace Poco


#endif // Foundation_NamedPipeImpl_WIN32U_INCLUDED
