/**
 * galactic-fall
 * Copyright (C) 2006-2022  Hagen Möbius
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stack>
#include <stdexcept>

#include <xml_parser/xml_parser.h>

#include "xml_puny_dom.h"

namespace XML
{
    class DOMReader : public XMLParser
    {
    public:
        DOMReader(std::istream & Stream, XML::Document * Document, XML::Element ** RootElement);
        auto ElementStart(std::string const & Name, std::map<std::string, std::string> const & Attributes) -> void override;
        auto ElementEnd(std::string const & Name) -> void override;
        auto Text(std::string const & Text) -> void override;
    private:
        XML::Document * m_Document;
        XML::Element ** m_RootElement;
        std::stack<XML::Element *> m_ElementStack;
    };
}

XML::DOMReader::DOMReader(std::istream & Stream, XML::Document * Document, XML::Element ** RootElement) :
    XMLParser{Stream},
    m_Document{Document},
    m_RootElement{RootElement}
{
}

void XML::DOMReader::ElementStart(std::string const & Name, std::map<std::string, std::string> const & Attributes)
{
    if(*m_RootElement == nullptr)
    {
        // we've got the root element
        m_ElementStack.push(*m_RootElement = new XML::Element{m_Document, Name, Attributes});
    }
    else
    {
        m_ElementStack.push(new XML::Element{m_ElementStack.top(), Name, Attributes});
    }
}

void XML::DOMReader::ElementEnd(std::string const & Name)
{
    if(m_ElementStack.size() == 0)
    {
        std::cerr << "Got '/" << Name << "' but stack is empty." << std::endl;
        
        throw std::domain_error{Name};
    }
    if(Name != m_ElementStack.top()->GetName())
    {
        std::cerr << "Got '/" << Name << "' but expected '/" << m_ElementStack.top()->GetName() << "'." << std::endl;
        
        throw std::domain_error{Name};
    }
    m_ElementStack.pop();
}

void XML::DOMReader::Text(std::string const & Text)
{
    if(m_ElementStack.size() > 0)
    {
        new XML::Text{m_ElementStack.top(), Text};
    }
    else
    {
        std::cerr << "Misformed XML: Got text \"" << Text << "\" outside of root element." << std::endl;
        
        throw std::domain_error("NoElement");
    }
}

XML::Node::Node(XML::NodeType NodeType, XML::Node * ParentNode) :
    m_NodeType{NodeType},
    m_ParentNode{ParentNode}
{
    if(m_ParentNode != nullptr)
    {
        m_ParentNode->m_ChildNodes.push_back(this);
    }
}

XML::Node::~Node(void)
{
    // delete it from the parent's child nodes vector
    if(m_ParentNode != nullptr)
    {
        auto Iterator = std::find(m_ParentNode->m_ChildNodes.begin(), m_ParentNode->m_ChildNodes.end(), this);
        
        assert(Iterator != m_ParentNode->m_ChildNodes.end());
        m_ParentNode->m_ChildNodes.erase(Iterator);
        m_ParentNode = nullptr;
    }
    while(m_ChildNodes.empty() == false)
    {
        // erasing the child from the m_ChildNodes vector will happen in the childs destructor
        delete m_ChildNodes.front();
    }
}

auto XML::Node::GetChildNodes(void) const -> std::vector<XML::Node *> const &
{
    return m_ChildNodes;
}

auto XML::Node::GetChildNode(std::vector< XML::Node * >::size_type Index) const -> XML::Node const *
{
    return m_ChildNodes[Index];
}

auto XML::Node::GetNodeType(void) const -> XML::NodeType
{
    return m_NodeType;
}

auto XML::Node::GetParentNode(void) const -> XML::Node const *
{
    return m_ParentNode;
}

XML::Element::Element(XML::Node * ParentNode, std::string const & Name, std::map<std::string, std::string> const & Attributes) :
    XML::Node{XML::NodeType::Element, ParentNode},
    m_Name{Name},
    m_Attributes{Attributes}
{
}

auto XML::Element::GetName(void) const -> std::string const &
{
    return m_Name;
}

auto XML::Element::GetAttributes(void) const -> std::map<std::string, std::string> const &
{
    return m_Attributes;
}

auto XML::Element::GetAttribute(std::string const & AttributeName) const -> std::string const &
{
    return m_Attributes.find(AttributeName)->second;
}

auto XML::Element::HasAttribute(std::string const & AttributeName) const -> bool
{
    return m_Attributes.find(AttributeName) != m_Attributes.end();
}

XML::Text::Text(XML::Node * ParentNode, std::string const & Text) :
    XML::Node{XML::NodeType::Text, ParentNode},
    m_Text{Text}
{
}

auto XML::Text::GetText(void) const -> std::string const &
{
    return m_Text;
}

XML::Document::Document(std::istream & Stream) :
    XML::Node{XML::NodeType::Document, nullptr},
    m_DocumentElement{nullptr}
{
    auto DOMReader = XML::DOMReader{Stream, this, &m_DocumentElement};
    
    DOMReader.Parse();
}

XML::Document::~Document(void)
{
    delete m_DocumentElement;
    m_DocumentElement = nullptr;
}

auto XML::Document::GetDocumentElement(void) const -> XML::Element const *
{
    return m_DocumentElement;
}

auto XML::IsElement(XML::Node const * Node) -> bool
{
    return Node->GetNodeType() == XML::NodeType::Element;
}

auto XML::IsText(XML::Node const * Node) -> bool
{
    return Node->GetNodeType() == XML::NodeType::Text;
}
