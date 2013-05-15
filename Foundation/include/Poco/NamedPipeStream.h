//
// NamedPipeStream.h
//
// $Id: //poco/1.4/Foundation/include/Poco/NamedPipeStream.h#2 $
//
// Library: Foundation
// Package: Processes
// Module:  NamedPipeStream
//
// Definition of the NamedPipeStream class.
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


#ifndef Foundation_NamedPipeStream_INCLUDED
#define Foundation_NamedPipeStream_INCLUDED


#include "Poco/Foundation.h"
#include "Poco/NamedPipe.h"
#include "Poco/BufferedStreamBuf.h"
#include <istream>
#include <ostream>


namespace Poco {


class Foundation_API NamedPipeStreamBuf: public BufferedStreamBuf
	/// This is the streambuf class used for reading from and writing to a NamedPipe.
{
public:
	typedef BufferedStreamBuf::openmode openmode;
	
	NamedPipeStreamBuf(const NamedPipe& pipe, openmode mode);
		/// Creates a NamedPipeStreamBuf with the given NamedPipe.

	~NamedPipeStreamBuf();
		/// Destroys the NamedPipeStreamBuf.
		
	void close();
		/// Closes the pipe.
		
protected:
	int readFromDevice(char* buffer, std::streamsize length);
	int writeToDevice(const char* buffer, std::streamsize length);

private:
	enum 
	{
		STREAM_BUFFER_SIZE = 1024
	};

	NamedPipe _pipe;
};


class Foundation_API NamedPipeIOS: public virtual std::ios
	/// The base class for NamedPipeInputStream and
	/// NamedPipeOutputStream.
	///
	/// This class is needed to ensure the correct initialization
	/// order of the stream buffer and base classes.
{
public:
	NamedPipeIOS(const NamedPipe& pipe, openmode mode);
		/// Creates the NamedPipeIOS with the given NamedPipe.
		
	~NamedPipeIOS();
		/// Destroys the NamedPipeIOS.
		///
		/// Flushes the buffer, but does not close the pipe.
		
	NamedPipeStreamBuf* rdbuf();
		/// Returns a pointer to the internal NamedPipeStreamBuf.
		
	void close();
		/// Flushes the stream and closes the pipe.

protected:
	NamedPipeStreamBuf _buf;
};


class Foundation_API NamedPipeOutputStream: public NamedPipeIOS, public std::ostream
	/// An output stream for writing to a NamedPipe.
{
public:
	NamedPipeOutputStream(const NamedPipe& pipe);
		/// Creates the NamedPipeOutputStream with the given NamedPipe.

	~NamedPipeOutputStream();
		/// Destroys the NamedPipeOutputStream.
		///
		/// Flushes the buffer, but does not close the pipe.
};


class Foundation_API NamedPipeInputStream: public NamedPipeIOS, public std::istream
	/// An input stream for reading from a NamedPipe.
	///
	/// Using formatted input from a NamedPipeInputStream
	/// is not recommended, due to the read-ahead behavior of
	/// istream with formatted reads.
{
public:
	NamedPipeInputStream(const NamedPipe& pipe);
		/// Creates the NamedPipeInputStream with the given NamedPipe.

	~NamedPipeInputStream();
		/// Destroys the NamedPipeInputStream.
};


} // namespace Poco


#endif // Foundation_NamedPipeStream_INCLUDED
