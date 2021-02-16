/*
 This file is part of MultiEmu3D.
 
 MultiEmu3D is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MultiEmu3D is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MultiEmu3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Exception.h"

using namespace std;

Exception::Exception(): exception()
{
	newException("", ExceptionType::Unknown);
}

Exception::Exception(string description): exception()
{
	newException(description, ExceptionType::Unknown);
}

Exception::Exception(string description, ExceptionType type): exception()
{
	newException(description, type);
}

Exception::~Exception() noexcept
{

}

ExceptionType Exception::GetType()
{
	return m_type;
}

void Exception::newException(string description, ExceptionType type)
{
	m_description = description;
	m_type = type;
}

const char *Exception::what() const throw()
{
	return m_description.c_str();
}
