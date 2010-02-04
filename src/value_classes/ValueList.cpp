//-----------------------------------------------------------------------------
//
//	ValueList.cpp
//
//	Base class for all OpenZWave Value Classes
//
//	Copyright (c) 2010 Mal Lansell <openzwave@lansell.org>
//
//	SOFTWARE NOTICE AND LICENSE
//
//	This file is part of OpenZWave.
//
//	OpenZWave is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published
//	by the Free Software Foundation, either version 3 of the License,
//	or (at your option) any later version.
//
//	OpenZWave is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------

#include "tinyxml.h"
#include "ValueList.h"
#include "Msg.h"
#include "Log.h"

using namespace OpenZWave;


//-----------------------------------------------------------------------------
// <ValueList::ValueList>
// Constructor
//-----------------------------------------------------------------------------
ValueList::ValueList
(
	uint8 const _nodeId,
	uint8 const _commandClassId,
	uint8 const _instance,
	uint8 const _index,
	uint32 const _genre,
	string const& _label,
	bool const _bReadOnly,
	vector<Item> const& _items,
	int32 const _valueIdx
):
	Value( _nodeId, _commandClassId, _instance, _index, _genre, _label, _bReadOnly ),
	m_items( _items ),
	m_valueIdx( _valueIdx )
{
}

//-----------------------------------------------------------------------------
// <ValueList::ValueList>
// Constructor (from XML)
//-----------------------------------------------------------------------------
ValueList::ValueList
(
	TiXmlElement* _pValueElement
):
	Value( _pValueElement )
{
	// Read the items
	TiXmlNode const* pItemNode = _pValueElement->FirstChild();
	while( pItemNode )
	{
		TiXmlElement const* pItemElement = pItemNode->ToElement();
		if( pItemElement )
		{
			char const* str = pItemElement->Value();
			if( str && !strcmp( str, "Item" ) )
			{
				char const* labelStr = pItemElement->Attribute( "label" );

				int value = 0;
				pItemElement->QueryIntAttribute( "value", &value );

				Item item;
				item.m_label = labelStr;
				item.m_value = value;

				m_items.push_back( item );
			}
		}

		pItemNode = pItemNode->NextSibling();
	}

	// Set the value
	_pValueElement->QueryIntAttribute( "value", &m_valueIdx );
}

//-----------------------------------------------------------------------------
// <ValueList::WriteXML>
// Write ourselves to an XML document
//-----------------------------------------------------------------------------
void ValueList::WriteXML
(
	TiXmlElement* _pValueElement
)
{
	Value::WriteXML( _pValueElement );
	
	
	char str[16];
	snprintf( str, 16, "%d", m_valueIdx );
	_pValueElement->SetAttribute( "value", str );

	for( vector<Item>::iterator it = m_items.begin(); it != m_items.end(); ++it )
	{
		TiXmlElement* pItemElement = new TiXmlElement( "Item" );
		pItemElement->SetAttribute( "label", (*it).m_label.c_str() );
		
		snprintf( str, 16, "%d", (*it).m_value );
		pItemElement->SetAttribute( "value", str );

		_pValueElement->LinkEndChild( pItemElement );
	}
}

//-----------------------------------------------------------------------------
// <ValueList::SetByLabel>
// Set a new value in the device, selected by item label
//-----------------------------------------------------------------------------
bool ValueList::SetByLabel
(
	string const& _label
)
{
	// Ensure the value is one of the options
	int32 index = GetItemIdxByLabel( _label );
	if( index < 0 )
	{
		// Item not found
		return false;
	}

	if( index == m_valueIdx )
	{
		// Value already set
		return true;
	}

	m_pendingIdx = index;
	return Value::Set();
}

//-----------------------------------------------------------------------------
// <ValueList::SetByValue>
// Set a new value in the device, selected by item value
//-----------------------------------------------------------------------------
bool ValueList::SetByValue
(
	int32 const _value
)
{
	// Ensure the value is one of the options
	int32 index = GetItemIdxByValue( _value );
	if( index < 0 )
	{
		// Item not found
		return false;
	}

	if( index == m_valueIdx )
	{
		// Value already set
		return true;
	}

	m_pendingIdx = index;
	return Value::Set();
}

//-----------------------------------------------------------------------------
// <ValueList::OnValueChanged>
// A value in a device has changed
//-----------------------------------------------------------------------------
void ValueList::OnValueChanged
(
	int32 const _valueIdx
)
{
	if( _valueIdx == m_valueIdx )
	{
		// Value already set
		return;
	}

	m_valueIdx = _valueIdx;
	Value::OnValueChanged();
}

//-----------------------------------------------------------------------------
// <ValueList::GetItemIdxByLabel>
// Get the index of an item from its label
//-----------------------------------------------------------------------------
int32 const ValueList::GetItemIdxByLabel
(
	string const& _label
)
{
	for( int32 i=0; i<(int32)m_items.size(); ++i )
	{
		if( _label == m_items[i].m_label )
		{
			return i;
		}
	}

	return -1;
}

//-----------------------------------------------------------------------------
// <ValueList::GetItemIdxByValue>
// Get the index of an item from its value
//-----------------------------------------------------------------------------
int32 const ValueList::GetItemIdxByValue
(
	int32 const _value
)
{
	for( int32 i=0; i<(int32)m_items.size(); ++i )
	{
		if( _value == m_items[i].m_value )
		{
			return i;
		}
	}

	return -1;
}






