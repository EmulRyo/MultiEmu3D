/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SMSException.h"

using namespace std;

SMSException::SMSException(): exception()
{
	newException("", SMSUnknown);
}

SMSException::SMSException(string description): exception()
{
	newException(description, SMSUnknown);
}

SMSException::SMSException(string description, ExceptionType type): exception()
{
	newException(description, type);
}

SMSException::~SMSException() throw()
{

}

ExceptionType SMSException::GetType()
{
	return m_type;
}

void SMSException::newException(string description, ExceptionType type)
{
	m_description = description;
	m_type = type;
}

const char * SMSException::what() const throw()
{
	return m_description.c_str();
}
